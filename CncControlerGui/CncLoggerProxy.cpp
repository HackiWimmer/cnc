#include "OSD/CncAsyncKeyboardState.h"
#include "CncCommon.h"
#include "CncConfig.h"
#include "MainFrame.h"
#include "GlobalFunctions.h"
#include "CncLoggerProxy.h"

namespace CncTC {
	const unsigned int defaultOverflowTimerValue = 800;
	const unsigned int maxLineBufferLengrh		 = 500;
};

//////////////////////////////////////////////////////////////
CncTextCtrl::CncTextCtrl(wxWindow *parent, wxWindowID id, const wxString &value, const wxPoint &pos, const wxSize &size, 
						 long style, const wxValidator &validator, const wxString &name)
: wxTextCtrl(parent, wxID_ANY, value, pos, size, style, validator, name)
, overflowTimer(new wxTimer())
, lineBuffer()
, lastAppend(CncTimeFunctions::getNanoTimestamp())
//////////////////////////////////////////////////////////////
{
	setupTimer(CncTC::defaultOverflowTimerValue);
}
//////////////////////////////////////////////////////////////
void CncTextCtrl::setupTimer(int interval) {
//////////////////////////////////////////////////////////////
	overflowTimer->Connect(wxEVT_TIMER, wxTimerEventHandler(CncTextCtrl::onOverflowTimer), NULL, this);
	overflowTimer->Start(interval);
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
size_t CncTextCtrl::flush() {
//////////////////////////////////////////////////////////////
	return flushLineBuffer();
}
//////////////////////////////////////////////////////////////
size_t CncTextCtrl::flushLineBuffer() {
//////////////////////////////////////////////////////////////
	const size_t len = lineBuffer.length();
	
	if ( lineBuffer.IsEmpty() == false ) {
		wxTextCtrl::AppendText(lineBuffer);
		lineBuffer.clear();
	}
	
	// restart the timer
	overflowTimer->Start();
	
	return len;
}
//////////////////////////////////////////////////////////////
void CncTextCtrl::onOverflowTimer(wxTimerEvent& event) { 
//////////////////////////////////////////////////////////////
	const unsigned int threshold = (CncTC::defaultOverflowTimerValue / 2 ) * 1000 * 1000;
	
	if ( CncTimeFunctions::getNanoTimestamp() - lastAppend > threshold )
		flushLineBuffer();
	
	event.Skip(); 
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
	lastAppend = CncTimeFunctions::getNanoTimestamp();
	lineBuffer.append(c);
	
	if ( c == '\n' )
		flushLineBuffer();
		
	if ( lineBuffer.length() > CncTC::maxLineBufferLengrh )
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
	
	// append what we have before
	lastAppend = CncTimeFunctions::getNanoTimestamp();
	flushLineBuffer();
	
	// default behaviour
	wxTextCtrl::AppendText(text);
}




// ----------------------------------------------------------------------------
// CncLoggerProxy Event Table
// ----------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(CncLoggerProxy, wxTextCtrl)
	EVT_KEY_DOWN(CncLoggerProxy::onKeyDown)
	EVT_LEFT_DCLICK(CncLoggerProxy::onLeftDClick)
	EVT_TEXT(wxID_ANY, CncLoggerProxy::onUpdateLogger)
wxEND_EVENT_TABLE()

//////////////////////////////////////////////////////////////
CncLoggerProxy::CncLoggerProxy(wxWindow *parent, wxWindowID id, const wxString &value, const wxPoint &pos, const wxSize &size, 
						       long style, const wxValidator &validator, const wxString &name)
: CncTextCtrl(parent, wxID_ANY, value, pos, size, style, validator, name)
, showOnDemandState(true)
//////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////
CncLoggerProxy::~CncLoggerProxy() {
//////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////
void CncLoggerProxy::onKeyDown(wxKeyEvent& event) {
//////////////////////////////////////////////////////////////
	bool ctlKey = CncAsyncKeyboardState::isControlPressed();
	const int c = event.GetUnicodeKey();
	
	if ( c == 'C' && ctlKey == true ) {
		bool unselect = false;
		
		if ( HasSelection() == false ) {
			SelectAll();
			unselect = true;
		}
		
		Copy();
		
		if ( unselect == true )
			SelectNone();
			
		Refresh();
	}
	
	event.Skip(true);
}
//////////////////////////////////////////////////////////////
void CncLoggerProxy::onLeftDClick(wxMouseEvent& event) {
//////////////////////////////////////////////////////////////
	long pos = GetInsertionPoint(), col = -1, row = -1;
	if ( PositionToXY(pos, &col, &row) == false )
		return;
		
	wxString line(GetLineText(row));
	line.Trim(false);
	line.MakeUpper();
	
	if ( line.StartsWith('[') == true ) {
		line.assign(line.BeforeFirst(']'));
		line.assign(line.AfterFirst('['));
		
	} else if ( line.Contains("LINE") == true ) {
		int p = line.Find("LINE");
		bool start = false;
		wxString ln;
		for (unsigned int i=p; i<line.length(); i++ ) {
			
			if ( start == false && isdigit((char)line[i]) != 0 )
				start = true;
			
			if ( start == true && isdigit((char)line[i]) == 0 )
				break;
				
			if ( start == true )
				ln.append(line[i]);
		}
		
		line.assign(ln);
		
	} else {
		
		return;
	}
	
	long lineNumber = -1;
	if ( line.ToLong(&lineNumber) == false )
		return;
	
	if ( CNC_READY )
		THE_APP->selectSourceControlLineNumber(lineNumber - 1);
}
//////////////////////////////////////////////////////////////
void CncLoggerProxy::onUpdateLogger(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////
	if ( showOnDemandState == false )
		return;

	if ( IsShown() == true && IsShownOnScreen() == false ) {
		if ( CNC_READY ) {
			THE_APP->showAuiPane("Logger");
			THE_APP->GetLoggerNotebook()->SetSelection(LoggerSelection::VAL::CNC);
		}
	}
}
