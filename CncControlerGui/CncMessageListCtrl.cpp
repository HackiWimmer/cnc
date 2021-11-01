#include <iostream>
#include <wx/imaglist.h>
#include "wxCrafterImages.h"
#include "CncArduino.h"
#include "CncCommon.h"
#include "GlobalFunctions.h"
#include "CncMessageListCtrl.h"

// ----------------------------------------------------------------------------
// CncMessageListCtrl Event Table
// ----------------------------------------------------------------------------
wxDEFINE_EVENT(wxEVT_MSG_LIST_DISPLAY_TIMER,  wxTimerEvent);

wxBEGIN_EVENT_TABLE(CncMessageListCtrl, CncLargeScaledListCtrl)
	EVT_SIZE				(								CncMessageListCtrl::onSize				)
	EVT_LIST_ITEM_SELECTED	(wxID_ANY,						CncMessageListCtrl::onSelectListItem	)
	EVT_LIST_ITEM_ACTIVATED	(wxID_ANY,						CncMessageListCtrl::onActivateListItem	)
	EVT_TIMER				(wxEVT_MSG_LIST_DISPLAY_TIMER,	CncMessageListCtrl::onDisplayTimer		)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncMessageListCtrl::CncMessageListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, messages			()
, updateInterval	(0)
, displayTimer		(this, wxEVT_MSG_LIST_DISPLAY_TIMER)
, itemAttrInfo		()
, itemAttrWarning	()
, itemAttrError		()
, itemAttrDebug		()
, itemAttrSeparator	()
/////////////////////////////////////////////////////////////
{
	// init
	setUpdateInterval(updateInterval);
	
	// add colums
	AppendColumn("Time", 		wxLIST_FORMAT_LEFT, 	 84);
	AppendColumn("Type", 		wxLIST_FORMAT_CENTER, 	 40);
	AppendColumn("Message",		wxLIST_FORMAT_LEFT, 	200);
	AppendColumn("Context",		wxLIST_FORMAT_LEFT, 	140);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::REVERSE);
	
	wxFont font(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(0,  0,  0));
	
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	//imageList->Add(ImageLibPosSpy().Bitmap("BMP_PID_MAJOR"));			// 0
	SetImageList(imageList, wxIMAGE_LIST_SMALL);
	
	wxFont boldFont(font); 		boldFont.MakeBold();
	wxFont boldItalic(font); 	boldItalic.MakeItalic();
	
	itemAttrInfo.SetBackgroundColour(GetBackgroundColour());
	itemAttrInfo.SetFont(font);
	itemAttrInfo.SetTextColour(*wxLIGHT_GREY);
	
	itemAttrWarning.SetBackgroundColour(GetBackgroundColour());
	itemAttrWarning.SetFont(boldFont);
	itemAttrWarning.SetTextColour(wxColour(242, 242,   0));
	
	itemAttrError.SetBackgroundColour(GetBackgroundColour());
	itemAttrError.SetFont(boldFont);
	itemAttrError.SetTextColour(wxColour(255, 128, 128));
	
	itemAttrDebug.SetBackgroundColour(GetBackgroundColour());
	itemAttrDebug.SetFont(boldItalic);
	itemAttrDebug.SetTextColour(wxColour(127, 127, 127));

	itemAttrSeparator.SetBackgroundColour(wxColour( 42, 42, 42));
	itemAttrSeparator.SetFont(boldFont);
	itemAttrSeparator.SetTextColour(wxColour(242, 242,   0));
}
/////////////////////////////////////////////////////////////
CncMessageListCtrl::~CncMessageListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
void CncMessageListCtrl::clear() {
/////////////////////////////////////////////////////////////
	CncLargeScaledListCtrl::clear();
	messages.clear();
	SetItemCount(messages.size());
}
/////////////////////////////////////////////////////////////
void CncMessageListCtrl::appendMessage(const char type, const wxString& message) {
/////////////////////////////////////////////////////////////
	appendMessage(type, message, "");
}
/////////////////////////////////////////////////////////////
void CncMessageListCtrl::appendMessage(const char type, const wxString& message, const wxString& context) {
/////////////////////////////////////////////////////////////
	messages.push_back(std::move(Message(type, message, context)));
	
	if ( displayTimer.IsRunning() == false )
	{
		SetItemCount(messages.size());
		ensureVisible((long)(messages.size() - 1));
	}
}
/////////////////////////////////////////////////////////////
bool CncMessageListCtrl::isItemValid(long item) const {
/////////////////////////////////////////////////////////////
	if ( item < 0 || item > (long)(messages.size() - 1) )
		return false;
		
	return true;
}
/////////////////////////////////////////////////////////////
int CncMessageListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	return -1;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncMessageListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return NULL;
	
	switch ( messages.at(item).type ) {
		case 'W':	return (wxListItemAttr*)(&itemAttrWarning);
		case 'E':	return (wxListItemAttr*)(&itemAttrError);
		case 'D':	return (wxListItemAttr*)(&itemAttrDebug);
		case 'S':	return (wxListItemAttr*)(&itemAttrSeparator);
	}
	
	return (wxListItemAttr*)(&itemAttrInfo);
}
/////////////////////////////////////////////////////////////
wxString CncMessageListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return _("");
		
	switch ( column ) {
		case COL_TIM:	return _(messages.at(item).ts.FormatISOTime());
		case COL_TYP:	return _(wxString::Format("%c", messages.at(item).type));
		case COL_MSG:	return _(messages.at(item).message);
		case COL_CTX:	return _(messages.at(item).context);
	}
	
	return _("");
}
/////////////////////////////////////////////////////////////
void CncMessageListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	// currently nothing todo
}
/////////////////////////////////////////////////////////////
void CncMessageListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	// currently nothing todo
}
/////////////////////////////////////////////////////////////////////
void CncMessageListCtrl::onSize(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////////////
	event.Skip();
	updateColumnWidth();
	Refresh();
}
/////////////////////////////////////////////////////////////////////
void CncMessageListCtrl::updateColumnWidth() {
/////////////////////////////////////////////////////////////////////
	if ( GetColumnCount() <= 0 )
		return;
		
	// avoid flicker
	GblFunc::freeze(this, true);
		
	int colWidthSum = 0;
	for ( int i = 0; i < GetColumnCount(); i++ ) {
		if ( i == COL_STRECH )
			continue;
			
		colWidthSum += GetColumnWidth(i);
	}
	
	const int scrollbarWidth = 26;
	int size = GetSize().GetWidth() 
	         - colWidthSum
			 - scrollbarWidth;
			 
	SetColumnWidth(COL_STRECH, size);

	GblFunc::freeze(this, false);
}
/////////////////////////////////////////////////////////////////////
bool CncMessageListCtrl::setUpdateInterval(int value) {
/////////////////////////////////////////////////////////////////////
	updateInterval = value;
	
	bool ret = true;
	if ( updateInterval > 50 )	ret = displayTimer.Start(updateInterval);
	else						displayTimer.Stop();
	
	return ret;
}
//////////////////////////////////////////////////
void CncMessageListCtrl::onDisplayTimer(wxTimerEvent& event) {
//////////////////////////////////////////////////
	if ( messages.size() == 0 )
		return;
	
	if ( GetItemCount() == (long)messages.size() )
		return;
	
	SetItemCount(messages.size());
	ensureVisible((long)(messages.size() - 1));
}