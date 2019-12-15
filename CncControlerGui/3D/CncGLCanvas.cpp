#include "CncContext.h"
#include "CncConfig.h"
#include "3D/CncGLCanvas.h"

//////////////////////////////////////////////////
CncGlCanvas::CncGlCanvas(wxWindow *parent, int *attribList) 
//////////////////////////////////////////////////
: wxGLCanvas(parent, wxID_ANY, attribList, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
, context(NULL)
, lastReshape()
, isShown(false)
, lastSetCurrent(false)
, mouseMoveMode(false)
{
}
//////////////////////////////////////////////////
CncGlCanvas::~CncGlCanvas() {
//////////////////////////////////////////////////
}
//////////////////////////////////////////////////
bool CncGlCanvas::Show(bool show) {
//////////////////////////////////////////////////
	GL_CTX_OBS->appendMessage('I', CNC_LOG_FUNCT, wxString::Format("%s Show=%s",
			                                                       (context ? context->getContextName() : "Unknown context name"),
																   (show ? "true" : "false")
																  ));
	return wxGLCanvas::Show(show);
}
//////////////////////////////////////////////////
bool CncGlCanvas::activateContext(GLContextBase* context, bool verbose) {
//////////////////////////////////////////////////
	return activateContext(context, *this, verbose);
}
//////////////////////////////////////////////////
bool CncGlCanvas::activateContext(GLContextBase* context, const wxGLCanvas &win, bool verbose) {
//////////////////////////////////////////////////
	lastSetCurrent = false;

	if ( context == NULL ) {
		wxASSERT ( context != NULL );
		return lastSetCurrent;
	} 
	
	if ( THE_CONTEXT->isWinOS() == true ) {
		lastSetCurrent = context->SetCurrent(win);
		return lastSetCurrent;
	} 
	
	if ( IsShown() ) {
		lastSetCurrent = context->SetCurrent(win);
		return lastSetCurrent;
	}

	if ( verbose == true && GL_ERROR_TRACE_LEVEL > 0 ) 
		GL_CTX_OBS->appendMessage('W', CNC_LOG_FUNCT, "isShow isn't true");
		
	return lastSetCurrent;
}
//////////////////////////////////////////////////
void CncGlCanvas::view(GLContextBase::ViewMode fm) {
//////////////////////////////////////////////////
	if ( context == NULL )
		return;
		
	context->setViewMode(fm);
	
	const wxSize cs = GetClientSize();
	context->reshapeViewMode(cs.GetWidth(), cs.GetHeight());
	Refresh();
}
//////////////////////////////////////////////////
void CncGlCanvas::incScale() {
//////////////////////////////////////////////////
	if ( context == NULL )
		return;
		
	context->getModelScale().decScale();
}
//////////////////////////////////////////////////
void CncGlCanvas::decScale() {
//////////////////////////////////////////////////
	if ( context == NULL )
		return;
		
	context->getModelScale().incScale();
}
//////////////////////////////////////////////////
void CncGlCanvas::initReshape() {
//////////////////////////////////////////////////
	const wxSize cs = GetClientSize();
	lastReshape.x = context->getLastReshapeX();
	lastReshape.y = cs.GetHeight() - context->getLastReshapeY();
}
//////////////////////////////////////////////////
void CncGlCanvas::reshapeRelative(int dx, int dy) {
//////////////////////////////////////////////////
	const wxSize cs = GetClientSize();
	const int nx 	= lastReshape.x + dx;
	const int ny 	= cs.GetHeight() - lastReshape.y - dy;
	
	context->reshape(cs.GetWidth(), cs.GetHeight(), nx, ny);
	Refresh();
}
//////////////////////////////////////////////////
void CncGlCanvas::onMouse(wxMouseEvent& event) {
//////////////////////////////////////////////////
	if ( context == NULL )
		return;

	const wxSize cs = GetClientSize();

	// activate the keyboard focus for this frame
	if ( event.LeftDown() )
		this->SetFocusFromKbd();
		
	// wheel
	const int rot = event.GetWheelRotation();
	if ( rot != 0 ) {
		if (rot < 0 ) 	decScale();
		else 			incScale();
		Refresh();
	}
	
	// move origin
	if ( event.ControlDown() == false ) {
		static int mx = 0, my = 0;
		
		if ( event.LeftDown() == true && mouseMoveMode == false ) {
			initReshape();
			
			mx = event.GetX();
			my = event.GetY();
			
			mouseMoveMode = true;
		}
		
		// calculate new origin
		if ( mouseMoveMode == true ) {
			const int dx = event.GetX() - mx;
			const int dy = event.GetY() - my;
			
			reshapeRelative(dx, dy);
		}
		
		// reset move mode
		if ( event.LeftUp() == true ) {
			mouseMoveMode = false;
		}

	// set origin
	} else {
		
		// left button
		if ( event.LeftIsDown() == true ) {
			// reverse y because the opengl viewport origin (0,0) is at left/bottom
			const int x = event.GetX();
			const int y = cs.GetHeight() - event.GetY();
			
			context->reshape(cs.GetWidth(), cs.GetHeight(), x, y);
			Refresh();
		}
	}
}
