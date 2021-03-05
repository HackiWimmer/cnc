#ifndef CNC_GAME_PAD_MSW_H
#define CNC_GAME_PAD_MSW_H

#include <stdint.h>
#include <Windows.h>
#include <Xinput.h>

class CncGamepadMsw
{
	protected:
		enum StickMode { STICK_MODE1, STICK_MODE2 };
		
		struct Data {
			float			leftStickX		= 0.0;
			float			leftStickY		= 0.0;
			float			rightStickX		= 0.0;
			float			rightStickY		= 0.0;
			float			leftTrigger		= 0.0;
			float			rightTrigger	= 0.0;
			
			void reset() {
				*this = Data();
			}
		};
		
		bool			connected;
		int				cId;
		StickMode		stickMode;
		
		float			deadzoneX;
		float			deadzoneY;
		
		XINPUT_STATE 	currState;
		XINPUT_STATE 	prevState;
		 
		Data			prevData;
		Data			currData;
		
		bool			isPressed(WORD button) const;
		bool			isChanged(WORD button) const;
		
	public:
	
		CncGamepadMsw();
		CncGamepadMsw(float dzX, float dzY);
		virtual ~CncGamepadMsw();
		
		virtual void setConnected()						{ connected = true; }
		virtual void setDisconnected()					{ connected = false; }
		virtual bool wasConnected()						const { return connected; }
		virtual int  getPort();
		virtual bool checkConnection();
		virtual bool refresh();
				
		virtual bool isServiceRuning()					const;
		virtual int  getStickResolutionFactor() 		const { return 16; }

		virtual float getLeftStickX()					const { return currData.leftStickX; }
		virtual float getLeftStickY()					const { return currData.leftStickY; }
		virtual float getRightStickX()					const { return currData.rightStickX; }
		virtual float getRightStickY()					const { return currData.rightStickY; }
		virtual float getLeftTrigger()					const { return currData.leftTrigger; }
		virtual float getRightTrigger()					const { return currData.rightTrigger; }
		
		virtual bool stateButtonA()						const { return isPressed(XINPUT_GAMEPAD_A); }
		virtual bool stateButtonB()						const { return isPressed(XINPUT_GAMEPAD_B); }
		virtual bool stateButtonX()						const { return isPressed(XINPUT_GAMEPAD_X); }
		virtual bool stateButtonY()						const { return isPressed(XINPUT_GAMEPAD_Y); }
		virtual bool stateButtonLeft()					const { return isPressed(XINPUT_GAMEPAD_DPAD_LEFT); }
		virtual bool stateButtonRight()					const { return isPressed(XINPUT_GAMEPAD_DPAD_RIGHT); }
		virtual bool stateButtonUp()					const { return isPressed(XINPUT_GAMEPAD_DPAD_UP); }
		virtual bool stateButtonDown()					const { return isPressed(XINPUT_GAMEPAD_DPAD_DOWN); }
		virtual bool stateButtonStart()					const { return isPressed(XINPUT_GAMEPAD_START); }
		virtual bool stateButtonBack()					const { return isPressed(XINPUT_GAMEPAD_BACK); }
		virtual bool stateButtonLeftStick()				const { return isPressed(XINPUT_GAMEPAD_LEFT_THUMB); }
		virtual bool stateButtonRightStick()			const { return isPressed(XINPUT_GAMEPAD_RIGHT_THUMB); }
		virtual bool stateButtonLeftShoulder()			const { return isPressed(XINPUT_GAMEPAD_LEFT_SHOULDER); }
		virtual bool stateButtonRightShoulder()			const { return isPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER); }
		
		virtual bool isChangedLeftStickX()				const { return prevData.leftStickX   != currData.leftStickX; }
		virtual bool isChangedLeftStickY()				const { return prevData.leftStickY   != currData.leftStickY; }
		virtual bool isChangedRightStickX()				const { return prevData.rightStickX  != currData.rightStickX; }
		virtual bool isChangedRightStickY()				const { return prevData.rightStickY  != currData.rightStickY; }
		virtual bool isChangedLeftTrigger()				const { return prevData.leftTrigger  != currData.leftTrigger; }
		virtual bool isChangedRightTrigger()			const { return prevData.rightTrigger != currData.rightTrigger; }
		
		virtual bool isChangedButtonA()					const { return isChanged(XINPUT_GAMEPAD_A); }
		virtual bool isChangedButtonB()					const { return isChanged(XINPUT_GAMEPAD_B); }
		virtual bool isChangedButtonX()					const { return isChanged(XINPUT_GAMEPAD_X); }
		virtual bool isChangedButtonY()					const { return isChanged(XINPUT_GAMEPAD_Y); }
		virtual bool isChangedButtonLeft()				const { return isChanged(XINPUT_GAMEPAD_DPAD_LEFT); }
		virtual bool isChangedButtonRight()				const { return isChanged(XINPUT_GAMEPAD_DPAD_RIGHT); }
		virtual bool isChangedButtonUp()				const { return isChanged(XINPUT_GAMEPAD_DPAD_UP); }
		virtual bool isChangedButtonDown()				const { return isChanged(XINPUT_GAMEPAD_DPAD_DOWN); }
		virtual bool isChangedButtonStart()				const { return isChanged(XINPUT_GAMEPAD_START); }
		virtual bool isChangedButtonBack()				const { return isChanged(XINPUT_GAMEPAD_BACK); }
		virtual bool isChangedButtonLeftStick()			const { return isChanged(XINPUT_GAMEPAD_LEFT_THUMB); }
		virtual bool isChangedButtonRightStick()		const { return isChanged(XINPUT_GAMEPAD_RIGHT_THUMB); }
		virtual bool isChangedButtonLeftShoulder()		const { return isChanged(XINPUT_GAMEPAD_LEFT_SHOULDER); }
		virtual bool isChangedButtonRightShoulder()		const { return isChanged(XINPUT_GAMEPAD_RIGHT_SHOULDER); }

};

#endif