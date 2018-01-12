
#ifndef _WX_WEBVIEW_DUMMY_H_
#define _WX_WEBVIEW_DUMMY_H_

#include "wx/defs.h"

#include "wx/panel.h"
#include "wx/event.h"
#include "wx/sstream.h"
#include "wx/sharedptr.h"
#include "wx/vector.h"


#ifdef wxUSE_WEBVIEW
	#undef wxUSE_WEBVIEW
#endif
#define wxUSE_WEBVIEW 1

/*
#if defined(__WXOSX__)
    #include "wx/osx/webviewhistoryitem_webkit.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/webviewhistoryitem_webkit.h"
#elif defined(__WXMSW__)
    #include "wx/msw/webviewhistoryitem_ie.h"
#else
    #error "wxWebView not implemented on this platform."
#endif
*/

//class wxFSFile;
//class wxFileSystem;
class wxWebView;

enum wxWebViewZoom
{
    wxWEBVIEW_ZOOM_TINY,
    wxWEBVIEW_ZOOM_SMALL,
    wxWEBVIEW_ZOOM_MEDIUM,
    wxWEBVIEW_ZOOM_LARGE,
    wxWEBVIEW_ZOOM_LARGEST
};

enum wxWebViewZoomType
{
    //Scales entire page, including images
    wxWEBVIEW_ZOOM_TYPE_LAYOUT,
    wxWEBVIEW_ZOOM_TYPE_TEXT
};

enum wxWebViewNavigationError
{
    wxWEBVIEW_NAV_ERR_CONNECTION,
    wxWEBVIEW_NAV_ERR_CERTIFICATE,
    wxWEBVIEW_NAV_ERR_AUTH,
    wxWEBVIEW_NAV_ERR_SECURITY,
    wxWEBVIEW_NAV_ERR_NOT_FOUND,
    wxWEBVIEW_NAV_ERR_REQUEST,
    wxWEBVIEW_NAV_ERR_USER_CANCELLED,
    wxWEBVIEW_NAV_ERR_OTHER
};

enum wxWebViewReloadFlags
{
    //Default, may access cache
    wxWEBVIEW_RELOAD_DEFAULT,
    wxWEBVIEW_RELOAD_NO_CACHE
};

enum wxWebViewFindFlags
{
    wxWEBVIEW_FIND_WRAP =             0x0001,
    wxWEBVIEW_FIND_ENTIRE_WORD =      0x0002,
    wxWEBVIEW_FIND_MATCH_CASE =       0x0004,
    wxWEBVIEW_FIND_HIGHLIGHT_RESULT = 0x0008,
    wxWEBVIEW_FIND_BACKWARDS =        0x0010,
    wxWEBVIEW_FIND_DEFAULT =          0
};


#define wxWebViewBackendDefault  ""


class wxWebView : public wxPanel {
public:
    wxWebView() 
    : wxPanel() {}
    
    wxWebView(wxWindow* parent, wxWindowID id, const wxString& url = "", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize) 
    {}
       
    	
    virtual ~wxWebView() {}


    virtual bool Create(wxWindow* parent,
           wxWindowID id,
           const wxString& url = "",
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = "") {
        	
        	wxPanel::Create(parent, id, pos, size);
        }


    // Factory methods allowing the use of custom factories registered with
    // RegisterFactory
    static wxWebView* New(const wxString& backend = "") {
    	return new wxWebView();
    }
    
    static wxWebView* New(wxWindow* parent,
                          wxWindowID id,
                          const wxString& url = "",
                          const wxPoint& pos = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize,
                          const wxString& backend = "",
                          long style = 0,
                          const wxString& name = "") {
                          	
                          	
		return new wxWebView(parent, id, "", pos, size);
    }

/*
    static void RegisterFactory(const wxString& backend, 
                                wxSharedPtr<wxWebViewFactory> factory);
*/

    // General methods
    virtual void EnableContextMenu(bool enable = true) {}
    virtual wxString GetCurrentTitle() const { return ""; }
    virtual wxString GetCurrentURL() const { return ""; }
    virtual wxString GetPageSource() const { return ""; }
    virtual wxString GetPageText() const { return ""; }
    virtual bool IsBusy() const { return false; }
    virtual bool IsContextMenuEnabled() const { return false; }
    virtual bool IsEditable() const { return false; }
    virtual void LoadURL(const wxString& url) {}
    virtual void Print() {}
    //virtual void RegisterHandler(wxSharedPtr<wxWebViewHandler> handler) {}
    virtual void Reload(wxWebViewReloadFlags flags = wxWEBVIEW_RELOAD_DEFAULT) {}
    virtual void RunScript(const wxString& javascript) {}
    virtual void SetEditable(bool enable = true) {}
    
    void SetPage(const wxString& html, const wxString& baseUrl) {}
    void SetPage(wxInputStream& html, wxString baseUrl) {}
    virtual void Stop() {}

    //History
    virtual bool CanGoBack() const { return false; }
    virtual bool CanGoForward() const { return false; }
    virtual void GoBack() {}
    virtual void GoForward() {}
    virtual void ClearHistory() {}
    virtual void EnableHistory(bool enable = true) {}
    //virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetBackwardHistory() = 0;
    //virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetForwardHistory() = 0;
    //virtual void LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item) = 0;

    //Zoom
    virtual bool CanSetZoomType(wxWebViewZoomType type) const { return false; }
    virtual wxWebViewZoom GetZoom() const { return wxWEBVIEW_ZOOM_TINY; }
    virtual wxWebViewZoomType GetZoomType() const { return wxWEBVIEW_ZOOM_TYPE_LAYOUT; }
    virtual void SetZoom(wxWebViewZoom zoom) {}
    virtual void SetZoomType(wxWebViewZoomType zoomType) {}

    //Selection
    virtual void SelectAll() {}
    virtual bool HasSelection() const { return false; }
    virtual void DeleteSelection() {}
    virtual wxString GetSelectedText() const { return ""; }
    virtual wxString GetSelectedSource() const { return ""; }
    virtual void ClearSelection() {}

    //Clipboard functions
    virtual bool CanCut() const { return false; }
    virtual bool CanCopy() const { return false; }
    virtual bool CanPaste() const { return false; }
    virtual void Cut() {}
    virtual void Copy() {}
    virtual void Paste() {}

    //Undo / redo functionality
    virtual bool CanUndo() const { return false; }
    virtual bool CanRedo() const { return false; }
    virtual void Undo() {};
    virtual void Redo() {};

    //Get the pointer to the underlying native engine.
    virtual void* GetNativeBackend() const { return NULL; }
    //Find function
    virtual long Find(const wxString& text, int flags = wxWEBVIEW_FIND_DEFAULT) { return 0L; }
};

#endif // _WX_WEBVIEW_DUMMY_H_
