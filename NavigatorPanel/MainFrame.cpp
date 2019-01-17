#include "MainFrame.h"
#include <wx/aboutdlg.h>
#include <iostream>

MainFrame* mf = NULL;

void log(const wxString& str) {
	
	mf->GetLogger()->AppendText(str);
}

void log1(const wxString& str) { mf->GetTextCtr_01()->ChangeValue(str); }
void log2(const wxString& str) { mf->GetTextCtr_02()->ChangeValue(str); }
void log3(const wxString& str) { mf->GetTextCtr_03()->ChangeValue(str); }
void log4(const wxString& str) { mf->GetTextCtr_04()->ChangeValue(str); }
void log5(const wxString& str) { mf->GetTextCtr_05()->ChangeValue(str); }
void log6(const wxString& str) { mf->GetTextCtr_06()->ChangeValue(str); }
void log7(const wxString& str) { mf->GetTextCtr_07()->ChangeValue(str); }
void log8(const wxString& str) { mf->GetTextCtr_08()->ChangeValue(str); }


///////////////////////////////////////////////////////////////////
void replaceControl(wxWindow* oldCtrl, wxWindow* newCtrl) {
///////////////////////////////////////////////////////////////////
	
	wxASSERT( oldCtrl != NULL );
	wxASSERT( newCtrl != NULL );
	
	wxWindow* parent = oldCtrl->GetParent();
	wxSizer* sizer   = oldCtrl->GetContainingSizer();
	
	wxASSERT( parent != NULL );
	wxASSERT( sizer != NULL );
	
	newCtrl->Reparent(parent);
	//newCtrl->SetPosition(oldCtrl->GetPosition());
	//newCtrl->SetClientSize(oldCtrl->GetClientSize());
	
	sizer->Replace(oldCtrl, newCtrl, true);
	sizer->Layout();
	
	newCtrl->SetPosition(oldCtrl->GetPosition());
	newCtrl->SetClientSize(oldCtrl->GetClientSize());
	
	// remove the placeholder
	oldCtrl->Destroy();
	// do not delete oldCtrl this will be handled by wx... 
}

CncMouseRemoteControl* remoteControl = NULL;

MainFrame::MainFrame(wxWindow* parent)
    : MainFrameBaseClass(parent)
{
	mf = this;
	log("MainFrame\n");
	
	CncNavigatorPanel::Config cfg;
	cfg.innerCircle = true;
	cfg.shortFormat = false;
	
	cfg.toolTipMap[CncNavigatorPanel::Direction::UD] 	= "";
	cfg.toolTipMap[CncNavigatorPanel::Direction::NN] 	= "+Y";
	cfg.toolTipMap[CncNavigatorPanel::Direction::SS] 	= "-Y";
	cfg.toolTipMap[CncNavigatorPanel::Direction::WW] 	= "-X";
	cfg.toolTipMap[CncNavigatorPanel::Direction::EE] 	= "+X";
	cfg.toolTipMap[CncNavigatorPanel::Direction::NW] 	= "+Y -X";
	cfg.toolTipMap[CncNavigatorPanel::Direction::NE] 	= "+Y +X";
	cfg.toolTipMap[CncNavigatorPanel::Direction::SW] 	= "-Y -X";
	cfg.toolTipMap[CncNavigatorPanel::Direction::SE] 	= "-Y +X";
	cfg.toolTipMap[CncNavigatorPanel::Direction::CP] 	= "+Z";
	cfg.toolTipMap[CncNavigatorPanel::Direction::CN] 	= "-Z";
	
	CncNavigatorPanel* p1 = new CncNavigatorPanel(this, cfg);
	
	replaceControl(m_mainPanel1, p1);

	cfg.innerCircle = true;
	cfg.shortFormat = true;
	CncNavigatorPanel* p2 = new CncNavigatorPanel(this, cfg);
	
	replaceControl(m_mainPanel2, p2);
	
	cfg.innerCircle = false;
	cfg.shortFormat = true;
	CncNavigatorPanel* p3 = new CncNavigatorPanel(this, cfg);
	
	replaceControl(m_mainPanel3, p3);
	
	CncMouseRemoteControl* m1 = new CncMouseRemoteControl(this);
	CncMouseRemoteControl::Config& config = m1->getConfig();
	config.colMouse_Center = *wxYELLOW;
	
	remoteControl = m1;
	replaceControl(m_remoteControl1, m1);
	
	this->Bind(wxEVT_CHAR_HOOK, 					&MainFrame::globalKeyDownHook, 			this);
	this->Bind(wxEVT_CNC_NAVIGATOR_PANEL, 			&MainFrame::onNavigatorPanel, 			this);
	this->Bind(wxEVT_CNC_MOUSE_REMOTE_CONTROL, 		&MainFrame::onRemoteControl, 			this);
}


MainFrame::~MainFrame()
{
	this->Unbind(wxEVT_CHAR_HOOK, 					&MainFrame::globalKeyDownHook, 			this);
	this->Unbind(wxEVT_CNC_NAVIGATOR_PANEL, 		&MainFrame::onNavigatorPanel, 			this, 0);
	this->Unbind(wxEVT_CNC_MOUSE_REMOTE_CONTROL, 	&MainFrame::onRemoteControl, 			this, 0);
}

void MainFrame::globalKeyDownHook(wxKeyEvent& event) {
	
		if ( remoteControl && remoteControl->IsShownOnScreen() ) {
		// This is necessary to avoid the default notebook key handling
		if ( remoteControl->HasFocus() ) {
			remoteControl->onKeyDown(event);
			event.Skip(false);
			return;
		}
	}
}

void MainFrame::onNavigatorPanel(CncNavigatorPanelEvent& event) {
	
	CncNavigatorPanelEvent::Id eventId = (CncNavigatorPanelEvent::Id)event.GetId();
	if ( eventId == CncNavigatorPanelEvent::Id::CNP_COORDINATES )
		return;
	
	
	log(wxString::Format("MainFrame::onNavigatorPanel: %s (%ld)\n", CncNavigatorPanelEvent::getEventIdAsString((CncNavigatorPanelEvent::Id)event.GetId()), event.getNavigatorPanel()));
	
	log1(wxString::Format("%.3lf",	event.radius));
	log2(wxString::Format("%.3lf",	event.angle));
	log3(wxString::Format("%d",  	event.mouseX));
	log4(wxString::Format("%d",  	event.mouseY));
	log5(wxString::Format("%d",  	event.GetId()));
	log6(wxString::Format("%d",  	event.activated));
	log8(wxString::Format("%s",  	CncNavigatorPanel::getDirectionAsString(event.direction)));
	
	
}

void MainFrame::onRemoteControl(CncMouseRemoteControlEvent& event) {
	if ( event.GetId() == CncMouseRemoteControlEvent::Id::CMRC_MOUSE )
		return;
		
	if ( event.GetId() == CncMouseRemoteControlEvent::Id::CMRC_RELEASE_LEFT_DOWN )
		log("CMRC_RELEASE_LEFT_DOWN\n");

	if ( event.GetId() == CncMouseRemoteControlEvent::Id::CMRC_RELEASE_LEFT_UP )
		log("CMRC_RELEASE_LEFT_UP\n");
		
	if ( event.GetId() == CncMouseRemoteControlEvent::Id::CMRC_ENTER )
		log("CMRC_ENTER\n");
		
	if ( event.GetId() == CncMouseRemoteControlEvent::Id::CMRC_LEAVE )
		log("CMRC_LEAVE\n");

	if ( event.GetId() == CncMouseRemoteControlEvent::Id::CMRC_SET_FOCUS )
		log("CMRC_SET_FOUCS\n");
		
	if ( event.GetId() == CncMouseRemoteControlEvent::Id::CMRC_KILL_FOCUS )
		log("CMRC_KILL_FOUCS\n");
	
	if ( event.GetId() == CncMouseRemoteControlEvent::Id::CMRC_MOUSE_MOVE ) {
		log1(wxString::Format("%d",  	event.mouseMove.GetWidth()));
		log2(wxString::Format("%d",  	event.mouseMove.GetHeight()));
	}
	
	if ( event.GetId() == CncMouseRemoteControlEvent::Id::CMRC_WHEEL_ROTATION ) {
		log4(wxString::Format("%d",  	event.wheelRotation));
	}
	
	if ( event.GetId() == CncMouseRemoteControlEvent::Id::CMRC_KEY_DOWN ) {
		log5(wxString::Format("%d",  	event.keyCode));
	}
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Close();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxAboutDialogInfo info;
    info.SetCopyright(_("My MainFrame"));
    info.SetLicence(_("GPL v2 or later"));
    info.SetDescription(_("Short description goes here"));
    ::wxAboutBox(info);
}
