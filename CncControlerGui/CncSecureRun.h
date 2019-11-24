#ifndef SECURERUN_H
#define SECURERUN_H

#include "wxCrafterSecurityRun.h"
#include "CncMouseRemoteControl.h"

class CncSecureRun : public CncSecureRunBase {
	public:
		CncSecureRun(wxWindow* parent);
		virtual ~CncSecureRun();
		
		void enableControls(bool state);
		
	protected:
		
		virtual void onKeyDown(wxKeyEvent& event);
		virtual void changeView(wxCommandEvent& event);
		virtual void blinkTimer(wxTimerEvent& event);
		virtual void startupTimer(wxTimerEvent& event);
		
		virtual void initDialog(wxInitDialogEvent& event);
		
		virtual void show(wxShowEvent& event);
		
		virtual void play(wxCommandEvent& event);
		virtual void emergengy(wxCommandEvent& event);
		virtual void stop(wxCommandEvent& event);
		
		void onRemoteControl(CncMouseRemoteControlEvent& event);
		
	private:
		CncMouseRemoteControl*	remoteControl;
		bool       				isPause;
		bool       				headerFlag;
		
		void hideDialog(int retValue);
		void resetPerspectiveButtons();
};

#endif // SECURERUN_H
