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
		 
		virtual void setConnected()					{ return CncGamePadOSD::setConnected(); }
		virtual void setDisconnected()				{ return CncGamePadOSD::setDisconnected(); }
		virtual int  getPort() 						{ return CncGamePadOSD::getPort(); }
		virtual bool checkConnection()				{ return CncGamePadOSD::checkConnection(); }
		virtual bool refresh()						{ return CncGamePadOSD::refresh(); }

		virtual bool isServiceRuning()				const { return CncGamePadOSD::isServiceRuning(); }
		virtual bool wasConnected()					const { return CncGamePadOSD::wasConnected(); }
		virtual int  getStickResolutionFactor() 	const { return CncGamePadOSD::getStickResolutionFactor(); }
		
		virtual float getLeftStickX()				const { return CncGamePadOSD::getLeftStickX(); }
		virtual float getLeftStickY()				const { return CncGamePadOSD::getLeftStickY(); }
		virtual float getRightStickX()				const { return CncGamePadOSD::getRightStickX(); }
		virtual float getRightStickY()				const { return CncGamePadOSD::getRightStickY(); }
		virtual float getLeftTrigger()				const { return CncGamePadOSD::getLeftTrigger(); }
		virtual float getRightTrigger()				const { return CncGamePadOSD::getRightTrigger(); }
		
		virtual bool stateButtonA()					const { return CncGamePadOSD::stateButtonA(); }
		virtual bool stateButtonB()					const { return CncGamePadOSD::stateButtonB(); }
		virtual bool stateButtonX()					const { return CncGamePadOSD::stateButtonX(); }
		virtual bool stateButtonY()					const { return CncGamePadOSD::stateButtonY(); }
		virtual bool stateButtonLeft()				const { return CncGamePadOSD::stateButtonLeft(); }
		virtual bool stateButtonRight()				const { return CncGamePadOSD::stateButtonRight(); }
		virtual bool stateButtonUp()				const { return CncGamePadOSD::stateButtonUp(); }
		virtual bool stateButtonDown()				const { return CncGamePadOSD::stateButtonDown(); }
		virtual bool stateButtonStart()				const { return CncGamePadOSD::stateButtonStart(); }
		virtual bool stateButtonBack()				const { return CncGamePadOSD::stateButtonBack(); }
		virtual bool stateButtonLeftStick()			const { return CncGamePadOSD::stateButtonLeftStick(); }
		virtual bool stateButtonRightStick()		const { return CncGamePadOSD::stateButtonRightStick(); }
		virtual bool stateButtonLeftShoulder()		const { return CncGamePadOSD::stateButtonLeftShoulder(); }
		virtual bool stateButtonRightShoulder()		const { return CncGamePadOSD::stateButtonRightShoulder(); }
		virtual bool hasMovement()					const { return !hasEmptyMovement(); }

		virtual bool hasEmptyMovement()				const;
		virtual bool isAnyStickActive()				const;
		virtual bool isLeftStickActive()			const;
		virtual bool isRightStickActive()			const;
		virtual bool isNaviButtonActive()			const;
};

#endif
