#include "LruFileList.h"
#include "wxCrafterImages.h"

////////////////////////////////////////////////////////////////
LruFileList::LruFileList(unsigned int s)
: size(s ? s : 5)
, ret()
, listControl(NULL)
, lruList()
, imageList(new wxImageList(16, 16, true))
////////////////////////////////////////////////////////////////
{
}
////////////////////////////////////////////////////////////////
LruFileList::~LruFileList() {
////////////////////////////////////////////////////////////////
	lruList.clear();
	
	if ( listControl )
		listControl->DeleteAllItems();
		
	delete imageList;
}
////////////////////////////////////////////////////////////////
void LruFileList::updateListControl() {
////////////////////////////////////////////////////////////////
	if ( listControl == NULL )
		return;
		
	// ensure that there are exact one coloum!
	// and setup the control
	if ( listControl->GetColumnCount() != 1 ) {
		listControl->DeleteAllColumns();
		listControl->AppendColumn("Last recently used:", wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);
		
		imageList->RemoveAll();
		imageList->Add(ImageLibFile().Bitmap("BMP_LRU_FILE"));
		
		listControl->SetImageList(imageList, wxIMAGE_LIST_SMALL);
	}
	
	listControl->SetColumnWidth(0, 300);
	listControl->DeleteAllItems();
	
	for ( LruList::iterator it=lruList.begin(); it!=lruList.end(); ++it) {
		const wxString fn((*it).GetFullName());
		if ( fn.IsEmpty() == false )
			listControl->InsertItem(listControl->GetItemCount(), fn, 0);
	}
	
	listControl->Refresh();
	listControl->Update();
}
////////////////////////////////////////////////////////////////
void LruFileList::setListControl(wxListCtrl* lc) {
////////////////////////////////////////////////////////////////
	listControl = lc;
}
////////////////////////////////////////////////////////////////
unsigned int LruFileList::getFileCount() const {
	return lruList.size();
}
////////////////////////////////////////////////////////////////
const char* LruFileList::getFileName(unsigned int pos) {
////////////////////////////////////////////////////////////////
	if ( pos < lruList.size() ) {
		ret = lruList.at(pos).GetFullPath();
		return ret.c_str();
	}
	
	return "";
}
////////////////////////////////////////////////////////////////
void LruFileList::addFile(const wxString& f) {
////////////////////////////////////////////////////////////////
	// f includes the full path
	wxFileName fn(f);
	
	if ( fn.Exists() == false ) {
		std::cerr << "LruFileList:addFile: Invalid file: " << f.c_str() << std::endl;
		return;
	}
	
	if ( lruList.size() > 0 ) {
		// is it already the first entry
		if ( *lruList.begin() == f ) {
			updateListControl();
			return;
		}
		
		// search if entry already exists
		for ( LruList::iterator it=lruList.begin(); it!=lruList.end(); ++it) {
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
	lruList.resize(size);
	
	updateListControl();
}
////////////////////////////////////////////////////////////////
void LruFileList::removeFile(const wxString& f) {
////////////////////////////////////////////////////////////////
	wxFileName fn(f);
	
	// search if entry exists
	for ( LruList::iterator it=lruList.begin(); it!=lruList.end(); ++it) {
		if ( *it == f ) {
			// remove the file
			lruList.erase(it);
			if ( lruList.size() == 0 )
				break;
			
			// restart to also find mutiple entries
			it = lruList.begin();
		} 
	}
	
	updateListControl();
}
////////////////////////////////////////////////////////////////
bool LruFileList::load(wxFileConfig* config) {
////////////////////////////////////////////////////////////////
	if ( config == NULL )
		return false;

	lruList.clear();
	
	wxString str;
	for ( unsigned int i=1; i<=size; i++ ) {
		wxString item(lruSection);
		item << "/";
		item << lruPrefix;
		item << i;
		
		config->Read(item, &str, "");
		str = str.Trim(true).Trim();
		
		if ( str != "" ) {
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
bool LruFileList::save(wxFileConfig* config) {
////////////////////////////////////////////////////////////////
	if ( config == NULL )
		return false;
	
	config->DeleteGroup(lruSection);
	
	unsigned int cnt = 0;
	for ( LruList::iterator it=lruList.begin(); it!=lruList.end(); ++it) {
		wxString item(lruSection);
		item << "/";
		item << lruPrefix;
		item << (++cnt);
		
		config->Write(item,(*it).GetFullPath());
	}
	config->Flush();
	return true;
}
