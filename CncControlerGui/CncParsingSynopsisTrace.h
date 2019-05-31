#ifndef CNCPARSINGSYNOPSISTRACE_H
#define CNCPARSINGSYNOPSISTRACE_H

#include "wxcrafter.h"
#include "CncParsingSynopsisTraceListCtrl.h"

class CncParsingSynopsisTrace : public CncParsingSynopsisTraceBase {
	private:
		CncParsingSynopsisListCtrl* synopsisTrace;
		
	public:
		CncParsingSynopsisTrace(wxWindow* parent);
		virtual ~CncParsingSynopsisTrace();
		
		void clear() 											{ synopsisTrace->clear(); addInfo("Trace cleared . . . "); }
		
		void addEntry(const char type, const wxString& entry) 	{ synopsisTrace->addEntry(type, entry); }
		void addInfo(const wxString& entry)						{ synopsisTrace->addInfo(entry); }
		void addWarning(const wxString& entry)					{ synopsisTrace->addWarning(entry); }
		void addError(const wxString& entry)					{ synopsisTrace->addError(entry); }
		void addSeparator(const wxString& entry="")				{ synopsisTrace->addSeparator(entry); }
};
#endif // CNCPARSINGSYNOPSISTRACE_H
