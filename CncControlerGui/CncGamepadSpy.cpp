#include <wx/uiaction.h>
#include <wx/frame.h>
#include "GlobalFunctions.h"
#include "GamepadEvent.h"
#include "MainFrameProxy.h"
#include "MainFrame.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "CncGamepadDirectionPanel.h"
#include "CncGamepadFilter.h"
#include "CncGamepadCommandHistory.h"
#include "CncGamepadSpy.h"

/////////////////////////////////////////////////////////////////////
CncGamepadSpy::CncGamepadSpy(wxWindow* parent)
: CncGamepadSpyBase(parent)
, currContext			(GPC_MAIN)
, dirXY					(NULL)
, dirZ					(NULL)
, cmdHistCtrl			(NULL)
, currentMovementState	(MS_STOPPED)
, currentMoveInfo		()
, activated				(false)
, keyInfo				()
, mouseInfo				()
/////////////////////////////////////////////////////////////////////
{
	dirXY = new CncGamepadDirectionPanel(this);
	GblFunc::replaceControl(m_dirXYPlaceholder, dirXY);

	dirZ = new CncGamepadDirectionPanel(this);
	GblFunc::replaceControl(m_dirZPlaceholder, dirZ);
	
	m_modeBook->SetSelection(MODE_GNC);
}
/////////////////////////////////////////////////////////////////////
CncGamepadSpy::~CncGamepadSpy() {
/////////////////////////////////////////////////////////////////////
	wxDELETE(dirXY);
	wxDELETE(dirZ);
}
/////////////////////////////////////////////////////////////////////
void CncGamepadSpy::updateModeText(const wxString& msg) {
/////////////////////////////////////////////////////////////////////
	m_modeText->SetLabel(msg);
	m_modeText->Layout();
}
/////////////////////////////////////////////////////////////////////
void CncGamepadSpy::setActivationState(bool state) {
/////////////////////////////////////////////////////////////////////
	activated = state;
	
	if ( activated == false && m_modeBook->GetSelection() != MODE_GNC ) {
		m_modeBook->SetSelection(MODE_GNC);
		updateModeText("Not activated");
	}
}
/////////////////////////////////////////////////////////////////////
void CncGamepadSpy::releaseCncActions(const GamepadEvent* state) {
/////////////////////////////////////////////////////////////////////
	const bool b = APP_PROXY::GetRbStepMode()->GetSelection() == SM_INTERACTIVE; 
	
	if ( b )	processInteractiveMovement(*state);
	else 		processStepwiseMovement(*state);
	
	// select step sensitivity - main frame
	if ( state->data.buttonA ) { 
		const unsigned int sel		= APP_PROXY::GetRbStepSensitivity()->GetSelection();
		const unsigned int max		= APP_PROXY::GetRbStepSensitivity()->GetCount();
		const unsigned int next		= (sel + 1) % max;
		const CncStepSensitivity s	= cnc::getStepSensitivityOfIndex(next);
		const double newSpeed   	= cnc::getSpeedValue(s);
		
		APP_PROXY::GetRbStepSensitivity()->SetSelection(next);
		APP_PROXY::updateSpeedSlider(newSpeed);
	}
	
	// select step sensitivity - main frame
	if ( state->data.buttonStart ) { 
		const unsigned int sel		= APP_PROXY::GetRbStepMode()->GetSelection();
		const unsigned int max		= APP_PROXY::GetRbStepMode()->GetCount();
		const unsigned int next		= (sel + 1) % max;
		
		APP_PROXY::GetRbStepMode()->SetSelection(next);
	}
}
/////////////////////////////////////////////////////////////////////
void CncGamepadSpy::releaseKeyboardActions(const GamepadEvent* state) {
/////////////////////////////////////////////////////////////////////
	// .............................................................
	auto hitKey = [&](int keyCode, int modifier = wxMOD_NONE) {
		// simulate one hit only
		wxUIActionSimulator uai;
		
		uai.KeyDown(keyCode, modifier);
		uai.KeyUp  (keyCode, modifier);
		
		switch ( modifier ) {
			case wxMOD_SHIFT: keyInfo.append("<SHIFT> ");	break;
			default:;
		}
		
		switch ( keyCode ) {
			case WXK_LEFT:	keyInfo.append("LEFT");			break;
			case WXK_RIGHT:	keyInfo.append("RIGHT");		break;
			case WXK_UP:	keyInfo.append("UP");			break;
			case WXK_DOWN:	keyInfo.append("DOWN");			break;

			case WXK_TAB:	keyInfo.append("TAB");			break;
			case WXK_SPACE:	keyInfo.append("SPACE");		break;
		}
	};
	
	// update key state info
	keyInfo.clear();
	
	// release corresponding key actions
	if ( state->data.buttonLeft )			{ hitKey(WXK_LEFT);				return; }
	if ( state->data.buttonRight )			{ hitKey(WXK_RIGHT);			return; }
	if ( state->data.buttonUp )				{ hitKey(WXK_UP);				return; }
	if ( state->data.buttonDown )			{ hitKey(WXK_DOWN);				return; }
	
	if ( state->data.buttonLeftShoulder )	{ hitKey(WXK_TAB, wxMOD_SHIFT);	return; }
	if ( state->data.buttonRightShoulder )	{ hitKey(WXK_TAB);				return; }
	if ( state->data.buttonB )				{ hitKey(WXK_SPACE);			return; }
}
/////////////////////////////////////////////////////////////////////
void CncGamepadSpy::releaseMouseActions(const GamepadEvent* state) {
/////////////////////////////////////////////////////////////////////
	static bool mouseLButtonDown = false;
	static bool mouseRButtonDown = false;
	
	// .............................................................
	auto mouseLButton = [&]() {
		wxUIActionSimulator uai;
		if ( mouseLButtonDown == false && state->data.buttonLeftStick == true ) {
			uai.MouseDown(wxMOUSE_BTN_LEFT);
			mouseInfo.append("L-BUTTON DOWN\n");
			mouseLButtonDown = true;
		}
		else if ( mouseLButtonDown == true && state->data.buttonLeftStick == false ) {
			uai.MouseUp  (wxMOUSE_BTN_LEFT);
			mouseInfo.append("L-BUTTON UP\n");
			mouseLButtonDown = false;
		}
	};
	
	// .............................................................
	auto mouseRButton = [&]() {
		wxUIActionSimulator uai;
		if ( mouseRButtonDown == false && state->data.buttonRightStick == true ) {
			uai.MouseDown(wxMOUSE_BTN_RIGHT);
			mouseInfo.append("R-BUTTON DOWN\n");
			mouseRButtonDown = true;
		}
		else if ( mouseRButtonDown == true && state->data.buttonRightStick == false ) {
			uai.MouseUp  (wxMOUSE_BTN_RIGHT);
			mouseInfo.append("R-BUTTON UP\n");
			mouseRButtonDown = false;
		}
	};
	
	// .............................................................
	auto mouseMove = [&](wxPoint p) {
		wxUIActionSimulator uai;
		mouseInfo.append("<MOVE>\n");
		uai.MouseMove(p);
	};
	
	// update mouse state info
	mouseInfo.clear();
	
	const wxPoint mc(wxGetMousePosition());
	wxPoint nc(mc);
	
	nc.x += (int)(state->data.leftStickX * (state->data.leftTrigger > 0.0 ? state->data.leftTrigger * 8 : 1.0 ));
	nc.y -= (int)(state->data.leftStickY * (state->data.leftTrigger > 0.0 ? state->data.leftTrigger * 8 : 1.0 ));
	
	mouseLButton();
	mouseRButton();
	
	if ( mc != nc )
		mouseMove(nc);
}
/////////////////////////////////////////////////////////////////////
void CncGamepadSpy::update(const GamepadEvent* state) {
/////////////////////////////////////////////////////////////////////
	if ( state == NULL || state->data.connected == false ) {
		// in this case nothing is to do
		if ( m_modeBook->GetSelection() == MODE_GNC )
			return;
			
		m_modeBook->SetSelection(MODE_GNC);
		updateModeText("Not connected");
		
		return;
	}
	
	if ( activated == false ) {
		// in this case nothing is to do
		if ( m_modeBook->GetSelection() == MODE_GNC )
			return;
		
		m_modeBook->SetSelection(MODE_GNC);
		updateModeText("Not activated");
		return;
	}
	
	// apply the cnc mode filter on demand
	const GamepadEvent::UsageMode um = state->data.usageMode;
	switch ( um ) {
		case GamepadEvent::UM_NAV_GUI:	if ( m_modeBook->GetSelection() != MODE_GUI ) {
											m_modeBook->SetSelection(MODE_GUI);
											updateModeText("GUI Interface");
										}
										
										updateGuiMode(state);
										break;
										
		case GamepadEvent::UM_NAV_CNC:	if ( m_modeBook->GetSelection() != MODE_CNC ) {
											m_modeBook->SetSelection(MODE_CNC);
											updateModeText("CNC Interface");
										}
										
										updateCncMode(state);
										break;
	}
}
/////////////////////////////////////////////////////////////////////
void CncGamepadSpy::updateGuiMode(const GamepadEvent* state) {
/////////////////////////////////////////////////////////////////////
	// Don't check this here to get a continuous user interaction
	//if ( state.isSomethingChanged == false )
	//	return;
	releaseMouseActions(state);
	releaseKeyboardActions(state);
	
	m_keyState->SetLabel(keyInfo);
	m_mouseState->SetLabel(mouseInfo);
}
/////////////////////////////////////////////////////////////////////
void CncGamepadSpy::updateCncMode(const GamepadEvent* state) {
/////////////////////////////////////////////////////////////////////
	// Position mode state
	static wxBitmap aBmpNaviXY      = ImageLibGamepadSpy().Bitmap("BMP_NAVI_XY");
	static wxBitmap aBmpNaviZ       = ImageLibGamepadSpy().Bitmap("BMP_NAVI_Z");
	static wxBitmap aBmpStickLeft   = ImageLibGamepadSpy().Bitmap("BMP_STICK_LEFT");
	static wxBitmap aBmpStickRight  = ImageLibGamepadSpy().Bitmap("BMP_STICK_RIGHT");
	static wxBitmap dBmpNaviXY      = ImageLibGamepadSpy().Bitmap("BMP_NAVI_XY").ConvertToDisabled();
	static wxBitmap dBmpNaviZ       = ImageLibGamepadSpy().Bitmap("BMP_NAVI_Z").ConvertToDisabled();
	static wxBitmap dBmpStickLeft   = ImageLibGamepadSpy().Bitmap("BMP_STICK_LEFT").ConvertToDisabled();
	static wxBitmap dBmpStickRight  = ImageLibGamepadSpy().Bitmap("BMP_STICK_RIGHT").ConvertToDisabled();
	
	// Don't check this here to get a continuous user interaction
	//if ( state.isSomethingChanged == false )
	//	return;
	releaseCncActions(state);

	// in this case nothing is to do
	if ( state->isSomethingChanged == false )
		return;
	
	dirXY->setDirection(state->data.dx,	state->data.dy);
	dirZ ->setDirection(CncNoneDir,		state->data.dz);
	
	switch ( state->data.posCtrlMode ) {
		case GamepadEvent::PCM_STICKS:
			GetGpBmp1()->SetBitmap(dBmpNaviXY);
			GetGpBmp2()->SetBitmap(dBmpNaviZ);
			GetGpBmp3()->SetBitmap(aBmpStickLeft);
			GetGpBmp4()->SetBitmap(aBmpStickRight);
			dirXY->setMode(CncGamepadDirectionPanel::M_STICK);
			dirZ ->setMode(CncGamepadDirectionPanel::M_STICK);
			break;
			
		case GamepadEvent::PCM_NAV_XY:
			GetGpBmp1()->SetBitmap(aBmpNaviXY);
			GetGpBmp2()->SetBitmap(dBmpNaviZ);
			GetGpBmp3()->SetBitmap(dBmpStickLeft);
			GetGpBmp4()->SetBitmap(dBmpStickRight);
			dirXY->setMode(CncGamepadDirectionPanel::M_NAVI);
			dirZ ->setMode(CncGamepadDirectionPanel::M_INACTIVE);
			break;
			
		case GamepadEvent::PCM_NAV_Z:
			GetGpBmp1()->SetBitmap(dBmpNaviXY);
			GetGpBmp2()->SetBitmap(aBmpNaviZ);
			GetGpBmp3()->SetBitmap(dBmpStickLeft);
			GetGpBmp4()->SetBitmap(dBmpStickRight);
			dirXY->setMode(CncGamepadDirectionPanel::M_INACTIVE);
			dirZ ->setMode(CncGamepadDirectionPanel::M_NAVI);
			break;
	}
}
///////////////////////////////////////////////////////////////////
void CncGamepadSpy::processStepwiseMovement(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	if ( state.hasMovementInformation() == false )
		return;
		
	const CLD dx = state.data.dx;
	const CLD dy = state.data.dy;
	const CLD dz = state.data.dz;
		
	APP_PROXY::startStepwiseMovement(dx, dy, dz);
}
///////////////////////////////////////////////////////////////////
void CncGamepadSpy::processInteractiveMovement(const GamepadEvent& state) {
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
				if ( cmdHistCtrl )
					cmdHistCtrl->addStartCommand(b);
			}
			
			break;
		}
		// -------------------------------------------------------
		case MS_RUNNING:
		{
			if ( state.hasEmptyMovementInformation() ) {
				const bool b = APP_PROXY::stopInteractiveMove();
				currentMovementState = b ? MS_STOPPED : MS_ERROR;
				currentMoveInfo.reset();
				
				if ( cmdHistCtrl )
					cmdHistCtrl->addStopCommand(b);
			}
			else {
				if ( currentMoveInfo.update(dx, dy, dz) == true ) {
					
					const bool b = APP_PROXY::updateInteractiveMove(dx, dy, dz, translateStickToSpeedValue(state) );
					currentMovementState = b ? MS_RUNNING : MS_ERROR;
					currentMoveInfo.reset();
					
					if ( cmdHistCtrl )
						cmdHistCtrl->addUpdateCommand(b, wxString::Format("%+d, %+d, %+d", (int)dx, (int)dy, (int)dz));
				}
				else {
					const bool b = APP_PROXY::updateInteractiveMove();
					currentMovementState = b ? MS_RUNNING : MS_ERROR;
					currentMoveInfo.reset();
					
					if ( cmdHistCtrl )
						cmdHistCtrl->addUpdateCommand(b, "");
				}
			}
			
			break;
		}
		// -------------------------------------------------------
		case MS_ERROR:
		{
			const bool b = APP_PROXY::stopInteractiveMove();
			currentMovementState = b ? MS_STOPPED : MS_ERROR;
			currentMoveInfo.reset();
			
			if ( cmdHistCtrl )
				cmdHistCtrl->addStopCommand(b);
				
			break;
		}
	}
}
///////////////////////////////////////////////////////////////////
int CncGamepadSpy::translateStickToSpeedValue(const GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	const float minStrickValue =  0.0f;
	const float maxStrickValue = 16.0f;
	const float v               = std::max(minStrickValue, std::min(maxStrickValue, state.data.minStickLen));
	
	if		( std::less_equal<float>{}(v,  4.0) ) return 1;
	else if	( std::less_equal<float>{}(v,  8.0) ) return 2;
	else if	( std::less_equal<float>{}(v, 11.0) ) return 3;
	else if	( std::less_equal<float>{}(v, 14.0) ) return 4;
	else if	( std::less_equal<float>{}(v, 16.0) ) return 5;
	
	return 1;
}
