#ifndef CNC_INFO_BAR_H
#define CNC_INFO_BAR_H

#include <wx/timer.h>
#include <wx/infobar.h>

class CncInfoBar : public wxInfoBar {
	
	private:
		bool			addTimeStampPrefix;
		wxTimer			displayTimer;

		void			onDisplayTimer(wxTimerEvent& event);
		
	protected:
		wxString		appendix;
		virtual void	traceFurther(const char type, const wxString& msg) = 0;
		virtual void	onLeftDClick(wxMouseEvent& event) = 0;

	public:
		CncInfoBar(wxWindow *parent);
		virtual ~CncInfoBar();
		
		void doAddTimeStampPrefix(bool state)		{ addTimeStampPrefix = state; }
		void setAppendix(const wxString& a)			{ appendix = a; }
		
		void showInformation(const wxString& msg)	{ showMessage('I', msg); }
		void showWaraning(const wxString& msg)		{ showMessage('W', msg); }
		void showError(const wxString& msg)			{ showMessage('E', msg); }
		
		void showMessage(const char type, const wxString& msg);
		virtual void ShowMessage(const wxString& msg, int flags = wxICON_INFORMATION) wxOVERRIDE;
		
		wxDECLARE_NO_COPY_CLASS(CncInfoBar);
		wxDECLARE_EVENT_TABLE();
};

class CncMainInfoBar : public CncInfoBar {

	protected:
		virtual void	traceFurther(const char type, const wxString& msg);
		virtual void	onLeftDClick(wxMouseEvent& event);

	public:
		CncMainInfoBar(wxWindow *parent)
		: CncInfoBar(parent)
		{
			appendix.assign("\n[Please also visit the trace history for more information . . . ]");
		}
		
		virtual ~CncMainInfoBar()
		{}
};

#endif
