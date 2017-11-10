#ifndef INTERRUPT_THREAD_H
#define INTERRUPT_THREAD_H

#include <wx/thread.h>
#include "CncCommon.h"
#include "ConcurrentQueue.h"
#include "3D/VerticeData.h"
#include "CncPosition.h"

class MainFrame;
class UpdateManagerThread : public wxThread {
	
	public:
		enum SpeedMode{UNDEFINED = '\0', RAPID = 'R', WORK = 'W'};
		
		struct Event{
			enum Type { EMPTY_UPD, 
			
						QUEUE_RESET, 
						POSSPY_RESET,
						Z_VIEW_RESET,
						
						CONFIG_UPD,
						COMMAND_UPD, 
						APP_POS_UPD, 
						CTL_POS_UPD,
						Z_VIEW_UPD,
						SPEED_UPD,
						
						SETTER_ADD
					};
			
			Type type      = EMPTY_UPD;
			bool processed = false;
			
			//////////////////////////////////////////////////////////////
			// no data
			
				inline const Event& ConfigUpdateEvent() {
					type          = CONFIG_UPD;
					processed     = false;
					return *this;
				}
				
			//////////////////////////////////////////////////////////////
			// no data
			
				inline const Event& QueueResetEvent() {
					type         = QUEUE_RESET;
					processed    = false;
					return *this;
				}
				
			//////////////////////////////////////////////////////////////
			// no data
			
				inline const Event& ZViewResetEvent() {
					type         = Z_VIEW_RESET;
					processed    = false;
					return *this;
				}

				inline const Event& ZViewUpdateEvent() {
					type         = Z_VIEW_UPD;
					processed    = false;
					return *this;
				}
				
			//////////////////////////////////////////////////////////////
			struct Spd {
				unsigned int xSpeed = 1; 
				unsigned int ySpeed = 1; 
				unsigned int zSpeed = 1; 
			} spd;
			
				inline const Event& SpeedEvent(unsigned int xs, unsigned int ys, unsigned int zs) {
					type         = SPEED_UPD;
					processed    = false;
					spd.xSpeed = xs; 
					spd.ySpeed = ys;
					spd.zSpeed = zs;
					return *this;
				}
			
			//////////////////////////////////////////////////////////////
			struct Set {
				unsigned char	id; 
				int32_t			value;
			} set;
			
				inline const Event& SetterEvent(unsigned char i, int32_t v) {
					type         = SETTER_ADD;
					processed    = false;
					set.id       = i;
					set.value    = v;
					return *this;
				}
			
			//////////////////////////////////////////////////////////////
			struct Cmd {
				long counter   = 0;
				long duration  = 0;;
			} cmd;
			
				inline const Event& CommandEvent(long c, long d) {
					type         = COMMAND_UPD;
					processed    = false;
					cmd.counter  = c;
					cmd.duration = d;
					return *this;
				}
			
			//////////////////////////////////////////////////////////////
			// no data
			
				inline const Event& PosSpyResetEvent() {
					type			= POSSPY_RESET;
					processed    	= false;
					return *this;
				}
			
			//////////////////////////////////////////////////////////////
			struct Pos {
				long id					= -1;
				SpeedMode speedMode		= UNDEFINED;
				CncLongPosition curr	= {0, 0, 0};
			} pos;
			
				inline const Event& CtlPosEvent(const CncLongPosition& p) {
					type			= CTL_POS_UPD;
					processed    	= false;
					pos.id			= -1;
					pos.speedMode	= UNDEFINED;
					pos.curr.set(p);
					return *this;
				}
				
				inline const Event& AppPosEvent(const CncLongPosition& p) {
					type			= APP_POS_UPD;
					processed    	= false;
					pos.id			= -1;
					pos.speedMode	= UNDEFINED;
					pos.curr.set(p);
					return *this;
				}
				
				inline const Event& AppPosEvent(long i, SpeedMode s, const CncLongPosition& p) {
					type			= APP_POS_UPD;
					processed    	= false;
					pos.id			= i;
					pos.speedMode	= s;
					pos.curr.set(p);
					return *this;
				}
				
				inline const Event& AppPosEvent(long i, const wxString& s, const CncLongPosition& p) {
					type			= APP_POS_UPD;
					processed    	= false;
					pos.id			= i;
					switch ( (char)s[0] ) {
						case 'R':	pos.speedMode	= RAPID; break;
						case 'W':	pos.speedMode	= WORK;  break;
						default:	pos.speedMode	= UNDEFINED;
					}
					pos.curr.set(p);
					return *this;
				}
				
			//////////////////////////////////////////////////////////////
			
		};
		
		UpdateManagerThread(MainFrame *handler);
		~UpdateManagerThread();
		
		// thread interface
		void stop();
		void postEvent(const UpdateManagerThread::Event& evt);
		
	protected:
		const unsigned int maxSetterEntries = 1000;
		
		MainFrame* pHandler;
		bool queueReset;
		bool exit;
		
		double displayFactX;
		double displayFactY;
		double displayFactZ;
		
		ConcurrentQueue<UpdateManagerThread::Event> eventQueue;
		UpdateManagerThread::Event lastCmdEvent;
		UpdateManagerThread::Event lastSpeedEvent;
		UpdateManagerThread::Event lastAppPosEvent;
		UpdateManagerThread::Event lastCtlPosEvent;
		
		virtual ExitCode Entry();
		
	private:
		inline void pop();
		inline void postHeartbeat();
		
		inline void idle();
		inline void freezeControl(wxWindow* ctl, bool onlyHidden);
		inline void thawControl(wxWindow* ctl);
		inline void freezeControls(bool state);
		
		inline void checkQueueReset();
		
		inline void immediateUpdate();
		
		inline void updateCmdInfo();
		inline void updateAppPosition();
		inline void updateCtlPosition();
		inline void updatePositionSpy(UpdateManagerThread::Event evt);
		inline void clearPositionSpy();
		
		inline void resetZView();
		inline void updateZView();
		
		inline void updateSpeedView();
		
		inline void updateSetterList(UpdateManagerThread::Event evt);
		
		inline void configUpdate();
};

#endif