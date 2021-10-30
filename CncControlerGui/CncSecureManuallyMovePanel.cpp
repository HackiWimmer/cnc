#include "CncCommon.h"
#include "CncMoveDefinition.h"
#include "CncSecureNumpadDialog.h"
#include "CncSecureSlidepadDialog.h"
#include "MainFrame.h"
#include "CncSecureManuallyMovePanel.h"

/////////////////////////////////////////////////////////////////
CncSecureManuallyMovePanel::CncSecureManuallyMovePanel(wxWindow* parent)
: CncSecureManuallyMovePanelBase		(parent)
, axisButtons							()
, dimButtons							()
, currentValueX							(0.0)
, currentValueY							(0.0)
, currentValueZ							(0.0)
, currentValueF							(cnc::getSpeedValue(FINE))
/////////////////////////////////////////////////////////////////
{
	axisButtons.push_back(m_btX);
	axisButtons.push_back(m_btY);
	axisButtons.push_back(m_btZ);
	
	dimButtons.push_back(m_bt1D);
	dimButtons.push_back(m_bt2D);
	dimButtons.push_back(m_bt3D);
	
	for ( auto it=axisButtons.begin(); it != axisButtons.end(); ++it )
		setTooltip(*it);
		
	for ( auto it=dimButtons.begin(); it != dimButtons.end(); ++it )
		(*it)->SetValue(false);

	m_bt2D->SetValue(true);
	updateResult();
}
/////////////////////////////////////////////////////////////////
CncSecureManuallyMovePanel::~CncSecureManuallyMovePanel() {
/////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////
void CncSecureManuallyMovePanel::setTooltip(wxToggleButton* bt) {
/////////////////////////////////////////////////////////////////
	const char axis = bt->GetLabel().Length() > 0 ? bt->GetLabel()[0] : INVALID_AXIS;
	if ( axis == INVALID_AXIS )
		return;
		
	if ( islower(axis) )	bt->SetToolTip("Switch to absolute move mode");
	else					bt->SetToolTip("Switch to relative move mode");
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
	currentValueF = cnc::getSpeedValue(FINE);
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
		const char axis = bt->GetLabel().Length() > 0 ? bt->GetLabel()[0] : INVALID_AXIS;
		
		bool tooltip = true;
		switch ( axis ) {
			case 'x':		bt->SetLabel("X"); break;
			case 'X':		bt->SetLabel("x"); break;
			case 'y':		bt->SetLabel("Y"); break;
			case 'Y':		bt->SetLabel("y"); break;
			case 'z':		bt->SetLabel("Z"); break;
			case 'Z':		bt->SetLabel("z"); break;
			case 'f':
			case 'F':		bt->SetLabel("F"); 
			default:		tooltip = false;
		}
		
		if ( tooltip == true )
			setTooltip(bt);
	}
}
/////////////////////////////////////////////////////////////////
void CncSecureManuallyMovePanel::updateResult() {
/////////////////////////////////////////////////////////////////
	m_valueX->ChangeValue(wxString::Format(AXIS_RESULT_FORMAT,  currentValueX));
	m_valueY->ChangeValue(wxString::Format(AXIS_RESULT_FORMAT,  currentValueY));
	m_valueZ->ChangeValue(wxString::Format(AXIS_RESULT_FORMAT,  currentValueZ));
	m_valueF->ChangeValue(wxString::Format(SPEED_RESULT_FORMAT, currentValueF));
	
	const bool b = ( currentValueX != 0.0 || currentValueY != 0.0 || currentValueZ != 0.0 );
	m_btMove->Enable(b);
}
/////////////////////////////////////////////////////////////////
void CncSecureManuallyMovePanel::onLeftDownResultValue(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////
	event.Skip(false);
	
	enum Type { NUMPAD, SLIDEPAD };
	wxTextCtrl* rv = ((wxTextCtrl*)(event.GetEventObject()));
	
	// ----------------------------------------------------------
	auto updateValue = [&](Type type, double& currentValue, const wxString& info) {
		switch ( type )
		{
			case SLIDEPAD:
			{
				CncSecureSlidepadDialog dlg(this);
				CncSecureSlidepad::SliderValues values;
				
				values.push_back(cnc::getSpeedValue(FINEST));
				values.push_back(cnc::getSpeedValue(FINE));
				values.push_back(cnc::getSpeedValue(MEDIUM));
				values.push_back(cnc::getSpeedValue(ROUGH));
				values.push_back(cnc::getSpeedValue(ROUGHEST));

				#warning
				
				//values.push_back(-cnc::getSpeedValue(ROUGHEST));
				//values.push_back(+cnc::getSpeedValue(FINEST));
				//values.push_back(0);
				//values.push_back(+cnc::getSpeedValue(ROUGHEST));
				
				
				dlg.setValues(values, currentValue);
				dlg.setInfo(info);
				dlg.Center(wxCENTRE_ON_SCREEN);
				
				if ( dlg.ShowModal() == wxID_OK ) 
					currentValue = (double)(dlg.getValue());
					
				break;
			}
			case NUMPAD:
			{
				CncSecureNumpadDialog dlg(this->GetParent(), CncSecureNumpad::Type::DOUBLE);
				dlg.setValue(currentValue);
				dlg.setInfo(info);
				dlg.Center(wxCENTRE_ON_SCREEN);
				
				if ( dlg.ShowModal() == wxID_OK ) 
					currentValue = dlg.getValueAsDouble();
					
				break;
			}
		}
	};
	
	// ----------------------------------------------------------
	if      ( rv == m_valueX )	{ updateValue(NUMPAD,   currentValueX, m_btX->GetLabel()); }
	else if ( rv == m_valueY )	{ updateValue(NUMPAD,   currentValueY, m_btY->GetLabel()); }
	else if ( rv == m_valueZ )	{ updateValue(NUMPAD,   currentValueZ, m_btZ->GetLabel()); }
	else if ( rv == m_valueF )	{ updateValue(SLIDEPAD, currentValueF, m_btF->GetLabel()); }
	else						return;
	
	updateResult();
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
	
	cmd.x.absolute	= (char)(m_btX->GetLabel()[0]) == 'X';
	cmd.x.value		= currentValueX;
	cmd.y.absolute	= (char)(m_btY->GetLabel()[0]) == 'Y';
	cmd.y.value		= currentValueY;
	cmd.z.absolute	= (char)(m_btZ->GetLabel()[0]) == 'Z';
	cmd.z.value		= currentValueZ;
	
	cmd.speedMode	= CncSpeedMode::CncSpeedRapid;
	cmd.speedValue	= currentValueF;
	
	cmd.moveMode	= CncMoveDefinition::convert(getDimMode());
	//std::cout << cmd;
	
	if ( THE_APP->processManualMove(cmd) == false )
	{
		CNC_CERR_FUNCT_A(": processManualMove() failed ");
		std::cerr << cmd;
	}
}

