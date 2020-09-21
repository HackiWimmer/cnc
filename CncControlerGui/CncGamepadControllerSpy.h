#ifndef CNCGAMEPADCONTROLLERSPY_H
#define CNCGAMEPADCONTROLLERSPY_H

#include "OSD/CncTimeFunctions.h"
#include "CncGamepadControllerState.h"
#include "wxCrafterGamePort.h"

class CncGamepadControllerSpy : public CncGamepadControllerSpyBase {
	
	public:
		CncGamepadControllerSpy(wxWindow* parent);
		virtual ~CncGamepadControllerSpy();
		
		void update(const GamepadEvent& state);
		
	protected:
		virtual void dclickLeftStick(wxMouseEvent& event);
		virtual void dclickNaviXY(wxMouseEvent& event);
		virtual void dclickNaviZ(wxMouseEvent& event);
		virtual void dclickRightStick(wxMouseEvent& event);
		virtual void selectSensitivity(wxCommandEvent& event);
		virtual void onContinuesTimer(wxTimerEvent& event);
		
	private:
		CncMilliTimestamp	tsLastUpdate;
		CncStepMode			lastStepMode;
		
		void decorateStepMode();
};

#endif // CNCGAMEPADCONTROLLERSPY_H
