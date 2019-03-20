#include <wx/filename.h>
#include "CncSvgControl.h"

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