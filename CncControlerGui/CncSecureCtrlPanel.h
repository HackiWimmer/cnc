#ifndef CNCSECURECTRLPANEL_H
#define CNCSECURECTRLPANEL_H

#include "CncSecureSlidepad.h"
#include "CncReferenceEvaluation.h"
#include "CncPodestMgmtMovement.h"
#include "CncSecureScrollButton.h"
#include "wxCrafterSecurePanel.h"

class CncSecurePortListCtrl;
class CncSecureManuallyMovePanel;

class CncSecureCtrlPanel	: public CncSecureCtrlPanelBase
							, public CncSecureSlidepad::CallbackInterface
							, public CncReferenceEvaluation::CallbackInterface
							, public CncPodestMgmtMovement::CallbackInterface
							, public CncSecureScrollButton::CallbackInterface
{
	public:
		
		enum MainBookPages {
			PAGE_CONNECT	= 0,
			PAGE_LOAD,
			PAGE_REF,
			PAGE_MOVE,
			PAGE_RUN,
			PAGE_PODEST,
			PAGE_MISC
		};
		
		CncSecureCtrlPanel(wxWindow* parent);
		virtual ~CncSecureCtrlPanel();
		
		void activate(bool b);
		void lockSelection(bool b);
		
		void clearPortSelection();
		void addPortName(const wxString& portName, const wxString& imageName = "");
		
		void updatePortSelection(const wxString& portName);
		void notifyConnection(bool state, const wxString& portName);
		
	protected:
		virtual void onReferenceSet(wxCommandEvent& event);
		virtual void onStackTraceStoreSec(wxCommandEvent& event);
		virtual void onSessionDirSec(wxCommandEvent& event);
		virtual void onTestFunctionSec(wxCommandEvent& event);
		virtual void onOpenTemplateSec(wxCommandEvent& event);
		virtual void onDetermineAnchorPositionsSec(wxCommandEvent& event);
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
		virtual void cameraNotifyPreview(bool show);
		virtual void referenceNotifyMessage(const wxString& msg, int flags = wxICON_INFORMATION);
		virtual void referenceDismissMessage();
		virtual void podestNotifyEnable(bool state);
		virtual void podestNotifyInit(bool state);
		virtual void podestNotifyClose(bool state);
		virtual void notifyValueChange(const CncSecureScrollButton::CBI::Data& d);
		
		friend class CncSecurePortListCtrl;
		
	private:
		
		struct PageInfo {
			int width = 520;
		};
		
		typedef std::vector<PageInfo> PageVector;
		
		CncSecurePortListCtrl*			portSelectorList;
		CncSecureManuallyMovePanel*		manuallyMovePanel;
		CncSecureScrollButton*			interactiveMoveX;
		CncSecureScrollButton*			interactiveMoveY;
		CncSecureScrollButton*			interactiveMoveZ;
		CncPodestMgmtMovement*			podestPanel;
		CncReferenceEvaluation*			referencePanel;
		CncSecureSlidepad*				speedpad;
		PageVector						pageVector;
		
		void performRightHeadline();
		void tryToProvideTemplate();
		void onLeftBookPageChanged();
};

#endif // CNCSECURECTRLPANEL_H
