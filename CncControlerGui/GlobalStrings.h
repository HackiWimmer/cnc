#ifndef GLOBAL_STRINGS_H
#define GLOBAL_STRINGS_H

#include "wx/string.h"

////////////////////////////////////////////////////////////////////
namespace VersionInfo {

	const wxString major			= "0";
	const wxString minor			= "9";
	const wxString index			= "0";

	const wxString text 			= "";
	const wxString number			= wxString::Format("%s.%s.%s", major, minor, index);
};

////////////////////////////////////////////////////////////////////
struct GlobalConstStringDatabase {
	
	#ifdef DEBUG
	const wxString buildFlag			= wxString("d");
	#else
	const wxString buildFlag			= wxString("r");
	#endif

	// common
	const wxString programTitel			= wxString("Woodworking CNC Controller");
	const wxString copyRight			= wxString("invented by Hacki Wimmer 2016 - 2019");
	const wxString versionNumber		= VersionInfo::number;
	const wxString programVersion 		= wxString(wxString::Format("%s.%s", versionNumber, buildFlag));
	

	// position spy
	const wxString posSpyRefFormat		= wxString("%08ld");
	
};

#endif
