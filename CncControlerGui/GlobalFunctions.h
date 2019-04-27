#ifndef GLOBAL_FUNCTIONS_H
#define GLOBAL_FUNCTIONS_H

#include <iostream>
#include <wx/window.h>

class GblFunc {
	private:
	
		static void storeStacktrace(const wxString& st);
	
	public:
		static void replaceControl(wxWindow* oldCtrl, wxWindow* newCtrl);
		
		static const std::ostream& 	stacktraceOnlyApp(std::ostream& o, bool lastOnly);
		static const wxString& 		stacktraceOnlyApp(wxString& ret, bool lastOnly);
		
		static const std::ostream& 	stacktrace(std::ostream& o, int maxLines = -1);
		static const wxString& 		stacktrace(wxString& ret, int maxLines = -1);
		
		static bool getStacktraceFromDatabase(std::ostream& o, unsigned int number);
		static void traceStacktraceDatabase(std::ostream& o);
	
};

#endif