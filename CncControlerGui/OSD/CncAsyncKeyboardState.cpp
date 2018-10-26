#include "CncAsyncKeyboardState.h"

#ifdef __WXMSW__
	#include <windows.h>

	bool CncAsyncKeyboardState::isControlPressed() 		{ return GetAsyncKeyState(VK_CONTROL) 	!= 0; }
	bool CncAsyncKeyboardState::isShiftPressed()		{ return GetAsyncKeyState(VK_SHIFT) 	!= 0; }
	bool CncAsyncKeyboardState::isAltPressed()			{ return GetAsyncKeyState(VK_MENU) 		!= 0; }
	bool CncAsyncKeyboardState::isAltGrPressed()		{ return GetAsyncKeyState(VK_RMENU) 	!= 0; }
	bool CncAsyncKeyboardState::isEscapePressed()		{ return GetAsyncKeyState(VK_ESCAPE)	!= 0; }
#endif

#ifdef __WXGTK__
	#warning todo GTK implementation
	bool CncAsyncKeyboardState::isControlPressed() 		{ return false; }
	bool CncAsyncKeyboardState::isShiftPressed()		{ return false; }
	bool CncAsyncKeyboardState::isAltPressed()			{ return false; }
	bool CncAsyncKeyboardState::isAltGrPressed()		{ return false; }
	bool CncAsyncKeyboardState::isEscapePressed()		{ return false; }
#endif
