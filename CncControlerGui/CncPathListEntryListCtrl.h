#ifndef CNC_PATH_LIST_ENTRY_LIST_CTRL_H
#define CNC_PATH_LIST_ENTRY_LIST_CTRL_H

#include <vector>
#include "CncPathListEntry.h"
#include "CncLargeScaleListCtrl.h"

class CncPathListEntryListCtrl : public CncLargeScaledListCtrl {
	
	private:
		wxListItemAttr defaultItemAttr;
		wxListItemAttr initialItemAttr;
		wxListItemAttr clientIdItemAttr;
		wxListItemAttr speedItemAttr;
		
		wxListItemAttr defaultItemAttrSelected;
		wxListItemAttr initialItemAttrSelected;
		wxListItemAttr clientIdItemAttrSelected;
		wxListItemAttr speedItemAttrSelected;
		
		typedef std::vector<CncPathListEntry> PathLists;
		PathLists pathLists;

		void updateColumnWidth();
		
		void onSize(wxSizeEvent& event);
		void onSelectListItem(wxListEvent& event);
		void onActivateListItem(wxListEvent& event);
		
		virtual bool isItemValid(long item) const;
		
		virtual wxString OnGetItemText(long item, long column) const;
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		
	public:

		static const int COL_TYPE 			=  0;
		static const int COL_REF 			=  1;
		static const int COL_CLD_ID			=  2;
		static const int COL_F				=  3;
		static const int COL_DISTANCE_X		=  4;
		static const int COL_DISTANCE_Y		=  5;
		static const int COL_DISTANCE_Z		=  6;
		static const int COL_TARGET_X		=  7;
		static const int COL_TARGET_Y		=  8;
		static const int COL_TARGET_Z		=  9;
		static const int COL_TOTAL_DISTANCE = 10;
		static const int COL_STRECH 		= 11;
		
		static const int TOTAL_COL_COUNT	= 12;
		
		static const int COL_SEARCH			= COL_CLD_ID;
		
		CncPathListEntryListCtrl(wxWindow *parent, long style);
		virtual ~CncPathListEntryListCtrl();
		
		void addPathListEntry(const CncPathListEntry& cpe);
		void clearAll();

		bool searchReference(const wxString& what);
		bool searchReferenceById(const long id);
		
		bool skipToFirstReference();
		bool skipToPrevReference();
		bool skipToNextReference();
		bool skipToLastReference();
		
		wxDECLARE_NO_COPY_CLASS(CncPathListEntryListCtrl);
		wxDECLARE_EVENT_TABLE();
		
};

#endif
