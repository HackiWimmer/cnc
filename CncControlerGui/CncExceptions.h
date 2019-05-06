#ifndef CNC_EXCEPTIONS_H
#define CNC_EXCEPTIONS_H

#include <exception>
#include <wx/string.h>

class CncInterruption : public std::runtime_error {
	
	public:
		explicit CncInterruption (const char* what_arg) 
		: runtime_error(wxString::Format("CncInterruption: %s", what_arg ? what_arg : "")) 
		{}
};

#endif