#include "SecureRun.h"

///////////////////////////////////////////////////////////////////
SecureRun::SecureRun(MainFrame* parent)
: SecureRunBase(parent)
, parentFrame(parent)
, isPause(false)
, headerFlag(false)
///////////////////////////////////////////////////////////////////
{
	m_startupTimer->Stop();
	m_blinkTimer->Stop();
	SetReturnCode(wxID_OK);
}
///////////////////////////////////////////////////////////////////
SecureRun::~SecureRun() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void SecureRun::enableControls(bool state) {
///////////////////////////////////////////////////////////////////
	m_btPlay->Enable(state);
	m_btStop->Enable(state);
} 
///////////////////////////////////////////////////////////////////
void SecureRun::initDialog(wxInitDialogEvent& event) {
///////////////////////////////////////////////////////////////////
	wxPoint pos = parentFrame->GetRcReset()->GetPosition();
	pos.x -= 38;
	SetPosition(pos);
	
	SetEscapeId(m_btEmergengy->GetId());
}
///////////////////////////////////////////////////////////////////
void SecureRun::show(wxShowEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( event.IsShown() == true ) {
		m_startupTimer->StartOnce(100);
		m_blinkTimer->Start(750);
	}
}
///////////////////////////////////////////////////////////////////
void SecureRun::hideDialog(int retValue) {
///////////////////////////////////////////////////////////////////
	SetReturnCode(retValue);
	
	m_blinkTimer->Stop();
	Show(false);
}
///////////////////////////////////////////////////////////////////
void SecureRun::startupTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	bool ret = parentFrame->secureRun();
	hideDialog( ret == true ? wxID_OK : wxID_ABORT );
}
///////////////////////////////////////////////////////////////////
void SecureRun::blinkTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	m_header->SetBackgroundColour(headerFlag == true ? wxColour(0, 0, 255) : wxColour(0, 0, 128));
	headerFlag == true ? headerFlag = false : headerFlag = true;
	
	m_header->Refresh();
	m_header->Update();
}
///////////////////////////////////////////////////////////////////
void SecureRun::stop(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
	wxPostEvent(parentFrame->GetRcStop(), evt);
	//hideDialog(wxID_CANCEL);
}
///////////////////////////////////////////////////////////////////
void SecureRun::emergengy(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
	wxPostEvent(parentFrame->GetBtnEmergenyStop(), evt);
	//hideDialog(wxID_ABORT);
}
///////////////////////////////////////////////////////////////////
void SecureRun::play(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( isPause == false ) {
		isPause = true;
		m_btPlay->SetBitmap(ImageLibSecureRun().Bitmap("BMP_RUN")); 
		m_btPlay->SetToolTip("Resume the current run");
		m_btPlay->SetLabel("Resume");
	} else {
		isPause = false;
		m_btPlay->SetBitmap(ImageLibSecureRun().Bitmap("BMP_PAUSE")); 
		m_btPlay->SetToolTip("Hold the current run");
		m_btPlay->SetLabel("Pause");
	}
	
	m_btPlay->SetBitmapPosition(wxBOTTOM);
	m_btPlay->Refresh();
	m_btPlay->Update();
	
	wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
	wxPostEvent(parentFrame->GetRcPause(), evt);
}
