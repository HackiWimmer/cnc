#ifndef SOFTWARE_SERIAL_H
#define SOFTWARE_SERIAL_H

	#include "SerialEndPoint.h"

	// defined at SerialThread.cpp
	extern SerialEndPoint* SERIAL_END_PTR_FOR_ARDUINO_ENV;
	#define Serial (*SERIAL_END_PTR_FOR_ARDUINO_ENV)

#endif
