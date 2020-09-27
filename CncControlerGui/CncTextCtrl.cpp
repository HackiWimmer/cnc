#include "OSD/CncAsyncKeyboardState.h"
#include "CncTextCtrl.h"

//////////////////////////////////////////////////////////////
CncTextCtrl::CncTextCtrl(wxWindow *parent, wxWindowID id, const wxString &value, const wxPoint &pos, const wxSize &size, 
						 long style, const wxValidator &validator, const wxString &name)
: wxTextCtrl(parent, wxID_ANY, value, pos, size, style, validator, name)
, overflowTimer(new wxTimer())
, lineBuffer{}
, index(0)
, loggedPos(0L)
//////////////////////////////////////////////////////////////
{
	overflowTimer->Connect(wxEVT_TIMER, wxTimerEventHandler(CncTextCtrl::onOverflowTimer), NULL, this);
	overflowTimer->Start(DEFAULT_OVERFLOW_PERIOD_VALUE);
}
//////////////////////////////////////////////////////////////
CncTextCtrl::~CncTextCtrl() {
//////////////////////////////////////////////////////////////
	if ( overflowTimer->IsRunning() )
		overflowTimer->Stop();
	
	overflowTimer->Disconnect(wxEVT_TIMER, wxTimerEventHandler(CncTextCtrl::onOverflowTimer), NULL, this);
	wxDELETE( overflowTimer );
}
//////////////////////////////////////////////////////////////
void CncTextCtrl::logCurrentPosition() {
//////////////////////////////////////////////////////////////
	loggedPos = GetLastPosition();
}
//////////////////////////////////////////////////////////////
long CncTextCtrl::getLoggedPosition() {
//////////////////////////////////////////////////////////////
	return loggedPos;
}
//////////////////////////////////////////////////////////////
bool CncTextCtrl::isLoggedPositionEqualCurrent() {
//////////////////////////////////////////////////////////////
	return loggedPos != GetLastPosition();
}
//////////////////////////////////////////////////////////////
bool CncTextCtrl::skipBackIfLoggedPositionEqualCurrent() {
//////////////////////////////////////////////////////////////
	if ( isLoggedPositionEqualCurrent() == false )
		return false;
		
	if ( loggedPos < 1 )
		return false;
		
	Remove(loggedPos - 1, loggedPos);
	return true;
}
//////////////////////////////////////////////////////////////
size_t CncTextCtrl::flush() {
//////////////////////////////////////////////////////////////
	return flushLineBuffer();
}
//////////////////////////////////////////////////////////////
size_t CncTextCtrl::flushLineBuffer() {
//////////////////////////////////////////////////////////////
	overflowTimer->Stop();
	
		const size_t len = index;
		if ( index > 0 ) {
			lineBuffer[index] = '\0';
			wxTextCtrl::AppendText(lineBuffer);
			index = 0;
		}
	
	overflowTimer->Start();
	return len;
}
//////////////////////////////////////////////////////////////
void CncTextCtrl::onOverflowTimer(wxTimerEvent& event) { 
//////////////////////////////////////////////////////////////
	flushLineBuffer();
	
	//event.Skip(); 
}
//////////////////////////////////////////////////////////////
bool CncTextCtrl::SetDefaultStyle(const wxTextAttr& style) {
//////////////////////////////////////////////////////////////
	if ( GetDefaultStyle().EqPartial(style, false) == true )
		;//return true;
	
	// if styles are different
	flushLineBuffer();
	return wxTextCtrl::SetDefaultStyle(style);
}
//////////////////////////////////////////////////////////////
void CncTextCtrl::AppendChar(char c) {
//////////////////////////////////////////////////////////////
	lineBuffer[index++] = c;
	
	if ( c == '\n' )
		flushLineBuffer();
		
	if ( index > MAX_LINE_BUFFER_SIZE - 8 )
		flushLineBuffer();
}
//////////////////////////////////////////////////////////////
void CncTextCtrl::AppendText(const wxString &text) {
//////////////////////////////////////////////////////////////
	// optimization
	if ( text.length() == 1 ) {
		AppendChar(text[0]);
		return;
	}
	
	// append what we have 
	flushLineBuffer();
	
	// default behaviour
	wxTextCtrl::AppendText(text);
}