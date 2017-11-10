#include <string> 
#include <sstream>
#include <wx/thread.h>
#include "strsafe.h"
#include "CncControl.h"
#include "CncCommon.h"
#include "CncArduino.h"
#include "SerialPort.h"
#include <windows.h>

char STATIC_CMD_CHAR[2];

///////////////////////////////////////////////////////////////////
Serial::Serial(CncControl* cnc)
: cncControl(cnc)
, connected(false)
, writeOnlyMoveCommands(false)
, isCommand(false)
, traceInfo(true)
, portName()
, lastFetchResult(RET_NULL)
{
///////////////////////////////////////////////////////////////////
	STATIC_CMD_CHAR[1] = '\0';
}
///////////////////////////////////////////////////////////////////
Serial::Serial(const char *portName)
: connected(false)
, writeOnlyMoveCommands(false)
, isCommand(false)
, traceInfo(true)
, portName()
, lastFetchResult(RET_NULL)
{
///////////////////////////////////////////////////////////////////
	STATIC_CMD_CHAR[1] = '\0';
	connect(portName);
}
///////////////////////////////////////////////////////////////////
Serial::~Serial() {
///////////////////////////////////////////////////////////////////
	disconnect();
}
///////////////////////////////////////////////////////////////////
void Serial::displayErrorInfo(LPTSTR lpszFunction) {
///////////////////////////////////////////////////////////////////
	// Retrieve the system error message for the last-error code
	
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError(); 

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	// Display the error message and exit the process
	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 

	StringCchPrintf((LPTSTR)lpDisplayBuf, 
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		"%S failed with error %ld: %S", 
		lpszFunction, dw, (LPCTSTR)lpMsgBuf); 

	std::cerr << (LPCTSTR)lpDisplayBuf;
    
    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
	 
}
///////////////////////////////////////////////////////////////////
bool Serial::connect(const char* portName) {
///////////////////////////////////////////////////////////////////
	//We're not yet connected
	this->connected = false;

	//Try to connect to the given port throuh CreateFile
	this->hSerial = CreateFileA(portName,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

	//Check if the connection was successfull
	if( this->hSerial==INVALID_HANDLE_VALUE )
	{
		if( GetLastError()==ERROR_FILE_NOT_FOUND ){
			std::cerr << " Serial::ERROR: Handle was not attached. Reason: " << portName << " not available.\n";
		} else {
			std::cerr << " Serial::ERROR: Unknown error!";
		}

		return false;
	} 
	
	//If connected we try to set the comm parameters
	DCB dcbSerialParams;
	dcbSerialParams.DCBlength = sizeof(DCB);

	//Try to get the current
	if ( !GetCommState(this->hSerial, &dcbSerialParams) ) {
		std::cerr << "Serial::ERROR: Failed to get current serial parameters!";
		return false;
	} 
	
	//Define serial connection parameters for the arduino board
	dcbSerialParams.BaudRate      	= BAUD_RATE;
	dcbSerialParams.fBinary       	= TRUE;
	dcbSerialParams.fParity      	= FALSE;
	dcbSerialParams.fOutxCtsFlow 	= FALSE;
	dcbSerialParams.fOutxDsrFlow  	= FALSE;
	dcbSerialParams.fDtrControl   	= DTR_CONTROL_DISABLE;
	dcbSerialParams.fDsrSensitivity = FALSE; // ??? should this be TRUE?
	dcbSerialParams.fNull         	= FALSE;
	dcbSerialParams.fOutX         	= FALSE;
	dcbSerialParams.fInX          	= FALSE;
	dcbSerialParams.fRtsControl   	= RTS_CONTROL_DISABLE;
	dcbSerialParams.fAbortOnError 	= TRUE;
	dcbSerialParams.ByteSize      	= 8;
	dcbSerialParams.Parity        	= NOPARITY;
	dcbSerialParams.StopBits      	= ONESTOPBIT;
	
	//dcbSerialParams.fDtrControl   	= DTR_CONTROL_ENABLE;
	//dcbSerialParams.fRtsControl   	= RTS_CONTROL_ENABLE;

	//Set the parameters and check for their proper application
	if(!SetCommState(hSerial, &dcbSerialParams)) {
		std::cerr << "Serial::ALERT: Could not set Serial Port parameters";
		return false;
	} 

	COMMTIMEOUTS times;
	times.ReadIntervalTimeout         = MAXDWORD;
	times.ReadTotalTimeoutMultiplier  = MAXDWORD;
	times.ReadTotalTimeoutConstant    = 15;
	times.WriteTotalTimeoutMultiplier = 0;
	times.WriteTotalTimeoutConstant   = 0;

	// I want ReadFile to return immediately when 1 or more bytes are in the queue
	if ( !SetCommTimeouts(hSerial, &times) ) {
		std::cerr << "Serial::ALERT: Could not set Serial Port timeout parameters";
		return false;
	}

	std::cout << " Serial::INFO: Connected to port: " << portName << "\n";
	std::cout << " Baud rate: " << BAUD_RATE << "\n";
	std::cout << " Waiting for controller board response ";
	std::cout.flush();

	
	//Flush any remaining characters in the buffers 
	PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
	
	for (unsigned int i=0; i<ARDUINO_WAIT_TIME; i+=ARDUINO_WAIT_TIME/5 ) {
		std::cout << " .";
		std::cout.flush();
		//Sleep(ARDUINO_WAIT_TIME/5);
		cncControl->waitActive(ARDUINO_WAIT_TIME/5);
		
		std::cout.flush();
	}
	std::cout << " Ready\n";
	this->connected = true;

	return true;
}
///////////////////////////////////////////////////////////////////
void Serial::disconnect(void) {
///////////////////////////////////////////////////////////////////
	if( connected ) {
		connected = false;
		isCommand = false;
		CloseHandle(hSerial);
	}
}
///////////////////////////////////////////////////////////////////
void Serial::purge(void) {
///////////////////////////////////////////////////////////////////
	//Flush any remaining characters in the buffers 
	PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
	Sleep(500);
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
int Serial::readData(void *buffer, unsigned int nbChar) {
///////////////////////////////////////////////////////////////////
	//Number of bytes we'll have read
	DWORD bytesRead;
	//Number of bytes we'll really ask to read
	unsigned int toRead;
	
	COMSTAT status;
	DWORD errors;

	//Use the ClearCommError function to get status info on the Serial port
	ClearCommError(this->hSerial, &errors, &status);

	//Check if there is something to read
	if( status.cbInQue > 0 ) {
		//Check if there is enough data to read the required number
		//of characters, if not we'll read only the available characters to prevent
		//locking of the application.
		if( status.cbInQue > nbChar ) {
			toRead = nbChar;
		} else {
			toRead = status.cbInQue;
		}

		//Try to read the require number of chars, and return the number of read bytes on success
		if( ReadFile(this->hSerial, buffer, toRead, &bytesRead, NULL) ) {
			return bytesRead;
		}
	}

	//If nothing has been read, or that an error was detected return 0
	return 0;
}
///////////////////////////////////////////////////////////////////
bool Serial::writeData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	DWORD bytesSend;

	COMSTAT status;
	DWORD errors;
		
	if( !WriteFile(this->hSerial, (void *)buffer, nbByte, &bytesSend, 0) ) {
		ClearCommError(this->hSerial, &errors, &status);
		return false;
	} 
	
	if ( nbByte != bytesSend ) {
		std::cerr << "Serial::writeData nbByte != bytesSend" << std::endl;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool Serial::isConnected() {
///////////////////////////////////////////////////////////////////
	return connected;
}
///////////////////////////////////////////////////////////////////
void Serial::fetchMessage(unsigned char * text, int maxBytes) {
///////////////////////////////////////////////////////////////////
	Sleep(200);
	int i = 0;
	for ( ; i<maxBytes; i++ ) {
		unsigned char b[1];
		if ( readData(b, 1) ) {
			if ( b[0] == MSG_CLOSE )
				break;
				
			text[i] = b[0];
		} else {
			break;
		}
	}

	text[i] = '\0';
}
///////////////////////////////////////////////////////////////////
void Serial::fetchMultiByteResult(unsigned char * text, int maxBytes) {
///////////////////////////////////////////////////////////////////
	Sleep(200);
	int bytes = readData(text, maxBytes);
	text[bytes] = '\0';
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
	int id;
	int nr;
	std::stringstream ss((char*)result);
	std::string s;

	switch ( cmd ) {
		case 'c': 
			if ( cncControl->hasControllerConfigControl() == true )
				cncControl->clearControllerConfigControl();
				
			while (getline(ss, s, '\n')) {
				wxString key;
				if ( (pos = s.find_first_of (':')) > 0 ) {
					id = atoi((s.substr(0,pos)).c_str());
					if ( s[0] == ' ' )
						key += "    ";
					key += ArduinoPIDs::getPIDLabel((int)id);
					
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
			
		case '?':
			if ( cncControl->hasControllerErrorControl() == true )
				cncControl->clearControllerErrorControl();
			
			while (getline(ss, s, '\n')) {
				if ( (pos = s.find_first_of (':')) > 0 ) {
					nr = atoi((s.substr(0,pos)).c_str());
					
					s = s.substr(pos+1, s.length()-1);
					if ( (pos = s.find_first_of (':')) > 0 ) {
						id = atoi((s.substr(0,pos)).c_str());
					
						if ( cncControl->hasControllerErrorControl() == true ) {
							cncControl->appendNumKeyValueToControllerErrorInfo(nr, id, ArduinoErrorCodes::getECLabel(id), s.substr(pos+1, s.length()-1).c_str() );
						} else {
							mutliByteStream << nr << ": " << ArduinoErrorCodes::getECLabel(id) /*<< "[" << id << "]"*/ << ": ";
							mutliByteStream << s.substr(pos+1, s.length()-1);
							mutliByteStream << "\n";
						}
					}
				}
			}
			break;
			
		case 'Q':
			if ( cncControl->hasControllerPinControl() == true )
				cncControl->clearControllerPinControl();
			
			while (getline(ss, s, '\n')) {
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
						cncControl->appendNumKeyValueToControllerErrorInfo((type == (int)'D' ? ArduinoDigitalPins::getPinLabel(pin) : ArduinoAnalogPins::getPinLabel(pin)),
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
int Serial::readDataUntilSizeAvailable(void *buffer, unsigned int nbByte, unsigned int maxDelay) {
///////////////////////////////////////////////////////////////////
	static const unsigned int maxBytes = 1024;
	unsigned char buf[maxBytes];
	
	unsigned char* p = (unsigned char*)buffer;
	
	unsigned int remainingBytes = nbByte;
	unsigned int bytesRead 		= 0;
	
	unsigned int cnt = 0;
	
	while ( remainingBytes > 0 ) {
		bytesRead = readData(buf, remainingBytes);
		if ( bytesRead > 0 ) {
			
			memcpy(p, &buf, bytesRead);
			p += bytesRead;
			remainingBytes -= bytesRead;
			bytesRead = 0;

		} else {
			Sleep(1);

			if ( ++cnt > maxDelay ) {
				std::cerr << "Serial::readDataUntilSizeAvailable Timout reached:" << std::endl;
				return 0;
			}
				
			if ( cnt%100 == 0 )
				cncControl->SerialCallback(0);
		}
	}
	
	return nbByte - remainingBytes;
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
	
	if ( traceInfo ) {
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
	
	if ( traceInfo ) {
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
	
	if ( traceInfo ) {
		cnc::spy.initializeResult();
		cnc::spy << "Send: '" << cmd[0] << "' [" << ArduinoCMDs::getCMDLabel(cmd[0]) << "][" << ArduinoPIDs::getPIDLabel((int)pid) << "][" << ntohl(value) << "]\n";
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
	
	if ( traceInfo ) {
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
		std::cout << "SERIAL::processCommand()::ERROR: Not connected\n";
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
	
	// Always log the start position
	cncControl->SerialCallback(0);
	
	char* p = (char*)cmd;
	for ( unsigned int i=0; i<strlen(cmd); i++ ) {
		
		if ( writeOnlyMoveCommands == true && isMoveCommand((unsigned char)cmd[i]) == false ) {
			p++; // important to have always the right index!
			continue;
		}

		if ( traceInfo ) {
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
bool Serial::processMove(unsigned int size, int32_t values[], bool alreadyRendered, CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false ) {
		std::cerr << "SERIAL::processMoveXY()::ERROR: Not connected\n";
		return false;
	}
	// Always log the start postion
	cncControl->SerialCallback(0);
	
	unsigned char* p = moveCommand;
	unsigned int idx = 0;

	if ( alreadyRendered  == true )	moveCommand[idx++] = 'm'; 
	else							moveCommand[idx++] = 'M'; 
	p++;
	
	for (unsigned int i=0; i<size; i++) {
		int32_t v  = htonl(values[i]);
		memcpy(p, &v, LONG_BUF_SIZE);
		idx += LONG_BUF_SIZE; 
		p   += LONG_BUF_SIZE;
	}
	
	if ( traceInfo ) {
		cnc::spy.initializeResult();
		cnc::spy << "Send: '" << moveCommand[0] << "' [" << ArduinoCMDs::getCMDLabel(moveCommand[0]) << "]\n";
	}
		
	if ( writeData(moveCommand, idx) ) {
		SerialFetchInfo sfi;
		sfi.command 		= moveCommand[0];
		sfi.retSOHAllowed 	= true;
		sfi.returnAfterSOH  = false;
		sfi.Mc.size 		= size;
		sfi.Mc.value1		= values[0];
		sfi.Mc.value2		= values[1];
		sfi.Mc.value3		= values[2];
		
		//std::cout << moveCommand[0] << "," << values[0] << "," << values[1] << "," << values[2] << std::endl;
		return evaluateResultWrapper(sfi, std::cout, pos);
	
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
	
	isCommand = true;
	
	bool ret = false;
	ret = evaluateResult(sfi, mutliByteStream, pos);
	if ( cncControl->isInterrupted() ) {
		sendInterrupt();
		ret = false;
	}
	
	isCommand = false;
	
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
				fetchMessage(sfi.multiByteResult, sizeof(sfi.multiByteResult)-1);

				ControllerMsgInfo cmi;
				decodeMessage(sfi.multiByteResult, cmi.message);
				cncControl->SerialMessageCallback(cmi);
				return evaluateResult(sfi, mutliByteStream, pos);
			}
			//evaluateResult..........................................
			default: {
				
				std::cerr << "Serial::evaluateResult: Invalid Acknowlege: \n" << sfi.command <<", as integer: " << (int)ret << "\n";
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
			if (traceInfo) 
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
			
			if (traceInfo)
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
			
			if (traceInfo) 
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
				
			if (traceInfo)
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
			fetchMultiByteResult(sfi.multiByteResult, sizeof(sfi.multiByteResult)-1);
			decodeMultiByteResults(sfi.command, sfi.multiByteResult, mutliByteStream);
			
			cncControl->SerialCallback(1);
			
			if (traceInfo)
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
		case PID_XYZ_POS:	return decodePositionInfo(sfi, cr);
		
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
	Sleep(sfi.retSOHSleep);

	if ( (sfi.Gc.bytes = readData(sfi.Gc.result, sizeof(sfi.Gc.result))) <= 0 ) {
		std::cerr << "ERROR while reading getter value(s). Nothing available" << std::endl;
		return false;
	}
	
	if ( sfi.Gc.bytes%4 != 0 ) {
		std::cerr << "ERROR while reading getter value(s). Result cant broken down to int32_t values. Byte count: " << sfi.Gc.bytes << std::endl;
		return false;
	}
	
	//fetch 1 to max N int32_t values
	sfi.Gc.p = sfi.Gc.result;
				
	for (int i=0; i<sfi.Gc.bytes; i+=LONG_BUF_SIZE) {
		memcpy(&sfi.Gc.value, sfi.Gc.p, LONG_BUF_SIZE);
		
		if ( sfi.Gc.list != NULL ) {
			sfi.Gc.list->push_back(sfi.Gc.value);
		} else {
			std::cerr << "Serial::RET_SOT_Handler: Invalid list" << std::endl;
		}
		
		sfi.Gc.p += LONG_BUF_SIZE;
	}
	
	if (traceInfo)
		cnc::spy.finalizeOK();
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool Serial::decodePositionInfo(SerialFetchInfo& sfi, unsigned char pid) {
///////////////////////////////////////////////////////////////////
	unsigned int size = LONG_BUF_SIZE;
	if ( pid == PID_XYZ_POS )
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
		
		if ( pid == PID_XYZ_POS ) {
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
	
	cncControl->SerialControllerCallback(ci);
	
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
	
	cncControl->SerialControllerCallback(ci);
	
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
	
	cncControl->SerialControllerCallback(ci);

	return true;
}
