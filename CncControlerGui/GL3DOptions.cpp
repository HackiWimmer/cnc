#include <iostream>
#include <wx/string.h>
#include <wx/textentry.h>
#include <wx/valnum.h>
#include "GL3DOptions.h"

/////////////////////////////////////////////////////////////////
GL3DOptions::GL3DOptions(CncMotionMonitor* parent)
: GL3DOptionsBase(parent)
, motionMonitor(parent)
, firstTime(true)
/////////////////////////////////////////////////////////////////
{
	wxFloatingPointValidator<float> validator(3, NULL, wxNUM_VAL_DEFAULT);
	
	validator.SetPrecision(1);
	validator.SetRange(0.1, 8.0);
	m_pgPropZoom->SetValidator(validator);
}
/////////////////////////////////////////////////////////////////
GL3DOptions::~GL3DOptions() {
/////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::update() {
/////////////////////////////////////////////////////////////////
	restoreFlags();
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::onCloseWindow(wxCloseEvent& event) {
/////////////////////////////////////////////////////////////////
	Show(false);
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::onActivate(wxActivateEvent& event) {
/////////////////////////////////////////////////////////////////
	if ( event.GetActive() == false)
		return;
	
	restoreFlags();
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::onShowWindow(wxShowEvent& event) {
/////////////////////////////////////////////////////////////////
	if ( event.IsShown() == false ) {
		motionMonitor->cameraRotationTimerHandler(CncMotionMonitor::CameraMode::CM_OFF);
		return;
	}
		
	if ( firstTime ) {
		m_collPaneOptions->Collapse(true);
		m_collPaneCameraRotation->Collapse(true);
		m_collPaneModelRotation->Collapse(true);
		m_collPaneOptions->Expand();
		
		firstTime = false;
	}
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::restoreFlags() {
/////////////////////////////////////////////////////////////////
	// update option grid
	m_pgPropDrawFlyPath->SetValue(motionMonitor->getFlags().drawFlyPath);
	m_pgPropPositionMarker->SetValue(motionMonitor->getFlags().positionMarker);
	m_pgPropAutoScaling->SetValue(motionMonitor->getFlags().autoScaling);
	
	m_pgPropFlyColour->SetValue(wxVariant(wxColourPropertyValue(motionMonitor->getFlags().rapidColour))); 
	m_pgPropWorkColour->SetValue(wxVariant(wxColourPropertyValue(motionMonitor->getFlags().workColour))); 
	
	// draw type
	switch ( motionMonitor->getDrawType() ) {
		case GLContextCncPathBase::DrawType::DT_POINTS:		m_pgPropDrawType->SetValue(0); break;
		case GLContextCncPathBase::DrawType::DT_LINES:		m_pgPropDrawType->SetValue(1); break;
		case GLContextCncPathBase::DrawType::DT_LINE_STRIP:	m_pgPropDrawType->SetValue(2); break;
	}
	
	// bound box
	m_pgPropDrawBoundBox->SetValue(motionMonitor->isBoundBoxEnabled());
	m_pgPropBoundBoxColour->SetValue(wxVariant(wxColourPropertyValue(motionMonitor->getBoundBoxColour())));
	
	// origin
	m_pgPropDrawOrigin->SetValue(motionMonitor->isOriginEnabled());
	
	// ruler
	m_pgPropDrawRuler->SetValue(motionMonitor->isRulerEnabled());
	
	// helpLines
	m_pgPropDrawHelpLines->SetValue(motionMonitor->isHelpLinesEnabled());
	
	// smoothing
	m_pgPropSmoothing->SetValue(motionMonitor->isSmoothingEnabled());
	
	// zoom
	m_pgPropZoom->SetValue(motionMonitor->getZoom());
	
	// update camera rotation
	m_sliderCameraRotation->SetValue(motionMonitor->getCameraEyeAngle());
	m_sliderCameraRotationSpeed->SetValue(motionMonitor->getCameraRotationSpeed());
	
	// update model rotation
	m_sliderModelRotationX->SetValue(motionMonitor->getAngleX());
	m_sliderModelRotationY->SetValue(motionMonitor->getAngleY());
	m_sliderModelRotationZ->SetValue(motionMonitor->getAngleZ());
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::propertyChanging(wxPropertyGridEvent& event) {
/////////////////////////////////////////////////////////////////
	// currently nothing to do . . .
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::propertyChanged(wxPropertyGridEvent& event) { 
/////////////////////////////////////////////////////////////////
	// update all flags
	motionMonitor->getFlags().drawFlyPath 		= m_pgPropDrawFlyPath->GetValue().GetBool();
	motionMonitor->getFlags().positionMarker	= m_pgPropPositionMarker->GetValue().GetBool();
	motionMonitor->getFlags().autoScaling		= m_pgPropAutoScaling->GetValue().GetBool();
	
	motionMonitor->getFlags().rapidColour 		= ((wxSystemColourProperty*)m_pgPropFlyColour)->GetVal().m_colour;
	motionMonitor->getFlags().workColour 		= ((wxSystemColourProperty*)m_pgPropWorkColour)->GetVal().m_colour;
	
	// draw type
	switch ( m_pgPropDrawType->GetValue().GetInteger() ) {
		case 0:	motionMonitor->setDrawType(GLContextCncPathBase::DrawType::DT_POINTS); 		break;
		case 1:	motionMonitor->setDrawType(GLContextCncPathBase::DrawType::DT_LINES); 		break;
		case 2:	motionMonitor->setDrawType(GLContextCncPathBase::DrawType::DT_LINE_STRIP);	break;
	}
	
	// bound box
	motionMonitor->enableBoundBox(m_pgPropDrawBoundBox->GetValue().GetBool());
	motionMonitor->setBoundBoxColour(((wxSystemColourProperty*)m_pgPropBoundBoxColour)->GetVal().m_colour);
	
	// origin
	motionMonitor->enableOrigin(m_pgPropDrawOrigin->GetValue().GetBool());
	
	// ruler
	motionMonitor->enableRuler(m_pgPropDrawRuler->GetValue().GetBool());
	
	// helpLines
	motionMonitor->enableHelpLines(m_pgPropDrawHelpLines->GetValue().GetBool());
	
	// zoom
	motionMonitor->setZoom((float)m_pgPropZoom->GetValue().GetDouble());
	
	// smoothing
	motionMonitor->enableSmoothing(m_pgPropSmoothing->GetValue().GetBool());
	
	// make shure the new properties are used
	motionMonitor->reconstruct();
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::modelRotationXChanged(wxScrollEvent& event) {
/////////////////////////////////////////////////////////////////
	motionMonitor->setAngleX(m_sliderModelRotationX->GetValue());
	m_sliderModelRotationXYZ->SetValue(0);
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::modelRotationYChanged(wxScrollEvent& event) {
/////////////////////////////////////////////////////////////////
	motionMonitor->setAngleY(m_sliderModelRotationY->GetValue());
	m_sliderModelRotationXYZ->SetValue(0);
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::modelRotationZChanged(wxScrollEvent& event) {
/////////////////////////////////////////////////////////////////
	motionMonitor->setAngleZ(m_sliderModelRotationZ->GetValue());
	m_sliderModelRotationXYZ->SetValue(0);
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::modelRotationXYZChanged(wxScrollEvent& event) {
/////////////////////////////////////////////////////////////////
	int val = m_sliderModelRotationXYZ->GetValue();
	motionMonitor->setAngleX(val);
	motionMonitor->setAngleY(val);
	motionMonitor->setAngleZ(val);
	
	m_sliderModelRotationX->SetValue(val);
	m_sliderModelRotationY->SetValue(val);
	m_sliderModelRotationZ->SetValue(val);
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::resetModelRotationX(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	m_sliderModelRotationX->SetValue(0);
	motionMonitor->setAngleX(m_sliderModelRotationX->GetValue());
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::resetModelRotationY(wxCommandEvent& event)  {
/////////////////////////////////////////////////////////////////
	m_sliderModelRotationY->SetValue(0);
	motionMonitor->setAngleY(m_sliderModelRotationY->GetValue());
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::resetModelRotationZ(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	m_sliderModelRotationZ->SetValue(0);
	motionMonitor->setAngleZ(m_sliderModelRotationZ->GetValue());
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::resetModelRotationXYZ(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	m_sliderModelRotationX->SetValue(0);
	m_sliderModelRotationY->SetValue(0);
	m_sliderModelRotationZ->SetValue(0);
	m_sliderModelRotationXYZ->SetValue(0);
	
	motionMonitor->setAngleX(m_sliderModelRotationX->GetValue());
	motionMonitor->setAngleY(m_sliderModelRotationY->GetValue());
	motionMonitor->setAngleZ(m_sliderModelRotationZ->GetValue());
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::cameraRotationChanged(wxScrollEvent& event) {
/////////////////////////////////////////////////////////////////
	motionMonitor->rotateCamera(m_sliderCameraRotation->GetValue());
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::autoCameraRotationAnticlockwise(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	bool b = m_btCameraRotionAnticlockwise->GetValue();
	m_sliderCameraRotation->Enable(!b);
	m_btCameraRotionClockwise->SetValue(!b);

	if ( b ) 	motionMonitor->cameraRotationTimerHandler(CncMotionMonitor::CameraMode::CM_COUNTER_CLOCKWISE);
	else 		motionMonitor->cameraRotationTimerHandler(CncMotionMonitor::CameraMode::CM_OFF);
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::autoCameraRotationClockwise(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	bool b = m_btCameraRotionClockwise->GetValue();
	m_sliderCameraRotation->Enable(!b);
	m_btCameraRotionAnticlockwise->SetValue(!b);
	
	if ( b )	motionMonitor->cameraRotationTimerHandler(CncMotionMonitor::CameraMode::CM_CLOCKWISE);
	else 		motionMonitor->cameraRotationTimerHandler(CncMotionMonitor::CameraMode::CM_OFF);
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::resetCameraPostion(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	m_sliderCameraRotation->SetValue(0);
	m_btCameraRotionClockwise->SetValue(false);
	m_btCameraRotionAnticlockwise->SetValue(false);
	m_sliderCameraRotation->Enable(true);
	
	motionMonitor->cameraRotationTimerHandler(CncMotionMonitor::CameraMode::CM_OFF);
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::cameraCallBack(int angle) {
/////////////////////////////////////////////////////////////////
	m_sliderCameraRotation->SetValue(angle%360);
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::cameraRotationSpeedChanged(wxScrollEvent& event) {
/////////////////////////////////////////////////////////////////
	motionMonitor->setCameraRotationSpeed(m_sliderCameraRotationSpeed->GetValue());
}
/////////////////////////////////////////////////////////////////
void GL3DOptions::onUpdateTimer(wxTimerEvent& event) {
/////////////////////////////////////////////////////////////////
	//Refresh();
}
