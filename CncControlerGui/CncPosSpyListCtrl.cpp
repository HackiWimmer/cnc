#include <wx/imaglist.h>
#include "wxcrafter.h"
#include "CncPosSpyListCtrl.h"

/////////////////////////////////////////////////////////////
CncPosSpyListCtrl::CncPosSpyListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Reference", 	wxLIST_FORMAT_RIGHT, 	100);
	AppendColumn("", 			wxLIST_FORMAT_CENTER, 	 26);
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
	imageList->Add(ImageLibPosSpy().Bitmap("BMP_REF"));
	imageList->Add(ImageLibPosSpy().Bitmap("BMP_TYPE_RAPID"));
	imageList->Add(ImageLibPosSpy().Bitmap("BMP_TYPE_WORK"));
	imageList->Add(ImageLibPosSpy().Bitmap("BMP_POS"));
	
	SetImageList(imageList, wxIMAGE_LIST_SMALL);
}

/////////////////////////////////////////////////////////////
CncPosSpyListCtrl::~CncPosSpyListCtrl() {
/////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////
int CncPosSpyListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	switch ( column ) {
		case 0:		return 0;
		
		case 1:		if ( GetItemText(item,column) == "R" )	return 1;
					else									return 2;
		case 3:
		case 4:
		case 5:		return 3;
	}
	
	return -1;
}
