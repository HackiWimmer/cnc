#include "GlobalFunctions.h"
#include "wxCrafterLCDPanel.h"
#include "MainFrame.h"
#include "CncLCDPositionPanel.h"

///////////////////////////////////////////////////////////////////
CncLCDPositionPanel::CncLCDPositionPanel(wxWindow* parent)
: CncLCDPositionPanelBase(parent)
, lcdF(NULL)
, lcdX(NULL)
, lcdY(NULL)
, lcdZ(NULL)
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
	lcdS->SetValue("0.0");
	
	lcdX->SetValue(THE_APP->GetXAxisCtl()->GetValue());
	lcdY->SetValue(THE_APP->GetYAxisCtl()->GetValue());
	lcdZ->SetValue(THE_APP->GetZAxisCtl()->GetValue());
}
///////////////////////////////////////////////////////////////////
void CncLCDPositionPanel::onUpdateTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
}
