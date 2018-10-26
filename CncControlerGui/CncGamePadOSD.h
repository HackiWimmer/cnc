#ifndef GAME_PAD_OSD_H
#define GAME_PAD_OSD_H

	#ifdef __WXMSW__
		#include "CncGamePadMsw.h"
		typedef CncGamepadMsw CncGamePadOSD;
	#else 
		#include "CncGamePadGtk.h"
		typedef CncGamepadGtk CncGamePadOSD;
	#endif

#endif