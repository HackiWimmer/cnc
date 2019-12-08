#include <wx/dcclient.h>
#include "CncMotionMonitor.h"
#include "GL3DDrawPane.h"

/////////////////////////////////////////////////////////////////
GL3DDrawPane::GL3DDrawPane(wxWindow* parent)
: GL3DDrawPaneBase(parent, wxID_ANY, wxDefaultPosition, wxSize(-1,-1),  wxTAB_TRAVERSAL)
, GLContextOptions::Callback()
, CncMotionMonitor::Callback()
, motionMonitor(NULL)
/////////////////////////////////////////////////////////////////
{
	// bind
	GetRotatePaneX3D()->Bind(wxEVT_PAINT, 	&GL3DDrawPane::onPaintRotatePaneX3D, 	this);
	GetRotatePaneY3D()->Bind(wxEVT_PAINT, 	&GL3DDrawPane::onPaintRotatePaneY3D, 	this);
	GetRotatePaneZ3D()->Bind(wxEVT_PAINT, 	&GL3DDrawPane::onPaintRotatePaneZ3D, 	this);
	GetScalePane3D()->Bind(wxEVT_PAINT, 	&GL3DDrawPane::onPaintScalePane3D, 		this);
	
	GetScalePane3D()->SetBackgroundColour(*wxBLACK);
	GetRotatePaneX3D()->SetBackgroundColour(*wxBLACK);
	GetRotatePaneY3D()->SetBackgroundColour(*wxBLACK);
	GetRotatePaneZ3D()->SetBackgroundColour(*wxBLACK);
}
/////////////////////////////////////////////////////////////////
GL3DDrawPane::~GL3DDrawPane() {
/////////////////////////////////////////////////////////////////
	// unbind
	GetRotatePaneX3D()->Unbind(wxEVT_PAINT, 	&GL3DDrawPane::onPaintRotatePaneX3D, 	this);
	GetRotatePaneY3D()->Unbind(wxEVT_PAINT, 	&GL3DDrawPane::onPaintRotatePaneY3D, 	this);
	GetRotatePaneZ3D()->Unbind(wxEVT_PAINT, 	&GL3DDrawPane::onPaintRotatePaneZ3D, 	this);
	GetScalePane3D()->Unbind(wxEVT_PAINT, 		&GL3DDrawPane::onPaintScalePane3D, 		this);
}
/////////////////////////////////////////////////////////////////
void GL3DDrawPane::setMotionMonitor(CncMotionMonitor* m) { 
/////////////////////////////////////////////////////////////////
	// this function has to be called once only
	wxASSERT(motionMonitor == NULL);
	
	motionMonitor = m; 
	if ( motionMonitor == NULL )
		return;
		
	motionMonitor->getContextOptions().registerCallback(this);
	motionMonitor->registerCallback(this);
	
	// initialize grid
	notifyChange(motionMonitor->getContextOptions());
}
/////////////////////////////////////////////////////////////////
void GL3DDrawPane::notifyChange(GLContextOptions& options) {
/////////////////////////////////////////////////////////////////
	 // currently nothing to do
}
/////////////////////////////////////////////////////////////////
void GL3DDrawPane::notifyChange(CncMotionMonitor& mm) {
/////////////////////////////////////////////////////////////////
	GetScalePane3D()->Refresh();
	GetRotatePaneX3D()->Refresh();
	GetRotatePaneY3D()->Refresh();
	GetRotatePaneZ3D()->Refresh();
}
/////////////////////////////////////////////////////////////////
void GL3DDrawPane::notifyCameraAngleChange(int angle) {
/////////////////////////////////////////////////////////////////
	// currently nothing to do
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::onPaintRotatePane3D(const char axis, wxPanel* panel, int angle) {
/////////////////////////////////////////////////////////////////////
	static wxColour posColour(0, 128, 0);
	static wxColour negColour(196, 0, 0);
	static wxColour colour;
	
	panel->SetToolTip(wxString::Format("Rotation %c: %d", axis, angle));
	
	angle < 0 ? colour = negColour : colour = posColour;
	angle = abs(angle);

	const wxSize size = panel->GetSize();
	const int height = size.GetHeight();
	
	const unsigned int pos = (unsigned int)(height * angle/360);
	const wxRect rect(0, height, size.GetWidth(), -pos);
	
	wxPaintDC dc(panel);
	dc.SetPen(wxPen(colour));
	dc.SetBrush(wxBrush(colour));
	dc.DrawRectangle(rect);
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::onPaintRotatePaneX3D(wxPaintEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;

	onPaintRotatePane3D('X', GetRotatePaneX3D(), motionMonitor->getAngleX());
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::onPaintRotatePaneY3D(wxPaintEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;

	onPaintRotatePane3D('Y', GetRotatePaneY3D(), motionMonitor->getAngleY());
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::onPaintRotatePaneZ3D(wxPaintEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;

	onPaintRotatePane3D('Z', GetRotatePaneZ3D(), motionMonitor->getAngleZ());
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::onPaintScalePane3D(wxPaintEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;

	static wxColour col(219,194,77);
	static wxBrush  brush(col);
	static wxPen    pen(col, 1, wxSOLID);
	
	wxPanel* pane          = GetScalePane3D();
	const wxSize size      = pane->GetSize();
	const int height       = size.GetHeight();
	const unsigned int pos = motionMonitor->calculateScaleDisplay(height) * 2;  // hack: * 2
	
	const wxRect rect(0, height, size.GetWidth(), -pos);
	pane->SetToolTip(wxString::Format("Scale: %d", pos));
	
	wxPaintDC dc(pane);
	dc.SetPen(pen);
	dc.SetBrush(brush);
	dc.DrawRectangle(rect);
}


