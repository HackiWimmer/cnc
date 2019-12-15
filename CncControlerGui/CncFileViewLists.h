#ifndef CNC_FILE_VIEW_LIST_CTRL_H
#define CNC_FILE_VIEW_LIST_CTRL_H

#include <wx/filename.h>
#include "CncLargeScaleListCtrl.h"

class CncFileViewListCtrl : public CncLargeScaledListCtrl {
	
	private:
		
		virtual wxString OnGetItemText(long item, long column) const;
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		
		void onSize(wxSizeEvent& event);
		void onLeaveWindow(wxMouseEvent& event);
		void onSelectListItem(wxListEvent& event);
		void onActivateListItem(wxListEvent& event);
		
	public:
		static const int COL_FILE 			= 0;
		static const int TOTAL_COL_COUNT	= 1;
		static const int COL_STRECH			= COL_FILE;
		
		CncFileViewListCtrl(wxWindow *parent, long style);
		virtual ~CncFileViewListCtrl();
		
		virtual bool Enable(bool enable=true);
		
		void updateColumnWidth();
		
		wxDECLARE_NO_COPY_CLASS(CncFileViewListCtrl);
		wxDECLARE_EVENT_TABLE();
};

class wxFileConfig;
class CncLruFileViewListCtrl : public CncLargeScaledListCtrl {
		
	private:
		
		const char* lruSection = "LRU_List";
		const char* lruPrefix  = "LRU_FILE_";

		typedef std::vector<wxFileName> LruList;
		LruList 		lruList;
		wxMenu* 		popupMenu;
		bool			isLeaveEventActive;
		unsigned int 	maxSize;
			
		virtual wxString OnGetItemText(long item, long column) const;
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		
		void onSize(wxSizeEvent& event);
		void onKeyDown(wxKeyEvent& event);
		void onRightDown(wxMouseEvent& event);
		void onLeaveWindow(wxMouseEvent& event);
		void onSelectListItem(wxListEvent& event);
		void onActivateListItem(wxListEvent& event);
		
		void removeSelectedItem();
		void updateListControl();
		
	protected:
		
		virtual bool isItemValid(long item) const;
		
	public:
		
		static const int miRemoveLruListEntry		= 8000;
		
		static const int COL_FILE 					= 0;
		static const int TOTAL_COL_COUNT			= 1;
		static const int COL_STRECH					= COL_FILE;
		
		CncLruFileViewListCtrl(wxWindow *parent, unsigned int ms, long style);
		virtual ~CncLruFileViewListCtrl();
		
		virtual bool Enable(bool enable=true);
		
		void updateColumnWidth();
		
		unsigned int getFileCount() const ;
		const char* getFileName(unsigned int pos);
		
		bool addFile(const wxString& f);
		bool removeFile(unsigned int idx);
		bool removeFile(const wxString& f);
		bool load(wxFileConfig* config);
		bool save(wxFileConfig* config);
		
		wxDECLARE_NO_COPY_CLASS(CncLruFileViewListCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif