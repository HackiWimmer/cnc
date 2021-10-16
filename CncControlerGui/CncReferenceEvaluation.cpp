#include <sstream> 
#include <wx/gdicmn.h>
#include <wx/richtooltip.h>
#include <wx/uiaction.h>
#include "CncCommon.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "CncBoundarySpace.h"
#include "GlobalFunctions.h"
#include "MainFrame.h"
#include "CncExternalViewBox.h"
#include "CncAutoProgressDialog.h"
#include "CncSecureNumpadDialog.h"
#include "CncReferenceEvaluation.h"
#include "wxCrafterImages.h"

///////////////////////////////////////////////////////////////////
CncReferenceEvaluation::CncReferenceEvaluation(wxWindow* parent)
: CncReferenceEvaluationBase				(parent)
, CncTouchBlockDetector::CallbackInterface	()
, CncVideoCapturePanel::CallbackInterface	()
, valid										(false)
, caller									(NULL)
, cameraCapture								(NULL)
, extCameraPreview							(new CncExternalViewBox(this))
, setButton									(NULL)
, cancelButton								(NULL)
, touchCorner								(TM_UNKNOWN)
, cameraSwapState							(SS_ATTACHED)
///////////////////////////////////////////////////////////////////
{
	if ( THE_CONFIG->getCameraSupportFlag() == true ) {
		cameraCapture = new CncVideoCapturePanel(this);
		cameraCapture->setCaller(this);
		
		updateCameraParameter();
		GblFunc::replaceControl(m_cameraCapturePlaceholder, cameraCapture);
		extCameraPreview->setupView(CncExternalViewBox::Default::VIEW1, cameraCapture, "External Camera Preview . . . ");
	}
	
	// default value
	setMode(CncRefPositionMode::CncRM_Mode5);
	m_notebookEvalMode->SetSelection(SEL_BY_EYE);
	selectEvaluationMode();
	
	imgTouchCorner[0] = wxImage();
	imgTouchCorner[1] = m_btTouchCorner->GetBitmap().ConvertToImage();
	imgTouchCorner[2] = imgTouchCorner[1].Rotate90(false);
	imgTouchCorner[3] = imgTouchCorner[2].Rotate90(false);
	imgTouchCorner[4] = imgTouchCorner[3].Rotate90(false);
	
	wxFloatingPointValidator<float> val(3, NULL, wxNUM_VAL_DEFAULT );
	val.SetRange(0, 50.0);	val.SetPrecision(3);
	
	val.SetRange(0.0, 100.0);
	val.SetPrecision(3);
	m_workpiceThickness->SetValidator(val);

	m_tbPrevTest		->SetValue(false);
	m_tbPrevTest		->SetLabel(m_tbPrevTest->GetValue() ? "Yes" : "No");
	m_touchDiameter		->SetValidator(val);
	m_touchDiameter		->SetValue(wxString::Format("%.3lf", 0.00));
	m_workpiceThickness	->SetValue(wxString::Format("%.3lf", 0.00));
	
	setTouchCorner(TM_A);
	updatePreview();
}
///////////////////////////////////////////////////////////////////
CncReferenceEvaluation::~CncReferenceEvaluation() {
///////////////////////////////////////////////////////////////////
	wxDELETE(cameraCapture);
	wxDELETE(extCameraPreview);
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::setMessage(const wxString& msg) {
///////////////////////////////////////////////////////////////////
	if ( caller )
		caller->referenceNotifyMessage(msg);
}
///////////////////////////////////////////////////////////////////
const RefPosResult& CncReferenceEvaluation::getResult(RefPosResult& result) const {
///////////////////////////////////////////////////////////////////
	result.refMode				= getReferenceMode();
	result.workpieceThickness	= getWorkpieceThickness();
	result.measurementOffset	= getMeasurementOffset();
	result.zeroX				= m_btZeroX->GetValue();
	result.zeroY				= m_btZeroY->GetValue();
	result.zeroZ				= m_btZeroZ->GetValue();
	
	return result;
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::init() {
///////////////////////////////////////////////////////////////////
	m_btZeroX->SetValue(true);
	m_btZeroY->SetValue(true);
	m_btZeroZ->SetValue(true);
	determineZeroMode();
	selectEvaluationMode();
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::cancel() {
///////////////////////////////////////////////////////////////////
	if ( cameraCapture != NULL ) 
		cameraCapture->stop();
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::set() {
///////////////////////////////////////////////////////////////////
	if ( isWorkpieceThicknessNeeded() ) {
		if ( cnc::dblCompareNull(getWorkpieceThickness()) == true ) {
			wxMessageDialog dlg(this, 
								_T("For the chosen mode workpiece thickness can't be 0.0"), 
								_T("Invalid Workpiece Thickness  . . ."), 
								wxOK|wxICON_ERROR|wxCENTRE);
			dlg.ShowModal();
			return;
		}
	}
	
	if ( cameraCapture != NULL ) 
		cameraCapture->stop();
		
	valid = true;
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::resetTempSetting() {
///////////////////////////////////////////////////////////////////
	m_btZeroX->SetValue(true);
	m_btZeroY->SetValue(true);
	m_btZeroZ->SetValue(true);
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::mode1(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	setMode(CncRefPositionMode::CncRM_Mode1);
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::mode2(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	setMode(CncRefPositionMode::CncRM_Mode2);
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::mode3(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	setMode(CncRefPositionMode::CncRM_Mode3);
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::mode4(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	setMode(CncRefPositionMode::CncRM_Mode4);
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::mode5(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	setMode(CncRefPositionMode::CncRM_Mode5);
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::mode6(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	setMode(CncRefPositionMode::CncRM_Mode6);
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::detachCameraCapture(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( cameraCapture == NULL )
		return;
		
	const bool isExtViewActive = !extCameraPreview->IsShown();
	
	// prepare and show external preview
	extCameraPreview->setupSwapButton(CncExternalViewBox::Default::VIEW1, ((wxButton*)event.GetEventObject()));
	extCameraPreview->selectView(CncExternalViewBox::Default::VIEW1);
	extCameraPreview->Show(isExtViewActive);
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::onSelectEvaluationMode(wxListbookEvent& event) {
///////////////////////////////////////////////////////////////////
	selectEvaluationMode();
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::onSelectTouchCorner(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	switch ( touchCorner ) {
		case TM_A:	setTouchCorner(TM_B);		break;
		case TM_B:	setTouchCorner(TM_C);		break;
		case TM_C:	setTouchCorner(TM_D);		break;
		case TM_D:	setTouchCorner(TM_A);		break;
		default:	setTouchCorner(TM_UNKNOWN);
	}
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::onTouchDiameterKeyDown(wxKeyEvent& event) {
///////////////////////////////////////////////////////////////////
	const wxChar uc = event.GetUnicodeKey();
	if ( uc == WXK_NONE ) {
		if ( event.GetKeyCode() == WXK_DOWN ) 
		{
			double d; m_touchDiameter->GetValue().ToDouble(&d);
					
			if ( d < 16 ) {
				d += 0.1;
				m_touchDiameter->SetValue(wxString::Format("%.3lf", d));
			}
			
			return;
		}
		
		if ( event.GetKeyCode() == WXK_UP ) 
		{
			double d; m_touchDiameter->GetValue().ToDouble(&d);
			
			if ( d > 0 ) {
				d -= 0.1;
				m_touchDiameter->SetValue(wxString::Format("%.3lf", d));
			}
			
			return;
		}
	}
	
	// to ensure the default handling
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::onTouchDiameterLeftDown(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( THE_CONTEXT->secureModeInfo.isActive == true ) {
		event.Skip(false);
		
		CncSecureNumpadDialog dlg(this, CncSecureNumpad::Type::DOUBLE, 3, 0.0, 30.0);
		dlg.setValue(m_touchDiameter->GetValue());
		dlg.setInfo("Touch Diameter:");
		dlg.Center(wxCENTRE_ON_SCREEN);
		
		if ( dlg.ShowModal() == wxID_OK ) 
			m_touchDiameter->ChangeValue(wxString::Format("%.3lf", dlg.getValueAsDouble()));
			
		return;
	}
	
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::onTouchTest(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( THE_CONTEXT->hasHardware() == false ) {
		if ( caller )
			caller->referenceNotifyMessage("The current connected serial port do not support hardware.\n Therefore, no test possible");
			
		return;
	}
	
	CncRunAnimationControl rac(THE_APP);
	
	Enable(false);
	CncAutoProgressDialog progressDlg(this);
	progressDlg.Show(true);
		
		wxString msg;
		msg =	"Close the contact manually. " \
				"The test stops after some seconds if no closed contact state was detected. [ESC] will abort the test . . .";
		
		if ( caller )
			caller->referenceNotifyMessage(msg);
		
		CncTouchBlockDetector::Parameters para;
		para.touchMode	= CncTouchBlockDetector::Parameters::TM_TOUCH_TEST;
		para.caller		= this;
		CncTouchBlockDetector::Result result = THE_APP->processTouchTest(para);
		
		if ( caller ) {
			if ( result.hasErrorInfo() )	caller->referenceNotifyMessage(result.errorInfo, wxICON_ERROR);
			else							caller->referenceDismissMessage();
		}
		
	progressDlg.Show(false);
	Enable(true);
	
	// final report
	wxRichToolTip tip("Contact Test:", result.processResult ? "Contact was closed." : "Contact was not closed!");
	tip.SetIcon(result.processResult ? wxICON_INFORMATION : wxICON_ERROR);
	tip.SetTipKind(wxTipKind_TopLeft);
	tip.ShowFor((wxWindow*)event.GetEventObject());
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::onTouchXYZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( THE_CONTEXT->hasHardware() == false ) {
		if ( caller )
			caller->referenceNotifyMessage("The current connected serial port do not support hardware.\n Therefore, no XYZ Touch possible");
			
		return;
	}

	typedef CncTouchBlockDetector::Parameters PARA;
	CncRunAnimationControl rac(THE_APP);
	touch((wxWindow*)event.GetEventObject(), PARA::TM_TOUCH_XYZ);
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::onTouchZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( THE_CONTEXT->hasHardware() == false ) {
		if ( caller )
			caller->referenceNotifyMessage("The current connected serial port do not support hardware.\n Therefore, no z Touch possible");
			
		return;
	}

	typedef CncTouchBlockDetector::Parameters PARA;
	CncRunAnimationControl rac(THE_APP);
	touch((wxWindow*)event.GetEventObject(), PARA::TM_TOUCH_Z);
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::updateCameraParameter(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	updateCameraParameter();
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::updateParameter(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	updatePreview();
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::toggleZeroX(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	determineZeroMode();
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::toggleZeroY(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	determineZeroMode();
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::toggleZeroZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	determineZeroMode();
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::setMode(short mode) {
///////////////////////////////////////////////////////////////////
	m_btMode1->SetValue(mode == CncRefPositionMode::CncRM_Mode1 ? true : false);
	m_btMode2->SetValue(mode == CncRefPositionMode::CncRM_Mode2 ? true : false);
	m_btMode3->SetValue(mode == CncRefPositionMode::CncRM_Mode3 ? true : false);
	m_btMode4->SetValue(mode == CncRefPositionMode::CncRM_Mode4 ? true : false);
	m_btMode5->SetValue(mode == CncRefPositionMode::CncRM_Mode5 ? true : false);
	m_btMode6->SetValue(mode == CncRefPositionMode::CncRM_Mode6 ? true : false);
	
	m_modeSelected->SetLabel(wxString::Format("%d", mode));
	
	wxString hint;
	switch ( mode ) {
		case CncRefPositionMode::CncRM_Mode1: 	hint.assign("Reference: Ground plate\nDon't consider any workpiece thickness\nWithout measure plate"); 
												m_workpiceThickness->Enable(false);
												m_measuremetOffsetZ->Enable(false);
												break;
		case CncRefPositionMode::CncRM_Mode2: 	hint.assign("Reference: Ground plate\nDon't consider any workpiece thickness\nWith measure plate"); 
												m_workpiceThickness->Enable(false);
												m_measuremetOffsetZ->Enable(true);
												break;
		case CncRefPositionMode::CncRM_Mode3: 	hint.assign("Reference: Ground plate\nConsider workpiece thickness\nWithout measure plate"); 
												m_workpiceThickness->Enable(true);
												m_measuremetOffsetZ->Enable(false);
												break;
		case CncRefPositionMode::CncRM_Mode4: 	hint.assign("Reference: Ground plate\nConsider workpiece thickness\nWith measure plate"); 
												m_workpiceThickness->Enable(true);
												m_measuremetOffsetZ->Enable(true);
												break;
		case CncRefPositionMode::CncRM_Mode5: 	hint.assign("Reference: Workpiece surface\nDon't consider any workpiece thickness\nWithout measure plate"); 
												m_workpiceThickness->Enable(false);
												m_measuremetOffsetZ->Enable(false);
												break;
		case CncRefPositionMode::CncRM_Mode6: 	hint.assign("Reference: Workpiece surface\nDon't consider any workpiece thickness\nWith measure plate"); 
												m_workpiceThickness->Enable(false);
												m_measuremetOffsetZ->Enable(true);
												break;
	}
	
	m_modeText->SetLabel(hint);
	updatePreview();
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::updatePreview() {
///////////////////////////////////////////////////////////////////
	CncBoundarySpace bs;
	bs.setHardwareOffset(THE_BOUNDS->getHardwareOffset());
	bs.setMeasurementOffset(getMeasurementOffset());
	bs.setWorkpieceThickness(getWorkpieceThickness());
	bs.setRefPositionMode(getReferenceMode());
	
	CncDoublePosition rp(bs.getCalculatedRefPositionMetric());
	
	if ( shouldZeroX() )	m_previewX->ChangeValue(wxString::Format("%.3lf", rp.getX()));
	else					m_previewX->ChangeValue(THE_APP->GetXAxis()->GetValue());
	
	if ( shouldZeroY() )	m_previewY->ChangeValue(wxString::Format("%.3lf", rp.getY()));
	else					m_previewY->ChangeValue(THE_APP->GetYAxis()->GetValue());

	if ( shouldZeroZ() )	m_previewZ->ChangeValue(wxString::Format("%.3lf", rp.getZ()));
	else					m_previewZ->ChangeValue(THE_APP->GetZAxis()->GetValue());
}
///////////////////////////////////////////////////////////////////
CncRefPositionMode CncReferenceEvaluation::getReferenceMode() const {
///////////////////////////////////////////////////////////////////
	return (CncRefPositionMode)evaluateMode();
}
///////////////////////////////////////////////////////////////////
double CncReferenceEvaluation::getWorkpieceThickness() const {
///////////////////////////////////////////////////////////////////
	const wxString wpt = m_workpiceThickness->GetValue();
	double ret = 0.0;
	
	if ( isWorkpieceThicknessAvailable() )
		wpt.ToDouble(&ret);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
CncDoubleOffset CncReferenceEvaluation::getMeasurementOffset() const {
///////////////////////////////////////////////////////////////////
	CncDoubleOffset mos;
	mos.setX(getValueAsDouble(m_measuremetOffsetX, 0.0));
	mos.setY(getValueAsDouble(m_measuremetOffsetY, 0.0));
	mos.setZ(getValueAsDouble(m_measuremetOffsetZ, 0.0));
	
	return mos;
}
///////////////////////////////////////////////////////////////////
short CncReferenceEvaluation::evaluateMode() const {
///////////////////////////////////////////////////////////////////
	const int sel = m_notebookEvalMode->GetSelection();

	switch ( sel ) {
		case SEL_TOUCHBLOCK:	return CncRM_Touchblock;
		case SEL_CAMERA:		return CncRM_Camera;

		case SEL_BY_EYE:		if (      m_btMode1->GetValue() == true ) return CncRM_Mode1;
								else if ( m_btMode2->GetValue() == true ) return CncRM_Mode2;
								else if ( m_btMode3->GetValue() == true ) return CncRM_Mode3;
								else if ( m_btMode4->GetValue() == true ) return CncRM_Mode4;
								else if ( m_btMode5->GetValue() == true ) return CncRM_Mode5;
								else if ( m_btMode6->GetValue() == true ) return CncRM_Mode6;
								break;
	}
	
	return CncRM_Unknown;
}
///////////////////////////////////////////////////////////////////
bool CncReferenceEvaluation::isWorkpieceThicknessNeeded() const {
///////////////////////////////////////////////////////////////////
	const short mode = evaluateMode();
	switch ( mode ) {
		case CncRM_Mode1:			return false;
		case CncRM_Mode2:			return false;
		case CncRM_Mode3:			return true;
		case CncRM_Mode4:			return true;
		case CncRM_Mode5:			return false;
		case CncRM_Mode6:			return false;
		case CncRM_Touchblock:		return false;
		case CncRM_Camera:			return false;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncReferenceEvaluation::isWorkpieceThicknessAvailable() const {
///////////////////////////////////////////////////////////////////
	const short mode = evaluateMode();
	switch ( mode ) {
		case CncRM_Mode1:			return false;
		case CncRM_Mode2:			return false;
		case CncRM_Mode3:			return true;
		case CncRM_Mode4:			return true;
		case CncRM_Mode5:			return true;
		case CncRM_Mode6:			return true;
		case CncRM_Touchblock:		return true;
		case CncRM_Camera:			return true;
	}
	
	return false;
}
///////////////////////////////////////////////////////////////////
double CncReferenceEvaluation::getValueAsDouble(wxTextCtrl* ctrl, double defaultValue) const {
///////////////////////////////////////////////////////////////////
	if ( ctrl == NULL )
		return defaultValue;
		
	double d;
	if ( ctrl->GetValue().ToDouble(&d) )
		return d;
		
	return defaultValue;
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::determineZeroMode() {
///////////////////////////////////////////////////////////////////
	auto evaluate = [&](wxBitmapToggleButton* bt) {
		wxASSERT( bt != NULL );
		return bt->GetValue() == true ? '0' : '-';
	};
	
	if ( setButton ) {
		setButton->SetLabel(wxString::Format("Zero (%c, %c, %c)", evaluate(m_btZeroX), evaluate(m_btZeroY), evaluate(m_btZeroZ)));
		setButton->Enable(m_btZeroX->GetValue() == true || m_btZeroY->GetValue() == true || m_btZeroZ->GetValue() == true);
	}
	
	updatePreview();
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::updateCameraParameter() {
///////////////////////////////////////////////////////////////////
	if ( cameraCapture == NULL )
		return;
		
	cameraCapture->showHVCross(m_btShowCrossHV->GetValue());
	cameraCapture->showDDCross(m_btShowCrossDD->GetValue());
	cameraCapture->showRect(m_btShowRect->GetValue());
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::selectEvaluationMode() {
///////////////////////////////////////////////////////////////////
	const int sel = m_notebookEvalMode->GetSelection();
	
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;
	
	m_measuremetOffsetX->Enable(false);
	m_measuremetOffsetY->Enable(false);
	m_workpiceThickness->Enable(isWorkpieceThicknessAvailable());
	
	if ( cameraCapture != NULL ) 
		cameraCapture->stop();
	
	switch ( sel ) {
		case SEL_TOUCHBLOCK:	x = THE_CONFIG->getTouchblockXCheckThickness();
								y = THE_CONFIG->getTouchblockYCheckThickness();
								z = THE_CONFIG->getTouchblockZThickness();
								m_measuremetOffsetZ->Enable(false);
								break;
								
		case SEL_BY_EYE:		x = 0.0;
								y = 0.0;
								z = THE_CONFIG->getTouchblockZThickness();
								m_measuremetOffsetZ->Enable(evaluateMode() % 2 == 0);
								break;
								
		case SEL_CAMERA:		if ( cameraCapture != NULL ) {
									x = THE_CONFIG->getCameraOffsetX();
									y = THE_CONFIG->getCameraOffsetY();
									z = 0.00;
									m_measuremetOffsetZ->Enable(false);
									cameraCapture->start();
								} 
								else {
									x = 0.0;
									y = 0.0;
									z = 0.0;
									m_measuremetOffsetZ->Enable(false);
								}
								break;
	}
	
	// attach/detach camera
	if ( THE_CONTEXT->hasHardware() == true ) 
	{
		if ( THE_APP->GetPanelCameraPreviewPlaceholder() && THE_APP->GetPanelCameraPreviewPlaceholder()->IsShown() )
		{
			if ( caller )
				caller->cameraNotifyPreview(sel == SEL_CAMERA);
			
			if (  sel == SEL_CAMERA && cameraSwapState == SS_ATTACHED ) {
				GblFunc::swapControls(THE_APP->GetPanelCameraPreviewPlaceholder(), m_panelCameraArea);
				cameraSwapState = SS_DETACHED; 
			}
				
			if (  sel != SEL_CAMERA && cameraSwapState == SS_DETACHED ) {
				GblFunc::swapControls(THE_APP->GetPanelCameraPreviewPlaceholder(), m_panelCameraArea);
				cameraSwapState = SS_ATTACHED;
			}
		}
	}
	
	m_measuremetOffsetX->SetValue(wxString::Format("%.3lf", x));
	m_measuremetOffsetY->SetValue(wxString::Format("%.3lf", y));
	m_measuremetOffsetZ->SetValue(wxString::Format("%.3lf", z));
	
	updatePreview();
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::setTouchCorner(const TouchCorner c) {
///////////////////////////////////////////////////////////////////
	touchCorner = c;
	m_btTouchCorner->SetBitmap(getTouchCornerImage(touchCorner));
}
///////////////////////////////////////////////////////////////////
const wxImage& CncReferenceEvaluation::getTouchCornerImage(const TouchCorner m) const {
///////////////////////////////////////////////////////////////////
	switch ( m ) {
		case TM_D:	return imgTouchCorner[4];
		case TM_C:	return imgTouchCorner[3];
		case TM_B:	return imgTouchCorner[2];
		case TM_A:	return imgTouchCorner[1];
		default:	return imgTouchCorner[0];
	}
	
	return imgTouchCorner[0];
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::notifyProgess(const wxString& msg) {
///////////////////////////////////////////////////////////////////
	if ( caller )
		caller->referenceNotifyMessage(msg, wxICON_INFORMATION);
		
	m_continuousTimer->Start();
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::notifyError(const wxString& msg) {
///////////////////////////////////////////////////////////////////
	if ( caller )
		caller->referenceNotifyMessage(msg, wxICON_ERROR);
		
	m_continuousTimer->Start();
}
///////////////////////////////////////////////////////////////////
const CncReferenceEvaluation::TouchCorner CncReferenceEvaluation::getTouchCorner() const {
///////////////////////////////////////////////////////////////////
	return touchCorner;
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::onContinuousTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	static wxDateTime tsLast = wxDateTime::Now();
	
	if ( (wxDateTime::Now() - tsLast).Abs().GetSeconds() > 5 ) 
	{
		if ( caller )
			caller->referenceDismissMessage();
		
		tsLast = wxDateTime::Now();
	}
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::onTogglePrevTest(wxCommandEvent& event) { 
///////////////////////////////////////////////////////////////////
	m_tbPrevTest->SetLabel( m_tbPrevTest->GetValue() ? "Yes" : "No");
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::setEnforceFlag(bool s) {
///////////////////////////////////////////////////////////////////
	valid = !s;
	
	const int refMode = (int)getReferenceMode();
	
	wxBitmap bmp(ImageLib24().Bitmap( valid ? "BMP_TRAFFIC_LIGHT_GREEN" : "BMP_TRAFFIC_LIGHT_RED")); 
	const wxString mod(wxString::Format("Reference position mode: %s [%d]", cnc::getReferenceModeAsString(getReferenceMode()), ( valid ? refMode : -1 ) ));
	const wxString tip(wxString::Format("Reference position state: %s\n%s", ( valid ? "Valid" : "Invalid" ), mod));
	
	// display ref pos mode too
	if ( valid == true ) {
		const wxFont font(9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));
		wxMemoryDC mdc(bmp);
		mdc.SetFont(font);
		mdc.SetTextForeground(wxColor(0, 0, 0));
		mdc.DrawText(wxString::Format("%d", refMode), {5, 1});
		bmp = mdc.GetAsBitmap();
	}
	
	THE_APP->GetRefPosState()->SetToolTip(tip);
	THE_APP->GetRefPosState()->SetBitmap(bmp);
	THE_APP->GetStatusBar()->Refresh();
	THE_APP->GetStatusBar()->Update();
	
	wxRichToolTip rTip("Reference Position Information", tip);
	rTip.SetIcon(wxICON_INFORMATION);
	//rTip.SetTipKind(wxTipKind_TopLeft);
	rTip.ShowFor(THE_APP->GetRefPosState());
}
///////////////////////////////////////////////////////////////////
void CncReferenceEvaluation::touch(wxWindow* btn, CncTouchBlockDetector::Parameters::TouchMode tm) {
///////////////////////////////////////////////////////////////////
	typedef CncTouchBlockDetector::Parameters PARA;
	
	auto evalTouchCorner = [&](const TouchCorner tc) {
		switch ( tc ) {
			case TM_D:		return PARA::TouchCorner::TC_LEFT_TOP;
			case TM_C:		return PARA::TouchCorner::TC_RIGHT_TOP;
			case TM_B:		return PARA::TouchCorner::TC_RIGHT_BOTTOM;
			case TM_A:
			default:		return PARA::TouchCorner::TC_LEFT_BOTTOM;
			
		}
		
		return PARA::TouchCorner::TC_LEFT_BOTTOM;
	};
	
	if ( tm != PARA::TM_TOUCH_Z && tm != PARA::TM_TOUCH_XYZ ) {
		std::cerr << CNC_LOG_FUNCT_A(": Invalid touch mode \n");
		return;
	}
	
	wxString touchToken(PARA::TM_TOUCH_Z ? "Z Touch" : "XYZ Touch");
	
	Enable(false);
	CncAutoProgressDialog progressDlg(this);
	progressDlg.Show(true);
	
		if ( m_tbPrevTest->GetValue() == true ) {
			
			wxString msg;
			msg =	"Close the contact manually. " \
					"The test stops after some seconds if no closed contact state was detected. [ESC] will abort the processing . . .";
			
			if ( caller )
				caller->referenceNotifyMessage(msg);
			
			CncTouchBlockDetector::Parameters testPara;
			testPara.touchMode	= CncTouchBlockDetector::Parameters::TM_TOUCH_TEST;
			testPara.caller		= this;
			CncTouchBlockDetector::Result result = THE_APP->processTouchTest(testPara);
			
			wxRichToolTip tip("Prior Contact Test:", result.processResult ? "Contact was closed." : "Contact was not closed!");
			tip.SetIcon(result.processResult ? wxICON_INFORMATION : wxICON_ERROR);
			tip.SetTipKind(wxTipKind_TopLeft);
			tip.ShowFor(btn);
			
			if ( result.processResult == false ) {
				if ( caller ) {
					if ( result.hasErrorInfo() )	caller->referenceNotifyMessage(result.errorInfo, wxICON_ERROR);
					else							caller->referenceDismissMessage();
				}
				progressDlg.Show(false);
				Enable(true);
				return;
			}
			
			if ( caller )
				caller->referenceNotifyMessage("The process will wait a second . . . ");
				
			THE_APP->waitActive(1000);
		}
		
		if ( caller )
			caller->referenceNotifyMessage("Running the " + touchToken + " . . . ");
		
		CncTouchBlockDetector::Parameters para;
		para.touchMode					= tm;
		para.caller						= this;
		para.touchCorner				= evalTouchCorner(getTouchCorner());
		para.touchDiameter				= getValueAsDouble(m_touchDiameter, 0.0);
		para.touchBlockZThickness		= THE_CONFIG->getTouchblockZThickness();
		para.touchBlockXCheekThickness	= THE_CONFIG->getTouchblockXCheckThickness();
		para.touchBlockYCheekThickness	= THE_CONFIG->getTouchblockYCheckThickness();
		
		CncTouchBlockDetector::Result result = THE_APP->processZTouch(para);
		if ( result.processResult == true ) {
			const CncLongOffset mos = result.getMesurementOffset();
			
			m_measuremetOffsetX->SetValue(wxString::Format("%.3lf", THE_CONFIG->convertStepsToMetricX(mos.getX())));
			m_measuremetOffsetY->SetValue(wxString::Format("%.3lf", THE_CONFIG->convertStepsToMetricY(mos.getY())));
			m_measuremetOffsetZ->SetValue(wxString::Format("%.3lf", THE_CONFIG->convertStepsToMetricZ(mos.getZ())));
		}
		else {
			m_measuremetOffsetX->SetValue(wxString::Format("%.3lf", 0.0));
			m_measuremetOffsetY->SetValue(wxString::Format("%.3lf", 0.0));
			m_measuremetOffsetZ->SetValue(wxString::Format("%.3lf", 0.0));
		}
	
	progressDlg.Show(false);
	Enable(true);
	
	if ( caller ) {
		if ( result.hasErrorInfo() )	caller->referenceNotifyMessage(result.errorInfo, wxICON_ERROR);
		else							caller->referenceDismissMessage();
	}

	wxRichToolTip tip(touchToken, result.processResult ? "Touch was successful." : "Touch was failed!");
	tip.SetIcon(result.processResult ? wxICON_INFORMATION : wxICON_ERROR);
	tip.SetTipKind(wxTipKind_TopLeft);
	tip.ShowFor(btn);
}
