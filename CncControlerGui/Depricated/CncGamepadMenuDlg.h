#ifndef CNCGAMEPADMENUDLG_H
#define CNCGAMEPADMENUDLG_H

#include <vector>
#include "wxCrafterGamePort.h"

class GamepadEvent;

class CncGamepadMenuDlg : public CncGamepadMenuDlgBase {
	
	public:
		CncGamepadMenuDlg(wxWindow* parent);
		virtual ~CncGamepadMenuDlg();
		
		void update(const GamepadEvent* state);
		
	protected:
		virtual void onPodiumManagement(wxCommandEvent& event);
		virtual void onCancel(wxCommandEvent& event);
		virtual void onReferencePosition(wxCommandEvent& event);
		virtual void onRunTemplate(wxCommandEvent& event);
		virtual void onFunction(wxCommandEvent& event);

	private:
		
		bool prevUp;
		bool prevDown;
		bool prevLeft;
		bool prevRight;
		bool prevA;
		bool prevB;
		
		void onSetFocus(wxFocusEvent& event);
		
		void close();
		void highlight(wxWindow* bt);
};

#endif // CNCGAMEPADMENUDLG_H
