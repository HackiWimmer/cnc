#ifndef GLOBAL_STRINGS_H
#define GLOBAL_STRINGS_H

#include "wx/string.h"
#include "Version.h"

// ----------------------------------------------------------------
// do this here (outside of Version.h) to have full wxString support

	#define CNC_VERSION_MAJOR_STR   wxString::Format("%d", CNC_VERSION_MAJOR_NUM)
	#define CNC_VERSION_MINOR_STR   wxString::Format("%d", CNC_VERSION_MINOR_NUM)
	#define CNC_VERSION_INDEX_STR   wxString::Format("%d", CNC_VERSION_INDEX_NUM)
	#define CNC_VERSION_BUILD_STR   wxString::Format("%d", CNC_VERSION_BUILD_NUM)

	#define CNC_VERSION_STR         wxString::Format("%d.%d.%d (%d)", \
									CNC_VERSION_MAJOR_NUM, \
									CNC_VERSION_MINOR_NUM, \
									CNC_VERSION_INDEX_NUM, \
									CNC_VERSION_BUILD_NUM)
									
	#define CNC_VERSION_LONG_STR    wxString::Format("%d.%d.%d (%d) [%02d.%02d.%04d %02d:%02d:%02d]", \
									CNC_VERSION_MAJOR_NUM, \
									CNC_VERSION_MINOR_NUM, \
									CNC_VERSION_INDEX_NUM, \
									CNC_VERSION_BUILD_NUM, \
									CNC_BUILD_DAY_INT, \
									CNC_BUILD_MONTH_INT, \
									CNC_BUILD_YEAR_INT, \
									CNC_BUILD_HOUR_INT, \
									CNC_BUILD_MIN_INT, \
									CNC_BUILD_SEC_INT) 
									
// ----------------------------------------------------------------

////////////////////////////////////////////////////////////////////
namespace VersionInfo 
{
	const wxString major				= CNC_VERSION_MAJOR_STR;
	const wxString minor				= CNC_VERSION_MINOR_STR;
	const wxString index				= CNC_VERSION_INDEX_STR;

	const wxString text 				= "";
	const wxString number				= CNC_VERSION_STR;
	const wxString longNumber			= CNC_VERSION_LONG_STR;
};

////////////////////////////////////////////////////////////////////
namespace PortSelector 
{
	const wxString BMP_PS_CONNECTED		= "BMP_PS_CONNECTED";
	const wxString BMP_PS_AVAILABLE		= "BMP_PS_AVAILABLE";
	const wxString BMP_PS_ACCESS_DENIED	= "BMP_PS_ACCESS_DENIED";
	const wxString BMP_PS_UNKNOWN		= "BMP_PS_UNKNOWN";
};

////////////////////////////////////////////////////////////////////
struct GlobalConstStringDatabase 
{
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
	const wxString programTitel			= wxString(CNC_DESCRIPTION_STR);
	const wxString copyRight			= wxString(CNC_COPYRIGHT_STR);
	const wxString versionNumber		= VersionInfo::number;
	const wxString programVersionOnly	= wxString(wxString::Format("%s - %s", CNC_VERSION_STR, buildFlag));
	const wxString programVersionShort	= wxString(wxString::Format("%s.%s - %s", programTitel, CNC_VERSION_STR, buildFlag));
	const wxString programVersionLong	= wxString(wxString::Format("%s.%s - %s", programTitel, CNC_VERSION_LONG_STR, buildFlag));
	
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
