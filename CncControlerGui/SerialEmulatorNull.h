#ifndef SERIAL_EMULATOR_CLASS
#define SERIAL_EMULATOR_CLASS

#include "SerialSpyPort.h"
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
	
	void resetMSG() {
		Msg.text.clear();
	}

	struct MSG {
		unsigned char type 		= 'I';
		wxString text			= "";
	} Msg;
	
};

class SerialEmulatorNULL : public SerialSpyPort
{
	private:
		int32_t posReplyThreshold;
		size_t positionCounter;
		size_t stepCounter;
		SetterMap setterMap;
		CncLongPosition curEmulatorPos;
		unsigned char lastSignal;
		
		inline bool writeMoveCmd(void *buffer, unsigned int nbByte);
		inline bool renderMove(int32_t dx , int32_t dy , int32_t dz, void *buffer, unsigned int nbByte);
		inline bool provideMove(int32_t dx , int32_t dy , int32_t dz, void *buffer, unsigned int nbByte, bool force=false);
		
		inline void reset();
		
	protected:
		LastCommand lastCommand;
		
		unsigned char getLastSignal() { return lastSignal; }
		
		const char* getConfiguration(wxString& ret);
		
		virtual bool writeSetter(void *buffer, unsigned int nbByte);
		virtual bool writeMoveCmd(int32_t x , int32_t y , int32_t z, void *buffer, unsigned int nbByte);
		
		virtual int readDefault(void *buffer, unsigned int nbByte, const char* response);
		virtual int readMessage(void *buffer, unsigned int nbByte, const char* response);
		virtual int readMove(void *buffer, unsigned int nbByte);

		virtual int getCurrentMoveCmdPID();
		virtual void getCurrentMoveCmdValues(int32_t &x, int32_t &y, int32_t &z);
		virtual bool evaluatePositions(std::vector<int32_t>& ret);
		virtual bool evaluateLimitStates(std::vector<int32_t>& ret);
	
	public:
	
		//Initialize Serial communication without an acitiv connection 
		SerialEmulatorNULL(CncControl* cnc);
		//Initialize Serial communication with the given COM port
		SerialEmulatorNULL(const char *portName);
		virtual ~SerialEmulatorNULL();
		
		// returns the class name
		virtual const char* getClassName() { return "SerialEmulatorNULL"; }
		// returns the emulator type
		virtual bool isEmulator() const { return true; }
		// return the port type
		virtual const CncPortType getPortType() const { return CncEMU_NULL; }
		// simulate connection
		virtual bool connect(const char* portName) { connected = true; return true; }
		// close the connection
		virtual void disconnect(void) { connected = false; }
		// process a getter call
		virtual bool processGetter(unsigned char pid, std::vector<int32_t>& ret);
		// simulate read
		virtual int readData(void *buffer, unsigned int nbByte);
		// simulate write
		virtual bool writeData(void *buffer, unsigned int nbByte);
		
		// position movement counting
		virtual void resetPostionCounter();
		virtual size_t getPostionCounter();

		virtual void resetStepCounter();
		virtual size_t getStepCounter();
};

#endif