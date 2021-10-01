#ifndef CNCPODESTMGMTMOVEMENT_H
#define CNCPODESTMGMTMOVEMENT_H

#include "CncCommon.h"
#include "wxCrafterPodestManagement.h"

class CncPodestMgmtMovement : public CncPodestMgmtMovementBase
{
	public:
		
		class Interface {
		
			public:
				Interface() {}
				virtual ~Interface() {}
				
				virtual void podestNotifyEnable(bool state) {}
				virtual void podestNotifyInit(bool state) {}
				virtual void podestNotifyClose(bool state) {}
		};
		
		CncPodestMgmtMovement(wxWindow* parent);
		virtual ~CncPodestMgmtMovement();
		
		bool init();
		bool close();
		
		void setCallbackInterface(Interface* inf) { caller = inf; }
		
	protected:
		virtual void onLeftDownDistance(wxMouseEvent& event);
		virtual void onPodestDownLeave(wxMouseEvent& event);
		virtual void onPodestDownLeftDown(wxMouseEvent& event);
		virtual void onPodestDownLeftUp(wxMouseEvent& event);
		virtual void onPodestRelativeDown(wxCommandEvent& event);
		virtual void onPodestRelativeUp(wxCommandEvent& event);
		virtual void onPodestUpLeave(wxMouseEvent& event);
		virtual void onPodestUpLeftDown(wxMouseEvent& event);
		virtual void onPodestUpLeftUp(wxMouseEvent& event);
		
	private:
		CncLinearDirection	direction;
		Interface*			caller;
		
		void reset();
		void process();
		
		void enable(bool state);
		
		double  evaluateMillimeterToMove();
		int32_t evaluateStepsToMove();
};

#endif // CNCPODESTMGMTMOVEMENT_H
