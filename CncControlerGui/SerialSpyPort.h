#ifndef SERIAL_SPY_PORT_H
#define SERIAL_SPY_PORT_H

#include "SerialPort.h"

class SerialSpyPort : public Serial {
	
	protected:
	
		void spyReadData(int prevRet, void *buffer, unsigned int nbByte);
		void spyWriteData(void *buffer, unsigned int nbByte);
		
	public:
		SerialSpyPort(CncControl* cnc);
		SerialSpyPort(const char *portName);
		virtual ~SerialSpyPort();
		
		// returns the class name
		virtual const char* getClassName() { return "SerialSpyPort"; }
		// returns the emulator type
		virtual bool isEmulator() const { return false; }
		// return the port type
		virtual const CncPortType getPortType() const { return CncPORT; }
		
		virtual int readData(void *buffer, unsigned int nbByte);
		virtual bool writeData(void *buffer, unsigned int nbByte);
		
};


#endif
