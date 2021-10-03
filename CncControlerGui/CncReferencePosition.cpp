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
: CncReferencePositionBase	(parent)
, valid						(false)
, navigationPanel			(NULL)
, referencePanel			(NULL)
, infoMessage				()
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
	
	referencePanel = new CncReferenceEvaluation(this);
	GblFunc::replaceControl(m_evaluateReferencePlaceholder, referencePanel);
	
	m_rbStepSensitivity->SetFocusFromKbd();
	referencePanel->updatePreview();
	
	this->Bind(wxEVT_CNC_NAVIGATOR_PANEL, 	&CncReferencePosition::onNavigatorPanel, this);
}
///////////////////////////////////////////////////////////////////
CncReferencePosition::~CncReferencePosition() {
///////////////////////////////////////////////////////////////////
	this->Unbind(wxEVT_CNC_NAVIGATOR_PANEL, &CncReferencePosition::onNavigatorPanel, this);
	
	wxDELETE(navigationPanel);
	wxDELETE(referencePanel);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::setMessage(const wxString& msg) {
///////////////////////////////////////////////////////////////////
	infoMessage.assign(msg);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::onNavigatorPanel(CncNavigatorPanelEvent& event) {
///////////////////////////////////////////////////////////////////
	// redirect to main frame . . .
	APP_PROXY::postEvent(new CncNavigatorPanelEvent(event));
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
	
	referencePanel->init();
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
	referencePanel->cancel();
	EndModal(wxID_CANCEL);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::set(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	referencePanel->set();
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
void CncReferencePosition::onInfoTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	m_infoTimer->Stop();
	showInformation();
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::resetTempSetting() {
///////////////////////////////////////////////////////////////////
	referencePanel->resetTempSetting();
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::setEnforceFlag(bool s) {
///////////////////////////////////////////////////////////////////
	valid = !s;
	
	const int refMode = (int)referencePanel->getReferenceMode();
	
	wxBitmap bmp(ImageLib24().Bitmap( valid ? "BMP_TRAFFIC_LIGHT_GREEN" : "BMP_TRAFFIC_LIGHT_RED")); 
	const wxString mod(wxString::Format("Reference position mode: %s [%d]", cnc::getReferenceModeAsString(referencePanel->getReferenceMode()), ( valid ? refMode : -1 ) ));
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
void CncReferencePosition::onKillCtrlFocus(wxFocusEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip();
	
	auto modify = [](wxWindow* wnd) {
		wnd->SetBackgroundColour(wnd->GetDefaultAttributes().colBg);
	};
	
	modify(m_rbStepSensitivity);
	modify(m_rbStepMode);
	
	#warning
	//(modify(m_cbPrevTest);
	//modify(m_notebookEvalMode);
	
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
	#warning
	//modify(m_cbPrevTest);
	//modify(m_notebookEvalMode);
	
	Refresh();
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::hitKey(int keyCode, int modifier) {
///////////////////////////////////////////////////////////////////
	wxWindow* focusWnd = wxWindow::FindFocus();
	
	#warning
	/*
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
	*/
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

