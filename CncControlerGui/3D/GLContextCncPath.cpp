#include <iostream>
#include "3D/GLContextCncPath.h"

#ifdef __DARWIN__
    #include <OpenGL/glu.h>
	#include <OpenGL/glut.h>
	#include <OpenGL/freeglut.h>
#else
    #include <GL/glu.h>
	#include <GL/glut.h>
	#include <GL/freeglut.h>
#endif

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
	if ( isEnabled() )	glClearColor(0.0, 0.0, 0.0, 0.0);
	else 				glClearColor(127.0/255.0, 127.0/255.0, 127.0/255.0, 0.0);
	
	glShadeModel(GL_FLAT);
	
	glEnable(GL_LINE_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
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

