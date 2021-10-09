#include <wx/dcclient.h>
#include "MainFrame.h"
#include "CncContext.h"
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
	
	showFromPerspective1();
	
	// initialize grid
	notifyChange(motionMonitor->getContextOptions());
}
/////////////////////////////////////////////////////////////////
void GL3DDrawPane::enable(bool state) {
/////////////////////////////////////////////////////////////////

//	m_btnOrigin->Enable(state);
//	m_btnRuler->Enable(state);
//	m_btnGuidePathes->Enable(state);
//	m_btnHelpLines->Enable(state);
//	m_btnPosMarker->Enable(state);
//	m_btnHardwareBox->Enable(state);
//	m_btnBoundBox->Enable(state);
//	m_btnFlyPath->Enable(state);
//	m_btnRefresh->Enable(state);
//	m_btnMillingCutter->Enable(state);

	m_btnClear->Enable(state);
	
	// at least update motion monitor
	if ( state == true )
		motionMonitor->Refresh();
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
///////////////////////////////////////////////////////////////////
void GL3DDrawPane::activate3DPerspectiveButton(wxButton* bt) {
///////////////////////////////////////////////////////////////////
	static wxColour active(171, 171, 171);
	static wxColour inactive(240, 240, 240);

	m_3D_Top->SetBackgroundColour(inactive);
	m_3D_Bottom->SetBackgroundColour(inactive);
	m_3D_Front->SetBackgroundColour(inactive);
	m_3D_Rear->SetBackgroundColour(inactive);
	m_3D_Left->SetBackgroundColour(inactive);
	m_3D_Right->SetBackgroundColour(inactive);
	m_3D_Perspective1->SetBackgroundColour(inactive);
	m_3D_Perspective2->SetBackgroundColour(inactive);
	m_3D_Perspective3->SetBackgroundColour(inactive);
	m_3D_Perspective4->SetBackgroundColour(inactive);
	
	if ( bt != NULL )
		bt->SetBackgroundColour(active);
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::toggleOptionPane() {
/////////////////////////////////////////////////////////////////////
	THE_APP->toggleMotionMonitorOptionPane(false);
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::showFromTop() {
/////////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
		
	activate3DPerspectiveButton(m_3D_Top);
	motionMonitor->viewTop();
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::showFromBottom() {
/////////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
		
	activate3DPerspectiveButton(m_3D_Bottom);
	motionMonitor->viewBottom();
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::showFromFront() {
/////////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
		
	activate3DPerspectiveButton(m_3D_Front);
	motionMonitor->viewFront();
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::showFromRear() {
/////////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
		
	activate3DPerspectiveButton(m_3D_Rear);
	motionMonitor->viewRear();
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::showFromLeft() {
/////////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
		
	activate3DPerspectiveButton(m_3D_Left);
	motionMonitor->viewLeft();
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::showFromRight() {
/////////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
		
	activate3DPerspectiveButton(m_3D_Right);
	motionMonitor->viewRight();
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::showFromPerspective1() {
/////////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
		
	activate3DPerspectiveButton(m_3D_Perspective1);
	motionMonitor->viewIso1();
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::showFromPerspective2() {
/////////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
		
	activate3DPerspectiveButton(m_3D_Perspective2);
	motionMonitor->viewIso2();
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::showFromPerspective3() {
/////////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
		
	activate3DPerspectiveButton(m_3D_Perspective3);
	motionMonitor->viewIso3();
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::showFromPerspective4() {
/////////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
		
	activate3DPerspectiveButton(m_3D_Perspective4);
	motionMonitor->viewIso4();
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::toggleHardwareBox() {
/////////////////////////////////////////////////////////////////////
	motionMonitor->getContextOptions().toggleOption(motionMonitor->getContextOptions().showHardwareBox);
	motionMonitor->updateMonitorAndOptions();
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::toggleBoundBox() {
/////////////////////////////////////////////////////////////////////
	motionMonitor->getContextOptions().toggleOption(motionMonitor->getContextOptions().showBoundBox);
	motionMonitor->updateMonitorAndOptions();
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::toggleFlyPathes() {
/////////////////////////////////////////////////////////////////////
	motionMonitor->getContextOptions().toggleOption(motionMonitor->getContextOptions().showFlyPath);
	motionMonitor->reconstruct();
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::toggleGuidePathes() {
/////////////////////////////////////////////////////////////////////
	motionMonitor->getContextOptions().toggleOption(motionMonitor->getContextOptions().showGuidePathes);
	motionMonitor->updateMonitorAndOptions();
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::toggleHelpLines() {
/////////////////////////////////////////////////////////////////////
	motionMonitor->getContextOptions().toggleOption(motionMonitor->getContextOptions().showHelpLines);
	motionMonitor->updateMonitorAndOptions();
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::toggleOrigin() {
/////////////////////////////////////////////////////////////////////
	motionMonitor->getContextOptions().toggleOption(motionMonitor->getContextOptions().showOrigin);
	motionMonitor->updateMonitorAndOptions();
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::togglePosMarker() {
/////////////////////////////////////////////////////////////////////
	motionMonitor->getContextOptions().toggleOption(motionMonitor->getContextOptions().showPosMarker);
	motionMonitor->updateMonitorAndOptions();
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::toggleMillingCutter() {
/////////////////////////////////////////////////////////////////////
	motionMonitor->getContextOptions().toggleOption(motionMonitor->getContextOptions().showMillingCutter);
	motionMonitor->updateMonitorAndOptions();
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::toggleRuler() {
/////////////////////////////////////////////////////////////////////
	motionMonitor->getContextOptions().toggleOption(motionMonitor->getContextOptions().showRuler);
	motionMonitor->updateMonitorAndOptions();
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::clearMonitor() {
/////////////////////////////////////////////////////////////////////
	THE_APP->clearMotionMonitor();
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::refreshMonitor() {
/////////////////////////////////////////////////////////////////////
	if ( THE_CONTEXT->isOnlineUpdateDrawPane() ) 
		motionMonitor->update(true);
}
/////////////////////////////////////////////////////////////////////
void GL3DDrawPane::resetView() {
/////////////////////////////////////////////////////////////////////
	motionMonitor->normalizeMonitor();
}
