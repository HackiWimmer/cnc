#ifndef CNCGAMEPADMENUDLG_H
#define CNCGAMEPADMENUDLG_H

#include "wxCrafterGamePort.h"

class GamepadEvent;

class CncGamepadMenuDlg : public CncGamepadMenuDlgBase {
	
	public:
		CncGamepadMenuDlg(wxWindow* parent);
		virtual ~CncGamepadMenuDlg();
		
		void update(const GamepadEvent* state);
		
	protected:
		virtual void onFunction1(wxCommandEvent& event);
		virtual void onFunction2(wxCommandEvent& event);
		virtual void onFunction3(wxCommandEvent& event);
		virtual void onFunction4(wxCommandEvent& event);
		
	private:
	
		void close();
};

#endif // CNCGAMEPADMENUDLG_H
