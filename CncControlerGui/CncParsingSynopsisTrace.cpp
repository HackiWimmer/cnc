#include "GlobalFunctions.h"
#include "CncParsingSynopsisTrace.h"

//////////////////////////////////////////////////////////////////
CncParsingSynopsisTrace::CncParsingSynopsisTrace(wxWindow* parent)
: CncParsingSynopsisTraceBase(parent)
, synopsisTrace(NULL)
//////////////////////////////////////////////////////////////////
{
	// synopsis list
	synopsisTrace = new CncParsingSynopsisListCtrl(this, wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_parsingSynopsisListPlaceholder, synopsisTrace);
}
//////////////////////////////////////////////////////////////////
CncParsingSynopsisTrace::~CncParsingSynopsisTrace() {
//////////////////////////////////////////////////////////////////
	delete synopsisTrace;
}
//////////////////////////////////////////////////////////////////
void CncParsingSynopsisTrace::addEntry(const char type, const wxString& entry) { 
//////////////////////////////////////////////////////////////////
	switch ( type ) {
		case 'W':		synopsisTrace->addWarnEntry(entry); 
						break;
		case 'E':		synopsisTrace->addErrorEntry(entry); 
						break;
		case 'S':		synopsisTrace->addSeparator(entry);
						break;
		default:		synopsisTrace->addInfoEntry(entry); 
	}
}

