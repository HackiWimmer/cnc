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

/////////////////////////////////////////////////////////////////
GLContextBase::GLContextBase(wxGLCanvas* canvas) 
: wxGLContext(canvas)
, initialized(false)
, drawViewPortBounderies(true)
, posMarker(true)
, zoom(1.0f)
, viewMode(V2D_TOP)
, coordOriginInfo()
, viewPort(NULL)
, modelScale()
, modelRotate()
, cameraPos()
{
/////////////////////////////////////////////////////////////////
	if ( canvas != NULL )
		SetCurrent(*canvas);
	
	// ensure the view mode and the model rotation is in line,
	// for this the view mode here has to be different to the
	// initialization above
	setViewMode(V3D_ISO1);
}
/////////////////////////////////////////////////////////////////
GLContextBase::~GLContextBase() {
/////////////////////////////////////////////////////////////////
	if ( viewPort != NULL )
		delete viewPort;
		
	viewPort = NULL;
}
/////////////////////////////////////////////////////////////////
void GLContextBase::globalInit() {
/////////////////////////////////////////////////////////////////
	// this is a static function and should be called one time
	// normallly before creating the first context
	// So, the initalization here is globally. 
	// If context sensetive initializion is needed use init() instead
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	
	glEnable(GL_LINE_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
}
/////////////////////////////////////////////////////////////////
void GLContextBase::init() {
/////////////////////////////////////////////////////////////////
	// call the initalization only one time
	if ( initialized == true )
		return;
		
	viewPort = createViewPort();
	
	initContext();
	initialized = true;
}
/////////////////////////////////////////////////////////////////////
void GLContextBase::checkGLError() {
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
			std::cerr << "OpenGLContextBase::checkGLError(): OpenGL error state couldn't be reset." << std::endl;
			return;
		}

		errLast = err;
 
		std::stringstream ss;
		ss << gluErrorString(err);
		std::cerr << "OpenGLContextBase::checkGLError(): " << wxString::Format("OpenGL error [ %d ]: %s", err, ss.str()) << std::endl;
	}
}
/////////////////////////////////////////////////////////////////
void GLContextBase::enableSmoothing(bool enable) {
/////////////////////////////////////////////////////////////////
	enable == true ? glEnable(GL_LINE_SMOOTH) : glDisable(GL_LINE_SMOOTH);
}
/////////////////////////////////////////////////////////////////
bool GLContextBase::isSmoothingEnabled() {
/////////////////////////////////////////////////////////////////
	return (bool)glIsEnabled(GL_LINE_SMOOTH);
}
/////////////////////////////////////////////////////////////////
GLViewPort::PreDefPos GLContextBase::convertViewMode(GLContextBase::ViewMode newMode) {
/////////////////////////////////////////////////////////////////
	switch ( newMode ) {
		case V2D_TOP:			return GLViewPort::PreDefPos::VPDP_BottomLeft;
		case V2D_BOTTOM:		return GLViewPort::PreDefPos::VPDP_BottomRight;
		case V2D_LEFT:			return GLViewPort::PreDefPos::VPDP_BottomRight; 
		case V2D_RIGHT:			return GLViewPort::PreDefPos::VPDP_BottomLeft; 
		case V2D_FRONT:			return GLViewPort::PreDefPos::VPDP_BottomLeft; 
		case V2D_REAR:			return GLViewPort::PreDefPos::VPDP_BottomRight;

		case V3D_ISO1:			return GLViewPort::PreDefPos::VPDP_Center;
		case V3D_ISO2:			return GLViewPort::PreDefPos::VPDP_Center;
		case V3D_ISO3:			return GLViewPort::PreDefPos::VPDP_Center;
		case V3D_ISO4:			return GLViewPort::PreDefPos::VPDP_Center;
		
		case V2D_CAM_ROT_XY_ZTOP:	return GLViewPort::PreDefPos::VPDP_Center;
	}
	
	return GLViewPort::PreDefPos::VPDP_Center;
}
/////////////////////////////////////////////////////////////////
const char* GLContextBase::getViewModeAsString() {
/////////////////////////////////////////////////////////////////
	switch ( viewMode ) {
		case V2D_TOP:			return "Top";
		case V2D_BOTTOM:		return "Bottom";
		case V2D_LEFT:			return "Left"; 
		case V2D_RIGHT:			return "Right"; 
		case V2D_FRONT:			return "Front"; 
		case V2D_REAR:			return "Rear";

		case V3D_ISO1:			return "3D ISO1";
		case V3D_ISO2:			return "3D ISO2";
		case V3D_ISO3:			return "3D ISO3";
		case V3D_ISO4:			return "3D ISO4";
		
		case V2D_CAM_ROT_XY_ZTOP:	return "3D XP Plane rotation; Z top";
	}
	
	return "Unknown view mode";
}
/////////////////////////////////////////////////////////////////
void GLContextBase::setViewMode(GLContextBase::ViewMode newMode) {
/////////////////////////////////////////////////////////////////
	// is this a change from 2d to 3d or vs.
	if ( getViewType() != getViewType(newMode) ) {
		// is the current origin postion customized?
		if ( viewPort != NULL && viewPort->getOriginPosType() == GLViewPort::VPOP_Custom ) {
			// switch to the center mode
			viewPort->resetCustomOrigPosType();
		}
		
		// if the view type going to change the model rotation
		// has to be switched too
		if ( isViewMode2D(newMode) == true )	modelRotate.restore2DDefaults();
		else									modelRotate.restore3DDefaults();
	}
	
	viewMode = newMode;
}
/////////////////////////////////////////////////////////////////
void GLContextBase::centerViewport() {
/////////////////////////////////////////////////////////////////
	if ( viewPort != NULL )
		viewPort->centerViewport();
}
/////////////////////////////////////////////////////////////////
void GLContextBase::keyboardHandler(unsigned char c) {
/////////////////////////////////////////////////////////////////
	// the base class has nothing to do here
}
/////////////////////////////////////////////////////////////////
void GLContextBase::determineViewPortBounderies() {
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
void  GLContextBase::renderBitmapString(float x, float y, float z, 
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
void GLContextBase::drawCoordinateOrigin() {
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
void GLContextBase::drawPosMarker(float x, float y, float z) {
/////////////////////////////////////////////////////////////////
	if ( isViewMode2D() ) {

		glLineStipple(2, 0xAAAA);
		glEnable(GL_LINE_STIPPLE);
		glColor3ub (255, 201, 14);
		glBegin(GL_LINES);

			// view the given position like a cross hair
			// over the following planes
			
			switch ( viewMode ) {
				case V2D_TOP:
				case V2D_BOTTOM:
							// xy plane
							glVertex3f(x, y, 0.0f); glVertex3f(+1.0f, y, 0.0f);
							glVertex3f(x, y, 0.0f); glVertex3f(-1.0f, y, 0.0f);
							
							glVertex3f(x, y, 0.0f); glVertex3f(x, +1.0f, 0.0f);
							glVertex3f(x, y, 0.0f); glVertex3f(x, -1.0f, 0.0f);
							break;
						
				case V2D_LEFT:
				case V2D_RIGHT:
							// yz plane
							glVertex3f(0.0f, y, z); glVertex3f(0.0f, +1.0f, z);
							glVertex3f(0.0f, y, z); glVertex3f(0.0f, -1.0f, z);

							glVertex3f(0.0f, y, z); glVertex3f(0.0f, y, +1.0f);
							glVertex3f(0.0f, y, z); glVertex3f(0.0f, y, -1.0f);
							break;
						
				case V2D_FRONT:
				case V2D_REAR:
							// xz plane
							glVertex3f(x, 0.0f, z); glVertex3f(+1.0f, 0.0f, z);
							glVertex3f(x, 0.0f, z); glVertex3f(-1.0f, 0.0f, z);

							glVertex3f(x, 0.0f, z); glVertex3f(x, 0.0f, +1.0f);
							glVertex3f(x, 0.0f, z); glVertex3f(x, 0.0f, -1.0f);
							break;
						
				default:	if ( isViewMode2D(viewMode) )
								std::cerr << "Missing view  2D mode: " << viewMode << std::endl;
			}

		glEnd();
		glDisable(GL_LINE_STIPPLE);

	} else {

		glColor3ub (255, 201, 14);
		glBegin(GL_LINES);

			// view the given position like a needle
			// vertically over the xy plane
			glVertex3f(x, y, z); 
			glVertex3f(x, y, +1.0f);

		glEnd();
	}
}
/////////////////////////////////////////////////////////////////
void GLContextBase::determineViewPort(int w, int h, int x, int y) {
/////////////////////////////////////////////////////////////////
	if ( viewPort == NULL ) {
		// default handling
		std::cerr << "No viewport defined, using default view port" << std::endl;
		glViewport (0, 0, (GLsizei)w, (GLsizei)h);
		return;
	}
	
	viewPort->evaluate(w, h, x, y);
	//viewPort->trace(std::clog);

	glViewport(viewPort->getX(),  viewPort->getY(),
			   (GLsizei)viewPort->getNormalizedSizeW(), 
			   (GLsizei)viewPort->getNormalizedSizeH());
}
/////////////////////////////////////////////////////////////////
void GLContextBase::determineProjection(int w, int h) {
/////////////////////////////////////////////////////////////////
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	if ( isViewMode2D() )	glOrtho(  -1.0, 1.0, -1.0, 1.0, 0.1, 200.0);
	else					glFrustum(-1.0, 1.0, -1.0, 1.0, 3.0, 100.0);
}
/////////////////////////////////////////////////////////////////
void GLContextBase::determineModel() {
/////////////////////////////////////////////////////////////////
	glMatrixMode(GL_MODELVIEW);
	
	// this function hs to be overridden by derived classes
	// to define what is to draw
}
/////////////////////////////////////////////////////////////////
void GLContextBase::determineCameraPosition() {
/////////////////////////////////////////////////////////////////
	glMatrixMode(GL_MODELVIEW);
	
	float eye2DX = 5.0;
	float eye2DY = 5.0;
	float eye2DZ = 5.0;
	
	float eye3DX = 3.0;
	float eye3DY = 3.0;
	float eye3DZ = 2.0;
	
	switch( viewMode ) {
		
		// 2D views - static eye positions
		case V2D_TOP:			cameraPos.setEyePos(    0.0,     0.0,  eye2DZ); cameraPos.setUpPos(GLI::CameraPosition::UpType::CUT_YTOP); break;
		case V2D_BOTTOM:		cameraPos.setEyePos(    0.0,     0.0, -eye2DZ); cameraPos.setUpPos(GLI::CameraPosition::UpType::CUT_YTOP); break;
		case V2D_LEFT:			cameraPos.setEyePos(-eye2DX,     0.0,     0.0); cameraPos.setUpPos(GLI::CameraPosition::UpType::CUT_ZTOP); break;
		case V2D_RIGHT:			cameraPos.setEyePos( eye2DX,     0.0,     0.0); cameraPos.setUpPos(GLI::CameraPosition::UpType::CUT_ZTOP); break;
		case V2D_FRONT:			cameraPos.setEyePos(    0.0, -eye2DY,     0.0); cameraPos.setUpPos(GLI::CameraPosition::UpType::CUT_ZTOP); break;
		case V2D_REAR:			cameraPos.setEyePos(    0.0,  eye2DY,     0.0); cameraPos.setUpPos(GLI::CameraPosition::UpType::CUT_ZTOP); break;
		
		// 3D views - static eye positions
		case V3D_ISO1:			cameraPos.setEyePos( eye3DX,  eye3DY,  eye3DZ); cameraPos.setUpPos(GLI::CameraPosition::UpType::CUT_ZTOP); break;
		case V3D_ISO2: 			cameraPos.setEyePos( eye3DX, -eye3DY,  eye3DZ); cameraPos.setUpPos(GLI::CameraPosition::UpType::CUT_ZTOP); break;
		case V3D_ISO3:			cameraPos.setEyePos(-eye3DX, -eye3DY,  eye3DZ); cameraPos.setUpPos(GLI::CameraPosition::UpType::CUT_ZTOP); break; 
		case V3D_ISO4:			cameraPos.setEyePos(-eye3DX,  eye3DY,  eye3DZ); cameraPos.setUpPos(GLI::CameraPosition::UpType::CUT_ZTOP); break;
		
		// 3D views - dynamic eye positions
		case V2D_CAM_ROT_XY_ZTOP:	break; // nothing to do here the camera postion will be configured externally
	}
	
	gluLookAt (cameraPos.getEyeX(),    cameraPos.getEyeY(),    cameraPos.getEyeZ(),  
			   cameraPos.getCenterX(), cameraPos.getCenterY(), cameraPos.getCenterZ(),
			   cameraPos.getUpX(),     cameraPos.getUpY(),     cameraPos.getUpZ());
}
/////////////////////////////////////////////////////////////////
void GLContextBase::reshape(int w, int h, int x, int y) {
/////////////////////////////////////////////////////////////////
	determineViewPort(w, h, x, y);
	determineProjection(w, h);
}
/////////////////////////////////////////////////////////////////
void GLContextBase::reshapeViewMode(int w, int h) {
/////////////////////////////////////////////////////////////////
	int x = 0, y = 0;
	// evaluate the corrsponding origin coordinates
	viewPort->getPreDefCoordinatesXY(convertViewMode(viewMode), w, h, x, y);
	reshape(w, h, x, y);
}
/////////////////////////////////////////////////////////////////
void GLContextBase::reshapeViewMode() {
/////////////////////////////////////////////////////////////////
	reshapeViewMode(viewPort->getCurrentWindowWidth(), viewPort->getCurrentWindowHeigth());
}
/////////////////////////////////////////////////////////////////
void GLContextBase::display() {
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
		glScalef(modelScale.factX() * viewPort->getDisplayFactor() * zoom, 
				 modelScale.factY() * viewPort->getDisplayFactor() * zoom, 
				 modelScale.factZ() * viewPort->getDisplayFactor() * zoom); 
		// rotate
		glRotatef(modelRotate.angleX(), 1.0f, 0.0f, 0.0f);
		glRotatef(modelRotate.angleY(), 0.0f, 1.0f, 0.0f);
		glRotatef(modelRotate.angleZ(), 0.0f, 0.0f, 1.0f);
				 
		// draw the scene
		determineModel();
		
		// draw the crosshair or whatever defined
		if ( isPositionMarkerEnabled() )
			markCurrentPosition();
		
	glPopMatrix();
	
	// draw additional things
	glPushMatrix();
	
		// bounderies - todo
		if ( drawViewPortBounderies == true )
			determineViewPortBounderies();
			
	glPopMatrix();
	
	// draw coordinate origin
	glPushMatrix();
	
		drawCoordinateOrigin();
		
	glPopMatrix();
	
	glFlush();
	checkGLError();
}












