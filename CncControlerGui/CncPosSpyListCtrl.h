#ifndef CNC_POS_SPY_LIST_CTRL_H
#define CNC_POS_SPY_LIST_CTRL_H

#include "CncLargeScaleListCtrl.h"

class CncPosSpyListCtrl : public CncLargeScaledListCtrl {
	
	public:
		CncPosSpyListCtrl(wxWindow *parent, long style);
		virtual ~CncPosSpyListCtrl();
		
		virtual int OnGetItemColumnImage(long item, long column) const;
};

#endif