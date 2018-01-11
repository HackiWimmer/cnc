#ifndef SERIALCLASS_H_INCLUDED
#define SERIALCLASS_H_INCLUDED

#include <vector> 
#include <map>
#include <wx/string.h>
#include "CncArduino.h"
#include "CncCommon.h"
#include "SvgUnitCalculator.h"
#include "OSD/CncTimeFunctions.h"
#include "CncPosition.h"
#include "SerialOSD.h"

typedef std::map<int, int32_t> SetterMap;

class CncControl;

struct SerialFetchInfo {
	unsigned char command				= '\0';
	unsigned char multiByteResult[2048];

	unsigned int singleFetchTimeout 	= 2000;
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
		std::vector<int32_t>* list		= NULL;
		unsigned char result[sizeof(int32_t)];
		unsigned int timeout 			= 1000;
		int32_t value 					= 0;
		int bytes 						= -1;
		int count 						= -1;
	} Gc;
	
	struct M {
		unsigned char result[sizeof(int32_t) * 4];
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
	
	unsigned char posType				= '\0';
	int32_t xCtrlPos					= 0L;
	int32_t yCtrlPos					= 0L;
	int32_t zCtrlPos					= 0L;
	
	double feedSpeed					= MIN_LONG;
	
	int setterId						= 0;
	int32_t	setterValue					= 0L;
	
	int32_t xLimit						= 0L;
	int32_t yLimit						= 0L;
	int32_t zLimit						= 0L;
	
	bool hasSpeedInformation() { return feedSpeed > 0.0; }
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

class Serial : public SerialOSD {
	public:
		
		// SerialSyPort parameter
		enum SypMode { SM_NONE = 0, SM_READ = 1, SM_WRITE = 2, SM_ALL = 3 };
		
		// define long size
		static const unsigned int LONG_BUF_SIZE = sizeof(int32_t);
		
	private:
		// total distance
		double totalDistance[4];
		double totalDistanceRef;
		double currentFeedSpeed;
		CncLongPosition measurementRefPos;
		CncNanoTimestamp tsMeasurementStart;
		CncNanoTimestamp tsMeasurementRef;
		CncNanoTimestamp tsMeasurementLast;
		
	protected:
		//cnc control object
		CncControl* cncControl;
		// Measurement status
		bool measurementActive;
		// for com porst this should always false
		bool writeOnlyMoveCommands;
		// flag if the comand evaluation routine currently runs
		bool isCommandRunning;
		// Port name
		std::string portName;
		// last fetch result
		unsigned char lastFetchResult;
		// spy flags
		bool traceSpyInfo;
		Serial::SypMode spyMode;
		bool spyRead;
		bool spyWrite;
		
		// display factors
		double factorX;
		double factorY;
		double factorZ;
		
		// ...
		unsigned char buf[LONG_BUF_SIZE];
		unsigned char moveCommand[MAX_MOVE_CMD_SIZE];
		
		virtual void waitDuringRead(unsigned int millis); 
		virtual void sleepMilliseconds(unsigned int millis);
		
		// decodes the given controler msg
		inline void decodeMessage(const unsigned char* message, std::ostream& mutliByteStream);
		// give the result a more human readabal format
		inline void decodeMultiByteResults(const char cmd, const unsigned char* result, std::ostream& mutliByteStream);
		// return true if the given cmd is a move command
		bool isMoveCommand(unsigned char cmd);
		// fetches the controler hand shake after sendData
		inline unsigned char fetchControllerResult(unsigned int maxDelay=1000);
		// reads data from controller until nbBytes are ceived or maxDelay was reached
		inline int readDataUntilSizeAvailable(unsigned char *buffer, unsigned int nbByte, unsigned int maxDelay = 1000, bool withErrorMsg = true);
		// reads data from controller until a MBYTE_CLOSE wa received or maxDelay was reached
		inline int readDataUntilMultyByteClose(unsigned char *buffer, unsigned int nbByte);
		// main handler for controller results
		inline bool evaluateResultWrapper(SerialFetchInfo& sfi, std::ostream& mutliByteStream, CncLongPosition& pos);
		inline bool evaluateResult(SerialFetchInfo& sfi, std::ostream& mutliByteStream, CncLongPosition& pos);
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
		inline bool decodePositionInfo(SerialFetchInfo& sfi, unsigned char pid);
		
		bool sendSignal(const unsigned char cmd);
		
		void resetLastFetchResult() { lastFetchResult = RET_NULL; }
		void setLastFetchType(unsigned char ret) { lastFetchResult = ret; }
		const unsigned char getLastFetchResult() const { return lastFetchResult; }
		
		void incTotalDistance(int32_t dx, int32_t dy, int32_t dz);
		void incTotalDistance(const CncLongPosition& pos, int32_t cx, int32_t cy, int32_t cz);
		void incTotalDistance(unsigned int size, const int32_t (&values)[3]);
		
		void resetTotalDistance() { totalDistance[0] = 0.0; totalDistance[1] = 0.0; totalDistance[2] = 0.0; totalDistance[3] = 0.0;}
		void adjustMeasurementRefTs();
		void logMeasurementRefTs(const CncLongPosition& pos);
		void logMeasurementLastTs();
				
		virtual void startMeasurementIntern() {}
		virtual void stopMeasurementIntern() {}
		
		bool sendSerialControllrCallback(ContollerInfo& ci);
		
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
		// will be released periodically be the main thread
		virtual void onPeriodicallyAppEvent(bool interrupted) {}
		// returns the port name
		virtual const char* getPortName() { return portName.c_str(); }
		// set spy mode
		void enableSpyOutput(bool show=true) { traceSpyInfo = show;}
		bool isSpyOutputOn() { return traceSpyInfo; }
		virtual void setSpyMode(Serial::SypMode sm);
		Serial::SypMode getSpyMode() { return spyMode; };
		// reurn the current command flag
		bool isCommandActive() { return isCommandRunning; }
		// port writting
		virtual bool processGetter(unsigned char pid, std::vector<int32_t>& ret);
		bool processSetter(unsigned char pid, int32_t value);
		
		bool processTest(int32_t testId);
		// indicates if idle message can be requested
		virtual bool canProcessIdle() { return true; }
		bool processIdle();
		
		bool processCommand(const unsigned char cmd, std::ostream& mutliByteStream, CncLongPosition& pos);
		bool processCommand(const char* cmd, std::ostream& mutliByteStream, CncLongPosition& pos);
		
		bool convertToMoveCommandAndProcess(unsigned char cmd, std::ostream& mutliByteStream, CncLongPosition& pos);
		
		bool processMoveXYZ(int32_t x1, int32_t y1, int32_t z1, bool alreadyRendered, CncLongPosition& pos);
		bool processMoveXY(int32_t x1, int32_t y1, bool alreadyRendered, CncLongPosition& pos);
		bool processMoveZ(int32_t z1, bool alreadyRendered, CncLongPosition& pos);
		bool processMove(unsigned int size, const int32_t (&values)[3], bool alreadyRendered, CncLongPosition& pos);
		
		bool sendInterrupt() 	{ return sendSignal(SIG_INTERRUPPT); }
		bool sendHalt() 		{ return sendSignal(SIG_HALT); }
		bool sendPause() 		{ return sendSignal(SIG_PAUSE); }
		bool sendResume() 		{ return sendSignal(SIG_RESUME); }
		
		// position movement counting
		virtual void resetPositionCounter();
		virtual size_t getPositionCounter();

		virtual void resetStepCounter();
		size_t requestStepCounter(unsigned char pid);
		virtual size_t getStepCounter();
		virtual size_t getStepCounterX();
		virtual size_t getStepCounterY();
		virtual size_t getStepCounterZ();
				
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
		
		void startMeasurement();
		void stopMeasurement();
		bool isMeasurementActive() const { return measurementActive; }
		CncNanoTimespan getMeasurementNanoTimeSpanTotal() const;
		CncNanoTimespan getMeasurementNanoTimeSpanLastRef() const;
		
		double getCurrentFeedSpeedAVG();
		double getCurrentFeedSpeed();
		double getTotalDistanceX() { return totalDistance[0]; }
		double getTotalDistanceY() { return totalDistance[1]; }
		double getTotalDistanceZ() { return totalDistance[2]; }
		double getTotalDistance()  { return totalDistance[3]; }
		
		virtual void traceSpeedInformation() {}
};

#endif // SERIALCLASS_H_INCLUDED