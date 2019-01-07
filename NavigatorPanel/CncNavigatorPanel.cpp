#include <iostream>
#include <wx/dcclient.h>
#include <wx/dcbuffer.h>
#include "CncNavigatorPanel.h"

						extern void log(const wxString& str);

						namespace cnc {

							bool dblCompare( double a, double b) {
								double eps = std::numeric_limits<double>::epsilon();
								return std::abs(a - b) <= eps; 
							}
							
							bool dblCompareNull( double a) {
								return dblCompare(a, 0.0);
							}

							
						}


						#define PI 3.14159265

// ----------------------------------------------------------------------------
// CncNavigatorPanel Event Table
// ----------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(CncNavigatorPanel, wxPanel)
	EVT_PAINT					(CncNavigatorPanel::onPaint)
	EVT_SIZE					(CncNavigatorPanel::onSize)
	EVT_MOUSE_EVENTS			(CncNavigatorPanel::onMouse)
	EVT_KEY_DOWN				(CncNavigatorPanel::onKeyDown)
	EVT_ENTER_WINDOW			(CncNavigatorPanel::onEnter)
	EVT_LEAVE_WINDOW			(CncNavigatorPanel::onLeave)
	EVT_SET_FOCUS				(CncNavigatorPanel::onSetFocus)
	EVT_KILL_FOCUS				(CncNavigatorPanel::onKillFocus)
	EVT_ERASE_BACKGROUND		(CncNavigatorPanel::onEraseBackground)
wxEND_EVENT_TABLE()

///////////////////////////////////////////////////////////////////
CncNavigatorPanel::CncNavigatorPanel(wxWindow *parent, const Config& cfg)
: wxPanel(parent)
, navEvent(new CncNavigatorPanelEvent(wxEVT_CNC_NAVIGATOR_PANEL))
, navRectangle()
, innerRadius(0)
, outerRadius(0)
, outerRegions()
, config(cfg)
, current()
///////////////////////////////////////////////////////////////////
{
	// This has to be done to use wxAutoBufferedPaintDC 
	// on EVT_PAINT events correctly
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	
	// Precreate the segment angles
	if ( config.shortFormat == false ) {
		
		const double increment = 45.0;
		const double offset    =  3.0;
		
		unsigned int idx = 0;
		for (double i = 22.5; i < 360.0; i += increment) {
			OuterCircleRegion ocr;
			ocr.startAngle = i             + offset;
			ocr.stopAngle  = i + increment - offset;
			ocr.midAngle   = ( ocr.startAngle + ocr.stopAngle ) / 2; 
			
			switch ( idx ) {
				case 0: 	ocr.direction = NE; break;
				case 1: 	ocr.direction = NN; break;
				case 2: 	ocr.direction = NW; break;
				case 3: 	ocr.direction = WW; break;
				case 4: 	ocr.direction = SW; break;
				case 5: 	ocr.direction = SS; break;
				case 6: 	ocr.direction = SE; break;
				case 7: 	ocr.direction = EE; break;
			}
			
			outerRegions.push_back(ocr);
			idx++;
		}
		
	} else {
		
		const double increment = 90.0;
		const double offset    =  3.0;
		
		unsigned int idx = 0;
		for (double i = 45.0; i < 360.0; i += increment) {
			OuterCircleRegion ocr;
			ocr.startAngle = i             + offset;
			ocr.stopAngle  = i + increment - offset;
			ocr.midAngle   = ( ocr.startAngle + ocr.stopAngle ) / 2; 
			
			switch ( idx ) {
				case 0: 	ocr.direction = NN; break;
				case 1: 	ocr.direction = WW; break;
				case 2: 	ocr.direction = SS; break;
				case 3: 	ocr.direction = EE; break;
			}
			
			outerRegions.push_back(ocr);
			idx++;
		}
	}
}
///////////////////////////////////////////////////////////////////
CncNavigatorPanel::~CncNavigatorPanel() {
///////////////////////////////////////////////////////////////////
	if ( navEvent != NULL )
		delete navEvent;
}
///////////////////////////////////////////////////////////////////
const char* CncNavigatorPanel::getDirectionAsString(const Direction d) {
///////////////////////////////////////////////////////////////////
	switch ( d ) {
		case NN:	return "North";
		case NE:	return "Nort East";
		case EE:	return "East";
		case SE:	return "South East";
		case SS:	return "South";
		case SW:	return "South West";
		case WW:	return "West";
		case NW:	return "Nort West";
		
		case CP:	return "Center Plus";
		case CN:	return "Center Minus";
		
		case UD:	return "Undefined Region";
	}
	
	return "???";
}
//////////////////////////////////////////////////
CncNavigatorPanelEvent& CncNavigatorPanel::getEvent(int id) { 
//////////////////////////////////////////////////
	wxASSERT(navEvent); 
	navEvent->reset();
	navEvent->SetEventObject(this); 
	navEvent->SetId(id); 
	
	return *navEvent; 
}
//////////////////////////////////////////////////
void CncNavigatorPanel::postEvent(const CncNavigatorPanelEvent& event) {
//////////////////////////////////////////////////
	wxPostEvent(GetParent(), event);
}
//////////////////////////////////////////////////
void CncNavigatorPanel::onEraseBackground(wxEraseEvent& event) {
//////////////////////////////////////////////////
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncNavigatorPanel::onKeyDown(wxKeyEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncNavigatorPanel::onEnter(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	typedef CncNavigatorPanelEvent::Id EID;
	enterPanel(getEvent(EID::CNP_ENTER_PANEL));
	
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncNavigatorPanel::onLeave(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	typedef CncNavigatorPanelEvent::Id EID;
	leavePanel(getEvent(EID::CNP_LEAVE_PANEL));
	
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncNavigatorPanel::onSetFocus(wxFocusEvent& event) {
///////////////////////////////////////////////////////////////////
	typedef CncNavigatorPanelEvent::Id EID;
	setFocus(getEvent(EID::CNP_SET_FOCUS));
	
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncNavigatorPanel::onKillFocus(wxFocusEvent& event) {
///////////////////////////////////////////////////////////////////
	typedef CncNavigatorPanelEvent::Id EID;
	killFocus(getEvent(EID::CNP_KILL_FOCUS));
	
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncNavigatorPanel::onPaint(wxPaintEvent& event) {
///////////////////////////////////////////////////////////////////
	wxAutoBufferedPaintDC dc(this);
	dc.Clear();
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	
	// common parameters
	const wxColour 		defaultColour  (127, 127, 127);
	const wxColour 		highlightColour(  0, 162, 232);
	const wxColour 		activatedColour(  0, 255, 255);
	const double 		midRadius = ( outerRadius + innerRadius ) / 2;
	
	
	// draw bounderies
	auto drawBounderies = [&](bool draw) {
		if ( draw == true ) {
			dc.SetPen(wxPen(*wxWHITE, 1, wxSOLID));
			dc.DrawRectangle(navRectangle);
		}
	};
	
	// draw center
	auto drawCenter = [&](bool draw) {
		if ( draw == true ) {
			dc.SetPen(wxPen(*wxRED, 1, wxSOLID));
			dc.DrawLine(-15, 0, +15, 0);
			dc.DrawLine(0, -15, 0, +15);
		}
	};
	
	// move orgin to center
	auto moveOrigin = [&]() {
		dc.SetDeviceOrigin(navRectangle.GetX() + navRectangle.GetWidth()  / 2,  
						   navRectangle.GetY() + navRectangle.GetHeight() / 2);
	};
	
	// draw navigator ( outer ) circle borders
	auto drawOuterCircleBorders = [&](const wxPen highlightPen, const wxPen defaultPen) {
		for ( auto it =outerRegions.begin(); it != outerRegions.end(); ++it ) {
			OuterCircleRegion ocr = *it;
			
			if ( ocr.direction == current.direction )	dc.SetPen(highlightPen);
			else										dc.SetPen(defaultPen);

			const int xi1 = cos(ocr.startAngle * PI / 180) * +innerRadius;
			const int yi1 = sin(ocr.startAngle * PI / 180) * -innerRadius;
			
			const int xo1 = cos(ocr.startAngle * PI / 180) * +outerRadius;
			const int yo1 = sin(ocr.startAngle * PI / 180) * -outerRadius;
			
			const int xi2 = cos(ocr.stopAngle  * PI / 180) * +innerRadius;
			const int yi2 = sin(ocr.stopAngle  * PI / 180) * -innerRadius;
			
			const int xo2 = cos(ocr.stopAngle  * PI / 180) * +outerRadius;
			const int yo2 = sin(ocr.stopAngle  * PI / 180) * -outerRadius;
			
			dc.DrawLine(xi1, yi1, xo1, yo1);
			dc.DrawLine(xi2, yi2, xo2, yo2);
			
			dc.DrawArc(xi1, yi1, xi2, yi2, 0.0, 0.0);
			dc.DrawArc(xo1, yo1, xo2, yo2, 0.0, 0.0);
			
		}
	};
	
	// draw navigator ( inner ) circle borders
	auto drawInnerCircleBorders = [&](int penWidth) {
		if ( config.innerCircle == true ) {
			
			// top region
			if ( current.direction == CP )	dc.SetPen(wxPen(highlightColour, penWidth, wxSOLID));
			else							dc.SetPen(wxPen(defaultColour,   penWidth, wxSOLID));
			
			dc.DrawArc (+innerRadius * 0.85, -5.0, -innerRadius * 0.85, -5.0, 0.0, 0.0);
			dc.DrawLine(-innerRadius * 0.85, -5.0, +innerRadius * 0.85, -5.0);
			
			// bottom region
			if ( current.direction == CN )	dc.SetPen(wxPen(highlightColour, penWidth, wxSOLID));
			else							dc.SetPen(wxPen(defaultColour,   penWidth, wxSOLID));
			
			dc.DrawArc (-innerRadius * 0.85, +5.0, +innerRadius * 0.85, +5.0, 0.0, 0.0);
			dc.DrawLine(-innerRadius * 0.85, +5.0, +innerRadius * 0.85, +5.0);
		}
	};

	
	// draw command chain
	drawBounderies(true);
	moveOrigin();
	drawCenter(false);
	drawOuterCircleBorders( current.acitvated ? wxPen(activatedColour, 2, wxSOLID) : wxPen(highlightColour, 2, wxSOLID), wxPen(defaultColour, 2, wxSOLID) );
	drawInnerCircleBorders(2);



	if ( current.direction == CP ) {
		
	} else {
		
	}
	
	if ( current.direction == CN ) {
		
	} else {
		
	}
	
	
	for ( auto it =outerRegions.begin(); it != outerRegions.end(); ++it ) {
		OuterCircleRegion ocr = *it;
		
		//dc.SetBrush(*wxTRANSPARENT_BRUSH);
		
		const int xm  = cos(ocr.midAngle   * PI / 180) * +midRadius;
		const int ym  = sin(ocr.midAngle   * PI / 180) * -midRadius;

		if ( ocr.direction == current.direction ) {
			
			if ( current.acitvated == false ) {
				
				dc.SetBrush(highlightColour);
				dc.FloodFill(xm, ym, highlightColour, wxFLOOD_BORDER );
				
			} else {
				dc.SetBrush(activatedColour);
				dc.FloodFill(xm, ym, activatedColour, wxFLOOD_BORDER );
			}
			
		} else {
			dc.SetBrush(defaultColour);
			dc.FloodFill(xm, ym, defaultColour, wxFLOOD_BORDER );
		}

	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	
	dc.SetFont(font);
	dc.DrawLabel("+X", wxRect(xm+10, ym+10, 300, 300));

	}
	
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	//drawOuterCircleBorders(wxPen(*wxBLACK, 2, wxSOLID), wxPen(*wxBLACK, 2, wxSOLID));
	
	
	
	
	
	
	const int TRIANGLE = 3;
	const int A = 0;
	const int B = 1;
	const int C = 2;
	
	dc.SetPen(wxPen(*wxBLACK, 1, wxSOLID));
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	
	// Arrows ( outer circle ) 
	for ( auto it =outerRegions.begin(); it != outerRegions.end(); ++it ) {
		OuterCircleRegion ocr = *it;
		wxColour col1;
		wxColour col2;
		
		switch ( ocr.direction ) {
			case NN: 	col1 = config.colNN; col2 = config.colNN; break;
			case SS: 	col1 = config.colSS; col2 = config.colSS; break;
			case WW: 	col1 = config.colWW; col2 = config.colWW; break;
			case EE: 	col1 = config.colEE; col2 = config.colEE; break;
			
			case NE: 	col1 = config.colNN; col2 = config.colEE; break;
			case NW: 	col1 = config.colWW; col2 = config.colNN; break;
			case SE: 	col1 = config.colEE; col2 = config.colSS; break;
			case SW: 	col1 = config.colSS; col2 = config.colWW; break;
			default: 	continue;
		}
		
		const double  hight = innerRadius * 0.18;
		const double  width = innerRadius * ( config.shortFormat ? 0.18 :  0.14 );
		
		wxPoint dp1[TRIANGLE];
		dp1[A].x = cos( ocr.midAngle          * PI / 180) * ( +(midRadius + hight));
		dp1[A].y = sin( ocr.midAngle          * PI / 180) * ( -(midRadius + hight));
		dp1[B].x = cos( ocr.midAngle          * PI / 180) * ( +(midRadius - hight));
		dp1[B].y = sin( ocr.midAngle          * PI / 180) * ( -(midRadius - hight));
		dp1[C].x = cos((ocr.midAngle + width) * PI / 180) * ( +(midRadius - hight));
		dp1[C].y = sin((ocr.midAngle + width) * PI / 180) * ( -(midRadius - hight));
		
		wxPoint dp2[TRIANGLE];
		dp2[A].x = cos( ocr.midAngle          * PI / 180) * ( +(midRadius + hight));
		dp2[A].y = sin( ocr.midAngle          * PI / 180) * ( -(midRadius + hight));
		dp2[B].x = cos((ocr.midAngle - width) * PI / 180) * ( +(midRadius - hight));
		dp2[B].y = sin((ocr.midAngle - width) * PI / 180) * ( -(midRadius - hight));
		dp2[C].x = cos( ocr.midAngle          * PI / 180) * ( +(midRadius - hight));
		dp2[C].y = sin( ocr.midAngle          * PI / 180) * ( -(midRadius - hight));
		
		dc.SetPen(wxPen(col1, 1, wxSOLID));
		dc.SetBrush(col1);
		dc.DrawPolygon(TRIANGLE, dp1);
		
		dc.SetPen(wxPen(col2, 1, wxSOLID));
		dc.SetBrush(col2);
		dc.DrawPolygon(TRIANGLE, dp2);
	}

	// Arrow CP
	if ( config.innerCircle == true ) {
		dc.SetPen(wxPen(config.colCP, 1, wxSOLID));
		dc.SetBrush(config.colCP);
		
		wxPoint dp[TRIANGLE];
		dp[A].x = 0;
		dp[A].y = -innerRadius * 0.8 / 2 - innerRadius * 0.18 - 5;
		dp[B].x = -innerRadius * 0.25;
		dp[B].y = -innerRadius * 0.8 / 2 + innerRadius * 0.18 - 5;
		dp[C].x = +innerRadius * 0.25;
		dp[C].y = -innerRadius * 0.8 / 2 + innerRadius * 0.18 - 5;
		
		dc.DrawPolygon(TRIANGLE, dp);
	}
	
	// Arrow CN
	if ( config.innerCircle == true ) {
		dc.SetPen(wxPen(config.colCN, 1, wxSOLID));
		dc.SetBrush(config.colCN);
		
		wxPoint dp[TRIANGLE];
		dp[A].x = 0;
		dp[A].y = +innerRadius * 0.8 / 2 + innerRadius * 0.18 + 5;
		dp[B].x = +innerRadius * 0.25;
		dp[B].y = +innerRadius * 0.8 / 2 - innerRadius * 0.18 + 5;
		dp[C].x = -innerRadius * 0.25;
		dp[C].y = +innerRadius * 0.8 / 2 - innerRadius * 0.18 + 5;
		
		dc.DrawPolygon(TRIANGLE, dp);
	}
}
///////////////////////////////////////////////////////////////////
void CncNavigatorPanel::onMouse(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	const int x = event.GetPosition().x;
	const int y = event.GetPosition().y;
	
	const int minX = navRectangle.GetX();
	const int maxX = minX + navRectangle.GetWidth();
	const int minY = navRectangle.GetY();
	const int maxY = minX + navRectangle.GetHeight();
	
	// check bounderies
	if ( x < minX || x > maxX || y < minY || y > maxY ) {
		current.acitvated = false;
		current.direction = UD;
		Refresh();
		
		return;
	}
	
	// normalize to a centered right hand coord-system
	MouseInfo mi;
	mi.normalizedX	= x - navRectangle.GetWidth() / 2 - navRectangle.GetX();
	mi.normalizedY	= navRectangle.GetHeight() - y - navRectangle.GetHeight() / 2 + navRectangle.GetY();
	mi.leftDown		= event.LeftDown();
	mi.leftUp		= event.LeftUp();
	
	onMouse(mi);
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncNavigatorPanel::onSize(wxSizeEvent& event) {
///////////////////////////////////////////////////////////////////
	const wxSize size = GetClientSize();
	const int margin  = 15;
	
	int navQuarderLenght  = std::min(size.GetWidth(), size.GetHeight());
	navQuarderLenght     -= (2 * margin);
	navQuarderLenght     -= (navQuarderLenght % 2);
	
	navRectangle.SetX(margin);
	navRectangle.SetY(margin);
	navRectangle.SetWidth(navQuarderLenght);
	navRectangle.SetHeight(navQuarderLenght);
	
	innerRadius = 0.4 * navQuarderLenght / 2;
	outerRadius = 0.9 * navQuarderLenght / 2;
	
	Refresh();
}
///////////////////////////////////////////////////////////////////
double CncNavigatorPanel::adjustAngle(double angle, double dx, double dy) {
///////////////////////////////////////////////////////////////////
	if ( cnc::dblCompareNull(dx) == true ) {
		if ( cnc::dblCompareNull(dy) == true )
			return 0.0;
		
		return dy > 0.0 ? angle : 360 - angle;
	}
	
	int qudrant = 0;
	if ( dx >= 0 ) 	qudrant = ( dy >= 0 ? 1 : 4 );
	else			qudrant = ( dy >= 0 ? 2 : 3 );
	
	if ( qudrant > 2 ) 
		angle = 360 - angle;
	
	return angle;
}
///////////////////////////////////////////////////////////////////
void CncNavigatorPanel::onMouse(const MouseInfo& mi) {
///////////////////////////////////////////////////////////////////
	const double radius = sqrt( pow(mi.normalizedX, 2 ) + pow(mi.normalizedY, 2 ) );
	const double angle  = adjustAngle(acos(mi.normalizedX/radius) * 180 / PI, mi.normalizedX, mi.normalizedY);
	
	typedef CncNavigatorPanelEvent::Id EID;
	
	CncNavigatorPanelEvent& evt = getEvent();
	evt.radius 	= radius;
	evt.angle	= angle;
	evt.mouseX 	= mi.normalizedX;
	evt.mouseY 	= mi.normalizedY;
	
	// Distribute coordinates
	evt.SetId(EID::CNP_COORDINATES);
	postEvent(evt);
	
	// reset the current direction
	Direction prevDirection = current.direction;
	current.direction = UD;
	
	// determine if a region is selected
	if ( radius < innerRadius * 0.85 ) {
		
		// handle inner regions
		if ( config.innerCircle == true ) {
			
			if ( abs( mi.normalizedY ) >= 5 ) {
				// detect the region
				if ( angle >= 0.0 && angle <= 180.0 )	current.direction = CP;
				else									current.direction = CN;
			}
		}
		
	} else {
		
		// handle outer regions
		if (  radius >= innerRadius && radius <= outerRadius ) {
			
			for ( auto it =outerRegions.begin(); it != outerRegions.end(); ++it ) {
				OuterCircleRegion ocr = *it;
				
				// Determine reagions angle boundings:
				//  In this case the start- or stop- angel can defined greater as 360°
				//  which has to be considered here.
				const bool greaterAsMin = ocr.stopAngle > 360.0 && angle < ocr.stopAngle - 360.0 ? angle >= 0.0                   : angle >= ocr.startAngle;
				const bool smallerAsMax = ocr.stopAngle > 360.0 && angle < ocr.stopAngle - 360.0 ? angle <= ocr.stopAngle - 360.0 : angle <= ocr.stopAngle;
				
				if ( greaterAsMin && smallerAsMax ) {
					
					// regions detected
					current.direction = ocr.direction;
					break;
				}
			}
		}
	}
	
	evt.direction = current.direction;
	
	if ( current.direction != UD ) {
		
		if ( current.direction != prevDirection ) {
			evt.SetId(EID::CNP_ENTER_REGION);
			enterRegion(evt);
		}
		
		if ( mi.leftDown == true ) {
			evt.SetId(EID::CNP_LEFT_DOWN_REGION);
			leftDownRegion(evt);
			
			evt.SetId(EID::CNP_ACTIVATE_REGION);
			current.acitvated = evt.activated = true;
			activateRegion(evt);
		}
		
		if ( mi.leftUp == true ) {
			evt.SetId(EID::CNP_LEFT_UP_REGION);
			leftDownRegion(evt);
			
			if ( current.acitvated  == true ) {
				evt.SetId(EID::CNP_DEACTIVATE_REGION);
				current.acitvated = evt.activated = false;;
				deactivateRegion(evt);
			}
		}
		
	} else {
		
		if ( current.direction != prevDirection ) {
			if ( current.acitvated  == true ) {
				evt.SetId(EID::CNP_DEACTIVATE_REGION);
				current.acitvated = evt.activated = false;;
				deactivateRegion(evt);
			}
			
			evt.SetId(EID::CNP_LEAVE_REGION);
			leaveRegion(evt);
		}
	}
	
	Refresh();
}
