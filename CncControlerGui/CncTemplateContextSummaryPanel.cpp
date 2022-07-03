#include <iostream>
#include "GlobalFunctions.h"
#include "CncCommon.h"
#include "CncContext.h"
#include "CncConfig.h"
#include "CncAutoFreezer.h"
#include "CncFileNameService.h"
#include "CncLoggerListCtrl.h"
#include "CncTemplateContext.h"
#include "CncTemplateContextSummaryPanel.h"

wxBEGIN_EVENT_TABLE(CncTemplateContextSummaryPanel, CncTemplateContextSummaryPanelBase)
	EVT_PAINT(				CncTemplateContextSummaryPanel::onPaint)
wxEND_EVENT_TABLE()

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
	
	const wxFont font = THE_CONTEXT->outboundListBookFont;
	m_loggerBook->GetListView()->SetFont(font);
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
void CncTemplateContextSummaryPanel::flagListItem(CncExtLoggerListCtrl* ctrl, bool flag) {
///////////////////////////////////////////////////////////////////
	if ( ctrl == NULL )
		return; 
		
	const int sel = m_loggerBook->FindPage(ctrl->GetParent());
	if ( sel < 0 || sel > m_loggerBook->GetListView()->GetItemCount() - 1 )
		return; 
		
	if ( flag == false )	m_loggerBook->GetListView()->SetItemTextColour(sel, *wxRED);
	else					m_loggerBook->GetListView()->SetItemTextColour(sel, *wxBLACK);
	
	const wxString ch("*");
	
	wxString text (m_loggerBook->GetListView()->GetItemText(sel));
	text.Replace(ch, "");
	
	if ( flag == false )
		text.Prepend(ch);
		
	m_loggerBook->GetListView()->SetItemText(sel, text);
}
///////////////////////////////////////////////////////////////////
void CncTemplateContextSummaryPanel::resetListFlagging() {
///////////////////////////////////////////////////////////////////
	for ( auto it = loggerRegister.begin(); it != loggerRegister.end(); ++it )
	{
		CncExtLoggerListCtrl* logger = *it;
		flagListItem(logger, true);
	}
}
///////////////////////////////////////////////////////////////////
void CncTemplateContextSummaryPanel::update(bool force) {
///////////////////////////////////////////////////////////////////
	if ( IsShownOnScreen() == false && force == false )
		return;

	const bool analized = THE_CONTEXT->templateContext->analized();
	if ( analized == false )
	{
		for ( auto it = loggerRegister.begin(); it != loggerRegister.end(); ++it )
		{
			CncExtLoggerListCtrl* logger = *it;
			logger->clearAll();
		}
		
		// only preview the summary in this case 
		// to have the boundaries already available 
		std::stringstream ss; 
		THE_CONTEXT->templateContext->traceTo(ss, 0);
		summary->addInfoEntry(ss.str().c_str());
		
		resetListFlagging();
		return;
	}
	
	// -------------------------------------------------------------
	// update main lists
	
	// first analyzing to get demand information available
	ContextInterface::ProcessResult result;
	THE_CONTEXT->templateContext->analizeContextEntries(result);
	const bool ok = THE_CONTEXT->templateContext->isValid();
	
	std::stringstream ss;
	
	// --------------------------------------------------------------
	// summary
	ss.str("");
	summary->clearAll();
	THE_CONTEXT->templateContext->traceTo(ss, 0);
	
	flagListItem(summary, ok);
	if ( ok == false )	summary->addErrorEntry(ss.str().c_str());
	else				summary->addInfoEntry(ss.str().c_str());
	
	// ---------------------------------------------------------
	// overall analysis
	ss.str("");
	analysisOverall->clearAll();
	ss << result;
	
	flagListItem(analysisOverall, ok);
	if ( ok == false )	analysisOverall->addErrorEntry(ss.str().c_str());
	else				analysisOverall->addInfoEntry(ss.str().c_str());
	
	// ---------------------------------------------------------
	// list all entries
	ss.str("");
	list->clearAll();
	THE_CONTEXT->templateContext->traceContextEntriesTo(ss);
	list->addInfoEntry(ss.str().c_str());
	
	// ---------------------------------------------------------
	// limit
	ss.str("");
	analysisLimit->clearAll();
	THE_CONTEXT->templateContext->filterAllLimitEntries(ss);
	const wxString analysisLimitStr(ss.str().c_str());
	
	if ( analysisLimitStr.length() > 0 )	analysisLimit->addErrorEntry(analysisLimitStr);
	else									analysisLimit->addInfoEntry(analysisLimitStr);
	
	flagListItem(analysisLimit, analysisLimitStr.IsEmpty());
	
	// ---------------------------------------------------------
	// movement
	ss.str("");
	analysisMovement->clearAll();
	THE_CONTEXT->templateContext->filterAllMovesWithoutSpindle(ss);
	const wxString analysisMovementStr(ss.str().c_str());
	
	if ( analysisMovementStr.length() > 0 )	analysisMovement->addErrorEntry(analysisMovementStr);
	else									analysisMovement->addInfoEntry(analysisMovementStr);
	
	flagListItem(analysisMovement, analysisMovementStr.IsEmpty());
}
///////////////////////////////////////////////////////////////////
void CncTemplateContextSummaryPanel::onPaint(wxPaintEvent& event) {
///////////////////////////////////////////////////////////////////
	const bool force = true;
	update(force);
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
