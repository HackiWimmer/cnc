#ifndef CNCGAMEPADCONTROLLERSTATE_H
#define CNCGAMEPADCONTROLLERSTATE_H

#include <wx/datetime.h>
#include "wxCrafterGamePort.h"
#include "GamepadThread.h"

class CncGamepadCommadHistoryListCtrl;

class CncGamepadControllerState : public CncGamepadControllerStateBase {
	
	private:
		
		CncGamepadCommadHistoryListCtrl* cmdHistCtrl;
		wxString 		serviceShortName;
		wxString 		serviceLongName;
		
		inline void processTrace(const GamepadEvent& state);
		
	protected:
		virtual void scpDriverInstaller(wxCommandEvent& event);
		virtual void scpMonitor(wxCommandEvent& event);
		virtual void scpSettings(wxCommandEvent& event);
		virtual void onClearHistory(wxCommandEvent& event);
		virtual void clearGamepadServiceTrace(wxCommandEvent& event);
		virtual void queryGamepadService(wxCommandEvent& event);
		virtual void startGamepadService(wxCommandEvent& event);
		virtual void stopGamepadService(wxCommandEvent& event);
		
		void executeCommand(const wxString& cmd);
		void traceCommand(const wxString& cmd);
		
	public:
		CncGamepadControllerState(wxWindow* parent);
		virtual ~CncGamepadControllerState();
		
		CncGamepadCommadHistoryListCtrl* getHistList() const { return cmdHistCtrl; }
		
		void update(const GamepadEvent& state);
		void trace(const wxString& msg);
		bool openTool(const wxString& tool, bool checkToolExists=true);
};
#endif // CNCGAMEPADCONTROLLERSTATE_H
