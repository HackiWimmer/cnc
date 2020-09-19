#include <iostream>
#include <fstream>

#include <chrono>
#include <sys/time.h>
#include "CncCommon.h"
#include "CncControl.h"
#include "MainFrame.h"
#include "CncContext.h"
#include "SerialEmulatorNull.h"

static const short POINT_LENGTH = 3;
int pointA[POINT_LENGTH], pointB[POINT_LENGTH];

///////////////////////////////////////////////////////////////////
SerialEmulatorNULL::SerialEmulatorNULL(CncControl* cnc)
: SerialSpyPort							(cnc)
, ArduinoPositionRenderer				()
, ArduinoAccelManager					()
, CncCommandDecoder::CallbackInterface	()
, posReplyThreshold						(1)
, limitStates							()
, tsMoveStart							(0LL)
, usToSleep								(0LL)
, stepperEnableState					(false)
, positionCounter						(MIN_LONG)
, stepCounterX							(MIN_LONG)
, stepCounterY							(MIN_LONG)
, stepCounterZ							(MIN_LONG)
, positionOverflowCounter				(0)
, stepOverflowCounterX					(0)
, stepOverflowCounterY					(0)
, stepOverflowCounterZ					(0)
, setterMap								()
, targetMajorPos						(0L, 0L, 0L)
, curEmulatorPos						(0L, 0L, 0L)
, cfgFeedSpeed_MMMin					(0.0)
, rtmFeedSpeed_MMMin					(0.0)
, lastCommand							()
, lastSignal							(CMD_INVALID)
, maxDimStepsX							(THE_CONFIG->getMaxDimensionStepsX())
, maxDimStepsY							(THE_CONFIG->getMaxDimensionStepsY())
, maxDimStepsZ							(THE_CONFIG->getMaxDimensionStepsZ())

///////////////////////////////////////////////////////////////////
{
	reset();
}
///////////////////////////////////////////////////////////////////
SerialEmulatorNULL::~SerialEmulatorNULL() {
///////////////////////////////////////////////////////////////////
	reset();
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::waitDuringRead(unsigned int millis) {
///////////////////////////////////////////////////////////////////
	// do nothing because the emulator runs in the same thread as
	// the application around
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::sleepMilliseconds(unsigned int millis) {
///////////////////////////////////////////////////////////////////
	// do nothing because the emulator runs in the same thread as
	// the application arround
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::addErrorInfo(unsigned char eid, const wxString& text) {
///////////////////////////////////////////////////////////////////
	lastCommand.Serial.write(RET_SOH);
	lastCommand.Serial.write(PID_MSG);
	lastCommand.Serial.write(MT_ERROR);
	
	if ( eid != E_NO_ERROR ) {
		lastCommand.Serial.write(MT_MID_FLAG);
		lastCommand.Serial.write(eid);
	}
	
	lastCommand.Serial.write(text);
	
	lastCommand.Serial.write(MBYTE_CLOSE);
	lastCommand.Serial.write(RET_OK);
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::reset() {
///////////////////////////////////////////////////////////////////
	lastCommand.restLastCmd();
	lastSignal = CMD_INVALID;
	
	lastCommand.MoveSequence.reset();
	
	posReplyThreshold = 1;
	curEmulatorPos.setXYZ(0L, 0L, 0L);
	
	setterMap.clear();
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::resetPositionCounter() {
///////////////////////////////////////////////////////////////////
	positionCounter			= MIN_LONG;
	positionOverflowCounter	= 0;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::resetStepCounter() {
///////////////////////////////////////////////////////////////////
	stepCounterX			= MIN_LONG;
	stepCounterY			= MIN_LONG;
	stepCounterZ			= MIN_LONG;
	stepOverflowCounterX	= 0;
	stepOverflowCounterY	= 0;
	stepOverflowCounterZ	= 0;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::resetCounter() {
///////////////////////////////////////////////////////////////////
	resetPositionCounter();
	resetStepCounter();
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::evaluatePositions(GetterValues& ret) {
///////////////////////////////////////////////////////////////////
	ret.push_back(targetMajorPos.getX());
	ret.push_back(targetMajorPos.getY());
	ret.push_back(targetMajorPos.getZ());
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::evaluateLimitStates(GetterValues& list) {
///////////////////////////////////////////////////////////////////
	bool ret = evaluateLimitStates();
	
	list.push_back(limitStates.getXLimit());
	list.push_back(limitStates.getYLimit());
	list.push_back(limitStates.getZLimit());

	return ret;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::evaluateLimitStates() {
///////////////////////////////////////////////////////////////////
	limitStates.reset();
	
	evaluateLimitStateX();
	evaluateLimitStateY();
	evaluateLimitStateZ();
	
	return limitStates.hasLimit();
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::evaluateLimitStateX() {
///////////////////////////////////////////////////////////////////
	bool ret = false;
	
	if ( curEmulatorPos.getX() <= -maxDimStepsX ) {
		limitStates.setXLimit(LimitSwitch::LIMIT_MIN);
		ret = true;
	}
		
	if ( curEmulatorPos.getX() >= +maxDimStepsX ) {
		limitStates.setXLimit(LimitSwitch::LIMIT_MAX);
		ret = true;
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::evaluateLimitStateY() {
///////////////////////////////////////////////////////////////////
	bool ret = false;
	
	if ( curEmulatorPos.getY() <= -(maxDimStepsY) ) {
		limitStates.setYLimit(LimitSwitch::LIMIT_MIN);
		ret = true;
	}
		
	if ( curEmulatorPos.getY() >= +(maxDimStepsY) ) {
		limitStates.setYLimit(LimitSwitch::LIMIT_MAX);
		ret = true;
	}

	return ret;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::evaluateLimitStateZ() {
///////////////////////////////////////////////////////////////////
	bool ret = false;
	
	if ( curEmulatorPos.getZ() <= -(maxDimStepsZ) ) {
		limitStates.setZLimit(LimitSwitch::LIMIT_MIN);
		ret = true;
	}
		
	if ( curEmulatorPos.getZ() >= +(maxDimStepsZ) ) {
		limitStates.setZLimit(LimitSwitch::LIMIT_MAX);
		ret = true;
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::isReadyToRun() {
///////////////////////////////////////////////////////////////////
	if ( isAccelInitialized() == false ) {
		std::cerr << CNC_LOG_FUNCT << " ArduinoAccelManager isn't initialized!" << std::endl;
		return false;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
int SerialEmulatorNULL::readData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	int ret = 0;
	
	static wxString firmWare(wxString::Format("%s", FIRMWARE_VERSION));
	static wxString retStr;
	
	// if the lastCommand.index is 0 then no read-spooling information available
	// or in other words lastCommand.Serial is EMPTY
	if ( lastCommand.index == 0 )  {
		switch( lastCommand.cmd ) { 
			
			case CMD_INVALID:						// do nothing if no command is registered
													ret = 0;
													break;
			
			case CMD_GETTER:						ret = performSerialBytes((unsigned char*)(buffer), nbByte);
													break;
			
			case CMD_IDLE:							ret = performSerialBytes((unsigned char*)(buffer), nbByte);
													break;
			
			case CMD_MOVE:
			case CMD_RENDER_AND_MOVE:
			case CMD_MOVE_UNIT_LIMIT_IS_FREE:		ret = performMajorMove((unsigned char*)(buffer), nbByte);
													break;
			
			case CMD_MOVE_SEQUENCE:					
			case CMD_RENDER_AND_MOVE_SEQUENCE:		ret = performSequenceMove((unsigned char*)(buffer), nbByte);;
													break;
			
			case CMD_PRINT_VERSION: 				ret = performText((unsigned char*)(buffer), nbByte, firmWare);
													break;
			
			case CMD_PRINT_TIMESTAMP: 				ret = performText((unsigned char*)(buffer), nbByte, __TIMESTAMP__);
													break;
			
			case CMD_PRINT_CONFIG: 					ret = performConfiguration((unsigned char*)(buffer), nbByte);
													break;
			
			case CMD_PRINT_PIN_REPORT: 				ret = performText((unsigned char*)(buffer), nbByte, wxString::Format("%i:0:0:0\n", MAX_PINS)); // see DataControlModel::addPinReportRow(...) for more details
													break;
			
			default:								// for all unsupported commands provide
													// a positive feedback
													((unsigned char*)buffer)[0] = RET_OK;
													ret = 1;
													lastCommand.restLastCmd();
		}
		
	} else {
		
		ret = 0;
		
		// only if an command is registed
		if ( lastCommand.cmd != CMD_INVALID ) {
			// now something should be availibale at the serial
			ret = performSerialBytes((unsigned char*)(buffer), nbByte);
		}
	}
	
	spyReadData(buffer, ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
int SerialEmulatorNULL::performSerialBytes(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	// only if something is requested
	// assumtion: buffer allocates nbByte bytes  
	if ( nbByte > 0 ) {
		// debug only
		bool debug = false;
		if ( debug ) {
			std::cout << "performSerialBytes: " << "index: " << lastCommand.index << ", nbByte: " << nbByte << ", length: " << lastCommand.Serial.length() << ", bytes: ";
			lastCommand.Serial.trace(std::clog);
		}
		
		// indicate end of serial as false
		bool last = false;
		
		// initialize bytes to copy
		unsigned int bytesToCopy = nbByte;
		
		// check boundings
		if ( lastCommand.index + bytesToCopy > lastCommand.Serial.length() ) {
			// in this case not enougth bytes availiable and bytesToCopy 
			// have to be truncated
			
			// prevent an overflow of bytesToCopy which is unsigned
			if ( lastCommand.Serial.length() - lastCommand.index < 0 ) {
				std::cerr << "SerialEmulatorNULL::performSerialBytes Determine bytesToCopy failed!"
				<< "  Current index:  " << lastCommand.index
				<< ", Bytes to copy: "  << bytesToCopy
				<< ", Total length: "   << lastCommand.Serial.length()
				<< std::endl;
				
				std::cerr << "Current command will be aborted!"
				<< std::endl;
				
				lastCommand.restLastCmd();
				return -1;
			}
			
			// indicates the end of the serial
			last = true;
			
			// ... now this substruction is safe
			bytesToCopy = lastCommand.Serial.length() - lastCommand.index;
		}
		
		// if the boundings are ok this definitive the signal to resetLastCmd()
		if ( lastCommand.index + bytesToCopy == lastCommand.Serial.length() )
			last = true;
		
		if ( debug ) {
			std::clog << "bytesToCopy: "<< bytesToCopy << std::endl;
		}
		
		if ( bytesToCopy > 0 ) {
			// read bytes to copy from serial
			if ( lastCommand.Serial.getBytes(buffer, lastCommand.index, bytesToCopy) == false ) {
				std::cerr << "SerialEmulatorNULL::performSerialBytes getBytes(...) failed!"
				<< "  Current index:  " << lastCommand.index
				<< ", Bytes to copy: "  << bytesToCopy
				<< ", Total length: "   << lastCommand.Serial.length()
				<< std::endl;
			
				std::cerr << "Current command will be aborted!"
				<< std::endl;
			
				lastCommand.restLastCmd();
				return -1;
			}

			// index handling
			lastCommand.index += bytesToCopy;
		}
		
		if ( debug ) {
			std::clog << "last: "<< last << std::endl;
		}

		// close/finalize the last command
		if ( last == true )
			lastCommand.restLastCmd();
		
		return bytesToCopy;
	}
	
	return 0;
}
///////////////////////////////////////////////////////////////////
int SerialEmulatorNULL::performText(unsigned char *buffer, unsigned int nbByte, const char* response) {
///////////////////////////////////////////////////////////////////
	wxASSERT( lastCommand.index == 0 );
		
	if ( response == NULL )
		return -1;
		
	if ( buffer == NULL )
		return -1;
		
	lastCommand.Serial.write(RET_SOH);
	lastCommand.Serial.write(PID_TEXT);
	lastCommand.Serial.write(response);
	lastCommand.Serial.write(MBYTE_CLOSE);
	lastCommand.Serial.write(RET_OK);
	
	// support the first byte
	return performSerialBytes(buffer, nbByte);
}

///////////////////////////////////////////////////////////////////
int SerialEmulatorNULL::performConfiguration(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( buffer == NULL )
		return -1;
		
	lastCommand.Serial.write(RET_SOH);
	lastCommand.Serial.write(PID_TEXT);
	lastCommand.Serial.write(wxString::Format("%d:%s\n", PID_COMMON, "Only a setter collection"));

	SetterMap::iterator it;
	for ( it=setterMap.begin(); it!=setterMap.end(); ++it ) {
		wxString valueList(wxString::Format(" %d:", it->first));
		cnc::SetterValueList values = it->second;
		unsigned int counter = 0;
		for ( auto itvl = values.begin(); itvl != values.end(); itvl++ ) {
			
			if ( it->first >= PID_FLOAT_RANG_START )	valueList.append(wxString::Format("%.2f", (float)(*itvl/FLT_FACT)));
			else										valueList.append(wxString::Format("%d",   *itvl));
			
			if ( ++counter != values.size() )
				valueList.append(", ");
		}
		valueList.append("\n");
		lastCommand.Serial.write(valueList);
	}
	
	lastCommand.Serial.write(MBYTE_CLOSE);
	lastCommand.Serial.write(RET_OK);
	
	// support the first byte
	return performSerialBytes(buffer, nbByte);
}
///////////////////////////////////////////////////////////////////
int SerialEmulatorNULL::performSequenceMove(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	wxASSERT( lastCommand.index == 0 );
	lastCommand.Serial.write(lastCommand.MoveSequence.ret);
	
	if ( lastCommand.MoveSequence.ret == RET_OK )
		lastCommand.MoveSequence.reset();
		
	// support the first byte
	return performSerialBytes(buffer, nbByte);
}
///////////////////////////////////////////////////////////////////
int SerialEmulatorNULL::performMajorMove(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	wxASSERT( lastCommand.index == 0 );
	
	if ( buffer == NULL )
		return -1;
	
	// first publish the limit states on demand . . .
	if ( limitStates.hasLimit() || limitStates.hasPreviousLimit() ) {
		lastCommand.Serial.write(RET_SOH);
		lastCommand.Serial.write(PID_LIMIT);
		lastCommand.Serial.write(limitStates.getXLimit(), limitStates.getYLimit(), limitStates.getZLimit());
	}
	
	// secondary write the major position . . .
	const int32_t speed = round(getRealtimeFeedSpeed_MMMin() * FLT_FACT);
	lastCommand.Serial.write(RET_SOH);
	lastCommand.Serial.write(PID_XYZ_POS_MAJOR);
	lastCommand.Serial.write(targetMajorPos.getX(), targetMajorPos.getY(), targetMajorPos.getZ(), speed);
	
	// and last but not least finalize the command
	lastCommand.Serial.write(lastCommand.ret);
	
	// support the first byte
	return performSerialBytes(buffer, nbByte);
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::writeData(void *b, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false )
		return false;
		
	spyWriteData(b, nbByte);
	
	unsigned char* buffer	= ((unsigned char*)b);
	unsigned char cmd 		= buffer[0];
	
	// sequence handling
	if ( lastCommand.MoveSequence.isActive() && cmd != CMD_RESET_CONTROLLER ) 
		cmd = lastCommand.MoveSequence.cmd;
	
	switch ( cmd ) {
		
		// signals
		case SIG_INTERRUPPT:
		case SIG_HALT:
		case SIG_PAUSE:
		case SIG_RESUME:
		case SIG_QUIT_MOVE: 				lastSignal = cmd;
											return true;
									
		// commands
		case CMD_IDLE:						lastCommand.cmd = cmd;
											lastCommand.ret = RET_OK;
											return writeHeartbeat(buffer, nbByte);
		
		case CMD_RESET_CONTROLLER:			reset();
											lastCommand.cmd = cmd;
											lastCommand.ret = RET_OK;
											return true;
		
		case CMD_GETTER:					lastCommand.cmd = cmd;
											lastCommand.ret = RET_OK;
											return writeGetter(buffer, nbByte);
		
		case CMD_SETTER:					lastCommand.cmd = cmd;
											lastCommand.ret = RET_OK;
											return writeSetter(buffer, nbByte);
		
		case CMD_MOVE:
		case CMD_RENDER_AND_MOVE:
		case CMD_MOVE_UNIT_LIMIT_IS_FREE:	lastCommand.cmd = cmd;
											return writeMoveCmdIntern(buffer, nbByte);
		
		case CMD_MOVE_SEQUENCE:
		case CMD_RENDER_AND_MOVE_SEQUENCE:	if ( lastCommand.MoveSequence.isActive() == false) 
												lastCommand.MoveSequence.cmd = cmd;
 
											lastCommand.ret = lastCommand.MoveSequence.ret;
											lastCommand.cmd = lastCommand.MoveSequence.cmd;
											return writeMoveSeqIntern(buffer, nbByte);
		
		default:							lastCommand.cmd = cmd;
											lastCommand.ret = RET_OK;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::writeHeartbeat(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	static int32_t counter = 0;
	
	lastCommand.Serial.write(RET_SOH);
	lastCommand.Serial.write(PID_HEARTBEAT);
	lastCommand.Serial.write(counter++ % INT32_MAX);
	lastCommand.Serial.write((unsigned char)'\0');
	lastCommand.Serial.write((unsigned char)'\0');
	lastCommand.Serial.write((unsigned char)255);
	lastCommand.Serial.write((unsigned char)255);
	lastCommand.Serial.write(RET_OK);
	
	return true;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::writerGetterValues(unsigned char pid, int32_t v) {
///////////////////////////////////////////////////////////////////
	lastCommand.Serial.write(pid);
	lastCommand.Serial.write((unsigned char)1);
	lastCommand.Serial.write(v);
	lastCommand.Serial.write(RET_OK);
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::writerGetterValues(unsigned char pid, int32_t v1, int32_t v2) {
///////////////////////////////////////////////////////////////////
	lastCommand.Serial.write(pid);
	lastCommand.Serial.write((unsigned char)2);
	lastCommand.Serial.write(v1, v2);
	lastCommand.Serial.write(RET_OK);
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::writerGetterValues(unsigned char pid, int32_t v1, int32_t v2, int32_t v3) {
///////////////////////////////////////////////////////////////////
	lastCommand.Serial.write(pid);
	lastCommand.Serial.write((unsigned char)3);
	lastCommand.Serial.write(v1, v2, v3);
	lastCommand.Serial.write(RET_OK);
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::writeGetter(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	wxASSERT(nbByte > 1 );
	unsigned char pid = buffer[1];
	
	lastCommand.Serial.write(RET_SOH);
	lastCommand.Serial.write(PID_GETTER);
	
	switch ( pid ) {
		
		case PID_QUERY_READY_TO_RUN:	    writerGetterValues(pid, (int32_t)isReadyToRun()); break;
		case PID_ENABLE_STEPPERS:	    	writerGetterValues(pid, (int32_t)stepperEnableState); break;
		
		case PID_X_POS:   					writerGetterValues(pid, curEmulatorPos.getX()); break;
		case PID_Y_POS:   					writerGetterValues(pid, curEmulatorPos.getY()); break;
		case PID_Z_POS:  					writerGetterValues(pid, curEmulatorPos.getZ()); break;
		case PID_XY_POS:  					writerGetterValues(pid, curEmulatorPos.getX(), curEmulatorPos.getY()); break;
		case PID_XYZ_POS: 					writerGetterValues(pid, curEmulatorPos.getX(), curEmulatorPos.getY(), curEmulatorPos.getZ()); break;
		
		case PID_LIMIT:						evaluateLimitStates();
											writerGetterValues(pid, limitStates.getXLimit(), limitStates.getYLimit(), limitStates.getZLimit());
											break;
		
		case PID_GET_POS_COUNTER:			writerGetterValues(pid, getEmuPositionCounter(), getEmuPositionOverflowCounter()); break;
		
		case PID_GET_STEP_COUNTER_X:		writerGetterValues(pid, getEmuStepCounterX(), getEmuStepOverflowCounterX()); break;
		case PID_GET_STEP_COUNTER_Y:		writerGetterValues(pid, getEmuStepCounterY(), getEmuStepOverflowCounterY()); break;
		case PID_GET_STEP_COUNTER_Z:		writerGetterValues(pid, getEmuStepCounterZ(), getEmuStepOverflowCounterZ()); break;
		
		default:							auto it = setterMap.find((int)pid);
											if ( it != setterMap.end() ) {
												
												cnc::SetterValueList values = (*it).second;
												lastCommand.Serial.write(pid);
												lastCommand.Serial.write((unsigned char)values.size());
												
												for ( auto itvl = values.begin(); itvl != values.end(); itvl++ )
													lastCommand.Serial.write(*itvl);
												
											} else {
												addErrorInfo(E_GETTER_ID_NOT_FOUND, wxString::Format("Getter as int value: %03d", (int)pid));
												writerGetterValues(PID_UNKNOWN, 0);
												
												lastCommand.ret = RET_ERROR;
												return false;
											}
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::notifySetter(const CncCommandDecoder::SetterInfo& si) {
///////////////////////////////////////////////////////////////////
	// currently noting to do
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::writeSetter(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( writeSetterRawCallback(buffer, nbByte) == false ) {
		lastCommand.ret = RET_ERROR;
		return false;
	}

	const unsigned int offset = 3; // CMD + PID + COUNT
	unsigned int valueSize  = nbByte - offset;
	
	if ( valueSize % LONG_BUF_SIZE == 0 ) {
		unsigned int valueCount = valueSize / LONG_BUF_SIZE;
		unsigned int index      = offset;
		unsigned char pid       = buffer[1];
		unsigned char valBuf[LONG_BUF_SIZE];
		
		// update setter map
		cnc::SetterValueList values;
		for ( unsigned int i=0; i<valueCount; i++ ) {
			
			valBuf[0] = buffer[index + 0];
			valBuf[1] = buffer[index + 1];
			valBuf[2] = buffer[index + 2];
			valBuf[3] = buffer[index + 3];
			
			int32_t value = 0;
			memcpy(&value, valBuf, LONG_BUF_SIZE);
			values.push_back(value);
		}
		
		setterMap[pid] = values;

		// -------------------------------------------------------------
		auto setupACM = [&](float AA, float AB, float AC, float DA, float DB, float DC) {
			Function fA(AA, AB, AC), fD(DA, DB, DC);
			ArduinoAccelManager::initialize(fA, fD);
		};

		// special handling for later use
		switch ( pid ) {
			case PID_ENABLE_STEPPERS:		stepperEnableState = (bool)( values.size() > 0 ? values.front() : 0 ); break;
			case PID_POS_REPLY_THRESHOLD:	posReplyThreshold  = ( values.size() > 0 ? values.front() : 0 ); break;
			
			case PID_RESERT_POS_COUNTER:  	resetPositionCounter(); break;
			case PID_RESERT_STEP_COUNTER: 	resetStepCounter(); 	break;
			
			case PID_X_POS:   				curEmulatorPos.setX(( values.size() > 0 ? values.front() : 0 )); break;
			case PID_Y_POS:   				curEmulatorPos.setY(( values.size() > 0 ? values.front() : 0 )); break;
			case PID_Z_POS:  				curEmulatorPos.setZ(( values.size() > 0 ? values.front() : 0 )); break;
			
			case PID_ACCEL_PROFILE:
			{
				/*
			    values[ 0] / FLT_FACT // Feedrate X
			    values[ 1] / FLT_FACT // Feedrate Y
			    values[ 2] / FLT_FACT // Feedrate Z

			    values[3] / FLT_FACT // HighPulsWidth X
			    values[4] / FLT_FACT // HighPulsWidth Y
			    values[5] / FLT_FACT // HighPulsWidth Z
				*/

				setupACM((float)values[ 6] / FLT_FACT,
						 (float)values[ 7] / FLT_FACT,
						 (float)values[ 8] / FLT_FACT,
						 (float)values[ 9] / FLT_FACT,
						 (float)values[10] / FLT_FACT,
						 (float)values[11] / FLT_FACT);
				break;
			}
			
			case PID_SPEED_MM_SEC: 			setFeedSpeed_MMMin((double)(60.0 * values.front()/FLT_FACT));
											break;
		}
		
		return true;
	}
	
	lastCommand.ret = RET_ERROR;
	return false;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::notifyMoveSequenceBegin(const CncCommandDecoder::MoveSequenceInfo& sequence) {
///////////////////////////////////////////////////////////////////
	limitStates.reset();

	const int32_t dx = sequence.Out.totLengthX;
	const int32_t dy = sequence.Out.totLengthY;
	const int32_t dz = sequence.Out.totLengthZ;

	// update speed simulator values
	initializeFeedProfile(dx, dy, dz);
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::notifyMoveSequenceNext(const CncCommandDecoder::MoveSequenceInfo& sequence) {
///////////////////////////////////////////////////////////////////
	// currently nothing to do
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::notifyMoveSequenceEnd(const CncCommandDecoder::MoveSequenceInfo& sequence) {
///////////////////////////////////////////////////////////////////
	if ( lastCommand.MoveSequence.sequence.Out.remainSize != 0 ) {
		std::cerr 	<< "SerialEmulatorNULL::notifyMoveSequenceEnd: Quality check failed" << std::endl
					<< " - lastCommand.MoveSequence.sequence.remainSize: " << lastCommand.MoveSequence.sequence.Out.remainSize 
					<< std::endl;
	}

	// perform any rest offset
	completeFeedProfile();

	// reset last signal
	lastSignal = CMD_INVALID;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::notifyMove(int32_t dx, int32_t dy, int32_t dz) {
///////////////////////////////////////////////////////////////////
	renderAndMove(dx, dy, dz);
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::writeMoveSeqIntern(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	// this call will activate: notifyMove(int32_t dx, int32_t dy, int32_t dz)
	if ( CncCommandDecoder::decodeMoveSequence(buffer, nbByte, lastCommand.MoveSequence.sequence, this) == false )
		return false;

	// determine handshake
	lastCommand.MoveSequence.ret = RET_MORE;
	
	if ( false )
		std::cout << "lastCommand.MoveSequence.sequence.remainSize: " << lastCommand.MoveSequence.sequence.Out.remainSize << std::endl;
	
	if ( lastCommand.MoveSequence.sequence.Out.isEnd() )
		lastCommand.MoveSequence.ret = RET_OK;
	
	replyPosition(true);

	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::writeMoveCmdIntern(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	limitStates.reset();
	
	if ( writeMoveRawCallback(buffer, nbByte)  == false )
		return false;
	
	int32_t x = 0L, y = 0L, z = 0L;
	if ( CncCommandDecoder::decodeMove(buffer, nbByte, x, y, z) == false ) 
		return false;
		
	if ( lastCommand.cmd == CMD_MOVE_UNIT_LIMIT_IS_FREE ) {
		if ( x != 0 ) limitStates.setLimitX(LimitSwitch::LIMIT_UNSET);
		if ( y != 0 ) limitStates.setLimitY(LimitSwitch::LIMIT_UNSET);
		if ( z != 0 ) limitStates.setLimitZ(LimitSwitch::LIMIT_UNSET);
		
		x = y = z = 0;
	}
	
	// determine the target major position, this is the current pos + the given move
	targetMajorPos.set(curEmulatorPos);
	targetMajorPos.inc(x, y, z);
	
	// the emulator function readData and writeData runs in the same thread.
	// so, it isn't possible to repeat a move command with serval position callbacks
	// as a real mirco controller can do.
	// Instead the the move is supported with its total distance - see readMove. 
	// however, for a preview this is good enougth!
	//
	// the following linear rendering is only to support a more detailed writeMoveCmd(...)
	const bool ret = initRenderAndMove(x, y, z);
	
	replyPosition(true);
	
	// reset last signal
	lastSignal = CMD_INVALID;
	return ret;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::initializeFeedProfile(int32_t dx , int32_t dy , int32_t dz) {
///////////////////////////////////////////////////////////////////
	static ArduinoImpulseCalculator impulseCalculator;
	const int32_t impulses  = impulseCalculator.calculate(dx, dy, dz);
	const double speed		= getFeedSpeed_MMSec();
	
	const bool ret = speed && impulses ? initMove(impulses, speed) : true;
	
	usToSleep = 0LL;
	
	if ( ret == false )
		std::cerr << CNC_LOG_FUNCT << wxString::Format(": initMove(impulses=%ld, speed=%lf, dx=%ld, dy=%ld, dz=%ld) failed!", impulses, speed, dx, dy, dz) << std::endl;
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::completeFeedProfile() {
///////////////////////////////////////////////////////////////////
	// nothing more to do
	if ( usToSleep >= 0LL )
		CncTimeFunctions::sleepMircoseconds(usToSleep);
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::initRenderAndMove(int32_t dx , int32_t dy , int32_t dz) {
///////////////////////////////////////////////////////////////////
	if ( initializeFeedProfile(dx, dy, dz) == false )
		return false;
	
	if ( renderAndMove(dx, dy, dz) == false )
		return false;
		
	return completeFeedProfile();;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::renderAndMove(int32_t dx, int32_t dy, int32_t dz) {
///////////////////////////////////////////////////////////////////
	// presetup the move return value, 
	// it will be overriden by movingXYZ on demand
	lastCommand.ret = RET_OK;
	
	tsMoveStart = CncTimeFunctions::getNanoTimestamp();
	if ( renderMove(dx, dy, dz) != RET_OK )
		return false;
	
	// do something with this coordinates
	return writeMoveRenderedCallback(dx, dy, dz);
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::translateStepAxisRetValue(unsigned char ret) {
///////////////////////////////////////////////////////////////////
	lastCommand.ret = ret;
	
	switch ( ret ) {
		case RET_INTERRUPT:
		case RET_ERROR:			return false;
		
		case RET_LIMIT:
		case RET_HALT:
		case RET_QUIT:
		default:				return true;
	}
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::replyPosition(bool force) {
///////////////////////////////////////////////////////////////////
	// simulate a direct controller callback.
	static int64_t lastReplyDistance = 0L;
	const  int64_t stepCounter       = stepCounterX + stepCounterY + stepCounterZ;
	const  int64_t diff              = stepCounter - lastReplyDistance;
	
	if ( diff >= posReplyThreshold || force == true ) {
		// due to the fact, that the emulators runs in the 
		// same thread as the main loop it makes not sense 
		// to write here someting to the serial. This is 
		// because the first main lool readData(...) call 
		// is at the earliest if this writeMove(...) call is 
		// totally finished. So, the one and only way to 
		// communicate continous with the cnc control is to call 
		// the SerialControllrCallback directly. Otherwise the 
		// complete serial data will be fetched in one block 
		// at the end if this writeMove(...) call was finalized.
		
		ContollerInfo ci;
		ci.infoType  			= CITPosition;
		ci.posType   			= curEmulatorPos != targetMajorPos ? PID_XYZ_POS_DETAIL : PID_XYZ_POS_MAJOR;
		ci.synchronizeAppPos 	= shouldCallbackAlsoSynchronizeAppPosition();
		ci.command   			= lastCommand.cmd;
		ci.xCtrlPos  			= curEmulatorPos.getX();
		ci.yCtrlPos  			= curEmulatorPos.getY();
		ci.zCtrlPos  			= curEmulatorPos.getZ();
		
		ci.feedSpeed 			= getRealtimeFeedSpeed_MMMin();
		
		sendSerialControllerCallback(ci);
		
		lastReplyDistance = stepCounter;
	}
}
///////////////////////////////////////////////////////////////////
unsigned char SerialEmulatorNULL::signalHandling() {
///////////////////////////////////////////////////////////////////
	// signal handling
	switch ( lastSignal ) {
	
		case SIG_INTERRUPPT:		return RET_INTERRUPT;
		case SIG_HALT:				return RET_HALT;
		case SIG_QUIT_MOVE:			return RET_QUIT;
		
		case SIG_PAUSE:				// pause handling
									while ( lastSignal == SIG_PAUSE ) {
										THE_APP->dispatchAll();
										THE_APP->waitActive(25, true);
									}
									break;
		
		case SIG_RESUME:			lastSignal = CMD_INVALID; 
									break;
		
		default:					; // Do nothing
	}
	
	return RET_OK;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::resetEmuPositionCounter() {
///////////////////////////////////////////////////////////////////
    positionCounter 		= MIN_LONG;
    positionOverflowCounter = 0;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::resetEmuStepCounter() {
///////////////////////////////////////////////////////////////////
    stepCounterX 			= MIN_LONG;
    stepCounterY 			= MIN_LONG;
    stepCounterZ 			= MIN_LONG;
    stepOverflowCounterX 	= 0;
    stepOverflowCounterY 	= 0;
    stepOverflowCounterZ 	= 0;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::incPosistionCounter() {
///////////////////////////////////////////////////////////////////
    // detect overflows
    if ( positionCounter == MAX_LONG ) {
        positionCounter = MIN_LONG;
        positionOverflowCounter++;
    }

    positionCounter++;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::incStepCounterX(int32_t dx) {
///////////////////////////////////////////////////////////////////
    // detect overflows
    int32_t test = MAX_LONG - ArdoObj::absolute(dx);
    if ( test < stepCounterX ) {
        stepCounterX  = MIN_LONG;
        stepCounterX += (test - stepCounterX);
        stepOverflowCounterX++;
        return;
    }

    stepCounterX += ArdoObj::absolute(dx);
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::incStepCounterY(int32_t dy) {
///////////////////////////////////////////////////////////////////
    // detect overflows
    int32_t test = MAX_LONG - ArdoObj::absolute(dy);
    if ( test < stepCounterY ) {
        stepCounterY  = MIN_LONG;
        stepCounterY += (test - stepCounterY);
        stepOverflowCounterY++;
        return;
    }

    stepCounterY += ArdoObj::absolute(dy);
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::incStepCounterZ(int32_t dz) {
///////////////////////////////////////////////////////////////////
    // detect overflows
    int32_t test = MAX_LONG - ArdoObj::absolute(dz);
    if ( test < stepCounterZ ) {
        stepCounterZ  = MIN_LONG;
        stepCounterZ += (test - stepCounterZ);
        stepOverflowCounterZ++;
        return;
    }

    stepCounterZ += ArdoObj::absolute(dz);
}


///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::notifyACMStateChange(State s) {
///////////////////////////////////////////////////////////////////
	
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::notifyACMInitMove() {
///////////////////////////////////////////////////////////////////
	
}
///////////////////////////////////////////////////////////////////
byte SerialEmulatorNULL::checkRuntimeEnv() {
///////////////////////////////////////////////////////////////////
	const unsigned char retSig = signalHandling();
	
	if ( retSig != RET_OK )  {
		lastCommand.ret = retSig; 
		return translateStepAxisRetValue(retSig); 
	} 
	
	return RET_OK;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::notifyMovePartAfter() {
///////////////////////////////////////////////////////////////////
	// ------------------------------------------------------------
	#define INC_AXIS(axis, delta) \
	{ \
		int32_t d = delta; \
		const int32_t newPos = curEmulatorPos.get##axis() + d; \
		\
		if ( newPos >= +maxDimSteps##axis ) { \
			d = +maxDimSteps##axis - curEmulatorPos.get##axis(); \
			limitStates.setLimit##axis(LimitSwitch::LIMIT_MAX); \
			lastCommand.ret = RET_LIMIT; \
		\
		} else if ( newPos <= -maxDimSteps##axis ) { \
			d = -maxDimSteps##axis - curEmulatorPos.get##axis(); \
			limitStates.setLimit##axis(LimitSwitch::LIMIT_MIN); \
			lastCommand.ret = RET_LIMIT; \
		\
		} else { \
			limitStates.setLimit##axis(LimitSwitch::LIMIT_UNSET); \
		} \
		\
		curEmulatorPos.inc##axis(d); \
		incStepCounter##axis(d); \
		\
	}
	
	if ( RS::stepSignature & ASG_X ) INC_AXIS(X, RS::dx());
	if ( RS::stepSignature & ASG_Y ) INC_AXIS(Y, RS::dy());
	if ( RS::stepSignature & ASG_Z ) INC_AXIS(Z, RS::dz());
	
	#undef INC_AXIS
	
	// speed management
	#warning no speed
	/*
	if ( getFeedSpeed_MMSec() > 0.0 ) {
		
		// determine the distance for each axis, 
		// due to the later pow() abs isn't necessary here 
		const float distX     = RS::xStepCount * THE_CONFIG->getDisplayFactX();
		const float distY     = RS::yStepCount * THE_CONFIG->getDisplayFactY();
		const float distZ     = RS::zStepCount * THE_CONFIG->getDisplayFactZ();
		const float dist_MM   = sqrt(pow(distX, 2) + pow(distY, 2) + pow(distZ, 2));

		// determine the time deviation between ehe measured and configured sight
		const int32_t timeElapsed_US    = (CncTimeFunctions::getNanoTimestamp() - tsMoveStart) / 1000;
		const int32_t timeDistance_US   = round(( 1000.0 * 1000.0 / getNextTargetSpeed_MMSec() ) * dist_MM);
		const int32_t timeDifference_US = timeDistance_US - timeElapsed_US;

		if ( timeDifference_US > 0 )
			usToSleep += timeDifference_US;
			
		if ( usToSleep >= 500 * 1000LL ) {
			CncTimeFunctions::sleepMilliseconds(usToSleep / 1000 );
			usToSleep = 0LL;
			
			// measure the current speed again
			const double rtmF_MMSec = ( 1000.0 * 1000.0 / (CncTimeFunctions::getNanoTimestamp() - tsMoveStart) ) * dist_MM;
			setRealtimeFeedSpeed_MM_MIN(rtmF_MMSec * 1000 * 60);
		}
	}
	 */
	
	// position mnagement
	replyPosition(false);
	incPosistionCounter();
}
///////////////////////////////////////////////////////////////////
byte SerialEmulatorNULL::initiateStep(AxisId aid) {
///////////////////////////////////////////////////////////////////
	switch ( aid ) {
		case IDX_X: return RET_OK;
		case IDX_Y: return RET_OK;
		case IDX_Z: return RET_OK;
	}
	
	return RET_ERROR;
}
///////////////////////////////////////////////////////////////////
byte SerialEmulatorNULL::finalizeStep(AxisId aid) {
///////////////////////////////////////////////////////////////////
	switch ( aid ) {
		case IDX_X: return RET_OK;
		case IDX_Y: return RET_OK;
		case IDX_Z: return RET_OK;
	}
	
	return RET_ERROR;
}
