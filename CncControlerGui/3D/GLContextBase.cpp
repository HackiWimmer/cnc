#include <iostream>
#include <sstream>
#include "3D/GLContextBase.h"


#ifdef __DARWIN__
    #include <OpenGL/glu.h>
	#include <OpenGL/glut.h>
#else
    #include <GL/glu.h>
	#include <GL/glut.h>
	#include <GL/glext.h>
#endif


#ifndef _USE_WX_DUMMY_CANVAS 
namespace OGL_DEBUG {
	/////////////////////////////////////////////////////////////////
	void debugInfo(const char* context, const char* function, const char* message) {
	/////////////////////////////////////////////////////////////////
		context == NULL ? 	std::clog << "???" : std::clog << context;
		std::clog << "::";
		
		function == NULL ? 	std::clog << "???" : std::clog << function;
		std::clog << ": ";
		
		message == NULL ? 	std::clog << "???" : std::clog << message;
		std::clog << std::endl;
	}
	
	/////////////////////////////////////////////////////////////////
	void debugError(const char* context, const char* function, const char* message) {
	/////////////////////////////////////////////////////////////////
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);

		context == NULL ? 	std::clog << "???" : std::clog << context;
		std::clog << "::";
		
		function == NULL ? 	std::clog << "???" : std::clog << function;
		std::clog << ": ";
		
		message == NULL ? 	std::clog << "???" : std::clog << message;
		std::clog << std::endl;
		
		SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
	}
#endif

/////////////////////////////////////////////////////////////////
OpenGLContextBase::OpenGLContextBase(wxGLCanvas* canvas) 
: wxGLContext(canvas)
, initialized(false)
, drawViewPortBounderies(true)
, viewMode(V2D_TOP)
, coordOriginInfo()
, viewPort(NULL)
, modelScale()
, modelRotate()
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
	
	enableSmoothing();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

	initialized = true;
}
/////////////////////////////////////////////////////////////////////
void OpenGLContextBase::checkGLError() {
/////////////////////////////////////////////////////////////////////
	GLenum errLast = GL_NO_ERROR;

	for ( ;; )
	{
		GLenum err = glGetError();
		if ( err == GL_NO_ERROR )
			return;

		// normally the error is reset by the call to glGetError() but if
		// glGetError() itself returns an error, we risk looping forever here
		// so check that we get a different error than the last time
		if ( err == errLast )
		{
			OGL_DEBUG::debugError("OpenGLContextBase" , "checkGLError()", "OpenGL error state couldn't be reset.");
			return;
		}

		errLast = err;
 
		std::stringstream ss;
		ss << gluErrorString(err);
		OGL_DEBUG::debugError("OpenGLContextBase" , "checkGLError()", wxString::Format("OpenGL error [ %d ]: %s", err, ss.str()));
	}
}
/////////////////////////////////////////////////////////////////
void OpenGLContextBase::enableSmoothing(bool enable) {
/////////////////////////////////////////////////////////////////
	enable == true ? glEnable(GL_LINE_SMOOTH) : glDisable(GL_LINE_SMOOTH);
}
/////////////////////////////////////////////////////////////////
bool OpenGLContextBase::isSmoothingEnabled() {
/////////////////////////////////////////////////////////////////
	return (bool)glIsEnabled(GL_LINE_SMOOTH);
}
/////////////////////////////////////////////////////////////////
GLViewPort::PreDefPos OpenGLContextBase::convertViewMode(OpenGLContextBase::ViewMode m) {
/////////////////////////////////////////////////////////////////
	switch ( m ) {
		case V2D_TOP:		return GLViewPort::PreDefPos::VPDP_BottomLeft;
		case V2D_BOTTOM:	return GLViewPort::PreDefPos::VPDP_BottomRight;
		case V2D_LEFT:		return GLViewPort::PreDefPos::VPDP_BottomLeft; 
		case V2D_RIGHT:		return GLViewPort::PreDefPos::VPDP_BottomRight; 
		case V2D_FRONT:		return GLViewPort::PreDefPos::VPDP_BottomLeft; 
		case V2D_REAR:		return GLViewPort::PreDefPos::VPDP_BottomRight;

		case V3D_ISO1:		return GLViewPort::PreDefPos::VPDP_Center;
		case V3D_ISO2:		return GLViewPort::PreDefPos::VPDP_Center;
		case V3D_ISO3:		return GLViewPort::PreDefPos::VPDP_Center;
		case V3D_ISO4:		return GLViewPort::PreDefPos::VPDP_Center;
	}
	
	return GLViewPort::PreDefPos::VPDP_Center;
}
/////////////////////////////////////////////////////////////////
void OpenGLContextBase::centerViewport() {
/////////////////////////////////////////////////////////////////
	if ( viewPort != NULL )
		viewPort->centerViewport();
}
/////////////////////////////////////////////////////////////////
void OpenGLContextBase::keyboardHandler(unsigned char c) {
/////////////////////////////////////////////////////////////////
	// the base class hass nothing to do here
}
/////////////////////////////////////////////////////////////////
void OpenGLContextBase::determineViewPortBounderies() {
/////////////////////////////////////////////////////////////////
	// ensure the right matrix
	glMatrixMode(GL_MODELVIEW);

	if ( isViewMode2D() ) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		GLfloat c = (GLfloat)128/255;
		glColor4f(c, c, c, 0.1);

		// xy plane
		glBegin(GL_POLYGON);
			glVertex3f(-1.0, -1.0, 0.0);
			glVertex3f(-1.0,  1.0, 0.0);
			glVertex3f( 1.0,  1.0, 0.0);
			glVertex3f( 1.0, -1.0, 0.0);
		glEnd();
		
		// xz plane
		glBegin(GL_POLYGON);
			glVertex3f(-1.0, 0.0, -1.0);
			glVertex3f(-1.0, 0.0,  1.0);
			glVertex3f( 1.0, 0.0,  1.0);
			glVertex3f( 1.0, 0.0, -1.0);
		glEnd();
		
		// yz plane
		glBegin(GL_POLYGON);
			glVertex3f(0.0, -1.0, -1.0);
			glVertex3f(0.0, -1.0,  1.0);
			glVertex3f(0.0,  1.0,  1.0);
			glVertex3f(0.0,  1.0, -1.0);
		glEnd();
			
	} else {
		//todo : what's useful for 3d?
	}
}
/////////////////////////////////////////////////////////////////
void  OpenGLContextBase::renderBitmapString(float x, float y, float z, 
                                            void* font, const char* string) {
/////////////////////////////////////////////////////////////////
	// ensure the right matrix
	glMatrixMode(GL_MODELVIEW);

	const char *c;
	glRasterPos3f(x, y, z);
	for (c = string; *c != '\0'; c++)
		glutBitmapCharacter(font, *c);
} 
/////////////////////////////////////////////////////////////////
void OpenGLContextBase::determineCoordinateOrigin() {
/////////////////////////////////////////////////////////////////
	wxASSERT( viewPort != NULL );
	
	// ensure the right matrix
	glMatrixMode(GL_MODELVIEW);
	
	// recsale due to the viewport enlargement, so we alwys have  
	// a constant sizing for the coordinate origin 
	glScalef(viewPort->getDisplayFactor(), viewPort->getDisplayFactor(), viewPort->getDisplayFactor()); 
	
	// rotate
	glRotatef(modelRotate.angleX(), 1.0f, 0.0f, 0.0f);
	glRotatef(modelRotate.angleY(), 0.0f, 1.0f, 0.0f);
	glRotatef(modelRotate.angleZ(), 0.0f, 0.0f, 1.0f);

	// x axis
	glBegin(GL_LINES);

		glColor3f(coordOriginInfo.colours.x.Red()/255, coordOriginInfo.colours.x.Green()/255, coordOriginInfo.colours.x.Blue()/255);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(coordOriginInfo.length, 0.0f, 0.0f);
		
	glEnd();
	renderBitmapString(coordOriginInfo.length, 0.0f, 0.0f, GLUT_BITMAP_9_BY_15, "X");
	
	// y axis
	glBegin(GL_LINES);

		glColor3f(coordOriginInfo.colours.y.Red()/255, coordOriginInfo.colours.y.Green()/255, coordOriginInfo.colours.y.Blue()/255);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, coordOriginInfo.length, 0.0f);
	
	glEnd();
	renderBitmapString(0.0f, coordOriginInfo.length, 0.0f, GLUT_BITMAP_9_BY_15, "Y");

	// z axis
	glBegin(GL_LINES);

		glColor3f(coordOriginInfo.colours.z.Red()/255, coordOriginInfo.colours.z.Green()/255, coordOriginInfo.colours.z.Blue()/255);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, coordOriginInfo.length);
	
	glEnd();
	renderBitmapString(0.0f, 0.0f, coordOriginInfo.length, GLUT_BITMAP_9_BY_15, "Z");
}
/////////////////////////////////////////////////////////////////
void OpenGLContextBase::setViewMode(OpenGLContextBase::ViewMode m) {
/////////////////////////////////////////////////////////////////
	// is this a change from 2d to 3d or vs.
	if ( getViewType() != getViewType(m) ) {
		// is the current origin postion customized?
		if ( viewPort->getOriginPosType() == GLViewPort::VPOP_Custom ) {
			// switch to the center mode
			viewPort->resetCustomOrigPosType();
		}
	}
	
	viewMode = m;
}
/////////////////////////////////////////////////////////////////
void OpenGLContextBase::determineViewPort(int w, int h, int x, int y) {
/////////////////////////////////////////////////////////////////
	if ( viewPort == NULL ) {
		// default handling
		std::cerr << "No viewport defined, using default view port" << std::endl;
		glViewport (0, 0, (GLsizei)w, (GLsizei)h);
		return;
	}
	
	viewPort->evaluate(w, h, x, y);
	viewPort->trace(std::clog);

	glViewport(viewPort->getX(),  viewPort->getY(),
			   (GLsizei)viewPort->getNormalizedSizeW(), 
			   (GLsizei)viewPort->getNormalizedSizeH());
}
/////////////////////////////////////////////////////////////////
void OpenGLContextBase::determineProjection(int w, int h) {
/////////////////////////////////////////////////////////////////
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	if ( isViewMode2D() )	glOrtho(-1.0, 1.0, -1.0, 1.0, 0.1, 20.0);
	else					glFrustum(-1.0, 1.0, -1.0, 1.0, 5.0, 10.0);
}
/////////////////////////////////////////////////////////////////
void OpenGLContextBase::determineModel() {
/////////////////////////////////////////////////////////////////
	glMatrixMode(GL_MODELVIEW);
	
	// this function should be overridden by derived classes
	// to define what is to draw
}
/////////////////////////////////////////////////////////////////
void OpenGLContextBase::determineCameraPosition() {
/////////////////////////////////////////////////////////////////
	glMatrixMode(GL_MODELVIEW);
	
	switch( viewMode ) {
		case V2D_TOP:		gluLookAt ( 0.0,  0.0,  5.0,  0.0,  0.0,  0.0,  0.0,  1.0,  0.0); break;
		case V2D_BOTTOM:	gluLookAt ( 0.0,  0.0, -5.0,  0.0,  0.0,  0.0,  0.0,  1.0,  0.0); break;
		case V2D_LEFT:		gluLookAt ( 5.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  1.0); break;
		case V2D_RIGHT:		gluLookAt (-5.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  1.0); break;
		case V2D_FRONT:		gluLookAt ( 0.0,  5.0,  0.0,  0.0,  0.0,  0.0,  1.0,  0.0,  0.0); break;
		case V2D_REAR:		gluLookAt ( 0.0, -5.0,  0.0,  0.0,  0.0,  0.0,  1.0,  0.0,  0.0); break;
		
		case V3D_ISO1:		gluLookAt ( 5.0,  5.0,  5.0,  0.0,  0.0,  0.0, -1.0, -1.0,  0.0); break;
		case V3D_ISO2:		gluLookAt (-5.0, -5.0, -5.0,  0.0,  0.0,  0.0, -1.0, -1.0,  0.0); break; 
		case V3D_ISO3: 		;//todo: the camera isn't rigt for all cases
		case V3D_ISO4:		;
	}
}
/////////////////////////////////////////////////////////////////
void OpenGLContextBase::reshape(int w, int h, int x, int y) {
/////////////////////////////////////////////////////////////////
	determineViewPort(w, h, x, y);
	determineProjection(w, h);
}
/////////////////////////////////////////////////////////////////
void OpenGLContextBase::reshapeViewMode(int w, int h) {
/////////////////////////////////////////////////////////////////
	int x = 0, y = 0;
	// evaluate the corrsponding origin coordinates
	viewPort->getPreDefCoordinatesXY(convertViewMode(viewMode), w, h, x, y);
	reshape(w, h, x, y);
}
/////////////////////////////////////////////////////////////////
void OpenGLContextBase::display() {
/////////////////////////////////////////////////////////////////
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	wxASSERT ( viewPort != NULL );
	
	// initialize model matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); 
	
	//first position the camera
	determineCameraPosition();
	
	// main model
	glPushMatrix();
		// scale
		glScalef(modelScale.factX() * viewPort->getDisplayFactor(), 
				 modelScale.factY() * viewPort->getDisplayFactor(), 
				 modelScale.factZ() * viewPort->getDisplayFactor()); 
				 
		// rotate
		glRotatef(modelRotate.angleX(), 1.0f, 0.0f, 0.0f);
		glRotatef(modelRotate.angleY(), 0.0f, 1.0f, 0.0f);
		glRotatef(modelRotate.angleZ(), 0.0f, 0.0f, 1.0f);
				 
		// draw the scene
		determineModel();
		
	glPopMatrix();
	
	// draw additional things
	glPushMatrix();
	
		// bounderies
		if ( drawViewPortBounderies == true )
			determineViewPortBounderies();
			
	glPopMatrix();
	
	// draw coordinate origin
	glPushMatrix();
	
		determineCoordinateOrigin();
		
	glPopMatrix();
	
	glFlush();
	checkGLError();
}




