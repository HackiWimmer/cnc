#include <iostream>
#include <wx/log.h>
#include <wx/scrolwin.h>
#include <wx/dcclient.h>
#include <wx/filename.h>
#include <wx/dcmemory.h>
#include "CncStringLogger.h"
#include "SVGFileParser.h"
#include "CncSvgControl.h"

BEGIN_EVENT_TABLE(CncSvgViewer, wxSVGCtrl)
	//EVT_MOUSE_EVENTS(CncSvgViewer::onMouse)
	
	EVT_LEFT_DOWN		(CncSvgViewer::onMouse)
	EVT_LEFT_UP			(CncSvgViewer::onMouse)
	EVT_LEFT_DCLICK		(CncSvgViewer::onMouse)
	EVT_MOTION			(CncSvgViewer::onMouse)
	
END_EVENT_TABLE()

/////////////////////////////////////////////////////////////////////
void CncSvgViewer::Init() {
/////////////////////////////////////////////////////////////////////
	wxSVGCtrl::Init();
	SetFitToFrame(true);
}
/////////////////////////////////////////////////////////////////////
void CncSvgViewer::onMouse(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( canSize == false )
	{
		event.Skip();
		return;
	}
	
	// -----------------------------------------------------------------
	// reset view
	if ( event.LeftDClick() )
	{
		if ( GetParent() != NULL )
		{
			wxSize pcs = GetParent()->GetClientSize();
			SetPosition(wxPoint(0, 0));
			SetClientSize(pcs);
			Refresh();
		}
		
		event.Skip();
		return ;
	}
	
	// -----------------------------------------------------------------
	// scale
	const int rot = event.GetWheelRotation();
	if ( rot != 0 ) 
	{
		if ( rot != 0 ) 
		{
			wxSize clientSize = GetClientSize();
			rot < 0 ? clientSize.Scale(0.9, 0.9) : clientSize.Scale(1.1, 1.1);
			
			SetClientSize(clientSize);
			Refresh();
		}
	}
	
	// -----------------------------------------------------------------
	// Always do this here! form a first point of view it looks a little bit crazy,
	// but we are within a wxMouseEvent callback.
	// However, if the the underlying window was left during the move and enter again
	// the previous event.LeftDown() isn't further present, therefore this . . 
	if ( wxGetMouseState().LeftIsDown() == false )
		mouseMoveMode = false;

	// -----------------------------------------------------------------
	// move
	if ( event.LeftDown() == true && mouseMoveMode == false )
	{
		mouseMoveX = event.GetX();
		mouseMoveY = event.GetY();
		
		mouseMoveMode = true;
	}
	
	// calculate new position
	if ( mouseMoveMode == true )
	{
		const int dx = (event.GetX() - mouseMoveX);
		const int dy = (event.GetY() - mouseMoveY);
		
		const wxPoint p1 = GetPosition();
		wxPoint p2 = p1;
		
		p2.x += dx;
		p2.y += dy;
		
		SetPosition(p2);
		
		if ( event.GetEventObject() != this )
		{
			// The event was triggered from a parent
			// may be the window containing this control
			
			// re-initialize to be further relative 
			mouseMoveX = event.GetX();
			mouseMoveY = event.GetY();
		}
		else
		{
			// The event was triggered by this
			
			// Re-initialize to be further relative, but 
			// in this case the position move has to be considered 
			// as a correction. Otherwise the next evaluation gets 
			// a wrong delta - see dx, dy above.
			mouseMoveX = event.GetX() - ( p2.x - p1.x );
			mouseMoveY = event.GetY() - ( p2.y - p1.y );
		}
	}
	
	// reset move mode
	if ( event.LeftUp() == true )
		mouseMoveMode = false;
	
	event.Skip();
}
//////////////////////////////////////////////////////////////////////////////
bool CncSvgViewer::loadFile(const wxString& filename, const char* contextInfo) {
//////////////////////////////////////////////////////////////////////////////
	wxFileName svgFile(filename);
	
	if ( svgFile.Exists() == false )
		return false;
		
	CncStringLogger tmpLogger;
	
	// Sometimes strange layout arise after a reload
	// Dirty fix: Clear() the control before Load()
	Clear();
	
	const bool ret = Load(filename);
	if ( ret == false )
	{
		if ( contextInfo != NULL )
			std::cerr << contextInfo << std::endl;
			
		std::cerr	<< filename << "\n" 
					<< tmpLogger.GetBuffer()
					<< std::endl;
	}
	else
	{
		CncDoubleRectangle rect;
		if ( SVGFileParser::evaluateMetricSize(filename, rect) )
		{
			boundaries.setMinBound(CncDoublePosition(rect.getX(), rect.getY(), 0.0));
			boundaries.setMaxBound(CncDoublePosition(rect.getW(), rect.getH(), 0.0));
		}
		else
		{
			boundaries.reset();
		}
	}
	
	Refresh();
	return ret;
}
//////////////////////////////////////////////////////////////////////////////
bool CncSvgViewer::getMetricBoundaries(CncDoubleBoundaries& box) {
//////////////////////////////////////////////////////////////////////////////
	box = boundaries;
	return box.hasBoundaries();
}