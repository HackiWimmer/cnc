#include <iostream>
#include <fstream>

#include <chrono>
#include <sys/time.h>
#include "CncCommon.h"
#include "CncControl.h"
#include "MainFrame.h"
#include "SerialEmulatorNull.h"

static const short POINT_LENGTH = 3;
int pointA[POINT_LENGTH], pointB[POINT_LENGTH];

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
///////////////////////////////////////////////////////////////////
{
	speedSimulator = new CncSpeedSimulator(	SPEED_MANAGER_CONST_STATIC_OFFSET_US, SPEED_MANAGER_CONST_LOOP_OFFSET_US,
											GBL_CONFIG->getPitchX(), GBL_CONFIG->getStepsX(), GBL_CONFIG->getLowPulsWidthX() + GBL_CONFIG->getHighPulsWidthX(),
											GBL_CONFIG->getPitchY(), GBL_CONFIG->getStepsY(), GBL_CONFIG->getLowPulsWidthY() + GBL_CONFIG->getHighPulsWidthY(),
											GBL_CONFIG->getPitchZ(), GBL_CONFIG->getStepsZ(), GBL_CONFIG->getLowPulsWidthZ() + GBL_CONFIG->getHighPulsWidthZ()
										  );
	
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
///////////////////////////////////////////////////////////////////
{
	speedSimulator = new CncSpeedSimulator(	SPEED_MANAGER_CONST_STATIC_OFFSET_US, SPEED_MANAGER_CONST_LOOP_OFFSET_US,
											GBL_CONFIG->getPitchX(), GBL_CONFIG->getStepsX(), GBL_CONFIG->getLowPulsWidthX() + GBL_CONFIG->getHighPulsWidthX(),
											GBL_CONFIG->getPitchY(), GBL_CONFIG->getStepsY(), GBL_CONFIG->getLowPulsWidthY() + GBL_CONFIG->getHighPulsWidthY(),
											GBL_CONFIG->getPitchZ(), GBL_CONFIG->getStepsZ(), GBL_CONFIG->getLowPulsWidthZ() + GBL_CONFIG->getHighPulsWidthZ()
										   );
	reset();
}
///////////////////////////////////////////////////////////////////
SerialEmulatorNULL::~SerialEmulatorNULL() {
///////////////////////////////////////////////////////////////////
	if ( speedSimulator != NULL )
		delete speedSimulator;
		
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
		limitStates.setXLimit(LimitSwitch::LIMIT_MIN);
		
	if ( (targetMajorPos.getX())/cncConfig->getCalculationFactX() >= +cncConfig->getMaxDimensionX() )
		limitStates.setXLimit(LimitSwitch::LIMIT_MAX);
	
	if ( (targetMajorPos.getY())/cncConfig->getCalculationFactY() <= -cncConfig->getMaxDimensionY() )
		limitStates.setYLimit(LimitSwitch::LIMIT_MIN);
		
	if ( (targetMajorPos.getY())/cncConfig->getCalculationFactY() >= +cncConfig->getMaxDimensionY() )
		limitStates.setYLimit(LimitSwitch::LIMIT_MAX);
		
	if ( (targetMajorPos.getZ())/cncConfig->getCalculationFactZ() <= -cncConfig->getMaxDimensionZ() )
		limitStates.setZLimit(LimitSwitch::LIMIT_MIN);
		
	if ( (targetMajorPos.getY())/cncConfig->getCalculationFactZ() >= +cncConfig->getMaxDimensionZ() )
		limitStates.setZLimit(LimitSwitch::LIMIT_MAX);
		
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
			case CMD_MOVE_UNIT_SIGNAL:				ret = performMajorMove((unsigned char*)(buffer), nbByte);
													break;
			
			case CMD_PRINT_VERSION: 				ret = performText((unsigned char*)(buffer), nbByte, firmWare);
													break;
			
			case CMD_PRINT_CONFIG: 					ret = performConfiguration((unsigned char*)(buffer), nbByte);
													break;
			
			case CMD_PRINT_PIN_REPORT: 				ret = performText((unsigned char*)(buffer), nbByte, wxString::Format("%i:0:0:0\n", MAX_PINS)); // see DataControlModel::addPinReportRow(...) for more details
													break;
			
			case CMD_TEST_INFO_MESSAGE:				ret = performMsg((unsigned char*)(buffer), nbByte, wxString::Format("%c%s", 'I', "This is a test message from type: INFO"));
													break;
			
			case CMD_TEST_WARN_MESSAGE:				ret = performMsg((unsigned char*)(buffer), nbByte, wxString::Format("%c%s", 'W', "This is a test message from type: WARNING"));
													break;
			
			case CMD_TEST_ERROR_MESSAGE:			ret = performMsg((unsigned char*)(buffer), nbByte, wxString::Format("%c%s", 'E', "This is a test message from type: ERROR"));
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
				<< ", Bytes to copy: "  << bytesToCopy
				<< ", Total length: "   << lastCommand.Serial.length()
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
int SerialEmulatorNULL::performMsg(unsigned char *buffer, unsigned int nbByte, const char* response) {
///////////////////////////////////////////////////////////////////
	wxASSERT( lastCommand.index == 0 );
		
	if ( response == NULL )
		return -1;
		
	if ( buffer == NULL )
		return -1;
		
	lastCommand.Serial.write(RET_SOH);
	lastCommand.Serial.write(PID_MSG);
	lastCommand.Serial.write(response);
	lastCommand.Serial.write(MBYTE_CLOSE);
	lastCommand.Serial.write(RET_OK);
	
	// support the first byte
	return performSerialBytes(buffer, nbByte);
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
		SetterValueList values = it->second;
		unsigned int counter = 0;
		for ( auto itvl = values.begin(); itvl != values.end(); itvl++ ) {
			
			if ( it->first >= PID_DOUBLE_RANG_START )	valueList.append(wxString::Format("%.2lf", (double)(*itvl/DBL_FACT)));
			else										valueList.append(wxString::Format("%d",    *itvl));
			
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
int SerialEmulatorNULL::performMajorMove(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	wxASSERT( lastCommand.index == 0 );
	wxASSERT( speedSimulator != NULL );
	
	if ( buffer == NULL )
		return -1;
	
	// first write the major position
	lastCommand.Serial.write(RET_SOH);
	lastCommand.Serial.write(PID_XYZ_POS_MAJOR);
	lastCommand.Serial.write(targetMajorPos.getX(),
			                 targetMajorPos.getY(),
							 targetMajorPos.getZ(),
							 (int32_t)(speedSimulator->getRealtimeFeedSpeed_MM_MIN() * DBL_FACT));
	lastCommand.Serial.write(lastCommand.ret);
	
	// secondary provide the limit information
	if ( limitStates.hasLimit() ) {
		lastCommand.Serial.write(RET_SOH);
		lastCommand.Serial.write(PID_LIMIT);
		lastCommand.Serial.write(limitStates.getXLimit(), limitStates.getYLimit(), limitStates.getZLimit());
		//lastCommand.Serial.write(lastCommand.ret);
		lastCommand.Serial.write(RET_LIMIT);
	}
	
	// support the first byte
	return performSerialBytes(buffer, nbByte);
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::writeData(void *b, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false )
		return false;
		
	spyWriteData(b, nbByte);
	
	unsigned char* buffer = ((unsigned char*)b);
	unsigned char cmd = buffer[0];
	
	switch ( cmd ) {
		
		case SIG_INTERRUPPT:
		case SIG_HALT:
		case SIG_PAUSE:
		case SIG_RESUME:
		case SIG_QUIT_MOVE: 		lastSignal = cmd;
									return true;
									
		case CMD_IDLE:				lastCommand.cmd = cmd;
									return writeHeartbeat(buffer, nbByte);
		
		case CMD_RESET_CONTROLLER:	reset();
									lastCommand.cmd = cmd;
									return true;
		
		case CMD_GETTER:			lastCommand.cmd = cmd;
									return writeGetter(buffer, nbByte);
		
		case CMD_SETTER:			lastCommand.cmd = cmd; 
									return writeSetter(buffer, nbByte);
		
		case CMD_MOVE:
		case CMD_RENDER_AND_MOVE:
		case CMD_MOVE_UNIT_SIGNAL:	lastCommand.cmd = cmd;
									return writeMoveCmd(buffer, nbByte);
		
		default:					lastCommand.cmd = cmd;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::writeHeartbeat(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	unsigned char byteCount = sizeof(int32_t);

  //writeLongValue(millis() % MAX_LONG);
  
	lastCommand.Serial.write(RET_SOH);
	lastCommand.Serial.write(PID_HEARTBEAT);
	lastCommand.Serial.write(byteCount);
	lastCommand.Serial.write(42);
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
		
		default:							auto it = setterMap.find((int)pid);
											if ( it != setterMap.end() ) {
												
												SetterValueList values = (*it).second;
												lastCommand.Serial.write(pid);
												lastCommand.Serial.write((unsigned char)values.size());
												
												for ( auto itvl = values.begin(); itvl != values.end(); itvl++ )
													lastCommand.Serial.write(*itvl);
												
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
	const unsigned int offset = 3; // CMD + PID + COUNT
	unsigned int valueSize  = nbByte - offset;
	
	if ( valueSize % LONG_BUF_SIZE == 0 ) {
		unsigned int valueCount = valueSize / LONG_BUF_SIZE;
		unsigned int index      = offset;
		unsigned char pid       = buffer[1];
		unsigned char valBuf[LONG_BUF_SIZE];
		
		// update setter map
		SetterValueList values;
		for ( unsigned int i=0; i<valueCount; i++ ) {
			
			valBuf[3] = buffer[index + 0];
			valBuf[2] = buffer[index + 1];
			valBuf[1] = buffer[index + 2];
			valBuf[0] = buffer[index + 3];
			
			int32_t value = 0;
			memcpy(&value, valBuf, LONG_BUF_SIZE);
			values.push_back(value);
		}
		
		setterMap[pid] = values;

		// special handling for later use
		switch ( pid ) {
			case PID_POS_REPLY_THRESHOLD_X: posReplyThresholdX = values.front(); break;
			case PID_POS_REPLY_THRESHOLD_Y: posReplyThresholdY = values.front(); break;
			case PID_POS_REPLY_THRESHOLD_Z: posReplyThresholdZ = values.front(); break;
			
			case PID_RESERT_POS_COUNTER:  		resetPositionCounter(); break;
			case PID_RESERT_STEP_COUNTER: 		resetStepCounter(); 	break;
			
			case PID_PITCH_X:
			case PID_PITCH_Y:
			case PID_PITCH_Z:
			case PID_STEPS_X:
			case PID_STEPS_Y:
			case PID_STEPS_Z:
			case PID_PULSE_WIDTH_LOW_X:
			case PID_PULSE_WIDTH_LOW_Y:
			case PID_PULSE_WIDTH_LOW_Z:
			case PID_PULSE_WIDTH_HIGH_X:
			case PID_PULSE_WIDTH_HIGH_Y:
			case PID_PULSE_WIDTH_HIGH_Z:	speedSimulator->X.setup(GBL_CONFIG->getStepsX(), GBL_CONFIG->getPitchX(), SPEED_MANAGER_CONST_STATIC_OFFSET_US, SPEED_MANAGER_CONST_LOOP_OFFSET_US, GBL_CONFIG->getLowPulsWidthX() + GBL_CONFIG->getHighPulsWidthX());
											speedSimulator->Y.setup(GBL_CONFIG->getStepsY(), GBL_CONFIG->getPitchY(), SPEED_MANAGER_CONST_STATIC_OFFSET_US, SPEED_MANAGER_CONST_LOOP_OFFSET_US, GBL_CONFIG->getLowPulsWidthY() + GBL_CONFIG->getHighPulsWidthY());
											speedSimulator->Z.setup(GBL_CONFIG->getStepsZ(), GBL_CONFIG->getPitchZ(), SPEED_MANAGER_CONST_STATIC_OFFSET_US, SPEED_MANAGER_CONST_LOOP_OFFSET_US, GBL_CONFIG->getLowPulsWidthZ() + GBL_CONFIG->getHighPulsWidthZ());
											break;

			
			case PID_SPEED_MM_MIN: 			speedSimulator->setFeedSpeed_MM_MIN((double)(values.front()/DBL_FACT));
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
	
	if (    lastCommand.cmd == CMD_RENDER_AND_MOVE 
	     || lastCommand.cmd == CMD_MOVE
		 || lastCommand.cmd == CMD_MOVE_UNIT_SIGNAL
		) {
		
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
	
	bool ret = false;
	if ( lastCommand.cmd == CMD_MOVE_UNIT_SIGNAL ) {
		ret = moveUntilSignal(x, y, z, buffer, nbByte);
	
	} else {
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
		ret = renderMove(x, y, z, buffer, nbByte);
	}
	
	// reset last signal
	lastSignal = CMD_INVALID;
	return ret;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::moveUntilSignal(int32_t dx , int32_t dy , int32_t dz, unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	// determine the target major position, this is the current pos + ...
	targetMajorPos.set(cncControl->getCurPos());
	
	// Always disable probe mode here, otherwise very long move distances appear
	// this is already done by the application
	
	// speed setup
	const double START_SPEED = speedSimulator->getStartSpeed_MM_MIN() * 0.5;
	const double MAX_SPEED   = GBL_CONFIG->getMaxSpeedXYZ_MM_MIN();
	const double DIFF_SPEED  = MAX_SPEED - START_SPEED;
	
	if ( DIFF_SPEED < 0.0 )
		return false;
	
	speedSimulator->setFeedSpeed_MM_MIN(START_SPEED);
	
	bool ret = false;
	CncMilliTimestamp tsStart = CncTimeFunctions::getMilliTimestamp();
	while ( (ret = renderMove(dx, dy, dz, buffer, nbByte) ) == true ) {
		
		// important because in case of SIG_QUIT_MOVE renderMove returns true
		if ( lastCommand.ret != RET_OK )
			break;
			
		unsigned int diff = CncTimeFunctions::getMilliTimestamp() - tsStart;
		if ( diff > moveUntilAccelPeriod ) {
			speedSimulator->setFeedSpeed_MM_MIN(MAX_SPEED);
			
		} else {
			speedSimulator->setFeedSpeed_MM_MIN(START_SPEED + DIFF_SPEED / moveUntilAccelPeriod * diff);
			
		}
	}
	
	// adjust last callback position
	targetMajorPos = curEmulatorPos;
	
	// reactivate configured probe mode state
	// this is already done by the application
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::renderMove(int32_t dx , int32_t dy , int32_t dz, unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	// update speed simulator values
	if ( GBL_CONFIG->isProbeMode() == false ) {
		wxASSERT( speedSimulator != NULL );
		speedSimulator->initMove(dx, dy, dz);
	}
	
	// presetup the move return value, 
	// it will be overriden by stepAxis on demand
	lastCommand.ret = RET_OK;
	
	// initialize
	int i, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
	memset(pointA, 0, sizeof(pointA));
	memset(pointB, 0, sizeof(pointB));
	
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
			
			if (err_1 > 0) { pointA[1] += y_inc; err_1 -= dx2; }
			if (err_2 > 0) { pointA[2] += z_inc; err_2 -= dx2; }
			
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
			
			if (err_1 > 0) { pointA[0] += x_inc; err_1 -= dy2; }
			if (err_2 > 0) { pointA[2] += z_inc; err_2 -= dy2; }
			
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
			
			if (err_1 > 0) { pointA[1] += y_inc; err_1 -= dz2; }
			if (err_2 > 0) { pointA[0] += x_inc; err_2 -= dz2; }
			
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
		speedSimulator->completeMove();
	} 
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::translateStepAxisRetValue(unsigned char ret) {
///////////////////////////////////////////////////////////////////
	lastCommand.ret = ret;
	
	switch ( ret ) {
		case RET_INTERRUPT:		return false;
		case RET_ERROR:			return false;
		
		case RET_HALT:			return true;
		case RET_QUIT:			return true;
		default:				return true;
	}
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
	unsigned char retVal;
	if ( (retVal = stepAxis('X', dx)) != RET_OK ) return translateStepAxisRetValue(retVal);
	if ( (retVal = stepAxis('Y', dy)) != RET_OK ) return translateStepAxisRetValue(retVal);
	if ( (retVal = stepAxis('Z', dz)) != RET_OK ) return translateStepAxisRetValue(retVal);
	
	// simulate a direct controller callback.
	static CncLongPosition lastReplyPos;
	CncLongPosition diff(curEmulatorPos - lastReplyPos);
	
	//std::cout << force << endl;
	//std::cout << curEmulatorPos.getX() << ", " << lastReplyPos.getX() << "; " << posReplyThresholdX << ", " << diff.getX() << std::endl;
	//std::cout << curEmulatorPos.getY() << ", " << lastReplyPos.getY() << "; " << posReplyThresholdY << ", " << diff.getY() << std::endl;
	//std::cout << curEmulatorPos.getZ() << ", " << lastReplyPos.getZ() << "; " << posReplyThresholdZ << ", " << diff.getZ() << std::endl;
	
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
			
			wxASSERT( speedSimulator != NULL );
			
			ContollerInfo ci;
			ci.infoType  = CITPosition;
			ci.command   = lastCommand.cmd;
			ci.posType   = PID_XYZ_POS_DETAIL;
			
			ci.xCtrlPos  = curEmulatorPos.getX();
			ci.yCtrlPos  = curEmulatorPos.getY();
			ci.zCtrlPos  = curEmulatorPos.getZ();
			
			ci.feedSpeed = speedSimulator->getRealtimeFeedSpeed_MM_MIN();
			
			sendSerialControllerCallback(ci);
			lastReplyPos.set(curEmulatorPos);
		}
	}
	
	// do something with this coordinates
	bool ret = writeMoveCmd(dx, dy, dz, buffer, nbByte);
	
	// copy point A into point B
	memcpy(pointB, pointA, sizeof(pointA));
	
	return ret;
}
///////////////////////////////////////////////////////////////////
unsigned char SerialEmulatorNULL::stepAxis(char axis, int32_t steps) {
///////////////////////////////////////////////////////////////////
	for ( int32_t i = 0; i < absolute(steps); i++ ) {
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
		
		// simulate speed
		if ( GBL_CONFIG->isProbeMode() == false ) {
			wxASSERT( speedSimulator != NULL );
			const int32_t val = absolute(steps);
			
			switch ( axis ) {
				case 'X':	if ( val > 0 ) speedSimulator->simulateSteppingX(val);
							break;
							
				case 'Y':	if ( val > 0 ) speedSimulator->simulateSteppingY(val);
							break;
							
				case 'Z':	if ( val > 0 ) speedSimulator->simulateSteppingZ(val);
							break;
							
				default:	wxASSERT(axis != 'X' && axis != 'Y' && axis != 'Z');
							return RET_ERROR;
			}
			
			speedSimulator->performCurrentOffset(false);
		}
	}
	
	return RET_OK;
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
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::traceSpeedInformation() {
///////////////////////////////////////////////////////////////////
	if ( speedSimulator == NULL )
		return;
		
	if ( speedSimulator->getTraceFlag() == true ) {
		wxString fn(wxString::Format("c:\\temp\\speed.%s.csv", wxDateTime::Now().Format("%Y%m%d-%H%M%S")));
		std::filebuf fb;
		fb.open (fn,std::ios::out);
		std::ostream os(&fb);
		
		speedSimulator->trace(os);
		
		fb.close();
	}
}
