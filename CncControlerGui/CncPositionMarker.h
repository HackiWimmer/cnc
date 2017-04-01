#ifndef CNC_POS_MARKER_H
#define CNC_POS_MARKER_H

#include <wx/panel.h>
#include <wx/dcclient.h>
#include <wx/pen.h>

enum XMarkerType { XMarkerTop, XMarkerBottom};

class CncPositionMarker {
	
	private:
		wxPoint position;
		XMarkerType xMarkerType;
		int xOffset;
		int yOffset;
		wxPanel* xMarkerTop;
		wxPanel* xMarkerBottom;
		wxPanel* yMarker;
		
public:
		//////////////////////////////////////////////////////////////
		CncPositionMarker() 
		: position(0,0)
		, xMarkerType(XMarkerTop)
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
		XMarkerType getXMarkerTyp() {
			return xMarkerType;
		}
		//////////////////////////////////////////////////////////////
		void setPosition(wxPoint p) {
			if ( xMarkerTop == NULL || xMarkerBottom == NULL || yMarker == NULL )
				return;
				
			position = p;
				
			wxClientDC xDc((xMarkerType == XMarkerTop ? xMarkerTop : xMarkerBottom));
			wxClientDC yDc(yMarker);
			
			drawPosX(xDc);
			drawPosY(yDc);
		}
		//////////////////////////////////////////////////////////////
		void drawPosX(wxDC& xDc) {
			xDc.SetPen(*wxBLACK_PEN);
			xDc.SetBrush(*wxBLACK_BRUSH);
			
			xDc.Clear();
			if ( xMarkerType == XMarkerTop ) {
				wxPoint xP[3] = {{position.x - 2, 0}, {position.x + 2, 0}, {position.x, 5}};
				xDc.DrawPolygon(3, xP);
			} else {
				wxPoint xP[3] = {{position.x - 2, 5}, {position.x + 2, 5}, {position.x, 0}};
				xDc.DrawPolygon(3, xP);
			}
		}
		//////////////////////////////////////////////////////////////
		void drawPosY(wxDC& yDc) {
			yDc.SetPen(*wxBLACK_PEN);
			yDc.SetBrush(*wxBLACK_BRUSH);
			
			yDc.Clear();
			wxPoint yP[3] = {{0, position.y - 2}, {0, position.y + 2}, {5, position.y}};
			yDc.DrawPolygon(3, yP, 0, yOffset);
		}
};

#endif