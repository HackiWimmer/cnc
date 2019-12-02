#include <iostream>
#include <wx/imaglist.h>
#include "MainFrame.h"
#include "GlobalStrings.h"
#include "CncConfig.h"
#include "CncPathListEntryListCtrl.h"

extern GlobalConstStringDatabase globalStrings;

// ----------------------------------------------------------------------------
// CncPathListEntryListCtrl Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(CncPathListEntryListCtrl, CncLargeScaledListCtrl)
	EVT_SIZE(							CncPathListEntryListCtrl::onSize)
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
, pathLists()
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Type",	 		wxLIST_FORMAT_LEFT, 	44);
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
	
	SetBackgroundColour(wxColour(191, 205, 219));
	
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

	initialItemAttr.SetBackgroundColour(GetBackgroundColour());
	initialItemAttr.SetFont(font.Underlined().Bold());
	initialItemAttr.SetTextColour(GetTextColour());

	clientIdItemAttr.SetBackgroundColour(GetBackgroundColour());
	clientIdItemAttr.SetFont(font.Bold());
	clientIdItemAttr.SetTextColour(GetTextColour());

	speedItemAttr.SetBackgroundColour(GetBackgroundColour());
	speedItemAttr.SetFont(font.Bold());
	speedItemAttr.SetTextColour(wxColour(63, 72, 204));
}
/////////////////////////////////////////////////////////////
CncPathListEntryListCtrl::~CncPathListEntryListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
wxString CncPathListEntryListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////
	static const wxString fmt(globalStrings.pathListRefFormat);
	
	if ( isItemValid(item) == false )
		return _("");
		
	const CncPathListEntry& cpe = pathLists.at(item);
	
	const bool displaySpeed    = ( cpe.isSpeedChange()    == true || cpe.isNothingChange() == true );
	const bool displayPosition = ( cpe.isPositionChange() == true || cpe.isNothingChange() == true );
	
	switch ( column ) {
		case CncPathListEntryListCtrl::COL_TYPE:			return wxString::Format("%d", 		cpe.type);
		case CncPathListEntryListCtrl::COL_REF: 			return wxString::Format("%lld", 	cpe.pathListReference);
		case CncPathListEntryListCtrl::COL_CLD_ID:			return wxString::Format(fmt, 		cpe.clientId);
		
		case CncPathListEntryListCtrl::COL_F:				return displaySpeed == true ? 		wxString::Format("%4.1lf %c", 	cpe.feedSpeed_MM_MIN, cnc::getCncSpeedTypeAsCharacter(cpe.feedSpeedMode)) : _("");
		
		case CncPathListEntryListCtrl::COL_DISTANCE_X:		return displayPosition == true ?	wxString::Format("%10.3lf", 	cpe.entryDistance.getX()) 	: _("");
		case CncPathListEntryListCtrl::COL_DISTANCE_Y: 		return displayPosition == true ?	wxString::Format("%10.3lf", 	cpe.entryDistance.getY()) 	: _("");
		case CncPathListEntryListCtrl::COL_DISTANCE_Z: 		return displayPosition == true ?	wxString::Format("%10.3lf", 	cpe.entryDistance.getZ()) 	: _("");
		case CncPathListEntryListCtrl::COL_TARGET_X: 		return displayPosition == true ?	wxString::Format("%10.3lf", 	cpe.entryTarget.getX()) 	: _("");
		case CncPathListEntryListCtrl::COL_TARGET_Y: 		return displayPosition == true ?	wxString::Format("%10.3lf", 	cpe.entryTarget.getY()) 	: _("");
		case CncPathListEntryListCtrl::COL_TARGET_Z: 		return displayPosition == true ?	wxString::Format("%10.3lf", 	cpe.entryTarget.getZ()) 	: _("");
		case CncPathListEntryListCtrl::COL_TOTAL_DISTANCE: 	return displayPosition == true ?	wxString::Format("%10.3lf", 	cpe.totalDistance) 			: _("");
	}

	return _("");
}
/////////////////////////////////////////////////////////////
int CncPathListEntryListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	if ( column == COL_TYPE ) {
		
		if ( isItemValid(item) == false )
			return -1;
			
		const CncPathListEntry& cpe = pathLists.at(item);
		switch ( cpe.type ) {
			case CncPathListEntry::Type::CHG_NOTHING:		return 0;
			case CncPathListEntry::Type::CHG_CLIENTID:		return 1;
			case CncPathListEntry::Type::CHG_SPEED:			return 2;
			case CncPathListEntry::Type::CHG_POSITION:		return 3;
		}
	}
	
	return -1;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncPathListEntryListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return (wxListItemAttr*)(&defaultItemAttr);
		
	const CncPathListEntry& cpe = pathLists.at(item);

	if 		( cpe.type == CncPathListEntry::Type::CHG_NOTHING ) 	return (wxListItemAttr*)(&initialItemAttr);
	else if	( cpe.type == CncPathListEntry::Type::CHG_CLIENTID )	return (wxListItemAttr*)(&clientIdItemAttr);
	else if ( cpe.type == CncPathListEntry::Type::CHG_SPEED ) 		return (wxListItemAttr*)(&speedItemAttr);

	// this indicates to use the default style
	return (wxListItemAttr*)(&defaultItemAttr);
}
/////////////////////////////////////////////////////////////
void CncPathListEntryListCtrl::onSize(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////
	updateColumnWidth();

	event.Skip(true);
}
/////////////////////////////////////////////////////////////////////
void CncPathListEntryListCtrl::updateColumnWidth() {
/////////////////////////////////////////////////////////////////////
	if ( GetColumnCount() <= 0 )
		return;

	// avoid flicker
	if ( IsFrozen() == false )
		Freeze();

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

	if ( IsFrozen() == true )
		Thaw();
}
/////////////////////////////////////////////////////////////
void CncPathListEntryListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
	
	setLastSelection(item);
	
	long ln;
	GetItemText(item, COL_SEARCH).ToLong(&ln);
	
	SelectEventBlocker blocker(this);
	THE_APP->tryToSelectClientId(ln, MainFrame::TemplateSelSource::TSS_PATH_LIST);
}
/////////////////////////////////////////////////////////////
void CncPathListEntryListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	// currently nothing todo
}
/////////////////////////////////////////////////////////////
bool CncPathListEntryListCtrl::isItemValid(long item) const {
/////////////////////////////////////////////////////////////
	return item >= 0 && item < (long)(pathLists.size());
}
/////////////////////////////////////////////////////////////
bool CncPathListEntryListCtrl::searchReference(const wxString& what) {
/////////////////////////////////////////////////////////////
	long ln;
	what.ToLong(&ln);
	
	return searchReferenceById(ln);
}
/////////////////////////////////////////////////////////////
bool CncPathListEntryListCtrl::searchReferenceById(const long id) {
/////////////////////////////////////////////////////////////
	long item = -1;
	
	// skip all item which should be hidden
	for ( auto it = pathLists.begin(); it != pathLists.end(); ++it ) {
		item++;
		
		if ( it->clientId == id ) {
			selectItem(item, true);
			return true;
		}
	}

	return false;
}
/////////////////////////////////////////////////////////////
void CncPathListEntryListCtrl::clearAll() {
/////////////////////////////////////////////////////////////
	pathLists.clear();
	clear();
}
/////////////////////////////////////////////////////////////
void CncPathListEntryListCtrl::addPathListEntry(const CncPathListEntry& cpe) {
/////////////////////////////////////////////////////////////
	pathLists.push_back(cpe);
	SetItemCount(pathLists.size());
}
