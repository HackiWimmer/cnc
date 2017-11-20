#include <wx/dcclient.h>

#include "3D/GLContextCncPath.h"
#include "3D/GLContextTestCube.h"
#include "GL3DOptions.h"
#include "CncConfig.h"
#include "CncCommon.h"
#include "CncMotionMonitor.h"

#ifdef __DARWIN__
    #include <OpenGL/glu.h>
	#include <OpenGL/glut.h>
#else
    #include <GL/glu.h>
	#include <GL/glut.h>
	#include <GL/glext.h>
#endif

#if !wxUSE_GLCANVAS
	#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

// ----------------------------------------------------------------------------
// GlutInitManager
// ----------------------------------------------------------------------------
class GlutLibInitManager {
	public:
		//////////////////////////////////////////////////////
		GlutLibInitManager() {
			// init glut lib a the startup
			int   argc = 1;
			char* argv[1] = { wxString("CncMotionMonitor").char_str() };
			glutInit(&argc, argv);
		}
		//////////////////////////////////////////////////////
		~GlutLibInitManager() {
		}
		
}; GlutLibInitManager _glim;

// ----------------------------------------------------------------------------
// CncMotionMonitor Event Table
// ----------------------------------------------------------------------------
wxDEFINE_EVENT(wxEVT_MONTION_MONITOR_TIMER, wxTimerEvent);

wxBEGIN_EVENT_TABLE(CncMotionMonitor, wxGLCanvas)
	EVT_PAINT(CncMotionMonitor::onPaint)
	EVT_SIZE(CncMotionMonitor::onSize)
	EVT_ERASE_BACKGROUND(CncMotionMonitor::onEraseBackground)
	EVT_MOUSE_EVENTS(CncMotionMonitor::onMouse)
	EVT_KEY_DOWN(CncMotionMonitor::onKeyDown)
	EVT_TIMER(wxEVT_MONTION_MONITOR_TIMER, CncMotionMonitor::onCameraRotationTimer)
wxEND_EVENT_TABLE()

//////////////////////////////////////////////////
CncMotionMonitor::CncMotionMonitor(wxWindow *parent, int *attribList) 
: wxGLCanvas(parent, wxID_ANY, attribList, wxDefaultPosition, wxDefaultSize, 
	         wxFULL_REPAINT_ON_RESIZE)
, flags()
, monitor(new GLContextCncPath(this))
, testCube(new GLContextTestCube(this))
, optionDlg(new GL3DOptions(this))
, cameraRotationTimer(this, wxEVT_MONTION_MONITOR_TIMER)
, cameraRotationStepWidth(0)
, cameraRotationSpeed(100)
, isShown(false)
, zoom(2.0f)
, currentClientID(-1L)
{
//////////////////////////////////////////////////
	GLContextBase::globalInit(); 
	monitor->init();
	testCube->init();
	
	// publish initial zoom factor
	monitor->setZoomFactor(zoom);
	
	// deactivte process mode by default
	popProcessMode();
}
//////////////////////////////////////////////////
CncMotionMonitor::~CncMotionMonitor() {
//////////////////////////////////////////////////
	if ( monitor != NULL ) 
		delete monitor;

	if ( testCube != NULL ) 
		delete testCube;

	if ( optionDlg != NULL ) 
		delete optionDlg;
}
//////////////////////////////////////////////////
void CncMotionMonitor::enable(bool state) {
//////////////////////////////////////////////////
	monitor->enable(state);
}
//////////////////////////////////////////////////
void CncMotionMonitor::clear() {
//////////////////////////////////////////////////
	monitor->clearPathData();
	display();
}
//////////////////////////////////////////////////
void CncMotionMonitor::tracePathData(std::ostream& s) {
//////////////////////////////////////////////////
	monitor->tracePathData(s);
}
//////////////////////////////////////////////////
void CncMotionMonitor::setModelType(const GLContextBase::ModelType mt) {
//////////////////////////////////////////////////
	monitor->setModelType(mt);
	monitor->setViewMode(GLContextBase::ViewMode::V3D_ISO1, true);
	monitor->reshapeViewMode();
}
//////////////////////////////////////////////////
void CncMotionMonitor::reconstruct() {
//////////////////////////////////////////////////
	GLI::GLCncPath tmpPath, curPath = monitor->getPathData();
	// copy the current path
	tmpPath.swap(curPath);
	// clear the real monitor path data - keep in mind curPath is only a local copy
	monitor->clearPathData();
	
	pushProcessMode();
	
		// reconstruct
		GLI::GLCncPathVertices d;
		for ( GLI::GLCncPath::iterator it = tmpPath.begin(); it != tmpPath.end(); ++it )
			appendVertice(it->getId(), it->getX(), it->getY(), it->getZ(), it->getCncMode());
		
		// redraw the scene with new properties
		display();
	
	popProcessMode();
}
//////////////////////////////////////////////////
void CncMotionMonitor::display() {
	//Refresh(false);
	onPaint();
}
//////////////////////////////////////////////////
void CncMotionMonitor::appendVertice(const GLI::VerticeLongData& vd) {
//////////////////////////////////////////////////	
	// Convert the given steps (abs) to a glpos (rel):
	
	// To do this first we need to normalize the values for x, y and z 
	// to a common unit like [mm], because [steps] are depends on 
	// the corresponding pitch which is configured for each axis separatly 
	float x = vd.getX() / CncConfig::getGlobalCncConfig()->getDispFactX3D(); 
	float y = vd.getY() / CncConfig::getGlobalCncConfig()->getDispFactY3D();
	float z = vd.getZ() / CncConfig::getGlobalCncConfig()->getDispFactZ3D();
	
	appendVertice(vd.getId(), x, y, z, vd.getMode());
}
//////////////////////////////////////////////////
void CncMotionMonitor::appendVertice(long id, float x, float y, float z, GLI::GLCncPathVertices::CncMode cm) {
//////////////////////////////////////////////////
	// x, y, z have to be given as glpos

	typedef GLI::GLCncPathVertices::FormatType PathVerticeType;
	typedef GLI::GLCncPathVertices::CncMode    DataVerticeMode;
	
	static wxColour 		colour;
	static PathVerticeType	formatType;
	
	// decorate
	switch ( cm ) {
		case DataVerticeMode::CM_WORK:	colour		= getFlags().workColour;
										formatType	= PathVerticeType::FT_SOLID;
										break;
										
		case DataVerticeMode::CM_FLY:	colour 		= getFlags().flyColour;
										formatType	= ( getFlags().drawFlyPath == true ? PathVerticeType::FT_DOT : PathVerticeType::FT_TRANSPARENT );
										break;
	}
	
	// append
	// todo - avoid duplicates last != new
	static GLI::GLCncPathVertices d;
	monitor->appendPathData(d.set(id, x, y, z, colour, formatType, cm)); 
}
//////////////////////////////////////////////////
void CncMotionMonitor::view(GLContextBase::ViewMode fm) {
//////////////////////////////////////////////////
	monitor->setViewMode(fm);
	
	const wxSize cs = GetClientSize();
	monitor->reshapeViewMode(cs.GetWidth(), cs.GetHeight());
	
	display();
}
/////////////////////////////////////////////////////////////////
void CncMotionMonitor::centerViewport() {
/////////////////////////////////////////////////////////////////
	monitor->centerViewport();
	display();
}
/////////////////////////////////////////////////////////////////
void CncMotionMonitor::resetRotation() {
/////////////////////////////////////////////////////////////////
	monitor->getModelRotation().reset2DDefaults();
	display();
}
/////////////////////////////////////////////////////////////////
void CncMotionMonitor::onPaint() {
/////////////////////////////////////////////////////////////////
	monitor->SetCurrent(*this);
	monitor->init();

	const wxSize cs = GetClientSize();
	
	if ( isShown )	monitor->reshape(cs.GetWidth(), cs.GetHeight());
	else 			monitor->reshapeViewMode(cs.GetWidth(), cs.GetHeight());
	monitor->display();
	
	// The first onPaint() if IsShownOnScreen() == true have to reshape the view mode
	// later this should not appear to support custom origin positions
	// see if above
	isShown = IsShownOnScreen();
	
	SwapBuffers();
}
//////////////////////////////////////////////////
void CncMotionMonitor::onPaint(wxPaintEvent& event) {
//////////////////////////////////////////////////
	// This is required even though dc is not used otherwise.
	wxPaintDC dc(this);
	onPaint();
}
//////////////////////////////////////////////////
void CncMotionMonitor::onSize(wxSizeEvent& event) {
//////////////////////////////////////////////////
	const wxSize cs = GetClientSize();
	
	monitor->reshapeViewMode(cs.GetWidth(), cs.GetHeight());
	monitor->display();
}
//////////////////////////////////////////////////
void CncMotionMonitor::onEraseBackground(wxEraseEvent& event) {
//////////////////////////////////////////////////
	// do nothing, only to avoid flashing on MSW
}
//////////////////////////////////////////////////
void CncMotionMonitor::onMouse(wxMouseEvent& event) {
//////////////////////////////////////////////////
	const wxSize cs = GetClientSize();

	// activate the keyboard focus for this frame
	this->SetFocusFromKbd();
	
	// wheel
	int rot = event.GetWheelRotation();
	if ( rot != 0 ) {
		if (rot < 0 ) 	monitor->getModelScale().decScale();
		else 			monitor->getModelScale().incScale();
		display();
	}
	
	if ( false ) {
		#warning - how to?
		static int ox, oy, x, y;
		
		if ( event.LeftDown() == true ) {
			ox = monitor->getOriginX() / 8;
			oy = monitor->getOriginY() / 8;
			x = event.GetX();
			y = event.GetY();
			
			cerr << x << endl;
			cerr << y << endl;
		}

		if ( event.LeftUp() == true ) {
			int dx = event.GetX() - x;
			int dy = event.GetY() - y;
			
			ox += dx;
			oy += dy;
			oy = cs.GetHeight() - y;
			
			clog << dx << ", " << ox << endl;
			clog << dy << ", " << oy << endl;

			monitor->reshape(cs.GetWidth(), cs.GetHeight(), ox, oy);
			display();
		}
	} else {
		
		// left button
		if ( event.LeftIsDown() == true ) {
			// reverse y because the opengl viewport origin (0,0) is at left/bottom
			int x = event.GetX();
			int y = cs.GetHeight() - event.GetY();
			
			monitor->reshape(cs.GetWidth(), cs.GetHeight(), x, y);
			display();
		}
	}
}
//////////////////////////////////////////////////
void CncMotionMonitor::onKeyDown(wxKeyEvent& event) {
//////////////////////////////////////////////////
	switch ( event.GetKeyCode() ) {
		case 'O':	showOptionDialog();
					break;
					
		case 'C':	monitor->centerViewport();
					display();
					break;
	}
}
//////////////////////////////////////////////////
void CncMotionMonitor::setCameraRotationSpeed(int speed) {
//////////////////////////////////////////////////
	cameraRotationSpeed = speed;
	
	// update timer settings on demand
	if ( cameraRotationTimer.IsRunning() ) {
		cameraRotationTimer.Stop();
		cameraRotationTimer.Start(cameraRotationSpeed);
	}
}
//////////////////////////////////////////////////
void CncMotionMonitor::cameraRotationTimerHandler(CncMotionMonitor::CameraMode cm) {
//////////////////////////////////////////////////
	if ( cameraRotationTimer.IsRunning() ) {
		cameraRotationTimer.Stop();
	}
	
	typedef CncMotionMonitor::CameraMode Mode;
	switch ( cm ) {
		case Mode::CM_OFF:					return;
		case Mode::CM_CLOCKWISE:			cameraRotationStepWidth = +1; break;
		case Mode::CM_COUNTER_CLOCKWISE:	cameraRotationStepWidth = -1; break;
	}
	
	monitor->setViewMode(GLContextBase::ViewMode::V2D_CAM_ROT_XY_ZTOP);
	monitor->reshapeViewMode();
	
	cameraRotationTimer.Start(cameraRotationSpeed);
}
//////////////////////////////////////////////////
void CncMotionMonitor::onCameraRotationTimer(wxTimerEvent& event) {
//////////////////////////////////////////////////
	int pos = monitor->getCameraPosition().getCurXYPlaneEyeAngle() + cameraRotationStepWidth;
	
	// update the camera postion
	rotateCamera(pos);
	// update the dialog
	optionDlg->cameraCallBack(pos);
}
//////////////////////////////////////////////////
void CncMotionMonitor::rotateCamera(int angle) {
//////////////////////////////////////////////////
	// important to have the right view
	monitor->setViewMode(GLContextBase::ViewMode::V2D_CAM_ROT_XY_ZTOP);
	monitor->reshapeViewMode();

	// rotate
	monitor->getCameraPosition().rotateXYPlanTopZ_3D(angle, 
													 monitor->getCameraPosition().getCurXYPlaneEyeRadius(),
													 monitor->getCameraPosition().getEyeZ());
	// redraw
	display();
}
//////////////////////////////////////////////////
void CncMotionMonitor::pushProcessMode() {
//////////////////////////////////////////////////
	//set processing flags
	#warning todo normalize rotation 
	//monitor->normalizeRotation();
	monitor->setAutoScaling(getFlags().autoScaling);
	monitor->enablePositionMarker(getFlags().positionMarker);
	
	resetCurrentClientId();
}
//////////////////////////////////////////////////
void CncMotionMonitor::popProcessMode() {
//////////////////////////////////////////////////
	// reset processing flags - if neccessary
	monitor->enablePositionMarker(false);
}
//////////////////////////////////////////////////
void CncMotionMonitor::showOptionDialog() {
//////////////////////////////////////////////////
	wxASSERT( optionDlg != NULL );
	
	if ( optionDlg->IsIconized() )
		optionDlg->Maximize(false);
	
	optionDlg->Show(!optionDlg->IsShown());
}


