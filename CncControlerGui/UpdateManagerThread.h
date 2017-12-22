#ifndef INTERRUPT_THREAD_H
#define INTERRUPT_THREAD_H

#include <wx/thread.h>
#include <wx/datetime.h>
#include <boost/lockfree/spsc_queue.hpp>
#include "CncPosSpyListCtrl.h"
#include "CncSetterListCtrl.h"
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
		enum SpeedMode  { UNDEFINED = '\0', RAPID = 'R', WORK = 'W' };
		
		struct Event{
			enum Type { EMPTY_UPD, 
			
						POSSPY_RESET,
						SETLST_RESET,
						
						CONFIG_UPD,
						
						POS_TYP_UPD,
						APP_POS_UPD,
						CTL_POS_UPD,
						
						SETTER_ADD
					};
			
			Type type      = EMPTY_UPD;
			wxDateTime ts  = wxDateTime::UNow();
			bool processed = false;
			
			const char* getTypeAsString() const {
				switch ( type ) {
					case EMPTY_UPD:			return "EMPTY_UPD Event"; 
					case POSSPY_RESET:		return "POSSPY_RESET Event";
					case SETLST_RESET:		return "SETLST_RESET Event";
					case CONFIG_UPD:		return "CONFIG_UPD Event";
					case POS_TYP_UPD:		return "POS_TYP_UPD Event";
					case APP_POS_UPD:		return "APP_POS_UPD Event";
					case CTL_POS_UPD:		return "CTL_POS_UPD Event";
					case SETTER_ADD:		return "SETTER_ADD Event";
				}
				
				return "Unknown event type";
			}
			
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
			// no data
			
				inline const Event& PosSpyResetEvent() {
					type			= POSSPY_RESET;
					processed    	= false;
					return *this;
				}
			
			//////////////////////////////////////////////////////////////
			struct Cnt {
				UpdateManagerThread::SpyContent posSpyType = CTL_POSITIONS;
			} cnt;
			
				inline const Event& PosSypContentEvent(UpdateManagerThread::SpyContent sc) {
					type         = POS_TYP_UPD;
					cnt.posSpyType   = sc;
					return *this;
				}

			//////////////////////////////////////////////////////////////
			struct Pos {
				unsigned char pid		= '\0';
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
				
				void set(unsigned char pid, long id, SpeedMode speedMode, double speedValue, const CncLongPosition& p) {
					this->pid 			= pid;
					this->id			= id;
					this->speedValue	= speedValue;
					this->speedMode		= speedMode;
					this->pos.set(p);
				}

				void set(unsigned char pid, long id, wxString speedMode, double sv, const CncLongPosition& p) {
					SpeedMode sm;
					switch ( (char)speedMode[0] ) {
						case 'R':	sm = RAPID; break;
						case 'W':	sm = WORK;  break;
						default:	sm = UNDEFINED;
					}
					set(pid, id, sm, sv, p);
				}
				
			} pos;
			
				inline const Event& AppPosEvent(unsigned char pid, long i, SpeedMode sm, double sv, const CncLongPosition& p) {
					type			= APP_POS_UPD;
					processed    	= false;
					pos.set(pid, i, sm, sv, p);
					return *this;
				}
				
				inline const Event& AppPosEvent(unsigned char pid, long i, const wxString& sm, double sv, const CncLongPosition& p) {
					type			= APP_POS_UPD;
					processed    	= false;
					pos.set(pid, i, sm, sv, p);
					return *this;
				}

				inline const Event& CtlPosEvent(unsigned char pid, long i, SpeedMode sm, double sv, const CncLongPosition& p) {
					type			= CTL_POS_UPD;
					processed    	= false;
					pos.set(pid, i, sm, sv, p);
					return *this;
				}
				
				inline const Event& CtlPosEvent(unsigned char pid, long i, const wxString& sm, double sv, const CncLongPosition& p) {
					type			= CTL_POS_UPD;
					processed    	= false;
					pos.set(pid, i, sm, sv, p);
					return *this;
				}
			//////////////////////////////////////////////////////////////
			
		};
		
		UpdateManagerThread(MainFrame *handler);
		virtual ~UpdateManagerThread();
		
		// thread interface
		void stop();
		void postEvent(const UpdateManagerThread::Event& evt);
		
		bool somethingLeftToDo();
		
		unsigned int fillPositionSpy(CncPosSpyListCtrl* lb);
		unsigned int fillSetterList(CncSetterListCtrl* lb);
		
	protected:
		
		typedef UpdateManagerThread::Event LastPosSpyEntry;
		typedef UpdateManagerThread::Event LastSetterEntry;
		
		static const unsigned long posQueueSize 	= 1024 * 1024;
		typedef std::vector<UpdateManagerThread::Event> SetterList;
		typedef boost::lockfree::spsc_queue<UpdateManagerThread::Event, boost::lockfree::capacity<posQueueSize> > PosSpyQueue;
		typedef boost::lockfree::spsc_queue<UpdateManagerThread::Event, boost::lockfree::capacity<posQueueSize> > SetterQueue;
		typedef boost::lockfree::spsc_queue<CncColumContainer, boost::lockfree::capacity<posQueueSize> > PosSpyStringQueue;
		typedef boost::lockfree::spsc_queue<CncColumContainer, boost::lockfree::capacity<posQueueSize> > SetterStringQueue;
		
		MainFrame* pHandler;
		bool exit;
		
		SpyContent posSpyContent;
		
		CncUnit unit;
		double displayFactX;
		double displayFactY;
		double displayFactZ;
		
		PosSpyQueue posSpyQueue;
		PosSpyStringQueue posSpyStringQueue;
		
		SetterQueue setterQueue;
		SetterStringQueue setterStringQueue;
		
		LastPosSpyEntry lpse;
		CncColumContainer posSpyRow;
		
		LastSetterEntry lste;
		CncColumContainer setterRow;
		
		virtual ExitCode Entry();
		inline void popAndFormatPosSpyQueue();
		inline void popAndFormatSetterQueue();
		
		inline void postInfo(const wxString& msg);
		inline void postWarning(const wxString& msg);
		inline void postError(const wxString& msg);
		
};

#endif