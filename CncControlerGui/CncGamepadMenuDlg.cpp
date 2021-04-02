#include <wx/uiaction.h>
#include "CncCommon.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "MainFrame.h"
#include "GamepadEvent.h"
#include "CncGamepadMenuDlg.h"

///////////////////////////////////////////////////////////////////
CncGamepadMenuDlg::CncGamepadMenuDlg(wxWindow* parent)
: CncGamepadMenuDlgBase(parent)
, prevUp		(false)
, prevDown		(false)
, prevLeft		(false)
, prevRight		(false)
, prevA			(false)
, prevB			(false)
///////////////////////////////////////////////////////////////////
{

}
///////////////////////////////////////////////////////////////////
CncGamepadMenuDlg::~CncGamepadMenuDlg() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void CncGamepadMenuDlg::close() {
///////////////////////////////////////////////////////////////////
	Show(false);
}
///////////////////////////////////////////////////////////////////
void CncGamepadMenuDlg::update(const GamepadEvent* state) {
///////////////////////////////////////////////////////////////////
	// .............................................................
	auto hitKey = [&](int keyCode, int modifier = wxMOD_NONE) {
		// simulate one hit only
		wxUIActionSimulator uai;
		
		uai.KeyDown(keyCode, modifier);
		uai.KeyUp  (keyCode, modifier);
	};
	
	// .............................................................
	auto switchFocus = [&](int direction) {
		if ( direction == 0 )
			return;
			
		wxWindow* fw = wxWindow::FindFocus();
		wxWindow* nw = NULL;
		
		if      ( fw && fw->GetParent() == m_panel1 )	{ direction > 0 ? nw = m_panel2 : nw = m_panel3; }
		else if ( fw && fw->GetParent() == m_panel2 )	{ direction > 0 ? nw = m_panel3 : nw = m_panel1; }
		else if ( fw && fw->GetParent() == m_panel3 )	{ direction > 0 ? nw = m_panel1 : nw = m_panel2; }
		else											{ nw = m_panel1;								 }
		
		if ( nw == NULL )
			return;
			
		for ( auto it = nw->GetChildren().begin(); it != nw->GetChildren().end(); ++it ) {
			
			wxWindow* nfw = dynamic_cast<wxWindow*>( *it );
			if ( nfw && nfw->IsFocusable() ) {
				nfw->SetFocus();
				break;
			}
		}
	};
	
	// .............................................................
	// avoid switch bouncing
	auto check = [&](bool& prev, bool curr) {
		
		if ( prev == false && curr == true )	prev = true;
		else									prev = false;
		
		return prev;
	};

	// .............................................................
	auto ckBtUp = [&](bool& prev, bool curr) {
		if ( prev == false && curr == false )
			return false;
		
		if      ( prev == true  && curr == false )	prev = false;
		else										prev = true;
		
		return !prev;
	};
	
	SetFocusFromKbd();
	
	// release corresponding key actions
	if ( ckBtUp(prevLeft,	state->data.buttonLeft)  )	{ switchFocus(-1);				return; }
	if ( ckBtUp(prevRight,	state->data.buttonRight) )	{ switchFocus(+1);				return; }
	if ( ckBtUp(prevUp,		state->data.buttonUp)    )	{ hitKey(WXK_UP);				return; }
	if ( ckBtUp(prevDown,	state->data.buttonDown)  )	{ hitKey(WXK_DOWN);				return; }
	
	if ( state->data.buttonLeftShoulder )				{ hitKey(WXK_TAB, wxMOD_SHIFT);	return; }
	if ( state->data.buttonRightShoulder )				{ hitKey(WXK_TAB);				return; }
	
	if ( ckBtUp(prevB, state->data.buttonB ))			{ hitKey(WXK_SPACE);			return; }
	if ( ckBtUp(prevA, state->data.buttonA ))			{ close();						return; }
	
	// add more necessary events
	// ....
}
///////////////////////////////////////////////////////////////////
void CncGamepadMenuDlg::onFunction(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	CNC_PRINT_FUNCT
	close();
}
///////////////////////////////////////////////////////////////////
void CncGamepadMenuDlg::onCancel(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	close();
}
///////////////////////////////////////////////////////////////////
void CncGamepadMenuDlg::onRunTemplate(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
	wxPostEvent(THE_APP->GetRcRun(), evt);
	
	close();
}
///////////////////////////////////////////////////////////////////
void CncGamepadMenuDlg::onReferencePosition(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
	wxPostEvent(THE_APP->GetRefPosition(), evt);
	
	close();
}
///////////////////////////////////////////////////////////////////
void CncGamepadMenuDlg::onPodestManagement(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxMenuEvent evt(wxEVT_COMMAND_MENU_SELECTED, THE_APP->m_miPodestManagement->GetId());
	wxPostEvent(THE_APP, evt);
	
	close();
}
