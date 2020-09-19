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
		CncGamepadGtk() 
		: connected(true)
		, cId(-1)
		, stickMode(STICK_MODE1)
		, deadzoneX(0.05f)
		, deadzoneY(0.02f) 
		{}
		
		CncGamepadGtk(float dzX, float dzY) 
		: connected(true) 
		, cId(-1)
		, stickMode(STICK_MODE1)
		, deadzoneX(dzX)
		, deadzoneY(dzY) {}
		
		virtual ~CncGamepadGtk()
		{}
		
		
		virtual void setConnected()					{ connected = false; }
		virtual void setDisconnected()				{ connected = false; }
		virtual int  getPort()						{ return -1; }
		virtual bool checkConnection()				{ return false; }
		virtual bool refresh()						{ return false; }

		virtual bool isServiceRuning()				const { return false; }
		virtual bool wasConnected()					const { return connected; }
		virtual int  getStickResolutionFactor() 	const { return 16; }

		virtual float getLeftStickX()				const { return 0.0; }
		virtual float getLeftStickY()				const { return 0.0; }
		virtual float getRightStickX()				const { return 0.0; }
		virtual float getRightStickY()				const { return 0.0; }
		virtual float getLeftTrigger()				const { return 0.0; }
		virtual float getRightTrigger()				const { return 0.0; }
		
		virtual bool stateButtonA()					const { return false; }
		virtual bool stateButtonB()					const { return false; }
		virtual bool stateButtonX()					const { return false; }
		virtual bool stateButtonY()					const { return false; }
		virtual bool stateButtonLeft()				const { return false; }
		virtual bool stateButtonRight()				const { return false; }
		virtual bool stateButtonUp()				const { return false; }
		virtual bool stateButtonDown()				const { return false; }
		virtual bool stateButtonStart()				const { return false; }
		virtual bool stateButtonBack()				const { return false; }
		virtual bool stateButtonLeftStick()			const { return false; }
		virtual bool stateButtonRightStick()		const { return false; }
		virtual bool stateButtonLeftShoulder()		const { return false; }
		virtual bool stateButtonRightShoulder()		const { return false; }
};


#endif