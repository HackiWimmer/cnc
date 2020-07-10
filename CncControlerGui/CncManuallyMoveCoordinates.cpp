#include "MainFrame.h"
#include "CncManuallyMoveCoordinates.h"

///////////////////////////////////////////////////////////////////
CncManuallyMoveCoordinates::CncManuallyMoveCoordinates(wxWindow* parent)
: CncManuallyMoveCoordinatesBase(parent)
, shouldClear(true)
///////////////////////////////////////////////////////////////////
{
	initialize();
}
///////////////////////////////////////////////////////////////////
CncManuallyMoveCoordinates::~CncManuallyMoveCoordinates() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::enableControls(bool state) {
///////////////////////////////////////////////////////////////////
	m_checkBoxToolEnabled->Enable(state);
	m_manuallyCorrectLimitPos->Enable(state);
	m_xManuallySlider->Enable(state);
	m_yManuallySlider->Enable(state);
	m_zManuallySlider->Enable(state);
	m_minManuallyXSlider->Enable(state);
	m_minManuallyYSlider->Enable(state);
	m_minManuallyZSlider->Enable(state);
	m_metricX->Enable(state);
	m_metricY->Enable(state);
	m_metricZ->Enable(state);
	m_maxManuallyXSlider->Enable(state);
	m_maxManuallyYSlider->Enable(state);
	m_maxManuallyZSlider->Enable(state);
	m_zeroManuallyXSlider->Enable(state);
	m_zeroManuallyYSlider->Enable(state);
	m_zeroManuallyZSlider->Enable(state);
	m_signManuallyXSlider->Enable(state);
	m_signManuallyYSlider->Enable(state);
	m_signManuallyZSlider->Enable(state);
	m_manuallyToolId->Enable(state);
	m_mmRadioCoordinates->Enable(state);
	m_metricCommon->Enable(state);
	m_btSetXYZ->Enable(state);
	m_btSetXY->Enable(state);
	m_btSetXZ->Enable(state);
	m_btSetYZ->Enable(state);
	
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::initialize() {
///////////////////////////////////////////////////////////////////
	enableControls(true);
	updateValidators();
	
	m_fxMax  ->ChangeValue(wxString::Format("%5.1lf", THE_CONFIG->getMaxSpeedX_MM_MIN()  ));
	m_fyMax  ->ChangeValue(wxString::Format("%5.1lf", THE_CONFIG->getMaxSpeedY_MM_MIN()  ));
	m_fzMax  ->ChangeValue(wxString::Format("%5.1lf", THE_CONFIG->getMaxSpeedZ_MM_MIN()  ));
	m_fxyMax ->ChangeValue(wxString::Format("%5.1lf", THE_CONFIG->getMaxSpeedXY_MM_MIN() ));
	m_fxyzMax->ChangeValue(wxString::Format("%5.1lf", THE_CONFIG->getMaxSpeedXYZ_MM_MIN()));
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::updateValidators() {
///////////////////////////////////////////////////////////////////
	const wxString unit(THE_APP->GetUnit()->GetValue());
	const int precision = ( unit == "mm" ? 3 : 0 );
	
	wxFloatingPointValidator<float> val(precision, NULL, wxNUM_VAL_DEFAULT );//, wxNUM_VAL_ZERO_AS_BLANK);
	
	m_metricX->SetValidator(val);
	m_metricY->SetValidator(val);
	m_metricZ->SetValidator(val);
	
	m_metricCommon->SetValidator(val);
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::updateUnit() {
///////////////////////////////////////////////////////////////////
	const wxString unit(THE_APP->GetUnit()->GetValue());
	
	double xLimit = +THE_CONFIG->getMaxDimensionX() * 0.8; // [mm]
	double yLimit = +THE_CONFIG->getMaxDimensionY() * 0.8; // [mm]
	double zLimit = +THE_CONFIG->getMaxDimensionZ() * 0.8; // [mm]
	
	auto setCommon = [&](double fact) {
		double value; m_metricCommon->GetValue().ToDouble(&value);
		wxString val;
		
		if ( unit == "steps" ) 	val = wxString::Format(wxT("%d"),       (int)(round(value)));
		else 					val = wxString::Format(wxT("%4.3f"), (double)(round(value)));
		
		m_metricCommon->ChangeValue(val);
	};
	
	if ( unit == "mm" ) { 
		setValueX(getValueX() * THE_CONFIG->getDisplayFactX());
		setValueY(getValueY() * THE_CONFIG->getDisplayFactY());
		setValueZ(getValueZ() * THE_CONFIG->getDisplayFactZ());
		
		setCommon(THE_CONFIG->getDisplayFactX());
		
	} else {
		setValueX(getValueX() * THE_CONFIG->getCalculationFactX());
		setValueY(getValueY() * THE_CONFIG->getCalculationFactY());
		setValueZ(getValueZ() * THE_CONFIG->getCalculationFactZ());
		
		xLimit *= THE_CONFIG->getCalculationFactX();
		yLimit *= THE_CONFIG->getCalculationFactY();
		zLimit *= THE_CONFIG->getCalculationFactZ();
		
		setCommon(THE_CONFIG->getCalculationFactZ());
	}
	
	updateValidators();
	
	m_xManuallySlider->SetMin(-xLimit);
	m_xManuallySlider->SetMax(+xLimit);
	m_yManuallySlider->SetMin(-yLimit);
	m_yManuallySlider->SetMax(+yLimit);
	m_zManuallySlider->SetMin(-zLimit);
	m_zManuallySlider->SetMax(+zLimit);
	
	m_xManuallySlider->SetValue(getValueX());
	m_yManuallySlider->SetValue(getValueY());
	m_zManuallySlider->SetValue(getValueZ());
	
	m_mmUnitX->SetLabel(unit);
	m_mmUnitY->SetLabel(unit);
	m_mmUnitZ->SetLabel(unit);
	m_mmUnitCommon->SetLabel(unit);
}
///////////////////////////////////////////////////////////////////
bool CncManuallyMoveCoordinates::shouldClearMontionMonitor() {
///////////////////////////////////////////////////////////////////
	const bool ret = shouldClear;
	shouldClear = false;
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::reset() {
///////////////////////////////////////////////////////////////////
	wxCommandEvent dummy;
	
	zeroManuallyXSlider(dummy);
	zeroManuallyYSlider(dummy);
	zeroManuallyZSlider(dummy);
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::resetClearViewState() {
///////////////////////////////////////////////////////////////////
	shouldClear = true;
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::setValueX(double value) {
///////////////////////////////////////////////////////////////////
	if ( value > m_xManuallySlider->GetMax() ) 
		value = m_xManuallySlider->GetMax();
	
	update(m_metricX, value);
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::setValueY(double value) {
///////////////////////////////////////////////////////////////////
	if ( value > m_yManuallySlider->GetMax() ) 
		value = m_yManuallySlider->GetMax();
	
	update(m_metricY, value);
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::setValueZ(double value) {
///////////////////////////////////////////////////////////////////
	if ( value > m_zManuallySlider->GetMax() ) 
		value = m_zManuallySlider->GetMax();
		
	update(m_metricZ, value);
}
///////////////////////////////////////////////////////////////////
double CncManuallyMoveCoordinates::getValueX() {
///////////////////////////////////////////////////////////////////
	double ret;
	m_metricX->GetValue().ToDouble(&ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
double CncManuallyMoveCoordinates::getValueY() {
///////////////////////////////////////////////////////////////////
	double ret;
	m_metricY->GetValue().ToDouble(&ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
double CncManuallyMoveCoordinates::getValueZ() {
///////////////////////////////////////////////////////////////////
	double ret;
	m_metricZ->GetValue().ToDouble(&ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::update(wxTextCtrl* ctrl, double value) {
///////////////////////////////////////////////////////////////////
	const wxString unit(THE_APP->GetUnit()->GetValue());
	wxString val;
	
	if ( unit == "steps" ) 	val = wxString::Format(wxT("%d"),       (int)(round(value)));
	else 					val = wxString::Format(wxT("%4.3f"), (double)(round(value)));
	
	ctrl->SetValue(val);
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::update(wxSlider* ctrl, double value) {
///////////////////////////////////////////////////////////////////
	const wxString unit(THE_APP->GetUnit()->GetValue());
	ctrl->SetValue((long)value);
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::update(wxSlider* ctrl, const wxString& value) {
///////////////////////////////////////////////////////////////////
	const wxString unit(THE_APP->GetUnit()->GetValue());

	if ( unit == "steps" ) {
		long v;
		value.ToLong(&v);
		ctrl->SetValue(v);
	} else {
		double v;
		value.ToDouble(&v);
		ctrl->SetValue(v);
	}
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::changeManuallySliderX(wxScrollEvent& event) {
///////////////////////////////////////////////////////////////////
	moveManuallySliderX(event);
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::changeManuallySliderY(wxScrollEvent& event) {
///////////////////////////////////////////////////////////////////
	moveManuallySliderY(event);
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::changeManuallySliderZ(wxScrollEvent& event) {
///////////////////////////////////////////////////////////////////
	moveManuallySliderZ(event);
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::minManuallyXSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_xManuallySlider->SetValue(m_xManuallySlider->GetMin());
	update(m_metricX, (double)(m_xManuallySlider->GetValue()));
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::maxManuallyXSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_xManuallySlider->SetValue(m_xManuallySlider->GetMax());
	update(m_metricX, (double)(m_xManuallySlider->GetValue()));
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::minManuallyYSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_yManuallySlider->SetValue(m_yManuallySlider->GetMin());
	update(m_metricY, (double)(m_yManuallySlider->GetValue()));
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::maxManuallyYSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_yManuallySlider->SetValue(m_yManuallySlider->GetMax());
	update(m_metricY, (double)(m_yManuallySlider->GetValue()));
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::minManuallyZSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_zManuallySlider->SetValue(m_zManuallySlider->GetMin());
	update(m_metricZ, (double)(m_zManuallySlider->GetValue()));
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::maxManuallyZSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_zManuallySlider->SetValue(m_zManuallySlider->GetMax());
	update(m_metricZ, (double)(m_zManuallySlider->GetValue()));
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::zeroManuallyXSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_xManuallySlider->SetValue(0);
	update(m_metricX, 0.0);
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::zeroManuallyYSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_yManuallySlider->SetValue(0);
	update(m_metricY, 0.0);
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::zeroManuallyZSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_zManuallySlider->SetValue(0);
	update(m_metricZ, 0.0);
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::signManuallyXSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString val = m_metricX->GetValue();
	double v;
	val.ToDouble(&v);
	if ( v != 0.0 )
		v *= -1;
	
	update(m_metricX, v);
	update(m_xManuallySlider, v);
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::signManuallyYSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString val = m_metricY->GetValue();
	double v;
	val.ToDouble(&v);
	if ( v != 0.0 )
		v *= -1;
	
	update(m_metricY, v);
	update(m_yManuallySlider, v);
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::signManuallyZSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString val = m_metricZ->GetValue();
	double v;
	val.ToDouble(&v);
	if ( v != 0.0 )
		v *= -1;
	
	update(m_metricZ, v);
	update(m_zManuallySlider, v);
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::updateMetricX(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	const wxString value = m_metricX->GetValue();
	update(m_xManuallySlider, value);
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::updateMetricY(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	const wxString value = m_metricY->GetValue();
	update(m_yManuallySlider, value);
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::updateMetricZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	const wxString value = m_metricZ->GetValue();
	update(m_zManuallySlider, value);
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::moveManuallySliderX(wxScrollEvent& event) {
///////////////////////////////////////////////////////////////////
	update(m_metricX, (double)(m_xManuallySlider->GetValue()));
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::moveManuallySliderY(wxScrollEvent& event) {
///////////////////////////////////////////////////////////////////
	update(m_metricY, (double)(m_yManuallySlider->GetValue()));
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::moveManuallySliderZ(wxScrollEvent& event) {
///////////////////////////////////////////////////////////////////
	update(m_metricZ, (double)(m_zManuallySlider->GetValue()));
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::onLBDownMax(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	wxTextCtrl* ctrl = (wxTextCtrl*)event.GetEventObject();
	
	if ( ctrl != NULL ) {
		double v; ctrl->GetValue().ToDouble(&v);
		THE_APP->updateSpeedSlider(v);
	}
	
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncManuallyMoveCoordinates::onSetCommonValue(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	const wxButton* btn = (wxButton*)event.GetEventObject();
	double value; m_metricCommon->GetValue().ToDouble(&value);
	
	if      ( btn == m_btSetXYZ )  { setValueX(value); setValueY(value); setValueZ(value); }
	else if ( btn == m_btSetXY )   { setValueX(value); setValueY(value); setValueZ(0.0);   } 
	else if ( btn == m_btSetXZ )   { setValueX(value); setValueY(0.0);   setValueZ(value); } 
	else if ( btn == m_btSetYZ )   { setValueX(0.0);   setValueY(value); setValueZ(value); } 
}
