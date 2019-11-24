#ifndef CNCGAMEPORTCONTROLLER_H
#define CNCGAMEPORTCONTROLLER_H

#include "CncGamepadControllerState.h"
#include "wxCrafterMainFrame.h"

class CncGameportController : public CncGameportControllerBase {
	
	public:
		CncGameportController(wxWindow* parent);
		virtual ~CncGameportController();
		
		void trace(const wxString& msg);
		void update(const GamepadEvent& state);
		
	protected:
		virtual void onClose(wxCommandEvent& event);
		
	private:
		CncGamepadControllerState* cncGamepadState;
};

#endif // CNCGAMEPORTCONTROLLER_H
