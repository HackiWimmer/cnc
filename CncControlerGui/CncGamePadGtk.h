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
		
		virtual ~CncGamepadMsw()
		{}
		
		virtual void setConnected()					{ connected = false; }
		virtual void setDisconnected()				{ connected = false; }
		virtual bool wasConnected()					{ return connected; }
		virtual int  getPort()						{ return -1; }
		virtual bool checkConnection()				{ return false; }
		virtual bool refresh()						{ return false; }
				
		virtual float getLeftStickX()				{ return 0.0; }
		virtual float getLeftStickY()				{ return 0.0; }
		virtual float getRightStickX()				{ return 0.0; }
		virtual float getRightStickY()				{ return 0.0; }
		virtual float getLeftTrigger()				{ return 0.0; }
		virtual float getRightTrigger()				{ return 0.0; }
		
		virtual bool stateButtonA()					{ return false; }
		virtual bool stateButtonB()					{ return false; }
		virtual bool stateButtonX()					{ return false; }
		virtual bool stateButtonY()					{ return false; }
		virtual bool stateButtonLeft()				{ return false; }
		virtual bool stateButtonRight()				{ return false; }
		virtual bool stateButtonUp()				{ return false; }
		virtual bool stateButtonDown()				{ return false; }
		virtual bool stateButtonStart()				{ return false; }
		virtual bool stateButtonBack()				{ return false; }
		virtual bool stateButtonLeftStick()			{ return false; }
		virtual bool stateButtonRightStick()		{ return false; }
		virtual bool stateButtonLeftShoulder()		{ return false; }
		virtual bool stateButtonRightShoulder()		{ return false; }
};


#endif