#ifndef CNC_VECTIES_LIST_CTRL_H
#define CNC_VECTIES_LIST_CTRL_H

#include "CncLargeScaleListCtrl.h"

class CncVectiesListCtrl : public CncLargeScaledListCtrl {
	
	private:
		
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr *OnGetItemAttr(long item) const;
		
	public:
	
		static const int COL_REF			= 0;
		static const int COL_T				= 1;
		static const int COL_X				= 2;
		static const int COL_Y				= 3;
		static const int COL_Z				= 4;
		
		static const int TOTAL_COL_COUNT	= 5;
		
		CncVectiesListCtrl(wxWindow *parent, long style);
		virtual ~CncVectiesListCtrl();
};

#endif