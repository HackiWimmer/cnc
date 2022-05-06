#include <wx/dcclient.h>

#include "3D/CncGCodePreview.h"
#include "CncConfig.h"
#include "MainFrame.h"
#include "CncCommon.h"

#include "3D/GLInclude.h"

// ----------------------------------------------------------------------------
// CncGCodePreview Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(CncGCodePreview, wxGLCanvas)
	EVT_PAINT(				CncGCodePreview::onPaint)
	EVT_SIZE(				CncGCodePreview::onSize)
	EVT_ERASE_BACKGROUND(	CncGCodePreview::onEraseBackground)
	EVT_MOUSE_EVENTS(		CncGCodePreview::onMouse)
	EVT_KEY_DOWN(			CncGCodePreview::onKeyDown)
wxEND_EVENT_TABLE()

//////////////////////////////////////////////////
CncGCodePreview::CncGCodePreview(wxWindow *parent, wxString name, int *attribList) 
: CncGlCanvas		(parent, attribList)
, previewName		(name)
, preview			(new GLContextGCodePreview(this, name))
, maxDimension		(400.0)
{
//////////////////////////////////////////////////
	GLContextBase::globalInit(); 
	preview->init();
	
	// Important: initialize the CncGlCanvas context
	context = preview;
	
	// publish initial zoom factor
	preview->setZoomFactor(2.0);
	preview->setViewMode(GLContextBase::ViewMode::V3D_ISO1);
}
//////////////////////////////////////////////////
CncGCodePreview::~CncGCodePreview() {
//////////////////////////////////////////////////
	delete preview;
}
//////////////////////////////////////////////////
void CncGCodePreview::initColours() {
//////////////////////////////////////////////////
	GLContextGCodePreview::initColours();
}
//////////////////////////////////////////////////
void CncGCodePreview::onPaint(wxPaintEvent& event) {
//////////////////////////////////////////////////
	// This is required even though dc is not used otherwise.
	wxPaintDC dc(this);
	
	// With respect to the GTK implementation SetCurrent() as well 
	// as SwapBuffers() isn't possible valid before
	if ( IsShownOnScreen() == false )
		return;
	
	const bool contextOk = activateContext(preview);
	if ( contextOk == false )
		return;
	
	if ( preview->init() == false )
		return;
	
	if ( alreadyPainted )	preview->reshape();
	else 					preview->reshapeViewMode();
	
	preview->display();
	
	// The first onPaint() if IsShownOnScreen() == true have to reshape the view mode
	// later this should not appear to support custom origin positions
	// see if above
	alreadyPainted = IsShown();
	SwapBuffers();
}
//////////////////////////////////////////////////
void CncGCodePreview::onSize(wxSizeEvent& event) {
//////////////////////////////////////////////////
	// noting to do, in this caes the onPaint() even
	// is also fired
}
//////////////////////////////////////////////////
void CncGCodePreview::onEraseBackground(wxEraseEvent& event) {
//////////////////////////////////////////////////
	// do nothing, only to avoid flashing on MSW
}
//////////////////////////////////////////////////
void CncGCodePreview::onMouse(wxMouseEvent& event) {
//////////////////////////////////////////////////
	CncGlCanvas::onMouse(event);
}
//////////////////////////////////////////////////
void CncGCodePreview::onKeyDown(wxKeyEvent& event) {
//////////////////////////////////////////////////
	switch ( event.GetKeyCode() ) {
		
		case 'C':	centreViewport();
					break;
					
		case 'R':	resetViewport();
					break;
	}
}
//////////////////////////////////////////////////
void CncGCodePreview::centreViewport() {
//////////////////////////////////////////////////
	preview->centreViewport();
	Refresh(false);
}
//////////////////////////////////////////////////
void CncGCodePreview::resetViewport() {
//////////////////////////////////////////////////
	
	std::cout << CNC_LOG_FUNCT << std::endl;
	
	preview->resetViewport();
	Refresh(true);
}
//////////////////////////////////////////////////
void CncGCodePreview::clear() {
//////////////////////////////////////////////////
	if ( activateContext(preview) == true ) 
		preview->clearPathData();
}
//////////////////////////////////////////////////
void CncGCodePreview::setMaxDimension(double maxDim) {
//////////////////////////////////////////////////
	if ( maxDim < 10.0 || maxDim > 2000.0 )
		return;
		
	maxDimension = maxDim;
}
//////////////////////////////////////////////////
void CncGCodePreview::pushProcessMode() {
//////////////////////////////////////////////////
	preview->getOptions().showPosMarker = false;
}
//////////////////////////////////////////////////
void CncGCodePreview::popProcessMode() {
//////////////////////////////////////////////////
	preview->getOptions().showPosMarker = false;
}
//////////////////////////////////////////////////
void CncGCodePreview::appendVertice(const GLI::VerticeDoubleData& vd) {
//////////////////////////////////////////////////
	static GLOpenGLPathBuffer::CncVertex vertex;

	const float x = vd.getX() / (maxDimension / THE_CONFIG->getCalculationFactX());
	const float y = vd.getY() / (maxDimension / THE_CONFIG->getCalculationFactY());
	const float z = vd.getZ() / (maxDimension / THE_CONFIG->getCalculationFactZ());
	const char sc = cnc::getCncSpeedTypeAsCharacter(vd.getSpeedMode());
	
	if ( activateContext(preview, true) == true )
		preview->appendPathData(vertex.set(sc, -1L, x, y, z)); 
}
