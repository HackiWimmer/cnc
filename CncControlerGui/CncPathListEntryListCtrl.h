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
		wxListItemAttr spindleItemAttr;
		wxListItemAttr toolItemAttr;
		
		wxListItemAttr defaultItemAttrSelected;
		wxListItemAttr initialItemAttrSelected;
		wxListItemAttr clientIdItemAttrSelected;
		wxListItemAttr speedItemAttrSelected;
		wxListItemAttr spindleItemAttrSelected;
		wxListItemAttr toolItemAttrSelected;
		
		typedef std::vector<CncPathListEntry> PathLists;
		PathLists pathLists;
		
		bool showAllFlag;

		void updateColumnWidth();
		
		void onPaint(wxPaintEvent& event);
		void onSize(wxSizeEvent& event);
		void onSelectListItem(wxListEvent& event);
		void onActivateListItem(wxListEvent& event);
		
		virtual bool isItemValid(long item) const override;
		
		virtual wxString OnGetItemText(long item, long column) const override;
		virtual int OnGetItemColumnImage(long item, long column) const override;
		virtual wxListItemAttr* OnGetItemAttr(long item) const override;
		
		virtual wxString getFormatedItemText(long item, int col = 0) const override;

	public:

		static const int COL_CONT 			=  0;
		static const int COL_REF 			=  1;
		static const int COL_CLD_ID			=  2;
		static const int COL_TOOL_ID		=  3;
		static const int COL_F				=  4;
		static const int COL_S				=  5;
		static const int COL_DISTANCE_X		=  6;
		static const int COL_DISTANCE_Y		=  7;
		static const int COL_DISTANCE_Z		=  8;
		static const int COL_TARGET_X		=  9;
		static const int COL_TARGET_Y		= 10;
		static const int COL_TARGET_Z		= 11;
		static const int COL_TOTAL_DISTANCE = 12;
		static const int COL_STRECH 		= 12;
		static const int TOTAL_COL_COUNT	= 13;
		
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
		
		virtual long getItemCount() const override { return pathLists.size(); }
		
		wxDECLARE_NO_COPY_CLASS(CncPathListEntryListCtrl);
		wxDECLARE_EVENT_TABLE();
		
};

#endif
