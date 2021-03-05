#ifndef GAME_PAD_GTK_H
#define GAME_PAD_GTK_H

class CncGamepadGtk
{
	protected:
		enum StickMode { STICK_MODE1, STICK_MODE2 };
		
		bool connected;
		int cId;
		StickMode stickMode;
		
		float deadzoneX;
		float deadzoneY;
		
	public:
		CncGamepadGtk();
		CncGamepadGtk(float dzX, float dzY);
		virtual ~CncGamepadGtk();
		
		virtual void setConnected()						{ connected = false; }
		virtual void setDisconnected()					{ connected = false; }
		virtual int  getPort()							{ return -1; }
		virtual bool checkConnection()					{ return false; }
		virtual bool refresh()							{ return false; }

		virtual bool isServiceRuning()					const { return false; }
		virtual bool wasConnected()						const { return connected; }
		virtual int  getStickResolutionFactor()			const { return 16; }

		virtual float getLeftStickX()					const { return 0.0; }
		virtual float getLeftStickY()					const { return 0.0; }
		virtual float getRightStickX()					const { return 0.0; }
		virtual float getRightStickY()					const { return 0.0; }
		virtual float getLeftTrigger()					const { return 0.0; }
		virtual float getRightTrigger()					const { return 0.0; }
		
		virtual bool stateButtonA()						const { return false; }
		virtual bool stateButtonB()						const { return false; }
		virtual bool stateButtonX()						const { return false; }
		virtual bool stateButtonY()						const { return false; }
		virtual bool stateButtonLeft()					const { return false; }
		virtual bool stateButtonRight()					const { return false; }
		virtual bool stateButtonUp()					const { return false; }
		virtual bool stateButtonDown()					const { return false; }
		virtual bool stateButtonStart()					const { return false; }
		virtual bool stateButtonBack()					const { return false; }
		virtual bool stateButtonLeftStick()				const { return false; }
		virtual bool stateButtonRightStick()			const { return false; }
		virtual bool stateButtonLeftShoulder()			const { return false; }
		virtual bool stateButtonRightShoulder()			const { return false; }
		
		virtual bool isChangedLeftStickX()				const { return false; }
		virtual bool isChangedLeftStickY()				const { return false; }
		virtual bool isChangedRightStickX()				const { return false; }
		virtual bool isChangedRightStickY()				const { return false; }
		virtual bool isChangedLeftTrigger()				const { return false; }
		virtual bool isChangedRightTrigger()			const { return false; }
		virtual bool isChangedButtonA()					const { return false; }
		virtual bool isChangedButtonB()					const { return false; }
		virtual bool isChangedButtonX()					const { return false; }
		virtual bool isChangedButtonY()					const { return false; }
		virtual bool isChangedButtonLeft()				const { return false; }
		virtual bool isChangedButtonRight()				const { return false; }
		virtual bool isChangedButtonUp()				const { return false; }
		virtual bool isChangedButtonDown()				const { return false; }
		virtual bool isChangedButtonStart()				const { return false; }
		virtual bool isChangedButtonBack()				const { return false; }
		virtual bool isChangedButtonLeftStick()			const { return false; }
		virtual bool isChangedButtonRightStick()		const { return false; }
		virtual bool isChangedButtonLeftShoulder()		const { return false; }
		virtual bool isChangedButtonRightShoulder()		const { return false; }
};


#endif