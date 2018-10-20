#ifndef CNC_SETTER_LIST_CTRL_H
#define CNC_SETTER_LIST_CTRL_H

#include "CncLargeScaleListCtrl.h"

class CncSetterListCtrl : public CncLargeScaledListCtrl {
	
	private:
		wxString separatorPid;
		wxListItemAttr defaultItemAttr;
		wxListItemAttr separatorRunItemAttr;
		wxListItemAttr separatorResetItemAttr;
		wxListItemAttr separatorSetupItemAttr;
		
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		
	public:
		static const int COL_TYPE 		= 0;
		static const int COL_NUM 		= 1;
		static const int COL_PID 		= 2;
		static const int COL_KEY 		= 3;
		static const int COL_VAL 		= 4;
		static const int COL_UNIT 		= 5;
		
		static const int TOTAL_COL_COUNT	= 6;
		
		CncSetterListCtrl(wxWindow *parent, long style);
		virtual ~CncSetterListCtrl();
		
		void updateColumnWidth();
		
		void onSize(wxSizeEvent& event);
		
		wxDECLARE_NO_COPY_CLASS(CncSetterListCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif