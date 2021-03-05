#ifndef CNCGAMEPADSPY_H
#define CNCGAMEPADSPY_H

#include "CncCommon.h"
#include "wxCrafterGamePort.h"

class GamepadEvent;
class CncGamepadCommadHistoryListCtrl;
class CncGamepadDirectionPanel;

class CncGamepadSpy : public CncGamepadSpyBase {
	
	public:
		enum Context { GPC_MAIN, GPC_REFPOS, GPC_MSGBOX };
		
		class ContextSwaper {
			
			private:
				CncGamepadSpy*	spy;
				Context			prev;
				
			public:
				ContextSwaper(CncGamepadSpy* s, const Context c) 
				: spy	(s)
				, prev	(c)
				{ 
					spy = s; 
					if ( spy != NULL ) {
						spy->setContext(c);
					}
				}
				
				~ContextSwaper() {
					if ( spy != NULL ) {
						spy->setContext(prev);
					}
				}
		};
		
		CncGamepadSpy(wxWindow* parent);
		virtual ~CncGamepadSpy();
		
		void setCondHistControl(CncGamepadCommadHistoryListCtrl* ctrl) 
			{ cmdHistCtrl = ctrl; }
		
		const Context getContext() const { return currContext; }
		void setContext(const Context c) { currContext = c; }
		
		void setActivationState(bool state);
		void update(const GamepadEvent* state);
		
	private:
		
		typedef CncLinearDirection CLD;
		
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

		static const int MODE_GNC = 0;
		static const int MODE_GUI = 1;
		static const int MODE_CNC = 2;
		
		Context								currContext;
		CncGamepadDirectionPanel*			dirXY;
		CncGamepadDirectionPanel*			dirZ;
		CncGamepadCommadHistoryListCtrl*	cmdHistCtrl;
		MovementState						currentMovementState;
		MoveInfo 							currentMoveInfo;

		bool								activated;
		wxString							keyInfo;
		wxString							mouseInfo;
		
		void updateModeText(const wxString& msg);
		void updateGuiMode(const GamepadEvent* state);
		void updateCncMode(const GamepadEvent* state);
		
		void releaseKeyboardActions(const GamepadEvent* state);
		void releaseMouseActions(const GamepadEvent* state);
		void releaseCncActions(const GamepadEvent* state);
		
		void processStepwiseMovement(const GamepadEvent& state);
		void processInteractiveMovement(const GamepadEvent& state);
};

#endif // CNCGAMEPADSPY_H
