//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: wxCrafterContextObserver.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#ifndef _CNCGUICONTROLLER_CNCCONTROLERGUI_WXCRAFTERCONTEXTOBSERVER_BASE_CLASSES_H
#define _CNCGUICONTROLLER_CNCCONTROLERGUI_WXCRAFTERCONTEXTOBSERVER_BASE_CLASSES_H

// clang-format off
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/frame.h>
#include <wx/iconbndl.h>
#include <wx/artprov.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/statbmp.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/imaglist.h>
#include <wx/splitter.h>
#include <wx/timer.h>
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

class CncOpenGLContextObserverBase : public wxFrame
{
protected:
    wxTextCtrl* m_infoText;
    wxStaticBitmap* m_bmpHeartbeat;
    wxNotebook* m_notebookContextInformation;
    wxPanel* m_panelCurrent;
    wxPanel* m_currentContextListPlaceholder;
    wxPanel* m_panelRegistered;
    wxPanel* m_allContextListPlaceholder;
    wxPanel* m_panelHistory;
    wxSplitterWindow* m_splitter44;
    wxPanel* m_splitterPage48;
    wxPanel* m_historyInfoPlaceholder;
    wxPanel* m_splitterPage52;
    wxTextCtrl* m_historyDetailInfo;
    wxTimer* m_startupTimer;
    wxTimer* m_continuousTimer;

protected:
    virtual void onCloseWindow(wxCloseEvent& event)
    {
	event.Skip();
    }
    virtual void onStartupTimer(wxTimerEvent& event)
    {
	event.Skip();
    }
    virtual void onContinuousTimer(wxTimerEvent& event)
    {
	event.Skip();
    }

public:
    wxTextCtrl* GetInfoText()
    {
	return m_infoText;
    }
    wxStaticBitmap* GetBmpHeartbeat()
    {
	return m_bmpHeartbeat;
    }
    wxPanel* GetCurrentContextListPlaceholder()
    {
	return m_currentContextListPlaceholder;
    }
    wxPanel* GetPanelCurrent()
    {
	return m_panelCurrent;
    }
    wxPanel* GetAllContextListPlaceholder()
    {
	return m_allContextListPlaceholder;
    }
    wxPanel* GetPanelRegistered()
    {
	return m_panelRegistered;
    }
    wxPanel* GetHistoryInfoPlaceholder()
    {
	return m_historyInfoPlaceholder;
    }
    wxPanel* GetSplitterPage48()
    {
	return m_splitterPage48;
    }
    wxTextCtrl* GetHistoryDetailInfo()
    {
	return m_historyDetailInfo;
    }
    wxPanel* GetSplitterPage52()
    {
	return m_splitterPage52;
    }
    wxSplitterWindow* GetSplitter44()
    {
	return m_splitter44;
    }
    wxPanel* GetPanelHistory()
    {
	return m_panelHistory;
    }
    wxNotebook* GetNotebookContextInformation()
    {
	return m_notebookContextInformation;
    }
    wxTimer* GetStartupTimer()
    {
	return m_startupTimer;
    }
    wxTimer* GetContinuousTimer()
    {
	return m_continuousTimer;
    }
    CncOpenGLContextObserverBase(wxWindow* parent,
        wxWindowID id = wxID_ANY,
        const wxString& title = _("OpenGL Context Observer . . ."),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(900, 600),
        long style = wxFRAME_FLOAT_ON_PARENT | wxCAPTION | wxRESIZE_BORDER | wxMAXIMIZE_BOX | wxMINIMIZE_BOX |
            wxCLOSE_BOX | wxSTAY_ON_TOP);
    virtual ~CncOpenGLContextObserverBase();
};

#endif
