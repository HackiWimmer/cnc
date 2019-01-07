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
void SecureRun::resetPerspectiveButtons() {
///////////////////////////////////////////////////////////////////
	m_btViewTop->SetValue(false);
	m_btViewBottom->SetValue(false);
	m_btViewFront->SetValue(false);
	m_btViewRear->SetValue(false);
	m_btViewLeft->SetValue(false);
	m_btViewRight->SetValue(false);
	m_btViewIso1->SetValue(false);
	m_btViewIso2->SetValue(false);
	m_btViewIso3->SetValue(false);
	m_btViewIso4->SetValue(false);
} 
///////////////////////////////////////////////////////////////////
void SecureRun::initDialog(wxInitDialogEvent& event) {
///////////////////////////////////////////////////////////////////
	wxPoint orignPos = parentFrame->GetScreenPosition();
	wxPoint pos      = parentFrame->GetRcReset()->GetPosition();
	orignPos.x += pos.x -38;
	orignPos.y += pos.y;
	SetPosition(orignPos);

	SetEscapeId(m_btEmergengy->GetId());
	
	resetPerspectiveButtons();
	
	typedef GLContextBase::ViewMode VM;
	switch ( THE_APP->getMotionMonitor()->getViewMode() ) {
		case VM::V2D_TOP:		m_btViewTop->SetValue(true); 	break;
		case VM::V2D_BOTTOM:	m_btViewBottom->SetValue(true); break;
		case VM::V2D_LEFT: 		m_btViewLeft->SetValue(true); 	break;
		case VM::V2D_RIGHT: 	m_btViewRight->SetValue(true); 	break;
		case VM::V2D_FRONT: 	m_btViewFront->SetValue(true); 	break;
		case VM::V2D_REAR:		m_btViewRear->SetValue(true); 	break;
		case VM::V3D_ISO1:		m_btViewIso1->SetValue(true); 	break;
		case VM::V3D_ISO2: 		m_btViewIso2->SetValue(true); 	break;
		case VM::V3D_ISO3: 		m_btViewIso3->SetValue(true); 	break;
		case VM::V3D_ISO4:		m_btViewIso4->SetValue(true); 	break;
		default:			;
	}
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
}
///////////////////////////////////////////////////////////////////
void SecureRun::emergengy(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
	wxPostEvent(parentFrame->GetBtnEmergenyStop(), evt);
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
///////////////////////////////////////////////////////////////////
void SecureRun::changeView(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxBitmapToggleButton* button = (wxBitmapToggleButton*)(event.GetEventObject());
	wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
	
	
	resetPerspectiveButtons();
	button->SetValue(true);
	
	if      ( button == m_btViewTop    )	{ evt.SetEventObject(m_btViewTop);	wxPostEvent(parentFrame->Get3D_Top(), 			evt); }
	else if ( button == m_btViewBottom )	{ evt.SetEventObject(m_btViewBottom); wxPostEvent(parentFrame->Get3D_Bottom(), 		evt); }
	else if ( button == m_btViewFront  )	{ evt.SetEventObject(m_btViewFront); 	wxPostEvent(parentFrame->Get3D_Front(), 	evt); }
	else if ( button == m_btViewRear   )	{ evt.SetEventObject(m_btViewRear); 	wxPostEvent(parentFrame->Get3D_Rear(), 		evt); }
	else if ( button == m_btViewLeft   )	{ evt.SetEventObject(m_btViewLeft); 	wxPostEvent(parentFrame->Get3D_Front(), 	evt); }
	else if ( button == m_btViewRight  )	{ evt.SetEventObject(m_btViewRight);	wxPostEvent(parentFrame->Get3D_Rear(), 		evt); }
	else if ( button == m_btViewIso1   )	{ evt.SetEventObject(m_btViewIso1);	wxPostEvent(parentFrame->Get3D_Perspective1(), 	evt); }
	else if ( button == m_btViewIso2   )	{ evt.SetEventObject(m_btViewIso2);	wxPostEvent(parentFrame->Get3D_Perspective2(),	evt); }
	else if ( button == m_btViewIso3   )	{ evt.SetEventObject(m_btViewIso3);	wxPostEvent(parentFrame->Get3D_Perspective3(), 	evt); }
	else if ( button == m_btViewIso4   )	{ evt.SetEventObject(m_btViewIso4);	wxPostEvent(parentFrame->Get3D_Perspective4(), 	evt); }
}
