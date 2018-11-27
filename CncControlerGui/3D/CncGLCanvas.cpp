#include "CncGLCanvas.h"


//////////////////////////////////////////////////
CncGlCanvas::CncGlCanvas(wxWindow *parent, int *attribList) 
//////////////////////////////////////////////////
: wxGLCanvas(parent, wxID_ANY, attribList, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
, context(NULL)
, mouseMoveMode(false)
{
}
//////////////////////////////////////////////////
CncGlCanvas::~CncGlCanvas() {
//////////////////////////////////////////////////
}
//////////////////////////////////////////////////
void CncGlCanvas::view(GLContextBase::ViewMode fm) {
//////////////////////////////////////////////////
	if ( context == NULL )
		return;
		
	context->setViewMode(fm);
	
	const wxSize cs = GetClientSize();
	context->reshapeViewMode(cs.GetWidth(), cs.GetHeight());
	display();
}
//////////////////////////////////////////////////
void CncGlCanvas::onMouse(wxMouseEvent& event) {
//////////////////////////////////////////////////
	if ( context == NULL )
		return;

	const wxSize cs = GetClientSize();

	// activate the keyboard focus for this frame
	this->SetFocusFromKbd();
	
	// wheel
	const int rot = event.GetWheelRotation();
	if ( rot != 0 ) {
		if (rot < 0 ) 	context->getModelScale().decScale();
		else 			context->getModelScale().incScale();
		display();
	}
	
	// move origin
	if ( event.ControlDown() == false ) {
		static int lx = 0, ly = 0;
		static int mx = 0, my = 0;
		
		if ( event.LeftDown() == true && mouseMoveMode == false ) {
			lx = context->getLastReshapeX();
			ly = cs.GetHeight() - context->getLastReshapeY();
			
			mx = event.GetX();
			my = event.GetY();
			
			mouseMoveMode = true;
		}
		
		// calculate new origin
		if ( mouseMoveMode == true ) {
			const int dx = event.GetX() - mx;
			const int dy = event.GetY() - my;
			
			const int nx = lx + dx;
			const int ny = cs.GetHeight() - ly - dy;
			
			context->reshape(cs.GetWidth(), cs.GetHeight(), nx, ny);
			display();
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
			display();
		}
	}
}
