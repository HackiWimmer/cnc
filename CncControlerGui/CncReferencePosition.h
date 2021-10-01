#ifndef CNCREFERENCEPOSITION_H
#define CNCREFERENCEPOSITION_H

#include "wxCrafterReferencePosition.h"
#include "CncControl.h"
#include "CncReferenceEvaluation.h"
#include "CncNavigatorPanel.h"

class CncExternalViewBox;
class CncReferencePosition	: public CncReferencePositionBase
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

	public:
		enum TouchCorner { TM_UNKNOWN, TM_A, TM_B, TM_C, TM_D };

		CncReferencePosition(wxWindow* parent);
		virtual ~CncReferencePosition();
		
		void					setMessage(const wxString& msg);
		
		void					hitKey(int keyCode, int modifier = wxMOD_NONE );
		
		CncRefPositionMode		getReferenceMode()		const	{ return referencePanel->getReferenceMode(); }
		double					getWorkpieceThickness()	const	{ return referencePanel->getWorkpieceThickness(); }
		CncDoubleOffset			getMeasurementOffset()	const	{ return referencePanel->getMeasurementOffset(); }
		bool					shouldZeroX()			const	{ return referencePanel->shouldZeroX(); }
		bool					shouldZeroY()			const	{ return referencePanel->shouldZeroY(); }
		bool					shouldZeroZ()			const	{ return referencePanel->shouldZeroZ(); }
		bool					isReferenceStateValid()	const	{ return valid; }
		
		void					shiftStepSensitivity();
		void					selectStepSensitivity(int sel);
		
		void					setEnforceFlag(bool s);
		void					resetTempSetting();
	
	private:

		bool					valid;
		CncNavigatorPanel*		navigationPanel;
		CncReferenceEvaluation*	referencePanel;

		wxString 				infoMessage;
		
		void					showInformation();
		void					onNavigatorPanel(CncNavigatorPanelEvent& event);
};

#endif // CNCREFERENCEPOSITION_H
