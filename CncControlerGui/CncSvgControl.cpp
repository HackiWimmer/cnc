#include <iostream>
#include <wx/log.h>
#include <wx/scrolwin.h>
#include <wx/dcclient.h>
#include <wx/filename.h>
#include <wx/dcmemory.h>
#include "CncStringLogger.h"
#include "SVGFileParser.h"
#include "CncSvgControl.h"

BEGIN_EVENT_TABLE(CncSvgViewer, SVG_VIEWER_CLASS_PARENT)
	EVT_MOUSE_EVENTS(CncSvgViewer::OnMouse)
END_EVENT_TABLE()

#ifdef WX_SVG_SUPPORT

//----------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////
	void CncSvgViewer::Init() {
	/////////////////////////////////////////////////////////////////////
		wxSVGCtrl::Init();
	}
	/////////////////////////////////////////////////////////////////////
	void CncSvgViewer::OnPaint(wxPaintEvent& event) {
	/////////////////////////////////////////////////////////////////////
		wxSVGCtrl::OnPaint(event);
		return;
		
		if ( IsShownOnScreen() ) {
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
	}
	/////////////////////////////////////////////////////////////////////
	void CncSvgViewer::OnResize(wxSizeEvent& event) {
	/////////////////////////////////////////////////////////////////////
		wxSVGCtrl::OnResize(event);
	}
	/////////////////////////////////////////////////////////////////////
	void CncSvgViewer::OnEraseBackground(wxEraseEvent &event) {
	/////////////////////////////////////////////////////////////////////
		wxSVGCtrl::OnEraseBackground(event);
		return;
		
		// this overrides the default behaviour to get access to adjust the background colour
		event.Skip(true);
	}
	/////////////////////////////////////////////////////////////////////
	void CncSvgViewer::RepaintBuffer() {
	/////////////////////////////////////////////////////////////////////
		wxSVGCtrl::RepaintBuffer();
		return;
		
		if ( IsShownOnScreen() ) {
			
			int w = -1, h = -1;
			if (m_fitToFrame)
				GetClientSize(&w, &h);

			if (m_repaintRect.width > 0 && m_repaintRect.height > 0
					&& (m_repaintRect.width < 2 * m_buffer.GetWidth() / 3
							|| m_repaintRect.height < 2 * m_buffer.GetHeight() / 3)) {
				m_repaintRect.x = wxMax(m_repaintRect.x, 0);
				m_repaintRect.y = wxMax(m_repaintRect.y, 0);
				wxSVGRect rect(m_repaintRect.x / GetScaleX(), m_repaintRect.y / GetScaleY(),
						m_repaintRect.width / GetScaleX(), m_repaintRect.height / GetScaleY());
				wxBitmap bitmap = m_doc->Render(w, h, &rect);
				wxMemoryDC dc;
				dc.SelectObject(m_buffer);
				dc.DrawBitmap(bitmap, m_repaintRect.x, m_repaintRect.y);
			} 
			else {
				const wxImage img(m_doc->Render(w, h));
				
				if ( img.IsOk() )	m_buffer = wxBitmap(img);
				else				m_buffer = wxBitmap();
			}
			
			m_repaintRect = wxRect();
		}
	}
	
	/////////////////////////////////////////////////////////////////////
	void CncSvgViewer::init() {
	/////////////////////////////////////////////////////////////////////
		SetFitToFrame(true);
	}

#else

	/////////////////////////////////////////////////////////////////////
	void CncSvgViewer::init() {
	/////////////////////////////////////////////////////////////////////
	}

#endif
//----------------------------------------------------------------------

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
		SetToolTip("");
	#endif
}
/////////////////////////////////////////////////////////////////////
void CncSvgViewer::update() {
/////////////////////////////////////////////////////////////////////
	#ifdef WX_SVG_SUPPORT
		 Update();
	#else
		SetToolTip("");
	#endif
}
//////////////////////////////////////////////////////////////////////////////
bool CncSvgViewer::loadFile(const wxString& filename, const char* contextInfo) {
//////////////////////////////////////////////////////////////////////////////
	wxFileName svgFile(filename);
	if ( svgFile.Exists() == false )
		return false;
		
	#ifdef WX_SVG_SUPPORT
	
		{
			CncStringLogger tmpLogger;
			const bool ret = Load(filename);
			if ( ret == false ) {
				
				if ( contextInfo != NULL )
					std::cerr << contextInfo << std::endl;
					
				std::cerr	<< filename << "\n" 
							<< tmpLogger.GetBuffer()
							<< std::endl;
			}
			else
			{
				CncDoubleRectangle rect;
				if ( SVGFileParser::evaluateMetricSize(filename, rect) )
				{
					boundaries.setMinBound(CncDoublePosition(rect.getX(), rect.getY(), 0.0));
					boundaries.setMaxBound(CncDoublePosition(rect.getW(), rect.getH(), 0.0));
				}
				else
				{
					boundaries.reset();
				}
			}
			
			Refresh();
			return ret;
		}
		
	#else
		
		boundaries.reset();
		SetToolTip(wxString::Format("wxPanel: Can't display %s", filename));
		return false;
		
	#endif
}
//////////////////////////////////////////////////////////////////////////////
bool CncSvgViewer::getMetricBoundaries(CncDoubleBoundaries& box) {
//////////////////////////////////////////////////////////////////////////////
	box = boundaries;
	return box.hasBoundaries();
}