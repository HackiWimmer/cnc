#ifdef __WXMSW__

#include <iostream>
#include "strsafe.h"
#include "CncArduino.h"
#include "CncCommon.h"
#include "CncConfig.h"
#include "MainFrameProxy.h"
#include "SerialMsw.h"

#include <windows.h>

///////////////////////////////////////////////////////////////////
void traceDCB(const DCB& dcbSerialParams) {
///////////////////////////////////////////////////////////////////

	std::cout << " dcbSerialParams.DCBlength:         " << dcbSerialParams.DCBlength << std::endl;
	std::cout << " dcbSerialParams.BaudRate:          " << dcbSerialParams.BaudRate << std::endl;
	std::cout << " dcbSerialParams.fBinary:           " << dcbSerialParams.fBinary << std::endl;
	std::cout << " dcbSerialParams.fParity:           " << dcbSerialParams.fParity << std::endl;
	std::cout << " dcbSerialParams.fOutxCtsFlow:      " << dcbSerialParams.fOutxCtsFlow << std::endl;
	std::cout << " dcbSerialParams.fOutxDsrFlow:      " << dcbSerialParams.fOutxDsrFlow << std::endl;
	std::cout << " dcbSerialParams.fDtrControl:       " << dcbSerialParams.fDtrControl << std::endl;
	std::cout << " dcbSerialParams.fDsrSensitivity:   " << dcbSerialParams.fDsrSensitivity << std::endl;
	std::cout << " dcbSerialParams.fTXContinueOnXoff: " << dcbSerialParams.fTXContinueOnXoff << std::endl;
	std::cout << " dcbSerialParams.fNull:             " << dcbSerialParams.fNull << std::endl;
	std::cout << " dcbSerialParams.fOutX:             " << dcbSerialParams.fOutX << std::endl;
	std::cout << " dcbSerialParams.fInX:              " << dcbSerialParams.fInX << std::endl;
	std::cout << " dcbSerialParams.fErrorChar:        " << dcbSerialParams.fInX << std::endl;
	std::cout << " dcbSerialParams.fRtsControl:       " << dcbSerialParams.fErrorChar << std::endl;
	std::cout << " dcbSerialParams.fAbortOnError:     " << dcbSerialParams.fAbortOnError << std::endl;
	std::cout << " dcbSerialParams.ByteSize:          " << (int)dcbSerialParams.ByteSize << std::endl;
	std::cout << " dcbSerialParams.Parity:            " << (int)dcbSerialParams.Parity << std::endl;
	std::cout << " dcbSerialParams.StopBits:          " << (int)dcbSerialParams.StopBits << std::endl;
	std::cout << " dcbSerialParams.XonLim:            " << (int)dcbSerialParams.XonLim << std::endl;
	std::cout << " dcbSerialParams.XoffLim:           " << (int)dcbSerialParams.XoffLim << std::endl;
 	std::cout << " dcbSerialParams.XonChar:           " << (int)dcbSerialParams.XonChar << std::endl;
	std::cout << " dcbSerialParams.XoffChar:          " << (int)dcbSerialParams.XoffChar << std::endl;
 	std::cout << " dcbSerialParams.ErrorChar:         " << (int)dcbSerialParams.ErrorChar << std::endl;
	std::cout << " dcbSerialParams.EofChar:           " << (int)dcbSerialParams.EofChar << std::endl;
	std::cout << " dcbSerialParams.EvtChar:           " << (int)dcbSerialParams.EvtChar << std::endl;
	std::cout << std::endl;
}

///////////////////////////////////////////////////////////////////
SerialMsw::SerialMsw(CncControl* cnc) 
: SerialSpyPort(cnc)
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
SerialMsw::~SerialMsw() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
bool SerialMsw::connect(const char* portName) {
///////////////////////////////////////////////////////////////////
	//We're not yet connected
	setConnected(false);
	
	//Try to connect to the given port throuh CreateFile
	hSerial = CreateFileA(portName,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	
	//Check if the connection was successful
	if( hSerial == INVALID_HANDLE_VALUE ) {
		DWORD lastError = GetLastError();
		if( lastError == ERROR_FILE_NOT_FOUND ){
			std::cerr << " Serial::ERROR: Handle was not attached. Reason: " << portName << " not available.\n";
		} else {
			std::cerr << " Serial::ERROR: Errno: " << lastError << "\n";
			displayErrorInfo(lastError, _T("CreateFileA"));
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
	
	if ( true ) {
		std::clog << "Received DCB parameters:" << std::endl;
		traceDCB(dcbSerialParams);
	}
	
	//Define serial connection parameters for the arduino board
	dcbSerialParams.BaudRate      	= BAUD_RATE;
	dcbSerialParams.fBinary       	= TRUE;
	dcbSerialParams.ByteSize      	= 8;
	dcbSerialParams.Parity        	= NOPARITY;
	dcbSerialParams.StopBits      	= ONESTOPBIT;
	dcbSerialParams.fOutxCtsFlow 	= FALSE;
	dcbSerialParams.fOutxDsrFlow  	= FALSE;
	dcbSerialParams.fDsrSensitivity = FALSE; // ??? should this be TRUE?
	dcbSerialParams.fNull         	= FALSE;
	dcbSerialParams.fOutX         	= FALSE;
	dcbSerialParams.fInX          	= FALSE;
	dcbSerialParams.fAbortOnError 	= TRUE;
	dcbSerialParams.fDtrControl   	= DTR_CONTROL_ENABLE;
	dcbSerialParams.fRtsControl   	= RTS_CONTROL_DISABLE;
	
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
	std::cout << " Baud rate: " << dcbSerialParams.BaudRate << "\n";
	std::cout << " Waiting for controller board response ";
	std::cout.flush();
	
	//Flush any remaining characters in the buffers 
	PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
	
	for (unsigned int i=0; i<ARDUINO_WAIT_TIME; i+=ARDUINO_WAIT_TIME/5 ) {
		std::cout << " .";
		std::cout.flush();
		//Sleep(ARDUINO_WAIT_TIME/5);
		APP_PROXY::waitActive(ARDUINO_WAIT_TIME/5);
		
		std::cout.flush();
	}
	std::cout << " Ready\n";
	SerialSpyPort::connect(portName);
	
	return true;
}
///////////////////////////////////////////////////////////////////
void SerialMsw::disconnect(void) {
///////////////////////////////////////////////////////////////////
	if ( isConnected() ) {
		setConnected(false);
		CloseHandle(hSerial);
	}
}
///////////////////////////////////////////////////////////////////
void SerialMsw::purge(void) {
///////////////////////////////////////////////////////////////////
	//Flush any remaining characters in the buffers 
	PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
	APP_PROXY::waitActive(500);
	
	SerialSpyPort::purge();
}
///////////////////////////////////////////////////////////////////
int SerialMsw::readData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	//Reads data in a buffer, if nbByte is greater than the
	//maximum number of bytes available, it will return only the
	//bytes available. The function return -1 when nothing could
	//be read, the number of bytes actually read.
	
	if ( nbByte == 0 )
		return 0;
		
	const int bytesRead   = readBufferedData(buffer, nbByte);
	const int bytesToRead = nbByte - bytesRead;
	
	return bytesToRead > 0 ? readDataNative(buffer, nbByte) : bytesRead;
}
///////////////////////////////////////////////////////////////////
bool SerialMsw::writeData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	//Writes data from a buffer through the Serial connection
	//return true on success.
	return writeDataNative(buffer, nbByte);
}
///////////////////////////////////////////////////////////////////
int SerialMsw::readDataNative(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	//Number of bytes we'll have read
	DWORD bytesRead 	= 0;
	//Number of bytes we'll really ask to read
	unsigned int toRead = 0;
	
	COMSTAT status;
	DWORD errors;
	
	//Use the ClearCommError function to get status info on the Serial port
	ClearCommError(this->hSerial, &errors, &status);
	
	//Check if there is something to read
	if( status.cbInQue > 0 ) {
		//Check if there is enough data to read the required number
		//of characters, if not we'll read only the available characters to prevent
		//locking of the application.
		if( status.cbInQue > nbByte ) 	toRead = nbByte;
		else							toRead = status.cbInQue;
	}
	
	//Try to read the require number of chars, and return the number of read bytes on success
	if( ReadFile(this->hSerial, buffer, toRead, &bytesRead, NULL) )
		return bytesRead;
	
	//If nothing has been read, or that an error was detected return 0
	return 0;
}
///////////////////////////////////////////////////////////////////
bool SerialMsw::writeDataNative(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	DWORD bytesSend;
	
	COMSTAT status;
	DWORD errors;
		
	if( !WriteFile(this->hSerial, buffer, nbByte, &bytesSend, 0) ) {
		std::cerr << "Serial::writeData(): WriteFile failed" << std::endl;
		ClearCommError(this->hSerial, &errors, &status);
		return false;
	} 
	
	if ( nbByte != bytesSend )
		std::cerr << "Serial::writeData(): nbByte != bytesSend" << std::endl;
	
	return true;
}
///////////////////////////////////////////////////////////////////
void SerialMsw::displayErrorInfo(DWORD lastError, LPCTSTR lpszFunction) {
///////////////////////////////////////////////////////////////////
	// Retrieve the system error message for the last-error code
	LPVOID lpMsgBuf;
	DWORD dw = lastError;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, 
		NULL
	);
		
	/*
	LPSTR   = char*
	LPCSTR  = const char*
	LPWSTR  = wchar_t*
	LPCWSTR = const wchar_t*
	LPTSTR  = char* or wchar_t* depending on _UNICODE
	LPCTSTR = const char* or const wchar_t* depending on _UNICODE
	*/
	std::cerr << " Error Message: " << wxString(static_cast<const wchar_t*>(lpMsgBuf)) << std::endl;
	LocalFree(lpMsgBuf);
}

#endif