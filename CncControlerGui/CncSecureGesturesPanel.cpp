#include <cmath>
#include <wx/dcclient.h>
#include <wx/dcgraph.h>
#include <wx/dcbuffer.h>
#include "CncCommon.h"
#include "CncPolarRegionDetector.h"
#include "wxCrafterSecurePanel.h"
#include "CncSecureGesturesPanel.h"

///////////////////////////////////////////////////////////////////
CncSecureGesturesPanel::CncSecureGesturesPanel(wxWindow* parent, wxOrientation o, Type t, Mode m, int s)
: wxPanel			(parent, wxID_ANY)
, caller			(NULL)
, callbackId		(defaultCallbackId)
, state				(S_INACTIVE)
, updateTimer		(new wxTimer(this, updateTimerId))
, observerTimer		(new wxTimer(this, observerTimerId))
, observerTs		(wxDateTime::UNow())
, orientation		(o)
, type				(t)
, mode				(m)
, sensitivity		(abs(s))
, centreBmp			(ImageLibSecure().Bitmap("BMP_CROSSHAIR"))
, centrePt			(0, 0)
, zeroPt			(0, 0)
, currentPt			(0, 0)
, innerRect			(0, 0, 0, 0)
, leftRect			(0, 0, 0, 0)
, rightRect			(0, 0, 0, 0)
, topRect			(0, 0, 0, 0)
, bottomRect		(0, 0, 0, 0)
, regionLen			(0)
, totalLen			(0)
, knobSize			(0, 0)
, knobColour		(wxColour(128, 128, 128))
, lineColour		(wxColour( 64,  64,  64))
, innerColour		(wxColour(200, 200, 200))
, mouseDown			(false)
, lastData			()
///////////////////////////////////////////////////////////////////
{
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	
	Bind(wxEVT_PAINT,			&CncSecureGesturesPanel::onPaint,			this);
	Bind(wxEVT_SIZE,			&CncSecureGesturesPanel::onSize,			this);
	Bind(wxEVT_MOTION,			&CncSecureGesturesPanel::onMouse,			this);
	Bind(wxEVT_LEFT_DOWN,		&CncSecureGesturesPanel::onMouse,			this);
	Bind(wxEVT_LEFT_UP,			&CncSecureGesturesPanel::onMouse,			this);
	Bind(wxEVT_LEAVE_WINDOW,	&CncSecureGesturesPanel::onLeave,			this);
	Bind(wxEVT_TIMER,			&CncSecureGesturesPanel::onTimer,			this);
	
	int eventsMask = 0;
	switch ( orientation ) {
		case wxHORIZONTAL:	eventsMask |= wxTOUCH_HORIZONTAL_PAN_GESTURE;
							break;
							
		case wxVERTICAL:	eventsMask |= wxTOUCH_VERTICAL_PAN_GESTURE;
							break;
							
		case wxBOTH:		eventsMask |= wxTOUCH_PAN_GESTURES;
							wxASSERT( m == M_BOTH );
							break;
	}
	
	/*
	wxTOUCH_PRESS_GESTURES
	wxTOUCH_ZOOM_GESTURE
	wxTOUCH_ROTATE_GESTURE
	wxTOUCH_PRESS_GESTURES 
	*/
	
	if ( !EnableTouchEvents(eventsMask) )
	{
		std::cerr << "Failed to enable touch events" << std::endl;
	}
	else
	{
		// Still bind event handlers just in case they still work
		Bind(wxEVT_GESTURE_PAN,		&CncSecureGesturesPanel::onPan,				this);
		Bind(wxEVT_GESTURE_ZOOM,	&CncSecureGesturesPanel::onZoom,			this);
		Bind(wxEVT_GESTURE_ROTATE,	&CncSecureGesturesPanel::onRotate,			this);
		Bind(wxEVT_TWO_FINGER_TAP,	&CncSecureGesturesPanel::onTwoFingerTap,	this);
		Bind(wxEVT_LONG_PRESS,		&CncSecureGesturesPanel::onLongPress,		this);
		Bind(wxEVT_PRESS_AND_TAP,	&CncSecureGesturesPanel::onPressAndTap,		this);
	}

	m_lastZoomFactor = 1.0;
	
	reset();
}
///////////////////////////////////////////////////////////////////
CncSecureGesturesPanel::~CncSecureGesturesPanel() {
///////////////////////////////////////////////////////////////////
	// unbind . . . 
	
	stopTimer();
}
///////////////////////////////////////////////////////////////////
CncSecureGesturesPanel::State CncSecureGesturesPanel::skipState() {
///////////////////////////////////////////////////////////////////
	switch ( state )
	{
		case S_INACTIVE:	state = S_STARTING;		break;
		case S_STARTING:	state = S_CONTINUING;	break;
		case S_CONTINUING:	state = S_CONTINUING;	break;
	}
	
	return state;
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::startTimer() {
///////////////////////////////////////////////////////////////////
	skipState();
	updateTimer->Start(500);
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::stopTimer() {
///////////////////////////////////////////////////////////////////
	observerTimer->Stop();
	updateTimer->Stop();
	state = S_INACTIVE;
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::setCallbackInterface(CallbackInterface* inf, int id) {
///////////////////////////////////////////////////////////////////
	caller 		= inf;
	callbackId	= id;
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::calculateCoordinates() {
///////////////////////////////////////////////////////////////////
	//-----------------------------------------------------------------
	// boundaries
	{
		const wxSize size = innerRect.GetSize();
		const int b       = border;
		const int h       = size.GetHeight();
		const int w       = size.GetWidth();
	
		int xp            = wxRound(m_translateDistance.m_x);
		int yp            = wxRound(m_translateDistance.m_y);
		
		switch ( orientation ) 
		{
			case wxHORIZONTAL:
				xp = std::max(xp, b + knobSize.GetWidth() / 2);
				xp = std::min(xp, b + w - knobSize.GetWidth() / 2);
				break;
			case wxVERTICAL:
				yp = std::max(yp, b + knobSize.GetHeight() / 2);
				yp = std::min(yp, b + h - knobSize.GetHeight() / 2);
				break;
			case wxBOTH:
				xp = std::max(xp, b + centreBmp.GetWidth() / 2);
				xp = std::min(xp, b + w - centreBmp.GetWidth() / 2);
				yp = std::max(yp, b + centreBmp.GetHeight() / 2);
				yp = std::min(yp, b + h - centreBmp.GetHeight() / 2);
				break;
		}
		
		centrePt.x = xp;
		centrePt.y = yp;
	}
	
	// something changed?
	if ( lastData.xPos == centrePt.x && lastData.yPos == centrePt.y )
		return;
	
	if ( IsShown() && caller && skipState() == S_STARTING )
		caller->notifyStarting(state);
		
	// determine distance to zeroPt
	const int dx = centrePt.x - zeroPt.x;
	const int dy = zeroPt.y   - centrePt.y;
	
	const Data ref = lastData;
	//lastData.reset();
	
	lastData.xPos	= centrePt.x;
	lastData.yPos	= centrePt.y;
	lastData.xVal	= dx;
	lastData.yVal	= dy;
	
	lastData.cbId	= callbackId;
	
	// -----------------------------------------------------------
	auto evalRange = [&](double delta){
		
		const double quotient = (((double)totalLen) / sensitivity);
		
		int range  = delta / quotient;
		range     += (delta < 0 ? -1 : delta > 0 ? + 1 : 0);
		
		// ensure boundaries
		return range >= 0 ? std::min(range, +sensitivity) : std::max(range, -sensitivity);
	};
	
	switch ( orientation ) 
	{
		case wxHORIZONTAL:
			lastData.radius	= dx;
			lastData.xMax	= totalLen;
			lastData.yMax	= 0;
			lastData.range	= evalRange(dx);
			lastData.ratio	= ((double)dx) / totalLen;
			lastData.angle	= 0.0;
			break;
			
		case wxVERTICAL:
			lastData.radius	= dy;
			lastData.xMax	= 0;
			lastData.yMax	= totalLen;
			lastData.range	= evalRange(dy);
			lastData.ratio	= ((double)dy) / totalLen;
			lastData.angle	= 0.0;
			break;
			
		case wxBOTH:
			lastData.radius	= wxRound(sqrt( dx * dx + dy * dy));
			lastData.xMax	= innerRect.GetWidth()  / 2;
			lastData.yMax	= innerRect.GetHeight() / 2;
			lastData.range	= evalRange(lastData.radius);
			lastData.ratio	= lastData.radius / totalLen;
			lastData.angle	= atan2(dy, dx) * 180 / PI;
			break;
	}
	
	// ensure boundaries
	lastData.ratio = std::max(-1.0, lastData.ratio);
	lastData.ratio = std::min(+1.0, lastData.ratio);
	
	// ensure 0 ... 360 degree
	if ( lastData.angle < 0.0 )
		lastData.angle = 180.0 + (180.0 + lastData.angle);
		
	// snap into axis  
	/*
	if ( orientation == wxBOTH ) 
	{
		auto between = [](double value, double checkpoint, double deviation ){
			return cnc::between(value, checkpoint, checkpoint + deviation);
		};
		
		double deviation = 6.0;
		bool snap = false;
		if (   between(lastData.angle,   0.0, deviation) 
			|| between(lastData.angle, 355.0, deviation) 
			|| between(lastData.angle, 175.0, deviation)
			|| between(lastData.angle, 180.0, deviation)
		)
		{
			snap = true;
			m_translateDistance.m_y = zeroPt.y;
		}
		
		if (   between(lastData.angle,  85.0, deviation) 
			|| between(lastData.angle,  90.0, deviation) 
			|| between(lastData.angle, 265.0, deviation)
			|| between(lastData.angle, 270.0, deviation)
		)
		{
			snap = true;
			m_translateDistance.m_x = zeroPt.x;
		}
		
		// recursive call 
		if ( snap == true )
			calculateCoordinates();
	}
	*/
	
	// additionally change information
	lastData.isRangeChanged = ( lastData.range != ref.range );
	lastData.isAngleChanged = ( lastData.angle != ref.angle );
	lastData.isTimerChanged = false;
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::calculateZero() {
///////////////////////////////////////////////////////////////////
	const wxSize size = GetClientSize();
	
	// determine zeroPt position
	switch ( mode )
	{
		case M_POSITIVE:
			switch ( orientation ) {
				case wxHORIZONTAL:	zeroPt.x = border + knobSize.GetWidth() / 2;
									zeroPt.y = size.GetHeight() / 2;
									break;
									
				case wxVERTICAL:	zeroPt.x = size.GetWidth()  / 2;
									zeroPt.y = size.GetHeight() - border - knobSize.GetHeight() / 2;
									break;
									
				case wxBOTH:		wxASSERT( orientation != wxBOTH );
									break;
			}
			break;
			
		case M_NEGATIVE:
			switch ( orientation ) {
				case wxHORIZONTAL:	zeroPt.x =  size.GetWidth() - border - knobSize.GetWidth() / 2;
									zeroPt.y = size.GetHeight() / 2;
									break;
									
				case wxVERTICAL:	zeroPt.x = size.GetWidth()  / 2;
									zeroPt.y = 0 + border + knobSize.GetHeight() / 2;
									break;
									
				case wxBOTH:		wxASSERT( orientation != wxBOTH );
									break;
			}
			break;
			
		case M_BOTH:
			zeroPt.x = size.GetWidth()  / 2;
			zeroPt.y = size.GetHeight() / 2;
			break;
	}
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::calculateDimensions() {
///////////////////////////////////////////////////////////////////
	const wxSize size = GetClientSize();
	
	centrePt		= wxPoint(size.GetWidth() / 2, size.GetHeight() / 2);
	innerRect	= wxRect(border, border, size.GetWidth() - 2 * border, size.GetHeight() - 2 * border);
	
	leftRect	= wxRect(innerRect.GetX(),                            innerRect.GetY(),                             innerRect.GetWidth() / 2, innerRect.GetHeight());
	rightRect	= wxRect(innerRect.GetX() + innerRect.GetWidth() / 2, innerRect.GetY(),                             innerRect.GetWidth() / 2, innerRect.GetHeight());
	
	topRect		= wxRect(innerRect.GetX(),                            innerRect.GetY(),                             innerRect.GetWidth(),     innerRect.GetHeight() / 2);
	bottomRect	= wxRect(innerRect.GetX(),                            innerRect.GetY() + innerRect.GetHeight() / 2, innerRect.GetWidth(),     innerRect.GetHeight() / 2);

	switch ( orientation ) 
	{
		case wxHORIZONTAL:
			knobSize.SetWidth(centreBmp.GetWidth() + 4);
			knobSize.SetHeight(innerRect.GetHeight());
			
			totalLen  = (size.GetWidth() / ( mode == M_BOTH ? 2 : 1 )) - knobSize.GetWidth();
			regionLen = (size.GetWidth() / ( mode == M_BOTH ? 2 : 1 )) / sensitivity;
			break;
			
		case wxVERTICAL:
			knobSize.SetWidth(innerRect.GetWidth());
			knobSize.SetHeight(centreBmp.GetHeight() + 4);
			
			totalLen  = (size.GetHeight() / ( mode == M_BOTH ? 2 : 1 )) - knobSize.GetHeight();
			regionLen = (size.GetHeight() / ( mode == M_BOTH ? 2 : 1 )) / sensitivity;
			break;
			
		case wxBOTH:
			knobSize.SetWidth(0);
			knobSize.SetHeight(0);
			
			totalLen  = std::min(size.GetWidth() / 2, size.GetHeight() / 2) - std::max(centreBmp.GetWidth(), centreBmp.GetHeight());
			regionLen = std::min(size.GetWidth() / 2, size.GetHeight() / 2) / sensitivity;
			break;
	}
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::recalculate() {
///////////////////////////////////////////////////////////////////
	if ( type == T_BUTTON )
		return reset();
		
	calculateZero();
	calculateDimensions();
	
	//std::cout << callbackId << ": " << lastData.xVal << ", " <<  lastData.yVal << std::endl;
	
	
	if ( lastData.xVal == 0 && lastData.yVal == 0 )
	{
		m_translateDistance.m_x = zeroPt.x;
		m_translateDistance.m_y = zeroPt.y;
	}
	else if ( lastData.xVal == 0 )
	{
		m_translateDistance.m_x = zeroPt.x;
	}
	else if ( lastData.yVal == 0 )
	{
		m_translateDistance.m_y = zeroPt.y;
	}
	else
	{
		
		
		
		#warning
	}
	
	Refresh();
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::reset() {
///////////////////////////////////////////////////////////////////
	stopTimer();
	
	calculateZero();
	calculateDimensions();
	
	m_translateDistance.m_x = zeroPt.x;
	m_translateDistance.m_y = zeroPt.y;
	
	applyPosChange(false);
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::applyPosChange(bool useTimer) {
///////////////////////////////////////////////////////////////////
	// calculate all depending values
	calculateCoordinates();
	
	//update control
	Refresh();
	
	// notify . . .
	if ( IsShown() && caller )
		caller->notifyPositionChanged(lastData);
		
	if ( useTimer == true )
	{
		const bool stop = ( m_translateDistance.m_x == zeroPt.x && m_translateDistance.m_y == zeroPt.y );
		
		if ( stop )	stopTimer();
		else		startTimer();
	}
	else
	{
		stopTimer();
	}
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::applyPosHeld() {
///////////////////////////////////////////////////////////////////
	if ( IsShown() && caller )
		caller->notifyPositionHeld(lastData);
}
/////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onSize(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////
	event.Skip();
	recalculate();
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	
	switch (event.GetTimer().GetId() )
	{
		case updateTimerId:
		{
			lastData.isTimerChanged = true;
			applyPosHeld();
			break;
		}
		case observerTimerId:
		{
			const wxTimeSpan diff = wxDateTime::UNow() - observerTs;
			if  ( diff.GetMilliseconds() > 500 ) {
				CNC_PRINT_FUNCT
				reset();
			}
			break;
		}
	}
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onPaint(wxPaintEvent& WXUNUSED(event)) {
///////////////////////////////////////////////////////////////////
	wxAutoBufferedPaintDC paintDC(this);
	paintDC.Clear();
	
	wxGCDC dc(paintDC);
	dc.SetTransformMatrix(m_affineMatrix);

	const wxSize size = innerRect.GetSize();
	const int b       = border;
	const int h       = size.GetHeight();
	const int w       = size.GetWidth();
	const int xp      = centrePt.x;
	const int yp      = centrePt.y;
	
	//-----------------------------------------------------------------
	auto fillBckRect = [&](const wxRect& r, const wxColour& c1, const wxColour& c2, wxDirection d) 
	{
		const wxColour c3( 78,  78,  78);
		dc.GradientFillLinear(r, c1, c2, d);
		if ( d == wxDOWN || d == wxUP )
		{
			for ( int y = 0; y <= r.GetHeight(); y += regionLen ) 
			{
				dc.SetPen(c3);
				dc.DrawLine(r.GetX(), r.GetY() + y, r.GetWidth(), r.GetY() + y);
			}
		}
		else if ( d == wxLEFT || d == wxRIGHT )
		{
			for ( int x = 0; x <= r.GetWidth(); x += regionLen ) 
			{
				dc.SetPen(c3);
				dc.DrawLine(r.GetX() + x, r.GetY(), r.GetX() + x, r.GetHeight());
			}
		}
	};
	
	//-----------------------------------------------------------------
	// background
	const wxColour c1 = innerColour;
	const wxColour c2 = innerColour.ChangeLightness(40);
	switch ( orientation ) 
	{
		// background ---------------------------------------------
		case wxHORIZONTAL:
		{
			switch ( mode ) 
			{
				case M_NEGATIVE:
					fillBckRect(innerRect,  c1, c2, wxLEFT);
					break;
				case M_POSITIVE:
					fillBckRect(innerRect,  c1, c2, wxRIGHT);
					break;
				case M_BOTH:
					fillBckRect(leftRect,   c1, c2, wxLEFT);
					fillBckRect(rightRect,  c1, c2, wxRIGHT);
					break;
			}
			break;
		}
		// background ---------------------------------------------
		case wxVERTICAL:
		{
			switch ( mode ) 
			{
				case M_NEGATIVE:
					fillBckRect(innerRect,  c1, c2, wxDOWN);
					break;
				case M_POSITIVE:
					fillBckRect(innerRect,  c1, c2, wxUP);
					break;
				case M_BOTH:
					fillBckRect(topRect,    c1, c2, wxUP);
					fillBckRect(bottomRect, c1, c2, wxDOWN);
					break;
			}
			break;
		}
		// background ---------------------------------------------
		case wxBOTH:
		{
			switch ( mode ) 
			{
				case M_NEGATIVE:
					wxASSERT( mode != M_NEGATIVE );
					break;
					
				case M_POSITIVE:
					wxASSERT( mode != M_POSITIVE );
					break;
					
				case M_BOTH:
					dc.GradientFillConcentric(innerRect, c1, c2, zeroPt);
					
					dc.SetPen(wxColour( 78,  78,  78));
					dc.SetBrush(*wxTRANSPARENT_BRUSH);
					
					for ( int r = totalLen - 2 * border; r > 0; r -= regionLen ) 
						dc.DrawCircle(zeroPt,  r);
					
					dc.DrawLine(zeroPt.x,        b, zeroPt.x,        h);
					dc.DrawLine(       b, zeroPt.y,        w, zeroPt.y);
					
					dc.SetPen(wxColour( 180,  180,  180));
					const int len = std::min(w / 2, h / 2);
					for ( auto it = CncPolarRegionDetector::getAngles().begin(); it != CncPolarRegionDetector::getAngles().end(); ++it )
					{
						int xx = wxRound(cos((*it) * PI / 180 ) * len);
						int yy = wxRound(sin((*it) * PI / 180 ) * len);
						dc.DrawLine(zeroPt.x, zeroPt.y, zeroPt.x + xx, (zeroPt.y + yy*(-1)));
					}
					break;
			}
			break;
		}
	}
	
	//-----------------------------------------------------------------
	// knob
	switch ( orientation ) 
	{
		case wxHORIZONTAL:
			dc.SetPen(knobColour);
			dc.SetBrush(wxBrush(wxColour( 64,  64,  64, 128)));
			dc.DrawRectangle(xp - knobSize.GetWidth() / 2, b, knobSize.GetWidth(), knobSize.GetHeight());
			break;
			
		case wxVERTICAL:
			dc.SetPen(knobColour);
			dc.SetBrush(wxBrush(wxColour( 64,  64,  64, 128)));
			dc.DrawRectangle(b, yp -  knobSize.GetHeight() / 2, knobSize.GetWidth(), knobSize.GetHeight());
			break;
			
		case wxBOTH:
			break;
	}
	
	//-----------------------------------------------------------------
	// cross-hair lines
	dc.SetPen(lineColour);
	dc.DrawLine(xp,  b, xp,  h);
	dc.DrawLine( b, yp,  w, yp);
	
	// bitmap
	dc.DrawBitmap(centreBmp, 1 + xp - centreBmp.GetWidth() / 2, 1 + yp - centreBmp.GetHeight() / 2);
	
	//-----------------------------------------------------------------
	// label
	const wxFont font(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Segoe UI"));
	dc.SetFont(font);
	
	wxRect rangeRect = innerRect;
	wxRect ratioRect = innerRect;
	int rangeAlign   = 0;
	int ratioAlign   = 0;
	
	switch ( orientation ) 
	{
		case wxHORIZONTAL:
			rangeRect.x = xp - knobSize.GetWidth() / 2;
			ratioRect.x = xp - knobSize.GetWidth() / 2;
			rangeAlign =  wxALIGN_TOP;
			ratioAlign =  wxALIGN_BOTTOM;
			break;
			
		case wxVERTICAL:
			rangeRect.y = yp;
			ratioRect.y = yp;
			rangeAlign =  wxALIGN_TOP | wxALIGN_LEFT;
			ratioAlign =  wxALIGN_TOP | wxALIGN_RIGHT;
			break;
			
		case wxBOTH:
			rangeAlign =  wxALIGN_TOP | wxALIGN_LEFT;
			ratioAlign =  wxALIGN_TOP | wxALIGN_RIGHT;
			
			dc.DrawLabel(wxString::Format("(%+4d|%+4d)",	lastData.xVal, lastData.yVal),	innerRect, wxALIGN_BOTTOM | wxALIGN_LEFT);
			dc.DrawLabel(wxString::Format("%+.0lf Degree ",	lastData.angle),				innerRect, wxALIGN_BOTTOM | wxALIGN_RIGHT);
			break;
	}
	
	dc.DrawLabel(wxString::Format("%+2d",		lastData.range),		rangeRect, rangeAlign);
	dc.DrawLabel(wxString::Format("%+.0lf%",	lastData.ratio * 100 ), ratioRect, ratioAlign);
}
/////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onLeave(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////
	event.Skip();
	
	mouseDown = false;
	
	if ( type == T_BUTTON )
		reset();
}
/////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onMouse(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////
	event.Skip();
	
	if ( event.LeftUp() )
	{
		mouseDown = false;
		
		if ( type == T_BUTTON )
			reset();
	}
	else
	{
		if ( event.LeftDown() )
			mouseDown = true;
		
		if ( mouseDown == true ) 
		{
			switch ( orientation )
			{
				case wxHORIZONTAL:	m_translateDistance.m_x =  event.GetX();
									m_translateDistance.m_y =  GetClientSize().GetHeight() / 2;
									break;
									
				case wxVERTICAL:	m_translateDistance.m_x =  GetClientSize().GetWidth() / 2;
									m_translateDistance.m_y =  event.GetY();
									break;
									
				case wxBOTH:		m_translateDistance.m_x =  event.GetX();
									m_translateDistance.m_y =  event.GetY();
									break;
			}
		}
		
		// apply
		applyPosChange(true);
	}
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onPan(wxPanGestureEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( event.IsGestureEnd() )
	{
		if ( type == T_BUTTON )
			reset();
	}
	if ( event.IsGestureStart() )
	{
		observerTimer->Start(500);
	}
	else
	{
		const wxPoint2DDouble ref = m_translateDistance;
		const wxPoint delta       = event.GetDelta();
		
		observerTs = wxDateTime::UNow();
		
		// Transform the distance using the transpose of the matrix,
		// in order to translate the image to match the screen coordinates
		wxMatrix2D m;
		m_affineMatrix.Get(&m, NULL);
		
		wxPoint2DDouble deltaD(m.m_11 * delta.x + m.m_12 * delta.y,
							   m.m_21 * delta.x + m.m_22 * delta.y);
		
		// Add it to the total translation
		m_translateDistance += deltaD;
		
		// override the not necessary direction 
		switch ( orientation ) 
		{
			case wxHORIZONTAL:	m_translateDistance.m_y = zeroPt.y;	break;
			case wxVERTICAL:	m_translateDistance.m_x = zeroPt.x;	break;
			case wxBOTH:											break;
		}
		
		// apply on demand
		if ( m_translateDistance.m_x == 0.0 && m_translateDistance.m_x == m_translateDistance.m_y )
		{
			// apply zero pos
			applyPosChange(false);
		}
		else
		{
			if ( ref != m_translateDistance )	applyPosChange(true);
			else								applyPosHeld();
		}
	}
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onZoom(wxZoomGestureEvent& event) {
///////////////////////////////////////////////////////////////////
/*
    if ( event.IsGestureStart() )
    {
        wxLogMessage("Zoom gesture started");

        m_lastZoomFactor = 1.0;
    }

    wxLogMessage("Zoom gesture performed with zoom center at (%d, %d) and zoom Factor = %f",
        event.GetPosition().x, event.GetPosition().y, event.GetZoomFactor());

    const wxPoint& zoomCenter = event.GetPosition();

    // Translate to zoom center
    m_affineMatrix.Translate(zoomCenter.x, zoomCenter.y);
    // Scale
    m_affineMatrix.Scale(event.GetZoomFactor() / m_lastZoomFactor, event.GetZoomFactor() / m_lastZoomFactor);
    // Translate back
    m_affineMatrix.Translate(-zoomCenter.x, -zoomCenter.y);

    if ( event.IsGestureEnd() )
    {
        wxLogMessage("Zoom gesture ended");
    }

    m_lastZoomFactor = event.GetZoomFactor();

    Refresh();
*/
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onRotate(wxRotateGestureEvent& event) {
///////////////////////////////////////////////////////////////////
 /*
   if ( event.IsGestureStart() )
    {
        wxLogMessage("Rotate gesture started");

        m_lastRotationAngle = 0.0;
    }

    wxLogMessage("Rotate gesture performed with rotation center at (%d, %d) and cumulative rotation angle = %f",
        event.GetPosition().x, event.GetPosition().y, event.GetRotationAngle());

    const wxPoint& rotationCenter = event.GetPosition();

    // Translate to rotation center
    m_affineMatrix.Translate(rotationCenter.x, rotationCenter.y);
    // Rotate
    m_affineMatrix.Rotate(event.GetRotationAngle() - m_lastRotationAngle);
    // Translate back
    m_affineMatrix.Translate(-rotationCenter.x, -rotationCenter.y);

    if ( event.IsGestureEnd() )
    {
        wxLogMessage("Rotate gesture ended");
    }

    m_lastRotationAngle = event.GetRotationAngle();

    Refresh();
*/
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onTwoFingerTap(wxTwoFingerTapEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( event.IsGestureStart() )
		std::cout << "Two Finger Tap gesture gesture started\n";

	std::cout << wxString::Format("Two Finger Tap gesture performed at (%d, %d)\n", event.GetPosition().x, event.GetPosition().y);

	if ( event.IsGestureEnd() )
		std::cout << "Two Finger Tap gesture ended\n";
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onLongPress(wxLongPressEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( event.IsGestureStart() )
		std::cout << "Long Press gesture started\n";

	std::cout << wxString::Format("Long Press gesture performed at (%d,%d)\n", event.GetPosition().x, event.GetPosition().y);

	if ( event.IsGestureEnd() )
		std::cout << "Long Press gesture ended\n";
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onPressAndTap(wxPressAndTapEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( event.IsGestureStart() )
		std::cout << "Press and Tap gesture started\n";

	std::cout << wxString::Format("Press and Tap gesture performed at (%d,%d)\n", event.GetPosition().x, event.GetPosition().y);

	if ( event.IsGestureEnd() )
		std::cout << "Press and Tap gesture ended\n";
}
