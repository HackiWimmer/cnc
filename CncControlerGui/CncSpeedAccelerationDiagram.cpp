#include <iostream>
#include "CncSpeedAccelerationDiagram.h"
#include <wx/dcgraph.h>
#include <wx/dcclient.h>

wxBEGIN_EVENT_TABLE(CncSpeedAccelerationDiagram, wxPanel) 
	EVT_PAINT			( CncSpeedAccelerationDiagram::onPaint  )
	EVT_SIZE			( CncSpeedAccelerationDiagram::onSize   )
	EVT_MOUSE_EVENTS	( CncSpeedAccelerationDiagram::onMouse  )
wxEND_EVENT_TABLE()

//////////////////////////////////////////////////
CncSpeedAccelerationDiagram::CncSpeedAccelerationDiagram(wxWindow* parent)
: wxPanel		(parent)
, graphList		()
, minD			(0)
, maxD			(0)
, minF			(0)
, maxF			(0)
, xUnit			("???")
, yUnit			("mm/min")
, showImpulse	(true)
//////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////
CncSpeedAccelerationDiagram::~CncSpeedAccelerationDiagram() {
//////////////////////////////////////////////////
	reset();
}
//////////////////////////////////////////////////
void CncSpeedAccelerationDiagram::reset() {
 //////////////////////////////////////////////////
	graphList.clear();
	minD 	= 0;
	maxD 	= 0;
	minF 	= 0;
	maxF 	= 0;
}
//////////////////////////////////////////////////
void CncSpeedAccelerationDiagram::display() {
//////////////////////////////////////////////////
	Refresh(true);
}
//////////////////////////////////////////////////
void CncSpeedAccelerationDiagram::initUnit(const wxString& xUnit, const wxString& yUnit) {
//////////////////////////////////////////////////
	this->xUnit = xUnit;
	this->yUnit = yUnit;

}
//////////////////////////////////////////////////
void CncSpeedAccelerationDiagram::initGraphCount(unsigned int c) { 
//////////////////////////////////////////////////
	graphList.clear();
	
	for ( unsigned int i=0; i<c; i++)
		graphList.push_back(Graph());
}
//////////////////////////////////////////////////
CncSpeedAccelerationDiagram::Graph* CncSpeedAccelerationDiagram::getGraph(unsigned int graphIndex) {
//////////////////////////////////////////////////
	if ( graphIndex > graphList.size() - 1 )
		return NULL;
		
	return &graphList[graphIndex];
}
//////////////////////////////////////////////////
bool CncSpeedAccelerationDiagram::clearGraph(unsigned int graphIndex) {
//////////////////////////////////////////////////
	CncSpeedAccelerationDiagram::Graph* g = getGraph(graphIndex);
	if ( g == NULL )
		return false;
	
	g->points.clear();
	return true;
}
//////////////////////////////////////////////////
bool CncSpeedAccelerationDiagram::appendPointToGraph(unsigned int graphIndex, const wxRealPoint& p, const Impulse& i) {
//////////////////////////////////////////////////
	CncSpeedAccelerationDiagram::Graph* g = getGraph(graphIndex);
	if ( g == NULL )
		return false;
		
	Point point;
	point.p = p;
	point.i = i;
	point.p.x -= minD;
	
	g->points.push_back(point);
	return true;
}
//////////////////////////////////////////////////
void CncSpeedAccelerationDiagram::initRange(int32_t minF, int32_t maxF, int32_t minD, int32_t maxD) {
//////////////////////////////////////////////////
	if (  minF >= maxF )
		return;

	this->minD	= minD;
	this->maxD	= maxD;
	this->minF 	= minF;
	this->maxF	= maxF;
	
	display();
}
//////////////////////////////////////////////////
void CncSpeedAccelerationDiagram::onMouse(wxMouseEvent& event) {
//////////////////////////////////////////////////
	const wxSize cs = GetClientSize();
	const int x = event.GetX()   -     BORDER;
	const int y = cs.GetHeight() -     BORDER - event.GetY();
	const int w = cs.GetWidth()  - 2 * BORDER;
	const int h = cs.GetHeight() - 2 * BORDER;
	
	const float X = maxD && w ? minD + ( x * (maxD - minD) / w ) : -42;
	const float Y = maxF && h ? minF + ( y * (maxF - minF) / h ) : -42;
	
	SetToolTip(wxString::Format("%.1lf, %.1lf", X, Y));
}
//////////////////////////////////////////////////
void CncSpeedAccelerationDiagram::onSize(wxSizeEvent& event) {
//////////////////////////////////////////////////
	display();
}
//////////////////////////////////////////////////
wxPoint CncSpeedAccelerationDiagram::cnvH2Graph(double x, double y) {
//////////////////////////////////////////////////
	const wxSize cs = GetClientSize();
	const int w = cs.GetWidth() - 2 * BORDER;
	
	wxPoint ret;
	ret.x = maxD ? BORDER + ( x * w ) / (maxD - minD) : 0;
	ret.y = y;
	
	return ret;
}
//////////////////////////////////////////////////
wxPoint CncSpeedAccelerationDiagram::cnvHV2Graph(double x, double y) {
//////////////////////////////////////////////////
	const wxSize cs = GetClientSize();
	const int w = cs.GetWidth()  - 2 * BORDER;
	const int h = cs.GetHeight() - 2 * BORDER;
	
	wxPoint ret;
	ret.x = maxD ? BORDER                  + ( x * w ) / (maxD - minD) : 0;
	ret.y = maxF ? cs.GetHeight() - BORDER - ( y * h ) / (maxF - minF) : 0;
	
	return ret;
}
//////////////////////////////////////////////////
void CncSpeedAccelerationDiagram::onPaint(wxPaintEvent& event) {
//////////////////////////////////////////////////
	wxPaintDC paintDC(this);
	paintDC.Clear();
	wxGCDC dc(paintDC);
	
	const wxSize cs = GetClientSize();
	
	const int32_t w = cs.GetWidth()  - 2 * BORDER;
	const int32_t h = cs.GetHeight() - 2 * BORDER;
	const int32_t maxDiff = maxF - minF;
	
	const wxPoint pLT(BORDER,			BORDER);
	const wxPoint pRT(BORDER + w,		BORDER);
	const wxPoint pLB(BORDER,			BORDER + h);
	const wxPoint pRB(BORDER + w,		BORDER + h);
	const wxPoint pLM(BORDER,			BORDER + h / 2);
	const wxPoint pRM(BORDER + w,		BORDER + h / 2);
	const wxPoint pMT(BORDER + w / 2, 	BORDER);
	const wxPoint pMB(BORDER + w / 2, 	BORDER + h);
	
	// draw axis
	{
		const wxColour colAxis(*wxYELLOW);
		dc.SetPen(wxPen(colAxis, 1, wxSOLID));
		
		// axis
		dc.DrawLine(pLT, pLB);
		dc.DrawLine(pLB, pRB);
		
		// marker f axis
		dc.DrawLine(pLT + wxSize( -3, +0) , pLT + wxSize( +3, +0));
		dc.DrawLine(pLM + wxSize( -3, +0) , pLM + wxSize( +3, +0));
		
		// marker s axis
		dc.DrawLine(pMB + wxSize( +0, -3) , pMB + wxSize( +0, +3));
		dc.DrawLine(pRB + wxSize( +0, -3) , pRB + wxSize( +0, +3));

		//help lines 
		dc.SetPen(wxPen(colAxis.ChangeLightness(32), 1, wxDOT));
		dc.DrawLine(pLT, pRT);
		dc.DrawLine(pLM, pRM);
		dc.DrawLine(pMT, pMB);
		dc.DrawLine(pRT, pRB);
		
		// labels
		const wxFont font(7, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
		dc.SetFont(font); 
		dc.SetTextForeground(colAxis.ChangeLightness(64));
		
		dc.DrawText(wxString::Format("%d", minF), 				pLB + wxSize(  -30, -10));
		dc.DrawText(wxString::Format("%d", (minF + maxF) / 2),	pLM + wxSize(  -30, +0));
		dc.DrawText(wxString::Format("%d", maxF), 				pLT + wxSize(  -30, +0));
		
		dc.DrawText(wxString::Format("%d", minD),				pLB + wxSize(  +25, +2));
		dc.DrawText(wxString::Format("%d", (minD + maxD) / 2), 	pMB + wxSize(  -25, +2));
		dc.DrawText(wxString::Format("%d", maxD), 				pRB + wxSize(  -25, +2));

		dc.DrawText(wxString::Format("[%s]", yUnit),		 	pLT + wxSize(  -30, -15));
		dc.DrawText(wxString::Format("[%s]", xUnit), 			pRB + wxSize(  -30, +15));
	}

	if ( maxDiff <= 0 )
		return;
		
	// draw graphs
	wxPen implX((wxColour(255, 128, 128)).ChangeLightness(64), 1, wxSOLID);
	wxPen implY((wxColour(  0, 128, 192)).ChangeLightness(64), 1, wxSOLID);
	wxPen implZ((wxColour(  0, 255,   0)).ChangeLightness(64), 1, wxSOLID);
	
	for ( auto gIt = graphList.begin(); gIt != graphList.end(); ++gIt ) {
		
		dc.SetPen(gIt->pen);
		
		// draw graph
		if ( gIt->points.size() > 1 ) {
			Point& point = *(gIt->points.begin());
			wxRealPoint p1 = point.p;
			wxRealPoint p2 = p1;
			Impulse     i2;
			
			for ( auto pIt = gIt->points.begin() + 1; pIt != gIt->points.end(); ++pIt ) {
				p2 = pIt->p;
				i2 = pIt->i;
				
				// main graph// optimization - simple, the last y value wins
				if ( p2.x != p1.x ) {
					
					// impulse
					if ( showImpulse == true ) {
						const uint16_t yRef = pLT.y;
						const uint16_t len  = (pLB.y - pLT.y ) / 3;
						wxRealPoint ip1(p2), ip2(p2);
						
						if ( i2.x != 0 ) { ip1.y = yRef + 0 * len + 1; ip2.y = ip1.y + len; dc.SetPen(implX); dc.DrawLine(cnvH2Graph(ip1), cnvH2Graph(ip2)); }
						if ( i2.y != 0 ) { ip1.y = yRef + 1 * len + 1; ip2.y = ip1.y + len; dc.SetPen(implY); dc.DrawLine(cnvH2Graph(ip1), cnvH2Graph(ip2)); }
						if ( i2.z != 0 ) { ip1.y = yRef + 2 * len + 1; ip2.y = ip1.y + len; dc.SetPen(implZ); dc.DrawLine(cnvH2Graph(ip1), cnvH2Graph(ip2)); }
					}
					
					// main graph
					dc.SetPen(gIt->pen);
					dc.DrawLine(cnvHV2Graph(p1), cnvHV2Graph(p2));
				}
				
				//swap
				p1 = p2;
			}
			
		} else {
			
			if ( gIt->points.size() > 0 ) {
				Point& point 	= *(gIt->points.begin());
				wxRealPoint p1	= point.p;
				dc.DrawPoint(cnvHV2Graph(p1));
			}
		}
	}
}
