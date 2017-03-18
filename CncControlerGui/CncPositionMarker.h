#ifndef CNC_POS_MARKER_H
#define CNC_POS_MARKER_H

#include <wx/panel.h>
#include <wx/dcclient.h>
#include <wx/pen.h>

enum XMarkerType { XMarkerTop, XMarkerBottom};

class CncPositionMarker {
	
	private:
		XMarkerType xMarkerType;
		int xOffset;
		int yOffset;
		wxPanel* xMarkerTop;
		wxPanel* xMarkerBottom;
		wxPanel* yMarker;
		
public:
		//////////////////////////////////////////////////////////////
		CncPositionMarker() 
		: xMarkerType(XMarkerTop)
		, xOffset(0)
		, yOffset(10)
		, xMarkerTop(NULL)
		, xMarkerBottom(NULL)
		, yMarker(NULL)
		{}
		//////////////////////////////////////////////////////////////
		~CncPositionMarker() {
		}
		//////////////////////////////////////////////////////////////
		void setMarkerControls(wxPanel* xmt, wxPanel* xmb, wxPanel* ym) {
			xMarkerTop 		= xmt;
			xMarkerBottom 	= xmb;
			yMarker 		= ym;
		}
		//////////////////////////////////////////////////////////////
		void setXMarkerTyp(XMarkerType t) {
			xMarkerType = t;
		}
		//////////////////////////////////////////////////////////////
		void setPosition(wxPoint p) {
			if ( xMarkerTop == NULL || xMarkerBottom == NULL || yMarker == NULL )
				return;
				
			wxClientDC xDc((xMarkerType == XMarkerTop ? xMarkerTop : xMarkerBottom));
			wxClientDC yDc(yMarker);
			xDc.SetPen(*wxBLACK_PEN);
			xDc.SetBrush(*wxBLACK_BRUSH);
			yDc.SetPen(*wxBLACK_PEN);
			yDc.SetBrush(*wxBLACK_BRUSH);
			
			xDc.Clear();
			if ( xMarkerType == XMarkerTop ) {
				wxPoint xP[3] = {{p.x - 2, 0}, {p.x + 2, 0}, {p.x, 5}};
				xDc.DrawPolygon(3, xP);
			} else {
				wxPoint xP[3] = {{p.x - 2, 5}, {p.x + 2, 5}, {p.x, 0}};
				xDc.DrawPolygon(3, xP);
			}

			yDc.Clear();
			wxPoint yP[3] = {{0, p.y - 2}, {0, p.y + 2}, {5, p.y}};
			yDc.DrawPolygon(3, yP, 0, yOffset);
		}
};

#endif