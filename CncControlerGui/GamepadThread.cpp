#include "GlobalFunctions.h"
#include "MainFrame.h"
#include "GamepadThread.h"

///////////////////////////////////////////////////////////////////
std::ostream& GamepadEvent::Data::trace(std::ostream &ostr, const GamepadEvent::Data &data) {
///////////////////////////////////////////////////////////////////
	ostr << "Button state(A;B;X;Y)          : " 
		<< data.buttonA 				<< "; " 
		<< data.buttonB 				<< "; " 
		<< data.buttonX 				<< "; " 
		<< data.buttonY 
		<< std::endl;
		
	ostr << "Button state(L;R;U;D)          : " 
		<< data.buttonLeft				<< "; " 
		<< data.buttonRight 			<< "; " 
		<< data.buttonUp 				<< "; " 
		<< data.buttonDown
		<< std::endl; 
		
	ostr << "Button (Start;Back)            : " 
		<< data.buttonStart	<< "; " 
		<< data.buttonBack 
		<< std::endl; 
		
	ostr << "Stick       Button state(L;R)  : " 
		<< data.buttonLeftStick	<< "; " 
		<< data.buttonRightStick 
		<< std::endl; 
		
	ostr << "Shoulder    Button state(L;R)  : " 
		<< data.buttonLeftShoulder	<< "; " 
		<< data.buttonRightShoulder 
		<< std::endl; 
		
	ostr << "Trigger     state(L;R)         : " 
		<< wxString::Format("%1.3lf; ", data.leftTrigger)
		<< wxString::Format("%1.3lf",   data.rightTrigger)
		<< std::endl; 
		
	ostr << "Left Stick  state(X;Y)         : " 
		<< wxString::Format("%+1.1lf; ", data.leftStickX)
		<< wxString::Format("%+1.1lf",   data.leftStickY)
		<< std::endl; 
			
	ostr << "Right Stick state(X;Y)         : " 
		<< wxString::Format("%+1.1lf; ", data.rightStickX)
		<< wxString::Format("%+1.1lf",   data.rightStickY)
		<< std::endl; 
			
	return ostr;
}

///////////////////////////////////////////////////////////////////
GamepadThread::GamepadThread(MainFrame *handler)
: wxThread(wxTHREAD_DETACHED)
, pHandler(handler)
, exit(false)
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
	APPEND_THREAD_IDTO_STACK_TRACE_FILE;
	
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
		} else {
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
void GamepadThread::evaluateNotifications(CncGamepad& gamepad, GamepadEvent& state) {
///////////////////////////////////////////////////////////////////
	state.data.buttonA				= gamepad.stateButtonA();
	state.data.buttonB 				= gamepad.stateButtonB();
	state.data.buttonX 				= gamepad.stateButtonX();
	state.data.buttonY 				= gamepad.stateButtonY();
	state.data.buttonLeft 			= gamepad.stateButtonLeft();
	state.data.buttonRight 			= gamepad.stateButtonRight();
	state.data.buttonUp 			= gamepad.stateButtonUp();
	state.data.buttonDown 			= gamepad.stateButtonDown();
	state.data.buttonStart			= gamepad.stateButtonStart();
	state.data.buttonBack			= gamepad.stateButtonBack();
	state.data.buttonLeftStick		= gamepad.stateButtonLeftStick();
	state.data.buttonRightStick		= gamepad.stateButtonRightStick();
	state.data.buttonLeftShoulder	= gamepad.stateButtonLeftShoulder();
	state.data.buttonRightShoulder	= gamepad.stateButtonRightShoulder();
	
	state.data.leftTrigger			= gamepad.getLeftTrigger();
	state.data.rightTrigger			= gamepad.getRightTrigger();
	
	state.data.leftStickX			= gamepad.getLeftStickX();
	state.data.leftStickY			= gamepad.getLeftStickY();
	state.data.rightStickX			= gamepad.getRightStickX();
	state.data.rightStickY			= gamepad.getRightStickY();
}
