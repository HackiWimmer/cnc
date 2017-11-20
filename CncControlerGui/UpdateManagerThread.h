#ifndef INTERRUPT_THREAD_H
#define INTERRUPT_THREAD_H

#include <wx/thread.h>
#include <wx/datetime.h>
#include "CncCommon.h"
#include "CncConfig.h"
#include "CncPosition.h"

////////////////////////////////////////////////////////////////////////////////////
class UpdateManagerEvent;
wxDECLARE_EVENT(wxEVT_UPDATE_MANAGER_THREAD, UpdateManagerEvent);

class UpdateManagerEvent : public wxThreadEvent {
	
	public:
		UpdateManagerEvent(wxEventType eventType = wxEVT_UPDATE_MANAGER_THREAD, int id = 0)
		: wxThreadEvent(eventType, id)
		, pos(0L, 0L, 0L)
		, referenceId(-1L)
		, speedMode('R')
		, speedValue(0.0)
		{}

		UpdateManagerEvent(const UpdateManagerEvent& event)
		: wxThreadEvent(event)
		, pos(event.getCurrentPosition())
		, referenceId(event.getReferenceId())
		, speedMode(event.getSpeedMode())
		, speedValue(event.getSpeedValue())
		{
			// already done by wxThreadEvent::wxThreadEvent(event)
			// SetString(GetString().Clone());
		}

		virtual wxEvent *Clone() const {
			return new UpdateManagerEvent(*this);
		}
		
		const CncLongPosition& getCurrentPosition() const { return pos; }
		void setCurrentPosition(const CncLongPosition& p) { pos.set(p); }
		
		const long getReferenceId() const { return referenceId; }
		void setReferenceId(const long id) { referenceId = id; }
		
		const char getSpeedMode() const { return speedMode; }
		void setSpeedMode(char m) { speedMode = m; }
		
		const double getSpeedValue() const { return speedValue; }
		void setSpeedValue(double sv) { speedValue = sv; }
		
	private:
		CncLongPosition pos;
		
		long referenceId;
		char speedMode;
		double speedValue;
};

typedef void (wxEvtHandler::*UpdateManagerEventFunction)(UpdateManagerEvent&);
#define UpdateManagerEventHandler(func) wxEVENT_HANDLER_CAST(UpdateManagerEventFunction, func)

////////////////////////////////////////////////////////////////////////////////////
class MainFrame;

class UpdateManagerThread : public wxThread {
	
	public:
		enum SpyContent { APP_POSITIONS = 0, CTL_POSITIONS = 1};
		enum EventId    { COMPLETED = 1, HEARTBEAT = 2, APP_POS_UPDATE = 3, CTL_POS_UPDATE = 4, DISPATCH_ALL = 5 };
		enum SpeedMode  { UNDEFINED = '\0', RAPID = 'R', WORK = 'W' };
		
		static const int UMT_SETLST_NUM = 0;
		static const int UMT_SETLST_KEY = 1;
		static const int UMT_SETLST_VAL = 2;
		
		struct Event{
			enum Type { EMPTY_UPD, 
			
						POSSPY_RESET,
						SETLST_RESET,
						
						CONFIG_UPD,
						COMMAND_UPD, 
						
						APP_POS_UPD,
						CTL_POS_UPD,
					
						SETTER_ADD
					};
			
			Type type      = EMPTY_UPD;
			wxDateTime ts  = wxDateTime::UNow();
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
			
				inline const Event& SetterListResetEvent() {
					type			= SETLST_RESET;
					processed    	= false;
					return *this;
				}
				
			//////////////////////////////////////////////////////////////
			struct Set {
				unsigned char	id; 
				int32_t			value;
			} set;
			
				inline const Event& SetterEvent(unsigned char i, int32_t v) {
					type         = SETTER_ADD;
					ts           = wxDateTime::UNow();
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
				double speedValue		= 0.0;
				SpeedMode speedMode		= UNDEFINED;
				CncLongPosition pos	= {0, 0, 0};
				
				void reset() {
					set();
				}
				
				void set() {
					id			= -1;
					speedValue	= 0.0;
					speedMode	= UNDEFINED;
					pos.set({0, 0, 0});
				}
				
				void set(long id, SpeedMode speedMode, double speedValue, const CncLongPosition& p) {
					this->id			= id;
					this->speedValue	= speedValue;
					this->speedMode		= speedMode;
					this->pos.set(p);
				}

				void set(long id, wxString speedMode, double sv, const CncLongPosition& p) {
					SpeedMode sm;
					switch ( (char)speedMode[0] ) {
						case 'R':	sm = RAPID; break;
						case 'W':	sm = WORK;  break;
						default:	sm = UNDEFINED;
					}
					set(id, sm, sv, p);
				}
				
			} pos;
			
				inline const Event& AppPosEvent(long i, SpeedMode sm, double sv, const CncLongPosition& p) {
					type			= APP_POS_UPD;
					processed    	= false;
					pos.set(i, sm, sv, p);
					return *this;
				}
				
				inline const Event& AppPosEvent(long i, const wxString& sm, double sv, const CncLongPosition& p) {
					type			= APP_POS_UPD;
					processed    	= false;
					pos.set(i, sm, sv, p);
					return *this;
				}

				inline const Event& CtlPosEvent(long i, SpeedMode sm, double sv, const CncLongPosition& p) {
					type			= CTL_POS_UPD;
					processed    	= false;
					pos.set(i, sm, sv, p);
					return *this;
				}
				
				inline const Event& CtlPosEvent(long i, const wxString& sm, double sv, const CncLongPosition& p) {
					type			= CTL_POS_UPD;
					processed    	= false;
					pos.set(i, sm, sv, p);
					return *this;
				}
			//////////////////////////////////////////////////////////////
			
		};
		
		typedef std::vector<UpdateManagerThread::Event> PosSpyList;
		typedef std::vector<UpdateManagerThread::Event> SetterList;
		
		UpdateManagerThread(MainFrame *handler);
		~UpdateManagerThread();
		
		// thread interface
		void stop();
		void postEvent(const UpdateManagerThread::Event& evt);
		
		void fillPositionSpy(wxListBox* lb, UpdateManagerThread::SpyContent sc, CncConfig& config);
		void fillSetterList(wxListCtrl* lb);
		
	protected:
		const unsigned int maxSetterEntries = 1000;
		
		MainFrame* pHandler;
		bool exit;
		
		PosSpyList appPosSpyList;
		PosSpyList ctlPosSpyList;
		SetterList setterList;
		
		virtual ExitCode Entry();
};

#endif