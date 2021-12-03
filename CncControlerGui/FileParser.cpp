
#include "CncSourceEditor.h"
#include <wx/menu.h>
#include <wx/evtloop.h>
#include <wx/dataview.h>
#include <wx/stattext.h>
#include <wx/propgrid/manager.h>
#include "OSD/CncTimeFunctions.h"
#include "OSD/webviewOSD.h"
#include "CncContext.h"
#include "MainFrame.h"
#include "CncConfig.h"
#include "CncCommon.h"
#include "wxCrafterImages.h"
#include "wxCrafterMainFrame.h"
#include "FileParser.h"

////////////////////////////////////////////////////////////////////////////
#define SHOULD_DEBUG_HERE													\
	if ( THE_CONTEXT->processingInfo->getCurrentDebugState() == false )		\
		return;

////////////////////////////////////////////////////////////////////////////
wxPropertyGridManager* FileParser::debuggerConfigurationPropertyGrid = NULL;
const char* CFG_DBG_AUTO_BRKP 	= "CFG_DBG_AUTO_BRKP";
const char* CFG_DBG_AREA_PREP	= "CFG_DBG_AREA_PREP";
const char* CFG_DBG_AREA_SPOO	= "CFG_DBG_AREA_SPOO";
const char* CFG_DBG_STOP_AFTE	= "CFG_DBG_STOP_AFTE";

typedef FileParser::DebugCtrl DEBUG_CTRL;
typedef FileParser::DebugCtrl::Config CONFIG;

////////////////////////////////////////////////////////////////////////////
wxPGProperty* CONFIG::getProperty(const wxString& name) {
////////////////////////////////////////////////////////////////////////////
	if ( debuggerConfigurationPropertyGrid == NULL )
		return NULL;
		
	wxPropertyGridPage* page = debuggerConfigurationPropertyGrid->GetPage(0);
	if ( page == NULL )
		 return NULL;
		 
	wxPGProperty* root = page->GetRoot();
	if ( root != NULL && root->Item(0) != NULL ) {
		wxPGProperty* config = root->Item(0);
		return config->GetPropertyByName(name);
		
	}
	
	return NULL;
}
////////////////////////////////////////////////////////////////////////////
bool CONFIG::autoBreakpoint()					{ return ( getProperty(CFG_DBG_AUTO_BRKP) != NULL ? getProperty(CFG_DBG_AUTO_BRKP)->GetValue().GetBool() : true ); }
bool CONFIG::shouldDebugPreprocessing()			{ return ( getProperty(CFG_DBG_AREA_PREP) != NULL ? getProperty(CFG_DBG_AREA_PREP)->GetValue().GetBool() : false ); }
bool CONFIG::shouldDebugSpooling()				{ return ( getProperty(CFG_DBG_AREA_SPOO) != NULL ? getProperty(CFG_DBG_AREA_SPOO)->GetValue().GetBool() : true ); }
bool CONFIG::shouldStopAfterPreprocessing()		{ return ( getProperty(CFG_DBG_STOP_AFTE) != NULL ? ( getProperty(CFG_DBG_STOP_AFTE)->GetValue().GetInteger() == 0 ) : false ); }
bool CONFIG::shouldStopAfterSpooling()			{ return ( getProperty(CFG_DBG_STOP_AFTE) != NULL ? ( getProperty(CFG_DBG_STOP_AFTE)->GetValue().GetInteger() == 1 ) : true ); }
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
void DEBUG_CTRL::currentNodeAdd(DcmRow& row) {
////////////////////////////////////////////////////////////////////////////
	if ( row.size() == 2 )
		currentNodeAdd(row.at(0).GetString(), row.at(1).GetString());
}
////////////////////////////////////////////////////////////////////////////
void DEBUG_CTRL::currentNodeAdd(DcmItemList& rows) {
////////////////////////////////////////////////////////////////////////////
	for ( DcmItemList::iterator it = rows.begin(); it != rows.end(); ++it ) {
		DcmRow row = *it;
		if ( row.size() == 2 )
			currentNodeAdd(row);
	}
}
////////////////////////////////////////////////////////////////////////////
void DEBUG_CTRL::currentNodeAdd(const wxString& key, const wxVariant& value) {
////////////////////////////////////////////////////////////////////////////
	if ( currentNode == NULL )
		return;
	
	wxString name(wxString::Format("[%06d]", propCount++));
	wxPGProperty* p = new wxStringProperty(key, name);
	p->SetValue(value);
	
	// find the last item before the firts category
	if ( currentNode->GetChildCount() > 0 && currentNode->Item(currentNode->GetChildCount() - 1)->IsCategory() ) {
		for ( unsigned int index = currentNode->GetChildCount() - 1; index >= 0; index-- ) {
			if ( currentNode->Item(index)->IsCategory() == false ) {
				// insert before the first category
				currentNode->InsertChild(index + 1, p);
				return;
			}
		}
	}
	
	// append at the end
	currentNode->AppendChild(p);
}
////////////////////////////////////////////////////////////////////////////
void DEBUG_CTRL::currentNodeAddToCategory(const wxString& catLabel, DcmRow& row) {
////////////////////////////////////////////////////////////////////////////
	if ( row.size() == 2 )
		currentNodeAddToCategory(catLabel, row.at(0).GetString(), row.at(1).GetString());
}
////////////////////////////////////////////////////////////////////////////
void DEBUG_CTRL::currentNodeAddToCategory(const wxString& catLabel, DcmItemList& rows) {
////////////////////////////////////////////////////////////////////////////
	for ( DcmItemList::iterator it = rows.begin(); it != rows.end(); ++it ) {
		DcmRow row = *it;
		if ( row.size() == 2 )
			currentNodeAddToCategory(catLabel, row);
	}
}
////////////////////////////////////////////////////////////////////////////
void DEBUG_CTRL::currentNodeAddToCategory(const wxString& catLabel, const wxString& key, const wxVariant& value) {
////////////////////////////////////////////////////////////////////////////
	if ( currentNode == NULL )
		return;
		
	if ( catLabel.IsEmpty() )
		return;
		
	wxPGProperty* parent = currentNode;
	if ( parent == NULL )
		return;
		
	wxString catName(wxString::Format("%s-%s", parent->GetName(), catLabel));
	wxPGProperty* cat = parent->GetPropertyByName(catName);
	if ( cat == NULL ) {
		cat = new wxPropertyCategory(wxString::Format("%s  ::%s", parent->GetLabel(), catLabel), catName);
		cat->SetExpanded(false);
		parent->AppendChild(cat);
	}
	
	cat->AppendChild(new wxStringProperty(key, wxString::Format("%d", propCount++), value));
}

////////////////////////////////////////////////////////////////////////////
FileParser::FileParser(const wxString& fn) 
: fileName				(fn)
, debugControls			()
, inboundSourceControl	(NULL)
, currentLineNumber		(UNDEFINED_LINE_NUMBER)
, toolIds				()
{
////////////////////////////////////////////////////////////////////////////
	displayToolId(-1);
}
////////////////////////////////////////////////////////////////////////////
FileParser::~FileParser() {
////////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
void FileParser::displayToolId(int id) {
//////////////////////////////////////////////////////////////////
	displayToolId(wxString::Format("%d", id));
}
//////////////////////////////////////////////////////////////////
void FileParser::displayToolId(const wxString& id) {
//////////////////////////////////////////////////////////////////
	wxTextCtrl* toolIdCtrl = CncConfig::getGlobalCncConfig()->getTheApp()->GetToolId();
	if ( toolIdCtrl == NULL )
		return;
		
	long toolId = 0;
	id.ToLong(&toolId);
	
	toolIdCtrl->SetValue(id);
	toolIdCtrl->SetToolTip(wxString::Format("Last Tool: %s", THE_CONFIG->getToolParamAsString(toolId)));
}
//////////////////////////////////////////////////////////////////
bool FileParser::setNextToolID(unsigned int id) {
//////////////////////////////////////////////////////////////////
	if ( shouldAToolChangeProcessed() == false )
		return true;
	
	int toolId = THE_CONFIG->translateToolId(id);
	
	// Check for tool change
	if ( toolIds.size() > 0 ) {
		if ( *toolIds.begin() != toolId ) {
			std::cerr << "A tool change isn't supported yet: Current id: " << toolIds[toolIds.size() - 1] 
			          << "; New id: " << toolId << " [" << id << "]" << std::endl;
			std::cerr << "The current tool stays unchanged! - this is may be an error!" << std::endl;
			return false;
		}
	}
	
	if ( THE_CONFIG->checkToolExists(toolId) == false ) {
		std::cerr << "Tool id didn't exists: " << toolId << " [" << id << "]" << std::endl;
		std::cerr << "The current tool stays unchanged! - this is may be an error!" << std::endl;
		displayToolId("-");
		return false;
	}
	
	THE_CONFIG->setCurrentToolId(toolId);
	displayToolId(toolId);
	toolIds.push_back(toolId);
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool FileParser::isAToolAvailable() {
//////////////////////////////////////////////////////////////////
	return ( toolIds.size() > 0 ); 
}
//////////////////////////////////////////////////////////////////
int FileParser::getCurrentToolId() {
//////////////////////////////////////////////////////////////////
	if ( isAToolAvailable() == false )
		return -9;
		
	return toolIds[toolIds.size() - 1];
}
//////////////////////////////////////////////////////////////////
void FileParser::installDebugConfigPage(wxPropertyGridManager* pgm) {
//////////////////////////////////////////////////////////////////
	wxASSERT( pgm != NULL );
	debuggerConfigurationPropertyGrid = pgm;
	
	// add config page
	wxPropertyGridPage* page;
	page = debuggerConfigurationPropertyGrid->AddPage(_("Configuration"),  ImageLibDebugger().Bitmap(_("BMP_DB_CONFIGURATION")));
	wxPGProperty* p = new wxPropertyCategory(_("Debugger Configuration"));
	p->SetExpanded(true);
	page->Append(p);
	
	// add config properties
	wxArrayString setupArr;
	wxArrayInt setupIntArr;
	setupArr.Add(_("Preprocessing"));
	setupArr.Add(_("Spooling"));
	
	p->AppendChild(new wxBoolProperty(_("Debug Area: Auto Breakpoint at Start of each Area"),	CFG_DBG_AUTO_BRKP, true));
	p->AppendChild(new wxBoolProperty(_("Debug Area: Preprocessing"), 							CFG_DBG_AREA_PREP, false));
	p->AppendChild(new wxBoolProperty(_("Debug Area: Spooling"), 								CFG_DBG_AREA_SPOO, true));
	p->AppendChild(new wxEnumProperty(_("Debug Area: Stop After"), 								CFG_DBG_STOP_AFTE, setupArr, setupIntArr, 1));
}
//////////////////////////////////////////////////////////////////
void FileParser::clearControls() {
//////////////////////////////////////////////////////////////////
	if ( debuggerConfigurationPropertyGrid != NULL ) {
		for ( int i = debuggerConfigurationPropertyGrid->GetPageCount() - 1; i >= staticPageOffset; i--)
			debuggerConfigurationPropertyGrid->RemovePage(i);

		debuggerConfigurationPropertyGrid->GetGrid()->ResetColours();
		debuggerConfigurationPropertyGrid->GetGrid()->SetCaptionBackgroundColour(wxColour(112,146,190));
		debuggerConfigurationPropertyGrid->GetGrid()->SetCaptionTextColour(wxColour(255,255,255));
		debuggerConfigurationPropertyGrid->GetGrid()->SetCellBackgroundColour(wxColour(255,255,255));
	}

	debugControls.currentPage 			= NULL;
	debugControls.currentMainCategory 	= NULL;
	debugControls.currentNode 			= NULL;
	debugControls.propCount 			= 0;
}
//////////////////////////////////////////////////////////////////
void FileParser::registerNextDebugNode(const wxString& nodeName) {
//////////////////////////////////////////////////////////////////
	if ( debugControls.currentMainCategory == NULL )
		return;
		
	SHOULD_DEBUG_HERE
		
	wxString label(wxString::Format("[%06d] - %s", currentLineNumber, nodeName));
	wxString name(wxString::Format("%d", debugControls.propCount++));
	
	debugControls.currentNode = new wxPropertyCategory(label, name);
	debugControls.currentNode->SetExpanded(true);
	
	debugControls.currentMainCategory->InsertChild(0, debugControls.currentNode);
	//debugControls.curentMainCategory->AppendChild(debugControls.currentNode);
	debuggerConfigurationPropertyGrid->Refresh();
}
//////////////////////////////////////////////////////////////////
void FileParser::appendDebugValueBase(DcmItemList& rows) {
//////////////////////////////////////////////////////////////////
	SHOULD_DEBUG_HERE
	debugControls.currentNodeAdd(rows);
}
//////////////////////////////////////////////////////////////////
void FileParser::appendDebugValueBase(DcmRow& row) {
//////////////////////////////////////////////////////////////////
	SHOULD_DEBUG_HERE
	debugControls.currentNodeAdd(row);
}
//////////////////////////////////////////////////////////////////
void FileParser::appendDebugValuePath(DcmItemList& rows) {
//////////////////////////////////////////////////////////////////
	SHOULD_DEBUG_HERE
	debugControls.currentNodeAddToCategory("Path", rows);
}
//////////////////////////////////////////////////////////////////
void FileParser::appendDebugValuePath(DcmRow& row) {
//////////////////////////////////////////////////////////////////
	SHOULD_DEBUG_HERE
	debugControls.currentNodeAddToCategory("Path", row);
}
//////////////////////////////////////////////////////////////////
void FileParser::appendDebugValueDetail(DcmItemList& rows) {
//////////////////////////////////////////////////////////////////
	SHOULD_DEBUG_HERE
	debugControls.currentNodeAddToCategory("Details", rows);
}
//////////////////////////////////////////////////////////////////
void FileParser::appendDebugValueDetail(DcmRow& row) {
//////////////////////////////////////////////////////////////////
	SHOULD_DEBUG_HERE
	debugControls.currentNodeAddToCategory("Details", row);
}
//////////////////////////////////////////////////////////////////
void FileParser::appendDebugValueBase(const char* key, wxVariant value) {
//////////////////////////////////////////////////////////////////
	SHOULD_DEBUG_HERE
	debugControls.currentNodeAdd(key, value);
}
//////////////////////////////////////////////////////////////////
void FileParser::appendDebugValuePath(const char* key, wxVariant value) {
//////////////////////////////////////////////////////////////////
	SHOULD_DEBUG_HERE
	debugControls.currentNodeAddToCategory("Path", key, value);
}
//////////////////////////////////////////////////////////////////
void FileParser::appendDebugValueDetail(const char* key, wxVariant value) {
//////////////////////////////////////////////////////////////////
	SHOULD_DEBUG_HERE
	debugControls.currentNodeAddToCategory("Details", key, value);
}
////////////////////////////////////////////////////////////////////////////
bool FileParser::processRelease() {
////////////////////////////////////////////////////////////////////////////
	THE_CONTEXT->processingInfo->releaseAllPhases();
	
	if ( debugControls.config.shouldStopAfterPreprocessing() )
		THE_CONTEXT->processingInfo->stopProcessingAfter(CncProcessingInfo::RP_Preprocesser);
		
	return process();
}
////////////////////////////////////////////////////////////////////////////
bool FileParser::processDebug() {
////////////////////////////////////////////////////////////////////////////
	THE_CONTEXT->processingInfo->debugAllPhases();
	THE_CONTEXT->processingInfo->setDebugMode(CncProcessingInfo::RP_Preprocesser, debugControls.config.shouldDebugPreprocessing());
	THE_CONTEXT->processingInfo->setDebugMode(CncProcessingInfo::RP_Spool, debugControls.config.shouldDebugSpooling());
	
	if ( debugControls.config.shouldStopAfterPreprocessing() )
		THE_CONTEXT->processingInfo->stopProcessingAfter(CncProcessingInfo::RP_Preprocesser);

	if ( debugControls.config.autoBreakpoint() )
		THE_CONTEXT->processingInfo->setStepByStepFlag(true);
	
	return process();
}
//////////////////////////////////////////////////////////////////
bool FileParser::process() {
//////////////////////////////////////////////////////////////////
	clearControls();
	toolIds.clear();
	
	// first: preprocessing
	initNextRunPhase(CncProcessingInfo::RP_Preprocesser);
	THE_CONTEXT->timestamps.logPreTimeStart();
	bool ret = preprocess();
	THE_CONTEXT->timestamps.logPreTimeEnd();
	
	// second: spooling
	if ( THE_CONTEXT->processingInfo->processMore() && ret == true )
	{
		initNextRunPhase(CncProcessingInfo::RP_Spool);
		
		THE_CONTEXT->timestamps.logSerialTimeStart();
		logMeasurementStart();

		ret = spool();

		logMeasurementEnd();
		THE_CONTEXT->timestamps.logSerialTimeEnd();
	} 
	
	if ( ret == true )
	{
		THE_CONTEXT->timestamps.logPostTimeStart();
		ret = postprocess();
		THE_CONTEXT->timestamps.logPostTimeEnd();
	}
	
	// at the end reset the selection
	if ( inboundSourceControl )
		inboundSourceControl->gotoBegin();
	
	if ( debuggerConfigurationPropertyGrid != NULL )
	{
		debuggerConfigurationPropertyGrid->Refresh();
		debuggerConfigurationPropertyGrid->Update();
	}
	
	initNextRunPhase(CncProcessingInfo::RP_Unknown);
	return ret;
}
////////////////////////////////////////////////////////////////////////////
void FileParser::setCurrentLineNumber(long ln) {
////////////////////////////////////////////////////////////////////////////
	currentLineNumber = ln;
	THE_CONTEXT->processingInfo->setLastLineNumber(ln);

	// to handle the pause flag
	evaluateProcessingState();
}
//////////////////////////////////////////////////////////////////
bool FileParser::hasLineABreakpoint(long ln) {
//////////////////////////////////////////////////////////////////
	if ( inboundSourceControl == NULL ) 
		return false;
	
	return (inboundSourceControl->MarginGetText(ln) == "B");
}
//////////////////////////////////////////////////////////////////
void FileParser::initNextRunPhase(CncProcessingInfo::RunPhase p) {
//////////////////////////////////////////////////////////////////
	THE_CONTEXT->processingInfo->setCurrentRunPhase(p);
	
	if ( debuggerConfigurationPropertyGrid != NULL && THE_CONTEXT->processingInfo->getCurrentDebugState() ) {
		switch ( p ) {
			case CncProcessingInfo::RP_Preprocesser:
				if ( debugControls.config.shouldDebugPreprocessing() == false )
					return;
					
				debugControls.currentPage = debuggerConfigurationPropertyGrid->AddPage(THE_CONTEXT->processingInfo->getCurrentDebugPhaseAsString(), 
				                                                                       ImageLibDebugger().Bitmap("BMP_DB_PREPROCESSING"));
				break;
				
			case CncProcessingInfo::RP_Spool:
				if ( debugControls.config.shouldDebugSpooling() == false )
					return;
					
				debugControls.currentPage = debuggerConfigurationPropertyGrid->AddPage(THE_CONTEXT->processingInfo->getCurrentDebugPhaseAsString(),
				                                                                       ImageLibDebugger().Bitmap("BMP_DB_SPOOLING"));
				break;
				
			default:
				return;
		}
		
		// select the new page
		debuggerConfigurationPropertyGrid->SelectPage(debuggerConfigurationPropertyGrid->GetPageCount() - 1);
		
		// create the main category for the new page
		wxString name(wxString::Format("%d", debugControls.propCount++));
		debugControls.currentMainCategory = new wxPropertyCategory(THE_CONTEXT->processingInfo->getCurrentDebugPhaseAsString(), name);
		debugControls.currentMainCategory->SetExpanded(true);
		debugControls.currentPage->Append(debugControls.currentMainCategory);
	}
}
////////////////////////////////////////////////////////////////////////////
void FileParser::debugNextBreakPoint() {
////////////////////////////////////////////////////////////////////////////
	if ( THE_CONTEXT->processingInfo->getCurrentDebugState() == true ) {
		THE_CONTEXT->processingInfo->setStepByStepFlag(false);
		THE_CONTEXT->processingInfo->resetLastLineNumber();
	}
}
////////////////////////////////////////////////////////////////////////////
void FileParser::debugNextStep() {
////////////////////////////////////////////////////////////////////////////
	if ( THE_CONTEXT->processingInfo->getCurrentDebugState() == true ) {
		THE_CONTEXT->processingInfo->setStepByStepFlag(true);
		THE_CONTEXT->processingInfo->resetLastLineNumber();
	}
}
////////////////////////////////////////////////////////////////////////////
void FileParser::debugStop() {
////////////////////////////////////////////////////////////////////////////
	THE_CONTEXT->processingInfo->setCurrentDebugState(false);
	THE_CONTEXT->processingInfo->setStopFlag(true);
	broadcastDebugState(false);
}
////////////////////////////////////////////////////////////////////////////
void FileParser::debugFinish() {
////////////////////////////////////////////////////////////////////////////
	THE_CONTEXT->processingInfo->setCurrentDebugState(false);
	broadcastDebugState(false);
}
////////////////////////////////////////////////////////////////////////////
void FileParser::selectSourceControl(unsigned long ln) {
////////////////////////////////////////////////////////////////////////////
	if ( inboundSourceControl == NULL ) 
		return;
	
	inboundSourceControl->selectLineNumber(ln);
}
//////////////////////////////////////////////////////////////////
bool FileParser::evaluateProcessingState() {
//////////////////////////////////////////////////////////////////
	return THE_APP->evaluateAndPerformProcessingState();
}
////////////////////////////////////////////////////////////////////////////
bool FileParser::evaluateDebugState(bool force) {
////////////////////////////////////////////////////////////////////////////
	// check abort condition
	if ( THE_APP->isInterrupted() == true )
		return false;
		
	// check abort condition
	if ( THE_CONTEXT->processingInfo->getStopFlag() == true )
		return false;
		
	// is debugging deactivated?
	if ( THE_CONTEXT->processingInfo->getCurrentDebugState() == false )
		return true;
		
	// ---------------------------------------------------------------------
	// debug handling - first update gui controls
		
	// select the current line number among hasLineABreakpoint()  
	// delivers a correct result
	selectSourceControl(currentLineNumber - 1);
	
	// ensure curent debug page is visible
	if ( debugControls.currentPage != NULL && debuggerConfigurationPropertyGrid->GetCurrentPage() != debugControls.currentPage )
		debuggerConfigurationPropertyGrid->SelectPage(debugControls.currentPage);
		
	// ensure curent debug node is visible
	wxPGProperty* p = debugControls.currentNode;
	if ( p != NULL ) {
		// if debugControls.currentNode has children select a little bit forward
		switch ( p->GetChildCount() ) {
			case 0: 	break; // nothing to do - debugControls.currentNode will be selected
			case 1:
			case 2:
			case 3:
			case 4: 	p = p->Item(p->GetChildCount() - 1); break;
			default: 	p = p->Item(4 - 1); break;
		}
		
		debuggerConfigurationPropertyGrid->SelectProperty(p, false);
		// select the note item as the global parent
		debuggerConfigurationPropertyGrid->SelectProperty(debugControls.currentNode, true);
	}

	// ---------------------------------------------------------------------
	// debug handling - wait for user events - on demand
	auto checkBreakpoint = [&]() 
	{
		if ( THE_CONTEXT->processingInfo->getStepByStepFlag() == true )
			return true;
			
		return hasLineABreakpoint(currentLineNumber - 1);
	};

	// loop while next debug step should be appear (user event)
	bool ret = true;
	while ( checkBreakpoint() == true )
	{
		THE_CONTEXT->processingInfo->setWaitingForUserEvents(true);
		THE_APP->dispatchAll();
		
		if ( THE_APP->isInterrupted() == true )
		{
			ret = false;
			break;
		}
		
		if ( THE_CONTEXT->processingInfo->getStopFlag() == true )
		{
			ret = false;
			break;
		}
		
		if ( THE_CONTEXT->processingInfo->getCurrentDebugState() == false )
			break;
			
		if ( THE_CONTEXT->processingInfo->isLastLineNumberDefined() == false )
			break;
			
		CncTimeFunctions::sleepMilliseconds(25);
	}
	
	THE_CONTEXT->processingInfo->setWaitingForUserEvents(false);
	return ret;
}


