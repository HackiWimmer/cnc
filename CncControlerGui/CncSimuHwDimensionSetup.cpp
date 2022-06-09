#include "MainFrame.h"
#include "CncCommon.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "CncBoundarySpace.h"
#include "CncFloatingPointValidator.h"
#include "CncSimuHwDimensionSetup.h"

#define BOTTOM			0
#define CENTERED		1
#define TOP				2

///////////////////////////////////////////////////////////////////
CncSimuHwDimensionSetup::CncSimuHwDimensionSetup(wxWindow* parent, Mode m)
: CncSimuHwDimensionSetupBase	(parent)
, previousSetup					()
, mode							(m)
///////////////////////////////////////////////////////////////////
{
	wxFloatingPointValidator<float> validator(3, NULL, wxNUM_VAL_DEFAULT);
	validator.SetRange(0.0, 5000.0);
	
	m_tcMaxDimX->SetValidator(validator);
	m_tcMaxDimY->SetValidator(validator);
	
	validator.SetRange(0.0, 500.0);
	m_tcMaxDimZ->SetValidator(validator);
	
	validator.SetRange(-5000.0, 5000.0);
	m_tcRefPosOffsetX->SetValidator(validator);
	m_tcRefPosOffsetY->SetValidator(validator);

	validator.SetRange(-500, 500.0);
	m_tcRefPosOffsetZ->SetValidator(validator);
}
///////////////////////////////////////////////////////////////////
CncSimuHwDimensionSetup::~CncSimuHwDimensionSetup() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
double CncSimuHwDimensionSetup::getValue(wxTextCtrl* ctrl, double dfltValue) const {
///////////////////////////////////////////////////////////////////
	double v; 
	if ( ctrl && ctrl->GetValue().ToCDouble(&v) ) 
		return v;
	
	return dfltValue;
};
///////////////////////////////////////////////////////////////////
void CncSimuHwDimensionSetup::onInitDialog(wxInitDialogEvent& event) {
///////////////////////////////////////////////////////////////////
	// setup gui
	m_tcMaxDimX->ChangeValue(wxString::Format(fltFormat, THE_BOUNDS->getMaxDimensionMetricX()));
	m_tcMaxDimY->ChangeValue(wxString::Format(fltFormat, THE_BOUNDS->getMaxDimensionMetricY()));
	m_tcMaxDimZ->ChangeValue(wxString::Format(fltFormat, THE_BOUNDS->getMaxDimensionMetricZ()));
	
	m_tcRefPosOffsetX->ChangeValue(wxString::Format(fltFormat, THE_BOUNDS->getHardwareOffset().getAsMetricX()));
	m_tcRefPosOffsetY->ChangeValue(wxString::Format(fltFormat, THE_BOUNDS->getHardwareOffset().getAsMetricY()));
	m_tcRefPosOffsetZ->ChangeValue(wxString::Format(fltFormat, THE_BOUNDS->getHardwareOffset().getAsMetricZ()));
	
	// store current to previous
	previousSetup.maxDimX	= THE_BOUNDS->getMaxDimensionMetricX();
	previousSetup.maxDimY	= THE_BOUNDS->getMaxDimensionMetricY();
	previousSetup.maxDimZ	= THE_BOUNDS->getMaxDimensionMetricZ();
	previousSetup.hwoX		= THE_BOUNDS->getHardwareOffset().getAsMetricX();
	previousSetup.hwoY		= THE_BOUNDS->getHardwareOffset().getAsMetricY();
	previousSetup.hwoZ		= THE_BOUNDS->getHardwareOffset().getAsMetricZ();
	
	switch ( mode )
	{
		case M_DIMENSION:	m_lbContext->SetSelection(0); break;
		case M_ORIGIN:		m_lbContext->SetSelection(1); break;
	}
}
///////////////////////////////////////////////////////////////////
bool CncSimuHwDimensionSetup::isSomethingChanged() const {
///////////////////////////////////////////////////////////////////
	const double hwoX = getValue(m_tcRefPosOffsetX, THE_BOUNDS->getHardwareOffset().getAsMetricX());
	const double hwoY = getValue(m_tcRefPosOffsetY, THE_BOUNDS->getHardwareOffset().getAsMetricY());
	const double hwoZ = getValue(m_tcRefPosOffsetZ, THE_BOUNDS->getHardwareOffset().getAsMetricZ());
	
	const double maxX = getValue(m_tcMaxDimX, THE_BOUNDS->getMaxDimensionMetricX());
	const double maxY = getValue(m_tcMaxDimY, THE_BOUNDS->getMaxDimensionMetricY());
	const double maxZ = getValue(m_tcMaxDimZ, THE_BOUNDS->getMaxDimensionMetricZ());

	if ( cnc::dblCmp::eq(previousSetup.maxDimX, maxX) == false )	return true;
	if ( cnc::dblCmp::eq(previousSetup.maxDimY, maxY) == false )	return true;
	if ( cnc::dblCmp::eq(previousSetup.maxDimZ, maxZ) == false )	return true;
	if ( cnc::dblCmp::eq(previousSetup.hwoX,    hwoX) == false )	return true;
	if ( cnc::dblCmp::eq(previousSetup.hwoY,    hwoY) == false )	return true;
	if ( cnc::dblCmp::eq(previousSetup.hwoZ,    hwoZ) == false )	return true;

	return false;
}
///////////////////////////////////////////////////////////////////
void CncSimuHwDimensionSetup::applyFinally() {
///////////////////////////////////////////////////////////////////
	if ( isSomethingChanged() == true )
	{
		// fake a physical hardware reference
		CNC_CEX2_A(	" No physical hardware support available for the connected port." \
					"\n A default hardware reference will be simply simulated for any test purpose.")
					
		apply();
		
		//THE_APP->getMotionMonitor()->normalizeMonitor();
	}
}
///////////////////////////////////////////////////////////////////
void CncSimuHwDimensionSetup::apply() {
///////////////////////////////////////////////////////////////////
	// first dimensions
	THE_BOUNDS->setMaxDimensionMetricX(getValue(m_tcMaxDimX, THE_BOUNDS->getMaxDimensionMetricX()));
	THE_BOUNDS->setMaxDimensionMetricY(getValue(m_tcMaxDimY, THE_BOUNDS->getMaxDimensionMetricY()));
	THE_BOUNDS->setMaxDimensionMetricZ(getValue(m_tcMaxDimZ, THE_BOUNDS->getMaxDimensionMetricZ()));
	
	// second hardware offset
	const double x = getValue(m_tcRefPosOffsetX, THE_BOUNDS->getHardwareOffset().getAsMetricX());
	const double y = getValue(m_tcRefPosOffsetY, THE_BOUNDS->getHardwareOffset().getAsMetricY());
	const double z = getValue(m_tcRefPosOffsetZ, THE_BOUNDS->getHardwareOffset().getAsMetricZ());
	
	CncLongPosition fakedHwRefPos
	(
		THE_CONFIG->convertMetricToStepsX(x),
		THE_CONFIG->convertMetricToStepsX(y),
		THE_CONFIG->convertMetricToStepsX(z)
	);
	
	THE_BOUNDS->setHardwareOffset(fakedHwRefPos);
	THE_BOUNDS->setHardwareOffsetValid(true); 

	THE_APP->getMotionMonitor()->clear();
	THE_APP->updateHardwareReference();
	
	THE_APP->getMotionMonitor()->makeHardwareSpaceVisible();
}
///////////////////////////////////////////////////////////////////
void CncSimuHwDimensionSetup::processDefault() {
///////////////////////////////////////////////////////////////////
	//setup default values
	m_tcMaxDimX->ChangeValue(wxString::Format(fltFormat, THE_BOUNDS->getMaxDimensionMetricX()));
	m_tcMaxDimY->ChangeValue(wxString::Format(fltFormat, THE_BOUNDS->getMaxDimensionMetricY()));
	m_tcMaxDimZ->ChangeValue(wxString::Format(fltFormat, THE_BOUNDS->getMaxDimensionMetricZ()));
	
	m_tcRefPosOffsetX->ChangeValue(wxString::Format(fltFormat, +0.0));
	m_tcRefPosOffsetY->ChangeValue(wxString::Format(fltFormat, +0.0));
	m_tcRefPosOffsetZ->ChangeValue(wxString::Format(fltFormat, +THE_BOUNDS->getMaxDimensionMetricZ()));
	
	apply();
}
///////////////////////////////////////////////////////////////////
void CncSimuHwDimensionSetup::cancel() {
///////////////////////////////////////////////////////////////////
	// restore previous values
	m_tcRefPosOffsetX->ChangeValue(wxString::Format(fltFormat, previousSetup.hwoX));
	m_tcRefPosOffsetY->ChangeValue(wxString::Format(fltFormat, previousSetup.hwoY));
	m_tcRefPosOffsetZ->ChangeValue(wxString::Format(fltFormat, previousSetup.hwoZ));

	m_tcMaxDimX->ChangeValue(wxString::Format(fltFormat, previousSetup.maxDimX));
	m_tcMaxDimY->ChangeValue(wxString::Format(fltFormat, previousSetup.maxDimY));
	m_tcMaxDimZ->ChangeValue(wxString::Format(fltFormat, previousSetup.maxDimZ));
	
	apply();
}
///////////////////////////////////////////////////////////////////
void CncSimuHwDimensionSetup::onCloseWindow(wxCloseEvent& event) {
///////////////////////////////////////////////////////////////////
	cancel();
	EndModal(wxID_CANCEL);
}
///////////////////////////////////////////////////////////////////
void CncSimuHwDimensionSetup::onCancel(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	cancel();
	EndModal(wxID_CANCEL);
}
///////////////////////////////////////////////////////////////////
void CncSimuHwDimensionSetup::onOk(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	applyFinally();
	EndModal(wxID_OK);
}
///////////////////////////////////////////////////////////////////
void CncSimuHwDimensionSetup::onTakeOverFromConfigMaxDimX(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_tcMaxDimX->ChangeValue(wxString::Format(fltFormat, THE_CONFIG->getMaxDimensionX()));
	apply();
}
///////////////////////////////////////////////////////////////////
void CncSimuHwDimensionSetup::onTakeOverFromConfigMaxDimY(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_tcMaxDimY->ChangeValue(wxString::Format(fltFormat, THE_CONFIG->getMaxDimensionY()));
	apply();
}
///////////////////////////////////////////////////////////////////
void CncSimuHwDimensionSetup::onTakeOverFromConfigMaxDimZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_tcMaxDimZ->ChangeValue(wxString::Format(fltFormat, THE_CONFIG->getMaxDimensionZ()));
	apply();
}
///////////////////////////////////////////////////////////////////
double CncSimuHwDimensionSetup::evaluateZLocation() {
///////////////////////////////////////////////////////////////////
	double z = +THE_BOUNDS->getMaxDimensionMetricZ();
	
	switch ( m_cbDefaultZ->GetSelection() )
	{
		case BOTTOM:	z = 0.0;											break;
		case CENTERED:	z = +THE_BOUNDS->getMaxDimensionMetricZ() / 2.0;	break;
		case TOP:
		default:		z = +THE_BOUNDS->getMaxDimensionMetricZ();
	}
	
	return z;
}
///////////////////////////////////////////////////////////////////
void CncSimuHwDimensionSetup::onRefCentered(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_tcRefPosOffsetX->ChangeValue(wxString::Format(fltFormat, -THE_BOUNDS->getMaxDimensionMetricX() / 2.0));
	m_tcRefPosOffsetY->ChangeValue(wxString::Format(fltFormat, -THE_BOUNDS->getMaxDimensionMetricY() / 2.0));
	m_tcRefPosOffsetZ->ChangeValue(wxString::Format(fltFormat, evaluateZLocation()));
	
	apply();
}
///////////////////////////////////////////////////////////////////
void CncSimuHwDimensionSetup::onRefSector1(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_tcRefPosOffsetX->ChangeValue(wxString::Format(fltFormat, +0.0));
	m_tcRefPosOffsetY->ChangeValue(wxString::Format(fltFormat, +0.0));
	m_tcRefPosOffsetZ->ChangeValue(wxString::Format(fltFormat, evaluateZLocation()));
	
	apply();
}
///////////////////////////////////////////////////////////////////
void CncSimuHwDimensionSetup::onRefSector2(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_tcRefPosOffsetX->ChangeValue(wxString::Format(fltFormat, -THE_BOUNDS->getMaxDimensionMetricX()));
	m_tcRefPosOffsetY->ChangeValue(wxString::Format(fltFormat, +0.0));
	m_tcRefPosOffsetZ->ChangeValue(wxString::Format(fltFormat, evaluateZLocation()));
	
	apply();
}
///////////////////////////////////////////////////////////////////
void CncSimuHwDimensionSetup::onRefSector3(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_tcRefPosOffsetX->ChangeValue(wxString::Format(fltFormat, 0.0));
	m_tcRefPosOffsetY->ChangeValue(wxString::Format(fltFormat, -THE_BOUNDS->getMaxDimensionMetricY()));
	m_tcRefPosOffsetZ->ChangeValue(wxString::Format(fltFormat, evaluateZLocation()));
	
	apply();
}
///////////////////////////////////////////////////////////////////
void CncSimuHwDimensionSetup::onRefSector4(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_tcRefPosOffsetX->ChangeValue(wxString::Format(fltFormat, -THE_BOUNDS->getMaxDimensionMetricX()));
	m_tcRefPosOffsetY->ChangeValue(wxString::Format(fltFormat, -THE_BOUNDS->getMaxDimensionMetricY()));
	m_tcRefPosOffsetZ->ChangeValue(wxString::Format(fltFormat, evaluateZLocation()));
	
	apply();
}
///////////////////////////////////////////////////////////////////
void CncSimuHwDimensionSetup::onSelectZLocation(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_tcRefPosOffsetZ->ChangeValue(wxString::Format(fltFormat, evaluateZLocation()));
	
	apply();
}
///////////////////////////////////////////////////////////////////
void CncSimuHwDimensionSetup::onUpdateTextValues(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	apply();
}
