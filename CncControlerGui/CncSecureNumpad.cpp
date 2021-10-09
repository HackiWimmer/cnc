#include <wx/event.h>
#include <wx/valnum.h>
#include "CncCommon.h"
#include "CncSecureNumpad.h"

///////////////////////////////////////////////////////////////////
class numeric;
CncSecureNumpad::CncSecureNumpad(wxWindow* parent, Type t, int p, double mi, double ma)
: CncSecureNumpadBase		(parent)
, type						(t)
, precision					(p >= 0 ? p : 3)
, length					(-1)
, min						(mi)
, max						(ma)
, prevContent				()
///////////////////////////////////////////////////////////////////
{
	length	= std::max(wxString::Format("%.0lf", min).Length(), wxString::Format("%.0lf", max).Length())
			+ precision
	;
	
	switch( type ) {
		case LONG:		m_btDot->Enable(false);
						setValue("");
						m_intervalText->SetLabel(wxString::Format("[ %ld ... %ld ]", (long)mi, (long)ma));
						break;
						
		case DOUBLE:	m_btDot->Enable(true);
						setValue("");
						length++;
						
						wxString format("%"); format.append(wxString::Format(".%dlf", precision));
						format.assign(wxString::Format("[ %s ... %s ]", format, format));
						
						m_intervalText->SetLabel(wxString::Format(format, mi, ma));
						break;
	}
	
	
}
///////////////////////////////////////////////////////////////////
CncSecureNumpad::~CncSecureNumpad() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
bool CncSecureNumpad::Show(bool show) {
///////////////////////////////////////////////////////////////////
	const bool ret = CncSecureNumpadBase::Show(show);
	return ret;
}
///////////////////////////////////////////////////////////////////
void CncSecureNumpad::setInfo(const wxString& info) {
///////////////////////////////////////////////////////////////////
	if ( info.EndsWith(":") )	m_infoText->SetLabel(info);
	else						m_infoText->SetLabel(wxString::Format("%s:", info));
}
///////////////////////////////////////////////////////////////////
void CncSecureNumpad::setValue(long v) {
///////////////////////////////////////////////////////////////////
	if ( v == 0 ) 	setValue("");
	else			setValue(wxString::Format("%ld", v));
}
///////////////////////////////////////////////////////////////////
void CncSecureNumpad::setValue(double v) {
///////////////////////////////////////////////////////////////////
	if ( cnc::dblCmp::nu(v) ) 
	{
		setValue(wxString::Format(".%s", wxString('0', precision)));
	}
	else
	{
		wxString format("%"); 
		format.append(wxString::Format(".%dlf", precision));
		setValue(wxString::Format(format, v));
	}
}
///////////////////////////////////////////////////////////////////
void CncSecureNumpad::setValue(const wxString& v)  {
///////////////////////////////////////////////////////////////////
	prevContent.assign(m_textResult->GetValue());
	
	m_signValue->SetValue(v.Contains("-") ? "-" : "+");
	
	wxString value(v);
	value.Replace("-", "");
	value.Replace("+", "");
	
	while ( value.StartsWith('0') )
		value.assign(value.Remove(0));
	
	m_textResult->ChangeValue(value);
	m_textResult->SetInsertionPoint(0);
	
	m_textResult->SetFocus();
	m_textResult->SelectAll();
}
///////////////////////////////////////////////////////////////////
long CncSecureNumpad::getValueAsLong() {
///////////////////////////////////////////////////////////////////
	long ret;
	m_textResult->GetValue().ToLong(&ret);
	
	if ( m_signValue->GetValue().Contains("-") )
		ret *= (-1);
		
	return ret;
}
///////////////////////////////////////////////////////////////////
double CncSecureNumpad::getValueAsDouble() {
///////////////////////////////////////////////////////////////////
	double ret;
	m_textResult->GetValue().ToDouble(&ret);
	
	if ( m_signValue->GetValue().Contains("-") )
		ret *= (-1);
		
	return ret;
}
///////////////////////////////////////////////////////////////////
void CncSecureNumpad::onContextMenuResult(wxContextMenuEvent& event) {
///////////////////////////////////////////////////////////////////
	// avoid context menu
	event.Skip(false);
}
///////////////////////////////////////////////////////////////////
void CncSecureNumpad::onLeft(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_textResult->HasFocus() == false )
		m_textResult->SetFocus();
		
	hitKey(WXK_LEFT);
}
///////////////////////////////////////////////////////////////////
void CncSecureNumpad::onRight(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_textResult->HasFocus() == false )
		m_textResult->SetFocus();
		
	hitKey(WXK_RIGHT);
}
///////////////////////////////////////////////////////////////////
void CncSecureNumpad::onBackspace(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_textResult->HasFocus() == false )
		m_textResult->SetFocus();
		
	int p = m_textResult->GetInsertionPoint();
	
	// if char to delete the '.' select all from '.' to end
	if ( p > 0 &&  m_textResult->GetRange(p - 1, p) == "." )
		m_textResult->SetSelection(p - 1, -1);
	
	hitKey(WXK_BACK);
}
///////////////////////////////////////////////////////////////////
void CncSecureNumpad::onClear(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_textResult->Clear();
}
/////////////////////////////////////////////////////////////////// 
void CncSecureNumpad::onSign(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_signValue->GetValue().Contains("-") )	m_signValue->ChangeValue("+");
	else											m_signValue->ChangeValue("-");
}
///////////////////////////////////////////////////////////////////
void CncSecureNumpad::onNumber(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	const wxButton* bt = ((wxButton*)(event.GetEventObject()));
	if ( bt ) {
		
		const wxString numStr = bt->GetLabel();
		long numVal;
		if ( numStr.ToLong(&numVal) ) 
		{
			if ( numVal == 0 )
			{
				const bool empty = m_textResult->GetValue().IsEmpty();
				
				if ( empty || m_textResult->GetInsertionPoint() > 0 ) 
				{
					m_textResult->SetFocus();
					
					hitChar('0');
					
					if ( empty )
						hitChar('.');
				}
			}
			else {
				m_textResult->SetFocus();
				hitChar('0' + (int)numVal);
			}
		}
		else if ( numStr.Contains('.') ) 
		{
			if ( m_textResult->GetValue().Contains('.') == false ) 
			{
				m_textResult->SetFocus();
				if ( m_textResult->GetValue().IsEmpty() )
					hitChar('0');
					
				if ( m_textResult->GetInsertionPoint() == 0 )
					m_textResult->SetInsertionPointEnd();
					
				hitChar('.');
			}
			else 
			{
				m_textResult->SetFocus();
				m_textResult->SetInsertionPoint(m_textResult->GetValue().First('.') + 1);
			}
		}
	}
}
///////////////////////////////////////////////////////////////////
void CncSecureNumpad::onCharResult(wxKeyEvent& event) {
///////////////////////////////////////////////////////////////////
	bool skip = false;
	
	if ( isdigit((char)event.GetKeyCode()) ) {
		skip = true;
	}
	else {
		switch ( event.GetKeyCode() ) {
			
			case WXK_BACK:
			case WXK_DELETE:
			case WXK_LEFT:
			case WXK_RIGHT:	skip = true; 
							break;
			case '-':
			case '+':		skip = false; 
							m_signValue->ChangeValue(wxString::Format("%c", event.GetKeyCode()));
							break;
			
			case '.':		skip = (type == DOUBLE && m_textResult->GetValue().Contains('.') == false ); 
							break;
		}
	}
	
	if ( skip == true )
		prevContent.assign(m_textResult->GetValue());
		
	event.Skip(skip);
}
///////////////////////////////////////////////////////////////////
void CncSecureNumpad::onTextUpdatedResult(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	double value;
	event.GetString().ToDouble(&value);
	
	if ( value < min || value > max )
	{
		if ( event.GetString().Contains('.') == false )
		{
			m_textResult->ChangeValue(prevContent);
			m_textResult->AppendText(".");
			m_textResult->SetInsertionPointEnd();
		}
		else 
		{
			wxBell();
			m_textResult->ChangeValue(prevContent);
		}
		
		return;
	}
	
	if ( event.GetString().Contains('.') ) {
		if ( event.GetString().AfterFirst('.').Length() > (size_t)precision ) {
			wxBell();
			m_textResult->ChangeValue(prevContent);
			m_textResult->SetInsertionPointEnd();
		}
	}
}


