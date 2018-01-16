#include <iostream>
#include <wx/imaglist.h>
#include "wxcrafter.h"
#include "MainFrame.h"
#include "CncConfig.h"
#include "CncArduino.h"
#include "CncSetterListCtrl.h"

// ----------------------------------------------------------------------------
// CncMotionMonitor Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(CncSetterListCtrl, CncLargeScaledListCtrl)
	EVT_SIZE(CncSetterListCtrl::onSize)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncSetterListCtrl::CncSetterListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, separatorPid(wxString::Format("%d", (int)PID_SEPARATOR))
, defaultItemAttr()
, separatorRunItemAttr()
, separatorResetItemAttr()
, separatorSetupItemAttr()
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("",	 	wxLIST_FORMAT_LEFT, 	wxLIST_AUTOSIZE);
	AppendColumn("#",	 	wxLIST_FORMAT_LEFT, 	wxLIST_AUTOSIZE);
	AppendColumn("PID", 	wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Key", 	wxLIST_FORMAT_LEFT, 	wxLIST_AUTOSIZE);
	AppendColumn("Value",	wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::REVERSE);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTION));
	
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibSetterList().Bitmap("BMP_SETTER"));		// 0
	imageList->Add(ImageLibSetterList().Bitmap("BMP_SEPARATOR"));	// 1
	
	SetImageList(imageList, wxIMAGE_LIST_SMALL);
	
	defaultItemAttr.SetBackgroundColour(GetBackgroundColour());
	defaultItemAttr.SetFont(font);
	defaultItemAttr.SetTextColour(GetTextColour());
	
	wxFont separatorFont(font);
	separatorFont.MakeBold();

	separatorRunItemAttr.SetBackgroundColour(wxColour(128, 64, 64));
	separatorRunItemAttr.SetFont(separatorFont);
	separatorRunItemAttr.SetTextColour(*wxWHITE);

	separatorResetItemAttr.SetBackgroundColour(wxColour(0, 0, 64));
	separatorResetItemAttr.SetFont(separatorFont);
	separatorResetItemAttr.SetTextColour(*wxWHITE);

	separatorSetupItemAttr.SetBackgroundColour(wxColour(0, 64, 64));
	separatorSetupItemAttr.SetFont(separatorFont);
	separatorSetupItemAttr.SetTextColour(*wxWHITE);
}
/////////////////////////////////////////////////////////////
CncSetterListCtrl::~CncSetterListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
int CncSetterListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	switch ( column ) {
		case COL_TYPE: 	if ( getItemText(item, COL_PID) == separatorPid )	return 1;
						else												return 0;
	}
	
	return -1;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncSetterListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	if ( getItemText(item, COL_PID) == separatorPid ) {
		wxString val(getItemText(item, COL_TYPE));
		long id;
		val.ToLong(&id);
		
		switch ( id ) {
			case SEPARARTOR_SETUP:	return (wxListItemAttr*)(&separatorSetupItemAttr);
			case SEPARARTOR_RESET:	return (wxListItemAttr*)(&separatorResetItemAttr);
			case SEPARARTOR_RUN:	return (wxListItemAttr*)(&separatorRunItemAttr);
		}
	}
	
	// this indicates to use the default style
	return (wxListItemAttr*)(&defaultItemAttr);
}
/////////////////////////////////////////////////////////////////////
void CncSetterListCtrl::updateColumnWidth() {
/////////////////////////////////////////////////////////////////////
	// avoid flicker
	if ( IsFrozen() == false )
		Freeze();
		
	// first set default sizes depending on content
	SetColumnWidth(COL_TYPE, 	 22);
	SetColumnWidth(COL_NUM, 	 84);
	SetColumnWidth(COL_PID, 	 40);
	SetColumnWidth(COL_KEY, 	wxLIST_AUTOSIZE);
	SetColumnWidth(COL_VAL, 	120);
	
	// try to strech the second (key) column
	const int scrollbarWidth = 26;
	int size = GetSize().GetWidth() - GetColumnWidth(COL_NUM) - GetColumnWidth(COL_VAL) - GetColumnWidth(COL_TYPE) - GetColumnWidth(COL_PID) - scrollbarWidth;
	if ( size > GetColumnWidth(COL_KEY) )
		SetColumnWidth(COL_KEY, size);
		
	if ( IsFrozen() == true )
		Thaw();
}
/////////////////////////////////////////////////////////////////////
void CncSetterListCtrl::onSize(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////////////
	updateColumnWidth();
}
