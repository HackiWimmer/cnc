#ifndef SERIAL_MSW_H
#define SERIAL_MSW_H

	#ifdef __WXMSW__
		
		#include "SerialSpyPort.h"
		#include <windef.h>
		
		class CncControl;
		
		class SerialMsw : public SerialSpyPort {
			
			private:
				//Serial com handler
				HANDLE hSerial;
				
				//determine OS error message
				inline void displayErrorInfo(DWORD lastError, LPCTSTR lpszFunction);
				
				int readDataNative(void *buffer, unsigned int nbByte);
				bool writeDataNative(void *buffer, unsigned int nbByte);
				
			protected:
				
				virtual int readData(void *buffer, unsigned int nbByte);
				virtual bool writeData(void *buffer, unsigned int nbByte);
				
			public:
				SerialMsw(CncControl* cnc);
				virtual ~SerialMsw();
				
				virtual bool connect(const char* portName);
				//Close the connection
				virtual void disconnect(void);
				//Flush any remaining characters in the serial buffers 
				virtual void purge(void);
		};
		
	#endif

#endif

