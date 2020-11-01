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
		
		virtual const char* getName() { return "GCodePathHandlerCnc"; }
		
		virtual bool isPathListUsed() { return true; }
		virtual void initNextClientId(long id);
		virtual void switchToolState(bool state);
		
	protected:
		
		virtual bool processDwellIntern(int64_t microseconds);
		virtual bool processLinearMove(bool alreadyRendered);
		virtual bool changeCurrentFeedSpeedXYZ(CncSpeedMode s, double value = 0.0);
		virtual bool initNextPath();
		virtual void prepareWorkImpl();
		virtual void finishWorkImpl();
		
	private:
		
		CncLongPosition	phBegPosition;
	
};

#endif