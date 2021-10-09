#ifndef CNCSECURECTRLPANEL_H
#define CNCSECURECTRLPANEL_H

#include "CncSecureSlidepad.h"
#include "CncReferenceEvaluation.h"
#include "CncPodestMgmtMovement.h"
#include "CncSecureGesturesPanel.h"
#include "wxCrafterSecurePanel.h"

class CncSecurePortListCtrl;
class CncSecureManuallyMovePanel;

class CncSecureCtrlPanel	: public CncSecureCtrlPanelBase
							, public CncSecureSlidepad::CallbackInterface
							, public CncReferenceEvaluation::CallbackInterface
							, public CncPodestMgmtMovement::CallbackInterface
							, public CncSecureGesturesPanel::CallbackInterface
{
	public:
		
		enum MainBookPages {
			PAGE_CONNECT	= 0,
			PAGE_LOAD,
			PAGE_REF,
			PAGE_MOVE,
			PAGE_RUN,
			PAGE_CONTROLLER,
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
		
		CncReferenceEvaluation* getReferencePanel() const { return referencePanel; }
		
	protected:
		virtual void requestResolveLimitStates(wxCommandEvent& event);
		virtual void onToggleTouchpadPane(wxCommandEvent& event);
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
		
		virtual void notifyStarting(const CncSecureGesturesPanel::State s);
		virtual void notifyPositionChanged(const CncSecureGesturesPanel::Data& d);
		virtual void notifyPositionHeld(const CncSecureGesturesPanel::Data& d);
		
		friend class CncSecurePortListCtrl;
		
	private:
		
		static const int CallbackID_SPX		=  1;
		static const int CallbackID_SPY		=  2;
		static const int CallbackID_SPZ		=  3;
		static const int CallbackID_TPXY	= 10;
		static const int CallbackID_TPZ		= 11;
		
		struct PageInfo {
			int width = 520;
		};
		
		typedef std::vector<PageInfo> PageVector;
		
		CncSecurePortListCtrl*			portSelectorList;
		CncSecureManuallyMovePanel*		manuallyMovePanel;
		CncSecureGesturesPanel*			interactiveMoveX;
		CncSecureGesturesPanel*			interactiveMoveY;
		CncSecureGesturesPanel*			interactiveMoveZ;
		CncSecureGesturesPanel*			interactiveTouchpadXYZ;
		CncPodestMgmtMovement*			podestPanel;
		CncReferenceEvaluation*			referencePanel;
		CncSecureSlidepad*				speedpad;
		PageVector						pageVector;
		
		void performRightHeadline();
		void tryToProvideTemplate();
		void onLeftBookPageChanged();
};

#endif // CNCSECURECTRLPANEL_H
