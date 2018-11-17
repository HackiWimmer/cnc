#include <iostream>
#include "CncArduino.h"
#include "CncCommandDecoder.h"

///////////////////////////////////////////////////////////////////
bool CncCommandDecoder::decodeMove(const unsigned char *buffer, unsigned int nbByte, int32_t& x , int32_t& y , int32_t& z) {
///////////////////////////////////////////////////////////////////
	if ( nbByte <= 0 || buffer == NULL ) {
		std::cerr << "CncCommandDecoder::decodeMove() Empty buffer" << std::endl;
		return false;
	}
	
	// Move cmd format
	// M....;			[len =  5]; only z axis
	// M........;		[len =  9]; x and y axis
	// M............;	[len = 13]; x, y and z axis
	
	x = 0; y = 0; z = 0;
	
	switch ( buffer[0] ) {
		case CMD_RENDER_AND_MOVE:
		case CMD_MOVE:
		case CMD_MOVE_UNIT_SIGNAL: 		break;
		
		default:						std::cerr << "CncCommandDecoder::decodeMove() Invalid command = " << buffer[0] << std::endl;
										return false;
	}
	
	unsigned int idx = 1;
	unsigned char buf[4];

	switch ( nbByte ) {
		case  5:	buf[3] = buffer[idx++];
					buf[2] = buffer[idx++];
					buf[1] = buffer[idx++];
					buf[0] = buffer[idx++];
					memcpy(&z, buf, sizeof(int32_t));
					break;
					
		case 9:		buf[3] = buffer[idx++];
					buf[2] = buffer[idx++];
					buf[1] = buffer[idx++];
					buf[0] = buffer[idx++];
					memcpy(&x, buf, sizeof(int32_t));
					
					buf[3] = buffer[idx++];
					buf[2] = buffer[idx++];
					buf[1] = buffer[idx++];
					buf[0] = buffer[idx++];
					memcpy(&y, buf, sizeof(int32_t));
					break;
					
		case 13:	buf[3] = buffer[idx++];
					buf[2] = buffer[idx++];
					buf[1] = buffer[idx++];
					buf[0] = buffer[idx++];
					memcpy(&x, buf, sizeof(int32_t));
					
					buf[3] = buffer[idx++];
					buf[2] = buffer[idx++];
					buf[1] = buffer[idx++];
					buf[0] = buffer[idx++];
					memcpy(&y, buf, sizeof(int32_t));
					
					buf[3] = buffer[idx++];
					buf[2] = buffer[idx++];
					buf[1] = buffer[idx++];
					buf[0] = buffer[idx++];
					memcpy(&z, buf, sizeof(int32_t));
					break;
		default:
					std::cerr << "SerialEmulatorNULL::writeMoveCmdIntern error." << std::endl;
					std::cerr << " Invalid byte count: " << nbByte << std::endl;
					return false;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncCommandDecoder::decodeSetter(const unsigned char *buffer, unsigned int nbByte, CncCommandDecoder::SetterInfo& si) {
///////////////////////////////////////////////////////////////////
	if ( nbByte <= 0 || buffer == NULL ) {
		std::cerr << "CncCommandDecoder::decodeSetter() Empty buffer" << std::endl;
		return false;
	}
	
	if ( buffer[0] != CMD_SETTER ) {
		std::cerr << "CncCommandDecoder::decodeSetter() Invalid command = " << buffer[0] << std::endl;
		return false;
	}
	
	const unsigned int offset = 3; // CMD + PID + COUNT
	
	if ( nbByte <= offset ) {
		std::cerr << "CncCommandDecoder::decodeSetter() Buffer to small: Size = " << nbByte << std::endl;
		return false;
	}
	
	unsigned int valueSize  = nbByte - offset;
	if ( valueSize % sizeof(int32_t) == 0 ) {
		si.pid 		= buffer[1];
		si.count 	= valueSize / sizeof(int32_t);;
		
		unsigned int index      = offset;
		unsigned char valBuf[sizeof(int32_t)];
		
		// evaluate values
		for ( unsigned int i=0; i<si.count; i++ ) {
			
			valBuf[3] = buffer[index + 0];
			valBuf[2] = buffer[index + 1];
			valBuf[1] = buffer[index + 2];
			valBuf[0] = buffer[index + 3];
			
			int32_t value = 0;
			memcpy(&value, valBuf, sizeof(int32_t));
			si.values.push_back(value);
		}
 
	} else {
		std::cerr << "CncCommandDecoder::decodeSetter() Buffer with wrong size = " << nbByte << std::endl;
		return false;
	}
	
	return true;
}