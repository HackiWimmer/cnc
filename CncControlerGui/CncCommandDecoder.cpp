#include <iostream>
#include "CncArduino.h"
#include "CncMoveSequence.h"
#include "CncCommandDecoder.h"

#ifdef __WXGTK__
	#include <arpa/inet.h>
#endif

#ifdef __WXMSW__
	#include <winsock.h>
#endif

///////////////////////////////////////////////////////////////////
bool CncCommandDecoder::decodeMoveSequence(const unsigned char *buffer, unsigned int nbByte,
                      	   	   	   	   	   CncCommandDecoder::MoveSequence& sequence,
										   CncCommandDecoder::CallbackInterface* caller)
///////////////////////////////////////////////////////////////////
{
	if ( nbByte <= 0 || buffer == NULL ) {
		std::cerr << "CncCommandDecoder::decodeMoveSequence() Empty buffer" << std::endl;
		return false;
	}

	unsigned int idx = 0;

	// ------------------------------------------------------------------------
	auto determineDataStructure = [](unsigned char pid, unsigned int& byteCount, unsigned int& valCount) {
		bool ret = true;
		switch ( pid ) {
			case PID_MV_SEQ_0_XYZ:  byteCount = 0; valCount = 3; break;
			case PID_MV_SEQ_0_XY:   byteCount = 0; valCount = 2; break;
			case PID_MV_SEQ_0_X:    byteCount = 0; valCount = 1; break;
			case PID_MV_SEQ_0_Y:    byteCount = 0; valCount = 1; break;
			case PID_MV_SEQ_0_Z:    byteCount = 0; valCount = 1; break;

			case PID_MV_SEQ_1_XYZ:  byteCount = 1; valCount = 3; break;
			case PID_MV_SEQ_1_XY:   byteCount = 1; valCount = 2; break;
			case PID_MV_SEQ_1_X:    byteCount = 1; valCount = 1; break;
			case PID_MV_SEQ_1_Y:    byteCount = 1; valCount = 1; break;
			case PID_MV_SEQ_1_Z:    byteCount = 1; valCount = 1; break;

			case PID_MV_SEQ_2_XYZ:  byteCount = 2; valCount = 3; break;
			case PID_MV_SEQ_2_XY:   byteCount = 2; valCount = 2; break;
			case PID_MV_SEQ_2_X:    byteCount = 2; valCount = 1; break;
			case PID_MV_SEQ_2_Y:    byteCount = 2; valCount = 1; break;
			case PID_MV_SEQ_2_Z:    byteCount = 2; valCount = 1; break;

			case PID_MV_SEQ_4_XYZ:  byteCount = 4; valCount = 3; break;
			case PID_MV_SEQ_4_XY:   byteCount = 4; valCount = 2; break;
			case PID_MV_SEQ_4_X:    byteCount = 4; valCount = 1; break;
			case PID_MV_SEQ_4_Y:    byteCount = 4; valCount = 1; break;
			case PID_MV_SEQ_4_Z:    byteCount = 4; valCount = 1; break;

			default:				ret = false;
		}
		return ret;
	};

	// ------------------------------------------------------------------------
	auto readOneByteToPos = [&] (CncLongPosition& pos) {
		unsigned char buf[1];
		buf[0] = buffer[idx++];

		pos.setX( buf[0] &  1 ?  +1 : buf[0] &  2 ? -1 : 0 );
		pos.setY( buf[0] &  4 ?  +1 : buf[0] &  8 ? -1 : 0 );
		pos.setZ( buf[0] & 16 ?  +1 : buf[0] & 32 ? -1 : 0 );

		return 1;
 	};

	// ------------------------------------------------------------------------
	auto readOneByte = [&] (int32_t& x, int32_t& y, int32_t& z) {
		unsigned char buf[1];
		buf[0] = buffer[idx++];

		x = buf[0] &  1 ?  +1 : buf[0] &  2 ? -1 : 0 ;
		y = buf[0] &  4 ?  +1 : buf[0] &  8 ? -1 : 0 ;
		z = buf[0] & 16 ?  +1 : buf[0] & 32 ? -1 : 0 ;

		return 1;
	};

	// ------------------------------------------------------------------------
	auto readInt8 = [&] () {
		int8_t ret = 0;
		unsigned char buf[sizeof(int8_t)];
		buf[0] = buffer[idx++];
		memcpy(&ret, buf, sizeof(int8_t));

		return ret;
	};

	// ------------------------------------------------------------------------
	auto readInt16 = [&] () {
		int16_t ret = 0;
		unsigned char buf[sizeof(int16_t)];
		buf[1] = buffer[idx++];
		buf[0] = buffer[idx++];
		memcpy(&ret, buf, sizeof(int16_t));

		return ret;
	};

	// ------------------------------------------------------------------------
	auto readInt32 = [&] () {
		int32_t ret = 0;
		unsigned char buf[sizeof(int32_t)];
		buf[3] = buffer[idx++];
		buf[2] = buffer[idx++];
		buf[1] = buffer[idx++];
		buf[0] = buffer[idx++];
		memcpy(&ret, buf, sizeof(int32_t));

		return ret;
	};

	// ------------------------------------------------------------------------
	auto readPosition = [&] (CncLongPosition& pos) {

		const unsigned char pid 	= buffer[idx++];
		unsigned int byteCount 		= 0;
		unsigned int valCount 		= 0;
		determineDataStructure(pid, byteCount, valCount);

		const bool trace = false;
		if ( trace )
			std::cout << "readPosition: " << (int)pid << ", " << byteCount << ", " << valCount << ", ";

		unsigned int valCounter  = 0;
		unsigned int byteCounter = 1; // consider pid

		int32_t values[3];
		while ( valCounter < valCount ) {
			switch ( byteCount ) {
				case 0:		readOneByte(values[0], values[1], values[2]);
							valCounter  += 3; 	// to break the while loop
							byteCounter += 1;	// to override 0
							break;

				case 1:		values[valCounter] = readInt8();
							byteCounter += 1;
							break;

				case 2:		values[valCounter] = readInt16();
							byteCounter += 2;
							break;

				case 4:		values[valCounter] = readInt32();
							byteCounter += 4;
							break;

				default: 	wxASSERT( byteCount != 0 && byteCount != 1 && byteCount != 2 && byteCount != 4 );
							std::cerr << "byteCount = " << byteCount << std::endl;
			}

			valCounter++;
		}

		// assign x, y, z depending on given pid
		switch ( pid ) {
			case PID_MV_SEQ_0_XYZ:
			case PID_MV_SEQ_1_XYZ:
			case PID_MV_SEQ_2_XYZ:
			case PID_MV_SEQ_4_XYZ:  pos.setX(values[0]); pos.setY(values[1]); pos.setZ(values[2]);
									break;
			case PID_MV_SEQ_0_XY:
			case PID_MV_SEQ_1_XY:
			case PID_MV_SEQ_2_XY:
			case PID_MV_SEQ_4_XY:   pos.setX(values[0]); pos.setY(values[1]); pos.setZ(0);
									break;
			case PID_MV_SEQ_0_X:
			case PID_MV_SEQ_1_X:
			case PID_MV_SEQ_2_X:
			case PID_MV_SEQ_4_X:    pos.setX(values[0]); pos.setY(0); pos.setZ(0);
									break;
			case PID_MV_SEQ_0_Y:
			case PID_MV_SEQ_1_Y:
			case PID_MV_SEQ_2_Y:
			case PID_MV_SEQ_4_Y:    pos.setX(0); pos.setY(values[0]); pos.setZ(0);
									break;
			case PID_MV_SEQ_0_Z:
			case PID_MV_SEQ_1_Z:
			case PID_MV_SEQ_2_Z:
			case PID_MV_SEQ_4_Z:    pos.setX(0); pos.setY(0); pos.setZ(values[0]);
									break;
		}

		if ( trace )
			std::cout << "pos: " << pos << std::endl;

		return byteCounter;
	};

	// ------------------------------------------------------------------------
	auto returnAndNotify = [&](bool ret) {
		if ( caller != NULL ) {
			if ( sequence.isEnd() )
				caller->notifyMoveSequenceEnd(sequence);
		}

		return ret;
	};

	// ------------------------------------------------------------------------
	// process first portion
	if ( sequence.isBegin() ) {
		const unsigned int totHeaderLength = sizeof(unsigned char)		// pid
				                           + 1 * sizeof(int32_t)		// totSeqSize
										   + 3 * sizeof(int32_t);		// totLengthX,Y,Z

		// read header
		switch ( buffer[0] ) {
			case CMD_MOVE_SEQUENCE:
			case CMD_RENDER_AND_MOVE_SEQUENCE: 	sequence.cmd = buffer[idx++];
												break;

			default:							std::cerr << "CncCommandDecoder::decodeMoveSequence(Begin) Invalid command = " << buffer[idx] << std::endl;
												return returnAndNotify(false);;
		}

		// notify
		if ( caller != NULL )
			caller->notifyMoveSequenceBegin(sequence);

		// totHeaderLength + 1: To consider also the portionSize byte after the header
		//                      which is in all other cases at buffer[0]
		if ( nbByte < totHeaderLength + 1) {
			std::cerr << "CncCommandDecoder::decodeMoveSequence() Invalid header length. Buffer size = " << nbByte << std::endl;
			return returnAndNotify(false);;
		}

		// read total length
		sequence.totSeqSize = readInt32();

		// read total path length
		sequence.totLengthX = readInt32();
		sequence.totLengthY = readInt32();
		sequence.totLengthZ = readInt32();

		if ( false ) {
			std::cout << "totSeqSize: " << sequence.totSeqSize << std::endl;
			std::cout << "totLengthX: " << sequence.totLengthX << std::endl;
			std::cout << "totLengthY: " << sequence.totLengthY << std::endl;
			std::cout << "totLengthZ: " << sequence.totLengthZ << std::endl;
		}

		// setup remaing size
		sequence.remainSize = sequence.totSeqSize + totHeaderLength;
	}
	else {
		// notify
		if ( caller != NULL )
			caller->notifyMoveSequenceNext(sequence);
	}

	// ------------------------------------------------------------------------
	// process portioon
	unsigned int portionSize = buffer[idx] + 1;
	if ( idx + portionSize != nbByte ) {
		std::cerr << "CncCommandDecoder::decodeMoveSequence() Invalid portion size" << std::endl;
		std::cerr << " - nbByte      : " << nbByte << std::endl;
		std::cerr << " - idx         : " << idx << std::endl;
		std::cerr << " - portionSize : " << portionSize << std::endl;
		return returnAndNotify(false);;
	}
	idx++;

	// over all positions
	unsigned int byteCounter = 0;
	while ( byteCounter < portionSize) {

		CncLongPosition pos;
		if ( sequence.cmd == CMD_RENDER_AND_MOVE_SEQUENCE ) {
			byteCounter += readPosition(pos);
		}
		else if ( sequence.cmd == CMD_MOVE_SEQUENCE ) {
			byteCounter += readOneByteToPos(pos);
		}
		else {
			std::cerr << "CncCommandDecoder::decodeMoveSequence(portion) Invalid command = " << buffer[idx]
			          << ". While reading portion" << std::endl;
			return returnAndNotify(false);
		}

		// notfy
		if ( caller != NULL )
			caller->notifyMove(pos.getX(), pos.getY(), pos.getZ());

		// store
		sequence.positions.push_back(pos);
	}

	// update remaining size
	sequence.remainSize -= (idx - 1);

	if ( false )
		std::cout << "sequence.remainSize = " << sequence.remainSize << std::endl;

	return returnAndNotify(true);
}

///////////////////////////////////////////////////////////////////
bool CncCommandDecoder::decodeMove(const unsigned char *buffer, unsigned int nbByte,
		                           CncCommandDecoder::CallbackInterface* caller)
///////////////////////////////////////////////////////////////////
{
	int32_t x = 0;
	int32_t y = 0;
	int32_t z = 0;
	bool ret = CncCommandDecoder::decodeMove(buffer, nbByte, x , y , z);

	if ( caller != NULL )
		caller->notifyMove(x, y, z);

	return ret;
}
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
					std::cerr << "CncCommandDecoder::decodeMove() error." << std::endl;
					std::cerr << " Invalid byte count: " << nbByte << std::endl;
					return false;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncCommandDecoder::decodeSetter(const unsigned char *buffer, unsigned int nbByte,
									 CncCommandDecoder::CallbackInterface* caller)
///////////////////////////////////////////////////////////////////
{
	CncCommandDecoder::SetterInfo si;
	bool ret = CncCommandDecoder::decodeSetter(buffer, nbByte, si);

	if ( caller != NULL )
		caller->notifySetter(si);

	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncCommandDecoder::decodeSetter(const unsigned char *buffer, unsigned int nbByte,
               		                 CncCommandDecoder::SetterInfo& si)
///////////////////////////////////////////////////////////////////
{
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
