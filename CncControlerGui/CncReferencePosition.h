#ifndef CNCREFERENCEPOSITION_H
#define CNCREFERENCEPOSITION_H

#include "wxCrafterReferencePosition.h"
#include "CncControl.h"
#include "CncNavigatorPanel.h"
#include "CncVideoCapturePanel.h"
#include "CncTouchBlockDetector.h"

class CncExternalViewBox;
//class CncGamepadSpy;

class CncReferencePosition	: public CncReferencePositionBase
							, public CncTouchBlockDetector::CallbackInterface
							, public CncVideoCapturePanel::CallbackInterface
{
	protected:
		virtual void updateCameraParameter(wxCommandEvent& event);
		virtual void detachCameraCapture(wxCommandEvent& event);
		virtual void onSelectEvaluationMode(wxListbookEvent& event);
		virtual void onTouchDiameterKeyDown(wxKeyEvent& event);
		virtual void onSelectTouchCorner(wxCommandEvent& event);
		virtual void onTouchTest(wxCommandEvent& event);
		virtual void onTouchXYZ(wxCommandEvent& event);
		virtual void onTouchZ(wxCommandEvent& event);
		virtual void onKillCtrlFocus(wxFocusEvent& event);
		virtual void onSetCtrlFocus(wxFocusEvent& event);
		virtual void updateParameter(wxCommandEvent& event);
		virtual void selectStepMode(wxCommandEvent& event);
		virtual void onInfoTimer(wxTimerEvent& event);
		virtual void onContinuousTimer(wxTimerEvent& event);
		virtual void selectStepSensitivity(wxCommandEvent& event);
		virtual void init(wxInitDialogEvent& event);
		virtual void show(wxShowEvent& event);
		virtual void cancel(wxCommandEvent& event);
		virtual void set(wxCommandEvent& event);
		virtual void mode1(wxCommandEvent& event);
		virtual void mode2(wxCommandEvent& event);
		virtual void mode3(wxCommandEvent& event);
		virtual void mode4(wxCommandEvent& event);
		virtual void mode5(wxCommandEvent& event);
		virtual void mode6(wxCommandEvent& event);
		
		virtual void toggleZeroX(wxCommandEvent& event) 	{ determineZeroMode(); }
		virtual void toggleZeroY(wxCommandEvent& event) 	{ determineZeroMode(); }
		virtual void toggleZeroZ(wxCommandEvent& event) 	{ determineZeroMode(); }
		
		virtual void notifyProgess(const wxString& msg);
		virtual void notifyError(const wxString& msg);

	public:
		enum TouchCorner { TM_UNKNOWN, TM_A, TM_B, TM_C, TM_D };

		CncReferencePosition(wxWindow* parent);
		virtual ~CncReferencePosition();
		
		void					setMessage(const wxString& msg);
		
		void					hitKey(int keyCode, int modifier = wxMOD_NONE );
		
		CncRefPositionMode		getReferenceMode()		const;
		double					getWorkpieceThickness()	const;
		CncDoubleOffset			getMeasurementOffset()	const;
		
		void					shiftStepSensitivity();
		void					selectStepSensitivity(int sel);
		
		bool					shouldZeroX()			const	{ return m_btZeroX->GetValue(); }
		bool					shouldZeroY()			const	{ return m_btZeroY->GetValue(); }
		bool					shouldZeroZ()			const	{ return m_btZeroZ->GetValue(); }
		
		bool					isReferenceStateValid()	const	{ return valid; }
		
		void					setEnforceFlag(bool s);
		void					resetTempSetting();
	
	private:
		
		static const int		SEL_TOUCHBLOCK	= 0;
		static const int		SEL_CAMERA		= 1;
		static const int		SEL_BY_EYE		= 2;

		wxImage					imgTouchCorner[5];
		
		bool					valid;
		CncNavigatorPanel*		navigationPanel;
		CncVideoCapturePanel*	cameraCapture;
		CncExternalViewBox* 	extCameraPreview;
		
		TouchCorner				touchCorner;
		wxString 				infoMessage;
		
		void					selectEvaluationMode();
		
		void					updateCameraParameter();
		
		bool					isWorkpieceThicknessAvailable()									const;
		bool					isWorkpieceThicknessNeeded()									const;
		void					determineZeroMode();
		void					showInformation();
		void					updatePreview();

		void					onNavigatorPanel(CncNavigatorPanelEvent& event);
		short					evaluateMode() const;
		void					setMode(short mode);
				
		const wxImage&			getTouchCornerImage(const TouchCorner m)						const;
		const TouchCorner		getTouchCorner()												const;
		void					setTouchCorner(const TouchCorner m);
		double					getValueAsDouble(wxTextCtrl* ctrl, double defaultValue = 0.0)	const;
		void					touch(wxWindow* btn, CncTouchBlockDetector::Parameters::TouchMode tm);
	
};
#endif // CNCREFERENCEPOSITION_H
