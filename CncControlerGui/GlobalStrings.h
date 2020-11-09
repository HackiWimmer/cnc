#ifndef GLOBAL_STRINGS_H
#define GLOBAL_STRINGS_H

#include "wx/string.h"

////////////////////////////////////////////////////////////////////
namespace VersionInfo {

	const wxString major			= "0";
	const wxString minor			= "9";
	const wxString index			= "7";

	const wxString text 			= "";
	const wxString number			= wxString::Format("%s.%s.%s", major, minor, index);
};

////////////////////////////////////////////////////////////////////
struct GlobalConstStringDatabase {
	
	#ifdef DEBUG
		const wxString buildFlag		= wxString("d");
	#else
	
		#ifdef GPROF
			const wxString buildFlag	= wxString("g");
		#else
			const wxString buildFlag	= wxString("r");
		#endif
		
	#endif

	// common
	const wxString programTitel			= wxString("Woodworking CNC Controller");
	const wxString copyRight			= wxString("invented by Hacki Wimmer 2016 - 2021");
	const wxString versionNumber		= VersionInfo::number;
	const wxString programVersion		= wxString(wxString::Format("%s.%s", versionNumber, buildFlag));
	
	// config
	const wxString renderSelectorFormat = wxString("%0.3lf mm - %u PPI");
	
	// position spy
	const wxString posSpyRefFormat		= wxString("%+08ld");
	const wxString pathListRefFormat	= wxString("%+08ld");
	const wxString moveSeqRefFormat		= wxString("%+08ld");
	const wxString gcodeSeqRefFormat	= wxString("%+08ld");
	const wxString vertexTraceFormat	= wxString("%+08ld");
	
};

#endif
