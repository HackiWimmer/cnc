#include "GlobalFunctions.h"
#include "CncLoggerListCtrl.h"
#include "CncFileNameService.h"
#include "CncTraceCtrl.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "CncProcessingInfo.h"
#include "CncLoggerView.h"

/////////////////////////////////////////////////////////////////////
CncLoggerView::CncLoggerView(wxWindow* parent)
: CncLoggerViewBase		(parent)
, currentLoggerIndex	(LoggerSelection::VAL::STARTUP)
, loggerLists			()
, traceInfoBar			(NULL)
, traceCtrl				(NULL)
/////////////////////////////////////////////////////////////////////{
{
	setErrorFlag(LoggerSelection::VAL::STARTUP,	false);
	setErrorFlag(LoggerSelection::VAL::CNC, 	false);
	
	select(LoggerSelection::VAL::CNC);
}
/////////////////////////////////////////////////////////////////////
CncLoggerView::~CncLoggerView() {
/////////////////////////////////////////////////////////////////////
	// save the current log information
	const bool doOpenWithEditor = false; // true did't work any more at this program state
	const bool doJoinApp        = false; // true will crash at this program state
	
	const wxString msg(wxString::Format("**********************************************************\n%s: Logger closed at: %s\n**********************************************************\n", 
						CNC_LOG_FUNCT, 
						wxDateTime::Now().FormatISOTime())
	);
	
	loggerLists.at(LoggerSelection::VAL::STARTUP)->setJoinTheAppState(doJoinApp);
	popImmediatelyMode(LoggerSelection::VAL::STARTUP);
	add(LoggerSelection::VAL::STARTUP, msg);
	saveAll(LoggerSelection::VAL::STARTUP, doOpenWithEditor);
	APPEND_LOCATION_TO_STACK_TRACE_FILE_A("after finalize startup logger . . .")

	loggerLists.at(LoggerSelection::VAL::CNC)->setJoinTheAppState(doJoinApp);
	popImmediatelyMode(LoggerSelection::VAL::CNC);
	add(LoggerSelection::VAL::CNC, msg);
	saveAll(LoggerSelection::VAL::CNC, doOpenWithEditor);
	APPEND_LOCATION_TO_STACK_TRACE_FILE_A("after finalize runtime logger . . .")
	
	loggerLists.clear();
	
	wxDELETE( traceCtrl );
	wxDELETE( traceInfoBar );
}
/////////////////////////////////////////////////////////////////
void CncLoggerView::setSecureMode(bool state) {
/////////////////////////////////////////////////////////////////
	if ( state == true ) 
	{
		m_btLoggerOnDemand->Hide();
		setShowOnDemandState(false);
	}
	else 
	{
		m_btLoggerOnDemand->Show();
		setShowOnDemandState(doShowLoggerOnCommand());
	}
	Layout();
}
/////////////////////////////////////////////////////////////////////
const char* CncLoggerView::getLoggerIDAsStr(LoggerSelection::VAL id) {
/////////////////////////////////////////////////////////////////////
	// the return value has to be file name compatible 
	switch ( id ) 
	{
		case LoggerSelection::VAL::STARTUP:	return "CNC-Startup";
		case LoggerSelection::VAL::CNC:		return "CNC-Runtime";
	}
	return "CNC-UnkownLogger";
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::select(LoggerSelection::VAL id) {
/////////////////////////////////////////////////////////////////////
	m_btCncStartupLogger->SetValue(false);
	m_btCncStandardLogger->SetValue(false);
	
	switch ( id ) {
		case LoggerSelection::VAL::STARTUP:	currentLoggerIndex = LoggerSelection::VAL::STARTUP;
											m_btCncStartupLogger->SetValue(true);
											break;
											
		case LoggerSelection::VAL::CNC:		currentLoggerIndex = LoggerSelection::VAL::CNC;
											m_btCncStandardLogger->SetValue(true);
											break;
	}
	
	m_loggerBook->SetSelection(currentLoggerIndex);
}
/////////////////////////////////////////////////////////////////////
bool CncLoggerView::Enable(bool state) {
/////////////////////////////////////////////////////////////////////
	enable(state);
	
	if ( state == true )
	{
		enableListCtrlsOnly(true);
	}
	else 
	{
		if ( THE_CONTEXT->processingInfo->isProcessing() )
			enableListCtrlsOnly(false);
	}
	
	return state;
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::enable(bool state) {
/////////////////////////////////////////////////////////////////////
	m_btClear->Enable(state);
	m_btCopy->Enable(state);
	m_btCopyAll->Enable(state);
	m_btSave->Enable(state);
	m_btSaveAll->Enable(state);
	m_btView->Enable(state);
	m_btViewAll->Enable(state);
	m_btLoggerOnDemand->Enable(state);
	m_btClearTraceHistory->Enable(state);
	m_btShowTraceHistory->Enable(state);
	
	// allow this explicit
	//m_btCncStartupLogger->Enable(state);
	//m_btCncStandardLogger->Enable(state);
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::enableListCtrlsOnly(bool state) {
/////////////////////////////////////////////////////////////////////
	for ( auto it = loggerLists.begin(); it != loggerLists.end(); ++it )
	{
		CncLoggerListCtrl* list = *it;
		list->enable(state);
	}
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::initialize() {
/////////////////////////////////////////////////////////////////////
	if ( traceCtrl == NULL ) {
		traceCtrl = new CncTraceCtrl(this);
		GblFunc::replaceControl(m_tracePlaceholder, traceCtrl);
	}
	
	// trace ctr is only a background control 
	// its content will be displayed with traceInfoBar
	traceCtrl->Show(false);
	
	traceInfoBar = new CncTraceInfoBar(this);
	GblFunc::replaceControl(m_traceViewInfobarPlaceholder, traceInfoBar);
	
	traceInfoBar->SetShowHideEffects(wxSHOW_EFFECT_ROLL_TO_TOP, wxSHOW_EFFECT_ROLL_TO_BOTTOM);
	traceInfoBar->SetEffectDuration(10);
	
	if ( loggerLists.size() == 0 )
	{
		CncLoggerListCtrl* startup = new CncLoggerListCtrl(this, wxLC_SINGLE_SEL); 
		GblFunc::replaceControl(m_startupLoggerPlaceholder, startup);
		loggerLists.push_back(startup);

		CncLoggerListCtrl* standard = new CncLoggerListCtrl(this, wxLC_SINGLE_SEL); 
		standard->setJoinTheAppState(true);
		standard->setShowOnDemand(m_btLoggerOnDemand->GetValue());
		GblFunc::replaceControl(m_standardLoggerPlaceholder, standard);
		loggerLists.push_back(standard);
	}
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::clearTrace() {
/////////////////////////////////////////////////////////////////////
	if ( traceCtrl )
		traceCtrl->clearTrace(traceInfoBar->getCurrentTimeStr());
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::changeTextAttr(const wxTextAttr& ta) {
/////////////////////////////////////////////////////////////////////
	if ( traceCtrl )
		traceCtrl->SetDefaultStyle(ta);
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::trace(const char c) {
/////////////////////////////////////////////////////////////////////
	if ( traceCtrl )
		traceCtrl->AppendChar(c);
	
	traceInfoBar->showMessage(cnc::trc.getCurrentDesignAsChar(), traceCtrl->GetValue());
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::trace(const wxString& text) {
/////////////////////////////////////////////////////////////////////
	if ( traceCtrl )
		traceCtrl->AppendText(text);
	
	traceInfoBar->showMessage(cnc::trc.getCurrentDesignAsChar(), traceCtrl->GetValue());
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::openTraceHistory() {
/////////////////////////////////////////////////////////////////////
	if ( traceCtrl )
		traceCtrl->openHistroyView();
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::onSelectStandardLogger(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	select(LoggerSelection::VAL::CNC);
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::onSelectStartupLogger(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	select( LoggerSelection::VAL::STARTUP);
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::onToggleShowOnDemand(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
		
	loggerLists.at(currentLoggerIndex)->setShowOnDemand(m_btLoggerOnDemand->GetValue());
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::onClearTraceHistory(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( traceCtrl )
		traceCtrl->clearTraceHistory();
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::onViewTraceHistory(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	openTraceHistory();
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::popImmediatelyMode(LoggerSelection::VAL id) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
		
	loggerLists.at(id)->popImmediatelyMode();
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::popProcessMode(LoggerSelection::VAL id) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
		
	loggerLists.at(id)->popProcessMode();
	select(id);
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::pushUpdateMode(LoggerSelection::VAL id) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
		
	loggerLists.at(id)->pushUpdateMode();
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::onClear(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
		
	loggerLists.at(currentLoggerIndex)->clearAll();
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::onCopy(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
		
	loggerLists.at(currentLoggerIndex)->copyToClipboard();
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::onCopyAll(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
	
	loggerLists.at(currentLoggerIndex)->copyToClipboard(true);
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::onSave(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;

	const wxString& fileName(wxString::Format("%s%s.%s%s",	CncFileNameService::getTempDirSession(), 
															getLoggerIDAsStr(currentLoggerIndex), 
															wxDateTime::Now().Format("%Y-%m-%d.%H-%M-%S"), 
															".txt")
											 );
	const wxFileName fn(fileName);
	
	if ( loggerLists.at(currentLoggerIndex)->writeToFile(fn) )
	{
		if ( fn.Exists() )
		{
			wxString tool;
			if ( CncConfig::getGlobalCncConfig() )
			{
				CncConfig::getGlobalCncConfig()->getEditorTool(tool);
				GblFunc::executeExternalProgram(tool, fileName, true);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::onSaveAll(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	saveAll(currentLoggerIndex, true);
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::saveAll(LoggerSelection::VAL id, bool doOpen) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
		
	const wxString& fileName(wxString::Format("%s%s.%s%s",	CncFileNameService::getTempDirSession(), 
															getLoggerIDAsStr(id), 
															wxDateTime::Now().Format("%Y-%m-%d.%H-%M-%S"), 
															".txt")
											 );
	const wxFileName fn(fileName);
	
	if ( loggerLists.at(id)->writeToFile(fn, true) )
	{
		if ( doOpen && fn.Exists() )
		{
			wxString tool;
			if ( CncConfig::getGlobalCncConfig() )
			{
				CncConfig::getGlobalCncConfig()->getEditorTool(tool);
				GblFunc::executeExternalProgram(tool, fileName, true);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::saveAll(bool doOpen) {
/////////////////////////////////////////////////////////////////////
	saveAll(currentLoggerIndex, doOpen);
}

/////////////////////////////////////////////////////////////////////
void CncLoggerView::onView(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
	
	loggerLists.at(currentLoggerIndex)->openAsTextView();
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::onViewAll(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
	
	loggerLists.at(currentLoggerIndex)->openAsTextView(true);
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::clear(LoggerSelection::VAL id) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
	
	loggerLists.at(id)->clearAll();
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::setErrorFlag(LoggerSelection::VAL id, bool flag) {
/////////////////////////////////////////////////////////////////////
	switch ( id ) {
		case LoggerSelection::VAL::STARTUP:
			m_btCncStartupLogger->SetBackgroundColour(flag ? *wxRED : *wxLIGHT_GREY);
			break;

		case LoggerSelection::VAL::CNC:
			m_btCncStandardLogger->SetBackgroundColour(flag ? *wxRED : *wxLIGHT_GREY);
			break;
	}
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::logCurrentPosition(LoggerSelection::VAL id) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
	
	loggerLists.at(id)->logRowNumber();
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::logLastFilledPosition(LoggerSelection::VAL id) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
	
	loggerLists.at(id)->logLastFilledRowNumber();
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::changeTextColour(LoggerSelection::VAL id, const wxColour& col) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
	
	loggerLists.at(id)->changeTextColour(col);
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::changeTextAttr(LoggerSelection::VAL id, const wxTextAttr& ta) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
	
	loggerLists.at(id)->changeTextAttr(ta);
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::changeResult(LoggerSelection::VAL id, const wxString& text, long int row) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
	
	loggerLists.at(id)->changeResult(text, row);
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::changeResultForLoggedPosition(LoggerSelection::VAL id, const wxString& result) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
	
	if ( loggerLists.at(id)->getLoggedRowNumber() > 0 )
		loggerLists.at(id)->changeResult(result, loggerLists.at(id)->getLoggedRowNumber());
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::changeResultForLastPosition(LoggerSelection::VAL id, const wxString& result) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
	
	loggerLists.at(id)->logRowNumber();
	loggerLists.at(id)->changeResult(result);
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::changeResultForLastFilledPosition(LoggerSelection::VAL id, const wxString& result) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
	
	loggerLists.at(id)->logLastFilledRowNumber();
	loggerLists.at(id)->changeResult(result, loggerLists.at(id)->getLoggedRowNumber());
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::add(LoggerSelection::VAL id, const char c) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
		
	loggerLists.at(id)->add(c);
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::add(LoggerSelection::VAL id, const wxString& text) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
		
	loggerLists.at(id)->add(text);
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::add(LoggerSelection::VAL id, const wxString& text, const wxString& result) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
		
	loggerLists.at(id)->add(text, result);
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::add(const char c) {
/////////////////////////////////////////////////////////////////////
	loggerLists.at(currentLoggerIndex)->add(c);
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::add(const wxString& text) {
/////////////////////////////////////////////////////////////////////
	loggerLists.at(currentLoggerIndex)->add(text);
}