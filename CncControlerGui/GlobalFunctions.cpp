#include <boost/stacktrace.hpp>
#include <wx/sizer.h>
#include "GlobalFunctions.h"

///////////////////////////////////////////////////////////////////
void GblFunc::replaceControl(wxWindow* oldCtrl, wxWindow* newCtrl) {
///////////////////////////////////////////////////////////////////
	
	wxASSERT( oldCtrl != NULL );
	wxASSERT( newCtrl != NULL );
	
	wxWindow* parent = oldCtrl->GetParent();
	wxSizer* sizer   = oldCtrl->GetContainingSizer();
	
	wxASSERT( parent != NULL );
	wxASSERT( sizer != NULL );
	
	newCtrl->Reparent(parent);
	newCtrl->SetPosition(oldCtrl->GetPosition());
	
	sizer->Replace(oldCtrl, newCtrl, true);
	sizer->Layout();
	
	// remove the placeholder
	oldCtrl->Destroy();
	// do not delete oldCtrl this will be handled by wx... 
}

///////////////////////////////////////////////////////////////////
static void GblFunc::stacktrace(std::ostream& o) {
//////////////////////////////////////////////////////////////////
	o << boost::stacktrace::stacktrace();
}
