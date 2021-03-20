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
CncCommandDecoder::MoveSequenceRecreator::MoveSequenceRecreator(CncMoveSequence* ms) 
: CallbackInterface()
, moveSequence(ms)
///////////////////////////////////////////////////////////////////
{
	wxASSERT( moveSequence != NULL );
}
///////////////////////////////////////////////////////////////////
void CncCommandDecoder::MoveSequenceRecreator::notifyMove(int32_t dx, int32_t dy, int32_t dz) {
///////////////////////////////////////////////////////////////////
	wxASSERT( moveSequence != NULL );
	moveSequence->addStepPosXYZ(dx, dy, dz);
}
///////////////////////////////////////////////////////////////////
void CncCommandDecoder::MoveSequenceRecreator::notifyMoveSequenceBegin(const CncCommandDecoder::MoveSequenceInfo& seqInfo) {
///////////////////////////////////////////////////////////////////
	wxASSERT( moveSequence != NULL );
	moveSequence->setType(seqInfo.Out.cmd);
}


///////////////////////////////////////////////////////////////////
bool CncCommandDecoder::decodeMoveSequence(const unsigned char *buffer, unsigned int nbByte, CncMoveSequence* retSeq) {
///////////////////////////////////////////////////////////////////
	CncCommandDecoder::MoveSequenceRecreator reCreator(retSeq);
	CncCommandDecoder::MoveSequenceInfo seqInfo;
	seqInfo.In.parseAllPortions = true;
	
	return CncCommandDecoder::decodeMoveSequence(buffer, nbByte, seqInfo, &reCreator);
}
///////////////////////////////////////////////////////////////////
int32_t CncCommandDecoder::convertBytesToInt32(const unsigned char *buffer, unsigned int& idx, int32_t& ret) {
///////////////////////////////////////////////////////////////////
	if ( buffer == NULL ) {
		std::cerr << "CncCommandDecoder::convertBytesToInt32() Empty buffer" << std::endl;
		return 0L;
	}
	
	unsigned char b[4];
	
	b[0] = buffer[idx++];
	b[1] = buffer[idx++];
	b[2] = buffer[idx++];
	b[3] = buffer[idx++];
	memcpy(&ret, b, sizeof(int32_t));
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncCommandDecoder::decodeMoveSequence(const unsigned char *buffer, unsigned int nbByte,
										   CncCommandDecoder::MoveSequenceInfo& sequence,
										   CncCommandDecoder::CallbackInterface* caller)
///////////////////////////////////////////////////////////////////
{
	if ( nbByte <= 0 || buffer == NULL ) {
		std::cerr << "CncCommandDecoder::decodeMoveSequence() Empty buffer" << std::endl;
		return false;
	}

	unsigned int idx = 0;

	// ------------------------------------------------------------------------
	auto readOneByteToPos = [&] (CncLongPosition& pos) {
		const unsigned char b = buffer[idx++];
		pos.setX( ArdoObj::OneByte::getX(b) );
		pos.setY( ArdoObj::OneByte::getY(b) );
		pos.setZ( ArdoObj::OneByte::getZ(b) );
		
		if ( false ) {
			std::cout	<< CNC_LOG_FUNCT	<< ": "
						<< pos.getX()		<< ", " 
						<< pos.getY()		<< ", " 
						<< pos.getZ()		<< " -> " 
						<< ArdoObj::OneByte::getAsString(b) << std::endl
						;
		}
		return 1;
	};

	// ------------------------------------------------------------------------
	auto readOneByte = [&] (int32_t& x, int32_t& y, int32_t& z) {
		const unsigned char b = buffer[idx++];
		x = ArdoObj::OneByte::getX(b);
		y = ArdoObj::OneByte::getY(b);
		z = ArdoObj::OneByte::getZ(b);
		
		if ( false ) {
			std::cout	<< CNC_LOG_FUNCT	<< ": "
						<< x				<< ", " 
						<< y				<< ", " 
						<< z				<< " -> " 
						<< ArdoObj::OneByte::getAsString(b) << std::endl
						;
		}
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
		buf[0] = buffer[idx++];
		buf[1] = buffer[idx++];
		memcpy(&ret, buf, sizeof(int16_t));

		return ret;
	};

	// ------------------------------------------------------------------------
	auto readInt32 = [&] () {
		int32_t ret = 0;
		unsigned char buf[sizeof(int32_t)];
		buf[0] = buffer[idx++];
		buf[1] = buffer[idx++];
		buf[2] = buffer[idx++];
		buf[3] = buffer[idx++];
		memcpy(&ret, buf, sizeof(int32_t));

		return ret;
	};

	// ------------------------------------------------------------------------
	auto readPosition = [&] (CncLongPosition& pos) {

		const unsigned char valueType 	= buffer[idx++];
		const ArdoObj::ValueInfo vi(valueType);
		
		if ( vi.isValid() == false ) {
			std::cerr << CNC_LOG_FUNCT << ": Invalid ValueInfo" << std::endl;
			return 0;
		}
			
		const unsigned int byteCount = (unsigned int)(vi.getByteCount()); 
		const unsigned int valCount  = (unsigned int)(vi.getValueCount());
		
		const bool trace = false;
		if ( trace )
			std::cout << "readPosition: " << (int)valueType << ", " << byteCount << ", " << valCount << ", ";

		unsigned int valCounter  = 0;
		unsigned int byteCounter = 1; // consider type

		int32_t values[ArdoObj::ValueInfo::MaxValueCount];
		
		while ( valCounter < valCount ) {
			switch ( byteCount ) {
				case 0:		readOneByte(values[0], values[1], values[2]);
							valCounter  += 3; 	// to break the while loop
							byteCounter += 1;
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
							std::cerr << "CncCommandDecoder::decodeMoveSequence::readPosition() Invalid byteCount = " << byteCount << std::endl;
			}

			valCounter++;
		}

		// assign x, y, z depending on given pid
		if		( vi.isXYZ() ) { pos.setX(values[0]); pos.setY(values[1]); pos.setZ(values[2]); }
		else if ( vi.isXY()  ) { pos.setX(values[0]); pos.setY(values[1]); pos.setZ(0);         }
		else if ( vi.isXZ()  ) { pos.setX(values[0]); pos.setY(0);         pos.setZ(values[1]); }
		else if ( vi.isYZ()  ) { pos.setX(0);         pos.setY(values[0]); pos.setZ(values[1]); }
		else if ( vi.isX()   ) { pos.setX(values[0]); pos.setY(0);         pos.setZ(0);         }
		else if ( vi.isY()   ) { pos.setX(0);         pos.setY(values[0]); pos.setZ(0);         }
		else if ( vi.isZ()   ) { pos.setX(0);         pos.setY(0);         pos.setZ(values[0]); }
		
		if ( trace )
			std::cout << "byteCounter: " << byteCounter << ", pos: " << pos << std::endl;

		return (int)byteCounter;
	};

	// ------------------------------------------------------------------------
	auto returnAndNotify = [&](bool ret) {
		if ( caller != NULL ) {
			if ( sequence.Out.isEnd() )
				caller->notifyMoveSequenceEnd(sequence);
		}

		return ret;
	};

	// ------------------------------------------------------------------------
	// process first portion
	if ( sequence.Out.isBegin() ) {
		const unsigned int totHeaderLength = sizeof(unsigned char)		// pid
				                           + 1 * sizeof(int32_t)		// totSeqSize
										   + 1 * sizeof(int32_t)		// impulseCount
										   + 3 * sizeof(int32_t);		// totLengthX,Y,Z

		// read header
		switch ( buffer[0] ) {
			case CMD_MOVE_SEQUENCE:
			case CMD_RENDER_AND_MOVE_SEQUENCE: 	sequence.Out.cmd = buffer[idx++];
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
			return returnAndNotify(false);
		}

		// read total length and impulse count
		sequence.Out.totSeqSize   = readInt32();
		sequence.Out.impulseCount = readInt32();
		
		// read total path length
		sequence.Out.totLengthX   = readInt32();
		sequence.Out.totLengthY   = readInt32();
		sequence.Out.totLengthZ   = readInt32();

		if ( false ) {
			std::cout << "totSeqSize: " << sequence.Out.totSeqSize << std::endl;
			std::cout << "totLengthX: " << sequence.Out.totLengthX << std::endl;
			std::cout << "totLengthY: " << sequence.Out.totLengthY << std::endl;
			std::cout << "totLengthZ: " << sequence.Out.totLengthZ << std::endl;
		}

		// setup remaing size
		sequence.Out.remainSize = sequence.Out.totSeqSize + totHeaderLength;
	}
	else {
		// single portion mode
		if ( sequence.In.parseAllPortions == false ) {
			// notify
			if ( caller != NULL )
				caller->notifyMoveSequenceNext(sequence);
		}
	}
	
	// ------------------------------------------------------------------------
	auto processPortion = [&]() {
		unsigned int portionSize = buffer[idx] + 1;
		
		// debug
		if ( false ) {
			std::clog << " idx               : " << idx << std::endl;
			std::clog << " portionSize       : " << portionSize << std::endl;
			std::clog << " idx + portionSize : " << idx + portionSize << std::endl;
			std::clog << " nbByte            : " << nbByte << std::endl;
			std::clog << " remainSize        : " << sequence.Out.remainSize << std::endl;
		}
		
		if ( idx + portionSize > nbByte ) {
			std::cerr << "CncCommandDecoder::decodeMoveSequence() Invalid portion size" << std::endl;
			std::cerr << " - nbByte      : " << nbByte << std::endl;
			std::cerr << " - idx         : " << idx << std::endl;
			std::cerr << " - portionSize : " << portionSize << std::endl;
			return false;
		}
		idx++;

		// over all positions
		unsigned int byteCounter = 0;
		while ( byteCounter < portionSize - 1) {

			CncLongPosition pos;
			if ( sequence.Out.cmd == CMD_RENDER_AND_MOVE_SEQUENCE ) {
				
				unsigned int bc = readPosition(pos);
				if ( bc == 0 )
					return false;
					
				byteCounter += bc;
			}
			else if ( sequence.Out.cmd == CMD_MOVE_SEQUENCE ) {
				
				byteCounter += readOneByteToPos(pos);
			}
			else {
				
				std::cerr << "CncCommandDecoder::decodeMoveSequence(portion) Invalid command = " << buffer[idx]
						  << ". While reading portion" << std::endl;
				return false;
			}

			// debug
			if ( false ) {
				std::cout << " pos = " << pos << std::endl;
			}

			// notify
			if ( caller != NULL )
				caller->notifyMove(pos.getX(), pos.getY(), pos.getZ());
			
			// debug
			if ( false ) {
				std::cout << " portionSize         = " << portionSize << std::endl;
				std::cout << " byteCounter         = " << byteCounter << std::endl;
				std::cout << " idx                 = " << idx << std::endl;
				std::cout << " sequence.remainSize = " << sequence.Out.remainSize << std::endl;
				
			}
		}

		// update remaining size
		sequence.Out.remainSize -= (byteCounter + 1);
		//std::clog << portionSize << ", " << idx << ", " << nbByte << ": " << sequence.Out.remainSize << " - " << (nbByte - idx) << std::endl;
		
		return true;
	};
	
	// perform header size
	sequence.Out.remainSize -= idx;
	//std::cout << 0 << ", " << idx << ", " << nbByte << ": " << sequence.Out.remainSize << " - " << (nbByte - idx) << std::endl;
	
	// over all portions
	bool ret = false;
	while ( sequence.Out.remainSize > 0 ) {
		ret = processPortion();
		if ( ret == false ) {
			std::cerr << "CncCommandDecoder::decodeMoveSequence() processPortion() failed!" << std::endl;
			break;
		}
		
		// single portion mode
		if ( sequence.In.parseAllPortions == false )
			break;
			
		// notify
		if ( sequence.Out.remainSize > 0 )
			if ( caller != NULL )
				caller->notifyMoveSequenceNext(sequence);
	}
	
	return returnAndNotify(ret);
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
		case CMD_MOVE_UNTIL_LIMIT_IS_FREE:	break;
		
		default:							std::cerr << "CncCommandDecoder::decodeMove() Invalid command = " << buffer[0] << std::endl;
											return false;
	}

	if ( nbByte < 1 ) {
		std::cerr << "CncCommandDecoder::decodeMove() Empty value count" << std::endl;
		return false;
	}
	
	const unsigned int valueCount = (unsigned int)buffer[1];

	unsigned int idx = 2;

	// idx will be increased by each call of convertBytesToInt32
	// by the sizeof(int32_t)
	switch ( valueCount ) {
		case 1:	convertBytesToInt32(buffer, idx, z);
				break;
					
		case 2:	convertBytesToInt32(buffer, idx, x);
				convertBytesToInt32(buffer, idx, y);
				break;
					
		case 3:	convertBytesToInt32(buffer, idx, x);
				convertBytesToInt32(buffer, idx, y);
				convertBytesToInt32(buffer, idx, z);
				break;

		default:
				std::cerr << "CncCommandDecoder::decodeMove() error." << std::endl;
				std::cerr << " Invalid byte count: " << nbByte << std::endl;
				return false;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncCommandDecoder::decodeSigUpdate(const unsigned char *buffer, unsigned int nbByte, int32_t& x , int32_t& y , int32_t& z) {
///////////////////////////////////////////////////////////////////
	x = 0; y = 0; z = 0;
	
	if ( nbByte <= 0 || buffer == NULL ) {
		std::cerr << "CncCommandDecoder::decodeSigUpdate(): Empty buffer" << std::endl;
		return false;
	}
	
	if ( nbByte < 2 ) {
		std::cerr << "CncCommandDecoder::decodeSigUpdate(); Buffer too short (A)" << std::endl;
		return false;
	}
	
	const unsigned char len = buffer[1];
	if ( nbByte < len ) { 
		std::cerr << "CncCommandDecoder::decodeSigUpdate(): Buffer too short (C1)" << std::endl; 
		return false; 
	}
	
	const unsigned char pid = buffer[2];
	switch ( pid ) {
		case PID_XYZ_INTERACTIVE_POS:
		{
			// As Int8 to resolve also negative values
			x = (int8_t)buffer[3];
			y = (int8_t)buffer[4];
			z = (int8_t)buffer[5];
			
			return true;
		}
		case PID_HEARTBEAT:
		{
			return true;
		}
		default:
		{
			std::cerr << "CncCommandDecoder::decodeSigUpdate(): Invalid PID: " << ArduinoPIDs::getPIDLabel(pid) << std::endl;
		}
	}
	
	return false;
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
		
		// evaluate values
		unsigned int index 	= offset;
		int32_t value 		= 0;
		
		for ( unsigned int i=0; i<si.count; i++ ) {
			// index will be increased by each call of convertBytesToInt32
			// by the sizeof(int32_t)
			convertBytesToInt32(buffer, index, value); 
			si.values.push_back(value);
		}
 
	} else {
		std::cerr << "CncCommandDecoder::decodeSetter() Buffer with wrong size = " << nbByte << std::endl;
		return false;
	}

	return true;
}
