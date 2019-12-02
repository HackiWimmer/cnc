#include "GlobalFunctions.h"
#include "CncExternalViewBox.h"

//////////////////////////////////////////////////////////////////
CncExternalViewBox::CncExternalViewBox(wxWindow* parent, wxWindow* source, long style)
: CncExternalViewBoxBase(parent, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(500, 300), wxSTAY_ON_TOP | style)
, guiSensitivity(true)
, swapState(SS_DEFAULT)
, sourceCtrl(source)
, moveDelta()
//////////////////////////////////////////////////////////////////
{
	wxASSERT( sourceCtrl != NULL );
}
//////////////////////////////////////////////////////////////////
CncExternalViewBox::~CncExternalViewBox() {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::swapControls() {
//////////////////////////////////////////////////////////////////
	wxASSERT( sourceCtrl != NULL );
	
	if ( swapState == SS_DEFAULT ) {
		GblFunc::swapControls(m_placeholder, sourceCtrl);
		swapState = SS_SWAPED;

		SetClientSize(GetDefaultSize());
		
	} else {
		GblFunc::swapControls(sourceCtrl, m_placeholder);
		swapState = SS_DEFAULT;
	}
	
	Update();
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::onShow(wxShowEvent& event) {
//////////////////////////////////////////////////////////////////
	wxString title(GetTitle());
	m_windowTitle->SetLabel(title);
	
	if 		( event.IsShown() == true  && swapState == SS_DEFAULT )	swapControls();
	else if ( event.IsShown() == false && swapState == SS_DEFAULT )	; // do nothing
	else if ( event.IsShown() == true  && swapState == SS_SWAPED )	; // do nothing
	else if ( event.IsShown() == false && swapState == SS_SWAPED )	swapControls();
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::onCloseFromButton(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	if ( guiSensitivity == false )
		return;

	Show(false);
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::onMinMax(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	if ( guiSensitivity == false )
		return;
	
	if ( IsMaximized() == false )	Maximize();
	else							Restore();
	
	m_btMinMax->SetToolTip( IsMaximized() ? "Minimize" : "Maximize" );
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::onStartMove(wxMouseEvent& event) {
//////////////////////////////////////////////////////////////////
	m_moveArea->CaptureMouse();
	
	if ( m_moveArea->HasCapture() ) {
		const wxPoint pos = ::wxGetMousePosition();
		const wxPoint origin = this->GetPosition();
		
		int dx = pos.x - origin.x;
		int dy = pos.y - origin.y;  
		
		moveDelta = wxPoint(dx, dy);
	}
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::onStopMove(wxMouseEvent& event) {
//////////////////////////////////////////////////////////////////
	if ( m_moveArea->HasCapture() )
		m_moveArea->ReleaseMouse();
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::onMotion(wxMouseEvent& event) {
//////////////////////////////////////////////////////////////////
	if ( m_moveArea->HasCapture() ) {
		if ( event.Dragging() && event.LeftIsDown() ) {
			const wxPoint pos = ::wxGetMousePosition();
			Move(wxPoint(pos.x - moveDelta.x, pos.y - moveDelta.y));
		}
	}
}
