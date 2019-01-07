//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: wxcrafter.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "wxcrafter.h"


// Declare the bitmap loading function
extern void wxC9ED9InitBitmapResources();

static bool bBitmapLoaded = false;


MainFrameBaseClass::MainFrameBaseClass(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxFrame(parent, id, title, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC9ED9InitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxBoxSizer* boxSizer1 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer1);
    
    wxFlexGridSizer* flexGridSizer12 = new wxFlexGridSizer(2, 1, 0, 0);
    flexGridSizer12->SetFlexibleDirection( wxBOTH );
    flexGridSizer12->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer12->AddGrowableCol(0);
    flexGridSizer12->AddGrowableRow(0);
    flexGridSizer12->AddGrowableRow(1);
    
    boxSizer1->Add(flexGridSizer12, 1, wxEXPAND, WXC_FROM_DIP(5));
    
    wxFlexGridSizer* flexGridSizer25 = new wxFlexGridSizer(1, 3, 0, 0);
    flexGridSizer25->SetFlexibleDirection( wxBOTH );
    flexGridSizer25->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer25->AddGrowableCol(0);
    flexGridSizer25->AddGrowableCol(1);
    flexGridSizer25->AddGrowableCol(2);
    flexGridSizer25->AddGrowableRow(0);
    
    flexGridSizer12->Add(flexGridSizer25, 1, wxEXPAND, WXC_FROM_DIP(1));
    
    m_mainPanel1 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxTAB_TRAVERSAL|wxBORDER_STATIC);
    m_mainPanel1->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    
    flexGridSizer25->Add(m_mainPanel1, 0, wxEXPAND, WXC_FROM_DIP(3));
    
    m_mainPanel2 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxTAB_TRAVERSAL|wxBORDER_STATIC);
    m_mainPanel2->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    
    flexGridSizer25->Add(m_mainPanel2, 0, wxEXPAND, WXC_FROM_DIP(3));
    
    m_mainPanel3 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxTAB_TRAVERSAL|wxBORDER_STATIC);
    m_mainPanel3->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    
    flexGridSizer25->Add(m_mainPanel3, 0, wxEXPAND, WXC_FROM_DIP(3));
    
    wxFlexGridSizer* flexGridSizer13 = new wxFlexGridSizer(2, 1, 0, 0);
    flexGridSizer13->SetFlexibleDirection( wxBOTH );
    flexGridSizer13->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer13->AddGrowableCol(0);
    flexGridSizer13->AddGrowableRow(1);
    
    flexGridSizer12->Add(flexGridSizer13, 0, wxALL|wxEXPAND, WXC_FROM_DIP(0));
    
    wxFlexGridSizer* flexGridSizer15 = new wxFlexGridSizer(2, 4, 0, 0);
    flexGridSizer15->SetFlexibleDirection( wxBOTH );
    flexGridSizer15->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer15->AddGrowableCol(0);
    flexGridSizer15->AddGrowableCol(1);
    flexGridSizer15->AddGrowableCol(2);
    flexGridSizer15->AddGrowableCol(3);
    flexGridSizer15->AddGrowableRow(0);
    flexGridSizer15->AddGrowableRow(1);
    
    flexGridSizer13->Add(flexGridSizer15, 1, wxALL|wxEXPAND, WXC_FROM_DIP(0));
    
    m_textCtr_01 = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxTE_RIGHT|wxTE_READONLY);
    #if wxVERSION_NUMBER >= 3000
    m_textCtr_01->SetHint(wxT(""));
    #endif
    
    flexGridSizer15->Add(m_textCtr_01, 0, wxALL, WXC_FROM_DIP(1));
    
    m_textCtr_02 = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxTE_RIGHT|wxTE_READONLY);
    #if wxVERSION_NUMBER >= 3000
    m_textCtr_02->SetHint(wxT(""));
    #endif
    
    flexGridSizer15->Add(m_textCtr_02, 0, wxALL, WXC_FROM_DIP(1));
    
    m_textCtr_03 = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxTE_RIGHT|wxTE_READONLY);
    #if wxVERSION_NUMBER >= 3000
    m_textCtr_03->SetHint(wxT(""));
    #endif
    
    flexGridSizer15->Add(m_textCtr_03, 0, wxALL, WXC_FROM_DIP(1));
    
    m_textCtr_04 = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxTE_RIGHT|wxTE_READONLY);
    #if wxVERSION_NUMBER >= 3000
    m_textCtr_04->SetHint(wxT(""));
    #endif
    
    flexGridSizer15->Add(m_textCtr_04, 0, wxALL, WXC_FROM_DIP(1));
    
    m_textCtr_05 = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxTE_RIGHT|wxTE_READONLY);
    #if wxVERSION_NUMBER >= 3000
    m_textCtr_05->SetHint(wxT(""));
    #endif
    
    flexGridSizer15->Add(m_textCtr_05, 0, wxALL, WXC_FROM_DIP(1));
    
    m_textCtr_06 = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxTE_RIGHT|wxTE_READONLY);
    #if wxVERSION_NUMBER >= 3000
    m_textCtr_06->SetHint(wxT(""));
    #endif
    
    flexGridSizer15->Add(m_textCtr_06, 0, wxALL, WXC_FROM_DIP(1));
    
    m_textCtr_07 = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxTE_RIGHT|wxTE_READONLY);
    #if wxVERSION_NUMBER >= 3000
    m_textCtr_07->SetHint(wxT(""));
    #endif
    
    flexGridSizer15->Add(m_textCtr_07, 0, wxALL, WXC_FROM_DIP(1));
    
    m_textCtr_08 = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxTE_RIGHT|wxTE_READONLY);
    #if wxVERSION_NUMBER >= 3000
    m_textCtr_08->SetHint(wxT(""));
    #endif
    
    flexGridSizer15->Add(m_textCtr_08, 0, wxALL, WXC_FROM_DIP(1));
    
    m_logger = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxTE_RICH|wxTE_READONLY|wxTE_MULTILINE);
    
    flexGridSizer13->Add(m_logger, 0, wxALL|wxEXPAND, WXC_FROM_DIP(0));
    
    m_menuBar = new wxMenuBar(0);
    this->SetMenuBar(m_menuBar);
    
    m_name6 = new wxMenu();
    m_menuBar->Append(m_name6, _("File"));
    
    m_menuItem7 = new wxMenuItem(m_name6, wxID_EXIT, _("Exit\tAlt-X"), _("Quit"), wxITEM_NORMAL);
    m_name6->Append(m_menuItem7);
    
    m_name8 = new wxMenu();
    m_menuBar->Append(m_name8, _("Help"));
    
    m_menuItem9 = new wxMenuItem(m_name8, wxID_ABOUT, _("About..."), wxT(""), wxITEM_NORMAL);
    m_name8->Append(m_menuItem9);
    
    m_mainToolbar = this->CreateToolBar(wxTB_FLAT, wxID_ANY);
    m_mainToolbar->SetToolBitmapSize(wxSize(16,16));
    
    SetName(wxT("MainFrameBaseClass"));
    SetSize(500,500);
    if (GetSizer()) {
         GetSizer()->Fit(this);
    }
    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }
#if wxVERSION_NUMBER >= 2900
    if(!wxPersistenceManager::Get().Find(this)) {
        wxPersistenceManager::Get().RegisterAndRestore(this);
    } else {
        wxPersistenceManager::Get().Restore(this);
    }
#endif
    // Connect events
    this->Connect(m_menuItem7->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrameBaseClass::OnExit), NULL, this);
    this->Connect(m_menuItem9->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrameBaseClass::OnAbout), NULL, this);
    
}

MainFrameBaseClass::~MainFrameBaseClass()
{
    this->Disconnect(m_menuItem7->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrameBaseClass::OnExit), NULL, this);
    this->Disconnect(m_menuItem9->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrameBaseClass::OnAbout), NULL, this);
    
}
