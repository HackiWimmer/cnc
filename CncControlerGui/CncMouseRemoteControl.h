#ifndef CNC_MOUSE_REMOTE_CONTROL
#define CNC_MOUSE_REMOTE_CONTROL

#include <wx/panel.h>
#include <wx/event.h>

// ----------------------------------------------------------------------------
class CncMouseRemoteControlEvent;
wxDECLARE_EVENT(wxEVT_CNC_MOUSE_REMOTE_CONTROL, CncMouseRemoteControlEvent);

// ----------------------------------------------------------------------------
class CncMouseRemoteControl : public wxPanel {
	
	public:
	
		struct Config {
			wxColour	colMouse_Center = wxColour(*wxBLUE);
			wxColour	colMouse_Motion = wxColour(*wxRED);
		};
		
		Config& getConfig() { return config; }
		
		CncMouseRemoteControl(wxWindow *parent);
		~CncMouseRemoteControl();
		
		void onKeyDown(wxKeyEvent& event);
		
	protected:
		
		virtual void enter		(const CncMouseRemoteControlEvent& event)	{ postEvent(event); }
		virtual void leave		(const CncMouseRemoteControlEvent& event)	{ postEvent(event); }
		virtual void setFocus	(const CncMouseRemoteControlEvent& event)	{ postEvent(event); }
		virtual void killFocus	(const CncMouseRemoteControlEvent& event)	{ postEvent(event); }
		virtual void mouse		(const CncMouseRemoteControlEvent& event)	{ postEvent(event); }
		virtual void leftDown	(const CncMouseRemoteControlEvent& event)	{ postEvent(event); }
		virtual void leftUp		(const CncMouseRemoteControlEvent& event)	{ postEvent(event); }
		virtual void keyDown	(const CncMouseRemoteControlEvent& event)	{ postEvent(event); }
		virtual void motion		(const CncMouseRemoteControlEvent& event)	{ postEvent(event); }
		virtual void wheel		(const CncMouseRemoteControlEvent& event)	{ postEvent(event); }
		
	private:
		
		struct MouseMotion {
			bool leftDown		= false;
			wxPoint start		= {0, 0};
			wxPoint end			= {0, 0};
			
		};
		
		void postEvent(const CncMouseRemoteControlEvent& event);
		void releaseLeftUp(CncMouseRemoteControlEvent& evt);
		
		void onPaint(wxPaintEvent& event);
		void onMouse(wxMouseEvent& event);
		void onSize(wxSizeEvent& event);
		void onEnter(wxMouseEvent& event);
		void onLeave(wxMouseEvent& event);
		void onSetFocus(wxFocusEvent& event);
		void onKillFocus(wxFocusEvent& event);
		void onEraseBackground(wxEraseEvent& event);
		
		Config			config;
		MouseMotion 	mouseMotion;
		wxPoint			normalizedCoord;
		
		wxDECLARE_NO_COPY_CLASS(CncMouseRemoteControl);
		wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
class CncMouseRemoteControlEvent : public wxCommandEvent {
	
	public:
		enum Id {
			CMRC_MOUSE	 			= 100,
			CMRC_RELEASE_LEFT_DOWN	= 200,
			CMRC_RELEASE_LEFT_UP	= 201,
			CMRC_MOUSE_MOVE			= 300,
			CMRC_WHEEL_ROTATION		= 400,
			CMRC_KEY_DOWN			= 500,
			CMRC_ENTER				= 601,
			CMRC_LEAVE				= 602,
			CMRC_SET_FOCUS			= 701,
			CMRC_KILL_FOCUS			= 702
		};
		
		enum MouseButtonState {
			MBS_UNUSED,
			MBS_DOWN,
			MBS_RELEASED
		};
		
		explicit CncMouseRemoteControlEvent(wxEventType eventType = wxEVT_CNC_MOUSE_REMOTE_CONTROL, int id = 0) 
		: wxCommandEvent(eventType, id)
		, leftButtonState(MBS_UNUSED)
		, remodeMouseEvent()
		, remoteKeyEvent()
		, mouseMove()
		, wheelRotation(0)
		, keyCode(WXK_NONE)
		{}
		
		explicit CncMouseRemoteControlEvent(const CncMouseRemoteControlEvent& event) 
		: wxCommandEvent(event)
		, leftButtonState(event.leftButtonState)
		, remodeMouseEvent(event.remodeMouseEvent)
		, remoteKeyEvent(event.remoteKeyEvent)
		, mouseMove(event.mouseMove)
		, wheelRotation(event.wheelRotation)
		, keyCode(event.keyCode)
		{}
		
		virtual ~CncMouseRemoteControlEvent() {
		}
		
		virtual wxEvent *Clone() const {
			return new CncMouseRemoteControlEvent(*this);
		}
		
		MouseButtonState	leftButtonState;
		wxMouseEvent 		remodeMouseEvent;
		wxKeyEvent			remoteKeyEvent;
		wxSize				mouseMove;
		int					wheelRotation;
		int					keyCode;

};

// ----------------------------------------------------------------------------
class CncMotionMonitorRemoteControl : public CncMouseRemoteControl {
	
};

#endif