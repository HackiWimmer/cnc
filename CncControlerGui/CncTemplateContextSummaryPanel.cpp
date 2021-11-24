#include <iostream>
#include "GlobalFunctions.h"
#include "CncContext.h"
#include "CncConfig.h"
#include "CncAutoFreezer.h"
#include "CncFileNameService.h"
#include "CncLoggerListCtrl.h"
#include "CncTemplateContext.h"
#include "CncTemplateContextSummaryPanel.h"

///////////////////////////////////////////////////////////////////
CncTemplateContextSummaryPanel::CncTemplateContextSummaryPanel(wxWindow* parent)
: CncTemplateContextSummaryPanelBase	(parent)
, loggerRegister						()
, summary								(NULL)
, list									(NULL)
, analysisOverall						(NULL)
, analysisLimit							(NULL)
, analysisMovement						(NULL)
, dryRunLogger							(NULL)
, parsingSynopsis						(NULL)
///////////////////////////////////////////////////////////////////
{
	summary = new CncExtLoggerListCtrl(this, wxLC_SINGLE_SEL);
	GblFunc::replaceControl(m_summaryPlaceholder, summary);
	loggerRegister.push_back(summary);
	
	list = new CncExtLoggerListCtrl(this, wxLC_SINGLE_SEL);
	GblFunc::replaceControl(m_listPlaceholder, list);
	loggerRegister.push_back(list);
	
	analysisOverall = new CncExtLoggerListCtrl(this, wxLC_SINGLE_SEL);
	GblFunc::replaceControl(m_analysisPlaceholder, analysisOverall);
	loggerRegister.push_back(analysisOverall);
	
	analysisLimit = new CncExtLoggerListCtrl(this, wxLC_SINGLE_SEL);
	GblFunc::replaceControl(m_analysisLimitPlaceholder, analysisLimit);
	loggerRegister.push_back(analysisLimit);
	
	analysisMovement = new CncExtLoggerListCtrl(this, wxLC_SINGLE_SEL);
	GblFunc::replaceControl(m_analysisMovementPlaceholder, analysisMovement);
	loggerRegister.push_back(analysisMovement);
	
	dryRunLogger = new CncExtLoggerListCtrl(this, wxLC_SINGLE_SEL);
	GblFunc::replaceControl(m_dryRunLoggerPlaceholder, dryRunLogger);
	loggerRegister.push_back(dryRunLogger);
	
	parsingSynopsis = new CncExtLoggerListCtrl(this, wxLC_SINGLE_SEL);
	GblFunc::replaceControl(m_parsingSynopsisPlaceholder, parsingSynopsis);
	loggerRegister.push_back(parsingSynopsis);
	
}
///////////////////////////////////////////////////////////////////
CncTemplateContextSummaryPanel::~CncTemplateContextSummaryPanel() {
///////////////////////////////////////////////////////////////////
	loggerRegister.clear();
	
	wxDELETE(summary);
	wxDELETE(list);
	wxDELETE(analysisOverall);
	wxDELETE(analysisLimit);
	wxDELETE(analysisMovement);
	wxDELETE(dryRunLogger);
	wxDELETE(parsingSynopsis);
}
///////////////////////////////////////////////////////////////////
void CncTemplateContextSummaryPanel::selectPage(CncExtLoggerListCtrl* page) const {
///////////////////////////////////////////////////////////////////
	if ( page != NULL )
	{
		const int sel = m_loggerBook->FindPage(page->GetParent());
		
		if ( sel != wxNOT_FOUND )
			m_loggerBook->SetSelection(sel);
	}
}
///////////////////////////////////////////////////////////////////
void CncTemplateContextSummaryPanel::update() {
///////////////////////////////////////////////////////////////////
	if ( THE_CONTEXT->templateContext->analized() == false )
	{
		for ( auto it = loggerRegister.begin(); it != loggerRegister.end(); ++it )
		{
			CncExtLoggerListCtrl* logger = *it;
			logger->clearAll();
		}
		
		summary->add("Nothing available . . . ");
		
		return;
	}
	
	// -------------------------------------------------------------
	// update main lists
	ContextInterface::Result result;
	THE_CONTEXT->templateContext->analizeContextEntries(result);
	const bool ok = THE_CONTEXT->templateContext->isValid();
	
	std::stringstream ss;
	
	// summary
	ss.str("");
	summary->clearAll();
	THE_CONTEXT->templateContext->traceTo(ss, 0);
	
	if ( ok == false )	summary->addErrorEntry(ss.str().c_str());
	else				summary->addInfoEntry(ss.str().c_str());
	
	// overall analysis
	ss.str("");
	analysisOverall->clearAll();
	ss << result;
	if ( ok == false )	analysisOverall->addErrorEntry(ss.str().c_str());
	else				analysisOverall->addInfoEntry(ss.str().c_str());
	
	
	
	
	// list all entries
	ss.str("");
	list->clearAll();
	THE_CONTEXT->templateContext->traceContextEntriesTo(ss);
	list->add(ss.str().c_str());
	
	
	
	
}
///////////////////////////////////////////////////////////////////
void CncTemplateContextSummaryPanel::onCopyCurrentList(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	for ( auto it = loggerRegister.begin(); it != loggerRegister.end(); ++it )
	{
		CncExtLoggerListCtrl* logger = *it;
		if ( logger->IsShownOnScreen() )
			logger->copyToClipboard(true);
	}
}
///////////////////////////////////////////////////////////////////
void CncTemplateContextSummaryPanel::onSaveCurrentList(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	for ( auto it = loggerRegister.begin(); it != loggerRegister.end(); ++it )
	{
		CncExtLoggerListCtrl* logger = *it;
		if ( logger->IsShownOnScreen() )
		{
			const wxString& fileName(wxString::Format(	"%s-%02u.%s%s",
														CncFileNameService::getTempDirSession(), 
														std::distance(loggerRegister.begin(), it), 
														wxDateTime::Now().Format("%Y-%m-%d.%H-%M-%S"), 
														".txt")
			);
			const wxFileName fn(fileName);
			
			if ( logger->writeToFile(fn, true) )
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
	}
}
