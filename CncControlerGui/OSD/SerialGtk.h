#ifndef SERIAL_GTK_H
#define SERIAL_GTK_H

	#ifdef __WXGTK__
	
		#include "SerialSpyPort.h"
		class CncControl;
		
		class SerialGtk : public SerialSpyPort {
			
			private:
			
			protected:
				
				virtual int  readData(void *buffer, unsigned int nbByte);
				virtual bool writeData(void *buffer, unsigned int nbByte);
				
			public:
				SerialGtk(CncControl* cnc);
				virtual ~SerialGtk();
				
				//Open the connection
				virtual bool connect(const char* portName);
				//Close the connection
				virtual void disconnect(void);
				//Flush any remaining characters in the serial buffers 
				virtual void purge(void);
		};
		
	#endif

#endif