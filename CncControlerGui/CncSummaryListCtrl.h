#ifndef CNC_SUMMARY_LIST_CTRL_H
#define CNC_SUMMARY_LIST_CTRL_H

#include "CncLargeScaleListCtrl.h"

class CncSummaryListCtrl : public CncLargeScaledListCtrl {
	
	private:
		
		wxListItemAttr defaultItemAttr;
		wxListItemAttr headlineItemAttr;
		
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr *OnGetItemAttr(long item) const;
		
	public:
		
		enum ParameterType { PT_HEADLINE = 0, PT_COMMON = 1, PT_GCODE = 2, PT_SVG = 3 };
		
		static const int COL_TYPE 			= 0;
		static const int COL_PARAM			= 1;
		static const int COL_VALUE			= 2;
		static const int COL_UNIT			= 3;
		
		static const int TOTAL_COL_COUNT	= 4;
		
		CncSummaryListCtrl(wxWindow *parent, long style);
		virtual ~CncSummaryListCtrl();
		
		void addHeadline(ParameterType pt, const wxString& text);
		void addParameter(ParameterType pt, const wxString& param, const wxVariant& value, const wxString& unit = _("-"));
		
		void onSize(wxSizeEvent& event);
		
		wxDECLARE_NO_COPY_CLASS(CncSummaryListCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif