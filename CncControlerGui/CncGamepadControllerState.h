#ifndef CNCGAMEPADCONTROLLERSTATE_H
#define CNCGAMEPADCONTROLLERSTATE_H

#include "wxCrafterGamePort.h"
#include "GamepadThread.h"

class CncGamepadCommadHistoryListCtrl;

class CncGamepadControllerState : public CncGamepadControllerStateBase {
	typedef CncLinearDirection CLD;
	
	private:
		
		enum MovementState { MS_STOPPED, MS_RUNNING, MS_ERROR };
	
		struct MoveInfo {
			CLD 	dx 		= CLD::CncNoneDir;
			CLD 	dy 		= CLD::CncNoneDir;
			CLD 	dz 		= CLD::CncNoneDir;
			
			MoveInfo()
			: dx(CLD::CncNoneDir)
			, dy(CLD::CncNoneDir)
			, dz(CLD::CncNoneDir)
			{}
			
			bool hasX() 	{ return dx != CLD::CncNoneDir;		}
			bool hasY() 	{ return dy != CLD::CncNoneDir;		}
			bool hasZ() 	{ return dz != CLD::CncNoneDir;		}
			bool hasXAndY()	{ return hasX() && hasY();			}
			bool hasXOrY() 	{ return hasX() || hasY();			}
			
			bool isEqual(CLD dx, CLD dy, CLD dz) const { 
				return dx == this->dx && dy == this->dy && dz == this->dz; 
			}
			
			bool update(CLD dx, CLD dy, CLD dz) { 
				if ( isEqual(dx, dy, dz) == false ) {
					set(dx, dy, dz);
					return true;
				}
				
				return false; 
			}
			
			void reset() { 
				dx = dy = dz = CLD::CncNoneDir;	
			}
			
			void set(CLD dx, CLD dy, CLD dz)  { 
				this->dx = dx;
				this->dy = dy;
				this->dz = dz; 
			}
		};
		
		CncGamepadCommadHistoryListCtrl* cmdHistCtrl;
		MovementState	currentMovementState;
		MoveInfo 		currentMoveInfo;
		wxString 		serviceShortName;
		wxString 		serviceLongName;
		
		inline bool isRefPosDlgMode();
		
		inline void processTrace(const GamepadEvent& state);
		inline void processOpenNavigator(const GamepadEvent& state);
		inline void processPositionControlMode(const GamepadEvent& state);
		inline void processRefPositionDlg(const GamepadEvent& state);
		inline void processStepwiseMovement(const GamepadEvent& state);
		inline void processInteractiveMovement(const GamepadEvent& state);
		
		inline const char* getMovementStateAsString(MovementState s);
		
		void decoarteConnectButton();
		
	protected:
		virtual void onClearHistory(wxCommandEvent& event);
		virtual void onConnectGamepad(wxCommandEvent& event);
		virtual void clearGamepadServiceTrace(wxCommandEvent& event);
		void executeCommand(const wxString& cmd);
		void traceCommand(const wxString& cmd);
		
		virtual void queryGamepadService(wxCommandEvent& event);
		virtual void startGamepadService(wxCommandEvent& event);
		virtual void stopGamepadService(wxCommandEvent& event);
		
	public:
		CncGamepadControllerState(wxWindow* parent);
		virtual ~CncGamepadControllerState();
		
		void update(const GamepadEvent& state);
		void trace(const wxString& msg);
};
#endif // CNCGAMEPADCONTROLLERSTATE_H
