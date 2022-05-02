#include <iostream>
#include <wx/imaglist.h>
#include "wxCrafterImages.h"
#include "GlobalStrings.h"
#include "CncConfig.h"
#include "MainFrameProxy.h"
#include "CncGCodeSequenceListCtrl.h"

extern GlobalConstStringDatabase globalStrings;

// ----------------------------------------------------------------------------
// CncGCodeSequenceListCtrl Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(CncGCodeSequenceListCtrl, CncLargeScaledListCtrl)
	EVT_PAINT				(			CncGCodeSequenceListCtrl::onPaint			)
	EVT_LIST_ITEM_SELECTED	(wxID_ANY, 	CncGCodeSequenceListCtrl::onSelectListItem	)
	EVT_LIST_ITEM_ACTIVATED	(wxID_ANY, 	CncGCodeSequenceListCtrl::onActivateListItem)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncGCodeSequenceListCtrl::CncGCodeSequenceListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl	(parent, style)
, defaultItemAttr			()
, defaultItemAttrSelected	()
, gcodes					()
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("ClientId", 		wxLIST_FORMAT_LEFT,  wxLIST_AUTOSIZE);
	AppendColumn("Cmd", 			wxLIST_FORMAT_LEFT,  40);
	AppendColumn("X", 				wxLIST_FORMAT_RIGHT, wxLIST_AUTOSIZE);
	AppendColumn("Y", 				wxLIST_FORMAT_RIGHT, wxLIST_AUTOSIZE);
	AppendColumn("Z", 				wxLIST_FORMAT_RIGHT, wxLIST_AUTOSIZE);
	AppendColumn("F", 				wxLIST_FORMAT_RIGHT, wxLIST_AUTOSIZE);
	AppendColumn("S", 				wxLIST_FORMAT_RIGHT, wxLIST_AUTOSIZE);
	AppendColumn("Further ...", 	wxLIST_FORMAT_LEFT,  400);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::NORMAL);
	
	//wxFont font(9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(*wxBLACK);
	SetForegroundColour(wxColour(227, 227, 227));
	SetTextColour(wxColour(227, 227, 227));
	
	defaultItemAttr.SetBackgroundColour(GetBackgroundColour());
	defaultItemAttr.SetFont(font);
	defaultItemAttr.SetTextColour(GetTextColour());
	
	defaultItemAttrSelected		= defaultItemAttr;
	defaultItemAttrSelected.SetTextColour(*wxYELLOW);
	defaultItemAttrSelected.SetFont(defaultItemAttrSelected.GetFont().Bold());
}
/////////////////////////////////////////////////////////////
CncGCodeSequenceListCtrl::~CncGCodeSequenceListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
void CncGCodeSequenceListCtrl::clearAll() {
/////////////////////////////////////////////////////////////
	gcodes.clear();
	clear();
}
/////////////////////////////////////////////////////////////
bool CncGCodeSequenceListCtrl::isItemValid(long item) const {
/////////////////////////////////////////////////////////////
	return item >= 0 && item < (long)(gcodes.size());
}
/////////////////////////////////////////////////////////////
wxString CncGCodeSequenceListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////
	static wxString value;
	
	if ( isItemValid(item) == false )
		return _("");
		
	const GCodeBlock& gcb = gcodes.at(item);
	
	switch ( column ) {
		
		case CncGCodeSequenceListCtrl::COL_ID: 		return wxString::Format(globalStrings.gcodeSeqRefFormat, gcb.clientID);
		case CncGCodeSequenceListCtrl::COL_CMD: 	return gcb.getCmdAsString(value);
		
		case CncGCodeSequenceListCtrl::COL_X:		return (gcb.hasX() ? wxString::Format("% 8.3f", gcb.x) : "");
		case CncGCodeSequenceListCtrl::COL_Y:		return (gcb.hasY() ? wxString::Format("% 8.3f", gcb.y) : "");
		case CncGCodeSequenceListCtrl::COL_Z:		return (gcb.hasZ() ? wxString::Format("% 8.3f", gcb.z) : "");
		case CncGCodeSequenceListCtrl::COL_F:		return (gcb.hasF() ? wxString::Format("% 8.1f", gcb.f) : "");
		case CncGCodeSequenceListCtrl::COL_S:		return (gcb.hasS() ? wxString::Format("% 8.1f", gcb.s) : "");
		
		case CncGCodeSequenceListCtrl::COL_MORE:	return gcb.traceMore(value);
	}
	
	return _("");
}
/////////////////////////////////////////////////////////////
int CncGCodeSequenceListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	return -1;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncGCodeSequenceListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	const bool b = (item == getLastSelection());

	// this indicates to use the default style
	return (wxListItemAttr*)( b ? (&defaultItemAttrSelected)  : (&defaultItemAttr) );
}
/////////////////////////////////////////////////////////////
void CncGCodeSequenceListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
		
	if ( isItemValid(item) == false )
		return;
		
	setLastSelection(item);
	
	const GCodeBlock& gcb = gcodes.at(item);
	
	SelectEventBlocker blocker(this);
	APP_PROXY::tryToSelectClientId(gcb.clientID, ClientIdSelSource::ID::TSS_GCODE_SEQ);
}
/////////////////////////////////////////////////////////////
void CncGCodeSequenceListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	// currently nothing todo
}
//////////////////////////////////////////////////
void CncGCodeSequenceListCtrl::onPaint(wxPaintEvent& event) {
//////////////////////////////////////////////////
	SetItemCount(gcodes.size());
	
	event.Skip();
}
/////////////////////////////////////////////////////////////
void CncGCodeSequenceListCtrl::addBlock(const GCodeBlock& gcb) {
/////////////////////////////////////////////////////////////
	gcodes.push_back(std::move(gcb));
	
	// To minimize the performance impact of SetItemCount(...)
	// for latge list content, it will be called ones at onPaint(...).
	//SetItemCount(gcodes.size());
}
/////////////////////////////////////////////////////////////
bool CncGCodeSequenceListCtrl::searchReference(const wxString& what) {
/////////////////////////////////////////////////////////////
	long ln;
	what.ToLong(&ln);
	
	return searchReferenceById(ln);
}
/////////////////////////////////////////////////////////////
bool CncGCodeSequenceListCtrl::searchReferenceById(const long id) {
/////////////////////////////////////////////////////////////
	long item = -1;
	
	for ( auto it = gcodes.begin(); it != gcodes.end(); ++it ) {
		// skip all items which should be hidden
		item++;
		
		if ( it->clientID == id ) {
			selectItem(item, true);
			return true;
		}
	}
	
	return false;
}
