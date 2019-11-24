#include "GlobalFunctions.h"
#include "MainFrame.h"
#include "CncSecureRun.h"

///////////////////////////////////////////////////////////////////
CncSecureRun::CncSecureRun(wxWindow* parent)
: CncSecureRunBase(parent)
, remoteControl(NULL)
, isPause(false)
, headerFlag(false)
///////////////////////////////////////////////////////////////////
{
	m_startupTimer->Stop();
	m_blinkTimer->Stop();
	SetReturnCode(wxID_OK);
	
	//remote control
	remoteControl = new CncMouseRemoteControl(this);
	GblFunc::replaceControl(m_remoteControl, remoteControl);
	
	this->Bind(wxEVT_CHAR_HOOK, 					&CncSecureRun::onKeyDown, 			this);
	this->Bind(wxEVT_CNC_MOUSE_REMOTE_CONTROL, 		&CncSecureRun::onRemoteControl, 	this);
}
///////////////////////////////////////////////////////////////////
CncSecureRun::~CncSecureRun() {
///////////////////////////////////////////////////////////////////
	if ( remoteControl != NULL )
		delete remoteControl;
	
	this->Unbind(wxEVT_CHAR_HOOK, 					&CncSecureRun::onKeyDown, 			this);
	this->Unbind(wxEVT_CNC_MOUSE_REMOTE_CONTROL, 	&CncSecureRun::onRemoteControl, 	this);
}
///////////////////////////////////////////////////////////////////
void CncSecureRun::enableControls(bool state) {
///////////////////////////////////////////////////////////////////
	m_btPlay->Enable(state);
	m_btStop->Enable(state);
}
///////////////////////////////////////////////////////////////////
void CncSecureRun::resetPerspectiveButtons() {
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
void CncSecureRun::initDialog(wxInitDialogEvent& event) {
///////////////////////////////////////////////////////////////////
	wxPoint orignPos = THE_APP->GetScreenPosition();
	wxPoint pos      = THE_APP->GetRcReset()->GetPosition();
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
void CncSecureRun::show(wxShowEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( event.IsShown() == true ) {
		m_startupTimer->StartOnce(100);
		m_blinkTimer->Start(750);
	}
	
	remoteControl->SetFocusFromKbd();
}
///////////////////////////////////////////////////////////////////
void CncSecureRun::hideDialog(int retValue) {
///////////////////////////////////////////////////////////////////
	m_blinkTimer->Stop();
	EndModal(retValue);
}
///////////////////////////////////////////////////////////////////
void CncSecureRun::startupTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	bool ret = THE_APP->secureRun();
	hideDialog( ret == true ? wxID_OK : wxID_ABORT );
}
///////////////////////////////////////////////////////////////////
void CncSecureRun::blinkTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	auto blink = [&](wxPanel* p, bool flag) {
		p->SetBackgroundColour(flag == true ? wxColour(0, 0, 255) : wxColour(0, 0, 128));
		p->Refresh();
		p->Update();
	};
	
	blink(m_header1, headerFlag);
	blink(m_header2, headerFlag);
	
	headerFlag == true ? headerFlag = false : headerFlag = true;
}
///////////////////////////////////////////////////////////////////
void CncSecureRun::stop(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
	wxPostEvent(THE_APP->GetRcStop(), evt);
}
///////////////////////////////////////////////////////////////////
void CncSecureRun::emergengy(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
	wxPostEvent(THE_APP->GetBtnEmergenyStop(), evt);
}
///////////////////////////////////////////////////////////////////
void CncSecureRun::play(wxCommandEvent& event) {
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
	wxPostEvent(THE_APP->GetRcPause(), evt);
}
///////////////////////////////////////////////////////////////////
void CncSecureRun::changeView(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxBitmapToggleButton* button = (wxBitmapToggleButton*)(event.GetEventObject());
	wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
	
	resetPerspectiveButtons();
	button->SetValue(true);
	
	if      ( button == m_btViewTop    )	{ evt.SetEventObject(m_btViewTop);							wxPostEvent(THE_APP->Get3D_Top(), 			evt); }
	else if ( button == m_btViewBottom )	{ evt.SetEventObject(m_btViewBottom); 						wxPostEvent(THE_APP->Get3D_Bottom(), 		evt); }
	else if ( button == m_btViewFront  )	{ evt.SetEventObject(m_btViewFront); 						wxPostEvent(THE_APP->Get3D_Front(), 		evt); }
	else if ( button == m_btViewRear   )	{ evt.SetEventObject(m_btViewRear); 						wxPostEvent(THE_APP->Get3D_Rear(), 			evt); }
	else if ( button == m_btViewLeft   )	{ evt.SetEventObject(m_btViewLeft); 						wxPostEvent(THE_APP->Get3D_Front(), 		evt); }
	else if ( button == m_btViewRight  )	{ evt.SetEventObject(m_btViewRight);						wxPostEvent(THE_APP->Get3D_Rear(), 			evt); }
	else if ( button == m_btViewIso1   )	{ evt.SetEventObject(THE_APP->Get3D_Perspective1());	wxPostEvent(THE_APP->Get3D_Perspective1(), 	evt); }
	else if ( button == m_btViewIso2   )	{ evt.SetEventObject(THE_APP->Get3D_Perspective2());	wxPostEvent(THE_APP->Get3D_Perspective2(),	evt); }
	else if ( button == m_btViewIso3   )	{ evt.SetEventObject(THE_APP->Get3D_Perspective3());	wxPostEvent(THE_APP->Get3D_Perspective3(), 	evt); }
	else if ( button == m_btViewIso4   )	{ evt.SetEventObject(THE_APP->Get3D_Perspective4());	wxPostEvent(THE_APP->Get3D_Perspective4(), 	evt); }
}
///////////////////////////////////////////////////////////////////
void CncSecureRun::onKeyDown(wxKeyEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( event.GetKeyCode() == WXK_ESCAPE ) {
		wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
		wxPostEvent(THE_APP->GetBtnEmergenyStop(), evt);
		
	} else {
		if ( remoteControl->IsShownOnScreen() ) {
			if ( remoteControl->HasFocus() ) {
				remoteControl->onKeyDown(event);
				event.Skip(false);
				return;
			}
		}
	}
}
///////////////////////////////////////////////////////////////////
void CncSecureRun::onRemoteControl(CncMouseRemoteControlEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( event.GetId() == CncMouseRemoteControlEvent::Id::CMRC_WHEEL_ROTATION ) {
		if ( event.wheelRotation != 0 ) {
			if ( event.wheelRotation < 0 )	THE_APP->getMotionMonitor()->decScale();
			else							THE_APP->getMotionMonitor()->incScale();
		}
	}
	
	if ( event.GetId() == CncMouseRemoteControlEvent::Id::CMRC_RELEASE_LEFT_DOWN )
		THE_APP->getMotionMonitor()->initReshape();
		
	if ( event.GetId() == CncMouseRemoteControlEvent::Id::CMRC_MOUSE_MOVE )
		THE_APP->getMotionMonitor()->reshapeRelative(event.mouseMove.GetWidth(), event.mouseMove.GetHeight());
		
	if ( event.GetId() == CncMouseRemoteControlEvent::Id::CMRC_KEY_DOWN )
		THE_APP->getMotionMonitor()->onKeyDown(event.keyCode);
}

