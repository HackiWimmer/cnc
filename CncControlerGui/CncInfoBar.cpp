#include <wx/datetime.h>
#include "MainFrame.h"
#include "CncLoggerView.h"
#include "CncCommon.h"
#include "CncInfoBar.h"

// ----------------------------------------------------------------------------
// CncInfoBar Event Table
// ----------------------------------------------------------------------------
wxDEFINE_EVENT(wxEVT_INFO_BAR_DISPLAY_TIMER,  wxTimerEvent);

wxBEGIN_EVENT_TABLE(CncInfoBar, wxInfoBar)
	EVT_TIMER				(wxEVT_INFO_BAR_DISPLAY_TIMER,	CncInfoBar::onDisplayTimer		)
	EVT_LEFT_DCLICK			(								CncInfoBar::onLeftDClick		)
wxEND_EVENT_TABLE()

//////////////////////////////////////////////////
CncInfoBar::CncInfoBar(wxWindow *parent) 
: wxInfoBar				(parent)
, addTimeStampPrefix	(true)
, displayTimer			(this, wxEVT_INFO_BAR_DISPLAY_TIMER)
, appendix				()
, currentTimeStr		(wxDateTime::Now().FormatISOTime())
//////////////////////////////////////////////////
{
	// ... changing the efault colours and/or fonts
	const wxFont font = wxFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	const wxColour fg = wxColour( 64,  64,  64);
	const wxColour bg = wxColour(239, 228, 176);
	
	SetFont(font);
	SetEffectDuration(600);
	SetShowHideEffects(wxSHOW_EFFECT_ROLL_TO_BOTTOM, wxSHOW_EFFECT_ROLL_TO_TOP);
	SetOwnBackgroundColour(bg);
	SetForegroundColour(fg);
}
//////////////////////////////////////////////////
CncInfoBar::~CncInfoBar() {
//////////////////////////////////////////////////
}
//////////////////////////////////////////////////
void CncInfoBar::onDisplayTimer(wxTimerEvent& event) {
//////////////////////////////////////////////////
	Dismiss();
}
//////////////////////////////////////////////////
void CncInfoBar::ShowMessage(const wxString &msg, int flags) {
//////////////////////////////////////////////////
	char type = 'I';
	switch ( flags ) {
		case wxICON_WARNING:	type = 'W'; break;
		case wxICON_ERROR:		type = 'E'; break;
	}
	
	showMessage(type, msg);
}
//////////////////////////////////////////////////
void CncInfoBar::showMessage(const char type, const wxString& msg) {
//////////////////////////////////////////////////
	wxColour bgc = wxColour(239, 228, 176);
	int timeout  = -1;
	int flags    = 0;
	
	switch ( type )
	{
		case 'E':	flags	= wxICON_ERROR;
					timeout = 8000;
					bgc		= wxColour(255, 170, 170);
					break;
					
		case 'W':	flags	= wxICON_WARNING;
					timeout = 4000;
					bgc		= wxColour(239, 228, 176);
					break;
					
		default:	flags	= wxICON_INFORMATION;
					bgc		= wxColour(192, 192, 192);
					timeout = 2500;
					break;
	}
	
	wxString m(msg);
	traceFurther(type, m);
	
	currentTimeStr.assign(wxDateTime::Now().FormatISOTime());
	
	// msg: prepend time stamp
	if ( addTimeStampPrefix == true )
		m = wxString::Format("%s: %s", currentTimeStr, m);
	
	// msg: appendix
	if ( appendix.IsEmpty() == false )
		m.append(appendix);
		
	// msg: display
	SetOwnBackgroundColour(bgc);
	wxInfoBar::ShowMessage(m, flags);
	displayTimer.StartOnce(timeout);
}


//////////////////////////////////////////////////
void CncMainInfoBar::traceFurther(char type,  const wxString& msg) {
//////////////////////////////////////////////////
	switch ( type ) 
	{
		case 'E':	cnc::trc.logError(msg);
					break;
					
		case 'W':	cnc::trc.logWarning(msg);
					break;
					
		default:	cnc::trc.logInfo(msg);
					break;
	}
}
//////////////////////////////////////////////////
void CncMainInfoBar::onLeftDClick(wxMouseEvent& event) {
//////////////////////////////////////////////////
	THE_APP->getLoggerView()->openTraceHistory();
}