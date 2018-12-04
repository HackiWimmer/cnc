#ifndef GLOBAL_FUNCTIONS_H
#define GLOBAL_FUNCTIONS_H

#include <iostream>
#include <wx/window.h>

class GblFunc {
	
	public:
		static void replaceControl(wxWindow* oldCtrl, wxWindow* newCtrl);
		static void stacktrace(std::ostream& o);
	
};

#endif