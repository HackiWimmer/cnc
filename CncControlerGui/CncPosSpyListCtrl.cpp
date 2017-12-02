#include <iostream>
#include <wx/imaglist.h>
#include "wxcrafter.h"
#include "CncArduino.h"
#include "CncPosSpyListCtrl.h"

/////////////////////////////////////////////////////////////
CncPosSpyListCtrl::CncPosSpyListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, majorPosPid(wxString::Format("%d", (int)PID_XYZ_POS_MAJOR))
, majorPosItemAttr()
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("",	 		wxLIST_FORMAT_LEFT, 	 22);
	AppendColumn("Reference", 	wxLIST_FORMAT_LEFT, 	 80);
	AppendColumn("", 			wxLIST_FORMAT_CENTER, 	 22);
	AppendColumn("Speed", 		wxLIST_FORMAT_RIGHT, 	 50);
	AppendColumn("X", 			wxLIST_FORMAT_RIGHT,	wxLIST_AUTOSIZE);
	AppendColumn("Y", 			wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Z", 			wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::REVERSE);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTION));
	
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibPosSpy().Bitmap("BMP_PID_MAJOR"));	// 0
	imageList->Add(ImageLibPosSpy().Bitmap("BMP_PID_DETAIL"));	// 1
	imageList->Add(ImageLibPosSpy().Bitmap("BMP_TYPE_RAPID"));	// 2
	imageList->Add(ImageLibPosSpy().Bitmap("BMP_TYPE_WORK"));	// 3
	imageList->Add(ImageLibPosSpy().Bitmap("BMP_POS"));			// 4
	
	SetImageList(imageList, wxIMAGE_LIST_SMALL);
	
	wxFont majorPosFont(font);
	majorPosFont.MakeBold();
	majorPosItemAttr.SetBackgroundColour(GetBackgroundColour());
	majorPosItemAttr.SetFont(majorPosFont);
	majorPosItemAttr.SetTextColour(wxColour(0, 0, 64));
}
/////////////////////////////////////////////////////////////
CncPosSpyListCtrl::~CncPosSpyListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
int CncPosSpyListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	switch ( column ) {
		case COL_PID: 	if ( GetItemText(item, column) == majorPosPid )	return 0;
						else											return 1;
						
		case COL_REF:	return -1;
		
		case COL_T:		if ( GetItemText(item, column) == "R" )			return 2;
						else											return 3;
						
		case COL_F:		return -1;
		
		case COL_X:
		case COL_Y:
		case COL_Z:		return 4;
	}

	return -1;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncPosSpyListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	if ( GetItemText(item, COL_PID) == majorPosPid )
		return (wxListItemAttr*)(&majorPosItemAttr);
	
	// this indicates to use the default style
	return NULL;
}

