#ifndef CNC_ASYNC_KEYBOARD_STATE
#define CNC_ASYNC_KEYBOARD_STATE

class CncAsyncKeyboardState {
	
	public:
		
		static bool isControlPressed();
		static bool isShiftPressed();
		static bool isAltPressed();
		static bool isAltGrPressed();
		static bool isEscapePressed();
};

#endif
