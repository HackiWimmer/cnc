#include <stdio.h>
#include <iostream>
#include <math.h>
#include <Windows.h>
#include <Xinput.h>

class Gamepad
{
	private:
		int cId;
		XINPUT_STATE state;
		 
		float deadzoneX;
		float deadzoneY;
	 
	public:
		Gamepad() : deadzoneX(0.05f), deadzoneY(0.02f) {}
		Gamepad(float dzX, float dzY) : deadzoneX(dzX), deadzoneY(dzY) {}
		 
		float leftStickX;
		float leftStickY;
		float rightStickX;
		float rightStickY;
		float leftTrigger;
		float rightTrigger;
		 
		int  GetPort();
		//XINPUT_GAMEPAD *GetState();
		bool CheckConnection();
		bool Refresh();
		bool IsPressed(WORD);
};
 
int Gamepad::GetPort() {
	return cId + 1;
}
 /*
XINPUT_GAMEPAD *Gamepad::GetState() {
	return &state.Gamepad;
}
*/
bool Gamepad::CheckConnection() {
	int controllerId = -1;
	 
	for (DWORD i = 0; i < XUSER_MAX_COUNT && controllerId == -1; i++) {
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		 
		if ( XInputGetState(i, &state) == ERROR_SUCCESS )
			controllerId = i;
	}
	
	cId = controllerId;
	return controllerId != -1;
}

// Returns false if the controller has been disconnected
bool Gamepad::Refresh() {
	if (cId == -1)
		CheckConnection();
	 
	if (cId != -1) {
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		
		if (XInputGetState(cId, &state) != ERROR_SUCCESS) {
			cId = -1;
			return false;
		}
	 
		float normLX = fmaxf(-1, (float) state.Gamepad.sThumbLX / 32767);
		float normLY = fmaxf(-1, (float) state.Gamepad.sThumbLY / 32767);
		 
		leftStickX = (abs(normLX) < deadzoneX ? 0 : (abs(normLX) - deadzoneX) * (normLX / abs(normLX)));
		leftStickY = (abs(normLY) < deadzoneY ? 0 : (abs(normLY) - deadzoneY) * (normLY / abs(normLY)));
		 
		if (deadzoneX > 0) leftStickX *= 1 / (1 - deadzoneX);
		if (deadzoneY > 0) leftStickY *= 1 / (1 - deadzoneY);
		 
		float normRX = fmaxf(-1, (float) state.Gamepad.sThumbRX / 32767);
		float normRY = fmaxf(-1, (float) state.Gamepad.sThumbRY / 32767);
		 
		rightStickX = (abs(normRX) < deadzoneX ? 0 : (abs(normRX) - deadzoneX) * (normRX / abs(normRX)));
		rightStickY = (abs(normRY) < deadzoneY ? 0 : (abs(normRY) - deadzoneY) * (normRY / abs(normRY)));
		 
		if (deadzoneX > 0) rightStickX *= 1 / (1 - deadzoneX);
		if (deadzoneY > 0) rightStickY *= 1 / (1 - deadzoneY);
		 
		leftTrigger = (float) state.Gamepad.bLeftTrigger / 255;
		rightTrigger = (float) state.Gamepad.bRightTrigger / 255;
	 
		return true;
	}
	
	return false;
}
 
bool Gamepad::IsPressed(WORD button) {
	return (state.Gamepad.wButtons & button) != 0;
}


int main(int argc, char **argv)
{
	std::cout << "hello world\n" << std::endl;
	
	Gamepad gamepad;
 
	bool wasConnected = true;
 
	while (true)
	{
		Sleep(100);
	 
		if (!gamepad.Refresh())
		{
			if (wasConnected)
			{
				wasConnected = false;
				std::cout << "Please connect an Xbox 360 controller." << std::endl;
			}
		}
		else
		{
			if (!wasConnected)
			{
				wasConnected = true;
				std::cout << "Controller connected on port " << gamepad.GetPort() << std::endl;
			}
		
		std::cout << "Left thumb stick: (" << gamepad.leftStickX << ", " << gamepad.leftStickY << ")   Right thumb stick : (" 
		                                   << gamepad.rightStickX << ", " << gamepad.rightStickY << ")" << std::endl;
 
		std::cout << "Left analog trigger: " << gamepad.leftTrigger << "   Right analog trigger: " << gamepad.rightTrigger << std::endl;
 
		if (gamepad.IsPressed(XINPUT_GAMEPAD_A)) 
			std::cout << "(A) button pressed" << std::endl;
		}

	}
	
	return 0;
}
