#ifdef __WXMSW__

#include <iostream>
#include "strsafe.h"
#include "CncArduino.h"
#include "CncConfig.h"
#include "MainFrame.h"
#include "SerialMsw.h"

#include <windows.h>

///////////////////////////////////////////////////////////////////
SerialMsw::SerialMsw() 
: connected(false)
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
	connected = false;
	
	//Try to connect to the given port throuh CreateFile
	hSerial = CreateFileA(portName,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	
	//Check if the connection was successfull
	if( hSerial==INVALID_HANDLE_VALUE )
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
		THE_APP->waitActive(ARDUINO_WAIT_TIME/5);
		
		std::cout.flush();
	}
	std::cout << " Ready\n";
	connected = true;
	
	return true;
}
///////////////////////////////////////////////////////////////////
void SerialMsw::disconnect(void) {
///////////////////////////////////////////////////////////////////
	if ( connected ) {
		connected = false;
		CloseHandle(hSerial);
	}
}
///////////////////////////////////////////////////////////////////
void SerialMsw::purge(void) {
///////////////////////////////////////////////////////////////////
	//Flush any remaining characters in the buffers 
	PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
	THE_APP->waitActive(500);
}
///////////////////////////////////////////////////////////////////
int SerialMsw::readData(void *buffer, unsigned int nbChar) {
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
bool SerialMsw::writeData(void *buffer, unsigned int nbByte) {
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
void SerialMsw::displayErrorInfo(LPTSTR lpszFunction) {
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


#endif