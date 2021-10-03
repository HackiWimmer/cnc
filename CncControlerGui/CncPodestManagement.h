#ifndef CNCPODESTMANAGEMENT_H
#define CNCPODESTMANAGEMENT_H

#include "CncCommon.h"
#include "CncPodestMgmtMovement.h"
#include "wxCrafterPodestManagement.h"

class CncPodestManagement	: public CncPodestManagementBase
							, public CncPodestMgmtMovement::CallbackInterface
{
	
	public:
		CncPodestManagement(wxWindow* parent);
		virtual ~CncPodestManagement();
		
	protected:
		virtual void onLefDownInfo(wxMouseEvent& event);
		virtual void onInit(wxInitDialogEvent& event);
		virtual void onShow(wxShowEvent& event);
		virtual void onClose(wxCommandEvent& event);
		
		virtual void podestNotifyEnable(bool state);
		virtual void podestNotifyInit(bool state);
		virtual void podestNotifyClose(bool state);
		
	private:
		CncPodestMgmtMovement*	movement;
		
		void showInfo();
};

#endif // CNCPODESTMANAGEMENT_H
