#ifndef CNC_FILE_VIEW_LIST_CTRL_H
#define CNC_FILE_VIEW_LIST_CTRL_H

#include <wx/filename.h>
#include <wx/timer.h>
#include "CncFileViewCallback.h"
#include "CncLargeScaleListCtrl.h"

class wxFileConfig;
class CncLruFileViewListCtrl : public CncLargeScaledListCtrl {
		
	private:
		
		enum EventType { UNKNOWN, PREVIEW, OPEN };
		
		const char* lruSection = "LRU_List";
		const char* lruPrefix  = "LRU_FILE_";

		typedef std::vector<wxFileName> LruList;
		LruList 		lruList;
		wxTimer* 		eventTimer;
		wxListItemAttr 	defaultItemAttr;
		wxListItemAttr 	selectedItemAttr;
		wxMenu* 		popupMenu;
		bool			isLeaveEventActive;
		unsigned int 	maxSize;
		EventType		lastEventType;
			
		virtual wxString OnGetItemText(long item, long column) const;
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		
		void onEventTimer(wxTimerEvent& event);
		void onSize(wxSizeEvent& event);
		void onKeyDown(wxKeyEvent& event);
		void onRightDown(wxMouseEvent& event);
		void onLeaveWindow(wxMouseEvent& event);
		void onSelectListItem(wxListEvent& event);
		void onActivateListItem(wxListEvent& event);
		
		void copySelectedItemName();
		void removeSelectedItem();
		void updateListControl();
		
	protected:
		virtual bool isItemValid(long item) const;
		bool save();
		
	public:
		
		static const int miSaveLruListEntry			= 8000;
		static const int miCopyLruListEntry			= 8001;
		static const int miRemoveLruListEntry		= 8002;
		
		static const int smallFontSize				=  8;
		static const int bigFontSize				= 16;
		
		static const int COL_FILE 					= 0;
		static const int TOTAL_COL_COUNT			= 1;
		static const int COL_STRECH					= COL_FILE;
		
		CncLruFileViewListCtrl(wxWindow *parent, unsigned int ms, long style);
		virtual ~CncLruFileViewListCtrl();
		
		virtual bool Enable(bool enable=true);
		
		void updateColumnWidth();
		void selectFirstItem();
		
		unsigned int getFileCount() const ;
		wxString getFileName(unsigned int pos);
		
		bool addFile(const wxString& f);
		bool removeFile(unsigned int idx);
		bool removeFile(const wxString& f);
		bool load(wxFileConfig* config);
		bool save(wxFileConfig* config);
		
		void setBigTheme(bool big);
		
		wxDECLARE_NO_COPY_CLASS(CncLruFileViewListCtrl);
		wxDECLARE_EVENT_TABLE();
};


class CncFileViewListCtrl : public CncLargeScaledListCtrl {
	
	public:
		enum FileListImage {
			FTI_FOLDER_UP 		= 0,
			FTI_FOLDER    		= 1,
			FTI_FILE      		= 2,
			FTI_ERROR     		= 3,
			FTI_FILE_SELECTED 	= 4
		};
		
		struct FileEntry {
			FileListImage	imageIdx;
			wxString		fileName;
			
			FileEntry(const wxString& n, FileListImage i) 
			: imageIdx(i)
			, fileName(n)
			{}
		};
		
		typedef std::vector<FileEntry> FileEntryList;
	
	private:
		
		enum EventType { UNKNOWN, PREVIEW, OPEN };
		
		FileEntryList 					fileEntries;
		wxTimer* 						eventTimer;

		wxListItemAttr					defaultItemAttr;
		wxListItemAttr					selectedItemAttr;
		EventType						lastEventType;
		CncFileViewCallback::Interface*	caller;
		
		virtual wxString OnGetItemText(long item, long column) const;
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		
		void onEventTimer(wxTimerEvent& event);
		void onSize(wxSizeEvent& event);
		void onLeaveWindow(wxMouseEvent& event);
		void onSelectListItem(wxListEvent& event);
		void onActivateListItem(wxListEvent& event);
		
	protected:
		virtual bool isItemValid(long item) const;

	public:
		static const int COL_FILE 			= 0;
		static const int TOTAL_COL_COUNT	= 1;
		static const int COL_STRECH			= COL_FILE;
		
		static const int smallFontSize		=  8;
		static const int bigFontSize		= 16;

		CncFileViewListCtrl(wxWindow *parent, long style);
		virtual ~CncFileViewListCtrl();
		
		virtual bool Enable(bool enable=true);
		
		void updateColumnWidth();
		
		void deleteAllEntries();
		void addFileEntry(const wxString& name, FileListImage fii);
		bool selectFileInList(const wxString& fileName);
		
		void setBigTheme(bool big);
		void setObserver(CncFileViewCallback::Interface* o) { caller = o; }
		
		wxDECLARE_NO_COPY_CLASS(CncFileViewListCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif