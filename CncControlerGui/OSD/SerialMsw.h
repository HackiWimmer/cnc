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
				inline void displayErrorInfo(LPTSTR lpszFunction);
				
			protected:
				
				void setConnected(bool state) { connected = state; }
				
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
				
				//Read data in a buffer, if nbByte is greater than the
				//maximum number of bytes available, it will return only the
				//bytes available. The function return -1 when nothing could
				//be read, the number of bytes actually read.
				virtual int readData(void *buffer, unsigned int nbByte);
				//Writes data from a buffer through the Serial connection
				//return true on success.
				virtual bool writeData(void *buffer, unsigned int nbByte);
				
		};
		
	#endif

#endif

