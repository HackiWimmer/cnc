#ifndef CNC_LOGGER_LIST_CTRL_H
#define CNC_LOGGER_LIST_CTRL_H

#include <vector>
#include <wx/timer.h>
#include "CncCommon.h"
#include "CncLargeScaleListCtrl.h"

class CncLoggerListCtrl : public CncLargeScaledListCtrl {
	
	private:
		
		enum UpdateMode { UM_Immediately, UM_Normal, UM_Slowly };
		
		static wxListItemAttr	defaultItemAttr;
		
		struct LoggerEntry {
			
			public:
				wxString 		text;
				wxString 		result;
				wxListItemAttr	listItemAttr;
				
				LoggerEntry();
		};
		
		typedef std::vector<LoggerEntry> Entries;
		
		Entries					entries;
		UpdateMode				updateMode;
		UpdateMode				updateModePreviously;
		bool					showOnDemand;
		bool					anyUpdate;
		long					selectedItem;
		long					loggedRowNumber;
		wxTimer 				displayTimer;
		int						displayTimerInterval;
		
		void					processUpdateMode();
		void					updateContent();
		void					updateColumnWidth();
		
		void 					onPaint(wxPaintEvent& event);
		void 					onSize(wxSizeEvent& event);
		void 					onDisplayTimer(wxTimerEvent& event);
		void 					onKeyDown(wxKeyEvent& event);
		void 					onLeftDClick(wxMouseEvent& event);
		void 					onRightDown(wxMouseEvent& event);
		void 					onSelectListItem(wxListEvent& event);
		void 					onActivateListItem(wxListEvent& event);
		
		virtual bool 			isItemValid(long item) const;
		virtual int 			OnGetItemColumnImage(long item, long column) const;
		virtual wxString 		OnGetItemText(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		
	public:
		
		static const int 		COL_LNR 			= 0;
		static const int 		COL_TXT 			= 1;
		static const int 		COL_RET 			= 2;
		static const int 		TOTAL_COL_COUNT		= 3;
		
		CncLoggerListCtrl(wxWindow *parent, long style);
		virtual ~CncLoggerListCtrl();
		
		void clearAll();
		
		void setShowOnDemand(bool s) { showOnDemand = s; }
		
		void logRowNumber(long rn=wxNOT_FOUND);
		long getLoggedRowNumber();
		
		void changeTextAttr(const wxTextAttr& ta);
		void changeResult(const wxString& text, long item=wxNOT_FOUND);
		
		void popImmediatelyMode();
		void popProcessMode();
		void pushUpdateMode();
		
		void next();
		void add(const char c);
		void add(const wxString& text);
		
		bool writeToFile(const wxFileName& fn, bool allRows=false);
		bool copyToClipboard(bool allRows=false);
		bool openAsTextView(bool allRows=false);
		
		wxDECLARE_NO_COPY_CLASS(CncLoggerListCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif