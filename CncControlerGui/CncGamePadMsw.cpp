#include <math.h>
#include <iostream>
#include "CncGamePadMsw.h"

#include <Windows.h>

///////////////////////////////////////////////////////////////////
CncGamepadMsw::CncGamepadMsw() 
: connected			(true)
, cId				(-1)
, stickMode			(STICK_MODE1)
, deadzoneX			(0.05f)
, deadzoneY			(0.02f) 
, currState			()
, prevState			()
, prevData			()
, currData			()
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
CncGamepadMsw::CncGamepadMsw(float dzX, float dzY) 
: connected			(true) 
, cId				(-1)
, stickMode			(STICK_MODE1)
, deadzoneX			(dzX)
, deadzoneY			(dzY)
, currState			()
, prevState			()
, prevData			()
, currData			()

///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
CncGamepadMsw::~CncGamepadMsw() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
bool CncGamepadMsw::isServiceRuning() const {
///////////////////////////////////////////////////////////////////
	const char* name = "Ds3Service";
	
	SC_HANDLE theService, scm;
	SERVICE_STATUS_PROCESS ssStatus;
	DWORD dwBytesNeeded;
	
	scm = OpenSCManager( nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE );
	if( !scm ) {
		return false;
	}
	
	theService = OpenService( scm, name, SERVICE_QUERY_STATUS );
	if( !theService ) {
		CloseServiceHandle( scm );
		return false;
	}
	
	auto result = QueryServiceStatusEx( theService, SC_STATUS_PROCESS_INFO,
		reinterpret_cast<LPBYTE>( &ssStatus ), sizeof( SERVICE_STATUS_PROCESS ),
		&dwBytesNeeded );
	
	CloseServiceHandle( theService );
	CloseServiceHandle( scm );
	
	if( result == 0 ) {
		return false;
	}

	if ( ssStatus.dwCurrentState != SERVICE_RUNNING ) {
		
		std::clog << "Servie status report for: " << name << std::endl;
		switch ( ssStatus.dwCurrentState ) {
			case SERVICE_CONTINUE_PENDING:		std::clog << "The service is about to continue."; 	break;
			case SERVICE_PAUSE_PENDING:			std::clog << "The service is pausing.";				break;
			case SERVICE_PAUSED:				std::clog << "The service is paused."; 				break;
			case SERVICE_RUNNING:				std::clog << "The service is running."; 			break;
			case SERVICE_START_PENDING:			std::clog << "The service is starting."; 			break;
			case SERVICE_STOP_PENDING:			std::clog << "The service is stopping."; 			break;
			case SERVICE_STOPPED:				std::clog << "The service has stopped."; 			break;
		}
		
		std::clog << std::endl;
	}
	
	return ( ssStatus.dwCurrentState == SERVICE_RUNNING );
}
///////////////////////////////////////////////////////////////////
int CncGamepadMsw::getPort() {
///////////////////////////////////////////////////////////////////
	return cId + 1;
}
///////////////////////////////////////////////////////////////////
bool CncGamepadMsw::checkConnection() {
///////////////////////////////////////////////////////////////////
	int controllerId = -1;
	 
	for ( DWORD i = 0; i < XUSER_MAX_COUNT && controllerId == -1; i++ ) {
		XINPUT_STATE cs;
		ZeroMemory(&cs, sizeof(XINPUT_STATE));
		 
		if ( XInputGetState(i, &cs) == ERROR_SUCCESS )
			controllerId = i;
	}
	
	cId = controllerId;
	return controllerId != -1;
}

///////////////////////////////////////////////////////////////////
// Returns false if the controller has been disconnected
bool CncGamepadMsw::refresh() {
///////////////////////////////////////////////////////////////////
	if ( cId == -1 )
		checkConnection();
	 
	if ( cId != -1 ) {
		// swap
		prevState	= currState;
		prevData	= currData;
		ZeroMemory(&currState, sizeof(XINPUT_STATE));
		
		if ( XInputGetState(cId, &currState) != ERROR_SUCCESS ) {
			cId = -1;
			ZeroMemory(&prevState, sizeof(XINPUT_STATE));
			ZeroMemory(&currState, sizeof(XINPUT_STATE));
			prevData.reset();
			currData.reset();
			return false;
		}
	 
		if ( stickMode == STICK_MODE1 ) {
			
			const int resolutionFactor = getStickResolutionFactor();
			currData.leftStickX  = (float)(currState.Gamepad.sThumbLX / (32767 / resolutionFactor));
			currData.leftStickY  = (float)(currState.Gamepad.sThumbLY / (32767 / resolutionFactor));
			
			currData.rightStickX = (float)(currState.Gamepad.sThumbRX / (32767 / resolutionFactor));
			currData.rightStickY = (float)(currState.Gamepad.sThumbRY / (32767 / resolutionFactor));
			
		} else {
			
			float normLX = fmaxf(-1, (float) currState.Gamepad.sThumbLX / 32767);
			float normLY = fmaxf(-1, (float) currState.Gamepad.sThumbLY / 32767);
			
			currData.leftStickX = (abs(normLX) < deadzoneX ? 0 : (abs(normLX) - deadzoneX) * (normLX / abs(normLX)));
			currData.leftStickY = (abs(normLY) < deadzoneY ? 0 : (abs(normLY) - deadzoneY) * (normLY / abs(normLY)));
			 
			if ( deadzoneX > 0 ) currData.leftStickX *= 1 / (1 - deadzoneX);
			if ( deadzoneY > 0 ) currData.leftStickY *= 1 / (1 - deadzoneY);
			 
			float normRX = fmaxf(-1, (float) currState.Gamepad.sThumbRX / 32767);
			float normRY = fmaxf(-1, (float) currState.Gamepad.sThumbRY / 32767);
			 
			currData.rightStickX = (abs(normRX) < deadzoneX ? 0 : (abs(normRX) - deadzoneX) * (normRX / abs(normRX)));
			currData.rightStickY = (abs(normRY) < deadzoneY ? 0 : (abs(normRY) - deadzoneY) * (normRY / abs(normRY)));
			 
			if ( deadzoneX > 0 ) currData.rightStickX *= 1 / (1 - deadzoneX);
			if ( deadzoneY > 0 ) currData.rightStickY *= 1 / (1 - deadzoneY);
		}
		 
		currData.leftTrigger  = (float) currState.Gamepad.bLeftTrigger  / 255;
		currData.rightTrigger = (float) currState.Gamepad.bRightTrigger / 255;
		
		return true;
	}
	
	return false;
}
/////////////////////////////////////////////////////////////////// 
bool CncGamepadMsw::isPressed(WORD button) const {
///////////////////////////////////////////////////////////////////
	return (currState.Gamepad.wButtons & button) != 0;
}
/////////////////////////////////////////////////////////////////// 
bool CncGamepadMsw::isChanged(WORD button) const {
/////////////////////////////////////////////////////////////////// 
	bool p = prevState.Gamepad.wButtons & button;
	bool c = currState.Gamepad.wButtons & button;
	
	return p != c;
}
