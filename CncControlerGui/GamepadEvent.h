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
		enum UsageMode		{ UM_NAV_GUI = 0, UM_NAV_CNC = 1 };
		enum PosCtrlMode	{ PCM_STICKS = 0, PCM_NAV_XY = 1, PCM_NAV_Z = 2 };
		
		GamepadEvent(wxEventType eventType = wxEVT_GAMEPAD_THREAD, int id = 0)
		: wxThreadEvent(eventType, id)
		, isSomethingChanged(false)
		, data()
		{}
		
		explicit GamepadEvent(const GamepadEvent& event)
		: wxThreadEvent(event)
		, isSomethingChanged(event.isSomethingChanged)
		, data(event.data)
		{}

		virtual wxEvent *Clone() const {
			return new GamepadEvent(*this);
		}
		
		struct Data {
			
			bool 	connected					= false;
			bool 	connectionStateChanged		= false;
			int  	gpId						= -1;
			
			bool 	buttonA						= false;
			bool 	buttonB						= false;
			bool 	buttonX						= false;
			bool 	buttonY						= false;
			
			bool 	buttonLeft					= false;
			bool 	buttonRight					= false;
			bool 	buttonUp					= false;
			bool 	buttonDown					= false;
			bool 	buttonStart					= false;
			bool 	buttonBack					= false;
			bool 	buttonLeftStick				= false;
			bool 	buttonRightStick			= false;
			bool 	buttonLeftShoulder			= false;
			bool 	buttonRightShoulder			= false;
			
			bool 	hasEmptyMovement			= false;
			bool 	isAnyStickActive			= false;
			bool 	isLeftStickActive			= false;
			bool 	isRightStickActive			= false;
			bool 	isNaviButtonActive			= false;
			
			float 	leftTrigger					= 0.0f;
			float 	rightTrigger				= 0.0f;
			
			int   	stickResolutionFactor		=    1;
			float 	leftStickX					= 0.0f;
			float 	leftStickY					= 0.0f;
			float	leftStickLen				= 0.0f;
			float 	rightStickX					= 0.0f;
			float 	rightStickY					= 0.0f;
			float	rightStickLen				= 0.0f;
			
			float	minStickLen					= 0.0f;
			float	maxStickLen					= 0.0f;
			
			bool isChangedLeftStickX			= false;
			bool isChangedLeftStickY			= false;
			bool isChangedRightStickX			= false;
			bool isChangedRightStickY			= false;
			bool isChangedLeftTrigger			= false;
			bool isChangedRightTrigger			= false;
			bool isChangedButtonA				= false;
			bool isChangedButtonB				= false;
			bool isChangedButtonX				= false;
			bool isChangedButtonY				= false;
			bool isChangedButtonLeft			= false;
			bool isChangedButtonRight			= false;
			bool isChangedButtonUp				= false;
			bool isChangedButtonDown			= false;
			bool isChangedButtonStart			= false;
			bool isChangedButtonBack			= false;
			bool isChangedButtonLeftStick		= false;
			bool isChangedButtonRightStick		= false;
			bool isChangedButtonLeftShoulder	= false;
			bool isChangedButtonRightShoulder	= false;
			
			UsageMode   usageMode			= UM_NAV_CNC;
			PosCtrlMode posCtrlMode			= PCM_STICKS;
			
			CLD 	dx 						= CLD::CncNoneDir;
			CLD 	dy 						= CLD::CncNoneDir;
			CLD 	dz 						= CLD::CncNoneDir;
			
			friend std::ostream &operator<< (std::ostream &ostr, const Data &data) { return trace(ostr, data); }
			static std::ostream& trace(std::ostream &ostr, const GamepadEvent::Data &data);
		};
		
		bool isSomethingChanged;
		Data data;
		
		/////////////////////////////////////////////////////////////////////////////
		bool hasEmptyMovementInformation() const {
			return ( data.dx == CLD::CncNoneDir && data.dy == CLD::CncNoneDir && data.dz == CLD::CncNoneDir );
		}
		
		/////////////////////////////////////////////////////////////////////////////
		bool hasMovementInformation() const {
			return !hasEmptyMovementInformation();
		}
		
		/////////////////////////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const GamepadEvent &event) {
			ostr << event.data;
			return ostr;
		}
};

#endif