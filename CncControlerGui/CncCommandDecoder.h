#ifndef CNC_COMMAND_DECODER_H
#define CNC_COMMAND_DECODER_H

#include <stdint.h>
#include <vector>
#include "CncPosition.h"
#include "CncCommon.h"

class CncMoveSequence;

struct CncCommandDecoder {
	
	// ----------------------------------------------------------
	struct SetterInfo {
		unsigned char pid	= 0;
		unsigned int count	= 0;
		
		cnc::SetterValueList values;
	};
	
	// ----------------------------------------------------------
	struct MoveSequenceInfo {
		
		struct IN {
				bool parseAllPortions = false;
		} In;
		
		struct OUT {
			unsigned char cmd	= 0;
			int32_t totSeqSize	= 0;
			int32_t remainSize	= 0;
			
			int32_t	totLengthX	= 0;
			int32_t	totLengthY	= 0;
			int32_t	totLengthZ	= 0;
			
			void reset() {
				cmd			=  0;
				totSeqSize	= -1;
				remainSize	= -1;
				
				totLengthX	=  0;
				totLengthY	=  0;
				totLengthZ	=  0;
			}

			bool isBegin() 	{ return (cmd 		 == 0); }
			bool isEnd() 	{ return (remainSize == 0); }
		} Out;
	};

	// ----------------------------------------------------------
	class CallbackInterface {

		public:
			CallbackInterface() {}
			virtual ~CallbackInterface() {}

			virtual void notifySetter(const CncCommandDecoder::SetterInfo& si) {}
			virtual void notifyMove(int32_t dx, int32_t dy, int32_t dz) {}

			virtual void notifyMoveSequenceBegin(const CncCommandDecoder::MoveSequenceInfo& sequence) {}
			virtual void notifyMoveSequenceNext(const CncCommandDecoder::MoveSequenceInfo& sequence) {}
			virtual void notifyMoveSequenceEnd(const CncCommandDecoder::MoveSequenceInfo& sequence) {}
	};
	
	// ----------------------------------------------------------
	class MoveSequenceRecreator: public CallbackInterface {
		
		private:
			CncMoveSequence* moveSequence;
		
		public:
			MoveSequenceRecreator(CncMoveSequence* ms);
			virtual ~MoveSequenceRecreator() {}

			void notifyMove(int32_t dx, int32_t dy, int32_t dz) final;
			void notifyMoveSequenceBegin(const CncCommandDecoder::MoveSequenceInfo& sequence) final;
	};
	
	static int32_t convertBytesToInt32(const unsigned char *buffer, unsigned int& idx, int32_t& ret);
	
	// ----------------------------------------------------------
	static bool decodeMoveSequence(const unsigned char *buffer, unsigned int nbByte,
								   CncMoveSequence* ret);
	
	static bool decodeMoveSequence(const unsigned char *buffer, unsigned int nbByte,
			                       CncCommandDecoder::MoveSequenceInfo& sequence,
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
