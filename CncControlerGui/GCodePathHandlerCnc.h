#ifndef GCODE_PATH_HANDLER_CNC_H
#define GCODE_PATH_HANDLER_CNC_H

#include "GCodePathHandlerBase.h"
#include "CncControl.h"
#include "CncPosition.h"

class GCodePathHandlerCnc : public GCodePathHandlerBase {
	
	public:
		GCodePathHandlerCnc(CncControl* cnc);
		virtual ~GCodePathHandlerCnc();
		
	protected:
		
		virtual bool processLinearMove(bool alreadyRendered);
		virtual bool changeWorkSpeedXY(CncSpeed s);
		virtual void prepareWorkImpl();
		virtual void finishWorkImpl();
		
	private:
		CncControl* cncControl;
		CncDoublePosition	previousPos;
};

#endif