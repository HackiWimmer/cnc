#ifndef CNC_AUTO_FREEZER
#define CNC_AUTO_FREEZER

#include <wx/window.h>

class CncAutoFreezer {
	
	protected:
		wxWindow* ctrl;
		
	public:
		CncAutoFreezer(wxWindow* ctrl);
		~CncAutoFreezer();
		
		void traw();
};

#endif