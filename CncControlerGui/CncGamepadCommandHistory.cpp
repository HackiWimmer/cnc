#include <iostream>
#include <wx/imaglist.h>
#include "wxCrafterGamePort.h"
#include "CncGamepadCommandHistory.h"

// ----------------------------------------------------------------------------
// CncGamepadCommadHistoryListCtrl Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(CncGamepadCommadHistoryListCtrl, CncLargeScaledListCtrl)
	EVT_LIST_ITEM_SELECTED	(wxID_ANY, 	CncGamepadCommadHistoryListCtrl::onSelectListItem	)
	EVT_LIST_ITEM_ACTIVATED	(wxID_ANY, 	CncGamepadCommadHistoryListCtrl::onActivateListItem	)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncGamepadCommadHistoryListCtrl::CncGamepadCommadHistoryListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, commands			()
, defaultItemAttr	()
, errorItemAttr		()
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Typ",			wxLIST_FORMAT_CENTRE,	 40);
	AppendColumn("Result",		wxLIST_FORMAT_CENTRE,	 46);
	AppendColumn("Command",		wxLIST_FORMAT_LEFT,		150);
	AppendColumn("Parameter", 	wxLIST_FORMAT_CENTRE,	150);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::NORMAL);
	
	wxFont font(9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(0, 0, 0));
	SetTextColour(wxColour(255, 255, 255));
	
	defaultItemAttr.SetBackgroundColour(GetBackgroundColour());
	defaultItemAttr.SetFont(font);
	defaultItemAttr.SetTextColour(GetTextColour());
	
	errorItemAttr 	= defaultItemAttr;
	errorItemAttr	.SetTextColour(wxColour(255, 128, 128));
	errorItemAttr	.SetFont(font.Bold());

	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibGamepadCommand().Bitmap("BMP_START"));
	imageList->Add(ImageLibGamepadCommand().Bitmap("BMP_STOP"));
	imageList->Add(ImageLibGamepadCommand().Bitmap("BMP_UPDATE"));
	imageList->Add(ImageLibGamepadCommand().Bitmap("BMP_UPDATE_WITH_CONTENT"));
	AssignImageList(imageList, wxIMAGE_LIST_SMALL);
	
	SetItemCount(commands.size());
}
/////////////////////////////////////////////////////////////
CncGamepadCommadHistoryListCtrl::~CncGamepadCommadHistoryListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
void CncGamepadCommadHistoryListCtrl::addCommand(int type, int result, const wxString& cmd, const wxString& para) {
/////////////////////////////////////////////////////////////
	CmdInfo ci;
	ci.type			= type;
	ci.result		= result;
	ci.command		= cmd;
	ci.parameter	= para;
	
	commands.insert(commands.begin(), std::move(ci));
	if ( commands.size() > 64 )
		commands.pop_back();
	
	SetItemCount(commands.size());
	
	if ( IsShownOnScreen() )
		Refresh();
}
/////////////////////////////////////////////////////////////
void CncGamepadCommadHistoryListCtrl::clearAll() {
/////////////////////////////////////////////////////////////
	clear();
	commands.clear();
	
	SetItemCount(commands.size());
}
/////////////////////////////////////////////////////////////
void CncGamepadCommadHistoryListCtrl::addStartCommand(bool result) {
/////////////////////////////////////////////////////////////
	addCommand(0, result, "Start", "");
}
/////////////////////////////////////////////////////////////
void CncGamepadCommadHistoryListCtrl::addStopCommand(bool result) {
/////////////////////////////////////////////////////////////
	addCommand(1, result, "Stop", "");
}
/////////////////////////////////////////////////////////////
void CncGamepadCommadHistoryListCtrl::addUpdateCommand(bool result, const wxString& para) {
/////////////////////////////////////////////////////////////
	const int type = para.IsEmpty() ? 2 : 3;
	addCommand(type, result, "Update", para);
}
/////////////////////////////////////////////////////////////////////
bool CncGamepadCommadHistoryListCtrl::isItemValid(long item) const {
/////////////////////////////////////////////////////////////////////
	if ( item < 0 || item > (long)(commands.size() - 1) )
		return false;
	
	return true;
}
/////////////////////////////////////////////////////////////
int CncGamepadCommadHistoryListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return -1;
		
	const CmdInfo& i = commands.at(item);
	
	if ( column == COL_TYP )
		return i.type;
	
	return -1;
}
/////////////////////////////////////////////////////////////////////
wxString CncGamepadCommadHistoryListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return _("");
		
	const CmdInfo& i = commands.at(item);

	switch ( column ) {
		case COL_TYP:			return _(wxString::Format("%d", i.type));
		case COL_RESULT:		return _(wxString::Format("%d", i.result));
		case COL_COMMAND:		return _(i.command);
		case COL_PARAMETER: 	return _(i.parameter);
	}
	
	return _("");
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncGamepadCommadHistoryListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	if ( isItemValid(item) == true ) {
		const CmdInfo& i = commands.at(item);
		if ( i.result == 0 )
			return (wxListItemAttr*)(&errorItemAttr);
	}
	
	return (wxListItemAttr*)(&defaultItemAttr);
}
/////////////////////////////////////////////////////////////////////
void CncGamepadCommadHistoryListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////
void CncGamepadCommadHistoryListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////////////
}



