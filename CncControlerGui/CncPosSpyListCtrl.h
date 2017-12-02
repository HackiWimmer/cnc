#ifndef CNC_POS_SPY_LIST_CTRL_H
#define CNC_POS_SPY_LIST_CTRL_H

#include "CncLargeScaleListCtrl.h"

class CncPosSpyListCtrl : public CncLargeScaledListCtrl {
	
	private:
		wxString majorPosPid;
		wxListItemAttr majorPosItemAttr;
		
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr *OnGetItemAttr(long item) const;
		
	public:
	
		static const int COL_PID			= 0;
		static const int COL_REF			= 1;
		static const int COL_T				= 2;
		static const int COL_F				= 3;
		static const int COL_X				= 4;
		static const int COL_Y				= 5;
		static const int COL_Z				= 6;
		
		static const int TOTAL_COL_COUNT	= 7;
		
		CncPosSpyListCtrl(wxWindow *parent, long style);
		virtual ~CncPosSpyListCtrl();
};

#endif