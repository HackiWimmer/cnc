#include <iostream>
#include <wx/imaglist.h>
#include "wxCrafterImages.h"
#include "GlobalStrings.h"
#include "CncConfig.h"
#include "MainFrameProxy.h"
#include "CncAutoFreezer.h"
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
, defaultItemAttr			()
, initialItemAttr			()
, clientIdItemAttr			()
, speedItemAttr				()
, toolItemAttr				()
, defaultItemAttrSelected	()
, initialItemAttrSelected	()
, clientIdItemAttrSelected	()
, speedItemAttrSelected		()
, toolItemAttrSelected		()
, pathLists					()
, showAllFlag				(false)
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Content",	 		wxLIST_FORMAT_LEFT, 	64);
	AppendColumn("PathList ID",		wxLIST_FORMAT_LEFT, 	120);
	AppendColumn("Client ID", 		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("F [mm/min]",		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("S [U/min]",		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	
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
	
	SetBackgroundColour(wxColour(  0,   0,   0));
	SetTextColour(*wxLIGHT_GREY);
	
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
	speedItemAttr.SetTextColour(wxColour(125, 133, 221));
	
	toolItemAttr.SetBackgroundColour(GetBackgroundColour());
	toolItemAttr.SetFont(font.Bold());
	toolItemAttr.SetTextColour(wxColour( 64, 128, 128));

	defaultItemAttrSelected		= defaultItemAttr;
	initialItemAttrSelected		= initialItemAttr;
	clientIdItemAttrSelected	= clientIdItemAttr;
	speedItemAttrSelected		= speedItemAttr;
	toolItemAttrSelected		= toolItemAttr;
	
	defaultItemAttrSelected.SetTextColour(*wxYELLOW);
	defaultItemAttrSelected.SetFont(defaultItemAttrSelected.GetFont().Bold());

	initialItemAttrSelected.SetTextColour(*wxYELLOW);
	initialItemAttrSelected.SetFont(initialItemAttrSelected.GetFont().Bold());

	clientIdItemAttrSelected.SetTextColour(*wxYELLOW);
	clientIdItemAttrSelected.SetFont(clientIdItemAttrSelected.GetFont().Bold());
	
	speedItemAttrSelected.SetTextColour(*wxYELLOW);
	speedItemAttrSelected.SetFont(speedItemAttrSelected.GetFont().Bold());
	
	toolItemAttrSelected.SetTextColour(*wxYELLOW);
	toolItemAttrSelected.SetFont(toolItemAttrSelected.GetFont().Bold());
}
/////////////////////////////////////////////////////////////
CncPathListEntryListCtrl::~CncPathListEntryListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
void CncPathListEntryListCtrl::setFormatFlag(bool flag) { 
/////////////////////////////////////////////////////////////
	showAllFlag = !flag;
	Refresh();
}
/////////////////////////////////////////////////////////////
wxString CncPathListEntryListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////
	static const wxString fmt(globalStrings.pathListRefFormat);
	
	// ------------------------------------------------------
	auto formatDistance = [](double val) {
		if ( cnc::dblCmp::nu(val) == false )
			return wxString::Format("%10.3lf", val);
			
		return _("");
	};
	
	// ------------------------------------------------------
	auto formatTarget = [](double val) {
		return wxString::Format("%10.3lf", val);
	};
	
	if ( isItemValid(item) == false )
		return _("");
		
	const CncPathListEntry& cpe = pathLists.at(item);
	
	const bool displayRef			= showAllFlag ? true : ( cpe.pathListReference   >= 0    );
	const bool displayClientID		= showAllFlag ? true : ( cpe.hasClientIdChange() == true );
	const bool displayFeedSpeed		= showAllFlag ? true : ( cpe.hasSpeedChange()    == true );
	const bool displaySpindleSpeed	= showAllFlag ? true : ( cpe.hasToolChange()     == true );
	const bool displayPosition		= showAllFlag ? true : ( cpe.hasPositionChange() == true );
	
	wxString contStr;
	if ( cpe.isNothingChanged() )	contStr.append('L');
	if ( cpe.hasClientIdChange() )	contStr.append('C');
	if ( cpe.hasSpeedChange() )		contStr.append('F');
	if ( cpe.hasToolChange() )		contStr.append('S');
	if ( cpe.hasPositionChange() )	contStr.append('P');
	
	switch ( column ) {
		case CncPathListEntryListCtrl::COL_CONT:			return wxString::Format("%s", contStr);
		
		case CncPathListEntryListCtrl::COL_REF: 			return displayRef          == true ?	wxString::Format("%lld",		cpe.pathListReference)		: _("");
		case CncPathListEntryListCtrl::COL_CLD_ID:			return displayClientID     == true ?	wxString::Format(fmt,			cpe.clientId) 				: _("");
		case CncPathListEntryListCtrl::COL_DISTANCE_X:		return displayPosition     == true ?	formatDistance(cpe.entryDistance.getX())					: _("");
		case CncPathListEntryListCtrl::COL_DISTANCE_Y: 		return displayPosition     == true ?	formatDistance(cpe.entryDistance.getY())					: _("");
		case CncPathListEntryListCtrl::COL_DISTANCE_Z: 		return displayPosition     == true ?	formatDistance(cpe.entryDistance.getZ())					: _("");
		case CncPathListEntryListCtrl::COL_TARGET_X: 		return displayPosition     == true ?	formatTarget  (cpe.entryTarget.getX())						: _("");
		case CncPathListEntryListCtrl::COL_TARGET_Y: 		return displayPosition     == true ?	formatTarget  (cpe.entryTarget.getY())						: _("");
		case CncPathListEntryListCtrl::COL_TARGET_Z: 		return displayPosition     == true ?	formatTarget  (cpe.entryTarget.getZ())						: _("");
		case CncPathListEntryListCtrl::COL_TOTAL_DISTANCE: 	return displayPosition     == true ?	formatDistance(cpe.totalDistance)							: _("");
		
		case CncPathListEntryListCtrl::COL_F:				return displayFeedSpeed    == true ?	wxString::Format("%4.1lf %c",	cpe.feedSpeed_MM_MIN, cnc::getCncSpeedTypeAsCharacter(cpe.feedSpeedMode)) : _("");
		case CncPathListEntryListCtrl::COL_S:				return displaySpindleSpeed == true ?	wxString::Format("%4.1lf %s",	cpe.spindleSpeed_U_MIN, cpe.spindleState ? "ON" : "OFF") : _("");
	}

	return _("");
}
/////////////////////////////////////////////////////////////
int CncPathListEntryListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	if ( column == COL_CONT ) {
		
		if ( isItemValid(item) == false )
			return -1;
	}
	
	return -1;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncPathListEntryListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return (wxListItemAttr*)(&defaultItemAttr);
		
	const CncPathListEntry& cpe = pathLists.at(item);
	const bool b = (item == getLastSelection());
	
	if      ( cpe.hasPositionChange() )		return (wxListItemAttr*)( b ? (&defaultItemAttrSelected)  : (&defaultItemAttr) );
	else if	( cpe.hasClientIdChange() )		return (wxListItemAttr*)( b ? (&clientIdItemAttrSelected) : (&clientIdItemAttr) );
	else if ( cpe.hasSpeedChange() )		return (wxListItemAttr*)( b ? (&speedItemAttrSelected)    : (&speedItemAttr) );
	else if ( cpe.hasToolChange() )			return (wxListItemAttr*)( b ? (&toolItemAttrSelected)     : (&toolItemAttr) );
	
	return (wxListItemAttr*)( b ? (&initialItemAttrSelected)  : (&initialItemAttr) );
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
	
	int colWidthSum = 0;
	for ( int i = 0; i < GetColumnCount(); i++ )
	{
		if ( i == COL_STRECH )
			continue;

		colWidthSum += GetColumnWidth(i);
	}

	const int scrollbarWidth = 26;
	int size = GetSize().GetWidth()
	         - colWidthSum
			 - scrollbarWidth;

	SetColumnWidth(COL_STRECH, size);
}
/////////////////////////////////////////////////////////////
void CncPathListEntryListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
	
	if ( isItemValid(item) == false )
		return;
		
	setLastSelection(item);
	
	const CncPathListEntry& cpe = pathLists.at(item);
	
	SelectEventBlocker blocker(this);
	APP_PROXY::tryToSelectClientId(cpe.clientId, ClientIdSelSource::ID::TSS_PATH_LIST);
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
	
	for ( auto it = pathLists.begin(); it != pathLists.end(); ++it ) {
		// skip all items which should be hidden
		item++;
		
		if ( it->clientId == id ) {
			selectItem(item, true);
			return true;
		}
	}
	
	return false;
}
/////////////////////////////////////////////////////////////
bool CncPathListEntryListCtrl::skipToFirstReference() {
/////////////////////////////////////////////////////////////
	if ( pathLists.size() == 0 )
		return false;
	
	// skip forward to the first position change
	long item = 0;
	for ( auto it = pathLists.begin(); it != pathLists.end(); ++it ) {
		if ( it->hasPositionChange() )
			break;
			
		item++;
	}
	
	if ( getLastSelection() == item )
		{ wxBell(); return false; }
		
	return selectItem(item, true);
}
/////////////////////////////////////////////////////////////
bool CncPathListEntryListCtrl::skipToPrevReference() {
/////////////////////////////////////////////////////////////
	if ( pathLists.size() == 0 )
		return false;
		
	long item = getLastSelection();
	
	if ( isItemValid(item) == false )
		skipToLastReference();
		
	item = getLastSelection();
	if ( isItemValid(item) == false )
		{ wxBell(); return false; }
		
	const long refClientID	= pathLists.at(item).clientId;
	const long ritem		= pathLists.size() - 1 - item;

	for ( auto it = pathLists.rbegin() + ritem; it != pathLists.rend(); ++it ) {
		item--;
		
		if ( it->clientId != refClientID )
			break;
	}
	
	if ( isItemValid(item) == false )
		{ wxBell(); return false; }
	
	return selectItem(item, true);
}
/////////////////////////////////////////////////////////////
bool CncPathListEntryListCtrl::skipToNextReference() {
/////////////////////////////////////////////////////////////
	if ( pathLists.size() == 0 )
		return false;
		
	long item = getLastSelection();
	
	if ( isItemValid(item) == false )
		skipToFirstReference();
	
	item = getLastSelection();
	if ( isItemValid(item) == false )
		{ wxBell(); return false; }
	
	const long refClientID	= pathLists.at(item).clientId;
	for ( auto it = pathLists.begin() + item; it != pathLists.end(); ++it ) {
		if ( it->clientId != refClientID ) {
			// skip forward to the first position change
			if ( it->hasPositionChange() ) {
				// skip backwards to the previous position change
				if ( it->hasPositionChange() )
					break;
			}
		}
			
		item++;
	}
	
	if ( isItemValid(item) == false )
		{ wxBell(); return false; }
		
	return selectItem(item, true);
}
/////////////////////////////////////////////////////////////
bool CncPathListEntryListCtrl::skipToLastReference() {
/////////////////////////////////////////////////////////////
	if ( pathLists.size() == 0 )
		return false;

	const long refClientID	= pathLists.back().clientId;
	long item 				= wxNOT_FOUND;
	
	for ( auto it = pathLists.rbegin(); it != pathLists.rend(); ++it ) {
		if ( it->clientId != refClientID ) {
			// skip backwards to the previous position change
			if ( it->hasPositionChange() )
				break;
		}
		
		item = std::abs(std::distance(pathLists.rend(), it));
	}
	
	if ( getLastSelection() == item )
		{ wxBell(); return false; }
		
	return selectItem(item, true);
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
/////////////////////////////////////////////////////////////
void CncPathListEntryListCtrl::addPathListEntries(const CncPathListManager& cpm) {
/////////////////////////////////////////////////////////////
	for ( auto it = cpm.cbegin(); it != cpm.cend(); ++it)
	{
		const CncPathListEntry& cpe = *it;
		addPathListEntry(cpe);
	}
}


