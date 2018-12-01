#ifndef GLOBAL_STRINGS_H
#define GLOBAL_STRINGS_H

#include "wx/string.h"

////////////////////////////////////////////////////////////////////
struct GlobalConstStringDatabase {
	
	#ifdef DEBUG
	const wxString buildFlag			= wxString("d");
	#else
	const wxString buildFlag			= wxString("r");
	#endif

	const wxString programTitel			= wxString("Woodworking CNC Controller");
	const wxString copyRight			= wxString("invented by Hacki Wimmer 2016 - 2019");
	const wxString versionNumber		= wxString("0.9.0");
	const wxString programVersion 		= wxString(wxString::Format("%s.%s", versionNumber, buildFlag));
	
	const wxString posSpyRefFormat		= wxString("%08ld");
	
};

#endif
