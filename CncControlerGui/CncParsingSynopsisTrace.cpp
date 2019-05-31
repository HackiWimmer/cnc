#include "GlobalFunctions.h"
#include "CncParsingSynopsisTrace.h"

//////////////////////////////////////////////////////////////////
CncParsingSynopsisTrace::CncParsingSynopsisTrace(wxWindow* parent)
: CncParsingSynopsisTraceBase(parent)
, synopsisTrace(NULL)
//////////////////////////////////////////////////////////////////
{
	// synopisis list
	synopsisTrace = new CncParsingSynopsisListCtrl(this, wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_parsingSynopsisListPlaceholder, synopsisTrace);
}
//////////////////////////////////////////////////////////////////
CncParsingSynopsisTrace::~CncParsingSynopsisTrace() {
//////////////////////////////////////////////////////////////////
	delete synopsisTrace;
}

