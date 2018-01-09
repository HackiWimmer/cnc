#ifndef SERIAL_OSD_H
#define SERIAL_OSD_H

#define ARDUINO_WAIT_TIME 2000

	#ifdef __WXMSW__
		#include "SerialMsw.h"
		typedef SerialMsw SerialOSD;
	#else 
		#include "SerialGtk.h"
		typedef SerialGtk SerialOSD;
	#endif

#endif