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
		
		virtual void			deligateTrigger(const Trigger::BeginRun& tr)				{ processTrigger(tr); }
		virtual void			deligateTrigger(const Trigger::EndRun& tr)					{ processTrigger(tr); }
		virtual void			changePathListRunnerInterface(const wxString& portName)		{ changePathListRunnerInterfaceImpl(portName); }
								
		virtual const char*		getName()						const	{ return "GCodePathHandlerCnc"; }
		virtual bool			isPathListUsed()				const	{ return true; }
		virtual void			initNextClientId(long id);
		virtual void			switchSpindleState(bool state);
		
	protected:
		
		virtual bool			processDwellIntern(int64_t microseconds);
		virtual bool			processLinearMove(bool alreadyRendered);
		virtual bool			changeCurrentFeedSpeedXYZ(CncSpeedMode s, double value = 0.0);
		virtual bool			changeCurrentSpindleSpeed(double value = 0.0);
		virtual bool			initNextPath();
		virtual bool			prepareWorkImpl();
		virtual bool			finishWorkImpl();
};

#endif