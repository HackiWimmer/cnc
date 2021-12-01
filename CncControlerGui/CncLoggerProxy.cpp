#include "CncCommon.h"
#include "CncConfig.h"
#include "MainFrame.h"
#include "CncTemplateContextSummaryPanel.h"
#include "CncLoggerListCtrl.h"
#include "CncLoggerView.h"
#include "CncLoggerProxy.h"


//////////////////////////////////////////////////////////////
CncLoggerProxy::CncLoggerProxy( wxWindow *parent, wxWindowID id, const wxString &value, const wxPoint &pos, const wxSize &size, 
								long style, const wxValidator &validator, const wxString &name)
: CncTextCtrl		(parent, wxID_ANY, value, pos, size, style, validator, name)
, lastSourceID		(NO_SOUREC_ID)
//////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////
CncLoggerProxy::~CncLoggerProxy() {
//////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////
void CncLoggerProxy::appendChar(char c, const wxColour& col, int sourceId) {
//////////////////////////////////////////////////////////////
	if ( lastSourceID != sourceId )
	{
		setTextColour(col);
		lastSourceID = sourceId;
	}
	
	CncLoggerListCtrl* lc = getListCtrl();
	wxASSERT( lc )
	lc->add(c);
}
//////////////////////////////////////////////////////////////
void CncLoggerProxy::appendChar(char c, const wxTextAttr& style, int sourceId) {
//////////////////////////////////////////////////////////////
	if ( lastSourceID != sourceId )
	{
		SetDefaultStyle(style);
		lastSourceID = sourceId;
	}
	
	CncLoggerListCtrl* lc = getListCtrl();
	wxASSERT( lc )
	lc->add(c);
}
//////////////////////////////////////////////////////////////
bool CncLoggerProxy::setTextColour(const wxColour& col) {
//////////////////////////////////////////////////////////////
	CncLoggerListCtrl* lc = getListCtrl();
	wxASSERT( lc )
	lc->changeTextColour(col);
	
	return true;
}
//////////////////////////////////////////////////////////////
bool CncLoggerProxy::SetDefaultStyle(const wxTextAttr& style) {
//////////////////////////////////////////////////////////////
	CncLoggerListCtrl* lc = getListCtrl();
	wxASSERT( lc )
	lc->changeTextAttr(style);
	
	return true;
}
//////////////////////////////////////////////////////////////
void CncLoggerProxy::AppendChar(char c) {
//////////////////////////////////////////////////////////////
	CncLoggerListCtrl* lc = getListCtrl();
	wxASSERT( lc )
	lc->add(c);
}
//////////////////////////////////////////////////////////////
void CncLoggerProxy::AppendText(const wxString &text) {
//////////////////////////////////////////////////////////////
	CncLoggerListCtrl* lc = getListCtrl();
	wxASSERT( lc )
	lc->add(text);
}


//////////////////////////////////////////////////////////////
CncLoggerListCtrl* CncStartupLoggerProxy::getListCtrl()		const { return THE_APP->getLoggerView()->getLoggerListCtrl(LoggerSelection::VAL::STARTUP); }
CncLoggerListCtrl* CncStandardLoggerProxy::getListCtrl()	const { return THE_APP->getLoggerView()->getLoggerListCtrl(LoggerSelection::VAL::CNC); }
CncLoggerListCtrl* CncDryRunLoggerProxy::getListCtrl()		const { return THE_APP->getTemplateContextSummary()->getDryRunLogger(); }
CncLoggerListCtrl* CncParserSynopsisProxy::getListCtrl()	const { return THE_APP->getTemplateContextSummary()->getParsingSynopsis(); }
CncLoggerListCtrl* CncMsgHistoryLoggerProxy::getListCtrl()	const { return THE_APP->getCtrlMsgHistoryList(); }
//////////////////////////////////////////////////////////////

bool CncParserSynopsisProxy::hasInfoEntries()				const { return THE_APP->getTemplateContextSummary()->getParsingSynopsis()->hasInfoEntries();    }
bool CncParserSynopsisProxy::hasNonInfoEntries()			const { return THE_APP->getTemplateContextSummary()->getParsingSynopsis()->hasNonInfoEntries(); }
bool CncParserSynopsisProxy::hasDebugEntries()				const { return THE_APP->getTemplateContextSummary()->getParsingSynopsis()->hasDebugEntries();   }
bool CncParserSynopsisProxy::hasWarnEntries()				const { return THE_APP->getTemplateContextSummary()->getParsingSynopsis()->hasWarnEntries();    }
bool CncParserSynopsisProxy::hasErrorEntries()				const { return THE_APP->getTemplateContextSummary()->getParsingSynopsis()->hasErrorEntries();   }

//////////////////////////////////////////////////////////////////
void CncParserSynopsisProxy::popProcessMode() {
//////////////////////////////////////////////////////////////////
	THE_APP->getTemplateContextSummary()->getParsingSynopsis()->popProcessMode(); 
}
//////////////////////////////////////////////////////////////////
void CncParserSynopsisProxy::pushUpdateMode() {
//////////////////////////////////////////////////////////////////
	THE_APP->getTemplateContextSummary()->getParsingSynopsis()->pushUpdateMode(); 
}
//////////////////////////////////////////////////////////////////
void CncParserSynopsisProxy::addEntry(const char type, const wxString& entry) { 
//////////////////////////////////////////////////////////////////
	CncExtLoggerListCtrl* logger = THE_APP->getTemplateContextSummary()->getParsingSynopsis();
	switch ( type )
	{
		case 'W':	logger->addWarnEntry(entry);	break;
		case 'D':	logger->addDebugEntry(entry);	break;
		case 'E':	logger->addErrorEntry(entry);	break;
		case 'S':	logger->addSeparator(entry);	break;
		default:	logger->addInfoEntry(entry);
	}
}




/////////////////////////////////////////////////////////////
CncTraceProxy::CncTraceProxy(wxWindow *parent, wxWindowID id, const wxString &value, const wxPoint &pos, const wxSize &size, 
								long style, const wxValidator &validator, const wxString &name)
: CncTextCtrl(parent, wxID_ANY, value, pos, size, style | wxTE_RICH | wxTE_READONLY | wxTE_MULTILINE | wxTE_DONTWRAP, validator, name)
//////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////
CncTraceProxy::~CncTraceProxy() {
//////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////
void CncTraceProxy::Clear() {
//////////////////////////////////////////////////////////////
	THE_APP->getLoggerView()->clearTrace();
}
//////////////////////////////////////////////////////////////
void CncTraceProxy::appendChar(char c, const wxColour& col, int sourceId) {
//////////////////////////////////////////////////////////////
	if ( lastSourceID != sourceId )
	{
		setTextColour(col);
		lastSourceID = sourceId;
	}
	
	THE_APP->getLoggerView()->add(c);
}
//////////////////////////////////////////////////////////////
void CncTraceProxy::appendChar(char c, const wxTextAttr& style, int sourceId) {
//////////////////////////////////////////////////////////////
	if ( lastSourceID != sourceId )
	{
		SetDefaultStyle(style);
		lastSourceID = sourceId;
	}
	
	THE_APP->getLoggerView()->add(c);
}
//////////////////////////////////////////////////////////////
bool CncTraceProxy::setTextColour(const wxColour& col) {
//////////////////////////////////////////////////////////////
	THE_APP->getLoggerView()->changeTextAttr(col);
	return true;
}
//////////////////////////////////////////////////////////////
bool CncTraceProxy::SetDefaultStyle(const wxTextAttr& style) {
//////////////////////////////////////////////////////////////
	THE_APP->getLoggerView()->changeTextAttr(style);
	return true;
}
//////////////////////////////////////////////////////////////
void CncTraceProxy::AppendChar(char c) {
//////////////////////////////////////////////////////////////
	THE_APP->getLoggerView()->trace(c);
}
//////////////////////////////////////////////////////////////
void CncTraceProxy::AppendText(const wxString &text) {
//////////////////////////////////////////////////////////////
	THE_APP->getLoggerView()->trace(text);
}

