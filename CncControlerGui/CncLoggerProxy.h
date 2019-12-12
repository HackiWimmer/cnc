#ifndef CNC_LOGGER_PROXY_H
#define CNC_LOGGER_PROXY_H

#include <wx/timer.h>
#include <wx/textctrl.h>
#include "OSD/CncTimeFunctions.h"

class CncTextCtrl : public wxTextCtrl  {
	
	private:
		wxTimer* overflowTimer;
		wxString lineBuffer;
		CncNanoTimestamp lastAppend;
		
		inline void setupTimer(int interval);
		inline size_t flushLineBuffer();
		
	protected:
		virtual void onOverflowTimer(wxTimerEvent& event);
		
	public:
		CncTextCtrl(wxWindow *parent, wxWindowID id=wxID_ANY, const wxString &value=wxEmptyString, 
					const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
					long style=0, const wxValidator &validator=wxDefaultValidator, const wxString 
					&name=wxTextCtrlNameStr);
		virtual ~CncTextCtrl();
		
		int getOverflowInterval()			const { return overflowTimer->GetInterval(); }
		const wxString& getLineBuffer() 	const { return lineBuffer; }
		
		virtual bool SetDefaultStyle(const wxTextAttr& style);
		virtual void AppendChar(char c);
		virtual void AppendText(const wxString &text);
		
		size_t flush();
};


class CncLoggerProxy : public CncTextCtrl  {
	
	protected:
		bool showOnDemandState;
		
		virtual void onKeyDown(wxKeyEvent& event);
		virtual void onLeftDClick(wxMouseEvent& event);
		virtual void onUpdateLogger(wxCommandEvent& event);

	public:
		CncLoggerProxy(wxWindow *parent, wxWindowID id=wxID_ANY, const wxString &value=wxEmptyString, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
		               long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxTextCtrlNameStr);
		virtual ~CncLoggerProxy();
		
		void setShowOnDemandState(bool state) { showOnDemandState = state; }
		
		wxDECLARE_NO_COPY_CLASS(CncLoggerProxy);
		wxDECLARE_EVENT_TABLE();
};

#endif