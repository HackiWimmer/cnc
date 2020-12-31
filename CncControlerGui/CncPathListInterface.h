#ifndef CNC_PATH_LIST_MONITOR
#define CNC_PATH_LIST_MONITOR

#include <sstream>
#include "OSD/CncTimeFunctions.h"
#include "CncPosition.h"
#include "CncPathListRunner.h"

// -------------------------------------------------------------------------------------
class CncPathListMonitor : public CncPathListRunner::Interface {
	
	protected:
		
		struct Current {
			CncDoublePosition	monitorPos			= {0.0, 0.0, 0.0};
			bool				toolState			= false;
			long				clientId			= CLIENT_ID.INVALID;
			double				speedValue_MM_MIN	= 0.0;
			CncSpeedMode		speedMode			= CncSpeedUserDefined;
		};
		
		Current				current;
		CncNanoTimestamp	tsMeasurementStart;
		CncNanoTimestamp	tsMeasurementLast;
		
		virtual void notifyNextPostion()												{}
		
	public:
		
		CncPathListMonitor();
		virtual ~CncPathListMonitor();
		
		virtual CncLongPosition   getPositionSteps()  const								;
		virtual CncDoublePosition getPositionMetric() const								{ return current.monitorPos; }
		
		virtual void logMeasurementStart()												{ tsMeasurementStart = CncTimeFunctions::getNanoTimestamp(); }
		virtual void logMeasurementEnd()												{ tsMeasurementLast  = CncTimeFunctions::getNanoTimestamp(); }
		virtual bool isInterrupted()													{ return false; }
		
		virtual void publishGuidePath(const CncPathListManager& plm, double zOffset);
		
		virtual void processClientIDChange(long cid)									;
		virtual bool processSpeedChange(double value_MM_MIN, CncSpeedMode m)			;
		virtual bool processToolChange(double diameter)									{ return true; }
		virtual bool processToolSwitch(bool on)											{ current.toolState = on; return true; }
		virtual bool processMoveSequence(CncMoveSequence& msq)							;
		virtual bool processPathListEntry(const CncPathListEntry& ple)					;
		
		virtual void processTrigger(const Trigger::BeginRun& tr)						{}
		virtual void processTrigger(const Trigger::EndRun& tr)							{}
		virtual void processTrigger(const Trigger::NextPath& tr)						{}
		virtual void processTrigger(const Trigger::SpeedChange& tr)						{}
		virtual void processTrigger(const Trigger::GuidePath& tr)						{}

};

// -------------------------------------------------------------------------------------
class CncPathListFileStore : public CncPathListMonitor {
	
	private:
		
		std::stringstream	headerStream;
		std::stringstream	bodyStream;
		std::stringstream	footerStream;
		
		virtual void notifyNextPostion();
		
	public:
		CncPathListFileStore();
		virtual ~CncPathListFileStore();
		
		virtual void processTrigger(const Trigger::BeginRun& tr);
		virtual void processTrigger(const Trigger::EndRun& tr);
		
		bool save(const wxString& fileName);
};

#endif