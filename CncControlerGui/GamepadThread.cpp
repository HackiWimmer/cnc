#include "GlobalFunctions.h"
#include "MainFrame.h"
#include "GamepadThread.h"


///////////////////////////////////////////////////////////////////
GamepadThread::GamepadThread(MainFrame *handler)
: wxThread(wxTHREAD_DETACHED)
, pHandler					(handler)
, exit						(false)
, prevButtonLeftStick		(false)
, prevButtonRightStick		(false)
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
	GamepadEvent initEvt(wxEVT_GAMEPAD_THREAD, MainFrame::EventId::INITIALIZED);
	wxPostEvent(pHandler, initEvt);
	
	CncGamepad gamepad(0.9f, 0.9f);
	GamepadEvent refState(wxEVT_GAMEPAD_THREAD, MainFrame::EventId::GAMEPAD_STATE);
	
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
		
		// send a notification message - on demand only
		if ( curState.isSomethingChanged(refState) == true ) {
			// post notification event
			wxPostEvent(pHandler, curState);
		} 
		else {
			// create and send a heartbeat
			GamepadEvent heartbeat(wxEVT_GAMEPAD_THREAD, MainFrame::EventId::GAMEPAD_HEARTBEAT);
			heartbeat.data = curState.data;
			wxPostEvent(pHandler, heartbeat);
		}
		
		// swap buffers
		refState.data = curState.data;
		
		this->Sleep(100);
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
void GamepadThread::evaluateNotifications(CncGamepad& gamepad, GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	state.data.buttonA					= gamepad.stateButtonA();
	state.data.buttonB 					= gamepad.stateButtonB();
	state.data.buttonX 					= gamepad.stateButtonX();
	state.data.buttonY 					= gamepad.stateButtonY();
	state.data.buttonLeft 				= gamepad.stateButtonLeft();
	state.data.buttonRight 				= gamepad.stateButtonRight();
	state.data.buttonUp 				= gamepad.stateButtonUp();
	state.data.buttonDown 				= gamepad.stateButtonDown();
	state.data.buttonStart				= gamepad.stateButtonStart();
	state.data.buttonBack				= gamepad.stateButtonBack();
	state.data.buttonLeftStick			= gamepad.stateButtonLeftStick();
	state.data.buttonRightStick			= gamepad.stateButtonRightStick();
	state.data.buttonLeftShoulder		= gamepad.stateButtonLeftShoulder();
	state.data.buttonRightShoulder		= gamepad.stateButtonRightShoulder();
	
	state.data.leftTrigger				= gamepad.getLeftTrigger();
	state.data.rightTrigger				= gamepad.getRightTrigger();
	
	state.data.stickResolutionFactor	= gamepad.getStickResolutionFactor();
	
	state.data.leftStickX				= gamepad.getLeftStickX();
	state.data.leftStickY				= gamepad.getLeftStickY();
	state.data.rightStickX				= gamepad.getRightStickX();
	state.data.rightStickY				= gamepad.getRightStickY();
	
	// -----------------------------------------------------------
	if ( prevButtonRightStick == false && state.data.buttonRightStick == true ) { 
		const bool b = prevPosCtrlMode == GamepadEvent::PCM_STICKS || prevPosCtrlMode == GamepadEvent::PCM_NAV_XY;
		if ( b )	state.data.posCtrlMode = GamepadEvent::PCM_NAV_Z;
		else		state.data.posCtrlMode = GamepadEvent::PCM_STICKS;
		
		prevButtonRightStick	= true;
		prevPosCtrlMode 		= state.data.posCtrlMode;
	}
	else {
		prevButtonRightStick	= false;
		state.data.posCtrlMode 	= prevPosCtrlMode;
	}
	
	if ( prevButtonLeftStick == false && state.data.buttonLeftStick == true ) { 
		const bool b = prevPosCtrlMode == GamepadEvent::PCM_STICKS || prevPosCtrlMode == GamepadEvent::PCM_NAV_Z;
		if ( b  )	state.data.posCtrlMode = GamepadEvent::PCM_NAV_XY; 
		else		state.data.posCtrlMode = GamepadEvent::PCM_STICKS;
		
		prevButtonLeftStick		= true;
		prevPosCtrlMode 		= state.data.posCtrlMode;
	}
	else {
		prevButtonLeftStick		= false;
		state.data.posCtrlMode 	= prevPosCtrlMode;
	}

	typedef CncLinearDirection CLD;
	
	auto isValueInRange = [&](float value) { 
		const float threshold = (float)state.data.stickResolutionFactor / 4;
		return (abs(value) >= abs(threshold)); 
	};
	
	state.data.dx 		= CLD::CncNoneDir;
	state.data.dy 		= CLD::CncNoneDir;
	state.data.dz 		= CLD::CncNoneDir;
	
	const bool left		= state.data.buttonLeft; 
	const bool right	= state.data.buttonRight;
	const bool up		= state.data.buttonUp;
	const bool down		= state.data.buttonDown;
	
	switch ( state.data.posCtrlMode ) {
		case GamepadEvent::PCM_STICKS: {
		
			if ( isValueInRange(state.data.leftStickX) )
				state.data.dx = state.data.leftStickX  >= 0.0f ? CLD::CncPosDir : CLD::CncNegDir;
								
			if ( isValueInRange(state.data.leftStickY) )
				state.data.dy = state.data.leftStickY  >= 0.0f ? CLD::CncPosDir : CLD::CncNegDir;
				
			if ( isValueInRange(state.data.rightStickY) )
				state.data.dz = state.data.rightStickY >= 0.0f ? CLD::CncPosDir : CLD::CncNegDir;
				
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
}

