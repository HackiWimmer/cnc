#ifndef GLOBAL_FUNCTIONS_H
#define GLOBAL_FUNCTIONS_H

#include <iostream>
#include <wx/window.h>

class GblFunc {
	
	public:
		static void replaceControl(wxWindow* oldCtrl, wxWindow* newCtrl);
		
		static void stacktrace(std::ostream& o, int maxLines = -1);
		static const wxString& stacktrace(wxString& ret, int maxLines = -1);
		
		static bool getStacktraceFromDatabase(std::ostream& o, unsigned int number);
		static void traceStacktraceDatabase(std::ostream& o);
	
};

#endif