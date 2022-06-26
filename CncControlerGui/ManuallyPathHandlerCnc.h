#ifndef MANUALLY_PATH_HANDLER_BASE_H
#define MANUALLY_PATH_HANDLER_BASE_H

//#include "CncArduino.h"
#include "PathHandlerBase.h"
#include "CncMoveDefinition.h"
#include "CncControl.h"
#include "CncPosition.h"

class ManuallyPathHandlerCnc : public PathHandlerBase 
{
	public:
		
		ManuallyPathHandlerCnc(CncControl* cnc);
		virtual ~ManuallyPathHandlerCnc();
		
		virtual const char*		getName() const { return "ManuallyPathHandlerCnc"; }
		
		virtual void 			logMeasurementStart();
		virtual void			logMeasurementEnd();
		
		virtual void			switchSpindleState(CncSpindlePowerState state);
		
		virtual bool			prepareWork(); 
		virtual bool			finishWork();
		
		virtual bool			processLinearMove(bool alreadyRendered);
		bool					processLinearMove(const CncMoveDefinition& md);
		
		void					swichtSpindleOn()  { switchSpindleState(SPINDLE_STATE_ON);  }
		void					swichtSpindleOff() { switchSpindleState(SPINDLE_STATE_OFF); }
		
	private:
		
		CncControl*				cncControl;
		CncSpeedMode			lastSpeedMode; 
		double					lastSpeedValue;
};

#endif