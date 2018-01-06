#ifndef SERIAL_SIMUATOR
#define SERIAL_SIMUATOR

#include <map>
#include <vector>
#include <wx/thread.h>
#include <wx/datetime.h>
#include <boost/lockfree/spsc_queue.hpp>
#include "SerialData.h"

class SerialSimulatorFacade;

/////////////////////////////////////////////////////////////////////////
class SerialSimulatorThread : public wxThread {
	
	public:
		SerialSimulatorThread(SerialSimulatorFacade* facade);
		virtual ~SerialSimulatorThread();
		
		const wxDateTime& getLastLog() const { return tsLastLog; }
		unsigned int readAvailable() const;
		unsigned char readBufferFrontByte() const;
		
		void purgeReadQueue();
		
		int readData(void *buffer, unsigned int nbByte);
		bool writeData(void *buffer, unsigned int nbByte);
		
		virtual const char* getClassName() = 0;
		
	protected:
		
		std::stringstream logStream;
		
		void publishLogStreamAsInfoMsg();
		void publishLogStreamAsWarningMsg();
		void publishLogStreamAsErrorMsg();
		void appendLogStreamToErrorInfo(unsigned char eid = E_PURE_TEXT_VALUE_ERROR);
		
		struct ErrorInfo {
			unsigned int id			= 0;
			wxString additionalInfo	= _("");
		};
		
		typedef std::vector<ErrorInfo> ErrorList;
		
		static const unsigned long queueSize 	= 1024;// * 1024;
		typedef boost::lockfree::spsc_queue<SerialData, boost::lockfree::capacity<queueSize> > WriteDataQueue;
		typedef boost::lockfree::spsc_queue<SerialByte, boost::lockfree::capacity<queueSize> > ReadDataQueue;
		typedef std::map<unsigned char, int32_t> SetterMap;
		
		void releaseCondition();
		
		// common helpers
		void addErrorInfo(unsigned int id, const char* info = NULL);
		void resetLastSignal() { lastSignal = CMD_INVALID; }
		
		void incPosistionCounter();
		void incStepCounterX(int32_t dx);
		void incStepCounterY(int32_t dy);
		void incStepCounterZ(int32_t dz);
		
		void resetPositionCounter();
		void resetStepCounter();
		
		int32_t getPositionCounter() 	{ return positionCounter; }
		int32_t getStepCounterX() 		{ return stepCounterX; }
		int32_t getStepCounterY() 		{ return stepCounterY; }
		int32_t getStepCounterZ() 		{ return stepCounterZ; }
		
		int32_t getPositionOverflowCounter() 	{ return positionOverflowCounter; }
		int32_t getStepOverflowCounterX() 		{ return stepOverflowCounterX; }
		int32_t getStepOverflowCounterY() 		{ return stepOverflowCounterY; }
		int32_t getStepOverflowCounterZ() 		{ return stepOverflowCounterZ; }
		
		bool isProbeMode() { return probeMode; }
		bool getSetterValueAsLong(unsigned char pid, int32_t& value, int32_t defaultValue = 0);
		bool getSetterValueAsDouble(unsigned char pid, double& value, double defaultValue = 0.0);

		int32_t getSetterValueAsLong(unsigned char pid, int32_t defaultValue = 0);
		double getSetterValueAsDouble(unsigned char pid, double defaultValue = 0.0);
		unsigned char getLastSignal() { return lastSignal; }
				
		// serial reading
		unsigned int Serial_available();
		unsigned char Serial_read();
		unsigned int Serial_readBytes(unsigned char* ret, unsigned int size);
		
		// serial writing
		void Serial_write(const unsigned char b);
		void Serial_write(const char* response);
		
		void Serial_writeLongValue(int32_t val);
		
		void Serial_writeGetterValues(unsigned char pid, int32_t val);
		void Serial_writeGetterValues(unsigned char pid, int32_t val1, int32_t val2);
		void Serial_writeGetterValues(unsigned char pid, int32_t val1, int32_t val2, int32_t val3);
		
		void Serial_writeLongValues(unsigned char pid, int32_t val);
		void Serial_writeLongValues(unsigned char pid, int32_t val1, int32_t val2);
		void Serial_writeLongValues(unsigned char pid, int32_t val1, int32_t val2, int32_t val3);
		
		// interface for inherited classes
		virtual void resetSerial()								= 0;
		virtual void performLimitStates()						= 0;
		virtual unsigned char performMove()						= 0;
		
		virtual unsigned char performGetterValue(unsigned char pid);
		virtual unsigned char performSetterValue(unsigned char pid, int32_t value);
		
	private:
		bool fatalErrorState;
		
		SerialSimulatorFacade* caller;
		wxCondition* callerCondition;
		wxMutex*     callerMutex;
		
		wxDateTime tsLastLog;
		
		int32_t positionCounter;
		int32_t stepCounterX;
		int32_t stepCounterY;
		int32_t stepCounterZ;
		
		int32_t positionOverflowCounter;
		int32_t stepOverflowCounterX;
		int32_t stepOverflowCounterY;
		int32_t stepOverflowCounterZ;
		
		WriteDataQueue writeDataQueue;
		ReadDataQueue readDataQueue;
		
		CncNanoTimestamp errorInfoResponseId;
		ErrorList errorList;
		SetterMap setterMap;
		
		SerialData serialData;
		SerialDataReader byteReader;
		SerialDataWriter byteWriter;
		
		bool probeMode;
		unsigned char lastSignal;
		
		virtual wxThread::ExitCode Entry();
		
		void pushAndReleaseBytes();
		
		void processNextCommand();
		
		void performResetController();
		void performResetErrorInfo();
		
		void performVersionInfo();
		void performConfiguration();
		void performPinReport();
		void performErrorInfo();
		void performLastErrorInfoResponseId();
		
		void performHeartbeat();
		
		void performMessage(unsigned char type, const char* msg);

		unsigned char performSetterValueIntern();
		unsigned char performGetterValueIntern();
		unsigned char performGetterValueDefault(unsigned char pid);
		
		void performNextErrorInfoResponseId();

};

#endif