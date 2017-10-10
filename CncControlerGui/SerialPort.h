#ifndef SERIALCLASS_H_INCLUDED
#define SERIALCLASS_H_INCLUDED

#define ARDUINO_WAIT_TIME 2000

#include <wx/string.h>
#include <windows.h>
#include <vector> 
#include <map>
#include "CncArduino.h"
#include "CncCommon.h"
#include "SvgUnitCalculator.h"
#include "CncPosition.h"

class CncControl;

struct SerialFetchInfo {
	unsigned char command				= '\0';
	unsigned char multiByteResult[2048];

	unsigned int singleFetchTimeout 	= 1000;
	unsigned int retSOTSleep			= 10;
	unsigned int retSOHSleep			= 10;
	bool retSOTAllowed					= false;
	bool returnAfterSOT					= true;
	bool retSOHAllowed					= false;
	bool returnAfterSOH					= true;
	bool autoCallErrorInfo 				= false;

	struct H {
		unsigned char result[sizeof(int32_t) * 2];
		unsigned char* p 				= NULL;
		int bytes 						= -1;
		int32_t value					= 0;
	} Hc;
	
	struct G {
		std::vector<int32_t>* list			= NULL;
		unsigned char result[64];
		unsigned char* p 				= NULL;
		int32_t value 					= 0;
		int bytes 						= -1;
	} Gc;
	
	struct M {
		unsigned char result[sizeof(int32_t) * 3];
		unsigned char* p 				= NULL;
		int32_t value 					= 0;
		int bytes 						= -1;
		unsigned int size				= -1;
		int32_t value1					= 0;
		int32_t value2					= 0;
		int32_t value3					= 0;
	} Mc;
	
	struct L {
		unsigned char result[sizeof(int32_t) * 3];
		unsigned char* p 				= NULL;
		int32_t value 					= 0;
		int bytes 						= -1;
		unsigned int size				= -1;
	} Lc;
	
	struct T {
		unsigned char result[sizeof(int32_t) * 1];
		unsigned char* p 				= NULL;
		int32_t value 					= 0;
		int bytes 						= -1;
		unsigned int size				= -1;
	} Tc;
};

enum ControllerInfoType {CITUnknown, CITHeartbeat, CITPosition, CITSetterInfo, CITLimitInfo};
struct ContollerInfo {
	ControllerInfoType infoType			= CITUnknown;
	unsigned char command				= '\0';
	
	int64_t heartbeatValue				= 0;
	
	CncLongPosition controllerPos 		= {0,0,0};
	
	int setterId						= 0;
	int32_t	setterValue					= 0L;
	
	bool isPause						= false;
	
	int32_t xLimit						= 0L;
	int32_t yLimit						= 0L;
	int32_t zLimit						= 0L;
};

struct ControllerMsgInfo {
	std::stringstream message;
};

struct SvgOutputParameters  {
	double 				zoomFactor 			= 1.0;
	bool 				copyOrigPath 		= true;
	bool 				onlyFirstCrossing 	= true;
};

struct SvgOriginalPathInfo {
	wxString pathData					= "";
	wxString transformInfo				= "";
	wxString useTransformInfo			= "";
};

class Serial {
	public:
		// define long size
		static const unsigned int LONG_BUF_SIZE = sizeof(int32_t);
		
	protected:
		//Serial com handler
		HANDLE hSerial;
		//cnc control object
		CncControl* cncControl;
		//Connection status
		bool connected;
		// for com porst this should always false
		bool writeOnlyMoveCommands;
		// flag to handle pause intervals
		bool isPause;
		// flag if the comand evaluation routine currently runs
		bool isCommand;
		//Get various information about the connection
		COMSTAT status;
		//Keep track of last error
		DWORD errors;
		// output flag
		bool traceInfo;
		// Port name
		std::string portName;
		// last fetch result
		unsigned char lastFetchResult;
		// Map of valid processed setters
		//std::map<int, int32_t> setterMap;
		
		unsigned char buf[LONG_BUF_SIZE];
		unsigned char moveCommand[MAX_MOVE_CMD_SIZE];
		
		//Fetch remaing bytes for message results
		inline void fetchMessage(unsigned char * text, int maxBytes);
		//Fetch remaing bytes for multi byte results  
		inline void fetchMultiByteResult(unsigned char * text, int maxBytes);
		//determine OS error message	
		inline void displayErrorInfo(LPTSTR lpszFunction);
		// decodes the given controler msg
		inline void decodeMessage(const unsigned char* message, std::ostream& mutliByteStream);
		// give the result a more human readabal format
		inline void decodeMultiByteResults(const char cmd, const unsigned char* result, std::ostream& mutliByteStream);
		// return true if the given cmd is a move command
		bool isMoveCommand(unsigned char cmd);
		// fetches the controler hand shake after sendData
		inline unsigned char fetchControllerResult(unsigned int maxDelay=1000);
		// reads data from controller until nbBytes are ceived 0r maxDelay reached
		inline int readDataUntilSizeAvailable(void *buffer, unsigned int nbByte, unsigned int maxDelay = 1000);
		// main handler for controller results
		inline bool evaluateResultWrapper(SerialFetchInfo& sfi, std::ostream& mutliByteStream, CncLongPosition& pos);
		inline bool evaluateResult(SerialFetchInfo& sfi, std::ostream& mutliByteStream, CncLongPosition& pos);
		// sends an interrupt command to the controller
		bool sendInterrupt();
		// handle receiving data from controller
		inline bool RET_OK_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream, CncLongPosition& pos);
		// handle receiving text data from controller
		inline bool RET_SOT_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream, CncLongPosition& pos);
		// handle receiving binary data from controller
		inline bool RET_SOH_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream, CncLongPosition& pos);
		// decodes the given fetch result depending cr
		inline bool decode_RET_SOH_Default(unsigned char cr, SerialFetchInfo& sfi);
		// decodes the give fetch result
		inline bool decodeGetter(SerialFetchInfo& sfi);
		inline bool decodeHeartbeat(SerialFetchInfo& sfi);
		inline bool decodeLimitInfo(SerialFetchInfo& sfi);
		inline bool decodePositionInfo(SerialFetchInfo& sfi);
		
		void resetLastFetchResult() { lastFetchResult = RET_NULL; }
		void setLastFetchType(unsigned char ret) { lastFetchResult = ret; }
		const unsigned char getLastFetchResult() const { return lastFetchResult; }
		
	public:
		//Initialize Serial communication without an acitiv connection 
		Serial(CncControl* cnc);
		//Initialize Serial communication with the given COM port
		Serial(const char *portName);
		//Close the connection
		virtual ~Serial();
		// gets a string represntation of ret
		static const char* decodeContollerResult(int ret);
		// returns the class name
		virtual const char* getClassName() { return "SerialPort"; } 
		// returns the emulator type
		virtual bool isEmulator() const { return false; }
		// return the port type
		virtual const CncPortType getPortType() const { return CncPORT; }
		//connect to port
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
		//Check if we are actually connected
		bool isConnected();
		// returns the port name
		virtual const char* getPortName() { return portName.c_str(); }
		// sets info debug mode
		void enableSpyOutput(bool show=true) {traceInfo = show;}
		bool isSpyOutput() {return traceInfo; }
		// return the current pause flag
		bool isPauseActive() { return isPause; }
		// reurn the current command flag
		bool isCommandActive() { return isCommand; }
		// port writting
		virtual bool processGetter(unsigned char pid, std::vector<int32_t>& ret);
		bool processSetter(unsigned char pid, int32_t value);
		
		bool processTest(int32_t testId);
		bool processIdle();
		
		bool processCommand(const unsigned char cmd, std::ostream& mutliByteStream, CncLongPosition& pos);
		bool processCommand(const char* cmd, std::ostream& mutliByteStream, CncLongPosition& pos);
		
		bool processMoveXYZ(int32_t x1, int32_t y1, int32_t z1, bool alreadyRendered, CncLongPosition& pos);
		bool processMoveXY(int32_t x1, int32_t y1, bool alreadyRendered, CncLongPosition& pos);
		bool processMoveZ(int32_t z1, bool alreadyRendered, CncLongPosition& pos);
		bool processMove(unsigned int size, int32_t values[], bool alreadyRendered, CncLongPosition& pos);
		
		//SVG path handling
		virtual void setSVGOutputParameters(const SvgOutputParameters& sp) {}
		virtual void beginSVG(SVGUnit u, double width, double heigth, const wxString& viewBox = "" ) {}
		virtual void beginDuration(unsigned int count) {}
		virtual void beginPath(double xStartPos, double yStartPos) {}
		virtual void beginSubPath(double xPos, double yPos) {}
		virtual void closePath() {}
		virtual void closeDuration() {}
		virtual void closeSVG() {}
		virtual void closeSVG(CncDoublePosition::Watermarks& wm) {}
		virtual void clearSVG() {}
		virtual void rebuildSVG() {}
		virtual void writeOrigPath(const SvgOriginalPathInfo& sopi) {}
		
		// sends the Test Suite end flag 't'
		bool sendTestSuiteEndFlag();
		
};

#endif // SERIALCLASS_H_INCLUDED