#include "CncContext.h"
#include "CncConfig.h"
#include "3D/CncGLCanvas.h"

//////////////////////////////////////////////////
CncGlCanvas::CncGlCanvas(wxWindow *parent, int *attribList) 
: wxGLCanvas(parent, wxID_ANY, attribList, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
, context			(NULL)
, lastReshape		()
, alreadyPainted	(false)
, lastSetCurrent	(false)
, mouseMoveMode		(false)
, mouseMoveX		(0) 
, mouseMoveY		(0)
//////////////////////////////////////////////////
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
void CncGlCanvas::update(bool force) {
//////////////////////////////////////////////////
	Refresh();
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

	if ( context == NULL )
	{
		wxASSERT ( context != NULL );
		return lastSetCurrent;
	} 
	
	if ( THE_CONTEXT->isWinOS() == true ) 
	{
		lastSetCurrent = context->SetCurrent(win);
		return lastSetCurrent;
	} 
	
	if ( win.IsShown() ) 
	{
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
	
	context->reshapeViewMode();
	update();
}
//////////////////////////////////////////////////
void CncGlCanvas::incScale(float s) {
//////////////////////////////////////////////////
	if ( context == NULL )
		return;
		
	context->getModelScale().decScale(s);
}
//////////////////////////////////////////////////
void CncGlCanvas::decScale(float s) {
//////////////////////////////////////////////////
	if ( context == NULL )
		return;
		
	context->getModelScale().incScale(s);
}
//////////////////////////////////////////////////
void CncGlCanvas::setScale(float scale) {
//////////////////////////////////////////////////
	if ( context == NULL )
		return;
		
	context->getModelScale().setScale(scale);
}
//////////////////////////////////////////////////
void CncGlCanvas::setScaleByRatio(float ratio) {
//////////////////////////////////////////////////
	if ( context == NULL )
		return;
		
	context->getModelScale().setScaleByRatio(ratio);
}
//////////////////////////////////////////////////
float CncGlCanvas::getScaleRatio() const {
//////////////////////////////////////////////////
	if ( context == NULL )
		return 1.0f;
		
	return context->getModelScale().getScaleRatio();
}
//////////////////////////////////////////////////
void CncGlCanvas::onMouse(wxMouseEvent& event) {
//////////////////////////////////////////////////
	if ( context == NULL )
		return;
		
	const wxSize cs = GetClientSize();
	
	if ( event.RightDown() )
	{
		if ( context->isViewMode2D() )
			CNC_PRINT_FUNCT
	}

	// wheel - rotate and scale
	const int rot = event.GetWheelRotation();
	if ( rot != 0 )
	{
		if ( event.ControlDown() && event.ShiftDown() )
		{
			if (rot < 0 )	context->getModelRotation().decAngleX();
			else			context->getModelRotation().incAngleX();
		}
		else if ( event.ShiftDown() ) 
		{
			if (rot < 0 )	context->getModelRotation().decAngleY();
			else			context->getModelRotation().incAngleY();
		}
		else if ( event.ControlDown() ) 
		{
			if (rot < 0 )	context->getModelRotation().decAngleZ();
			else			context->getModelRotation().incAngleZ();
		}
		else if ( event.AltDown() ) 
		{
			context->getModelRotation().reset();
		}
		else 
		{
			if (rot < 0 ) 	decScale();
			else 			incScale();
		}
		
		update();
		event.Skip();
		return;
	}
	
	// always do this here! form a first point of view it looks a little bit crazy
	// because we are within a callback which contains a wxMouseEvent event,
	// but if the the underlying window was left during the move and enter again
	// the previous event.LeftDown() isn't further present, therefore this . . 
	if ( wxGetMouseState().LeftIsDown() == false )
		mouseMoveMode = false;

	// move origin
	if ( event.ControlDown() == false ) 
	{
		if ( event.LeftDown() == true && mouseMoveMode == false )
		{
			mouseMoveX = event.GetX();
			mouseMoveY = event.GetY();
			
			mouseMoveMode = true;
		}
		
		// calculate new origin
		if ( mouseMoveMode == true )
		{
			const int	 dx = (event.GetX() - mouseMoveX);
			const int	 dy = (event.GetY() - mouseMoveY) * -1;
			
			// re-initialize to be further relative 
			mouseMoveX = event.GetX();
			mouseMoveY = event.GetY();
			
			context->reshapeRelative(dx, dy);
			update();
		}
		
		// reset move mode
		if ( event.LeftUp() == true )
			mouseMoveMode = false;
		
	}
	else
	{
		// left button
		if ( event.LeftIsDown() == true )
		{
			// reverse y because the opengl viewport origin (0,0) is at left/bottom
			const int px = event.GetX();
			const int py = cs.GetHeight() - event.GetY();
			
			// set origin absolute
			context->reshapeAbsolute(px, py);
			update();
		}
	}
	
	event.Skip();
}
