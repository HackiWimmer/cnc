#include <wx/gdicmn.h>
#include <wx/richtooltip.h>
#include "CncReferencePosition.h"
#include "GlobalFunctions.h"
#include "MainFrameProxy.h"
#include "CncCommon.h"
///////////////////////////////////////////////////////////////////
CncReferencePosition::CncReferencePosition(wxWindow* parent)
: CncReferencePositionBase(parent)
, navigationPanel(NULL)
, infoMessage()
///////////////////////////////////////////////////////////////////
{
	setMode(CncRefPositionMode::CncRM_Mode1);
	setMeasurePlateThickness(0.0);
	
	//navigation panel control
	CncNavigatorPanel::Config cfg;
	cfg.innerCircle = true;
	cfg.shortFormat = true;
	cfg.alignment	= wxALIGN_RIGHT;
	cfg.initToolTipMapAsCoordSytem();
	
	navigationPanel = new CncNavigatorPanel(this, cfg);
	GblFunc::replaceControl(m_navigationPanelPlaceholder, navigationPanel);
	
	this->Bind(wxEVT_CNC_NAVIGATOR_PANEL, 	&CncReferencePosition::onNavigatorPanel, 	this);
}
///////////////////////////////////////////////////////////////////
CncReferencePosition::~CncReferencePosition() {
///////////////////////////////////////////////////////////////////
	this->Unbind(wxEVT_CNC_NAVIGATOR_PANEL, &CncReferencePosition::onNavigatorPanel, 	this);
}
///////////////////////////////////////////////////////////////////
bool CncReferencePosition::isWorkpieceThicknessNeeded() {
///////////////////////////////////////////////////////////////////
	short mode = evaluateMode();
	return ( mode == CncRefPositionMode::CncRM_Mode3 || mode == CncRefPositionMode::CncRM_Mode3 );
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::setMessage(const wxString& msg) {
///////////////////////////////////////////////////////////////////
	infoMessage.assign(msg);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::setMeasurePlateThickness(const double mpt) {
///////////////////////////////////////////////////////////////////
	m_measurePlateThickness->SetValue(wxString::Format("%1.2lf", mpt));
}
///////////////////////////////////////////////////////////////////
CncRefPositionMode CncReferencePosition::getReferenceMode() {
///////////////////////////////////////////////////////////////////
	return (CncRefPositionMode)evaluateMode();
}
///////////////////////////////////////////////////////////////////
double CncReferencePosition::getWorkpieceThickness() {
///////////////////////////////////////////////////////////////////
	const wxString wpt = m_workpiceThickness->GetValue();
	double ret = 0.0;
	
	if ( isWorkpieceThicknessNeeded() )
		wpt.ToDouble(&ret);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::shiftReferenceMode() {
///////////////////////////////////////////////////////////////////
	short mode = evaluateMode();
	
	m_btMode1->SetValue(false);
	m_btMode2->SetValue(false);
	m_btMode3->SetValue(false);
	m_btMode4->SetValue(false);
	m_btMode5->SetValue(false);
	m_btMode6->SetValue(false);
	
	if ( mode == CncRefPositionMode::CncRM_Mode6 ) 	mode = 1;
	else											mode++;
	
	setMode(mode);
}
///////////////////////////////////////////////////////////////////
short CncReferencePosition::evaluateMode() {
///////////////////////////////////////////////////////////////////
	if (      m_btMode1->GetValue() == true ) return 1;
	else if ( m_btMode2->GetValue() == true ) return 2;
	else if ( m_btMode3->GetValue() == true ) return 3;
	else if ( m_btMode4->GetValue() == true ) return 4;
	else if ( m_btMode5->GetValue() == true ) return 5;
	else if ( m_btMode6->GetValue() == true ) return 6;
	
	return -1;
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
												break;
		case CncRefPositionMode::CncRM_Mode2: 	hint.assign("Reference: Ground plate\nDon't consider any workpiece thickness\nWith measure plate"); 
												m_workpiceThickness->Enable(false);
												break;
		case CncRefPositionMode::CncRM_Mode3: 	hint.assign("Reference: Ground plate\nConsider workpiece thickness\nWithout measure plate"); 
												m_workpiceThickness->Enable(true);
												break;
		case CncRefPositionMode::CncRM_Mode4: 	hint.assign("Reference: Ground plate\nConsider workpiece thickness\nWith measure plate"); 
												m_workpiceThickness->Enable(true);
												break;
		case CncRefPositionMode::CncRM_Mode5: 	hint.assign("Reference: Workpiece surface\nDon't consider any workpiece thickness\nWithout measure plate"); 
												m_workpiceThickness->Enable(false);
												break;
		case CncRefPositionMode::CncRM_Mode6: 	hint.assign("Reference: Workpiece surface\nDon't consider any workpiece thickness\nWith measure plate"); 
												m_workpiceThickness->Enable(false);
												break;
	}
	
	m_modeText->SetLabel(hint);
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
	
	wxFloatingPointValidator<float> val(2, NULL, wxNUM_VAL_DEFAULT );//, wxNUM_VAL_ZERO_AS_BLANK);
	val.SetRange(0.0, 100.0);
	val.SetPrecision(2);
	m_workpiceThickness->SetValidator(val);
	
	m_btZeroX->SetValue(true);
	m_btZeroY->SetValue(true);
	m_btZeroZ->SetValue(true);
	determineZeroMode();
	
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
	
	EndModal(wxID_OK);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::selectStepSensitivity(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( APP_PROXY::isAppPointerAvailable() == true )
		APP_PROXY::GetRbStepSensitivity()->SetSelection(m_rbStepSensitivity->GetSelection());
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::selectStepMode(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( APP_PROXY::isAppPointerAvailable() == true )
		APP_PROXY::GetRbStepMode()->SetSelection(m_rbStepMode->GetSelection());
		
	const CncStepMode sm = m_rbStepMode->GetSelection() == 0 ? SM_INTERACTIVE : SM_STEPWISE;
	navigationPanel->setStepMode(sm);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::showInformation() {
///////////////////////////////////////////////////////////////////
	static wxBitmap bmp(m_infoBitmap->GetBitmap());
	
	if ( infoMessage.IsEmpty() == false ) {
		m_infoBitmap->Enable(true);
		m_infoBitmap->SetBitmap(bmp);
		
		wxRichToolTip tip("Additional information", infoMessage);
		
		tip.SetIcon(wxICON_INFORMATION);
		tip.SetTipKind(wxTipKind_TopLeft);
		tip.ShowFor(m_btSet);
	} 
	else {
		m_infoBitmap->Enable(false);
		
		wxBitmap disabled(bmp);
		m_infoBitmap->SetBitmap(disabled.ConvertToDisabled());
	}
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::selectInformation(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_infoBitmap->IsEnabled() )
		showInformation();
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
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::onInfoTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	m_infoTimer->Stop();
	showInformation();
}
