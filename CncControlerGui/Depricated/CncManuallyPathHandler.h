#ifndef MANUALLY_PATH_HANDLER_BASE_H
#define MANUALLY_PATH_HANDLER_BASE_H

#include "PathHandlerBase.h"
#include "CncControl.h"
#include "CncPosition.h"

class ManuallyPathHandlerCnc : public PathHandlerBase {
	
	public:
		
		ManuallyPathHandlerCnc(CncControl* cnc);
		virtual ~ManuallyPathHandlerCnc();
		
		virtual void logMeasurementStart();
		virtual void logMeasurementEnd();
		
		/*
		void prepareWork(); 
		void finishWork();
		
		bool processRapidLinearMove(GCodeBlock& gcb);
		bool processLinearMove(GCodeBlock& gcb);
		bool processArcMove(GCodeBlock& gcb, bool sweep);
		bool processDwell(GCodeBlock& gcb);
		bool moveToOrigin(GCodeBlock& gcb);
		
		virtual void switchToolState(bool state) = 0;
		void swichtToolOn()  { switchToolState(true); }
		void swichtToolOff() { switchToolState(false); }
		*/
	protected:
		/*
		virtual bool processDwellIntern(int64_t microseconds) { return true; }
		virtual bool processLinearMove(bool alreadyRendered) = 0;
		virtual bool changeCurrentFeedSpeedXYZ(CncSpeed s, double value = 0.0) = 0;
		virtual void prepareWorkImpl() = 0;
		virtual void finishWorkImpl() = 0;
		*/
	private:
		CncControl* cncControl;
};

#endif