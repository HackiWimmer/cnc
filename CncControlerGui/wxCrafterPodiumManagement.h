//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: wxCrafterPodiumManagement.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#ifndef _CNCGUICONTROLLER_CNCCONTROLERGUI_WXCRAFTERPODIUMMANAGEMENT_BASE_CLASSES_H
#define _CNCGUICONTROLLER_CNCCONTROLERGUI_WXCRAFTERPODIUMMANAGEMENT_BASE_CLASSES_H

// clang-format off
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/dialog.h>
#include <wx/iconbndl.h>
#include <wx/artprov.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/statbmp.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/bmpbuttn.h>
#if wxVERSION_NUMBER >= 2900
#include <wx/persist.h>
#include <wx/persist/toplevel.h>
#include <wx/persist/bookctrl.h>
#include <wx/persist/treebook.h>
#endif

#ifdef WXC_FROM_DIP
#undef WXC_FROM_DIP
#endif
#if wxVERSION_NUMBER >= 3100
#define WXC_FROM_DIP(x) wxWindow::FromDIP(x, NULL)
#else
#define WXC_FROM_DIP(x) x
#endif

// clang-format on

class CncPodiumManagementBase : public wxDialog
{
protected:
    wxPanel* m_basePanel;
    wxPanel* m_panelPicture;
    wxStaticBitmap* m_staticBitmap9569;
    wxStaticLine* m_staticLine9571;
    wxPanel* m_panelMovementPlaceholder;
    wxStaticLine* m_staticLine9554;
    wxStaticBitmap* m_info;
    wxButton* m_btClose;

protected:
    virtual void onShow(wxShowEvent& event)
    {
	event.Skip();
    }
    virtual void onInit(wxInitDialogEvent& event)
    {
	event.Skip();
    }
    virtual void onLefDownInfo(wxMouseEvent& event)
    {
	event.Skip();
    }
    virtual void onClose(wxCommandEvent& event)
    {
	event.Skip();
    }

public:
    wxStaticBitmap* GetStaticBitmap9569()
    {
	return m_staticBitmap9569;
    }
    wxPanel* GetPanelPicture()
    {
	return m_panelPicture;
    }
    wxStaticLine* GetStaticLine9571()
    {
	return m_staticLine9571;
    }
    wxPanel* GetPanelMovementPlaceholder()
    {
	return m_panelMovementPlaceholder;
    }
    wxStaticLine* GetStaticLine9554()
    {
	return m_staticLine9554;
    }
    wxStaticBitmap* GetInfo()
    {
	return m_info;
    }
    wxButton* GetBtClose()
    {
	return m_btClose;
    }
    wxPanel* GetBasePanel()
    {
	return m_basePanel;
    }
    CncPodiumManagementBase(wxWindow* parent,
        wxWindowID id = wxID_ANY,
        const wxString& title = _("Cnc Podium Management"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(-1, -1),
        long style = wxDEFAULT_DIALOG_STYLE);
    virtual ~CncPodiumManagementBase();
};

class CncPodiumMgmtMovementBase : public wxPanel
{
protected:
    wxTextCtrl* m_textCtrl9604;
    wxPanel* m_intactiveMovePlaceholder;
    wxStaticLine* m_staticLine9622;
    wxTextCtrl* m_textCtrl96042;
    wxStaticText* m_staticText9624;
    wxStaticText* m_staticText9611;
    wxTextCtrl* m_moveRelative;
    wxBitmapButton* m_btRelativeUp;
    wxBitmapButton* m_btRelativeDown1;
    wxBitmapButton* m_btRelativeDown;

protected:
    virtual void onLeftDownDistance(wxMouseEvent& event)
    {
	event.Skip();
    }
    virtual void onPodiumRelativeUp(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void onPodiumRelativeDown(wxCommandEvent& event)
    {
	event.Skip();
    }

public:
    wxTextCtrl* GetTextCtrl9604()
    {
	return m_textCtrl9604;
    }
    wxPanel* GetIntactiveMovePlaceholder()
    {
	return m_intactiveMovePlaceholder;
    }
    wxStaticLine* GetStaticLine9622()
    {
	return m_staticLine9622;
    }
    wxTextCtrl* GetTextCtrl96042()
    {
	return m_textCtrl96042;
    }
    wxStaticText* GetStaticText9624()
    {
	return m_staticText9624;
    }
    wxStaticText* GetStaticText9611()
    {
	return m_staticText9611;
    }
    wxTextCtrl* GetMoveRelative()
    {
	return m_moveRelative;
    }
    wxBitmapButton* GetBtRelativeUp()
    {
	return m_btRelativeUp;
    }
    wxBitmapButton* GetBtRelativeDown1()
    {
	return m_btRelativeDown1;
    }
    wxBitmapButton* GetBtRelativeDown()
    {
	return m_btRelativeDown;
    }
    CncPodiumMgmtMovementBase(wxWindow* parent,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(-1, -1),
        long style = wxTAB_TRAVERSAL);
    virtual ~CncPodiumMgmtMovementBase();
};

#endif
