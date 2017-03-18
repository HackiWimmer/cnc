#ifndef SERIAL_EMULATOR_CLASS
#define SERIAL_EMULATOR_CLASS

#include "SerialPort.h"
#include "CncArduino.h"

struct LastCommand {
	unsigned char cmd 		= '\0';
	unsigned int index 		= 0;
	
	void restLastCmd() {
		cmd 				= '\0';
		index 				= 0;
	}
	
	void resetM() {
		Mc.index 			= 0;
		Mc.lastMoveX 		= 0;
		Mc.lastMoveY 		= 0;
		Mc.lastMoveZ 		= 0; 
		Mc.respondCount 	= 1;
		Mc.respondCounter 	= 0;
	}
	
	struct M {
		unsigned int index 			= 0;
		unsigned int respondCount	= 1;
		unsigned int respondCounter	= 0;
		bool lastLimitState 		= false;
		int32_t lastMoveX 			= 0;
		int32_t lastMoveY 			= 0;
		int32_t lastMoveZ 			= 0;
	} Mc;
	
};

class SerialEmulatorNULL : public Serial
{
	private:
		bool writeMoveCmd(void *buffer, unsigned int nbByte);;
	
	protected:
		LastCommand lastCommand;
		
		virtual bool writeSetter(void *buffer, unsigned int nbByte);
		virtual bool writeMoveCmd(int32_t x , int32_t y , int32_t z, void *buffer, unsigned int nbByte);
		
		virtual int readDefault(void *buffer, unsigned int nbByte, const char* response);
		virtual int readMove(void *buffer, unsigned int nbByte);

		virtual int getCurrentMoveCmdPID();
		virtual void getCurrentMoveCmdValues(int32_t &x, int32_t &y, int32_t &z);
		virtual bool evaluatePositions(std::vector<int32_t>& ret);
		virtual bool evaluateLimitStates(std::vector<int32_t>& ret);
	
	public:
	
		//Initialize Serial communication without an acitiv connection 
		SerialEmulatorNULL(CncControl* cnc)
		: Serial(cnc)
		{}
		//Initialize Serial communication with the given COM port
		SerialEmulatorNULL(const char *portName): Serial(portName) {
		}
		// returns the class name
		virtual const char* getClassName() { return "SerialEmulatorNULL"; }
		//Simulate connection
		virtual bool connect(const char* portName) {
			connected = true;
			return true;
		}
		//Close the connection
		virtual void disconnect(void) {
			connected = false;
		}
		virtual bool processGetter(unsigned char pid, std::vector<int32_t>& ret);
		// simulate read
		virtual int readData(void *buffer, unsigned int nbByte);
		// simulate write
		virtual bool writeData(void *buffer, unsigned int nbByte);
};

#endif