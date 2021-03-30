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
		virtual void onFunction(wxCommandEvent& event);
		
	private:
		
		bool prevUp;
		bool prevDown;
		bool prevLeft;
		bool prevRight;
		
		void close();
};

#endif // CNCGAMEPADMENUDLG_H
