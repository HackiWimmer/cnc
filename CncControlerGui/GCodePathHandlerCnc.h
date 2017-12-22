#ifndef GCODE_PATH_HANDLER_CNC_H
#define GCODE_PATH_HANDLER_CNC_H

#include "GCodePathHandlerBase.h"
#include "CncControl.h"
#include "CncPosition.h"

class GCodePathHandlerCnc : public GCodePathHandlerBase {
	
	public:
		GCodePathHandlerCnc(CncControl* cnc);
		virtual ~GCodePathHandlerCnc();
		
		virtual void initNextClientId(long id);
		virtual void switchToolState(bool state);
		
		virtual void logMeasurementStart();
		virtual void logMeasurementEnd();
		
	protected:
		
		virtual bool processDwellIntern(int64_t microseconds);
		virtual bool processLinearMove(bool alreadyRendered);
		virtual bool changeCurrentFeedSpeedXYZ(CncSpeed s, double value = 0.0);
		virtual void prepareWorkImpl();
		virtual void finishWorkImpl();
		
	private:
		CncControl* cncControl;
		CncDoublePosition	previousPos;
};

#endif