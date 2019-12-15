#include <iostream>
#include <wx/imaglist.h>
#include <wx/fileconf.h>
#include <wx/menu.h>
#include "OSD/CncAsyncKeyboardState.h"
#include "MainFrameProxy.h"
#include "GlobalFunctions.h"
#include "wxCrafterImages.h"
#include "CncFileViewLists.h"

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
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Workarea:", wxLIST_FORMAT_LEFT, 250);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::REVERSE);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(0, 0, 0));
	SetTextColour(wxColour(255, 255, 255));
	
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibFile().Bitmap("BMP_FILE"));		// 0
	SetImageList(imageList, wxIMAGE_LIST_SMALL);
}
/////////////////////////////////////////////////////////////
CncFileViewListCtrl::~CncFileViewListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
int CncFileViewListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	return 0;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncFileViewListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	return (wxListItemAttr*)(NULL);
}
/////////////////////////////////////////////////////////////////////
void CncFileViewListCtrl::updateColumnWidth() {
/////////////////////////////////////////////////////////////////////
	// avoid flicker
	if ( IsFrozen() == false )
		Freeze();
		
	SetColumnWidth(COL_STRECH, wxLIST_AUTOSIZE);
	
	// try to strech the second (key) column
	const int size = GetSize().GetWidth(); 
	
	if ( size > GetColumnWidth(COL_STRECH) )
		SetColumnWidth(COL_STRECH, size);
		
	if ( IsFrozen() == true )
		Thaw();
}
/////////////////////////////////////////////////////////////////////
bool CncFileViewListCtrl::Enable(bool enable) {
/////////////////////////////////////////////////////////////////////
	if ( enable == false ) 	{ if ( IsFrozen() == false ) 	Freeze(); }
	else					{ if ( IsFrozen() == true ) 	Thaw();   }
	
	return true;
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
	
}
/////////////////////////////////////////////////////////////////////
void CncFileViewListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////////////
	
}
/////////////////////////////////////////////////////////////////////
void CncFileViewListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////////////
	
}
/////////////////////////////////////////////////////////////////////
wxString CncFileViewListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////////////
	
	return _("");
}




// ----------------------------------------------------------------------------
// CncFileViewListCtrl Event Table
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
, popupMenu(NULL)
, isLeaveEventActive(true)
, maxSize(ms)
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
}
/////////////////////////////////////////////////////////////
CncLruFileViewListCtrl::~CncLruFileViewListCtrl() {
/////////////////////////////////////////////////////////////
	delete popupMenu;
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
	return (wxListItemAttr*)(NULL);
}
/////////////////////////////////////////////////////////////////////
void CncLruFileViewListCtrl::updateColumnWidth() {
/////////////////////////////////////////////////////////////////////
	// avoid flicker
	if ( IsFrozen() == false )
		Freeze();
		
	SetColumnWidth(COL_STRECH, wxLIST_AUTOSIZE);
	
	// try to strech the second (key) column
	const int size = GetSize().GetWidth(); 
	
	if ( size > GetColumnWidth(COL_STRECH) )
		SetColumnWidth(COL_STRECH, size);
		
	if ( IsFrozen() == true )
		Thaw();
}
/////////////////////////////////////////////////////////////////////
bool CncLruFileViewListCtrl::Enable(bool enable) {
/////////////////////////////////////////////////////////////////////
	GblFunc::freeze(this, enable);
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

	std::clog << "LRUsize: " << lruList.size() << std::endl;

	SetItemCount(lruList.size());
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
	updateColumnWidth();
	event.Skip(true);
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
		
	APP_PROXY::lruListItemLeave();
	deselectAll();
	updateListControl();
}
/////////////////////////////////////////////////////////////////////
void CncLruFileViewListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////////////
	setLastSelection(event.GetIndex());
	
	const wxString fn(getFileName(event.GetIndex()));
	if ( fn.IsEmpty() == false )
		APP_PROXY::openMainPreview(fn);
}
/////////////////////////////////////////////////////////////////////
void CncLruFileViewListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////////////
	const wxString fn(getFileName(event.GetIndex()));
	if ( fn.IsEmpty() == false )
		APP_PROXY::openFileFromFileManager(fn);
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
	if ( item < 0 || item > (long)(lruList.size()) )
		return false;
	
	return true;
}
