#include <fstream>
#include <sstream>
#include <vector>
#include <wx/sizer.h>
#include <wx/tokenzr.h>
#include <wx/listbook.h>
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


bool GblFunc::replaceSizer(wxSizer* oldSizer, wxSizer* newSizer) {
	
	// only a test, currently this cashes
	
	wxASSERT_MSG( oldSizer != NULL, "Old sizer is NULL");
	wxWindow* w1 = oldSizer->GetContainingWindow();
	
	std::cout << "1: "<< (long long)w1 << std::endl;
	std::cout << "2: "<< (long long)w1->GetSizer() << std::endl;
	
	std::vector<wxSizerItem*> vector;
	
	for ( size_t i=0; i< oldSizer->GetItemCount(); i++ ) {
		wxSizerItem* si = oldSizer->GetItem(i);
		vector.push_back(si);
	}
	

	std::cout << oldSizer->GetItemCount()<< std::endl;
	std::cout << "ret: "<< w1->GetSizer()->Replace(oldSizer, newSizer, true) << std::endl;
	//std::cout << oldSizer->GetItemCount()<< std::endl;
	
	for ( auto it = vector.begin(); it != vector.end(); ++it )
		;//newSizer->Add(*it, wxSizerFlags(100).Expand());
	
	return true;
}

///////////////////////////////////////////////////////////////////
int GblFunc::fixListBookBmpVisibleBug(wxWindow* lb) {
//
// Hack to make images visible 
//
///////////////////////////////////////////////////////////////////
	if ( lb == NULL )
		return -1;
	
	wxListbook* listBook = static_cast<wxListbook*>(lb);
	wxImageList* imgListOld = listBook->GetImageList();
	
	const wxSize size = imgListOld->GetSize();
    wxImageList* imgListNew = new wxImageList(size.GetWidth(), size.GetHeight());
	
	const int count = imgListOld->GetImageCount();
	for (int i = 0; i<count; i++ )
		imgListNew->Add(imgListOld->GetBitmap(i));
	
	listBook->AssignImageList(imgListNew);
	
	return count;
}

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
void GblFunc::cloneAttributes(wxWindow* source, wxWindow* target) {
///////////////////////////////////////////////////////////////////
	wxASSERT( source != NULL );
	wxASSERT( target != NULL );
	
	// copy more attributes
	target->SetBackgroundColour(source->GetBackgroundColour());
	target->SetForegroundColour(source->GetForegroundColour());
	target->SetFont(source->GetFont());
	target->SetToolTip(source->GetToolTipText());
}
///////////////////////////////////////////////////////////////////
void GblFunc::freeze(wxWindow* wnd, bool state) {
///////////////////////////////////////////////////////////////////
	wxASSERT( wnd != NULL );
	
	if ( state == true ) 	{ if ( wnd->IsFrozen() == false ) wnd->Freeze(); } 
	else 					{ if ( wnd->IsFrozen() == true  ) wnd->Thaw();   }
}
///////////////////////////////////////////////////////////////////
void GblFunc::appendToStackTraceFile(const wxString& st) {
///////////////////////////////////////////////////////////////////
	std::ofstream ofs (CncFileNameService::getStackTraceFileName().c_str().AsChar(), std::ofstream::app);
	if ( ofs.good() ) 
		ofs << st << std::endl;
	
	ofs.close();
}
///////////////////////////////////////////////////////////////////
void GblFunc::appendToStackTraceFileTS(const wxString& st) {
///////////////////////////////////////////////////////////////////
	wxDateTime ts(wxDateTime::UNow());
	GblFunc::appendToStackTraceFile(wxString::Format("%s.%03u: %s", ts.FormatISOTime(), ts.GetMillisecond(), st));
}
///////////////////////////////////////////////////////////////////
void GblFunc::storeStacktrace(const wxString& st) {
///////////////////////////////////////////////////////////////////
	unsigned int id = StackTrace::Database.size();
	StackTrace::Database.push_back(st);
	
	std::ofstream ofs (CncFileNameService::getStackTraceFileName().c_str().AsChar(), std::ofstream::app);
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
//////////////////////////////////////////////////////////////////
bool GblFunc::executeExternalProgram(bool admin, const wxString& tool, const wxString& file, bool checkToolExists) {
//////////////////////////////////////////////////////////////////
	if ( checkToolExists == true ) {
		if ( wxFileName(tool).Exists() == false ) {
			if ( CncFileNameService::findAbsoluteValidPath(tool).IsEmpty() ) {
				std::cerr << "MainFrame::openFileExtern: Failed:" << std::endl;
				std::cerr << " Can't find tool:" << tool << std::endl;
				return false;
			}
		}
	}
	
	wxString cmd(tool);
	if ( file.IsEmpty() == false ) {
		
		if ( wxFileName(file).Exists() == false ) {
			std::cerr << "MainFrame::openFileExtern: Failed:" << std::endl;
			std::cerr << " Can't open:" << file << std::endl;
			return false;
		}
		
		cmd += " \"";
		cmd += file;
		cmd += "\"";
	}
	
	cnc::trc.logInfoMessage(wxString::Format("Open: %s", cmd));
	
	if ( admin == true ) {
		#ifdef __WXMSW__
		
			wchar_t* wtool = new wchar_t[tool.length() * 2 + 1];
			wchar_t* wfile = new wchar_t[file.length() * 2 + 1];
			
			mbstowcs(wtool, tool.c_str().AsChar(), tool.length() + 1);
			mbstowcs(wfile, file.c_str().AsChar(), file.length() + 1);
			
			ShellExecute(0, L"open", wtool, wfile, 0, SW_SHOW);
			return true;
			
		#endif
	}
	
	wxExecute(cmd, wxEXEC_ASYNC);
	return true;
}

//////////////////////////////////////////////////////////////////
bool GblFunc::executeExternalProgram(const wxString& tool, const wxString& file, bool checkToolExists) {
//////////////////////////////////////////////////////////////////
	return executeExternalProgram(false, tool, file, checkToolExists);
}
//////////////////////////////////////////////////////////////////
bool GblFunc::executeExternalProgramAdmin(const wxString& tool, const wxString& file, bool checkToolExists) {
//////////////////////////////////////////////////////////////////
	return executeExternalProgram(true, tool, file, checkToolExists);
}