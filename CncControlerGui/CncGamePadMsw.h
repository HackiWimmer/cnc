#ifndef CNC_GAME_PAD_MSW_H
#define CNC_GAME_PAD_MSW_H

#include <stdint.h>
#include <Windows.h>
#include <Xinput.h>

class CncGamepadMsw
{
	protected:
		enum StickMode { STICK_MODE1, STICK_MODE2 };
		
		bool connected;
		int cId;
		StickMode stickMode;
		
		XINPUT_STATE state;
		 
		float deadzoneX;
		float deadzoneY;
		
		float leftStickX;
		float leftStickY;
		float rightStickX;
		float rightStickY;
		float leftTrigger;
		float rightTrigger;
		
		XINPUT_GAMEPAD *getState();
		bool isPressed(WORD button);
		
	public:
		CncGamepadMsw() 
		: connected(true)
		, cId(-1)
		, stickMode(STICK_MODE1)
		, deadzoneX(0.05f)
		, deadzoneY(0.02f) 
		{}
		
		CncGamepadMsw(float dzX, float dzY) 
		: connected(true) 
		, cId(-1)
		, stickMode(STICK_MODE1)
		, deadzoneX(dzX)
		, deadzoneY(dzY) {}
		
		virtual ~CncGamepadMsw()
		{}
		
		virtual bool isServiceRuning();
		
		virtual void setConnected()					{ connected = true; }
		virtual void setDisconnected()				{ connected = false; }
		virtual bool wasConnected()					{ return connected; }
		virtual int  getPort();
		virtual bool checkConnection();
		virtual bool refresh();
				
		virtual float getLeftStickX()				{ return leftStickX; }
		virtual float getLeftStickY()				{ return leftStickY; }
		virtual float getRightStickX()				{ return rightStickX; }
		virtual float getRightStickY()				{ return rightStickY; }
		virtual float getLeftTrigger()				{ return leftTrigger; }
		virtual float getRightTrigger()				{ return rightTrigger; }
		
		virtual bool stateButtonA()					{ return isPressed(XINPUT_GAMEPAD_A); }
		virtual bool stateButtonB()					{ return isPressed(XINPUT_GAMEPAD_B); }
		virtual bool stateButtonX()					{ return isPressed(XINPUT_GAMEPAD_X); }
		virtual bool stateButtonY()					{ return isPressed(XINPUT_GAMEPAD_Y); }
		virtual bool stateButtonLeft()				{ return isPressed(XINPUT_GAMEPAD_DPAD_LEFT); }
		virtual bool stateButtonRight()				{ return isPressed(XINPUT_GAMEPAD_DPAD_RIGHT); }
		virtual bool stateButtonUp()				{ return isPressed(XINPUT_GAMEPAD_DPAD_UP); }
		virtual bool stateButtonDown()				{ return isPressed(XINPUT_GAMEPAD_DPAD_DOWN); }
		virtual bool stateButtonStart()				{ return isPressed(XINPUT_GAMEPAD_START); }
		virtual bool stateButtonBack()				{ return isPressed(XINPUT_GAMEPAD_BACK); }
		virtual bool stateButtonLeftStick()			{ return isPressed(XINPUT_GAMEPAD_LEFT_THUMB); }
		virtual bool stateButtonRightStick()		{ return isPressed(XINPUT_GAMEPAD_RIGHT_THUMB); }
		virtual bool stateButtonLeftShoulder()		{ return isPressed(XINPUT_GAMEPAD_LEFT_SHOULDER); }
		virtual bool stateButtonRightShoulder()		{ return isPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER); }
};

#endif