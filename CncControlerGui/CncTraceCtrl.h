#ifndef CNC_TRACE_CTRL_H
#define CNC_TRACE_CTRL_H

#include <vector>
#include <wx/timer.h>
#include "CncTextCtrl.h"

class CncTraceCtrl : public CncTextCtrl  {
	
	private:
	
		struct TraceEntry {
			wxString timeStamp;
			wxString text;
			
			TraceEntry(const wxString& t, const wxString& s)
			: timeStamp(t)
			, text(s)
			{}
		};
		
		static const int timerInterval = 3000;
		
		typedef std::vector<TraceEntry> Entries;
		Entries			entries;
		wxTimer 		displayTimer;
		
		void 			onDisplayTimer(wxTimerEvent& event);
		
	public:
		CncTraceCtrl(wxWindow *parent, wxWindowID id=wxID_ANY, const wxString &value=wxEmptyString, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
		               long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxTextCtrlNameStr);
		virtual ~CncTraceCtrl();
		
		virtual bool	SetDefaultStyle(const wxTextAttr& style);
		virtual void	AppendChar(char c);
		virtual void	AppendText(const wxString &text);
		void			clearTraceHistory();
		void			clearTrace(const wxString& timeStamp);
		void 			openHistroyView();
		
		wxDECLARE_NO_COPY_CLASS(CncTraceCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif