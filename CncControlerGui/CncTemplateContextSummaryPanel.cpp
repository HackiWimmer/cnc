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
, analysis								(NULL)
///////////////////////////////////////////////////////////////////
{
	summary = new CncExtLoggerListCtrl(this, wxLC_SINGLE_SEL);
	GblFunc::replaceControl(m_summaryPlaceholder, summary);
	loggerRegister.push_back(summary);
	
	list = new CncExtLoggerListCtrl(this, wxLC_SINGLE_SEL);
	GblFunc::replaceControl(m_listPlaceholder, list);
	loggerRegister.push_back(list);
	
	analysis = new CncExtLoggerListCtrl(this, wxLC_SINGLE_SEL);
	GblFunc::replaceControl(m_analysisPlaceholder, analysis);
	loggerRegister.push_back(analysis);
	
	tryRunLogger = new CncExtLoggerListCtrl(this, wxLC_SINGLE_SEL);
	GblFunc::replaceControl(m_tryRunLoggerPlaceholder, tryRunLogger);
	loggerRegister.push_back(tryRunLogger);
	
}
///////////////////////////////////////////////////////////////////
CncTemplateContextSummaryPanel::~CncTemplateContextSummaryPanel() {
///////////////////////////////////////////////////////////////////
	loggerRegister.clear();
	
	wxDELETE(summary);
	wxDELETE(list);
	wxDELETE(analysis);
	wxDELETE(tryRunLogger);
}
///////////////////////////////////////////////////////////////////
void CncTemplateContextSummaryPanel::selectTryRun() {
///////////////////////////////////////////////////////////////////
	const int sel = m_loggerBook->FindPage(tryRunLogger->GetParent());
	
	if ( sel != wxNOT_FOUND )
		m_loggerBook->SetSelection(sel);
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
	
	{
		summary->clearAll();
		
		std::stringstream ss;
		THE_CONTEXT->templateContext->traceTo(ss, 0);
		
		if ( ok == false )	summary->addErrorEntry(ss.str().c_str());
		else				summary->addInfoEntry(ss.str().c_str());
	}
	{
		list->clearAll();
		
		std::stringstream ss;
		THE_CONTEXT->templateContext->traceContextEntriesTo(ss);
		list->add(ss.str().c_str());
	}
	{
		analysis->clearAll();
		
		std::stringstream ss;
		ss << result;
		analysis->add(ss.str().c_str());
	}
	
	
	
	
	
	
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
