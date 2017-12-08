#ifndef CNC_STAT_SUM_LIST_CTRL_H
#define CNC_STAT_SUM_LIST_CTRL_H

#include <map>
#include "CncLargeScaleListCtrl.h"

class CncStatisticSummaryListCtrl : public CncLargeScaledListCtrl {
	
	private:
		
		wxListItemAttr defaultItemAttr;
		wxListItemAttr valueItemAttr;
		
		typedef std::map<wxString, long> KeyMap;
		KeyMap keyMap;
		
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr *OnGetItemAttr(long item) const;
		virtual wxListItemAttr* OnGetItemColumnAttr(long item, long column ) const;
		
		void addKeyIntern(const wxString& key, const wxString& content, const wxString& unit);
		
	public:
	
		static const int COL_KEY			= 0;
		static const int COL_CONT			= 1;
		static const int COL_VAL1			= 2;
		static const int COL_VAL2			= 3;
		static const int COL_VAL3			= 4;
		static const int COL_VAL4			= 5;
		static const int COL_UNIT			= 6;
		
		static const int TOTAL_COL_COUNT	= 7;
		
		CncStatisticSummaryListCtrl(wxWindow *parent, long style);
		virtual ~CncStatisticSummaryListCtrl();
		
		void addKey(const wxString& key, const wxString& content, const wxString& unit = _(""));
		void updateValues(const wxString& key, const wxString& v1, const wxString& v2, const wxString& v3, const wxString& v4);
		void resetValues();
};

#endif