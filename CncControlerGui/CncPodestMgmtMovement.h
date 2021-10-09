#ifndef CNCPODESTMGMTMOVEMENT_H
#define CNCPODESTMGMTMOVEMENT_H

#include "CncCommon.h"
#include "CncSecureGesturesPanel.h"
#include "wxCrafterPodestManagement.h"

class CncPodestMgmtMovement	: public CncPodestMgmtMovementBase
							, public CncSecureGesturesPanel::CallbackInterface
{
	public:
		
		class CallbackInterface {
		
			public:
				CallbackInterface() {}
				virtual ~CallbackInterface() {}
				
				virtual void podestNotifyEnable(bool state) {}
				virtual void podestNotifyInit(bool state) {}
				virtual void podestNotifyClose(bool state) {}
		};
		
		CncPodestMgmtMovement(wxWindow* parent);
		virtual ~CncPodestMgmtMovement();
		
		bool init();
		bool close();
		
		void setCallbackInterface(CallbackInterface* inf) { caller = inf; }
		
	protected:
	
		virtual void onLeftDownDistance(wxMouseEvent& event);
		virtual void onPodestRelativeDown(wxCommandEvent& event);
		virtual void onPodestRelativeUp(wxCommandEvent& event);
		
		virtual void notifyStarting(const CncSecureGesturesPanel::State s);
		virtual void notifyPositionChanged(const CncSecureGesturesPanel::Data& d);
		virtual void notifyPositionHeld(const CncSecureGesturesPanel::Data& d);
		
	private:
		CncLinearDirection			direction;
		CncSecureGesturesPanel*		interactiveMove;
		CallbackInterface*			caller;
		
		void reset();
		void process();
		
		void enable(bool state);
		
		double  evaluateMillimeterToMove();
		int32_t evaluateStepsToMove();
};

#endif // CNCPODESTMGMTMOVEMENT_H
