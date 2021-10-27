#include "GlobalFunctions.h"
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
}
/////////////////////////////////////////////////////////////////////
CncSecureSlidepad::~CncSecureSlidepad() {
/////////////////////////////////////////////////////////////////////
	Unbind(wxEVT_CNC_SECURE_GESTURES_PANEL, &CncSecureSlidepad::onSliderEvent, this);
	
	wxDELETE(slider);
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
	
	if ( sliderValues.size() == 2 ) 
	{
		const float ratio = CncRangeTranslator<int>(min, max, cncLeft).ratioFromValue(startValue);
		slider->setValueByRatio(ratio);
	}
	else
	{
		
		
		
		
	}
	
	return true;
}
/*
/////////////////////////////////////////////////////////////////////
int CncSecureSlidepad::findValue(int value) const {
/////////////////////////////////////////////////////////////////////
	if ( sliderValues.size() < 2 ) 
		return -1;
		
	if  ( sliderValues.size() == 2 ) 
	{
		auto it = sliderValues.begin();
		for ( int i=0; i < m_scrollbar->GetRange(); i++)  {
			
			if ( (*it) + i == value )
				return i;
		}
	}
	else
	{
		for( auto it = sliderValues.begin(); it != sliderValues.end(); ++it ) {
			if ( *it == value )
				return std::distance(sliderValues.begin(), it);
		}
	}
	
	return -1;
}
*/





/////////////////////////////////////////////////////////////////////
void CncSecureSlidepad::updateResult(float ratio) {
/////////////////////////////////////////////////////////////////////
	if ( sliderValues.size() < 2 ) 
	{
		m_textResult->ChangeValue("0");
	}
	else if ( sliderValues.size() == 2 ) 
	{
		int val = CncRangeTranslator<int>(sliderValues.front(), sliderValues.back(), cncLeft).valueFromRatio(ratio);
		val = wxRound(double(val) / resolution) * resolution;
		m_textResult->ChangeValue(wxString::Format("%d", val));
	}
	else
	{
		
		
		
		
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
