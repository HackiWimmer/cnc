#include <cmath>
#include <wx/dcclient.h>
#include <wx/dcgraph.h>
#include <wx/dcbuffer.h>
#include "GlobalFunctions.h"
#include "CncCommon.h"
#include "CncPolarRegionDetector.h"
#include "wxCrafterSecurePanel.h"
#include "CncSecureGesturesPanel.h"

wxDEFINE_EVENT(wxEVT_CNC_SECURE_GESTURES_PANEL,	CncSecureGesturesPanelEvent);

namespace CncTest {
	
	void test1()
	{
		auto testInt = [](CncEdge e, int min, int max, float ratio)
		{
			const int len = 100;
			const int val = CncRangeTranslator<int>(min, max, e, len).calcByRatio(ratio);
			std::cout << "  " << "min: " << min << " max: " << max << " len: " << len << " edge: " << (int)e;
			std::cout << "   " << "ratio: " << ratio << " -> val: " << val << std::endl;
		};
		
		testInt(cncLeft,  0, 30,  0.0);
		testInt(cncLeft,  0, 30, +0.5);
		testInt(cncLeft,  0, 30, +1.0);
		testInt(cncLeft,  0, 30, -0.5);
		testInt(cncLeft,  0, 30, -1.0);
		std::cout << std::endl;
		
		testInt(cncLeft, -30,  0,  0.0);
		testInt(cncLeft, -30,  0, -0.5);
		testInt(cncLeft, -30,  0, -1.0);
		testInt(cncLeft, -30,  0, +0.5);
		testInt(cncLeft, -30,  0, +1.0);
		std::cout << std::endl;
		
		testInt(cncLeft, -30, 30,  0.0);
		testInt(cncLeft, -30, 30, +0.2);
		testInt(cncLeft, -30, 30, +0.5);
		testInt(cncLeft, -30, 30, +1.0);
		testInt(cncLeft, -30, 30, -0.2);
		testInt(cncLeft, -30, 30, -0.5);
		testInt(cncLeft, -30, 30, -1.0);
		std::cout << std::endl;

		testInt(cncRight,  0, 30,  0.0);
		testInt(cncRight,  0, 30, +0.5);
		testInt(cncRight,  0, 30, +1.0);
		testInt(cncRight,  0, 30, -0.5);
		testInt(cncRight,  0, 30, -1.0);
		std::cout << std::endl;
		
		testInt(cncRight, -30,  0,  0.0);
		testInt(cncRight, -30,  0, -0.5);
		testInt(cncRight, -30,  0, -1.0);
		testInt(cncRight, -30,  0, +0.5);
		testInt(cncRight, -30,  0, +1.0);
		std::cout << std::endl;
		
		testInt(cncRight, -30, 30,  0.0);
		testInt(cncRight, -30, 30, +0.2);
		testInt(cncRight, -30, 30, +0.5);
		testInt(cncRight, -30, 30, +1.0);
		testInt(cncRight, -30, 30, -0.5);
		testInt(cncRight, -30, 30, -1.0);
		std::cout << std::endl;
	}
	
	class Starter
	{
		public:
			Starter()
			{
				test1();
				
				abort();
			}
	};//S;
};

///////////////////////////////////////////////////////////////////

	#define LOG_GESTURE_PANEL 0
	#if LOG_GESTURE_PANEL > 0
		
		#define COUT_GESTURE_PANEL(...)		{ trace(std::cout, CNC_LOG_FUNCT, wxString::Format(__VA_ARGS__)); }
		#define CLOG_GESTURE_PANEL(...)		{ trace(std::clog, CNC_LOG_FUNCT, wxString::Format(__VA_ARGS__)); }
		#define CERR_GESTURE_PANEL(...)		{ trace(std::cerr, CNC_LOG_FUNCT, wxString::Format(__VA_ARGS__)); }
		#define CEX1_GESTURE_PANEL(...)		{ trace(cnc::cex1, CNC_LOG_FUNCT, wxString::Format(__VA_ARGS__)); }
		
	#else
		
		#define COUT_GESTURE_PANEL(...)		{}
		#define CLOG_GESTURE_PANEL(...)		{}
		#define CERR_GESTURE_PANEL(...)		{}
		#define CEX1_GESTURE_PANEL(...)		{}
		
	#endif

///////////////////////////////////////////////////////////////////
CncSecureGesturesPanel::CncSecureGesturesPanel(wxWindow* parent, wxOrientation o, Type t, Mode m, int s)
: wxPanel				(parent, wxID_ANY)
, callbackId			(defaultCallbackId)
, state					(S_INACTIVE)
, updateTimer			(new wxTimer(this, updateTimerId))
, observerTimer			(new wxTimer(this, observerTimerId))
, observerTs			(wxDateTime::UNow())
, behaviorChanged		(true)
, orientation			(o)
, type					(t)
, mode					(m)
, sensitivity			(abs(s))
, centreBmp				(ImageLibSecure().Bitmap("BMP_CROSSHAIR"))
, centrePt				(0, 0)
, zeroPt				(0, 0)
, currentPt				(0, 0)
, innerRect				(0, 0, 0, 0)
, leftRect				(0, 0, 0, 0)
, rightRect				(0, 0, 0, 0)
, topRect				(0, 0, 0, 0)
, bottomRect			(0, 0, 0, 0)
, regionLen				(0)
, totalLen				(0)
, knobSize				(0, 0)
, knobColour			(wxColour(128, 128, 128))
, lineColour			(wxColour( 64,  64,  64))
, innerColour			(wxColour(200, 200, 200))
, mouseDown				(false)
, lastEvent				(new CncSecureGesturesPanelEvent())
, translatedDistance	(zeroPt)
///////////////////////////////////////////////////////////////////
{
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	
	const wxFont font(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Segoe UI"));
	SetFont(font);
	
	Bind(wxEVT_PAINT,			&CncSecureGesturesPanel::onPaint,			this);
	Bind(wxEVT_SIZE,			&CncSecureGesturesPanel::onSize,			this);
	Bind(wxEVT_SHOW,			&CncSecureGesturesPanel::onShow,			this);
	Bind(wxEVT_INIT_DIALOG,		&CncSecureGesturesPanel::onInitPanel,		this);
	Bind(wxEVT_MOTION,			&CncSecureGesturesPanel::onMouse,			this);
	Bind(wxEVT_LEFT_DCLICK,		&CncSecureGesturesPanel::onMouse,			this);
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
	
	// further touch events
	// wxTOUCH_PRESS_GESTURES, wxTOUCH_ZOOM_GESTURE, wxTOUCH_ROTATE_GESTURE,wxTOUCH_PRESS_GESTURES 
	
	if ( !EnableTouchEvents(eventsMask) )
	{
		CNC_PRINT_FUNCT_A("Failed to enable touch events\n");
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
}
///////////////////////////////////////////////////////////////////
CncSecureGesturesPanel::~CncSecureGesturesPanel() {
///////////////////////////////////////////////////////////////////
	// unbind . . . 
	Unbind(wxEVT_PAINT,				&CncSecureGesturesPanel::onPaint,			this);
	Unbind(wxEVT_SIZE,				&CncSecureGesturesPanel::onSize,			this);
	Unbind(wxEVT_SHOW,				&CncSecureGesturesPanel::onShow,			this);
	Unbind(wxEVT_MOTION,			&CncSecureGesturesPanel::onMouse,			this);
	Unbind(wxEVT_LEFT_DCLICK,		&CncSecureGesturesPanel::onMouse,			this);
	Unbind(wxEVT_LEFT_DOWN,			&CncSecureGesturesPanel::onMouse,			this);
	Unbind(wxEVT_LEFT_UP,			&CncSecureGesturesPanel::onMouse,			this);
	Unbind(wxEVT_LEAVE_WINDOW,		&CncSecureGesturesPanel::onLeave,			this);
	Unbind(wxEVT_TIMER,				&CncSecureGesturesPanel::onTimer,			this);
	Unbind(wxEVT_GESTURE_PAN,		&CncSecureGesturesPanel::onPan,				this);
	Unbind(wxEVT_GESTURE_ZOOM,		&CncSecureGesturesPanel::onZoom,			this);
	Unbind(wxEVT_GESTURE_ROTATE,	&CncSecureGesturesPanel::onRotate,			this);
	Unbind(wxEVT_TWO_FINGER_TAP,	&CncSecureGesturesPanel::onTwoFingerTap,	this);
	Unbind(wxEVT_LONG_PRESS,		&CncSecureGesturesPanel::onLongPress,		this);
	Unbind(wxEVT_PRESS_AND_TAP,		&CncSecureGesturesPanel::onPressAndTap,		this);
	
	stopTimer();
	
	wxDELETE(lastEvent);
}
//////////////////////////////////////////////////
CncSecureGesturesPanelEvent& CncSecureGesturesPanel::prepareEvent(int id) { 
//////////////////////////////////////////////////
	wxASSERT(lastEvent); 
	lastEvent->SetEventObject(this); 
	lastEvent->SetId(id); 
	
	return *lastEvent; 
}
///////////////////////////////////////////////////////////////////
bool CncSecureGesturesPanel::Layout() {
///////////////////////////////////////////////////////////////////
	return wxPanel::Layout();
}
///////////////////////////////////////////////////////////////////
bool CncSecureGesturesPanel::Enable(bool enable) {
///////////////////////////////////////////////////////////////////
	return wxPanel::Enable(enable);
}
///////////////////////////////////////////////////////////////////
bool CncSecureGesturesPanel::Show(bool show) {
///////////////////////////////////////////////////////////////////
	const bool ret = wxPanel::Show(show);
	
	if ( ret == true && show == true )
	{
		init();
	}
	
	return ret;
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
	//CNC_CEX1_FUNCT_A("U:%d, O:%d, S:%d", updateTimer->IsRunning(), observerTimer->IsRunning(), (int) state )
	//GblFunc::stacktrace(cnc::cex2);
	
	observerTimer->Stop();
	updateTimer->Stop();
	state = S_INACTIVE;
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onShow(wxShowEvent& event) {
///////////////////////////////////////////////////////////////////
	//CNC_PRINT_FUNCT_A(":ID=%d", callbackId)
	
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onInitPanel(wxInitDialogEvent& event) {
///////////////////////////////////////////////////////////////////
	//CNC_PRINT_FUNCT_A(":ID=%d", callbackId)
	
	event.Skip();
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
		dc.SetPen(c3);
		
		switch ( d )
		{
			case wxUP:		for ( int y = r.GetHeight(); y > 0; y -= regionLen ) 
								dc.DrawLine(r.GetX(), r.GetY() + y, r.GetWidth(), r.GetY() + y);
							break;
						
			case wxDOWN:	for ( int y = 0; y <= r.GetHeight(); y += regionLen ) 
								dc.DrawLine(r.GetX(), r.GetY() + y, r.GetWidth(), r.GetY() + y);
							break;
							
			case wxLEFT:	for ( int x = r.GetWidth(); x > 0; x -= regionLen ) 
								dc.DrawLine(r.GetX() + x, r.GetY(), r.GetX() + x, r.GetHeight());
							break;
							
			case wxRIGHT:	for ( int x = 0; x <= r.GetWidth(); x += regionLen ) 
								dc.DrawLine(r.GetX() + x, r.GetY(), r.GetX() + x, r.GetHeight());
							break;
							
			default:		wxASSERT_MSG(NULL, "Invalid situation, should not appear!");
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
	dc.SetFont(GetFont());
	
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
			
			dc.DrawLabel(wxString::Format("(%+4d|%+4d)",	lastEvent->data.xVal, lastEvent->data.yVal),	innerRect, wxALIGN_BOTTOM | wxALIGN_LEFT);
			dc.DrawLabel(wxString::Format("%+.1lf Degree ",	lastEvent->data.angle),							innerRect, wxALIGN_BOTTOM | wxALIGN_RIGHT);
			break;
	}
	
	dc.DrawLabel(wxString::Format("%+2d",		lastEvent->data.range),			rangeRect, rangeAlign);
	dc.DrawLabel(wxString::Format("%+.1lf%",	lastEvent->data.ratio * 100 ),	ratioRect, ratioAlign);
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::calculateDimensions() {
///////////////////////////////////////////////////////////////////
	// ensure even dimensions 
	if ( true )
	{
		wxSize virtualSize = GetClientSize();
		if ( virtualSize.GetWidth() % 2 != 0 )
			virtualSize.SetWidth(virtualSize.GetWidth() + 1);
		
		if ( virtualSize.GetHeight() % 2 != 0 )
			virtualSize.SetHeight(virtualSize.GetHeight() + 1);
		
		SetVirtualSize(virtualSize);
	}
	
	// determine base . . .
	const wxSize size = GetVirtualSize();
	
	// ------------------------------------------------------------
	// determine zero position
	switch ( mode )
	{
		case M_POSITIVE:
		{
			switch ( orientation )
			{
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
		}
		case M_NEGATIVE:
		{
			switch ( orientation )
			{
				case wxHORIZONTAL:	zeroPt.x = size.GetWidth() - border - knobSize.GetWidth() / 2;
									zeroPt.y = size.GetHeight() / 2;
									break;
									
				case wxVERTICAL:	zeroPt.x = size.GetWidth()  / 2;
									zeroPt.y = 0 + border + knobSize.GetHeight() / 2;
									break;
									
				case wxBOTH:		wxASSERT( orientation != wxBOTH );
									break;
			}
			
			
			break;
		}
		case M_BOTH:
		{
			zeroPt.x = size.GetWidth()  / 2;
			zeroPt.y = size.GetHeight() / 2;
			
			break;
		}
		
		
		// during the initialization the window proportions may be to small
		// and the consideration of border and knob size results in negative values
		if ( zeroPt.x < 0 )
			zeroPt.x = 0;
			
		if ( zeroPt.y < 0 )
			zeroPt.y = 0;
	}
	
	// ------------------------------------------------------------
	// determine depending coordinates
	const int b2 = border * 2;
	const int kb = 4;
	
	centrePt	= zeroPt;
	innerRect	= wxRect(border, border, size.GetWidth() - b2, size.GetHeight() - b2);
	
	leftRect	= wxRect(innerRect.GetX(),                            innerRect.GetY(),                             innerRect.GetWidth() / 2, innerRect.GetHeight());
	rightRect	= wxRect(innerRect.GetX() + innerRect.GetWidth() / 2, innerRect.GetY(),                             innerRect.GetWidth() / 2, innerRect.GetHeight());
	
	topRect		= wxRect(innerRect.GetX(),                            innerRect.GetY(),                             innerRect.GetWidth(),     innerRect.GetHeight() / 2);
	bottomRect	= wxRect(innerRect.GetX(),                            innerRect.GetY() + innerRect.GetHeight() / 2, innerRect.GetWidth(),     innerRect.GetHeight() / 2);
	
	
	switch ( orientation ) 
	{
		case wxHORIZONTAL:
		{
			knobSize.SetWidth(centreBmp.GetWidth() + kb);
			knobSize.SetHeight(innerRect.GetHeight());
			
			const int len = wxRound(float(size.GetWidth() - b2) /( mode == M_BOTH ? 2 : 1 ));
			regionLen     = std::max(0, wxRound(float(len) / sensitivity));
			totalLen      = std::max(0, len - knobSize.GetWidth());
			break;
		}
		case wxVERTICAL:
		{
			knobSize.SetWidth(innerRect.GetWidth());
			knobSize.SetHeight(centreBmp.GetHeight() + kb);
			
			const int len = wxRound(float(size.GetHeight() - b2) / ( mode == M_BOTH ? 2 : 1 ));
			regionLen     = std::max(0, wxRound(float(len) / sensitivity));
			totalLen      = std::max(0, len - knobSize.GetHeight());
			
			break;
		}
		case wxBOTH:
		{
			knobSize.SetWidth(0);
			knobSize.SetHeight(0);
			
			const int sw = (size.GetWidth()  - b2 ) / 2;
			const int sh = (size.GetHeight() - b2 ) / 2;
			
			const int len = std::min(sw, sh);
			totalLen      = std::max(0, len - std::max(centreBmp.GetWidth(), centreBmp.GetHeight()));
			regionLen     = std::max(0, wxRound(float(len / sensitivity)));
			break;
		}
	}
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::calculateCoordinates() {
///////////////////////////////////////////////////////////////////
	if ( behaviorChanged == true )
		return reset();
		
	if ( totalLen <= 0 )
		return;
		
	//-----------------------------------------------------------------
	// ensure boundaries and finally determine the knob centre
	{
		const wxSize size = innerRect.GetSize();
		const int b       = border;
		const int h       = size.GetHeight();
		const int w       = size.GetWidth();
		
		int xp            = wxRound(translatedDistance.m_x);
		int yp            = wxRound(translatedDistance.m_y);
		
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
		
		// knob centre 
		centrePt.x = xp;
		centrePt.y = yp;
	}
	
	// create a easy to use reference for further changes
	CSGPEvent::Data& lastData = lastEvent->data;
	
	// something changed?
	if ( lastData.xPos == centrePt.x && lastData.yPos == centrePt.y )
		return;
		
	// copy previous information
	const CSGPEvent::Data ref = lastEvent->data;
	
	const State s = skipState();
	if ( s == S_STARTING )
		wxPostEvent(GetParent(), prepareEvent(CncSecureGesturesPanelEvent::Id::CSGP_STARTING));
	
	// determine distance to zeroPt
	const int dx = centrePt.x - zeroPt.x;
	const int dy = zeroPt.y   - centrePt.y;
	
	//const CSGPEvent::Data ref = lastData;
	
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
			lastData.ratio	= totalLen ? (double(dx)) / totalLen : 0.0;
			lastData.angle	= 0.0;
			break;
			
		case wxVERTICAL:
			lastData.radius	= dy;
			lastData.xMax	= 0;
			lastData.yMax	= totalLen;
			lastData.range	= evalRange(dy);
			lastData.ratio	= totalLen ? (double(dy)) / totalLen : 0.0;
			lastData.angle	= 0.0;
			break;
			
		case wxBOTH:
			lastData.radius	= sqrt( dx * dx + dy * dy);
			lastData.xMax	= innerRect.GetWidth()  / 2;
			lastData.yMax	= innerRect.GetHeight() / 2;
			lastData.range	= evalRange(lastData.radius);
			lastData.ratio	= totalLen ? lastData.radius / totalLen : 0.0;
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
			translatedDistance.m_y = zeroPt.y;
		}
		
		if (   between(lastData.angle,  85.0, deviation) 
			|| between(lastData.angle,  90.0, deviation) 
			|| between(lastData.angle, 265.0, deviation)
			|| between(lastData.angle, 270.0, deviation)
		)
		{
			snap = true;
			translatedDistance.m_x = zeroPt.x;
		}
		
		// recursive call 
		if ( snap == true )
			calculateCoordinates();
	}
	*/
	
	// additionally change information
	lastData.isTimerChanged = false;
	lastData.isRangeChanged = ( lastData.range != ref.range );
	lastData.isRatioChanged = ( cnc::dblCmp::eq(lastData.ratio, ref.ratio) == false );
	lastData.isAngleChanged = ( cnc::dblCmp::eq(lastData.angle, ref.angle) == false );
	
	CLOG_GESTURE_PANEL("")
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::recalculate() {
///////////////////////////////////////////////////////////////////
	if ( type == T_BUTTON )
		return reset();
		
	// copy last data
	CSGPEvent::Data lastData = lastEvent->data;
	
	calculateDimensions();
	
	// (re)set the old value
	setValueByRatio(lastData.ratio, lastData.angle);
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::reset() {
///////////////////////////////////////////////////////////////////
	stopTimer();
	
	calculateDimensions();
	
	behaviorChanged = false;
	
	translatedDistance.m_x = zeroPt.x;
	translatedDistance.m_y = zeroPt.y;
	
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
	if ( IsShown() )
		wxPostEvent(GetParent(), prepareEvent(CncSecureGesturesPanelEvent::Id::CSGP_POS_CHANGED));
	
	if ( useTimer == true )
	{
		/*
		const bool zero = (	cnc::dblCmp::eq(translatedDistance.m_x, double(zeroPt.x)) && 
							cnc::dblCmp::eq(translatedDistance.m_y, double(zeroPt.y)) 
		);
		*/
		
		const bool zero = lastEvent->data.isZero();
		
		if ( zero )	reset();
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
	CncBoolSwitch bs(lastEvent->data.userTriggered);
	
	lastEvent->data.isTimerChanged = true;
	lastEvent->data.isRangeChanged = false;
	lastEvent->data.isRatioChanged = false;
	lastEvent->data.isAngleChanged = false;
	
	if ( IsShownOnScreen() )
		wxPostEvent(GetParent(), prepareEvent(CncSecureGesturesPanelEvent::Id::CSGP_POS_HELD));
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
			if ( type == T_BUTTON )
			{
				applyPosHeld();
			}
			
			break;
		}
		case observerTimerId:
		{
			const wxTimeSpan diff = wxDateTime::UNow() - observerTs;
			if ( diff.GetMilliseconds() > 500 )
			{
				if ( type == T_BUTTON )
					reset();
			}
			
			break;
		}
	}
}
/////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onLeave(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////
	event.Skip();
	
	const bool b = updateTimer->IsRunning()
				|| mouseDown == true
				|| state     != S_INACTIVE;
	
	if ( b )
	{
		mouseDown = false;
		
		if ( type == T_BUTTON )
			reset();
	}
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::setValueByRatio(float ratio, float angle) {
///////////////////////////////////////////////////////////////////
	if ( totalLen <= 0 )
		return;
		
	wxString errMsg;
	switch ( mode )
	{
		case M_NEGATIVE:	if ( cnc::fltCmp::gt(ratio, 0.0f) )	errMsg.assign("Mode is NEGATIVE, therefore ratio can't be positive.");
							else								errMsg.clear();
							break;
							
		case M_POSITIVE:	if ( cnc::fltCmp::lt(ratio, 0.0f) )	errMsg.assign("Mode is POSITIVE, therefore ratio can't be negative.");
							else								errMsg.clear();
							break;
							
		case M_BOTH:		errMsg.clear();
							break;
	}
	
	// auto correction
	if ( errMsg.IsEmpty() == false )
	{
		std::cerr	<< CNC_LOG_FUNCT 
					<< ": ID:" << callbackId
					<< ". Wrong ratio(" << ratio << ") sign!\n"
					<< errMsg << " Ratio sign will be inverted"
					<< std::endl;
					
		ratio *= (-1);
	}
	
	// ----------------------------------------------------------------------
	auto translateRange = [](const wxString& context, int min, int max, CncEdge edge, int len, float ratio) 
	{
		CncRangeTranslator<int> trans(min, max, edge, len);
		const int ret = trans.calcByRatio(ratio);
		
		if ( CncRangeTranslator<int>::isErrorValue( ret ) ) 
		{
			std::cerr	<< CNC_LOG_LOCATION << ": Invalid value " << ret
						<< std::endl
						<< " --> " << trans
						<< std::endl
			; 
		}
		
		return ret;
	};
	
	// prepare new control position . . .
	switch ( orientation ) 
	{
		case wxHORIZONTAL:
		{
			// constant Y coordinate
			translatedDistance.m_y = zeroPt.y;
			
			// determine X coordinate
			switch ( mode )
			{
				case M_NEGATIVE:
				{	
					translatedDistance.m_x  = translateRange(CNC_LOG_LOCATION, -totalLen, 0, cncLeft, totalLen, ratio);
					translatedDistance.m_x += knobSize.GetWidth() / 2 + border;
					break;
				}
				case M_POSITIVE:
				{
					translatedDistance.m_x  = translateRange(CNC_LOG_LOCATION, 0, +totalLen, cncLeft, totalLen, ratio);
					translatedDistance.m_x += knobSize.GetWidth() / 2 + border;
					break;
				}
				case M_BOTH:
				{
					const int len = GetVirtualSize().GetWidth();
					const int min = wxRound(float(-len) / 2);
					const int max = min * (-1);
					
					translatedDistance.m_x = translateRange(CNC_LOG_LOCATION, min, max, cncCenter, len, ratio);
					break;
				}
			}
			
			CEX1_GESTURE_PANEL("A ratio=%lf angle=%lf", ratio, angle)
			break;
		}
		case wxVERTICAL:
		{
			// constant X coordinate
			translatedDistance.m_x = zeroPt.x;
			
			// determine Y coordinate
			switch ( mode )
			{
				case M_NEGATIVE:
				{
					translatedDistance.m_y  = translateRange(CNC_LOG_LOCATION, -totalLen, 0, cncBottom, totalLen, ratio);
					translatedDistance.m_y += (knobSize.GetHeight() / 2 + border);
					break;
				}	
				case M_POSITIVE:
				{
					translatedDistance.m_y  = translateRange(CNC_LOG_LOCATION, 0, +totalLen, cncBottom, totalLen, ratio);
					translatedDistance.m_y += (knobSize.GetHeight() / 2 + border);
					break;
				}
				case M_BOTH:
				{
					const int len = GetVirtualSize().GetHeight();// % 2 == 0 ? GetClientSize().GetHeight() : GetClientSize().GetHeight() + 1;
					const int min = wxRound(float(-len) / 2);
					const int max = min * (-1);
					
					translatedDistance.m_y = translateRange(CNC_LOG_LOCATION, min, max, cncCenter, len, ratio);
					break;
				}
			}
			
			CEX1_GESTURE_PANEL("B ratio=%lf angle=%lf", ratio, angle)
			break;
		}
		case wxBOTH:
		{
			switch ( mode )
			{
				case M_NEGATIVE:
				case M_POSITIVE:
				{
					// not valid
					translatedDistance.m_x = 0;
					translatedDistance.m_y = 0;
					break;
				}
				case M_BOTH:
				{
					const float radius = CncRangeTranslator<float>(0.0f, float(totalLen), cncCenter, totalLen).calcByRatio(ratio);
					const float dx = radius * cos(angle * PI / 180);
					const float dy = radius * sin(angle * PI / 180);
					
					translatedDistance.m_x = dx;
					translatedDistance.m_y = dy;
					
					CEX1_GESTURE_PANEL("C ratio=%lf angle=%lf", ratio, angle)
					break;
				}
			}
			
			break;
		}
	}
	
	applyPosChange(false);
}
/////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onMouse(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////
	if ( IsEnabled() == false )
		return;
		
	event.Skip();
	
	if ( event.LeftDClick() )
	{
		if ( type == T_SWITCH )
			reset();
			
		return;
	}
	
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
				case wxHORIZONTAL:	translatedDistance.m_x = event.GetX();
									translatedDistance.m_y = zeroPt.y;
									break;
									
				case wxVERTICAL:	translatedDistance.m_x = zeroPt.x;
									translatedDistance.m_y = event.GetY();
									break;
									
				case wxBOTH:		translatedDistance.m_x = event.GetX();
									translatedDistance.m_y = event.GetY();
									break;
			}
			
			//CNC_PRINT_FUNCT_A("(%d)(%lf, %lf)", callbackId, translatedDistance.m_x, translatedDistance.m_y)
			
			// apply
			CncBoolSwitch bs(lastEvent->data.userTriggered);
			applyPosChange(true);
		}
	}
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onPan(wxPanGestureEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( IsEnabled() == false )
		return;

	event.StopPropagation();
	
	if ( event.IsGestureEnd() )
	{
		if ( type == T_BUTTON )
			reset();
	}
	
	if ( event.IsGestureStart() )
	{
		observerTimer->Start(400);
	}
	else
	{
		const wxPoint2DDouble ref = translatedDistance;
		const wxPoint delta       = event.GetDelta();
		
		observerTs = wxDateTime::UNow();
		
		// Transform the distance using the transpose of the matrix,
		// in order to translate the image to match the screen coordinates
		wxMatrix2D m;
		m_affineMatrix.Get(&m, NULL);
		
		wxPoint2DDouble deltaD(m.m_11 * delta.x + m.m_12 * delta.y,
							   m.m_21 * delta.x + m.m_22 * delta.y);
		
		// Add it to the total translation
		translatedDistance += deltaD;
		
		// override the not necessary direction 
		switch ( orientation ) 
		{
			case wxHORIZONTAL:	translatedDistance.m_y = zeroPt.y;	break;
			case wxVERTICAL:	translatedDistance.m_x = zeroPt.x;	break;
			case wxBOTH:											break;
		}
		
		// apply on demand
		CncBoolSwitch bs(lastEvent->data.userTriggered);
		
		if ( cnc::dblCmp::nu(translatedDistance.m_x) && cnc::dblCmp::eq(translatedDistance.m_x, translatedDistance.m_y) )
		{
			// apply zero pos
			applyPosChange(false);
		}
		else
		{
			if ( ref != translatedDistance )	applyPosChange(true);
			else								applyPosHeld();
		}
	}
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onZoom(wxZoomGestureEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( IsEnabled() == false )
		return;

	// currently only implemented to disable all gesture events
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onRotate(wxRotateGestureEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( IsEnabled() == false )
		return;

	// currently only implemented to disable all gesture events
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onTwoFingerTap(wxTwoFingerTapEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( IsEnabled() == false )
		return;

	if ( event.IsGestureStart() )
	{
		if ( type == T_SWITCH )
			reset();
	}
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onLongPress(wxLongPressEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( IsEnabled() == false )
		return;
		
/*
	if ( event.IsGestureStart() )
		std::cout << "Long Press gesture started\n";

	std::cout << wxString::Format("Long Press gesture performed at (%d,%d)\n", event.GetPosition().x, event.GetPosition().y);

	if ( event.IsGestureEnd() )
		std::cout << "Long Press gesture ended\n";
*/
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::onPressAndTap(wxPressAndTapEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( IsEnabled() == false )
		return;
		
/*
	if ( event.IsGestureStart() )
		std::cout << "Press and Tap gesture started\n";

	std::cout << wxString::Format("Press and Tap gesture performed at (%d,%d)\n", event.GetPosition().x, event.GetPosition().y);

	if ( event.IsGestureEnd() )
		std::cout << "Press and Tap gesture ended\n";
*/
}
///////////////////////////////////////////////////////////////////
void CncSecureGesturesPanel::trace(std::ostream& o, const wxString & context, const wxString& more) {
///////////////////////////////////////////////////////////////////
	// a simple filter
	//const bool traceIt = false;
	//const bool traceIt = callbackId > 6000;
	const bool traceIt = callbackId == 1111 || callbackId == 1110;
	
	if ( traceIt )
	{
		o	<< CNC_FORMAT("%s:\n --> TRACE: %c.%c(%d):\n",
				context,
				getOrientationAsCharacter(orientation),
				getModeAsCharacter(mode),
				callbackId
			)
		;
		
		if ( more.IsEmpty() == false )
		{
			o	<< " --> " 
				<< more 
				<< std::endl
			;
		}
		
		o	<< CNC_FORMAT(" --> zero.x=(%d|%d), centre.x=(%d|%d) knob(%d,%d)\n",
				zeroPt.x, zeroPt.y,
				centrePt.x, centrePt.y,
				knobSize.GetWidth(),
				knobSize.GetHeight()
			)
		;
		
		o	<< CNC_FORMAT(" --> IsShown=%d, IsShowOS=%d\n",
				IsShown(),
				IsShownOnScreen()
			)
		;
		
		o	<< CNC_FORMAT(" --> m_xy=(%lf, %lf), totalLen=%d, cs[%d,%d]\n",
				translatedDistance.m_x,
				translatedDistance.m_y,
				totalLen, 
				GetVirtualSize().GetWidth(),
				GetVirtualSize().GetHeight()
			)
		;
		
		o	<< " --> " 
			<< lastEvent->data  
			<< std::endl
		;
	}
}