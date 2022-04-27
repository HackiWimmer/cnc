//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: wxCrafterFileView.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#ifndef _CNCGUICONTROLLER_CNCCONTROLERGUI_WXCRAFTERFILEVIEW_BASE_CLASSES_H
#define _CNCGUICONTROLLER_CNCCONTROLERGUI_WXCRAFTERFILEVIEW_BASE_CLASSES_H

// clang-format off
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/frame.h>
#include <wx/iconbndl.h>
#include <wx/artprov.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/simplebook.h>
#include <wx/imaglist.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/bmpbuttn.h>
#include <wx/combobox.h>
#include <wx/arrstr.h>
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

class CncFilePreviewWndBase : public wxFrame
{
protected:
    wxTextCtrl* m_previewFileName;
    wxTextCtrl* m_previewFileType;
    wxPanel* m_previewPlaceHolder;

protected:
    virtual void onClose(wxCloseEvent& event)
    {
	event.Skip();
    }

public:
    wxTextCtrl* GetPreviewFileName()
    {
	return m_previewFileName;
    }
    wxTextCtrl* GetPreviewFileType()
    {
	return m_previewFileType;
    }
    wxPanel* GetPreviewPlaceHolder()
    {
	return m_previewPlaceHolder;
    }
    CncFilePreviewWndBase(wxWindow* parent,
        wxWindowID id = wxID_ANY,
        const wxString& title = _("Cnc File Preview"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(500, 300),
        long style = wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT);
    virtual ~CncFilePreviewWndBase();
};

class CncFilePreviewBase : public wxPanel
{
protected:
    wxPanel* m_panel52;
    wxStaticText* m_previewTitle;
    wxSimplebook* m_previewBook;
    wxPanel* m_panel3764;
    wxPanel* m_svgPreviewPlaceholder;
    wxPanel* m_panel3766;
    wxPanel* m_gcodePreviewPlaceholder;
    wxButton* m_3D_Top;
    wxButton* m_3D_Bottom;
    wxButton* m_3D_Front;
    wxButton* m_3D_Rear;
    wxButton* m_3D_Left;
    wxButton* m_3D_Right;
    wxStaticLine* m_staticLine3483;
    wxButton* m_3D_Perspective1;
    wxButton* m_3D_Perspective2;
    wxButton* m_3D_Perspective3;
    wxButton* m_3D_Perspective4;

protected:
    virtual void previewBookChanged(wxNotebookEvent& event)
    {
	event.Skip();
    }
    virtual void previewBookPaint(wxPaintEvent& event)
    {
	event.Skip();
    }
    virtual void showFromTop3D(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void showFromBottom3D(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void showFromFront3D(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void showFromRear3D(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void showFromLeft3D(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void showFromRight3D(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void show3D(wxCommandEvent& event)
    {
	event.Skip();
    }

public:
    wxStaticText* GetPreviewTitle()
    {
	return m_previewTitle;
    }
    wxPanel* GetPanel52()
    {
	return m_panel52;
    }
    wxPanel* GetSvgPreviewPlaceholder()
    {
	return m_svgPreviewPlaceholder;
    }
    wxPanel* GetPanel3764()
    {
	return m_panel3764;
    }
    wxPanel* GetGcodePreviewPlaceholder()
    {
	return m_gcodePreviewPlaceholder;
    }
    wxButton* Get3D_Top()
    {
	return m_3D_Top;
    }
    wxButton* Get3D_Bottom()
    {
	return m_3D_Bottom;
    }
    wxButton* Get3D_Front()
    {
	return m_3D_Front;
    }
    wxButton* Get3D_Rear()
    {
	return m_3D_Rear;
    }
    wxButton* Get3D_Left()
    {
	return m_3D_Left;
    }
    wxButton* Get3D_Right()
    {
	return m_3D_Right;
    }
    wxStaticLine* GetStaticLine3483()
    {
	return m_staticLine3483;
    }
    wxButton* Get3D_Perspective1()
    {
	return m_3D_Perspective1;
    }
    wxButton* Get3D_Perspective2()
    {
	return m_3D_Perspective2;
    }
    wxButton* Get3D_Perspective3()
    {
	return m_3D_Perspective3;
    }
    wxButton* Get3D_Perspective4()
    {
	return m_3D_Perspective4;
    }
    wxPanel* GetPanel3766()
    {
	return m_panel3766;
    }
    wxSimplebook* GetPreviewBook()
    {
	return m_previewBook;
    }
    CncFilePreviewBase(wxWindow* parent,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(500, 300),
        long style = wxTAB_TRAVERSAL);
    virtual ~CncFilePreviewBase();
};

class CncFileViewBase : public wxPanel
{
protected:
    wxPanel* m_fileListPlaceholder;
    wxBitmapButton* m_btDirUp;
    wxBitmapButton* m_btRefresh;
    wxBitmapButton* m_btDefaultPath;
    wxStaticLine* m_staticLine5035;
    wxBitmapButton* m_btNewTemplate;
    wxBitmapButton* m_btOpenTemplate;
    wxComboBox* m_filterExtention;
    wxTextCtrl* m_currentDirectory;

protected:
    virtual void aFolderUp(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void refresh(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void selectDefault(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void selectNewTemplate(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void selectOpenTemplate(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void selectFilter(wxCommandEvent& event)
    {
	event.Skip();
    }

public:
    wxPanel* GetFileListPlaceholder()
    {
	return m_fileListPlaceholder;
    }
    wxBitmapButton* GetBtDirUp()
    {
	return m_btDirUp;
    }
    wxBitmapButton* GetBtRefresh()
    {
	return m_btRefresh;
    }
    wxBitmapButton* GetBtDefaultPath()
    {
	return m_btDefaultPath;
    }
    wxStaticLine* GetStaticLine5035()
    {
	return m_staticLine5035;
    }
    wxBitmapButton* GetBtNewTemplate()
    {
	return m_btNewTemplate;
    }
    wxBitmapButton* GetBtOpenTemplate()
    {
	return m_btOpenTemplate;
    }
    wxComboBox* GetFilterExtention()
    {
	return m_filterExtention;
    }
    wxTextCtrl* GetCurrentDirectory()
    {
	return m_currentDirectory;
    }
    CncFileViewBase(wxWindow* parent,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(-1, -1),
        long style = wxTAB_TRAVERSAL);
    virtual ~CncFileViewBase();
};

#endif
