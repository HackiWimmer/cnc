//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: wxCrafterLCDPanel.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "wxCrafterLCDPanel.h"

// Declare the bitmap loading function
extern void wxCDC1CInitBitmapResources();

static bool bBitmapLoaded = false;

CncLCDPositionPanelBase::CncLCDPositionPanelBase(wxWindow* parent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    long style)
    : wxPanel(parent, id, pos, size, style)
{
    if(!bBitmapLoaded) {
	// We need to initialise the default bitmap handler
	wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
	wxCDC1CInitBitmapResources();
	bBitmapLoaded = true;
    }
    this->SetBackgroundColour(wxColour(wxT("rgb(64,64,64)")));

    wxFlexGridSizer* flexGridSizer3 = new wxFlexGridSizer(6, 1, 0, 0);
    flexGridSizer3->SetFlexibleDirection(wxBOTH);
    flexGridSizer3->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer3->AddGrowableCol(0);
    flexGridSizer3->AddGrowableRow(0);
    flexGridSizer3->AddGrowableRow(1);
    flexGridSizer3->AddGrowableRow(2);
    flexGridSizer3->AddGrowableRow(3);
    flexGridSizer3->AddGrowableRow(4);
    this->SetSizer(flexGridSizer3);

    wxFlexGridSizer* flexGridSizerF = new wxFlexGridSizer(1, 3, 0, 0);
    flexGridSizerF->SetFlexibleDirection(wxBOTH);
    flexGridSizerF->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizerF->AddGrowableCol(1);
    flexGridSizerF->AddGrowableRow(0);

    flexGridSizer3->Add(flexGridSizerF, 1, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    m_panel58 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_panel58->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));

    flexGridSizerF->Add(m_panel58, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));

    wxFlexGridSizer* flexGridSizer60 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer60->SetFlexibleDirection(wxBOTH);
    flexGridSizer60->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer60->AddGrowableCol(0);
    flexGridSizer60->AddGrowableRow(0);
    m_panel58->SetSizer(flexGridSizer60);

    m_staticText7 = new wxStaticText(
        m_panel58, wxID_ANY, _("F"), wxDefaultPosition, wxDLG_UNIT(m_panel58, wxSize(30, 40)), wxALIGN_CENTRE);
    m_staticText7->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));
    wxFont m_staticText7Font(26, wxFONTFAMILY_SWISS, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
    m_staticText7->SetFont(m_staticText7Font);

    flexGridSizer60->Add(m_staticText7, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(2));
    m_staticText7->SetMinSize(wxSize(30, 40));

    m_lcdPlaceholderF =
        new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_lcdPlaceholderF->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));

    flexGridSizerF->Add(m_lcdPlaceholderF, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));

    m_panel70 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_panel70->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    m_panel70->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));

    flexGridSizerF->Add(m_panel70, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));

    wxFlexGridSizer* flexGridSizer27 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer27->SetFlexibleDirection(wxBOTH);
    flexGridSizer27->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer27->AddGrowableCol(0);
    flexGridSizer27->AddGrowableRow(0);
    m_panel70->SetSizer(flexGridSizer27);

    m_unitW = new wxStaticText(
        m_panel70, wxID_ANY, _("mm/\nmin"), wxDefaultPosition, wxDLG_UNIT(m_panel70, wxSize(38, -1)), wxALIGN_CENTRE);
    wxFont m_unitWFont(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
    m_unitW->SetFont(m_unitWFont);

    flexGridSizer27->Add(
        m_unitW, 0, wxLEFT | wxRIGHT | wxTOP | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(0));
    m_unitW->SetMinSize(wxSize(38, -1));

    wxFlexGridSizer* flexGridSizerS = new wxFlexGridSizer(1, 3, 0, 0);
    flexGridSizerS->SetFlexibleDirection(wxBOTH);
    flexGridSizerS->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizerS->AddGrowableCol(1);
    flexGridSizerS->AddGrowableRow(0);

    flexGridSizer3->Add(flexGridSizerS, 1, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    m_panel582 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_panel582->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));

    flexGridSizerS->Add(m_panel582, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));

    wxFlexGridSizer* flexGridSizer603 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer603->SetFlexibleDirection(wxBOTH);
    flexGridSizer603->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer603->AddGrowableCol(0);
    flexGridSizer603->AddGrowableRow(0);
    m_panel582->SetSizer(flexGridSizer603);

    m_staticText74 = new wxStaticText(
        m_panel582, wxID_ANY, _("S"), wxDefaultPosition, wxDLG_UNIT(m_panel582, wxSize(30, 40)), wxALIGN_CENTRE);
    m_staticText74->SetForegroundColour(wxColour(wxT("rgb(255,255,132)")));
    wxFont m_staticText74Font(26, wxFONTFAMILY_SWISS, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
    m_staticText74->SetFont(m_staticText74Font);

    flexGridSizer603->Add(m_staticText74, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(2));
    m_staticText74->SetMinSize(wxSize(30, 40));

    m_lcdPlaceholderS =
        new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_lcdPlaceholderS->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));

    flexGridSizerS->Add(m_lcdPlaceholderS, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));

    m_panel706 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_panel706->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    m_panel706->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));

    flexGridSizerS->Add(m_panel706, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));

    wxFlexGridSizer* flexGridSizer277 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer277->SetFlexibleDirection(wxBOTH);
    flexGridSizer277->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer277->AddGrowableCol(0);
    flexGridSizer277->AddGrowableRow(0);
    m_panel706->SetSizer(flexGridSizer277);

    m_unitW8 = new wxStaticText(
        m_panel706, wxID_ANY, _("U/\nmin"), wxDefaultPosition, wxDLG_UNIT(m_panel706, wxSize(38, -1)), wxALIGN_CENTRE);
    m_unitW8->SetForegroundColour(wxColour(wxT("rgb(255,255,132)")));
    wxFont m_unitW8Font(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
    m_unitW8->SetFont(m_unitW8Font);

    flexGridSizer277->Add(
        m_unitW8, 0, wxLEFT | wxRIGHT | wxTOP | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(0));
    m_unitW8->SetMinSize(wxSize(38, -1));

    wxFlexGridSizer* flexGridSizerX = new wxFlexGridSizer(0, 3, 0, 0);
    flexGridSizerX->SetFlexibleDirection(wxBOTH);
    flexGridSizerX->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizerX->AddGrowableCol(1);
    flexGridSizerX->AddGrowableRow(0);

    flexGridSizer3->Add(flexGridSizerX, 1, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    m_panel54 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_panel54->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));

    flexGridSizerX->Add(m_panel54, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));

    wxFlexGridSizer* flexGridSizer56 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer56->SetFlexibleDirection(wxBOTH);
    flexGridSizer56->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer56->AddGrowableCol(0);
    flexGridSizer56->AddGrowableRow(0);
    m_panel54->SetSizer(flexGridSizer56);

    m_staticText72 = new wxStaticText(
        m_panel54, wxID_ANY, _("X"), wxDefaultPosition, wxDLG_UNIT(m_panel54, wxSize(30, 40)), wxALIGN_CENTRE);
    m_staticText72->SetForegroundColour(wxColour(wxT("rgb(255,128,128)")));
    wxFont m_staticText72Font(26, wxFONTFAMILY_SWISS, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
    m_staticText72->SetFont(m_staticText72Font);

    flexGridSizer56->Add(
        m_staticText72, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(2));
    m_staticText72->SetMinSize(wxSize(30, 40));

    m_lcdPlaceholderX =
        new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_lcdPlaceholderX->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));

    flexGridSizerX->Add(m_lcdPlaceholderX, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));

    m_panel72 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_panel72->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    m_panel72->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));

    flexGridSizerX->Add(m_panel72, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));

    wxFlexGridSizer* flexGridSizer38 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer38->SetFlexibleDirection(wxBOTH);
    flexGridSizer38->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer38->AddGrowableCol(0);
    flexGridSizer38->AddGrowableRow(0);
    m_panel72->SetSizer(flexGridSizer38);

    m_unitX = new wxStaticText(
        m_panel72, wxID_ANY, _("steps"), wxDefaultPosition, wxDLG_UNIT(m_panel72, wxSize(38, -1)), wxALIGN_CENTRE);
    m_unitX->SetForegroundColour(wxColour(wxT("rgb(255,128,128)")));
    wxFont m_unitXFont(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
    m_unitX->SetFont(m_unitXFont);

    flexGridSizer38->Add(m_unitX, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(0));
    m_unitX->SetMinSize(wxSize(38, -1));

    wxFlexGridSizer* flexGridSizerY = new wxFlexGridSizer(1, 3, 0, 0);
    flexGridSizerY->SetFlexibleDirection(wxBOTH);
    flexGridSizerY->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizerY->AddGrowableCol(1);
    flexGridSizerY->AddGrowableRow(0);

    flexGridSizer3->Add(flexGridSizerY, 1, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    m_panel62 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_panel62->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));

    flexGridSizerY->Add(m_panel62, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));

    wxFlexGridSizer* flexGridSizer64 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer64->SetFlexibleDirection(wxBOTH);
    flexGridSizer64->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer64->AddGrowableCol(0);
    flexGridSizer64->AddGrowableRow(0);
    m_panel62->SetSizer(flexGridSizer64);

    m_staticText725 = new wxStaticText(
        m_panel62, wxID_ANY, _("Y"), wxDefaultPosition, wxDLG_UNIT(m_panel62, wxSize(30, 40)), wxALIGN_CENTRE);
    m_staticText725->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    wxFont m_staticText725Font(26, wxFONTFAMILY_SWISS, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
    m_staticText725->SetFont(m_staticText725Font);

    flexGridSizer64->Add(
        m_staticText725, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(2));
    m_staticText725->SetMinSize(wxSize(30, 40));

    m_lcdPlaceholderY =
        new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_lcdPlaceholderY->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));

    flexGridSizerY->Add(m_lcdPlaceholderY, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));

    m_panel74 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_panel74->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    m_panel74->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));

    flexGridSizerY->Add(m_panel74, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));

    wxFlexGridSizer* flexGridSizer35 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer35->SetFlexibleDirection(wxBOTH);
    flexGridSizer35->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer35->AddGrowableCol(0);
    flexGridSizer35->AddGrowableRow(0);
    m_panel74->SetSizer(flexGridSizer35);

    m_unitY = new wxStaticText(
        m_panel74, wxID_ANY, _("mm"), wxDefaultPosition, wxDLG_UNIT(m_panel74, wxSize(38, -1)), wxALIGN_CENTRE);
    m_unitY->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    wxFont m_unitYFont(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
    m_unitY->SetFont(m_unitYFont);

    flexGridSizer35->Add(m_unitY, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(0));
    m_unitY->SetMinSize(wxSize(38, -1));

    wxFlexGridSizer* flexGridSizerZ = new wxFlexGridSizer(1, 3, 0, 0);
    flexGridSizerZ->SetFlexibleDirection(wxBOTH);
    flexGridSizerZ->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizerZ->AddGrowableCol(1);
    flexGridSizerZ->AddGrowableRow(0);

    flexGridSizer3->Add(flexGridSizerZ, 1, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    m_panel66 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_panel66->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));

    flexGridSizerZ->Add(m_panel66, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(2));

    wxFlexGridSizer* flexGridSizer68 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer68->SetFlexibleDirection(wxBOTH);
    flexGridSizer68->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer68->AddGrowableCol(0);
    flexGridSizer68->AddGrowableRow(0);
    m_panel66->SetSizer(flexGridSizer68);

    m_staticText7258 = new wxStaticText(
        m_panel66, wxID_ANY, _("Z"), wxDefaultPosition, wxDLG_UNIT(m_panel66, wxSize(30, 40)), wxALIGN_CENTRE);
    m_staticText7258->SetForegroundColour(wxColour(wxT("rgb(0,128,64)")));
    wxFont m_staticText7258Font(
        26, wxFONTFAMILY_SWISS, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
    m_staticText7258->SetFont(m_staticText7258Font);

    flexGridSizer68->Add(
        m_staticText7258, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(2));
    m_staticText7258->SetMinSize(wxSize(30, 40));

    m_lcdPlaceholderZ =
        new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_lcdPlaceholderZ->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));

    flexGridSizerZ->Add(m_lcdPlaceholderZ, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));

    m_panel76 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_panel76->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    m_panel76->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));

    flexGridSizerZ->Add(m_panel76, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));

    wxFlexGridSizer* flexGridSizer32 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer32->SetFlexibleDirection(wxBOTH);
    flexGridSizer32->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer32->AddGrowableCol(0);
    flexGridSizer32->AddGrowableRow(0);
    m_panel76->SetSizer(flexGridSizer32);

    m_unitZ = new wxStaticText(
        m_panel76, wxID_ANY, _("mm"), wxDefaultPosition, wxDLG_UNIT(m_panel76, wxSize(38, -1)), wxALIGN_CENTRE);
    m_unitZ->SetForegroundColour(wxColour(wxT("rgb(0,128,64)")));
    wxFont m_unitZFont(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
    m_unitZ->SetFont(m_unitZFont);

    flexGridSizer32->Add(m_unitZ, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(0));
    m_unitZ->SetMinSize(wxSize(38, -1));

    wxArrayString m_cbPosTYpeArr;
    m_cbPosTYpeArr.Add(_("Physical hardware positions"));
    m_cbPosTYpeArr.Add(_("Logical CNC positions"));
    m_cbPosTYpe = new wxComboBox(
        this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), m_cbPosTYpeArr, wxCB_READONLY);
    m_cbPosTYpe->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    m_cbPosTYpe->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    wxFont m_cbPosTYpeFont(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Segoe UI"));
    m_cbPosTYpe->SetFont(m_cbPosTYpeFont);
    m_cbPosTYpe->SetSelection(1);

    flexGridSizer3->Add(m_cbPosTYpe, 0, wxALL | wxEXPAND, WXC_FROM_DIP(1));

    m_updateTimer = new wxTimer;
    m_updateTimer->Start(300, false);

    m_startupTimer = new wxTimer;
    m_startupTimer->Start(300, true);

    SetBackgroundColour(wxColour(wxT("rgb(64,64,64)")));
    SetName(wxT("CncLCDPositionPanelBase"));
    SetMinClientSize(wxSize(500, 280));
    SetSize(wxDLG_UNIT(this, wxSize(500, 280)));
    if(GetSizer()) {
	GetSizer()->Fit(this);
    }
    // Connect events
    this->Bind(wxEVT_PAINT, &CncLCDPositionPanelBase::onPaint, this);
    this->Bind(wxEVT_SIZE, &CncLCDPositionPanelBase::onSize, this);
    m_cbPosTYpe->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CncLCDPositionPanelBase::onChangePositionType, this);
    m_updateTimer->Bind(wxEVT_TIMER, &CncLCDPositionPanelBase::onUpdateTimer, this);
    m_startupTimer->Bind(wxEVT_TIMER, &CncLCDPositionPanelBase::onStartupTimer, this);
}

CncLCDPositionPanelBase::~CncLCDPositionPanelBase()
{
    this->Unbind(wxEVT_PAINT, &CncLCDPositionPanelBase::onPaint, this);
    this->Unbind(wxEVT_SIZE, &CncLCDPositionPanelBase::onSize, this);
    m_cbPosTYpe->Unbind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CncLCDPositionPanelBase::onChangePositionType, this);
    m_updateTimer->Unbind(wxEVT_TIMER, &CncLCDPositionPanelBase::onUpdateTimer, this);
    m_startupTimer->Unbind(wxEVT_TIMER, &CncLCDPositionPanelBase::onStartupTimer, this);

    m_updateTimer->Stop();
    wxDELETE(m_updateTimer);

    m_startupTimer->Stop();
    wxDELETE(m_startupTimer);
}
