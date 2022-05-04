#ifndef CNCREFERENCEPOSITION_H
#define CNCREFERENCEPOSITION_H

#include "wxCrafterReferencePosition.h"
#include "CncControl.h"
#include "CncReferenceEvaluation.h"
#include "CncNavigatorPanel.h"

class CncExternalViewBox;
class CncReferencePosition	: public CncReferencePositionBase
							, public CncReferenceEvaluation::CallbackInterface
{
	protected:
		virtual void onKillCtrlFocus(wxFocusEvent& event);
		virtual void onSetCtrlFocus(wxFocusEvent& event);
		virtual void selectStepMode(wxCommandEvent& event);
		virtual void onInfoTimer(wxTimerEvent& event);
		virtual void selectStepSensitivity(wxCommandEvent& event);
		virtual void init(wxInitDialogEvent& event);
		virtual void show(wxShowEvent& event);
		virtual void cancel(wxCommandEvent& event);
		virtual void set(wxCommandEvent& event);
		
		virtual void referenceNotifyMessage(const wxString& msg, int flags = wxICON_INFORMATION);
		virtual void referenceDismissMessage();

	public:
		enum TouchCorner { TM_UNKNOWN, TM_A, TM_B, TM_C, TM_D };

		CncReferencePosition(wxWindow* parent);
		virtual ~CncReferencePosition();
		
		void					setMessage(const wxString& msg);
		
		const RefPosResult&		getResult(RefPosResult& result)	const { return referencePanel->getResult(result); } 
		bool					isReferenceStateValid()			const { return referencePanel->isReferenceStateValid(); }
		
		void					shiftStepSensitivity();
		void					selectStepSensitivity(int sel);
		
		void					setEnforceFlag(bool s)			      { referencePanel->setEnforceFlag(s); }
		void					resetTempSetting()				      { referencePanel->resetTempSetting(); }
	
	private:

		CncNavigatorPanel*		navigationPanel;
		CncReferenceEvaluation*	referencePanel;

		wxString 				infoMessage;
		
		void					showInformation();
		void					onNavigatorPanel(CncNavigatorPanelEvent& event);
};

#endif // CNCREFERENCEPOSITION_H
