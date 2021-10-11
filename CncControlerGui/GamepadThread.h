#ifndef GameControllerThread_H
#define GameControllerThread_H

#include <wx/thread.h>
#include "GamepadEvent.h"
#include "CncPolarRegionDetector.h"
#include "CncGamePad.h"

////////////////////////////////////////////////////////////////////////////////////
class MainFrame;

typedef void (wxEvtHandler::*GamepadEventFunction)(GamepadEvent&);
#define GamepadEventHandler(func) wxEVENT_HANDLER_CAST(GamepadEventFunction, func)

class GamepadThread : public wxThread {
	
	protected:
		MainFrame*					pHandler;
		CncPolarRegionDetector		polarDetector;
		bool						exit;
		bool						prevButtonLeftStick;
		bool						prevButtonRightStick;
		bool						prevBackButton;
		int							defaultSleepMillis;
		int							currentSleepMillis;
		int							avoidSwitchBouncingFact; 
		GamepadEvent::UsageMode 	prevUsageMode;
		GamepadEvent::PosCtrlMode 	prevPosCtrlMode;
		
		virtual ExitCode Entry();
		void evaluateNotifications(const CncGamepad& gamepad, GamepadEvent& state);
		
		void sendMessage(const wxString& msg);
		
	public:
		GamepadThread(MainFrame *handler);
		virtual ~GamepadThread();
		
		// thread interface
		void stop();
};

#endif
