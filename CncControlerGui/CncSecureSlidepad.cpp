#include "CncSecureSlidepad.h"

/////////////////////////////////////////////////////////////////////
CncSecureSlidepad::CncSecureSlidepad(wxWindow* parent)
: CncSecureSlidepadBase		(parent)
, sliderValues				()
, caller					(NULL)
/////////////////////////////////////////////////////////////////////
{
	sliderValues.push_back(0);
	sliderValues.push_back(1);
	
	prepareScrollbar();
}
/////////////////////////////////////////////////////////////////////
CncSecureSlidepad::~CncSecureSlidepad() {
/////////////////////////////////////////////////////////////////////
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
		
	sliderValues = list;
	prepareScrollbar();
	
	const int index = findValue(startValue);
	if  ( sliderValues.size() == 2 ) 
	{
		m_scrollbar->SetThumbPosition(index);
	}
	else
	{
		if ( index >=0 && index < (int)sliderValues.size() )	m_scrollbar->SetThumbPosition(index);
		else													m_scrollbar->SetThumbPosition(0);
	}
	
	updateResult(); 
	return true;
}
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
/////////////////////////////////////////////////////////////////////
void CncSecureSlidepad::prepareScrollbar() {
/////////////////////////////////////////////////////////////////////
	m_scrollbar->SetThumbSize(1);
	m_scrollbar->SetPageSize(1);
	
	
	if ( sliderValues.size() < 2 ) 
	{
		m_scrollbar->SetRange(1);
		m_scrollbar->Enable(false);
	}
	else if ( sliderValues.size() == 2 ) 
	{
		m_scrollbar->SetRange((sliderValues[1] - sliderValues[0]) + 1);
		m_scrollbar->Enable(true);
	}
	else 
	{
		m_scrollbar->SetRange(sliderValues.size() );
		m_scrollbar->Enable(true);
	}
	
	updateResult(); 
}
/////////////////////////////////////////////////////////////////////
void CncSecureSlidepad::updateResult() {
/////////////////////////////////////////////////////////////////////
	if ( sliderValues.size() < 2 ) 
	{
		m_textResult->ChangeValue("0");
	}
	else if ( sliderValues.size() == 2 ) 
	{
		const int newVal = sliderValues[0] + m_scrollbar->GetThumbPosition();
		m_textResult->ChangeValue(wxString::Format("%d", newVal));
		
		if ( caller )
			caller->sliderValueChanged(m_scrollbar->GetThumbPosition(), newVal);
	}
	else 
	{
		const int val = m_scrollbar->GetThumbPosition();
		
		int newVal = 0;
		if ( val < (int)sliderValues.size() )
			newVal =  sliderValues[val];
			
		m_textResult->ChangeValue(wxString::Format("%d", newVal));
		
		if ( caller )
			caller->sliderValueChanged(val, newVal);
	}
}
/////////////////////////////////////////////////////////////////////
void CncSecureSlidepad::onScrollChanged(wxScrollEvent& event) {
/////////////////////////////////////////////////////////////////////
	updateResult();
}
