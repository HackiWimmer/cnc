#include <iostream>
#include "3D/GLContextGCodePreview.h"
#include "3D/GLInclude.h"

/////////////////////////////////////////////////////////////////
GLContextGCodePreview::GLContextGCodePreview(wxGLCanvas* canvas, wxString name) 
: GLContextCncPathBase(canvas, name)
, previewName(name)
{
/////////////////////////////////////////////////////////////////
	// do something here on demand
}
/////////////////////////////////////////////////////////////////
GLContextGCodePreview::~GLContextGCodePreview() {
/////////////////////////////////////////////////////////////////
	// do something here on demand
}
/////////////////////////////////////////////////////////////////
GLViewPort* GLContextGCodePreview::createViewPort() {
/////////////////////////////////////////////////////////////////
	// determine the destort type
	return new GLViewPort(GLViewPort::VPT_Undistored);
}
/////////////////////////////////////////////////////////////////
void GLContextGCodePreview::initContext() {
/////////////////////////////////////////////////////////////////
	if ( GLCommon::getTraceLevel() > 0 )
		std::cout << CNC_LOG_FUNCT << std::endl;

	// do context specific initalization here
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glShadeModel(GL_FLAT);
	
	glEnable(GL_LINE_SMOOTH);

	if ( options.blending ) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 
		glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	}
	
	GLOpenGLPathBuffer::VertexColours colours = GLOpenGLPathBuffer::getColours();
	colours.work  = *wxBLACK;
	colours.rapid = *wxYELLOW;
	GLOpenGLPathBuffer::setColours(colours);
}
/////////////////////////////////////////////////////////////////
void GLContextGCodePreview::determineProjection(int w, int h) {
/////////////////////////////////////////////////////////////////
	GLContextBase::determineProjection(w, h);
}
/////////////////////////////////////////////////////////////////
void GLContextGCodePreview::determineModel() {
/////////////////////////////////////////////////////////////////
	if ( GLCommon::getTraceLevel() > 0 )
		std::cout << CNC_LOG_FUNCT << std::endl;

	// draw the scene
	GLContextCncPathBase::determineModel();
}

