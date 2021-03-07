#include <sstream> 
#include <wx/gdicmn.h>
#include <wx/richtooltip.h>
#include <wx/uiaction.h>
#include "GlobalFunctions.h"
#include "MainFrameProxy.h"
#include "MainFrame.h"
#include "CncConfig.h"
#include "CncBoundarySpace.h"
#include "CncCommon.h"
#include "CncContext.h"
#include "CncExternalViewBox.h"
#include "CncAutoProgressDialog.h"
#include "wxCrafterImages.h"
#include "CncReferencePosition.h"

///////////////////////////////////////////////////////////////////
CncReferencePosition::CncReferencePosition(wxWindow* parent)
: CncReferencePositionBase(parent)
, valid				(false)
, navigationPanel	(NULL)
, cameraCapture		(NULL)
, extCameraPreview	(new CncExternalViewBox(this))
, touchCorner		(TM_UNKNOWN)
, infoMessage		()
///////////////////////////////////////////////////////////////////
{
	//navigation panel control
	CncNavigatorPanel::Config cfg;
	cfg.innerCircle 	= true;
	cfg.shortFormat 	= false;
	cfg.alignment		= wxALIGN_RIGHT;
	cfg.showRegionInfo	= false;
	cfg.initToolTipMapAsCoordSytem();

	navigationPanel = new CncNavigatorPanel(this, cfg);
	GblFunc::replaceControl(m_navigationPanelPlaceholder, navigationPanel);
	
	if ( THE_CONFIG->getCameraSupportFlag() == true ) {
		cameraCapture = new CncVideoCapturePanel(this);
		cameraCapture->setCaller(this);
		
		updateCameraParameter();
		GblFunc::replaceControl(m_cameraCapturePlaceholder, cameraCapture);
		extCameraPreview->setupView(CncExternalViewBox::Default::VIEW1, cameraCapture, "External Camera Preview . . . ");
	}
	
	setMode(CncRefPositionMode::CncRM_Mode5);
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

	m_cbPrevTest		->SetValue(false);
	m_touchDiameter		->SetValidator(val);
	m_touchDiameter		->SetValue(wxString::Format("%.3lf", 3.25));
	m_workpiceThickness	->SetValue(wxString::Format("%.3lf", 0.00));
	
	setTouchCorner(TM_A);
	m_rbStepSensitivity->SetFocusFromKbd();
	updatePreview();
	
	this->Bind(wxEVT_CNC_NAVIGATOR_PANEL, 	&CncReferencePosition::onNavigatorPanel, this);
}
///////////////////////////////////////////////////////////////////
CncReferencePosition::~CncReferencePosition() {
///////////////////////////////////////////////////////////////////
	this->Unbind(wxEVT_CNC_NAVIGATOR_PANEL, &CncReferencePosition::onNavigatorPanel, this);
	
	wxDELETE(navigationPanel);
	wxDELETE(cameraCapture);
	wxDELETE(extCameraPreview);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::onSelectEvaluationMode(wxListbookEvent& event) {
///////////////////////////////////////////////////////////////////
	m_simpleBookEvalMode->SetSelection(m_notebookEvalMode->GetSelection());
	selectEvaluationMode();
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::selectEvaluationMode() {
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
	
	m_measuremetOffsetX->SetValue(wxString::Format("%.3lf", x));
	m_measuremetOffsetY->SetValue(wxString::Format("%.3lf", y));
	m_measuremetOffsetZ->SetValue(wxString::Format("%.3lf", z));
	
	updatePreview();
}
///////////////////////////////////////////////////////////////////
bool CncReferencePosition::isWorkpieceThicknessNeeded() const {
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
bool CncReferencePosition::isWorkpieceThicknessAvailable() const {
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
void CncReferencePosition::setMessage(const wxString& msg) {
///////////////////////////////////////////////////////////////////
	infoMessage.assign(msg);
}
///////////////////////////////////////////////////////////////////
CncRefPositionMode CncReferencePosition::getReferenceMode() const {
///////////////////////////////////////////////////////////////////
	return (CncRefPositionMode)evaluateMode();
}
///////////////////////////////////////////////////////////////////
double CncReferencePosition::getWorkpieceThickness() const {
///////////////////////////////////////////////////////////////////
	const wxString wpt = m_workpiceThickness->GetValue();
	double ret = 0.0;
	
	if ( isWorkpieceThicknessAvailable() )
		wpt.ToDouble(&ret);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
CncDoubleOffset CncReferencePosition::getMeasurementOffset() const {
///////////////////////////////////////////////////////////////////
	CncDoubleOffset mos;
	mos.setX(getValueAsDouble(m_measuremetOffsetX, 0.0));
	mos.setY(getValueAsDouble(m_measuremetOffsetY, 0.0));
	mos.setZ(getValueAsDouble(m_measuremetOffsetZ, 0.0));
	
	return mos;
}
///////////////////////////////////////////////////////////////////
short CncReferencePosition::evaluateMode() const {
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
void CncReferencePosition::setMode(short mode) {
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
void CncReferencePosition::onNavigatorPanel(CncNavigatorPanelEvent& event) {
///////////////////////////////////////////////////////////////////
	// redirect to main frame . . .
	APP_PROXY::postEvent(new CncNavigatorPanelEvent(event));
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::mode1(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	setMode(CncRefPositionMode::CncRM_Mode1);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::mode2(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	setMode(CncRefPositionMode::CncRM_Mode2);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::mode3(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	setMode(CncRefPositionMode::CncRM_Mode3);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::mode4(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	setMode(CncRefPositionMode::CncRM_Mode4);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::mode5(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	setMode(CncRefPositionMode::CncRM_Mode5);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::mode6(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	setMode(CncRefPositionMode::CncRM_Mode6);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::init(wxInitDialogEvent& event) {
///////////////////////////////////////////////////////////////////
	wxPoint pos = APP_PROXY::GetScreenPosition();
	pos.x += 16;
	pos.y += 60;
	SetPosition(pos);
	
	SetAffirmativeId(m_btSet->GetId());
	SetEscapeId(m_btCancel->GetId());
	
	m_btZeroX->SetValue(true);
	m_btZeroY->SetValue(true);
	m_btZeroZ->SetValue(true);
	determineZeroMode();
	selectEvaluationMode();
	
	m_infobar->Dismiss();
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::show(wxShowEvent& event) {
///////////////////////////////////////////////////////////////////
	m_rbStepSensitivity->SetSelection(APP_PROXY::GetRbStepSensitivity()->GetSelection());
	m_rbStepMode->SetSelection(APP_PROXY::GetRbStepMode()->GetSelection());
	
	if ( event.IsShown() )
		m_infoTimer->Start(200);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::cancel(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( cameraCapture != NULL ) 
		cameraCapture->stop();

	EndModal(wxID_CANCEL);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::set(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( isWorkpieceThicknessNeeded() ) {
		if ( cnc::dblCompareNull(getWorkpieceThickness()) == true ) {
			wxMessageDialog dlg(this, 
								_T("For the choosen mode workpiece thickness can't be 0.0"), 
								_T("Invalid Workpiece Thickess  . . ."), 
								wxOK|wxICON_ERROR|wxCENTRE);
			dlg.ShowModal();
			return;
		}
	}
	
	if ( cameraCapture != NULL ) 
		cameraCapture->stop();

	valid = true;
	EndModal(wxID_OK);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::selectStepSensitivity(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( APP_PROXY::isAppPointerAvailable() == true )
		APP_PROXY::GetRbStepSensitivity()->SetSelection(m_rbStepSensitivity->GetSelection());
		
	const CncStepSensitivity ss = cnc::getStepSensitivityOfIndex(m_rbStepSensitivity->GetSelection());
	APP_PROXY::updateSpeedSlider(cnc::getSpeedValue(ss));
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::selectStepMode(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( APP_PROXY::isAppPointerAvailable() == true )
		APP_PROXY::GetRbStepMode()->SetSelection(m_rbStepMode->GetSelection());
	
	const CncStepMode sm = m_rbStepMode->GetSelection() == 0 ? SM_INTERACTIVE : SM_STEPWISE;
	navigationPanel->setStepMode(sm);
	
	const CncStepSensitivity ss = cnc::getStepSensitivityOfIndex(m_rbStepSensitivity->GetSelection());
	APP_PROXY::updateSpeedSlider(sm == SM_INTERACTIVE ? cnc::getSpeedValue(ss) : 1.0);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::showInformation() {
///////////////////////////////////////////////////////////////////
	if ( infoMessage.IsEmpty() == false )
		m_infobar->ShowMessage(infoMessage);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::determineZeroMode() {
///////////////////////////////////////////////////////////////////
	auto evaluate = [&](wxBitmapToggleButton* bt) {
		wxASSERT( bt != NULL );
		return bt->GetValue() == true ? '0' : '-';
	};
	
	m_btSet->SetLabel(wxString::Format("Zero (%c, %c, %c)", evaluate(m_btZeroX), evaluate(m_btZeroY), evaluate(m_btZeroZ)));
	m_btSet->Enable(m_btZeroX->GetValue() == true || m_btZeroY->GetValue() == true || m_btZeroZ->GetValue() == true);
	
	updatePreview();
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::onInfoTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	m_infoTimer->Stop();
	showInformation();
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::onContinuousTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	static wxDateTime tsLast = wxDateTime::Now();
	
	if ( (wxDateTime::Now() - tsLast).Abs().GetSeconds() > 5 ) {
		m_infobar->Dismiss();
		tsLast = wxDateTime::Now();
	}
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::updateParameter(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	updatePreview();
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::updatePreview() {
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
void CncReferencePosition::resetTempSetting() {
///////////////////////////////////////////////////////////////////
	m_btZeroX->SetValue(true);
	m_btZeroY->SetValue(true);
	m_btZeroZ->SetValue(true);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::setEnforceFlag(bool s) {
///////////////////////////////////////////////////////////////////
	valid = !s;
	
	wxBitmap bmp(ImageLib24().Bitmap( valid ? "BMP_TRAFFIC_LIGHT_GREEN" : "BMP_TRAFFIC_LIGHT_RED")); 
	const wxString mod(wxString::Format("Reference position mode: %d",      ( valid ? (int)getReferenceMode() : -1 ) ));
	const wxString tip(wxString::Format("Reference position state: %s\n%s", ( valid ? "Valid" : "Invalid" ), mod));
	
	// display ref pos mode too
	if ( valid == true ) {
		const wxFont font(9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));
		wxMemoryDC mdc(bmp);
		mdc.SetFont(font);
		mdc.SetTextForeground(wxColor(0, 0, 0));
		mdc.DrawText(wxString::Format("%d", (int)getReferenceMode()), {5, 1});
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
void CncReferencePosition::onKillCtrlFocus(wxFocusEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip();
	
	auto modify = [](wxWindow* wnd) {
		wnd->SetBackgroundColour(wnd->GetDefaultAttributes().colBg);
	};
	
	modify(m_rbStepSensitivity);
	modify(m_rbStepMode);
	modify(m_cbPrevTest);
	modify(m_notebookEvalMode);
	
	Refresh();
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::onSetCtrlFocus(wxFocusEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip();
	
	auto modify = [](wxWindow* wnd) {
		if ( wnd == wxWindow::FindFocus())
			wnd->SetBackgroundColour(wnd->GetDefaultAttributes().colBg.ChangeLightness(190));
	};

	modify(m_rbStepSensitivity);
	modify(m_rbStepMode);
	modify(m_cbPrevTest);
	modify(m_notebookEvalMode);
	
	Refresh();
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::hitKey(int keyCode, int modifier) {
///////////////////////////////////////////////////////////////////
	wxWindow* focusWnd = wxWindow::FindFocus();
	
	wxUIActionSimulator uai;
	// Special redirect for this control to keep the default wxTextControl 
	// handling for cursor left and right 
	if ( focusWnd == m_touchDiameter ) {
		if ( keyCode == WXK_LEFT )
			keyCode = WXK_UP;
		
		if ( keyCode == WXK_RIGHT )
			keyCode = WXK_DOWN;
	}
	
	uai.KeyDown(keyCode, modifier);
	uai.KeyUp(keyCode, modifier);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::onSelectTouchCorner(wxCommandEvent& event) {
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
const wxImage& CncReferencePosition::getTouchCornerImage(const TouchCorner m) const {
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
const CncReferencePosition::TouchCorner CncReferencePosition::getTouchCorner() const {
///////////////////////////////////////////////////////////////////
	return touchCorner;
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::setTouchCorner(const TouchCorner c) {
///////////////////////////////////////////////////////////////////
	touchCorner = c;
	m_btTouchCorner->SetBitmap(getTouchCornerImage(touchCorner));
}
///////////////////////////////////////////////////////////////////
double CncReferencePosition::getValueAsDouble(wxTextCtrl* ctrl, double defaultValue) const {
///////////////////////////////////////////////////////////////////
	if ( ctrl == NULL )
		return defaultValue;
		
	double d;
	if ( ctrl->GetValue().ToDouble(&d) )
		return d;
		
	return defaultValue;
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::onTouchDiameterKeyDown(wxKeyEvent& event) {
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
void CncReferencePosition::notifyProgess(const wxString& msg) {
///////////////////////////////////////////////////////////////////
	m_infobar->ShowMessage(msg, wxICON_INFORMATION);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::notifyError(const wxString& msg) {
///////////////////////////////////////////////////////////////////
	m_infobar->ShowMessage(msg, wxICON_ERROR);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::shiftStepSensitivity() {
///////////////////////////////////////////////////////////////////
	const int sel = m_rbStepSensitivity->GetSelection();
	const int max = m_rbStepSensitivity->GetCount();
	
	m_rbStepSensitivity->SetSelection( (sel + 1) % max );
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::selectStepSensitivity(int sel) {
///////////////////////////////////////////////////////////////////
	if ( sel >= 0 && sel < (int)m_rbStepSensitivity->GetCount() )
		m_rbStepSensitivity->SetSelection(sel);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::onTouchTest(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	CncRunAnimationControl rac(THE_APP);
	
	Enable(false);
	CncAutoProgressDialog progressDlg(this);
	progressDlg.Show(true);
		
		wxString msg;
		msg =	"Close the contact manually. " \
				"The test stops after some seconds if no closed contact state was detected. [ESC] will abort the test . . .";
		
		m_infobar->ShowMessage(msg);
		
		CncTouchBlockDetector::Parameters para;
		para.touchMode	= CncTouchBlockDetector::Parameters::TM_TOUCH_TEST;
		para.caller		= this;
		CncTouchBlockDetector::Result result = THE_APP->processTouchTest(para);
		
		if ( result.hasErrorInfo() )	m_infobar->ShowMessage(result.errorInfo, wxICON_ERROR);
		else							m_infobar->Dismiss();
		
	progressDlg.Show(false);
	Enable(true);
	
	// final report
	wxRichToolTip tip("Contact Test:", result.processResult ? "Contact was closed." : "Contact was not closed!");
	tip.SetIcon(result.processResult ? wxICON_INFORMATION : wxICON_ERROR);
	tip.SetTipKind(wxTipKind_TopLeft);
	tip.ShowFor((wxWindow*)event.GetEventObject());
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::onTouchXYZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	typedef CncTouchBlockDetector::Parameters PARA;
	CncRunAnimationControl rac(THE_APP);
	touch((wxWindow*)event.GetEventObject(), PARA::TM_TOUCH_XYZ);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::onTouchZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	typedef CncTouchBlockDetector::Parameters PARA;
	CncRunAnimationControl rac(THE_APP);
	touch((wxWindow*)event.GetEventObject(), PARA::TM_TOUCH_Z);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::touch(wxWindow* btn, CncTouchBlockDetector::Parameters::TouchMode tm) {
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
	
		if ( m_cbPrevTest->GetValue() == true ) {
			
			wxString msg;
			msg =	"Close the contact manually. " \
					"The test stops after some seconds if no closed contact state was detected. [ESC] will abort the processing . . .";
			
			m_infobar->ShowMessage(msg);
			
			CncTouchBlockDetector::Parameters testPara;
			testPara.touchMode	= CncTouchBlockDetector::Parameters::TM_TOUCH_TEST;
			testPara.caller		= this;
			CncTouchBlockDetector::Result result = THE_APP->processTouchTest(testPara);
			
			wxRichToolTip tip("Prior Contact Test:", result.processResult ? "Contact was closed." : "Contact was not closed!");
			tip.SetIcon(result.processResult ? wxICON_INFORMATION : wxICON_ERROR);
			tip.SetTipKind(wxTipKind_TopLeft);
			tip.ShowFor(btn);
			
			if ( result.processResult == false ) {
				if ( result.hasErrorInfo() )	m_infobar->ShowMessage(result.errorInfo, wxICON_ERROR);
				else							m_infobar->Dismiss();
				
				progressDlg.Show(false);
				Enable(true);
				return;
			}
			
			m_infobar->ShowMessage("The process will wait a second . . . ");
			THE_APP->waitActive(1000);
		}
		
		m_infobar->ShowMessage("Running the " + touchToken + " . . . ");
		
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
	
	if ( result.hasErrorInfo() )	m_infobar->ShowMessage(result.errorInfo, wxICON_ERROR);
	else							m_infobar->Dismiss();
	
	wxRichToolTip tip(touchToken, result.processResult ? "Touch was successful." : "Touch was failed!");
	tip.SetIcon(result.processResult ? wxICON_INFORMATION : wxICON_ERROR);
	tip.SetTipKind(wxTipKind_TopLeft);
	tip.ShowFor(btn);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::detachCameraCapture(wxCommandEvent& event) {
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
void CncReferencePosition::CncReferencePosition::updateCameraParameter(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	updateCameraParameter();
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::CncReferencePosition::updateCameraParameter() {
///////////////////////////////////////////////////////////////////
	if ( cameraCapture == NULL )
		return;
		
	cameraCapture->showHVCross(m_btShowCrossHV->GetValue());
	cameraCapture->showDDCross(m_btShowCrossDD->GetValue());
	cameraCapture->showRect(m_btShowRect->GetValue());
}
