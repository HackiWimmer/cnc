#ifndef CNCREFERENCEEVALUATION_H
#define CNCREFERENCEEVALUATION_H

#include "wxCrafterReferencePosition.h"
#include "CncControl.h"
#include "CncVideoCapturePanel.h"
#include "CncTouchBlockDetector.h"

struct RefPosResult 
{
	CncRefPositionMode	refMode				= CncRM_Unknown;
	double 				workpieceThickness	= 0.0;
	CncDoubleOffset		measurementOffset	= CncDoubleOffset();
	bool				zeroX				= false;
	bool				zeroY				= false;
	bool				zeroZ				= false;
};

class CncExternalViewBox;
class CncReferenceEvaluation	: public CncReferenceEvaluationBase
								, public CncTouchBlockDetector::CallbackInterface
								, public CncVideoCapturePanel::CallbackInterface
{
	public:
		enum TouchCorner { TM_UNKNOWN, TM_A, TM_B, TM_C, TM_D };
		
		class CallbackInterface 
		{
			public:
				CallbackInterface() {}
				virtual ~CallbackInterface() {}
				
				virtual void cameraNotifyPreview(bool show) {}
				virtual void referenceNotifyMessage(const wxString& msg, int flags = wxICON_INFORMATION) {}
				virtual void referenceDismissMessage() {}
		};
		
		void setCallbackInterface(CallbackInterface* inf) { caller = inf; }
		
		CncReferenceEvaluation(wxWindow* parent);
		virtual ~CncReferenceEvaluation();
		
		bool					isReferenceStateValid()	const	{ return valid; }
		
		const RefPosResult& 	getResult(RefPosResult&)const;
		
		CncRefPositionMode		getReferenceMode()		const;
		double					getWorkpieceThickness()	const;
		CncDoubleOffset			getMeasurementOffset()	const;
		
		bool					shouldZeroX()			const	{ return m_btZeroX->GetValue(); }
		bool					shouldZeroY()			const	{ return m_btZeroY->GetValue(); }
		bool					shouldZeroZ()			const	{ return m_btZeroZ->GetValue(); }
		
		void 					init();
		void					set();
		void					cancel();
		void					updatePreview();
		void					resetTempSetting();
		
		void					setEnforceFlag(bool s);
		void					setSetButton(wxButton* bt)    { setButton    = bt; }
		void					setCancelButton(wxButton* bt) { cancelButton = bt; }
		
		void					setMessage(const wxString& msg);
		
	protected:

		virtual void toggleZeroX(wxCommandEvent& event);
		virtual void toggleZeroY(wxCommandEvent& event);
		virtual void toggleZeroZ(wxCommandEvent& event);
		virtual void detachCameraCapture(wxCommandEvent& event);
		virtual void mode1(wxCommandEvent& event);
		virtual void mode2(wxCommandEvent& event);
		virtual void mode3(wxCommandEvent& event);
		virtual void mode4(wxCommandEvent& event);
		virtual void mode5(wxCommandEvent& event);
		virtual void mode6(wxCommandEvent& event);
		virtual void onTouchDiameterLeftDown(wxMouseEvent& event);
		virtual void onTogglePrevTest(wxCommandEvent& event);
		virtual void onContinuousTimer(wxTimerEvent& event);
		virtual void onSelectEvaluationMode(wxListbookEvent& event);
		virtual void onSelectTouchCorner(wxCommandEvent& event);
		virtual void onTouchDiameterKeyDown(wxKeyEvent& event);
		virtual void onTouchTest(wxCommandEvent& event);
		virtual void onTouchXYZ(wxCommandEvent& event);
		virtual void onTouchZ(wxCommandEvent& event);
		virtual void updateCameraParameter(wxCommandEvent& event);
		virtual void updateParameter(wxCommandEvent& event);
		
		virtual void notifyProgess(const wxString& msg);
		virtual void notifyError(const wxString& msg);

		short evaluateMode() const;
		
		void setMode(short mode);
		
	private:
		
		enum SwapState {SS_ATTACHED, SS_DETACHED};
		
		static const int		SEL_TOUCHBLOCK	= 0;
		static const int		SEL_CAMERA		= 1;
		static const int		SEL_BY_EYE		= 2;
		
		wxImage					imgTouchCorner[5];
		
		bool					valid;
		
		CallbackInterface*		caller;
		CncVideoCapturePanel*	cameraCapture;
		CncExternalViewBox* 	extCameraPreview;
		
		wxButton*				setButton;
		wxButton*				cancelButton;
		
		TouchCorner				touchCorner;
		SwapState				cameraSwapState;
		
		void					selectEvaluationMode();
		void					updateCameraParameter();
		
		const wxImage&			getTouchCornerImage(const TouchCorner m)						const;
		const TouchCorner		getTouchCorner()												const;
		void					setTouchCorner(const TouchCorner m);
		
		void					determineZeroMode();
		bool					isWorkpieceThicknessAvailable()									const;
		bool					isWorkpieceThicknessNeeded()									const;
		double					getValueAsDouble(wxTextCtrl* ctrl, double defaultValue = 0.0)	const;
		
		void					touch(wxWindow* btn, CncTouchBlockDetector::Parameters::TouchMode tm);
};

#endif // CNCREFERENCEEVALUATION_H
