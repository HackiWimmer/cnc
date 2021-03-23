#ifndef MANUALLY_PATH_HANDLER_BASE_H
#define MANUALLY_PATH_HANDLER_BASE_H

#include "PathHandlerBase.h"
#include "CncControl.h"
#include "CncPosition.h"

class ManuallyPathHandlerCnc : public PathHandlerBase {
	
	public:
		
		struct MoveDefinition {
			
			CncSpeedMode speedMode	= CncSpeedMode::CncSpeedRapid;
			bool absoluteMove 		= false;
			bool toolState 			= false;
			bool correctLimit   	= true;
			
			double f = 0.0;
			double x = 0.0;
			double y = 0.0;
			double z = 0.0;
		};
		
		ManuallyPathHandlerCnc(CncControl* cnc);
		virtual ~ManuallyPathHandlerCnc();
		
		virtual const char*		getName() const { return "ManuallyPathHandlerCnc"; }
		
		virtual void 			logMeasurementStart();
		virtual void			logMeasurementEnd();
		
		virtual void			switchSpindleState(bool state);
		
		virtual bool			prepareWork(); 
		virtual bool			finishWork();
		
		virtual bool			processLinearMove(bool alreadyRendered);
		bool					processLinearMove(const MoveDefinition& md);
		
		void					swichtSpindleOn()  { switchSpindleState(true); }
		void					swichtSpindleOff() { switchSpindleState(false); }
		
	private:
		
		CncControl*				cncControl;
		CncSpeedMode			lastSpeedMode; 
		double					lastSpeedValue;
};

#endif