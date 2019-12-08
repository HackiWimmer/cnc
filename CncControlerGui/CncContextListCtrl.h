#ifndef CNC_CONTEXT_LIST_CTRL_H
#define CNC_CONTEXT_LIST_CTRL_H

#include <wx/textctrl.h>
#include "CncLargeScaleListCtrl.h"

class CncContextListCtrl : public CncLargeScaledListCtrl {
	
	private:
		
		wxTextCtrl* keyControl;
		wxTextCtrl* valControl;
		
		void updateColumnWidth();
		
		void onSize(wxSizeEvent& event);
		void onSelectListItem(wxListEvent& event);
		
		virtual wxString OnGetItemText(long item, long column) const;
		
	public:

		static const int COL_KEY 			=  0;
		static const int COL_VAL 			=  1;
		static const int TOTAL_COL_COUNT	=  2;
		
		static const int COL_STRECH			= COL_VAL;
		
		static const long MAX_ITEM_COUNT	= 100;
		
		CncContextListCtrl(wxWindow *parent, long style);
		virtual ~CncContextListCtrl();
		
		void setControls(wxTextCtrl* key, wxTextCtrl* val);
		
		wxDECLARE_NO_COPY_CLASS(CncContextListCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif