#ifndef GCODE_PATH_HANDLER_CNC_H
#define GCODE_PATH_HANDLER_CNC_H

#include "PathHandlerBase.h"
#include "CncControl.h"
#include "CncPosition.h"
#include "GCodeBlock.h"

class GCodePathHandlerCnc : public PathHandlerBase {
	
	public:
		GCodePathHandlerCnc(CncControl* cnc);
		virtual ~GCodePathHandlerCnc();
		
		void prepareWork(); 
		void finishWork();
		
		bool processRapidLinearMove(GCodeBlock& gcb);
		bool processLinearMove(GCodeBlock& gcb);
		bool processArcMove(GCodeBlock& gcb, bool sweep);
		bool moveToOrigin(GCodeBlock& gcb);
		
	protected:
		virtual bool processLinearMove(bool alreadyRendered);
		
	private:
		CncControl* 		cncControl;
		
		void updateCurrentPosition(GCodeBlock& gcb);
};

#endif