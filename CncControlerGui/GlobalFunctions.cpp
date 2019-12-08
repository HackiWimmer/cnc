#include <fstream>
#include <sstream>
#include <vector>
#include <wx/sizer.h>
#include <wx/tokenzr.h>
#include <boost/stacktrace.hpp>
#include "CncFileNameService.h"
#include "GlobalFunctions.h"

///////////////////////////////////////////////////////////////////
typedef std::vector<wxString> STACKTRACE_DB;
class StackTrace {
	
	public:
		static STACKTRACE_DB Database;
		
		StackTrace() {
		}
		
		~StackTrace() {
			// causes *** glibc detected *** cnc: double free or corruption (!prev): 0x000000000599d230 ***
			// Database.clear();
		}
		
} StackTraceInitializer;

STACKTRACE_DB StackTrace::Database;

///////////////////////////////////////////////////////////////////
void GblFunc::swapControls(wxWindow* targetCtrl, wxWindow* sourceCtrl) {
///////////////////////////////////////////////////////////////////
	wxASSERT( targetCtrl != NULL );
	wxASSERT( sourceCtrl != NULL );
	
	wxWindow* 	targetParent 	= targetCtrl->GetParent();
	wxSizer* 	targetSizer   	= targetCtrl->GetContainingSizer();
	
	wxWindow* 	sourceParent 	= sourceCtrl->GetParent();
	wxSizer* 	sourceSizer   	= sourceCtrl->GetContainingSizer();

	wxASSERT( targetParent != NULL );
	wxASSERT( sourceParent != NULL );
	wxASSERT( sourceSizer  != NULL );
	wxASSERT( targetSizer  != NULL );
	
	wxSizerItem* targetSizerItem = targetSizer->GetItem(targetCtrl);
	wxSizerItem* sourceSizerItem = sourceSizer->GetItem(sourceCtrl);
	
	wxASSERT( targetSizerItem  != NULL );
	wxASSERT( sourceSizerItem  != NULL );
	
	targetSizerItem->AssignWindow(sourceCtrl);
	sourceSizerItem->AssignWindow(targetCtrl);
	
	targetCtrl->Reparent(sourceParent);
	sourceCtrl->Reparent(targetParent);
	
	targetSizer->Layout();
	sourceSizer->Layout();
	
	targetCtrl->SetContainingSizer(sourceSizer);
	sourceCtrl->SetContainingSizer(targetSizer);
}
///////////////////////////////////////////////////////////////////
void GblFunc::replaceControl(wxWindow* oldCtrl, wxWindow* newCtrl) {
///////////////////////////////////////////////////////////////////
	wxASSERT( oldCtrl != NULL );
	wxASSERT( newCtrl != NULL );
	
	wxWindow* parent 	= oldCtrl->GetParent();
	wxSizer* sizer   	= oldCtrl->GetContainingSizer();
	wxSize defSize		= oldCtrl->GetClientSize();
	wxSize minSize		= oldCtrl->GetMinClientSize();
	wxPoint pos			= oldCtrl->GetPosition();
	
	wxASSERT( parent != NULL );
	wxASSERT( sizer  != NULL );
	
	newCtrl->SetPosition(pos);
	newCtrl->SetClientSize(defSize);
	newCtrl->SetMinClientSize(minSize);
	
	newCtrl->Reparent(parent);
	sizer->Replace(oldCtrl, newCtrl, false);
	sizer->Layout();
	
	newCtrl->SetPosition(pos);
	newCtrl->SetClientSize(defSize);
	newCtrl->SetMinClientSize(minSize);
	
	// remove the placeholder
	parent->RemoveChild(oldCtrl);
	oldCtrl->Destroy();
	// do not delete oldCtrl this will be handled by wx... 
}
///////////////////////////////////////////////////////////////////
void GblFunc::appendToStackTraceFile(const wxString& st) {
///////////////////////////////////////////////////////////////////
	std::ofstream ofs (CncFileNameService::getStackTraceFileName(), std::ofstream::app);
	if ( ofs.good() ) {
		ofs << st
			<< std::endl;
	}
	
	ofs.close();
}
///////////////////////////////////////////////////////////////////
void GblFunc::storeStacktrace(const wxString& st) {
///////////////////////////////////////////////////////////////////
	unsigned int id = StackTrace::Database.size();
	StackTrace::Database.push_back(st);
	
	std::ofstream ofs (CncFileNameService::getStackTraceFileName(), std::ofstream::app);
	if ( ofs.good() ) {
		ofs << "Stacktrace #: " << id << std::endl
			<< st
			<< std::endl;
	}
	
	ofs.close();
}
///////////////////////////////////////////////////////////////////
bool GblFunc::getStacktraceFromDatabase(std::ostream& o, unsigned int number) {
///////////////////////////////////////////////////////////////////
	if ( number > StackTrace::Database.size() -1 )
		return false;
		
	o << StackTrace::Database.at(number);
	return true;
}
///////////////////////////////////////////////////////////////////
void GblFunc::traceStacktraceDatabase(std::ostream& o) {
///////////////////////////////////////////////////////////////////
	for ( auto it = StackTrace::Database.begin(); it != StackTrace::Database.end(); ++it ) {
		o << "Stacktrace #: " << std::distance(StackTrace::Database.begin(), it) << std::endl;
		o << *it;
	}
}
///////////////////////////////////////////////////////////////////
const std::ostream& GblFunc::stacktraceOnlyApp(std::ostream& o, bool lastOnly) {
///////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << boost::stacktrace::stacktrace(); 
	
	o << "Stacktrace (App Only)#: " << StackTrace::Database.size() << std::endl;
	storeStacktrace(ss.str());
	
	wxStringTokenizer tokenizer(ss.str().c_str(), "\n");
	int counter = 0;
	while ( tokenizer.HasMoreTokens() ) {
		const wxString token(tokenizer.GetNextToken());
		
		// avoid printout of "main->globalFunctions->stacktrace"
		if ( counter++ < 3 ) {
			if ( token.Contains("stacktrace.hpp") || token.Contains("GlobalFunctions.cpp") || token.Contains("main.cpp") )
				continue;
		}

		if ( token.Matches("* at *:*") ) {
			o << token << std::endl;
			
			if ( lastOnly == true )
				break;
		}
	}
	
	return o;
}
///////////////////////////////////////////////////////////////////
const wxString& GblFunc::stacktraceOnlyApp(wxString& ret, bool lastOnly) {
///////////////////////////////////////////////////////////////////
	std::stringstream ss;
	stacktraceOnlyApp(ss, lastOnly);
	
	ret.assign(ss.str().c_str());
	return ret;
}
///////////////////////////////////////////////////////////////////
const std::ostream& GblFunc::stacktrace(std::ostream& o, int maxLines) {
///////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << boost::stacktrace::stacktrace(); 
	
	o << "Stacktrace #: " << StackTrace::Database.size() << std::endl;
	storeStacktrace(ss.str());
	
	if ( maxLines > 0 ) {
		wxStringTokenizer tokenizer(ss.str().c_str(), "\n");
		int counter = 0;
		while ( tokenizer.HasMoreTokens() ) {
			const wxString token(tokenizer.GetNextToken());
			
			// avoid printout of "main->globalFunctions->stacktrace"
			if ( counter++ < 3 ) {
				if ( token.Contains("stacktrace.hpp") || token.Contains("GlobalFunctions.cpp") || token.Contains("main.cpp") )
					continue;
			}
			
			o << token << std::endl;

			if ( counter >= maxLines ) {
				o << " ..." << std::endl;
				break;
			}
		}
	} 
	else {
		o << ss.str(); 
	}
	
	return o;
}
//////////////////////////////////////////////////////////////////
const wxString& GblFunc::stacktrace(wxString& ret, int maxLines) {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	stacktrace(ss, maxLines);
	
	ret.assign(ss.str().c_str());
	return ret;
}
