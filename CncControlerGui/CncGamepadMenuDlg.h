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
		virtual void onPodestManagement(wxCommandEvent& event);
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
		
		void close();
};

#endif // CNCGAMEPADMENUDLG_H
