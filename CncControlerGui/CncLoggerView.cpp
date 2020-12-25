#include "GlobalFunctions.h"
#include "CncLoggerListCtrl.h"
#include "CncFileNameService.h"
#include "CncTraceCtrl.h"
#include "CncConfig.h"
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
	// this will document any content logged during the shutdown processing
	// without any change to read it in the gui.
	saveAll(false);
	
	loggerLists.clear();
	
	wxDELETE( traceCtrl );
	wxDELETE( traceInfoBar );
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
	
	if ( loggerLists.size() == 0 ) {
		
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
		
	loggerLists.at(currentLoggerIndex)->popImmediatelyMode();
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::popProcessMode(LoggerSelection::VAL id) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
		
	loggerLists.at(currentLoggerIndex)->popProcessMode();
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::pushUpdateMode(LoggerSelection::VAL id) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
		
	loggerLists.at(currentLoggerIndex)->pushUpdateMode();
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

	const wxString& fileName(wxString::Format("%s%s%s%s",	CncFileNameService::getTempDirSession(), 
															"CncLoggerContent", 
															wxDateTime::Now().Format("%Y-%m-%d.%H-%M-%S"), 
															".txt")
											 );
	const wxFileName fn(fileName);
	
	loggerLists.at(currentLoggerIndex)->writeToFile(fn);
	if ( fn.Exists() ) {
		 
		wxString tool;
		CncConfig::getGlobalCncConfig()->getEditorTool(tool);
		
		GblFunc::executeExternalProgram(tool, fileName, true);
	}
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::saveAll(bool doOpen) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
		
	const wxString& fileName(wxString::Format("%s%s%s%s",	CncFileNameService::getTempDirSession(), 
															"CncLoggerContent", 
															wxDateTime::Now().Format("%Y-%m-%d.%H-%M-%S"), 
															".txt")
											 );
	const wxFileName fn(fileName);
	
	loggerLists.at(currentLoggerIndex)->writeToFile(fn, true);
	
	if ( doOpen && fn.Exists() ) {
		wxString tool;
		CncConfig::getGlobalCncConfig()->getEditorTool(tool);
		GblFunc::executeExternalProgram(tool, fileName, true);
	}
}
/////////////////////////////////////////////////////////////////////
void CncLoggerView::onSaveAll(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	saveAll(true);
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
void CncLoggerView::changeResultForLoggedPosition(LoggerSelection::VAL id, const wxString& text) {
/////////////////////////////////////////////////////////////////////
	if ( loggerLists.size() != MaxLoggerCount )
		return;
	
	if ( loggerLists.at(id)->getLoggedRowNumber() > 0 )
		loggerLists.at(id)->changeResult(text);
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
