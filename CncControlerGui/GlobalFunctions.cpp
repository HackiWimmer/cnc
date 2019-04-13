#include <sstream>
#include <vector>
#include <wx/sizer.h>
#include <wx/tokenzr.h>
#include <boost/stacktrace.hpp>
#include "GlobalFunctions.h"

///////////////////////////////////////////////////////////////////
typedef std::vector<wxString> STACKTRACE_DB;
class StackTrace {
	
	public:
		static STACKTRACE_DB Database;
		
		StackTrace() {
		}
		
		~StackTrace() {
			Database.clear();
		}
		
} StackTraceInitializer;

STACKTRACE_DB StackTrace::Database;

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
	oldCtrl->Destroy();
	// do not delete oldCtrl this will be handled by wx... 
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
void GblFunc::stacktrace(std::ostream& o, int maxLines) {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << boost::stacktrace::stacktrace(); 
	
	o << "Stacktrace #: " << StackTrace::Database.size() << std::endl;
	StackTrace::Database.push_back(ss.str());
	
	if ( maxLines > 0 ) {
		wxStringTokenizer tokenizer(ss.str().c_str(), "\n");
		int counter = 0;
		while ( tokenizer.HasMoreTokens() ) {
			o << tokenizer.GetNextToken() << std::endl;
			
			if ( ++counter >= maxLines ) {
				o << " ..." << std::endl;
				break;
			}
		}
	} 
	else {
		o << ss.str(); 
	}
}
//////////////////////////////////////////////////////////////////
const wxString& GblFunc::stacktrace(wxString& ret, int maxLines) {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	stacktrace(ss, maxLines);
	
	ret.assign(ss.str().c_str());
	return ret;
}