#include <string> 
#include <sstream>
#include <chrono>
#include <sys/time.h>
#include <wx/thread.h>
#include "CncControl.h"
#include "CncCommon.h"
#include "CncArduino.h"
#include "SerialPort.h"
#include "MainFrame.h"

char STATIC_CMD_CHAR[2];
///////////////////////////////////////////////////////////////////
Serial::Serial(CncControl* cnc)
: SerialOSD()
, totalDistance{0.0, 0.0, 0.0, 0.0}
, totalDistanceRef(0.0)
, currentFeedSpeed(0.0)
, measurementRefPos(0, 0, 0)
, tsMeasurementStart(0LL)
, tsMeasurementRef(0LL)
, tsMeasurementLast(0LL)
, cncControl(cnc)
, measurementActive(false)
, writeOnlyMoveCommands(false)
, isCommandRunning(false)
, portName()
, lastFetchResult(RET_NULL)
, traceSpyInfo(true)
, spyMode(Serial::SypMode::SM_NONE)
, spyRead(false)
, spyWrite(false)
, factorX(GBL_CONFIG->getDisplayFactX())
, factorY(GBL_CONFIG->getDisplayFactY())
, factorZ(GBL_CONFIG->getDisplayFactZ())
{
///////////////////////////////////////////////////////////////////
	resetTotalDistance();
	STATIC_CMD_CHAR[1] = '\0';
}
///////////////////////////////////////////////////////////////////
Serial::Serial(const char *portName)
: SerialOSD()
, totalDistance{0.0, 0.0, 0.0, 0.0}
, totalDistanceRef(0.0)
, currentFeedSpeed(0.0)
, measurementRefPos(0, 0, 0)
, tsMeasurementStart(0LL)
, tsMeasurementRef(0LL)
, tsMeasurementLast(0LL)
, measurementActive(false)
, writeOnlyMoveCommands(false)
, isCommandRunning(false)
, portName()
, lastFetchResult(RET_NULL)
, traceSpyInfo(true)
, spyMode(Serial::SypMode::SM_NONE)
, spyRead(false)
, spyWrite(false)
, factorX(GBL_CONFIG->getDisplayFactX())
, factorY(GBL_CONFIG->getDisplayFactY())
, factorZ(GBL_CONFIG->getDisplayFactZ())
{
///////////////////////////////////////////////////////////////////
	resetTotalDistance();
	STATIC_CMD_CHAR[1] = '\0';
	
	connect(portName);
}
///////////////////////////////////////////////////////////////////
Serial::~Serial() {
///////////////////////////////////////////////////////////////////
	disconnect();
}
///////////////////////////////////////////////////////////////////
void Serial::waitDuringRead(unsigned int millis) {
///////////////////////////////////////////////////////////////////
	Sleep(millis);
}
///////////////////////////////////////////////////////////////////
void Serial::sleepMilliseconds(unsigned int millis) {
///////////////////////////////////////////////////////////////////
	// Sleep a while to give the real microcontroller a portion 
	// of time to do something
	Sleep(millis);
}
///////////////////////////////////////////////////////////////////
void Serial::startMeasurement() {
///////////////////////////////////////////////////////////////////
	measurementActive = true;
	
	resetPositionCounter();
	resetStepCounter();
	resetTotalDistance();
	
	tsMeasurementStart = CncTimeFunctions::getNanoTimestamp();
	tsMeasurementRef   = tsMeasurementStart;
	tsMeasurementLast  = tsMeasurementStart;
	startMeasurementIntern();
}
///////////////////////////////////////////////////////////////////
void Serial::stopMeasurement() {
///////////////////////////////////////////////////////////////////
	measurementActive = false;
	
	stopMeasurementIntern();
	logMeasurementLastTs();
	
	// reset calculated feed speed
	currentFeedSpeed = 0.0;
}
///////////////////////////////////////////////////////////////////
void Serial::logMeasurementLastTs() {
///////////////////////////////////////////////////////////////////
	tsMeasurementLast = CncTimeFunctions::getNanoTimestamp();
	
	// calculate current feed speed
	if ( GBL_CONFIG->isProbeMode() == false ) {
		const short T = 3;
		CncNanoTimestamp tDiff = getMeasurementNanoTimeSpanLastRef();
		double pDiff           = totalDistance[T] - totalDistanceRef;
		
		// to avoid miss caluclations on the basis of to short difference
		// this is with respect, that windows can't sleep exactly
		if ( tDiff > 0 && pDiff > 0.1 ) {
			currentFeedSpeed  = pDiff;
			currentFeedSpeed /= tDiff;
			currentFeedSpeed *= std::nano::den;
			currentFeedSpeed *= 60;
		}
	}
}
///////////////////////////////////////////////////////////////////
void Serial::adjustMeasurementRefTs() {
///////////////////////////////////////////////////////////////////
	logMeasurementRefTs(measurementRefPos);
}
///////////////////////////////////////////////////////////////////
void Serial::logMeasurementRefTs(const CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	tsMeasurementRef = CncTimeFunctions::getNanoTimestamp();
	
	if ( GBL_CONFIG->isProbeMode() == false ) {
		const short T = 3;
		totalDistanceRef = totalDistance[T];
		measurementRefPos.set(pos);
	}
}
///////////////////////////////////////////////////////////////////
CncNanoTimespan Serial::getMeasurementNanoTimeSpanTotal() const {
///////////////////////////////////////////////////////////////////
	return tsMeasurementLast - tsMeasurementStart;
}
///////////////////////////////////////////////////////////////////
CncNanoTimespan Serial::getMeasurementNanoTimeSpanLastRef() const {
///////////////////////////////////////////////////////////////////
	return tsMeasurementLast - tsMeasurementRef;
}
///////////////////////////////////////////////////////////////////
bool Serial::sendSerialControllrCallback(ContollerInfo& ci) {
///////////////////////////////////////////////////////////////////
	if ( cncControl == NULL )
		return false;
		
	// provide speed calculation info
	if ( ci.infoType == CITPosition )
		incTotalDistance(measurementRefPos, ci.xCtrlPos, ci.yCtrlPos, ci.zCtrlPos);
	
	return cncControl->SerialControllerCallback(ci);
}
///////////////////////////////////////////////////////////////////
void Serial::incTotalDistance(int32_t dx, int32_t dy, int32_t dz) {
///////////////////////////////////////////////////////////////////
	//dx, dy and dz acts as relative coordinates here
	const short X = 0, Y = 1, Z = 2, T = 3;
	double x 			 = (absolute(dx) * factorX);
	double y 			 = (absolute(dy) * factorY);
	double z 			 = (absolute(dz) * factorZ);
	double t 			 = sqrt(x*x + y*y + z*z);
	
	totalDistance[X] 	+= x;
	totalDistance[Y] 	+= y;
	totalDistance[Z] 	+= z;
	totalDistance[T] 	+= t;

	logMeasurementLastTs();
}
///////////////////////////////////////////////////////////////////
void Serial::incTotalDistance(unsigned int size, const int32_t (&values)[3]) {
///////////////////////////////////////////////////////////////////
	switch ( size ) {
		case 1:		incTotalDistance(0,         0,         values[0]);	break;
		case 2:		incTotalDistance(values[0], values[1], 0        ); 	break;
		case 3:		incTotalDistance(values[0], values[1], values[2]); 	break;
		default:	std::cerr << "Serial::incTotalDistance: Invalid size: " << size << endl;
	}
}
///////////////////////////////////////////////////////////////////
void Serial::incTotalDistance(const CncLongPosition& pos, int32_t cx, int32_t cy, int32_t cz) {
///////////////////////////////////////////////////////////////////
	// pos acts as reference here
	// cx, cy and cz are absolte coordinates
	
	// create relative deltas to make a increment possible
	incTotalDistance(cx - pos.getX(), cy - pos.getY(), cz - pos.getZ());
	
	// adjust ref pos
	measurementRefPos.setXYZ(cx, cy, cz);
}
///////////////////////////////////////////////////////////////////
double Serial::getCurrentFeedSpeed() {
///////////////////////////////////////////////////////////////////
	return currentFeedSpeed;
}
///////////////////////////////////////////////////////////////////
double Serial::getCurrentFeedSpeedAVG() {
///////////////////////////////////////////////////////////////////
	if ( getMeasurementNanoTimeSpanTotal() == 0L )
		return 0.0;
	
	if ( cnc::dblCompareNull(getTotalDistance()) == true )
		return 0.0;
		
	if ( isMeasurementActive() == false )
		return 0.0;
	
	// getTotalDistance()		==> mm
	// getMeasurementTimeSpan 	==> ns
	// ret 						==> mm/min
	
	double timeSpan = (double)(getMeasurementNanoTimeSpanTotal() / std::giga::den);
	if ( cnc::dblCompareNull(timeSpan) == true )
		return 0.0;
		
	return (getTotalDistance() / timeSpan) * 60;
}
///////////////////////////////////////////////////////////////////
void Serial::setSpyMode(Serial::SypMode sm) {
///////////////////////////////////////////////////////////////////
	spyMode = sm;
	switch ( sm ) {
		case Serial::SypMode::SM_NONE:	spyRead 	= false;
										spyWrite	= false;
										break;
										
		case Serial::SypMode::SM_READ:	spyRead 	= true;
										spyWrite	= false;
										break;
										
		case Serial::SypMode::SM_WRITE:	spyRead 	= false;
										spyWrite	= true;
										break;
										
		case Serial::SypMode::SM_ALL:	spyRead 	= true;
										spyWrite	= true;
										break;
	}
}
///////////////////////////////////////////////////////////////////
bool Serial::connect(const char* portName) {
///////////////////////////////////////////////////////////////////
	return SerialOSD::connect(portName);
}
///////////////////////////////////////////////////////////////////
void Serial::disconnect(void) {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == true ) {
		
		SerialOSD::disconnect();
		isCommandRunning = false;
	}
}
///////////////////////////////////////////////////////////////////
void Serial::purge(void) {
///////////////////////////////////////////////////////////////////
	SerialOSD::purge();
}
///////////////////////////////////////////////////////////////////
bool Serial::isMoveCommand(unsigned char cmd) {
///////////////////////////////////////////////////////////////////
	if ( cmd =='x' || 
		 cmd =='X' || 
		 cmd =='y' || 
		 cmd =='Y' || 
		 cmd =='z' || 
		 cmd =='Z' || 
		 cmd =='m' || 
		 cmd =='M'
	   ) {
			return true;   
	   }
		
	return false;
}
///////////////////////////////////////////////////////////////////
void Serial::resetPositionCounter() {
///////////////////////////////////////////////////////////////////
	processSetter(PID_RESERT_POS_COUNTER, 0);
}
///////////////////////////////////////////////////////////////////
size_t Serial::getPositionCounter() {
///////////////////////////////////////////////////////////////////
	if ( cncControl->isInterrupted() )
		return 0;
		
	if ( cncControl->isConnected() == false )
		return 0;
		
	if ( isCommandRunning )
		return 0;
	
	std::vector<int32_t> list;
	if ( processGetter(PID_GET_POS_COUNTER, list) && list.size() == 2 ) {
		// the controler delivers a signed value because the getter interface didn't alow a unsigned.
		// to get the whole unsigned int32_t range the controller starts counting with MIN_LONG
		size_t ret  = list.at(0) + abs(MIN_LONG);
		ret        += list.at(1) * (abs(MIN_LONG) + MAX_LONG);
		return ret;
	}
	
	return 0;
}
///////////////////////////////////////////////////////////////////
void Serial::resetStepCounter() {
///////////////////////////////////////////////////////////////////
	processSetter(PID_RESERT_STEP_COUNTER, 0);
}
///////////////////////////////////////////////////////////////////
size_t Serial::requestStepCounter(unsigned char pid) {
///////////////////////////////////////////////////////////////////
	if ( cncControl->isInterrupted() )
		return 0;
		
	if ( cncControl->isConnected() == false )
		return 0;
	
	if ( isCommandRunning )
		return 0;
	
	std::vector<int32_t> list;
	if ( processGetter(pid, list) && list.size() == 2 ) {
		// the controler delivers a signed value because the getter interface didn't alow a unsigned.
		// to get the whole unsigned int32_t range the controller starts counting with MIN_LONG
		size_t ret =  list.at(0) + abs(MIN_LONG);
		ret        += list.at(1) * (abs(MIN_LONG) + MAX_LONG);
		return ret;
	}
	
	return 0;
}
///////////////////////////////////////////////////////////////////
size_t Serial::getStepCounter() {
///////////////////////////////////////////////////////////////////
	size_t ret = 0;
	ret += requestStepCounter(PID_GET_STEP_COUNTER_X);
	ret += requestStepCounter(PID_GET_STEP_COUNTER_Y);
	ret += requestStepCounter(PID_GET_STEP_COUNTER_Z);
	return ret;
}
///////////////////////////////////////////////////////////////////
size_t Serial::getStepCounterX() {
///////////////////////////////////////////////////////////////////
	return requestStepCounter(PID_GET_STEP_COUNTER_X);
}
///////////////////////////////////////////////////////////////////
size_t Serial::getStepCounterY() {
///////////////////////////////////////////////////////////////////
	return requestStepCounter(PID_GET_STEP_COUNTER_Y);
}
///////////////////////////////////////////////////////////////////
size_t Serial::getStepCounterZ() {
///////////////////////////////////////////////////////////////////
	return requestStepCounter(PID_GET_STEP_COUNTER_Z);
}
///////////////////////////////////////////////////////////////////
int Serial::readData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	return SerialOSD::readData(buffer, nbByte);
}
///////////////////////////////////////////////////////////////////
bool Serial::writeData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	return SerialOSD::writeData(buffer, nbByte);
}
///////////////////////////////////////////////////////////////////
void Serial::decodeMessage(const unsigned char* message, std::ostream& mutliByteStream) {
///////////////////////////////////////////////////////////////////
	std::stringstream ss((char*)message);
	
	std::string s;
	while ( getline(ss, s, '\n') ) {
		mutliByteStream << s.substr(0, s.length()).c_str()  << std::endl;
	}
}
///////////////////////////////////////////////////////////////////
void Serial::decodeMultiByteResults(const char cmd, const unsigned char* result, std::ostream& mutliByteStream) {
///////////////////////////////////////////////////////////////////
	size_t pos;
	unsigned int lineCounter = 0;
	int id;
	int nr;
	std::stringstream ss((char*)result);
	std::string s;

	switch ( cmd ) {
		case CMD_PRINT_CONFIG: 
			if ( cncControl->hasControllerConfigControl() == true )
				cncControl->clearControllerConfigControl();
				
			while (getline(ss, s, '\n')) {
				wxString key;
				if ( (pos = s.find_first_of (':')) > 0 ) {
					id = atoi((s.substr(0,pos)).c_str());
					for ( unsigned int i=0; i<s.length(); i++) {
						if ( s[i] != ' ')
							break;
							
						key += "    ";
					}
					
					key += ArduinoPIDs::getPIDLabel((unsigned int)id);
					
					if ( cncControl->hasControllerConfigControl() == true ) {
						cncControl->appendPidKeyValueToControllerConfig(id, key, s.substr(pos+1, s.length()-1).c_str() );
					} else {
						mutliByteStream << key.c_str() /*<< "[" << id << "]"*/ << ": ";
						mutliByteStream << s.substr(pos+1, s.length()-1).c_str() << std::endl;
						mutliByteStream << "\n";
					}
				}
			}
			break;
			
		case CMD_PRINT_ERRORINFO:
			if ( cncControl->hasControllerErrorControl() == true )
				cncControl->clearControllerErrorControl();
			
			lineCounter = 0;
			while (getline(ss, s, '\n')) {
				lineCounter++;
				
				// first fetch response id
				if ( lineCounter == 1 ) {
					GBL_CONFIG->getTheApp()->GetLastErrorInfoResponseId()->ChangeValue(s.c_str());
					continue;
				}
				
				// ... now decode error info
				if ( (pos = s.find_first_of (':')) > 0 ) {
					nr = atoi((s.substr(0,pos)).c_str());
					
					s = s.substr(pos+1, s.length()-1);
					if ( (pos = s.find_first_of (':')) > 0 ) {
						id = atoi((s.substr(0,pos)).c_str());
						
						if ( cncControl->hasControllerErrorControl() == true ) {
							cncControl->appendNumKeyValueToControllerErrorInfo(nr, id, ArduinoErrorCodes::getECLabel(id), s.substr(pos+1, s.length()-1).c_str() );
						} else {
							mutliByteStream << nr << ": " << ArduinoErrorCodes::getECLabel(id) << ": ";
							mutliByteStream << s.substr(pos+1, s.length()-1);
							mutliByteStream << "\n";
						}
					}
				}
			}
			break;
			
		case CMD_PRINT_PIN_REPORT:
			if ( cncControl->hasControllerPinControl() == true )
				cncControl->clearControllerPinControl();
			
			while ( getline(ss, s, '\n') ) {
				
				int pin   = -1;
				int type  = -1;
				int mode  = -1;
				int value = -1;
				
				if ( (pos = s.find_first_of (':')) > 0 ) 
					pin = atoi((s.substr(0,pos)).c_str());
					
				s = s.substr(pos+1, s.length()-1);
				if ( (pos = s.find_first_of (':')) > 0 ) 
					type = atoi((s.substr(0,pos)).c_str());
					
				s = s.substr(pos+1, s.length()-1);
				if ( (pos = s.find_first_of (':')) > 0 ) 
					mode = atoi((s.substr(0,pos)).c_str());
					
				s = s.substr(pos+1, s.length()-1);
				if ( (pos = s.find_first_of ('\n')) > 0 ) 
					value = atoi((s.substr(0,pos)).c_str());
				
				if ( pin != -1 && type != -1 && mode != -1 && value != -1 ) {
					
					if ( cncControl->hasControllerPinControl() == true ) {
						cncControl->appendNumKeyValueToControllerPinInfo((type == (int)'D' ? ArduinoDigitalPins::getPinLabel(pin) : ArduinoAnalogPins::getPinLabel(pin)),
                                                                          pin, type, mode, value);
					} else {
						mutliByteStream << (type == (int)'D' ? ArduinoDigitalPins::getPinLabel(pin) : ArduinoAnalogPins::getPinLabel(pin)) << ": ";
						mutliByteStream	<< (char)type << ": ";
						mutliByteStream << (char)mode << ": ";
						mutliByteStream << value << "\n";
					}
				}
			}
			break;
			
		default:
			mutliByteStream << result;
	}
}
///////////////////////////////////////////////////////////////////
unsigned char Serial::fetchControllerResult(unsigned int maxDelay) {
///////////////////////////////////////////////////////////////////
	unsigned char ret[1];
	ret[0] = 0;
	
	if ( readDataUntilSizeAvailable(ret, 1, maxDelay) != 1 ) {
		std::cerr << "Serial::fetchControllerResult: Read failed." << std::endl;
		return RET_ERROR;
	}
	
	return ret[0];
}
///////////////////////////////////////////////////////////////////
int Serial::readDataUntilSizeAvailable(unsigned char *buffer, unsigned int nbByte, unsigned int maxDelay, bool withErrorMsg) {
///////////////////////////////////////////////////////////////////
	// Assumtion buffer allocates nbByte bytes
	static const unsigned int maxBytes = 1024;
	unsigned char oneReadBuf[maxBytes];

	unsigned char* p = (unsigned char*)buffer;
	
	unsigned int remainingBytes = nbByte;
	int bytesRead 		= 0;
	
	unsigned int cnt = 0;
	
	while ( remainingBytes > 0 ) {
		bytesRead = readData(oneReadBuf, std::min(remainingBytes, maxBytes));
		if ( bytesRead > 0 ) {
			
			// Make the memcpy below safe
			// Please note this condition normally not occurs
			if ( bytesRead > (int)remainingBytes ) {
				std::cerr << "Serial::readDataUntilSizeAvailable: readData(...) delivers to much bytes:" 
				<< " Requested: " << std::min(remainingBytes, maxBytes)
				<< " Received: "  << bytesRead
 				<< std::endl;
				
				bytesRead = remainingBytes;
			} 
			
			memcpy(p, &oneReadBuf, bytesRead);
			p += bytesRead;
			remainingBytes -= bytesRead;
			bytesRead = 0;
			
		} else {
			waitDuringRead(1);
			
			if ( ++cnt > maxDelay ) {
				if ( withErrorMsg == true )
					std::cerr << "Serial::readDataUntilSizeAvailable Timeout reached:" << std::endl;
					
				//return 0;
				break;
			}
				
			if ( cnt%100 == 0 )
				cncControl->SerialCallback(0);
		}
	}
	
	return nbByte - remainingBytes;
}
///////////////////////////////////////////////////////////////////
int Serial::readDataUntilMultyByteClose(unsigned char* buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	// Assumtion buffer allocates nbByte bytes
	static const unsigned int size = 128;
	unsigned char b[size];
	
	int byteCounter       = 0;
	int bytesRead         = 0;
	int readFailedCounter = 0;
	
	bool runWhile = true;
	while ( runWhile ) {
		
		// error handling
		if ( readFailedCounter > 100 ) {
			std::cerr << "Serial::readDataUntilMultyByteClose: Max count of failed reads a reached." 
			<< std::endl;
			break;
		}
		
		// error handling
		if ( (unsigned int)byteCounter >= nbByte ) {
			std::cerr << "Serial::readDataUntilMultyByteClose: To much data availiable." 
			<< " Max bytes: " << size
			<< " Byte counter: " << byteCounter
			<< std::endl;
			break;
		}
		
		if ( ( bytesRead = readData(b, size) ) > 0 ) {
			for ( int i = 0; i < bytesRead; i++ ) {
				if ( b[i] == MBYTE_CLOSE ) {
					// close the string
					buffer[byteCounter] = '\0';
					
					// clog << "MBYTE_CLOSE received "<< endl;
					// stop fetching here
					runWhile = false;
					break;
					
				} else {
					
					// append response
					buffer[byteCounter] = b[i];
					byteCounter++;
				}
			}
			
			// to release user events
			cncControl->SerialCallback(0);
			
		} else {
			
			waitDuringRead(1);
			readFailedCounter++;
		}
	}
	
	// final safty
	buffer[nbByte - 1] = '\0';
	
	return byteCounter;
}
///////////////////////////////////////////////////////////////////
bool Serial::processIdle() {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false ) {
		std::cerr << "SERIAL::processMoveXY()::ERROR: Not connected\n";
		return false;
	}
	
	if ( writeOnlyMoveCommands == true )
		return true;
	
	unsigned char cmd[8];
	unsigned char* p = cmd;
	
	int idx = 0;
	cmd[idx++] = 'i';
	p++;
	
	if ( traceSpyInfo && spyWrite ) {
		cnc::spy.initializeResult();
		cnc::spy << "Send: '" << cmd[0] << "' [" << ArduinoCMDs::getCMDLabel(cmd[0]) << "]\n";
	}
		
	if ( writeData(cmd, idx) ) {
		// only a dummy here
		CncLongPosition pos(0,0,0);
		
		SerialFetchInfo sfi;
		sfi.command 		= cmd[0];
		sfi.retSOHAllowed 	= true;
		sfi.returnAfterSOH  = false;

		bool ret = evaluateResultWrapper(sfi, std::cout, pos);
		if ( ret == false ) {
			//std::cerr << "Error while processing idle: " << std::endl;
			//std::cerr << "Request \"Error Info\" for more details." << std::endl;
		}
		return ret;
		
	} else {
		std::cerr << "Serial::processSetter: Unable to write data" << std::endl;
		cncControl->SerialCallback(0);
		return false;
	}
	
	return false;
}
///////////////////////////////////////////////////////////////////
bool Serial::processTest(int32_t testId) {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false ) {
		std::cerr << "SERIAL::processMoveXY()::ERROR: Not connected\n";
		return false;
	}
	
	if ( isCommandRunning ) {
		std::clog << "Serial::processTest: Serial is currently in fetching mode: This command will be rejected:" << std::endl;
		std::clog << " Test ID: '" << testId << std::endl;
		return true;
	}
	
	if ( writeOnlyMoveCommands == true )
		return true;
	
	unsigned char cmd[8];
	unsigned char* p = cmd;
	
	int idx = 0;
	cmd[idx++] = 'T';
	p++;
	
	testId = htonl(testId);
	memcpy(p, &testId, LONG_BUF_SIZE);
	idx += LONG_BUF_SIZE;
	
	if ( traceSpyInfo && spyWrite ) {
		cnc::spy.initializeResult();
		cnc::spy << "Send: '" << cmd[0] << "' [" << ArduinoCMDs::getCMDLabel(cmd[0]) << "]\n";
	}
		
	if ( writeData(cmd, idx) ) {
		// only a dummy here
		CncLongPosition pos(0,0,0);
		
		SerialFetchInfo sfi;
		sfi.command 		= cmd[0];
		sfi.retSOHAllowed 	= true;
		sfi.returnAfterSOH  = false;

		bool ret = evaluateResultWrapper(sfi, std::cout, pos);
		if ( ret == false ) {
			std::cerr << "Error while processing test: " << ntohl(testId) << std::endl;
			std::cerr << "Request \"Error Info\" for more details." << std::endl;
		}
		return ret;
		
	} else {
		std::cerr << "Serial::processSetter: Unable to write data" << std::endl;
		cncControl->SerialCallback(0);
		return false;
	}
	
	return false;
}
///////////////////////////////////////////////////////////////////
bool Serial::processSetter(unsigned char pid, int32_t value) {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false ) {
		std::cerr << "SERIAL::processSetter()::ERROR: Not connected\n";
		return false;
	}
	
	// this shouldn't published to the controller
	if ( pid == PID_SEPARATOR )
		return true;
		
	if ( writeOnlyMoveCommands == true )
		return true;
	
	unsigned char cmd[8];
	unsigned char* p = cmd;
	
	int idx = 0;
	cmd[idx++] = 'S';
	p++;
	
	cmd[idx++] = pid;
	p++;

	value = htonl(value);
	memcpy(p, &value, LONG_BUF_SIZE);
	idx += LONG_BUF_SIZE;
	
	int32_t v = ntohl(value);
	if ( isCommandRunning ) {
		std::clog << "Serial::processSetter: Serial is currently in fetching mode: This command will be rejected:" << std::endl;
		if ( pid < PID_DOUBLE_RANG_START )	std::clog << " Command: '" << cmd[0] << "' [" << ArduinoCMDs::getCMDLabel(cmd[0]) << "][" << ArduinoPIDs::getPIDLabel((int)pid) << "][" << v << "]\n";
		else								std::clog << " Command: '" << cmd[0] << "' [" << ArduinoCMDs::getCMDLabel(cmd[0]) << "][" << ArduinoPIDs::getPIDLabel((int)pid) << "][" << (double)(v)/DBL_FACT << "]\n";
		return true;
	}
	
	if ( traceSpyInfo && spyWrite ) {
		cnc::spy.initializeResult();
		
		if ( pid < PID_DOUBLE_RANG_START )	cnc::spy << "Send: '" << cmd[0] << "' [" << ArduinoCMDs::getCMDLabel(cmd[0]) << "][" << ArduinoPIDs::getPIDLabel((int)pid) << "][" << v << "]\n";
		else								cnc::spy << "Send '" << cmd[0] << "' [" << ArduinoCMDs::getCMDLabel(cmd[0]) << "][" << ArduinoPIDs::getPIDLabel((int)pid) << "][" << (double)(v)/DBL_FACT << "]\n";
	}
		
	if ( writeData(cmd, idx) ) {
		// only a dummy here
		CncLongPosition pos(0,0,0);
		
		SerialFetchInfo sfi;
		sfi.command = cmd[0];
		sfi.retSOTAllowed = false;
		sfi.returnAfterSOT = true;

		return evaluateResultWrapper(sfi, std::cout, pos);
		
	} else {
		std::cerr << "Serial::processSetter: Unable to write data" << std::endl;
		cncControl->SerialCallback(0);
		return false;
	}
	
	return false;
}
///////////////////////////////////////////////////////////////////
bool Serial::processGetter(unsigned char pid, std::vector<int32_t>& list) {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false ) {
		std::cerr << "SERIAL::processSetter()::ERROR: Not connected\n";
		return false;
	}
	
	if ( writeOnlyMoveCommands == true )
		return true;
	
	unsigned char cmd[2];
	cmd[0] = 'G';
	cmd[1] = pid;
	
	if ( isCommandRunning ) {
		std::clog << "Serial::processGetter: Serial is currently in fetching mode: This command will be rejected:" << std::endl;
		std::clog << " Command: '" << cmd[0] << "' [" << ArduinoCMDs::getCMDLabel(cmd[0]) << "][" << ArduinoPIDs::getPIDLabel((pid)) << "]\n";
		return true;
	}
	
	if ( traceSpyInfo && spyWrite ) {
		cnc::spy.initializeResult();
		cnc::spy << "Send: '" << cmd[0] << "' [" << ArduinoCMDs::getCMDLabel(cmd[0]) << "][" << ArduinoPIDs::getPIDLabel((pid)) << "]\n";
	}
	
	list.clear();
	
	if ( writeData((char*)cmd, sizeof(cmd)) ) {
		// only a dummy here
		CncLongPosition pos(0,0,0);
		
		SerialFetchInfo sfi;
		sfi.command = cmd[0];
		sfi.singleFetchTimeout = 100;
		sfi.retSOHAllowed = true;
		sfi.returnAfterSOH = true;
		sfi.Gc.list = &list;

		return evaluateResultWrapper(sfi, std::cout, pos);
		
	} else {
		std::cerr << "Serial::processGetter: Unable to write data" << std::endl;
		cncControl->SerialCallback(0);
		return false;
	}
	
	return false;
}
///////////////////////////////////////////////////////////////////
bool Serial::sendSignal(const unsigned char cmd) {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false ) {
		std::cout << "SERIAL::sendSignal()::ERROR: Not connected\n";
		return false;
	}
	
	STATIC_CMD_CHAR[0] = cmd;
	return writeData(STATIC_CMD_CHAR, 1);
}
///////////////////////////////////////////////////////////////////
bool Serial::processCommand(const unsigned char cmd, std::ostream& mutliByteStream, CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	STATIC_CMD_CHAR[0] = cmd;
	return processCommand(STATIC_CMD_CHAR, mutliByteStream, pos);
}
///////////////////////////////////////////////////////////////////
bool Serial::processCommand(const char* cmd, std::ostream& mutliByteStream, CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false ) {
		std::cout << "SERIAL::processCommand()::ERROR: Not connected\n";
		return false;
	}
	
	if ( isCommandRunning ) {
		std::clog << "Serial::processCommand: Serial is currently in fetching mode: This command will be rejected:" << std::endl;
		std::clog << " Command: '" << cmd[0] << "' [" << ArduinoCMDs::getCMDLabel(cmd[0]) << "]\n";
		return true;
	}

	// Always log the start position
	cncControl->SerialCallback(0);
	
	char* p = (char*)cmd;
	for ( unsigned int i=0; i<strlen(cmd); i++ ) {
		
		if ( writeOnlyMoveCommands == true && isMoveCommand((unsigned char)cmd[i]) == false ) {
			p++; // important to have always the right index!
			continue;
		}
		
		// redirect this command types
		switch ( cmd[i] ) {
			case CMD_NEG_STEP_X:
			case CMD_POS_STEP_X:
			case CMD_NEG_STEP_Y:
			case CMD_POS_STEP_Y:
			case CMD_NEG_STEP_Z:
			case CMD_POS_STEP_Z: 	if ( convertToMoveCommandAndProcess(cmd[i], mutliByteStream, pos) == false ) {
										std::cerr << "Serial::processCommand: convertToMoveCommandAndProcess failed!" << std::endl;
										return false;
									}
									
									p++; // important to have always the right index!
									continue;
		}
		
		if ( traceSpyInfo && spyWrite ) {
			cnc::spy.initializeResult();
			cnc::spy << "Send: '" << cmd[0] << "' [" << ArduinoCMDs::getCMDLabel(cmd[0]) << "]\n";
		}

		if ( writeData(p++, 1) ) {
			SerialFetchInfo sfi;
			sfi.command = cmd[i];
			sfi.retSOTAllowed = true;
			sfi.returnAfterSOT = true;
			
			bool ret = evaluateResultWrapper(sfi, mutliByteStream, pos);
			
			if ( ret == false ) {
				std::cerr << "ERROR! Rest of command skipped [" << p << "]\n";
			}
			
			return ret;
		
		} else {
			std::cerr << "Serial::processCommand: Unable to write data" << std::endl;
			cncControl->SerialCallback(0);
			return false;
		}
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool Serial::convertToMoveCommandAndProcess(unsigned char cmd, std::ostream& mutliByteStream, CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	bool ret = false;
	
	const unsigned int size = 3;
	const short X=0, Y=1, Z=2;
	int32_t values[size];
	values[X] = 0;
	values[Y] = 0;
	values[Z] = 0;
	
	switch ( cmd ) {
		case CMD_NEG_STEP_X:	values[X] = -1; ret = processMove(size, values, false, pos); break;
		case CMD_POS_STEP_X:	values[X] = +1; ret = processMove(size, values, false, pos); break;
		case CMD_NEG_STEP_Y:	values[Y] = -1; ret = processMove(size, values, false, pos); break;
		case CMD_POS_STEP_Y:	values[Y] = +1; ret = processMove(size, values, false, pos); break;
		case CMD_NEG_STEP_Z:	values[Z] = -1; ret = processMove(size, values, false, pos); break;
		case CMD_POS_STEP_Z: 	values[Z] = +1; ret = processMove(size, values, false, pos); break;
		default:				std::cerr << "Serial::convertToMoveCommandAndProcess: Invalid command: " << cmd << std::endl;
								ret = false;
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool Serial::processMoveZ(int32_t z1, bool alreadyRendered, CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	unsigned int size = 1;
	static int32_t values[3];
	values[0] = z1;
	values[1] = 0;
	values[2] = 0;

	return processMove(size, values, alreadyRendered, pos);
}
///////////////////////////////////////////////////////////////////
bool Serial::processMoveXY(int32_t x1, int32_t y1, bool alreadyRendered, CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	unsigned int size = 2;
	static int32_t values[3];
	values[0] = x1;
	values[1] = y1;
	values[2] = 0;

	return processMove(size, values, alreadyRendered, pos);
}
///////////////////////////////////////////////////////////////////
bool Serial::processMoveXYZ(int32_t x1, int32_t y1, int32_t z1, bool alreadyRendered, CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	unsigned int size = 3;
	static int32_t values[3];
	values[0] = x1;
	values[1] = y1;
	values[2] = z1;

	return processMove(size, values, alreadyRendered, pos);
}
///////////////////////////////////////////////////////////////////
bool Serial::processMove(unsigned int size, const int32_t (&values)[3], bool alreadyRendered, CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false ) {
		std::cerr << "SERIAL::processMoveXY()::ERROR: Not connected\n";
		return false;
	}
	
	if ( isCommandRunning ) {
		std::clog << "Serial::processMove: Serial is currently in fetching mode: This command will be rejected:" << std::endl;
		std::clog << " Command: '" << moveCommand[0] << "' [" << ArduinoCMDs::getCMDLabel(moveCommand[0]) << "]\n";
		std::clog << " Values: ";
		for ( unsigned int i=0; i<size; i++ )
			std::clog << values[i] << ", ";
			
		std::clog << std::endl;
		return true;
	}

	// Always log the start postion
	cncControl->SerialCallback(0);
	
	unsigned char* p = moveCommand;
	unsigned int idx = 0;

	if ( alreadyRendered  == true )	moveCommand[idx++] = CMD_MOVE; 
	else							moveCommand[idx++] = CMD_RENDER_AND_MOVE; 
	p++;
	
	for (unsigned int i=0; i<size; i++) {
		int32_t v  = htonl(values[i]);
		memcpy(p, &v, LONG_BUF_SIZE);
		idx += LONG_BUF_SIZE; 
		p   += LONG_BUF_SIZE;
	}
	
	if ( traceSpyInfo && spyWrite ) {
		cnc::spy.initializeResult();
		cnc::spy << "Send: '" << moveCommand[0] << "' [" << ArduinoCMDs::getCMDLabel(moveCommand[0]) << "]\n";
	}
	
	// to provide a time an pos reference for the speed calculation
	logMeasurementRefTs(pos);
	
	if ( writeData(moveCommand, idx) ) {
		SerialFetchInfo sfi;
		sfi.command 		= moveCommand[0];
		sfi.retSOHAllowed 	= true;
		sfi.returnAfterSOH  = false;
		sfi.Mc.size 		= size;
		sfi.Mc.value1		= values[0];
		sfi.Mc.value2		= values[1];
		sfi.Mc.value3		= values[2];
		
		bool ret = evaluateResultWrapper(sfi, std::cout, pos);
		// latest log this move
		logMeasurementLastTs();
		return ret;
		
	} else {
		std::cerr << "Serial::processMove: Unable to write data" << std::endl;
		cncControl->SerialCallback(0);
		return false;
	}

	return false;
}
///////////////////////////////////////////////////////////////////
const char* Serial::decodeContollerResult(int ret) {
///////////////////////////////////////////////////////////////////
	switch ( ret ) {
		case RET_NULL:		return "RET_NULL";
		case RET_OK: 		return "RET_OK";
		case RET_ERROR:		return "RET_ERROR";
		case RET_SOT:		return "RET_SOT";
		case RET_SOH:		return "RET_SOH";
		case RET_MSG:		return "RET_MSG";
		default:			static wxString s(wxString::Format("UNKNOWN(%d)", ret));
							return s.c_str();
	}
	
	return "";
}
///////////////////////////////////////////////////////////////////
bool Serial::evaluateResultWrapper(SerialFetchInfo& sfi, std::ostream& mutliByteStream, CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	if ( cncControl->isInterrupted() )
		return false;
	
	isCommandRunning = true;
	
	bool ret = false;
	ret = evaluateResult(sfi, mutliByteStream, pos);
	if ( cncControl->isInterrupted() ) {
		sendInterrupt();
		ret = false;
	}
	
	isCommandRunning = false;
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool Serial::evaluateResult(SerialFetchInfo& sfi, std::ostream& mutliByteStream, CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	// fetch loop
	while ( true ) {
		// read one byte from serial
		resetLastFetchResult();
		unsigned char ret = fetchControllerResult(sfi.singleFetchTimeout);
		setLastFetchType(ret);
		
		switch( ret ) {
			//evaluateResult..........................................
			case RET_OK:
			{
				cncControl->SerialCallback(0);
				return RET_OK_Handler(sfi, mutliByteStream, pos);
			}
			//evaluateResult..........................................
			case RET_ERROR:
			{
				std::cerr << "Serial::evaluateResult: ERROR while processing cmd: " << sfi.command << std::endl;
				
				if ( sfi.autoCallErrorInfo == true ) {
					processCommand("?", mutliByteStream, pos);
				}
	
				cncControl->SerialCallback(1);
				return false;
			}
			//evaluateResult..........................................
			case RET_SOT:
			{
				cncControl->SerialCallback(0);

				if ( sfi.retSOTAllowed == false ) {
					std::cerr << "Multibyte text results are not allowed in the context of command: " << sfi.command << std::endl;
					return false;
				}
				
				if ( RET_SOT_Handler(sfi, mutliByteStream, pos) == true ) {
					if ( sfi.returnAfterSOT == true ) {
						return true;
					} else {
						// check for interrupt
						if (cncControl->isInterrupted() ) {
							std::cerr << "RET_SOT: Interrupt detected. Fetch loop will be broken."<< std::endl;
							return false;
						}
						
						break; // stay in loop
					}
				} else {
					return false;
				}
				
				// should not occur
				wxASSERT(0);
			}
			
			//evaluateResult..........................................
			case RET_SOH:
			{
				cncControl->SerialCallback(0);
				
				if ( sfi.retSOHAllowed == false ) {
					
					// in this case try to decode this RET_SOH
					unsigned char cr = fetchControllerResult(sfi.singleFetchTimeout);
					if ( decode_RET_SOH_Default(cr, sfi) == true )
						// if OK call ths function recursive to handle the original request
						return evaluateResult(sfi, mutliByteStream, pos);
					
					std::cerr << "Multibyte results are not allowed in the context of command: " << sfi.command << std::endl;
					return false;
				}

				if ( RET_SOH_Handler(sfi, mutliByteStream, pos) == true ) {
					if ( sfi.returnAfterSOH == true ) {
						return true;
					} else  {
						// check for interrupt
						if (cncControl->isInterrupted() ) {
							std::cerr << "RET_SOH: Interrupt detected. Fetch loop will be broken."<< std::endl;
							return false;
						}
						
						break; // stay in loop
					}
				} else {
					return false;
				}
				
				// should not occur
				wxASSERT(0);
			}
			//evaluateResult..........................................
			case RET_MSG:
			{
				if ( readDataUntilMultyByteClose(sfi.multiByteResult, sizeof(sfi.multiByteResult) - 1)  > 0 ) {
					ControllerMsgInfo cmi;
					decodeMessage(sfi.multiByteResult, cmi.message);
					cncControl->SerialMessageCallback(cmi);
					
					return true;
				}
				
				return false;
			}
			//evaluateResult..........................................
			default: {
				
				std::cerr << "Serial::evaluateResult: Invalid Acknowlege: \n Cmd: " << sfi.command << ", Acknowlege as integer: " << (int)ret << "\n";
				cncControl->SerialCallback(1);
				
				return false;
			}
		}
	}
	
	return true;
}

///////////////////////////////////////////////////////////////////
bool Serial::RET_OK_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream, CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	switch ( sfi.command ) {
		//RET_OK_Handler...........................................
		case 'S':
		{
			if ( traceSpyInfo ) 
				cnc::spy.finalizeOK();

			cncControl->SerialCallback(1);
			return true;
		}
		
		//RET_OK_Handler...........................................
		case 'G':
		{
			cncControl->SerialCallback(1);

			if ( isEmulator() == false ) {
				wxString msg("RET_OK isn't valid for class command G\n");
				std::cerr << msg;
				cnc::spy.finalizeERROR(msg);
				return false;
			}
			
			if ( traceSpyInfo )
				cnc::spy.finalizeOK();
				
			return true;
		}
		
		//RET_OK_Handler...........................................
		case 'm':
		case 'M':
		{
			switch ( sfi.Mc.size ) {
				case 1:
					pos.incZ(sfi.Mc.value1);
					break;
				case 2:
					pos.incX(sfi.Mc.value1);
					pos.incY(sfi.Mc.value2);
					break;
				case 3:
					pos.incX(sfi.Mc.value1);
					pos.incY(sfi.Mc.value2);
					pos.incZ(sfi.Mc.value3);
					break;
			}
			
			// ensure the position update
			cncControl->SerialCallback(1);
			
			if ( traceSpyInfo ) 
				cnc::spy.finalizeOK();
				
			return true;
		}
		
		//RET_OK_Handler...........................................
		default:
		{
			switch ( sfi.command ) {
				case 'x': pos.decX(1); break;
				case 'X': pos.incX(1); break;
				case 'y': pos.decY(1); break;
				case 'Y': pos.incY(1); break;
				case 'z': pos.decZ(1); break;
				case 'Z': pos.incZ(1); break;
				default: ;// Do nothing
			}
			
			// ensure the position update
			cncControl->SerialCallback(1);
				
			if ( traceSpyInfo )
				cnc::spy.finalizeOK();
			
			return true;
		}
	}

	// should not occur
	wxASSERT(false);
	return false;
}
///////////////////////////////////////////////////////////////////
bool Serial::RET_SOT_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream, CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	switch ( sfi.command ) {
		//RET_SOT_Handler..........................................
		default:
		{
			if ( readDataUntilMultyByteClose(sfi.multiByteResult, sizeof(sfi.multiByteResult)-1) > 0 )
				decodeMultiByteResults(sfi.command, sfi.multiByteResult, mutliByteStream);
			
			cncControl->SerialCallback(1);
			
			if ( traceSpyInfo )
				cnc::spy.finalizeOK();
				
			return true;
		}
	}

	// should not occur
	wxASSERT(false);
	return true;
}
///////////////////////////////////////////////////////////////////
bool Serial::RET_SOH_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream, CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	// read first byte (content info) after RET_SOH
	unsigned char cr = fetchControllerResult(sfi.singleFetchTimeout);
	
	if ( cr == RET_ERROR ) {
		wxString msg("Serial::RET_SOH_Handler: Can't read content info");
		std::cerr << msg<< std::endl;
		cnc::spy.finalizeERROR(msg);
		return false;
	}
	
	switch ( sfi.command ) {
		//RET_SOH_Handler..........................................
		case 'G': 		return decodeGetter(sfi);
		
		//RET_SOH_Handler..........................................
		default:		return decode_RET_SOH_Default(cr, sfi);
	}

	// should not occur
	wxASSERT(false);
	return false;
}
///////////////////////////////////////////////////////////////////
bool Serial::decode_RET_SOH_Default(unsigned char cr, SerialFetchInfo& sfi) {
///////////////////////////////////////////////////////////////////
	if ( cr == RET_ERROR ) {
		std::cerr << "Serial::decode_RET_SOH_Default: Can't read content info" << std::endl;
		return false;
	}

	switch( cr ) {
		
		//RET_SOH_Handler..........................................
		case PID_HEARTBEAT:	return decodeHeartbeat(sfi);
		
		//RET_SOH_Handler..........................................
		case PID_X_POS:
		case PID_Y_POS:
		case PID_Z_POS:
		case PID_XYZ_POS:
		case PID_XYZ_POS_MAJOR:
		case PID_XYZ_POS_DETAIL:	return decodePositionInfo(sfi, cr);
		
		//RET_SOH_Handler..........................................
		case PID_LIMIT:		return decodeLimitInfo(sfi);
		
		//RET_SOH_Handler..........................................
		default:
		{
			std::cerr << "Serial::decode_RET_SOH_Default: Undefined content info: " << ArduinoPIDs::getPIDLabel((int)cr) << std::endl;
			return false;
		}
	}
	// should not occur
	wxASSERT(false);
	return false;
}
///////////////////////////////////////////////////////////////////
bool Serial::decodeGetter(SerialFetchInfo& sfi) {
///////////////////////////////////////////////////////////////////
	// first read getter count
	if ( ( sfi.Gc.bytes = readDataUntilSizeAvailable(sfi.Gc.result, 1, sfi.Gc.timeout) ) != 1 ) {
		std::cerr << "Serial::decodeGetter: Read of getter count failed." << std::endl;
		return false;
	}
	
	// determine value count
	sfi.Gc.count = (int)sfi.Gc.result[0];
	
	// error handling
	if ( sfi.Gc.count <= 0 ) {
		std::cerr << "Serial::decodeGetter: Read failed, nothing available." << std::endl;
		return false;
	}
	
	// error handling
	if ( sfi.Gc.count > 32 ) {
		std::cerr << "Serial::decodeGetter: Read failed, to much values!. Received count: " << sfi.Gc.bytes << std::endl;
		return false;
	}
	
	// error handling
	if ( sfi.Gc.list == NULL ) {
		std::cerr << "Serial::RET_SOT_Handler: Invalid list" << std::endl;
		return false;
	}
	
	// fetch the values
	int count = sfi.Gc.count;
	for ( int i=0; i<count; i++ ) {
		
		if ( ( sfi.Gc.bytes = readDataUntilSizeAvailable(sfi.Gc.result, LONG_BUF_SIZE, sfi.Gc.timeout) ) != LONG_BUF_SIZE ) {
			std::cerr << "Serial::decodeGetter: Read failed. Size error! Received size: " << sfi.Gc.bytes << std::endl;
			return false;
		}
		
		memcpy(&sfi.Gc.value, sfi.Gc.result, LONG_BUF_SIZE);
		sfi.Gc.list->push_back(sfi.Gc.value);
	}
	
	// read return code
	if ( ( sfi.Gc.bytes = readDataUntilSizeAvailable(sfi.Gc.result, 1, sfi.Gc.timeout) ) != 1 ) {
		std::cerr << "Serial::decodeGetter: Read return code failed." << std::endl;
		return false;
	}
	
	if ( traceSpyInfo )
		cnc::spy.finalizeOK();
	
	return (sfi.Gc.result[0] == RET_OK);
}
///////////////////////////////////////////////////////////////////
bool Serial::decodePositionInfo(SerialFetchInfo& sfi, unsigned char pid) {
///////////////////////////////////////////////////////////////////
	unsigned int size = LONG_BUF_SIZE;
	if ( pid == PID_XYZ_POS || pid == PID_XYZ_POS_MAJOR || pid == PID_XYZ_POS_DETAIL )
		size = sizeof(sfi.Mc.result);

	if ( (sfi.Mc.bytes = readDataUntilSizeAvailable(sfi.Mc.result, size)) <= 0 ) {
		std::cerr << "ERROR while reading position value(s). Nothing available" << std::endl;
		return false;
	}
	
	if ( sfi.Mc.bytes%4 != 0 ) {
		std::cerr << "ERROR while reading position value(s). Result cant broken down to int32_t values. Byte count: " << sfi.Mc.bytes << std::endl;
		return false;
	}

	//fetch 3 int32_t values
	ContollerInfo ci;
	ci.infoType = CITPosition;
	ci.command  = sfi.command;
	ci.posType 	= pid;

	sfi.Mc.p = sfi.Mc.result;
	for (int i=0; i<sfi.Mc.bytes; i+=LONG_BUF_SIZE) {
		memcpy(&sfi.Mc.value, sfi.Mc.p, LONG_BUF_SIZE);
		
		if ( pid == PID_XYZ_POS || pid == PID_XYZ_POS_MAJOR || pid == PID_XYZ_POS_DETAIL ) {
			switch (i) {
				case 0:	ci.xCtrlPos = sfi.Mc.value; break;
				case 4:	ci.yCtrlPos = sfi.Mc.value; break;
				case 8:	ci.zCtrlPos = sfi.Mc.value; break;
			}
		} else {
			switch ( pid ) {
				case PID_X_POS: ci.xCtrlPos = sfi.Mc.value; break;
				case PID_Y_POS: ci.yCtrlPos = sfi.Mc.value; break;
				case PID_Z_POS: ci.zCtrlPos = sfi.Mc.value; break;
			}
		}
		
		sfi.Mc.p += LONG_BUF_SIZE;
	}
	
	sendSerialControllrCallback(ci);
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool Serial::decodeHeartbeat(SerialFetchInfo& sfi) {
///////////////////////////////////////////////////////////////////
	if ( (sfi.Hc.bytes = readDataUntilSizeAvailable(sfi.Hc.result, sizeof(sfi.Hc.result))) <= 0 ) {
		std::cerr << "ERROR while reading heartbeat. Nothing available" << std::endl;
		return false;
	}
	
	if ( sfi.Hc.bytes%4 != 0 ) {
		std::cerr << "ERROR while reading heartbeat. Result cant broken down to int32_t values. Byte count: " << sfi.Hc.bytes << std::endl;
		return false;
	}
	
	ContollerInfo ci;
	ci.infoType = CITHeartbeat;
	ci.command   = sfi.command;
	
	sfi.Hc.p = sfi.Hc.result;
	for (int i=0; i<sfi.Hc.bytes; i+=LONG_BUF_SIZE) {
		memcpy(&sfi.Hc.value, sfi.Hc.p, LONG_BUF_SIZE);
		
		switch (i) {
			case 0:	ci.heartbeatValue  = sfi.Hc.value; break;
			case 4:	ci.heartbeatValue += sfi.Hc.value; break;
		}

		sfi.Hc.p += LONG_BUF_SIZE;
	}
	
	sendSerialControllrCallback(ci);
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool Serial::decodeLimitInfo(SerialFetchInfo& sfi) {
///////////////////////////////////////////////////////////////////
	if ( (sfi.Lc.bytes = readDataUntilSizeAvailable(sfi.Lc.result, sizeof(sfi.Lc.result))) <= 0 ) {
		std::cerr << "ERROR while reading limit values. Nothing available" << std::endl;
		return false;
	}
	
	if ( sfi.Lc.bytes%4 != 0 ) {
		std::cerr << "ERROR while reading limit values. Result cant broken down to int32_t values. Byte count: " << sfi.Lc.bytes << std::endl;
		return false;
	}

	//fetch 3 int32_t values
	ContollerInfo ci;
	ci.infoType = CITLimitInfo;
	ci.command  = sfi.command;
	
	sfi.Lc.p = sfi.Lc.result;
	for (int i=0; i<sfi.Lc.bytes; i+=LONG_BUF_SIZE) {
		memcpy(&sfi.Lc.value, sfi.Lc.p, LONG_BUF_SIZE);
		
		switch (i) {
			case 0:	ci.xLimit  = sfi.Lc.value; break;
			case 4:	ci.yLimit  = sfi.Lc.value; break;
			case 8:	ci.zLimit  = sfi.Lc.value; break;
		}

		sfi.Lc.p += LONG_BUF_SIZE;
	}
	
	sendSerialControllrCallback(ci);
	return true;
}
