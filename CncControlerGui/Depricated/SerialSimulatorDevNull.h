#ifndef SERIAL_SIMULATOR_NULL
#define SERIAL_SIMULATOR_NULL

#include "CncPosition.h"
#include "CncSpeedSimulator.h"
#include "SerialSimulatorBase.h"

class SerialSimulatorDevNull : public SerialSimulatorThread {
	
	public:
		SerialSimulatorDevNull(SerialSimulatorFacade* facade);
		virtual ~SerialSimulatorDevNull();
		
		virtual const char* getClassName() { return "SerialSimulatorDevNull"; }
		
	protected:
		typedef CncLongPosition LimitStates;
		CncLongPosition targetMajorPos;
		CncLongPosition curSimulatorPos;
		LimitStates curLimitStates;
		
		virtual void resetSerial();
		virtual void performLimitStates();
		virtual unsigned char performMove();
		virtual unsigned char performGetterValue(unsigned char pid);
		virtual unsigned char performSetterValue(unsigned char pid, int32_t value);
		
	private:
		CncSpeedSimulator* speedSimulator;
		int32_t posReplyThresholdX;
		int32_t posReplyThresholdY;
		int32_t posReplyThresholdZ;
		
		int32_t renderPointA[3];
		int32_t renderPointB[3];
		
		void performCurrentPositions(unsigned char pid);
		
		unsigned char decodeMove();
		bool renderAndStepAxisXYZ(int32_t dx, int32_t dy, int32_t dz);
		bool stepAxisXYZ(int32_t dx, int32_t dy, int32_t dz, bool force=false);
		bool stepAxis(char axis, int32_t steps);
};

#endif