#ifndef SERIAL_OSD_H
#define SERIAL_OSD_H

#ifdef __WXMSW__
	#include "SerialMsw.h"
	typedef SerialMsw SerialOSD_Impl;
#else 
	#include "SerialGtk.h"
	typedef SerialGtk SerialOSD_Impl;
#endif

class SerialOSD : public SerialOSD_Impl {
	
	public:
		SerialOSD(CncControl* cnc)
		: SerialOSD_Impl(cnc)
		{}
		
		virtual ~SerialOSD()
		{}
};

#endif