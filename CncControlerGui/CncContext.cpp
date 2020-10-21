#include <wx/app.h>
#include "GlobalFunctions.h"
#include "MainFrameProxy.h"
#include "CncPositionStorageView.h"
#include "CncContext.h"

////////////////////////////////////////////////////////////////////////
CncPositionStorageView* CncContext::PositionStorage::storage	= NULL;
// This initalized all array items with false (default value)
bool CncContext::PositionStorage::posStorageIdState[MAX_SIDS] 	= {};

////////////////////////////////////////////////////////////////////////
CncContext::CncContext() 
: timestamps()
, secureModeInfo()
, versionInfoMap()
, hardwareOriginOffset()
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
void CncContext::PositionStorage::init(uint8_t sid, bool state) {
////////////////////////////////////////////////////////////////////////
	if ( storage == NULL )
		return;
	
	if ( storage->init(sid, state) == false )
		;
}
////////////////////////////////////////////////////////////////////////
void CncContext::PositionStorage::clear(uint8_t sid) {
////////////////////////////////////////////////////////////////////////
	if ( storage == NULL )
		return;
	
	if ( storage->clear(sid) == false )
		;
}
////////////////////////////////////////////////////////////////////////
void CncContext::PositionStorage::addPos(uint8_t sid, const CncLongPosition& p) {
////////////////////////////////////////////////////////////////////////
	if ( CncContext::PositionStorage::isPosStorageEnabledFor(sid) == false )
		return;
	
	if ( storage == NULL )
		return;
	
	if ( storage->addPos(sid, p) == false )
		;
}
////////////////////////////////////////////////////////////////////////
void CncContext::PositionStorage::addPos(uint8_t sid, const CncDoublePosition& p) {
////////////////////////////////////////////////////////////////////////
	if ( CncContext::PositionStorage::isPosStorageEnabledFor(sid) == false )
		return;
		
	if ( storage == NULL )
		return;
	
	if ( storage->addPos(sid, p) == false )
		;
}
////////////////////////////////////////////////////////////////////////
void CncContext::PositionStorage::addPos(uint8_t sid, int32_t px, int32_t py, int32_t pz) {
////////////////////////////////////////////////////////////////////////
	if ( CncContext::PositionStorage::isPosStorageEnabledFor(sid) == false )
		return;
		
	if ( storage == NULL )
		return;
	
	if ( storage->addPos(sid, px, py, pz) == false )
		;
}
////////////////////////////////////////////////////////////////////////
void CncContext::PositionStorage::addPos(uint8_t sid, double px, double py, double pz) {
////////////////////////////////////////////////////////////////////////
	if ( CncContext::PositionStorage::isPosStorageEnabledFor(sid) == false )
		return;
		
	if ( storage == NULL )
		return;
	
	if ( storage->addPos(sid, px, py, pz) == false )
		;
}
////////////////////////////////////////////////////////////////////////
void CncContext::PositionStorage::addMove(uint8_t sid, int32_t dx, int32_t dy, int32_t dz) {
////////////////////////////////////////////////////////////////////////
	if ( CncContext::PositionStorage::isPosStorageEnabledFor(sid) == false )
		return;
		
	if ( storage == NULL )
		return;
	
	if ( storage->addMove(sid, dx, dy, dz) == false )
		;
}
////////////////////////////////////////////////////////////////////////
void CncContext::PositionStorage::addMove(uint8_t sid, double dx, double dy, double dz) {
////////////////////////////////////////////////////////////////////////
	if ( CncContext::PositionStorage::isPosStorageEnabledFor(sid) == false )
		return;
		
	if ( storage == NULL )
		return;
	
	if ( storage->addMove(sid, dx, dy, dz) == false )
		;
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
