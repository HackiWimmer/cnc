#ifndef CNCPODIUMMANAGEMENT_H
#define CNCPODIUMMANAGEMENT_H

#include "CncCommon.h"
#include "CncPodiumMgmtMovement.h"
#include "wxCrafterPodiumManagement.h"

class CncPodiumManagement	: public CncPodiumManagementBase
							, public CncPodiumMgmtMovement::CallbackInterface
{
	
	public:
		CncPodiumManagement(wxWindow* parent);
		virtual ~CncPodiumManagement();
		
		void dispatchAll();
		
	protected:
		virtual void onLefDownInfo(wxMouseEvent& event);
		virtual void onInit(wxInitDialogEvent& event);
		virtual void onShow(wxShowEvent& event);
		virtual void onClose(wxCommandEvent& event);
		
		virtual void podiumNotifyEnable(bool state);
		virtual void podiumNotifyInit(bool state);
		virtual void podiumNotifyClose(bool state);
		
	private:
	
		bool					enabled;
		CncPodiumMgmtMovement*	movement;
		
		void showInfo();
};

#endif // CNCPODIUMMANAGEMENT_H
