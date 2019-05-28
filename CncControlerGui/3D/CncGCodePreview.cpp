#include <wx/dcclient.h>

#include "3D/CncGCodePreview.h"
#include "CncConfig.h"
#include "CncCommon.h"

#include "3D/GLInclude.h"

// ----------------------------------------------------------------------------
// CncGCodePreview Event Table
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
: CncGlCanvas(parent, attribList)
, preview(new GLContextGCodePreview(this))
, maxDimension(400.0)
, isShown(false)
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
	if ( preview != NULL ) 
		delete preview;
}
//////////////////////////////////////////////////
void CncGCodePreview::display() {
//////////////////////////////////////////////////
	preview->display();
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
	display();
	
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
	CncGlCanvas::onMouse(event);
	Refresh();
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
	typedef GLI::GLCncPathVertices::FormatType PathVerticeType;
	typedef GLI::GLCncPathVertices::CncMode    DataVerticeMode;
	
	static wxColour 		colour(128,128,128);
	static PathVerticeType	formatType = PathVerticeType::FT_SOLID;
	
	// decorate
	switch ( vd.getMode() ) {
		case DataVerticeMode::CM_WORK:	colour.Set(0, 0, 0);
										formatType	= PathVerticeType::FT_SOLID;
										break;
										
		case DataVerticeMode::CM_RAPID:	colour.Set(255, 201, 14);
										formatType	= PathVerticeType::FT_DOT;
										break;
										
		case DataVerticeMode::CM_MAX:	colour.Set(0, 255, 0);
										formatType	= PathVerticeType::FT_SOLID;
										break;
										
		case DataVerticeMode::CM_USER_DEFINED:
										colour.Set(0, 0, 255);
										formatType	= PathVerticeType::FT_SOLID;
										break;
	}
	
	const double x = vd.getX() / (maxDimension / GBL_CONFIG->getCalculationFactX());
	const double y = vd.getY() / (maxDimension / GBL_CONFIG->getCalculationFactY());
	const double z = vd.getZ() / (maxDimension / GBL_CONFIG->getCalculationFactZ());
	
	#warning !!!!!
	static GLI::GLCncPathVertices d;
	//preview->appendPathData(d.set(-1L, x, y, z, colour, formatType, vd.getMode())); 
}