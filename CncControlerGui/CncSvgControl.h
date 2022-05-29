#ifndef CNC_SVG_CONTROL_H
#define CNC_SVG_CONTROL_H

#define WX_SVG_SUPPORT
#define WX_WEBVIEW_SUPPORT_XXXXX

#ifdef WX_SVG_SUPPORT
	#include <wxSVG/svgctrl.h>
	#include <wxSVG/SVGDocument.h>
	#include <wxSVG/SVGCanvas.h>
	#include <CncWxSvgVersionInfo.h>
	#define CNC_SVG_VERSIONFO 				CNC_WX_SVG_VERSIONFO
	#define SVG_VIEWER_CLASS_PARENT 		wxSVGCtrl
#else
	#define CNC_SVG_VERSIONFO				"<wxPanel>"
	#define SVG_VIEWER_CLASS_PARENT			wxPanel
#endif

#include "CncPosition.h"

//////////////////////////////////////////////////////////////////////////////
class CncSvgViewer : public SVG_VIEWER_CLASS_PARENT {
	
	public:
		CncSvgViewer(wxWindow* parent) 
		: SVG_VIEWER_CLASS_PARENT	(parent)
		, boundaries				()
		, canSize					(true)
		{ 
			init();
		}
		
		~CncSvgViewer() 
		{}
		
		void setContentSizable(bool state) { canSize = state; }
		
		void clear();
		void update();
		bool loadFile(const wxString& filename, const char* contextInfo = NULL);
		
		bool getMetricBoundaries(CncDoubleBoundaries& box);
		
		// It isn't better available currently
		static const char* versionInfo() { return CNC_SVG_VERSIONFO; }
		
	protected:
		
		void OnMouse(wxMouseEvent& event);
		
	#ifdef WX_SVG_SUPPORT
		virtual void Init();
		virtual void OnPaint(wxPaintEvent& event);
		virtual void OnResize(wxSizeEvent& event);
		virtual void OnEraseBackground(wxEraseEvent &event);
		virtual void RepaintBuffer();
	#endif
	
	private:
		
		CncDoubleBoundaries	boundaries;
		bool				canSize;
		
		void init();
		
	wxDECLARE_EVENT_TABLE();
};

//////////////////////////////////////////////////////////////////////////////
class CncSvgBuilder : public CncSvgViewer {
	
	public:
		CncSvgBuilder(wxWindow* parent) 
		: CncSvgViewer(parent)
		{
			wxASSERT(IsKindOf(wxCLASSINFO(wxSVGCtrl)));
		}
		
		~CncSvgBuilder() {
		}
};

#endif