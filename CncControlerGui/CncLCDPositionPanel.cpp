#include "GlobalFunctions.h"
#include "wxCrafterLCDPanel.h"
#include "MainFrame.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "CncBoundarySpace.h"
#include "CncLCDPositionPanel.h"

///////////////////////////////////////////////////////////////////
CncLCDPositionPanel::CncLCDPositionPanel(wxWindow* parent)
: CncLCDPositionPanelBase(parent)
, posType 	(PT_LOGICAL)
, lcdF		(NULL)
, lcdX		(NULL)
, lcdY		(NULL)
, lcdZ		(NULL)
///////////////////////////////////////////////////////////////////
{
	lcdF = new wxLCDWindow(this, wxDefaultPosition, wxDefaultSize);
	GblFunc::replaceControl(m_lcdPlaceholderF, lcdF);
	
	lcdS = new wxLCDWindow(this, wxDefaultPosition, wxDefaultSize);
	GblFunc::replaceControl(m_lcdPlaceholderS, lcdS);

	lcdX = new wxLCDWindow(this, wxDefaultPosition, wxDefaultSize);
	GblFunc::replaceControl(m_lcdPlaceholderX, lcdX);
	
	lcdY = new wxLCDWindow(this, wxDefaultPosition, wxDefaultSize);
	GblFunc::replaceControl(m_lcdPlaceholderY, lcdY);
	
	lcdZ = new wxLCDWindow(this, wxDefaultPosition, wxDefaultSize);
	GblFunc::replaceControl(m_lcdPlaceholderZ, lcdZ);
	
	lcdF->SetNumberDigits(8);
	lcdS->SetNumberDigits(8);
	lcdX->SetNumberDigits(8);
	lcdY->SetNumberDigits(8);
	lcdZ->SetNumberDigits(8);
	
	lcdF->SetValue(   "0.0" ); 
	lcdS->SetValue(   "0.0" ); 
	lcdX->SetValue( "0.000" ); 
	lcdY->SetValue( "0.000" ); 
	lcdZ->SetValue( "0.000" ); 
	
	evaluatePositionType();
}
///////////////////////////////////////////////////////////////////
CncLCDPositionPanel::~CncLCDPositionPanel() {
///////////////////////////////////////////////////////////////////
	delete lcdF;
	delete lcdS;
	delete lcdX;
	delete lcdY;
	delete lcdZ;
}
///////////////////////////////////////////////////////////////////
void CncLCDPositionPanel::onStartupTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	//SetClientSize(wxSize(100, 500));
	Refresh();
}
///////////////////////////////////////////////////////////////////
void CncLCDPositionPanel::onPaint(wxPaintEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip();
	
	lcdF->Refresh();
	lcdS->Refresh();
	lcdX->Refresh();
	lcdY->Refresh();
	lcdZ->Refresh();
}
///////////////////////////////////////////////////////////////////
void CncLCDPositionPanel::onSize(wxSizeEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip();
	
	lcdF->Refresh();
	lcdS->Refresh();
	lcdX->Refresh();
	lcdY->Refresh();
	lcdZ->Refresh();
}
///////////////////////////////////////////////////////////////////
void CncLCDPositionPanel::updateUnit() {
///////////////////////////////////////////////////////////////////
	const wxString unit(THE_APP->GetUnit()->GetValue());
	
	if ( m_unitX->GetLabel() != unit)
		m_unitX->SetLabel(unit);
		
	if ( m_unitY->GetLabel() != unit)
		m_unitY->SetLabel(unit);
		
	if ( m_unitZ->GetLabel() != unit)
		m_unitZ->SetLabel(unit);
}
///////////////////////////////////////////////////////////////////
void CncLCDPositionPanel::updateValues() {
///////////////////////////////////////////////////////////////////
	lcdF->SetValue(THE_APP->GetConfiguredFeedSpeed()->GetValue());
	lcdS->SetValue(wxString::Format("%.1lf", THE_APP->getConfiguredSpindleSpeed()));
	
	if ( posType == PT_LOGICAL ) {
		lcdX->SetValue(THE_APP->GetXAxisCtl()->GetValue());
		lcdY->SetValue(THE_APP->GetYAxisCtl()->GetValue());
		lcdZ->SetValue(THE_APP->GetZAxisCtl()->GetValue());
	}
	else {
		double x; THE_APP->GetXAxisCtl()->GetValue().ToDouble(&x);
		double y; THE_APP->GetYAxisCtl()->GetValue().ToDouble(&y);
		double z; THE_APP->GetZAxisCtl()->GetValue().ToDouble(&z);
		
		CncDoublePosition logPos(x, y, z);
		CncDoublePosition phyPos = THE_BOUNDS->getHardwareOffset().transLog2Phy(logPos);
		
		lcdX->SetValue(wxString::Format("%.3lf", phyPos.getX()));
		lcdY->SetValue(wxString::Format("%.3lf", phyPos.getY()));
		lcdZ->SetValue(wxString::Format("%.3lf", phyPos.getZ()));
	}
}
///////////////////////////////////////////////////////////////////
void CncLCDPositionPanel::onChangePositionType(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	evaluatePositionType();
}
///////////////////////////////////////////////////////////////////
void CncLCDPositionPanel::evaluatePositionType() {
///////////////////////////////////////////////////////////////////
	const wxString& t = m_cbPosTYpe->GetValue();
	
	if ( t.StartsWith("Phy") )	posType = PT_PHYSICAL;
	else						posType = PT_LOGICAL;
	
	if ( THE_CONTEXT->hasHardware() == true ) {
		if ( THE_BOUNDS->getHardwareOffset().isValid() == false )
			cnc::trc.logWarning("The hardware offset is not available. Please perform a reference evaluation, otherwise there's no difference between the physical and logical mode.");
	}
}
///////////////////////////////////////////////////////////////////
void CncLCDPositionPanel::onUpdateTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
}
