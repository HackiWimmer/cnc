#include <iostream>
#include <wx/filename.h>
#include <wx/scrolwin.h>
#include <wx/dcclient.h>
#include "CncSvgControl.h"

BEGIN_EVENT_TABLE(CncSvgViewer, wxSVGCtrl)
	EVT_MOUSE_EVENTS(CncSvgViewer::OnMouse)
END_EVENT_TABLE()

//----------------------------------------------------------------------
#ifdef WX_SVG_SUPPORT

	/////////////////////////////////////////////////////////////////////
	void CncSvgViewer::Init() {
	/////////////////////////////////////////////////////////////////////
		wxSVGCtrl::Init();
	}
	/////////////////////////////////////////////////////////////////////
	void CncSvgViewer::OnPaint(wxPaintEvent& event) {
	/////////////////////////////////////////////////////////////////////
		// this overrides the default behaviour to get access to adjust the background colour
		if (!m_doc)				m_buffer = wxBitmap();
		else if (m_repaint)		RepaintBuffer();
		
		wxPaintDC dc(this);
		
		int w = GetClientSize().GetWidth();
		int h = GetClientSize().GetHeight();
		dc.SetPen(wxPen(GetBackgroundColour()));
		dc.SetBrush(wxBrush(GetBackgroundColour()));
		dc.DrawRectangle(m_buffer.GetWidth(), 0, w - m_buffer.GetWidth(), h);
		dc.DrawRectangle(0, m_buffer.GetHeight(), m_buffer.GetWidth(), h - m_buffer.GetHeight());
		
		if (m_buffer.IsOk())
			dc.DrawBitmap(m_buffer, 0, 0);
	}
	/////////////////////////////////////////////////////////////////////
	void CncSvgViewer::OnResize(wxSizeEvent& event) {
	/////////////////////////////////////////////////////////////////////
		wxSVGCtrl::OnResize(event);
	}
	/////////////////////////////////////////////////////////////////////
	void CncSvgViewer::OnEraseBackground(wxEraseEvent &event) {
	/////////////////////////////////////////////////////////////////////
		// this overrides the default behaviour to get access to adjust the background colour
		event.Skip(true);
	}
	/////////////////////////////////////////////////////////////////////
	void CncSvgViewer::RepaintBuffer() {
	/////////////////////////////////////////////////////////////////////
		wxSVGCtrl::RepaintBuffer();
	}
	
#endif
//----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
void CncSvgViewer::init() {
/////////////////////////////////////////////////////////////////////
	SetFitToFrame(true);
}
/////////////////////////////////////////////////////////////////////
void CncSvgViewer::OnMouse(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( canSize == false )
		return;
		
	if ( event.GetWheelRotation() != 0 ) {
		wxSize clientSize = GetClientSize();
		
		const int rot = event.GetWheelRotation();
		
		if ( rot != 0 ) {
			rot < 0 ? clientSize.Scale(0.9, 0.9) : clientSize.Scale(1.1, 1.1);
			
			SetClientSize(clientSize);
			
			if ( GetParent() && GetParent()->IsKindOf(wxCLASSINFO(wxScrolledWindow)) )
				GetParent()->SetVirtualSize(clientSize);
			
			Refresh();
		}
	}
}
/////////////////////////////////////////////////////////////////////
void CncSvgViewer::clear() {
/////////////////////////////////////////////////////////////////////
	#ifdef WX_SVG_SUPPORT
		 Clear();
	#else
		#ifdef WX_WEBVIEW_SUPPORT
			LoadURL("about:blank");
		#else
			SetToolTip(""));
		#endif
	#endif
}
//////////////////////////////////////////////////////////////////////////////
bool CncSvgViewer::loadFile(const wxString& filename) {
//////////////////////////////////////////////////////////////////////////////
	wxFileName svgFile(filename);
	if ( svgFile.Exists() == false )
		return false;
		
	#ifdef WX_SVG_SUPPORT
		return Load(filename);
	#else
		#ifdef WX_WEBVIEW_SUPPORT
			return LoadURL(filename);
		#else
			SetToolTip(wxString::Format("wxPanel: Can't display %s", filename));
			return false;
		#endif
	#endif
}

