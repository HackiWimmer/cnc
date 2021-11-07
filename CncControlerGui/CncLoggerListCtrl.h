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
				LoggerEntry(const wxString& t, const wxString& r, const wxListItemAttr& a);
		};
		
		typedef std::vector<LoggerEntry> Entries;
		
		Entries					entries;
		UpdateMode				updateMode;
		UpdateMode				updateModePreviously;
		bool					canScroll;
		bool					sizeChanged;
		bool					joinTheApp;
		bool					showOnDemand;
		bool					anyUpdate;
		long					selectedItem;
		long					loggedRowNumber;
		wxTimer 				displayTimer;
		int						displayTimerInterval;
		
		void					processUpdateMode();
		void					updateContent();
		
		void 					onPaint(wxPaintEvent& event);
		void 					onSize(wxSizeEvent& event);
		void 					onDisplayTimer(wxTimerEvent& event);
		void 					onKeyDown(wxKeyEvent& event);
		void 					onLeftDClick(wxMouseEvent& event);
		void 					onRightDown(wxMouseEvent& event);
		void 					onSelectListItem(wxListEvent& event);
		void 					onActivateListItem(wxListEvent& event);
		
		void 					onScroll(wxScrollWinEvent& event);
		
		virtual bool 			isItemValid(long item) const;
		virtual int 			OnGetItemColumnImage(long item, long column) const;
		virtual wxString 		OnGetItemText(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		
	protected:
		
		void					tokenAndAdd(const wxString& text, const wxListItemAttr& lia);
		
	public:
		
		static const int 		COL_LNR 			= 0;
		static const int 		COL_TXT 			= 1;
		static const int 		COL_RET 			= 2;
		static const int 		TOTAL_COL_COUNT		= 3;
		static const int		COL_STRECH			= COL_TXT;
		
		CncLoggerListCtrl(wxWindow *parent, long style);
		virtual ~CncLoggerListCtrl();
		
		virtual void clearAll();
		
		void setJoinTheAppState(bool s)	{ joinTheApp = s; }
		void setShowOnDemand(bool s) 	{ showOnDemand = s; }
		
		void logRowNumber(long rn=wxNOT_FOUND);
		long getLoggedRowNumber();
		
		void changeTextAttr(const wxTextAttr& ta);
		void changeResult(const wxString& text, long item=wxNOT_FOUND);
		
		void popImmediatelyMode();
		void popProcessMode();
		void pushUpdateMode();
		
		void enable(bool state);
		
		void next();
		void add(const char c);
		void add(const wxString& text);
		
		void add(const wxString& text, const wxListItemAttr& lia);
		
		bool writeToFile(const wxFileName& fn, bool allRows=false);
		bool copyToClipboard(bool allRows=false);
		bool openAsTextView(bool allRows=false);
		
		wxDECLARE_NO_COPY_CLASS(CncLoggerListCtrl);
		wxDECLARE_EVENT_TABLE();
};

class CncExtLoggerListCtrl : public CncLoggerListCtrl {
	
	protected:
		bool bDebugEntries;
		bool bWarnEntries;
		bool bErrorEntries;
	
	public:
		
		CncExtLoggerListCtrl(wxWindow *parent, long style)
		: CncLoggerListCtrl(parent, style)
		, bDebugEntries		(false)
		, bWarnEntries		(false)
		, bErrorEntries		(false)
		{}
		
		virtual ~CncExtLoggerListCtrl()
		{}
		
		bool hasDebugEntries()	const	{ return bDebugEntries; }
		bool hasWarnEntries()	const	{ return bWarnEntries;  }
		bool hasErrorEntries()	const 	{ return bErrorEntries; }
		
		virtual void clearAll() {
			bDebugEntries	=  false;
			bWarnEntries	=  false;
			bErrorEntries	=  false;
			CncLoggerListCtrl::clearAll();
		}
		
		typedef wxListItemAttr LIA;
		void addSeparator	(const wxString& s) { const LIA lia(wxColour(255, 255, 255), wxColour(185, 122,  87),	GetFont()); tokenAndAdd(s, lia); }
		void addInfoEntry	(const wxString& s) { const LIA lia(GetTextColour(),         GetBackgroundColour(),		GetFont()); add(s, lia); }
		void addDebugEntry	(const wxString& s) { const LIA lia(wxColour(128, 128,   0), GetBackgroundColour(),		GetFont()); add(s, lia); bDebugEntries = true; }
		void addWarnEntry	(const wxString& s) { const LIA lia(wxColour(255, 201,  14), GetBackgroundColour(),		GetFont()); add(s, lia); bWarnEntries  = true; }
		void addErrorEntry	(const wxString& s) { const LIA lia(wxColour(255, 128, 128), GetBackgroundColour(),		GetFont()); add(s, lia); bErrorEntries = true; }
};

#endif