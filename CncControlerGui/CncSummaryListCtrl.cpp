#include <iostream>
#include <wx/imaglist.h>
#include "wxcrafter.h"
#include "CncArduino.h"
#include "CncSummaryListCtrl.h"

// ----------------------------------------------------------------------------
// CncMotionMonitor Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(CncSummaryListCtrl, CncLargeScaledListCtrl)
	EVT_SIZE(CncSummaryListCtrl::onSize)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncSummaryListCtrl::CncSummaryListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, defaultItemAttr()
, headlineItemAttr()
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Type", 		wxLIST_FORMAT_LEFT, 	wxLIST_AUTOSIZE);
	AppendColumn("Parameter", 	wxLIST_FORMAT_LEFT, 	wxLIST_AUTOSIZE);
	AppendColumn("Value",		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Unit", 		wxLIST_FORMAT_CENTER,	wxLIST_AUTOSIZE);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::NORMAL);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTION));

	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibSummary().Bitmap("BMP_HEADLINE"));		// 0
	imageList->Add(ImageLibSummary().Bitmap("BMP_TYPE_COMMON"));	// 1
	imageList->Add(ImageLibSummary().Bitmap("BMP_TYPE_GCODE"));		// 2
	imageList->Add(ImageLibSummary().Bitmap("BMP_TYPE_SVG"));		// 3
	
	SetImageList(imageList, wxIMAGE_LIST_SMALL);
	
	defaultItemAttr.SetBackgroundColour(GetBackgroundColour());
	defaultItemAttr.SetFont(font);
	defaultItemAttr.SetTextColour(GetTextColour());
	
	wxFont headlineFont(font);
	headlineFont.MakeBold();

	headlineItemAttr.SetBackgroundColour(wxColour(0, 64, 128));
	headlineItemAttr.SetFont(headlineFont);
	headlineItemAttr.SetTextColour(*wxWHITE);
}
/////////////////////////////////////////////////////////////
CncSummaryListCtrl::~CncSummaryListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
int CncSummaryListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	long idx = -1;
	switch ( column ) {
		case COL_TYPE:		GetItemText(item, column).ToLong(&idx);
							break;
	}
	
	return idx;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncSummaryListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	if ( GetItemText(item, COL_TYPE) == wxString::Format("%d", PT_HEADLINE) ) {
		return (wxListItemAttr*)(&headlineItemAttr);
	}
	
	// this indicates to use the default style
	return (wxListItemAttr*)(&defaultItemAttr);
}
/////////////////////////////////////////////////////////////////////
void CncSummaryListCtrl::onSize(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////////////
	// avoid flicker
	if ( IsFrozen() == false )
		Freeze();
		
	// first set default sizes depending on content
	SetColumnWidth(COL_TYPE, 	 40);
	SetColumnWidth(COL_PARAM, 	wxLIST_AUTOSIZE);
	SetColumnWidth(COL_VALUE, 	150);
	SetColumnWidth(COL_UNIT, 	 50);
	
	// try to strech the second (key) column
	const int scrollbarWidth = 26;
	int size = GetSize().GetWidth() - GetColumnWidth(COL_TYPE) - GetColumnWidth(COL_VALUE) - GetColumnWidth(COL_UNIT) - scrollbarWidth;
	if ( size > GetColumnWidth(COL_PARAM) )
		SetColumnWidth(COL_PARAM, size);
		
	if ( IsFrozen() == true )
		Thaw();
}
/////////////////////////////////////////////////////////////
void CncSummaryListCtrl::addHeadline(ParameterType pt, const wxString& text) {
/////////////////////////////////////////////////////////////
	CncColumContainer cc(TOTAL_COL_COUNT);
	cc.updateItem(COL_TYPE, 	wxString::Format("%d", pt));
	cc.updateItem(COL_PARAM, 	text);

	appendItem(cc);
}
/////////////////////////////////////////////////////////////
void CncSummaryListCtrl::addParameter(ParameterType pt, const wxString& param, const wxVariant& value, const wxString& unit) {
/////////////////////////////////////////////////////////////
	CncColumContainer cc(TOTAL_COL_COUNT);
	cc.updateItem(COL_TYPE, 	wxString::Format("%d", pt));
	cc.updateItem(COL_PARAM, 	param);
	cc.updateItem(COL_VALUE, 	value);
	cc.updateItem(COL_UNIT, 	unit);

	appendItem(cc);

}