#include <wx/app.h>
#include "GlobalFunctions.h"
#include "MainFrameProxy.h"
#include "CncAnchorInfo.h"
#include "CncGamepadFilter.h"
#include "CncTemplateContext.h"
#include "CncBoundarySpace.h"
#include "CncContext.h"

////////////////////////////////////////////////////////////////////////
CncContext::CncContext() 
: timestamps				()
, secureModeInfo			()
, versionInfoMap			()
, boundarySpace				(new CncBoundarySpace())
, anchorMap					(new CncAnchorMap())
, templateContext			(new CncTemplateContext(boundarySpace))
, gamepadFilterInstance		(new CncGamepadFilterInstance())
////////////////////////////////////////////////////////////////////////
{
	#ifdef __WXMSW__
		os = OSType::WXMSW;
	#endif
	
	#ifdef __WXGTK__
		os = OSType::WXGTK;
	#endif
}
////////////////////////////////////////////////////////////////////////
CncContext::~CncContext() {
////////////////////////////////////////////////////////////////////////
	
	wxDELETE( boundarySpace );
	wxDELETE( anchorMap );
	wxDELETE( templateContext );
	wxDELETE( gamepadFilterInstance );
	
	APPEND_LOCATION_TO_STACK_TRACE_FILE
}
////////////////////////////////////////////////////////////////////////
const char* CncContext::getOSTypeAsString() {
////////////////////////////////////////////////////////////////////////
	switch ( os ) {
		case OSType::WXMSW:		return "wxWin";
		case OSType::WXGTK:		return "wxGtk";
		
		case OSType::UNDEF:		return "Undefined";
	}
	
	return "Unknown";
}
////////////////////////////////////////////////////////////////////////
void CncContext::setProbeMode(bool state) { 
////////////////////////////////////////////////////////////////////////
	probeMode = state;
	
	if ( APP_PROXY::isAppPointerAvailable() == true )
		APP_PROXY::decorateProbeMode(probeMode);
}
////////////////////////////////////////////////////////////////////////
const std::ostream& CncContext::traceVersionInfo(std::ostream& os) const {
////////////////////////////////////////////////////////////////////////
	os << "Version Information:\n";
	
	for ( auto it = versionInfoMap.begin(); it != versionInfoMap.end(); ++it ) {
		wxString value(it->second);
		value.Replace("\n", "");
		os << it->first << " = " << value << std::endl;
	}
	
	return os;
}
////////////////////////////////////////////////////////////////////////
const std::ostream& CncContext::traceCommandLineParameter(std::ostream& os) const {
////////////////////////////////////////////////////////////////////////
	for (int i=0; i<wxTheApp->argc; i++ ) 
		os << wxTheApp->argv[i] << " ";
	
	return os;
}
////////////////////////////////////////////////////////////////////////
const wxString& CncContext::traceCommandLineParameter(wxString& s) const {
////////////////////////////////////////////////////////////////////////
	s.clear();
	for (int i=0; i<wxTheApp->argc; i++ ) {
		s.append(wxTheApp->argv[i]);
		s.append(" ");
	}
	return s;
}
