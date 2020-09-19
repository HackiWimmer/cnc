#include <sstream>
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
, currentMovementState	(MS_STOPPED)
, currentMoveInfo		()
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
const char* CncGamepadControllerState::getMovementStateAsString(MovementState s) {
///////////////////////////////////////////////////////////////////
	switch ( currentMovementState ) {
		case MS_STOPPED:	return "Stopped";
		case MS_RUNNING:	return "Running";
		case MS_ERROR:		return "Error";
	}
	
	return "Unknown";
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
void CncGamepadControllerState::onConnectGamepad(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	decoarteConnectButton();
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::decoarteConnectButton() {
///////////////////////////////////////////////////////////////////
	const bool b   = m_btConnect->GetValue();
	const char* t1 = "For Debug purpose only!\nDisconnect Gamepad from CNC Controller";
	const char* t2 = "For Debug purpose only!\nConnect Gamepad To CNC Controller";
	
	m_btConnect->SetBitmap(ImageLib16().Bitmap(b ? "BMP_CONNECTED" : "BMP_DISCONNECTED")); 
	m_btConnect->SetToolTip(b ? t1 : t2);
	m_btConnect->Refresh();
	m_btConnect->Update();
	
	// to clear
	if ( b )
		cnc::trc.logInfo("");
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::update(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	if ( m_btConnect->GetValue() == true )
		processMovement(state);

	if ( state.isSomethingChanged == false )
		return;
		
	processTrace(state);
	
	if ( m_btConnect->GetValue() == false ) {
		cnc::trc.logWarning("Gamepad currently decoupled from the CNC Controller!");
		return;
	}
	
	processOpenNavigator(state);
	processRefPositionDlg(state);
}
///////////////////////////////////////////////////////////////////
bool CncGamepadControllerState::isRefPosDlgMode() {
///////////////////////////////////////////////////////////////////
	if ( APP_PROXY::getRefPositionDlg() == NULL )
		return false;

	return APP_PROXY::getRefPositionDlg()->IsShown();
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::trace(const wxString& msg) {
//////////////////////////////////////////////////////////////////
	m_gamepadTrace->ChangeValue(msg);
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::processTrace(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << state;
	ss << "\nCurrent Movement State         : ";
	ss << getMovementStateAsString(currentMovementState);
	
	m_gamepadTrace->ChangeValue(ss.str());
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::processOpenNavigator(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	if ( state.data.buttonStart ) 
		APP_PROXY::openNavigatorFromGamepad();
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
			const unsigned int sel  = APP_PROXY::GetRbStepSensitivity()->GetSelection();
			const unsigned int max  = APP_PROXY::GetRbStepSensitivity()->GetCount();
			const unsigned int next = (sel + 1) % max;
			const StepSensitivity s = cnc::getStepSensitivityOfIndex(next);
			const double newSpeed   = cnc::getSpeedValue(s);
			
			APP_PROXY::GetRbStepSensitivity()->SetSelection(next);
			APP_PROXY::updateSpeedSlider(newSpeed);
		}
		
		// always return here
		return;
	}
	
	// manage ref pos dlg
	
	// sensitivity
	if ( state.data.buttonA == true ) {
		wxRadioBox* rbs  = APP_PROXY::getRefPositionDlg()->GetRbStepSensitivity();
		
		const unsigned int sel  = rbs->GetSelection();
		const unsigned int max  = rbs->GetCount();
		const unsigned int next = (sel + 1) % max;
		const StepSensitivity s = cnc::getStepSensitivityOfIndex(next);
		const double newSpeed   = cnc::getSpeedValue(s);

		rbs->SetSelection(next);
		
		rbs  = APP_PROXY::GetRbStepSensitivity();
		rbs->SetSelection(next);
		
		APP_PROXY::updateSpeedSlider(newSpeed);
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
void CncGamepadControllerState::processMovement(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	typedef CncLinearDirection CLD;
	const CLD dx = state.data.dx;
	const CLD dy = state.data.dy;
	const CLD dz = state.data.dz;

	switch ( currentMovementState ) {
		// -------------------------------------------------------
		case MS_STOPPED:
		{
			if ( state.hasMovementInformation() ) {
				const bool b = APP_PROXY::startInteractiveMove(CncInteractiveMoveDriver::IMD_GAMEPAD);
				currentMovementState = b ? MS_RUNNING : MS_ERROR;
				currentMoveInfo.reset();
				cmdHistCtrl->addStartCommand(b);
			}
			
			m_btConnect->Enable(true);
			break;
		}
		// -------------------------------------------------------
		case MS_RUNNING:
		{
			if ( state.hasEmptyMovementInformation() ) {
				const bool b = APP_PROXY::stopInteractiveMove();
				currentMovementState = b ? MS_STOPPED : MS_ERROR;
				currentMoveInfo.reset();
				cmdHistCtrl->addStopCommand(b);
			}
			else {
				if ( currentMoveInfo.update(dx, dy, dz) == true ) {
					const bool b = APP_PROXY::updateInteractiveMove(dx, dy, dz);
					currentMovementState = b ? MS_RUNNING : MS_ERROR;
					currentMoveInfo.reset();
					cmdHistCtrl->addUpdateCommand(b, wxString::Format("%+d, %+d, %+d", (int)dx, (int)dy, (int)dz));
				}
				else {
					const bool b = APP_PROXY::updateInteractiveMove();
					currentMovementState = b ? MS_RUNNING : MS_ERROR;
					currentMoveInfo.reset();
					cmdHistCtrl->addUpdateCommand(b, "");
				}
			}
			
			m_btConnect->Enable(false);
			break;
		}
		// -------------------------------------------------------
		case MS_ERROR:
		{
			const bool b = APP_PROXY::stopInteractiveMove();
			currentMovementState = b ? MS_STOPPED : MS_ERROR;
			currentMoveInfo.reset();
			
			m_btConnect->Enable(false);
			break;
		}
	}
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
