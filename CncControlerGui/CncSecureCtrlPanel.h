#ifndef CNCSECURECTRLPANEL_H
#define CNCSECURECTRLPANEL_H

#include "wxCrafterSecurePanel.h"

class CncSecurePortListCtrl;
class CncSecureManuallyMovePanel;

class CncSecureCtrlPanel : public CncSecureCtrlPanelBase
{
	public:
		CncSecureCtrlPanel(wxWindow* parent);
		virtual ~CncSecureCtrlPanel();
		
		void activate(bool b);
		void lockSelection(bool b);
		
		void setPortSelection(const wxString& portName);
		
		void clearPortSelection();
		void addPortName(const wxString& portName, const wxString& imageName = "");
		
	protected:
		virtual void onStackTraceStoreSec(wxCommandEvent& event);
		virtual void onSessionDirSec(wxCommandEvent& event);
		virtual void onTestFunctionSec(wxCommandEvent& event);
		virtual void onOpenTemplateSec(wxCommandEvent& event);
		virtual void onDetermineAnchorPositionsSec(wxCommandEvent& event);
		virtual void onSetReferencePosition(wxCommandEvent& event);
		virtual void onEvaluateHardwareReference(wxCommandEvent& event);
		
		virtual void onConnectSec(wxCommandEvent& event);
		virtual void onResetSec(wxCommandEvent& event);
		virtual void onEmergencySec(wxCommandEvent& event);
		virtual void onPauseSec(wxCommandEvent& event);
		virtual void onRunSec(wxCommandEvent& event);
		virtual void onStopSec(wxCommandEvent& event);
		virtual void onLeftBookPageChanged(wxListbookEvent& event);
		
	private:
	
		CncSecurePortListCtrl*			portSelectorList;
		CncSecureManuallyMovePanel*		manuallyMovePanel;
		
		void performRightHeadline();
		void tryToProvideTemplate();
};

#endif // CNCSECURECTRLPANEL_H
