#ifndef CNC_SVG_CONTROL_H
#define CNC_SVG_CONTROL_H

#define WX_SVG_SUPPORT
#define WX_WEBVIEW_SUPPORT


	#ifdef WX_SVG_SUPPORT
		#include <wxSVG/svgctrl.h>
		#define SVG_VIEWER_CLASS_PARENT 		wxSVGCtrl
		#define SVG_BUILDER_CLASS_PARENT 		wxSVGCtrl
	#else
		#ifdef WX_WEBVIEW_SUPPORT
			#include <wx/webview.h>
			#define SVG_VIEWER_CLASS_PARENT 	wxWebView
		#esle
			#define SVG_VIEWER_CLASS_PARENT		wxPanel
		#endif
		
		#define SVG_BUILDER_CLASS_PARENT		wxPanel
	#endif

//////////////////////////////////////////////////////////////////////////////
class CncSvgViewer : public SVG_VIEWER_CLASS_PARENT {
	
	public:
		CncSvgViewer(wxWindow* parent) 
		: SVG_VIEWER_CLASS_PARENT(parent)
		{}
		
		~CncSvgViewer() {
		}
		
		bool loadFile(const wxString& filename);
};

//////////////////////////////////////////////////////////////////////////////
class CncSvgBuilder : public SVG_BUILDER_CLASS_PARENT {
	
	public:
		CncSvgBuilder(wxWindow* parent) 
		: SVG_BUILDER_CLASS_PARENT(parent)
		{
		}
		
		~CncSvgBuilder() {
		}
		
		// todo
};

#endif