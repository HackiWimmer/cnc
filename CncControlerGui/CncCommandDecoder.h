#ifndef CNC_COMMAND_DECODER_H
#define CNC_COMMAND_DECODER_H

#include <stdint.h>
#include <vector>
#include "CncPosition.h"
#include "CncCommon.h"



struct CncCommandDecoder {
	
	struct SetterInfo {
		unsigned char pid	= 0;
		unsigned int count	= 0;
		
		cnc::SetterValueList values;
	};
	
	struct MoveSequence {
		unsigned char cmd	= 0;
		int32_t totSeqSize	= 0;
		int32_t remainSize	= 0;
		
		int32_t	totLengthX	= 0;
		int32_t	totLengthY	= 0;
		int32_t	totLengthZ	= 0;
		
		typedef std::vector<CncLongPosition> Positions;
		Positions positions;
		
		void reset() {
			positions.clear();
			
			cmd			=  0;
			totSeqSize	= -1;
			remainSize	= -1;
			
			totLengthX	=  0;
			totLengthY	=  0;
			totLengthZ	=  0;
		}

		bool isBegin() 	{ return (cmd 		 == 0); }
		bool isEnd() 	{ return (remainSize == 0); }
	};

	class CallbackInterface {

		public:
			CallbackInterface() {}
			virtual ~CallbackInterface() {}

			virtual void notifySetter(const SetterInfo& si) {}
			virtual void notifyMove(int32_t dx, int32_t dy, int32_t dz, int32_t f) {}

			virtual void notifyMoveSequenceBegin(const CncCommandDecoder::MoveSequence& sequence) {}
			virtual void notifyMoveSequenceNext(const CncCommandDecoder::MoveSequence& sequence) {}
			virtual void notifyMoveSequenceEnd(const CncCommandDecoder::MoveSequence& sequence) {}
	};


	static bool decodeMoveSequence(const unsigned char *buffer, unsigned int nbByte,
			                       CncCommandDecoder::MoveSequence& sequence,
								   CncCommandDecoder::CallbackInterface* caller);

	static bool decodeMove(const unsigned char *buffer, unsigned int nbByte,
			               int32_t& dx, int32_t& dy, int32_t& dz);

	static bool decodeMove(const unsigned char *buffer, unsigned int nbByte,
			               CncCommandDecoder::CallbackInterface* caller);

	static bool decodeSetter(const unsigned char *buffer, unsigned int nbByte,
			                 CncCommandDecoder::SetterInfo& si);

	static bool decodeSetter(const unsigned char *buffer, unsigned int nbByte,
							 CncCommandDecoder::CallbackInterface* caller);
	
};

#endif
