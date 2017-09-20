#include <iostream>
#include "3D/GLContextBase.h"

#ifdef __DARWIN__
    #include <OpenGL/glu.h>
	#include <OpenGL/glut.h>
#else
    #include <GL/glu.h>
	#include <GL/glut.h>
	#include <GL/glext.h>
#endif

/////////////////////////////////////////////////////////////////
OpenGLContextBase::OpenGLContextBase(wxGLCanvas* canvas) 
: wxGLContext(canvas)
, initialized(false)
, coordOriginInfo()
, viewPort(NULL)
{
/////////////////////////////////////////////////////////////////
	if ( canvas != NULL )
		SetCurrent(*canvas);
}
/////////////////////////////////////////////////////////////////
OpenGLContextBase::~OpenGLContextBase() {
/////////////////////////////////////////////////////////////////
	if ( viewPort != NULL )
		delete viewPort;
		
	viewPort = NULL;
}
/////////////////////////////////////////////////////////////////
void OpenGLContextBase::init() {
/////////////////////////////////////////////////////////////////
	// call the initalization only one time
	if ( initialized == false )
		initContext();
		
	viewPort = createViewPort();
	
	initialized = true;
}
/////////////////////////////////////////////////////////////////
void OpenGLContextBase::keyboardHandler(unsigned char c) {
/////////////////////////////////////////////////////////////////
	// the base class hass nothing to do here
}
/////////////////////////////////////////////////////////////////
void OpenGLContextBase::displayCoordinateOrigin() {
/////////////////////////////////////////////////////////////////
	// x axis
	glBegin(GL_LINES);

		glColor3f(coordOriginInfo.colours.x.Red()/255, coordOriginInfo.colours.x.Green()/255, coordOriginInfo.colours.x.Blue()/255);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(coordOriginInfo.length, 0.0f, 0.0f);
		
	glEnd();
	
	// y axis
	glBegin(GL_LINES);

		glColor3f(coordOriginInfo.colours.y.Red()/255, coordOriginInfo.colours.y.Green()/255, coordOriginInfo.colours.y.Blue()/255);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, coordOriginInfo.length, 0.0f);
	
	glEnd();

	// z axis
	glBegin(GL_LINES);

		glColor3f(coordOriginInfo.colours.z.Red()/255, coordOriginInfo.colours.z.Green()/255, coordOriginInfo.colours.z.Blue()/255);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, coordOriginInfo.length);
	
	glEnd();
}
/////////////////////////////////////////////////////////////////
void OpenGLContextBase::determineViewPort(int w, int h) {
/////////////////////////////////////////////////////////////////
	if ( viewPort == NULL ) {
		// default handling
		std::cerr << "No viewpoert defined, using default view port" << std::endl;
		glViewport (0, 0, (GLsizei)w, (GLsizei)h);
		return;
	}
	
	viewPort->evaluate(w, h);
	glViewport(viewPort->getX(), 
			   viewPort->getY(), 
			   (GLsizei)viewPort->getNormalizedSizeW(), 
			   (GLsizei)viewPort->getNormalizedSizeH());
}
/////////////////////////////////////////////////////////////////
void OpenGLContextBase::reshape(int w, int h) {
/////////////////////////////////////////////////////////////////
	determineViewPort(w, h);
	determineProjection(w, h);
}
/////////////////////////////////////////////////////////////////
void OpenGLContextBase::display() {
/////////////////////////////////////////////////////////////////
	glClear (GL_COLOR_BUFFER_BIT);
	
	displayCoordinateOrigin();
	displayContext();
	
	glFlush();
}

