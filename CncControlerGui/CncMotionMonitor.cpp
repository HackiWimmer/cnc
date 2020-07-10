#include <wx/dcclient.h>

#include "OSD/CncAsyncKeyboardState.h"
#include "3D/GLContextCncPath.h"
#include "3D/GLContextTestCube.h"
#include "CncMotionVertexTrace.h"
#include "GL3DDrawPane.h"
#include "CncContext.h"
#include "CncConfig.h"
#include "CncCommon.h"
#include "MainFrame.h"
#include "GlobalStrings.h"
#include "CncMotionMonitor.h"

#include "3D/GLInclude.h"

extern GlobalConstStringDatabase 	globalStrings;

// ----------------------------------------------------------------------------
// GlutInitManager
// ----------------------------------------------------------------------------
class GlutLibInitManager {
	public:
		//////////////////////////////////////////////////////
		GlutLibInitManager() {
			
			// init glut lib a the startup
			int   argc = 2;
			char* argv[2] = { globalStrings.programTitel.char_str(), wxString("-gldebug").char_str() };
			glutInit(&argc, argv);
			/*
			glutInitDisplayMode (GL_DOUBLE | GLUT_DEPTH | GLUT_RGB);
			 */
		}
		//////////////////////////////////////////////////////
		~GlutLibInitManager() {
		}
		
}; GlutLibInitManager _glim;

// ----------------------------------------------------------------------------
// CncMotionMonitor Event Table
// ----------------------------------------------------------------------------
wxDEFINE_EVENT(wxEVT_MONITOR_CAMERA_TIMER,  wxTimerEvent);

wxBEGIN_EVENT_TABLE(CncMotionMonitor, wxGLCanvas)
	EVT_PAINT(								CncMotionMonitor::onPaint)
	EVT_SIZE(								CncMotionMonitor::onSize)
	EVT_ERASE_BACKGROUND(					CncMotionMonitor::onEraseBackground)
	EVT_MOUSE_EVENTS(						CncMotionMonitor::onMouse)
	EVT_KEY_DOWN(							CncMotionMonitor::onKeyDown)
	EVT_LEAVE_WINDOW(						CncMotionMonitor::onLeave)
	EVT_TIMER(wxEVT_MONITOR_CAMERA_TIMER,	CncMotionMonitor::onCameraRotationTimer)
wxEND_EVENT_TABLE()

//////////////////////////////////////////////////
CncMotionMonitor::CncMotionMonitor(wxWindow *parent, int *attribList) 
: CncGlCanvas(parent, attribList)
, monitor(new GLContextCncPath(this, "GLMotionMonitor"))
, cameraRotationTimer(this, wxEVT_MONITOR_CAMERA_TIMER)
, cameraRotationStepWidth(0)
, cameraRotationSpeed(100)
, zoom(2.0f)
, currentClientID(-1L)
, processMode(false)
{
//////////////////////////////////////////////////
	GLContextBase::globalInit(); 
	monitor->init();
	
	// Important: initialize the CncGlCanvas context
	context = monitor;
	
	// publish initial zoom factor
	monitor->setZoomFactor(zoom);
	monitor->registerCallback(this);
	
	// deactivate process mode by default
	popProcessMode();
	
	createRuler(GLContextBase::ModelType::MT_RIGHT_HAND);
}
//////////////////////////////////////////////////
CncMotionMonitor::~CncMotionMonitor() {
//////////////////////////////////////////////////
	if ( monitor != NULL ) 
		delete monitor;
}
//////////////////////////////////////////////////
void CncMotionMonitor::notifyCncPathChanged() {
//////////////////////////////////////////////////
	// currently nothing to do
	// std::clog << "CncMotionMonitor::notifyCncPathChanged()" << std::endl;
}
//////////////////////////////////////////////////
unsigned int CncMotionMonitor::calculateScaleDisplay(unsigned int height) {
//////////////////////////////////////////////////
	float fact = ( monitor->getCurrentScaleFactor() * 1000 ) / ( monitor->getMaxScaleFactor() * 1000 );
	//std::cerr <<  monitor->getCurrentScaleFactor() << ", " <<  monitor->getMaxScaleFactor() << ", " << fact << std::endl;
	return (unsigned int)(fact * height);
}
//////////////////////////////////////////////////
void CncMotionMonitor::initVertexListCtr() {
//////////////////////////////////////////////////
	wxASSERT(THE_APP->getMotionVertexTrace());
	THE_APP->getMotionVertexTrace()->getVertexDataList()->setVertexBufferStore(monitor->getOpenGLBufferStore());
	THE_APP->getMotionVertexTrace()->getVertexIndexList()->setVertexBufferStore(monitor->getOpenGLBufferStore());
}
//////////////////////////////////////////////////
void CncMotionMonitor::enable(bool state) {
//////////////////////////////////////////////////
	monitor->enable(state);
}
//////////////////////////////////////////////////
void CncMotionMonitor::decorateProbeMode(bool state) {
//////////////////////////////////////////////////
	monitor->decorateProbeMode(state);
}
//////////////////////////////////////////////////
void CncMotionMonitor::clear() {
//////////////////////////////////////////////////
	monitor->clearPathData();
	onPaint();
}
//////////////////////////////////////////////////
void CncMotionMonitor::createRuler(const GLContextBase::ModelType mt) {
//////////////////////////////////////////////////
	// cleanup
	monitor->getXYPlane().helpLinesX.clear();
	monitor->getXYPlane().helpLinesY.clear();
	monitor->getXZPlane().helpLinesX.clear();
	monitor->getXZPlane().helpLinesZ.clear();
	monitor->getYZPlane().helpLinesY.clear();
	monitor->getYZPlane().helpLinesZ.clear();
	
	monitor->getRulerX().clear();
	monitor->getRulerY().clear();;
	monitor->getRulerZ().clear();

	// setup
	const double dimX = THE_CONFIG->getMaxDimensionX();
	const double dimY = THE_CONFIG->getMaxDimensionY();
	const double dimZ = THE_CONFIG->getMaxDimensionZ();
	
	CncMetricRulerSetup mrs;
	
	const double oo = 1.0 * getContextOptions().rulerOriginOffsetAbs;
	if ( mt == GLContextBase::ModelType::MT_LEFT_HAND )	mrs.setupOrigin({-oo, +oo, -oo});
	else												mrs.setupOrigin({-oo, -oo, -oo});

	mrs.setupSize(dimX, dimY, dimZ);
	
	mrs.createHelpLinesXY(monitor->getXYPlane().helpLinesX, monitor->getXYPlane().helpLinesY);
	mrs.createHelpLinesXZ(monitor->getXZPlane().helpLinesX, monitor->getXZPlane().helpLinesZ);
	mrs.createHelpLinesYZ(monitor->getYZPlane().helpLinesY, monitor->getYZPlane().helpLinesZ);
	
	mrs.createRulerX(monitor->getRulerX());
	mrs.createRulerY(monitor->getRulerY());
	mrs.createRulerZ(monitor->getRulerZ());
	
	mrs.check(monitor, std::cerr);
	//mrs.trace(monitor, std::cout);
}
//////////////////////////////////////////////////
void CncMotionMonitor::setModelType(const GLContextBase::ModelType mt) {
//////////////////////////////////////////////////
	createRuler(mt);
	monitor->setModelType(mt);
	monitor->reshapeViewMode();
	onPaint();
}
//////////////////////////////////////////////////
void CncMotionMonitor::reconstruct() {
//////////////////////////////////////////////////
	pushProcessMode();
		
		GLOpenGLPathBuffer::ReconstructOptions opt;
		opt.showRapidPathes = getContextOptions().showFlyPath;
		
		monitor->reconstruct(opt);
		onPaint();
		
	popProcessMode();
}
//////////////////////////////////////////////////
void CncMotionMonitor::appendVertex(const GLI::VerticeLongData& vd) {
//////////////////////////////////////////////////
	// Convert the given steps (abs) to a glpos (rel):
	
	// To do this first we need to normalize the values for x, y and z 
	// to a common unit like [mm], because [steps] are depends on 
	// the corresponding pitch which is configured for each axis separatly 
	const float x = vd.getX() / THE_CONFIG->getDispFactX3D(); 
	const float y = vd.getY() / THE_CONFIG->getDispFactY3D();
	const float z = vd.getZ() / THE_CONFIG->getDispFactZ3D();
	
	appendVertex(vd.getId(), vd.getSpeedMode(), x, y, z);
}
//////////////////////////////////////////////////
void CncMotionMonitor::appendVertex(long clientId, CncSpeedMode sm, const CncLongPosition& pos) {
//////////////////////////////////////////////////
	// Convert the given steps (abs) to a glpos (rel):
	
	// To do this first we need to normalize the values for x, y and z 
	// to a common unit like [mm], because [steps] are depends on 
	// the corresponding pitch which is configured for each axis separatly 
	const float x = pos.getX() / THE_CONFIG->getDispFactX3D(); 
	const float y = pos.getY() / THE_CONFIG->getDispFactY3D();
	const float z = pos.getZ() / THE_CONFIG->getDispFactZ3D();
	
	appendVertex(clientId, sm, x, y, z);
}
//////////////////////////////////////////////////
void CncMotionMonitor::appendVertex(long id, CncSpeedMode sm, float x, float y, float z) {
//////////////////////////////////////////////////
	static GLOpenGLPathBuffer::CncVertex vertex;

	if ( activateContext(monitor, true) == true ) {
		const char sc = cnc::getCncSpeedTypeAsCharacter(sm);
		monitor->appendPathData(vertex.set(sc, id, x, y, z)); 
	}
}
/////////////////////////////////////////////////////////////////
void CncMotionMonitor::centerViewport() {
/////////////////////////////////////////////////////////////////
	monitor->centerViewport();
	onPaint();
}
/////////////////////////////////////////////////////////////////
void CncMotionMonitor::resetRotation() {
/////////////////////////////////////////////////////////////////
	monitor->getModelRotation().reset2DDefaults();
	onPaint();
}
/////////////////////////////////////////////////////////////////
void CncMotionMonitor::onPaint() {
/////////////////////////////////////////////////////////////////
	// With respect to the GTK implementation SetCurrent() as well 
	// as SwapBuffers() isn't possible valid before
	if ( IsShownOnScreen() == false )
		return;

	const bool contextOk = activateContext(monitor);
	if ( contextOk == false )
		return;

	if ( monitor->init() == false )
		return;

	const wxSize cs = GetClientSize();
	
	if ( alreadyPainted )	monitor->reshape(cs.GetWidth(), cs.GetHeight());
	else 					monitor->reshapeViewMode(cs.GetWidth(), cs.GetHeight());
	
	monitor->display();
	
	// update additional controls
	notifyChange();
	
	// The first onPaint() if IsShownOnScreen() == true have to reshape the view mode
	// later this should not appear to support custom origin positions
	// see if above
	alreadyPainted = IsShown();
	SwapBuffers();
}
//////////////////////////////////////////////////
void CncMotionMonitor::update(bool force) { 
//////////////////////////////////////////////////
	if ( force == true || processMode == false)
		onPaint(); 
}
//////////////////////////////////////////////////
void CncMotionMonitor::onPaint(wxPaintEvent& event) {
//////////////////////////////////////////////////
	// This is required even though dc is not used otherwise.
	const wxPaintDC dc(this);
	onPaint();
}
//////////////////////////////////////////////////
void CncMotionMonitor::onSize(wxSizeEvent& event) {
//////////////////////////////////////////////////
	const wxSize cs = GetClientSize();
	
	monitor->reshapeViewMode(cs.GetWidth(), cs.GetHeight());
	event.Skip();
}
//////////////////////////////////////////////////
void CncMotionMonitor::onEraseBackground(wxEraseEvent& event) {
//////////////////////////////////////////////////
	// update background and avoid flashing on MSW 
}
//////////////////////////////////////////////////
void CncMotionMonitor::onLeave(wxMouseEvent& event) {
//////////////////////////////////////////////////
	mouseMoveMode = false;
}
//////////////////////////////////////////////////
void CncMotionMonitor::performMouseCoordAndToolTip() {
//////////////////////////////////////////////////
	typedef GLContextBase::ViewMode VT;
	
	wxString tt;
	switch ( context->getViewMode() ) {
		// xy plane
		case VT::V2D_TOP:
		case VT::V2D_BOTTOM:	tt.assign(wxString::Format("(X,Y) = %8.3lf, %8.3lf", 
															context->getMouseVertexAsMetricX(), 
															context->getMouseVertexAsMetricY()
														  )
								);
								THE_APP->GetMouseCoordX()->ChangeValue(wxString::Format("%.3lf", context->getMouseVertexAsMetricX()));
								THE_APP->GetMouseCoordY()->ChangeValue(wxString::Format("%.3lf", context->getMouseVertexAsMetricY()));
								THE_APP->GetMouseCoordZ()->ChangeValue(wxString::Format("%.3lf", 0.0));
								break;
		// yz plane
		case VT::V2D_LEFT:
		case VT::V2D_RIGHT:		tt.assign(wxString::Format("(Y,Z) = %8.3lf, %8.3lf", 
															context->getMouseVertexAsMetricY(), 
															context->getMouseVertexAsMetricZ()
														  )
								);
								THE_APP->GetMouseCoordX()->ChangeValue(wxString::Format("%.3lf", 0.0));
								THE_APP->GetMouseCoordY()->ChangeValue(wxString::Format("%.3lf", context->getMouseVertexAsMetricY()));
								THE_APP->GetMouseCoordZ()->ChangeValue(wxString::Format("%.3lf", context->getMouseVertexAsMetricZ()));
								break;
		// xz plane
		case VT::V2D_FRONT:
		case VT::V2D_REAR:		tt.assign(wxString::Format("(X,Z) = %8.3lf, %8.3lf", 
															context->getMouseVertexAsMetricX(), 
															context->getMouseVertexAsMetricZ()
														  )
								);
								THE_APP->GetMouseCoordX()->ChangeValue(wxString::Format("%.3lf", context->getMouseVertexAsMetricX()));
								THE_APP->GetMouseCoordY()->ChangeValue(wxString::Format("%.3lf", 0.0));
								THE_APP->GetMouseCoordZ()->ChangeValue(wxString::Format("%.3lf", context->getMouseVertexAsMetricZ()));
								break;
								
		default:				tt.clear();
	}
	
	if ( CncAsyncKeyboardState::isShiftPressed() == true ) 
		SetToolTip(tt);
}
//////////////////////////////////////////////////
void CncMotionMonitor::onMouse(wxMouseEvent& event) {
//////////////////////////////////////////////////
	static bool displayCoords = false;

	auto clearCoords = [&]() {
		if ( displayCoords == true ) {
			SetToolTip("");
			THE_APP->GetMouseCoordX()->ChangeValue("");
			THE_APP->GetMouseCoordY()->ChangeValue("");
			THE_APP->GetMouseCoordZ()->ChangeValue("");
		}
	};
	
	clearCoords();
	
	if ( event.ControlDown() == true ) {
		if ( context->logWinCoordsToVertex(event.GetX(), event.GetY()) )  {
			// the refresh() (not update()) is necessary here to get the coordinates well
			Refresh();
			performMouseCoordAndToolTip();
			displayCoords = true;
		}
	
	} else {
		context->delWinCoordsToVertex();
		update();
		clearCoords();
		displayCoords = false;
	}
	
	// default handling
	CncGlCanvas::onMouse(event);
}
//////////////////////////////////////////////////
void CncMotionMonitor::onKeyDown(int keyCode) {
//////////////////////////////////////////////////
	static const int delta = 10;
	const wxSize cs = GetClientSize();
	
	int ox = monitor->getLastReshapeX();
	int oy = monitor->getLastReshapeY();
	if ( ox == 0 )	ox = cs.GetWidth()/2;
	if ( oy == 0 )	oy = cs.GetHeight()/2;
	
	switch ( keyCode ) {
		
		case 'C':			monitor->centerViewport();
							onPaint();
							break;
					
		case WXK_UP:		oy += delta; monitor->reshape(cs.GetWidth(), cs.GetHeight(), ox, oy);
							onPaint();
							break;
							
		case WXK_DOWN:		oy -= delta; monitor->reshape(cs.GetWidth(), cs.GetHeight(), ox, oy);
							onPaint();
							break;
		
		case WXK_LEFT:		ox -= delta; 
							monitor->reshape(cs.GetWidth(), cs.GetHeight(), ox, oy);
							onPaint();
							break;
							
		case WXK_RIGHT:		ox += delta; 
							monitor->reshape(cs.GetWidth(), cs.GetHeight(), ox, oy);
							onPaint();
							break;
	}
}
//////////////////////////////////////////////////
void CncMotionMonitor::onKeyDown(wxKeyEvent& event) {
//////////////////////////////////////////////////
	onKeyDown(event.GetKeyCode());
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
	
	// update the option pane
	notifyCameraAngleChange(pos);
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
	onPaint();
}
//////////////////////////////////////////////////
void CncMotionMonitor::updateMonitorAndOptions() {
//////////////////////////////////////////////////
	Refresh();
	getContextOptions().notifyChange();
}
//////////////////////////////////////////////////
void CncMotionMonitor::normalizeMonitor() {
//////////////////////////////////////////////////
	monitor->normalizeRotation();
	monitor->normalizeCamera();
	
	Refresh();
}
//////////////////////////////////////////////////
void CncMotionMonitor::pushProcessMode() {
//////////////////////////////////////////////////
	//set processing flags
	normalizeMonitor();
	resetCurrentClientId();
	
	processMode = true;
	monitor->deactivateNotifications();
}
//////////////////////////////////////////////////
void CncMotionMonitor::popProcessMode() {
//////////////////////////////////////////////////
	processMode = false;
	monitor->activateNotifications();
}


