#ifndef CNCCONTROLERGUI_CNC_MOVE_SEQUENCE_H
#define CNCCONTROLERGUI_CNC_MOVE_SEQUENCE_H

#include <vector>
#include "OSD/CncTimeFunctions.h"
#include "CncCommon.h"

class CncMoveSequence {

	public:

		// -------------------------------------------------------------------------------
		struct SequencePoint {
			int32_t x;
			int32_t y;
			int32_t z;
			int32_t f;
			long 	clientID;

			struct Parameter {
				unsigned char 	type			= 0;
				unsigned int 	necessarySize	= 0;
			} param;

			explicit SequencePoint(long clientID)                     				 : x(0), y(0), z(0), f(0), clientID(clientID), param() {}
			SequencePoint(long clientID, int32_t z)   								 : x(0), y(0), z(z), f(0), clientID(clientID), param() {}
			SequencePoint(long clientID, int32_t x, int32_t y)						 : x(x), y(y), z(0), f(0), clientID(clientID), param() {}
			SequencePoint(long clientID, int32_t x, int32_t y, int32_t z)			 : x(x), y(y), z(z), f(0), clientID(clientID), param() {}
			SequencePoint(long clientID, int32_t x, int32_t y, int32_t z, int32_t f) : x(x), y(y), z(z), f(f), clientID(clientID), param() {}

			//                                       pid             + 4                        * value
			static const unsigned int MaxPointSize = sizeof(int32_t) + ValueInfo::MaxValueCount * sizeof(int32_t);

			bool isOneByte() const { return ( cnc::between(x, -2, +2)               && cnc::between(y, -2, +2)               && cnc::between(z, -2, +2)               && cnc::between(f, -2, +2) ); } 
			bool isInt8()    const { return ( cnc::between(x, INT8_MIN, INT8_MAX)   && cnc::between(y, INT8_MIN, INT8_MAX)   && cnc::between(z, INT8_MIN, INT8_MAX)  && cnc::between(f, INT8_MIN, INT8_MAX) ); }
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

		CncNanoTimestamp	reference;
		MoveSequence		sequence;
		ProtionIndex		portionIndex;
		SequenceData		data;
		
		unsigned int 		maxSerialSize;
		unsigned char		moveCmd;
		unsigned char*		moveSequenceBuffer;
		unsigned int		moveSequenceBufferSize;
		unsigned int 		moveSequenceFlushedSize;

		long 				curClientId;
		long 				minClientId;
		long 				maxClientId;

		unsigned int 		getHeaderSize() const;
		unsigned int 		getPointSize()  const;
		unsigned int 		determineSafeBufferSize() const;

		unsigned int 		calculateFlushPortionCount();

		void 				createBuffer();
		void 				destroyBuffer();

		void 				addClientId(long id);

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

		explicit CncMoveSequence(unsigned char pid = CMD_RENDER_AND_MOVE_SEQUENCE);
		~CncMoveSequence();

		bool 					isValid() const						{ return (moveCmd == CMD_MOVE_SEQUENCE || moveCmd == CMD_RENDER_AND_MOVE_SEQUENCE); }
		unsigned char			getType() const						{ return moveCmd; }

		CncNanoTimestamp		getReference() const				{ return reference; }
		void					setClientId(long clientId)			{ curClientId = clientId; }
		long					getClientId()						{ return curClientId; }

		void 					addMetricPosXYZF(double dx, double dy, double dz, double f);


		void 					addStepPosXYZF(int32_t dx, int32_t dy, int32_t dz, int32_t f);
		
		void 					addStepPosXYZ (int32_t dx, int32_t dy, int32_t dz)	{ addStepPosXYZF(dx, dy,  0, 0); }
		
		void 					addStepPosXYF (int32_t dx, int32_t dy, int32_t f)	{ addStepPosXYZF(dx, dy,  0, f); }
		void 					addStepPosXY  (int32_t dx, int32_t dy)				{ addStepPosXYZF(dx, dy,  0, 0); }
		
		void 					addStepPosXF  (int32_t dx, int32_t f)				{ addStepPosXYZF(dx,  0,  0, f); }
		void 					addStepPosX   (int32_t dx)							{ addStepPosXYZF(dx,  0,  0, 0); }
		
		void 					addStepPosYF  (int32_t dy, int32_t f)				{ addStepPosXYZF( 0, dy,  0, f); }
		void 					addStepPosY   (int32_t dy)							{ addStepPosXYZF( 0, dy,  0, 0); }
		
		void 					addStepPosZF  (int32_t dz, int32_t f)				{ addStepPosXYZF( 0,  0, dz, f); }
		void 					addStepPosZ   (int32_t dz)							{ addStepPosXYZF( 0,  0, dz, 0); }

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

		const MoveSequence::const_iterator const_begin() const { return sequence.cbegin(); }
		const MoveSequence::const_iterator const_end()   const { return sequence.cend(); }

	private:

		unsigned int 			flushData(FlushResult& result);
		unsigned int 			flushPoint(const SequencePoint& p, unsigned char* sequenceBuffer);
};

#endif
