#ifndef CNC_GAME_PAD_H
#define CNC_GAME_PAD_H

#include <stdint.h>
#include "CncGamePadOSD.h"

////////////////////////////////////////////////////////////////////////////////////
class CncGamepad : public CncGamePadOSD
{
	public:
		CncGamepad() 
		: CncGamePadOSD()
		{}
		
		CncGamepad(float dzX, float dzY) 
		: CncGamePadOSD(dzX, dzY) 
		{}
		
		virtual ~CncGamepad()
		{}
		
		virtual bool isServiceRuning()				{ return CncGamePadOSD::isServiceRuning(); }
		 
		virtual int  getStickResolutionFactor() 	{ return CncGamePadOSD::getStickResolutionFactor(); }
		 
		virtual float getLeftStickX()				{ return CncGamePadOSD::getLeftStickX(); }
		virtual float getLeftStickY()				{ return CncGamePadOSD::getLeftStickY(); }
		virtual float getRightStickX()				{ return CncGamePadOSD::getRightStickX(); }
		virtual float getRightStickY()				{ return CncGamePadOSD::getRightStickY(); }
		virtual float getLeftTrigger()				{ return CncGamePadOSD::getLeftTrigger(); }
		virtual float getRightTrigger()				{ return CncGamePadOSD::getRightTrigger(); }
		
		virtual void setConnected()					{ return CncGamePadOSD::setConnected(); }
		virtual void setDisconnected()				{ return CncGamePadOSD::setDisconnected(); }
		virtual bool wasConnected()					{ return CncGamePadOSD::wasConnected(); }
		virtual int  getPort() 						{ return CncGamePadOSD::getPort(); }
		virtual bool checkConnection()				{ return CncGamePadOSD::checkConnection(); }
		virtual bool refresh()						{ return CncGamePadOSD::refresh(); }
		
		virtual bool stateButtonA()					{ return CncGamePadOSD::stateButtonA(); }
		virtual bool stateButtonB()					{ return CncGamePadOSD::stateButtonB(); }
		virtual bool stateButtonX()					{ return CncGamePadOSD::stateButtonX(); }
		virtual bool stateButtonY()					{ return CncGamePadOSD::stateButtonY(); }
		virtual bool stateButtonLeft()				{ return CncGamePadOSD::stateButtonLeft(); }
		virtual bool stateButtonRight()				{ return CncGamePadOSD::stateButtonRight(); }
		virtual bool stateButtonUp()				{ return CncGamePadOSD::stateButtonUp(); }
		virtual bool stateButtonDown()				{ return CncGamePadOSD::stateButtonDown(); }
		virtual bool stateButtonStart()				{ return CncGamePadOSD::stateButtonStart(); }
		virtual bool stateButtonBack()				{ return CncGamePadOSD::stateButtonBack(); }
		virtual bool stateButtonLeftStick()			{ return CncGamePadOSD::stateButtonLeftStick(); }
		virtual bool stateButtonRightStick()		{ return CncGamePadOSD::stateButtonRightStick(); }
		virtual bool stateButtonLeftShoulder()		{ return CncGamePadOSD::stateButtonLeftShoulder(); }
		virtual bool stateButtonRightShoulder()		{ return CncGamePadOSD::stateButtonRightShoulder(); }
};

#endif
