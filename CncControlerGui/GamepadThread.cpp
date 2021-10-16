#include "GlobalFunctions.h"
#include "MainFrame.h"
#include "GamepadThread.h"

///////////////////////////////////////////////////////////////////
GamepadThread::GamepadThread(MainFrame *handler)
: wxThread(wxTHREAD_DETACHED)
, pHandler					(handler)
, polarDetector				()
, exit						(false)
, prevButtonLeftStick		(false)
, prevButtonRightStick		(false)
, prevBackButton			(false)
, defaultSleepMillis		(200)
, currentSleepMillis		(defaultSleepMillis)
, avoidSwitchBouncingFact	(1)
, prevUsageMode				(GamepadEvent::UM_NAV_GUI)
, prevPosCtrlMode			(GamepadEvent::PCM_STICKS)
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
GamepadThread::~GamepadThread() {
///////////////////////////////////////////////////////////////////
	wxCriticalSectionLocker enter(pHandler->pGamepadThreadCS);
	// the thread is being destroyed; make sure not to leave dangling pointers around
	pHandler->gamepadThread = NULL;
}
///////////////////////////////////////////////////////////////////
void GamepadThread::stop() {
///////////////////////////////////////////////////////////////////
	exit = true;
}
///////////////////////////////////////////////////////////////////
wxThread::ExitCode GamepadThread::Entry() {
///////////////////////////////////////////////////////////////////
	APPEND_THREAD_ID_TO_STACK_TRACE_FILE;
	
	// post complete event
	wxQueueEvent(pHandler, new GamepadEvent(wxEVT_GAMEPAD_THREAD, MainFrame::EventId::INITIALIZED));
	
	CncGamepad gamepad(0.9f, 0.9f);
	GamepadEvent refState(wxEVT_GAMEPAD_THREAD, MainFrame::EventId::GAMEPAD_STATE);
	
	wxDateTime tsLast = wxDateTime::Now();
	
	while ( !TestDestroy() ) {
		
		// recheck this here after the sleep
		if ( TestDestroy() ) break;
		if ( exit == true )  break;
		
		GamepadEvent curState(wxEVT_GAMEPAD_THREAD, MainFrame::EventId::GAMEPAD_STATE);
		
		if ( gamepad.refresh() == false ) {
			// gamepad isn't available
			if ( gamepad.wasConnected() == true ) {
				// change connection state
				gamepad.setDisconnected();
				curState.data.connectionStateChanged = true;
			}
			
		} else {
			// gamepad is available
			if ( gamepad.wasConnected() == false ) {
				// change connection state
				gamepad.setConnected();
				curState.data.connectionStateChanged = true;
			}
			
			evaluateNotifications(gamepad, curState);
		}
		
		// update current connection
		curState.data.connected 	= gamepad.wasConnected();
		curState.data.gpId			= gamepad.getPort();
		
		curState.isSomethingChanged	= ( memcmp(&(refState.data), &(curState.data), sizeof(GamepadEvent::Data)) != 0 );
		
		if ( curState.isSomethingChanged )
			tsLast = wxDateTime::Now();

		// fall back to GUI mode
		if ( curState.data.connected == true ) {
			if ( curState.data.usageMode == GamepadEvent::UM_NAV_CNC ) {
				if ( (wxDateTime::Now() - tsLast).Abs().GetSeconds() > 10 ) {
					curState.data.buttonBack	= true;
					curState.data.usageMode		= GamepadEvent::UM_NAV_GUI;
					
					prevBackButton				= true;
					prevUsageMode				= curState.data.usageMode;
				}
			}
		}

		// send a notification message 
		wxQueueEvent(pHandler, new GamepadEvent(curState));
		
		// swap buffers
		refState.data = curState.data;
		
		this->Sleep(currentSleepMillis * avoidSwitchBouncingFact );
		if ( avoidSwitchBouncingFact != 1 )
			avoidSwitchBouncingFact = 1;
			
	} // while
	
	// post complete event
	wxQueueEvent(pHandler, new GamepadEvent(wxEVT_GAMEPAD_THREAD, MainFrame::EventId::COMPLETED));
	
	return NULL;
}
///////////////////////////////////////////////////////////////////
void GamepadThread::sendMessage(const wxString& msg) {
///////////////////////////////////////////////////////////////////
	GamepadEvent* message = new GamepadEvent(wxEVT_GAMEPAD_THREAD, MainFrame::EventId::GAMEPAD_MESSAGE);
	message->SetString(msg);
	
	wxQueueEvent(pHandler, message);
}
///////////////////////////////////////////////////////////////////
void GamepadThread::evaluateNotifications(const CncGamepad& gamepad, GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	state.data.buttonA						= gamepad.stateButtonA();
	state.data.buttonB 						= gamepad.stateButtonB();
	state.data.buttonX 						= gamepad.stateButtonX();
	state.data.buttonY 						= gamepad.stateButtonY();
	state.data.buttonLeft 					= gamepad.stateButtonLeft();
	state.data.buttonRight 					= gamepad.stateButtonRight();
	state.data.buttonUp 					= gamepad.stateButtonUp();
	state.data.buttonDown 					= gamepad.stateButtonDown();
	state.data.buttonStart					= gamepad.stateButtonStart();
	state.data.buttonBack					= gamepad.stateButtonBack();
	state.data.buttonLeftStick				= gamepad.stateButtonLeftStick();
	state.data.buttonRightStick				= gamepad.stateButtonRightStick();
	state.data.buttonLeftShoulder			= gamepad.stateButtonLeftShoulder();
	state.data.buttonRightShoulder			= gamepad.stateButtonRightShoulder();
	
	state.data.leftTrigger					= gamepad.getLeftTrigger();
	state.data.rightTrigger					= gamepad.getRightTrigger();
	
	state.data.stickResolutionFactor		= gamepad.getStickResolutionFactor();
	
	state.data.leftStickX					= gamepad.getLeftStickX();
	state.data.leftStickY					= gamepad.getLeftStickY();
	state.data.rightStickX					= gamepad.getRightStickX();
	state.data.rightStickY					= gamepad.getRightStickY();
	
	state.data.isChangedLeftStickX			= gamepad.isChangedLeftStickX();
	state.data.isChangedLeftStickY			= gamepad.isChangedLeftStickY();
	state.data.isChangedRightStickX			= gamepad.isChangedRightStickX();
	state.data.isChangedRightStickY			= gamepad.isChangedRightStickY();
	state.data.isChangedLeftTrigger			= gamepad.isChangedLeftTrigger();
	state.data.isChangedRightTrigger		= gamepad.isChangedRightTrigger();
	state.data.isChangedButtonA				= gamepad.isChangedButtonA();
	state.data.isChangedButtonB				= gamepad.isChangedButtonB();
	state.data.isChangedButtonX				= gamepad.isChangedButtonX();
	state.data.isChangedButtonY				= gamepad.isChangedButtonY();
	state.data.isChangedButtonLeft			= gamepad.isChangedButtonLeft();
	state.data.isChangedButtonRight			= gamepad.isChangedButtonRight();
	state.data.isChangedButtonUp			= gamepad.isChangedButtonUp();
	state.data.isChangedButtonDown			= gamepad.isChangedButtonDown();
	state.data.isChangedButtonStart			= gamepad.isChangedButtonStart();
	state.data.isChangedButtonBack			= gamepad.isChangedButtonBack();
	state.data.isChangedButtonLeftStick		= gamepad.isChangedButtonLeftStick();
	state.data.isChangedButtonRightStick	= gamepad.isChangedButtonRightStick();
	state.data.isChangedButtonLeftShoulder	= gamepad.isChangedButtonLeftShoulder();
	state.data.isChangedButtonRightShoulder	= gamepad.isChangedButtonRightShoulder();
	
	const int bouncingFact = 3;
	
	// first of all determine the additional usage mode
	if ( prevBackButton == false && state.data.buttonBack == true ) {
		const bool b = prevUsageMode == GamepadEvent::UM_NAV_CNC;
		
		if ( b )	state.data.usageMode = GamepadEvent::UM_NAV_GUI;
		else		state.data.usageMode = GamepadEvent::UM_NAV_CNC;
		
		prevBackButton			= true;
		prevUsageMode			= state.data.usageMode;
		
		avoidSwitchBouncingFact = bouncingFact;
	}
	else {
		prevBackButton			= false;
		state.data.usageMode	= prevUsageMode;
	}
	
	if ( state.data.usageMode == GamepadEvent::UM_NAV_GUI ) {
		state.data.hasEmptyMovement = true;
		// nothing more to do
	}
	else {
		// -----------------------------------------------------------
		if ( prevButtonRightStick == false && state.data.buttonRightStick == true ) { 
			const bool b = prevPosCtrlMode == GamepadEvent::PCM_STICKS || prevPosCtrlMode == GamepadEvent::PCM_NAV_XY;
			if ( b )	state.data.posCtrlMode = GamepadEvent::PCM_NAV_Z;
			else		state.data.posCtrlMode = GamepadEvent::PCM_STICKS;
			
			prevButtonRightStick	= true;
			prevPosCtrlMode 		= state.data.posCtrlMode;
			
			avoidSwitchBouncingFact = bouncingFact;
		}
		else {
			prevButtonRightStick	= false;
			state.data.posCtrlMode 	= prevPosCtrlMode;
		}
		
		if ( prevButtonLeftStick == false && state.data.buttonLeftStick == true ) { 
			const bool b = prevPosCtrlMode == GamepadEvent::PCM_STICKS || prevPosCtrlMode == GamepadEvent::PCM_NAV_Z;
			if ( b )	state.data.posCtrlMode = GamepadEvent::PCM_NAV_XY; 
			else		state.data.posCtrlMode = GamepadEvent::PCM_STICKS;
			
			prevButtonLeftStick		= true;
			prevPosCtrlMode 		= state.data.posCtrlMode;
			
			avoidSwitchBouncingFact = bouncingFact;

		}
		else {
			prevButtonLeftStick		= false;
			state.data.posCtrlMode 	= prevPosCtrlMode;
		}
		
		typedef CncLinearDirection CLD;
		
		state.data.dx				= CLD::CncNoneDir;
		state.data.dy				= CLD::CncNoneDir;
		state.data.dz				= CLD::CncNoneDir;
		state.data.leftStickLen		= 0.0f;
		state.data.rightStickLen	= 0.0f;
		state.data.minStickLen		= 0.0f;
		state.data.maxStickLen		= 0.0f;
		
		const bool left		= state.data.buttonLeft; 
		const bool right	= state.data.buttonRight;
		const bool up		= state.data.buttonUp;
		const bool down		= state.data.buttonDown;
		
		switch ( state.data.posCtrlMode ) {
			case GamepadEvent::PCM_STICKS: {
				polarDetector.setCoordinatesAbs(state.data.leftStickX, state.data.leftStickY);
				state.data.dx = polarDetector.getDirectionX();
				state.data.dy = polarDetector.getDirectionY();
				state.data.leftStickLen = polarDetector.getLength();
				
				polarDetector.setCoordinatesAbs(float(0.0), state.data.rightStickY);
				state.data.dz = polarDetector.getDirectionY();
				state.data.rightStickLen = polarDetector.getLength();
				
				// length are always positive
				const bool bln = cnc::dblCmp::nu(state.data.leftStickLen);
				const bool brn = cnc::dblCmp::nu(state.data.rightStickLen);
				
				if      ( bln && !brn )
				{
					state.data.minStickLen = state.data.rightStickLen;
					state.data.maxStickLen = state.data.rightStickLen;
				}
				else if ( !bln && brn )
				{
					state.data.minStickLen = state.data.leftStickLen;
					state.data.maxStickLen = state.data.leftStickLen;
				}
				else
				{
					state.data.minStickLen = std::min(state.data.leftStickLen, state.data.rightStickLen);
					state.data.maxStickLen = std::max(state.data.leftStickLen, state.data.rightStickLen);
				}
				
				break;
			}
			case GamepadEvent::PCM_NAV_XY: {
				if ( left  == true ) 	state.data.dx = CLD::CncNegDir; 
				if ( right == true ) 	state.data.dx = CLD::CncPosDir; 
				if ( up    == true ) 	state.data.dy = CLD::CncPosDir;
				if ( down  == true ) 	state.data.dy = CLD::CncNegDir;
				
				break;
			}
			case GamepadEvent::PCM_NAV_Z: {
				if ( up    == true ) 	state.data.dz = CLD::CncPosDir;
				if ( down  == true ) 	state.data.dz = CLD::CncNegDir;
				
				break;
			}
		}
		
		state.data.hasEmptyMovement		= gamepad.hasEmptyMovement();
		state.data.isAnyStickActive		= gamepad.isAnyStickActive();
		state.data.isLeftStickActive	= gamepad.isLeftStickActive();
		state.data.isRightStickActive	= gamepad.isRightStickActive();
		state.data.isNaviButtonActive	= gamepad.isNaviButtonActive();
	}
}


