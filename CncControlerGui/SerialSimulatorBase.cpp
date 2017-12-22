#include "CncArduino.h"
#include "CncTimeFunctions.h"
#include "MainFrame.h"
#include "SerialSimulatorFacade.h"
#include "SerialSimulatorBase.h"

/*
 *	Don't use std::cout|clog &Co directly in this thtead.
 *	Due to the fact that this will deadlock this thread,
 *  because the main thread sleeps in condition of
 * 	sleepMilliseconds(...) and the redirected streams cant
 * 	be well solved.
 *
 *	Use instead:
 * 		wxThreadEvent evt(wxEVT_TRACE_FROM_THREAD, MainFrame::EventId::POST_INFO|WARNING|ERROR);
 *		evt.SetString("<... message to log ...>");
 * 		wxPostEvent(GBL_CONFIG->getTheApp(), evt);
*/

///////////////////////////////////////////////////////////////////
SerialSimulatorThread::SerialSimulatorThread(SerialSimulatorFacade* facade) 
: wxThread(wxTHREAD_DETACHED)
, caller(facade)
, callerCondition(&caller->serialCondition)
, callerMutex(&caller->serialMutex)
, tsLastLog(wxDateTime::UNow())
, positionCounter(MIN_LONG)
, stepCounterX(MIN_LONG)
, stepCounterY(MIN_LONG)
, stepCounterZ(MIN_LONG)
, positionOverflowCounter(0)
, stepOverflowCounterX(0)
, stepOverflowCounterY(0)
, stepOverflowCounterZ(0)
, writeDataQueue()
, readDataQueue()
, errorList()
, setterMap()
, byteReader()
, byteWriter()
, probeMode(true)
, lastSignal(0)
///////////////////////////////////////////////////////////////////
{
	memset(bytes, 0, maxByteArraySize);
}
///////////////////////////////////////////////////////////////////
SerialSimulatorThread::~SerialSimulatorThread() {
///////////////////////////////////////////////////////////////////
	{
		wxASSERT(caller != NULL);
		wxCriticalSectionLocker enter(caller->serialThreadCS);
		
		writeDataQueue.reset();
		readDataQueue.reset();
		setterMap.clear();
		errorList.clear();
	}
}
///////////////////////////////////////////////////////////////////
wxThread::ExitCode SerialSimulatorThread::Entry() {
///////////////////////////////////////////////////////////////////
	// dispatch with maximum speed here
	// the thread will be paused if nothing is to do
	while ( !TestDestroy() ) {
		processNextCommand();
	}
	
	return NULL;
}
///////////////////////////////////////////////////////////////////
unsigned int SerialSimulatorThread::readAvailable() const {
///////////////////////////////////////////////////////////////////
	return readDataQueue.read_available();
}
///////////////////////////////////////////////////////////////////
unsigned char SerialSimulatorThread::readBufferFrontByte() const {
///////////////////////////////////////////////////////////////////
	if ( readDataQueue.read_available() > 0 ) {
		wxASSERT(caller != NULL);
		wxCriticalSectionLocker enter(caller->serialThreadCS);
		
		return readDataQueue.front().c;
	}
		
	return RET_NULL;
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::purgeReadQueue() {
///////////////////////////////////////////////////////////////////
	if ( readDataQueue.read_available() > 0 ) {
		wxASSERT(caller != NULL);
		wxCriticalSectionLocker enter(caller->serialThreadCS);
		
		readDataQueue.reset();
	}
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::performResetController() {
///////////////////////////////////////////////////////////////////
	// vertial call
	resetSerial();
	
	{
		wxASSERT(caller != NULL);
		wxCriticalSectionLocker enter(caller->serialThreadCS);
		// clear members
		writeDataQueue.reset();
		readDataQueue.reset();
		setterMap.clear();
	}
	
	performResetErrorInfo();
	
	// init some values
	positionCounter = MIN_LONG;
	stepCounterX    = MIN_LONG;
	stepCounterY    = MIN_LONG;
	stepCounterZ    = MIN_LONG;
	
	positionOverflowCounter = 0;
	stepOverflowCounterX    = 0;
	stepOverflowCounterY    = 0;
	stepOverflowCounterZ    = 0;
	
	setterMap[PID_QUERY_READY_TO_RUN] 		= 1;
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::performResetErrorInfo() {
	{
		wxASSERT(caller != NULL);
		wxCriticalSectionLocker enter(caller->serialThreadCS);
		errorList.clear();
	}
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::addErrorInfo(unsigned int id, const char* info) {
///////////////////////////////////////////////////////////////////
	ErrorInfo ei;
	ei.id = id;
	
	if ( info != NULL )
		ei.additionalInfo = info;
		
	errorList.push_back(ei);
}
///////////////////////////////////////////////////////////////////
bool SerialSimulatorThread::getSetterValueAsLong(unsigned char pid, int32_t& value, int32_t defaultValue) {
///////////////////////////////////////////////////////////////////
	value = defaultValue;
	
	auto it=setterMap.find(pid);
	if ( it == setterMap.end() )
		return false;
		
	value = it->second;
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialSimulatorThread::getSetterValueAsDouble(unsigned char pid, double& value, double defaultValue) {
///////////////////////////////////////////////////////////////////	
	value = defaultValue;
	
	int32_t val = 0;
	if ( getSetterValueAsLong(pid, val, 0) == false )
		return false;
	
	if ( pid >= PID_DOUBLE_RANG_START )	value = (double)(val/DBL_FACT);
	else								value = (double)(val);
	
	return true;
}
///////////////////////////////////////////////////////////////////
int32_t SerialSimulatorThread::getSetterValueAsLong(unsigned char pid, int32_t defaultValue) {
///////////////////////////////////////////////////////////////////
	int32_t ret = defaultValue;
	auto it=setterMap.find(pid);
	if ( it == setterMap.end() )
		return ret;
		
	return  it->second;
}
///////////////////////////////////////////////////////////////////
double SerialSimulatorThread::getSetterValueAsDouble(unsigned char pid, double defaultValue) {
///////////////////////////////////////////////////////////////////
	double ret = defaultValue;
	int32_t val = 0;
	if ( getSetterValueAsLong(pid, val, 0) == false )
		return ret;
	
	if ( pid >= PID_DOUBLE_RANG_START )	ret = (double)(val/DBL_FACT);
	else								ret = (double)(val);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::incPosIstionCounter() {
///////////////////////////////////////////////////////////////////
	// detect overflows
	if ( positionCounter == MAX_LONG ) { 
		positionCounter = MIN_LONG;
		positionOverflowCounter++;
	}
	
	positionCounter++;
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::incStepCounterX(int32_t dx) {
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
void SerialSimulatorThread::incStepCounterY(int32_t dy) {
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
void SerialSimulatorThread::incStepCounterZ(int32_t dz) {
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
unsigned int SerialSimulatorThread::Serial_available() {
///////////////////////////////////////////////////////////////////
	return byteReader.available();
}
///////////////////////////////////////////////////////////////////
unsigned char SerialSimulatorThread::Serial_read() {
///////////////////////////////////////////////////////////////////
	return byteReader.read();
}
///////////////////////////////////////////////////////////////////
unsigned int SerialSimulatorThread::Serial_readBytes(unsigned char* ret, unsigned int size) {
///////////////////////////////////////////////////////////////////
	return byteReader.readBytes(ret, size);
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::pushAndReleaseBytes() {
///////////////////////////////////////////////////////////////////
	unsigned int pushedSize = 0;
	unsigned int size       = byteWriter.size();
	
	if ( ( pushedSize = readDataQueue.push(byteWriter.getBuffer(), size) ) != size ) {
		
		wxThreadEvent evt(wxEVT_TRACE_FROM_THREAD, MainFrame::EventId::POST_ERROR);
		evt.SetString(wxString::Format("SerialFacadeThread::pushAndReleaseBytes: readDataQueue.push failed: Size required: %u, Size pushed: %u", size, pushedSize));
		wxPostEvent(GBL_CONFIG->getTheApp(), evt);
		return;
	}
	
	releaseCondition();
	byteWriter.reset();
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::Serial_write(const unsigned char b) { 
///////////////////////////////////////////////////////////////////
	byteWriter.write(b);
	pushAndReleaseBytes();
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::Serial_write(const char* response) { 
///////////////////////////////////////////////////////////////////
	byteWriter.write(response);
	pushAndReleaseBytes();
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::Serial_writeLongValue(int32_t val) {
///////////////////////////////////////////////////////////////////
	byteWriter.write(val);
	pushAndReleaseBytes();
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::Serial_writeLongValue(unsigned char pid, int32_t val) {
///////////////////////////////////////////////////////////////////
	byteWriter.write(RET_SOH);
	byteWriter.write(pid);
	byteWriter.write(val);
	pushAndReleaseBytes();
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::Serial_writeLongValues(unsigned char pid, int32_t val1, int32_t val2) {
///////////////////////////////////////////////////////////////////
	byteWriter.write(RET_SOH);
	byteWriter.write(pid);
	byteWriter.write(val1);
	byteWriter.write(val2);
	pushAndReleaseBytes();
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::Serial_writeLongValues(unsigned char pid, int32_t val1, int32_t val2, int32_t val3) {
///////////////////////////////////////////////////////////////////
	byteWriter.write(RET_SOH);
	byteWriter.write(pid);
	byteWriter.write(val1);
	byteWriter.write(val2);
	byteWriter.write(val3);
	pushAndReleaseBytes();
}
///////////////////////////////////////////////////////////////////
int SerialSimulatorThread::readData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	static const unsigned int MAX_ITEMS = 32000;
	static SerialByte serialBytes[MAX_ITEMS];
	
	tsLastLog = wxDateTime::UNow();
	
	unsigned int sizeAvailable = readDataQueue.pop(serialBytes, nbByte);
	
	if ( sizeAvailable == 0 )
		return 0;
		
	memcpy(buffer, serialBytes, nbByte);
	return sizeAvailable;
}
///////////////////////////////////////////////////////////////////
bool SerialSimulatorThread::writeData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	tsLastLog = wxDateTime::UNow();
	
	if ( buffer == NULL || nbByte == 0 )
		return true;
	
	writeDataQueue.push(SerialData(buffer, nbByte));
	return true;
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::performErrorInfo() {
///////////////////////////////////////////////////////////////////
	Serial_write(RET_SOT);

	Serial_write("0");
	Serial_write(TEXT_SEPARATOR);
	Serial_write(E_TOTAL_COUNT);
	Serial_write(TEXT_SEPARATOR);
	Serial_write(wxString::Format("%u", (unsigned int)errorList.size()));
	Serial_write(TEXT_CLOSE);
	
	unsigned int cnt = 1;
	for ( auto it = errorList.begin(); it != errorList.end(); ++it ) {
		
		Serial_write(wxString::Format("%u", cnt++));
		Serial_write(TEXT_SEPARATOR);
		Serial_write(wxString::Format("%u", it->id));
		Serial_write(TEXT_SEPARATOR);
		Serial_write(it->additionalInfo);
		Serial_write(TEXT_CLOSE);
	}
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::performConfiguration() {
///////////////////////////////////////////////////////////////////
	Serial_write(RET_SOT);

	wxString config(wxString::Format("%d:%s\n", PID_COMMON, "Here are only the setter map values, because there's a simulator connection"));
	for ( auto it=setterMap.begin(); it!=setterMap.end(); ++it ) {
		if ( it->first >= PID_DOUBLE_RANG_START )	config.append(wxString::Format(" %d:%.2lf\n", it->first, (double)(it->second/DBL_FACT)));
		else										config.append(wxString::Format(" %d:%d\n",    it->first, it->second));
	}
	
	Serial_write(config);
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::performPinReport() {
///////////////////////////////////////////////////////////////////
	// see DataControlModel for more details
	Serial_write(RET_SOT);
	Serial_write(wxString::Format("%i:0:0:0\n", MAX_PINS));
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::performMessage(unsigned char type, const char* msg) {
///////////////////////////////////////////////////////////////////
	if ( msg == NULL)
		return;
		
	Serial_write(RET_MSG);
	
	switch ( type ) {
		case CMD_TEST_WARN_MESSAGE:		Serial_write('W'); break;
		case CMD_TEST_ERROR_MESSAGE:	Serial_write('E'); break;
		default:						Serial_write('I'); break;
	}
	
	Serial_write(msg);
	Serial_write(MSG_CLOSE);
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::performHeartbeat() {
///////////////////////////////////////////////////////////////////
	unsigned long val = CncTimeFunctions::getMilliTimestamp();
	long v1 = 0;
	long v2 = 0;

	if ( val > MAX_LONG ) {
		v1 = MAX_LONG;
		v2 = val - MAX_LONG;    
	} else {
		v1 = val;
		v2 = 0;
	}
	
	Serial_writeLongValues(PID_HEARTBEAT, v1, v2);
}
///////////////////////////////////////////////////////////////////
unsigned char SerialSimulatorThread::performGetterValueIntern() {
///////////////////////////////////////////////////////////////////
	if ( Serial_available() == 0 ) {
		Serial_writeLongValue(PID_UNKNOWN, 0);
		addErrorInfo(E_INVALID_GETTER_ID);
		return RET_ERROR;
	}
	
	unsigned char pid = Serial_read();
	return performGetterValue(pid);
}
///////////////////////////////////////////////////////////////////
unsigned char SerialSimulatorThread::performGetterValueDefault(unsigned char pid) {
///////////////////////////////////////////////////////////////////
	switch ( pid ) {
		case PID_ERROR_COUNT:				Serial_writeLongValue(PID_ERROR_COUNT, (int32_t)(errorList.size())); break;
		
		default:							SetterMap::iterator it;
											it = setterMap.find((int)pid);
											if ( it != setterMap.end() ) {
												Serial_writeLongValue(pid, (*it).second); break;
												
											} else {
												
												Serial_writeLongValue(PID_UNKNOWN, 0);
												addErrorInfo(E_INVALID_GETTER_ID, wxString::Format("%u", pid));
												return RET_ERROR;
											}
	}
	
	return RET_OK;
}
///////////////////////////////////////////////////////////////////
unsigned char SerialSimulatorThread::performGetterValue(unsigned char pid) {
///////////////////////////////////////////////////////////////////
	return performGetterValueDefault(pid);
}
///////////////////////////////////////////////////////////////////
unsigned char SerialSimulatorThread::performSetterValueIntern() {
//////////////////////////////////////////////////////////////////
	if ( Serial_available() == 0) {
		addErrorInfo(E_INVALID_PARAM_ID, "");
		return RET_ERROR;
	}
	
	unsigned char pid = Serial_read();
	
	unsigned char b[4];
	int32_t lValue = 0;

	unsigned int size  = 0;
	if ( Serial_available() > 0) {
		size = Serial_readBytes(b, 4);
	
		if ( size != 4 ) {
			wxString et(wxString::Format("Invalid setter byte array size: %d. Required size%4. PID: ", size, pid));
			addErrorInfo(E_INVALID_PARAM_STREAM, et);
			return RET_ERROR;
		}
		
		lValue  = (int32_t)b[0] << 24;
		lValue += (int32_t)b[1] << 16;
		lValue += (int32_t)b[2] << 8;
		lValue += (int32_t)b[3];
	
	} else {
		wxString et(wxString::Format("Invalid setter byte array size: %d. Required size%4. PID: ", size, pid));
		addErrorInfo(E_INVALID_PARAM_STREAM, et);
		return RET_ERROR;
	}
	
	setterMap[pid] = lValue;
	performSetterValue(pid, lValue);
	
	return RET_OK;
}
///////////////////////////////////////////////////////////////////
unsigned char SerialSimulatorThread::performSetterValue(unsigned char pid, int32_t value) {
///////////////////////////////////////////////////////////////////
	// does nothing more . . .
	return RET_OK;
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::releaseCondition() {
///////////////////////////////////////////////////////////////////
	// free the wxCondition - see sleepMilliseconds(...)
	// bytes to read now available
	wxMutexLocker lock(*callerMutex);
	// same as Signal() here -- one waiter only
	callerCondition->Broadcast();
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::processNextCommand() {
///////////////////////////////////////////////////////////////////
	//std::cerr << "processNextCommand " << std::endl;
	if ( writeDataQueue.read_available() == 0 )
		return;
	
	if ( writeDataQueue.pop(serialData) == false )
		return;
	
	byteReader.init(serialData);
	if ( Serial_available() == 0 )
		return;
		
	unsigned char cmd = Serial_read();
	unsigned char ret = RET_OK;
	
	switch ( cmd ) {
		
		// --------------------------------------------------------------------------
		// Signals
		// --------------------------------------------------------------------------
		
		// Interrupt
		case SIG_INTERRUPPT:
		case SIG_HALT:
		case SIG_PAUSE:
		case SIG_RESUME:				lastSignal = cmd;
										return;
		
		// --------------------------------------------------------------------------
		// Commands - which requires a return value
		// - return --> r = RET_OK | RET_ERROR
		// --------------------------------------------------------------------------
		case CMD_RESET_CONTROLLER:		performResetController();
										ret = RET_OK;
										break;
										
		case CMD_RESET_ERRORINFO:		performResetErrorInfo();
										ret = RET_OK;
										break;
										
		case CMD_IDLE:					performLimitStates();
										ret = RET_OK;
										break;
										
		case CMD_MOVE:					// obsolete command
		case CMD_RENDER_AND_MOVE:		//controller.setPosReplyState(true);
										ret = performMove();
										//controller.sendCurrentPositions(PID_XYZ_POS_MAJOR, true);
										//controller.setPosReplyState(false);
										break;
										
		case CMD_SETTER:				ret = performSetterValueIntern();
										break;
										
		case CMD_TEST_INFO_MESSAGE:		performMessage(cmd, "This ia a test message from type: 'INFO'");
										ret = RET_OK;
										break;
										
		case CMD_TEST_WARN_MESSAGE:		performMessage(cmd, "This ia a test message from type: 'WARNING'");
										ret = RET_OK;
										break;
										
		case CMD_TEST_ERROR_MESSAGE:	performMessage(cmd, "This ia a test message from type: 'ERROR'");
										ret = RET_OK;
										break;
										
		case CMD_ENABLE_PROBE_MODE:		probeMode = true;
										ret = RET_OK;
										break;
										
		case CMD_DISABLE_PROBE_MODE:	probeMode = false;
										ret = RET_OK;
										break;

		// --------------------------------------------------------------------------
		// Commands - multi byte return
		// - must always return directly
		// --------------------------------------------------------------------------
		case CMD_PRINT_VERSION: 		Serial_write(RET_SOT);
										Serial_write(FIRMWARE_VERSION);
										return;
		
		case CMD_GETTER:				performGetterValueIntern();
										return;
										
		case CMD_PRINT_CONFIG:			performConfiguration();
										return;
		
		case CMD_PRINT_PIN_REPORT:		performPinReport();
										return;
		
		case CMD_PRINT_ERRORINFO:		performErrorInfo();
										return;
										
		// --------------------------------------------------------------------------
		// Error handling
		// --------------------------------------------------------------------------
		default:						// with respect to the fact that not all commands
										// are managed in this case return default RET_OK
										ret = RET_OK;
	}
	
	Serial_write(ret);
}
