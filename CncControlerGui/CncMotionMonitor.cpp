#include <wx/dcclient.h>
#include <wx/dcclient.h>

#include "OSD/CncAsyncKeyboardState.h"
#include "3D/GLContextCncPath.h"
#include "3D/GLContextTestCube.h"
#include "CncMotionVertexTrace.h"
#include "GL3DDrawPane.h"
#include "CncContext.h"
#include "CncConfig.h"
#include "CncCommon.h"
#include "CncBoundarySpace.h"
#include "CncMessageDialog.h"
#include "MainFrame.h"
#include "GlobalStrings.h"
#include "CncMotionMonitor.h"

#include "3D/GLInclude.h"

extern GlobalConstStringDatabase 	globalStrings;

// ----------------------------------------------------------------------------
// GlutInitManager
// ----------------------------------------------------------------------------
class GlutLibInitManager
{
	public:
		//////////////////////////////////////////////////////
		GlutLibInitManager() 
		{
			// init glut lib a the startup
			int   argc = 2;
			char* argv[2] = { globalStrings.programTitel.char_str(), wxString("-gldebug").char_str() };
			glutInit(&argc, argv);
			/*
			glutInitDisplayMode (GL_DOUBLE | GLUT_DEPTH | GLUT_RGB);
			*/
		}
		//////////////////////////////////////////////////////
		~GlutLibInitManager() 
		{
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
, monitor					(new GLContextCncPath(this, "GLMotionMonitor"))
, cameraRotationTimer		(this, wxEVT_MONITOR_CAMERA_TIMER)
, cameraRotationStepWidth	(0)
, cameraRotationSpeed		(100)
, zoom						(2.0f)
, lastGestureZoomFactor		(1.0d)
, currentClientID			(CLIENT_ID.INVALID)
, processMode				(false)
{
//////////////////////////////////////////////////
	GLContextBase::globalInit(); 
	monitor->init();
	
	// Important: initialize the CncGlCanvas context
	context = monitor;
	
	// publish initial zoom factor
	monitor->setZoomFactor(zoom);
	monitor->registerCallback(this);
	
	setFrontCatchingMode(GLContextBase::FrontCatchingMode::FCM_KEEP_IN_FRAME);
	
	// deactivate process mode by default
	popProcessMode();
	
	createRuler(GLContextBase::ModelType::MT_RIGHT_HAND);
	
	if ( !EnableTouchEvents(wxTOUCH_ZOOM_GESTURE | wxTOUCH_ROTATE_GESTURE | wxTOUCH_PRESS_GESTURES) )
	//if ( !EnableTouchEvents(wxTOUCH_ALL_GESTURES) )
	{
		CNC_PRINT_FUNCT_A("Failed to enable touch events\n");
	}
	else
	{
		// Still bind event handlers just in case they still work
		//Bind(wxEVT_GESTURE_PAN,		&CncMotionMonitor::onPan,			this);
		Bind(wxEVT_GESTURE_ZOOM,	&CncMotionMonitor::onZoom,			this);
		Bind(wxEVT_GESTURE_ROTATE,	&CncMotionMonitor::onRotate,		this);
		Bind(wxEVT_TWO_FINGER_TAP,	&CncMotionMonitor::onTwoFingerTap,	this);
		Bind(wxEVT_LONG_PRESS,		&CncMotionMonitor::onLongPress,		this);
		Bind(wxEVT_PRESS_AND_TAP,	&CncMotionMonitor::onPressAndTap,	this);
	}
}
//////////////////////////////////////////////////
CncMotionMonitor::~CncMotionMonitor() {
//////////////////////////////////////////////////
	if ( monitor != NULL ) 
		delete monitor;
}
//////////////////////////////////////////////////
bool CncMotionMonitor::Show(bool show) {
//////////////////////////////////////////////////
	return CncGlCanvas::Show(show);
}
//////////////////////////////////////////////////
void CncMotionMonitor::synchronizeClientId() { 
//////////////////////////////////////////////////
	if ( currentClientID > 0 )
		monitor->setCurrentClientId(currentClientID); 
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
	monitor->clearGuidePathes();
	
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
	const double dimX = THE_BOUNDS->getMaxDimensionMetricX();
	const double dimY = THE_BOUNDS->getMaxDimensionMetricY();
	const double dimZ = THE_BOUNDS->getMaxDimensionMetricZ();
	
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
void CncMotionMonitor::setCurrentClientId(long id) {
////////////////////////////////////////////////// 
	currentClientID = id; 
	
	if ( IsShownOnScreen() == true )
		monitor->setCurrentClientId(id); 
}
//////////////////////////////////////////////////
void CncMotionMonitor::highlightClientId(long firstClientId, long lastClientId) {
//////////////////////////////////////////////////
	if ( IsShownOnScreen() == false )
		return;
		
	monitor->highlightClientId(firstClientId, lastClientId);
}
//////////////////////////////////////////////////
void CncMotionMonitor::dimAllVectiesDown() {
//////////////////////////////////////////////////
	GLOpenGLPathBuffer::dimDownColours();
	
	monitor->dimDownGudePathes();
	
	GLOpenGLPathBuffer::ReconstructOptions opt;
	monitor->reconstruct(opt);
}
//////////////////////////////////////////////////
void CncMotionMonitor::normalizeAllSelectionEffects() {
//////////////////////////////////////////////////
	GLOpenGLPathBuffer::dimUpColours();
	
	monitor->dimUpGudePathes();
	
	GLOpenGLPathBuffer::ReconstructOptions opt;
	monitor->reconstruct(opt);
}
//////////////////////////////////////////////////
void CncMotionMonitor::appendGuidPath(const CncPathListManager& plm) {
//////////////////////////////////////////////////
	GLGuidePath gp(plm);
	monitor->addGuidePath(gp);
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

	if ( activateContext(monitor, true) == true ) 
	{
		const char sc = cnc::getCncSpeedTypeAsCharacter(sm);
		monitor->appendPathData(vertex.set(sc, id, x, y, z)); 
	}
}
/////////////////////////////////////////////////////////////////
void CncMotionMonitor::centreViewport() {
/////////////////////////////////////////////////////////////////
	monitor->centreViewport();
	onPaint();
}
/////////////////////////////////////////////////////////////////
void CncMotionMonitor::resetRotation() {
/////////////////////////////////////////////////////////////////
	monitor->getModelRotation().reset();
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

	if ( alreadyPainted )	monitor->reshape();
	else 					monitor->reshapeViewMode();
	
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
	monitor->reshapeViewMode();
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
}
//////////////////////////////////////////////////
void CncMotionMonitor::performMouseCoordAndToolTip() {
//////////////////////////////////////////////////
	GLContextBase::MouseVertexInfo mvi = context->getCurrentMouseVertexInfo();
	
	const double maxDim = THE_BOUNDS->getMaxDimensionMetric();
	
	const double tx		= mvi.getAsMetricX(1.0);
	const double ty		= mvi.getAsMetricY(1.0);
	const double tz		= mvi.getAsMetricZ(1.0);
	
	THE_APP->GetMouseCoordX()->Clear();
	THE_APP->GetMouseCoordY()->Clear();
	THE_APP->GetMouseCoordZ()->Clear();
	
	if ( cnc::between(tx, -maxDim, +maxDim) )
		THE_APP->GetMouseCoordX()->ChangeValue(wxString::Format("%.3lf", tx));
		
	if ( cnc::between(ty, -maxDim, +maxDim) )
		THE_APP->GetMouseCoordY()->ChangeValue(wxString::Format("%.3lf", ty));
		
	if ( cnc::between(tz, -maxDim, +maxDim) )
		THE_APP->GetMouseCoordZ()->ChangeValue(wxString::Format("%.3lf", tz));

	if ( CncAsyncKeyboardState::isShiftPressed() == true ) {
		
		const wxString sx( cnc::between(tx, -maxDim, +maxDim) ? wxString::Format("%8.3lf", tx) : "----.---" );
		const wxString sy( cnc::between(ty, -maxDim, +maxDim) ? wxString::Format("%8.3lf", ty) : "----.---" );
		const wxString sz( cnc::between(tz, -maxDim, +maxDim) ? wxString::Format("%8.3lf", tz) : "----.---" );
		const wxString id( context->getNormalizedClientIdOfPos(mvi.vecX, mvi.vecY, mvi.vecZ) );
		
		SetToolTip(wxString::Format("(X,Y,Z): %s, %s, %s (Client ID: ): %s", sx, sy, sz, id));
	}
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
	
	switch ( keyCode ) {
		
		case 'C':			monitor->centreViewport();
							onPaint();
							break;
					
		case 'R':			monitor->resetViewport();
							onPaint();
							break;
							
		case 'W':			if ( makeWorkingSpaceVisible() == true )	onPaint();
							else										cnc::trc.logInfo("No working space available");
							break;
							
		case 'H':			if ( makeHardwareSpaceVisible() == true )	onPaint();
							else										cnc::trc.logInfo("No hardware space available");
							break;

		case '0':			setFrontCatchingMode(GLContextBase::FrontCatchingMode::FCM_OFF);
							onPaint();
							break;
							
		case '1':			setFrontCatchingMode(GLContextBase::FrontCatchingMode::FCM_KEEP_IN_FRAME);
							onPaint();
							break;
							
		case '2':			setFrontCatchingMode(GLContextBase::FrontCatchingMode::FCM_ALWAYS_CENTRED);
							onPaint();
							break;
							
		case WXK_UP:		monitor->reshapeRelative(0, +delta);
							onPaint();
							break;
							
		case WXK_DOWN:		monitor->reshapeRelative(0, -delta);
							onPaint();
							break;
		
		case WXK_LEFT:		monitor->reshapeRelative(-delta, 0);
							onPaint();
							break;
							
		case WXK_RIGHT:		monitor->reshapeRelative(+delta, 0);
							onPaint();
							break;
	}
}
//////////////////////////////////////////////////
void CncMotionMonitor::setFrontCatchingMode(GLContextBase::FrontCatchingMode fcm) {
//////////////////////////////////////////////////
	static wxColour active  (171, 171, 171);
	static wxColour inactive(240, 240, 240);

	THE_APP->GetFrontKeepModeInFrame()	->SetBackgroundColour(inactive);
	THE_APP->GetFrontKeepModeCentered()	->SetBackgroundColour(inactive);
	THE_APP->GetFrontKeepModeOff()		->SetBackgroundColour(inactive);
	
	switch ( fcm )
	{
		case GLContextBase::FrontCatchingMode::FCM_KEEP_IN_FRAME:
					THE_APP->GetFrontKeepModeInFrame()->SetBackgroundColour(active);
					break;
					
		case GLContextBase::FrontCatchingMode::FCM_ALWAYS_CENTRED:
					THE_APP->GetFrontKeepModeCentered()->SetBackgroundColour(active);
					break;
					
		case GLContextBase::FrontCatchingMode::FCM_OFF:
					THE_APP->GetFrontKeepModeOff()->SetBackgroundColour(active);
					break;
	}

	monitor->setFrontCatchingMode(fcm);
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
	
	makeWorkingSpaceVisible();
	
	Refresh();
}
//////////////////////////////////////////////////
void CncMotionMonitor::pushReplayMode() {
//////////////////////////////////////////////////
	monitor->pushReplayMode();
}
//////////////////////////////////////////////////
void CncMotionMonitor::popReplayMode() {
//////////////////////////////////////////////////
	monitor->popReplayMode();
}
//////////////////////////////////////////////////
void CncMotionMonitor::pushProcessMode() {
//////////////////////////////////////////////////
	//set processing flags
	//normalizeMonitor();
	resetCurrentClientId();
	
	processMode = true;
	monitor->pushProcessMode();
}
//////////////////////////////////////////////////
void CncMotionMonitor::popProcessMode() {
//////////////////////////////////////////////////
	processMode = false;
	monitor->popProcessMode();
}
//////////////////////////////////////////////////
void CncMotionMonitor::pushInteractiveProcessMode() {
//////////////////////////////////////////////////
	processMode = true;
	monitor->pushInteractiveProcessMode();
}
//////////////////////////////////////////////////
void CncMotionMonitor::popInteractiveProcessMode() {
//////////////////////////////////////////////////
	processMode = false;
	monitor->popInteractiveProcessMode();
}
//////////////////////////////////////////////////
void CncMotionMonitor::onPan(wxPanGestureEvent& event) {
//////////////////////////////////////////////////
	// allows the scene movement
	event.Skip();
}
//////////////////////////////////////////////////
void CncMotionMonitor::onZoom(wxZoomGestureEvent& event) {
//////////////////////////////////////////////////
	if ( event.IsGestureStart() )
		lastGestureZoomFactor = 1.0;
		
	const double delta = lastGestureZoomFactor - event.GetZoomFactor();
	
	if ( cnc::dblCmp::nu(delta) == false )
	{
		//CNC_CLOG_FUNCT_A("%lf %lf %lf ", delta, lastGestureZoomFactor, event.GetZoomFactor())
		
		if ( cnc::dblCmp::lt(delta, 0.0) )	decScale(0.01f);
		else 								incScale(0.01f);
		
		Refresh();
	}
	
	lastGestureZoomFactor = event.GetZoomFactor();
}
//////////////////////////////////////////////////
void CncMotionMonitor::onRotate(wxRotateGestureEvent& event) {
//////////////////////////////////////////////////
	// currently only implemented to disable all gesture events
}
//////////////////////////////////////////////////
void CncMotionMonitor::onTwoFingerTap(wxTwoFingerTapEvent& event) {
//////////////////////////////////////////////////
	// currently only implemented to disable all gesture events
}
//////////////////////////////////////////////////
void CncMotionMonitor::onLongPress(wxLongPressEvent& event) {
//////////////////////////////////////////////////
	// currently only implemented to disable all gesture events
}
//////////////////////////////////////////////////
void CncMotionMonitor::onPressAndTap(wxPressAndTapEvent& event) {
//////////////////////////////////////////////////
	// currently only implemented to disable all gesture events
}
//////////////////////////////////////////////////
bool CncMotionMonitor::makeCompleteVisible(const CncDoubleBoundaries& box) { 
//////////////////////////////////////////////////
	if ( box.hasBoundaries() == false )
		return false;
		
	const bool ret = monitor->makeCompleteVisible(box); 
	onPaint();
	
	return ret;
}
//////////////////////////////////////////////////
bool CncMotionMonitor::makeCompleteVisibleMetric(const CncDoubleBoundaries& box) { 
//////////////////////////////////////////////////
	if ( box.hasBoundaries() == false )
		return false;
	
	CncDoubleBoundaries b(box);
	b.multiply(
		THE_CONFIG->getCalculationFactX(), 
		THE_CONFIG->getCalculationFactY(), 
		THE_CONFIG->getCalculationFactZ()
	);
	b.divide(
		THE_CONFIG->getDispFactX3D(), 
		THE_CONFIG->getDispFactY3D(), 
		THE_CONFIG->getDispFactZ3D()
	);
	
	return makeCompleteVisible(b);
}
//////////////////////////////////////////////////
bool CncMotionMonitor::makeWorkingSpaceVisible() {
//////////////////////////////////////////////////
	CncDoubleBoundaries b;
	if ( monitor->getBounderies(b) )	return makeCompleteVisible(b);
	else								return makeHardwareSpaceVisible();
		
	return false;
}
//////////////////////////////////////////////////
bool CncMotionMonitor::makeHardwareSpaceVisible() {
//////////////////////////////////////////////////
	if ( THE_BOUNDS->hasHardwareOffset() == false )
		return false;
		
	CncDoubleBoundaries b(THE_BOUNDS->getPhysicallyBoundaries());
	b.multiply(
		THE_CONFIG->getCalculationFactX(), 
		THE_CONFIG->getCalculationFactY(), 
		THE_CONFIG->getCalculationFactZ()
	);
	b.divide(
		THE_CONFIG->getDispFactX3D(), 
		THE_CONFIG->getDispFactY3D(), 
		THE_CONFIG->getDispFactZ3D()
	);
	
	return makeCompleteVisible(b);
}
//////////////////////////////////////////////////
std::ostream& CncMotionMonitor::traceInformation(std::ostream& o) const {
//////////////////////////////////////////////////
	auto traceBoundsMetric = [&]()
	{
		CncDoubleBoundaries b;
		if ( monitor->getBounderies(b) )
		{
			b.multiply(
				THE_CONFIG->getDispFactX3D(), 
				THE_CONFIG->getDispFactY3D(), 
				THE_CONFIG->getDispFactZ3D()
			);
			b.divide(
				THE_CONFIG->getCalculationFactX(), 
				THE_CONFIG->getCalculationFactY(), 
				THE_CONFIG->getCalculationFactZ()
			);
			
			return wxString::Format("(%.3lf, %.3lf)(%.3lf, %.3lf)(%.3lf, %.3lf)"
									, b.getMinBound().getX(), b.getMaxBound().getX()
									, b.getMinBound().getY(), b.getMaxBound().getY()
									, b.getMinBound().getZ(), b.getMaxBound().getZ()
								   );
		}
		
		return wxString::Format("n/a");
	};

	o	<< "Gui:\n"
		<< " Window size  (width/height)[px] : "  << GetClientSize().GetWidth() << " / " << GetClientSize().GetHeight() << std::endl
		<< " Cnc Path Boundaries        [mm] : " << traceBoundsMetric()		<< std::endl
		<< std::endl
	;
	
	o	<< "Context:\n";
	monitor->traceInformation(o);

	return o;
}
//////////////////////////////////////////////////
void CncMotionMonitor::traceInformation() {
//////////////////////////////////////////////////
	std::stringstream ss;
	traceInformation(ss);
	
	wxSize s = GetClientSize();
	s.DecBy(20);

	CncMessageDialog md(this, ss.str().c_str(), "Collected Information", "Cnc Motion Monitor");
	md.SetClientSize(s);
	md.CenterOnParent();
	md.ShowModal();
}
//////////////////////////////////////////////////
bool CncMotionMonitor::saveContentAsBitmap(const wxFileName& fileName) {
//////////////////////////////////////////////////
	const unsigned int w = GetClientSize().GetWidth();
	const unsigned int h = GetClientSize().GetHeight();

	wxClientDC cltDC(this);
	wxMemoryDC memDC;
	wxBitmap bitmap(w, h);
	
	memDC.SelectObject(bitmap);
	memDC.Blit( 0, 
				0, 
				w, 
				h,
				&cltDC, 
				0, 
				0 
			);
	
	memDC.SelectObject(wxNullBitmap);
	return bitmap.SaveFile(fileName.GetFullPath(), wxBITMAP_TYPE_BMP);
}