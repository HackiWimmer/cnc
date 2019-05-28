#include <iostream>
#include "3D/GLContextCncPath.h"
#include "3D/GLInclude.h"

/////////////////////////////////////////////////////////////////
GLContextCncPath::GLContextCncPath(wxGLCanvas* canvas) 
: GLContextCncPathBase(canvas)
{
/////////////////////////////////////////////////////////////////
	// do something here on demand
}
/////////////////////////////////////////////////////////////////
GLContextCncPath::~GLContextCncPath() {
/////////////////////////////////////////////////////////////////
	// do something here on demand
	glDisable(GL_SCISSOR_TEST);
}
/////////////////////////////////////////////////////////////////
GLViewPort* GLContextCncPath::createViewPort() {
/////////////////////////////////////////////////////////////////
	// determine the destort type
	return new GLViewPort(GLViewPort::VPT_Undistored);
}
/////////////////////////////////////////////////////////////////
void GLContextCncPath::initContext() {
/////////////////////////////////////////////////////////////////
	// do context specific initalization here
	if ( isProbeMode() )	glClearColor(0.0, 0.0, 0.0, 0.0);
	else 					glClearColor(22.0/255.0, 22.0/255.0, 22.0/255.0, 0.0);
	
	glShadeModel(GL_FLAT);
	
	glEnable(GL_LINE_SMOOTH);

	if ( options.blending ) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 
		glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	}
	
	GLOpenGLPathBuffer::VertexColours& colours = cncPath.getColoursAsReference();
	colours.rapid 	= options.rapidColour;
	colours.work 	= options.workColour;
	colours.user 	= options.userColour;
	colours.max 	= options.maxColour;
	
	cncPath.setColours(colours);
	
	// currently this didn't work correctly
	//glEnable(GL_SCISSOR_TEST);
}
/////////////////////////////////////////////////////////////////
void GLContextCncPath::determineProjection(int w, int h) {
/////////////////////////////////////////////////////////////////
	GLContextBase::determineProjection(w, h);
}
/////////////////////////////////////////////////////////////////
void GLContextCncPath::determineModel() {
/////////////////////////////////////////////////////////////////
	// draw the scene
	 GLContextCncPathBase::determineModel();
}

