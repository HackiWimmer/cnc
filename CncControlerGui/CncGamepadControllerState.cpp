#include <sstream>
#include "MainFrame.h"
#include "CncReferencePosition.h"
#include "CncGamepadControllerState.h"

///////////////////////////////////////////////////////////////////
CncGamepadControllerState::CncGamepadControllerState(MainFrame* parent)
: CncGamepadControllerStateBase(parent)
, mainFrame(parent)
, posCtrlMode(PCM_STICKS)
, running(false)
, xyNavigationActive(false)
, zNavigationActive(false)
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
	if ( mainFrame == NULL )
		return false;
		
	if ( mainFrame->refPositionDlg == NULL )
		return false;

	return mainFrame->refPositionDlg->IsShown();
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
		wxPostEvent(mainFrame->GetBtSelectReferences(), evt);
	}
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::processRefPositionDlg(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	if ( mainFrame == NULL )
		return;
		
	if ( mainFrame->refPositionDlg == NULL )
		return;
		
	// check what to do
	if ( mainFrame->refPositionDlg->IsShown() == false ) {
		// open ref dialog - on demand
		if ( state.data.buttonX ) {
			mainFrame->showReferencePositionDlg("Opened by Gamepad");
			return;
		}
		
		// select step sensitivity - main frame
		if ( state.data.buttonA ) { 
			unsigned int sel = mainFrame->GetRbStepSensitivity()->GetSelection();
			unsigned int cnt = mainFrame->GetRbStepSensitivity()->GetCount();
		
			if ( sel + 1 >= cnt ) 	mainFrame->GetRbStepSensitivity()->SetSelection(0);
			else					mainFrame->GetRbStepSensitivity()->SetSelection(sel +1);
		}
		
		// always return here
		return;
	}
	
	// manage ref pos dlg
	
	// sensitivity
	if ( state.data.buttonA == true ) {
		wxRadioBox* rbs  = mainFrame->refPositionDlg->GetRbStepSensitivity();
		unsigned int sel = rbs->GetSelection();
		unsigned int cnt = rbs->GetCount();
		
		// select it at the ref pos dlg
		if ( sel + 1 >= cnt ) 	rbs->SetSelection(0);
		else					rbs->SetSelection(sel +1);
		
		// select it at the main frame
		rbs  = mainFrame->GetRbStepSensitivity();
		if ( sel + 1 >= cnt ) 	rbs->SetSelection(0);
		else					rbs->SetSelection(sel +1);
	}
	
	// ref pos mode
	if ( state.data.buttonB ) {
		mainFrame->refPositionDlg->shiftReferenceMode();
	}
	
	// set and close
	if ( state.data.buttonX ) {
		wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
		wxPostEvent(mainFrame->refPositionDlg->GetBtSet(), evt);
	}
	
	// cancel and close
	if ( state.data.buttonY ) {
		wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
		wxPostEvent(mainFrame->refPositionDlg->GetBtCancel(), evt);
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
	if ( mainFrame == NULL )
		return;
		
	typedef CncLinearDirection CLD;
	
	// z navigation
	if ( state.data.rightStickY != 0.0f ) {
		
		zNavigationActive = true;
		if ( state.data.rightStickY > 0.0f )		mainFrame->manualContinuousMoveStart(CLD::CncNoneDir, CLD::CncNoneDir, CLD::CncPosDir);
		if (  state.data.rightStickY < 0.0f )		mainFrame->manualContinuousMoveStart(CLD::CncNoneDir, CLD::CncNoneDir, CLD::CncNegDir);
			
	} else {
	
		// xy navigation
		if ( state.data.leftStickX != 0.0f || state.data.leftStickY != 0.0f ) {
			
			xyNavigationActive = true;
			if ( state.data.leftStickX > 0.0f )		mainFrame->manualContinuousMoveStart(CLD::CncPosDir,  CLD::CncNoneDir, CLD::CncNoneDir);
			if (  state.data.leftStickX < 0.0f )	mainFrame->manualContinuousMoveStart(CLD::CncNegDir,  CLD::CncNoneDir, CLD::CncNoneDir);
			if ( state.data.leftStickY > 0.0f )		mainFrame->manualContinuousMoveStart(CLD::CncNoneDir, CLD::CncPosDir,  CLD::CncNoneDir);
			if (  state.data.leftStickY < 0.0f )	mainFrame->manualContinuousMoveStart(CLD::CncNoneDir, CLD::CncNegDir,  CLD::CncNoneDir);
		}
	}
	
	// stop move
	if ( zNavigationActive == true ) {
		if ( state.data.rightStickY == 0.0f && state.data.rightStickY == 0.0f )
			mainFrame->manualContinuousMoveStop();
			
	} else if ( xyNavigationActive == true ) {
		if ( state.data.leftStickX  == 0.0f && state.data.leftStickY  == 0.0f )
			mainFrame->manualContinuousMoveStop();
			
	} else {
		// safety
		mainFrame->manualContinuousMoveStop();
	}
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::managePositionViaNavi(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	if ( mainFrame == NULL )
		return;
	
	bool left 	= state.data.buttonLeft; 
	bool right 	= state.data.buttonRight;
	bool up 	= state.data.buttonUp;
	bool down 	= state.data.buttonDown;
	
	typedef CncLinearDirection CLD;
	
	switch ( posCtrlMode ) {
		case PCM_NAV_XY:	if ( left  == true ) 	{ mainFrame->manualContinuousMoveStart(CLD::CncNegDir,  CLD::CncNoneDir, CLD::CncNoneDir); }
							if ( right == true ) 	{ mainFrame->manualContinuousMoveStart(CLD::CncPosDir,  CLD::CncNoneDir, CLD::CncNoneDir); }
							if ( up    == true ) 	{ mainFrame->manualContinuousMoveStart(CLD::CncNoneDir, CLD::CncPosDir,  CLD::CncNoneDir); }
							if ( down  == true ) 	{ mainFrame->manualContinuousMoveStart(CLD::CncNoneDir, CLD::CncNegDir,  CLD::CncNoneDir); }
							break;
							
		case PCM_NAV_Z:		if ( up    == true ) 	{ mainFrame->manualContinuousMoveStart(CLD::CncNoneDir, CLD::CncNoneDir, CLD::CncPosDir); }
							if ( down  == true ) 	{ mainFrame->manualContinuousMoveStart(CLD::CncNoneDir, CLD::CncNoneDir, CLD::CncNegDir); }
							break;
							
		default:			return;
	}
	 
	if ( left == false && right == false && up == false && down == false )
		mainFrame->manualContinuousMoveStop();
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerState::mangageMainView(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	if ( mainFrame == NULL )
		return;
		
	mainFrame->GetGpBmp1()->Show(false);
	mainFrame->GetGpBmp2()->Show(false);
	mainFrame->GetGpBmp3()->Show(false);
	mainFrame->GetGpBmp4()->Show(false);
	
	switch ( posCtrlMode ) {
		case PCM_STICKS:	mainFrame->GetGpBmp1()->SetBitmap(ImageLibGamepad().Bitmap("BMP_STICK_LEFT"));
							mainFrame->GetGpBmp2()->SetBitmap(ImageLibGamepad().Bitmap("BMP_STICK_RIGHT"));
							mainFrame->GetGpBmp1()->Show();
							mainFrame->GetGpBmp2()->Show();
							break;
							
		case PCM_NAV_XY:	mainFrame->GetGpBmp1()->SetBitmap(ImageLibGamepad().Bitmap("BMP_NAVI_XY"));
							mainFrame->GetGpBmp1()->Show();
							break;
							
		case PCM_NAV_Z:		mainFrame->GetGpBmp1()->SetBitmap(ImageLibGamepad().Bitmap("BMP_NAVI_Z"));
							mainFrame->GetGpBmp1()->Show();
							break;
	}
	
	mainFrame->GetGpBmp1()->Refresh();
	mainFrame->GetGpBmp2()->Refresh();
	mainFrame->GetGpBmp3()->Refresh();
	mainFrame->GetGpBmp4()->Refresh();
	
	mainFrame->GetGpBmp1()->GetParent()->Layout();
}
