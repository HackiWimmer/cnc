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
#include "CncReferencePosition.h"

///////////////////////////////////////////////////////////////////
CncReferencePosition::CncReferencePosition(wxWindow* parent)
: CncReferencePositionBase	(parent)
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
	referencePanel->setCallbackInterface(this);
	
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
/////////////////////////////////////////////////////////////////////
void CncReferencePosition::referenceNotifyMessage(const wxString& msg, int flags) {
//////////// /////////////////////////////////////////////////////////
	wxString m(msg);
	while ( m.EndsWith("\n") )
		m.RemoveLast();
	
	m_infobar->ShowMessage(m, flags);
}
/////////////////////////////////////////////////////////////////////
void CncReferencePosition::referenceDismissMessage() {
/////////////////////////////////////////////////////////////////////
	//CNC_PRINT_FUNCT
	m_infobar->Dismiss();
	Layout();
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
void CncReferencePosition::onKillCtrlFocus(wxFocusEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip();
	
	auto modify = [](wxWindow* wnd) {
		wnd->SetBackgroundColour(wnd->GetDefaultAttributes().colBg);
	};
	
	modify(m_rbStepSensitivity);
	modify(m_rbStepMode);
	
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
	
	Refresh();
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::hitKey(int keyCode, int modifier) {
///////////////////////////////////////////////////////////////////
	#warning Is this longer necessary ? (Nov 2021)
	/*
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

