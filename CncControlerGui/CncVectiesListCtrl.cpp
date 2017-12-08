#include <iostream>
#include <wx/imaglist.h>
#include "wxcrafter.h"
#include "CncArduino.h"
#include "CncVectiesListCtrl.h"

/////////////////////////////////////////////////////////////
CncVectiesListCtrl::CncVectiesListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Reference", 	wxLIST_FORMAT_LEFT, 	120);
	AppendColumn("",			wxLIST_FORMAT_CENTER, 	 26);
	AppendColumn("X", 			wxLIST_FORMAT_RIGHT,	150);
	AppendColumn("Y", 			wxLIST_FORMAT_RIGHT, 	150);
	AppendColumn("Z", 			wxLIST_FORMAT_RIGHT, 	150);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::REVERSE);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTION));

	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibPosSpy().Bitmap("BMP_TYPE_RAPID"));	// 0
	imageList->Add(ImageLibPosSpy().Bitmap("BMP_TYPE_WORK"));	// 1
	
	SetImageList(imageList, wxIMAGE_LIST_SMALL);
}
/////////////////////////////////////////////////////////////
CncVectiesListCtrl::~CncVectiesListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
int CncVectiesListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	switch ( column ) {
		case COL_T:		if ( GetItemText(item, column) == "R" )		return 0;
						else										return 1;
	}
	
	return -1;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncVectiesListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	// this indicates to use the default style
	return NULL;
}