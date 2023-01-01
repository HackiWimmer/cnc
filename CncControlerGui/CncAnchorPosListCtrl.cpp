#include "CncAnchorPosListCtrl.h"

// ----------------------------------------------------------------------------
// CncAnchorPosition Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(CncAnchorPosListCtrl, CncLargeScaledListCtrl)
	EVT_LIST_ITEM_SELECTED	(wxID_ANY, 	CncAnchorPosListCtrl::onSelectListItem	)
	EVT_LIST_ITEM_ACTIVATED	(wxID_ANY, 	CncAnchorPosListCtrl::onActivateListItem	)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncAnchorPosListCtrl::CncAnchorPosListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl	(parent, style)
, anchorMap					()
, anchorIdx					()
, caller					(NULL)
, defaultItemAttr			()
, defaultItemAttrSelected	()
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn(" ",		wxLIST_FORMAT_LEFT, 	 20);
	AppendColumn("Name:",	wxLIST_FORMAT_LEFT, 	160);
	AppendColumn("*",		wxLIST_FORMAT_CENTRE, 	 40);
	AppendColumn("Type",	wxLIST_FORMAT_RIGHT,	 80);
	AppendColumn("X",		wxLIST_FORMAT_RIGHT,	 80);
	AppendColumn("Y",		wxLIST_FORMAT_RIGHT,	 80);
	AppendColumn("Z",		wxLIST_FORMAT_RIGHT,	 80);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(0, 0, 0));
	SetTextColour(wxColour(255, 255, 255));
	SetItemCount(0);
	
	defaultItemAttr.SetBackgroundColour(GetBackgroundColour());
	defaultItemAttr.SetFont(font);
	defaultItemAttr.SetTextColour(GetTextColour());
	
	defaultItemAttrSelected = defaultItemAttr;
	defaultItemAttrSelected.SetTextColour(*wxYELLOW);
	defaultItemAttrSelected.SetFont(defaultItemAttrSelected.GetFont().Bold());
}
/////////////////////////////////////////////////////////////
CncAnchorPosListCtrl::~CncAnchorPosListCtrl() {
/////////////////////////////////////////////////////////////
	anchorMap.clear();
	anchorIdx.clear();
}
/////////////////////////////////////////////////////////////////////
bool CncAnchorPosListCtrl::isItemValid(long item) const {
/////////////////////////////////////////////////////////////////////
	if ( item < 0 || item > (long)(anchorIdx.size() - 1) )
		return false;
	
	return true;
}
/////////////////////////////////////////////////////////////
bool CncAnchorPosListCtrl::hasAnchor(const wxString& name) const {
/////////////////////////////////////////////////////////////
	auto it = anchorMap.find (name);
	
	return it != anchorMap.end();
}
/////////////////////////////////////////////////////////////
bool CncAnchorPosListCtrl::isAnchorFixed(const wxString& name) const {
/////////////////////////////////////////////////////////////
	auto it = anchorMap.find (name);
	if ( it == anchorMap.end() )
		return true;
	
	return it->second.fixed;
}
/////////////////////////////////////////////////////////////
int CncAnchorPosListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	return -1;
}
/////////////////////////////////////////////////////////////////////
wxString CncAnchorPosListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return _("");
		
	const AnchorInfo ai = anchorMap.at(anchorIdx.at(item));
	
	switch ( column )
	{
		case COL_NAME:	return _(ai.name);
		case COL_MODE:	return _(ai.fixed			? "*"          : " ");
		case COL_SHOW:	return _(ai.show 			? "Y"          : "N");
		case COL_TYPE:	return _(ai.isPhysically()	? "Physically" : "Logically");
		case COL_X:		return wxString::Format("%.3lf", ai.pos.getX());
		case COL_Y:		return wxString::Format("%.3lf", ai.pos.getY());
		case COL_Z:		return wxString::Format("%.3lf", ai.pos.getZ());
	}
	
	return _("");
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncAnchorPosListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return (wxListItemAttr*)(NULL);
		
	const AnchorInfo ai = anchorMap.at(anchorIdx.at(item));
	const bool b = (item == getLastSelection());
	
	return (wxListItemAttr*)( b ? (&defaultItemAttrSelected)  : (&defaultItemAttr) );
}
/////////////////////////////////////////////////////////////////////
void CncAnchorPosListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////////////
	long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
	
	if ( isItemValid(item) == false )
		return;
	
	setLastSelection(item);
	
	const AnchorInfo ai = anchorMap.at(anchorIdx.at(item));
	
	if ( caller )
		caller->notifySelection(ai);
}
/////////////////////////////////////////////////////////////////////
void CncAnchorPosListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////////////
	long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
	
	if ( isItemValid(item) == false )
		return;
		
	const AnchorInfo ai = anchorMap.at(anchorIdx.at(item));
	
	if ( caller )
		caller->notifyActivation(ai);
}
/////////////////////////////////////////////////////////////////////
void CncAnchorPosListCtrl::addAnchor(const AnchorInfo& ai) {
/////////////////////////////////////////////////////////////////////
	modAnchor(ai);
}
/////////////////////////////////////////////////////////////////////
void CncAnchorPosListCtrl::modAnchor(const AnchorInfo& ai) {
/////////////////////////////////////////////////////////////////////
	anchorMap[ai.name] = ai;
	updateIndex();
	
	SetItemCount(anchorMap.size());
	selAnchor(ai.name);
	Refresh();
}
/////////////////////////////////////////////////////////////////////
void CncAnchorPosListCtrl::delAnchor(const wxString& name) {
/////////////////////////////////////////////////////////////////////
	auto it = anchorMap.find (name);
	if ( it == anchorMap.end() )
		return;
		
	anchorMap.erase(it);
	
	updateIndex();
	SetItemCount(anchorMap.size());
	selAnchor("");
	Refresh();
}
/////////////////////////////////////////////////////////////////////
void CncAnchorPosListCtrl::selAnchor(const wxString& name) {
/////////////////////////////////////////////////////////////////////
	if ( name.IsEmpty() == true ) 
	{
		selectItem(0, true);
		return;
	}
	
	auto it = anchorMap.find(name);
	if ( it == anchorMap.end() )
		return;
	
	selectItem(it->second.index, true);
}
/////////////////////////////////////////////////////////////////////
void CncAnchorPosListCtrl::updateIndex() {
/////////////////////////////////////////////////////////////////////
	anchorIdx.clear();
	for ( auto it = anchorMap.begin(); it != anchorMap.end(); ++it )
	{
		it->second.index = anchorIdx.size();
		anchorIdx.push_back(it->first);
	}
}


