#include <sstream>
#include <wx/uiaction.h>
#include <wx/gdicmn.h>
#include <wx/richtooltip.h>
#include "wxCrafterImages.h"
#include "MainFrame.h"
#include "MainFrameProxy.h"
#include "GlobalFunctions.h"
#include "CncReferencePosition.h"
#include "CncGamepadCommandHistory.h"
#include "CncGamepadControllerState.h"

///////////////////////////////////////////////////////////////////
CncGamepadControllerState::CncGamepadControllerState(wxWindow* parent)
: CncGamepadControllerStateBase(parent)
, cmdHistCtrl			(NULL)
, serviceShortName		("Ds3Service")
, serviceLongName		("SCP DSx Service")
///////////////////////////////////////////////////////////////////
{
	cmdHistCtrl = new CncGamepadCommadHistoryListCtrl(this, wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL);
	GblFunc::replaceControl(m_gamepadCmdHistoryPlaceholder, cmdHistCtrl);
}
///////////////////////////////////////////////////////////////////
CncGamepadControllerState::~CncGamepadControllerState() {
///////////////////////////////////////////////////////////////////
	wxDELETE( cmdHistCtrl );
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::clearGamepadServiceTrace(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_gamepadServiceTrace->Clear();
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::onClearHistory(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT( cmdHistCtrl );
	cmdHistCtrl->clearAll();
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::update(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	processTrace(state);
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::trace(const wxString& msg) {
//////////////////////////////////////////////////////////////////
	m_gamepadTrace->ChangeValue(msg);
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::processTrace(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	if ( IsShownOnScreen() == false )
		return;
	
	std::stringstream ss;
	ss << state;
	
	m_gamepadTrace->ChangeValue(ss.str());
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::executeCommand(const wxString& cmd) {
///////////////////////////////////////////////////////////////////
	wxArrayString output;
	wxArrayString errors;
	
	wxExecute(cmd, output, errors);
	
	if ( output.GetCount() > 0 ) {
		for (size_t i = 0; i < output.GetCount(); i++)
			m_gamepadServiceTrace->AppendText(wxString::Format("  %s\n", output[i]));
	}
	
	if ( errors.GetCount() > 0 ) {
		
		const wxTextAttr defaultAttri = m_gamepadServiceTrace->GetDefaultStyle();
		
		wxTextAttr errorAttri = defaultAttri;
		errorAttri.SetTextColour(wxColour(241, 75, 84));
		m_gamepadServiceTrace->SetDefaultStyle(errorAttri);
		
		for (size_t i = 0; i < errors.GetCount(); i++)
			m_gamepadServiceTrace->AppendText(wxString::Format("  %s\n", errors[i]));
			
		m_gamepadServiceTrace->SetDefaultStyle(defaultAttri);
	}
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::startGamepadService(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	#ifdef __WXMSW__
		m_gamepadServiceTrace->AppendText("Administrator rights required");
		m_gamepadServiceTrace->AppendText(wxString::Format("Manually start the service [ %s(\"%s\") ] . . . \n", serviceLongName, serviceShortName));
		if ( false )	executeCommand("net start Ds3Service"); // only with admin rights possible
		else			executeCommand("cmd /C services.msc");
	#else
		m_gamepadServiceTrace->AppendText("SCP Toolkit: Start Gamepad Service isn't supported");
	#endif
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::stopGamepadService(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	#ifdef __WXMSW__
		m_gamepadServiceTrace->AppendText("Administrator rights required");
	m_gamepadServiceTrace->AppendText(wxString::Format("Manually stop the service  [ %s(\"%s\") ] . . . \n", serviceLongName, serviceShortName));
		if ( false )	executeCommand("net stop Ds3Service");  // only with admin rights possible
		else			executeCommand("cmd /C services.msc");
	#else
		m_gamepadServiceTrace->AppendText("SCP Toolkit: Stop Gamepad Service isn't supported");
	#endif
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::queryGamepadService(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_gamepadServiceTrace->AppendText(wxString::Format("Query the service [ %s(\"%s\") ] . . . \n", serviceLongName, serviceShortName));
	
	#ifdef __WXMSW__
		executeCommand("sc query Ds3Service");
	#else
		m_gamepadServiceTrace->AppendText("SCP Toolkit: Query Gamepad Service isn't supported");
	#endif
}
///////////////////////////////////////////////////////////////////
bool CncGamepadControllerState::openTool(const wxString& tool, bool checkToolExists) {
///////////////////////////////////////////////////////////////////
	CncRunAnimationControl rac(THE_APP);
	
	const bool ret = GblFunc::executeExternalProgram(tool, "", checkToolExists);
	if ( ret == true )
		THE_APP->waitActive(1500);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::scpDriverInstaller(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	#ifdef __WXMSW__
		openTool("C:\\Program Files\\Nefarius Software Solutions\\ScpToolkit\\ScpDriverInstaller.exe");
	#else
		m_gamepadServiceTrace->AppendText("SCP Toolkit isn't supported");
	#endif
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::scpMonitor(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	#ifdef __WXMSW__
		openTool("C:\\Program Files\\Nefarius Software Solutions\\ScpToolkit\\ScpMonitor.exe");
	#else
		m_gamepadServiceTrace->AppendText("SCP Toolkit isn't supported");
	#endif
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::scpSettings(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	#ifdef __WXMSW__
		openTool("C:\\Program Files\\Nefarius Software Solutions\\ScpToolkit\\ScpSettings.exe");
	#else
		m_gamepadServiceTrace->AppendText("SCP Toolkit isn't supported");
	#endif
}
