#ifndef CNC_PATH_LIST_INTERFACE_CNC
#define CNC_PATH_LIST_INTERFACE_CNC

#include "CncPosition.h"
#include "CncPathListRunner.h"

// -------------------------------------------------------------------------------------
class CncControl;
class CncPathListInterfaceCnc : public CncPathListRunner::Interface {
	
	private:
		
		CncControl* cnc;
	
	public:
		
		CncPathListInterfaceCnc(CncControl* cc);
		virtual ~CncPathListInterfaceCnc();
		
		virtual CncLongPosition   getPositionSteps()  const;
		virtual CncDoublePosition getPositionMetric() const;
		
		virtual void logMeasurementStart();
		virtual void logMeasurementEnd();
		virtual bool isInterrupted();
		
		virtual void publishGuidePath(const CncPathListManager& plm, double zOffset);
		
		virtual void processClientIDChange(long cid);
		virtual bool processFeedSpeedChange(double value_MM_MIN, CncSpeedMode m);
		virtual bool processToolChange(double diameter);
		virtual bool processSpindleStateSwitch(bool on, bool force=false);
		virtual bool processSpindleSpeedChange(double value_U_MIN);
		virtual bool processMoveSequence(CncMoveSequence& msq);
		virtual bool processPathListEntry(const CncPathListEntry& ple);
		
		virtual void processTrigger(const Trigger::BeginRun& tr);
		virtual void processTrigger(const Trigger::EndRun& tr);
		virtual void processTrigger(const Trigger::NextPath& tr);
		virtual void processTrigger(const Trigger::SpeedChange& tr);
		virtual void processTrigger(const Trigger::GuidePath& tr);
};

#endif