#include <iostream>
#include <wx/imaglist.h>
#include <wx/fileconf.h>
#include <wx/menu.h>
#include "OSD/CncAsyncKeyboardState.h"
#include "MainFrameProxy.h"
#include "GlobalFunctions.h"
#include "wxCrafterImages.h"
#include "CncFileView.h"
#include "CncFileViewLists.h"

#define CFVL_PRINT_LOCATION_CTX_FILE		//	CNC_PRINT_LOCATION
#define CFVL_PRINT_LOCATION_CTX_SOMETHING	//	CNC_PRINT_LOCATION

namespace TimerEvent {
	const int SelectInterval 	= 400;
	const int ActivateInterval 	=  10;
};

// ----------------------------------------------------------------------------
// CncLruFileViewListCtrl Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(CncLruFileViewListCtrl, CncLargeScaledListCtrl)
	EVT_SIZE				(			CncLruFileViewListCtrl::onSize				)
	EVT_KEY_DOWN			(			CncLruFileViewListCtrl::onKeyDown			)
	EVT_RIGHT_DOWN			(			CncLruFileViewListCtrl::onRightDown			)
	EVT_LEAVE_WINDOW		(			CncLruFileViewListCtrl::onLeaveWindow		)
	EVT_LIST_ITEM_SELECTED	(wxID_ANY, 	CncLruFileViewListCtrl::onSelectListItem	)
	EVT_LIST_ITEM_ACTIVATED	(wxID_ANY, 	CncLruFileViewListCtrl::onActivateListItem	)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncLruFileViewListCtrl::CncLruFileViewListCtrl(wxWindow *parent, unsigned int ms, long style)
: CncLargeScaledListCtrl(parent, style)
, lruList()
, eventTimer(new wxTimer())
, defaultItemAttr()
, selectedItemAttr()
, popupMenu(NULL)
, isLeaveEventActive(true)
, maxSize(ms)
, lastEventType(EventType::UNKNOWN)
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Last recently used:", wxLIST_FORMAT_LEFT, 250);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::REVERSE);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(0, 0, 0));
	SetTextColour(wxColour(255, 255, 255));
	
	defaultItemAttr.SetBackgroundColour(GetBackgroundColour());
	defaultItemAttr.SetFont(font);
	defaultItemAttr.SetTextColour(GetTextColour());
	
	selectedItemAttr 	= defaultItemAttr;
	selectedItemAttr	.SetTextColour(wxColour(255, 242,   0));
	selectedItemAttr	.SetFont(font.Bold());
	
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibFile().Bitmap("BMP_LRU_FILE"));
	imageList->Add(ImageLibFile().Bitmap("BMP_LRU_FILE_SELECTED"));
	SetImageList(imageList, wxIMAGE_LIST_SMALL);
	
	popupMenu = new wxMenu("");
	popupMenu->Append(miRemoveLruListEntry, wxT("Remove selected LRU List Entry"));
	
	//............................................
	popupMenu->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [&](wxCommandEvent& event) {
		this->removeSelectedItem();
	 }, miRemoveLruListEntry, miRemoveLruListEntry);
	 
	eventTimer->Connect(wxEVT_TIMER, wxTimerEventHandler(CncLruFileViewListCtrl::onEventTimer), NULL, this);
}
/////////////////////////////////////////////////////////////
CncLruFileViewListCtrl::~CncLruFileViewListCtrl() {
/////////////////////////////////////////////////////////////
	wxDELETE( popupMenu );
	
	if ( eventTimer->IsRunning() )
		eventTimer->Stop();
	
	eventTimer->Disconnect(wxEVT_TIMER, wxTimerEventHandler(CncLruFileViewListCtrl::onEventTimer), NULL, this);
	wxDELETE( eventTimer );
}
/////////////////////////////////////////////////////////////
int CncLruFileViewListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return -1;
		
	if ( column != COL_FILE )
		return -1;

	return item != getLastSelection() ? 0 : 1;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncLruFileViewListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	return (wxListItemAttr*)(item != getLastSelection() ? &defaultItemAttr : &selectedItemAttr);
}
/////////////////////////////////////////////////////////////////////
void CncLruFileViewListCtrl::updateColumnWidth() {
/////////////////////////////////////////////////////////////////////
	// avoid flicker
	GblFunc::freeze(this, true);
		
	// try to strech the second (key) column
	const int size = GetSize().GetWidth() - 26; 
	
	if ( size > GetColumnWidth(COL_STRECH) )
		SetColumnWidth(COL_STRECH, size);
		
	GblFunc::freeze(this, false);
}
/////////////////////////////////////////////////////////////////////
bool CncLruFileViewListCtrl::Enable(bool enable) {
/////////////////////////////////////////////////////////////////////
	GblFunc::freeze(this, !enable);
	Refresh();
	return CncLargeScaledListCtrl::Enable(enable); 
}
////////////////////////////////////////////////////////////////
void CncLruFileViewListCtrl::removeSelectedItem() {
////////////////////////////////////////////////////////////////
	const long ls = getLastSelection();
	
	if ( isItemValid(ls) == false )
		return;
		
	removeFile((unsigned int)ls);
}
////////////////////////////////////////////////////////////////
void CncLruFileViewListCtrl::updateListControl() {
////////////////////////////////////////////////////////////////
	deselectAll();
	SetItemCount(lruList.size());
	setLastSelection(0);
	Refresh();
}
////////////////////////////////////////////////////////////////
unsigned int CncLruFileViewListCtrl::getFileCount() const {
////////////////////////////////////////////////////////////////
	return lruList.size();
}
////////////////////////////////////////////////////////////////
const char* CncLruFileViewListCtrl::getFileName(unsigned int pos) {
////////////////////////////////////////////////////////////////
	static wxString ret;
	
	if ( pos < lruList.size() ) {
		ret.assign(lruList.at(pos).GetFullPath());
		return ret.c_str();
	}
	
	return "";
}
////////////////////////////////////////////////////////////////
bool CncLruFileViewListCtrl::addFile(const wxString& f) {
////////////////////////////////////////////////////////////////
	// f includes the full path
	wxFileName fn(f);
	
	if ( fn.Exists() == false ) {
		std::cerr << "LruFileList:addFile: Invalid file: " << f.c_str() << std::endl;
		return false;
	}
	
	if ( lruList.size() > 0 ) {
		// is it already the first entry
		if ( *lruList.begin() == f ) {
			updateListControl();
			return false;
		}
		
		// search if entry already exists
		for ( auto it=lruList.begin(); it!=lruList.end(); ++it ) {
			if ( *it == f ) {
				// remove the file
				lruList.erase(it);
				if ( lruList.size() == 0 )
					break;
				
				// restart to also find mutiple entries
				it = lruList.begin();
			} 
		}
	}
	
	
	// inster the current file at the top
	lruList.insert(lruList.begin(), fn);
	
	// shrink the lru list to the max size
	const unsigned int size = lruList.size();
	lruList.resize(size > maxSize ? maxSize : size );
	
	updateListControl();
	return true;
}
////////////////////////////////////////////////////////////////
bool CncLruFileViewListCtrl::removeFile(unsigned int idx) {
////////////////////////////////////////////////////////////////
	if ( isItemValid(idx) == false )
		return false;
		
	 auto it = lruList.begin() + idx;
	 if ( it ==  lruList.end() )
		 return false;
	 
	lruList.erase(it);
	
	updateListControl();
	return true;
}
////////////////////////////////////////////////////////////////
bool CncLruFileViewListCtrl::removeFile(const wxString& f) {
////////////////////////////////////////////////////////////////
	wxFileName fn(f);
	
	// search if entry exists
	bool ret = false;
	for ( auto it=lruList.begin(); it!=lruList.end(); ++it) {
		if ( *it == f ) {
			ret = true;
			
			// remove the file
			lruList.erase(it);
			if ( lruList.size() == 0 )
				break;
			
			// restart to also find mutiple entries
			it = lruList.begin();
		} 
	}
	
	updateListControl();
	return ret;
}
////////////////////////////////////////////////////////////////
bool CncLruFileViewListCtrl::load(wxFileConfig* config) {
////////////////////////////////////////////////////////////////
	if ( config == NULL )
		return false;
	
	CncLargeScaledListCtrl::clear();
	lruList.clear();
	
	for ( unsigned int i = 1; i <= maxSize; i++ ) {
		const wxString item(wxString::Format("%s/%s%d", lruSection, lruPrefix, i));
		wxString str;
		
		config->Read(item, &str, "");
		str = str.Trim(true).Trim();
		
		if ( str.IsEmpty() == false ) {
			
			wxFileName fn(str);
			if ( fn.Exists() ) {
				lruList.push_back(fn);
			}
		}
	}
	
	updateListControl();
	return true;
}
////////////////////////////////////////////////////////////////
bool CncLruFileViewListCtrl::save(wxFileConfig* config) {
////////////////////////////////////////////////////////////////
	if ( config == NULL )
		return false;
	
	config->DeleteGroup(lruSection);
	
	unsigned int cnt = 0;
	for ( auto it=lruList.begin(); it!=lruList.end(); ++it) {
		const wxString item(wxString::Format("%s/%s%d", lruSection, lruPrefix, (++cnt)));
		const wxString value((*it).GetFullPath());
		
		if ( value.IsEmpty() == false )
			config->Write(item,(*it).GetFullPath());
	}
	config->Flush();
	return true;
}
/////////////////////////////////////////////////////////////////////
void CncLruFileViewListCtrl::onSize(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////////////
	event.Skip();
	
	updateColumnWidth();
	Refresh();
}
//////////////////////////////////////////////////
void CncLruFileViewListCtrl::onKeyDown(wxKeyEvent& event) {
//////////////////////////////////////////////////
	bool ctlKey = CncAsyncKeyboardState::isControlPressed();
	int c = event.GetUnicodeKey();
	
	// save
	if ( c == 'S' && ctlKey == true ) {
		save(APP_PROXY::getLruStore());
		std::clog << "LRU List saved . . . " << std::endl;
		return;
	}
	
	event.Skip(true);
}
/////////////////////////////////////////////////////////////////////
void CncLruFileViewListCtrl::onRightDown(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	isLeaveEventActive = false;
	
		wxMenuItem* mi = popupMenu->FindChildItem(miRemoveLruListEntry);
		const long ls = getLastSelection();
		
		if ( isItemValid(ls) && mi != NULL ) {
			const wxString fn(GetItemText(getLastSelection(), COL_FILE));
			
			mi->SetItemLabel(wxString::Format("Remove LRU Item '%s'", fn));
			PopupMenu(popupMenu);
		}
		
	isLeaveEventActive = true;
}
/////////////////////////////////////////////////////////////////////
void CncLruFileViewListCtrl::onLeaveWindow(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( isLeaveEventActive == false )
		return;
		
	APP_PROXY::filePreviewListLeave();
}
/////////////////////////////////////////////////////////////////////
void CncLruFileViewListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////////////
	CFVL_PRINT_LOCATION_CTX_FILE
	
	deselectAll(true);
	setLastSelection(event.GetIndex());
	lastEventType = EventType::PREVIEW;
	
	// start a timer here to prevent the immediately call of 
	// this event and to wait if a activate event follows.
	// normal sequence: select, activate
	if ( eventTimer->IsRunning() == false )
		eventTimer->StartOnce(TimerEvent::SelectInterval);
}
/////////////////////////////////////////////////////////////////////
void CncLruFileViewListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////////////
	CFVL_PRINT_LOCATION_CTX_FILE
	
	lastEventType = EventType::OPEN;
	
	// in case of a previous select event the timer is still runing
	// but if not stat a short timer here
	// possible sequence select, <some longer time inmterval>,  activate
	if ( eventTimer->IsRunning() == false )
		eventTimer->StartOnce(TimerEvent::ActivateInterval);
}
//////////////////////////////////////////////////////////////
void CncLruFileViewListCtrl::onEventTimer(wxTimerEvent& event) { 
//////////////////////////////////////////////////////////////
	CFVL_PRINT_LOCATION_CTX_FILE
	
	eventTimer->Stop();
	
	const wxString fn(getFileName(getLastSelection()));
	if ( fn.IsEmpty() == true )
		return; 
		
	switch ( lastEventType ) {
		case EventType::PREVIEW:	APP_PROXY::openMainPreview(fn);
									break;
									
		case EventType::OPEN:		APP_PROXY::openFileFromFileManager(fn);
									setLastSelection(0);
									break;
		default:					;
	}
	
	lastEventType = EventType::UNKNOWN;
}
/////////////////////////////////////////////////////////////////////
void CncLruFileViewListCtrl::selectFirstItem() {
/////////////////////////////////////////////////////////////////////
	updateListControl();
}
/////////////////////////////////////////////////////////////////////
wxString CncLruFileViewListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return _("");
		
	if ( column != COL_FILE )
		return _("");
	
	static wxString fn;
	fn.assign(lruList.at(item).GetFullName());
	
	return fn.IsEmpty() == false ? fn : _("");
}
/////////////////////////////////////////////////////////////////////
bool CncLruFileViewListCtrl::isItemValid(long item) const {
/////////////////////////////////////////////////////////////////////
	if ( item < 0 || item > (long)(lruList.size() - 1) )
		return false;
	
	return true;
}






// ----------------------------------------------------------------------------
// CncFileViewListCtrl Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(CncFileViewListCtrl, CncLargeScaledListCtrl)
	EVT_SIZE				(			CncFileViewListCtrl::onSize				)
	EVT_LEAVE_WINDOW		(			CncFileViewListCtrl::onLeaveWindow		)
	EVT_LIST_ITEM_SELECTED	(wxID_ANY, 	CncFileViewListCtrl::onSelectListItem	)
	EVT_LIST_ITEM_ACTIVATED	(wxID_ANY, 	CncFileViewListCtrl::onActivateListItem	)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncFileViewListCtrl::CncFileViewListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, fileEntries()
, eventTimer(new wxTimer())
, defaultItemAttr()
, selectedItemAttr()
, lastEventType(EventType::UNKNOWN)
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Workarea:", wxLIST_FORMAT_LEFT, 250);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::NORMAL);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(0, 0, 0));
	SetTextColour(wxColour(255, 255, 255));
	
	defaultItemAttr.SetBackgroundColour(GetBackgroundColour());
	defaultItemAttr.SetFont(font);
	defaultItemAttr.SetTextColour(GetTextColour());
	
	selectedItemAttr 	= defaultItemAttr;
	selectedItemAttr	.SetTextColour(wxColour(255, 242,   0));
	selectedItemAttr	.SetFont(font.Bold());
	
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibFile().Bitmap("BMP_FOLDER_UP"));
	imageList->Add(ImageLibFile().Bitmap("BMP_FOLDER"));
	imageList->Add(ImageLibFile().Bitmap("BMP_FILE"));
	imageList->Add(ImageLibFile().Bitmap("BMP_ERROR"));
	imageList->Add(ImageLibFile().Bitmap("BMP_FILE_SELECTED"));
	
	SetImageList(imageList, wxIMAGE_LIST_SMALL);
	
	eventTimer->Connect(wxEVT_TIMER, wxTimerEventHandler(CncFileViewListCtrl::onEventTimer), NULL, this);
}
/////////////////////////////////////////////////////////////
CncFileViewListCtrl::~CncFileViewListCtrl() {
/////////////////////////////////////////////////////////////
	deleteAllEntries();
	
	if ( eventTimer->IsRunning() )
		eventTimer->Stop();
	
	eventTimer->Disconnect(wxEVT_TIMER, wxTimerEventHandler(CncFileViewListCtrl::onEventTimer), NULL, this);
	wxDELETE( eventTimer );
}
/////////////////////////////////////////////////////////////////////
bool CncFileViewListCtrl::isItemValid(long item) const {
/////////////////////////////////////////////////////////////////////
	if( item < 0 || item > (long)(fileEntries.size() - 1) )
		return false;
		
	return true;
}
/////////////////////////////////////////////////////////////////////
void CncFileViewListCtrl::deleteAllEntries() {
/////////////////////////////////////////////////////////////////////
	CncLargeScaledListCtrl::clear();
	fileEntries.clear();
	
	SetItemCount(fileEntries.size());
	Refresh();
}
/////////////////////////////////////////////////////////////////////
void CncFileViewListCtrl::addFileEntry(const wxString& name, FileListImage fii) {
/////////////////////////////////////////////////////////////////////
	fileEntries.push_back(FileEntry(name, fii));
	SetItemCount(fileEntries.size());
	Refresh();
}
/////////////////////////////////////////////////////////////
bool CncFileViewListCtrl::selectFileInList(const wxString& fileName) {
/////////////////////////////////////////////////////////////
	for ( auto it = fileEntries.begin(); it != fileEntries.end(); ++it ) {
		
		if ( it->fileName == fileName ) {
			selectItem(std::distance(fileEntries.begin(), it), true);
			return true;
		}
	}
	
	return false;
}
/////////////////////////////////////////////////////////////
int CncFileViewListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return -1;
		
	if ( column != COL_FILE )
		return -1;
		
	if ( fileEntries.at(item).imageIdx == FileListImage::FTI_FILE_SELECTED && item != getLastSelection() )
		return FileListImage::FTI_FILE_SELECTED;
		
	return fileEntries.at(item).imageIdx;
}
/////////////////////////////////////////////////////////////////////
wxString CncFileViewListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return _("");
		
	if ( column != COL_FILE )
		return _("");
	
	return fileEntries.at(item).fileName;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncFileViewListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	return (wxListItemAttr*)(item != getLastSelection() ? &defaultItemAttr : &selectedItemAttr);
}
/////////////////////////////////////////////////////////////////////
void CncFileViewListCtrl::updateColumnWidth() {
/////////////////////////////////////////////////////////////////////
	// avoid flicker
	GblFunc::freeze(this, true);
		
	// try to strech the second (key) column
	const int size = GetSize().GetWidth() - 26; 
	
	if ( size > GetColumnWidth(COL_STRECH) )
		SetColumnWidth(COL_STRECH, size);
		
	GblFunc::freeze(this, false);
}
/////////////////////////////////////////////////////////////////////
bool CncFileViewListCtrl::Enable(bool enable) {
/////////////////////////////////////////////////////////////////////
	GblFunc::freeze(this, !enable);
	return CncLargeScaledListCtrl::Enable(enable); 
}
/////////////////////////////////////////////////////////////////////
void CncFileViewListCtrl::onSize(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////////////
	updateColumnWidth();
	event.Skip(true);
}
/////////////////////////////////////////////////////////////////////
void CncFileViewListCtrl::onLeaveWindow(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	CncFileView* fv = static_cast<CncFileView*>(GetParent());
	if ( fv == NULL )
		return;
	
	fv->fileListLeave(event);
}
/////////////////////////////////////////////////////////////////////
void CncFileViewListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////////////
	CFVL_PRINT_LOCATION_CTX_FILE
	
	setLastSelection(event.GetIndex());
	lastEventType = EventType::PREVIEW;
	
	// start a timer here to prevent the immediately call of 
	// this event and to wait if a activate event follows.
	// normal sequence: select, activate
	if ( eventTimer->IsRunning() == false )
		eventTimer->StartOnce(TimerEvent::SelectInterval);
}
/////////////////////////////////////////////////////////////////////
void CncFileViewListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////////////
	CFVL_PRINT_LOCATION_CTX_FILE
	
	lastEventType = EventType::OPEN;
	
	// in case of a previous select event the timer is still runing
	// but if not stat a short timer here
	// possible sequence select, <some longer time inmterval>,  activate
	if ( eventTimer->IsRunning() == false )
		eventTimer->StartOnce(TimerEvent::ActivateInterval);
}
//////////////////////////////////////////////////////////////
void CncFileViewListCtrl::onEventTimer(wxTimerEvent& event) { 
//////////////////////////////////////////////////////////////
	CFVL_PRINT_LOCATION_CTX_FILE

	CncFileView* fv = static_cast<CncFileView*>(GetParent());
	if ( fv == NULL )
		return;
		
	switch ( lastEventType ) {
		case EventType::PREVIEW:	fv->fileListSelected(getLastSelection()); 	break;
		case EventType::OPEN:		fv->fileListActivated(getLastSelection());	break;
		default:					;
	}
	
	lastEventType = EventType::UNKNOWN;
}





