#include <sstream>
#include <wx/sizer.h>
#include <boost/stacktrace.hpp>

#include "GlobalFunctions.h"

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
void GblFunc::stacktrace(std::ostream& o) {
//////////////////////////////////////////////////////////////////
	o << boost::stacktrace::stacktrace(); 
}
//////////////////////////////////////////////////////////////////
const wxString& GblFunc::stacktrace(wxString& ret) {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	stacktrace(ss);
	
	ret.assign(ss.str().c_str());
	return ret;
}