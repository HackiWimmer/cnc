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

	const wxSize cs = GetClientSize();
	preview->reshape(cs.GetWidth(), cs.GetHeight());
	preview->display();
	
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
void CncGCodePreview::appendVertice(const CncGCodePreview::VerticeData& vd) {
//////////////////////////////////////////////////


	typedef GLI::GLCncPathVertices::FormatType PathVerticeType;
	typedef GLI::GLCncPathVertices::CncMode    DataVerticeMode;
	
	
	static wxColour 		colour(128,128,128);
	static PathVerticeType	formatType = PathVerticeType::FT_SOLID;
	
	double x = vd.getX() / 400.0;
	double y = vd.getY() / 400.0;
	double z = vd.getZ() / 400.0;
	
	static GLI::GLCncPathVertices d;
	preview->appendPathData(d.set(x, y, z, colour, formatType, GLI::GLCncPathVertices::CncMode::CM_WORK)); 
}