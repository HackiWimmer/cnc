#include <wx/sizer.h>
#include "GlobalFunctions.h"

#include <boost/stacktrace.hpp>

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
	o << "boost::stacktrace::stacktrace()";
}