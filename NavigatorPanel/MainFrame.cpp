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
	newCtrl->SetPosition(oldCtrl->GetPosition());
	
	sizer->Replace(oldCtrl, newCtrl, true);
	sizer->Layout();
	
	// remove the placeholder
	oldCtrl->Destroy();
	// do not delete oldCtrl this will be handled by wx... 
}


wxDEFINE_EVENT(wxEVT_CNC_NAVIGATOR_PANEL, 			CncNavigatorPanelEvent);


MainFrame::MainFrame(wxWindow* parent)
    : MainFrameBaseClass(parent)
{
	mf = this;
	log("MainFrame\n");
	
	CncNavigatorPanel::Config cfg;
	cfg.innerCircle = true;
	cfg.shortFormat = false;
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

	this->Bind(wxEVT_CNC_NAVIGATOR_PANEL, 		&MainFrame::onNavigatorPanel, 			this);
}


MainFrame::~MainFrame()
{
	this->Unbind(wxEVT_CNC_NAVIGATOR_PANEL, 	&MainFrame::onNavigatorPanel, 			this, 0);
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
