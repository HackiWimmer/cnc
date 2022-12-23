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
				wxDateTime		timestamp;
				
				LoggerEntry();
				explicit LoggerEntry(const wxListItemAttr& a);
				LoggerEntry(const wxString& t, const wxListItemAttr& a);
				LoggerEntry(const wxString& t, const wxString& r, const wxListItemAttr& a);
		};
		
		typedef std::vector<LoggerEntry> Entries;
		
		Entries					entries;
		UpdateMode				updateMode;
		UpdateMode				updateModePreviously;
		wxString				currentIndent;
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
		virtual wxListItemAttr* OnGetItemColumnAttr(long item, long column) const; 
		
	protected:
		
		void					tokenAndAdd(const wxString& text, const wxListItemAttr& lia);
		
	public:
		
		static const int 		COL_LNR 			= 0;
		static const int 		COL_TIM 			= 1;
		static const int 		COL_TXT 			= 2;
		static const int 		COL_RET 			= 3;
		static const int 		TOTAL_COL_COUNT		= 4;
		static const int		COL_STRECH			= COL_TXT;
		
		CncLoggerListCtrl(wxWindow *parent, long style);
		virtual ~CncLoggerListCtrl();
		
		virtual void clearAll();
		
		void setJoinTheAppState(bool s)	{ joinTheApp = s; }
		void setShowOnDemand(bool s) 	{ showOnDemand = s; }
		
		long getLoggedRowNumber() const;
		void logRowNumber(long rn = wxNOT_FOUND);
		void logLastFilledRowNumber();
		void logNextRowNumber();
		
		void changeTextAttr(const wxTextAttr& ta);
		void changeTextColour(const wxColour& col);
		
		void changeResult						(const wxString& result, long item = wxNOT_FOUND);
		void changeResultForLoggedPosition		(const wxString& result);
		void changeResultForLastPosition		(const wxString& result);
		void changeResultForLastFilledPosition	(const wxString& result);
		
		void popImmediatelyMode();
		void popProcessMode();
		void pushUpdateMode();
		
		void enable(bool state);
		
		void next();
		void add(const char c);
		void add(const wxString& text);
		void add(const wxString& text, const wxListItemAttr& lia);
		
		void add(const wxString& text, const wxString& result);
		void add(const wxString& text, const wxString& result, const wxListItemAttr& lia);
		
		void addSeparator();
		
		virtual bool writeToFile(const wxFileName& fn, bool allRows=false)		override;
		virtual bool copyToClipboard(bool allRows=false)						override;
		
		void incCurrentIndent();
		void decCurrentIndent();
		void setCurrentIndent(unsigned int i);
		unsigned int getCurrentIndent() const { return currentIndent.length(); }
		
		void displayTimeColumn(bool show);
		
		void forceUpdate();
		
		virtual long getItemCount() const override { return entries.size(); }
		
		wxDECLARE_NO_COPY_CLASS(CncLoggerListCtrl);
		wxDECLARE_EVENT_TABLE();
};

class CncExtLoggerListCtrl : public CncLoggerListCtrl {
	
	protected:
		
		typedef std::vector<wxString> LoggedEntries;
		
		LoggedEntries	loggedInfoEntries;
		LoggedEntries	loggedDebugEntries;
		LoggedEntries	loggedWarnEntries;
		LoggedEntries	loggedErrorEntries;
		
	public:
		
		CncExtLoggerListCtrl(wxWindow *parent, long style)
		: CncLoggerListCtrl(parent, style)
		, loggedInfoEntries		()
		, loggedDebugEntries	()
		, loggedWarnEntries		()
		, loggedErrorEntries	()
		{}
		
		virtual ~CncExtLoggerListCtrl()
		{}
		
		bool hasInfoEntries()		const	{ return loggedInfoEntries.size() > 0; }
		bool hasNonInfoEntries()	const	{ return hasDebugEntries() == true || hasWarnEntries() == true || hasErrorEntries() == true; }
		bool hasDebugEntries()		const	{ return loggedDebugEntries.size() > 0; }
		bool hasWarnEntries()		const	{ return loggedWarnEntries.size() > 0;  }
		bool hasErrorEntries()		const 	{ return loggedErrorEntries.size() > 0; }
		
		void traceInfoEntries(std::ostream& ostr);
		void traceDebugEntries(std::ostream& ostr);
		void traceWarnEntries(std::ostream& ostr);
		void traceErrorEntries(std::ostream& ostr);
		
		virtual void clearAll()
		{
			loggedInfoEntries.clear();
			loggedDebugEntries.clear();
			loggedWarnEntries.clear();
			loggedErrorEntries.clear();

			CncLoggerListCtrl::clearAll();
		}
		
		typedef wxListItemAttr LIA;
		void addSeparator	(const wxString& s) { const LIA lia(wxColour(255, 255, 255), wxColour(185, 122,  87),	GetFont()); tokenAndAdd(s, lia); }
		void addInfoEntry	(const wxString& s) { const LIA lia(GetTextColour(),         GetBackgroundColour(),		GetFont()); add(s, lia); loggedInfoEntries.push_back(s); }
		void addDebugEntry	(const wxString& s) { const LIA lia(wxColour(128, 128,   0), GetBackgroundColour(),		GetFont()); add(s, lia); loggedDebugEntries.push_back(s); }
		void addWarnEntry	(const wxString& s) { const LIA lia(wxColour(255, 201,  14), GetBackgroundColour(),		GetFont()); add(s, lia); loggedWarnEntries.push_back(s); }
		void addErrorEntry	(const wxString& s) { const LIA lia(wxColour(255, 128, 128), GetBackgroundColour(),		GetFont()); add(s, lia); loggedErrorEntries.push_back(s); }
};

#endif