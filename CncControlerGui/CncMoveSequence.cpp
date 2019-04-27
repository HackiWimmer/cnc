#include "CncArduino.h"
#include "CncConfig.h"
#include "CncMoveSequence.h"

#ifdef __WXGTK__
	#include <arpa/inet.h>
#endif

#ifdef __WXMSW__
	#include <winsock.h>
#endif


///////////////////////////////////////////////////////////////////
CncMoveSequence::CncMoveSequence(unsigned char cmd)
: sequence()
, portionIndex()
, data()
, maxSerialSize(MAX_SERIAL_BUFFER_SIZE - 4)
, moveCmd(cmd)
, moveSequenceBuffer(NULL)
, moveSequenceBufferSize(0)
, moveSequenceFlushedSize(0)
///////////////////////////////////////////////////////////////////
{
	wxASSERT(isValid());
}
///////////////////////////////////////////////////////////////////
CncMoveSequence::~CncMoveSequence() {
///////////////////////////////////////////////////////////////////
	destroyBuffer();
}
///////////////////////////////////////////////////////////////////
void CncMoveSequence::clear() {
///////////////////////////////////////////////////////////////////
	sequence.clear();
	data.reset();
	destroyBuffer();
}
///////////////////////////////////////////////////////////////////
unsigned int CncMoveSequence::getCount() const {
///////////////////////////////////////////////////////////////////
	return sequence.size();
}
///////////////////////////////////////////////////////////////////
unsigned int CncMoveSequence::calculateFlushPortionCount() {
///////////////////////////////////////////////////////////////////
	if ( isValid() == false )
		return 0;

	const int rest = maxSerialSize - getHeaderSize();
	wxASSERT ( rest >= 0 );

	return ( rest / getPointSize() );
}
///////////////////////////////////////////////////////////////////
unsigned int CncMoveSequence::getHeaderSize() const {
///////////////////////////////////////////////////////////////////
	const unsigned int headerSize 	= sizeof(int8_t) 		// CMD
									+ sizeof(int32_t)		// SIZE
									+ sizeof(int32_t)		// LEN X
									+ sizeof(int32_t)		// LEN Y
									+ sizeof(int32_t);		// LEN Z

	return headerSize;
}
///////////////////////////////////////////////////////////////////
unsigned int CncMoveSequence::getPointSize() const {
///////////////////////////////////////////////////////////////////
	if ( isValid() == false )
		return 0;

	return (moveCmd == CMD_RENDER_AND_MOVE_SEQUENCE ? SequencePoint::MaxPointSize : 1);
}
///////////////////////////////////////////////////////////////////
unsigned int CncMoveSequence::determineSafeBufferSize() const {
///////////////////////////////////////////////////////////////////
	if ( isValid() == false )
		return 0;

	const unsigned int headerSize 	= getHeaderSize();
	const unsigned int pointSize 	= getPointSize();

	//                                                              			offset to be always safe
	const unsigned int portionCount	= getCount() * pointSize / maxSerialSize 	+ 1;
	const unsigned int dataSize		= getCount() * pointSize					+ pointSize;

	// + ( portionCount * 1 ): Each portion start with one byte of the
	// current portion size
	return ( headerSize + dataSize + ( portionCount * 1 ) );
}
///////////////////////////////////////////////////////////////////
void CncMoveSequence::addMetricPosXYZF(double dx, double dy, double dz, double f) {
///////////////////////////////////////////////////////////////////
	const double sx = dx * GBL_CONFIG->getCalculationFactX();
	const double sy = dy * GBL_CONFIG->getCalculationFactY();
	const double sz = dz * GBL_CONFIG->getCalculationFactZ();
	
	addStepPosXYZF( (int32_t)round(sx), 
					(int32_t)round(sy),
					(int32_t)round(sz),
					0); // ???
}
///////////////////////////////////////////////////////////////////
void CncMoveSequence::addStepPosXYZF(int32_t dx, int32_t dy, int32_t dz, int32_t f)  {
///////////////////////////////////////////////////////////////////
	if ( isValid() == false )
		return;

	if ( dx == 0 && dy == 0 && dz == 0 && f == 0 )
		return;

	sequence.push_back(SequencePoint(dx, dy, dz, f));
	data.add(dx, dy, dz);
}
///////////////////////////////////////////////////////////////////
void CncMoveSequence::createBuffer() {
///////////////////////////////////////////////////////////////////
	destroyBuffer();
	moveSequenceBufferSize = determineSafeBufferSize();

	if ( moveSequenceBufferSize == 0 )
		return;

	moveSequenceBuffer = new unsigned char[moveSequenceBufferSize];
}
///////////////////////////////////////////////////////////////////
void CncMoveSequence::destroyBuffer() {
///////////////////////////////////////////////////////////////////
	if ( moveSequenceBuffer != NULL )
		delete [] moveSequenceBuffer;

	moveSequenceBuffer 		= NULL;
	moveSequenceBufferSize	= 0;
}
///////////////////////////////////////////////////////////////////
bool CncMoveSequence::flush(FlushResult& result) {
///////////////////////////////////////////////////////////////////
	if ( isValid() == false )
		return 0;

	result.reset();

	const unsigned int flushedCount = flushData(result);
	if ( flushedCount == 0 )
		return false;

	const unsigned int flushedSize = getFlushedSize();
	if ( flushedSize == 0 )
		return false;

	result.bufferSize	= getBufferSize();
	result.flushedSize 	= flushedSize;
	result.buffer		= (unsigned char*)getBuffer();
	result.flushedCount	= flushedCount;
	result.more			= hasMore();

	return true;
}
///////////////////////////////////////////////////////////////////
unsigned int CncMoveSequence::flushData(FlushResult& result) {
///////////////////////////////////////////////////////////////////
	if ( isValid() == false )
		return 0;

	createBuffer();

	unsigned int flushed 	= 0;
	unsigned int byteCount 	= 0;
	unsigned char* pPointer = moveSequenceBuffer;
	unsigned char* pSize    = pPointer + 1;

	// -------------------------------------------------------------
	auto flushInt32 = [&](int32_t value) {
		int32_t v = htonl(value);
		memcpy(pPointer, &v, sizeof(int32_t));
		byteCount += sizeof(int32_t);
		pPointer  += sizeof(int32_t);
	};

	// -------------------------------------------------------------
	auto flushHeader = [&]() {
		// HEADER - CMD
		moveSequenceBuffer[byteCount] = moveCmd;
		byteCount++;
		pPointer++;

		// HEADER - SIZE (empty)
		flushInt32(0);

		// HEADER - PATH LENGTHES
		flushInt32(data.lengthX);
		flushInt32(data.lengthY);
		flushInt32(data.lengthZ);
		
		if ( false ) {
			std::cout 	<< "data.length X,Y,Z: " 
						<< data.lengthX << ", " << data.lengthY << ", " << data.lengthZ
						<< std::endl;
		}

		// ensure the header can be read in one serial portion
		wxASSERT( byteCount < maxSerialSize );
	};

	// -------------------------------------------------------------
	auto updateHeaderSize = [&] (int32_t value) {
		int32_t length = htonl(value);
		memcpy(pSize, &length, sizeof(int32_t));
	};

	// -------------------------------------------------------------
	auto flushPortionSize = [&] (unsigned char value) {
		// log index
		portionIndex.push_back(byteCount);
		// copy value
		memcpy(pPointer, &value, 1);
		byteCount++;
		pPointer++;
	};

	// -------------------------------------------------------------
	auto updatePortionSize = [&] (unsigned int offset, unsigned char value) {
		// copy value
		const unsigned char s = ( portionIndex.size() == 1 ? value - offset : value );
		memcpy(moveSequenceBuffer + portionIndex.back(), &s, 1);
	};

	flushHeader();

	// DATA POINTS
	const unsigned int byteOffset 	= byteCount;
	unsigned int portionSize 		= byteOffset;

	// init the first portion
	flushPortionSize(0);

	for ( auto it = sequence.begin(); it != sequence.end(); ++it ) {
		SequencePoint sp = *it;
		sp.determineParameters(moveCmd);
		
		// switch portion
		if ( portionSize + sp.param.necessarySize > maxSerialSize ) {
			// update current portion
			updatePortionSize(byteOffset, portionSize);
			// init next portion
			flushPortionSize(0);
			// reset portion size
			portionSize = 0;
		}

		// flushing point
		const unsigned int fs = flushPoint(sp, pPointer);

		// position management
		byteCount 	+= fs;
		portionSize	+= fs;
		pPointer  	+= fs;

		// error handling
		if ( byteCount > moveSequenceBufferSize - getPointSize() ) {
			std::cerr << "CncMoveSequence::flush(): The max buffer size is reached!" << std::endl;
			std::cerr << " - byteCount : " 									<< byteCount << std::endl;
			std::cerr << " - moveSequenceBufferSize - getPointSize() : "	<< moveSequenceBufferSize - getPointSize() << std::endl;
			std::cerr << " - moveSequenceBufferSize : " 					<< moveSequenceBufferSize << std::endl;
			std::cerr << " - getPointSize()         : " 					<< getPointSize() << std::endl;
			std::cerr << " - The sequence is definitely incomplete!" 		<< std::endl;
			std::cerr << " - This shouldn't be happen because the buffer is created dynamically!" << std::endl;

			wxASSERT(byteCount <= moveSequenceBufferSize - getPointSize());
			break;
		}

		// copy data
		result.sequenceData = data;

		flushed++;
	}

	// update the last portion size
	updatePortionSize(byteOffset, portionSize);

	// Update header size
	updateHeaderSize(byteCount - byteOffset);

	// removed flushed positions
	sequence.erase(sequence.begin(), sequence.begin() + flushed);

	// final byte management
	moveSequenceFlushedSize = byteCount;

	// debug
	if ( false ) {
		std::cout << "byteOffset              : " << byteOffset					<< std::endl;
		std::cout << "byteCount               : " << byteCount					<< std::endl;
		std::cout << "flushedCount            : " << flushed					<< std::endl;
		std::cout << "remaining size          : " << sequence.size()			<< std::endl;
		std::cout << "portionCount            : " << getPortionIndex().size()	<< std::endl;

		// debug portion index
		std::cout << "   ";
		for ( auto it = getPortionIndex().begin(); it != getPortionIndex().end(); ++it ) {
			std::cout << *it << "=" << (int)moveSequenceBuffer[*it] << ", ";

			if ( it != getPortionIndex().begin() && std::distance(getPortionIndex().begin(), it) % 10 == 0 )
				std::cout << std::endl << "   ";
		}
		std::cout << std::endl;
	}

	return flushed;
}
///////////////////////////////////////////////////////////////////
unsigned int CncMoveSequence::flushPoint(const SequencePoint& sp, unsigned char* sequenceBuffer) {
///////////////////////////////////////////////////////////////////
	if ( sequenceBuffer == NULL )
		return 0;

	unsigned char* pointer	= sequenceBuffer;
	unsigned int byteCount	= 0;

	// -------------------------------------------------------------
	auto setBit = [&](unsigned char& value, int idx) {
		switch ( idx ) {
			case 8:     value |= (1 << 7); break;
			case 7:     value |= (1 << 6); break;
			case 6:     value |= (1 << 5); break;
			case 5:     value |= (1 << 4); break;
			case 4:     value |= (1 << 3); break;
			case 3:     value |= (1 << 2); break;
			case 2:     value |= (1 << 1); break;
			case 1:     value |= (1 << 0); break;
		}
	};

	// -------------------------------------------------------------
	auto copy = [&] (unsigned char* dest, void* src, unsigned int len) {
		memcpy(dest, src, len);
		byteCount 	+= len;
		pointer		+= len;
	};

	// -------------------------------------------------------------
	auto putOneByte = [&](int8_t x, int8_t y, int8_t z, int8_t f) {
		unsigned char xyz = 0;

		if 		( x > 0 ) 	setBit(xyz, 1);
		else if ( x < 0 )	setBit(xyz, 2);

		if 		( y > 0 ) 	setBit(xyz, 3);
		else if ( y < 0 ) 	setBit(xyz, 4);

		if 		( z > 0 ) 	setBit(xyz, 5);
		else if ( z < 0 )	setBit(xyz, 6);

		if 		( f > 0 ) 	setBit(xyz, 7);
		else if ( f < 0 )	setBit(xyz, 8);

		copy(pointer, &xyz, sizeof(unsigned char));
	};

	// -------------------------------------------------------------
	auto putInt8 = [&](int8_t value) {
		int8_t v = value;
		copy(pointer, &v, sizeof(int8_t));
	};

	// -------------------------------------------------------------
	auto putInt16 = [&](int16_t value) {
		int16_t v = htons(value);
		copy(pointer, &v, sizeof(int16_t));
	};

	// -------------------------------------------------------------
	auto putInt32 = [&](int32_t value) {
		int32_t v = htonl(value);
		copy(pointer, &v, sizeof(int32_t));
	};

	// -------------------------------------------------------------
	auto flushPoint_RENDER_AND_MOVE_SEQUENCE = [&]() {
		unsigned char valueType = sp.param.type;
		copy(pointer, &valueType, 1);
		
		ValueInfo vi(valueType);
		if ( vi.getByteCount() == 0 ) {
			
			putOneByte(sp.x, sp.y, sp.z, sp.f);
		}
		else if ( vi.getByteCount() == 1 ) {
			if ( vi.hasF() )
				putInt8(sp.f); 
			
			if 		( vi.hasXYZ() ) { putInt8(sp.x); putInt8(sp.y); putInt8(sp.z); }
			else if ( vi.hasXY()  ) { putInt8(sp.x); putInt8(sp.y); }
			else if ( vi.hasX()   ) { putInt8(sp.x); }
			else if ( vi.hasY()   ) { putInt8(sp.y); }
			else if ( vi.hasZ()   ) { putInt8(sp.z); }
		}
		else if ( vi.getByteCount() == 2 ) {
			if ( vi.hasF() )
				putInt16(sp.f); 

			if 		( vi.hasXYZ() ) { putInt16(sp.x); putInt16(sp.y); putInt16(sp.z); }
			else if ( vi.hasXY()  ) { putInt16(sp.x); putInt16(sp.y); }
			else if ( vi.hasX()   ) { putInt16(sp.x); }
			else if ( vi.hasY()   ) { putInt16(sp.y); }
			else if ( vi.hasZ()   ) { putInt16(sp.z); }
		}
		else if ( vi.getByteCount() == 4 ) {
			if ( vi.hasF() )
				putInt32(sp.f); 
			
			if 		( vi.hasXYZ() ) { putInt32(sp.x); putInt32(sp.y); putInt32(sp.z); }
			else if ( vi.hasXY()  ) { putInt32(sp.x); putInt32(sp.y); }
			else if ( vi.hasX()   ) { putInt32(sp.x); }
			else if ( vi.hasY()   ) { putInt32(sp.y); }
			else if ( vi.hasZ()   ) { putInt32(sp.z); }
		}
		else {
			std::cerr << "CncMoveSequence::flushPoint::flushPoint_RENDER_AND_MOVE_SEQUENCE: Invalid ByteCount: " << vi.getByteCount() << std::endl;
		}
	};

	// -------------------------------------------------------------
	auto flushPoint_MOVE_SEQUENCE = [&]() {
		wxASSERT( sp.isOneByte() );
		putOneByte(sp.x, sp.y, sp.z, sp.f);
	};

	// -------------------------------------------------------------
	if 		( moveCmd == CMD_RENDER_AND_MOVE_SEQUENCE )	flushPoint_RENDER_AND_MOVE_SEQUENCE();
	else if ( moveCmd == CMD_MOVE_SEQUENCE ) 			flushPoint_MOVE_SEQUENCE();
	else {
		std::cerr << "fCncMoveSequence::flushPoint: Invalid Command: " << moveCmd << std::endl;
	}

	if ( false ) {
		std::cout << " moveCmd                : " << (int)moveCmd	<< std::endl;
		std::cout << " byteCount              : " << byteCount		<< std::endl;
	}

	return byteCount;
}



