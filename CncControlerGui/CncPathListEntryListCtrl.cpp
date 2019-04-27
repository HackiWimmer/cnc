#include <iostream>
#include <wx/imaglist.h>
#include "wxcrafter.h"
#include "MainFrame.h"
#include "GlobalStrings.h"
#include "CncConfig.h"
#include "CncPathListEntryListCtrl.h"

extern GlobalConstStringDatabase globalStrings;

// ----------------------------------------------------------------------------
// CncPathListEntryListCtrl Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(CncPathListEntryListCtrl, CncLargeScaledListCtrl)
	EVT_LIST_ITEM_SELECTED(wxID_ANY, 	CncPathListEntryListCtrl::onSelectListItem)
	EVT_LIST_ITEM_ACTIVATED(wxID_ANY, 	CncPathListEntryListCtrl::onActivateListItem)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncPathListEntryListCtrl::CncPathListEntryListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, defaultItemAttr()
, initialItemAttr()
, clientIdItemAttr()
, speedItemAttr()
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Type",	 		wxLIST_FORMAT_LEFT, 	34);
	AppendColumn("PathList ID",		wxLIST_FORMAT_LEFT, 	120);
	AppendColumn("Client ID", 		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("F [mm/min]",		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	
	AppendColumn("X-Distance", 		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Y-Distance", 		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Z-Distance",		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	
	AppendColumn("X-Target", 		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Y-Target", 		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Z-Target",		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	
	AppendColumn("Total Length",	wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::NORMAL);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTION));
	
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibPathList().Bitmap("BMP_NO_CHG"));	// 0
	imageList->Add(ImageLibPathList().Bitmap("BMP_CLIENT_ID"));	// 1
	imageList->Add(ImageLibPathList().Bitmap("BMP_SPEED"));		// 2
	imageList->Add(ImageLibPathList().Bitmap("BMP_POSITION"));	// 3
	
	SetImageList(imageList, wxIMAGE_LIST_SMALL);
	
	defaultItemAttr.SetBackgroundColour(GetBackgroundColour());
	defaultItemAttr.SetFont(font);
	defaultItemAttr.SetTextColour(GetTextColour());

	clientIdItemAttr.SetBackgroundColour(wxColour(128, 128, 128));
	clientIdItemAttr.SetFont(font);
	clientIdItemAttr.SetTextColour(GetTextColour());

	initialItemAttr.SetBackgroundColour(wxColour(112, 146, 190));
	initialItemAttr.SetFont(font);
	initialItemAttr.SetTextColour(GetTextColour());

	speedItemAttr.SetBackgroundColour(wxColour(255, 225, 121));
	speedItemAttr.SetFont(font);
	speedItemAttr.SetTextColour(GetTextColour());
}
/////////////////////////////////////////////////////////////
CncPathListEntryListCtrl::~CncPathListEntryListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
int CncPathListEntryListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	//enum Type {CHG_NOTHING=0, CHG_CLIENTID=1, CHG_SPEED=2, CHG_POSITION=3};
	//imageList->Add(ImageLibPathList().Bitmap("BMP_NO_CHG"));		// 0
	//imageList->Add(ImageLibPathList().Bitmap("BMP_CLIENT_ID"));	// 1
	//imageList->Add(ImageLibPathList().Bitmap("BMP_SPEED"));		// 2
	//imageList->Add(ImageLibPathList().Bitmap("BMP_POSITION"));	// 3
	
	if ( column == COL_TYPE ) {
		long type = 0; 
		getItemText(item, COL_TYPE).ToLong(&type);
		
		switch ( type ) {
			case 0:			return 0;
			case 1:			return 1;
			case 2:			return 2;
			case 3:			return 3;
		}
	}
	
	return -1;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncPathListEntryListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	//enum Type {CHG_NOTHING=0, CHG_CLIENTID=1, CHG_SPEED=2, CHG_POSITION=3};

	if 		( getItemText(item, COL_TYPE) == "0" ) return (wxListItemAttr*)(&initialItemAttr);
	else if	( getItemText(item, COL_TYPE) == "1" ) return (wxListItemAttr*)(&clientIdItemAttr);
	else if ( getItemText(item, COL_TYPE) == "2" ) return (wxListItemAttr*)(&speedItemAttr);

	// this indicates to use the default style
	return (wxListItemAttr*)(&defaultItemAttr);
}
/////////////////////////////////////////////////////////////
void CncPathListEntryListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
	
	setLastSelection(item);
	
	long ln;
	getRow(item).getItem(COL_SEARCH).ToLong(&ln);
	
	SelectEventBlocker blocker(this);
	THE_APP->tryToSelectClientId(ln, MainFrame::TemplateSelSource::TSS_PATH_LIST);
}
/////////////////////////////////////////////////////////////
void CncPathListEntryListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	// currently nothing todo
}
/////////////////////////////////////////////////////////////
bool CncPathListEntryListCtrl::searchReference(const wxString& what) {
/////////////////////////////////////////////////////////////
	long ret = searchRow(what, COL_SEARCH);
	
	if ( ret >= 0 )
		selectItem(ret);
	
	return (ret >= 0 );
}
/////////////////////////////////////////////////////////////
bool CncPathListEntryListCtrl::searchReferenceById(const long id) {
/////////////////////////////////////////////////////////////
	wxString what(wxString::Format(globalStrings.pathListRefFormat, id));
	return searchReference(what);
}

