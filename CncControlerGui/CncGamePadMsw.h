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
		bool isPressed(WORD button) const;
		
	public:
		CncGamepadMsw() 
		: connected(true)
		, cId(-1)
		, stickMode(STICK_MODE1)
		, deadzoneX(0.05f)
		, deadzoneY(0.02f) 
		, leftStickX(0.0)
		, leftStickY(0.0)
		, rightStickX(0.0)
		, rightStickY(0.0)
		, leftTrigger(0.0)
		, rightTrigger(0.0)
		{}
		
		CncGamepadMsw(float dzX, float dzY) 
		: connected(true) 
		, cId(-1)
		, stickMode(STICK_MODE1)
		, deadzoneX(dzX)
		, deadzoneY(dzY)
		, leftStickX(0.0)
		, leftStickY(0.0)
		, rightStickX(0.0)
		, rightStickY(0.0)
		, leftTrigger(0.0)
		, rightTrigger(0.0)
		{}
		
		virtual ~CncGamepadMsw()
		{}
		
		
		virtual void setConnected()					{ connected = true; }
		virtual void setDisconnected()				{ connected = false; }
		virtual bool wasConnected()					const { return connected; }
		virtual int  getPort();
		virtual bool checkConnection();
		virtual bool refresh();
				
		virtual bool isServiceRuning()				const;
		virtual int  getStickResolutionFactor() 	const { return 16; }

		virtual float getLeftStickX()				const { return leftStickX; }
		virtual float getLeftStickY()				const { return leftStickY; }
		virtual float getRightStickX()				const { return rightStickX; }
		virtual float getRightStickY()				const { return rightStickY; }
		virtual float getLeftTrigger()				const { return leftTrigger; }
		virtual float getRightTrigger()				const { return rightTrigger; }
		
		virtual bool stateButtonA()					const { return isPressed(XINPUT_GAMEPAD_A); }
		virtual bool stateButtonB()					const { return isPressed(XINPUT_GAMEPAD_B); }
		virtual bool stateButtonX()					const { return isPressed(XINPUT_GAMEPAD_X); }
		virtual bool stateButtonY()					const { return isPressed(XINPUT_GAMEPAD_Y); }
		virtual bool stateButtonLeft()				const { return isPressed(XINPUT_GAMEPAD_DPAD_LEFT); }
		virtual bool stateButtonRight()				const { return isPressed(XINPUT_GAMEPAD_DPAD_RIGHT); }
		virtual bool stateButtonUp()				const { return isPressed(XINPUT_GAMEPAD_DPAD_UP); }
		virtual bool stateButtonDown()				const { return isPressed(XINPUT_GAMEPAD_DPAD_DOWN); }
		virtual bool stateButtonStart()				const { return isPressed(XINPUT_GAMEPAD_START); }
		virtual bool stateButtonBack()				const { return isPressed(XINPUT_GAMEPAD_BACK); }
		virtual bool stateButtonLeftStick()			const { return isPressed(XINPUT_GAMEPAD_LEFT_THUMB); }
		virtual bool stateButtonRightStick()		const { return isPressed(XINPUT_GAMEPAD_RIGHT_THUMB); }
		virtual bool stateButtonLeftShoulder()		const { return isPressed(XINPUT_GAMEPAD_LEFT_SHOULDER); }
		virtual bool stateButtonRightShoulder()		const { return isPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER); }
};

#endif