#ifndef CNC_COMMAND_DECODER_H
#define CNC_COMMAND_DECODER_H

#include <stdint.h>
#include <vector>
#include "CncCommon.h"

struct CncCommandDecoder {
	
	struct SetterInfo {
		unsigned char pid;
		unsigned int count;
		
		cnc::SetterValueList values;
	};

	static bool decodeMove(const unsigned char *buffer, unsigned int nbByte, int32_t& dx , int32_t& dy , int32_t& dz);
	static bool decodeSetter(const unsigned char *buffer, unsigned int nbByte, CncCommandDecoder::SetterInfo& si);
	
};

#endif
