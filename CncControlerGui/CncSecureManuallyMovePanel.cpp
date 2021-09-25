#include "CncCommon.h"
#include "CncMoveDefinition.h"
#include "CncSecureNumpadDialog.h"
#include "CncSecureSlidepadDialog.h"
#include "CncSecureManuallyMovePanel.h"

/////////////////////////////////////////////////////////////////
CncSecureManuallyMovePanel::CncSecureManuallyMovePanel(wxWindow* parent)
: CncSecureManuallyMovePanelBase		(parent)
, axisButtons							()
, dimButtons							()
, currentAxis							(INVALID_AXIS)
, lastNumber							(INVALID_NUMBER)
, currentValueX							(0.0)
, currentValueY							(0.0)
, currentValueZ							(0.0)
, currentValueF							(cnc::getSpeedValue(FINE))
/////////////////////////////////////////////////////////////////
{
	axisButtons.push_back(m_btX);
	axisButtons.push_back(m_btY);
	axisButtons.push_back(m_btZ);
	axisButtons.push_back(m_btx);
	axisButtons.push_back(m_bty);
	axisButtons.push_back(m_btz);
	
	dimButtons.push_back(m_bt1D);
	dimButtons.push_back(m_bt2D);
	dimButtons.push_back(m_bt3D);
	
	for ( auto it=axisButtons.begin(); it != axisButtons.end(); ++it )
		(*it)->SetValue(false);
	
	for ( auto it=dimButtons.begin(); it != dimButtons.end(); ++it )
		(*it)->SetValue(false);

	m_bt2D->SetValue(true);
	setCurrentAxisMode('X');
	updateResult();
}
/////////////////////////////////////////////////////////////////
CncSecureManuallyMovePanel::~CncSecureManuallyMovePanel() {
/////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////
void CncSecureManuallyMovePanel::onClearX(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////
	currentValueX = 0.0;
	updateResult();
}
/////////////////////////////////////////////////////////////////
void CncSecureManuallyMovePanel::onClearY(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////
	currentValueY = 0.0;
	updateResult();
}
/////////////////////////////////////////////////////////////////
void CncSecureManuallyMovePanel::onClearZ(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////
	currentValueZ = 0.0;
	updateResult();
}
/////////////////////////////////////////////////////////////////
void CncSecureManuallyMovePanel::onClearF(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////
	updateResult();
}
/////////////////////////////////////////////////////////////////
void CncSecureManuallyMovePanel::onSetDimMode(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	wxToggleButton* bt = ((wxToggleButton*)(event.GetEventObject()));
	
	for ( auto it=dimButtons.begin(); it != dimButtons.end(); ++it )
		(*it)->SetValue(false);
		
	if ( bt )	
		bt->SetValue(true);
}
/////////////////////////////////////////////////////////////////
void CncSecureManuallyMovePanel::onAxis(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	wxToggleButton* bt = ((wxToggleButton*)(event.GetEventObject()));
	
	if ( bt ) {
		setCurrentAxisMode(bt->GetLabel());
		updateResult();
	}
}
/////////////////////////////////////////////////////////////////
bool CncSecureManuallyMovePanel::setCurrentAxisMode(const char axis) {
/////////////////////////////////////////////////////////////////
	wxToggleButton* bt = NULL;
	
	switch ( axis ) {
		case 'x':		currentAxis = axis; bt = m_btx; break;
		case 'X':		currentAxis = axis; bt = m_btX; break;
		case 'y':		currentAxis = axis; bt = m_bty; break;
		case 'Y':		currentAxis = axis; bt = m_btY; break;
		case 'z':		currentAxis = axis; bt = m_btz; break;
		case 'Z':		currentAxis = axis; bt = m_btZ; break;
		default:		currentAxis = INVALID_AXIS;
	}
	
	// reset selection
	for ( auto it=axisButtons.begin(); it != axisButtons.end(); ++it ) {
		(*it)->SetValue(false);
	}
	
	if ( bt == NULL )
		return false;
	
	bt->SetValue(true);
	return true;
}
/////////////////////////////////////////////////////////////////
bool CncSecureManuallyMovePanel::setCurrentAxisMode(const wxString& axis) {
/////////////////////////////////////////////////////////////////
	if ( axis.Length() > 0 )
		return setCurrentAxisMode((const char)axis[0]);
	
	return false;
}
/////////////////////////////////////////////////////////////////
void CncSecureManuallyMovePanel::updateResult() {
/////////////////////////////////////////////////////////////////
	switch ( currentAxis ) {
		case 'x':
		case 'X':		m_axisX->ChangeValue(wxString::Format("%c", currentAxis));
						break;
		case 'y':
		case 'Y':		m_axisY->ChangeValue(wxString::Format("%c", currentAxis));
						break;
		case 'z':
		case 'Z':		m_axisZ->ChangeValue(wxString::Format("%c", currentAxis));
						break;
	}
	
	m_valueX->ChangeValue(wxString::Format(AXIS_RESULT_FORMAT,  currentValueX));
	m_valueY->ChangeValue(wxString::Format(AXIS_RESULT_FORMAT,  currentValueY));
	m_valueZ->ChangeValue(wxString::Format(AXIS_RESULT_FORMAT,  currentValueZ));
	m_valueF->ChangeValue(wxString::Format(SPEED_RESULT_FORMAT, currentValueF));
	
	const bool b = ( currentValueX != 0.0 || currentValueY != 0.0 || currentValueZ != 0.0 );
	m_btMove->Enable(b);
}
/////////////////////////////////////////////////////////////////
double CncSecureManuallyMovePanel::getCurrentAxisValue() {
/////////////////////////////////////////////////////////////////
	double value = 0.0;
	switch ( currentAxis ) {
		case 'x':
		case 'X':		value = currentValueX;
						break;
		case 'y':
		case 'Y':		value = currentValueY;
						break;
		case 'z':
		case 'Z':		value = currentValueZ;
						break;
						
		default:		std::cerr	<< CNC_LOG_FUNCT_A(wxString::Format(" : Invalid currentAxis '%c'!", currentAxis)) 
									<< std::endl;
	}
	
	return value;
}
/////////////////////////////////////////////////////////////////
bool CncSecureManuallyMovePanel::setCurrentAxisValue(double v) {
/////////////////////////////////////////////////////////////////
	switch ( currentAxis ) {
		case 'x':
		case 'X':		currentValueX = v;
						return true;
		case 'y':
		case 'Y':		currentValueY = v;
						return true;
		case 'z':
		case 'Z':		currentValueZ = v;
						return true;
						
		default:		std::cerr	<< CNC_LOG_FUNCT_A(wxString::Format(" : Invalid currentAxis '%c'!", currentAxis)) 
									<< std::endl;
	}
	
	return false;
}
/////////////////////////////////////////////////////////////////
const wxString& CncSecureManuallyMovePanel::prepareStringValue(wxString& strVal) {
/////////////////////////////////////////////////////////////////
	double value			= getCurrentAxisValue();
	
	if ( value == 0.0 )  {
		if ( lastNumber == DOT )	strVal.assign(wxString::Format("0%c", DOT));
		else						strVal.assign("");
	}
	else {
		strVal.assign(wxString::Format(AXIS_RESULT_FORMAT, value));
		
		// remove tailing charters on demand
		if ( strVal.Length() > 0 && strVal.Contains(DOT) ) {
			while ( strVal.Length() > 0 && ( strVal.Last() == '0' || strVal.Last() == DOT ) ) {
				strVal.RemoveLast();
			}
		}
		
		if ( lastNumber == DOT ) {
			strVal.append(DOT);
		}
		else {
			if ( lastNumber != BKS && strVal.Length() == wxString("+123").Length() ) {
				strVal.append(DOT);
			}
		}
	}
	
	return strVal;
}
/////////////////////////////////////////////////////////////////
void CncSecureManuallyMovePanel::onLeftDownResultValue(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////
	event.Skip(false);
	
	wxString info;
	wxTextCtrl* rv = ((wxTextCtrl*)(event.GetEventObject()));
	if      ( rv == m_valueX )	{ setCurrentAxisMode(m_axisX->GetValue());	info.assign(m_axisX->GetValue()); }
	else if ( rv == m_valueY )	{ setCurrentAxisMode(m_axisY->GetValue());	info.assign(m_axisY->GetValue()); }
	else if ( rv == m_valueZ )	{ setCurrentAxisMode(m_axisZ->GetValue());	info.assign(m_axisZ->GetValue()); }
	else if ( rv == m_valueF )	{ setCurrentAxisMode(INVALID_AXIS); 		info.assign(m_axisF->GetValue()); }
	else						return;
	
	if ( currentAxis == INVALID_AXIS )
	{
		CncSecureSlidepadDialog dlg(this);
		CncSecureSlidepad::SliderValues values;
		/*
		values.push_back(cnc::getSpeedValue(FINEST));
		values.push_back(cnc::getSpeedValue(FINE));
		values.push_back(cnc::getSpeedValue(MEDIUM));
		values.push_back(cnc::getSpeedValue(ROUGH));
		values.push_back(cnc::getSpeedValue(ROUGHEST));
		*/
		values.push_back(cnc::getSpeedValue(FINEST));
		values.push_back(cnc::getSpeedValue(ROUGHEST));
		
		double val; m_valueF->GetValue().ToDouble(&val);
		dlg.setValues(values, val);
		dlg.setInfo(info);
		dlg.Center(wxCENTRE_ON_SCREEN);
		
		if ( dlg.ShowModal() == wxID_OK ) 
			m_valueF->ChangeValue(wxString::Format(SPEED_RESULT_FORMAT, (double)(dlg.getValue())));
	}
	else 
	{
		CncSecureNumpadDialog dlg(this->GetParent(), CncSecureNumpad::Type::DOUBLE);
		dlg.setValue(getCurrentAxisValue());
		dlg.setInfo(info);
		dlg.Center(wxCENTRE_ON_SCREEN);
		
		if ( dlg.ShowModal() == wxID_OK ) 
		{
			setCurrentAxisValue(dlg.getValueAsDouble());
			updateResult();
		}
	}
}
/////////////////////////////////////////////////////////////////
void CncSecureManuallyMovePanel::onMove(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	
	// ----------------------------------------------------------
	auto getDimMode = [&]() {
		if ( m_bt1D->GetValue() == true ) return "1D";
		if ( m_bt2D->GetValue() == true ) return "2D";
		if ( m_bt3D->GetValue() == true ) return "3D";
		
		return "1D";
	};
	
	CncMoveDefinition cmd;
	
	cmd.x.absolute	= (char)(m_axisX->GetValue()[0]) == 'X';
	cmd.x.value		= currentValueX;
	cmd.y.absolute	= (char)(m_axisY->GetValue()[0]) == 'Y';
	cmd.y.value		= currentValueY;
	cmd.z.absolute	= (char)(m_axisZ->GetValue()[0]) == 'Z';
	cmd.z.value		= currentValueZ;
	//cmd.f			= currentEnumF;
	cmd.speedMode	= CncSpeedMode::CncSpeedRapid;
	cmd.moveMode	= CncMoveDefinition::convert(getDimMode());
	
	std::cout << cmd;
}

