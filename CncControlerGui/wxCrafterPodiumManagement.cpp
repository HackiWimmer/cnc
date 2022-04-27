//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: wxCrafterPodiumManagement.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "wxCrafterPodiumManagement.h"

// Declare the bitmap loading function
extern void wxC9ED9XXInitBitmapResources();

static bool bBitmapLoaded = false;

CncPodiumManagementBase::CncPodiumManagementBase(wxWindow* parent,
    wxWindowID id,
    const wxString& title,
    const wxPoint& pos,
    const wxSize& size,
    long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    if(!bBitmapLoaded) {
	// We need to initialise the default bitmap handler
	wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
	wxC9ED9XXInitBitmapResources();
	bBitmapLoaded = true;
    }
    // Set icon(s) to the application/dialog
    wxIconBundle app_icons;
    {
	wxBitmap iconBmp = wxXmlResource::Get()->LoadBitmap(wxT("16-sort"));
	wxIcon icn;
	icn.CopyFromBitmap(iconBmp);
	app_icons.AddIcon(icn);
    }
    SetIcons(app_icons);

    wxFlexGridSizer* flexGridSizer9548 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer9548->SetFlexibleDirection(wxBOTH);
    flexGridSizer9548->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer9548->AddGrowableCol(0);
    flexGridSizer9548->AddGrowableRow(0);
    flexGridSizer9548->SetMinSize(540, 500);
    this->SetSizer(flexGridSizer9548);

    m_basePanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_basePanel->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));

    flexGridSizer9548->Add(m_basePanel, 0, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    wxFlexGridSizer* flexGridSizer9572 = new wxFlexGridSizer(3, 1, 0, 0);
    flexGridSizer9572->SetFlexibleDirection(wxBOTH);
    flexGridSizer9572->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer9572->AddGrowableCol(0);
    flexGridSizer9572->AddGrowableRow(0);
    m_basePanel->SetSizer(flexGridSizer9572);

    wxFlexGridSizer* flexGridSizer9567 = new wxFlexGridSizer(1, 5, 0, 0);
    flexGridSizer9567->SetFlexibleDirection(wxBOTH);
    flexGridSizer9567->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer9567->AddGrowableCol(0);
    flexGridSizer9567->AddGrowableCol(2);
    flexGridSizer9567->AddGrowableCol(4);
    flexGridSizer9567->AddGrowableRow(0);

    flexGridSizer9572->Add(flexGridSizer9567, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_panelPicture =
        new wxPanel(m_basePanel, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_basePanel, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_panelPicture->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW));

    flexGridSizer9567->Add(m_panelPicture, 0, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    wxFlexGridSizer* flexGridSizer9615 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer9615->SetFlexibleDirection(wxBOTH);
    flexGridSizer9615->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer9615->AddGrowableCol(0);
    flexGridSizer9615->AddGrowableRow(0);
    m_panelPicture->SetSizer(flexGridSizer9615);

    m_staticBitmap9569 = new wxStaticBitmap(m_panelPicture, wxID_ANY, wxXmlResource::Get()->LoadBitmap(wxT("lift")),
        wxDefaultPosition, wxDLG_UNIT(m_panelPicture, wxSize(-1, -1)), 0);

    flexGridSizer9615->Add(m_staticBitmap9569, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_staticLine9571 = new wxStaticLine(
        m_basePanel, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_basePanel, wxSize(-1, -1)), wxLI_VERTICAL);

    flexGridSizer9567->Add(m_staticLine9571, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_panelMovementPlaceholder =
        new wxPanel(m_basePanel, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_basePanel, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_panelMovementPlaceholder->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));

    flexGridSizer9567->Add(m_panelMovementPlaceholder, 0, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    m_staticLine9554 = new wxStaticLine(
        m_basePanel, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_basePanel, wxSize(-1, -1)), wxLI_HORIZONTAL);

    flexGridSizer9572->Add(m_staticLine9554, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));

    wxFlexGridSizer* flexGridSizer9562 = new wxFlexGridSizer(1, 2, 0, 0);
    flexGridSizer9562->SetFlexibleDirection(wxBOTH);
    flexGridSizer9562->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer9562->AddGrowableCol(1);
    flexGridSizer9562->AddGrowableRow(0);

    flexGridSizer9572->Add(flexGridSizer9562, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));

    m_info = new wxStaticBitmap(m_basePanel, wxID_ANY, wxXmlResource::Get()->LoadBitmap(wxT("16-info@2x")),
        wxDefaultPosition, wxDLG_UNIT(m_basePanel, wxSize(-1, -1)), 0);

    flexGridSizer9562->Add(m_info, 0, wxALL, WXC_FROM_DIP(0));

    m_btClose =
        new wxButton(m_basePanel, wxID_ANY, _("Close"), wxDefaultPosition, wxDLG_UNIT(m_basePanel, wxSize(-1, -1)), 0);
#if wxVERSION_NUMBER >= 2904
    m_btClose->SetBitmap(wxXmlResource::Get()->LoadBitmap(wxT("window-close-3")), wxLEFT);
    m_btClose->SetBitmapMargins(2, 2);
#endif
    wxFont m_btCloseFont(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Segoe UI"));
    m_btClose->SetFont(m_btCloseFont);

    flexGridSizer9562->Add(m_btClose, 0, wxALL | wxALIGN_RIGHT, WXC_FROM_DIP(2));

    SetName(wxT("CncPodiumManagementBase"));
    SetMinClientSize(wxSize(540, 500));
    SetSize(wxDLG_UNIT(this, wxSize(-1, -1)));
    if(GetSizer()) {
	GetSizer()->Fit(this);
    }
    if(GetParent()) {
	CentreOnParent(wxBOTH);
    } else {
	CentreOnScreen(wxBOTH);
    }
    // Connect events
    this->Bind(wxEVT_SHOW, &CncPodiumManagementBase::onShow, this);
    this->Bind(wxEVT_INIT_DIALOG, &CncPodiumManagementBase::onInit, this);
    m_info->Bind(wxEVT_LEFT_DOWN, &CncPodiumManagementBase::onLefDownInfo, this);
    m_btClose->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CncPodiumManagementBase::onClose, this);
}

CncPodiumManagementBase::~CncPodiumManagementBase()
{
    this->Unbind(wxEVT_SHOW, &CncPodiumManagementBase::onShow, this);
    this->Unbind(wxEVT_INIT_DIALOG, &CncPodiumManagementBase::onInit, this);
    m_info->Unbind(wxEVT_LEFT_DOWN, &CncPodiumManagementBase::onLefDownInfo, this);
    m_btClose->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &CncPodiumManagementBase::onClose, this);
}

CncPodiumMgmtMovementBase::CncPodiumMgmtMovementBase(wxWindow* parent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    long style)
    : wxPanel(parent, id, pos, size, style)
{
    if(!bBitmapLoaded) {
	// We need to initialise the default bitmap handler
	wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
	wxC9ED9XXInitBitmapResources();
	bBitmapLoaded = true;
    }
    this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));

    wxFlexGridSizer* flexGridSizer9618 = new wxFlexGridSizer(3, 1, 0, 0);
    flexGridSizer9618->SetFlexibleDirection(wxBOTH);
    flexGridSizer9618->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer9618->AddGrowableCol(0);
    flexGridSizer9618->AddGrowableRow(0);
    this->SetSizer(flexGridSizer9618);

    wxFlexGridSizer* flexGridSizer9543 = new wxFlexGridSizer(2, 1, 0, 0);
    flexGridSizer9543->SetFlexibleDirection(wxBOTH);
    flexGridSizer9543->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer9543->AddGrowableCol(0);
    flexGridSizer9543->AddGrowableRow(1);

    flexGridSizer9618->Add(flexGridSizer9543, 1, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    wxFlexGridSizer* flexGridSizer9609 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer9609->SetFlexibleDirection(wxBOTH);
    flexGridSizer9609->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer9609->AddGrowableCol(0);
    flexGridSizer9609->AddGrowableRow(0);

    flexGridSizer9543->Add(flexGridSizer9609, 0, wxALL | wxEXPAND, WXC_FROM_DIP(3));

    m_textCtrl9604 = new wxTextCtrl(this, wxID_ANY, wxT("Freehand Movement"), wxDefaultPosition,
        wxDLG_UNIT(this, wxSize(-1, -1)), wxTE_READONLY | wxTE_CENTRE);
    m_textCtrl9604->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    m_textCtrl9604->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));
    wxFont m_textCtrl9604Font(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Segoe UI"));
    m_textCtrl9604->SetFont(m_textCtrl9604Font);
#if wxVERSION_NUMBER >= 3000
    m_textCtrl9604->SetHint(wxT(""));
#endif

    flexGridSizer9609->Add(m_textCtrl9604, 0, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    m_intactiveMovePlaceholder =
        new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_intactiveMovePlaceholder->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));

    flexGridSizer9543->Add(m_intactiveMovePlaceholder, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticLine9622 =
        new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxLI_HORIZONTAL);

    flexGridSizer9618->Add(m_staticLine9622, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxFlexGridSizer* flexGridSizer95431 = new wxFlexGridSizer(4, 1, 0, 0);
    flexGridSizer95431->SetFlexibleDirection(wxBOTH);
    flexGridSizer95431->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer95431->AddGrowableCol(0);

    flexGridSizer9618->Add(flexGridSizer95431, 1, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    m_textCtrl96042 = new wxTextCtrl(this, wxID_ANY, wxT("Exact Movement"), wxDefaultPosition,
        wxDLG_UNIT(this, wxSize(-1, -1)), wxTE_READONLY | wxTE_CENTRE);
    m_textCtrl96042->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    m_textCtrl96042->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));
    wxFont m_textCtrl96042Font(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Segoe UI"));
    m_textCtrl96042->SetFont(m_textCtrl96042Font);
#if wxVERSION_NUMBER >= 3000
    m_textCtrl96042->SetHint(wxT(""));
#endif

    flexGridSizer95431->Add(m_textCtrl96042, 0, wxALL | wxEXPAND, WXC_FROM_DIP(3));

    wxFlexGridSizer* flexGridSizer9600 = new wxFlexGridSizer(2, 3, 0, 0);
    flexGridSizer9600->SetFlexibleDirection(wxBOTH);
    flexGridSizer9600->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer9600->AddGrowableCol(2);

    flexGridSizer95431->Add(flexGridSizer9600, 0, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    m_staticText9624 =
        new wxStaticText(this, wxID_ANY, _("Distance:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    wxFont m_staticText9624Font(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Segoe UI"));
    m_staticText9624->SetFont(m_staticText9624Font);

    flexGridSizer9600->Add(m_staticText9624, 0, wxALL, WXC_FROM_DIP(5));

    m_staticText9611 =
        new wxStaticText(this, wxID_ANY, _("[mm]"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    wxFont m_staticText9611Font(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Segoe UI"));
    m_staticText9611->SetFont(m_staticText9611Font);

    flexGridSizer9600->Add(m_staticText9611, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_moveRelative =
        new wxTextCtrl(this, wxID_ANY, wxT("99.999"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTE_RIGHT);
    wxFont m_moveRelativeFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
    m_moveRelative->SetFont(m_moveRelativeFont);
#if wxVERSION_NUMBER >= 3000
    m_moveRelative->SetHint(wxT(""));
#endif

    flexGridSizer9600->Add(m_moveRelative, 0, wxALL | wxEXPAND, WXC_FROM_DIP(4));
    m_moveRelative->SetMinSize(wxSize(64, 32));

    m_btRelativeUp = new wxBitmapButton(this, wxID_ANY, wxXmlResource::Get()->LoadBitmap(wxT("podiumRelativeUp")),
        wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, 80)), wxBU_AUTODRAW);
    m_btRelativeUp->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));

    flexGridSizer95431->Add(m_btRelativeUp, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));
    m_btRelativeUp->SetMinSize(wxSize(-1, 80));

    m_btRelativeDown1 = new wxBitmapButton(this, wxID_ANY, wxXmlResource::Get()->LoadBitmap(wxT("podiumRelativeDown")),
        wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, 80)), wxBU_AUTODRAW);
    m_btRelativeDown1->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));

    flexGridSizer95431->Add(m_btRelativeDown1, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));
    m_btRelativeDown1->SetMinSize(wxSize(-1, 80));

    m_btRelativeDown = new wxBitmapButton(this, wxID_ANY, wxXmlResource::Get()->LoadBitmap(wxT("podiumRelativeDown")),
        wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, 80)), wxBU_AUTODRAW);
    m_btRelativeDown->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));

    flexGridSizer95431->Add(m_btRelativeDown, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));
    m_btRelativeDown->SetMinSize(wxSize(-1, 80));

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));
    SetName(wxT("CncPodiumMgmtMovementBase"));
    SetMinClientSize(wxSize(300, 400));
    SetSize(wxDLG_UNIT(this, wxSize(-1, -1)));
    if(GetSizer()) {
	GetSizer()->Fit(this);
    }
    // Connect events
    m_moveRelative->Bind(wxEVT_LEFT_DOWN, &CncPodiumMgmtMovementBase::onLeftDownDistance, this);
    m_btRelativeUp->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CncPodiumMgmtMovementBase::onPodiumRelativeUp, this);
    m_btRelativeDown->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CncPodiumMgmtMovementBase::onPodiumRelativeDown, this);
}

CncPodiumMgmtMovementBase::~CncPodiumMgmtMovementBase()
{
    m_moveRelative->Unbind(wxEVT_LEFT_DOWN, &CncPodiumMgmtMovementBase::onLeftDownDistance, this);
    m_btRelativeUp->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &CncPodiumMgmtMovementBase::onPodiumRelativeUp, this);
    m_btRelativeDown->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &CncPodiumMgmtMovementBase::onPodiumRelativeDown, this);
}
