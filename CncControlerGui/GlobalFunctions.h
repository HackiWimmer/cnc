#ifndef GLOBAL_FUNCTIONS_H
#define GLOBAL_FUNCTIONS_H

#include <iostream>
#include <wx/window.h>
#include "CncCommon.h"

class GblFunc {
	private:
	
		static void storeStacktrace(const wxString& st);
	
	public:
		static void swapControls(wxWindow* targetCtrl, wxWindow* sourceCtrl);
		static void replaceControl(wxWindow* oldCtrl, wxWindow* newCtrl);
		static void cloneAttributes(wxWindow* source, wxWindow* target);
		
		static const std::ostream& 	stacktraceOnlyApp(std::ostream& o, bool lastOnly);
		static const wxString& 		stacktraceOnlyApp(wxString& ret, bool lastOnly);
		
		static const std::ostream& 	stacktrace(std::ostream& o, int maxLines = -1);
		static const wxString& 		stacktrace(wxString& ret, int maxLines = -1);
		
		static bool getStacktraceFromDatabase(std::ostream& o, unsigned int number);
		static void traceStacktraceDatabase(std::ostream& o);
	
		static void appendToStackTraceFile(const wxString& st);
};

#define APPEND_LOCATION_TO_STACK_TRACE_FILE 		GblFunc::appendToStackTraceFile(CNC_LOG_LOCATION);
#define APPEND_LOCATION_TO_STACK_TRACE_FILE_A(msg) 	GblFunc::appendToStackTraceFile(CNC_LOG_LOCATION_A(msg));

#endif