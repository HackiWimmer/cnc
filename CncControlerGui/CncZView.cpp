#include <iostream>
#include <cfloat>
#include <wx/dcclient.h>
#include "CncCommon.h"
#include "CncBoundarySpace.h"
#include "CncContext.h"
#include "CncConfig.h"
#include "CncZView.h"

wxBEGIN_EVENT_TABLE(CncZView, wxPanel)
    EVT_PAINT			(CncZView::OnPaint)
	EVT_MOUSE_EVENTS	(CncZView::OnMouse)
	EVT_KILL_FOCUS		(CncZView::OnKillFocus)
	EVT_LEAVE_WINDOW	(CncZView::OnLeaveWindow)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////////////
CncZView::CncZView(wxWindow *parent, wxWindowID id) 
//////////////////////////////////////////////////////////////////////
: wxPanel				(parent, id)
, lowWaterMark			(0.0)
, highWaterMark			(0.0)
, gravity				(defaultGravity)
, scale					(defaultScale)
, value					(0.0)
, maxValue				(50.0)
, durationThickness		(2.0)
, workPieceThickness	(0)
, workPieceOffset		(0)
, label					(INT_MAX)
, font					(7, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Segoe UI"))
, valueColour			(255,128,128)
, graduationColour		(128,128,128)
, posColour				(0,159,0)
, negColour				(255,32,32)
, labelColour			(*wxWHITE)
, wptColour				(255,217,83)
, wpoColour				(234,181,0)
, wpsColour				(128,64,64)
, posBrush				(posColour)
, negBrush				(negColour)
, valueBrush			(valueColour)
, wptBrush				(wptColour)
, wpoBrush				(wpoColour)
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
	resetWaterMarks();
	refresh(RT_ALL);
}
/////////////////////////////////////////////////////////////////////
void CncZView::resetWaterMarks() {
/////////////////////////////////////////////////////////////////////
	setLowWaterMark(0.0);
	setHighWaterMark(0.0);
}
/////////////////////////////////////////////////////////////////////
void CncZView::setGravity(double val) {
/////////////////////////////////////////////////////////////////////
	// sets the gravity, allowed value > 0.0 .... 1.0, else default 0.5
	if ( val > 0.0  && val <= 1.0)	gravity = val;
	else 							gravity = defaultGravity;
	
	// refesh always all
	refresh(RT_ALL);
}
/////////////////////////////////////////////////////////////////////
void CncZView::setScale(double val, double errorValue) {
/////////////////////////////////////////////////////////////////////
	wxASSERT( scale > 0.0 );
	double unscaledMaxValue = maxValue / scale;

	// sets the scale, allowed value > 0.0 .... 1.0, else default 1.0
	if ( val > 0.1  && val <= 1.0)	scale = val;
	else 							scale = errorValue;
	
	maxValue = unscaledMaxValue * scale;
	
	// refesh always all
	refresh(RT_ALL);
}
/////////////////////////////////////////////////////////////////////
void CncZView::refresh(RefreshType rt) {
/////////////////////////////////////////////////////////////////////
	const wxSize cs = GetClientSize();
	
	switch ( rt ) {
		case RT_GRADUATION:			RefreshRect(wxRect(getGraduationOffsetX(), 	0, getGraduationWidth(), 	cs.GetHeight()));
									break;
		case RT_DATA:				RefreshRect(wxRect(getDataOffsetX(), 		0, getDataWidth(), 			cs.GetHeight()));
									break;
		case RT_WORKPIECE:			RefreshRect(wxRect(getWorkpieceOffsetX(), 	0, getWorkpieceWidth(), 	cs.GetHeight()));
									break;
		case RT_WATER_MARKS:		RefreshRect(wxRect(getWaterMarkOffsetX(), 	0, getWaterMarkWidth(), 	cs.GetHeight()));
									break;
		case RT_LABEL:				RefreshRect(wxRect(getDataOffsetX(), 		0, cs.GetWidth(), 			cs.GetHeight()));
									break;
		default:					Refresh(true);
	}
}
/////////////////////////////////////////////////////////////////////
void CncZView::setYNull(int pos) {
/////////////////////////////////////////////////////////////////////
	const wxSize cs = GetClientSize();
	
	if ( pos > 10 && pos < cs.GetHeight() - 10)
		setGravity((double)pos/cs.GetHeight());
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
	lowWaterMark = val;
	refresh(RT_WATER_MARKS);
}
/////////////////////////////////////////////////////////////////////
void CncZView::setHighWaterMark(double val) {
/////////////////////////////////////////////////////////////////////
	highWaterMark = val;
	refresh(RT_WATER_MARKS);
}
/////////////////////////////////////////////////////////////////////
void CncZView::updateView(double val) {
/////////////////////////////////////////////////////////////////////
	if ( THE_CONTEXT->isOnlineUpdateCoordinates() == false )
		return;
	
	if ( THE_CONTEXT->isProcessing() == false )
	{
		if ( cnc::dblCompare(maxValue, THE_BOUNDS->getMaxDimensionMetricZ() * scale) == false )
		{
			maxValue = THE_BOUNDS->getMaxDimensionMetricZ() * scale;
			resetWaterMarks();
			refresh(RT_ALL);
		}
		
		if ( cnc::dblCompare(durationThickness, THE_CONFIG->getMaxDurationThickness()) == false )
		{
			durationThickness = THE_CONFIG->getMaxDurationThickness();
			resetWaterMarks();
			refresh(RT_ALL);
		}
		
		if ( cnc::dblCompare(workPieceThickness, THE_BOUNDS->getWorkpieceThickness()) == false )
		{
			resetWaterMarks();
			workPieceThickness = THE_BOUNDS->getWorkpieceThickness();
			refresh(RT_WORKPIECE);
		}
		
		if ( cnc::dblCompare(workPieceOffset, THE_CONFIG->getWorkpieceOffset()) == false ) 
		{
			workPieceOffset = THE_CONFIG->getWorkpieceOffset();
			resetWaterMarks();
			refresh(RT_WORKPIECE);
		}
	}
	
	if ( cnc::dblCompare(value, val) == false ) 
	{
		value = val;
		if ( value == 0.0 || value < lowWaterMark )		setLowWaterMark(value);
		if ( value > highWaterMark ) 					setHighWaterMark(value);
		
		// adjust visible range
		while ( convertPosition(value) < 0 ) 
		{
			setScale(scale + 0.1, 1.0);
			
			if (scale >= 1.0 )
				break;
		}
		
		refresh(RT_DATA);
	}
}
/////////////////////////////////////////////////////////////////////
void CncZView::drawGraduation(wxPaintDC& dc, int yNull) {
/////////////////////////////////////////////////////////////////////
	const wxSize cs = GetClientSize();
	int graduation = convertValue(durationThickness);

	dc.SetPen(wxPen(graduationColour, 1, wxSOLID));
	dc.DrawLine(wxPoint(getGraduationOffsetX(), 0), wxPoint(getGraduationOffsetX(), cs.GetHeight()));
	
	for ( int cnt=0, i=0; i<cs.GetHeight(); i+=graduation, cnt++) {
		int len = getGraduationWidth();
		if ( cnt%2 != 0 )
			len /= 2;
			
		bool text = true;
		if ( scale > 0.5 )
			text = (cnt%2 != 0);
		
		if ( yNull - i > 0 ) {
			dc.DrawLine(wxPoint(getGraduationOffsetX(), yNull - i), wxPoint(getGraduationOffsetX() + len, yNull - i));
			
			dc.SetTextForeground(posColour);
			if ( text == true )
				dc.DrawText(wxString::Format("%d", (int)(cnt * durationThickness)), wxPoint(getGraduationOffsetX(), yNull - i));
		}
			
		if ( yNull - i < cs.GetHeight() ) {
			dc.DrawLine(wxPoint(getGraduationOffsetX(), yNull + i), wxPoint(getGraduationOffsetX() + len, yNull + i));
			
			dc.SetTextForeground(negColour);
			if ( text == true )
				dc.DrawText(wxString::Format("%d", (int)(cnt * durationThickness)), wxPoint(getGraduationOffsetX(), yNull + i));
		}
	}
}
/////////////////////////////////////////////////////////////////////
void CncZView::OnPaint(wxPaintEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxPaintDC dc(this);
	const wxSize cs = GetClientSize();
	
	if ( cs.GetHeight() > 2 ) {
		dc.SetFont(font);
		
		int yNull = getYNull();
		double v  = 0.0;
		
		// graduation
		drawGraduation(dc, yNull);

		// high water mark
		dc.SetPen(wxPen(posColour, 1, wxSOLID));
		dc.SetBrush(posColour);
		dc.DrawRectangle(wxRect(getWaterMarkOffsetX(), convertPosition(highWaterMark), getWorkpieceWidth(), convertValue(highWaterMark)));

		// low water mark
		dc.SetPen(wxPen(negColour, 1, wxSOLID));
		dc.SetBrush(negColour);
		dc.DrawRectangle(wxRect(getWaterMarkOffsetX(), convertPosition(lowWaterMark), getWorkpieceWidth(), convertValue(lowWaterMark)));
		
		// workpiece thickness
		dc.SetBrush(wptBrush);
		dc.SetPen(wptColour);
		dc.DrawRectangle(getWorkpieceOffsetX(), convertPosition(workPieceThickness), getWorkpieceWidth(), convertValue(workPieceThickness));
		
		// workpiece offset
		dc.SetBrush(wpoBrush);
		dc.SetPen(wpoColour);
		dc.DrawRectangle(getWorkpieceOffsetX(), convertPosition(workPieceThickness + workPieceOffset), getWorkpieceWidth(), convertValue(workPieceOffset));
		
		// zero marker
		dc.SetPen(wxPen(graduationColour, 1, wxSOLID));
		dc.DrawLine(wxPoint(getDataOffsetX(), yNull), wxPoint(getWidth(), yNull));
		
		// value
		v = convertPosition(value);
		dc.SetPen(valueColour);
		
		dc.DrawLine(wxPoint(getDataOffsetX(), v), wxPoint(getDataOffsetX() + getDataWidth() - 1, v));
		wxPoint points[3] = {wxPoint(getDataOffsetX() + 5, v - 5), wxPoint(getDataOffsetX() + 5, v + 5), wxPoint(getDataOffsetX(), v)};
		dc.SetBrush(valueBrush);
		dc.DrawPolygon(3, points);
		
		// label
		if ( label != INT_MAX ) {
			v = (getYNull() - label) / cs.GetHeight() * maxValue;
			dc.SetTextForeground(labelColour);
			dc.DrawText(wxString::Format("%2.1f", v), wxPoint(getDataOffsetX(), label - 10));
			label = INT_MAX;
		}
	}
	
	event.Skip(false);
}
/////////////////////////////////////////////////////////////////////
void CncZView::OnMouse(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( event.ControlDown() ) {
		// move gravity (zero position)
		setYNull(event.GetPosition().y);
	} else {
		if ( event.LeftDClick() ) {
			resetWaterMarks();
		} else {
			// setup label
			label = event.GetPosition().y;
			refresh(RT_LABEL);
		}
	}
	
	// scale view 
	int rot = event.GetWheelRotation();
	if ( rot != 0 ) {
		setScale(scale + (rot > 0 ? +0.1 : -0.1), scale);
	}
	event.Skip();
}
/////////////////////////////////////////////////////////////////////
void CncZView::OnKillFocus(wxFocusEvent& event) {
/////////////////////////////////////////////////////////////////////
	label = INT_MAX;
	refresh(RT_LABEL);
	event.Skip();
}
/////////////////////////////////////////////////////////////////////
void CncZView::OnLeaveWindow(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	label = INT_MAX;
	refresh(RT_LABEL);
	event.Skip();
}