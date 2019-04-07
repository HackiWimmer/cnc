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

			struct Parameter {
				unsigned char 	pid				= 0;
				unsigned int 	necessarySize	= 0;
			} param;

			SequencePoint() {
				this->x = 0;
				this->y = 0;
				this->z = 0;
			}

			explicit SequencePoint(int32_t z) {
				this->x = 0;
				this->y = 0;
				this->z = z;
			}

			SequencePoint(int32_t x, int32_t y) {
				this->x = x;
				this->y = y;
				this->z = 0;
			}

			SequencePoint(int32_t x, int32_t y, int32_t z) {
				this->x = x;
				this->y = y;
				this->z = z;
			}

			//                                       pid             + 3 * value
			static const unsigned int MaxPointSize = sizeof(int32_t) + 3 * sizeof(int32_t);

			bool hasX() const { return x != 0; }
			bool hasY() const { return y != 0; }
			bool hasZ() const { return z != 0; }

			void calculateParameters() {
				param.pid 				= 0;
				param.necessarySize		= 0;

				// cnc::between(x, -2, +2) means values [-1, 0, +1]
				if      ( cnc::between(x, -2, +2)               && cnc::between(y, -2, +2)               && cnc::between(z, -2, +2) ) {
					if 		( hasX() && hasY() && hasZ() )	{ param.pid = PID_MV_SEQ_0_XYZ;	param.necessarySize =  3; }
					else if ( hasX() && hasY() )			{ param.pid = PID_MV_SEQ_0_XY;	param.necessarySize =  2; }
					else if ( hasX() )						{ param.pid = PID_MV_SEQ_0_X;	param.necessarySize =  1; }
					else if ( hasY() )						{ param.pid = PID_MV_SEQ_0_Y;	param.necessarySize =  1; }
					else if ( hasZ() )						{ param.pid = PID_MV_SEQ_0_Z;	param.necessarySize =  1; }
					else 									{ param.pid = 0;				param.necessarySize =  0; }
				}
				else if ( cnc::between(x, INT8_MIN, INT8_MAX)   && cnc::between(y, INT8_MIN, INT8_MAX)   && cnc::between(z, INT8_MIN, INT8_MAX) ) {
					if 		( hasX() && hasY() && hasZ() )	{ param.pid = PID_MV_SEQ_1_XYZ;	param.necessarySize =  3; }
					else if ( hasX() && hasY() )			{ param.pid = PID_MV_SEQ_1_XY;	param.necessarySize =  2; }
					else if ( hasX() )						{ param.pid = PID_MV_SEQ_1_X;	param.necessarySize =  1; }
					else if ( hasY() )						{ param.pid = PID_MV_SEQ_1_Y;	param.necessarySize =  1; }
					else if ( hasZ() )						{ param.pid = PID_MV_SEQ_1_Z;	param.necessarySize =  1; }
					else 									{ param.pid = 0;				param.necessarySize =  0; }
				}
				else if ( cnc::between(x, INT16_MIN, INT16_MAX) && cnc::between(y, INT16_MIN, INT16_MAX) && cnc::between(z, INT16_MIN, INT16_MAX) ) {
					if 		( hasX() && hasY() && hasZ() )	{ param.pid = PID_MV_SEQ_2_XYZ;	param.necessarySize =  6; }
					else if ( hasX() && hasY() )			{ param.pid = PID_MV_SEQ_2_XY;	param.necessarySize =  4; }
					else if ( hasX() )						{ param.pid = PID_MV_SEQ_2_X;	param.necessarySize =  2; }
					else if ( hasY() )						{ param.pid = PID_MV_SEQ_2_Y;	param.necessarySize =  2; }
					else if ( hasZ() )						{ param.pid = PID_MV_SEQ_2_Z;	param.necessarySize =  2; }
					else 									{ param.pid = 0;				param.necessarySize =  0; }
				}
				else {
					if 		( hasX() && hasY() && hasZ() )	{ param.pid = PID_MV_SEQ_4_XYZ;	param.necessarySize = 12; }
					else if ( hasX() && hasY() )			{ param.pid = PID_MV_SEQ_4_XY;	param.necessarySize =  8; }
					else if ( hasX() )						{ param.pid = PID_MV_SEQ_4_X;	param.necessarySize =  4; }
					else if ( hasY() )						{ param.pid = PID_MV_SEQ_4_Y;	param.necessarySize =  4; }
					else if ( hasZ() )						{ param.pid = PID_MV_SEQ_4_Z;	param.necessarySize =  4; }
					else 									{ param.pid = 0;				param.necessarySize =  0; }
				}
			}
		};

	public:
		// -------------------------------------------------------------------------------
		struct SequenceData {
			int32_t lengthX	= 0;
			int32_t lengthY	= 0;
			int32_t lengthZ	= 0;

			int32_t targetX		= 0;
			int32_t targetY		= 0;
			int32_t targetZ		= 0;

			void reset() {
				lengthX	= lengthY = lengthZ	= 0;
				targetX = targetY = targetZ = 0;
			}

			void add(int32_t dx, int32_t dy, int32_t dz) {
				targetX += dx;
				targetY += dy;
				targetZ += dz;

				lengthX	+= abs(dx);
				lengthY	+= abs(dy);
				lengthZ	+= abs(dz);
			}
		};

		typedef std::vector<SequencePoint> MoveSequence;
		typedef std::vector<unsigned int>  ProtionIndex;

	private:

		MoveSequence	sequence;
		ProtionIndex	portionIndex;
		SequenceData	data;
		unsigned char	type;
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

		bool 					isValid() const						{ return (type == CMD_MOVE_SEQUENCE || type == CMD_RENDER_AND_MOVE_SEQUENCE); }
		unsigned char			getType() const						{ return type; }

		void 					addPosXYZ(int32_t dx, int32_t dy, int32_t dz);
		void 					addPosXY (int32_t dx, int32_t dy)	{ addPosXYZ(dx, dy,  0); }
		void 					addPosX  (int32_t dx)				{ addPosXYZ(dx,  0,  0); }
		void 					addPosY  (int32_t dy)				{ addPosXYZ( 0, dy,  0); }
		void 					addPosZ  (int32_t dz)				{ addPosXYZ( 0,  0, dz); }

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
