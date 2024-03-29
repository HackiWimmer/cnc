//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: wxCrafterContextObserver.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "wxCrafterContextObserver.h"

// Declare the bitmap loading function
extern void wxCBB59InitBitmapResources();

static bool bBitmapLoaded = false;

CncOpenGLContextObserverBase::CncOpenGLContextObserverBase(wxWindow* parent,
    wxWindowID id,
    const wxString& title,
    const wxPoint& pos,
    const wxSize& size,
    long style)
    : wxFrame(parent, id, title, pos, size, style)
{
    if(!bBitmapLoaded) {
	// We need to initialise the default bitmap handler
	wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
	wxCBB59InitBitmapResources();
	bBitmapLoaded = true;
    }
    // Set icon(s) to the application/dialog
    wxIconBundle app_icons;
    {
	wxBitmap iconBmp = wxXmlResource::Get()->LoadBitmap(wxT("layout-content"));
	wxIcon icn;
	icn.CopyFromBitmap(iconBmp);
	app_icons.AddIcon(icn);
    }
    SetIcons(app_icons);

    wxFlexGridSizer* flexGridSizer8031 = new wxFlexGridSizer(3, 1, 0, 0);
    flexGridSizer8031->SetFlexibleDirection(wxBOTH);
    flexGridSizer8031->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer8031->AddGrowableCol(0);
    flexGridSizer8031->AddGrowableRow(1);
    this->SetSizer(flexGridSizer8031);

    wxFlexGridSizer* flexGridSizer8061 = new wxFlexGridSizer(1, 3, 0, 0);
    flexGridSizer8061->SetFlexibleDirection(wxBOTH);
    flexGridSizer8061->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer8061->AddGrowableCol(0);
    flexGridSizer8061->AddGrowableRow(0);

    flexGridSizer8031->Add(flexGridSizer8061, 0, wxALL | wxEXPAND, WXC_FROM_DIP(1));

    m_infoText =
        new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTE_READONLY);
    m_infoText->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_SCROLLBAR));
    wxFont m_infoTextFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));
    m_infoText->SetFont(m_infoTextFont);
    m_infoText->SetToolTip(_("Notification Hint"));
#if wxVERSION_NUMBER >= 3000
    m_infoText->SetHint(wxT(""));
#endif

    flexGridSizer8061->Add(m_infoText, 0, wxALL | wxEXPAND, WXC_FROM_DIP(1));

    flexGridSizer8061->Add(5, 0, 1, wxALL, WXC_FROM_DIP(0));

    m_bmpHeartbeat = new wxStaticBitmap(this, wxID_ANY, wxXmlResource::Get()->LoadBitmap(wxT("heart-2")),
        wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_bmpHeartbeat->SetToolTip(_("Context Observer Heartbeat\n(Content Update)"));

    flexGridSizer8061->Add(m_bmpHeartbeat, 0, wxALL | wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

    m_notebookContextInformation =
        new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxBK_BOTTOM | wxBK_DEFAULT);
    wxFont m_notebookContextInformationFont(
        10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
    m_notebookContextInformation->SetFont(m_notebookContextInformationFont);
    m_notebookContextInformation->SetName(wxT("m_notebookContextInformation"));
    wxImageList* m_notebookContextInformation_il = new wxImageList(16, 16);
    m_notebookContextInformation->AssignImageList(m_notebookContextInformation_il);

    flexGridSizer8031->Add(m_notebookContextInformation, 0, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    m_panelCurrent = new wxPanel(m_notebookContextInformation, wxID_ANY, wxDefaultPosition,
        wxDLG_UNIT(m_notebookContextInformation, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    int m_panelCurrentImgIndex;
    m_panelCurrentImgIndex = m_notebookContextInformation_il->Add(wxXmlResource::Get()->LoadBitmap(wxT("16-mime-svg")));
    m_notebookContextInformation->AddPage(m_panelCurrent, _("Current"), false, m_panelCurrentImgIndex);

    wxFlexGridSizer* flexGridSizer8039 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer8039->SetFlexibleDirection(wxBOTH);
    flexGridSizer8039->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer8039->AddGrowableCol(0);
    flexGridSizer8039->AddGrowableRow(0);
    m_panelCurrent->SetSizer(flexGridSizer8039);

    m_currentContextListPlaceholder = new wxPanel(
        m_panelCurrent, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panelCurrent, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_currentContextListPlaceholder->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTION));

    flexGridSizer8039->Add(m_currentContextListPlaceholder, 0, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    m_panelRegistered = new wxPanel(m_notebookContextInformation, wxID_ANY, wxDefaultPosition,
        wxDLG_UNIT(m_notebookContextInformation, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    int m_panelRegisteredImgIndex;
    m_panelRegisteredImgIndex = m_notebookContextInformation_il->Add(wxXmlResource::Get()->LoadBitmap(wxT("db")));
    m_notebookContextInformation->AddPage(m_panelRegistered, _("Registered"), false, m_panelRegisteredImgIndex);

    wxFlexGridSizer* flexGridSizer8045 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer8045->SetFlexibleDirection(wxBOTH);
    flexGridSizer8045->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer8045->AddGrowableCol(0);
    flexGridSizer8045->AddGrowableRow(0);
    m_panelRegistered->SetSizer(flexGridSizer8045);

    m_allContextListPlaceholder = new wxPanel(
        m_panelRegistered, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panelRegistered, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_allContextListPlaceholder->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTION));

    flexGridSizer8045->Add(m_allContextListPlaceholder, 0, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    m_panelHistory = new wxPanel(m_notebookContextInformation, wxID_ANY, wxDefaultPosition,
        wxDLG_UNIT(m_notebookContextInformation, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    int m_panelHistoryImgIndex;
    m_panelHistoryImgIndex =
        m_notebookContextInformation_il->Add(wxXmlResource::Get()->LoadBitmap(wxT("16-db-column")));
    m_notebookContextInformation->AddPage(m_panelHistory, _("History"), false, m_panelHistoryImgIndex);

    wxFlexGridSizer* flexGridSizer8058 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer8058->SetFlexibleDirection(wxBOTH);
    flexGridSizer8058->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer8058->AddGrowableCol(0);
    flexGridSizer8058->AddGrowableRow(0);
    m_panelHistory->SetSizer(flexGridSizer8058);

    m_splitter44 = new wxSplitterWindow(
        m_panelHistory, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panelHistory, wxSize(-1, -1)), wxSP_3D);
    m_splitter44->SetSashGravity(0.8);
    m_splitter44->SetMinimumPaneSize(10);

    flexGridSizer8058->Add(m_splitter44, 0, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    m_splitterPage48 = new wxPanel(
        m_splitter44, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitter44, wxSize(-1, -1)), wxTAB_TRAVERSAL);

    wxFlexGridSizer* flexGridSizer54 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer54->SetFlexibleDirection(wxBOTH);
    flexGridSizer54->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer54->AddGrowableCol(0);
    flexGridSizer54->AddGrowableRow(0);
    m_splitterPage48->SetSizer(flexGridSizer54);

    m_historyInfoPlaceholder = new wxPanel(
        m_splitterPage48, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitterPage48, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_historyInfoPlaceholder->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTION));

    flexGridSizer54->Add(m_historyInfoPlaceholder, 0, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    m_splitterPage52 = new wxPanel(
        m_splitter44, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitter44, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_splitter44->SplitHorizontally(m_splitterPage48, m_splitterPage52, 0);

    wxFlexGridSizer* flexGridSizer56 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer56->SetFlexibleDirection(wxBOTH);
    flexGridSizer56->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer56->AddGrowableCol(0);
    flexGridSizer56->AddGrowableRow(0);
    m_splitterPage52->SetSizer(flexGridSizer56);

    m_historyDetailInfo = new wxTextCtrl(m_splitterPage52, wxID_ANY, wxT(""), wxDefaultPosition,
        wxDLG_UNIT(m_splitterPage52, wxSize(-1, -1)), wxTE_READONLY | wxTE_MULTILINE);
    m_historyDetailInfo->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));
    m_historyDetailInfo->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));

    flexGridSizer56->Add(m_historyDetailInfo, 0, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    flexGridSizer8031->Add(0, 3, 1, wxALL, WXC_FROM_DIP(0));

    m_startupTimer = new wxTimer;
    m_startupTimer->Start(500, true);

    m_continuousTimer = new wxTimer;
    m_continuousTimer->Start(1000, false);

#if wxVERSION_NUMBER >= 2900
    if(!wxPersistenceManager::Get().Find(m_notebookContextInformation)) {
	wxPersistenceManager::Get().RegisterAndRestore(m_notebookContextInformation);
    } else {
	wxPersistenceManager::Get().Restore(m_notebookContextInformation);
    }
#endif

    SetName(wxT("CncOpenGLContextObserverBase"));
    SetMinClientSize(wxSize(500, 300));
    SetSize(wxDLG_UNIT(this, wxSize(900, 600)));
    if(GetSizer()) {
	GetSizer()->Fit(this);
    }
    if(GetParent()) {
	CentreOnParent(wxBOTH);
    } else {
	CentreOnScreen(wxBOTH);
    }
    if(!wxPersistenceManager::Get().Find(this)) {
	wxPersistenceManager::Get().RegisterAndRestore(this);
    } else {
	wxPersistenceManager::Get().Restore(this);
    }
    // Connect events
    this->Bind(wxEVT_CLOSE_WINDOW, &CncOpenGLContextObserverBase::onCloseWindow, this);
    m_startupTimer->Bind(wxEVT_TIMER, &CncOpenGLContextObserverBase::onStartupTimer, this);
    m_continuousTimer->Bind(wxEVT_TIMER, &CncOpenGLContextObserverBase::onContinuousTimer, this);
}

CncOpenGLContextObserverBase::~CncOpenGLContextObserverBase()
{
    this->Unbind(wxEVT_CLOSE_WINDOW, &CncOpenGLContextObserverBase::onCloseWindow, this);
    m_startupTimer->Unbind(wxEVT_TIMER, &CncOpenGLContextObserverBase::onStartupTimer, this);
    m_continuousTimer->Unbind(wxEVT_TIMER, &CncOpenGLContextObserverBase::onContinuousTimer, this);

    m_startupTimer->Stop();
    wxDELETE(m_startupTimer);

    m_continuousTimer->Stop();
    wxDELETE(m_continuousTimer);
}
