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
OpenGLContextCncPath::OpenGLContextCncPath(wxGLCanvas* canvas) 
: OpenGLContextCncPathBase(canvas)
{
/////////////////////////////////////////////////////////////////
	// do nothing here
}
/////////////////////////////////////////////////////////////////
OpenGLContextCncPath::~OpenGLContextCncPath() {
/////////////////////////////////////////////////////////////////
	// do nothing here
}
/////////////////////////////////////////////////////////////////
GLViewPort* OpenGLContextCncPath::createViewPort() {
/////////////////////////////////////////////////////////////////
	return new GLViewPort(GLViewPort::VPT_Undistored);
}
/////////////////////////////////////////////////////////////////
void OpenGLContextCncPath::initContext() {
/////////////////////////////////////////////////////////////////
	// do context specific initalization here
	
}
/////////////////////////////////////////////////////////////////
void OpenGLContextCncPath::determineProjection(int w, int h) {
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glFrustum (-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
	glMatrixMode (GL_MODELVIEW);
}
/////////////////////////////////////////////////////////////////
void OpenGLContextCncPath::displayContext() {
/////////////////////////////////////////////////////////////////
	glColor3f (1.0, 1.0, 1.0);
	glLoadIdentity ();             /* clear the matrix */
		   /* viewing transformation  */
	gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glScalef (1.0, 2.0, 1.0);      /* modeling transformation */ 
	glutWireCube (1.0);
}

