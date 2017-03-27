#include <iostream>
#include <cfloat>
#include <wx/dcclient.h>
#include "CncZView.h"

wxBEGIN_EVENT_TABLE(CncZView, wxPanel)
    EVT_PAINT(CncZView::OnPaint)
	EVT_MOUSE_EVENTS(CncZView::OnMouse)
	EVT_KILL_FOCUS(CncZView::OnKillFocus)
	EVT_LEAVE_WINDOW(CncZView::OnLeaveWindow)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////////////
CncZView::CncZView(wxWindow *parent, wxWindowID id) 
//////////////////////////////////////////////////////////////////////
: wxPanel(parent, id)
, lowWaterMark(0.0)
, highWaterMark(0.0)
, gravity(defaultGravity)
, scale(defaultScale)
, value(0.0)
, maxValue(50.0)
, durationThickness(2.0)
, leftMargin(10)
, rightMargin(5)
, label(DBL_MAX)
, font(7, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Segoe UI"))
, valueColour(*wxRED)
, graduationColour(128,128,128)
, posColour(0,159,0)
, negColour(255,32,32)
, labelColour(*wxWHITE)
, posBrush(posColour)
, negBrush(negColour)
, valueBrush(valueColour)
{
	SetBackgroundColour(*wxBLACK);
	SetToolTip("");
}
/////////////////////////////////////////////////////////////////////
CncZView::~CncZView() {
/////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////
void CncZView::resetAll() {
/////////////////////////////////////////////////////////////////////
	value = 0.0;
	setLowWaterMark(value);
	setHighWaterMark(value);
	
	refresh(RT_ALL);
}
/////////////////////////////////////////////////////////////////////
void CncZView::setGravity(double val) {
/////////////////////////////////////////////////////////////////////
	// sets the gravity, allowed value > 0.0 .... 1.0, else default 0.5
	if ( gravity > 0.0  && gravity <= 1.0)	gravity = val;
	else 									gravity = defaultGravity;
	
	// refesh always all
	refresh(RT_ALL);
}
/////////////////////////////////////////////////////////////////////
void CncZView::setScale(double val) {
/////////////////////////////////////////////////////////////////////
	// sets the scale, allowed value > 0.0 .... 1.0, else default 1.0
	if ( scale > 0.0  && scale <= 1.0)	scale = val;
	else 								scale = noScale;
	
	// refesh always all
	refresh(RT_ALL);
}
/////////////////////////////////////////////////////////////////////
void CncZView::refresh(RefreshType rt) {
	const wxSize cs = GetClientSize();
	
	switch ( rt ) {
		case RT_WATER_MARKS:		RefreshRect(wxRect(cs.GetWidth() - rightMargin, 0, rightMargin, cs.GetHeight()));
									break;
		case RT_GRADUATION:			RefreshRect(wxRect(0, 0, leftMargin, cs.GetHeight()));
									break;
		case RT_DATA:				RefreshRect(wxRect(leftMargin, 0, cs.GetWidth() - rightMargin, cs.GetHeight()));
									break;
		default:					Refresh(true);
	}
}
/////////////////////////////////////////////////////////////////////
void CncZView::updateView(double val, CncConfig& cc) {
/////////////////////////////////////////////////////////////////////
	if ( cc.isOnlineUpdateCoordinates() == false )
		return;
		
	durationThickness 	= cc.getMaxDurationThickness();
	maxValue			= cc.getMaxDimensionZ() * scale;
	
	value = val;
	if ( val == 0.0 || val < lowWaterMark )  setLowWaterMark(value);
	if ( val > highWaterMark ) setHighWaterMark(value);
	
	
	 
	/*
	double wpt = getCncConfig()->getWorkpieceThickness();
	double oft = getCncConfig()->getWorkpieceOffset();
	
	unsigned int max = (wpt + oft ) * getCncConfig()->getCalculationFactZ();
	setZSliderMaxValue(max * 100);
	 */
	refresh(RT_DATA);
}
/////////////////////////////////////////////////////////////////////
double CncZView::getYNull() {
/////////////////////////////////////////////////////////////////////
	const wxSize cs = GetClientSize();
	return cs.GetHeight() * gravity;
}
/////////////////////////////////////////////////////////////////////
double CncZView::convertValue(double val) {
/////////////////////////////////////////////////////////////////////
	const wxSize cs = GetClientSize();
	return val * cs.GetHeight() / maxValue;
}
/////////////////////////////////////////////////////////////////////
double CncZView::convertPosition(double val) {
/////////////////////////////////////////////////////////////////////
	return getYNull() - convertValue(val);
}
/////////////////////////////////////////////////////////////////////
void CncZView::setLowWaterMark(double val) {
/////////////////////////////////////////////////////////////////////
	lowWaterMark =  val;
	refresh(RT_WATER_MARKS);
}
/////////////////////////////////////////////////////////////////////
void CncZView::setHighWaterMark(double val) {
/////////////////////////////////////////////////////////////////////
	highWaterMark = val;
	refresh(RT_WATER_MARKS);
}
/////////////////////////////////////////////////////////////////////
void CncZView::OnPaint(wxPaintEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxPaintDC dc(this);
	const wxSize cs = GetClientSize();
	
	if ( cs.GetHeight() > 2 ) {
		dc.SetFont(font);
		
		int yNull 		= getYNull();
		int graduation 	= convertValue(durationThickness);
		
		// graduation
		dc.SetPen(wxPen(graduationColour, 1, wxSOLID));
		for ( int cnt=0, i=0; i<cs.GetHeight(); i+=graduation, cnt++) {
			int len = leftMargin;
			if ( cnt%2 != 0 )
				len /= 2;
			
			if ( yNull - i > 0 ) {
				dc.DrawLine(wxPoint(0, yNull - i), wxPoint(len, yNull - i));
				
				dc.SetTextForeground(posColour);
				dc.DrawText(wxString::Format("%d", (int)(cnt * durationThickness)), wxPoint(0, yNull - i));
			}
				
			if ( yNull - i < cs.GetHeight() ) {
				dc.DrawLine(wxPoint(0, yNull + i), wxPoint(len, yNull + i));
				
				dc.SetTextForeground(negColour);
				dc.DrawText(wxString::Format("%d", (int)(cnt * durationThickness)), wxPoint(0, yNull + i));
			}
		}
		
		double v = 0.0;
		
		// high water mark
		v = convertPosition(highWaterMark);
		dc.SetPen(wxPen(posColour, 1, wxSOLID));
		dc.DrawLine(wxPoint(cs.GetWidth() - rightMargin, v - 1), wxPoint(cs.GetWidth(), v - 1));
		dc.DrawLine(wxPoint(cs.GetWidth() - rightMargin, v + 0), wxPoint(cs.GetWidth(), v + 0));
		//dc.SetBrush(wxColour(34,117,76));
		//dc.DrawRectangle(wxRect(cs.GetWidth() - rightMargin, v, cs.GetWidth(), getYNull()));

		// low water mark
		v = convertPosition(lowWaterMark);
		dc.SetPen(wxPen(negColour, 1, wxSOLID));
		dc.DrawLine(wxPoint(cs.GetWidth() - rightMargin, v - 1), wxPoint(cs.GetWidth(), v - 1));
		dc.DrawLine(wxPoint(cs.GetWidth() - rightMargin, v + 0), wxPoint(cs.GetWidth(), v + 0));
		
		// zero marker
		dc.SetPen(wxPen(graduationColour, 1, wxSOLID));
		dc.DrawLine(wxPoint(leftMargin, yNull), wxPoint(cs.GetWidth() - rightMargin, yNull));
		
		// value
		v = convertPosition(value);
		dc.SetPen(valueColour);
		dc.DrawLine(wxPoint(leftMargin + 1, v + 0), wxPoint(cs.GetWidth() - rightMargin - 1, v + 0));
		int mid = (cs.GetWidth() - rightMargin + leftMargin) / 2 - 1;
		wxPoint points[3] = {wxPoint(mid - 3, v - 7), wxPoint(mid + 3, v - 7), wxPoint(mid, v)};
		dc.SetBrush(valueBrush);
		dc.DrawPolygon(3, points);
			
		// label
		if ( label != DBL_MAX ) {
			v = (getYNull() - label) * maxValue / cs.GetHeight();
			dc.SetTextForeground(labelColour);
			dc.DrawText(wxString::Format("%2.1f", v), wxPoint(leftMargin, label - 10));
			label = DBL_MAX;
		}
	}
	
	event.Skip(false);
}
/////////////////////////////////////////////////////////////////////
void CncZView::OnMouse(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	label = event.GetPosition().y;
	refresh(RT_DATA);
	event.Skip();
}
/////////////////////////////////////////////////////////////////////
void CncZView::OnKillFocus(wxFocusEvent& event) {
/////////////////////////////////////////////////////////////////////
	label = DBL_MAX;
	refresh(RT_DATA);
	event.Skip();
}
/////////////////////////////////////////////////////////////////////
void CncZView::OnLeaveWindow(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	label = DBL_MAX;
	refresh(RT_DATA);
	event.Skip();
}