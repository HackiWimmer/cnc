#include <iostream>
#include <wx/filename.h>
#include <wx/scrolwin.h>
#include "CncSvgControl.h"

BEGIN_EVENT_TABLE(CncSvgViewer, wxSVGCtrl)
	EVT_MOUSE_EVENTS(CncSvgViewer::OnMouse)
END_EVENT_TABLE()

/////////////////////////////////////////////////////////////////////
void CncSvgViewer::init() {
/////////////////////////////////////////////////////////////////////
	SetFitToFrame(true);
}
/////////////////////////////////////////////////////////////////////
void CncSvgViewer::OnMouse(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
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

