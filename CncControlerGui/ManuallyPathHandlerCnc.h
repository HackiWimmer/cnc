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
		
		virtual const char*		getName() const override { return "ManuallyPathHandlerCnc"; }
		
		virtual void 			logMeasurementStart()										override;
		virtual void			logMeasurementEnd()											override;
		
		virtual bool			prepareWork()												override;
		virtual bool			initToolChange(int id)										override;
		virtual bool			finishWork()												override;
		
		virtual bool			processLinearMove(bool alreadyRendered)						override;
		bool					processLinearMove(const CncMoveDefinition& md);
		
		void					switchSpindleState(CncSpindlePowerState state);
		
		void					swichtSpindleOn()  { switchSpindleState(SPINDLE_STATE_ON);  }
		void					swichtSpindleOff() { switchSpindleState(SPINDLE_STATE_OFF); }
		
	private:
		
		CncControl*				cncControl;
		CncSpeedMode			lastSpeedMode; 
		double					lastSpeedValue;
};

#endif