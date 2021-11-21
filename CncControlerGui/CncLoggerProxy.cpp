#include "CncCommon.h"
#include "CncConfig.h"
#include "MainFrame.h"
#include "CncTemplateContextSummaryPanel.h"
#include "CncLoggerListCtrl.h"
#include "CncLoggerView.h"
#include "CncLoggerProxy.h"

//////////////////////////////////////////////////////////////
CncStartupLoggerProxy::CncStartupLoggerProxy(wxWindow *parent, wxWindowID id, const wxString &value, const wxPoint &pos, const wxSize &size, 
												long style, const wxValidator &validator, const wxString &name)
: CncTextCtrl(parent, wxID_ANY, value, pos, size, style, validator, name)
//////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////
CncStartupLoggerProxy::~CncStartupLoggerProxy() {
//////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////
bool CncStartupLoggerProxy::SetDefaultStyle(const wxTextAttr& style) {
//////////////////////////////////////////////////////////////
	THE_APP->getLoggerView()->changeTextAttr(LoggerSelection::VAL::STARTUP, style);
	return true;
}
//////////////////////////////////////////////////////////////
void CncStartupLoggerProxy::AppendChar(char c) {
//////////////////////////////////////////////////////////////
	THE_APP->getLoggerView()->add(LoggerSelection::VAL::STARTUP, c);
}
//////////////////////////////////////////////////////////////
void CncStartupLoggerProxy::AppendText(const wxString &text) {
//////////////////////////////////////////////////////////////
	THE_APP->getLoggerView()->add(LoggerSelection::VAL::STARTUP, text);
}



//////////////////////////////////////////////////////////////
CncStandardLoggerProxy::CncStandardLoggerProxy(wxWindow *parent, wxWindowID id, const wxString &value, const wxPoint &pos, const wxSize &size, 
													long style, const wxValidator &validator, const wxString &name)
: CncTextCtrl(parent, wxID_ANY, value, pos, size, style, validator, name)
//////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////
CncStandardLoggerProxy::~CncStandardLoggerProxy() {
//////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////
bool CncStandardLoggerProxy::SetDefaultStyle(const wxTextAttr& style) {
//////////////////////////////////////////////////////////////
	THE_APP->getLoggerView()->changeTextAttr(LoggerSelection::VAL::CNC, style);
	return true;
}
//////////////////////////////////////////////////////////////
void CncStandardLoggerProxy::AppendChar(char c) {
//////////////////////////////////////////////////////////////
	THE_APP->getLoggerView()->add(LoggerSelection::VAL::CNC, c);
}
//////////////////////////////////////////////////////////////
void CncStandardLoggerProxy::AppendText(const wxString &text) {
//////////////////////////////////////////////////////////////
	THE_APP->getLoggerView()->add(LoggerSelection::VAL::CNC, text);
}


//////////////////////////////////////////////////////////////
CncTryRunLoggerProxy::CncTryRunLoggerProxy(wxWindow *parent, wxWindowID id, const wxString &value, const wxPoint &pos, const wxSize &size, 
												long style, const wxValidator &validator, const wxString &name)
: CncTextCtrl(parent, wxID_ANY, value, pos, size, style, validator, name)
//////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////
CncTryRunLoggerProxy::~CncTryRunLoggerProxy() {
//////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////
bool CncTryRunLoggerProxy::SetDefaultStyle(const wxTextAttr& style) {
//////////////////////////////////////////////////////////////
	THE_APP->getTemplateContextSummary()->getTryRunLogger()->changeTextAttr(style);
	return true;
}
//////////////////////////////////////////////////////////////
void CncTryRunLoggerProxy::AppendChar(char c) {
//////////////////////////////////////////////////////////////
	THE_APP->getTemplateContextSummary()->getTryRunLogger()->add(c);
}
//////////////////////////////////////////////////////////////
void CncTryRunLoggerProxy::AppendText(const wxString &text) {
//////////////////////////////////////////////////////////////
	THE_APP->getTemplateContextSummary()->getTryRunLogger()->add(text);
}


//////////////////////////////////////////////////////////////
CncParserSynopsisProxy::CncParserSynopsisProxy(wxWindow *parent, wxWindowID id, const wxString &value, const wxPoint &pos, const wxSize &size, 
												long style, const wxValidator &validator, const wxString &name)
: CncTextCtrl(parent, wxID_ANY, value, pos, size, style, validator, name)
//////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////
CncParserSynopsisProxy::~CncParserSynopsisProxy() {
//////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
bool CncParserSynopsisProxy::hasDebugEntries() const { 
//////////////////////////////////////////////////////////////////
	return THE_APP->getTemplateContextSummary()->getParsingSynopsis()->hasDebugEntries(); 
}
//////////////////////////////////////////////////////////////////
bool CncParserSynopsisProxy::hasWarnEntries() const {
//////////////////////////////////////////////////////////////////
	return THE_APP->getTemplateContextSummary()->getParsingSynopsis()->hasWarnEntries(); 
}
//////////////////////////////////////////////////////////////////
bool CncParserSynopsisProxy::hasErrorEntries() const { 
//////////////////////////////////////////////////////////////////
	return THE_APP->getTemplateContextSummary()->getParsingSynopsis()->hasErrorEntries(); 
}
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
		case 'E':	logger->addErrorEntry(entry);	break;
		case 'S':	logger->addSeparator(entry);	break;
		default:	logger->addInfoEntry(entry);
	}
}
//////////////////////////////////////////////////////////////
bool CncParserSynopsisProxy::SetDefaultStyle(const wxTextAttr& style) {
//////////////////////////////////////////////////////////////
	THE_APP->getTemplateContextSummary()->getParsingSynopsis()->changeTextAttr(style);
	return true;
}
//////////////////////////////////////////////////////////////
void CncParserSynopsisProxy::AppendChar(char c) {
//////////////////////////////////////////////////////////////
	THE_APP->getTemplateContextSummary()->getParsingSynopsis()->add(c);
}
//////////////////////////////////////////////////////////////
void CncParserSynopsisProxy::AppendText(const wxString &text) {
//////////////////////////////////////////////////////////////
	THE_APP->getTemplateContextSummary()->getParsingSynopsis()->add(text);
}


//////////////////////////////////////////////////////////////
CncMsgHistoryLoggerProxy::CncMsgHistoryLoggerProxy(wxWindow *parent, wxWindowID id, const wxString &value, const wxPoint &pos, const wxSize &size, 
													long style, const wxValidator &validator, const wxString &name)
: CncTextCtrl(parent, wxID_ANY, value, pos, size, style, validator, name)
//////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////
CncMsgHistoryLoggerProxy::~CncMsgHistoryLoggerProxy() {
//////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////
bool CncMsgHistoryLoggerProxy::SetDefaultStyle(const wxTextAttr& style) {
//////////////////////////////////////////////////////////////
	THE_APP->getCtrlMsgHistoryList()->changeTextAttr(style);
	return true;
}
//////////////////////////////////////////////////////////////
void CncMsgHistoryLoggerProxy::AppendChar(char c) {
//////////////////////////////////////////////////////////////
	THE_APP->getCtrlMsgHistoryList()->add(c);
}
//////////////////////////////////////////////////////////////
void CncMsgHistoryLoggerProxy::AppendText(const wxString &text) {
//////////////////////////////////////////////////////////////
	THE_APP->getCtrlMsgHistoryList()->add(text);
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

