#ifndef GCODE_PATH_HANDLER_CNC_H
#define GCODE_PATH_HANDLER_CNC_H

#include "GCodePathHandlerBase.h"
#include "CncPathListRunner.h"

class CncControl;

class GCodePathHandlerCnc : public GCodePathHandlerBase
                          , public CncPathListRunner
{
	public:
		GCodePathHandlerCnc(CncControl* cnc);
		virtual ~GCodePathHandlerCnc();
		
		virtual void			deligateTrigger(const Trigger::BeginRun& tr)				override { processTrigger(tr); }
		virtual void			deligateTrigger(const Trigger::EndRun& tr)					override { processTrigger(tr); }
		virtual void			changePathListRunnerInterface(const wxString& portName)		override { changePathListRunnerInterfaceImpl(portName); }
								
		virtual const char*		getName()						const						override { return "GCodePathHandlerCnc"; }
		virtual bool			isPathListUsed()				const						override { return true; }
		virtual void			initNextClientId(long id)									override;
		virtual void			switchSpindleState(CncSpindlePowerState state)				override;
		
	protected:
		
		virtual bool			processDwellIntern(int64_t microseconds)					override;
		virtual bool			processLinearMove(bool alreadyRendered)						override;
		virtual bool			changeCurrentFeedSpeedXYZ(CncSpeedMode s)					override;
		virtual bool			changeCurrentFeedSpeedXYZ(CncSpeedMode s, double value)		override;
		virtual bool			changeCurrentSpindleSpeed(double value = 0.0)				override;
		virtual bool			initNextPath()												override;
		virtual bool			prepareWorkImpl()											override;
		virtual bool			finishWorkImpl()											override;
		
		virtual void			resetWorkflow()												override { CncPathListRunner::resetWorkflow(); }
		virtual bool			spoolWorkflow()												override { return CncPathListRunner::spoolWorkflow(); }
};

#endif