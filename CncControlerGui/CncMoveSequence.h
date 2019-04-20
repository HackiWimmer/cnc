#ifndef CNCCONTROLERGUI_CNC_MOVE_SEQUENCE_H
#define CNCCONTROLERGUI_CNC_MOVE_SEQUENCE_H

#include <vector>
#include "CncCommon.h"

class CncMoveSequence {

	private:

		// -------------------------------------------------------------------------------
		struct SequencePoint {
			int32_t x;
			int32_t y;
			int32_t z;
			int32_t f;

			struct Parameter {
				unsigned char 	type			= 0;
				unsigned int 	necessarySize	= 0;
			} param;

			SequencePoint() 
			: param()
			{
				this->x = 0;
				this->y = 0;
				this->z = 0;
				this->f = 0;
			}

			explicit SequencePoint(int32_t z)
			: param()
			{
				this->x = 0;
				this->y = 0;
				this->z = z;
				this->f = 0;
			}

			SequencePoint(int32_t x, int32_t y)
			: param()
			{
				this->x = x;
				this->y = y;
				this->z = 0;
				this->f = 0;
			}

			SequencePoint(int32_t x, int32_t y, int32_t z)
			: param()
			{
				this->x = x;
				this->y = y;
				this->z = z;
				this->f = 0;
			}

			SequencePoint(int32_t x, int32_t y, int32_t z, int32_t f)
			: param()
			{
				this->x = x;
				this->y = y;
				this->z = z;
				this->f = f;
			}

			//                                       pid             + 4                        * value
			static const unsigned int MaxPointSize = sizeof(int32_t) + ValueInfo::MaxValueCount * sizeof(int32_t);

			bool isOneByte() const { return ( cnc::between(x, -2, +2)               && cnc::between(y, -2, +2)               && cnc::between(z, -2, +2)               && cnc::between(f, -2, +2) ); } 
			bool isInt8()    const { return ( cnc::between(x, INT8_MIN, INT8_MAX)   && cnc::between(y, INT8_MIN, INT8_MAX)   &&  cnc::between(z, INT8_MIN, INT8_MAX)  && cnc::between(f, INT8_MIN, INT8_MAX) ); } 
			bool isInt16()   const { return ( cnc::between(x, INT16_MIN, INT16_MAX) && cnc::between(y, INT16_MIN, INT16_MAX) && cnc::between(z, INT16_MIN, INT16_MAX) && cnc::between(f, INT16_MIN, INT16_MAX) ); } 
			bool isInt32()   const { return ( cnc::between(x, INT32_MIN, INT32_MAX) && cnc::between(y, INT32_MIN, INT32_MAX) && cnc::between(z, INT32_MIN, INT32_MAX) && cnc::between(f, INT32_MIN, INT32_MAX) ); } 

			bool hasX() const { return x != 0; }
			bool hasY() const { return y != 0; }
			bool hasZ() const { return z != 0; }
			
			bool hasF() const { return f != 0; }
			
			bool notX() const { return x == 0; }
			bool notY() const { return y == 0; }
			bool notZ() const { return z == 0; }
			
			bool notF() const { return f == 0; }
			
			void determineOneByte() {
				param.type 				= 0;
				param.necessarySize		= 1;
			}
			
			void determineParameters(unsigned char cmd) {
				if ( cmd == CMD_MOVE_SEQUENCE) {
					param.type 				= 0;
					param.necessarySize		= 1;
					return;
				}
				
				param.type 				= 0;
				param.necessarySize		= 0;
				
				// cnc::between(x, -2, +2) means values [-1, 0, +1]
				if      ( isOneByte() ) {
					ValueInfo vi(ValueInfo::Size::One, x, y, z, f);
					param.type 				= vi.getType();
					param.necessarySize		= vi.getNecessarySize();
				}
				else if ( isInt8() ) {
					ValueInfo vi(ValueInfo::Size::Int8, x, y, z, f);
					param.type 				= vi.getType();
					param.necessarySize		= vi.getNecessarySize();
				}
				else if ( isInt16() ) {
					ValueInfo vi(ValueInfo::Size::Int16, x, y, z, f);
					param.type 				= vi.getType();
					param.necessarySize		= vi.getNecessarySize();
				}
				else if ( isInt32() ) {
					ValueInfo vi(ValueInfo::Size::Int32, x, y, z, f);
					param.type 				= vi.getType();
					param.necessarySize		= vi.getNecessarySize();
				}
				
				wxASSERT( param.type 			!= 0 );
				wxASSERT( param.necessarySize 	!= 0 );
			}
		};

	public:
		// -------------------------------------------------------------------------------
		struct SequenceData {
			int32_t lengthX	= 0;
			int32_t lengthY	= 0;
			int32_t lengthZ	= 0;

			int32_t targetX	= 0;
			int32_t targetY	= 0;
			int32_t targetZ	= 0;

			void reset() {
				lengthX	= lengthY = lengthZ	= 0;
				targetX = targetY = targetZ = 0;
			}

			void add(int32_t dx, int32_t dy, int32_t dz) {
				targetX += dx;
				targetY += dy;
				targetZ += dz;

				lengthX	+= absolute(dx);
				lengthY	+= absolute(dy);
				lengthZ	+= absolute(dz);
			}
		};

		typedef std::vector<SequencePoint> MoveSequence;
		typedef std::vector<unsigned int>  ProtionIndex;

	private:

		MoveSequence	sequence;
		ProtionIndex	portionIndex;
		SequenceData	data;
		
		unsigned int 	maxSerialSize;
		unsigned char	moveCmd;
		unsigned char*	moveSequenceBuffer;
		unsigned int	moveSequenceBufferSize;
		unsigned int 	moveSequenceFlushedSize;

		unsigned int 	getHeaderSize() const;
		unsigned int 	getPointSize()  const;
		unsigned int 	determineSafeBufferSize() const;

		unsigned int 	calculateFlushPortionCount();

		void 			createBuffer();
		void 			destroyBuffer();

	public:
		struct FlushResult {
			unsigned char* 	buffer			= NULL;
			unsigned int 	bufferSize		= 0;
			unsigned int 	flushedSize		= 0;
			unsigned int 	flushedCount	= 0;
			bool 			more			= false;
			SequenceData	sequenceData;

			void reset() {
				buffer						= NULL;
				bufferSize					= 0;
				flushedCount				= 0;
				more						= false;

				sequenceData.reset();
			}
		};

		explicit CncMoveSequence(unsigned char pid);
		~CncMoveSequence();

		bool 					isValid() const						{ return (moveCmd == CMD_MOVE_SEQUENCE || moveCmd == CMD_RENDER_AND_MOVE_SEQUENCE); }
		unsigned char			getType() const						{ return moveCmd; }

		void 					addPosXYZF(int32_t dx, int32_t dy, int32_t dz, int32_t f);
		
		void 					addPosXYZ (int32_t dx, int32_t dy, int32_t dz)	{ addPosXYZF(dx, dy,  0, 0); }
		
		void 					addPosXYF (int32_t dx, int32_t dy, int32_t f)	{ addPosXYZF(dx, dy,  0, f); }
		void 					addPosXY  (int32_t dx, int32_t dy)				{ addPosXYZF(dx, dy,  0, 0); }
		
		void 					addPosXF  (int32_t dx, int32_t f)				{ addPosXYZF(dx,  0,  0, f); }
		void 					addPosX   (int32_t dx)							{ addPosXYZF(dx,  0,  0, 0); }
		
		void 					addPosYF  (int32_t dy, int32_t f)				{ addPosXYZF( 0, dy,  0, f); }
		void 					addPosY   (int32_t dy)							{ addPosXYZF( 0, dy,  0, 0); }
		
		void 					addPosZF  (int32_t dz, int32_t f)				{ addPosXYZF( 0,  0, dz, f); }
		void 					addPosZ   (int32_t dz)							{ addPosXYZF( 0,  0, dz, 0); }

		bool 					hasMore() const 					{ return getCount() > 0; }
		unsigned int 			getCount() const;

		void 					clear();

		bool					flush(FlushResult& result);

		const unsigned int		getFlushedSize()  					{ return moveSequenceFlushedSize; }

		const unsigned char*	getBuffer() 						{ return moveSequenceBuffer; }
		const unsigned int   	getBufferSize()  					{ return moveSequenceBufferSize; }

		const ProtionIndex& 	getPortionIndex()					{ return portionIndex; }

		int32_t 				getAccumulatedDeltaX()				{ return data.targetX; }
		int32_t 				getAccumulatedDeltaY()				{ return data.targetY; }
		int32_t 				getAccumulatedDeltaZ()				{ return data.targetZ; }

	private:

		unsigned int 			flushData(FlushResult& result);
		unsigned int 			flushPoint(const SequencePoint& p, unsigned char* sequenceBuffer);
};

#endif
