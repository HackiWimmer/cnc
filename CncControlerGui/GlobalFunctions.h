#ifndef GLOBAL_FUNCTIONS_H
#define GLOBAL_FUNCTIONS_H

#include <iostream>
#include <wx/window.h>
#include "CncCommon.h"

class GblFunc {
	private:
	
		static void storeStacktrace(const wxString& st);
		static bool executeExternalProgram(bool admin, const wxString& tool, const wxString& file, bool checkToolExists);
		
	public:
		static void swapControls(wxWindow* targetCtrl, wxWindow* sourceCtrl);
		static void replaceControl(wxWindow* oldCtrl, wxWindow* newCtrl);
		static void cloneAttributes(wxWindow* source, wxWindow* target);
		static void freeze(wxWindow* wnd, bool state);
		
		static int fixListBookBmpVisibleBug(wxWindow* lb);
		
		static bool replaceSizer(wxSizer* oldSizer, wxSizer* newSizer);

		static const std::ostream& 	stacktraceOnlyApp(std::ostream& o, bool lastOnly);
		static const wxString& 		stacktraceOnlyApp(wxString& ret, bool lastOnly);
		
		static const std::ostream& 	stacktrace(std::ostream& o, int maxLines = -1);
		static const wxString& 		stacktrace(wxString& ret, int maxLines = -1);
		
		static bool getStacktraceFromDatabase(std::ostream& o, unsigned int number);
		static void traceStacktraceDatabase(std::ostream& o);
	
		static void appendToStackTraceFile(const wxString& st);
		static void appendToStackTraceFileTS(const wxString& st);
		
		static bool executeExternalProgram(const wxString& tool, const wxString& file, bool checkToolExists=true);
		static bool executeExternalProgramAdmin(const wxString& tool, const wxString& file, bool checkToolExists=true);
		
};

class GblGuiCtrlDisabler {
	
	private:
		wxWindow* ctrl;
		
	public:
		GblGuiCtrlDisabler(wxWindow* c)
		: ctrl(c)
		{
			if ( ctrl != NULL )
				ctrl->Enable(false);
		}
		
		~GblGuiCtrlDisabler()
		{
			if ( ctrl != NULL )
				ctrl->Enable(true);
		}
};

#define APPEND_THREAD_ID_TO_STACK_TRACE_FILE		GblFunc::appendToStackTraceFileTS(wxString::Format("%s: TID=[%ld]",    CNC_LOG_FUNCT, wxThread::GetCurrentId()));
#define APPEND_THREAD_IDTO_STACK_TRACE_FILE_A(msg)	GblFunc::appendToStackTraceFileTS(wxString::Format("%s: TID=[%ld] %s", CNC_LOG_FUNCT, wxThread::GetCurrentId(), msg));
#define APPEND_LOCATION_TO_STACK_TRACE_FILE 		GblFunc::appendToStackTraceFileTS(CNC_LOG_LOCATION);
#define APPEND_LOCATION_TO_STACK_TRACE_FILE_A(msg) 	GblFunc::appendToStackTraceFileTS(CNC_LOG_LOCATION_A(msg));

#define cncDELETE( p ) { wxDELETE( p ); APPEND_LOCATION_TO_STACK_TRACE_FILE_A("finalized dtor of '"#p"'"); }

#endif