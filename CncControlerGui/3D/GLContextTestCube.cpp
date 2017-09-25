#include <iostream>
#include "3D/GLContextTestCube.h"

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
GLContextTestCube::GLContextTestCube(wxGLCanvas* canvas) 
: GLContextCncPathBase(canvas)
{
/////////////////////////////////////////////////////////////////
	// do something here on demand
}
/////////////////////////////////////////////////////////////////
GLContextTestCube::~GLContextTestCube() {
/////////////////////////////////////////////////////////////////
	// do something here on demand
}
/////////////////////////////////////////////////////////////////
GLViewPort* GLContextTestCube::createViewPort() {
/////////////////////////////////////////////////////////////////
	// determine the destort type
	return new GLViewPort(GLViewPort::VPT_Distored);
}
/////////////////////////////////////////////////////////////////
void GLContextTestCube::initContext() {
/////////////////////////////////////////////////////////////////
	// do context specific initalization here
	setViewMode(GLContextBase::V3D_ISO1);
}
/////////////////////////////////////////////////////////////////
void GLContextTestCube::determineProjection(int w, int h) {
	GLContextBase::determineProjection(w, h);
}
/////////////////////////////////////////////////////////////////
void GLContextTestCube::determineModel() {
/////////////////////////////////////////////////////////////////
	glMatrixMode(GL_MODELVIEW);

	// draw the scene
	glColor3f (1.0, 1.0, 1.0);
	glutWireCube (1.0);
	
	glPushMatrix();
		glColor3f (1.0, 0.0, 0.0);
		glTranslatef(0.3f, 0.3f, 0.3f);
		glutWireCube (0.3);
	glPopMatrix();
	
}
