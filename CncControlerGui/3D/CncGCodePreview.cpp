#include <wx/dcclient.h>

#include "3D/CncGCodePreview.h"
#include "GL3DOptions.h"
#include "CncCommon.h"

#ifdef __DARWIN__
    #include <OpenGL/glu.h>
	#include <OpenGL/glut.h>
#else
    #include <GL/glu.h>
	#include <GL/glut.h>
	#include <GL/glext.h>
#endif

#if !wxUSE_GLCANVAS
	#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

// ----------------------------------------------------------------------------
// CncMotionMonitor Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(CncGCodePreview, wxGLCanvas)
	EVT_PAINT(CncGCodePreview::onPaint)
	EVT_SIZE(CncGCodePreview::onSize)
	EVT_ERASE_BACKGROUND(CncGCodePreview::onEraseBackground)
	EVT_MOUSE_EVENTS(CncGCodePreview::onMouse)
	EVT_KEY_DOWN(CncGCodePreview::onKeyDown)
wxEND_EVENT_TABLE()

//////////////////////////////////////////////////
CncGCodePreview::CncGCodePreview(wxWindow *parent, int *attribList) 
: wxGLCanvas(parent, wxID_ANY, attribList, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
, preview(new GLContextGCodePreview(this))
, maxDimension(400.0)
, isShown(false)
{
//////////////////////////////////////////////////
	GLContextBase::globalInit(); 
	preview->init();

	// publish initial zoom factor
	preview->setZoomFactor(2.0);
	preview->setViewMode(GLContextBase::ViewMode::V3D_ISO1);
}
//////////////////////////////////////////////////
CncGCodePreview::~CncGCodePreview() {
//////////////////////////////////////////////////
	if ( preview != NULL ) 
		delete preview;
}
//////////////////////////////////////////////////
void CncGCodePreview::onPaint(wxPaintEvent& event) {
//////////////////////////////////////////////////
	// This is required even though dc is not used otherwise.
	wxPaintDC dc(this);
	preview->SetCurrent(*this);
	preview->init();

	const wxSize cs = GetClientSize();
	
	if ( isShown )	preview->reshape(cs.GetWidth(), cs.GetHeight());
	else 			preview->reshapeViewMode(cs.GetWidth(), cs.GetHeight());
	preview->display();
	
	// The first onPaint() if IsShownOnScreen() == true have to reshape the view mode
	// later this should not appear to support custom origin positions
	// see if above
	isShown = IsShownOnScreen();
	
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
	const wxSize cs = GetClientSize();

	// activate the keyboard focus for this frame
	this->SetFocusFromKbd();
	
	// wheel
	int rot = event.GetWheelRotation();
	if ( rot != 0 ) {
		if (rot < 0 ) 	preview->getModelScale().decScale();
		else 			preview->getModelScale().incScale();
		Refresh(false);
	}
	
	// left button
	if ( event.LeftIsDown() == true ) {
		// reverse y because the opengl viewport origin (0,0) is at left/bottom
		int x = event.GetX();
		int y = cs.GetHeight() - event.GetY();
		
		preview->reshape(cs.GetWidth(), cs.GetHeight(), x, y);
		Refresh(false);
	}
}
//////////////////////////////////////////////////
void CncGCodePreview::onKeyDown(wxKeyEvent& event) {
//////////////////////////////////////////////////
	switch ( event.GetKeyCode() ) {
		case 'C':	preview->centerViewport();
					Refresh(false);
					break;
	}
}
//////////////////////////////////////////////////
void CncGCodePreview::clear() {
//////////////////////////////////////////////////
	preview->clearPathData();
	Refresh(false);
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
	clear();
	preview->enablePositionMarker(false);
}
//////////////////////////////////////////////////
void CncGCodePreview::popProcessMode() {
//////////////////////////////////////////////////
	preview->enablePositionMarker(false);
}
//////////////////////////////////////////////////
void CncGCodePreview::appendVertice(const GLI::VerticeDoubleData& vd) {
//////////////////////////////////////////////////
	typedef GLI::GLCncPathVertices::FormatType PathVerticeType;
	typedef GLI::GLCncPathVertices::CncMode    DataVerticeMode;
	
	static wxColour 		colour(128,128,128);
	static PathVerticeType	formatType = PathVerticeType::FT_SOLID;
	
	// decorate
	switch ( vd.getMode() ) {
		case DataVerticeMode::CM_WORK:	colour.Set(0, 0, 0);
										formatType	= PathVerticeType::FT_SOLID;
										break;
										
		case DataVerticeMode::CM_FLY:	colour.Set(255, 201, 14);
										formatType	= PathVerticeType::FT_DOT;
										break;
	}
	
	double x = vd.getX() / maxDimension;
	double y = vd.getY() / maxDimension;
	double z = vd.getZ() / maxDimension;
	
	static GLI::GLCncPathVertices d;
	preview->appendPathData(d.set(-1L, x, y, z, colour, formatType, vd.getMode())); 
}