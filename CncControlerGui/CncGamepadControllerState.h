#ifndef CNCGAMEPADCONTROLLERSTATE_H
#define CNCGAMEPADCONTROLLERSTATE_H

#include "wxcrafter.h"
#include "GamepadThread.h"

class MainFrame;

class CncGamepadControllerState : public CncGamepadControllerStateBase {
	
	private:
		enum PosCtrlMode { PCM_STICKS = 0, PCM_NAV_XY = 1, PCM_NAV_Z = 2 };
		
		MainFrame* mainFrame;
		PosCtrlMode posCtrlMode;
		bool running;
		
		bool xyNavigationActive;
		bool zNavigationActive;
		
		wxString serviceShortName;
		wxString serviceLongName;
		
		inline bool isRefPosDlgMode();
		
		inline void processTrace(const GamepadEvent& state);
		inline void processReferencePage(const GamepadEvent& state);
		inline void processPositionControlMode(const GamepadEvent& state);
		inline void processRefPositionDlg(const GamepadEvent& state);
		inline void processPosition(const GamepadEvent& state);
		
		inline void managePositionViaStick(const GamepadEvent& state);
		inline void managePositionViaNavi(const GamepadEvent& state);
		inline void mangageMainView(const GamepadEvent& state);
		
	public:
		CncGamepadControllerState(MainFrame* parent);
		virtual ~CncGamepadControllerState();
		
		bool isRunning() { return running; }
		void update(const GamepadEvent& state);

	protected:
		virtual void clearGamepadServiceTrace(wxCommandEvent& event);
		void executeCommand(const wxString& cmd);
		
		virtual void queryGamepadService(wxCommandEvent& event);
		virtual void startGamepadService(wxCommandEvent& event);
		virtual void stopGamepadService(wxCommandEvent& event);
		
};
#endif // CNCGAMEPADCONTROLLERSTATE_H
