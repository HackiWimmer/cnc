#include <wx/textdlg.h>
#include "GlobalFunctions.h"
#include "MainFrame.h"
#include "CncMessageDialog.h"
#include "CncSerialSpyListCtrl.h"
#include "CncSerialSpyPanel.h"
#include "wxCrafterImages.h"

/////////////////////////////////////////////////////////////
CncSerialSpyPanel::CncSerialSpyPanel(wxWindow* parent)
: CncSerialSpyPanelBase	(parent)
, serialSpyListCtrl		(NULL)
, spyDetailWindow		(NULL)
/////////////////////////////////////////////////////////////
{
	serialSpyListCtrl = new CncSerialSpyListCtrl(this, wxLC_VRULES | wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_serialSpyPlaceholder, serialSpyListCtrl);
	
	serialSpyListCtrl->enableLiveDecoding(m_liveDecoding->GetValue());
	serialSpyListCtrl->enableAutoScrolling(m_autoColumnSizing->GetValue());
	serialSpyListCtrl->enableAutoColumnSizing(m_autoScroling->GetValue());
	serialSpyListCtrl->installOpenDetails(m_openDetails);
	
	wxFont font(9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));
	const wxColour fg = *wxLIGHT_GREY;
	const wxColour bg = *wxBLACK;
	
	m_infobar->SetFont(font);
	m_infobar->SetEffectDuration(60);
	m_infobar->SetShowHideEffects(wxSHOW_EFFECT_ROLL_TO_BOTTOM, wxSHOW_EFFECT_ROLL_TO_TOP);
	m_infobar->SetOwnBackgroundColour(bg);
	m_infobar->SetForegroundColour(fg);
}
/////////////////////////////////////////////////////////////
CncSerialSpyPanel::~CncSerialSpyPanel() {
/////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void CncSerialSpyPanel::initDuringConnect() {
///////////////////////////////////////////////////////////////////
	enableSerialSpy(m_enableSerialSpy->GetValue());
	selectSerialSpyMode();
	clearSerialSpyOnConnect();
	
	if (m_enableSerialSpy->GetValue() == true )
		serialSpyListCtrl->addLine("Connect Serial", CncSerialSpyListCtrl::LineType::LT_Marker);
}
///////////////////////////////////////////////////////////////////
void CncSerialSpyPanel::decorateSerialSpy() {
///////////////////////////////////////////////////////////////////
	const bool b = m_enableSerialSpy->GetValue();
	
	m_enableSerialSpy->SetBitmap(ImageLib16().Bitmap(b ? "BMP_CONNECTED": "BMP_DISCONNECTED")); 
	m_enableSerialSpy->SetToolTip(b ? "Disable Serial Spy" : "Enable Serial Spy");
	b ? cnc::spy.enableMessage() : cnc::spy.disableMessage();
	
	m_enableSerialSpy->Refresh();
	m_enableSerialSpy->Update();
}
/////////////////////////////////////////////////////////////
void CncSerialSpyPanel::enableSerialSpy(bool state) {
/////////////////////////////////////////////////////////////
	if ( THE_APP->getCncControl() ) 
		THE_APP->getCncControl()->enableSpyOutput(state);
	
	m_enableSerialSpy->SetValue(state);
	
	decorateSerialSpy();
}
/////////////////////////////////////////////////////////////
void CncSerialSpyPanel::clearSerialSpyBeforNextRun() {
/////////////////////////////////////////////////////////////
	serialSpyListCtrl->clearAll();
}
/////////////////////////////////////////////////////////////
void CncSerialSpyPanel::clearSerialSpyOnConnect() {
/////////////////////////////////////////////////////////////
	serialSpyListCtrl->clearAll();
}
/////////////////////////////////////////////////////////////
void CncSerialSpyPanel::selectSerialSpyMode() {
/////////////////////////////////////////////////////////////
	if ( THE_APP->getCncControl() == NULL )
		return;

	int sm = m_cbSerialSpyMode->GetSelection();
	switch ( sm ) {
		case 0:		THE_APP->getCncControl()->setSpyMode(Serial::SypMode::SM_READ);		break;
		case 1:		THE_APP->getCncControl()->setSpyMode(Serial::SypMode::SM_WRITE);	break;
		default:	THE_APP->getCncControl()->setSpyMode(Serial::SypMode::SM_ALL);		break;
	}
}
/////////////////////////////////////////////////////////////
void CncSerialSpyPanel::clearSerialSpy(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////
	serialSpyListCtrl->clearAll();
}
/////////////////////////////////////////////////////////////
void CncSerialSpyPanel::enableSerialSpy(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////
	if ( THE_APP->getCncControl() ) 
		THE_APP->getCncControl()->enableSpyOutput(m_enableSerialSpy->GetValue());
		
	decorateSerialSpy();
}
/////////////////////////////////////////////////////////////
void CncSerialSpyPanel::onAutoColumnSize(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////
	serialSpyListCtrl->enableAutoColumnSizing(m_autoColumnSizing->GetValue());
}
/////////////////////////////////////////////////////////////
void CncSerialSpyPanel::onAutoScrolling(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////
	serialSpyListCtrl->enableAutoScrolling(m_autoScroling->GetValue());
}
/////////////////////////////////////////////////////////////
void CncSerialSpyPanel::onLiveDecoding(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////
	serialSpyListCtrl->enableLiveDecoding(m_liveDecoding->GetValue());
}
/////////////////////////////////////////////////////////////
void CncSerialSpyPanel::markSerialSpy(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////
	wxString defaultValue(wxString::Format("Marker::%06d", serialSpyListCtrl->getItemCount()));
	
	wxTextEntryDialog dlg(this, "Marker Label:", "Add Spy Marker . . .", defaultValue);
	dlg.SetMaxLength(64);
	
	if ( dlg.ShowModal() == wxID_OK )
		serialSpyListCtrl->addLine(dlg.GetValue(), CncSerialSpyListCtrl::LineType::LT_Marker);
}
/////////////////////////////////////////////////////////////
void CncSerialSpyPanel::openSpyDetailWindow(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////
	wxString details;

	m_infobar->Dismiss();
	serialSpyListCtrl->decodeSelectedSpyLine(details);
	
	details.Replace(" | ", "\n", true);
	details.Replace(" |",  "\n", true);
	details.Replace("| ",  "\n", true);
	details.Replace("|",   "\n", true);
	
	if ( details.IsEmpty() == false )
		m_infobar->ShowMessage(details);
}
/////////////////////////////////////////////////////////////
void CncSerialSpyPanel::selectSerialSpyMode(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////
	selectSerialSpyMode();
}
