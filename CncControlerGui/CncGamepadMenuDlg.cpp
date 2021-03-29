#include <wx/uiaction.h>
#include "CncCommon.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "GamepadEvent.h"
#include "CncGamepadMenuDlg.h"

///////////////////////////////////////////////////////////////////
CncGamepadMenuDlg::CncGamepadMenuDlg(wxWindow* parent)
: CncGamepadMenuDlgBase(parent)
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
	
	SetFocusFromKbd();
	
	/*
	if ( state->data.buttonStart == true ) {
		close();
		return;
	}
	*/

	// release corresponding key actions
	if ( state->data.buttonLeft )			{ switchFocus(-1);				return; }
	if ( state->data.buttonRight )			{ switchFocus(+1);				return; }
	if ( state->data.buttonUp )				{ hitKey(WXK_UP);				return; }
	if ( state->data.buttonDown )			{ hitKey(WXK_DOWN);				return; }
	
	if ( state->data.buttonLeftShoulder )	{ hitKey(WXK_TAB, wxMOD_SHIFT);	return; }
	if ( state->data.buttonRightShoulder )	{ hitKey(WXK_TAB);				return; }
	if ( state->data.buttonB )				{ hitKey(WXK_SPACE);			return; }
	
	// add more necessary events
	// ....
}
///////////////////////////////////////////////////////////////////
void CncGamepadMenuDlg::onFunction(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	CNC_PRINT_FUNCT
	close();
}
