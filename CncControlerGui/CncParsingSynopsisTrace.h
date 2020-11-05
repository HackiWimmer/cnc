#ifndef CNCPARSINGSYNOPSISTRACE_H
#define CNCPARSINGSYNOPSISTRACE_H

#include "wxCrafterMotionMonitor.h"
#include "CncLoggerListCtrl.h"

// ----------------------------------------------------------------------------
class CncParsingSynopsisListCtrl : public CncExtLoggerListCtrl {
	public:
		
		CncParsingSynopsisListCtrl(wxWindow *parent, long style)
		: CncExtLoggerListCtrl(parent, style)
		{}

		virtual ~CncParsingSynopsisListCtrl()
		{}
		
		typedef wxListItemAttr LIA;
		//void addMovSeqSep	(const wxString& s) { const LIA lia(*wxBLACK, wxColour(128, 128, 255), GetFont()); add(s, lia); }
		//void addPthLstSep	(const wxString& s) { const LIA lia(*wxBLACK, wxColour(255, 140, 198), GetFont()); add(s, lia); }
};

// ----------------------------------------------------------------------------
class CncParsingSynopsisTrace : public CncParsingSynopsisTraceBase {
	private:
		CncParsingSynopsisListCtrl* synopsisTrace;
		
	public:
		CncParsingSynopsisTrace(wxWindow* parent);
		virtual ~CncParsingSynopsisTrace();
		
		void clearAll()												{ synopsisTrace->clearAll(); addInfo("Trace cleared . . . "); }
		
		void addEntry(const char type, const wxString& entry);
		void addInfo(const wxString& entry)							{ addEntry('I', entry); }
		void addWarning(const wxString& entry)						{ addEntry('W', entry); }
		void addError(const wxString& entry)						{ addEntry('E', entry); }
		void addSeparator(const wxString& entry="")					{ addEntry('S', entry); }
		
		void popProcessMode()										{ synopsisTrace->popProcessMode(); }
		void pushUpdateMode()										{ synopsisTrace->pushUpdateMode(); }

		bool writeToFile(const wxFileName& fn, bool allRows=false)	{ return synopsisTrace->writeToFile(fn, allRows); }
		bool copyToClipboard(bool allRows=false)					{ return synopsisTrace->copyToClipboard(allRows); }
		bool openAsTextView(bool allRows=false)						{ return synopsisTrace->openAsTextView(allRows); }

};
#endif // CNCPARSINGSYNOPSISTRACE_H
