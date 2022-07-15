#ifndef CNC_SVG_CONTROL_H
#define CNC_SVG_CONTROL_H

#include <wxSVG/svgctrl.h>
#include <wxSVG/SVGDocument.h>
#include <wxSVG/SVGCanvas.h>
#include <CncWxSvgVersionInfo.h>
#include "CncPosition.h"

//////////////////////////////////////////////////////////////////////////////
class CncSvgViewer : public wxSVGCtrl {
	
	public:
		CncSvgViewer(wxWindow* parent) 
		: wxSVGCtrl					(parent)
		, boundaries				()
		, canSize					(true)
		, mouseMoveMode				(false)
		, mouseMoveX				(0) 
		, mouseMoveY				(0)
		{ 
			Init();
		}
		
		virtual ~CncSvgViewer() 
		{}
		
		void setContentSizable(bool state) { canSize = state; }
		bool loadFile(const wxString& filename, const char* contextInfo = NULL);
		bool getMetricBoundaries(CncDoubleBoundaries& box);
		
		virtual void onMouse(wxMouseEvent& event);

		// It isn't better available currently
		static const char* versionInfo() { return CNC_WX_SVG_VERSIONFO; }
		
	protected:
		virtual void Init();
		//virtual void OnPaint(wxPaintEvent& event);
		//virtual void OnResize(wxSizeEvent& event);
		//virtual void OnEraseBackground(wxEraseEvent &event);
		//virtual void RepaintBuffer();
	
	private:
		
		CncDoubleBoundaries		boundaries;
		bool					canSize;
		
		bool 					mouseMoveMode;
		int						mouseMoveX;
		int						mouseMoveY;

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