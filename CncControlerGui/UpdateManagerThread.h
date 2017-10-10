#ifndef INTERRUPT_THREAD_H
#define INTERRUPT_THREAD_H

#include <wx/thread.h>
#include "CncCommon.h"
#include "ConcurrentQueue.h"
#include "3D/VerticeData.h"
#include "CncPosition.h"

typedef std::pair<unsigned char, int32_t> SetterPair;
typedef std::vector<SetterPair> SetterList;

class MainFrame;
class UpdateManagerThread : public wxThread {
	
	public:
		enum SpeedMode{UNDEFINED, RAPID, WORK};
		
		struct Event{
			enum Type { EMPTY_UPD, QUEUE_RESET, COMMAND_UPD, APP_POS_UPD, CTL_POS_UPD };
			
			Type type      = EMPTY_UPD;
			bool processed = false;
			
			//////////////////////////////////////////////////////////////
			// no data
			
				inline const Event& QueueResetEvent() {
					type         = QUEUE_RESET;
					processed    = false;
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
			//////////////////////////////////////////////////////////////
			
		};
		
		UpdateManagerThread(MainFrame *handler);
		~UpdateManagerThread();
		
		// thread interface
		void stop();
		void enableDisplay(bool state=true);
		void postEvent(const UpdateManagerThread::Event& evt);
		void postConfigUpdate(CncConfig* cncConfig);
		void postClearPositionSpy();
		
		
		
		
		
// old value interface

void postSetterValue(unsigned char id, int32_t value);
void postResetZView();
void postUpdateZView();
		

		
	protected:
		const unsigned int maxSetterEntries = 1000;
		
		MainFrame* pHandler;
		CncConfig* cncConfig;
		SetterList setterList;
		unsigned int setterCounter;
		bool enabled;
		bool queueReset;
		bool exit;
		
		inline void updateConfigurationControls();
		inline void updateSetterControls();
		
		
		
		
		
		
		
		ConcurrentQueue<UpdateManagerThread::Event> eventQueue;
		UpdateManagerThread::Event lastCmdEvent;
		UpdateManagerThread::Event lastAppPosEvent;
		UpdateManagerThread::Event lastCtlPosEvent;
		
		virtual ExitCode Entry();
		
	private:
		inline void pop();
		inline void postHeartbeat();
		
		inline void checkQueueReset();
		
		inline void immediateUpdate();
		
		inline void updateCmdInfo();
		inline void updateAppPosition();
		inline void updateCtlPosition();
		inline void updatePositionSpy(UpdateManagerThread::Event evt);
};

#endif