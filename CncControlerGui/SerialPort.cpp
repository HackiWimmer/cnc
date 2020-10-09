#include <string> 
#include <sstream>
#include <chrono>
#include <sys/time.h>
#include <wx/thread.h>

#ifdef __WXGTK__
	#include <arpa/inet.h>
#endif

#include "OSD/CncTimeFunctions.h"
#include "CncControl.h"
#include "CncCommon.h"
#include "CncArduino.h"
#include "CncContext.h"
#include "CncCommon.h"
#include "SerialPort.h"

unsigned char SerialCommandLocker::lockedCommand = CMD_INVALID;
///////////////////////////////////////////////////////////////////
bool SerialCommandLocker::lock(CncControl* cnc) {
///////////////////////////////////////////////////////////////////
/*
	if ( cnc != NULL ) {
		// wait a portion of time
		unsigned int counter = 0;
		while ( lockedCommand != CMD_INVALID ) {
			cnc->waitActive(10);
			if ( counter++ > 10 )
				break;
		}
	}
*/
	// error sitiuation
	if ( lockedCommand != CMD_INVALID ) {
		
		std::cerr 	<< "Try to lock command: '" 
					<< ArduinoCMDs::getCMDLabel(command) 
					<< "', but '" 
					<< ArduinoCMDs::getCMDLabel(lockedCommand)
					<< "' is still active. Therefore, the '" 
					<< ArduinoCMDs::getCMDLabel(command) 
					<< "' operation will be rejected!" << std::endl;
		return false;
	}
	
	// always fine here
	locking       = true;
	lockedCommand = command;
	
	return true;
}


///////////////////////////////////////////////////////////////////
Serial::Serial(CncControl* cnc)
: SerialOSD()
, readBuffer()
, totalDistanceSteps{0L, 0L, 0L, 0L}
, totalDistanceMetric{0.0, 0.0, 0.0, 0.0}
, totalDistanceMetricRef(0.0)
, measuredFeedSpeed_MM_SEC(0.0)
, measurementRefPos(0, 0, 0)
, tsMeasurementStart(0LL)
, tsMeasurementRef(0LL)
, tsMeasurementLast(0LL)
, cncControl(cnc)
, measurementActive(false)
, writeOnlyMoveCommands(false)
, portName()
, lastFetchResult()
, traceSpyInfo(true)
, spyMode(Serial::SypMode::SM_NONE)
, spyRead(false)
, spyWrite(false)
, shouldCallbackSynchronizeAppPosition(false)
, factorX(THE_CONFIG->getDisplayFactX())
, factorY(THE_CONFIG->getDisplayFactY())
, factorZ(THE_CONFIG->getDisplayFactZ())
{
///////////////////////////////////////////////////////////////////
	resetTotalDistance();
}
///////////////////////////////////////////////////////////////////
Serial::~Serial() {
///////////////////////////////////////////////////////////////////
	disconnect();
}
///////////////////////////////////////////////////////////////////
void Serial::waitDuringRead(unsigned int millis) {
///////////////////////////////////////////////////////////////////
	CncTimeFunctions::sleepMilliseconds(millis);
}
///////////////////////////////////////////////////////////////////
void Serial::sleepMilliseconds(unsigned int millis) {
///////////////////////////////////////////////////////////////////
	// Sleep a while to give the real microcontroller a portion 
	// of time to do something
	CncTimeFunctions::sleepMilliseconds(millis);
}
///////////////////////////////////////////////////////////////////
void Serial::resetTotalDistance() { 
///////////////////////////////////////////////////////////////////
	totalDistanceMetric[0] = 0.0;
	totalDistanceMetric[1] = 0.0;
	totalDistanceMetric[2] = 0.0;
	totalDistanceMetric[3] = 0.0;
	
	totalDistanceSteps[0]  = 0L;
	totalDistanceSteps[1]  = 0L;
	totalDistanceSteps[2]  = 0L;
	totalDistanceSteps[3]  = 0L;
}
///////////////////////////////////////////////////////////////////
void Serial::startMeasurement() {
///////////////////////////////////////////////////////////////////
	measurementActive = true;
	
	resetTotalDistance();
	
	tsMeasurementStart  = CncTimeFunctions::getNanoTimestamp();
	tsMeasurementRef    = tsMeasurementStart;
	tsMeasurementLast   = tsMeasurementStart;
	
	// reset calculated feed speed
	measuredFeedSpeed_MM_SEC = 0.0;
	
	startMeasurementIntern();
}
///////////////////////////////////////////////////////////////////
void Serial::stopMeasurement() {
///////////////////////////////////////////////////////////////////
	measurementActive = false;
	
	stopMeasurementIntern();
	logMeasurementLastTs();
	
	// reset calculated feed speed
	measuredFeedSpeed_MM_SEC = 0.0;
}
///////////////////////////////////////////////////////////////////
void Serial::logMeasurementLastTs() {
///////////////////////////////////////////////////////////////////
	tsMeasurementLast = CncTimeFunctions::getNanoTimestamp();
	
	// calculate current feed speed
	if ( THE_CONTEXT->isProbeMode() == false ) {
		const short T = 3;
		const CncNanoTimespan tDiff = getMeasurementNanoTimeSpanLastRef();
		const double pDiff          = totalDistanceMetric[T] - totalDistanceMetricRef;
		
		// to avoid totalDistanceMetric calculations on the basis of to short difference
		// this is with respect, that windows can't sleep exactly
		const CncNanoTimestamp tThreshold = CncTimeFunctions::minWaitPeriod;
		const double           pThreshold = 0.5; // ->     mm
		
		if ( tDiff > tThreshold && pDiff > pThreshold ) {
			float F = pDiff;
			F      /= tDiff;
			F      *= std::nano::den;
			
			measuredFeedSpeed_MM_SEC  = F;
			
			if ( false ) {
				std::cout   << "tpDiff: " << tDiff <<  ", " << tDiff /1000 /1000 /1000  <<  ", " 
										  << pDiff <<  ", "
										  << measuredFeedSpeed_MM_SEC * 60
										  << std::endl;
			}
		}
	}
}
///////////////////////////////////////////////////////////////////
void Serial::logMeasurementRefTs(const CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	tsMeasurementRef = CncTimeFunctions::getNanoTimestamp();
	
	if ( THE_CONTEXT->isProbeMode() == false ) {
		const short T = 3;
		totalDistanceMetricRef = totalDistanceMetric[T];
		measurementRefPos.set(pos);
	}
}
///////////////////////////////////////////////////////////
CncNanoTimespan Serial::getMeasurementNanoTimeSpanTotal() const {
///////////////////////////////////////////////////////////////////
	return CncTimeFunctions::getTimeSpan(tsMeasurementLast, tsMeasurementStart);
}
///////////////////////////////////////////////////////////////////
CncNanoTimespan Serial::getMeasurementNanoTimeSpanLastRef() const {
///////////////////////////////////////////////////////////////////
	return CncTimeFunctions::getTimeSpan(tsMeasurementLast, tsMeasurementRef);
}
///////////////////////////////////////////////////////////////////
bool Serial::sendSerialControllerCallback(ContollerInfo& ci) {
///////////////////////////////////////////////////////////////////
	if ( cncControl == NULL )
		return false;
		
	// provide speed calculation info
	if ( ci.infoType == CITPosition )
		incTotalDistance(measurementRefPos, ci.xCtrlPos, ci.yCtrlPos, ci.zCtrlPos);
	
	return cncControl->SerialControllerCallback(ci);
}
///////////////////////////////////////////////////////////////////
bool Serial::sendSerialControllerCallback(ContollerExecuteInfo& cei) {
///////////////////////////////////////////////////////////////////
	if ( cncControl == NULL )
		return false;
		
	return cncControl->SerialExecuteControllerCallback(cei);
}
///////////////////////////////////////////////////////////////////
void Serial::incTotalDistance(int32_t dx, int32_t dy, int32_t dz) {
///////////////////////////////////////////////////////////////////
//dx, dy and dz acts as relative coordinates here
	const short    X = 0, Y = 1, Z = 2, T = 3;
	
	const int32_t ax = ArdoObj::absolute(dx);
	const int32_t ay = ArdoObj::absolute(dy);
	const int32_t az = ArdoObj::absolute(dz);
	
	// Attention: Each axis are moved separately, therefore the 
	// total distance is the addition of dx + dy + dz and not 
	// the corresponding vector sqrt(x*x + y*y + z*z)!
	
	totalDistanceSteps[X] 	+= ax;
	totalDistanceSteps[Y] 	+= ay;
	totalDistanceSteps[Z] 	+= az;
	totalDistanceSteps[T] 	+= (ax + ay + az);
	
	const double x 	= (ax * factorX);
	const double y 	= (ay * factorY);
	const double z 	= (az * factorZ);
	const double t 	= (x + y + z);
	
	totalDistanceMetric[X] 	+= x;
	totalDistanceMetric[Y] 	+= y;
	totalDistanceMetric[Z] 	+= z;
	totalDistanceMetric[T] 	+= t;

	logMeasurementLastTs();
}
///////////////////////////////////////////////////////////////////
void Serial::incTotalDistance(unsigned int size, const int32_t (&values)[3]) {
///////////////////////////////////////////////////////////////////
	switch ( size ) {
		case 1:		incTotalDistance(0,         0,         values[0]);	break;
		case 2:		incTotalDistance(values[0], values[1], 0        ); 	break;
		case 3:		incTotalDistance(values[0], values[1], values[2]); 	break;
		default:	std::cerr << "Serial::incTotalDistance: Invalid size: " << size << std::endl;
	}
}
///////////////////////////////////////////////////////////////////
void Serial::incTotalDistance(const CncLongPosition& pos, int32_t cx, int32_t cy, int32_t cz) {
///////////////////////////////////////////////////////////////////
	// pos acts as reference here
	// cx, cy and cz are absolute coordinates
	
	// create relative deltas to make a increment possible
	incTotalDistance(cx - pos.getX(), cy - pos.getY(), cz - pos.getZ());
	
	// adjust ref pos
	measurementRefPos.setXYZ(cx, cy, cz);
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
	bool ret = SerialOSD::connect(portName);
	ret == true ? this->portName = portName : this->portName = "";
	
	clearReadBuffer();
	return ret;
}
///////////////////////////////////////////////////////////////////
void Serial::disconnect(void) {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == true )
		SerialOSD::disconnect();
	
	clearReadBuffer();
	this->portName = "";
}
///////////////////////////////////////////////////////////////////
void Serial::purge(void) {
///////////////////////////////////////////////////////////////////
	clearReadBuffer();
	SerialOSD::purge();
}
///////////////////////////////////////////////////////////////////
void Serial::clearReadBuffer() {
///////////////////////////////////////////////////////////////////
	ReadBuffer empty;
	std::swap(readBuffer, empty);
}
///////////////////////////////////////////////////////////////////
bool Serial::isMoveCommand(unsigned char cmd) {
///////////////////////////////////////////////////////////////////
	if ( cmd == CMD_MOVE					|| 
		 cmd == CMD_MOVE_UNIT_LIMIT_IS_FREE	||
		 cmd == CMD_RENDER_AND_MOVE			||
		 cmd == CMD_MOVE_INTERACTIVE   
	   ) {
			return true;   
	   }
		
	return false;
}
///////////////////////////////////////////////////////////////////
bool Serial::dataAvailable() {
///////////////////////////////////////////////////////////////////
	unsigned char buffer[1];
	return peekData(buffer, 1) > 0;
}
///////////////////////////////////////////////////////////////////
int Serial::peekData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	//Peek data in a buffer, if nbByte is greater than the
	//maximum number of bytes available, it will return only the
	//bytes available. The function return -1 when nothing could
	//be read, the number of bytes actually read.
	
	if ( traceSpyInfo && spyWrite )
		cnc::spy.addMarker("Peek Data");
	
	int bytesRead = readData(buffer, nbByte);
	unsigned char *p = (unsigned char*)buffer;
	
	for ( int i=0; i<bytesRead ; i++ ) {
		readBuffer.push(*p);
		p++;
	}
	
	if ( bytesRead > 0 && traceSpyInfo && spyWrite )
		cnc::spy.addMarker(wxString::Format(" %d Byte(s) peeked . . . ", bytesRead));
	
	return bytesRead;
}
///////////////////////////////////////////////////////////////////
int Serial::readBufferedData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( readBuffer.empty() || nbByte == 0 )
		return 0;
	
	// if buffered data exists
	unsigned char *p		 = (unsigned char*)buffer;
	unsigned int bytesToRead = nbByte;
	
	while ( readBuffer.empty() == false ) {
		*p = readBuffer.front();
		readBuffer.pop();
		p++;
		
		if ( --bytesToRead == 0 )
			break;
	}
	
	// prefill buffer
	buffer = p;
	
	// return the byte count are already read
	const int bytesRead = nbByte - bytesToRead;
	return bytesRead;
}
///////////////////////////////////////////////////////////////////
int Serial::readData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	//Read data in a buffer, if nbByte is greater than the
	//maximum number of bytes available, it will return only the
	//bytes available. The function return -1 when nothing could
	//be read, the number of bytes actually read.
	
	if ( nbByte == 0 )
		return 0;
		
	const int bytesRead   = readBufferedData(buffer, nbByte);
	const int bytesToRead = nbByte - bytesRead;
	
	return bytesToRead > 0 ? SerialOSD::readData(buffer, bytesToRead) : bytesRead;
}
///////////////////////////////////////////////////////////////////
bool Serial::writeData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	//Writes data from a buffer through the Serial connection
	//return true on success.
	return SerialOSD::writeData(buffer, nbByte);
}
///////////////////////////////////////////////////////////////////
bool Serial::writeData(unsigned char cmd) {
///////////////////////////////////////////////////////////////////
	static unsigned char buffer[2];
	buffer[0] = cmd;
	buffer[1] = '\0';
	
	lastFetchResult.init(cmd);
	return writeData(buffer, 1);
}
///////////////////////////////////////////////////////////////////
void Serial::decodeMessage(const int bytes, const unsigned char* mutliByteStream, std::ostream& message) {
///////////////////////////////////////////////////////////////////
	if ( bytes <= 0 ) {
		message << MT_INFO;
		message << "Empty Message";
		return;
	}
	
	unsigned const char * p = mutliByteStream;
	if ( bytes > 0 ) {
		message << mutliByteStream[0];
		p++;
	}
	
	// include error message id on demand
	if ( bytes > 2 && mutliByteStream[1] == MT_MID_FLAG) {
		
		if ( mutliByteStream[2] != E_NO_ERROR ) {
			message << ArduinoErrorCodes::getECLabel(mutliByteStream[2]);
			message << " ";
		}
		
		// skip over to the text part
		p += 2;
	}
	
	// stream text part on demand
	if ( (char*)p != NULL ) {
		std::stringstream ss((char*)p);
		std::string s;
		while ( getline(ss, s, '\n') )
			message << s.substr(0, s.length()).c_str()  << std::endl;
			
		p += strlen(ss.str().c_str());
	}
}
///////////////////////////////////////////////////////////////////
void Serial::decodeMultiByteResults(const char cmd, const unsigned char* result, std::ostream& mutliByteStream) {
///////////////////////////////////////////////////////////////////
	size_t pos;
	int id;
	std::stringstream ss((char*)result);
	std::string s;
	
	//--------------------------------------------------------
	auto printBin = [&](wxString& ret, long value) {
		ret.clear();
		
		int i = CHAR_BIT * sizeof value;
		while(i--)
			ret.append((char)('0' + ((value >> i) & 1))); 
		
		return ret;
	};
	
	switch ( cmd ) {
		case CMD_PRINT_CONFIG: 
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
					
					const wxString unit = ArduinoPIDs::getPIDUnit((unsigned int)id);
					wxString value (s.substr(pos+1, s.length()-1).c_str());
					
					if ( unit == "BIN" ) {
						long v; 
						if ( value.ToLong(&v) ) {
							printBin(value, v);
							
							value.append(wxString::Format(" [% 12ld]", v));
						}
						
					}
					
					cncControl->appendPidKeyValueToControllerConfig(id, key, value, unit);
				}
			}
			break;
			
		case CMD_PRINT_PIN_REPORT:
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
					cncControl->appendNumKeyValueToControllerPinInfo((type == (int)'D' ? ArduinoDigitalPins::getPinLabel(pin) : ArduinoAnalogPins::getPinLabel(pin)),
																	  pin, type, mode, value);
				} else {
					mutliByteStream << CNC_LOG_FUNCT << ": Invalid format:\n";
					mutliByteStream << (int )pin  << ":";
					mutliByteStream	<< (char)type << ":";
					mutliByteStream << (char)mode << ":";
					mutliByteStream << value      << "\n";
				}
			}
			break;
			
		default:
			mutliByteStream << result;
	}
}
///////////////////////////////////////////////////////////////////
bool Serial::clearRemainingBytes(bool trace) {
///////////////////////////////////////////////////////////////////
	static const unsigned int maxBytes = 1024;
	unsigned char oneReadBuf[maxBytes];
	unsigned int bytes   = 0;
	unsigned int counter = 0;
	
	if ( traceSpyInfo && spyWrite )
		cnc::spy.addMarker(CNC_LOG_FUNCT);
		
	while ( ( bytes = readData(oneReadBuf, maxBytes - 1)) ) {
		std::cout << CNC_LOG_FUNCT;
		
		const wxString appendix(wxString::Format("Cleared bytes (# %u):", bytes));
		
		if ( traceSpyInfo && spyWrite )
			cnc::spy.addMarker(appendix);
			
		if ( trace == true ) {
			std::cout << ": " << appendix;
			
			wxString bStr;
			for (unsigned int i=0; i<bytes; i++)
				bStr.append(wxString::Format("%02X ", (int)oneReadBuf[i]));
			
			//std::cout << bStr << std::endl;
			std::cout << std::endl;
		}
		
		if ( counter++ > 16 ) {
			std::cerr << CNC_LOG_FUNCT << ": Fatal Error. Cancel reading more bytes from serial" << std::endl;
			return false;
		}
	}
	
	if ( trace == true && bytes == 0 ) {
		std::cout << " Remaining bytes (# 0): " << std::endl;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
unsigned char Serial::fetchControllerResult(SerialFetchInfo& sfi) {
///////////////////////////////////////////////////////////////////
	const int readSize = 1;
	unsigned char ret[readSize];
	ret[0] = 0;
	
	if ( readDataUntilSizeAvailable(ret, readSize, sfi.fetchTimeout, sfi.fetchTimeoutErrorInfo) != readSize ) {
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
	int bytesRead 	 = 0;
	unsigned int cnt = 0;
	
	while ( remainingBytes > 0 ) {
		bytesRead = readData(oneReadBuf, std::min(remainingBytes, maxBytes));
		//bytesRead = readData(oneReadBuf, 1);
		
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
			waitDuringRead(10);
			
			if ( ++cnt > maxDelay ) {
				if ( withErrorMsg == true )
					std::cerr << "Serial::readDataUntilSizeAvailable Timeout reached:" << std::endl;
					
				//return 0;
				break;
			}
				
			if ( cnt%100 == 0 )
				cncControl->SerialCallback();
		}
	}
	
	return nbByte - remainingBytes;
}
///////////////////////////////////////////////////////////////////
int Serial::readDataUntilMultyByteClose(unsigned char* buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	// Assumtion given buffer allocates nbByte bytes
	unsigned char b[1];
	
	int byteCounter       = 0;
	int bytesRead         = 0;
	int readFailedCounter = 0;
	
	bool fetch = true;
	while ( fetch ) {
		
		// error handling
		if ( readFailedCounter > 100 ) {
			std::cerr << "Serial::readDataUntilMultyByteClose: Max count of failed reads a reached." 
			<< std::endl;
			fetch = false;
			break;
		}
		
		// error handling
		if ( (unsigned int)byteCounter >= nbByte ) {
			std::cerr << "Serial::readDataUntilMultyByteClose: To much data availiable." 
			<< " Max bytes: "    << nbByte
			<< " Byte counter: " << byteCounter
			<< std::endl;
			fetch = false;
			break;
		}
		
		// read
		if ( ( bytesRead = readData(b, 1) ) > 0 ) {
			if ( b[0] == MBYTE_CLOSE ) {
				// close the string
				buffer[byteCounter] = '\0';
				// stop fetching here
				fetch = false;
				break;
				
			} else {
				
				// append response
				buffer[byteCounter] = b[0];
				byteCounter++;
			}
			
			// to release user events
			cncControl->SerialCallback();
			
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
bool Serial::popSerial() {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false ) {
		std::cerr << "SERIAL::popSerial()::ERROR: Not connected\n";
		return false;
	}
	
	const unsigned char cmd = CMD_POP_SERIAL;
	
	SerialCommandLocker scl(cmd);
	if ( scl.lock(cncControl) == false )
		return true;
	
	if ( writeOnlyMoveCommands == true )
		return true;
	
	if ( traceSpyInfo && spyWrite )
		cnc::spy.initializeResult(wxString::Format("Send: '%s'", ArduinoCMDs::getCMDLabel(cmd)));
	
	SerialFetchInfo sfi(cmd);
	sfi.fetchTimeout 			= 1000;
	sfi.fetchTimeoutErrorInfo 	= false;
	
	lastFetchResult.init(cmd);
	
	bool ret = true;
	if ( dataAvailable() ) {
		
		ret = evaluateResultWrapper(sfi, std::cout);
		if ( ret == false ) {
			std::cerr   << "Error while processing 'Serial::popSerial()': "											<< std::endl
						<< " Serial CMD = " << ArduinoCMDs::getCMDLabel((const unsigned char)lastFetchResult.cmd)	<< std::endl
						<< " Serial RET = " << ArduinoPIDs::getPIDLabel((const unsigned char)lastFetchResult.ret)	<< std::endl
			;
			
			return false;
		}
	}
	
	return cncControl->SerialCallback();
}
///////////////////////////////////////////////////////////////////
bool Serial::processIdle() {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false ) {
		std::cerr << "SERIAL::processIdle()::ERROR: Not connected\n";
		return false;
	}
	
	const unsigned char cmd = CMD_IDLE;
	
	SerialCommandLocker scl(cmd);
	if ( scl.lock(NULL) == false )
		return true;
		
	if ( writeOnlyMoveCommands == true )
		return true;
	
	if ( traceSpyInfo && spyWrite )
		cnc::spy.initializeResult(wxString::Format("Send: '%s'", ArduinoCMDs::getCMDLabel(cmd)));
	
	lastFetchResult.init(cmd);
	if ( writeData(cmd) ) {
		
		SerialFetchInfo sfi(lastFetchResult.cmd);

		bool ret = evaluateResultWrapper(sfi, std::cout);
		if ( ret == false ) {
			//std::cerr << "Error while processing idle: " << std::endl;
			//std::cerr << "Request \"Error Info\" for more details." << std::endl;
		}
		return ret;
		
	} else {
		std::cerr << "Serial::processSetter: Unable to write data" << std::endl;
		cncControl->SerialCallback();
		return false;
	}
	
	return false;
}
///////////////////////////////////////////////////////////////////
bool Serial::processSetter(unsigned char pid, int32_t value) {
///////////////////////////////////////////////////////////////////
	cnc::SetterValueList values;
	values.push_back(value);
	
	return processSetter(pid, values);
}
///////////////////////////////////////////////////////////////////
bool Serial::processSetter(unsigned char pid, const cnc::SetterValueList& values) {
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
	
	unsigned char cmd[3 * sizeof(unsigned char) + MAX_SETTER_VALUES * LONG_BUF_SIZE];
	unsigned char* p = cmd;
	
	int idx = 0;
	cmd[idx++] = CMD_SETTER;
	p++;
	
	cmd[idx++] = pid;
	p++;
	
	cmd[idx++] = (unsigned char)values.size();
	p++;

	for ( auto it = values.begin(); it != values.end(); it++ ) {
		//int32_t value = htonl(*it);
		int32_t value = *it;
		memcpy(p, &value, LONG_BUF_SIZE);
		idx += LONG_BUF_SIZE;
		p   += LONG_BUF_SIZE;
	}
	
	SerialCommandLocker scl(cmd[0]);
	if ( scl.lock(cncControl) == false ) {
		std::clog << "Serial::processSetter: Serial is currently in fetching mode: This command will be rejected:" << std::endl;
		std::clog << " Running Command: '" << ArduinoCMDs::getCMDLabel(SerialCommandLocker::getLockedCommand()) << "'\n";
		if ( pid < PID_FLOAT_RANG_START )	{
			std::clog << " This Command: '" << cmd[0] << "' [" << ArduinoCMDs::getCMDLabel(cmd[0]) 	<< "]"
												 <<   "[" << ArduinoPIDs::getPIDLabel((int)pid) 	<< "]"
												 <<   "["; 
												 cnc::traceSetterValueList(std::clog, values, 1);
			std::clog                            << "]\n";
		} else {
			std::clog << " This Command: '" << cmd[0] << "' [" << ArduinoCMDs::getCMDLabel(cmd[0]) 	<< "]"
			                                     <<   "[" << ArduinoPIDs::getPIDLabel((int)pid) 	<< "]"
												 <<   "[";
												 cnc::traceSetterValueList(std::clog, values, FLT_FACT);
			std::clog                            << "]\n";
		}
		return true;
	}
	
	if ( traceSpyInfo && spyWrite ) {
		std::stringstream ss;
		if ( pid < PID_FLOAT_RANG_START )	{
			ss << "Send: '" << cmd[0] 	<< "' [" << ArduinoCMDs::getCMDLabel(cmd[0])						<< "]"
										<<   "[" << ArduinoPIDs::getPIDLabel((int)pid) 						<< "]"
										<<   "[";
										cnc::traceSetterValueList(ss, values, 1);
			ss 							<<   "]";
		} 
		else {
			ss << "Send: '" << cmd[0] 	<< "' [" << ArduinoCMDs::getCMDLabel(cmd[0]) 						<< "]"
										<<   "[" << ArduinoPIDs::getPIDLabel((int)pid) 						<< "]"
										<<   "[";
										cnc::traceSetterValueList(ss, values, FLT_FACT);
			ss 							<<   "]";
		}
		
		cnc::spy.initializeResult(ss.str().c_str());
	}
	
	SerialFetchInfo sfi(cmd[0]);
	if ( serializeSetter(sfi, cmd, idx) == false ) {
		cncControl->SerialCallback();
		return false;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool Serial::processGetter(unsigned char pid, GetterValues& list) {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false ) {
		std::cerr << "SERIAL::processGetter()::ERROR: Not connected\n";
		return false;
	}
	
	if ( writeOnlyMoveCommands == true )
		return true;
	
	unsigned char cmd[2];
	cmd[0] = CMD_GETTER;
	cmd[1] = pid;
	
	SerialCommandLocker scl(cmd[0]);
	if ( scl.lock(cncControl) == false ) {
		std::clog << "Serial::processGetter: Serial is currently in fetching mode: This command will be rejected:" << std::endl;
		std::clog << " Running Command: '" << ArduinoCMDs::getCMDLabel(SerialCommandLocker::getLockedCommand()) << "'\n";
		std::clog << " This Command   : '" << ArduinoCMDs::getCMDLabel(cmd[0]) << ", " << ArduinoPIDs::getPIDLabel((pid)) << "'\n";
		return true;
	}
	
	if ( traceSpyInfo && spyWrite )
		cnc::spy.initializeResult(wxString::Format("Send: '%c' [%s][%s]", cmd[0], ArduinoCMDs::getCMDLabel(cmd[0]), ArduinoPIDs::getPIDLabel(pid)));
	
	list.clear();
	
	bool ret = false;
	
	lastFetchResult.init(cmd[0]);
	if ( writeData((char*)cmd, sizeof(cmd)) ) {
		
		SerialFetchInfo sfi(lastFetchResult.cmd);
		sfi.fetchTimeout = 1000;
		sfi.Gc.list = &list;

		ret = evaluateResultWrapper(sfi, std::cout);
		
	} else {
		std::cerr << "Serial::processGetter: Unable to write data" << std::endl;
		cncControl->SerialCallback();
		ret = false;
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool Serial::processGetterList(PidList pidList, GetterListValues& map) {
///////////////////////////////////////////////////////////////////
	if ( pidList.size() == 0 )
		return true;
		
	map.clear();
	for (PidList::iterator it = pidList.begin() ; it != pidList.end(); ++it) {
		unsigned char pid = *it;
		GetterValues list;
			
		if ( processGetter(pid, list) == true ) {
			map[pid] = list;
		} else {
			std::cerr << "Serial::processGetterList: Error while processing PID "<< ArduinoPIDs::getPIDLabel(pid) << std::endl;
		}
	}
	
	return ( map.size() > 0 );
}
///////////////////////////////////////////////////////////////////
bool Serial::sendSignal(const unsigned char cmd) {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false ) {
		std::cout << "SERIAL::sendSignal("<< cmd << ")::ERROR: Not connected\n";
		return false;
	}
	
	if ( traceSpyInfo && spyWrite )
		cnc::spy.initializeResult(wxString::Format("Send: '%c' [%s]", cmd, ArduinoCMDs::getCMDLabel(cmd)));
	
	lastFetchResult.init(cmd);
	return writeData(cmd);
}
///////////////////////////////////////////////////////////////////
bool Serial::execute(const unsigned char* buffer, unsigned int nbByte) { 
///////////////////////////////////////////////////////////////////
	if ( nbByte <=0 || buffer == NULL )
		return true;
		
	// activate this during the execute command
	ControllerCallbackShouldSynchronizeAppPosition instance(this);

	const unsigned char cmd = buffer[0];
	bool ret = false;
	
	switch ( cmd ) {
		// --------------------------------------------------------
		case CMD_SETTER:
		{
			// log setter (for gui only)
			CncCommandDecoder::SetterInfo si;
			if ( CncCommandDecoder::decodeSetter(buffer, nbByte, si) ) {
				ContollerExecuteInfo cei;
				cei.infoType 		= CEITSetter;
				cei.setterPid		= si.pid;
				cei.setterValueList = si.values;
				sendSerialControllerCallback(cei);
				
				// serialize
				SerialFetchInfo sfi(cmd);
				ret = serializeSetter(sfi, buffer, nbByte);
			}
			else {
				std::cerr << "Serial::execute(): CncCommandDecoder::decodeSetter() failed!" << std::endl;
				ret = false;
			}

			break;
		}
		// --------------------------------------------------------
		case CMD_RENDER_AND_MOVE:
		case CMD_MOVE:
		{
			SerialFetchInfo sfi(cmd);
			sfi.fetchTimeout 	= 3000;
			sfi.Mc.size 		= 3;
			sfi.Mc.value1		= 0;
			sfi.Mc.value2		= 0;
			sfi.Mc.value3		= 0;
			
			ret = serializeMove(sfi, buffer, nbByte);
			break;
		}
		// --------------------------------------------------------
		case CMD_MOVE_SEQUENCE:
		case CMD_RENDER_AND_MOVE_SEQUENCE:
		{
			CncMoveSequence retSeq(cmd);
			if ( CncCommandDecoder::decodeMoveSequence(buffer, nbByte, &retSeq) ) {
				ret = processMoveSequence(retSeq);
			}
			else {
				std::cerr << "Serial::execute(): CncCommandDecoder::decodeMoveSequence() failed!" << std::endl;
				ret = false;
			}
			
			break;
		}
		// --------------------------------------------------------
		default:
		{
			std::cerr << "Serial::execute(): No case for: '" << cmd << "'" << std::endl;
			ret = false;
		}
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool Serial::processCommand(const unsigned char cmd, std::ostream& mutliByteStream) {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false ) {
		std::cout << "SERIAL::processCommand(" << cmd << ")::ERROR: Not connected\n";
		return false;
	}
	
	SerialCommandLocker scl(cmd);
	if ( scl.lock(cncControl) == false ) {
		std::clog << "Serial::processCommand(): Serial is currently in fetching mode. This command will be rejected:" << std::endl;
		std::clog << " Running Command: '" << ArduinoCMDs::getCMDLabel(SerialCommandLocker::getLockedCommand()) << "'\n";
		std::clog << " This Command   : '" << ArduinoCMDs::getCMDLabel(cmd) << "'\n";
		return true;
	}

	// Always log the start position
	if ( cncControl->SerialCallback() == false )
		return false;
	
	bool ret = false;

	if ( writeOnlyMoveCommands == true && isMoveCommand((unsigned char)cmd) == false )
		return true;
	
	if ( traceSpyInfo && spyWrite )
		cnc::spy.initializeResult(wxString::Format("Send: '%c' [%s]", cmd, ArduinoCMDs::getCMDLabel(cmd)));
	
	lastFetchResult.init(cmd);
	if ( writeData(cmd) ) {
		SerialFetchInfo sfi(lastFetchResult.cmd);
		ret = evaluateResultWrapper(sfi, mutliByteStream);
	
	} else {
		std::cerr << "Serial::processCommand(" << cmd << "): Unable to write data" << std::endl;
		cncControl->SerialCallback();
		ret =  false;
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool Serial::processMoveZ(int32_t z1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	const unsigned int size = 1;
	static int32_t values[3];
	values[0] = z1;
	values[1] = 0;
	values[2] = 0;

	return processMove(size, values, alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool Serial::processMoveXY(int32_t x1, int32_t y1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	const unsigned int size = 2;
	static int32_t values[3];
	values[0] = x1;
	values[1] = y1;
	values[2] = 0;

	return processMove(size, values, alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool Serial::processMoveXYZ(int32_t x1, int32_t y1, int32_t z1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	const unsigned int size = 3;
	static int32_t values[3];
	values[0] = x1;
	values[1] = y1;
	values[2] = z1;

	return processMove(size, values, alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool Serial::processStartInteractiveMove() {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false ) {
		std::cout << "SERIAL::processUpdateInteractiveMove()::ERROR: Not connected\n";
		return false;
	}
	
	const unsigned int LEN = 1;
	unsigned char cmd[LEN];
	cmd[0] = CMD_MOVE_INTERACTIVE;
	
	if ( traceSpyInfo && spyWrite )
		cnc::spy.initializeResult(wxString::Format("Send: '%c' [%s]", cmd[0], ArduinoCMDs::getCMDLabel(cmd[0])));

	lastFetchResult.init(cmd[0]);
	return writeData(cmd, LEN);
}
///////////////////////////////////////////////////////////////////
bool Serial::processUpdateInteractiveMove(const CncLinearDirection x, const CncLinearDirection y, const CncLinearDirection z) {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false ) {
		std::cout << "SERIAL::processUpdateInteractiveMove()::ERROR: Not connected\n";
		return false;
	}
	
	const unsigned int LEN = 6;
	unsigned char cmd[LEN];
	cmd[0] = SIG_UPDATE;
	cmd[1] = (unsigned char)LEN;
	cmd[2] = PID_XYZ_INTERACTIVE_POS;
	cmd[3] = (unsigned char)x;
	cmd[4] = (unsigned char)y;
	cmd[5] = (unsigned char)z;
	
	if ( traceSpyInfo && spyWrite )
		cnc::spy.initializeResult(wxString::Format("Send: '%c' [%s]", cmd[0], ArduinoCMDs::getCMDLabel(cmd[0])));
		
	lastFetchResult.init(cmd[0]);
	return writeData(cmd, LEN);
}
///////////////////////////////////////////////////////////////////
bool Serial::resolveLimits(unsigned int size, const int32_t (&values)[3]) {
///////////////////////////////////////////////////////////////////
	unsigned char cmdType = CMD_MOVE_UNIT_LIMIT_IS_FREE;
	return processMoveInternal(size, values, cmdType);
}
///////////////////////////////////////////////////////////////////
bool Serial::processMove(unsigned int size, const int32_t (&values)[3], bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	unsigned char cmdType = alreadyRendered == true ? CMD_MOVE : CMD_RENDER_AND_MOVE;
	return processMoveInternal(size, values, cmdType);
}
///////////////////////////////////////////////////////////////////
bool Serial::processMoveInternal(unsigned int size, const int32_t (&values)[3], unsigned char cmdType) {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false ) {
		std::cerr << "SERIAL::processMoveXY()::ERROR: Not connected\n";
		return false;
	}
	
	SerialCommandLocker scl(moveCommand[0]);
	if ( scl.lock(cncControl) == false ) {
		std::clog << "Serial::processMove: Serial is currently in fetching mode: This command will be rejected:" << std::endl;
		std::clog << " Running Command : '" << ArduinoCMDs::getCMDLabel(SerialCommandLocker::getLockedCommand()) << "'\n";
		std::clog << " This Command    : '" << ArduinoCMDs::getCMDLabel(moveCommand[0]) << "'\n";
		std::clog << "  Values: ";
		for ( unsigned int i=0; i<size; i++ )
			std::clog << values[i] << ", ";
			
		std::clog << std::endl;
		return true;
	}

	// Always log the start postion
	if ( cncControl->SerialCallback() == false )
		return false;
	
	unsigned char* p = moveCommand;
	unsigned int idx = 0;
	
	moveCommand[idx++] = cmdType;
	p++;
	
	moveCommand[idx++] = (unsigned char)size;
	p++;

	for (unsigned int i=0; i<size; i++) {
		int32_t v  = values[i];
		memcpy(p, &v, LONG_BUF_SIZE);
		idx += LONG_BUF_SIZE; 
		p   += LONG_BUF_SIZE;
	}
	
	if ( traceSpyInfo && spyWrite )
		cnc::spy.initializeResult(wxString::Format("Send: '%c' [%s]", moveCommand[0], ArduinoCMDs::getCMDLabel(moveCommand[0])));
	
	SerialFetchInfo sfi(moveCommand[0]);
	sfi.fetchTimeout 	= 3000;
	sfi.Mc.size 		= size;
	sfi.Mc.value1		= values[0];
	sfi.Mc.value2		= values[1];
	sfi.Mc.value3		= values[2];
	
	if ( serializeMove(sfi, moveCommand, idx) == false ) {
		cncControl->SerialCallback();
		return false;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
const char* Serial::decodeContollerResult(int ret) {
///////////////////////////////////////////////////////////////////
	switch ( ret ) {
		case RET_NULL:			return "RET_NULL";
		case RET_OK: 			return "RET_OK";
		case RET_ERROR:			return "RET_ERROR";
		case RET_LIMIT:			return "RET_LIMIT";
		case RET_SOH:			return "RET_SOH";
		case RET_INTERRUPT:		return "RET_INTERRUPT";
		case RET_HALT:			return "RET_HALT";
		case RET_QUIT:			return "RET_QUIT";
		default:				static wxString s(wxString::Format("UNKNOWN(%d)", ret));
								return s.c_str();
	}
	
	return "";
}
///////////////////////////////////////////////////////////////////
bool Serial::evaluateResultWrapper(SerialFetchInfo& sfi, std::ostream& mutliByteStream) {
///////////////////////////////////////////////////////////////////
	if ( cncControl->isInterrupted() )
		return false;
	
	bool ret = evaluateResult(sfi, mutliByteStream);
	
	if ( cncControl->isInterrupted() ) {
		sendInterrupt();
		ret = false;
	}

	return ret;
}
///////////////////////////////////////////////////////////////////
bool Serial::evaluateResult(SerialFetchInfo& sfi, std::ostream& mutliByteStream) {
///////////////////////////////////////////////////////////////////
	static ContollerInfo handshakeInfo;
	handshakeInfo.infoType = CITHandshake;
	
	#define LOG_HANDSHAKE( ret ) \
		if ( lastFetchResult.cmd != sfi.command ) \
			if ( ArdoObj::isSignal(lastFetchResult.cmd) == false ) \
				std::cout << "lastFetchResult.cmd != sfi.command --> " << '"' << ArduinoCMDs::getCMDLabel((int)lastFetchResult.cmd) << '"'\
				                                                       << " != " \
																	   << '"' << ArduinoCMDs::getCMDLabel((int)sfi.command) << '"'\
																	   << std::endl; \
		 \
		lastFetchResult.ret		= ret; \
		handshakeInfo.command	= sfi.command; \
		handshakeInfo.handshake = ret; \
		cncControl->SerialControllerCallback(handshakeInfo);
	
	// main fetch loop
	lastFetchResult.resetResult(); 
	
	bool fetchMore = true;
	while ( fetchMore ) {
		
		// reset the index
		lastFetchResult.index = 0;
		
		// read one byte from serial
		unsigned char ret = fetchControllerResult(sfi);
		switch( ret ) {
			
			// -----------------------------------------------------------
			// fetch type handshake
			// -----------------------------------------------------------
			//evaluateResult..........................................
			case RET_MORE:
			{
				LOG_HANDSHAKE(ret)
				// always break the fetch loop
				return RET_MORE_Handler(sfi, mutliByteStream);
			}
			//evaluateResult..........................................
			case RET_OK:
			{
				LOG_HANDSHAKE(ret)
				// always break the fetch loop
				return RET_OK_Handler(sfi, mutliByteStream);
			}
			//evaluateResult..........................................
			case RET_ERROR:
			{
				LOG_HANDSHAKE(ret)
				// always break the fetch loop
				return RET_ERROR_Handler(sfi, mutliByteStream);
			}
			//evaluateResult..........................................
			case RET_INTERRUPT:
			{
				LOG_HANDSHAKE(ret)
				// always break the fetch loop
				return RET_INTERRUPT_Handler(sfi, mutliByteStream);
			}
			//evaluateResult..........................................
			case RET_HALT:
			{
				LOG_HANDSHAKE(ret)
				// always break the fetch loop
				return RET_HALT_Handler(sfi, mutliByteStream);
			}
			//evaluateResult..........................................
			case RET_QUIT:
			{
				LOG_HANDSHAKE(ret)
				// always break the fetch loop
				return RET_QUIT_Handler(sfi, mutliByteStream);
			}
			//evaluateResult..........................................
			case RET_LIMIT:
			{
				LOG_HANDSHAKE(ret)
				// always break the fetch loop
				return RET_LIMIT_Handler(sfi, mutliByteStream);
			}
			
			// -----------------------------------------------------------
			// fetch type multibyte results
			// -----------------------------------------------------------
			//evaluateResult..........................................
			case RET_SOH:
			{
				LOG_HANDSHAKE(ret)
				
				if ( RET_SOH_Handler(sfi, mutliByteStream) == true ) {
					
					// check for interrupt
					if ( cncControl->isInterrupted() ) {
						std::cerr << "RET_SOH: Interrupt detected. Fetch loop will be broken."<< std::endl;
						return false;
					}
					
					// Don't wait while command == CMD_POP_SERIAL 
					if ( sfi.command == CMD_POP_SERIAL ) {
						if ( dataAvailable() == false) {
							// break fetch loop
							return true;
						}
					}
					
					// stay in while loop and fetch more
					break;
					
				} else {
					// break fetch loop
					return false;
				}
				
				// should not occur
				wxASSERT(0);
			}
			
			// -----------------------------------------------------------
			// fetch type unknown
			// -----------------------------------------------------------
			//evaluateResult..........................................
			default: {
				std::stringstream ss;
				ss	<< "Serial::evaluateResult : Invalid Acknowlege:" << std::endl
					<< " Command               : '" << sfi.command << "' - " << ArduinoCMDs::getCMDLabel((int)sfi.command) << std::endl
					<< " Acknowlege as integer : "  << (int)ret << std::endl;
						  
				if ( traceSpyInfo )
					cnc::spy.finalizeRET_ERROR(ss.str().c_str());
					
				std::cerr << ss.str().c_str();
				
				cncControl->SerialCallback();
				
				if ( clearRemainingBytes(true) == false )
					std::cerr << "The function clearRemainingBytes also returned false!" << std::endl;
				
				return false;
			}
		}
	}
	
	return true;
	
	#undef LOG_HANDSHAKE
}
//////////////////////////////////////////////////////////////////
bool Serial::RET_MORE_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream) {
///////////////////////////////////////////////////////////////////
	// currently nothing specific to do

	if ( traceSpyInfo )
		cnc::spy.finalizeRET_MORE();
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool Serial::RET_OK_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream) {
///////////////////////////////////////////////////////////////////
	#warning - thinking about cncControl->getCurAppPosAsReference() here
	
	CncLongPosition& pos = cncControl->getCurAppPosAsReference();
	switch ( sfi.command ) {

		//RET_OK_Handler...........................................
		case CMD_MOVE:
		case CMD_RENDER_AND_MOVE:
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
			
			break;
		}
		
		case CMD_MOVE_SEQUENCE:
		case CMD_RENDER_AND_MOVE_SEQUENCE:
		{
			switch ( sfi.Msc.size ) {
				case 1:
					pos.incZ(sfi.Msc.value1);
					break;

				case 2:
					pos.incX(sfi.Msc.value1);
					pos.incY(sfi.Msc.value2);
					break;

				case 3:
					pos.incX(sfi.Msc.value1);
					pos.incY(sfi.Msc.value2);
					pos.incZ(sfi.Msc.value3);
					break;
			}
			
			break;
		}
	}
	
	if ( traceSpyInfo )
		cnc::spy.finalizeRET_OK();
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool Serial::RET_ERROR_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream) {
///////////////////////////////////////////////////////////////////
	std::cerr << "Serial::evaluateResult: " << decodeContollerResult(RET_ERROR) 
			  << " while processing: "      << ArduinoCMDs::getCMDLabel((int)sfi.command) 
			  << std::endl;
	
	if ( traceSpyInfo )
		cnc::spy.finalizeRET_ERROR();
	
	return false;
}
///////////////////////////////////////////////////////////////////
bool Serial::RET_INTERRUPT_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream) {
///////////////////////////////////////////////////////////////////
	std::cerr << "Serial::evaluateResult: " << decodeContollerResult(RET_INTERRUPT) 
			  << " while processing: "      << ArduinoCMDs::getCMDLabel((int)sfi.command) 
			  << std::endl;
	
	if ( traceSpyInfo )
		cnc::spy.finalizeRET_INTERRUPT();
	
	return false;
}
///////////////////////////////////////////////////////////////////
bool Serial::RET_HALT_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream) {
///////////////////////////////////////////////////////////////////
	cnc::cex1 << "Serial::evaluateResult: " << decodeContollerResult(RET_HALT) 
			  << " while processing: "      << ArduinoCMDs::getCMDLabel((int)sfi.command) 
			  << std::endl;
	
	if ( traceSpyInfo )
		cnc::spy.finalizeRET_HALT();
	
	return false;
}
///////////////////////////////////////////////////////////////////
bool Serial::RET_QUIT_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream) {
///////////////////////////////////////////////////////////////////
	if ( traceSpyInfo )
		cnc::spy.finalizeRET_QUIT();
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool Serial::RET_LIMIT_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream) {
///////////////////////////////////////////////////////////////////
	std::cout << "Serial::evaluateResult: " 
			  << decodeContollerResult(RET_LIMIT) 
			  << " while processing: " 
			  << ArduinoCMDs::getCMDLabel((int)sfi.command) << std::endl;
	
	if ( traceSpyInfo )
		cnc::spy.finalizeRET_LIMIT();
	
	return false;
}
///////////////////////////////////////////////////////////////////
bool Serial::RET_SOH_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream) {
///////////////////////////////////////////////////////////////////
	static wxString msg;
	
	// read first byte (content info) after RET_SOH
	unsigned char pid = fetchControllerResult(sfi);
	
	// handle nested RET_SOH
	if ( pid == RET_SOH )
		return true;

	lastFetchResult.pid = pid;
	
	switch( pid ) {
		//RET_SOH_Handler..........................................
		case PID_GETTER:			return decodeGetter(sfi);
		//RET_SOH_Handler..........................................
		case PID_MSG:
		case PID_TEXT:				return decodeText(pid, sfi, mutliByteStream);
		//RET_SOH_Handler..........................................
		case PID_HEARTBEAT:			return decodeHeartbeat(sfi);
		//RET_SOH_Handler..........................................
		case PID_X_POS:
		case PID_Y_POS:
		case PID_Z_POS:
		case PID_XYZ_POS:
		case PID_XYZ_POS_MAJOR:
		case PID_XYZ_POS_DETAIL:	return decodePositionInfo(pid, sfi);
		//RET_SOH_Handler..........................................
		case PID_LIMIT:				return decodeLimitInfo(sfi);
		
		//RET_SOH_Handler..........................................
		case RET_ERROR:
		case RET_INTERRUPT:
		case RET_HALT:
		case RET_QUIT:
		case RET_LIMIT:				msg.assign(wxString::Format("Serial::RET_SOH_Handler: Received %s. Can't read content info", decodeContollerResult(pid)));
									std::cerr << msg << std::endl;
									if ( traceSpyInfo )
										cnc::spy.finalizeRET_ERROR(msg);
									return false;
			
		//RET_SOH_Handler..........................................
		default:
		{
			std::cerr << "Serial::decode_RET_SOH_Default: Undefined content info: " << ArduinoPIDs::getPIDLabel((int)pid) << std::endl;
			return false;
		}
	}
	
	// should not occur
	wxASSERT(false);
	return false;
}
///////////////////////////////////////////////////////////////////
bool Serial::decodeText(unsigned char pid, SerialFetchInfo& sfi, std::ostream& mutliByteStream) {
///////////////////////////////////////////////////////////////////
	bool ret 	= false;
	int bytes 	= 0;
	
	switch ( pid ) {
		case PID_TEXT:	if ( (bytes = readDataUntilMultyByteClose(sfi.multiByteResult, sizeof(sfi.multiByteResult) - 1)) > 0 ) {
							ret = true;
							decodeMultiByteResults(sfi.command, sfi.multiByteResult, mutliByteStream);
						}
						break;
		
		case PID_MSG:	if ( (bytes = readDataUntilMultyByteClose(sfi.multiByteResult, sizeof(sfi.multiByteResult) - 1)) > 0 ) {
							ret = true;
							ControllerMsgInfo cmi;
							decodeMessage(bytes, sfi.multiByteResult, cmi.message);
							cncControl->SerialMessageCallback(cmi);
						}
						break;

		default:		std::cerr << "Serial::decodeText: PID: " << ArduinoPIDs::getPIDLabel((int)pid) << "currently not supported" << std::endl;
						clearRemainingBytes(true);
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool Serial::decodeGetter(SerialFetchInfo& sfi) {
///////////////////////////////////////////////////////////////////
	// read concrete getter PID
	unsigned char pid = fetchControllerResult(sfi);
	
	// first read getter count
	if ( ( sfi.Gc.bytes = readDataUntilSizeAvailable(sfi.Gc.result, 1, sfi.Gc.timeout) ) != 1 ) {
		std::cerr << "Serial::decodeGetter: Read of getter count failed. PID: " << ArduinoPIDs::getPIDLabel(pid) << std::endl;
		return false;
	}
	
	// determine value count
	sfi.Gc.count = (int)sfi.Gc.result[0];
	
	// error handling
	if ( sfi.Gc.count <= 0 ) {
		std::cerr << "Serial::decodeGetter: Read failed, nothing available. PID: " << ArduinoPIDs::getPIDLabel(pid) << std::endl;
		return false;
	}
	
	// error handling
	if ( sfi.Gc.count > 32 ) {
		std::cerr << "Serial::decodeGetter: Read failed, too much values!. Received count: " << sfi.Gc.bytes << "; PID: " << ArduinoPIDs::getPIDLabel(pid) << std::endl;
		return false;
	}
	
	// error handling
	if ( sfi.Gc.list == NULL ) {
		std::cerr << "Serial::decodeGetter: Invalid list. PID " << ArduinoPIDs::getPIDLabel(pid) << std::endl;
		return false;
	}
	
	// fetch the values
	int count = sfi.Gc.count;
	for ( int i=0; i<count; i++ ) {
		
		if ( ( sfi.Gc.bytes = readDataUntilSizeAvailable(sfi.Gc.result, LONG_BUF_SIZE, sfi.Gc.timeout) ) != LONG_BUF_SIZE ) {
			std::cerr << "Serial::decodeGetter: Read failed. Size error! Received size: " << sfi.Gc.bytes << "; PID: " << ArduinoPIDs::getPIDLabel(pid) << std::endl;
			return false;
		}
		
		memcpy(&sfi.Gc.value, sfi.Gc.result, LONG_BUF_SIZE);
		sfi.Gc.list->push_back(sfi.Gc.value);
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool Serial::decodePositionInfo(unsigned char pid, SerialFetchInfo& sfi) {
///////////////////////////////////////////////////////////////////
	unsigned int size = LONG_BUF_SIZE;
	if ( pid == PID_XYZ_POS || pid == PID_XYZ_POS_MAJOR || pid == PID_XYZ_POS_DETAIL )
		size = sizeof(sfi.Mc.result);

	if ( (sfi.Mc.bytes = readDataUntilSizeAvailable(sfi.Mc.result, size)) <= 0 ) {
		std::cerr << "ERROR while reading position value(s). Nothing available" << std::endl;
		return false;
	}
	
	if ( sfi.Mc.bytes%4 != 0 ) {
		std::cerr << "ERROR while reading position value(s). Result can't broken down to int32_t values. Byte count: " << sfi.Mc.bytes << std::endl;
		return false;
	}

	//fetch 4 int32_t values
	ContollerInfo ci;
	ci.infoType = CITPosition;
	ci.command  = sfi.command;
	ci.posType 	= pid;

	sfi.Mc.p = sfi.Mc.result;
	for (int i=0; i<sfi.Mc.bytes; i+=LONG_BUF_SIZE) {
		memcpy(&sfi.Mc.value, sfi.Mc.p, LONG_BUF_SIZE);
		
		if ( pid == PID_XYZ_POS || pid == PID_XYZ_POS_MAJOR || pid == PID_XYZ_POS_DETAIL ) {
			switch (i) {
				case  0:	ci.xCtrlPos  = sfi.Mc.value; 			break;
				case  4:	ci.yCtrlPos  = sfi.Mc.value; 			break;
				case  8:	ci.zCtrlPos  = sfi.Mc.value; 			break;
				case 12:	ci.feedSpeed = sfi.Mc.value/FLT_FACT; 	break;
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
	
	sendSerialControllerCallback(ci);
	
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
		std::cerr << "ERROR while reading limit values. Result can't broken down to int32_t values. Byte count: " << sfi.Lc.bytes << std::endl;
		return false;
	}

	//fetch 3 int32_t values
	ContollerInfo ci;
	ci.infoType = CITLimit;
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
	
	sendSerialControllerCallback(ci);
	return true;
}
///////////////////////////////////////////////////////////////////
bool Serial::decodeHeartbeat(SerialFetchInfo& sfi) {
///////////////////////////////////////////////////////////////////
	// first reading heartbeat value
	if ( (sfi.Sc.bytes = readDataUntilSizeAvailable(sfi.Sc.result, LONG_BUF_SIZE)) < (int)LONG_BUF_SIZE ) {
		std::cerr << "ERROR while reading heartbeat counter value. Nothing available" << std::endl;
		return false;
	}
	sfi.Sc.p = sfi.Sc.result;
	int32_t heartbeatValue = 0;
	memcpy(&heartbeatValue, sfi.Sc.p, LONG_BUF_SIZE);
	
	ContollerInfo ci;
	ci.infoType 		= CITHeartbeat;
	ci.command   		= sfi.command;
	ci.heartbeatValue  	= heartbeatValue;
	
	// read remaining bytes
	const int remainingBytes = 4;
	sfi.Sc.bytes = readDataUntilSizeAvailable(sfi.Sc.result, remainingBytes);
	sfi.Sc.p 	 = sfi.Sc.result;
	
	if ( sfi.Sc.bytes != remainingBytes ) {
		std::cerr << "WARNING while reading furter heartbeat bytes. Expected byte count: " << remainingBytes;
		std::cerr << " received: " << sfi.Sc.bytes << std::endl;
	}
	
	for (int b=0; b<sfi.Sc.bytes; b++) {
		memcpy(&sfi.Sc.value, sfi.Sc.p, 1);
		
		switch ( b ) {
			case 0:		ci.limitState   = true; ci.limitStateValue   = sfi.Sc.value; break;
			case 1:		ci.supportState = true; ci.supportStateValue = sfi.Sc.value; break;
			case 2:		break;
			case 3:		break;
			
			default: std::cerr << "ERROR while reading state info values. Currently only 4 bytes (int32) possible" << std::endl;
		}
		
		sfi.Sc.p += 1;
	}

	sendSerialControllerCallback(ci);
	return true;
}
///////////////////////////////////////////////////////////////////
bool Serial::processMoveSequence(CncMoveSequence& sequence) {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false ) {
		std::cerr << "SERIAL::processMoveSequence()::ERROR: Not connected\n";
		return false;
	}
	
	SerialCommandLocker scl(sequence.getType());
	if ( scl.lock(cncControl) == false ) {
		std::clog << "SERIAL::processMoveSequence(): Serial is currently in fetching mode: This command will be rejected:" << std::endl;
		return true;
	}
	
	// Always log the start postion
	if ( cncControl->SerialCallback() == false )
		return false;
		
	// create move sequence write buffer
	CncMoveSequence::FlushResult result;
	if ( sequence.flush(result) == false ) {
		std::clog << "SERIAL::processMoveSequence(): seqeunce.flush failed" << std::endl;
		return false;
	}
	
	if ( writeMoveSequenceRawCallback( result.buffer, result.flushedSize) == false ) {
		std::clog << "SERIAL::processMoveSequence(): writeMoveSequenceRawCallback failed" << std::endl;
		return false;
	}
	
	unsigned int totalFlushedSize	= sequence.getFlushedSize();
	unsigned int currentFlushedSize	= *(sequence.getPortionIndex().begin());
	unsigned char* moveSequence 	= result.buffer;
	moveCommand[0] 					= moveSequence[0];
	
	// to provide a time and pos reference for the speed calculation
	logMeasurementRefTs(cncControl->getCurAppPos());
	
	// over all portions
	for ( auto it = sequence.getPortionIndex().begin(); it != sequence.getPortionIndex().end(); ++it ) {
		const unsigned int portionCounter = std::distance(sequence.getPortionIndex().begin(), it);
		
		if ( traceSpyInfo && spyWrite )
			cnc::spy.initializeResult(wxString::Format("Send: '%c' [%s]; portion=%u",	moveSequence[0],
																						ArduinoCMDs::getCMDLabel(moveSequence[0]),
																						portionCounter + 1
									 ));
		// notify
		if ( cncControl->SerialCallback() == false )
			return false;
		
		const unsigned int portionStart		= *it;
		const unsigned int portionLength	= (unsigned int)moveSequence[portionStart];
		const unsigned int portionTotLength	= portionLength + 1;
		
		const unsigned int writeStart		= portionCounter > 0 ? portionStart  : 0;
		const unsigned int writeLength		= portionCounter > 0 ? portionTotLength : portionStart + portionTotLength;
		
		if ( false ) {
			std::cout << "portionCounter: " << portionCounter << std::endl;
			std::cout << " - portion : " << portionStart << "->" << portionLength << std::endl;
			std::clog << " - write   : " << writeStart   << "->" << writeLength   << std::endl;
		}
		 
		// write ....
		lastFetchResult.init(moveSequence[0], portionCounter);
		if ( writeData(moveSequence + writeStart, writeLength) ) {
			currentFlushedSize += portionTotLength; 
			
			SerialFetchInfo sfi(lastFetchResult.cmd);
			sfi.fetchTimeout 		= 3000;
			sfi.Msc.size 			= 3;
			sfi.Msc.value1			= result.sequenceData.targetX;
			sfi.Msc.value2			= result.sequenceData.targetY;
			sfi.Msc.value3			= result.sequenceData.targetZ;
			
			// evaluate handshake
			bool handshake = evaluateResultWrapper(sfi, std::cout);
			if ( handshake == false ) {
				std::cerr << "SERIAL::processMoveSequence(" << portionCounter << "): Invalid handshake" << std::endl;
				cncControl->SerialCallback();
				return false;
			}
			
			if ( lastFetchResult.ret != RET_MORE && lastFetchResult.ret != RET_OK ) {
				std::clog << "SERIAL::processMoveSequence(" << portionCounter << "): lastFetchResult.ret != RET_MORE | RET OK" << std::endl;
				break;
			}
			
			// notify
			if ( cncControl->SerialCallback() == false )
				return false;
			
		} else {
			std::cerr << "SERIAL::processMoveSequence(" << portionCounter << "): Unable to write data" << std::endl;
			cncControl->SerialCallback();
			return false;
		}
	}
	
	// final quality check
	if ( totalFlushedSize != currentFlushedSize ) {
		std::cerr << "SERIAL::processMoveSequence(final): totalFlushedSize != currentFlushedSize" << std::endl;
		std::cerr << " - totalFlushedSize            : " << totalFlushedSize 	<< std::endl;
		std::cerr << " - currentFlushedSize          : " << currentFlushedSize 	<< std::endl;
		return false;
	}
	
	// latest log this move
	logMeasurementLastTs();
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool Serial::test() {
///////////////////////////////////////////////////////////////////
	bool ret = false;
	
	if ( false ) {
		CncMoveSequence cms(CMD_RENDER_AND_MOVE_SEQUENCE);
		cms.addMetricPosXYZ( +200,  +0,   0);

		return processMoveSequence(cms);
	}

	if ( true ) {
		CncMoveSequence cms(CMD_RENDER_AND_MOVE_SEQUENCE);
		
		float q = 1.0;
		for ( auto i=0; i<5; i++) {
			for ( auto j=0; j<10; j++) 
				cms.addMetricPosXYZ( q * +10, q *  +10,   0);
			
			for ( auto j=0; j<10; j++) 
				cms.addMetricPosXYZ( q * +10, q *  -10,   0);
				
			for ( auto j=0; j<10; j++) 
				cms.addMetricPosXYZ( q * -10, q *  -10,   0);
				
			for ( auto j=0; j<10; j++) 
				cms.addMetricPosXYZ( q * -10, q * +10,   0);
				
			q -= 0.1;
		}

		return processMoveSequence(cms);
	}

	if ( true ) {
		CncMoveSequence cms(CMD_RENDER_AND_MOVE_SEQUENCE);
		cms.addMetricPosXYZ(+0,   +0, +10);
		cms.addMetricPosXYZ(+10, +20,  +0);
		cms.addMetricPosXYZ(+0,   +0, -10);


		cms.addMetricPosXYZ( +50,  +20,   0);
		cms.addMetricPosXYZ( +20,  +10,   0);
		cms.addMetricPosXYZ( +20,   -5,   0);
		cms.addMetricPosXYZ( +15,   -3,   0);
		cms.addMetricPosXYZ( +10,   -2,   0);
		cms.addMetricPosXYZ( +20,  +10,   0);

		return processMoveSequence(cms);
	}
	
	if ( true ) {
		CncMoveSequence cms(CMD_RENDER_AND_MOVE_SEQUENCE);
		
		bool fOneB  = true;
		bool fInt8  = true;
		bool fInt16 = false;
		bool fInt32 = false;
		
		for ( int i = 0; i <50; i++ ) {
			// one byte values
			if ( fOneB ) {
				cms.addStepPosXYZ(+1, -1, +1);
				cms.addStepPosXYZ(-1, +1,  0);
				cms.addStepPosXYZ(+1,  0,  0);
				cms.addStepPosXYZ( 0, -1,  0);
				cms.addStepPosXYZ( 0,  0, +1);
			}
			
			// int8_t values
			if ( fInt8 ) {
				cms.addStepPosXYZ(+11, -12, +13);
				cms.addStepPosXYZ(-11,  12,   0);
				cms.addStepPosXYZ(-11,   0,   0);
				cms.addStepPosXYZ(  0,  12,   0);
				cms.addStepPosXYZ(  0,   0, -13);
			}
			
			// int16_t values
			if ( fInt16 ) {
				cms.addStepPosXYZ(+1000, +2000, +30);
				cms.addStepPosXYZ(+1000, +2000,   0);
				cms.addStepPosXYZ(-1000,     0,   0);
				cms.addStepPosXYZ(    0, -2000,   0);
				cms.addStepPosXYZ(    0,     0, -30);
			}
			
			// int32_t values
			if ( fInt32 ) {
				cms.addStepPosXYZ(+100000, +200000, +30);
				cms.addStepPosXYZ(+100000, +200000,   0);
				cms.addStepPosXYZ(-100000,       0,   0);
				cms.addStepPosXYZ(      0, -200000,   0);
				cms.addStepPosXYZ(      0,     0,   -30);
			}
		}
		
		std::clog << "processMoveSequence: count = " << cms.getCount() << std::endl;
		ret = processMoveSequence(cms);
	}
	else {
		CncMoveSequence cms(CMD_MOVE_SEQUENCE);
		
		for ( int i = 0; i < 500; i++ ) {
			// one byte values
			cms.addStepPosXYZ(+1, -1, +1);
			cms.addStepPosXYZ(-1, +1,  0);
			cms.addStepPosXYZ(+1,  0,  0);
			cms.addStepPosXYZ( 0, -1,  0);
			cms.addStepPosXYZ( 0,  0, +1);
		}
		
		std::clog << "processMoveSequence: count = " << cms.getCount() << std::endl;
		ret = processMoveSequence(cms);
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool Serial::serializeSetter(SerialFetchInfo& sfi, const unsigned char* buffer, unsigned int nbByte) { 
///////////////////////////////////////////////////////////////////
	if ( buffer == 0 || nbByte == 0 ) {
		std::cerr << "Serial::serializeSetter(): Invalid buffer" << std::endl;
		return false;
	}
	
	lastFetchResult.init(buffer[0]);
	
	if ( writeData((void*)buffer, nbByte) == false ) {
		std::cerr << "Serial::processSetter: Unable to write data" << std::endl;
		return false;
	}
	
	return evaluateResultWrapper(sfi, std::cout);
}
///////////////////////////////////////////////////////////////////
bool Serial::serializeMove(SerialFetchInfo& sfi, const unsigned char* buffer, unsigned int nbByte) { 
///////////////////////////////////////////////////////////////////
	if ( buffer == 0 || nbByte == 0 ) {
		std::cerr << "Serial::serializeMove(): Invalid buffer" << std::endl;
		return false;
	}

	// to provide a time an pos reference for the speed calculation
	logMeasurementRefTs(cncControl->getCurAppPos());
	
	lastFetchResult.init(buffer[0]);
	
	if ( writeData((void*)buffer, nbByte) == false) {
		std::cerr << "Serial::processMove: Unable to write data" << std::endl;
		return false;
	}
	
	bool ret = evaluateResultWrapper(sfi, std::cout);
	logMeasurementLastTs();
	
	return ret;
}
