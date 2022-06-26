#ifndef CNC_PATH_LIST_MONITOR
#define CNC_PATH_LIST_MONITOR

#include <sstream>
#include "OSD/CncTimeFunctions.h"
#include "CncPosition.h"
#include "CncPathListRunner.h"

// -------------------------------------------------------------------------------------
class CncPathListMonitor : public CncPathListRunner::Interface 
{
	protected:
		
		struct Current 
		{
			CncDoublePosition		monitorPos			= {0.0, 0.0, 0.0};
			CncSpindlePowerState	spindleState		= SPINDLE_STATE_OFF;
			double					spindleSpeed		= 0.0;
			long					clientId			= CLIENT_ID.INVALID;
			double					speedValue_MM_MIN	= 0.0;
			CncSpeedMode			speedMode			= CncSpeedUserDefined;
		};
		
		Current				current;
		CncNanoTimestamp	tsMeasurementStart;
		CncNanoTimestamp	tsMeasurementLast;
		
		bool dispatchEventQueue();
		virtual void notifyNextPostion() {}
		
	public:
		
		CncPathListMonitor();
		virtual ~CncPathListMonitor();
		
		virtual bool spoolInstructions()														override { return true; }
		virtual void resetInstructions()														override {}
		
		virtual CncLongPosition		getCurrentPositionSteps()  const							override ;
		virtual CncDoublePosition	getCurrentPositionMetric() const							override  { return current.monitorPos; }
		virtual void				setCurrentPositionMetric(double px, double py, double pz)	override;
		virtual void				setCurrentPositionMetric(const CncDoublePosition& pos)		override;
		
		virtual void logMeasurementStart()														override { tsMeasurementStart = CncTimeFunctions::getNanoTimestamp(); }
		virtual void logMeasurementEnd()														override { tsMeasurementLast  = CncTimeFunctions::getNanoTimestamp(); }
		virtual bool isInterrupted()															override { return false; }
		
		virtual bool processGuidePath(const CncPathListManager& plm);
		virtual bool processClientIDChange(long cid)											override;
		virtual bool processFeedSpeedChange(double value_MM_MIN, CncSpeedMode m)				override;
		virtual bool processToolChange(double diameter)											override { return true; }
		virtual bool processSpindleStateSwitch(bool on, bool force=false)						override { current.spindleState = on; return true; }
		virtual bool processSpindleSpeedChange(double value_U_MIN)								override { current.spindleSpeed = value_U_MIN; return true; }
		virtual bool processMoveSequence(CncMoveSequence& msq)									override;
		virtual bool processPathListEntry(const CncPathListEntry& ple)							override;
		
		virtual void processTrigger(const Trigger::BeginRun& tr)								override {}
		virtual void processTrigger(const Trigger::EndRun& tr)									override {}
		virtual void processTrigger(const Trigger::NextPath& tr)								override {}
		virtual void processTrigger(const Trigger::SpeedChange& tr)								override {}
		virtual void processTrigger(const Trigger::GuidePath& tr)								override {}
};

// -------------------------------------------------------------------------------------
class CncPathListFileStore : public CncPathListMonitor {
	
	private:
		
		std::stringstream	headerStream;
		std::stringstream	bodyStream;
		std::stringstream	footerStream;
		const char			delimiter;
		
		virtual void notifyNextPostion()							override;
		
	public:
		CncPathListFileStore();
		virtual ~CncPathListFileStore();
		
		virtual void processTrigger(const Trigger::BeginRun& tr)	override;
		virtual void processTrigger(const Trigger::EndRun& tr)		override;
		
		bool save(const wxString& fileName);
};

#endif