#ifndef SERIAL_GTK_H
#define SERIAL_GTK_H

	#ifdef __WXGTK__
		class SerialGtk {
			
			private:
				//Connection status
				bool connected;
			
			protected:
				
				void setConnected(bool state) { connected = state; }
				
			public:
				SerialGtk();
				virtual ~SerialGtk();
				
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