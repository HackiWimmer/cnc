#include <iostream>
#include <fstream>
#include <wx/imaglist.h>
#include <wx/clipbrd.h>
#include <wx/tokenzr.h>
#include "wxCrafterImages.h"
#include "OSD/CncAsyncKeyboardState.h"
#include "CncCommon.h"
#include "CncContext.h"
#include "CncConfig.h"
#include "MainFrame.h"
#include "GlobalFunctions.h"
#include "CncFileNameService.h"
#include "CncMessageDialog.h"
#include "CncLoggerView.h"
#include "CncLoggerListCtrl.h"

wxListItemAttr	CncLoggerListCtrl::defaultItemAttr = wxListItemAttr();

/////////////////////////////////////////////////////////////
CncLoggerListCtrl::LoggerEntry::LoggerEntry() 
: text			()
, result		()
, listItemAttr	(defaultItemAttr)
/////////////////////////////////////////////////////////////
{}

/////////////////////////////////////////////////////////////
CncLoggerListCtrl::LoggerEntry::LoggerEntry(const wxString& t, const wxString& r, const wxListItemAttr& a)
: text			(t)
, result		(r)
, listItemAttr	(a)
/////////////////////////////////////////////////////////////
{}

// ----------------------------------------------------------------------------
// CncLoggerListCtrl Event Table
// ----------------------------------------------------------------------------
wxDEFINE_EVENT(wxEVT_LOGGER_DISPLAY_TIMER,  wxTimerEvent);

wxBEGIN_EVENT_TABLE(CncLoggerListCtrl, CncLargeScaledListCtrl)
	EVT_PAINT				(								CncLoggerListCtrl::onPaint				)
	EVT_SIZE				(								CncLoggerListCtrl::onSize				)
	EVT_TIMER				(wxEVT_LOGGER_DISPLAY_TIMER,	CncLoggerListCtrl::onDisplayTimer		)
	
	EVT_KEY_DOWN			(								CncLoggerListCtrl::onKeyDown			)
	EVT_RIGHT_DOWN			(								CncLoggerListCtrl::onRightDown			)

	EVT_LEFT_DCLICK			(								CncLoggerListCtrl::onLeftDClick			)
	
	EVT_LIST_ITEM_SELECTED	(wxID_ANY, 						CncLoggerListCtrl::onSelectListItem		)
	EVT_LIST_ITEM_ACTIVATED	(wxID_ANY, 						CncLoggerListCtrl::onActivateListItem	)
	
	EVT_SCROLLWIN			(								CncLoggerListCtrl::onScroll				)

wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncLoggerListCtrl::CncLoggerListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl	(parent, style | wxLC_NO_HEADER)
, entries					() 
, updateMode				(UM_Normal)
, updateModePreviously		(UM_Normal)
, canScroll					(true)
, sizeChanged				(false)
, joinTheApp				(false)
, showOnDemand				(false)
, anyUpdate					(false)
, selectedItem				(wxNOT_FOUND)
, loggedRowNumber			(wxNOT_FOUND)
, displayTimer				(this, wxEVT_LOGGER_DISPLAY_TIMER)
, displayTimerInterval		(200)
/////////////////////////////////////////////////////////////
{
	entries.reserve(10 * 1000);
	next();
	
	// add columns
	AppendColumn("A",	wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("B",	wxLIST_FORMAT_LEFT, 	wxLIST_AUTOSIZE);
	AppendColumn("C",	wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	
	wxFont font(9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(32, 32, 32));
	SetTextColour(wxColour(255, 255, 255));
	
	defaultItemAttr.SetBackgroundColour(GetBackgroundColour());
	defaultItemAttr.SetFont(font);
	defaultItemAttr.SetTextColour(GetTextColour());
	
	processUpdateMode();
}
/////////////////////////////////////////////////////////////
CncLoggerListCtrl::~CncLoggerListCtrl() {
/////////////////////////////////////////////////////////////
	displayTimer.Stop();
	entries.clear();
}
/////////////////////////////////////////////////////////////
void CncLoggerListCtrl::enable(bool state) {
/////////////////////////////////////////////////////////////
	updateContent();
	
	canScroll = state;
	Refresh();
}
/////////////////////////////////////////////////////////////
void CncLoggerListCtrl::onScroll(wxScrollWinEvent& event) {
/////////////////////////////////////////////////////////////
	event.Skip();
}
/////////////////////////////////////////////////////////////
void CncLoggerListCtrl::popImmediatelyMode() {
/////////////////////////////////////////////////////////////
	updateModePreviously = updateMode;
	updateMode = UM_Immediately;
	processUpdateMode();
}
/////////////////////////////////////////////////////////////
void CncLoggerListCtrl::popProcessMode() {
/////////////////////////////////////////////////////////////
	updateModePreviously = updateMode;
	updateMode = UM_Slowly;
	processUpdateMode();
}
/////////////////////////////////////////////////////////////
void CncLoggerListCtrl::pushUpdateMode() {
/////////////////////////////////////////////////////////////
	updateModePreviously = updateMode;
	processUpdateMode();
}
/////////////////////////////////////////////////////////////
void CncLoggerListCtrl::processUpdateMode() {
/////////////////////////////////////////////////////////////
	switch ( updateMode )
	{
		case UM_Immediately:	displayTimer.Stop();
								break;
		case UM_Normal:			displayTimer.Start(displayTimerInterval);
								break;
		case UM_Slowly:			displayTimer.Start(displayTimerInterval * 5);
								break;
	}
}
/////////////////////////////////////////////////////////////
void CncLoggerListCtrl::clearAll() {
/////////////////////////////////////////////////////////////
	entries.clear();
	clear();
	
	SetItemCount(entries.size());
	Refresh();
}
/////////////////////////////////////////////////////////////
void CncLoggerListCtrl::next() {
/////////////////////////////////////////////////////////////
	entries.push_back(std::move(LoggerEntry()));
	updateContent();
}
/////////////////////////////////////////////////////////////
void CncLoggerListCtrl::changeTextAttr(const wxTextAttr& ta) {
/////////////////////////////////////////////////////////////
	if ( entries.size() == 0 )
		next();
		
	wxListItemAttr& lia = entries.back().listItemAttr;
	lia.SetTextColour(ta.GetTextColour());
	
	//updateContent();
}
/////////////////////////////////////////////////////////////
void CncLoggerListCtrl::changeResult(const wxString& text, long item) {
/////////////////////////////////////////////////////////////
	if ( entries.size() == 0 )
		return;
		
	if ( item < 0 )
		item = entries.size() - 1;
		
	LoggerEntry& le = entries.at(item);
	le.result.assign(text);
	updateContent();
}
/////////////////////////////////////////////////////////////
void CncLoggerListCtrl::logRowNumber(long rn) {
/////////////////////////////////////////////////////////////
	const long size = (long)entries.size();
	
	if 		( size == 0 )		{ loggedRowNumber = wxNOT_FOUND; }
	else if ( rn > size -1 )	{ loggedRowNumber = wxNOT_FOUND; }
	else if ( rn < 0 ) 			{ loggedRowNumber = entries.size() - 1; }
	else						{ loggedRowNumber = rn; }
}
/////////////////////////////////////////////////////////////
long CncLoggerListCtrl::getLoggedRowNumber() {
/////////////////////////////////////////////////////////////
	return loggedRowNumber;
}
/////////////////////////////////////////////////////////////
void CncLoggerListCtrl::add(const char c) {
/////////////////////////////////////////////////////////////
	if ( entries.size() == 0 )
		next();
	
	if ( c == '\n' ) {
		next();
	}
	else {
		wxString& text = entries.back().text;
		text.append(c);
		updateContent();
	}
}
/////////////////////////////////////////////////////////////
void CncLoggerListCtrl::add(const wxString& text) {
/////////////////////////////////////////////////////////////
	if ( entries.size() == 0 )
		next();
	
	const wxChar delimiter = '\n';
	wxStringTokenizer lines(text, wxString("\n"));
	while ( lines.HasMoreTokens() )
	{
		const wxString token = lines.GetNextToken();
		
		if ( token.Len() > 0 ) {
			wxString& text = entries.back().text;
			text.append(token);
		}
		
		if ( lines.GetLastDelimiter() == delimiter )
			next();
	}
	
	updateContent();
}
/////////////////////////////////////////////////////////////
void CncLoggerListCtrl::add(const wxString& text, const wxListItemAttr& lia) {
/////////////////////////////////////////////////////////////
	if ( text.IsEmpty() )
		return;
		
	if ( entries.size() == 0 )
		next();
	
	entries.push_back(std::move(LoggerEntry(text, "", lia)));
	updateContent();
}
/////////////////////////////////////////////////////////////
void CncLoggerListCtrl::tokenAndAdd(const wxString& text, const wxListItemAttr& lia) {
/////////////////////////////////////////////////////////////
	wxStringTokenizer lines(text, "\n");
	
	while ( lines.HasMoreTokens() ) {
		const wxString& token = lines.GetNextToken();
		entries.push_back(std::move(LoggerEntry(token, "", lia)));
		next();
	}
}
/////////////////////////////////////////////////////////////
bool CncLoggerListCtrl::isItemValid(long item) const {
/////////////////////////////////////////////////////////////
	return item >= 0 && item < (long)(entries.size());
}
/////////////////////////////////////////////////////////////
int CncLoggerListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	return -1;
}
/////////////////////////////////////////////////////////////
wxString CncLoggerListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return _("");
	
	const LoggerEntry& le = entries.at(item);
	
	switch ( column ) {
		case COL_LNR:	return wxString::Format("%06ld",	item + 1);
		case COL_TXT:	return wxString::Format("%s",		le.text);
		case COL_RET:	return wxString::Format("%s",		le.result);
	}
	
	return _("");
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncLoggerListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	if ( isItemValid(item) == true ) {
		return (wxListItemAttr*)(&entries.at(item).listItemAttr);
	}
	
	// this indicates to use the default style
	return NULL;//(wxListItemAttr*)(&defaultItemAttr);
}
/////////////////////////////////////////////////////////////
void CncLoggerListCtrl::updateContent() {
/////////////////////////////////////////////////////////////
	if ( joinTheApp == true )
	{
		const bool secure = THE_CONTEXT->secureModeInfo.isActive;
		
		if (   showOnDemand 		== true  
			&& IsShownOnScreen()	== false 
			&& IsShown()			== true
			&& secure				== false
		   ) 
		{
			if ( CNC_READY ) {
				THE_APP->showAuiPane("Logger");
				THE_APP->getLoggerView()->select(LoggerSelection::VAL::CNC);
			}
		}
	}
	
	if ( updateMode != UM_Immediately ) {
		anyUpdate = true;
		return;
	}
	
	SetItemCount(entries.size());
	
	if ( GetItemCount() > 0 && IsShownOnScreen() == true )
	{
		ensureVisible(GetItemCount() - 1);
		Refresh();
	}
}
//////////////////////////////////////////////////
void CncLoggerListCtrl::onDisplayTimer(wxTimerEvent& event) {
//////////////////////////////////////////////////
	if ( anyUpdate == true ) 
	{
		SetItemCount(entries.size());
		
		if ( GetItemCount() > 0 && IsShownOnScreen() == true )
		{
			ensureVisible(GetItemCount() - 1);
			Refresh();
		}
		anyUpdate = false;
	}
}
/////////////////////////////////////////////////////////////////////
void CncLoggerListCtrl::onSize(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////////////
	sizeChanged = true;
	event.Skip();
}
//////////////////////////////////////////////////
void CncLoggerListCtrl::onPaint(wxPaintEvent& event) {
//////////////////////////////////////////////////
	if ( sizeChanged == true )
	{
		updateColumnWidth(COL_STRECH);
		sizeChanged = false;
	}
	
	if ( canScroll == false )
		SetScrollbar(0, 0, 0, 0);
	
	event.Skip();
}
//////////////////////////////////////////////////
void CncLoggerListCtrl::onKeyDown(wxKeyEvent& event) {
//////////////////////////////////////////////////
	const bool ctlKey = CncAsyncKeyboardState::isControlPressed();
	const int c       = event.GetUnicodeKey();
	
	if ( c == 'C' && ctlKey == true ) {
		
		if ( selectedItem != wxNOT_FOUND ) {
			if ( wxTheClipboard->Open() ) {
				wxTheClipboard->SetData( new wxTextDataObject(entries.at(selectedItem).text) );
				wxTheClipboard->Close();
			}
		}
		else {
			copyToClipboard(false);
		}
	}
	
	event.Skip(true);
}

//////////////////////////////////////////////////////////////
void CncLoggerListCtrl::onLeftDClick(wxMouseEvent& event) {
//////////////////////////////////////////////////////////////
	if ( selectedItem == wxNOT_FOUND )
		return;
		
	if ( joinTheApp == false )
		return;
	
	const bool ctlKey = CncAsyncKeyboardState::isControlPressed();
	
	if ( ctlKey == false )
		return;
		
	long lineNumber = -1;
	
	const wxString rawLine(entries.at(selectedItem).text);
	wxString line(rawLine);
	line.Trim(false);
	line.MakeUpper();
	
	// ---------------------------------------------------
	if ( line.StartsWith('[') == true ) {
		line.assign(line.BeforeFirst(']'));
		line.assign(line.AfterFirst('['));
	} 
	// ---------------------------------------------------
	else if ( line.Contains("LINE") == true ) {
		int p = line.Find("LINE");
		bool start = false;
		wxString ln;
		for (unsigned int i=p; i<line.length(); i++ ) {
			
			if ( start == false && isdigit((char)line[i]) != 0 )
				start = true;
			
			if ( start == true && isdigit((char)line[i]) == 0 )
				break;
				
			if ( start == true )
				ln.append(line[i]);
		}
		
		line.assign(ln);
		
	}
	// ---------------------------------------------------
	else if ( line.Contains("FILE:") == true ) {
		wxStringTokenizer words(rawLine, " \t");
		while ( words.HasMoreTokens() ) {
			const wxString token = words.GetNextToken();
			const wxFileName fn(token);
			
			if ( fn.Exists() ) {
				wxString tool;
				THE_CONFIG->getEditorTool(tool);
				GblFunc::executeExternalProgram(tool, token, true);
			}
		}
	}
	// ---------------------------------------------------
	else if ( line.Contains("PARSING SYNOPSIS TRACE") == true ) {
		THE_APP->selectParsingSynopsisTrace();
		return;
	}
	
	// try to select the evaluated line number - on demand
	if ( lineNumber > 0 && line.ToLong(&lineNumber) == true ) {
		if ( CNC_READY )
			THE_APP->selectSourceControlLineNumber(lineNumber - 1);
	}
}

//////////////////////////////////////////////////
void CncLoggerListCtrl::onRightDown(wxMouseEvent& event) {
//////////////////////////////////////////////////
	// currently nothing to do
}
//////////////////////////////////////////////////
void CncLoggerListCtrl::onSelectListItem(wxListEvent& event) {
//////////////////////////////////////////////////
	selectedItem = event.m_itemIndex;
	
	/*
		if ( isItemValid(item) == false )
		return _("");
	
	const LoggerEntry& le = entries.at(item);
	
	switch ( column ) {
		case COL_LNR:	return wxString::Format("%06ld",	item + 1);
		case COL_TXT:	return wxString::Format("%s",		le.text);
		case COL_RET:	return wxString::Format("%s",		le.result);
	}
	*/
}
//////////////////////////////////////////////////
void CncLoggerListCtrl::onActivateListItem(wxListEvent& event) {
//////////////////////////////////////////////////
	// currently nothing to do
}
//////////////////////////////////////////////////
bool CncLoggerListCtrl::writeToFile(const wxFileName& fn, bool allRows) {
//////////////////////////////////////////////////
	if ( fn.DirExists() == false ) {
		std::cerr <<  CNC_LOG_FUNCT_A(": Invalid file name: ") << fn.GetFullPath() << std::endl;
		return false;
	}
	
	std::ofstream out(fn.GetFullPath().c_str().AsChar(), std::ofstream::out);
	if ( !out.good() ) {
		std::cerr <<  CNC_LOG_FUNCT_A(": Can't create file: ") << fn.GetFullPath() << std::endl;
		return false;
	}
	
	if ( GetItemCount() > 0 )
	{
		const size_t firstRow = allRows ? 0                  : GetTopItem();
		const size_t lastRow  = allRows ? GetItemCount() - 1 : firstRow + std::min(GetCountPerPage(), GetItemCount() - 1);
		
		// check boundaries
		if ( firstRow >= entries.size() || lastRow >= entries.size() )
		{
			std::cerr <<  CNC_LOG_FUNCT_A(": Invalid boundaries:")						<< std::endl;
			std::cerr << " Item row count            : " << GetItemCount()				<< std::endl;
			std::cerr << " Available row count       : " << entries.size()				<< std::endl;
			std::cerr << " Evaluated first / last row: " << firstRow << ", " << lastRow	<< std::endl;
			
			out.close();
			return false;
		}
		
		auto beg = entries.begin() + firstRow;
		auto end = entries.begin() + lastRow + 1;
		
		for ( auto it = beg; it != end; ++it )
			out << wxString::Format("%s\n", it->text);
	}
	
	out.close();
	return true;
}
//////////////////////////////////////////////////
bool CncLoggerListCtrl::copyToClipboard(bool allRows) {
//////////////////////////////////////////////////
	bool ret = false;
	
	// Write the content of entries to the clipboard
	if ( wxTheClipboard->Open() )
	{
		if ( GetItemCount() > 0 )
		{
			const size_t firstRow = allRows ? 0                  : GetTopItem();
			const size_t lastRow  = allRows ? GetItemCount() - 1 : firstRow + std::min(GetCountPerPage(), GetItemCount() - 1 );
			
			// check boundaries
			if ( firstRow >= entries.size() || lastRow >= entries.size() )  {
				
				std::cerr << CNC_LOG_FUNCT_A(": Invalid boundaries:")						<< std::endl;
				std::cerr << " Available row count       : " << entries.size()				<< std::endl;
				std::cerr << " Evaluated first / last row: " << firstRow << ", " << lastRow	<< std::endl;
				
				wxTheClipboard->Close();
				return false;
			}
			
			auto beg = entries.begin() + firstRow;
			auto end = entries.begin() + lastRow;
			
			wxString content;
			for ( auto it = beg; it != end; ++it )
				content.append(wxString::Format("%s\n", it->text));
			
			// This data objects are held by the clipboard,
			// so do not delete them in the app.
			ret = wxTheClipboard->SetData( new wxTextDataObject(content) );
		}
		else 
		{
			ret = true;
		}
		wxTheClipboard->Close();
	}
	
	return ret;
}
//////////////////////////////////////////////////
bool CncLoggerListCtrl::openAsTextView(bool allRows) {
//////////////////////////////////////////////////
	const wxString& fileName(wxString::Format("%s%s", CncFileNameService::getTempDirSession(), "CncLoggerListCtrl.txt"));
	const wxFileName fn(fileName);
	
	if ( writeToFile(fn, allRows) == false )
		return false;
	
	// find the main window
	wxWindow* parent = this;
	while ( parent->GetParent() != NULL )
		parent = parent->GetParent();
	
	CncFileContentDialog dlg(parent, fileName, allRows ? "Complete Content" : "Visible Content", "Cnc Logger List View");
	dlg.SetSize(800, 900);
	dlg.CenterOnParent();
	dlg.setWordWrap(false);
	dlg.ShowModal();
	
	return true;
}

