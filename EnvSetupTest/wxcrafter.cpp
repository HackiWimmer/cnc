//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: wxcrafter.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "wxcrafter.h"


// Declare the bitmap loading function
extern void wxC9ED9InitBitmapResources();

static bool bBitmapLoaded = false;


MainDialogBaseClass::MainDialogBaseClass(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC9ED9InitBitmapResources();
        bBitmapLoaded = true;
    }
    // Set icon(s) to the application/dialog
    wxIconBundle app_icons;
    {
        wxBitmap iconBmp = wxXmlResource::Get()->LoadBitmap(wxT("CncTest"));
        wxIcon icn;
        icn.CopyFromBitmap(iconBmp);
        app_icons.AddIcon( icn );
    }
    SetIcons( app_icons );

    
    wxFlexGridSizer* flexGridSizer46 = new wxFlexGridSizer(3, 1, 0, 0);
    flexGridSizer46->SetFlexibleDirection( wxBOTH );
    flexGridSizer46->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer46->AddGrowableCol(0);
    flexGridSizer46->AddGrowableRow(0);
    flexGridSizer46->SetMinSize(500,800);
    this->SetSizer(flexGridSizer46);
    
    wxFlexGridSizer* flexGridSizer31 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer31->SetFlexibleDirection( wxBOTH );
    flexGridSizer31->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer31->AddGrowableCol(0);
    flexGridSizer31->AddGrowableRow(0);
    
    flexGridSizer46->Add(flexGridSizer31, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_choicebook74 = new wxChoicebook(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxBK_DEFAULT);
    m_choicebook74->SetName(wxT("m_choicebook74"));
    
    flexGridSizer31->Add(m_choicebook74, 0, wxALL|wxEXPAND, WXC_FROM_DIP(0));
    
    m_panel35 = new wxPanel(m_choicebook74, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_choicebook74, wxSize(-1,-1)), wxTAB_TRAVERSAL);
    m_choicebook74->AddPage(m_panel35, _("Version"), true);
    
    wxFlexGridSizer* flexGridSizer17 = new wxFlexGridSizer(4, 2, 0, 0);
    flexGridSizer17->SetFlexibleDirection( wxBOTH );
    flexGridSizer17->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    m_panel35->SetSizer(flexGridSizer17);
    
    m_staticText21 = new wxStaticText(m_panel35, wxID_ANY, _("wxWidget Version:"), wxDefaultPosition, wxDLG_UNIT(m_panel35, wxSize(-1,-1)), 0);
    
    flexGridSizer17->Add(m_staticText21, 0, wxALL, WXC_FROM_DIP(5));
    
    m_wxVersionInfo = new wxStaticText(m_panel35, wxID_ANY, _("?????"), wxDefaultPosition, wxDLG_UNIT(m_panel35, wxSize(-1,-1)), 0);
    
    flexGridSizer17->Add(m_wxVersionInfo, 0, wxALL, WXC_FROM_DIP(5));
    
    m_staticText22 = new wxStaticText(m_panel35, wxID_ANY, _("boost Version:"), wxDefaultPosition, wxDLG_UNIT(m_panel35, wxSize(-1,-1)), 0);
    
    flexGridSizer17->Add(m_staticText22, 0, wxALL, WXC_FROM_DIP(5));
    
    m_boostVersionInfo = new wxStaticText(m_panel35, wxID_ANY, _("?????"), wxDefaultPosition, wxDLG_UNIT(m_panel35, wxSize(-1,-1)), 0);
    
    flexGridSizer17->Add(m_boostVersionInfo, 0, wxALL, WXC_FROM_DIP(5));
    
    m_staticText27 = new wxStaticText(m_panel35, wxID_ANY, _("gcc Version:"), wxDefaultPosition, wxDLG_UNIT(m_panel35, wxSize(-1,-1)), 0);
    
    flexGridSizer17->Add(m_staticText27, 0, wxALL, WXC_FROM_DIP(5));
    
    m_gccVersionInfo = new wxStaticText(m_panel35, wxID_ANY, _("?????"), wxDefaultPosition, wxDLG_UNIT(m_panel35, wxSize(-1,-1)), 0);
    
    flexGridSizer17->Add(m_gccVersionInfo, 0, wxALL, WXC_FROM_DIP(5));
    
    m_staticText70 = new wxStaticText(m_panel35, wxID_ANY, _("wxSvg Version:"), wxDefaultPosition, wxDLG_UNIT(m_panel35, wxSize(-1,-1)), 0);
    
    flexGridSizer17->Add(m_staticText70, 0, wxALL, WXC_FROM_DIP(5));
    
    m_wxSvgVersionInfo = new wxStaticText(m_panel35, wxID_ANY, _("?????"), wxDefaultPosition, wxDLG_UNIT(m_panel35, wxSize(-1,-1)), 0);
    
    flexGridSizer17->Add(m_wxSvgVersionInfo, 0, wxALL, WXC_FROM_DIP(5));
    
    m_panel37 = new wxPanel(m_choicebook74, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_choicebook74, wxSize(-1,-1)), wxTAB_TRAVERSAL);
    m_choicebook74->AddPage(m_panel37, _("wxSvg"), false);
    
    wxFlexGridSizer* flexGridSizer43 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer43->SetFlexibleDirection( wxBOTH );
    flexGridSizer43->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer43->AddGrowableCol(0);
    flexGridSizer43->AddGrowableRow(0);
    m_panel37->SetSizer(flexGridSizer43);
    
    m_wxSvgParent = new wxPanel(m_panel37, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel37, wxSize(-1,-1)), wxTAB_TRAVERSAL);
    
    flexGridSizer43->Add(m_wxSvgParent, 0, wxALL|wxEXPAND, WXC_FROM_DIP(0));
    m_wxSvgParent->SetMinSize(wxSize(100,100));
    
    m_panel39 = new wxPanel(m_choicebook74, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_choicebook74, wxSize(-1,-1)), wxTAB_TRAVERSAL);
    m_choicebook74->AddPage(m_panel39, _("OpenGl"), false);
    
    m_panel41 = new wxPanel(m_choicebook74, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_choicebook74, wxSize(-1,-1)), wxTAB_TRAVERSAL);
    m_choicebook74->AddPage(m_panel41, _("OpenCV"), false);
    
    m_panel163 = new wxPanel(m_choicebook74, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_choicebook74, wxSize(-1,-1)), wxTAB_TRAVERSAL);
    m_choicebook74->AddPage(m_panel163, _("COM Scan"), true);
    
    wxFlexGridSizer* flexGridSizer165 = new wxFlexGridSizer(2, 1, 0, 0);
    flexGridSizer165->SetFlexibleDirection( wxBOTH );
    flexGridSizer165->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer165->AddGrowableCol(0);
    flexGridSizer165->AddGrowableRow(1);
    m_panel163->SetSizer(flexGridSizer165);
    
    m_button167 = new wxButton(m_panel163, wxID_ANY, _("Scan"), wxDefaultPosition, wxDLG_UNIT(m_panel163, wxSize(-1,-1)), 0);
    
    flexGridSizer165->Add(m_button167, 0, wxALL|wxEXPAND, WXC_FROM_DIP(0));
    
    m_comScanOutput = new wxTextCtrl(m_panel163, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(m_panel163, wxSize(-1,-1)), wxTE_READONLY|wxTE_MULTILINE|wxTE_DONTWRAP);
    
    flexGridSizer165->Add(m_comScanOutput, 0, wxALL|wxEXPAND, WXC_FROM_DIP(0));
    
    m_staticLine15 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxLI_HORIZONTAL);
    
    flexGridSizer46->Add(m_staticLine15, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    wxBoxSizer* boxSizer12 = new wxBoxSizer(wxHORIZONTAL);
    
    flexGridSizer46->Add(boxSizer12, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));
    
    m_buttonOK = new wxButton(this, wxID_OK, _("OK"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    m_buttonOK->SetDefault();
    
    boxSizer12->Add(m_buttonOK, 0, wxALL, WXC_FROM_DIP(5));
    
    m_buttonCancel = new wxButton(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    
    boxSizer12->Add(m_buttonCancel, 0, wxALL, WXC_FROM_DIP(5));
    
    SetName(wxT("MainDialogBaseClass"));
    SetSize(wxDLG_UNIT(this, wxSize(500,800)));
    if (GetSizer()) {
         GetSizer()->Fit(this);
    }
    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }
    // Connect events
    this->Connect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(MainDialogBaseClass::onInitDialog), NULL, this);
    m_button167->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainDialogBaseClass::onScanCOM), NULL, this);
    
}

MainDialogBaseClass::~MainDialogBaseClass()
{
    this->Disconnect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(MainDialogBaseClass::onInitDialog), NULL, this);
    m_button167->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainDialogBaseClass::onScanCOM), NULL, this);
    
}
