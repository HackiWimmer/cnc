#ifndef SERIAL_GTK_H
#define SERIAL_GTK_H

	#ifdef __WXGTK__
		class SerialGtk {
			
			private:
				//Connection status
				bool connected;
			
			protected:
				
				void setConnected(bool state) { connected = state; }
				
				virtual int readData(void *buffer, unsigned int nbByte);
				virtual bool writeData(void *buffer, unsigned int nbByte);
				
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
		};
		
	#endif

#endif