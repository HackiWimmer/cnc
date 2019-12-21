#ifndef SERIAL_THERAD_STUB
#define SERIAL_THERAD_STUB

#include "SerialSpyPort.h"

class SerialCircularBuffer;
class SerialThreadStub : public SerialSpyPort {
	
	protected:
		
		inline bool isSerialThreadInstalled();
		inline bool isSerialThreadRunning();
		inline void resumeSerialThread();
		inline void pauseSerialThread();
		inline void wakeUpOnDemand();
		
		virtual void notifySetupSuccesfullyFinsihed();
		
	public:
		
		//Initialize Serial communication without an acitiv connection 
		SerialThreadStub(CncControl* cnc);
		//Initialize Serial communication with the given COM port
		SerialThreadStub(const char *portName);
		virtual ~SerialThreadStub();
		
		// returns the class name
		virtual const char* getClassName() { return "SerialThreadStub"; }
		// returns the emulator type
		virtual bool isEmulator() const { return true; }
		// return the port type
		virtual const CncPortType getPortType() const { return CncPORT_EMU_ARDUINO; }
		// indicates if idle message can be requested
		virtual bool canProcessIdle() { return false; }
		// will be released periodically be the main thread
		virtual void onPeriodicallyAppEvent(bool interrupted);
		// simulate connection
		virtual bool connect(const char* portName);
		// close the connection
		virtual void disconnect(void);
		// simulate read
		virtual int readData(void *buffer, unsigned int nbByte);
		// simulate write
		virtual bool writeData(void *buffer, unsigned int nbByte);
};

#endif
