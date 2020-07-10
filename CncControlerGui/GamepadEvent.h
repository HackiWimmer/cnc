#ifndef GAMEPAD_EVENT_H
#define GAMEPAD_EVENT_H

#include <wx/event.h>
#include "CncCommon.h"

////////////////////////////////////////////////////////////////////////////////////
class GamepadEvent;
wxDECLARE_EVENT(wxEVT_GAMEPAD_THREAD, GamepadEvent);

class GamepadEvent : public wxThreadEvent {
	typedef CncLinearDirection CLD;
	
	public:
		enum PosCtrlMode { PCM_STICKS = 0, PCM_NAV_XY = 1, PCM_NAV_Z = 2 };
		
		GamepadEvent(wxEventType eventType = wxEVT_GAMEPAD_THREAD, int id = 0)
		: wxThreadEvent(eventType, id)
		, data()
		{}
		
		explicit GamepadEvent(const GamepadEvent& event)
		: wxThreadEvent(event)
		, data(event.data)
		{}

		virtual wxEvent *Clone() const {
			return new GamepadEvent(*this);
		}
		
		struct Data {
			bool 	connected				= false;
			bool 	connectionStateChanged	= false;
			int  	gpId					= -1;
			
			bool 	buttonA					= false;
			bool 	buttonB					= false;
			bool 	buttonX					= false;
			bool 	buttonY					= false;
			
			bool 	buttonLeft				= false;
			bool 	buttonRight				= false;
			bool 	buttonUp				= false;
			bool 	buttonDown				= false;
			bool 	buttonStart				= false;
			bool 	buttonBack				= false;
			bool 	buttonLeftStick			= false;
			bool 	buttonRightStick		= false;
			bool 	buttonLeftShoulder		= false;
			bool 	buttonRightShoulder		= false;
			
			float 	leftTrigger				= 0.0f;
			float 	rightTrigger			= 0.0f;
			
			int   	stickResolutionFactor	=    1;
			float 	leftStickX				= 0.0f;
			float 	leftStickY				= 0.0f;
			float 	rightStickX				= 0.0f;
			float 	rightStickY				= 0.0f;
			
			PosCtrlMode posCtrlMode			= PCM_STICKS;
			
			CLD 	dx 						= CLD::CncNoneDir;
			CLD 	dy 						= CLD::CncNoneDir;
			CLD 	dz 						= CLD::CncNoneDir;
			
			friend std::ostream &operator<< (std::ostream &ostr, const Data &data) { return trace(ostr, data); }
			static std::ostream& trace(std::ostream &ostr, const GamepadEvent::Data &data);
		} data;
		
		/////////////////////////////////////////////////////////////////////////////
		bool isSomethingChanged(const GamepadEvent& event) {
			return memcmp(&(this->data), &(event.data), sizeof(GamepadEvent::Data)) != 0;
		}
		
		/////////////////////////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const GamepadEvent &event) {
			ostr << event.data;
			return ostr;
		}
};

#endif