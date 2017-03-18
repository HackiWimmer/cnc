#include <iostream>
#include "EndSwitchDialog.h"

///////////////////////////////////////////////////////////////////
EndSwitchDialog::EndSwitchDialog(wxWindow* parent, CncControl* c)
: EndSwitchDialogBase(parent)
, X(false)
, Y(false)
, Z(false)
, cnc(c)
{
	wxASSERT(c != NULL);
	CncLimitStates ls = cnc->getLimitState();
	X = (ls.getXLimit() != LIMIT_UNSET);
	Y = (ls.getYLimit() != LIMIT_UNSET);
	Z = (ls.getZLimit() != LIMIT_UNSET);
	configureControls();
}
///////////////////////////////////////////////////////////////////
EndSwitchDialog::~EndSwitchDialog() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void EndSwitchDialog::configureControls() {
///////////////////////////////////////////////////////////////////
	m_XAxisEndSwitchBox->Enable(X);
	m_YAxisEndSwitchBox->Enable(Y);
	m_ZAxisEndSwitchBox->Enable(Z);
	
	m_btDissolveX->Enable(X);
	m_btDissolveY->Enable(Y);
	m_btDissolveZ->Enable(Z);
	
	wxString ls;
	wxString msg;
	if ( X == true || Y == true || Z == true ) 	msg += "Current end switch state isn't valid!\n";
	else										msg += "Current end switch state is valid\n";
		
	msg += "\n";
	msg += cnc->getLimitInfoString(ls);
	m_limitInfo->SetValue(msg);
}
///////////////////////////////////////////////////////////////////
void EndSwitchDialog::cancle(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	Close(true);
}
///////////////////////////////////////////////////////////////////
void EndSwitchDialog::evaluateState() {
///////////////////////////////////////////////////////////////////
	cnc->evaluateLimitState();
	CncLimitStates ls = cnc->getLimitState();
	if ( ls.getXLimit() == LIMIT_UNSET ) X = false;
	if ( ls.getYLimit() == LIMIT_UNSET ) Y = false;
	if ( ls.getZLimit() == LIMIT_UNSET ) Z = false;
	
	if ( X == true || Y == true || Z == true ) {
		cnc->reset();
	}
	configureControls();
}
///////////////////////////////////////////////////////////////////
void EndSwitchDialog::selectX(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	unsigned int sel = m_XAxisEndSwitchBox->GetSelection();
		
	if ( sel == 0 ) cnc->processSetter(PID_X_LIMIT, -1.0);
	else 			cnc->processSetter(PID_X_LIMIT, +1.0);

	cnc->evaluateLimitState();
}
///////////////////////////////////////////////////////////////////
void EndSwitchDialog::selectY(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	unsigned int sel = m_YAxisEndSwitchBox->GetSelection();
		
	if ( sel == 0 ) cnc->processSetter(PID_Y_LIMIT, -1.0);
	else 			cnc->processSetter(PID_Y_LIMIT, +1.0);

	cnc->evaluateLimitState();
}
///////////////////////////////////////////////////////////////////
void EndSwitchDialog::selectZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	unsigned int sel = m_ZAxisEndSwitchBox->GetSelection();
		
	if ( sel == 0 ) cnc->processSetter(PID_Z_LIMIT, -1.0);
	else 			cnc->processSetter(PID_Z_LIMIT, +1.0);

	cnc->evaluateLimitState();
}
///////////////////////////////////////////////////////////////////
void EndSwitchDialog::dissolveX(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( X == true ) {
		if ( m_XAxisEndSwitchBox->GetSelection() == 0 ) cnc->manualSimpleMoveSteps(+endSwitchStepBackMertic, 0.0, 0.0);
		else											cnc->manualSimpleMoveSteps(-endSwitchStepBackMertic, 0.0, 0.0);
	}
	evaluateState();
}
///////////////////////////////////////////////////////////////////
void EndSwitchDialog::dissolveY(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( Y == true ) {
		if ( m_YAxisEndSwitchBox->GetSelection() == 0 ) cnc->manualSimpleMoveSteps(0.0, +endSwitchStepBackMertic, 0.0);
		else											cnc->manualSimpleMoveSteps(0.0, -endSwitchStepBackMertic, 0.0);
	}
	evaluateState();
}
///////////////////////////////////////////////////////////////////
void EndSwitchDialog::dissolveZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( Z == true ) {
		if ( m_ZAxisEndSwitchBox->GetSelection() == 0 ) cnc->manualSimpleMoveSteps(0.0, 0.0, +endSwitchStepBackMertic);
		else											cnc->manualSimpleMoveSteps(0.0, 0.0, -endSwitchStepBackMertic);
	}
	evaluateState();
}
