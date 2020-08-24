#ifndef CNCGAMEPADCONTROLLERSTATE_H
#define CNCGAMEPADCONTROLLERSTATE_H

#include "wxCrafterGamePort.h"
#include "GamepadThread.h"

class CncGamepadControllerState : public CncGamepadControllerStateBase {
	typedef CncLinearDirection CLD;
	
	private:
		struct MoveState {
			bool 	moving 	= false;
			CLD 	dx 		= CLD::CncNoneDir;
			CLD 	dy 		= CLD::CncNoneDir;
			CLD 	dz 		= CLD::CncNoneDir;
			
			MoveState()
			: moving(false)
			, dx(CLD::CncNoneDir)
			, dy(CLD::CncNoneDir)
			, dz(CLD::CncNoneDir)
			{}
			
			bool hasX() 	{ return dx != CLD::CncNoneDir;		}
			bool hasY() 	{ return dy != CLD::CncNoneDir;		}
			bool hasZ() 	{ return dz != CLD::CncNoneDir;		}
			bool hasXAndY()	{ return hasX() && hasY();			}
			bool hasXOrY() 	{ return hasX() || hasY();			}
			
			bool isEqual(CLD dx, CLD dy, CLD dz)  { 
				return dx == this->dx && dy == this->dy && dz == this->dz; 
			}
			
			void reset() { 
				moving = false;  
				dx = dy = dz = CLD::CncNoneDir;	
			}
			
			void set(CLD dx, CLD dy, CLD dz)  { 
				this->dx = dx;
				this->dy = dy;
				this->dz = dz; 
			}
		};
		
		bool 			running;
		MoveState 		currentMoveState;
		wxString 		serviceShortName;
		wxString 		serviceLongName;
		
		inline bool isRefPosDlgMode();
		
		inline void processTrace(const GamepadEvent& state);
		inline void processReferencePage(const GamepadEvent& state);
		inline void processPositionControlMode(const GamepadEvent& state);
		inline void processRefPositionDlg(const GamepadEvent& state);
		inline void processPosition(const GamepadEvent& state);
		
	public:
		CncGamepadControllerState(wxWindow* parent);
		virtual ~CncGamepadControllerState();
		
		bool isRunning() { return running; }
		void update(const GamepadEvent& state);
		void trace(const wxString& msg);
		
	protected:
		virtual void clearGamepadServiceTrace(wxCommandEvent& event);
		void executeCommand(const wxString& cmd);
		
		virtual void queryGamepadService(wxCommandEvent& event);
		virtual void startGamepadService(wxCommandEvent& event);
		virtual void stopGamepadService(wxCommandEvent& event);
		
};
#endif // CNCGAMEPADCONTROLLERSTATE_H
