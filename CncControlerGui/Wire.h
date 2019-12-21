#ifndef WIRE_H
#define WIREL_H

	#include "WireEndPoint.h"

	// defined at SerialThread.cpp
	extern WireEndPoint* WIRE_END_PTR_FOR_ARDUINO_ENV;
	#define Wire (*WIRE_END_PTR_FOR_ARDUINO_ENV)

#endif
