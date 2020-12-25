#ifndef CNC_TRACE_CTRL_H
#define CNC_TRACE_CTRL_H

#include <vector>
#include <wx/timer.h>
#include "CncTextCtrl.h"
#include "CncInfoBar.h"

class CncTraceInfoBar : public CncInfoBar {

	protected:
		
		virtual void	traceFurther(const char type, const wxString& msg) {}
		virtual void	notifyDisplayTimer();
		virtual void	onLeftDClick(wxMouseEvent& event);

	public:
		CncTraceInfoBar(wxWindow *parent)
		: CncInfoBar	(parent)
		{}
		
		virtual ~CncTraceInfoBar()
		{}
		
		virtual void showMessage(const char type, const wxString& msg);
};


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
		
		static const int timerInterval = 1000;
		
		typedef std::vector<TraceEntry> Entries;
		Entries			entries;
		wxTimer 		clearTimer;
		
		void 			onClearTimer(wxTimerEvent& event);
		
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