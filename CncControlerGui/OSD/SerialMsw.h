#ifndef SERIAL_MSW_H
#define SERIAL_MSW_H

	#ifdef __WXMSW__
	
		#include <windef.h>
		
		class SerialMsw {
			
			private:
				//Serial com handler
				HANDLE hSerial;
				//Connection status
				bool connected;
				
				//determine OS error message
				inline void displayErrorInfo(DWORD lastError, LPCTSTR lpszFunction);
				
			protected:
				
				void setConnected(bool state) { connected = state; }
				
				virtual int readData(void *buffer, unsigned int nbByte);
				virtual bool writeData(void *buffer, unsigned int nbByte);
				
			public:
				SerialMsw();
				virtual ~SerialMsw();
				
				// get the connection state
				virtual bool isConnected() { return connected; }
				//Open the connection
				virtual bool connect(const char* portName);
				//Close the connection
				virtual void disconnect(void);
				//Flush any remaining characters in the serial buffers 
				virtual void purge(void);
		};
		
	#endif

#endif

