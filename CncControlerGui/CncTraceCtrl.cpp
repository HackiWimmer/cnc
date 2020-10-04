#include "CncConfig.h"
#include "MainFrame.h"
#include "CncLoggerView.h"
#include "CncMessageDialog.h"
#include "CncTraceCtrl.h"

// ----------------------------------------------------------------------------
// CncSetterListCtrl Event Table
// ----------------------------------------------------------------------------
wxDEFINE_EVENT(wxEVT_CNC_TRACE_DISPLAY_TIMER,  wxTimerEvent);

wxBEGIN_EVENT_TABLE(CncTraceCtrl, wxTextCtrl)
	EVT_TIMER	(wxEVT_CNC_TRACE_DISPLAY_TIMER,	CncTraceCtrl::onDisplayTimer)
wxEND_EVENT_TABLE()

//////////////////////////////////////////////////////////////
CncTraceCtrl::CncTraceCtrl(wxWindow *parent, wxWindowID id, const wxString &value, const wxPoint &pos, const wxSize &size, 
						     long style, const wxValidator &validator, const wxString &name)
: CncTextCtrl	(parent, wxID_ANY, value, pos, size, style | wxTE_RICH | wxTE_READONLY | wxTE_MULTILINE | wxTE_DONTWRAP, validator, name)
, entries		()
, displayTimer	(this, wxEVT_CNC_TRACE_DISPLAY_TIMER)
//////////////////////////////////////////////////////////////
{
	wxFont font(9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));
	SetFont(font);
	
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
	SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));
	
	SetToolTip(_("Trace Information"));
	//Enable(false);
	
	displayTimer.Start(timerInterval);
}
//////////////////////////////////////////////////////////////
CncTraceCtrl::~CncTraceCtrl() {
//////////////////////////////////////////////////////////////
	displayTimer.Stop();
}
//////////////////////////////////////////////////////////////
void CncTraceCtrl::clearTraceHistory() {
//////////////////////////////////////////////////////////////
	entries.clear();
	CncTextCtrl::Clear();
	
	displayTimer.Start(timerInterval);
}
//////////////////////////////////////////////////////////////
void CncTraceCtrl::clearTrace(const wxString& timeStamp) {
//////////////////////////////////////////////////////////////
	const wxString line(GetValue());
	if ( line.IsEmpty() == false ) {
		entries.push_back(std::move(TraceEntry(timeStamp, line)));
		CncTextCtrl::Clear();
		
		displayTimer.Start(timerInterval);
	}
}
//////////////////////////////////////////////////////////////
bool CncTraceCtrl::SetDefaultStyle(const wxTextAttr& style) {
//////////////////////////////////////////////////////////////
	return CncTextCtrl::SetDefaultStyle(style);
}
//////////////////////////////////////////////////////////////
void CncTraceCtrl::AppendChar(char c) {
//////////////////////////////////////////////////////////////
	CncTextCtrl::AppendChar(c);
	
	displayTimer.Start(timerInterval);
}
//////////////////////////////////////////////////////////////
void CncTraceCtrl::AppendText(const wxString &text) {
//////////////////////////////////////////////////////////////
	CncTextCtrl::AppendText(text);
	
	displayTimer.Start(timerInterval);
}
//////////////////////////////////////////////////////////////
void CncTraceCtrl::openHistroyView() {
//////////////////////////////////////////////////////////////
	wxString content;
	for ( auto it = entries.begin(); it != entries.end(); ++it ) 
		content.append(wxString::Format("%s %s\n", it->timeStamp, it->text));

	// find the main window
	wxWindow* parent = this;
	while ( parent->GetParent() != NULL )
		parent = parent->GetParent();
		
	CncMessageDialog dlg(parent, content, "Complete Trace Content", "Cnc Trace Histrory");
	dlg.SetSize(800, 900);
	dlg.CenterOnParent();
	dlg.setWordWrap(false);
	dlg.ShowModal();
}
//////////////////////////////////////////////////////////////
void CncTraceCtrl::onDisplayTimer(wxTimerEvent& event) {
//////////////////////////////////////////////////////////////
	THE_APP->getLoggerView()->clearTrace();
}




