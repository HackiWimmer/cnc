#ifndef MANUALLY_PATH_HANDLER_BASE_H
#define MANUALLY_PATH_HANDLER_BASE_H

#include "PathHandlerBase.h"
#include "CncControl.h"
#include "CncPosition.h"

class ManuallyPathHandlerCnc : public PathHandlerBase {
	
	public:
		
		struct MoveDefinition {
			CncSpeed speedType 	= CncSpeedRapid;
			bool absoluteMove 	= false;
			bool toolState 		= false;
			bool correctLimit   = true;
			
			double f = 0.0;
			double x = 0.0;
			double y = 0.0;
			double z = 0.0;
		};
		
		ManuallyPathHandlerCnc(CncControl* cnc);
		virtual ~ManuallyPathHandlerCnc();
		
		virtual const char* getName() { return "ManuallyPathHandlerCnc"; }
		
		virtual void logMeasurementStart();
		virtual void logMeasurementEnd();
		
		virtual void switchToolState(bool state);
		
		virtual void prepareWork(); 
		virtual void finishWork();
		
		virtual bool processLinearMove(bool alreadyRendered);
		bool processLinearMove(const MoveDefinition& md);
		
		void swichtToolOn()  { switchToolState(true); }
		void swichtToolOff() { switchToolState(false); }
		
	private:
		CncControl* cncControl;
		
		CncSpeed lastSpeedType; 
		double lastSpeedValue;
};

#endif