#ifndef CNC_LOGGER_PROXY_H
#define CNC_LOGGER_PROXY_H

#include <wx/timer.h>
#include <wx/textctrl.h>
#include "OSD/CncTimeFunctions.h"

class CncTextCtrl : public wxTextCtrl  {
	
	private:
		static const unsigned int MAX_LINE_BUFFER_SIZE 				= 1024;
		static const unsigned int DEFAULT_OVERFLOW_PERIOD_VALUE 	=  800;

		wxTimer* 			overflowTimer;
		char				lineBuffer[MAX_LINE_BUFFER_SIZE];
		unsigned int		index;
		long 				loggedPos;
		
		inline size_t flushLineBuffer();
		
	protected:
		virtual void onOverflowTimer(wxTimerEvent& event);
		
	public:
		CncTextCtrl(wxWindow *parent, wxWindowID id=wxID_ANY, const wxString &value=wxEmptyString, 
					const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
					long style=0, const wxValidator &validator=wxDefaultValidator, const wxString 
					&name=wxTextCtrlNameStr);
		virtual ~CncTextCtrl();
		
		virtual bool SetDefaultStyle(const wxTextAttr& style);
		virtual void AppendChar(char c);
		virtual void AppendText(const wxString &text);
		
		void logCurrentPosition();
		long getLoggedPosition();
		bool isLoggedPositionEqualCurrent();
		bool skipBackIfLoggedPositionEqualCurrent();
		
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