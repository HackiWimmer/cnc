#include "CncReferencePosition.h"
#include "CncCommon.h"
///////////////////////////////////////////////////////////////////
CncReferencePosition::CncReferencePosition(MainFrame* parent)
: CncReferencePositionBase(parent)
, parentFrame(parent)
///////////////////////////////////////////////////////////////////
{
	setMode(CncRefPositionMode::CncRM_Mode1);
	setMeasurePlateThickness(0.0);
}
///////////////////////////////////////////////////////////////////
CncReferencePosition::~CncReferencePosition() {
///////////////////////////////////////////////////////////////////
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
	wxString m(msg);
	if ( msg.IsEmpty() )
		m.assign("No additional information available");
		
	m_textMessage->ChangeValue(m);
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
void CncReferencePosition::navigateXPos(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(parentFrame);
	parentFrame->manualContinuousMoveStart(CncLinearDirection::CncPosDir, CncLinearDirection::CncNoneDir, CncLinearDirection::CncNoneDir);
	event.Skip(false);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::navigateXNeg(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(parentFrame);
	parentFrame->manualContinuousMoveStart(CncLinearDirection::CncNegDir, CncLinearDirection::CncNoneDir, CncLinearDirection::CncNoneDir);
	event.Skip(false);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::navigateYPos(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(parentFrame);
	parentFrame->manualContinuousMoveStart(CncLinearDirection::CncNoneDir, CncLinearDirection::CncPosDir, CncLinearDirection::CncNoneDir);
	event.Skip(false);
}
//////////////////////////////////////////////////////////////////
void CncReferencePosition::navigateYNeg(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(parentFrame);
	parentFrame->manualContinuousMoveStart(CncLinearDirection::CncNoneDir, CncLinearDirection::CncNegDir, CncLinearDirection::CncNoneDir);
	event.Skip(false);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::navigateZPos(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(parentFrame);
	parentFrame->manualContinuousMoveStart(CncLinearDirection::CncNoneDir, CncLinearDirection::CncNoneDir, CncLinearDirection::CncPosDir);
	event.Skip(false);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::navigateZNeg(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(parentFrame);
	parentFrame->manualContinuousMoveStart(CncLinearDirection::CncNoneDir, CncLinearDirection::CncNoneDir, CncLinearDirection::CncNegDir);
	event.Skip(false);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::navigateXNegYNeg(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(parentFrame);
	parentFrame->manualContinuousMoveStart(CncLinearDirection::CncNegDir, CncLinearDirection::CncNegDir, CncLinearDirection::CncNoneDir);
	event.Skip(false);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::navigateXNegYPos(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(parentFrame);
	parentFrame->manualContinuousMoveStart(CncLinearDirection::CncNegDir, CncLinearDirection::CncPosDir, CncLinearDirection::CncNoneDir);
	event.Skip(false);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::navigateXPosYNeg(wxMouseEvent& event){
///////////////////////////////////////////////////////////////////
	wxASSERT(parentFrame);
	parentFrame->manualContinuousMoveStart(CncLinearDirection::CncPosDir, CncLinearDirection::CncNegDir, CncLinearDirection::CncNoneDir);
	event.Skip(false);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::navigateXPosYPos(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(parentFrame);
	parentFrame->manualContinuousMoveStart(CncLinearDirection::CncPosDir, CncLinearDirection::CncPosDir, CncLinearDirection::CncNoneDir);
	event.Skip(false);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::navigateDClick(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(parentFrame);
	parentFrame->manualContinuousMoveStop();
	event.Skip(false);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::navigateKillFocus(wxFocusEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(parentFrame);
	parentFrame->manualContinuousMoveStop();
	event.Skip(false);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::navigateLeave(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(parentFrame);
	parentFrame->manualContinuousMoveStop();
	event.Skip(false);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::navigateLeftUp(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(parentFrame);
	parentFrame->manualContinuousMoveStop();
	event.Skip(false);
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
	wxPoint pos = {10, 60};
	SetPosition(pos);
	
	SetAffirmativeId(m_btSet->GetId());
	SetEscapeId(m_btCancel->GetId());
	
	wxFloatingPointValidator<float> val(2, NULL, wxNUM_VAL_DEFAULT );//, wxNUM_VAL_ZERO_AS_BLANK);
	val.SetRange(0.0, 100.0);
	val.SetPrecision(2);
	m_workpiceThickness->SetValidator(val);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::show(wxShowEvent& event) {
///////////////////////////////////////////////////////////////////
	m_rbStepSensitivity->SetSelection(parentFrame->GetRbStepSensitivity()->GetSelection());
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::cancel(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	SetReturnCode(wxID_CANCEL);
	Show(false);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::set(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( isWorkpieceThicknessNeeded() ) {
		if ( cnc::dblCompareNull(getWorkpieceThickness()) == true ) {
			wxMessageDialog dlg(this, 
								_T("For the choosen mode workpiece thickness cant be 0.0"), 
								_T("Invalid Workpiece Thickess  . . ."), 
								wxOK|wxICON_ERROR|wxCENTRE);
			dlg.ShowModal();
			return;
		}
	}
	
	SetReturnCode(wxID_OK);
	Show(false);
}
///////////////////////////////////////////////////////////////////
void CncReferencePosition::selectStepSensitivity(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( parentFrame == NULL)
		return;
		
	parentFrame->GetRbStepSensitivity()->SetSelection(m_rbStepSensitivity->GetSelection());
}
