#ifndef GCODE_PATH_HANDLER_BASE_H
#define GCODE_PATH_HANDLER_BASE_H

#include "PathHandlerBase.h"
#include "CncControl.h"
#include "CncPosition.h"
#include "GCodeBlock.h"

class GCodePathHandlerBase : public PathHandlerBase {
	
	public:
		GCodePathHandlerBase();
		virtual ~GCodePathHandlerBase();
		
		void prepareWork(); 
		void finishWork();
		
		bool processRapidLinearMove(GCodeBlock& gcb);
		bool processLinearMove(GCodeBlock& gcb);
		bool processArcMove(GCodeBlock& gcb, bool sweep);
		bool moveToOrigin(GCodeBlock& gcb);
		
		virtual void switchToolState(bool state) = 0;
		void swichtToolOn()  { switchToolState(true); }
		void swichtToolOff() { switchToolState(false); }
		
	protected:
		
		virtual bool processLinearMove(bool alreadyRendered) = 0;
		virtual bool changeCurrentFeedSpeedXYZ(CncSpeed s, double value = 0.0) = 0;
		virtual void prepareWorkImpl() = 0;
		virtual void finishWorkImpl() = 0;
		
	private:
		
		void updateCurrentPosition(GCodeBlock& gcb);
};

#endif