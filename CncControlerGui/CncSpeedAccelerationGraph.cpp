#include "CncSpeedAccelerationDiagram.h"
#include <wx/dcclient.h>

wxBEGIN_EVENT_TABLE(CncSpeedAccelerationDiagram, wxPanel) 
	EVT_PAINT			( CncSpeedAccelerationDiagram::onPaint)
	EVT_SIZE			( CncSpeedAccelerationDiagram::onSize   )
	EVT_MOUSE_EVENTS	( CncSpeedAccelerationDiagram::onMouse  )
wxEND_EVENT_TABLE()

//////////////////////////////////////////////////
CncSpeedAccelerationDiagram::CncSpeedAccelerationDiagram(wxWindow* parent)
: wxPanel(parent)
, graphList()
, totS_MM(0)
, minF_MMMin(0)
, maxF_MMMin(0)
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
	totS_MM 	= 0;
	minF_MMMin 	= 0;
	maxF_MMMin 	= 0;
}
//////////////////////////////////////////////////
void CncSpeedAccelerationDiagram::display() {
//////////////////////////////////////////////////
	Refresh(true);
	Update();
}
//////////////////////////////////////////////////
void CncSpeedAccelerationDiagram::initGraphCount(unsigned int c) { 
//////////////////////////////////////////////////
	graphList.clear();
	
	for ( unsigned int i=0; i<c; i++)
		graphList.push_back(Graph());
}
//////////////////////////////////////////////////
CncSpeedAccelerationDiagram::Graph* CncSpeedAccelerationDiagram::getGraph(unsigned int index) {
//////////////////////////////////////////////////
	if ( index > graphList.size() - 1 )
		return NULL;
		
	return &graphList[index];
}
//////////////////////////////////////////////////
void CncSpeedAccelerationDiagram::initRange(int32_t minF_MMMin, int32_t maxF_MMMin, int32_t totS_MM) {
//////////////////////////////////////////////////
	if (  minF_MMMin >= maxF_MMMin )
		return;

	this->totS_MM 		= totS_MM;
	this->minF_MMMin 	= minF_MMMin;
	this->maxF_MMMin 	= maxF_MMMin;
	
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
	
	const int X = totS_MM    && w ? ( x * totS_MM )    / w : -42;
	const int Y = maxF_MMMin && h ? ( y * maxF_MMMin ) / h : -42;
	
	SetToolTip(wxString::Format("%d, %d", X, Y));
}
//////////////////////////////////////////////////
void CncSpeedAccelerationDiagram::onSize(wxSizeEvent& event) {
//////////////////////////////////////////////////
	display();
}
//////////////////////////////////////////////////
wxPoint CncSpeedAccelerationDiagram::cnv2Graph(int32_t x, int32_t y) {
//////////////////////////////////////////////////
	const wxSize cs = GetClientSize();
	const int w = cs.GetWidth()  - 2 * BORDER;
	const int h = cs.GetHeight() - 2 * BORDER;
	
	wxPoint ret;
	
	ret.x = totS_MM    ? BORDER                  + ( x * w ) / totS_MM    : 0;
	ret.y = maxF_MMMin ? cs.GetHeight() - BORDER - ( y * h ) / maxF_MMMin : 0;
	
	return ret;
}
//////////////////////////////////////////////////
wxPoint CncSpeedAccelerationDiagram::cnv2Graph(wxPoint p) {
//////////////////////////////////////////////////
	return cnv2Graph(p.x, p.y);
}
//////////////////////////////////////////////////
void CncSpeedAccelerationDiagram::onPaint(wxPaintEvent& event) {
//////////////////////////////////////////////////
	wxPaintDC dc(this);
	const wxSize cs = GetClientSize();
	
	const int32_t w = cs.GetWidth()  - 2 * BORDER;
	const int32_t h = cs.GetHeight() - 2 * BORDER;
	const int32_t maxDiff = maxF_MMMin - minF_MMMin;
	
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
		
		dc.DrawText(wxString::Format("%d", 0), 				pLB + wxSize(  -10, +2));
		
		dc.DrawText(wxString::Format("%d", maxF_MMMin / 2), pLM + wxSize(  -20, +0));
		dc.DrawText(wxString::Format("%d", maxF_MMMin), 	pLT + wxSize(  -20, +0));
		
		dc.DrawText(wxString::Format("%d", totS_MM / 2), 	pMB + wxSize(  -25, +2));
		dc.DrawText(wxString::Format("%d", totS_MM), 		pRB + wxSize(  -25, +2));
	}

	if ( maxDiff <= 0 )
		return;

    // draw graphs
	for ( auto gIt = graphList.begin(); gIt != graphList.end(); ++gIt ) {
		
		dc.SetPen(wxPen(gIt->colour, 1, wxLONG_DASH));
		
		// draw graph
		if ( gIt->points.size() > 1 ) {
			wxPoint p1 = *(gIt->points.begin());
			wxPoint p2 = p1;
			
			for ( auto pIt = gIt->points.begin() + 1; pIt != gIt->points.end(); ++pIt ) {
				p2 = *pIt;
				dc.DrawLine(cnv2Graph(p1), cnv2Graph(p2));
				
				//swap
				p1 = p2;
			}
		} else {
			wxPoint p1 = *(gIt->points.begin());
			dc.DrawPoint(p1);
		}
	}
}