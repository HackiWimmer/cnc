#ifndef LRU_LIST_H
#define LRU_LIST_H

#include <vector>
#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/listbox.h>
#include <wx/string.h>

typedef std::vector<wxFileName> LruList;
class LruFileList {
	
	private:
		const char* lruSection = "LRU_List";
		const char* lruPrefix= "LRU_FILE_";
		unsigned int size;
		wxString ret;
		wxListBox* listControl;
		LruList lruList;
		
		////////////////////////////////////////////////////////////////
		void updateListControl() {
			if ( listControl == NULL )
				return;
			
			listControl->Clear();
			for ( LruList::iterator it=lruList.begin(); it!=lruList.end(); ++it) {
				listControl->Append((*it).GetFullName());
			}
			
			listControl->Refresh();
			listControl->Update();
		}
	
	public:
		////////////////////////////////////////////////////////////////
		LruFileList(unsigned int s)
		: size(s ? s : 5)
		, listControl(NULL)
		{
			
		}
		
		~LruFileList() {
			lruList.clear();
		}
		
		////////////////////////////////////////////////////////////////
		void setListControl(wxListBox* lc) {
			listControl = lc;
		}
		
		////////////////////////////////////////////////////////////////
		unsigned int getFileCount() {
			return lruList.size();
		}
		
		////////////////////////////////////////////////////////////////
		const char* getFileName(unsigned int pos) {
			if ( pos < lruList.size() ) {
				ret = lruList.at(pos).GetFullPath();
				return ret.c_str();
			}
			return "";
		}
		
		////////////////////////////////////////////////////////////////
		void addFile(const wxString& f) {
			wxFileName fn(f);
			
			if ( fn.Exists() == false ) {
				std::cerr << "LruFileList:addFile: Invalid file: " << f.c_str() << std::endl;
				return;
			}
			
			for ( LruList::iterator it=lruList.begin(); it!=lruList.end(); ++it) {
				if ( *it == f ) {
					lruList.erase(it);
					
					if ( lruList.size() == 0 )
						break;
					
					it = lruList.begin();
				}
			}
			
			lruList.insert(lruList.begin(), fn);
			lruList.resize(size);
			
			updateListControl();
		}
		
		////////////////////////////////////////////////////////////////
		bool load(wxFileConfig* config) {
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
		bool save(wxFileConfig* config) {
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
};


#endif