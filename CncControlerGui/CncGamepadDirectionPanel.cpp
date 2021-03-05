#include <wx/dcclient.h>
#include <wx/graphics.h>
#include "CncGamepadDirectionPanel.h"

// ----------------------------------------------------------------------------
// CncGamepadDirectionPanel Event Table
// ----------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(CncGamepadDirectionPanel, wxPanel)
	EVT_PAINT					(CncGamepadDirectionPanel::onPaint)
	EVT_SIZE					(CncGamepadDirectionPanel::onSize)
	EVT_ERASE_BACKGROUND		(CncGamepadDirectionPanel::onEraseBackground)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////////////
CncGamepadDirectionPanel::CncGamepadDirectionPanel(wxWindow* parent)
: wxPanel(parent)
, xDir		(CncNoneDir)
, yDir		(CncNoneDir)
, angle		(-1)
/////////////////////////////////////////////////////////////////////{
{
	SetBackgroundColour(wxColour( 32,  32,  32));
}
/////////////////////////////////////////////////////////////////////
CncGamepadDirectionPanel::~CncGamepadDirectionPanel() {
/////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////
void CncGamepadDirectionPanel::onSize(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( IsShownOnScreen() )
		Refresh();
}
/////////////////////////////////////////////////////////////////////
void CncGamepadDirectionPanel::onEraseBackground(wxEraseEvent& event) {
/////////////////////////////////////////////////////////////////////
	//Refresh();
}
/////////////////////////////////////////////////////////////////////
void CncGamepadDirectionPanel::setDirection(const CncLinearDirection x, const CncLinearDirection y) {
/////////////////////////////////////////////////////////////////////
	xDir = x;
	yDir = y;
	
	if      ( xDir == CncPosDir  && yDir == CncNoneDir )	angle =   0;
	else if ( xDir == CncNegDir  && yDir == CncNoneDir )	angle = 180;
	else if ( xDir == CncNoneDir && yDir == CncPosDir  )	angle =  90;
	else if ( xDir == CncNoneDir && yDir == CncNegDir  )	angle = 270;
	else if ( xDir == CncPosDir  && yDir == CncPosDir  )	angle =  45;
	else if ( xDir == CncNegDir  && yDir == CncPosDir  )	angle = 135;
	else if ( xDir == CncPosDir  && yDir == CncNegDir  )	angle = 315;
	else if ( xDir == CncNegDir  && yDir == CncNegDir  )	angle = 225;
	else 													angle =  -1;

	if ( IsShownOnScreen() )
		Refresh();
}
/////////////////////////////////////////////////////////////////////
void CncGamepadDirectionPanel::onPaint(wxPaintEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxPaintDC dc(this);
	dc.Clear();
	
	wxGraphicsContext *gc = wxGraphicsContext::Create(dc);
	if ( gc != NULL ) {
		
		const wxRealPoint center(dc.GetSize().x / 2.0, dc.GetSize().y / 2.0);
		const double radius	= std::min(center.x, center.y) * 0.85;
		
		{
			wxGraphicsPath path = gc->CreatePath();
			gc->SetPen(wxPen(wxColour(100, 100, 100), 4, wxSOLID));
				path.AddCircle( center.x, center.y, 5.0 );
				path.AddCircle( center.x, center.y, radius );
				path.CloseSubpath();
			gc->StrokePath(path);
		}
		
		{
			wxGraphicsPath path = gc->CreatePath();
			if ( angle >= 0 ) {
				
				double xr = 0.0;
				double yr = 0.0;

				xr = round(cos(angle * PI / 180) * +(radius * 0.8));
				yr = round(sin(angle * PI / 180) * -(radius * 0.8));
				const wxRealPoint t1(center.x + xr, center.y + yr);
				
				xr = round(cos(angle * PI / 180) * +(radius * 0.6));
				yr = round(sin(angle * PI / 180) * -(radius * 0.6));
				const wxRealPoint t2(center.x + xr, center.y + yr);

				const int	 aOffset = 30;
				const double rFact   = 0.3;
				xr = round(cos((angle - aOffset) * PI / 180) * +(radius * rFact));
				yr = round(sin((angle - aOffset) * PI / 180) * -(radius * rFact));
				const wxRealPoint q1(center.x + xr, center.y + yr);
				
				xr = round(cos((angle + aOffset) * PI / 180) * +(radius * rFact));
				yr = round(sin((angle + aOffset) * PI / 180) * -(radius * rFact));
				const wxRealPoint q2(center.x + xr, center.y + yr);

				gc->SetPen(wxPen(*wxLIGHT_GREY, 2, wxSOLID));
					path.MoveToPoint(center.x, center.y);
					path.AddLineToPoint(t1.x, t1.y);
					
					path.MoveToPoint(t1.x, t1.y);
					path.AddLineToPoint(q1.x, q1.y);
					path.AddLineToPoint(t2.x, t2.y);
					
					path.MoveToPoint(t1.x, t1.y);
					path.AddLineToPoint(q2.x, q2.y);
					path.AddLineToPoint(t2.x, t2.y);

					path.CloseSubpath();
				gc->StrokePath(path);
			}
		}
		
		delete gc;
	}
}

