#ifndef CNC_NAVIGATOR_PANEL_H
#define CNC_NAVIGATOR_PANEL_H

#include <map>
#include <wx/panel.h>
#include <wx/event.h>
#include <wx/timer.h>

// ----------------------------------------------------------------------------
class CncNavigatorPanelEvent;
wxDECLARE_EVENT(wxEVT_CNC_NAVIGATOR_PANEL, CncNavigatorPanelEvent);

// ----------------------------------------------------------------------------
class CncNavigatorPanel : public wxPanel {
	
	public:
		
		enum Direction { UD=0, NN=1, SS=2, EE=3, WW=4, NW, NE, SW, SE, CP, CN };
		typedef std::map<Direction, wxString> ToolTipMap;
		
		struct Config {
			bool innerCircle 			= false;
			bool shortFormat			= false;
			bool speedByAmplitude		= true;
			
			bool showToolTip			= false;
			bool showRegionInfo			= true;
			bool showRegionTip			= true;
			
			bool showContextMenu		= true;
			
			int margin 					= 10;
			int alignment				= wxALIGN_NOT;
			
			wxColour colNN				= wxColour(89, 89, 255);
			wxColour colSS				= colNN;
			wxColour colEE				= wxColour(255, 64, 64);
			wxColour colWW				= colEE;
			wxColour colCP				= wxColour(27, 139, 61);
			wxColour colCN				= colCP;
			
			wxColour defaultColour		= wxColour(192, 192, 192);
			wxColour highlightColour	= wxColour(  0, 182, 255);
			wxColour activatedColour	= wxColour(  0, 162, 232);
					
			ToolTipMap toolTipMap;
			
			Config() {
				toolTipMap[UD] 	= "Undefined Region";
				toolTipMap[NN] 	= "North";
				toolTipMap[SS] 	= "South";
				toolTipMap[WW] 	= "West";
				toolTipMap[EE] 	= "East";
				toolTipMap[NW] 	= "North West";
				toolTipMap[NE] 	= "North East";
				toolTipMap[SW] 	= "South West";
				toolTipMap[SE] 	= "South East";
				toolTipMap[CP] 	= "Center Positive";
				toolTipMap[CN] 	= "Center Negative";
			}
			
			void initToolTipMapAsCoordSytem() {
				toolTipMap[UD] 	= "";
				toolTipMap[NN] 	= "+Y";
				toolTipMap[SS] 	= "-Y";
				toolTipMap[WW] 	= "-X";
				toolTipMap[EE] 	= "+X";
				toolTipMap[NW] 	= "+Y-X";
				toolTipMap[NE] 	= "+Y+X";
				toolTipMap[SW] 	= "-Y-X";
				toolTipMap[SE] 	= "-Y+X";
				toolTipMap[CP] 	= "+Z";
				toolTipMap[CN] 	= "-Z";
			}
		
		};
		
		static const char* getDirectionAsString(const Direction d);
		
		CncNavigatorPanel(wxWindow *parent, const Config& cfg);
		virtual ~CncNavigatorPanel();
		
		virtual bool Enable(bool enable=true);
		
		void setSecureMode(bool state);
		void setStepMode(CncStepMode sm) { stepMode = sm; }
		
	private:
		
		const unsigned int defaultTimeout  =  100;
		const unsigned int stepwiseTimeout = 1000;
		
		struct Current {
			Direction	direction	= UD;
			double		amplitude	= 0.0;
			bool		acitvated	= false;
		};
		
		struct MouseInfo {
			int			normalizedX	= 0;
			int 		normalizedY	= 0;
			bool 		leftDown	= false;
			bool 		leftUp		= false;
		};
		
		struct OuterCircleRegion {
			Direction 	direction	= UD;
			double 		startAngle	= 0.0;
			double 		midAngle	= 0.0;
			double 		stopAngle	= 0.0;
		};
		
		typedef std::vector<OuterCircleRegion> OuterRegions;
		
		CncNavigatorPanelEvent*	navEvent;
		CncNavigatorPanelEvent*	continuousEvent;
		wxMenu*					popupMenu;
		CncStepMode				stepMode;
		wxTimer					continuousTimer;
		wxRect 					navRectangle;
		int 					innerRadius;
		int 					outerRadius;
		OuterRegions 			outerRegions;
		Config 					config;
		Current					current;
		
		CncNavigatorPanelEvent& prepareEvent(int id = 0); 
		
		double adjustAngle(double angle, double dx, double dy);
		void onMouse(const MouseInfo& mi);
		void postEvent(const CncNavigatorPanelEvent& event);
		void drawToolTip();
		
		void startContinuousEvent(int id);
		void stopContinuousEvent();
		
		void displayContextMenu();
		void precreateSegmentAngles();
		
		void onPaint(wxPaintEvent& event);
		void onMouse(wxMouseEvent& event);
		void onSize(wxSizeEvent& event);
		void onKeyDown(wxKeyEvent& event);
		void onEnter(wxMouseEvent& event);
		void onLeave(wxMouseEvent& event);
		void onSetFocus(wxFocusEvent& event);
		void onKillFocus(wxFocusEvent& event);
		void onEraseBackground(wxEraseEvent& event);
		void onContinuousTimer(wxTimerEvent& event);
				
		wxDECLARE_NO_COPY_CLASS(CncNavigatorPanel);
		wxDECLARE_EVENT_TABLE();
		
	protected:
		
		virtual void enterPanel(const CncNavigatorPanelEvent& event)		{ postEvent(event); }
		virtual void leavePanel(const CncNavigatorPanelEvent& event)		{ postEvent(event); }
		virtual void setFocus(const CncNavigatorPanelEvent& event)			{ postEvent(event); }
		virtual void killFocus(const CncNavigatorPanelEvent& event)			{ postEvent(event); }
		virtual void enterRegion(const CncNavigatorPanelEvent& event)		{ postEvent(event); }
		virtual void changeSpeedIndex(const CncNavigatorPanelEvent& event)	{ postEvent(event); }
		virtual void leaveRegion(const CncNavigatorPanelEvent& event)		{ postEvent(event); }
		virtual void activateRegion(const CncNavigatorPanelEvent& event)	{ postEvent(event); }
		virtual void deactivateRegion(const CncNavigatorPanelEvent& event)	{ postEvent(event); }
		virtual void leftDownRegion(const CncNavigatorPanelEvent& event);
		virtual void leftUpRegion(const CncNavigatorPanelEvent& event);
		virtual void rightDownRegion(const CncNavigatorPanelEvent& event);
		virtual void rightUpRegion(const CncNavigatorPanelEvent& event);
		
};

// ----------------------------------------------------------------------------
class CncNavigatorPanelEvent : public wxCommandEvent {
	
	public:
		
		enum Id {
			CNP_COORDINATES 		= 100,
			CNP_ENTER_PANEL			= 200, 
			CNP_LEAVE_PANEL			= 201,
			CNP_SET_FOCUS			= 202, 
			CNP_KILL_FOCUS			= 203,
			CNP_CHANGE_SPEED_INDEX	= 204,
			CNP_ENTER_REGION		= 300, 
			CNP_LEAVE_REGION		= 301,
			CNP_LEFT_DOWN_REGION	= 400,
			CNP_LEFT_UP_REGION		= 401,
			CNP_ACTIVATE_REGION		= 500,
			CNP_DEACTIVATE_REGION	= 501,
			CNP_LEFT_DOWN_FOLLOWUP	= 900

		};
		
		static const char* getEventIdAsString(Id id) {
			switch ( id ) {
				case CNP_COORDINATES:				return "CNP_COORDINATES";
				case CNP_ENTER_PANEL:				return "CNP_ENTER_PANEL";
				case CNP_LEAVE_PANEL:				return "CNP_LEAVE_PANEL";
				case CNP_SET_FOCUS:					return "CNP_SET_FOCUS";
				case CNP_KILL_FOCUS:				return "CNP_KILL_FOCUS";
				case CNP_CHANGE_SPEED_INDEX:		return "CNP_CHANGE_SPEED_INDEX";
				case CNP_ENTER_REGION:				return "CNP_ENTER_REGION";
				case CNP_LEAVE_REGION:				return "CNP_LEAVE_REGION";
				case CNP_LEFT_DOWN_REGION:			return "CNP_LEFT_DOWN_REGION";
				case CNP_LEFT_UP_REGION:			return "CNP_LEFT_UP_REGION";
				case CNP_ACTIVATE_REGION:			return "CNP_ACTIVATE_REGION";
				case CNP_DEACTIVATE_REGION:			return "CNP_DEACTIVATE_REGION";
				case CNP_LEFT_DOWN_FOLLOWUP:		return "CNP_LEFT_DOWN_FOLLOWUP";
			}
			
			return "CNP_???";
		}
		
		CncNavigatorPanelEvent(wxEventType eventType = wxEVT_CNC_NAVIGATOR_PANEL, int id = 0) 
		: wxCommandEvent(eventType, id)
		, radius		(0.0)
		, amplitude		(0.0)
		, angle			(0.0)
		, speedIndex	(0.0)
		, mouseX		(0)
		, mouseY		(0)
		, direction		(CncNavigatorPanel::Direction::UD)
		, activated		(false)
		{}
		
		explicit CncNavigatorPanelEvent(const CncNavigatorPanelEvent& event) 
		: wxCommandEvent(event)
		, radius		(event.radius)
		, amplitude		(event.amplitude)
		, angle			(event.angle)
		, speedIndex	(event.speedIndex)
		, mouseX		(event.mouseX)
		, mouseY		(event.mouseY)
		, direction		(event.direction)
		, activated		(event.activated)
		{}
		
		virtual ~CncNavigatorPanelEvent() {
		}
		
		virtual wxEvent *Clone() const {
			return new CncNavigatorPanelEvent(*this);
		}
		
		void reset() {
			radius		= 0.0;
			amplitude	= 0.0;
			angle		= 0.0;
			speedIndex	= 0;
			mouseX		= 0;
			mouseY		= 0;
			direction	= CncNavigatorPanel::Direction::UD;
			activated	= false;
		}
		
		const CncNavigatorPanelEvent* getNavigatorPanel() const { return static_cast<CncNavigatorPanelEvent*>(GetEventObject()); }
		
		double 							radius;
		double							amplitude;
		double 							angle;
		
		int								speedIndex;
		int 							mouseX;
		int 							mouseY;
		
		CncNavigatorPanel::Direction	direction;
		bool							activated;
		
};

#endif