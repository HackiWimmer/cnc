#ifndef CNCPODESTMANAGEMENT_H
#define CNCPODESTMANAGEMENT_H

#include "CncCommon.h"
#include "wxCrafterPodestManagement.h"

class CncPodestManagement : public CncPodestManagementBase {
	
	public:
		CncPodestManagement(wxWindow* parent);
		virtual ~CncPodestManagement();
		
	protected:
		virtual void onPodestRelativeDown(wxCommandEvent& event);
		virtual void onPodestRelativeUp(wxCommandEvent& event);
		virtual void onPodestDownLeave(wxMouseEvent& event);
		virtual void onPodestUpLeave(wxMouseEvent& event);
		virtual void onPodestUpLeftDown(wxMouseEvent& event);
		virtual void onPodestUpLeftUp(wxMouseEvent& event);
		virtual void onPodestDownLeftDown(wxMouseEvent& event);
		virtual void onPodestDownLeftUp(wxMouseEvent& event);
		virtual void onLefDownInfo(wxMouseEvent& event);
		virtual void onInit(wxInitDialogEvent& event);
		virtual void onShow(wxShowEvent& event);
		virtual void onClose(wxCommandEvent& event);
		
	private:
		CncLinearDirection	direction;
		
		void reset();
		void process();
		void showInfo();
		
		void enable(bool state);
		
		double  evaluateMillimeterToMove();
		int32_t evaluateStepsToMove();
		
};

#endif // CNCPODESTMANAGEMENT_H
