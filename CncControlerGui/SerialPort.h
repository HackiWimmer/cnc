#ifndef SERIALCLASS_H_INCLUDED
#define SERIALCLASS_H_INCLUDED

#include <vector> 
#include <map>
#include <wx/string.h>
#include "OSD/CncTimeFunctions.h"
#include "OSD/SerialOSD.h"
#include "CncArduino.h"
#include "CncCommon.h"
#include "SvgUnitCalculator.h"
#include "CncPosition.h"

typedef std::map<int, int32_t> 					SetterMap;
typedef std::vector<unsigned char> 				PidList;
typedef std::vector<int32_t> 					GetterValues;
typedef std::map<unsigned char, GetterValues> 	GetterListValues;

class CncControl;

struct LastSerialResult {
	unsigned char cmd 				    = '\0';
	unsigned char ret				    = RET_NULL;
	
	void reset() { 
		cmd 				 			= '\0';
		ret								= RET_NULL;
	}
};

struct SerialFetchInfo {
	unsigned char command				= '\0';
	unsigned char multiByteResult[2048];

	unsigned int singleFetchTimeout 	= 2000;
	bool autoCallErrorInfo 				= false;

	struct H {
		unsigned char result[sizeof(int32_t) * 2];
		unsigned char* p 				= NULL;
		int bytes 						= -1;
		int32_t value					= 0;
	} Hc;
	
	struct G {
		GetterValues* list				= NULL;
		GetterListValues* map			= NULL;
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

	struct S {
		unsigned char result[sizeof(int32_t) * 4];
		unsigned char* p 				= NULL;
		unsigned char value 			= 0;
		int bytes 						= -1;
		unsigned int size				= -1;
	} Sc;
	
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
	
	int32_t heartbeatValue				= 0;
	bool limitState						= false;
	bool supportState					= false;
	unsigned char limitStateValue		= 0;
	unsigned char supportStateValue		= 0;
	
	unsigned char posType				= '\0';
	int32_t xCtrlPos					= 0;
	int32_t yCtrlPos					= 0;
	int32_t zCtrlPos					= 0;
	
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
		
		bool canIdle;
		
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
		LastSerialResult lastFetchResult;
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
		inline void decodeMessage(const int bytes, const unsigned char* mutliByteStream, std::ostream& message);
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
		inline bool RET_ERROR_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream, CncLongPosition& pos);
		inline bool RET_INTERRUPT_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream, CncLongPosition& pos);
		inline bool RET_HALT_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream, CncLongPosition& pos);
		inline bool RET_QUIT_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream, CncLongPosition& pos);
		inline bool RET_LIMIT_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream, CncLongPosition& pos);	
		// handle receiving binary data from controller
		inline bool RET_SOH_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream, CncLongPosition& pos);
		// decodes the give fetch result
		inline bool decodeGetter(SerialFetchInfo& sfi);
		inline bool decodeText(unsigned char pid, SerialFetchInfo& sfi, std::ostream& mutliByteStream);
		inline bool decodeHeartbeat(SerialFetchInfo& sfi);
		inline bool decodeLimitInfo(SerialFetchInfo& sfi);
		inline bool decodePositionInfo(unsigned char pid, SerialFetchInfo& sfi);
		
		const LastSerialResult& getLastFetchResult() const { return lastFetchResult; }
		
		void incTotalDistance(int32_t dx, int32_t dy, int32_t dz);
		void incTotalDistance(const CncLongPosition& pos, int32_t cx, int32_t cy, int32_t cz);
		void incTotalDistance(unsigned int size, const int32_t (&values)[3]);
		
		void resetTotalDistance() { totalDistance[0] = 0.0; totalDistance[1] = 0.0; totalDistance[2] = 0.0; totalDistance[3] = 0.0;}
		void adjustMeasurementRefTs();
		void logMeasurementRefTs(const CncLongPosition& pos);
		void logMeasurementLastTs();
				
		virtual void startMeasurementIntern() {}
		virtual void stopMeasurementIntern() {}
		
		bool sendSerialControllerCallback(ContollerInfo& ci);
		
		inline bool processMoveInternal(unsigned int size, const int32_t (&values)[3], unsigned char command, CncLongPosition& pos);
		
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
		// read all remaining bytes from serial to /dev/null
		virtual void clearRemainingBytes(bool trace=false);
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
		// signals
		bool sendSignal(const unsigned char cmd);
		// port writting
		bool processGetter(unsigned char pid, GetterValues& ret);
		bool processGetterList(PidList pidList, GetterListValues& ret);
		bool processSetter(unsigned char pid, int32_t value);
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
		bool processMoveUntilSignal(unsigned int size, const int32_t (&values)[3], CncLongPosition& pos);
		
		bool sendInterrupt() 		{ return sendSignal(SIG_INTERRUPPT);     }
		bool sendHalt() 			{ return sendSignal(SIG_HALT);           }
		bool sendPause() 			{ return sendSignal(SIG_PAUSE);          }
		bool sendResume() 			{ return sendSignal(SIG_RESUME);         }
		bool sendQuitMove()			{ return sendSignal(SIG_QUIT_MOVE);      }
		bool sendSoftwareReset() 	{ return sendSignal(SIG_SOFTWARE_RESET); }
		
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
		
		double getTotalDistanceX() { return totalDistance[0]; }
		double getTotalDistanceY() { return totalDistance[1]; }
		double getTotalDistanceZ() { return totalDistance[2]; }
		double getTotalDistance()  { return totalDistance[3]; }
		
		virtual void traceSpeedInformation() {}
};

#endif // SERIALCLASS_H_INCLUDED