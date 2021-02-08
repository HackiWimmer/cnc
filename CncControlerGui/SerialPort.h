#ifndef SERIALCLASS_H_INCLUDED
#define SERIALCLASS_H_INCLUDED

#include <vector> 
#include <map>
#include <queue> 
#include <wx/string.h>
#include "OSD/CncTimeFunctions.h"
#include "OSD/SerialOSD.h"
#include "CncArduino.h"
#include "CncCommon.h"
#include "CncMoveSequence.h"
#include "CncCommandDecoder.h"
#include "CncBinaryTemplateStreamer.h"
#include "SvgUnitCalculator.h"
#include "CncPosition.h"

//------------------------------------------------------------
typedef std::map<unsigned char, cnc::SetterValueList> 	SetterMap;
typedef std::vector<unsigned char> 						PidList;
typedef std::vector<int32_t> 							GetterValues;
typedef std::map<unsigned char, GetterValues> 			GetterListValues;

class CncControl;
class CncPathListManager;

//------------------------------------------------------------
class SerialCommandLocker {
	
	private:
		static unsigned char lockedCommand;
		
		bool locking;
		unsigned char command;
		
	public:
		SerialCommandLocker(unsigned char cmd) 
		: locking(false)
		, command(cmd)
		{}
		
		~SerialCommandLocker() {
			if ( locking == true )
				lockedCommand = CMD_INVALID;
		}
		
		bool lock(CncControl* cnc);
		unsigned char getCommand() const { return command; }
		
		static unsigned char getLockedCommand() { return lockedCommand; }
		static bool isCommandActive()			{ return lockedCommand != CMD_INVALID; }
};

//------------------------------------------------------------
struct LastSerialResult {
	unsigned char cmd 		= CMD_INVALID;
	unsigned char ret		= RET_NULL;
	unsigned char pid		= RET_NULL;
	
	unsigned int index		= 0;
	unsigned int portion	= 0;
	
	void init(unsigned char command, unsigned int portionCount = 0) {
		cmd 	= command;
		portion = portionCount;
		resetResult();
	} 
	
	void resetAll() { 
		cmd = CMD_INVALID;
		resetResult();
	}
	
	void resetResult() { 
		ret		= RET_NULL;
		pid		= RET_NULL;
		
		index	= 0;
	}
};

//------------------------------------------------------------
struct SerialFetchInfo {
	unsigned char command				= CMD_INVALID;
	unsigned char multiByteResult[2048];

	unsigned int fetchTimeout 	= 2000;
	bool fetchTimeoutErrorInfo 	= true;
	
	explicit SerialFetchInfo(unsigned char cmd) 
	: command(cmd)
	{}

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
	
	struct MS {
		unsigned char result[sizeof(int32_t) * 4];
		unsigned char* p 				= NULL;
		int32_t value 					= 0;
		int bytes 						= -1;
		unsigned int size				= -1;
		int32_t value1					= 0;
		int32_t value2					= 0;
		int32_t value3					= 0;
	} Msc;

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

//------------------------------------------------------------
enum ControllerInfoType { CITDefault, CITHandshake, CITHeartbeat, CITPosition, CITLimit };
struct ContollerInfo {
	ControllerInfoType infoType			= CITDefault;
	unsigned char command				= '\0';
	
	unsigned char handshake				= '\0';
	
	int32_t heartbeatValue				= 0;
	bool limitState						= false;
	bool supportState					= false;
	bool healtyState					= false;
	unsigned char limitStateValue		= 0;
	unsigned char supportStateValue		= 0;
	unsigned char healtyStateValue		= 0;
	
	unsigned char posType				= '\0';
	bool synchronizeAppPos				= false;
	int32_t xCtrlPos					= 0;
	int32_t yCtrlPos					= 0;
	int32_t zCtrlPos					= 0;
	
	double feedSpeed					= MIN_LONG;
	
	int32_t xLimit						= 0L;
	int32_t yLimit						= 0L;
	int32_t zLimit						= 0L;
	
	bool hasSpeedInformation() const { return feedSpeed > 0.0; }
};

//------------------------------------------------------------
enum ControllerExecuteInfoType { CEITUnknown, CEITSetter };
struct ContollerExecuteInfo {

	ControllerExecuteInfoType infoType	= CEITUnknown;
	unsigned char setterPid				= PID_UNKNOWN;
	cnc::SetterValueList setterValueList;
};

//------------------------------------------------------------
struct ControllerMsgInfo {
	std::stringstream message;
};

//------------------------------------------------------------
struct SvgOutputParameters  {
	double 				zoomFactor 			= 1.0;
	bool 				copyOrigPath 		= true;
	bool 				onlyFirstCrossing 	= true;
};

class Serial : public SerialOSD {
	
	public:
		
		// SerialSyPort parameter
		enum SypMode { SM_NONE = 0, SM_READ = 1, SM_WRITE = 2, SM_ALL = 3 };
		
		// define long size
		static const unsigned int LONG_BUF_SIZE = sizeof(int32_t);
		
	private:
		
		// Readbuffer, primary filled by peekData
		typedef std::queue<unsigned char> ReadBuffer;
		ReadBuffer readBuffer; 
		
		// total distance
		size_t totalDistanceSteps[4];
		double totalDistanceMetric[4];
		double totalDistanceMetricRef;
		
		double measuredFeedSpeed_MM_SEC;
		
		CncLongPosition measurementRefPos;
		CncNanoTimestamp tsMeasurementStart;
		CncNanoTimestamp tsMeasurementRef;
		CncNanoTimestamp tsMeasurementLast;
		
		void clearReadBuffer();
		
	protected:
		
		class ControllerCallbackShouldSynchronizeAppPosition {
			private:
				Serial* serial;
				
			public:
				ControllerCallbackShouldSynchronizeAppPosition(Serial* s)
				: serial(s) 
				{ 
					if ( serial != NULL )
						serial->shouldCallbackSynchronizeAppPosition = true; 
				}
				
				~ ControllerCallbackShouldSynchronizeAppPosition() {
					if ( serial != NULL )
						serial->shouldCallbackSynchronizeAppPosition = false; 
				}
		};
		
		// 
		int readBufferedData(void *buffer, unsigned int nbByte);
		
		//cnc control object
		CncControl* cncControl;
		// Measurement status
		bool measurementActive;
		// for com porst this should always false
		bool writeOnlyMoveCommands;
		// Port name
		std::string portName;
		// last fetch result
		LastSerialResult lastFetchResult;
		// spy flags
		bool traceSpyInfo;
		Serial::SypMode spyMode;
		bool spyRead;
		bool spyWrite;
		bool shouldCallbackSynchronizeAppPosition;
		
		// display factors
		double factorX;
		double factorY;
		double factorZ;
		
		// ...
		unsigned char buf[LONG_BUF_SIZE];
		unsigned char moveCommand[MAX_MOVE_CMD_SIZE];
		
		virtual void waitDuringRead(unsigned int millis); 
		virtual void sleepMilliseconds(unsigned int millis);
		
		bool shouldCallbackAlsoSynchronizeAppPosition() { return shouldCallbackSynchronizeAppPosition; }
		
		// decodes the given controler msg
		inline void decodeMessage(const int bytes, const unsigned char* mutliByteStream, std::ostream& message);
		// give the result a more human readabal format
		inline void decodeMultiByteResults(const char cmd, const unsigned char* result, std::ostream& mutliByteStream);
		// return true if the given cmd is a move command
		bool isMoveCommand(unsigned char cmd);
		// fetches the controler hand shake after sendData
		inline unsigned char fetchControllerResult(SerialFetchInfo& sfi);
		// reads data from controller until nbBytes are ceived or maxDelay was reached
		inline int readDataUntilSizeAvailable(unsigned char *buffer, unsigned int nbByte, unsigned int maxDelay = 1000, bool withErrorMsg = true);
		// reads data from controller until a MBYTE_CLOSE wa received or maxDelay was reached
		inline int readDataUntilMultyByteClose(unsigned char *buffer, unsigned int nbByte);
		// main handler for controller results
		inline bool evaluateResultWrapper(SerialFetchInfo& sfi, std::ostream& mutliByteStream);
		inline bool evaluateResult(SerialFetchInfo& sfi, std::ostream& mutliByteStream);
		// handle receiving data from controller
		inline bool RET_MORE_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream);
		inline bool RET_OK_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream);
		inline bool RET_ERROR_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream);
		inline bool RET_INTERRUPT_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream);
		inline bool RET_HALT_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream);
		inline bool RET_QUIT_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream);
		inline bool RET_LIMIT_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream);
		// handle receiving binary data from controller
		inline bool RET_SOH_Handler(SerialFetchInfo& sfi, std::ostream& mutliByteStream);
		// decodes the give fetch result
		inline bool decodeGetter(SerialFetchInfo& sfi);
		inline bool decodeText(unsigned char pid, SerialFetchInfo& sfi, std::ostream& mutliByteStream);
		inline bool decodeHeartbeat(SerialFetchInfo& sfi);
		inline bool decodeLimitInfo(SerialFetchInfo& sfi);
		inline bool decodePositionInfo(unsigned char pid, SerialFetchInfo& sfi);
		
		void incTotalDistance(int32_t dx, int32_t dy, int32_t dz);
		void incTotalDistance(const CncLongPosition& pos, int32_t cx, int32_t cy, int32_t cz);
		void incTotalDistance(unsigned int size, const int32_t (&values)[3]);
		void resetTotalDistance(); 
		
		void logMeasurementRefTs(const CncLongPosition& pos);
		void logMeasurementLastTs();
		
		bool sendSerialControllerCallback(ContollerInfo& ci);
		bool sendSerialControllerCallback(ContollerExecuteInfo& cei);
		
		inline bool processMoveInternal(unsigned int size, const int32_t (&values)[3], unsigned char command);
		
		virtual void startMeasurementIntern() {}
		virtual void stopMeasurementIntern() {}
		
		// physically serialize interface
		virtual int  peekData(void *buffer, unsigned int nbByte);
		virtual int  readData(void *buffer, unsigned int nbByte);
		virtual bool writeData(unsigned char cmd);
		virtual bool writeData(void *buffer, unsigned int nbByte);
		
		virtual bool serializeSetter(SerialFetchInfo& sfi, const unsigned char* buffer, unsigned int nbByte);
		virtual bool serializeMove(SerialFetchInfo& sfi, const unsigned char* buffer, unsigned int nbByte);
		
		virtual bool writeMoveSequenceRawCallback(unsigned char* buffer, unsigned int nbByte) { return true; }
		
		friend class SerialCommandLocker;
		
	public:
	
		//Initialize Serial communication without an acitiv connection 
		Serial(CncControl* cnc);
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
		virtual bool clearRemainingBytes(bool trace=false);
		
		virtual bool dataAvailable();
		virtual const char* getPortName() 						const	{ return portName.c_str(); }
		virtual void onPeriodicallyAppEvent(bool interrupted)			{}
		virtual bool canProcessIdle() 									{ return true; }
		virtual bool isOutputAsTemplateAvailable()						{ return false; }
		virtual void setSpyMode(Serial::SypMode sm);

		void enableSpyOutput(bool show=true) 							{ traceSpyInfo = show;}
		bool isSpyOutputOn() 											{ return traceSpyInfo; }
		Serial::SypMode getSpyMode() 									{ return spyMode; };
		
		const LastSerialResult& getLastFetchResult() { return lastFetchResult; }
		
		// return the current command flag
		bool isCommandActive() 		{ return SerialCommandLocker::isCommandActive(); }
		bool isPopSerialActive()	{ return SerialCommandLocker::getLockedCommand() == CMD_POP_SERIAL; }
		bool isIdleActive()			{ return SerialCommandLocker::getLockedCommand() == CMD_IDLE; }
		
		// port writting
		bool popSerial();
		bool processIdle();
		
		bool processGetter(unsigned char pid, GetterValues& ret);
		bool processGetterList(PidList pidList, GetterListValues& ret);
		bool processSetter(unsigned char pid, int32_t value);
		bool processSetter(unsigned char pid, const cnc::SetterValueList& values);
		
		bool pushCommand(const unsigned char cmd);
		bool processCommand(const unsigned char cmd, std::ostream& mutliByteStream);
		
		bool processStartInteractiveMove();
		bool processUpdateInteractiveMove(const CncLinearDirection x, const CncLinearDirection y, const CncLinearDirection z);
		
		bool processMoveSequence(CncMoveSequence& moveSequence);
		
		bool resolveLimits(unsigned int size, const int32_t (&values)[3]);
		
		bool processMovePodest(int32_t steps);

		bool processMove(unsigned int size, const int32_t (&values)[3], bool alreadyRendered);
		bool processMoveXYZ(int32_t x1, int32_t y1, int32_t z1, bool alreadyRendered);
		bool processMoveXY(int32_t x1, int32_t y1, bool alreadyRendered);
		bool processMoveZ(int32_t z1, bool alreadyRendered);
		
		bool execute(const unsigned char* buffer, unsigned int nbByte);
		
		// signals
		bool sendSignal(const unsigned char cmd);
		
		bool sendInterrupt()		{ return sendSignal(SIG_INTERRUPPT);     }
		bool sendHalt()				{ return sendSignal(SIG_HALT);           }
		bool sendPause()			{ return sendSignal(SIG_PAUSE);          }
		bool sendResume()			{ return sendSignal(SIG_RESUME);         }
		bool sendQuitMove()			{ return sendSignal(SIG_QUIT_MOVE);      }
		bool sendSoftwareReset()	{ return sendSignal(SIG_SOFTWARE_RESET); }
		
		// trigger
		virtual void processTrigger(const Trigger::BeginRun& tr)	{}
		virtual void processTrigger(const Trigger::EndRun& tr)		{}
		virtual void processTrigger(const Trigger::NextPath& tr)	{}
		virtual void processTrigger(const Trigger::SpeedChange& tr)	{}
		virtual void processTrigger(const Trigger::GuidePath& tr)	{}
		
		size_t getTotalDistanceStepsX() { return totalDistanceSteps[0]; }
		size_t getTotalDistanceStepsY() { return totalDistanceSteps[1]; }
		size_t getTotalDistanceStepsZ() { return totalDistanceSteps[2]; }
		size_t getTotalDistanceSteps()  { return totalDistanceSteps[3]; }

		double getTotalDistanceMetricX() { return totalDistanceMetric[0]; }
		double getTotalDistanceMetricY() { return totalDistanceMetric[1]; }
		double getTotalDistanceMetricZ() { return totalDistanceMetric[2]; }
		double getTotalDistanceMetric()  { return totalDistanceMetric[3]; }
		
		void startMeasurement();
		void stopMeasurement();
		bool isMeasurementActive() const { return measurementActive; }
		CncNanoTimespan getMeasurementNanoTimeSpanTotal() const;
		CncNanoTimespan getMeasurementNanoTimeSpanLastRef() const;
		
		double getMeasuredFeedSpeed_MM_MIN() 		{ return measuredFeedSpeed_MM_SEC * 60; }
		double getMeasuredFeedSpeed_MM_SEC() 		{ return measuredFeedSpeed_MM_SEC; }

		virtual void notifySetupSuccesfullyFinsihed() {}

		// test behavior only
		virtual bool test();
};

#endif // SERIALCLASS_H_INCLUDED
