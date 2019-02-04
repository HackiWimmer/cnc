#include <wx/datetime.h>
#include "MainFrame.h"
#include "CncConfig.h"
#include "CncSourceEditor.h"
#include "CncTemplateObserver.h"

//////////////////////////////////////////////////////////////
CncTemplateObserver::CncTemplateObserver(wxWindow* parent)
: CncTemplateObserverBase(parent)
, observationActive(false)
, styles()
, lastTemplateModification(wxDateTime::UNow())
//////////////////////////////////////////////////////////////
{
	m_observerTrace->Clear();
}
//////////////////////////////////////////////////////////////
CncTemplateObserver::~CncTemplateObserver() {
//////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////
void CncTemplateObserver::clearTrace(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////
	m_observerTrace->Clear();
}
//////////////////////////////////////////////////////////////
void CncTemplateObserver::logTimestamp() {
//////////////////////////////////////////////////////////////
	wxDateTime s = wxDateTime::UNow();
	m_lastTimestamp->ChangeValue(s.FormatISOTime());
}
//////////////////////////////////////////////////////////////
void CncTemplateObserver::logInformation(const wxString& msg, const wxTextAttr ta) {
//////////////////////////////////////////////////////////////
	m_observerTrace->SetDefaultStyle(ta);
	m_observerTrace->AppendText(wxString::Format("%s %s", wxDateTime::UNow().FormatISOTime(), msg));
}
//////////////////////////////////////////////////////////////
void CncTemplateObserver::logInformation(const wxString& msg) {
//////////////////////////////////////////////////////////////
	logInformation(msg, styles.taInfo);
}
//////////////////////////////////////////////////////////////
void CncTemplateObserver::logWarning(const wxString& msg) {
//////////////////////////////////////////////////////////////
	m_observerTrace->SetDefaultStyle(styles.taWarning);
	m_observerTrace->AppendText(wxString::Format("%s %s", wxDateTime::UNow().FormatISOTime(), msg));
}
//////////////////////////////////////////////////////////////
void CncTemplateObserver::logError(const wxString& msg) {
//////////////////////////////////////////////////////////////
	m_observerTrace->SetDefaultStyle(styles.taError);
	m_observerTrace->AppendText(wxString::Format("%s %s", wxDateTime::UNow().FormatISOTime(), msg));
}
//////////////////////////////////////////////////////////////
void CncTemplateObserver::activateSourecEditor(bool state) {
//////////////////////////////////////////////////////////////
	if ( state == true )	logInformation("Activate source editor . . .\n",   styles.taActivate);
	else					logInformation("Deactivate source editor . . .\n", styles.taDeactivate);
	
	THE_APP->sourceEditor->Enable(state);
}
//////////////////////////////////////////////////////////////
void CncTemplateObserver::changeObservationMode(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////
	observationActive = m_observationMode->GetValue();
	
	activateSourecEditor(!observationActive);
	changeObservation(observationActive);
}
//////////////////////////////////////////////////////////////
void CncTemplateObserver::enableObservationTimer(bool state) {
//////////////////////////////////////////////////////////////
	if ( state == true ) {
		if ( m_observeTimer->IsRunning() == true )
			return;
			
		m_observeTimer->Start(1000, false);
		
	} else {
		if ( m_observeTimer->IsRunning() == false )
			return;
			
		m_observeTimer->Stop();
	}
}
//////////////////////////////////////////////////////////////
void CncTemplateObserver::suspendObservation() {
//////////////////////////////////////////////////////////////
	wxString fn(THE_APP->getCurrentTemplatePathFileName());
	logInformation(wxString::Format("Suspending observation for  : '%s'\n", fn), styles.taSuspend);
	
	m_lastTimestamp->ChangeValue("Suspended");
	enableObservationTimer(false);
}
//////////////////////////////////////////////////////////////
void CncTemplateObserver::reconstructObservation() {
//////////////////////////////////////////////////////////////
	if ( observationActive == true ) {
		wxString fn(THE_APP->getCurrentTemplatePathFileName());
		logInformation(wxString::Format("Reconstruct observation for : '%s'\n", fn), styles.taReconstruct);
		
		logTimestamp();
		enableObservationTimer(true);
	}
}
//////////////////////////////////////////////////////////////
void CncTemplateObserver::startObservation() {
//////////////////////////////////////////////////////////////
	wxString fn(THE_APP->getCurrentTemplatePathFileName());
	logInformation(wxString::Format("Starting observation for    : '%s'\n", fn), styles.taStart);
	
	logTemplateModificationTimeStamp();
	
	logTimestamp();
	
	THE_APP->reloadTemplate(TemplateBookSelection::VAL::OBSERVER);
	THE_APP->selectMonitorBookTemplatePanel();
	
	enableObservationTimer(true);
}
//////////////////////////////////////////////////////////////
void CncTemplateObserver::stopObservation() {
//////////////////////////////////////////////////////////////
	wxString fn(THE_APP->getCurrentTemplatePathFileName());
	logInformation(wxString::Format("Stopping observation for    : '%s'\n", fn), styles.taEnd);
	
	m_lastTimestamp->ChangeValue("Inactive");
	enableObservationTimer(false);
}
//////////////////////////////////////////////////////////////
void CncTemplateObserver::changeObservation(bool state) {
//////////////////////////////////////////////////////////////
	if ( observationActive == true )	startObservation();
	else								stopObservation();
}
///////////////////////////////////////////////////////////////////
void CncTemplateObserver::logTemplateModificationTimeStamp() {
///////////////////////////////////////////////////////////////////
	wxString fn(THE_APP->getCurrentTemplatePathFileName());
	wxFileName tplFile(fn);
	
	if ( tplFile.Exists() == false ) {
		lastTemplateModification = wxDateTime::UNow();
		return;
	}
	
	lastTemplateModification = tplFile.GetModificationTime();
}
//////////////////////////////////////////////////////////////
bool CncTemplateObserver::isCurrentTemplateChanged() {
//////////////////////////////////////////////////////////////
	wxString fn(THE_APP->getCurrentTemplatePathFileName());
	wxFileName tplFile(fn);
	
	if ( tplFile.Exists() == false ) {
		logWarning(wxString::Format("The template file '%s' didn'i exists!\n", fn));
		return false;
	}
	
	wxDateTime dt = tplFile.GetModificationTime();
	
	bool ret = false;
	if ( dt != lastTemplateModification ) {
		logInformation(wxString::Format("An externally template change is detected . . .\n\t   Name              : %s\n\t   Last Modification : %s\n\t   File size         : %s\n",
										 tplFile.GetFullPath(),
										 tplFile.GetModificationTime().FormatISOCombined(' '),
										 tplFile.GetHumanReadableSize()
									   ), styles.taDetected);
		
		lastTemplateModification = dt;
		ret = true;
	}
	
	return ret;
}
//////////////////////////////////////////////////////////////
void CncTemplateObserver::enableControls(bool state) {
//////////////////////////////////////////////////////////////
	m_observationMode->Enable(state);
	m_actionSelection->Enable(state);
}
//////////////////////////////////////////////////////////////
void CncTemplateObserver::observe(wxTimerEvent& event) {
//////////////////////////////////////////////////////////////
	logTimestamp();
	
	if ( isCurrentTemplateChanged() == true )
		performActions();
}
//////////////////////////////////////////////////////////////
void CncTemplateObserver::performActions() {
//////////////////////////////////////////////////////////////
	const int ACTION_PREVIEW		= 1;
	const int ACTION_RUN			= 2;
	const int ACTION_DEBUG			= 3;
	
	wxString fn(THE_APP->getCurrentTemplatePathFileName());
	
	if ( THE_APP->openFile(TemplateBookSelection::VAL::OBSERVER) == false ) {
		logError(wxString::Format("CncTemplateObserver::performActions() Can't open template '%s'\n", fn));
		
	} else {
	
		switch ( m_actionSelection->GetSelection() ) {
			
			case ACTION_PREVIEW:	THE_APP->selectMonitorBookTemplatePanel();
									logInformation(wxString::Format(" --> Action: '%s' performed\n", "Update Preview"), styles.taAction);
									break;
									
			case ACTION_RUN:		
			case ACTION_DEBUG:		
									if ( THE_APP->cnc->isEmulator() == false ) {
										logError(wxString::Format(" --> This action isn't allowed for none emulator serials\n", fn));
										m_actionSelection->SetSelection(ACTION_PREVIEW);
										
									} else {
										THE_APP->selectMonitorBookCncPanel();
										
										if ( m_actionSelection->GetSelection() == ACTION_RUN ) {
											wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
											wxPostEvent(THE_APP->GetRcRun(), evt);
											
											logInformation(wxString::Format(" --> Action: '%s' performed\n", "Run Template"), styles.taAction);
											
										} else {
											wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
											wxPostEvent(THE_APP->GetRcDebug(), evt);
											
											logInformation(wxString::Format(" --> Action: '%s' performed\n", "Run Template"), styles.taAction);
										}
									}
									
									break;
		}
	}
}

