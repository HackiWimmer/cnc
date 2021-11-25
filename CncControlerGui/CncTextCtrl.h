#ifndef CNC_TEXT_CTRIL_H
#define CNC_TEXT_CTRIL_H

#include <wx/timer.h>
#include <wx/textctrl.h>

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
		
		virtual bool setTextColour(const wxColour& col);
		virtual void appendChar(char c, const wxColour& col, int sourceId);
		virtual void appendChar(char c, const wxTextAttr& style, int sourceId);
		
		virtual bool SetDefaultStyle(const wxTextAttr& style);
		virtual void AppendChar(char c);
		virtual void AppendText(const wxString &text);
		
		void logCurrentPosition();
		long getLoggedPosition();
		bool isLoggedPositionEqualCurrent();
		bool skipBackIfLoggedPositionEqualCurrent();
		
		size_t flush();
};

#endif