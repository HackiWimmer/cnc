#include <wx/dcclient.h>
#include "CfgAccelerationGraph.h"

wxBEGIN_EVENT_TABLE(CfgAccelerationGraph, wxPanel)
	EVT_PAINT(CfgAccelerationGraph::onPaint)
	EVT_SIZE(CfgAccelerationGraph::onSize)
	//EVT_ERASE_BACKGROUND(CfgAccelerationGraph::onEraseBackground)
	//EVT_MOUSE_EVENTS(CfgAccelerationGraph::onMouse)
	//EVT_KEY_DOWN(CfgAccelerationGraph::onKeyDown)
wxEND_EVENT_TABLE()

//////////////////////////////////////////////////
CfgAccelerationGraph::CfgAccelerationGraph(wxWindow* parent)
: wxPanel(parent)
, speedController(NULL)
, graph()
, minValue(0)
, maxValue(0)
, stepsToMove(0)
, startMarker(0)
, stopMarker(0)
//////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////
CfgAccelerationGraph::~CfgAccelerationGraph() {
//////////////////////////////////////////////////
	reset();
}
//////////////////////////////////////////////////
void CfgAccelerationGraph::reset() {
//////////////////////////////////////////////////
	graph.clear();
	minValue 		= 0;
	maxValue 		= 0;
	stepsToMove 	= 0;
	startMarker		= 0;
	stopMarker		= 0;
}
//////////////////////////////////////////////////
void CfgAccelerationGraph::init(CncSpeedController* sc) {
//////////////////////////////////////////////////
	speedController = sc;
}
//////////////////////////////////////////////////
bool CfgAccelerationGraph::calculate(char axis, int32_t stm) {
//////////////////////////////////////////////////
	if ( speedController == NULL )
		return false;
		
	reset();
	
	CncSpeedController::Axis* A = NULL;
	int32_t dx = 0, dy = 0, dz = 0;
	switch ( axis ) {
		case 'X':	A = &(speedController->X); dx = stm; break;
		case 'Y':	A = &(speedController->Y); dy = stm; break;
		case 'Z':	A = &(speedController->Z); dz = stm; break;
		default:	return false;
	}
	
	speedController->initMove(dx, dy, dz);
	
	stepsToMove = abs(stm);
	startMarker	= A->AP.startStepCountMark;
	stopMarker	= A->AP.stopStepCountMark;
	
	for ( uint32_t s = 0; s < stepsToMove; s++ ) {
		
		Point p;
		p.value = A->AP.getNextAccelDelay() + A->synthSpeedDelay + A->totalOffset;
		p.step  = s;
		graph.push_back(p);
		
		minValue = std::min(minValue, p.value);
		maxValue = std::max(maxValue, p.value);
	}
	
	return true;
}
//////////////////////////////////////////////////
void CfgAccelerationGraph::display() {
//////////////////////////////////////////////////
	Refresh(true);
	Update();
}
//////////////////////////////////////////////////
void CfgAccelerationGraph::onSize(wxSizeEvent& event) {
//////////////////////////////////////////////////
	display();
}
//////////////////////////////////////////////////
void CfgAccelerationGraph::onPaint(wxPaintEvent& event) {
// displayes are the acceleration delays and they are
// inversely proportional to the acceleration
//////////////////////////////////////////////////
	wxPaintDC dc(this);
	const wxSize cs = GetClientSize();
	
	const int border 		= 5;
	const int32_t w  		= cs.GetWidth()  - 2 * border;
	const int32_t h      	= cs.GetHeight() - 2 * border;
	const int32_t maxDiff   = maxValue - minValue;
	dc.SetPen(wxPen(*wxWHITE, 1, wxSOLID));
	
	// draw graph
	for ( auto it = graph.begin(); it != graph.end(); it++) {
		Point& p = *it;
		//std::cout << p.step << ", " << p.value << std::endl;
		dc.DrawPoint(wxPoint(border + p.step  * w / stepsToMove, 
		                     border + p.value * h / maxDiff));
	}
	
	// draw bounds
	dc.SetPen(wxPen(*wxYELLOW, 1, wxLONG_DASH));
	dc.DrawLine(wxPoint(border, border - minValue * h / maxDiff),     wxPoint(border - w, border + minValue * h / maxDiff));
	dc.DrawLine(wxPoint(border, border + h - maxValue * h / maxDiff), wxPoint(border + w, border + h - maxValue * h / maxDiff));
	
	// drwaw axis
	dc.SetPen(wxPen(*wxRED, 1, wxSOLID));
	dc.DrawLine(wxPoint(border, border), wxPoint(border, border + h));
	
	wxFont font(7, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	dc.SetFont(font);
	dc.SetTextForeground(*wxRED);
	
	dc.SetPen(wxPen(*wxRED, 1, wxDOT));
	if ( startMarker != 0 ) {
		dc.DrawLine(wxPoint(border + startMarker * w / stepsToMove, border), wxPoint(border + startMarker * w / stepsToMove, border + h));
		dc.DrawText(wxString::Format("%d", startMarker), wxPoint(border + 2 + startMarker * w / stepsToMove, border + 2));
	}
		
	if ( stopMarker != 0 ) {
		dc.DrawLine(wxPoint(border + stopMarker * w / stepsToMove, border), wxPoint(border + stopMarker * w / stepsToMove, border + h));
		dc.DrawText(wxString::Format("%d", startMarker), wxPoint(border + 2 + stopMarker * w / stepsToMove, border + 2));
	}
}