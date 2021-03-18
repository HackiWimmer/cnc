#ifndef CNC_STRING_LOGGER_H
#define CNC_STRING_LOGGER_H

#include <wx/log.h>

class CncStringLogger : public wxLogBuffer {
	
	private:
		wxLog* prevLogger;
	
	public:
		CncStringLogger()
		: wxLogBuffer()
		, prevLogger(wxLog::SetActiveTarget(this))
		{
		}
		
		~CncStringLogger()
		{
			wxLog::SetActiveTarget(prevLogger);
		}
		
		virtual void Flush()
		{
			// overridden to avoid the default message box.
			// the content of this logger can be retrieved by calling
			// GetBuffer() otherwise it is forever lost.
		}
};

#endif