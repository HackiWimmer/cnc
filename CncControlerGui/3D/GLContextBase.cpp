#include <iostream>
#include <sstream>
#include <wx/tokenzr.h>
#include "wxCrafterImages.h"
#include "CncConfig.h"
#include "3D/GLCommon.h"
#include "3D/GLContextBase.h"

/////////////////////////////////////////////////////////////////
int32_t GLContextBase::MouseVertexInfo::getAsStepsX (float scaleFactor) { return x / scaleFactor * THE_CONFIG->getDispFactX3D(); }
int32_t GLContextBase::MouseVertexInfo::getAsStepsY (float scaleFactor) { return y / scaleFactor * THE_CONFIG->getDispFactY3D(); }
int32_t GLContextBase::MouseVertexInfo::getAsStepsZ (float scaleFactor) { return z / scaleFactor * THE_CONFIG->getDispFactZ3D(); }
double  GLContextBase::MouseVertexInfo::getAsMetricX(float scaleFactor) { return THE_CONFIG->convertStepsToMetricX(getAsStepsX(scaleFactor)); }
double  GLContextBase::MouseVertexInfo::getAsMetricY(float scaleFactor) { return THE_CONFIG->convertStepsToMetricY(getAsStepsY(scaleFactor)); }
double  GLContextBase::MouseVertexInfo::getAsMetricZ(float scaleFactor) { return THE_CONFIG->convertStepsToMetricZ(getAsStepsZ(scaleFactor)); }

const wxGLCanvas* GLContextBase::currentCanvas = NULL;

/////////////////////////////////////////////////////////////////
GLContextBase::GLContextBase(wxGLCanvas* canvas, const wxString& name) 
: wxGLContext				(canvas)
, associatedCanvas			(canvas)
, contextName				(name)
, enabled					(true)
, initialized				(false)
, guidePathes				()
, currentMouseVertexInfo	()
, options					()
, zoom						(1.0f)
, viewMode					(V2D_TOP)
, coordOriginInfo			()
, modelType					(ModelType::MT_RIGHT_HAND)
, viewPort					(NULL)
, modelScale				()
, modelRotate				()
, cameraPos					()
, lastReshapeX				(0)
, lastReshapeY				(0)
, lastReshapeW				(0)
, lastReshapeH				(0)
, theTexture				(0)
{
/////////////////////////////////////////////////////////////////
	// With respect to the GTK implementation SetCurrent() isn't 
	// possible valid before the canvas was already shown in screen
	//if ( canvas != NULL )
		//SetCurrent(*canvas);
	
	// ensure the view mode and the model rotation is in line,
	// for this the view mode here has to be different to the
	// initialization above
	setViewMode(V3D_ISO1);
}
/////////////////////////////////////////////////////////////////
GLContextBase::~GLContextBase() {
/////////////////////////////////////////////////////////////////
	clearGuidePathes();
	
	if ( viewPort != NULL )
		delete viewPort;
		
	viewPort = NULL;
}
/////////////////////////////////////////////////////////////////
void GLContextBase::clearGuidePathes() {
/////////////////////////////////////////////////////////////////
	guidePathes.clear();
}
/////////////////////////////////////////////////////////////////
void GLContextBase::addGuidePath(const GLGuidePath& gp) {
/////////////////////////////////////////////////////////////////
	guidePathes.push_back(std::move(gp));
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
bool GLContextBase::init() {
/////////////////////////////////////////////////////////////////
	if ( GLCommon::initOpenGL() == false )
		return false;

	glutInitDisplayMode (GL_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	
	initBufferStore();
	initContext();
	
	// call the initalization only one time
	if ( initialized == true )
		return true;
	
	viewPort = createViewPort();
	initialized = true;

	return true;
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
void GLContextBase::drawCrossHair() {
/////////////////////////////////////////////////////////////////
	if ( isViewMode2D() )
		drawPosMarker(currentMouseVertexInfo.x, currentMouseVertexInfo.y, currentMouseVertexInfo.z);
		
	currentMouseVertexInfo.reset();
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
void GLContextBase::drawTeapot(void) {
/////////////////////////////////////////////////////////////////
	if ( GLCommon::isGlewAvailable() == false )
		return;
		
	glPushMatrix();

		glColor3f (1.0, 0.5, 0.1);
		glutSolidTeapot (1.0);

		//glLineWidth (2.0);
		
		//glColor3f (0.0, 0.2, 0.9);
		//glutWireTeapot (1.01);

	glPopMatrix();
}
/////////////////////////////////////////////////////////////////
void GLContextBase::determineModel() {
/////////////////////////////////////////////////////////////////
	glMatrixMode(GL_MODELVIEW);
	
	// this function has to be overridden by derived classes
	// to define what is to draw
	
	// default funny behaviour
	drawTeapot();
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
	if ( lastReshapeX == 0 && lastReshapeY == 0 ) {
		lastReshapeX = w/2;
		lastReshapeY = h/2;
	}

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
	options.autoScale = as;
	normalizeScaling();
}
/////////////////////////////////////////////////////////////////
void GLContextBase::normalizeScaling() {
/////////////////////////////////////////////////////////////////
	if ( options.autoScale == true )
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
	const float scaleFact = viewPort->getDisplayFactor() * zoom / 0.1;
	return scaleFact * modelScale.getMaxScaleFactor();
}
/////////////////////////////////////////////////////////////////
float GLContextBase::getCurrentScaleFactor() {
/////////////////////////////////////////////////////////////////
	const float scaleFact = viewPort->getDisplayFactor() * zoom / getAutoScaleFactor();
	return scaleFact * modelScale.getScaleFactor();
}
/////////////////////////////////////////////////////////////////
void GLContextBase::decorateProbeMode(bool state) {
/////////////////////////////////////////////////////////////////
	options.probeMode = state;
}
/////////////////////////////////////////////////////////////////
void GLContextBase::display() {
/////////////////////////////////////////////////////////////////
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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
		if ( GL_COMMON_CHECK_ERROR > 0 )
			return;
				 
		// rotate
		glRotatef(modelRotate.angleX(), 1.0f, 0.0f, 0.0f);
		glRotatef(modelRotate.angleY(), 0.0f, 1.0f, 0.0f);
		glRotatef(modelRotate.angleZ(), 0.0f, 0.0f, 1.0f);
		if ( GL_COMMON_CHECK_ERROR > 0 )
			return;
		
		// draw the scene
		determineModel();
		if ( GL_COMMON_CHECK_ERROR > 0 ) 
			return;
		
		// draw the crosshair or whatever defined
		if ( options.showPosMarker ) {
			markCurrentPosition();
			if ( GL_COMMON_CHECK_ERROR > 0 )
				return;
		}
		
	glPopMatrix();
	
	// draw additional things
	glPushMatrix();
	
		if ( options.showViewPortBounderies == true ) {
			determineViewPortBounderies();
			if ( GL_COMMON_CHECK_ERROR > 0 )
				return;
		}
			
	glPopMatrix();
	
	// draw coordinate origin
	glPushMatrix();
		
 		if ( options.showOrigin == true ) {
			drawCoordinateOrigin();
			if ( GL_COMMON_CHECK_ERROR > 0 )
				return;
		}
		
	glPopMatrix();
	
	// draw crosshair
	glPushMatrix();
	
		if ( currentMouseVertexInfo.showCrossHair == true )
			drawCrossHair();

	glPopMatrix();

	glFlush();
	GL_COMMON_CHECK_ERROR;
	
}
////////////////////////////////////////////////////////////////
void GLContextBase::delWinCoordsToVertex() {
////////////////////////////////////////////////////////////////
	currentMouseVertexInfo.reset();
}
////////////////////////////////////////////////////////////////
bool GLContextBase::logWinCoordsToVertex(int winX, int winY) {
////////////////////////////////////////////////////////////////
	currentMouseVertexInfo.showCrossHair = convertWinCoordsToVertex(winX, 
																	winY, 
																	currentMouseVertexInfo.x, 
																	currentMouseVertexInfo.y, 
																	currentMouseVertexInfo.z
																    );
	//std::cout << wxString::Format(" logWinCoordsToVertex(%d): %9.6lf, %9.6lf, %9.6lf", currentMouseVertexInfo.showCrossHair, currentMouseVertexInfo.x, currentMouseVertexInfo.y, currentMouseVertexInfo.z) << std::endl;
	return currentMouseVertexInfo.showCrossHair;
}
////////////////////////////////////////////////////////////////
bool GLContextBase::convertWinCoordsToVertex(int winX, int winY, GLdouble & vX, GLdouble & vY, GLdouble & vZ) {
/////////////////////////////////////////////////////////////////
	GLint 	 glViewport[4];
	GLdouble glModelview[16];
	GLdouble glProjection[16];

	if ( viewPort == NULL )
		return false;

	glGetDoublev( GL_MODELVIEW_MATRIX, 	glModelview );
	glGetDoublev( GL_PROJECTION_MATRIX, glProjection );
	glGetIntegerv( GL_VIEWPORT, 		glViewport );

	GLfloat x = (float)winX;
	GLfloat y = (float)viewPort->getCurrentWindowHeigth() - (float)winY;
	GLfloat z = 0;
	
	glReadPixels(winX, (int)y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z );
	return gluUnProject(x, y, z, glModelview, glProjection, glViewport, &vX, &vY, &vZ) == GLU_TRUE;
}
/////////////////////////////////////////////////////////////////
GLuint GLContextBase::LoadBMP(const wxImage& img) {
/////////////////////////////////////////////////////////////////
	const unsigned char *data = img.GetData();
	unsigned int width        = img.GetWidth();
	unsigned int height       = img.GetHeight();
	
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	return texture;
}
/////////////////////////////////////////////////////////////////
void GLContextBase::drawBox(GLfloat size, GLenum type) {
/////////////////////////////////////////////////////////////////
	static GLfloat n[6][3] =
	{
	{-1.0, 0.0, 0.0},
	{0.0, 1.0, 0.0},
	{1.0, 0.0, 0.0},
	{0.0, -1.0, 0.0},
	{0.0, 0.0, 1.0},
	{0.0, 0.0, -1.0}
	};
	
	static GLint faces[6][4] =
	{
	{0, 1, 2, 3},
	{3, 2, 6, 7},
	{7, 6, 5, 4},
	{4, 5, 1, 0},
	{5, 6, 2, 1},
	{7, 4, 0, 3}
	};
	
	if ( theTexture == 0 ) {
		wxBitmap bmp = ImageLibBig().Bitmap("BMP_CNC");
		wxImage img  = bmp.ConvertToImage();
		theTexture   = LoadBMP(img);
	}

	GLfloat v[8][3];
	GLint i;

	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = +size / 2;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = +size / 2;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = +size / 2;
	
	glBindTexture(GL_TEXTURE_2D, theTexture);
	
	for (i = 5; i >= 0; i--) {
		glBegin(type);
			glNormal3fv(&n[i][0]);
			glTexCoord2f(0.0, 0.0); glVertex3fv(&v[faces[i][0]][0]);
			glTexCoord2f(1.0, 0.0); glVertex3fv(&v[faces[i][1]][0]);
			glTexCoord2f(1.0, 1.0); glVertex3fv(&v[faces[i][2]][0]);
			glTexCoord2f(0.0, 1.0); glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}
}
