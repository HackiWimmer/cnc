#ifndef CNC_PARSING_SYNOPSIS_TRACE_LIST_CTRL_H
#define CNC_PARSING_SYNOPSIS_TRACE_LIST_CTRL_H

#include "CncLargeScaleListCtrl.h"

class CncParsingSynopsisListCtrl : public CncLargeScaledListCtrl {
	
	private:
		wxListItemAttr defaultItemAttr;
		wxListItemAttr infoItemAttr;
		wxListItemAttr warningItemAttr;
		wxListItemAttr errorItemAttr;
		wxListItemAttr separatorItemAttr;
		
		void updateColumnWidth();
		
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		
	public:
		static const int COL_TYPE			= 0;
		static const int COL_TRACE			= 1;
		
		static const int TOTAL_COL_COUNT	= 2;
		static const int COL_STRECH			= COL_TRACE;
		
		CncParsingSynopsisListCtrl(wxWindow *parent, long style);
		virtual ~CncParsingSynopsisListCtrl();
		
		void addEntry(const char type, const wxString& entry);
		
		void addInfo(const wxString& entry)			{ addEntry('I', entry); }
		void addWarning(const wxString& entry)		{ addEntry('W', entry); }
		void addError(const wxString& entry)		{ addEntry('E', entry); }
		
		void addSeparator(const wxString& entry)	{ addEntry('S', wxString('.', 4 * 64)); }
		
		void onSize(wxSizeEvent& event);
		void onSelectListItem(wxListEvent& event);
		void onActivateListItem(wxListEvent& event);

		void popProcessMode()						{ }
		void pushUpdateMode()						{ }

		bool writeToFile(const wxFileName& fn, bool allRows=false);
		bool copyToClipboard(bool allRows=false);
		bool openAsTextView(bool allRows=false);

		wxDECLARE_NO_COPY_CLASS(CncParsingSynopsisListCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif