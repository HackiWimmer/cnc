#ifndef CNC_MOVE_SEQUENCE_LIST_CTRL_H
#define CNC_MOVE_SEQUENCE_LIST_CTRL_H

#include "CncLargeScaleListCtrl.h"

class CncMoveSequenceListCtrl : public CncLargeScaledListCtrl {

	private:
		wxListItemAttr defaultItemAttr;
		wxListItemAttr initialItemAttr;
		wxListItemAttr clientIdItemAttr;
		wxListItemAttr speedItemAttr;

		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		
		void updateColumnWidth();

	public:
		static const int COL_TYPE 			=  0;
		static const int COL_REF 			=  1;
		static const int COL_CLD_ID			=  2;
		static const int COL_DISTANCE_X		=  3;
		static const int COL_DISTANCE_Y		=  4;
		static const int COL_DISTANCE_Z		=  5;

		static const int TOTAL_COL_COUNT	=  6;
		static const int COL_SEARCH			= COL_CLD_ID;
		static const int COL_STRECH			= COL_REF;

		CncMoveSequenceListCtrl(wxWindow *parent, long style);
		virtual ~CncMoveSequenceListCtrl();

		void onSize(wxSizeEvent& event);
		void onSelectListItem(wxListEvent& event);
		void onActivateListItem(wxListEvent& event);

		bool searchReference(const wxString& what);
		bool searchReferenceById(const long id);

		wxDECLARE_NO_COPY_CLASS(CncMoveSequenceListCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif
