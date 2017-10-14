#include "CncConfig.h"
#include "CncSpeedView.h"

wxBEGIN_EVENT_TABLE(CncSpeedView, wxPanel)
	EVT_PAINT(CncSpeedView::OnPaint)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////////////
CncSpeedView::CncSpeedView(wxWindow *parent, wxWindowID id)
/////////////////////////////////////////////////////////////////////
: wxPanel(parent, id)
, maxSpeed(0)
, speedX(0)
, speedY(0)
, speedZ(0)
, font(7, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Segoe UI"))
, colourX(127,0,0)
, colourY(0,127,0)
, colourZ(0,64,127)
, brushX(colourX)
, brushY(colourY)
, brushZ(colourZ)
{
	SetBackgroundColour(*wxBLACK);
	SetToolTip("");
}
/////////////////////////////////////////////////////////////////////
CncSpeedView::~CncSpeedView() {
/////////////////////////////////////////////////////////////////////
	
}
/////////////////////////////////////////////////////////////////////
void CncSpeedView::refresh(RefreshType rt) {
/////////////////////////////////////////////////////////////////////
	const wxSize cs = GetClientSize();
	
	switch ( rt ) {
		case RT_X:		RefreshRect(wxRect(getOffsetX(), 0, getWidthX(), cs.GetHeight()));
						break;
		case RT_Y:		RefreshRect(wxRect(getOffsetY(), 0, getWidthY(), cs.GetHeight()));
						break;
		case RT_Z:		RefreshRect(wxRect(getOffsetZ(), 0, getWidthZ(), cs.GetHeight()));
						break;
		default:		Refresh(true);
	}
}
/////////////////////////////////////////////////////////////////////
void CncSpeedView::updateToolTip() {
	wxString tt;
	tt << "Speed Max: ";
	tt << maxSpeed;
	tt << "  \n";
	tt << "Speed X: ";
	tt << speedX;
	tt << "  \n";
	tt << "Speed Y: ";
	tt << speedY;
	tt << "  \n";
	tt << "Speed Z: ";
	tt << speedZ;
	
	SetToolTip(tt);
}
/////////////////////////////////////////////////////////////////////
void CncSpeedView::setMaxSpeed(unsigned int value) {
/////////////////////////////////////////////////////////////////////
	if ( value > maxSpeed )
		maxSpeed = value;
	
	updateToolTip();
}
/////////////////////////////////////////////////////////////////////
unsigned int CncSpeedView::convertValue(unsigned int value) {
/////////////////////////////////////////////////////////////////////
	const wxSize cs = GetClientSize();
	unsigned int total = cs.GetHeight() - topMargin - bottomMargin;
	
	if ( maxSpeed == 0 )
		return 0;
	
	return value * total / maxSpeed;
}
/////////////////////////////////////////////////////////////////////
void CncSpeedView::OnPaint(wxPaintEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxPaintDC dc(this);
	const wxSize cs = GetClientSize();
	
	dc.SetFont(font);
	
	// graduation
	dc.SetPen(wxPen(*wxLIGHT_GREY, 1, wxSOLID));
	dc.DrawLine(wxPoint(getGraduationOffset() + 1, topMargin), wxPoint(getGraduationOffset() + 1 , cs.GetHeight() - bottomMargin));
	for ( unsigned int i=0; i<=maxSpeed; i++ ) {
		
		if ( i == 0 || i%100 == 0) {
			dc.SetPen(wxPen(*wxLIGHT_GREY, 1, wxSOLID));
			dc.DrawLine(wxPoint(getGraduationOffset() + 1, cs.GetHeight() - bottomMargin - convertValue(i)), wxPoint(cs.GetWidth(), cs.GetHeight() - bottomMargin - convertValue(i)));
			if ( i != 0 ) {
				dc.SetTextForeground(*wxWHITE);
				dc.DrawText(wxString::Format("%d", i), wxPoint(2, cs.GetHeight() - bottomMargin - convertValue(i)));
			}
		}
		if ( i != 0 && i%100 != 0 && i%50 == 0 ) {
			dc.SetPen(wxPen(wxColour(80,80,80), 1, wxDOT));
			dc.DrawLine(wxPoint(getGraduationOffset() + 1, cs.GetHeight() - bottomMargin - convertValue(i)), wxPoint(cs.GetWidth(), cs.GetHeight() - bottomMargin - convertValue(i)));
		}
	}
	
	// X
	dc.SetBrush(brushX);
	dc.SetPen(colourX);
	dc.DrawRectangle(getOffsetX(), cs.GetHeight() - convertValue(speedX) - bottomMargin, getWidthX(), convertValue(speedX));
	
	// Y
	dc.SetBrush(brushY);
	dc.SetPen(colourY);
	dc.DrawRectangle(getOffsetY(), cs.GetHeight() - convertValue(speedY) - bottomMargin, getWidthY(), convertValue(speedY));
	
	// Z
	dc.SetBrush(brushZ);
	dc.SetPen(colourZ);
	dc.DrawRectangle(getOffsetZ(), cs.GetHeight() - convertValue(speedZ) - bottomMargin, getWidthZ(), convertValue(speedZ));
	
	// legend
	dc.SetTextForeground(*wxWHITE);
	dc.DrawText("X", wxPoint(getOffsetX() + (getWidthX() / 2) - 3, cs.GetHeight() - 11));
	dc.DrawText("Y", wxPoint(getOffsetY() + (getWidthY() / 2) - 3, cs.GetHeight() - 11));
	dc.DrawText("Z", wxPoint(getOffsetZ() + (getWidthZ() / 2) - 3, cs.GetHeight() - 11));
}
