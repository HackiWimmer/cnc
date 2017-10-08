#include <wx/webview.h>
#include <wx/stc/stc.h>
#include <wx/menu.h>
#include <wx/evtloop.h>
#include <wx/dataview.h>
#include <wx/stattext.h>
#include "CncCommon.h"
#include "FileParser.h"

////////////////////////////////////////////////////////////////////////////
FileParser::FileParser(const wxString& fn) 
: fileName(fn)
, runInfo()
, debugControls()
, inboundSourceControl(NULL)
, currentLineNumber(UNDEFINED_LINE_NUMBER)
{
////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////
FileParser::~FileParser() {
////////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
void FileParser::clearDebugControlBase() {
//////////////////////////////////////////////////////////////////
	if ( debugControls.debuggerControlBase == NULL )
		return;
	
	debugControls.debuggerControlBase->DeleteAllItems();
	debugControls.debuggerControlBase->Update();
	debugControls.debuggerControlBase->Refresh();
	clearDebugControlPath();
}
//////////////////////////////////////////////////////////////////
void FileParser::clearDebugControlPath() {
//////////////////////////////////////////////////////////////////
	if ( debugControls.debuggerControlPath == NULL )
		return;
	
	debugControls.debuggerControlPath->DeleteAllItems();
	debugControls.debuggerControlPath->Update();
	debugControls.debuggerControlPath->Refresh();
	clearDebugControlDetail();
}
//////////////////////////////////////////////////////////////////
void FileParser::clearDebugControlDetail() {
//////////////////////////////////////////////////////////////////
	if ( debugControls.debuggerControlDetail == NULL )
		return;

	debugControls.debuggerControlDetail->DeleteAllItems();
	debugControls.debuggerControlDetail->Update();
	debugControls.debuggerControlDetail->Refresh();
}
//////////////////////////////////////////////////////////////////
void FileParser::appendDebugValue(wxDataViewListCtrl* ctl, const char* key, wxVariant value) {
//////////////////////////////////////////////////////////////////
	if ( runInfo.getCurrentDebugState() == false )
		return;
		
	if ( checkIfCurrentRunPhaseShouldBeDebugged() == false )
		return;
		
	if ( ctl == NULL )
		return;
		
	wxVector<wxVariant> row;
	row.push_back(wxString(key));
	row.push_back(value.GetString());
	ctl->AppendItem(row);
	
	int itemCount = ctl->GetItemCount();
	ctl->EnsureVisible(ctl->RowToItem(itemCount - 1));
	ctl->EnsureVisible(ctl->RowToItem(0));
	ctl->Update();
	ctl->Refresh();
}
//////////////////////////////////////////////////////////////////
void FileParser::appendDebugValue(wxDataViewListCtrl* ctl, DcmItemList& rows) {
//////////////////////////////////////////////////////////////////
	if ( runInfo.getCurrentDebugState() == false )
		return;
		
	if ( checkIfCurrentRunPhaseShouldBeDebugged() == false )
		return;
		
	for ( DcmItemList::iterator it = rows.begin(); it != rows.end(); ++it ) {
		DcmRow row = *it;
		if ( row.size() == 2 ) {
			appendDebugValue(ctl, row.at(0).GetString(), row.at(1).GetString());
		}
	}
}
//////////////////////////////////////////////////////////////////
void FileParser::appendDebugValueBase( DcmItemList& rows) {
//////////////////////////////////////////////////////////////////
	appendDebugValue(debugControls.debuggerControlBase, rows);
}
//////////////////////////////////////////////////////////////////
void FileParser::appendDebugValuePath( DcmItemList& rows) {
//////////////////////////////////////////////////////////////////
	appendDebugValue(debugControls.debuggerControlPath, rows);
}
//////////////////////////////////////////////////////////////////
void FileParser::appendDebugValueDetail( DcmItemList& rows) {
//////////////////////////////////////////////////////////////////
	appendDebugValue(debugControls.debuggerControlDetail, rows);
}
//////////////////////////////////////////////////////////////////
void FileParser::appendDebugValueBase(const char* key, wxVariant value) {
//////////////////////////////////////////////////////////////////
	appendDebugValue(debugControls.debuggerControlBase, key, value);
}
//////////////////////////////////////////////////////////////////
void FileParser::appendDebugValuePath(const char* key, wxVariant value) {
//////////////////////////////////////////////////////////////////
	appendDebugValue(debugControls.debuggerControlPath, key, value);
}
//////////////////////////////////////////////////////////////////
void FileParser::appendDebugValueDetail(const char* key, wxVariant value) {
//////////////////////////////////////////////////////////////////
	appendDebugValue(debugControls.debuggerControlDetail, key, value);
}
////////////////////////////////////////////////////////////////////////////
bool FileParser::processRelease() {
////////////////////////////////////////////////////////////////////////////
	runInfo.releaseAllPhases();
	clearControls();
	
	return process();
}
////////////////////////////////////////////////////////////////////////////
bool FileParser::processDebug() {
////////////////////////////////////////////////////////////////////////////
	runInfo.debugAllPhases();
	runInfo.setStepByStepFlag(true);
	clearControls();
	
	return process();
}
//////////////////////////////////////////////////////////////////
bool FileParser::process() {
//////////////////////////////////////////////////////////////////
	initNextRunPhase(FileParserRunInfo::RP_Preprocesser);
	clearDebugControlBase();
	bool ret = preprocess();
	
	if ( runInfo.processMore() && ret == true ) {
		initNextRunPhase(FileParserRunInfo::RP_Spool);
		ret = spool();
	}
	
	if ( inboundSourceControl ) {
		inboundSourceControl->GotoPos(0);
		inboundSourceControl->SetSelection(0, 0);
	}
	
	initNextRunPhase(FileParserRunInfo::RP_Unknown);
	return ret;
}
////////////////////////////////////////////////////////////////////////////
void FileParser::setCurrentLineNumber(long ln) {
////////////////////////////////////////////////////////////////////////////
	currentLineNumber = ln;
	runInfo.setLastLineNumber(ln);

	if ( runInfo.getCurrentDebugState() == true )
		selectSourceControl(currentLineNumber - 1);
	
	// to handle the pause flag
	evaluateProcessingState();
}
//////////////////////////////////////////////////////////////////
bool FileParser::hasLineABreakpoint(long ln) {
//////////////////////////////////////////////////////////////////
	if ( inboundSourceControl == NULL ) 
		return false;

	// important! if this isn't done here checkBreakpoint() delivers a wrong result
	selectSourceControl(ln);
		
	return (inboundSourceControl->MarginGetText(ln) == "B");
}
//////////////////////////////////////////////////////////////////
bool FileParser::checkBreakpoint() {
//////////////////////////////////////////////////////////////////
	if ( runInfo.getStepByStepFlag() == true )
		return true;
		
	return hasLineABreakpoint(currentLineNumber - 1);
}
//////////////////////////////////////////////////////////////////
void FileParser::initNextRunPhase(FileParserRunInfo::RunPhase p) {
//////////////////////////////////////////////////////////////////
	runInfo.setCurrentRunPhase(p);
	
	if ( debugControls.debugPhase == NULL )
		return;
		
	debugControls.debugPhase->SetLabel(runInfo.getCurrentDebugPhaseAsString());
}
////////////////////////////////////////////////////////////////////////////
bool FileParser::togglePause() {
////////////////////////////////////////////////////////////////////////////
	runInfo.setPauseFlag(!runInfo.getPauseFlag());
	return runInfo.getPauseFlag();
}
////////////////////////////////////////////////////////////////////////////
void FileParser::debugNextBreakPoint() {
////////////////////////////////////////////////////////////////////////////
	if ( runInfo.getCurrentDebugState() == true ) {
		runInfo.setStepByStepFlag(false);
		runInfo.resetLastLineNumber();
	}
}
////////////////////////////////////////////////////////////////////////////
void FileParser::debugNextStep() {
////////////////////////////////////////////////////////////////////////////
	if ( runInfo.getCurrentDebugState() == true ) {
		runInfo.setStepByStepFlag(true);
		runInfo.resetLastLineNumber();
	}
}
////////////////////////////////////////////////////////////////////////////
void FileParser::debugStop() {
////////////////////////////////////////////////////////////////////////////
	runInfo.setCurrentDebugState(false);
	runInfo.setStopFlag(true);
	broadcastDebugState(false);
}
////////////////////////////////////////////////////////////////////////////
void FileParser::debugFinish() {
////////////////////////////////////////////////////////////////////////////
	runInfo.setCurrentDebugState(false);
	broadcastDebugState(false);
}
//////////////////////////////////////////////////////////////////
bool FileParser::checkIfCurrentRunPhaseShouldBeDebugged() {
//////////////////////////////////////////////////////////////////
	//evaluate if current processing phase should be debugged
	if ( runInfo.getCurrentRunPhase() == FileParserRunInfo::RP_Preprocesser && 
		 debugControls.debugPreprocessing != NULL && 
		 debugControls.debugPreprocessing->IsChecked() == false )
		return false;
		
	//evaluate if current processing phase should be debugged
	if ( runInfo.getCurrentRunPhase() == FileParserRunInfo::RP_Spool && 
		 debugControls.debugSpooling != NULL && 
		 debugControls.debugSpooling->IsChecked() == false )
		return false;
		
	return true;
}
//////////////////////////////////////////////////////////////////
bool FileParser::evaluateProcessingState() {
//////////////////////////////////////////////////////////////////
	if ( isInterrupted() == true )
		return false;
		
	if ( runInfo.getStopFlag() == true )
		return false;
		
	// necessary to update gui controls e. g. the source selection
	if ( runInfo.getCurrentDebugState() == true ) {
		while ( wxEventLoopBase::GetActive()->Pending() )
			wxEventLoopBase::GetActive()->Dispatch();
	}
	
	// pause handling
	while ( runInfo.getPauseFlag() == true ) {
		while ( wxEventLoopBase::GetActive()->Pending() )
			wxEventLoopBase::GetActive()->Dispatch();
			
		if ( isInterrupted() == true ) {
			runInfo.setPauseFlag(false);
			return false;
		}
			
		if ( runInfo.getStopFlag() == true ) {
			runInfo.setPauseFlag(false);
			return false;
		}
	}
	
	return true;
}
////////////////////////////////////////////////////////////////////////////
bool FileParser::evaluateDebugState(bool force) {
////////////////////////////////////////////////////////////////////////////
	if ( isInterrupted() == true )
		return false;
		
	if ( runInfo.getStopFlag() == true )
		return false;
		
	// debug handling
	if ( runInfo.getCurrentDebugState() == true ) {
		
		if ( force == false ) {
			if ( checkIfCurrentRunPhaseShouldBeDebugged() == false )
				return true;
		}

		//loop while next debug step should be appear (user event)
		while ( checkBreakpoint() == true ) {
			while ( wxEventLoopBase::GetActive()->Pending() )
				wxEventLoopBase::GetActive()->Dispatch();
		
			if ( isInterrupted() == true )
				return false;
				
			if ( runInfo.getStopFlag() == true )
				return false;
				
			if ( runInfo.getCurrentDebugState() == false )
				break;
				
			if ( runInfo.isLastLineNumberDefined() == false )
				break;
		}
	}
	
	return true;
}
////////////////////////////////////////////////////////////////////////////
void FileParser::selectSourceControl(unsigned long pos) {
////////////////////////////////////////////////////////////////////////////
	if ( inboundSourceControl == NULL ) 
		return;
	
	inboundSourceControl->GotoLine(pos);
	
	if ( pos == 0 ) {
		inboundSourceControl->SetSelectionStart(0);
		inboundSourceControl->SetSelectionEnd(0);
	} else {
		inboundSourceControl->SetSelectionStart(inboundSourceControl->GetCurrentPos());
		inboundSourceControl->SetSelectionEnd(inboundSourceControl->GetLineEndPosition(pos));
	}
}
////////////////////////////////////////////////////////////////////////////
void FileParser::setDebuggerControls(DebugControls& dc) { 
////////////////////////////////////////////////////////////////////////////
	debugControls = dc; 
	broadcastDebugControls(dc);
}

