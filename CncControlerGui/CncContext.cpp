#include <wx/app.h>
#include "GlobalFunctions.h"
#include "MainFrame.h"
#include "CncAnchorInfo.h"
#include "CncGamepadFilter.h"
#include "CncTemplateContext.h"
#include "CncBoundarySpace.h"
#include "CncProcessingInfo.h"
#include "CncConfig.h"
#include "CncContext.h"

////////////////////////////////////////////////////////////////////////
CncContext::CncContext() 
: timestamps				()
, secureModeInfo			()
, versionInfoMap			()
, processingInfo			(new CncProcessingInfo())
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
	wxDELETE( processingInfo );
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
double CncContext::getCurrentToolDiameter() const { 
////////////////////////////////////////////////////////////////////////
	if ( THE_CONFIG )
		return THE_CONFIG->getToolDiameter(currentToolId);
	
	return 0.0; 
}
////////////////////////////////////////////////////////////////////////
void CncContext::setProbeMode(bool state) { 
////////////////////////////////////////////////////////////////////////
	probeMode = state;
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
////////////////////////////////////////////////////////////////////////
bool CncContext::isPause() const { 
////////////////////////////////////////////////////////////////////////
	return processingInfo->isPause(); 
}
////////////////////////////////////////////////////////////////////////
bool CncContext::isProcessing() const { 
////////////////////////////////////////////////////////////////////////
	return processingInfo->isProcessing(); 
}
////////////////////////////////////////////////////////////////////////
bool CncContext::togglePause() { 
////////////////////////////////////////////////////////////////////////
	return processingInfo->togglePause(); 
}
////////////////////////////////////////////////////////////////////////
void CncContext::initPreparationPhase() {
////////////////////////////////////////////////////////////////////////
	processingInfo->initPrepationPhase();
}
////////////////////////////////////////////////////////////////////////
void CncContext::resetProcessing() {
////////////////////////////////////////////////////////////////////////
	processingInfo->reset();
}
////////////////////////////////////////////////////////////////////////
void CncContext::setCurrentToolId(int id) { 
////////////////////////////////////////////////////////////////////////
	currentToolId = id; 
	THE_TPL_CTX->addToolIdToSelList(id);
	
	wxTextCtrl* toolIdCtrl = THE_APP->GetToolId();
	if ( toolIdCtrl != NULL )
	{
		toolIdCtrl->SetValue(wxString::Format("%d", currentToolId));
		toolIdCtrl->SetToolTip(wxString::Format("Current Tool: %s", THE_CONFIG->getToolParamAsInfoStr(currentToolId)));
	}
}
////////////////////////////////////////////////////////////////////////
void CncContext::addParsingSynopisSeparator	(const wxString& m) const { THE_APP->getParserSynopsisProxy()->addSeparator(m); }
void CncContext::addParsingSynopisInfo		(const wxString& m) const { THE_APP->getParserSynopsisProxy()->addInfo(m);      }
void CncContext::addParsingSynopisWarning	(const wxString& m) const { THE_APP->getParserSynopsisProxy()->addWarning(m);   }
void CncContext::addParsingSynopisDebug		(const wxString& m) const { THE_APP->getParserSynopsisProxy()->addDebug(m);     }
void CncContext::addParsingSynopisError		(const wxString& m) const { THE_APP->getParserSynopsisProxy()->addError(m);     }

bool CncContext::parsingSynopsisHasInfoEntries()    const { return THE_APP->getParserSynopsisProxy()->hasInfoEntries();    }
bool CncContext::parsingSynopsisHasNonInfoEntries() const { return THE_APP->getParserSynopsisProxy()->hasNonInfoEntries(); }
bool CncContext::parsingSynopsisHasDebugEntries()   const { return THE_APP->getParserSynopsisProxy()->hasDebugEntries();   }
bool CncContext::parsingSynopsisHasWarningEntries() const { return THE_APP->getParserSynopsisProxy()->hasWarnEntries();    }
bool CncContext::parsingSynopsisHasErrorEntries()   const { return THE_APP->getParserSynopsisProxy()->hasErrorEntries();   }
////////////////////////////////////////////////////////////////////////
