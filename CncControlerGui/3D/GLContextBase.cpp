#include <iostream>
#include <sstream>
#include <wx/tokenzr.h>
#include "wxCrafterImages.h"
#include "MainFrame.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "CncVector.h"
#include "CncBoundarySpace.h"
#include "3D/GLCommon.h"
#include "3D/GLContextBase.h"

/////////////////////////////////////////////////////////////////
int32_t GLContextBase::MouseVertexInfo::getAsStepsX (float scaleFactor) { return vecX / 1 * THE_CONFIG->getDispFactX3D(); }
int32_t GLContextBase::MouseVertexInfo::getAsStepsY (float scaleFactor) { return vecY / 1 * THE_CONFIG->getDispFactY3D(); }
int32_t GLContextBase::MouseVertexInfo::getAsStepsZ (float scaleFactor) { return vecZ / 1 * THE_CONFIG->getDispFactZ3D(); }
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
, frontCatchingMode			(FCM_KEEP_IN_FRAME)
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
, spindlePowerState			(SPINDLE_STATE_OFF)
, theTexture				(0)
, feedbackVertices			()
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
	// normally before creating the first context
	// So, the initialization here is globally. 
	// If context sensitive initialize is needed use init() instead
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	
	// Hint: The scissor test affects glClear. so glEnable(GL_SCISSOR_TEST); and glScissor(x, y, width, height); 
	// All measurements are in pixels. After this, all calls to glClear will only clear the area in the scissor box. Remember to disable it again afterwards. 
	//glEnable(GL_SCISSOR_TEST);
	
	glEnable(GL_LINE_SMOOTH);
	
	glEnable(GL_DEPTH_TEST);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
}
/////////////////////////////////////////////////////////////////
bool GLContextBase::init() {
/////////////////////////////////////////////////////////////////
	if ( GLCommon::initOpenGL() == false )
		return false;

	glutInitDisplayMode (GL_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	
	initBufferStore();
	initContext();
	
	// call the initialization only one time
	if ( initialized == true )
		return true;
	
	viewPort = createViewPort();
	initialized = true;

	return true;
}
/////////////////////////////////////////////////////////////////
CncLongPosition GLContextBase::evaluateWindowCoordinates(double px, double py, double pz) const {
/////////////////////////////////////////////////////////////////
	GLint		curGlViewport	[ 4];
	GLdouble	curGlModelview	[16];
	GLdouble	curGlProjection	[16];
	
	glGetDoublev ( GL_MODELVIEW_MATRIX,		curGlModelview  );
	glGetDoublev ( GL_PROJECTION_MATRIX,	curGlProjection );
	glGetIntegerv( GL_VIEWPORT,				curGlViewport   );
	
	GLdouble winX, winY, winZ;
	if ( gluProject(	px, py, pz, 
						curGlModelview, curGlProjection, curGlViewport, 
						&winX, &winY, &winZ) 
		== GLU_FALSE )
	{
		 std::cerr << CNC_LOG_FUNCT_A(": Call of gluProject() failed");
		 
		 // any default in this case
		 return CncLongPosition();
	}
	
	return CncLongPosition(round(winX), round(winY), round(winZ));
}
/////////////////////////////////////////////////////////////////
CncLongPosition GLContextBase::evaluateWindowCoordinates(const CncDoublePosition& p) const {
/////////////////////////////////////////////////////////////////
	return evaluateWindowCoordinates(p.getX(), p.getY(), p.getZ());
}
/////////////////////////////////////////////////////////////////
int GLContextBase::getVertextMostlyInDirection(wxEdge direction, const CncDoublePositionVector& v) {
/////////////////////////////////////////////////////////////////
	long minX = LONG_MAX, maxX = LONG_MIN;
	long minY = LONG_MAX, maxY = LONG_MIN;
	
	int index = -1;
	
	for ( auto it = v.begin(); it != v.end(); ++it )
	{
		const CncDoublePosition&	pv = (*it);
		const CncLongPosition 		pw(evaluateWindowCoordinates(pv));
		
		const long winX(pw.getX());
		const long winY(pw.getY());
		
		switch ( direction )
		{
			case wxLeft:	if ( winX < minX ) { minX = winX; index = std::distance(v.begin(), it); } break;
			case wxRight:	if ( winX > maxX ) { maxX = winX; index = std::distance(v.begin(), it); } break;
			
			case wxTop:		if ( winY > maxY ) { maxY = winY; index = std::distance(v.begin(), it); } break;
			case wxBottom:	if ( winY < minY ) { minY = winY; index = std::distance(v.begin(), it); } break;
			
			default:		;
		}
	}
	
	return index;
}
/////////////////////////////////////////////////////////////////
int GLContextBase::getCornerIdxMostlyInDirection(wxEdge direction, const CncDoubleBoundaries& b) {
/////////////////////////////////////////////////////////////////
	CncDoubleBoundaries::Corners corners;
	b.getAllCorners(corners, CncDoubleBoundaries::CornerArea::CA_ALL);
	
	return getVertextMostlyInDirection(direction, corners);
}
/////////////////////////////////////////////////////////////////
bool GLContextBase::getBounderies(CncDoubleBoundaries& ret) const {
/////////////////////////////////////////////////////////////////
	// inherited classes have to provide a real bound box
	// here nop further path information available
	ret.reset();
	return ret.hasBoundaries();
}
/////////////////////////////////////////////////////////////////
wxPoint GLContextBase::determineBestOrigin(const CncDoubleBoundaries& box) {
/////////////////////////////////////////////////////////////////
	const wxSize cs = associatedCanvas->GetClientSize();
	
	const int ix1 = getCornerIdxMostlyInDirection(wxLeft,	box);
	const int ix2 = getCornerIdxMostlyInDirection(wxRight,	box);
	const int iy1 = getCornerIdxMostlyInDirection(wxTop, 	box);
	const int iy2 = getCornerIdxMostlyInDirection(wxBottom,	box);

	const CncDoublePosition px1 = box.getCorner(CncDoubleBoundaries::CornerArea::CA_ALL, ix1);
	const CncDoublePosition px2 = box.getCorner(CncDoubleBoundaries::CornerArea::CA_ALL, ix2);
	const CncDoublePosition py1 = box.getCorner(CncDoubleBoundaries::CornerArea::CA_ALL, iy1);
	const CncDoublePosition py2 = box.getCorner(CncDoubleBoundaries::CornerArea::CA_ALL, iy2);
	
	const double totalX = fabs(px1.getX()) + fabs(px2.getX());
	const double totalY = fabs(py1.getY()) + fabs(py2.getY());
	
	const bool bx = cnc::dblCmp::nu(totalX) == false;
	const bool by = cnc::dblCmp::nu(totalY) == false;
	
	if ( bx && by )
	{
		const float xRatio = fabs(px1.getX()) / totalX;
		const float yRatio = fabs(py1.getY()) / totalY;
	
		wxPoint ret(xRatio * cs.GetWidth(), cs.GetHeight() - (yRatio * cs.GetHeight()));
		if ( ret.x != 0 && ret.y != 0 )
			return ret;
	}
	
	return viewPort->evaluatePreDefPositions(convertViewMode(viewMode), cs.GetWidth(), cs.GetHeight());
}
/////////////////////////////////////////////////////////////////
void GLContextBase::setSpindlePowerState(CncSpindlePowerState state) {
/////////////////////////////////////////////////////////////////
	spindlePowerState = state;
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
void GLContextBase::drawSilhouetteCone(GLdouble radius, GLdouble height, GLint slices, GLint stacks, bool bottomUp) {
/////////////////////////////////////////////////////////////////
	GLUquadricObj* quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);
	gluQuadricDrawStyle(quadric, GLU_SILHOUETTE);
	
	if ( bottomUp == true )		gluCylinder(quadric, radius, 0.0, height, slices, stacks);
	else						gluCylinder(quadric, 0.0, radius, height, slices, stacks);
	
	gluDeleteQuadric(quadric);
}
/////////////////////////////////////////////////////////////////
void GLContextBase::drawSolidCone(GLdouble radius, GLdouble height, GLint slices, GLint stacks, bool bottomUp) {
/////////////////////////////////////////////////////////////////
	GLUquadricObj* quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);
	gluQuadricDrawStyle(quadric, GLU_FILL);
	
	if ( bottomUp == true )		gluCylinder(quadric, radius, 0.0, height, slices, stacks);
	else						gluCylinder(quadric, 0.0, radius, height, slices, stacks);
	
	gluDeleteQuadric(quadric);
}
/////////////////////////////////////////////////////////////////
void GLContextBase::drawSolidCylinder(GLdouble radius, GLdouble height, GLint slices, GLint stacks) {
/////////////////////////////////////////////////////////////////
	GLUquadricObj* quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);
	gluQuadricDrawStyle(quadric, GLU_FILL);
	gluCylinder(quadric, radius, radius, height, slices, stacks);
	gluDeleteQuadric(quadric);
}
/////////////////////////////////////////////////////////////////
void GLContextBase::drawMillingCutter(CncDimensions d, float x, float y, float z) {
/////////////////////////////////////////////////////////////////
	const double maxDim			= THE_BOUNDS->getMaxDimensionMetric();
	const float toolDiameter	= THE_CONTEXT->getCurrentToolDiameter();
	const float toolRadius		= toolDiameter / 2.0	/ maxDim;
	const float refRadius		=  1.0					/ maxDim;
	const float shaftRadius		=  4.0					/ maxDim;
	const float shaftLength		= 25.0					/ maxDim;
	const float totalLength		= 50.0					/ maxDim;
	
	const GLint slices = 32;
	const GLint stacks = 32;
	
	spindlePowerState == SPINDLE_STATE_ON ? glColor4ub (255, 200, 200, 32) : glColor4ub (255, 233, 157, 32);
	
	// tool part
	{
		glPushMatrix();
		
			glTranslatef(x, y, z); 

			GLUquadricObj* tool = gluNewQuadric();
			gluQuadricTexture(tool, GL_TRUE);
			gluQuadricDrawStyle(tool, spindlePowerState == SPINDLE_STATE_ON ? GLU_LINE: GLU_SILHOUETTE);
			
			switch ( d ) {
				case CncDimension3D: 
				{
					if ( toolRadius > refRadius )	gluCylinder(tool, toolRadius, toolRadius,  totalLength - shaftLength, slices, stacks);
					else							gluCylinder(tool, toolRadius, shaftRadius, totalLength - shaftLength, slices, stacks);
					
					break;
				}
				
				default: 
				{
					gluCylinder(tool, toolRadius, toolRadius,  totalLength - shaftLength, slices, stacks);
				}
			}
			
			gluDeleteQuadric(tool);
			
		glPopMatrix();
	}
	
	// shaft part
	{
		glPushMatrix();
		
			glTranslatef(x, y, z + (totalLength - shaftLength) );
			
			GLUquadricObj* shaft = gluNewQuadric();
			gluQuadricTexture(shaft, GL_TRUE);
			gluQuadricDrawStyle(shaft, spindlePowerState == SPINDLE_STATE_ON ? GLU_LINE : GLU_SILHOUETTE);
			gluCylinder(shaft, shaftRadius, shaftRadius, totalLength - shaftLength, slices * 2, stacks * 2);
			gluDeleteQuadric(shaft);
			
		glPopMatrix();
	}
}
/////////////////////////////////////////////////////////////////
GLViewPort::PreDefPos GLContextBase::convertViewMode(GLContextBase::ViewMode newMode) {
/////////////////////////////////////////////////////////////////
	if ( modelType == ModelType::MT_RIGHT_HAND ) 
	{
		switch ( newMode ) 
		{
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
	} 
	else
	{
		switch ( newMode ) 
		{
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
	switch ( viewMode ) 
	{
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
	
	/*
	centreViewport();
	
	// is this a change from 2d to 3d or vs.
	if ( getViewType() != getViewType(newMode) ) {
		// if the view type going to change the model rotation
		// has to be switched too
		if ( isViewMode2D(newMode) == true )	modelRotate.restore2DDefaults();
		else									modelRotate.restore3DDefaults();
	}
	*/
	
	
	viewMode = newMode;
}
/////////////////////////////////////////////////////////////////
void GLContextBase::centreViewport() {
/////////////////////////////////////////////////////////////////
	if ( associatedCanvas == NULL )
		return;
		
	if ( viewPort == NULL )
		return;
		
	viewPort->centreViewport();
	reshape();
}
/////////////////////////////////////////////////////////////////
void GLContextBase::resetViewport() {
/////////////////////////////////////////////////////////////////
	modelRotate.reset();
	reshapeViewMode();
}
/////////////////////////////////////////////////////////////////
void GLContextBase::determineViewPortBoundaries() {
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
	
	// rescale/ remove the main model scale factor
	const float sf = 1 / getCurrentScaleFactor();
	glScalef(sf, sf, sf);
	
	// and then rescale to the viewport enlargement, 
	// so we always have a constant sizing for the coordinate origin 
	glScalef(viewPort->getDisplayFactor(), viewPort->getDisplayFactor(), viewPort->getDisplayFactor()); 
	
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
		
	glPopMatrix();
}
/////////////////////////////////////////////////////////////////
void GLContextBase::drawCrossHair(float x, float y, float z) {
/////////////////////////////////////////////////////////////////
	if ( isViewMode2D() ) 
	{
		glLineStipple(2, 0xAAAA);
		glEnable(GL_LINE_STIPPLE);
		glColor3ub (255, 201, 14);
		glBegin(GL_LINES);

			// view the given position like a cross hair
			// over the following planes
			
			switch ( viewMode ) 
			{
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
			
	}
	else
	{
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
void GLContextBase::drawMousePosition() {
/////////////////////////////////////////////////////////////////
	if ( isViewMode2D() ) 
		drawCrossHair(currentMouseVertexInfo.vecX, currentMouseVertexInfo.vecY, currentMouseVertexInfo.vecZ);
		
	currentMouseVertexInfo.reset();
}
/////////////////////////////////////////////////////////////////
void GLContextBase::drawMovePosition(float x, float y, float z) {
/////////////////////////////////////////////////////////////////
	drawCrossHair(x, y, z);
	
	const CncDimensions dim = isViewMode3D() ? CncDimension3D : CncDimension2D;
	if ( options.showMillingCutter )  
		drawMillingCutter(dim, x, y, z);
}
/////////////////////////////////////////////////////////////////
void GLContextBase::drawAdditionalThings() {
/////////////////////////////////////////////////////////////////
	// draw additional things
	/*
	glPushMatrix();
	
		if ( options.showViewPortBoundaries == true )
		{
			determineViewPortBoundaries();
			
			if ( GL_COMMON_CHECK_ERROR > 0 )
				return;
		}
			
	glPopMatrix();
	*/
}
/////////////////////////////////////////////////////////////////
void GLContextBase::drawFeedbackVertices() {
/////////////////////////////////////////////////////////////////
	if ( feedbackVertices.size() == 0 )
		return;
		
	// ensure the right matrix
	glMatrixMode(GL_MODELVIEW);
	//glColor3f (1.0, 0.0, 0.0);
	
	// draw a point for each entry
	glPassThrough(42.0);
	glBegin(GL_POINTS);
	
		for ( auto it = feedbackVertices.begin(); it != feedbackVertices.end(); ++it)
		{
			const CncDoublePosition& p = (*it);
			glVertex3f(p.getX(), p.getY(), p.getZ());
		}
		
	glEnd();
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
void GLContextBase::determineViewPort(int w, int h) {
/////////////////////////////////////////////////////////////////
	if ( associatedCanvas == NULL )
		return;
		
	if ( associatedCanvas->IsShownOnScreen() == false )
		return;

	if ( viewPort == NULL ) {
		GLViewPort::processDefault(w, h);
		return;
	}
	
	viewPort->evaluate(w, h);
	viewPort->process();
}
/////////////////////////////////////////////////////////////////
void GLContextBase::determineViewPort(int w, int h, int x, int y) {
/////////////////////////////////////////////////////////////////
	if ( associatedCanvas == NULL )
		return;
	
	if ( associatedCanvas->IsShownOnScreen() == false )
		return;

	if ( viewPort == NULL ) {
		GLViewPort::processDefault(w, h);
		return;
	}
	
	viewPort->evaluate(w, h, x, y);
	viewPort->process();
}
/////////////////////////////////////////////////////////////////
void GLContextBase::determineProjection(int w, int h) {
/////////////////////////////////////////////////////////////////
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
		if ( isViewMode2D() )	glOrtho  (-1.0, 1.0, -1.0, 1.0, 0.1, 200.0);
		else					glOrtho  (-1.0, 1.0, -1.0, 1.0, 0.1, 200.0);//glFrustum(-1.0, 1.0, -1.0, 1.0, 3.0, 100.0);
		
		if ( associatedCanvas != NULL && associatedCanvas->IsShownOnScreen() ) {
			if ( GL_COMMON_CHECK_ERROR > 0 )
				std::cerr << CNC_LOG_FUNCT_A(": Change perspective failed\n");
		}
		
	glMatrixMode(GL_MODELVIEW);
}
/////////////////////////////////////////////////////////////////
void GLContextBase::determineCameraPosition() {
/////////////////////////////////////////////////////////////////
	const float eye2DX = 5.0;
	const float eye2DY = 5.0;
	const float eye2DZ = 5.0;
	
	const float eye3DX = 5.0;
	const float eye3DY = 5.0;
	const float eye3DZ = 5.0;
	
	switch( viewMode )
	{
		// 2D views - static eye positions
		case V2D_TOP:			cameraPos.setEyePos(    0.0,     0.0, +eye2DZ); cameraPos.setUpPos(GLI::CameraPosition::UpType::CUT_YTOP); break;
		case V2D_BOTTOM:		cameraPos.setEyePos(    0.0,     0.0, -eye2DZ); cameraPos.setUpPos(GLI::CameraPosition::UpType::CUT_YTOP); break;
		case V2D_LEFT:			cameraPos.setEyePos(-eye2DX,     0.0,     0.0); cameraPos.setUpPos(GLI::CameraPosition::UpType::CUT_ZTOP); break;
		case V2D_RIGHT:			cameraPos.setEyePos( eye2DX,     0.0,     0.0); cameraPos.setUpPos(GLI::CameraPosition::UpType::CUT_ZTOP); break;
		case V2D_FRONT:			cameraPos.setEyePos(    0.0, -eye2DY,     0.0); cameraPos.setUpPos(GLI::CameraPosition::UpType::CUT_ZTOP); break;
		case V2D_REAR:			cameraPos.setEyePos(    0.0, +eye2DY,     0.0); cameraPos.setUpPos(GLI::CameraPosition::UpType::CUT_ZTOP); break;
		
		// 3D views - static eye positions
		case V3D_ISO1:			cameraPos.setEyePos(+eye3DX, -eye3DY, +eye3DZ); cameraPos.setUpPos(GLI::CameraPosition::UpType::CUT_ZTOP); break;
		case V3D_ISO2: 			cameraPos.setEyePos(-eye3DX, -eye3DY, +eye3DZ); cameraPos.setUpPos(GLI::CameraPosition::UpType::CUT_ZTOP); break;
		case V3D_ISO3:			cameraPos.setEyePos(-eye3DX, +eye3DY, +eye3DZ); cameraPos.setUpPos(GLI::CameraPosition::UpType::CUT_ZTOP); break; 
		case V3D_ISO4:			cameraPos.setEyePos(+eye3DX, +eye3DY, +eye3DZ); cameraPos.setUpPos(GLI::CameraPosition::UpType::CUT_ZTOP); break;
		
		// 3D views - dynamic eye positions
		case V2D_CAM_ROT_XY_ZTOP:	break; // nothing to do here the camera position will be configured externally
	}
	
	glMatrixMode(GL_PROJECTION);
	
		gluLookAt (cameraPos.getEyeX(),    cameraPos.getEyeY(),    cameraPos.getEyeZ(),  
				   cameraPos.getCenterX(), cameraPos.getCenterY(), cameraPos.getCenterZ(),
				   cameraPos.getUpX(),     cameraPos.getUpY(),     cameraPos.getUpZ());
				   
		if ( associatedCanvas != NULL && associatedCanvas->IsShownOnScreen() )
		{
			if ( GL_COMMON_CHECK_ERROR > 0 )
				std::cerr << CNC_LOG_FUNCT_A(": gluLookAt failed\n");
		}
		
	glMatrixMode(GL_MODELVIEW);
}
/////////////////////////////////////////////////////////////////
int GLContextBase::getLastReshapeX() const { 
/////////////////////////////////////////////////////////////////
	if ( viewPort == NULL )
		return 0;
		
	return viewPort->getCurrentOriginX(); 
}
/////////////////////////////////////////////////////////////////
int GLContextBase::getLastReshapeY() const {  
/////////////////////////////////////////////////////////////////
	if ( viewPort == NULL )
		return 0;
		
	return viewPort->getCurrentOriginY(); 
}
/////////////////////////////////////////////////////////////////
int GLContextBase::getCurrentWindowWidth() const { 
/////////////////////////////////////////////////////////////////
	if ( viewPort == NULL )
		return 0;
		
	return viewPort->getCurrentWindowWidth(); 
}
/////////////////////////////////////////////////////////////////
int GLContextBase::getCurrentWindowHeight() const { 
/////////////////////////////////////////////////////////////////
	if ( viewPort == NULL )
		return 0;
		
	return viewPort->getCurrentWindowHeight(); 
}
/////////////////////////////////////////////////////////////////
void GLContextBase::reshape() {
/////////////////////////////////////////////////////////////////
	if ( associatedCanvas == NULL )
		return;
		
	const wxSize cs = associatedCanvas->GetClientSize();

	determineViewPort(cs.GetWidth(), cs.GetHeight());
	determineProjection(cs.GetWidth(), cs.GetHeight());
}
/////////////////////////////////////////////////////////////////
void GLContextBase::reshapeAbsolute(int px, int py) {
/////////////////////////////////////////////////////////////////
	if ( associatedCanvas == NULL )
		return;
		
	const wxSize cs = associatedCanvas->GetClientSize();
	determineViewPort(cs.GetWidth(), cs.GetHeight(), px, py);
	determineProjection(cs.GetWidth(), cs.GetHeight());
}
//////////////////////////////////////////////////
void GLContextBase::reshapeRelative(int dx, int dy) {
//////////////////////////////////////////////////
	const int px 	= getLastReshapeX() + dx;
	const int py 	= getLastReshapeY() + dy;
	
	reshapeAbsolute(px, py);
}
//////////////////////////////////////////////////
void GLContextBase::reshapePosToCenter(const CncLongBoundaries& box) {
//////////////////////////////////////////////////
	reshapePosToCenter
	(
		box.getCentre().getX(),
		box.getCentre().getY()
	);
}
/////////////////////////////////////////////////////////////////
void GLContextBase::reshapeBestOrigion(const CncDoubleBoundaries& box) {
/////////////////////////////////////////////////////////////////
	const wxPoint o = determineBestOrigin(box);
	reshapeAbsolute(o.x, o.y);
}
//////////////////////////////////////////////////
void GLContextBase::reshapePosToCenter(const CncDoubleBoundaries& box) {
//////////////////////////////////////////////////
	GLdouble winX, winY, winZ;
	
	// ----------------------------------------------------------
	auto evalWinPos = [&](GLdouble px, GLdouble py, GLdouble pz)
	{
		GLint		curGlViewport	[ 4];
		GLdouble	curGlModelview	[16];
		GLdouble	curGlProjection	[16];
		
		glGetDoublev ( GL_MODELVIEW_MATRIX,		curGlModelview  );
		glGetDoublev ( GL_PROJECTION_MATRIX,	curGlProjection );
		glGetIntegerv( GL_VIEWPORT,				curGlViewport   );
		
		gluProject(px, py, pz, curGlModelview, curGlProjection, curGlViewport, &winX, &winY, &winZ);
	};
	
	evalWinPos
	(
		box.getCentre().getX(),
		box.getCentre().getY(),
		box.getCentre().getZ()
	);
	
	reshapePosToCenter(winX, winY);
}
//////////////////////////////////////////////////
void GLContextBase::reshapePosToCenter(int px, int py) {
//////////////////////////////////////////////////
	if ( associatedCanvas == NULL )
		return;
		
	const wxSize cs = associatedCanvas->GetClientSize();
	const int dx 	= cs.GetWidth()  / 2 - px;
	const int dy 	= cs.GetHeight() / 2 - py;
	
	reshapeRelative(dx, dy);
}
//////////////////////////////////////////////////
void GLContextBase::reshapePosToCenterIfOutOfFocus(int px, int py) {
//////////////////////////////////////////////////
	if ( associatedCanvas == NULL )
		return;
		
	static int occurrenceCounter = 0;

	const wxSize wSize = associatedCanvas->GetClientSize();
	const int brd = std::max(wSize.GetWidth(), wSize.GetHeight()) * 0.1;
	
	const bool bx = px > brd && px < wSize.GetWidth()  - brd;
	const bool by = py > brd && py < wSize.GetHeight() - brd;
	
	if ( !bx || !by )
	{
		if ( (++occurrenceCounter) == 3 )
		{
			// shrink the view if out of focus occurs to often
			getModelScale().decScale();
			occurrenceCounter = 0;
		}
		
		reshapePosToCenter(px, py);
	}
}
/////////////////////////////////////////////////////////////////
void GLContextBase::reshapeViewMode() {
/////////////////////////////////////////////////////////////////
	if ( associatedCanvas == NULL )
		return;

	if ( viewPort == NULL )
		return;
		
	const wxSize cs = associatedCanvas->GetClientSize();

	// evaluate the corresponding origin coordinates
	const wxPoint origin = viewPort->evaluatePreDefPositions(convertViewMode(viewMode), cs.GetWidth(), cs.GetHeight());
	
	determineViewPort(cs.GetWidth(), cs.GetHeight(), origin.x, origin.y);
	determineProjection(cs.GetWidth(), cs.GetHeight());
}
/////////////////////////////////////////////////////////////////
void GLContextBase::reshapeCompleteVisible() {
/////////////////////////////////////////////////////////////////
	CncDoubleBoundaries b;
	
	if ( getBounderies(b) )
		reshapeCompleteVisible(b);
}
/////////////////////////////////////////////////////////////////
void GLContextBase::reshapeCompleteVisible(const CncDoubleBoundaries& box) {
/////////////////////////////////////////////////////////////////
	if ( box.hasBoundaries() )
		makeCompleteVisible(box);
}
/////////////////////////////////////////////////////////////////
void GLContextBase::setAutoScaling(bool as) {
/////////////////////////////////////////////////////////////////
	options.autoScale = as;
	if ( as == true )
		normalizeScaling();
}
/////////////////////////////////////////////////////////////////
void GLContextBase::normalizeScaling() {
/////////////////////////////////////////////////////////////////
	modelScale.resetScale();
}
/////////////////////////////////////////////////////////////////
void GLContextBase::normalizeRotation() {
/////////////////////////////////////////////////////////////////
	resetViewport();
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
float GLContextBase::getCurrentScaleFactor() const {
/////////////////////////////////////////////////////////////////
	const float scaleFact = viewPort->getDisplayFactor() * zoom;
	return scaleFact * modelScale.getScaleFactor();
}
/////////////////////////////////////////////////////////////////
void GLContextBase::decorateProbeMode(bool state) {
/////////////////////////////////////////////////////////////////
	options.probeMode = state;
}
/////////////////////////////////////////////////////////////////
void GLContextBase::traceOpenGLMatrix(std::ostream &ostr, int id) {
/////////////////////////////////////////////////////////////////
	GLfloat	matrix[16];
	glGetFloatv(id, matrix);
	
	if ( GL_COMMON_CHECK_ERROR == 0 )
		CncFloatMatrix4x4::traceRawMatrix(std::cout, matrix);
}
/////////////////////////////////////////////////////////////////
void GLContextBase::display() {
/////////////////////////////////////////////////////////////////
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	wxASSERT ( viewPort != NULL );
	
	GLint renderMode;
	glGetIntegerv(GL_RENDER_MODE, &renderMode);
	
	// first determine camera
	determineCameraPosition();
	
	// initialize model matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); 
	
		if ( GL_COMMON_CHECK_ERROR > 0 )
			return;
		
		// scale
		const float sf = getCurrentScaleFactor();
		glScalef(sf, sf, sf);
		
		if ( GL_COMMON_CHECK_ERROR > 0 )
			return;
		
		// rotate
		glRotatef(modelRotate.angleZ(), 0.0f, 0.0f, 1.0f);
		glRotatef(modelRotate.angleY(), 0.0f, 1.0f, 0.0f);
		glRotatef(modelRotate.angleX(), 1.0f, 0.0f, 0.0f);
		
		if ( GL_COMMON_CHECK_ERROR > 0 )
			return;
		
		if ( renderMode == GL_RENDER )
		{
			// draw the scene
			determineModel();
			
			if ( GL_COMMON_CHECK_ERROR > 0 ) 
				return;
		
			// draw the cross-hair or whatever defined
			if ( options.showPosMarker ) 
			{
				markCurrentPosition();
				if ( GL_COMMON_CHECK_ERROR > 0 )
					return;
			}
		
			// draw coordinate origin, use a separate matrix to keep the size constant
			glPushMatrix();
				
				if ( options.showOrigin == true )
				{
					drawCoordinateOrigin();
					if ( GL_COMMON_CHECK_ERROR > 0 )
						return;
				}
				
			glPopMatrix();
	
			// draw mouse cross-hair
			if ( currentMouseVertexInfo.valid == true )
				drawMousePosition();
		
			if ( GL_COMMON_CHECK_ERROR > 0 )
				return;
			
			// draw additional things
			drawAdditionalThings();
			
			// for testing only
			//drawFeedbackVertices();
		}
		else if ( renderMode == GL_FEEDBACK )
		{
			drawFeedbackVertices();
		}
		
	glFlush();
	GL_COMMON_CHECK_ERROR;
}
/////////////////////////////////////////////////////////////////
long GLContextBase::getPositionWithinBuffer(float x, float y, float z) {
/////////////////////////////////////////////////////////////////
	// have to be overridden by derived classes
	return -1;
}
/////////////////////////////////////////////////////////////////
wxString GLContextBase::getNormalizedClientIdOfPos(float x, float y, float z) {
/////////////////////////////////////////////////////////////////
	return _("");
}
////////////////////////////////////////////////////////////////
void GLContextBase::setFrontCatchingMode(FrontCatchingMode mode) {
////////////////////////////////////////////////////////////////
	frontCatchingMode = mode;
	
	cnc::trc.logInfoMessage(wxString::Format("Motion monitor: Front catching mode changed to: '%s'", 
												getFrontCatchingModeAsStr(frontCatchingMode)));
}
////////////////////////////////////////////////////////////////
const char* GLContextBase::getFrontCatchingModeAsStr(FrontCatchingMode mode) {
////////////////////////////////////////////////////////////////
	switch ( mode )
	{
		case FCM_OFF:				return "Off";
		case FCM_KEEP_IN_FRAME:		return "Keep always in frame";
		case FCM_ALWAYS_CENTRED:	return "Keep always centred";
	}
	
	return "???";
}
////////////////////////////////////////////////////////////////
void GLContextBase::delWinCoordsToVertex() {
////////////////////////////////////////////////////////////////
	currentMouseVertexInfo.reset();
}
////////////////////////////////////////////////////////////////
bool GLContextBase::logWinCoordsToVertex(int winX, int winY) {
////////////////////////////////////////////////////////////////
	currentMouseVertexInfo.winX = winX;
	currentMouseVertexInfo.winY = winY;
	currentMouseVertexInfo.winZ = 0;
	
	currentMouseVertexInfo.valid = 
			convertWinCoordsToVertex
			(	currentMouseVertexInfo.winX, 
				currentMouseVertexInfo.winY, 
				currentMouseVertexInfo.vecX, 
				currentMouseVertexInfo.vecY, 
				currentMouseVertexInfo.vecZ
			);
	
	if ( currentMouseVertexInfo.valid == true ) {
		currentMouseVertexInfo.bufPos = getPositionWithinBuffer(
											currentMouseVertexInfo.vecX, 
											currentMouseVertexInfo.vecY, 
											currentMouseVertexInfo.vecZ
										);
	}
	
	return currentMouseVertexInfo.valid;
}
////////////////////////////////////////////////////////////////
bool GLContextBase::convertWinCoordsToVertex(int winX, int winY, GLdouble& vecX, GLdouble& vecY, GLdouble& vecZ) {
/////////////////////////////////////////////////////////////////
	if ( viewPort == NULL )
		return false;

	GLint		curGlViewport  [ 4];
	GLdouble	curGlModelview [16];
	GLdouble	curGlProjection[16];
	
	glGetDoublev ( GL_MODELVIEW_MATRIX,		curGlModelview  );
	glGetDoublev ( GL_PROJECTION_MATRIX,	curGlProjection );
	glGetIntegerv( GL_VIEWPORT,				curGlViewport   );
	
	GLfloat x = (float)winX;
	GLfloat y = (float)(viewPort->getCurrentWindowHeight() - winY);
	GLfloat z = 0;
	
	// try to evaluate the z part on demand if possible;
	GLboolean dt; glGetBooleanv(GL_DEPTH_TEST, &dt);
	if ( dt == GL_TRUE ) 
	{
		glReadPixels(winX, (int)y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z );
		GL_COMMON_CHECK_ERROR;
	}
	
	const bool ret = gluUnProject(x, y, z, curGlModelview, curGlProjection, curGlViewport, &vecX, &vecY, &vecZ) == GLU_TRUE;
	vecZ *= -1;
	return ret;
}
/////////////////////////////////////////////////////////////////
bool GLContextBase::keepVisible(GLdouble px, GLdouble py, GLdouble pz) {
/////////////////////////////////////////////////////////////////
	if ( viewPort == NULL )
		return false;
		
	if ( associatedCanvas == NULL )
		return false;
		
	if ( frontCatchingMode == FCM_OFF )
		return true;
	
	GLint		curGlViewport	[ 4];
	GLdouble	curGlModelview	[16];
	GLdouble	curGlProjection	[16];
	
	glGetDoublev ( GL_MODELVIEW_MATRIX,		curGlModelview  );
	glGetDoublev ( GL_PROJECTION_MATRIX,	curGlProjection );
	glGetIntegerv( GL_VIEWPORT,				curGlViewport   );
	
	GLdouble winX, winY, winZ;
	gluProject(px, py, pz, curGlModelview, curGlProjection, curGlViewport, &winX, &winY, &winZ);
	
	switch ( frontCatchingMode ) 
	{
		case FCM_ALWAYS_CENTRED:	reshapePosToCenter(winX, winY);
									break;
									
		case FCM_KEEP_IN_FRAME:
		default:					reshapePosToCenterIfOutOfFocus(winX, winY);
	}
	
	return true;
}
/////////////////////////////////////////////////////////////////
bool GLContextBase::isBoxVisible(const CncDoubleBoundaries& box) {
/////////////////////////////////////////////////////////////////
	// box has always already aligned to DispFact*3D
	if ( viewPort == NULL )
		return false;
		
	if ( associatedCanvas == NULL )
		return false;
	
	const wxSize cs = associatedCanvas->GetClientSize();
	
	CncDoubleBoundaries::Corners corners;
	box.getAllCorners(corners, CncDoubleBoundaries::CornerArea::CA_ALL);
	
	for ( auto it = corners.begin(); it != corners.end(); ++it )
	{
		const CncDoublePosition& pv = (*it);
		const CncLongPosition pw(evaluateWindowCoordinates(pv));
		
		if ( pw.getX() < 0 || pw.getX() > cs.GetWidth() )
			return false;
			
		if ( pw.getY() < 0 || pw.getY() > cs.GetHeight() )
			return false;
	}
	
	return true;
}
/////////////////////////////////////////////////////////////////
bool GLContextBase::isVertexVisible(const CncDoublePosition& p) {
/////////////////////////////////////////////////////////////////
	return isVertexVisible(p.getX(), p.getY(), p.getZ());
}
/////////////////////////////////////////////////////////////////
bool GLContextBase::isVertexVisible(GLdouble px, GLdouble py, GLdouble pz) {
/////////////////////////////////////////////////////////////////
	// box has always already aligned to DispFact*3D
	if ( viewPort == NULL )
		return false;
		
	if ( associatedCanvas == NULL )
		return false;
	
	const wxSize cs = associatedCanvas->GetClientSize();

	if ( px < 0 || px > cs.GetWidth() )
		return false;
		
	if ( py < 0 || py > cs.GetHeight() )
		return false;
	
	return true;

	//Feedback buffer works currently not reliable!
	//If GL_POINTS is used only one vertex works currently within the feedback buffer 
	//this is then replicated as often as there are s points.
	
	/*
	FeedbackBuffer fb(GL_3D, this);
	
	feedbackVertices.push_back(CncDoublePosition(px, py, pz));
	display();
	feedbackVertices.clear();
	
	fb.deactivate();
	return fb.checkIfAllFeedbackPointsAreVisible();
	*/
}
/////////////////////////////////////////////////////////////////
bool GLContextBase::makeCompleteVisible(const CncDoubleBoundaries& box) {
/////////////////////////////////////////////////////////////////
	// box has always already aligned to DispFact*3D
	if ( viewPort == NULL )
		return false;
		
	if ( associatedCanvas == NULL )
		return false;
		
	if ( box.hasBoundaries() == false )
		return false;
		
	if ( options.autoScale == false )
		return false;
		
	// after a reshape always display
	reshapeBestOrigion(box);
	display();
	
	//traceBoundariesInfos(std::clog, box);
	
	// zoom in or out to the best shape to get corner 2 visible - if possible
	bool result = isBoxVisible(box);
	if ( result == false )
	{
		unsigned int count = 0;
		
		// try to zoom out 
		while ( modelScale.canDecScale() )
		{
			reshape();
			display();
			
			result = isBoxVisible(box);
			if ( result == true )
			{
				break;
			}
			
			const float stepWidth = modelScale.getScaleFactor() <= 1.0 ? modelScale.getStepWidth() / 8 : modelScale.getStepWidth();
			modelScale.decScale(stepWidth);
			
			count++;
			//std::cout << "count: " << count << " --> " << modelScale.getScaleFactor() << std::endl;
		}
	
		// the box can't scaled completely visible
		if ( result == false )
			;
	}
	else
	{
		// try to zoom in 
		while ( modelScale.canIncScale() )
		{
			const float stepWidth = modelScale.getScaleFactor() <= 1.0 ? modelScale.getStepWidth() / 8 : modelScale.getStepWidth();
			modelScale.incScale(stepWidth);
			
			reshape();
			display();
			
			result = isBoxVisible(box);
			if ( result == false )
			{
				modelScale.decScale(stepWidth);
				break;
			}
		}
	}

	return true;
}
/////////////////////////////////////////////////////////////////
void GLContextBase::traceBoundariesInfos(std::ostream& o, const CncDoubleBoundaries& box) const {
/////////////////////////////////////////////////////////////////
	CncDoubleBoundaries::Corners corners;
	box.getAllCorners(corners, CncDoubleBoundaries::CornerArea::CA_ALL);
	
	const wxSize cs = associatedCanvas->GetClientSize();
	o << "Window w / h: " << cs.GetWidth() << " / " << cs.GetHeight() << std::endl;
	
	auto evalVisiblity = [cs](const CncLongPosition& p)
	{
		if ( p.getX() < 0 || p.getX() > cs.GetWidth() )
			return false;
			
		if ( p.getY() < 0 || p.getY() > cs.GetHeight() )
			return false;
			
		return true;
	};	
	
	for ( auto it = corners.begin(); it != corners.end(); ++it )
	{
		const int distance = std::distance(corners.begin(), it);
		const CncDoublePosition& pv = (*it);
		const CncLongPosition pw(evaluateWindowCoordinates(pv));
		
		o	<< box.getCornerName(CncDoubleBoundaries::CornerArea::CA_ALL, distance) << ": "
			<< std::fixed << std::setw(10) << std::setprecision(6) << std::setfill(' ')
			<< pv
			<< " --> winCoord: "
			<< std::fixed << std::setw(10) << std::setfill(' ')
			<< pw.getX() 
			<< ", " 
			<< std::fixed << std::setw(10) << std::setfill(' ')
			<< pw.getY() 
			<< " -> visible: " 
			<< evalVisiblity(pw)
			<< std::endl
		;
	}
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
	
	for (i = 5; i >= 0; i--)
	{
		glBegin(type);
			glNormal3fv(&n[i][0]);
			glTexCoord2f(0.0, 0.0); glVertex3fv(&v[faces[i][0]][0]);
			glTexCoord2f(1.0, 0.0); glVertex3fv(&v[faces[i][1]][0]);
			glTexCoord2f(1.0, 1.0); glVertex3fv(&v[faces[i][2]][0]);
			glTexCoord2f(0.0, 1.0); glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}
}
/////////////////////////////////////////////////////////////////
std::ostream& GLContextBase::traceInformation(std::ostream& o) const {
/////////////////////////////////////////////////////////////////
	o	<< " Zoom factor                     : " << zoom						<< std::endl
		<< " Scale factor                    : " << getCurrentScaleFactor()		<< std::endl
	;
	return o;
}



/////////////////////////////////////////////////////////////////
GLContextBase::FeedbackBuffer::FeedbackBuffer(GLenum t, GLContextBase* ctx)
: type		(t)
, context	(ctx)
, size		(1024)
, count		(0)
, buffer	(new GLfloat[size])
, prevMode	(-1)
/////////////////////////////////////////////////////////////////
{
	glFeedbackBuffer(size, type, buffer);
	activate();
}
/////////////////////////////////////////////////////////////////
GLContextBase::FeedbackBuffer::~FeedbackBuffer() {
/////////////////////////////////////////////////////////////////
	deactivate();
	delete buffer;
}
/////////////////////////////////////////////////////////////////
void GLContextBase::FeedbackBuffer::activate() {
/////////////////////////////////////////////////////////////////
	glGetIntegerv(GL_RENDER_MODE, &prevMode);

	if ( prevMode != GL_FEEDBACK)
		glRenderMode(GL_FEEDBACK);
	
	count = 0;
	
	GL_COMMON_CHECK_ERROR;
}
/////////////////////////////////////////////////////////////////
GLint GLContextBase::FeedbackBuffer::deactivate() {
/////////////////////////////////////////////////////////////////
	GLint mode;
	glGetIntegerv(GL_RENDER_MODE, &mode);

	if ( mode != GL_FEEDBACK )
		return 0;
	
	GLint ret = 0;
	
	if ( prevMode > 0 )
		ret = glRenderMode(prevMode);
		
	prevMode = -1;
	
	if ( GL_COMMON_CHECK_ERROR > 0 )
		return 0;
		
	count = ret;
	
	if ( true )
		//traceFeedbackBufferRaw(std::cout);
		traceFeedbackBuffer(std::cout);
	
	return ret;
}
/////////////////////////////////////////////////////////////////
void GLContextBase::FeedbackBuffer::traceFeedbackBufferRaw(std::ostream& out) {
/////////////////////////////////////////////////////////////////
	out << std::endl;
	for ( GLint i = 0; i< count; i++ )
	{
		out << wxString::Format("% 4d: %4.2f\n", i, buffer[i]);
	}
	
	out << std::endl;
}
/////////////////////////////////////////////////////////////////
void GLContextBase::FeedbackBuffer::traceFeedbackBuffer(std::ostream& out) {
/////////////////////////////////////////////////////////////////
	// ----------------------------------------------------------
	const int ENTRY_SIZE = getEntrySize();
	auto printVertexInfo = [ENTRY_SIZE](std::ostream& o, GLint c, GLint& r, GLfloat *b)
	{
		o << "  ";
		
		for (int i = 0; i < ENTRY_SIZE; i++)
		{
			o << wxString::Format("%4.2f ", b[c - r]);
			r--;
		}
		
		o << std::endl;
	};
	
	if ( count <= 0 )
		return;
	
	// ----------------------------------------------------------
	out << std::endl;
	
	GLint	rest = count;
	while ( rest > 0 ) 
	{
		out << wxString::Format("%04d > ", count - rest);
		const GLfloat token = buffer[count - rest]; 
		
		rest--;
		
		if ( token == GL_PASS_THROUGH_TOKEN )
		{
			out << "GL_PASS_THROUGH_TOKEN " << wxString::Format("  %4.2f\n", buffer[count - rest]);
			rest--;
		}
		else if ( token == GL_POINT_TOKEN )
		{
			out << "GL_POINT_TOKEN\n";
			printVertexInfo (out, count, rest, buffer);
		}
		else if ( token == GL_LINE_TOKEN ) 
		{
			out << "GL_LINE_TOKEN\n";
			printVertexInfo (out, count, rest, buffer);
			printVertexInfo (out, count, rest, buffer);
		}
		else if ( token == GL_LINE_RESET_TOKEN )
		{
			out << "GL_LINE_RESET_TOKEN\n";
			printVertexInfo (out, count, rest, buffer);
			printVertexInfo (out, count, rest, buffer);
		}
	}
}
/////////////////////////////////////////////////////////////////
bool GLContextBase::FeedbackBuffer::checkIfAllFeedbackPointsAreVisible() {
/////////////////////////////////////////////////////////////////
	// An empty buffer never contains any visible point
	if ( count == 0 )
		return false;
	
	if ( context == NULL )
		return false;
	
	if ( false )
	{
		std::cout	<< "XYWH: " 
					<< context->getLastReshapeX()			<< ", " 
					<< context->getLastReshapeY()			<< ", " 
					<< context->getCurrentWindowWidth()		<< ", " 
					<< context->getCurrentWindowHeight()	<< std::endl
		; 
	}
	
	const int	ENTRY_SIZE		= getEntrySize();
	int			pointCounter	= 0;
	GLint		rest			= count;
	
	// over all feedback buffer entries
	while ( rest >= 0 ) 
	{
		const GLfloat token = buffer[ count - rest ]; 
		rest--;
		
		if ( token == GL_POINT_TOKEN )
		{
			pointCounter++;
			
			float x = -1.0, y = -1.0;//, z;
			for ( int i = 0; i < ENTRY_SIZE; i++ ) 
			{
				switch ( i )
				{
					case 0: x = buffer[count - rest]; break;
					case 1: y = buffer[count - rest]; break;
					//case 2: z = buffer[count - rest]; break;
				}
				
				rest--;
			}
			
			// x, y, z are delivered in window coordinates (feedback buffer standard)
			// check if x and y outside the current available window frame
			// if true the the current point was clipped (not visible)
			if ( x < float(0) )									return false;
			if ( x > float(context->getCurrentWindowWidth()) )	return false;
			
			if ( y < float(0) )									return false;
			if ( y > float(context->getCurrentWindowHeight()) )	return false;
		}
		else if ( token == GL_PASS_THROUGH_TOKEN )	{ rest = rest - 1;				}
		else if ( token == GL_LINE_TOKEN )			{ rest = rest - 2 * ENTRY_SIZE;	}
		else if ( token == GL_LINE_RESET_TOKEN )	{ rest = rest - 2 * ENTRY_SIZE;	}
	}
	
	// It's also an error if no point found
	return pointCounter > 0;
}

