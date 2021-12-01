#include "CncArduino.h"
#include "CncContext.h"
#include "CncConfig.h"
#include "CncMoveSequence.h"
#include "CncFlowPositionConverter.h"
#include "GlobalStrings.h"

extern GlobalConstStringDatabase globalStrings;

#ifdef __WXGTK__
	#include <arpa/inet.h>
#endif

#ifdef __WXMSW__
	#include <winsock.h>
#endif

//////////////////////////////////////////////////////////////////
CncMoveSequence::CncMoveSequence(unsigned char cmd)
: reference(CncTimeFunctions::getNanoTimestamp())
, sequence					()
, portionIndex				()
, data						()
, optimizedClientIds		()
, maxSerialSize				(MAX_SERIAL_BUFFER_SIZE - 4)
, moveCmd					(cmd)
, moveSequenceBuffer		(NULL)
, moveSequenceBufferSize	(0)
, moveSequenceFlushedSize	(0)
, speedInfo					()
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
	sequence			.clear();
	portionIndex		.clear();
	data				.reset();
	optimizedClientIds	.clear();
	
	destroyBuffer();
}
///////////////////////////////////////////////////////////////////
const wxString& CncMoveSequence::getClientIdRangeAsString() const {
///////////////////////////////////////////////////////////////////
	static const wxString fmt(globalStrings.moveSeqRefFormat);
	static wxString ret;
	ret.clear();
	
	if ( optimizedClientIds.size() > 0 ) {
		
		if ( optimizedClientIds.size() > 1 ) {
			ret.append(wxString::Format(fmt, getFirstClientId()));
			ret.append(" ... ");
			ret.append(wxString::Format(fmt, getLastClientId()));
		} else {
			ret.assign(wxString::Format(fmt, getClientId()));
		}
		
	} else {
		ret.assign("/");
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
const wxString& CncMoveSequence::getClientIdsAsString() const {
///////////////////////////////////////////////////////////////////
	static const wxString fmt(globalStrings.moveSeqRefFormat);
	static wxString ret;
	ret.clear();
	
	if ( optimizedClientIds.size() > 0 ) {
		for (auto it = optimizedClientIds.begin(); it != optimizedClientIds.end(); ++it) {
			ret.append(wxString::Format(fmt, *it));
			
			if ( std::distance(optimizedClientIds.begin(), it) < (long)(optimizedClientIds.size() - 1) )
				ret.append(", ");
		}
		
	} else {
		ret.assign("/");
	}
	return ret;
}
///////////////////////////////////////////////////////////////////
long CncMoveSequence::getFirstClientId() const {
///////////////////////////////////////////////////////////////////
	if ( optimizedClientIds.size() == 0 )
		return CLIENT_ID.INVALID;
		
	return optimizedClientIds.front();
}
///////////////////////////////////////////////////////////////////
long CncMoveSequence::getLastClientId() const {
///////////////////////////////////////////////////////////////////
	if ( optimizedClientIds.size() == 0 )
		return CLIENT_ID.INVALID;
		
	return optimizedClientIds.back();
}
///////////////////////////////////////////////////////////////////
void CncMoveSequence::addClientId(long cid) {
///////////////////////////////////////////////////////////////////
	optimizedClientIds.push_back(cid);
	/*
	if  ( optimizedClientIds.size() > 0 ) {
		
		// if  ( optimizedClientIds.back() != cid )
			optimizedClientIds.push_back(cid);
		
	} else {
		
		optimizedClientIds.push_back(cid);
	}
	*/
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

	//                                                                          offset to be always safe
	const unsigned int portionCount	= getCount() * pointSize / maxSerialSize	+ 1;
	const unsigned int dataSize		= getCount() * pointSize					+ pointSize;

	// + ( portionCount * 1 ): Each portion start with one byte of the
	// current portion size
	return ( headerSize + dataSize + ( portionCount * 1 ) );
}
///////////////////////////////////////////////////////////////////
void CncMoveSequence::addMetricPosXYZ(double dx, double dy, double dz) {
///////////////////////////////////////////////////////////////////
	// using the flow converter here to avoid to loos the reset 
	// while still round the values
	static CncFlowPositionConverter flowCnv;
	
	// add givven movements
	flowCnv.set(dx, dy, dz);
	
	// the the converted values
	const CncLongPosition& p = flowCnv.get();
	addStepPosXYZ(p.getX(), p.getY(), p.getZ());
}
///////////////////////////////////////////////////////////////////
void CncMoveSequence::addStepPosXYZ(int32_t dx, int32_t dy, int32_t dz)  {
///////////////////////////////////////////////////////////////////
	if ( isValid() == false )
		return;
		
	if ( dx == 0 && dy == 0 && dz == 0 )
		return;

	PositionStorage::addMove(PositionStorage::TRIGGER_MOV_SEQ_ADD, dx, dy, dz);

	sequence.push_back(SequencePoint(getLastClientId(), dx, dy, dz));
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
	result.type = getType();

	if ( getCount() > 0 )
	{
		const unsigned int flushedCount = flushData(result);
		if ( flushedCount == 0 )
			return false;

		const unsigned int flushedSize = getFlushedSize();
		if ( flushedSize == 0 )
			return false;

		result.bufferSize	= getBufferSize();
		result.buffer		= (unsigned char*)getBuffer();
		result.flushedSize 	= flushedSize;
		result.flushedCount	= flushedCount;
		result.more			= hasMore();
	}

	return true;
}
///////////////////////////////////////////////////////////////////
unsigned int CncMoveSequence::flushData(FlushResult& result) {
///////////////////////////////////////////////////////////////////
	if ( isValid() == false )
		return 0;

	if ( getCount() <= 0 )
		return 0;

	createBuffer();

	unsigned int flushed 	= 0;
	unsigned int byteCount 	= 0;
	unsigned char* pPointer = moveSequenceBuffer;
	unsigned char* pSize    = pPointer + 1;

	// -------------------------------------------------------------
	auto flushInt32 = [&](int32_t value) 
	{
		memcpy(pPointer, &value, sizeof(int32_t));
		byteCount += sizeof(int32_t);
		pPointer  += sizeof(int32_t);
	};

	// -------------------------------------------------------------
	auto flushHeader = [&]() 
	{
		// HEADER - CMD
		moveSequenceBuffer[byteCount] = moveCmd;
		byteCount++;
		pPointer++;

		// HEADER - SIZE (empty)
		flushInt32(0);

		// HEADER - PATH LENGTHES
		flushInt32(data.impulseCount);
		flushInt32(data.lengthX);
		flushInt32(data.lengthY);
		flushInt32(data.lengthZ);
		
		if ( false ) {
			std::cout 	<< "data.length I,X,Y,Z: " 
						<< data.impulseCount << ", "
						<< data.lengthX << ", " << data.lengthY << ", " << data.lengthZ
						<< std::endl;
		}

		// ensure the header can be read in one serial portion
		wxASSERT( byteCount < maxSerialSize );
	};

	// -------------------------------------------------------------
	auto updateHeaderSize = [&] (int32_t value) 
	{
		//int32_t length = htonl(value);
		int32_t length = value;
		memcpy(pSize, &length, sizeof(int32_t));
	};

	// -------------------------------------------------------------
	auto flushPortionSize = [&] (unsigned char value) 
	{
		// log index
		portionIndex.push_back(byteCount);
		// copy value
		memcpy(pPointer, &value, 1);
		byteCount++;
		pPointer++;
	};

	// -------------------------------------------------------------
	auto updatePortionSize = [&] (unsigned int offset, unsigned char value) 
	{
		// copy value
		const unsigned char s = ( portionIndex.size() == 1 ? value - offset : value );
		
		if ( portionIndex.size() > 0 )	memcpy(moveSequenceBuffer + portionIndex.back(), &s, 1);
		else 							memcpy(moveSequenceBuffer                      , &s, 1);
	};

	flushHeader();

	// DATA POINTS
	const unsigned int byteOffset 	= byteCount;
	unsigned int portionSize 		= byteOffset;

	// init the first portion
	flushPortionSize(0);

	for ( auto it = sequence.begin(); it != sequence.end(); ++it ) 
	{
		SequencePoint& sp = *it;
		sp.determineParameters(moveCmd);
		
		// switch portion
		if ( portionSize + sp.param.necessarySize > maxSerialSize )
		{
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
		if ( byteCount > moveSequenceBufferSize - getPointSize() )
		{
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
	if ( false )
	{
		std::cout << "byteOffset              : " << byteOffset					<< std::endl;
		std::cout << "byteCount               : " << byteCount					<< std::endl;
		std::cout << "flushedCount            : " << flushed					<< std::endl;
		std::cout << "remaining size          : " << sequence.size()			<< std::endl;
		std::cout << "portionCount            : " << getPortionIndex().size()	<< std::endl;

		// debug portion index
		std::cout << "   ";
		for ( auto it = getPortionIndex().begin(); it != getPortionIndex().end(); ++it )
		{
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
	auto copy = [&] (unsigned char* dest, void* src, unsigned int len) {
		memcpy(dest, src, len);
		byteCount 	+= len;
		pointer		+= len;
	};

	// -------------------------------------------------------------
	auto putOneByte = [&](int8_t x, int8_t y, int8_t z) {
		unsigned char xyz = 0; // --> 0b00000000
		
		if 		( x > 0 )	{ xyz |= (1 << ArdoObj::OneByte::Index::Pos::X); }
		else if ( x < 0 )	{ xyz |= (1 << ArdoObj::OneByte::Index::Neg::X); }

		if 		( y > 0 )	{ xyz |= (1 << ArdoObj::OneByte::Index::Pos::Y); }
		else if ( y < 0 )	{ xyz |= (1 << ArdoObj::OneByte::Index::Neg::Y); }

		if 		( z > 0 )	{ xyz |= (1 << ArdoObj::OneByte::Index::Pos::Z); }
		else if ( z < 0 )	{ xyz |= (1 << ArdoObj::OneByte::Index::Neg::Z); }
		
		if ( false ) {
			std::cout	<< CNC_LOG_FUNCT	<< ": "
						<< (int)x			<< ", " 
						<< (int)y			<< ", " 
						<< (int)z			<< " -> " 
						<< ArdoObj::OneByte::getAsString(xyz) << std::endl
						;
		}
		
		copy(pointer, &xyz, sizeof(unsigned char));
	};

	// -------------------------------------------------------------
	auto putInt8 = [&](int8_t value) {
		copy(pointer, &value, sizeof(int8_t));
	};

	// -------------------------------------------------------------
	auto putInt16 = [&](int16_t value) {
		copy(pointer, &value, sizeof(int16_t));
	};

	// -------------------------------------------------------------
	auto putInt32 = [&](int32_t value) {
		copy(pointer, &value, sizeof(int32_t));
	};

	// -------------------------------------------------------------
	auto flushPoint_RENDER_AND_MOVE_SEQUENCE = [&]() {
		unsigned char valueType = sp.param.type;
		copy(pointer, &valueType, 1);
		
		ArdoObj::ValueInfo vi(valueType);
		if      ( vi.getByteCount() == 0 ) {
			
			putOneByte(sp.x, sp.y, sp.z);
		}
		else if ( vi.getByteCount() == 1 ) {
			if ( vi.hasX() ) { putInt8(sp.x); }
			if ( vi.hasY() ) { putInt8(sp.y); }
			if ( vi.hasZ() ) { putInt8(sp.z); }
		}
		else if ( vi.getByteCount() == 2 ) {
			if ( vi.hasX() ) { putInt16(sp.x); }
			if ( vi.hasY() ) { putInt16(sp.y); }
			if ( vi.hasZ() ) { putInt16(sp.z); }
		}
		else if ( vi.getByteCount() == 4 ) {
			if ( vi.hasX() ) { putInt32(sp.x); }
			if ( vi.hasY() ) { putInt32(sp.y); }
			if ( vi.hasZ() ) { putInt32(sp.z); }
		}
		else {
			std::cerr << CNC_LOG_FUNCT_A(": Invalid Byte Count: %d", (int)vi.getByteCount() );
		}
	};

	// -------------------------------------------------------------
	auto flushPoint_MOVE_SEQUENCE = [&]() {
		wxASSERT( sp.isOneByte() );
		putOneByte(sp.x, sp.y, sp.z);
	};

	// -------------------------------------------------------------
	if 		( moveCmd == CMD_RENDER_AND_MOVE_SEQUENCE )	flushPoint_RENDER_AND_MOVE_SEQUENCE();
	else if ( moveCmd == CMD_MOVE_SEQUENCE ) 			flushPoint_MOVE_SEQUENCE();
	else    { std::cerr << "CncMoveSequence::flushPoint: Invalid Command: " << moveCmd << std::endl; }

	if ( false ) {
		std::cout << " moveCmd                : " << (int)moveCmd	<< std::endl;
		std::cout << " byteCount              : " << byteCount		<< std::endl;
	}

	return byteCount;
}
///////////////////////////////////////////////////////////////////
std::ostream& CncMoveSequence::outputOperator(std::ostream &ostr, const CncLongPosition startPos) const {
///////////////////////////////////////////////////////////////////
	CncDoublePosition startPosMetric; THE_CONFIG->convertStepsToMetric(startPosMetric, startPos);
	
	ostr << "Type                    : " << ArduinoCMDs::getCMDLabel((int)getType())	<< std::endl
		 << "Reference               : " << getReference()								<< std::endl
		 << "Start Position          : " << cnc::longFormat(startPos)					<< std::endl
		 << "Start Position Metric   : " << cnc::dblFormat(startPosMetric)				<< std::endl
		 << "Buffer Size             : " << getBufferSize()								<< std::endl
		 << "Item Count              : " << getCount()									<< std::endl
		 << "Impulse Count           : " << getImpulseCount()							<< std::endl
		 << "Length X                : " << getLengthX()								<< std::endl
		 << "Length Y                : " << getLengthY()								<< std::endl
		 << "Length Z                : " << getLengthZ()								<< std::endl
		 << "Is optimized            : " << isOptimized()								<< std::endl
		 << "First Client ID         : " << getFirstClientId()							<< std::endl
		 << "Last  Client ID         : " << getLastClientId()							<< std::endl
		 << "Entries: "																	<< std::endl
	;
	
	CncLongPosition iterStartPos(startPos);
	for ( auto it = sequence.begin(); it != sequence.end(); ++it ) {
		const SequencePoint& sp = *it;
		const CncLongPosition newPos(iterStartPos.getX() + sp.x, iterStartPos.getY() + sp.y, iterStartPos.getZ() + sp.z);
		CncDoublePosition newPosMetric; THE_CONFIG->convertStepsToMetric(newPosMetric, newPos);
		
		ostr << wxString::Format("  %05ld ", (long)std::distance(sequence.begin(), it))
			 << "MSE: ( P ) " 
			 << wxString::Format("% 6ld", sp.clientID)				<< " " 
			 << wxString::Format("% 8ld", sp.x)						<< ", " 
			 << wxString::Format("% 8ld", sp.y)						<< ", " 
			 << wxString::Format("% 8ld", sp.z)						<< " --> "
			 << cnc::longFormat(newPos)								<< "| "
			 << cnc::dblFormat(newPosMetric)						<< std::endl
		;
		
		iterStartPos = newPos;
	}
	
	return ostr;
}
///////////////////////////////////////////////////////////////////
CncMoveSequenceImage::CncMoveSequenceImage(CncMoveSequence& sequence)
: valid			(false)
, bufferCopy	(NULL)
, result		()
, portionIndex	()
///////////////////////////////////////////////////////////////////
{
	if ( sequence.flush(result) == false )
	{
		CNC_CERR_FUNCT_A("sequence.flush(...) failed")
		portionIndex.clear();
		return;
	}
	
	// It is necessary to create a deep copy of the sequence byte buffer.
	// This is done outside of the FlushResult structure,
	// because the corresponding buffer is normally handled
	// by the move sequence itself and this class is managed as part of a 
	// container where the original sequence isn't no longer available.
	//
	// And last but not least, it is the main reason of CncMoveSequenceImage
	// to provide a minimal copy of the original MoveSequence.
	bufferCopy = new unsigned char[result.bufferSize];
	memcpy(bufferCopy, result.buffer, result.bufferSize);
	result.buffer = bufferCopy;
	
	// for the portion index the move systematic is much sufficient
	portionIndex = std::move(sequence.getPortionIndex());
	
	//... all other move sequence date isn't necessary to copy or move,
	// because it isn't further needed.
	
	valid = true;
}
///////////////////////////////////////////////////////////////////
CncMoveSequenceImage::~CncMoveSequenceImage() {
///////////////////////////////////////////////////////////////////
	portionIndex.clear();
	
	if ( bufferCopy != NULL )
		delete []bufferCopy;
}