#include "CncPathListEntry.h"

const bool 				CncPathListEntry::DefaultAlreadyRendered  = false;
const long 				CncPathListEntry::DefaultClientID   	  = -1L;
const CncSpeedMode 		CncPathListEntry::DefaultSpeedMode  	  =  CncSpeedUserDefined;
const double			CncPathListEntry::DefaultSpeedValue 	  = -1.0;
const CncDoublePosition	CncPathListEntry::ZeroTarget			  = {0.0, 0.0, 0.0};
const CncDoubleDistance	CncPathListEntry::NoDistance			  = {0.0, 0.0, 0.0};
const CncNanoTimestamp	CncPathListEntry::NoReference			  = -1LL;

