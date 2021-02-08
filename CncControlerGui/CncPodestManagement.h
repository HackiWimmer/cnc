#ifndef CNCPODESTMANAGEMENT_H
#define CNCPODESTMANAGEMENT_H

#include "CncCommon.h"
#include "wxCrafterPodestManagement.h"

class CncPodestManagement : public CncPodestManagementBase {
	
	public:
		CncPodestManagement(wxWindow* parent);
		virtual ~CncPodestManagement();
		
	protected:
		virtual void onPodestUpLeftDown(wxMouseEvent& event);
		virtual void onPodestUpLeftUp(wxMouseEvent& event);
		virtual void onPodestDownLeftDown(wxMouseEvent& event);
		virtual void onPodestDownLeftUp(wxMouseEvent& event);
		virtual void onLefDownInfo(wxMouseEvent& event);
		virtual void onStartupTimer(wxTimerEvent& event);
		virtual void onClose(wxCommandEvent& event);
		
	private:
		CncLinearDirection	direction;
		
		void process();
		void showInfo();
};

#endif // CNCPODESTMANAGEMENT_H
