#include "GL3DOptionPane.h"

/////////////////////////////////////////////////////////////////
GL3DOptionPane::GL3DOptionPane(wxWindow* parent)
// wxSize(1, 1) is a hack to overrule the sizer management
: GL3DOptionPaneBase(parent, wxID_ANY, wxDefaultPosition, wxSize(1, 1),  wxTAB_TRAVERSAL)
, GLContextOptions::Callback()
, CncMotionMonitor::Callback()
, motionMonitor(NULL)
/////////////////////////////////////////////////////////////////
{
}
/////////////////////////////////////////////////////////////////
GL3DOptionPane::~GL3DOptionPane() {
/////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////
void GL3DOptionPane::setMotionMonitor(CncMotionMonitor* m) { 
/////////////////////////////////////////////////////////////////
	// this function has to be called once only
	wxASSERT(motionMonitor == NULL);

	motionMonitor = m; 
	if ( motionMonitor == NULL )
		return;
		
	motionMonitor->getContextOptions().registerCallback(this);
	motionMonitor->registerCallback(this);
	
	// initialize grid
	notifyChange(motionMonitor->getContextOptions());
}
/////////////////////////////////////////////////////////////////
void GL3DOptionPane::propertyChanging(wxPropertyGridEvent& event) {
/////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
		
	// currently nothing more to do
}
/////////////////////////////////////////////////////////////////
void GL3DOptionPane::propertyChanged(wxPropertyGridEvent& event) {
/////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
	
	// update all options
	motionMonitor->getContextOptions().showOrigin 			= m_pgPropDrawOrigin->GetValue().GetBool();
	motionMonitor->getContextOptions().showFlyPath 			= m_pgPropDrawFlyPath->GetValue().GetBool();
	motionMonitor->getContextOptions().showPosMarker		= m_pgPropPositionMarker->GetValue().GetBool();
	motionMonitor->getContextOptions().autoScale			= m_pgPropAutoScaling->GetValue().GetBool();
	motionMonitor->getContextOptions().showRuler			= m_pgPropDrawRuler->GetValue().GetBool();
	motionMonitor->getContextOptions().showHelpLines		= m_pgPropDrawHelpLines->GetValue().GetBool();
	motionMonitor->getContextOptions().showBoundBox			= m_pgPropDrawBoundBox->GetValue().GetBool();
	motionMonitor->getContextOptions().helpLines3D_XYPlane	= m_pgPropHLXYPlane->GetValue().GetBool();
	motionMonitor->getContextOptions().helpLines3D_XZPlane	= m_pgPropHLXZPlane->GetValue().GetBool();
	motionMonitor->getContextOptions().helpLines3D_YZPlane	= m_pgPropHLYZPlane->GetValue().GetBool();
	
	// update all colors
	motionMonitor->getContextOptions().rapidColour 			= ((wxSystemColourProperty*)m_pgPropFlyColour)->GetVal().m_colour;
	motionMonitor->getContextOptions().workColour 			= ((wxSystemColourProperty*)m_pgPropWorkColour)->GetVal().m_colour;
	motionMonitor->getContextOptions().userColour 			= ((wxSystemColourProperty*)m_pgPropUserColour)->GetVal().m_colour;
	motionMonitor->getContextOptions().maxColour 			= ((wxSystemColourProperty*)m_pgPropMaxColour)->GetVal().m_colour;
	motionMonitor->getContextOptions().boundBoxColour		= ((wxSystemColourProperty*)m_pgPropBoundBoxColour)->GetVal().m_colour;
	
	// draw type
	switch ( m_pgPropDrawType->GetValue().GetInteger() ) {
		case 0:	motionMonitor->setDrawType(GLContextCncPathBase::DrawType::DT_POINTS); 		break;
		case 1:	motionMonitor->setDrawType(GLContextCncPathBase::DrawType::DT_LINES); 		break;
		case 2:	motionMonitor->setDrawType(GLContextCncPathBase::DrawType::DT_LINE_STRIP);	break;
	}
	
	// other oiptions
	motionMonitor->enableSmoothing(m_pgPropSmoothing->GetValue().GetBool());
	motionMonitor->setZoom((float)m_pgPropZoom->GetValue().GetDouble());
	
	motionMonitor->reconstruct();
}
/////////////////////////////////////////////////////////////////
void GL3DOptionPane::notifyChange(GLContextOptions& options) {
/////////////////////////////////////////////////////////////////
	// update all options
	m_pgPropDrawOrigin->SetValue(options.showOrigin);
	m_pgPropDrawFlyPath->SetValue(options.showFlyPath);
	m_pgPropPositionMarker->SetValue(options.showPosMarker);
	m_pgPropAutoScaling->SetValue(options.autoScale);
	m_pgPropDrawRuler->SetValue(options.showRuler);
	m_pgPropDrawHelpLines->SetValue(options.showHelpLines);
	m_pgPropDrawBoundBox->SetValue(options.showBoundBox);
	m_pgPropHLXYPlane->SetValue(options.helpLines3D_XYPlane);
	m_pgPropHLXZPlane->SetValue(options.helpLines3D_XZPlane);
	m_pgPropHLYZPlane->SetValue(options.helpLines3D_YZPlane);
	
	// update all colors
	m_pgPropFlyColour->SetValue(wxVariant(wxColourPropertyValue(options.rapidColour))); 
	m_pgPropWorkColour->SetValue(wxVariant(wxColourPropertyValue(options.workColour))); 
	m_pgPropBoundBoxColour->SetValue(wxVariant(wxColourPropertyValue(options.boundBoxColour)));
	m_pgPropUserColour->SetValue(wxVariant(wxColourPropertyValue(options.userColour)));
	m_pgPropMaxColour->SetValue(wxVariant(wxColourPropertyValue(options.maxColour)));
}
/////////////////////////////////////////////////////////////////
void GL3DOptionPane::notifyChange(CncMotionMonitor& mm) {
/////////////////////////////////////////////////////////////////
	// draw type
	switch ( mm.getDrawType() ) {
		case GLContextCncPathBase::DrawType::DT_POINTS:		m_pgPropDrawType->SetValue(0); break;
		case GLContextCncPathBase::DrawType::DT_LINES:		m_pgPropDrawType->SetValue(1); break;
		case GLContextCncPathBase::DrawType::DT_LINE_STRIP:	m_pgPropDrawType->SetValue(2); break;
	}
	
	// smoothing
	m_pgPropSmoothing->SetValue(mm.isSmoothingEnabled());
	
	// zoom
	m_pgPropZoom->SetValue(mm.getZoom());
	
	// update camera rotation
	m_sliderCameraRotation->SetValue(mm.getCameraEyeAngle());
	m_sliderCameraRotationSpeed->SetValue(mm.getCameraRotationSpeed());
	
	// update model rotation
	m_sliderModelRotationX->SetValue(mm.getAngleX());
	m_sliderModelRotationY->SetValue(mm.getAngleY());
	m_sliderModelRotationZ->SetValue(mm.getAngleZ());
}
/////////////////////////////////////////////////////////////////
void GL3DOptionPane::notifyCameraAngleChange(int angle) {
/////////////////////////////////////////////////////////////////
	m_sliderCameraRotation->SetValue(angle%360);
}
/////////////////////////////////////////////////////////////////
void GL3DOptionPane::modelRotationXChanged(wxScrollEvent& event) {
/////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;

	motionMonitor->setAngleX(m_sliderModelRotationX->GetValue());
	m_sliderModelRotationXYZ->SetValue(0);
}
/////////////////////////////////////////////////////////////////
void GL3DOptionPane::modelRotationYChanged(wxScrollEvent& event) {
/////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;

	motionMonitor->setAngleY(m_sliderModelRotationY->GetValue());
	m_sliderModelRotationXYZ->SetValue(0);
}
/////////////////////////////////////////////////////////////////
void GL3DOptionPane::modelRotationZChanged(wxScrollEvent& event) {
/////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;

	motionMonitor->setAngleZ(m_sliderModelRotationZ->GetValue());
	m_sliderModelRotationXYZ->SetValue(0);
}
/////////////////////////////////////////////////////////////////
void GL3DOptionPane::modelRotationXYZChanged(wxScrollEvent& event) {
/////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;

	int val = m_sliderModelRotationXYZ->GetValue();
	motionMonitor->setAngleX(val);
	motionMonitor->setAngleY(val);
	motionMonitor->setAngleZ(val);
	
	m_sliderModelRotationX->SetValue(val);
	m_sliderModelRotationY->SetValue(val);
	m_sliderModelRotationZ->SetValue(val);
}
/////////////////////////////////////////////////////////////////
void GL3DOptionPane::resetModelPostion(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;

	motionMonitor->normalizeMonitor();
}
/////////////////////////////////////////////////////////////////
void GL3DOptionPane::resetModelRotationX(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;

	m_sliderModelRotationX->SetValue(0);
	motionMonitor->setAngleX(m_sliderModelRotationX->GetValue());
}
/////////////////////////////////////////////////////////////////
void GL3DOptionPane::resetModelRotationY(wxCommandEvent& event)  {
/////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;

	m_sliderModelRotationY->SetValue(0);
	motionMonitor->setAngleY(m_sliderModelRotationY->GetValue());
}
/////////////////////////////////////////////////////////////////
void GL3DOptionPane::resetModelRotationZ(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;

	m_sliderModelRotationZ->SetValue(0);
	motionMonitor->setAngleZ(m_sliderModelRotationZ->GetValue());
}
/////////////////////////////////////////////////////////////////
void GL3DOptionPane::resetModelRotationXYZ(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;

	m_sliderModelRotationX->SetValue(0);
	m_sliderModelRotationY->SetValue(0);
	m_sliderModelRotationZ->SetValue(0);
	m_sliderModelRotationXYZ->SetValue(0);
	
	motionMonitor->setAngleX(m_sliderModelRotationX->GetValue());
	motionMonitor->setAngleY(m_sliderModelRotationY->GetValue());
	motionMonitor->setAngleZ(m_sliderModelRotationZ->GetValue());
}
/////////////////////////////////////////////////////////////////
void GL3DOptionPane::cameraRotationChanged(wxScrollEvent& event) {
/////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;

	motionMonitor->rotateCamera(m_sliderCameraRotation->GetValue());
}
/////////////////////////////////////////////////////////////////
void GL3DOptionPane::autoCameraRotationAnticlockwise(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;

	bool b = m_btCameraRotionAnticlockwise->GetValue();
	m_sliderCameraRotation->Enable(!b);
	m_btCameraRotionClockwise->SetValue(!b);

	if ( b ) 	motionMonitor->cameraRotationTimerHandler(CncMotionMonitor::CameraMode::CM_COUNTER_CLOCKWISE);
	else 		motionMonitor->cameraRotationTimerHandler(CncMotionMonitor::CameraMode::CM_OFF);
}
/////////////////////////////////////////////////////////////////
void GL3DOptionPane::autoCameraRotationClockwise(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;

	bool b = m_btCameraRotionClockwise->GetValue();
	m_sliderCameraRotation->Enable(!b);
	m_btCameraRotionAnticlockwise->SetValue(!b);
	
	if ( b )	motionMonitor->cameraRotationTimerHandler(CncMotionMonitor::CameraMode::CM_CLOCKWISE);
	else 		motionMonitor->cameraRotationTimerHandler(CncMotionMonitor::CameraMode::CM_OFF);
}
/////////////////////////////////////////////////////////////////
void GL3DOptionPane::resetCameraPostion(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;

	m_sliderCameraRotation->SetValue(0);
	m_btCameraRotionClockwise->SetValue(false);
	m_btCameraRotionAnticlockwise->SetValue(false);
	m_sliderCameraRotation->Enable(true);
	
	motionMonitor->cameraRotationTimerHandler(CncMotionMonitor::CameraMode::CM_OFF);
	motionMonitor->normalizeMonitor();
}
/////////////////////////////////////////////////////////////////
void GL3DOptionPane::cameraRotationSpeedChanged(wxScrollEvent& event) {
/////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;

	motionMonitor->setCameraRotationSpeed(m_sliderCameraRotationSpeed->GetValue());
}
