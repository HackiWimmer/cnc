#include <sstream>
#include "wxCrafterImages.h"
#include "MainFrame.h"
#include "MainFrameProxy.h"
#include "CncReferencePosition.h"
#include "CncGamepadControllerState.h"

///////////////////////////////////////////////////////////////////
CncGamepadControllerState::CncGamepadControllerState(wxWindow* parent)
: CncGamepadControllerStateBase(parent)
, running(false)
, currentMoveState()
, serviceShortName("Ds3Service")
, serviceLongName("SCP DSx Service")
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
CncGamepadControllerState::~CncGamepadControllerState() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::update(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	running = true;
		processTrace(state);
		processReferencePage(state);
		processRefPositionDlg(state);
		processPosition(state);
	running = false;
}
///////////////////////////////////////////////////////////////////
bool CncGamepadControllerState::isRefPosDlgMode() {
///////////////////////////////////////////////////////////////////
	if ( APP_PROXY::getRefPositionDlg() == NULL )
		return false;

	return APP_PROXY::getRefPositionDlg()->IsShown();
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::processTrace(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << state;
	m_gamepadTrace->ChangeValue(ss.str());
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::processReferencePage(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	if ( state.data.buttonStart ) {
		wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
		wxPostEvent(APP_PROXY::GetBtSelectReferences(), evt);
	}
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::processRefPositionDlg(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	if ( APP_PROXY::getRefPositionDlg() == NULL )
		return;
		
	// check what to do
	if ( APP_PROXY::getRefPositionDlg()->IsShown() == false ) {
		// open ref dialog - on demand
		if ( state.data.buttonX ) {
			APP_PROXY::showReferencePositionDlg("Opened by Gamepad");
			return;
		}
		
		// select step sensitivity - main frame
		if ( state.data.buttonA ) { 
			unsigned int sel = APP_PROXY::GetRbStepSensitivity()->GetSelection();
			unsigned int cnt = APP_PROXY::GetRbStepSensitivity()->GetCount();
		
			if ( sel + 1 >= cnt ) 	APP_PROXY::GetRbStepSensitivity()->SetSelection(0);
			else					APP_PROXY::GetRbStepSensitivity()->SetSelection(sel +1);
		}
		
		// always return here
		return;
	}
	
	// manage ref pos dlg
	
	// sensitivity
	if ( state.data.buttonA == true ) {
		wxRadioBox* rbs  = APP_PROXY::getRefPositionDlg()->GetRbStepSensitivity();
		unsigned int sel = rbs->GetSelection();
		unsigned int cnt = rbs->GetCount();
		
		// select it at the ref pos dlg
		if ( sel + 1 >= cnt ) 	rbs->SetSelection(0);
		else					rbs->SetSelection(sel +1);
		
		// select it at the main frame
		rbs  = APP_PROXY::GetRbStepSensitivity();
		if ( sel + 1 >= cnt ) 	rbs->SetSelection(0);
		else					rbs->SetSelection(sel +1);
	}
	
	// ref pos mode
	if ( state.data.buttonB ) {
		APP_PROXY::getRefPositionDlg()->shiftReferenceMode();
	}
	
	// set and close
	if ( state.data.buttonX ) {
		wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
		wxPostEvent(APP_PROXY::getRefPositionDlg()->GetBtSet(), evt);
	}
	
	// cancel and close
	if ( state.data.buttonY ) {
		wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
		wxPostEvent(APP_PROXY::getRefPositionDlg()->GetBtCancel(), evt);
	}
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::processPosition(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	typedef CncLinearDirection CLD;
	
	//---------------------------------------------------------
	auto stopMove = [&](){
		currentMoveState.reset();
		APP_PROXY::stopInteractiveMove();
	};
	
	const CLD dx = state.data.dx;
	const CLD dy = state.data.dy;
	const CLD dz = state.data.dz;
	
	//
	if ( dx == CLD::CncNoneDir && dy == CLD::CncNoneDir && dz == CLD::CncNoneDir ) {
		stopMove();
	}
	else { 
		
		if ( dx != CLD::CncNoneDir || dy != CLD::CncNoneDir || dz != CLD::CncNoneDir ) {
			
			if ( currentMoveState.isEqual(dx, dy, dz) == false ) {
				
				if ( false ) {
					std::cout   << " x or y changed: " << dx << ", " << dy << ", " << dz 
					            << "-----------" << currentMoveState.dx << ", " << currentMoveState.dy << ", " << currentMoveState.dz
								<< " -- Stick: "
								<< state.data.leftStickX
								<< ","
								<< state.data.leftStickY
								<< std::endl;
				}
				
				currentMoveState.set(dx, dy, dz);
				APP_PROXY::updateInteractiveMove(dx, dy, dz);
			}
		}
	}
	
	currentMoveState.set(dx, dy, dz);
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
	m_gamepadServiceTrace->AppendText(wxString::Format("Try to start the service [ %s(\"%s\") ] . . . \n", serviceLongName, serviceShortName));
	
	#ifdef __WXMSW__
		if ( false )	executeCommand("net start Ds3Service"); // only with admin rights possible
		else			executeCommand("cmd /C services.msc");
	#else
		m_gamepadServiceTrace->AppendText("Start Gamepad Service isn't supported");
	#endif
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::stopGamepadService(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_gamepadServiceTrace->AppendText(wxString::Format("Try to stop the service  [ %s(\"%s\") ] . . . \n", serviceLongName, serviceShortName));
	
	#ifdef __WXMSW__
		if ( false )	executeCommand("net stop Ds3Service");  // only with admin rights possible
		else			executeCommand("cmd /C services.msc");
	#else
		m_gamepadServiceTrace->AppendText("Stop Gamepad Service isn't supported");
	#endif
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::queryGamepadService(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_gamepadServiceTrace->AppendText(wxString::Format("Query the service [ %s(\"%s\") ] . . . \n", serviceLongName, serviceShortName));
	
	#ifdef __WXMSW__
		executeCommand("sc query Ds3Service");
	#else
		m_gamepadServiceTrace->AppendText("Query Gamepad Service isn't supported");
	#endif
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::clearGamepadServiceTrace(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_gamepadServiceTrace->Clear();
}
