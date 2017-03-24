#include "wxcrafter.h"
#include "HexDecoder.h"
#include "CncSpyControl.h"

wxBEGIN_EVENT_TABLE(CncSpyControl, wxTextCtrl)
    EVT_MOUSE_EVENTS(CncSpyControl::OnMouse)
    EVT_KEY_DOWN(CncSpyControl::OnKeyDown)
    //EVT_TIMER(SpinTimer, CncOpenGLDrawPane::OnSpinTimer)
wxEND_EVENT_TABLE()

///////////////////////////////////////////////////////////////////
CncSpyControl::CncSpyControl(wxWindow *parent, wxWindowID id, wxTextCtrl* dc) 
///////////////////////////////////////////////////////////////////
: wxTextCtrl(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH|wxTE_READONLY|wxTE_MULTILINE|wxALWAYS_SHOW_SB|wxHSCROLL|wxVSCROLL)
, sytleChanged(false)
, detailCtrl(dc)
{
	wxASSERT(detailCtrl);
	
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
	SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));
	wxFont m_serialSpyFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));
	SetFont(m_serialSpyFont);
}
///////////////////////////////////////////////////////////////////
CncSpyControl::~CncSpyControl() {
///////////////////////////////////////////////////////////////////
	
}
///////////////////////////////////////////////////////////////////
void CncSpyControl::Clear() {
///////////////////////////////////////////////////////////////////
	wxTextCtrl::Clear();
	detailCtrl->Clear();
}
///////////////////////////////////////////////////////////////////
bool CncSpyControl::SetDefaultStyle(const wxTextAttr &style) {
///////////////////////////////////////////////////////////////////
	sytleChanged = true;
	return wxTextCtrl::SetDefaultStyle(style);
}
///////////////////////////////////////////////////////////////////
void CncSpyControl::AppendText(const wxString & text) {
///////////////////////////////////////////////////////////////////
	sytleChanged = false;
	
	
	wxTextCtrl::AppendText(text);
}
///////////////////////////////////////////////////////////////////
void CncSpyControl::AppendText(const wxChar & c) {
///////////////////////////////////////////////////////////////////
	sytleChanged = false;
	
	
	
	wxTextCtrl::AppendText(c);
}
///////////////////////////////////////////////////////////////////
void CncSpyControl::decodeSerialSpyLine(const wxString& line) {
///////////////////////////////////////////////////////////////////
	detailCtrl->Clear();
	detailCtrl->AppendText(line);
	detailCtrl->AppendText("\n");
	
	if ( line.Find("0x[") == wxNOT_FOUND ) {
		detailCtrl->AppendText("Nothing more to decode . . .\n");
		return;
	}
	
	int s = line.Find('{');
	int e = line.Find('}');
	
	if ( s != wxNOT_FOUND && e == wxNOT_FOUND ) {
		detailCtrl->AppendText("{....} not type value found . . .\n");
		return;
	}
	
	wxString type = line.SubString(s + 1, e - 1).Trim(true).Trim(false);
	
	s = line.Find('[');
	e = line.Find(']');
	
	if ( s != wxNOT_FOUND && e == wxNOT_FOUND ) {
		detailCtrl->AppendText("[....] not HEX values found . . .\n");
		return;
	}
	
	wxString hexString = line.SubString(s + 1, e - 1).Trim(true).Trim(false);
	
	SpyHexDecoder shd(type, hexString);
	detailCtrl->AppendText(shd.decode());
}
/////////////////////////////////////////////////////////////////////
void CncSpyControl::OnKeyDown(wxKeyEvent& event) {
/////////////////////////////////////////////////////////////////////
	long row, col;
	PositionToXY(GetInsertionPoint(), &col, &row);
	
	switch ( event.GetKeyCode() ) {
		case WXK_RIGHT:
		case WXK_LEFT:	event.Skip(true); 
						return;
						
		case WXK_DOWN: 	row++;
						break;
						
		case WXK_UP: 	row--;
						break;
	}
	
	if ( row < 0 )
		row = 0;
		
	if ( row > GetNumberOfLines() -1 )
		row = GetNumberOfLines() -1;
	
	decodeSerialSpyLine(GetLineText(row));
	event.Skip(true);
}
/////////////////////////////////////////////////////////////////////
void CncSpyControl::OnMouse(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxTextCoord col;
	wxTextCoord row;
	
	if ( HitTest(event.GetPosition(), &col, &row) != wxTE_HT_UNKNOWN ) {
		wxString line = GetLineText(row);
		decodeSerialSpyLine(line);
	}
	
	event.Skip();
}
