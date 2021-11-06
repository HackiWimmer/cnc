#ifndef CNCPODIUMMGMTMOVEMENT_H
#define CNCPODIUMMGMTMOVEMENT_H

#include "CncCommon.h"
#include "CncSecureGesturesPanel.h"
#include "wxCrafterPodiumManagement.h"

class CncPodiumMgmtMovement	: public CncPodiumMgmtMovementBase
{
	public:
		
		class CallbackInterface {
		
			public:
				CallbackInterface() {}
				virtual ~CallbackInterface() {}
				
				virtual void podiumNotifyEnable(bool state) {}
				virtual void podiumNotifyInit(bool state) {}
				virtual void podiumNotifyClose(bool state) {}
		};
		
		CncPodiumMgmtMovement(wxWindow* parent);
		virtual ~CncPodiumMgmtMovement();
		
		bool init();
		bool close();
		
		void setCallbackInterface(CallbackInterface* inf) { caller = inf; }
		
	protected:
	
		virtual void onLeftDownDistance(wxMouseEvent& event);
		virtual void onPodiumRelativeDown(wxCommandEvent& event);
		virtual void onPodiumRelativeUp(wxCommandEvent& event);
		
		void onInteractiveMove(CncSecureGesturesPanelEvent& event);
		
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

#endif // CNCPODIUMMGMTMOVEMENT_H
