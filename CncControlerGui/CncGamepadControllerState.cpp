#include <sstream>
#include "MainFrame.h"
#include "CncReferencePosition.h"
#include "CncGamepadControllerState.h"

///////////////////////////////////////////////////////////////////
CncGamepadControllerState::CncGamepadControllerState(wxWindow* parent)
: CncGamepadControllerStateBase(parent)
, posCtrlMode(PCM_STICKS)
, running(false)
, xyNavigationActive(false)
, zNavigationActive(false)
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
		processPositionControlMode(state);
		processPosition(state);
	running = false;
}
///////////////////////////////////////////////////////////////////
bool CncGamepadControllerState::isRefPosDlgMode() {
///////////////////////////////////////////////////////////////////
	if ( THE_APP->refPositionDlg == NULL )
		return false;

	return THE_APP->refPositionDlg->IsShown();
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::processTrace(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << state;
	m_gamepadTrace->ChangeValue(ss.str());
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::processPositionControlMode(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	if ( state.data.buttonRightStick == true ) { 
		if ( posCtrlMode == PCM_STICKS || posCtrlMode == PCM_NAV_XY ) 	posCtrlMode = PCM_NAV_Z;
		else 															posCtrlMode = PCM_STICKS;
		
	} 
	
	if ( state.data.buttonLeftStick  == true ) { 
		if ( posCtrlMode == PCM_STICKS || posCtrlMode == PCM_NAV_Z  ) 	posCtrlMode = PCM_NAV_XY; 
		else															posCtrlMode = PCM_STICKS;
	}
	
	mangageMainView(state);
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::processReferencePage(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	if ( state.data.buttonStart ) {
		wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
		wxPostEvent(THE_APP->GetBtSelectReferences(), evt);
	}
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::processRefPositionDlg(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	if ( THE_APP->refPositionDlg == NULL )
		return;
		
	// check what to do
	if ( THE_APP->refPositionDlg->IsShown() == false ) {
		// open ref dialog - on demand
		if ( state.data.buttonX ) {
			THE_APP->showReferencePositionDlg("Opened by Gamepad");
			return;
		}
		
		// select step sensitivity - main frame
		if ( state.data.buttonA ) { 
			unsigned int sel = THE_APP->GetRbStepSensitivity()->GetSelection();
			unsigned int cnt = THE_APP->GetRbStepSensitivity()->GetCount();
		
			if ( sel + 1 >= cnt ) 	THE_APP->GetRbStepSensitivity()->SetSelection(0);
			else					THE_APP->GetRbStepSensitivity()->SetSelection(sel +1);
		}
		
		// always return here
		return;
	}
	
	// manage ref pos dlg
	
	// sensitivity
	if ( state.data.buttonA == true ) {
		wxRadioBox* rbs  = THE_APP->refPositionDlg->GetRbStepSensitivity();
		unsigned int sel = rbs->GetSelection();
		unsigned int cnt = rbs->GetCount();
		
		// select it at the ref pos dlg
		if ( sel + 1 >= cnt ) 	rbs->SetSelection(0);
		else					rbs->SetSelection(sel +1);
		
		// select it at the main frame
		rbs  = THE_APP->GetRbStepSensitivity();
		if ( sel + 1 >= cnt ) 	rbs->SetSelection(0);
		else					rbs->SetSelection(sel +1);
	}
	
	// ref pos mode
	if ( state.data.buttonB ) {
		THE_APP->refPositionDlg->shiftReferenceMode();
	}
	
	// set and close
	if ( state.data.buttonX ) {
		wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
		wxPostEvent(THE_APP->refPositionDlg->GetBtSet(), evt);
	}
	
	// cancel and close
	if ( state.data.buttonY ) {
		wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
		wxPostEvent(THE_APP->refPositionDlg->GetBtCancel(), evt);
	}
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::processPosition(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	if ( posCtrlMode == PCM_STICKS ) return managePositionViaStick(state);
	
	return managePositionViaNavi(state);
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::managePositionViaStick(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	typedef CncLinearDirection CLD;
	
	// z navigation
	if ( state.data.rightStickY != 0.0f ) {
		
		zNavigationActive = true;
		if ( state.data.rightStickY > 0.0f )		THE_APP->manualContinuousMoveStart(CLD::CncNoneDir, CLD::CncNoneDir, CLD::CncPosDir);
		if (  state.data.rightStickY < 0.0f )		THE_APP->manualContinuousMoveStart(CLD::CncNoneDir, CLD::CncNoneDir, CLD::CncNegDir);
			
	} else {
	
		// xy navigation
		if ( state.data.leftStickX != 0.0f || state.data.leftStickY != 0.0f ) {
			
			xyNavigationActive = true;
			if ( state.data.leftStickX > 0.0f )		THE_APP->manualContinuousMoveStart(CLD::CncPosDir,  CLD::CncNoneDir, CLD::CncNoneDir);
			if (  state.data.leftStickX < 0.0f )	THE_APP->manualContinuousMoveStart(CLD::CncNegDir,  CLD::CncNoneDir, CLD::CncNoneDir);
			if ( state.data.leftStickY > 0.0f )		THE_APP->manualContinuousMoveStart(CLD::CncNoneDir, CLD::CncPosDir,  CLD::CncNoneDir);
			if (  state.data.leftStickY < 0.0f )	THE_APP->manualContinuousMoveStart(CLD::CncNoneDir, CLD::CncNegDir,  CLD::CncNoneDir);
		}
	}
	
	// stop move
	if ( zNavigationActive == true ) {
		if ( state.data.rightStickY == 0.0f && state.data.rightStickY == 0.0f )
			THE_APP->manualContinuousMoveStop();
			
	} else if ( xyNavigationActive == true ) {
		if ( state.data.leftStickX  == 0.0f && state.data.leftStickY  == 0.0f )
			THE_APP->manualContinuousMoveStop();
			
	} else {
		// safety
		THE_APP->manualContinuousMoveStop();
	}
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::managePositionViaNavi(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	bool left 	= state.data.buttonLeft; 
	bool right 	= state.data.buttonRight;
	bool up 	= state.data.buttonUp;
	bool down 	= state.data.buttonDown;
	
	typedef CncLinearDirection CLD;
	
	switch ( posCtrlMode ) {
		case PCM_NAV_XY:	if ( left  == true ) 	{ THE_APP->manualContinuousMoveStart(CLD::CncNegDir,  CLD::CncNoneDir, CLD::CncNoneDir); }
							if ( right == true ) 	{ THE_APP->manualContinuousMoveStart(CLD::CncPosDir,  CLD::CncNoneDir, CLD::CncNoneDir); }
							if ( up    == true ) 	{ THE_APP->manualContinuousMoveStart(CLD::CncNoneDir, CLD::CncPosDir,  CLD::CncNoneDir); }
							if ( down  == true ) 	{ THE_APP->manualContinuousMoveStart(CLD::CncNoneDir, CLD::CncNegDir,  CLD::CncNoneDir); }
							break;
							
		case PCM_NAV_Z:		if ( up    == true ) 	{ THE_APP->manualContinuousMoveStart(CLD::CncNoneDir, CLD::CncNoneDir, CLD::CncPosDir); }
							if ( down  == true ) 	{ THE_APP->manualContinuousMoveStart(CLD::CncNoneDir, CLD::CncNoneDir, CLD::CncNegDir); }
							break;
							
		default:			return;
	}
	 
	if ( left == false && right == false && up == false && down == false )
		THE_APP->manualContinuousMoveStop();
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::mangageMainView(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	THE_APP->GetGpBmp1()->Show(false);
	THE_APP->GetGpBmp2()->Show(false);
	THE_APP->GetGpBmp3()->Show(false);
	THE_APP->GetGpBmp4()->Show(false);
	
	switch ( posCtrlMode ) {
		case PCM_STICKS:	THE_APP->GetGpBmp1()->SetBitmap(ImageLibGamepad().Bitmap("BMP_STICK_LEFT"));
							THE_APP->GetGpBmp2()->SetBitmap(ImageLibGamepad().Bitmap("BMP_STICK_RIGHT"));
							THE_APP->GetGpBmp1()->Show();
							THE_APP->GetGpBmp2()->Show();
							break;
							
		case PCM_NAV_XY:	THE_APP->GetGpBmp1()->SetBitmap(ImageLibGamepad().Bitmap("BMP_NAVI_XY"));
							THE_APP->GetGpBmp1()->Show();
							break;
							
		case PCM_NAV_Z:		THE_APP->GetGpBmp1()->SetBitmap(ImageLibGamepad().Bitmap("BMP_NAVI_Z"));
							THE_APP->GetGpBmp1()->Show();
							break;
	}
	
	THE_APP->GetGpBmp1()->Refresh();
	THE_APP->GetGpBmp2()->Refresh();
	THE_APP->GetGpBmp3()->Refresh();
	THE_APP->GetGpBmp4()->Refresh();
	
	THE_APP->GetGpBmp1()->GetParent()->Layout();
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
