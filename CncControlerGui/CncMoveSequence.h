#ifndef CNCCONTROLERGUI_CNC_MOVE_SEQUENCE_H
#define CNCCONTROLERGUI_CNC_MOVE_SEQUENCE_H

#include <vector>
#include "../Arduino/StepperEnvironment/CncRndr.h"
#include "OSD/CncTimeFunctions.h"
#include "CncCommon.h"

class CncMoveSequence {

	public:

		// -------------------------------------------------------------------------------
		struct SequencePoint {
			int32_t x;
			int32_t y;
			int32_t z;
			long 	clientID;

			struct Parameter {
				unsigned char 	type			= 0;
				unsigned int 	necessarySize	= 0;
			} param;

			explicit SequencePoint(long clientID)                     				 : x(0), y(0), z(0), clientID(clientID), param() {}
			SequencePoint(long clientID, int32_t z)   								 : x(0), y(0), z(z), clientID(clientID), param() {}
			SequencePoint(long clientID, int32_t x, int32_t y)						 : x(x), y(y), z(0), clientID(clientID), param() {}
			SequencePoint(long clientID, int32_t x, int32_t y, int32_t z)			 : x(x), y(y), z(z), clientID(clientID), param() {}

			//                                       pid             + 3                                 * value
			static const unsigned int MaxPointSize = sizeof(int32_t) + ArdoObj::ValueInfo::MaxValueCount * sizeof(int32_t);

			bool isOneByte() const { return ( cnc::between(x, -2, +2)               && cnc::between(y, -2, +2)               && cnc::between(z, -2, +2)                ); } 
			bool isInt8()    const { return ( cnc::between(x, INT8_MIN, INT8_MAX)   && cnc::between(y, INT8_MIN, INT8_MAX)   && cnc::between(z, INT8_MIN, INT8_MAX)    ); }
			bool isInt16()   const { return ( cnc::between(x, INT16_MIN, INT16_MAX) && cnc::between(y, INT16_MIN, INT16_MAX) && cnc::between(z, INT16_MIN, INT16_MAX)  ); } 
			bool isInt32()   const { return ( cnc::between(x, INT32_MIN, INT32_MAX) && cnc::between(y, INT32_MIN, INT32_MAX) && cnc::between(z, INT32_MIN, INT32_MAX)  ); } 

			bool hasX() const { return x != 0; }
			bool hasY() const { return y != 0; }
			bool hasZ() const { return z != 0; }
			
			bool notX() const { return x == 0; }
			bool notY() const { return y == 0; }
			bool notZ() const { return z == 0; }
			
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
					ArdoObj::ValueInfo vi(ArdoObj::ValueInfo::Size::One, x, y, z);
					param.type 				= vi.getType();
					param.necessarySize		= vi.getNecessarySize();
				}
				else if ( isInt8() ) {
					ArdoObj::ValueInfo vi(ArdoObj::ValueInfo::Size::Int8, x, y, z);
					param.type 				= vi.getType();
					param.necessarySize		= vi.getNecessarySize();
				}
				else if ( isInt16() ) {
					ArdoObj::ValueInfo vi(ArdoObj::ValueInfo::Size::Int16, x, y, z);
					param.type 				= vi.getType();
					param.necessarySize		= vi.getNecessarySize();
				}
				else if ( isInt32() ) {
					ArdoObj::ValueInfo vi(ArdoObj::ValueInfo::Size::Int32, x, y, z);
					param.type 				= vi.getType();
					param.necessarySize		= vi.getNecessarySize();
				}
				
				wxASSERT( param.type 			!= 0 );
				wxASSERT( param.necessarySize 	!= 0 );
			}
		};

		// -------------------------------------------------------------------------------
		struct SequenceData {
			int32_t impulseCount 	= 0;

			int32_t lengthX			= 0;
			int32_t lengthY			= 0;
			int32_t lengthZ			= 0;

			int32_t targetX			= 0;
			int32_t targetY			= 0;
			int32_t targetZ			= 0;

			void reset() {
				lengthX	= lengthY = lengthZ	= 0;
				targetX = targetY = targetZ = 0;
			}

			void add(int32_t dx, int32_t dy, int32_t dz) {
				const int32_t DX = ArdoObj::absolute(dx);
				const int32_t DY = ArdoObj::absolute(dy);
				const int32_t DZ = ArdoObj::absolute(dz);
				const int32_t ic = ArduinoImpulseCalculator().calculate(DX, DY, DZ);
				
				if ( ic < 0 ) 	std::cerr << "Error while: SequenceData::add(): Invalid impulse count!" << std::endl;
				else			impulseCount += ic;
				
				targetX += dx;
				targetY	+= dy;
				targetZ	+= dz;
				
				lengthX	+= DX;
				lengthY	+= DY;
				lengthZ	+= DZ;
			}
		};

		typedef std::vector<SequencePoint> MoveSequence;
		typedef std::vector<unsigned int>  PortionIndex;
		typedef std::vector<long>		   OptimizedClientIdList;

	private:

		CncNanoTimestamp		reference;
		MoveSequence			sequence;
		PortionIndex			portionIndex;
		SequenceData			data;
		OptimizedClientIdList	optimizedClientIds;
		
		unsigned int 			maxSerialSize;
		unsigned char			moveCmd;
		unsigned char*			moveSequenceBuffer;
		unsigned int			moveSequenceBufferSize;
		unsigned int 			moveSequenceFlushedSize;

		unsigned int 			getHeaderSize() const;
		unsigned int 			getPointSize()  const;
		unsigned int 			determineSafeBufferSize() const;

		unsigned int 			calculateFlushPortionCount();

		void 					createBuffer();
		void 					destroyBuffer();

	protected:
		
		void setType(unsigned char t) {  moveCmd = t; }
		friend class CncCommandDecoder;
		
	public:
	
		struct SpeedInfo {
			char mode 		= '-';
			double value	= 0.0;
		};

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

		bool 					isValid() const										{ return (moveCmd == CMD_MOVE_SEQUENCE || moveCmd == CMD_RENDER_AND_MOVE_SEQUENCE); }
		unsigned char			getType() const										{ return moveCmd; }
		
		const SpeedInfo&		getCurrentSpeedInfo() const							{ return speedInfo; }
		void					setCurrentSpeedInfo(const SpeedInfo& si)			{ speedInfo = si; }
		
		bool 					isOptimized() const 								{ return optimizedClientIds.size() > 0; }
		unsigned int			getClientIdCount() const							{ return optimizedClientIds.size(); }
		long					getClientId() const									{ return getLastClientId(); }
		const wxString&			getClientIdsAsString() const;
		const wxString&			getClientIdRangeAsString() const;
		long					getFirstClientId() const;
		long					getLastClientId() const;
		void					addClientId(long cid);

		CncNanoTimestamp		getReference() const								{ return reference; }

		void 					addMetricPosXYZ(double dx, double dy, double dz);
		
		void 					addStepPosXYZ (int32_t dx, int32_t dy, int32_t dz);
		void 					addStepPosXY  (int32_t dx, int32_t dy)				{ addStepPosXYZ(dx, dy,  0); }
		void 					addStepPosX   (int32_t dx)							{ addStepPosXYZ(dx,  0,  0); }
		void 					addStepPosY   (int32_t dy)							{ addStepPosXYZ( 0, dy,  0); }
		void 					addStepPosZ   (int32_t dz)							{ addStepPosXYZ( 0,  0, dz); }

		bool 					hasMore() const 									{ return getCount() > 0; }
		unsigned int 			getCount() const;

		void 					clear();

		bool					flush(FlushResult& result);

		const unsigned int		getFlushedSize()  			const	{ return moveSequenceFlushedSize; }

		const unsigned char*	getBuffer() 				const	{ return moveSequenceBuffer; }
		const unsigned int   	getBufferSize()  			const	{ return moveSequenceBufferSize; }

		const PortionIndex& 	getPortionIndex()			const	{ return portionIndex; }

		int32_t 				getAccumulatedDeltaX()		const	{ return data.targetX; }
		int32_t 				getAccumulatedDeltaY()		const	{ return data.targetY; }
		int32_t 				getAccumulatedDeltaZ()		const	{ return data.targetZ; }

		int32_t					getImpulseCount()			const	{ return data.impulseCount;};
		int32_t					getLengthX()				const	{ return data.lengthX; }
		int32_t					getLengthY()				const	{ return data.lengthY; }
		int32_t					getLengthZ()				const	{ return data.lengthZ; }

		const MoveSequence::const_iterator const_begin()	const	{ return sequence.cbegin(); }
		const MoveSequence::const_iterator const_end()		const	{ return sequence.cend(); }

	private:
		
		SpeedInfo				speedInfo;
		
		unsigned int 			flushData(FlushResult& result);
		unsigned int 			flushPoint(const SequencePoint& p, unsigned char* sequenceBuffer);
};

#endif
