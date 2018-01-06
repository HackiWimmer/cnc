#include <chrono>
#include <sys/time.h>
#include "CncControl.h"
#include "MainFrame.h"
#include "SerialEmulatorNull.h"

int pointA[3], pointB[3];

///////////////////////////////////////////////////////////////////
SerialEmulatorNULL::SerialEmulatorNULL(CncControl* cnc)
: SerialSpyPort(cnc)
, posReplyThresholdX(1)
, posReplyThresholdY(1)
, posReplyThresholdZ(1)
, limitStates()
, speedSimulator(NULL)
, positionCounter(MIN_LONG)
, stepCounterX(MIN_LONG)
, stepCounterY(MIN_LONG)
, stepCounterZ(MIN_LONG)
, positionOverflowCounter(0)
, stepOverflowCounterX(0)
, stepOverflowCounterY(0)
, stepOverflowCounterZ(0)
, setterMap()
, targetMajorPos(0L, 0L, 0L)
, curEmulatorPos(0L, 0L, 0L)
, lastCommand()
, lastSignal(CMD_INVALID)
, errorInfoResponseId(0LL)
, errorList()
///////////////////////////////////////////////////////////////////
{
	speedSimulator = new CncSpeedSimulator(	defaultLoopDuration,
											GBL_CONFIG->getPitchX(), GBL_CONFIG->getStepsX(), 2 * GBL_CONFIG->getPulsWidthOffsetX(),
											GBL_CONFIG->getPitchY(), GBL_CONFIG->getStepsY(), 2 * GBL_CONFIG->getPulsWidthOffsetY(),
											GBL_CONFIG->getPitchZ(), GBL_CONFIG->getStepsZ(), 2 * GBL_CONFIG->getPulsWidthOffsetZ());
	reset();
}
///////////////////////////////////////////////////////////////////
SerialEmulatorNULL::SerialEmulatorNULL(const char *portName)
: SerialSpyPort(portName) 
, posReplyThresholdX(1)
, posReplyThresholdY(1)
, posReplyThresholdZ(1)
, limitStates()
, speedSimulator(NULL)
, positionCounter(MIN_LONG)
, stepCounterX(MIN_LONG)
, stepCounterY(MIN_LONG)
, stepCounterZ(MIN_LONG)
, positionOverflowCounter(0)
, stepOverflowCounterX(0)
, stepOverflowCounterY(0)
, stepOverflowCounterZ(0)
, setterMap()
, targetMajorPos(0L, 0L, 0L)
, curEmulatorPos(0L, 0L, 0L)
, lastCommand()
, lastSignal(CMD_INVALID)
, errorInfoResponseId(0LL)
, errorList()
///////////////////////////////////////////////////////////////////
{
	speedSimulator = new CncSpeedSimulator(	defaultLoopDuration,
											GBL_CONFIG->getPitchX(), GBL_CONFIG->getStepsX(), 2 * GBL_CONFIG->getPulsWidthOffsetX(),
											GBL_CONFIG->getPitchY(), GBL_CONFIG->getStepsY(), 2 * GBL_CONFIG->getPulsWidthOffsetY(),
											GBL_CONFIG->getPitchZ(), GBL_CONFIG->getStepsZ(), 2 * GBL_CONFIG->getPulsWidthOffsetZ());
	reset();
}
///////////////////////////////////////////////////////////////////
SerialEmulatorNULL::~SerialEmulatorNULL() {
///////////////////////////////////////////////////////////////////
	if ( speedSimulator != NULL )
		delete speedSimulator;
		
	reset();
	resetErrorInfo();
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
void SerialEmulatorNULL::performNextErrorInfoResponseId() {
	errorInfoResponseId = CncTimeFunctions::getNanoTimestamp();
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::addErrorInfo(unsigned char eid, const wxString& text) {
///////////////////////////////////////////////////////////////////
	ErrorInfo inf;
	inf.id = eid;
	inf.additionalInfo.assign(text);
	errorList.push_back(inf);
	performNextErrorInfoResponseId();
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::resetErrorInfo() {
///////////////////////////////////////////////////////////////////
	errorList.clear();
	performNextErrorInfoResponseId();
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::reset() {
///////////////////////////////////////////////////////////////////
	lastCommand.restLastCmd();
	lastSignal = CMD_INVALID;
	
	posReplyThresholdX = 1;
	posReplyThresholdY = 1;
	posReplyThresholdZ = 1;
	
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
	stepOverflowCounterX	= 0;;
	stepOverflowCounterY	= 0;;
	stepOverflowCounterZ	= 0;;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::resetCounter() {
///////////////////////////////////////////////////////////////////
	resetPositionCounter();
	resetStepCounter();
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::evaluatePositions(std::vector<int32_t>& ret) {
///////////////////////////////////////////////////////////////////
	ret.push_back(targetMajorPos.getX());
	ret.push_back(targetMajorPos.getY());
	ret.push_back(targetMajorPos.getZ());
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::evaluateLimitStates(std::vector<int32_t>& list) {
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
	
	CncConfig* cncConfig = CncConfig::getGlobalCncConfig();
	
	if ( (targetMajorPos.getX())/cncConfig->getCalculationFactX() <= -cncConfig->getMaxDimensionX() )
		limitStates.setXLimit(LIMIT_MIN);
		
	if ( (targetMajorPos.getX())/cncConfig->getCalculationFactX() >= +cncConfig->getMaxDimensionX() )
		limitStates.setXLimit(LIMIT_MAX);
	
	if ( (targetMajorPos.getY())/cncConfig->getCalculationFactY() <= -cncConfig->getMaxDimensionY() )
		limitStates.setYLimit(LIMIT_MIN);
		
	if ( (targetMajorPos.getY())/cncConfig->getCalculationFactY() >= +cncConfig->getMaxDimensionY() )
		limitStates.setYLimit(LIMIT_MAX);
		
	if ( (targetMajorPos.getZ())/cncConfig->getCalculationFactZ() <= -cncConfig->getMaxDimensionZ() )
		limitStates.setZLimit(LIMIT_MIN);
		
	if ( (targetMajorPos.getY())/cncConfig->getCalculationFactZ() >= +cncConfig->getMaxDimensionZ() )
		limitStates.setZLimit(LIMIT_MAX);
		
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
			
			case CMD_MOVE:
			case CMD_RENDER_AND_MOVE:				ret = performMajorMove((unsigned char*)(buffer), nbByte);
													break;
			
			case CMD_PRINT_VERSION: 				ret = performSOT((unsigned char*)(buffer), nbByte, firmWare);
													break;
			
			case CMD_PRINT_ERRORINFO: 				ret = performErrorInfo((unsigned char*)(buffer), nbByte);
													break;
											
			case CMD_PRINT_LAST_ERROR_RESPONSE_ID:	ret = performLastErrorInfoResponseId((unsigned char*)(buffer), nbByte);
													break;
			
			case CMD_PRINT_CONFIG: 					ret = performConfiguration((unsigned char*)(buffer), nbByte);
													break;
			
			case CMD_PRINT_PIN_REPORT: 				ret = performSOT((unsigned char*)(buffer), nbByte, wxString::Format("%i:0:0:0\n", MAX_PINS)); // see DataControlModel::addPinReportRow(...) for more details
													break;
			
			case CMD_TEST_INFO_MESSAGE:				ret = performMSG((unsigned char*)(buffer), nbByte, wxString::Format("%c%s", 'I', "This is a test message from type: INFO"));
													break;
			
			case CMD_TEST_WARN_MESSAGE:				ret = performMSG((unsigned char*)(buffer), nbByte, wxString::Format("%c%s", 'W', "This is a test message from type: WARNING"));
													break;
			
			case CMD_TEST_ERROR_MESSAGE:			ret = performMSG((unsigned char*)(buffer), nbByte, wxString::Format("%c%s", 'E', "This is a test message from type: ERROR"));
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
	
	spyReadData(ret, buffer, nbByte);
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
			cout << "performSerialBytes: " << "index: " << lastCommand.index << ", nbByte: " << nbByte << ", length: " << lastCommand.Serial.length() << ", bytes: ";
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
				cerr << "SerialEmulatorNULL::performSerialBytes Determine bytesToCopy failed!"
				<< "  Current index:  " << lastCommand.index
				<< ", Bytes to copy: " << bytesToCopy
				<< ", Total length: " << lastCommand.Serial.length()
				<< std::endl;
				
				cerr << "Current command will be aborted!"
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
			clog << "bytesToCopy: "<< bytesToCopy << endl;
		}
		
		if ( bytesToCopy > 0 ) {
			// read bytes to copy from serial
			if ( lastCommand.Serial.getBytes(buffer, lastCommand.index, bytesToCopy) == false ) {
				cerr << "SerialEmulatorNULL::performSerialBytes getBytes(...) failed!"
				<< "  Current index:  " << lastCommand.index
				<< ", Bytes to copy: " << bytesToCopy
				<< ", Total length: " << lastCommand.Serial.length()
				<< std::endl;
			
				cerr << "Current command will be aborted!"
				<< std::endl;
			
				lastCommand.restLastCmd();
				return -1;
			}

			// index handling
			lastCommand.index += bytesToCopy;
		}
		
		if ( debug ) {
			clog << "last: "<< last << endl;
		}

		// close/finalize the last command
		if ( last == true )
			lastCommand.restLastCmd();
		
		return bytesToCopy;
	}
	
	return 0;
}
///////////////////////////////////////////////////////////////////
int SerialEmulatorNULL::performMSG(unsigned char *buffer, unsigned int nbByte, const char* response) {
///////////////////////////////////////////////////////////////////
	wxASSERT( lastCommand.index == 0 );
		
	if ( response == NULL )
		return -1;
		
	if ( buffer == NULL )
		return -1;
		
	lastCommand.Serial.write(RET_MSG);
	lastCommand.Serial.write(response);
	lastCommand.Serial.write(MBYTE_CLOSE);
	
	// support the first byte
	return performSerialBytes(buffer, nbByte);
}
///////////////////////////////////////////////////////////////////
int SerialEmulatorNULL::performSOT(unsigned char *buffer, unsigned int nbByte, const char* response) {
///////////////////////////////////////////////////////////////////
	wxASSERT( lastCommand.index == 0 );
		
	if ( response == NULL )
		return -1;
		
	if ( buffer == NULL )
		return -1;
		
	lastCommand.Serial.write(RET_SOT);
	lastCommand.Serial.write(response);
	lastCommand.Serial.write(MBYTE_CLOSE);
	
	// support the first byte
	return performSerialBytes(buffer, nbByte);
}

///////////////////////////////////////////////////////////////////
int SerialEmulatorNULL::performConfiguration(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( buffer == NULL )
		return -1;
		
	lastCommand.Serial.write(RET_SOT);
	lastCommand.Serial.write(wxString::Format("%d:%s\n", PID_COMMON, "Here only collected setter values, because there's no controller connection"));

	SetterMap::iterator it;
	for ( it=setterMap.begin(); it!=setterMap.end(); ++it ) {
		if ( it->first >= PID_DOUBLE_RANG_START )	lastCommand.Serial.write(wxString::Format(" %d:%.2lf\n", it->first, (double)(it->second/DBL_FACT)));
		else										lastCommand.Serial.write(wxString::Format(" %d:%d\n",    it->first, it->second));
	}
	
	lastCommand.Serial.write(MBYTE_CLOSE);
	
	// support the first byte
	return performSerialBytes(buffer, nbByte);
}
///////////////////////////////////////////////////////////////////
int SerialEmulatorNULL::performLastErrorInfoResponseId(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	lastCommand.Serial.write(RET_SOT);
	lastCommand.Serial.write(wxString::Format("%lld", errorInfoResponseId));
	lastCommand.Serial.write(MBYTE_CLOSE);
	
	// support the first byte
	return performSerialBytes(buffer, nbByte);
}
///////////////////////////////////////////////////////////////////
int SerialEmulatorNULL::performErrorInfo(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( buffer == NULL )
		return -1;
		
	lastCommand.Serial.write(RET_SOT);
	
	lastCommand.Serial.write(wxString::Format("%lld", errorInfoResponseId));
	lastCommand.Serial.write(TEXT_CLOSE);
	
	lastCommand.Serial.write("0");
	lastCommand.Serial.write(TEXT_SEPARATOR);
	lastCommand.Serial.write(wxString::Format("%u", E_TOTAL_COUNT));
	lastCommand.Serial.write(TEXT_SEPARATOR);
	lastCommand.Serial.write(wxString::Format("%u", (unsigned int)errorList.size()));
	lastCommand.Serial.write(TEXT_CLOSE);
	
	unsigned int cnt = 1;
	for ( auto it = errorList.begin(); it != errorList.end(); ++it ) {
		
		lastCommand.Serial.write(wxString::Format("%u", cnt++));
		lastCommand.Serial.write(TEXT_SEPARATOR);
		lastCommand.Serial.write(wxString::Format("%u", it->id));
		lastCommand.Serial.write(TEXT_SEPARATOR);
		lastCommand.Serial.write(it->additionalInfo);
		lastCommand.Serial.write(TEXT_CLOSE);
	}
	
	lastCommand.Serial.write(MBYTE_CLOSE);
	
	// support the first byte
	return performSerialBytes(buffer, nbByte);
}
///////////////////////////////////////////////////////////////////
int SerialEmulatorNULL::performMajorMove(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	wxASSERT( lastCommand.index == 0 );
	
	if ( buffer == NULL )
		return -1;
	
	// first write the major position
	lastCommand.Serial.write(RET_SOH);
	lastCommand.Serial.write(PID_XYZ_POS_MAJOR);
	lastCommand.Serial.write(targetMajorPos.getX(), targetMajorPos.getY(), targetMajorPos.getZ());
	lastCommand.Serial.write(RET_OK);
	
	// secondary provide the limit information
	if ( limitStates.hasLimit() ) {
		lastCommand.Serial.write(RET_SOH);
		lastCommand.Serial.write(PID_LIMIT);
		lastCommand.Serial.write(limitStates.getXLimit(), limitStates.getYLimit(), limitStates.getZLimit());
		lastCommand.Serial.write(RET_OK);
	}
	
	// support the first byte
	return performSerialBytes(buffer, nbByte);
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::writeData(void *b, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( connected == false )
		return false;
		
	spyWriteData(b, nbByte);
	
	unsigned char* buffer = ((unsigned char*)b);
	unsigned char cmd = buffer[0];
	
	switch ( cmd ) {
		
		case SIG_INTERRUPPT:
		case SIG_HALT:
		case SIG_PAUSE:
		case SIG_RESUME:			lastSignal = cmd;
									return true;
		
		case CMD_RESET_CONTROLLER:	reset();
									lastCommand.cmd = cmd;
									return true;
		
		case CMD_RESET_ERRORINFO:	resetErrorInfo();
									lastCommand.cmd = cmd;
									return true;
		
		case CMD_GETTER:			lastCommand.cmd = cmd;
									return writeGetter(buffer, nbByte);
		
		case CMD_SETTER:			lastCommand.cmd = cmd; 
									return writeSetter(buffer, nbByte);
		
		case CMD_MOVE:				std::cerr << "SerialEmulatorNULL::writeData: The use of 'CMD_MOVE' is generally obsolete. Use 'CMD_RENDER_AND_MOVE' instead." << std::endl;
		case CMD_RENDER_AND_MOVE:	lastCommand.cmd = cmd;
									return writeMoveCmd(buffer, nbByte);
		
		default:					lastCommand.cmd = cmd;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::writerGetterValues(unsigned char pid, int32_t v) {
///////////////////////////////////////////////////////////////////
    lastCommand.Serial.write(RET_SOH);
    lastCommand.Serial.write(pid);
	lastCommand.Serial.write((unsigned char)1);
    lastCommand.Serial.write(v);
    lastCommand.Serial.write(RET_OK);
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::writerGetterValues(unsigned char pid, int32_t v1, int32_t v2) {
///////////////////////////////////////////////////////////////////
    lastCommand.Serial.write(RET_SOH);
    lastCommand.Serial.write(pid);
	lastCommand.Serial.write((unsigned char)2);
    lastCommand.Serial.write(v1, v2);
    lastCommand.Serial.write(RET_OK);
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::writerGetterValues(unsigned char pid, int32_t v1, int32_t v2, int32_t v3) {
///////////////////////////////////////////////////////////////////
    lastCommand.Serial.write(RET_SOH);
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
	
	switch ( pid ) {
		
		case PID_ERROR_COUNT:			    writerGetterValues(pid, (int32_t)errorList.size()); break;
		case PID_QUERY_READY_TO_RUN:	    writerGetterValues(pid, (int32_t)1); break;
		
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
		
		default:							SetterMap::iterator it;
											it = setterMap.find((int)pid);
											if ( it != setterMap.end() ) {
												
												writerGetterValues(pid, (*it).second);
												
											} else {
												addErrorInfo(E_GETTER_ID_NOT_FOUND, wxString::Format("Getter as int value: %03d", (int)pid));
												writerGetterValues(PID_UNKNOWN, 0);
												return false;
											}
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::writeSetter(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( nbByte == LONG_BUF_SIZE + 2 ) {
		unsigned char id = buffer[1];
		unsigned char valBuf[4];
		int32_t val = 0;
	
		valBuf[3] = buffer[2];
		valBuf[2] = buffer[3];
		valBuf[1] = buffer[4];
		valBuf[0] = buffer[5];
		memcpy(&val, valBuf, LONG_BUF_SIZE);
		
		ContollerInfo ci;
		ci.infoType    = CITSetterInfo;
		ci.setterId    = (int)id;
		ci.setterValue = val;
		cncControl->SerialControllerCallback(ci);
		
		// update setter map
		setterMap[(int)id] = val;
		
		// special handling for later use
		switch ( id ) {
			case PID_POS_REPLY_THRESHOLD_X: posReplyThresholdX = val; break;
			case PID_POS_REPLY_THRESHOLD_Y: posReplyThresholdY = val; break;
			case PID_POS_REPLY_THRESHOLD_Z: posReplyThresholdZ = val; break;
			
			case PID_RESERT_POS_COUNTER:  		resetPositionCounter(); break;
			case PID_RESERT_STEP_COUNTER: 		resetStepCounter(); 	break;
			
			case PID_PITCH_X:
			case PID_PITCH_Y:
			case PID_PITCH_Z:
			case PID_STEPS_X:
			case PID_STEPS_Y:
			case PID_STEPS_Z:
			case PID_PULSE_WIDTH_OFFSET_X:
			case PID_PULSE_WIDTH_OFFSET_Y:
			case PID_PULSE_WIDTH_OFFSET_Z:	speedSimulator->setup(	defaultLoopDuration,
																	GBL_CONFIG->getPitchX(), GBL_CONFIG->getStepsX(), 2 * GBL_CONFIG->getPulsWidthOffsetX(),
																	GBL_CONFIG->getPitchY(), GBL_CONFIG->getStepsY(), 2 * GBL_CONFIG->getPulsWidthOffsetY(),
																	GBL_CONFIG->getPitchZ(), GBL_CONFIG->getStepsZ(), 2 * GBL_CONFIG->getPulsWidthOffsetZ());
											break;

			
			case PID_SPEED_MM_MIN: 			speedSimulator->setFeedSpeed((double)(val/DBL_FACT));
											break;
		}
		
		return true;
	}

	return false;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::writeMoveCmd(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	int32_t x = 0L, y = 0L, z = 0L;
	
	if ( lastCommand.cmd == CMD_RENDER_AND_MOVE || lastCommand.cmd == CMD_MOVE) {
		
		// update the current emulator position
		curEmulatorPos.set(cncControl->getCurPos());
		
		// Move cmd format
		// M....;			[len =  5]; only z axis
		// M........;		[len =  9]; x and y axis
		// M............;	[len = 13]; x, y and z axis
		
		unsigned int idx = 1;
		unsigned char buf[4];

		switch ( nbByte ) {
			case  5:	buf[3] = buffer[idx++];
						buf[2] = buffer[idx++];
						buf[1] = buffer[idx++];
						buf[0] = buffer[idx++];
						memcpy(&z, buf, LONG_BUF_SIZE);
						break;
						
			case 9:		buf[3] = buffer[idx++];
						buf[2] = buffer[idx++];
						buf[1] = buffer[idx++];
						buf[0] = buffer[idx++];
						memcpy(&x, buf, LONG_BUF_SIZE);
						
						buf[3] = buffer[idx++];
						buf[2] = buffer[idx++];
						buf[1] = buffer[idx++];
						buf[0] = buffer[idx++];
						memcpy(&y, buf, LONG_BUF_SIZE);
						break;
						
			case 13:	buf[3] = buffer[idx++];
						buf[2] = buffer[idx++];
						buf[1] = buffer[idx++];
						buf[0] = buffer[idx++];
						memcpy(&x, buf, LONG_BUF_SIZE);
						
						buf[3] = buffer[idx++];
						buf[2] = buffer[idx++];
						buf[1] = buffer[idx++];
						buf[0] = buffer[idx++];
						memcpy(&y, buf, LONG_BUF_SIZE);
						
						buf[3] = buffer[idx++];
						buf[2] = buffer[idx++];
						buf[1] = buffer[idx++];
						buf[0] = buffer[idx++];
						memcpy(&z, buf, LONG_BUF_SIZE);
						break;
			default:
						std::cerr << "SerialEmulatorNULL::handleMoveCmd error." << std::endl;
						std::cerr << " Invalid byte count: " << nbByte << std::endl;
						return false;
		}
	}
	
	// determine the target major position, this is the current pos + the given move
	targetMajorPos.set(cncControl->getCurPos());
	targetMajorPos.inc(x, y, z);
	
	// the emulator function readData and writeData runs in the same thread.
	// so, it isn't possible to repeat a move command with serval position callbacks
	// as a real mirco controller can do.
	// Instead the the move is supported with its total distance - see readMove. 
	// however, for a preview this is good enougth!
	//
	// the following linear rendering is only to support a more detailed writeMoveCmd(...)
	bool ret = renderMove(x, y, z, buffer, nbByte);
	
	// reset last signal
	lastSignal = CMD_INVALID;
	return ret;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::renderMove(int32_t dx , int32_t dy , int32_t dz, unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	// update speed simulator values
	if ( GBL_CONFIG->isProbeMode() == false ) {
		wxASSERT( speedSimulator != NULL );
		speedSimulator->setNextMove(dx, dy, dz);
	}
	
	// initialize
	int i, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
	memset(&pointA, 0, sizeof(pointA));
	memset(&pointB, 0, sizeof(pointB));
	
	x_inc = (dx < 0) ? -1 : 1;
	l = abs(dx);
	
	y_inc = (dy < 0) ? -1 : 1;
	m = abs(dy);
	
	z_inc = (dz < 0) ? -1 : 1;
	n = abs(dz);
	
	dx2 = l << 1;
	dy2 = m << 1;
	dz2 = n << 1;

	// -------------------------------------------------------------
	if ((l >= m) && (l >= n)) {
		err_1 = dy2 - l;
		err_2 = dz2 - l;
		for (i = 0; i < l; i++) {
			
			if ( provideMove(pointA[0] - pointB[0], pointA[1] - pointB[1], pointA[2] - pointB[2], buffer, nbByte) == false )
				return false;
			
			if (err_1 > 0) {
				pointA[1] += y_inc;
				err_1     -= dx2;
			}
			if (err_2 > 0) {
				pointA[2] += z_inc;
				err_2     -= dx2;
			}
			err_1     += dy2;
			err_2     += dz2;
			pointA[0] += x_inc;
		}
	
	// -------------------------------------------------------------
	} else if ((m >= l) && (m >= n)) {
		err_1 = dx2 - m;
		err_2 = dz2 - m;
		for (i = 0; i < m; i++) {
			
			if ( provideMove(pointA[0] - pointB[0], pointA[1] - pointB[1], pointA[2] - pointB[2], buffer, nbByte) == false )
				return false;
			
			if (err_1 > 0) {
				pointA[0] += x_inc;
				err_1     -= dy2;
			}
			if (err_2 > 0) {
				pointA[2] += z_inc;
				err_2     -= dy2;
			}
			err_1     += dx2;
			err_2     += dz2;
			pointA[1] += y_inc;
		}
		
	// -------------------------------------------------------------
	} else {
		err_1 = dy2 - n;
		err_2 = dx2 - n;
		for (i = 0; i < n; i++) {
			
			if ( provideMove(pointA[0] - pointB[0], pointA[1] - pointB[1], pointA[2] - pointB[2], buffer, nbByte) == false )
				return false;
			
			if (err_1 > 0) {
				pointA[1] += y_inc;
				err_1     -= dz2;
			}
			if (err_2 > 0) {
				pointA[0] += x_inc;
				err_2     -= dz2;
			}
			err_1     += dy2;
			err_2     += dx2;
			pointA[2] += z_inc;
		}
	}
	
	// -------------------------------------------------------------
	if ( provideMove(pointA[0] - pointB[0], pointA[1] - pointB[1], pointA[2] - pointB[2], buffer, nbByte, true) == false )
		return false;
	
	// perform any rest offset
	if ( GBL_CONFIG->isProbeMode() == false ) {
		wxASSERT( speedSimulator != NULL );
		speedSimulator->performCurrentOffset(true);
		speedSimulator->reset();
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::provideMove(int32_t dx , int32_t dy , int32_t dz, unsigned char *buffer, unsigned int nbByte, bool force) {
///////////////////////////////////////////////////////////////////
	// statistic counting
	incPosistionCounter();

    incStepCounterX(dx);
    incStepCounterY(dy);
    incStepCounterZ(dz);

	// position management
	curEmulatorPos.incX(dx);
	curEmulatorPos.incY(dy);
	curEmulatorPos.incZ(dz);

	// simulate speed
	if ( stepAxis('X', dx) == false ) return false;
	if ( stepAxis('Y', dy) == false ) return false;
	if ( stepAxis('Z', dz) == false ) return false;
	
	// simulate a direct controller callback.
	static CncLongPosition lastReplyPos;
	CncLongPosition diff(curEmulatorPos - lastReplyPos);
	
	if ( absolute( diff.getX() ) >= posReplyThresholdX || 
	     absolute( diff.getY() ) >= posReplyThresholdY || 
		 absolute( diff.getZ() ) >= posReplyThresholdZ ||
		 force == true ) 
	{
		if ( curEmulatorPos != targetMajorPos ) {
			// due to the fact, that the emulators runs in the 
			// same thread as the main loop it makes not sense 
			// to write here someting to the serial. This is 
			// because the first main lool readData(...) call 
			// is at the earliest if this writeMove(...) call is 
			// totally finished. So, the one and only way to 
			// communicate continous with the cnc control is to call 
			// the SerialControllrCallback directly. Otherwise the 
			// complete serial data will be fetch in one block 
			// at the end if this writeMove(...) call was finalized.

			ContollerInfo ci;
			ci.infoType = CITPosition;
			ci.command  = lastCommand.cmd;
			ci.posType 	= PID_XYZ_POS_DETAIL;
			
			ci.xCtrlPos = curEmulatorPos.getX();
			ci.yCtrlPos = curEmulatorPos.getY();
			ci.zCtrlPos = curEmulatorPos.getZ();
			
			sendSerialControllrCallback(ci);
			lastReplyPos.set(curEmulatorPos);
		}
	}
	
	// do something with this coordinates
	bool ret = writeMoveCmd(dx, dy, dz, buffer, nbByte);
	
	// copy point A into point B
	memcpy(&pointB, &pointA, sizeof(pointA));
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::stepAxis(char axis, int32_t steps) {
///////////////////////////////////////////////////////////////////
	for ( int32_t i = 0; i < absolute(steps); i++ ) {
		// signal handling
		switch ( lastSignal ) {
		
			case SIG_INTERRUPPT:
			case SIG_HALT:				return false;
			
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
		
		// simulate speed
		if ( GBL_CONFIG->isProbeMode() == false ) {
			wxASSERT( speedSimulator != NULL );
			int32_t val = absolute(steps);
			
			switch ( axis ) {
				case 'X':	if ( val > 0 ) speedSimulator->simulateSteppingX(val);
							break;
							
				case 'Y':	if ( val > 0 ) speedSimulator->simulateSteppingY(val);
							break;
							
				case 'Z':	if ( val > 0 ) speedSimulator->simulateSteppingZ(val);
							break;
							
				default:	wxASSERT(axis != 'X' && axis != 'Y' && axis != 'Z');
							return false;
			}
			
			speedSimulator->performCurrentOffset(false);
		}
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::writeMoveCmd(int32_t x, int32_t y, int32_t z, unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	// inheried classes do the work here
	return true;
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
    int32_t test = MAX_LONG - absolute(dx);
    if ( test < stepCounterX ) {
        stepCounterX  = MIN_LONG;
        stepCounterX += (test - stepCounterX);
        stepOverflowCounterX++;
        return;
    }

    stepCounterX += absolute(dx);
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::incStepCounterY(int32_t dy) {
///////////////////////////////////////////////////////////////////
    // detect overflows
    int32_t test = MAX_LONG - absolute(dy);
    if ( test < stepCounterY ) {
        stepCounterY  = MIN_LONG;
        stepCounterY += (test - stepCounterY);
        stepOverflowCounterY++;
        return;
    }

    stepCounterY += absolute(dy);
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::incStepCounterZ(int32_t dz) {
///////////////////////////////////////////////////////////////////
    // detect overflows
    int32_t test = MAX_LONG - absolute(dz);
    if ( test < stepCounterZ ) {
        stepCounterZ  = MIN_LONG;
        stepCounterZ += (test - stepCounterZ);
        stepOverflowCounterZ++;
        return;
    }

    stepCounterZ += absolute(dz);
}

