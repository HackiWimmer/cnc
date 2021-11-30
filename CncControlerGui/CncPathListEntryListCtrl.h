#ifndef CNC_PATH_LIST_ENTRY_LIST_CTRL_H
#define CNC_PATH_LIST_ENTRY_LIST_CTRL_H

#include <vector>
#include "CncPathListManager.h"
#include "CncLargeScaleListCtrl.h"

class CncPathListEntryListCtrl : public CncLargeScaledListCtrl {
	
	private:
		wxListItemAttr defaultItemAttr;
		wxListItemAttr initialItemAttr;
		wxListItemAttr clientIdItemAttr;
		wxListItemAttr speedItemAttr;
		wxListItemAttr toolItemAttr;
		
		wxListItemAttr defaultItemAttrSelected;
		wxListItemAttr initialItemAttrSelected;
		wxListItemAttr clientIdItemAttrSelected;
		wxListItemAttr speedItemAttrSelected;
		wxListItemAttr toolItemAttrSelected;
		
		typedef std::vector<CncPathListEntry> PathLists;
		PathLists pathLists;
		
		bool showAllFlag;

		void updateColumnWidth();
		
		void onSize(wxSizeEvent& event);
		void onSelectListItem(wxListEvent& event);
		void onActivateListItem(wxListEvent& event);
		
		virtual bool isItemValid(long item) const;
		
		virtual wxString OnGetItemText(long item, long column) const;
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		
	public:

		static const int COL_CONT 			=  0;
		static const int COL_REF 			=  1;
		static const int COL_CLD_ID			=  2;
		static const int COL_F				=  3;
		static const int COL_S				=  4;
		static const int COL_DISTANCE_X		=  5;
		static const int COL_DISTANCE_Y		=  6;
		static const int COL_DISTANCE_Z		=  7;
		static const int COL_TARGET_X		=  8;
		static const int COL_TARGET_Y		=  9;
		static const int COL_TARGET_Z		= 10;
		static const int COL_TOTAL_DISTANCE = 11;
		static const int COL_STRECH 		= 12;
		
		static const int TOTAL_COL_COUNT	= 12;
		
		static const int COL_SEARCH			= COL_CLD_ID;
		
		CncPathListEntryListCtrl(wxWindow *parent, long style);
		virtual ~CncPathListEntryListCtrl();
		
		void addPathListEntries(const CncPathListManager& cpm);
		void addPathListEntry(const CncPathListEntry& cpe);
		void clearAll();

		bool searchReference(const wxString& what);
		bool searchReferenceById(const long id);
		
		void setFormatFlag(bool flag);
		
		bool skipToFirstReference();
		bool skipToPrevReference();
		bool skipToNextReference();
		bool skipToLastReference();
		
		wxDECLARE_NO_COPY_CLASS(CncPathListEntryListCtrl);
		wxDECLARE_EVENT_TABLE();
		
};

#endif
