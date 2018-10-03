#include <iostream>
#include <sstream>
#include <wx/tokenzr.h>
#include "3D/GLContextBase.h"

#ifdef __DARWIN__
    #include <OpenGL/glu.h>
	#include <OpenGL/glut.h>
	#include <OpenGL/freeglut.h>
#else
    #include <GL/glu.h>
	#include <GL/glut.h>
	#include <GL/freeglut.h>
	#include <GL/glext.h>
#endif

/////////////////////////////////////////////////////////////////
GLContextBase::GLContextBase(wxGLCanvas* canvas) 
: wxGLContext(canvas)
, enabled(true)
, initialized(false)
, drawViewPortBounderies(true)
, posMarker(true)
, autoScale(true)
, blending(true)
, probeMode(false)
, zoom(1.0f)
, viewMode(V2D_TOP)
, coordOriginInfo()
, modelType(ModelType::MT_RIGHT_HAND)
, viewPort(NULL)
, modelScale()
, modelRotate()
, cameraPos()
, lastReshapeX(0)
, lastReshapeY(0)
, lastReshapeW(0)
, lastReshapeH(0)
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
	static bool alreadyCalled = false;
	if ( alreadyCalled == true )
		return;
	
	// this is a static function and should be called one time
	// normallly before creating the first context
	// So, the initalization here is globally. 
	// If context sensetive initializion is needed use init() instead
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	
	// Hint: The scissor test affects glClear. so glEnable(GL_SCISSOR_TEST); and glScissor(x, y, width, height); 
	// All measurements are in pixels. After this, all calls to glClear will only clear the area in the scissor box. Remember to disable it again afterwards. 
	//glEnable(GL_SCISSOR_TEST);
	
	glEnable(GL_LINE_SMOOTH);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
}
/////////////////////////////////////////////////////////////////
void GLContextBase::traceOpenGLVersionInfo(std::ostream& s) {
/////////////////////////////////////////////////////////////////
	s << " :: OpenGL version info: ";
	s << (char*)glGetString(GL_VERSION) 	<< "; ";
	s << (char*)glGetString(GL_VENDOR) 		<< "; ";
	s << (char*)glGetString(GL_RENDERER) 	<< std::endl;
	//s << glGetString(GL_SHADING_LANGUAGE_VERSION​) << std::endl;
}
/////////////////////////////////////////////////////////////////
void GLContextBase::traceeOpenGLExtentionInfo(std::ostream& s) {
/////////////////////////////////////////////////////////////////
	s << "Extention list:" << std::endl;
	wxString ext(glGetString(GL_EXTENSIONS));
	wxStringTokenizer extentions(ext, " ");
	while ( extentions.HasMoreTokens() ) {
		wxString token = extentions.GetNextToken();
		s << " " << token << std::endl;
	}
}
/////////////////////////////////////////////////////////////////
void GLContextBase::init() {
/////////////////////////////////////////////////////////////////
	initContext();
	
	// call the initalization only one time
	if ( initialized == true )
		return;
		
	viewPort = createViewPort();
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
void GLContextBase::drawSolidCone(GLdouble base, GLdouble height, GLint slices, GLint stacks) {
/////////////////////////////////////////////////////////////////
	GLUquadricObj* quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);
	gluQuadricDrawStyle(quadric, GLU_FILL);
	gluCylinder(quadric, base, 0.0, height, slices, stacks);
	gluDeleteQuadric(quadric);
}
/////////////////////////////////////////////////////////////////
GLViewPort::PreDefPos GLContextBase::convertViewMode(GLContextBase::ViewMode newMode) {
/////////////////////////////////////////////////////////////////
	if ( modelType == ModelType::MT_RIGHT_HAND ) {
		switch ( newMode ) {
			case V2D_TOP:				return GLViewPort::PreDefPos::VPDP_BottomLeft;
			case V2D_BOTTOM:			return GLViewPort::PreDefPos::VPDP_BottomRight;
			case V2D_LEFT:				return GLViewPort::PreDefPos::VPDP_BottomRight; 
			case V2D_RIGHT:				return GLViewPort::PreDefPos::VPDP_BottomLeft; 
			case V2D_FRONT:				return GLViewPort::PreDefPos::VPDP_BottomLeft; 
			case V2D_REAR:				return GLViewPort::PreDefPos::VPDP_BottomRight;

			case V3D_ISO1:				return GLViewPort::PreDefPos::VPDP_LeftMid;
			case V3D_ISO2:				return GLViewPort::PreDefPos::VPDP_BottomMid;
			case V3D_ISO3:				return GLViewPort::PreDefPos::VPDP_RightMid;
			case V3D_ISO4:				return GLViewPort::PreDefPos::VPDP_TopMid;
			
			case V2D_CAM_ROT_XY_ZTOP:	return GLViewPort::PreDefPos::VPDP_Center;
		}
	} else {
		switch ( newMode ) {
			case V2D_TOP:				return GLViewPort::PreDefPos::VPDP_TopLeft;
			case V2D_BOTTOM:			return GLViewPort::PreDefPos::VPDP_TopRight;
			case V2D_LEFT:				return GLViewPort::PreDefPos::VPDP_BottomRight; 
			case V2D_RIGHT:				return GLViewPort::PreDefPos::VPDP_BottomLeft; 
			case V2D_FRONT:				return GLViewPort::PreDefPos::VPDP_BottomLeft; 
			case V2D_REAR:				return GLViewPort::PreDefPos::VPDP_BottomRight;

			case V3D_ISO1:				return GLViewPort::PreDefPos::VPDP_Center;
			case V3D_ISO2:				return GLViewPort::PreDefPos::VPDP_Center;
			case V3D_ISO3:				return GLViewPort::PreDefPos::VPDP_Center;
			case V3D_ISO4:				return GLViewPort::PreDefPos::VPDP_Center;
			
			case V2D_CAM_ROT_XY_ZTOP:	return GLViewPort::PreDefPos::VPDP_Center;
		}
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
void GLContextBase::setViewMode(GLContextBase::ViewMode newMode, bool force) {
/////////////////////////////////////////////////////////////////
	if ( viewMode == newMode && force == false )
		return;
	
	// is the current origin position customized?
	if ( viewPort != NULL && viewPort->getOriginPosType() == GLViewPort::VPOP_Custom ) {
		// switch to the center mode
		viewPort->resetCustomOrigPosType();
	}
	
	// is this a change from 2d to 3d or vs.
	if ( getViewType() != getViewType(newMode) ) {
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
	
	reshape(lastReshapeW, lastReshapeH, 0, 0);
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
	
	const float croneDiameter 	= 0.02f;
	const float croneHight 		= 0.07f;
	const float charOffset		= 0.006f;
	
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
			
			glColor3ub(coordOriginInfo.colours.x.Red(), coordOriginInfo.colours.x.Green(), coordOriginInfo.colours.x.Blue());
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(coordOriginInfo.length, 0.0f, 0.0f);
			
		glEnd();
		renderBitmapString(coordOriginInfo.length + croneHight + charOffset, charOffset, charOffset, GLUT_BITMAP_8_BY_13, "X");
		
		glPushMatrix();
			glTranslatef(coordOriginInfo.length, 0.0f, 0.0f);
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			drawSolidCone(croneDiameter, croneHight, 30, 30);
		glPopMatrix();
		
		// y axis
		glBegin(GL_LINES);
			
			glColor3ub(coordOriginInfo.colours.y.Red(), coordOriginInfo.colours.y.Green(), coordOriginInfo.colours.y.Blue());
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, coordOriginInfo.length, 0.0f);
		
		glEnd();
		renderBitmapString(charOffset, coordOriginInfo.length + croneHight-charOffset, charOffset, GLUT_BITMAP_8_BY_13, "Y");
			
		glPushMatrix();
			glTranslatef(0.0f, coordOriginInfo.length, 0.0f);
			glRotatef(270.0f, 1.0f, 0.0f, 0.0f);
			drawSolidCone(croneDiameter, croneHight, 30, 30);
		glPopMatrix();
		
		// z axis
		glBegin(GL_LINES);
			
			glColor3ub(coordOriginInfo.colours.z.Red(), coordOriginInfo.colours.z.Green(), coordOriginInfo.colours.z.Blue());
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, coordOriginInfo.length);
		
		glEnd();
		renderBitmapString(charOffset, charOffset, coordOriginInfo.length + croneHight + charOffset, GLUT_BITMAP_8_BY_13, "Z");
		
		glPushMatrix();
			
			glColor3ub(coordOriginInfo.colours.z.Red(), coordOriginInfo.colours.z.Green(), coordOriginInfo.colours.z.Blue());
			glTranslatef(0.0f, 0.0f, coordOriginInfo.length);
			drawSolidCone(croneDiameter, croneHight, 30, 30);
			/*
			glColor3ub(255, 255, 255);
			glTranslatef(0.0f, 0.0f, coordOriginInfo.length);
			drawSolidCone(croneDiameter * 0.8f, 0.0001f, 30, 30);
			
			glColor3ub(255, 242, 0);
			glTranslatef(0.0f, 0.0f, croneHight);
			drawSolidCone(croneDiameter * 0.5f, 0.0001f, 30, 30);
			*/
		glPopMatrix();
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
	/*
	glScissor(viewPort->getScissorX(), viewPort->getScissorY(),
			  viewPort->getScissorW(), viewPort->getScissorH());
	*/
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
	
	// this function has to be overridden by derived classes
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
void GLContextBase::reshape(int w, int h) {
/////////////////////////////////////////////////////////////////
	determineViewPort(w, h, lastReshapeX, lastReshapeY);
	determineProjection(w, h);
}
/////////////////////////////////////////////////////////////////
void GLContextBase::reshape(int w, int h, int x, int y) {
/////////////////////////////////////////////////////////////////
	lastReshapeX = x;
	lastReshapeY = y;
	lastReshapeW = w;
	lastReshapeH = h;
	
	determineViewPort(w, h, x, y);
	determineProjection(w, h);
}
/////////////////////////////////////////////////////////////////
void GLContextBase::reshapeViewMode(int w, int h) {
/////////////////////////////////////////////////////////////////
	if ( viewPort == NULL )
		return;
		
	int x = 0, y = 0;
	// evaluate the corrsponding origin coordinates
	viewPort->getPreDefCoordinatesXY(convertViewMode(viewMode), w, h, x, y);
	reshape(w, h, x, y);
}
/////////////////////////////////////////////////////////////////
void GLContextBase::reshapeViewMode() {
/////////////////////////////////////////////////////////////////
	if ( viewPort == NULL )
		return;

	reshapeViewMode(viewPort->getCurrentWindowWidth(), viewPort->getCurrentWindowHeigth());
}
/////////////////////////////////////////////////////////////////
void GLContextBase::setAutoScaling(bool as) {
/////////////////////////////////////////////////////////////////
	autoScale = as;
	normalizeScaling();
}
/////////////////////////////////////////////////////////////////
void GLContextBase::normalizeScaling() {
/////////////////////////////////////////////////////////////////
	if ( autoScale == true )
		modelScale.resetScale();
}
/////////////////////////////////////////////////////////////////
void GLContextBase::normalizeRotation() {
/////////////////////////////////////////////////////////////////
	if ( isViewMode2D() )	modelRotate.reset2DDefaults();
	else 					modelRotate.reset3DDefaults();
}
/////////////////////////////////////////////////////////////////
void GLContextBase::normalizeCamera() {
/////////////////////////////////////////////////////////////////
	cameraPos.reset();
	
	if ( viewMode == V2D_CAM_ROT_XY_ZTOP )
		viewMode = V3D_ISO1;
	
	determineCameraPosition();
}
/////////////////////////////////////////////////////////////////
float GLContextBase::getMaxScaleFactor() {
/////////////////////////////////////////////////////////////////
	float scaleFact = viewPort->getDisplayFactor() * zoom / 0.1;
	scaleFact *= modelScale.getMaxScaleFactor();
	
	return scaleFact;
}
/////////////////////////////////////////////////////////////////
float GLContextBase::getCurrentScaleFactor() {
/////////////////////////////////////////////////////////////////
	float scaleFact = viewPort->getDisplayFactor() * zoom / getAutoScaleFactor();
	scaleFact *= modelScale.factX();
	
	return scaleFact;
}
/////////////////////////////////////////////////////////////////
void GLContextBase::decorateProbeMode(bool state) {
/////////////////////////////////////////////////////////////////
	probeMode = state;
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
		float sf = getCurrentScaleFactor();
		glScalef(sf, sf, sf);
				 
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
