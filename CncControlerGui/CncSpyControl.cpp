#include "wxcrafter.h"
#include "HexDecoder.h"
#include "DataControlModel.h"
#include "MainFrame.h"
#include "CncSpyControl.h"

wxBEGIN_EVENT_TABLE(CncSpyControl, wxTextCtrl)
    EVT_MOUSE_EVENTS(CncSpyControl::OnMouse)
    EVT_KEY_DOWN(CncSpyControl::OnKeyDown)
    //EVT_TIMER(SpinTimer, CncOpenGLDrawPane::OnSpinTimer)
wxEND_EVENT_TABLE()

///////////////////////////////////////////////////////////////////
CncSpyControl::CncSpyControl(wxWindow *parent, wxWindowID id) 
///////////////////////////////////////////////////////////////////
: wxTextCtrl(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH|wxTE_READONLY|wxTE_MULTILINE|wxALWAYS_SHOW_SB|wxHSCROLL|wxVSCROLL)
, sytleChanged(false)
{
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
	SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));
	wxFont m_serialSpyFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));
	SetFont(m_serialSpyFont);
	
	decodeSerialSpyLine("", true);
}
///////////////////////////////////////////////////////////////////
CncSpyControl::~CncSpyControl() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void CncSpyControl::Clear() {
///////////////////////////////////////////////////////////////////
	wxTextCtrl::Clear();
	clearDetails();
}
///////////////////////////////////////////////////////////////////
bool CncSpyControl::SetDefaultStyle(const wxTextAttr &style) {
///////////////////////////////////////////////////////////////////
	sytleChanged = true;
	return wxTextCtrl::SetDefaultStyle(style);
}
///////////////////////////////////////////////////////////////////
void CncSpyControl::addMarker(const wxString& mt) {
///////////////////////////////////////////////////////////////////
	cnc::spy.addMarker(mt);
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
void CncSpyControl::clearDetails() {
///////////////////////////////////////////////////////////////////
	THE_APP->GetSpyInboundDetails()->DeleteAllItems();
	THE_APP->GetSpyOutboundDetails()->DeleteAllItems();
	THE_APP->GetSpyUnknownDetails()->DeleteAllItems();
	
	THE_APP->GetSypDetailsBook()->SetSelection(SpyHexDecoder::Details::DT_UNKNOWN);
	
	decodeSerialSpyLine("", true);
}
///////////////////////////////////////////////////////////////////
void CncSpyControl::decodeSerialSpyLine(const wxString& line, bool displayInfo) {
///////////////////////////////////////////////////////////////////
	if ( IsFrozen() )
		return;
	
	struct Details : public SpyHexDecoder::Details {
		private:
			DcmRow row;
			
		public:
			
			///////////////////////////////////////////////////////////
			Details() 
			: SpyHexDecoder::Details()
			{
				THE_APP->GetSpyInboundDetails()->DeleteAllItems();
				THE_APP->GetSpyOutboundDetails()->DeleteAllItems();
				THE_APP->GetSpyUnknownDetails()->DeleteAllItems();
				
				THE_APP->GetSypDetailsBook()->SetSelection(DT_UNKNOWN);
				more.assign("No detail information available . . .");
			}
			
			///////////////////////////////////////////////////////////
			void display() {
				
				switch ( type ) {
					case DT_INBOUND: 	DataControlModel::addSpyInboundRow(row, 
																		   context, 
																		   pid, 
																		   index, 
																		   more);
										THE_APP->GetSpyInboundDetails()->AppendItem(row);
										THE_APP->GetSypDetailsBook()->SetSelection(DT_INBOUND);
										break;
										
					case DT_OUTBOUND: 	DataControlModel::addSpyOutboundRow(row, 
																			context,
																			cmd,
																			more);
										THE_APP->GetSpyOutboundDetails()->AppendItem(row);
										THE_APP->GetSypDetailsBook()->SetSelection(DT_OUTBOUND);
										break;
										
					default:			DataControlModel::addSpyDateilInfoRow(row, more);
										THE_APP->GetSpyUnknownDetails()->AppendItem(row);
										THE_APP->GetSypDetailsBook()->SetSelection(DT_UNKNOWN);
				}
			}
		
	} details;
	
	// determine detail type
	if 		( line.Contains("::<<") ) 	details.type = SpyHexDecoder::Details::DT_INBOUND;
	else if ( line.Contains("::>>") ) 	details.type = SpyHexDecoder::Details::DT_OUTBOUND;
	else								details.type = SpyHexDecoder::Details::DT_UNKNOWN;
	
	if ( details.type == SpyHexDecoder::Details::DT_UNKNOWN ) {
		details.display();
		return;
	}
		
	// general check
	if ( line.Find("0x[") == wxNOT_FOUND ) {
		if ( displayInfo == true ) {
			details.more.assign("Nothing to decode . . .\n");
			details.display();
		}
			
		return;
	}
	
	int s = line.Find('{');
	int e = line.Find('}');
	
	if ( s != wxNOT_FOUND && e == wxNOT_FOUND ) {
		details.more.assign("{....} not type value found . . .\n");
		details.display();
		return;
	}
	
	wxString contextInfo(line.SubString(s + 1, e - 1).Trim(true).Trim(false));
	
	s = line.Find('[');
	e = line.Find(']');
	
	if ( s != wxNOT_FOUND && e == wxNOT_FOUND ) {
		details.more.assign("[....] not HEX values found . . .\n");
		details.display();
		return;
	}
	
	wxString hexString = line.SubString(s + 1, e - 1).Trim(true).Trim(false);
	
	SpyHexDecoder shd(contextInfo, hexString);
	shd.decode(details);
	details.display();
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
	if ( event.LeftDown() ) {
		wxTextCoord col;
		wxTextCoord row;
		
		if ( HitTest(event.GetPosition(), &col, &row) != wxTE_HT_UNKNOWN ) {
			wxString line = GetLineText(row);
			decodeSerialSpyLine(line);
		}
	}
	
	event.Skip();
}
