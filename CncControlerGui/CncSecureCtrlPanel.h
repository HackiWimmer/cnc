#ifndef CNCSECURECTRLPANEL_H
#define CNCSECURECTRLPANEL_H

#include "CncSecureSlidepad.h"
#include "CncReferenceEvaluation.h"
#include "wxCrafterSecurePanel.h"

class CncSecurePortListCtrl;
class CncSecureManuallyMovePanel;

class CncSecureCtrlPanel	: public CncSecureCtrlPanelBase
							, public CncSecureSlidepad::Interface
							, public CncReferenceEvaluation::Interface
{
	public:
		CncSecureCtrlPanel(wxWindow* parent);
		virtual ~CncSecureCtrlPanel();
		
		void activate(bool b);
		void lockSelection(bool b);
		
		void clearPortSelection();
		void addPortName(const wxString& portName, const wxString& imageName = "");
		
		void updatePortSelection(const wxString& portName);
		void notifyConnection(bool state, const wxString& portName);
		
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
		
		void setPortSelection(const wxString& portName);
		
		virtual void sliderValueChanged(int pos, int value);
		virtual void referenceNotifyMessage(const wxString& msg, int flags = wxICON_INFORMATION);
		
		friend class CncSecurePortListCtrl;
		
	private:
		
		struct PageInfo {
			int width = 520;
		};
		
		typedef std::vector<PageInfo> PageVector;
		
		CncSecurePortListCtrl*			portSelectorList;
		CncSecureManuallyMovePanel*		manuallyMovePanel; 
		CncReferenceEvaluation*			referencePanel;
		CncSecureSlidepad*				speedpad;
		PageVector						pageVector;
		
		void performRightHeadline();
		void tryToProvideTemplate();
		void onLeftBookPageChanged();
};

#endif // CNCSECURECTRLPANEL_H
