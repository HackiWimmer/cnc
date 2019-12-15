#ifndef LRU_LIST_H
#define LRU_LIST_H

#include <iostream>
#include <vector>
#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include <wx/string.h>

typedef std::vector<wxFileName> LruList;
class LruFileList {
	
	private:
		const char* lruSection = "LRU_List";
		const char* lruPrefix  = "LRU_FILE_";

		unsigned int 	size;
		wxString 		ret;
		wxListCtrl* 	listControl;
		LruList 		lruList;
		wxImageList* 	imageList;
		
		void updateListControl();
		
	public:
		
		LruFileList(unsigned int s);
		virtual ~LruFileList();
		
		unsigned int getFileCount() const ;
		const char* getFileName(unsigned int pos);

		void setListControl(wxListCtrl* lc);
		void addFile(const wxString& f);
		void removeFile(const wxString& f);
		bool load(wxFileConfig* config);
		bool save(wxFileConfig* config);
};

#endif