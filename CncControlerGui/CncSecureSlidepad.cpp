#include "GlobalFunctions.h"
#include "CncSecureNumpadDialog.h"
#include "CncSecureSlidepad.h"

/////////////////////////////////////////////////////////////////////
CncSecureSlidepad::CncSecureSlidepad(wxWindow* parent)
: CncSecureSlidepadBase		(parent)
, sliderValues				()
, slider					(NULL)
, caller					(NULL)
, resolution				(ResTens)
/////////////////////////////////////////////////////////////////////
{
	sliderValues.push_back(0);
	sliderValues.push_back(1);
	
	const wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Segoe UI"));
	
	slider = new CncSecureGesturesPanel(this, wxHORIZONTAL, CncSecureGesturesPanel::Type::T_SWITCH, CncSecureGesturesPanel::Mode::M_POSITIVE, 10);
	GblFunc::replaceControl(m_gesturePanelPlaceholder, slider);
	slider->setCallbackId(callbackId);
	slider->SetBackgroundColour(wxColour(  0, 128,  0));
	slider->SetFont(font);
	slider->setCenterBitmap(ImageLibSecure().Bitmap("BMP_NAVI_LEFT_RIGHT32"));
	slider->init();
	
	Bind(wxEVT_CNC_SECURE_GESTURES_PANEL, &CncSecureSlidepad::onSliderEvent, this);
	
	setShowEndButtons(false);
}
/////////////////////////////////////////////////////////////////////
CncSecureSlidepad::~CncSecureSlidepad() {
/////////////////////////////////////////////////////////////////////
	Unbind(wxEVT_CNC_SECURE_GESTURES_PANEL, &CncSecureSlidepad::onSliderEvent, this);
	
	wxDELETE(slider);
}
///////////////////////////////////////////////////////////////////
void CncSecureSlidepad::setShowEndButtons(bool show) {
///////////////////////////////////////////////////////////////////
	m_btMin->Show(show);
	m_btMax->Show(show);
}
///////////////////////////////////////////////////////////////////
void CncSecureSlidepad::setInfo(const wxString& info) {
///////////////////////////////////////////////////////////////////
	if ( info.EndsWith(":") )	m_infoText->SetLabel(info);
	else						m_infoText->SetLabel(wxString::Format("%s:", info));
}
/////////////////////////////////////////////////////////////////////
int CncSecureSlidepad::getValue() const {
/////////////////////////////////////////////////////////////////////
	long ret;
	if ( m_textResult->GetValue().ToLong(&ret) )
		return (int)ret;
	
	return 0;
}
/////////////////////////////////////////////////////////////////////
int CncSecureSlidepad::findIndex(int value) const {
/////////////////////////////////////////////////////////////////////
	for( auto it = sliderValues.begin(); it != sliderValues.end(); ++it )
	{
		if ( *it == value )
			return std::distance(sliderValues.begin(), it);
	}
	
	return -1;
}
/////////////////////////////////////////////////////////////////////
bool CncSecureSlidepad::setValues(const SliderValues& list, int startValue) { 
/////////////////////////////////////////////////////////////////////
	if ( list.size() < 2 ) 
		return false;
		
	const int max = list.back();
	const int min = list.front();
	
	if ( max == min || max < min )
	{
		std::cerr << CNC_LOG_FUNCT << ": Invalid boundaries: min=" << min << " max=" << max  << std::endl;
		return false;
	}
	
	if ( startValue < min || startValue > max )
	{
		std::cerr << CNC_LOG_FUNCT << ": Invalid start value: min=" << min << " value=" << startValue << " max=" << max  << std::endl;
		return false;
	}
	
	sliderValues = list;
	
	if		( min <= 0 && max <= 0 )	slider->setMode(CncSecureGesturesPanel::Mode::M_NEGATIVE);
	else if ( min  < 0 && max >= 0 )	slider->setMode(CncSecureGesturesPanel::Mode::M_BOTH);
	else if ( min >= 0 && max  > 0 )	slider->setMode(CncSecureGesturesPanel::Mode::M_POSITIVE);
	
	slider->update();
	
	float ratio = 0.0;
	if ( sliderValues.size() == 2 ) 
	{
		CncRangeTranslator<int> trans(min, max, cncLeft);
		ratio = trans.ratioFromValue(startValue);
	}
	else
	{
		ratio = CncRangeTranslator<int>::ratioFromIndex(findIndex(startValue), sliderValues.size());
	}
	
	//cnc::cex1 << CNC_LOG_FUNCT_A(" ratio=%f ", ratio) << trans << std::endl;
	slider->setValueByRatio(ratio);
	
	return true;
}
/////////////////////////////////////////////////////////////////////
void CncSecureSlidepad::updateResult(float ratio) {
/////////////////////////////////////////////////////////////////////
	const int min = sliderValues.front();
	const int max = sliderValues.back();
	
	if ( sliderValues.size() < 2 ) 
	{
		m_textResult->ChangeValue("0");
	}
	else if ( sliderValues.size() == 2 ) 
	{
		CncRangeTranslator<int> trans(min, max, cncLeft);
		int val = trans.valueFromRatio(ratio);
		
		// perform resolution and display
		val = wxRound(double(val) / resolution) * resolution;
		m_textResult->ChangeValue(wxString::Format("%d", val));
	}
	else
	{
		// ----------------------------------------------------------
		auto displayDefault = [&](int index)
		{
			CNC_CERR_FUNCT_A(": Invalid index from ratio: %d", index)
			
			// perform resolution and display using min as default
			int val = wxRound(double(min) / resolution) * resolution;
			m_textResult->ChangeValue(wxString::Format("%d", val));
		};
		
		const size_t index = CncRangeTranslator<int>::indexFromRatio(ratio, sliderValues.size());
		//CNC_CLOG_FUNCT_A(": index=%d", index);
		
		if ( index < 0 )
		{
			displayDefault(index);
		}
		else
		{
			if ( (unsigned int)(index) < sliderValues.size() )
			{
				int val = wxRound(double(sliderValues.at(index)) / resolution) * resolution;
				m_textResult->ChangeValue(wxString::Format("%d", val));
			}
			else
			{
				displayDefault(index);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////
void CncSecureSlidepad::onSliderEvent(CncSecureGesturesPanelEvent& event) {
/////////////////////////////////////////////////////////////////////
	// may be nothing to to
	if ( event.data.isRatioChanged == false )
		return;
	
	switch( event.GetId() )
	{
		//case CncSecureGesturesPanelEvent::Id::CSGP_STARTING:
		//case CncSecureGesturesPanelEvent::Id::CSGP_POS_HELD:
		case CncSecureGesturesPanelEvent::Id::CSGP_POS_CHANGED:
		{
			if ( event.data.cbId == callbackId )
			{
				updateResult(event.data.ratio);
			}
			
			break;
		}
	}
}
/////////////////////////////////////////////////////////////////////
void CncSecureSlidepad::onLeftDownResult(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	const int max = sliderValues.back();
	const int min = sliderValues.front();
	
	CncSecureNumpadDialog dlg(this, CncSecureNumpad::Type::LONG, 0, min, max);
	dlg.setValue(m_textResult->GetValue());
	dlg.setInfo(m_infoText->GetLabel());
	dlg.Center(wxCENTRE_ON_SCREEN);
	
	if ( dlg.ShowModal() != wxID_OK )
		return; 
	
	const double newValue = dlg.getValueAsDouble();
	if ( newValue < double(min) || newValue > double(max) )
	{
		CNC_CERR_FUNCT_A(": Value out of range(%d,%d): %.1lf", min, max, newValue)
		return;
	}
	
	CncRangeTranslator<int> trans(min, max, cncLeft);
	const float ratio = trans.ratioFromValue(int(newValue));
	slider->setValueByRatio(ratio);
}
/////////////////////////////////////////////////////////////////////
void CncSecureSlidepad::onSkipToMax(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	slider->setValueByRatio(1.0);
}
/////////////////////////////////////////////////////////////////////
void CncSecureSlidepad::onSkipToMin(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	slider->setValueByRatio(0.0);
}
