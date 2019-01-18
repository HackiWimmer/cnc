#include <wx/dcclient.h>
#include <wx/dcbuffer.h>
#include <wx/event.h>
#include "CncMouseRemoteControl.h"

// ----------------------------------------------------------------------------
// CncMouseRemoteControl Event Table
// ----------------------------------------------------------------------------
wxDEFINE_EVENT(wxEVT_CNC_MOUSE_REMOTE_CONTROL, CncMouseRemoteControlEvent);

wxBEGIN_EVENT_TABLE(CncMouseRemoteControl, wxPanel)
	EVT_PAINT					(CncMouseRemoteControl::onPaint)
	EVT_SIZE					(CncMouseRemoteControl::onSize)
	EVT_MOUSE_EVENTS			(CncMouseRemoteControl::onMouse)
	EVT_KEY_DOWN				(CncMouseRemoteControl::onKeyDown)
	EVT_ENTER_WINDOW			(CncMouseRemoteControl::onEnter)
	EVT_LEAVE_WINDOW			(CncMouseRemoteControl::onLeave)
	EVT_SET_FOCUS				(CncMouseRemoteControl::onSetFocus)
	EVT_KILL_FOCUS				(CncMouseRemoteControl::onKillFocus)
	EVT_ERASE_BACKGROUND		(CncMouseRemoteControl::onEraseBackground)
wxEND_EVENT_TABLE()

///////////////////////////////////////////////////////////////////
CncMouseRemoteControl::CncMouseRemoteControl(wxWindow *parent)
: wxPanel(parent)
, config()
, mouseMotion()
, normalizedCoord()
///////////////////////////////////////////////////////////////////
{
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	SetBackgroundColour(*wxBLACK);
}
///////////////////////////////////////////////////////////////////
CncMouseRemoteControl::~CncMouseRemoteControl() {
///////////////////////////////////////////////////////////////////
	
}
//////////////////////////////////////////////////
void CncMouseRemoteControl::postEvent(const CncMouseRemoteControlEvent& event) {
//////////////////////////////////////////////////
	wxPostEvent(GetParent(), event);
}
///////////////////////////////////////////////////////////////////
void CncMouseRemoteControl::onSize(wxSizeEvent& event) {
///////////////////////////////////////////////////////////////////
	Refresh();
}
///////////////////////////////////////////////////////////////////
void CncMouseRemoteControl::onKeyDown(wxKeyEvent& event) {
///////////////////////////////////////////////////////////////////
	CncMouseRemoteControlEvent evt;
	evt.remoteKeyEvent = event;
	evt.SetId(CncMouseRemoteControlEvent::Id::CMRC_KEY_DOWN);
	evt.keyCode = event.GetKeyCode();
	
	keyDown(evt);
}
///////////////////////////////////////////////////////////////////
void CncMouseRemoteControl::onEnter(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	mouseMotion.leftDown = false;
	
	CncMouseRemoteControlEvent evt;
	evt.SetId(CncMouseRemoteControlEvent::Id::CMRC_ENTER);
	evt.remodeMouseEvent = event;
	enter(evt);
}
///////////////////////////////////////////////////////////////////
void CncMouseRemoteControl::onLeave(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	CncMouseRemoteControlEvent evt;
	evt.SetId(CncMouseRemoteControlEvent::Id::CMRC_LEAVE);
	evt.remodeMouseEvent = event;
	leave(evt);
	
	if ( mouseMotion.leftDown == true ) 
		releaseLeftUp(evt);
	
	mouseMotion.leftDown = false;
}
///////////////////////////////////////////////////////////////////
void CncMouseRemoteControl::onSetFocus(wxFocusEvent& event) {
///////////////////////////////////////////////////////////////////
	CncMouseRemoteControlEvent evt;
	evt.SetId(CncMouseRemoteControlEvent::Id::CMRC_SET_FOCUS);
	setFocus(evt);
}
///////////////////////////////////////////////////////////////////
void CncMouseRemoteControl::onKillFocus(wxFocusEvent& event) {
///////////////////////////////////////////////////////////////////
	CncMouseRemoteControlEvent evt;
	evt.SetId(CncMouseRemoteControlEvent::Id::CMRC_KILL_FOCUS);
	killFocus(evt);
	
	if ( mouseMotion.leftDown == true ) 
		releaseLeftUp(evt);
	
	mouseMotion.leftDown = false;
}
///////////////////////////////////////////////////////////////////
void CncMouseRemoteControl::onEraseBackground(wxEraseEvent& event) {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void CncMouseRemoteControl::onPaint(wxPaintEvent& event) {
///////////////////////////////////////////////////////////////////
	wxAutoBufferedPaintDC dc(this);
	dc.Clear();
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	
	const wxSize size = GetClientSize();
	dc.SetDeviceOrigin(size.GetWidth()  / 2, size.GetHeight() / 2);
	
	auto drawCrossHair = [&](const wxPen& pen, int x, int y) {
		dc.SetPen(pen);
		
		const int l1 = 20;
		const int l2 = 0.75 * l1;
		
		dc.DrawLine(x - l1, y,      x + l1, y);
		dc.DrawLine(x,      y - l1, x,      y + l1);
		dc.DrawCircle(x, y, l2);
		
		dc.SetBrush(wxBrush(pen.GetColour()));
		dc.DrawCircle(x, y, l2 * 0.5);
	};
	
	if ( mouseMotion.leftDown == true ) drawCrossHair(wxPen(config.colMouse_Motion, 1, wxSOLID), normalizedCoord.x, normalizedCoord.y);
	else								drawCrossHair(wxPen(config.colMouse_Center, 1, wxSOLID), 0, 0);
	
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
}
///////////////////////////////////////////////////////////////////
void CncMouseRemoteControl::onMouse(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	CncMouseRemoteControlEvent evt;
	evt.remodeMouseEvent = event;
	evt.SetId(CncMouseRemoteControlEvent::Id::CMRC_MOUSE);
	
	mouse(evt);
	
	const wxSize size = GetClientSize();
	const int x = event.GetPosition().x;
	const int y = event.GetPosition().y;
	
	normalizedCoord.x = x - size.GetWidth()  / 2;
	normalizedCoord.y = y - size.GetHeight() / 2;
	
	bool leftReleasedDown = false;
	if ( event.LeftDown() ) {
		leftReleasedDown			= true;
		mouseMotion.leftDown 		= true;
		mouseMotion.start			= normalizedCoord;
	}
	
	bool leftReleasedUp = false;
	if ( event.LeftUp() ) {
		if ( mouseMotion.leftDown == true )
			leftReleasedUp = true;
			
		mouseMotion.leftDown 		= false;
		mouseMotion.end				= normalizedCoord;
	}
	
	if 		( mouseMotion.leftDown == true )	evt.leftButtonState = CncMouseRemoteControlEvent::MouseButtonState::MBS_DOWN;
	else if ( leftReleasedUp       == true )	evt.leftButtonState = CncMouseRemoteControlEvent::MouseButtonState::MBS_RELEASED;
	
	if ( mouseMotion.leftDown == true ) {
		wxPoint mouseDiff = normalizedCoord - mouseMotion.start;
		mouseDiff.y *= -1;
		
		if ( mouseDiff.x != 0 || mouseDiff.y != 0 ) {
			evt.SetId(CncMouseRemoteControlEvent::Id::CMRC_MOUSE_MOVE);
			evt.mouseMove.SetWidth(mouseDiff.x);
			evt.mouseMove.SetHeight(mouseDiff.y);
			motion(evt);
		}
	}
	
	if ( event.GetWheelRotation() != 0 ) {
		evt.SetId(CncMouseRemoteControlEvent::Id::CMRC_WHEEL_ROTATION);
		evt.wheelRotation = event.GetWheelRotation() / event.GetWheelDelta();
		wheel(evt);
	}
	
	if ( leftReleasedDown == true ) {
		evt.SetId(CncMouseRemoteControlEvent::Id::CMRC_RELEASE_LEFT_DOWN);
		leftDown(evt);
	}
	
	if ( leftReleasedUp == true )
		releaseLeftUp(evt);
		
	Refresh();
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncMouseRemoteControl::releaseLeftUp(CncMouseRemoteControlEvent& evt) {
///////////////////////////////////////////////////////////////////
	evt.SetId(CncMouseRemoteControlEvent::Id::CMRC_RELEASE_LEFT_UP);
	leftUp(evt);
}