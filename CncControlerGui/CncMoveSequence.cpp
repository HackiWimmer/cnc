#include "CncArduino.h"
#include "CncMoveSequence.h"

#ifdef __WXGTK__
	#include <arpa/inet.h>
#endif

#ifdef __WXMSW__
	#include <winsock.h>
#endif


///////////////////////////////////////////////////////////////////
CncMoveSequence::CncMoveSequence(unsigned char pid)
: sequence()
, portionIndex()
, data()
, type(pid)
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

	const int rest = MAX_SERIAL_BUFFER_SIZE - getHeaderSize();
	wxASSERT ( rest >= 0 );

	return ( rest / getPointSize() );
}
///////////////////////////////////////////////////////////////////
unsigned int CncMoveSequence::getHeaderSize() const {
///////////////////////////////////////////////////////////////////
	const unsigned int headerSize 	= sizeof(int8_t) 		// CMD
									+ sizeof(int32_t);		// SIZE

	return headerSize;
}
///////////////////////////////////////////////////////////////////
unsigned int CncMoveSequence::getPointSize() const {
///////////////////////////////////////////////////////////////////
	if ( isValid() == false )
		return 0;

	return (type == CMD_RENDER_AND_MOVE_SEQUENCE ? SequencePoint::MaxPointSize : 1);
}
///////////////////////////////////////////////////////////////////
unsigned int CncMoveSequence::determineSafeBufferSize() const {
///////////////////////////////////////////////////////////////////
	if ( isValid() == false )
		return 0;

	const unsigned int headerSize 	= getHeaderSize();
	const unsigned int pointSize 	= getPointSize();

	//                                                                      offset to be safe
	const unsigned int portionCount	= getCount() / MAX_SERIAL_BUFFER_SIZE 	+ 1;
	const unsigned int dataSize		= getCount() * pointSize				+ pointSize;

	// + ( portionCount * 1 ): Each portion start with one byte of the
	// current portion size
	return ( headerSize + dataSize + ( portionCount * 1 ) );
}
///////////////////////////////////////////////////////////////////
void CncMoveSequence::addPosXYZ(int32_t dx, int32_t dy, int32_t dz)  {
///////////////////////////////////////////////////////////////////
	if ( isValid() == false )
		return;

	if ( dx == 0 && dy == 0 && dz == 0 )
		return;

	sequence.push_back(SequencePoint(dx, dy, dz));
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
		moveSequenceBuffer[byteCount] = type;
		byteCount++;
		pPointer++;

		// HEADER - SIZE (empty)
		flushInt32(0);

		// HEADER - PATH LENGTHES
		flushInt32(data.lengthX);
		flushInt32(data.lengthZ);
		flushInt32(data.lengthZ);

		// ensure the header can be read in one serial portion
		wxASSERT( byteCount < MAX_SERIAL_BUFFER_SIZE);
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
		sp.calculateParameters();

		// switch portion
		if ( portionSize + sp.param.necessarySize > MAX_SERIAL_BUFFER_SIZE ) {
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
		if ( byteCount >= moveSequenceBufferSize - getPointSize() ) {
			std::cerr << "CncMoveSequence::flush(): The max buffer size is reached!" << std::endl;
			std::cerr << " - The sequence is definitely incomplete!" << std::endl;
			std::cerr << " - This shouldn't be happen because the buffer is created dynamically!" << std::endl;

			wxASSERT(byteCount < moveSequenceBufferSize - getPointSize());
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
		std::cout << "portionIndexCount       : " << getPortionIndex().size()	<< std::endl;

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
	auto putOneByte = [&](int8_t x, int8_t y, int8_t z) {
		unsigned char xyz = 0;

		if ( x > 0 ) 	setBit(xyz, 1);
		else 			setBit(xyz, 2);

		if ( y > 0 ) 	setBit(xyz, 3);
		else 			setBit(xyz, 4);

		if ( z > 0 ) 	setBit(xyz, 5);
		else 			setBit(xyz, 6);

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

	unsigned char pid = 0;
	// -------------------------------------------------------------
	auto flushPoint_RENDER_AND_MOVE_SEQUENCE = [&]() {
		pid = sp.param.pid;
		copy(pointer, &pid, 1);

		switch ( pid ) {
			case  PID_MV_SEQ_0_XYZ:	putOneByte(sp.x, sp.y, sp.z);					break;
			case  PID_MV_SEQ_1_XYZ:	putInt8(sp.x);	putInt8(sp.y); 	putInt8(sp.z); 	break;
			case  PID_MV_SEQ_2_XYZ:	putInt16(sp.x);	putInt16(sp.y); putInt16(sp.z); break;
			case  PID_MV_SEQ_4_XYZ:	putInt32(sp.x);	putInt32(sp.y); putInt32(sp.z); break;

			case  PID_MV_SEQ_0_XY:	putOneByte(sp.x, sp.y, sp.z);					break;
			case  PID_MV_SEQ_1_XY:	putInt8(sp.x);	putInt8(sp.y); 				 	break;
			case  PID_MV_SEQ_2_XY:	putInt16(sp.x);	putInt16(sp.y);					break;
			case  PID_MV_SEQ_4_XY:	putInt32(sp.x);	putInt32(sp.y);					break;

			case  PID_MV_SEQ_0_X:	putOneByte(sp.x, sp.y, sp.z);					break;
			case  PID_MV_SEQ_1_X:	putInt8(sp.x);				 				 	break;
			case  PID_MV_SEQ_2_X:	putInt16(sp.x);				 				 	break;
			case  PID_MV_SEQ_4_X:	putInt32(sp.x);				 				 	break;

			case  PID_MV_SEQ_0_Y:	putOneByte(sp.x, sp.y, sp.z);					break;
			case  PID_MV_SEQ_1_Y:	putInt8(sp.y);				 				 	break;
			case  PID_MV_SEQ_2_Y:	putInt16(sp.y);				 				 	break;
			case  PID_MV_SEQ_4_Y:	putInt32(sp.y);				 				 	break;

			case  PID_MV_SEQ_0_Z:	putOneByte(sp.x, sp.y, sp.z);					break;
			case  PID_MV_SEQ_1_Z:	putInt8(sp.z);				 				 	break;
			case  PID_MV_SEQ_2_Z:	putInt16(sp.z);				 				 	break;
			case  PID_MV_SEQ_4_Z: 	putInt32(sp.z);				 				 	break;
		}
	};

	// -------------------------------------------------------------
	auto flushPoint_MOVE_SEQUENCE = [&]() {
		pid = sp.param.pid;
		switch ( pid ) {
			case  PID_MV_SEQ_0_XYZ:	putOneByte(sp.x, sp.y, sp.z);	break;
			case  PID_MV_SEQ_0_XY:	putOneByte(sp.x, sp.y,    0);	break;
			case  PID_MV_SEQ_0_X:	putOneByte(sp.x,    0,    0);	break;
			case  PID_MV_SEQ_0_Y:	putOneByte(   0, sp.y,    0);	break;
			case  PID_MV_SEQ_0_Z:	putOneByte(   0,    0, sp.z);	break;
		}
	};

	// -------------------------------------------------------------
	if ( type == CMD_RENDER_AND_MOVE_SEQUENCE ) flushPoint_RENDER_AND_MOVE_SEQUENCE();
	else 										flushPoint_MOVE_SEQUENCE();

	if ( false ) {
		std::cout << " pid                    : " << (int)pid	<< std::endl;
		std::cout << " byteCount              : " << byteCount	<< std::endl;
	}

	return byteCount;
}



