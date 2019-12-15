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
	EVT_LIST_ITEM_SELECTED(wxID_ANY, 	CncGCodeSequenceListCtrl::onSelectListItem)
	EVT_LIST_ITEM_ACTIVATED(wxID_ANY, 	CncGCodeSequenceListCtrl::onActivateListItem)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncGCodeSequenceListCtrl::CncGCodeSequenceListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, defaultItemAttr()
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("ClientId", 		wxLIST_FORMAT_LEFT,  wxLIST_AUTOSIZE);
	AppendColumn("Cmd", 			wxLIST_FORMAT_LEFT,  40);
	AppendColumn("X", 				wxLIST_FORMAT_RIGHT, wxLIST_AUTOSIZE);
	AppendColumn("Y", 				wxLIST_FORMAT_RIGHT, wxLIST_AUTOSIZE);
	AppendColumn("Z", 				wxLIST_FORMAT_RIGHT, wxLIST_AUTOSIZE);
	AppendColumn("F", 				wxLIST_FORMAT_RIGHT, wxLIST_AUTOSIZE);
	AppendColumn("Further ...", 	wxLIST_FORMAT_LEFT,  400);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::NORMAL);
	
	//wxFont font(9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(191, 205, 219));
	
	defaultItemAttr.SetBackgroundColour(GetBackgroundColour());
	defaultItemAttr.SetFont(font);
	defaultItemAttr.SetTextColour(GetTextColour());
}
/////////////////////////////////////////////////////////////
CncGCodeSequenceListCtrl::~CncGCodeSequenceListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
int CncGCodeSequenceListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	return -1;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncGCodeSequenceListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	// this indicates to use the default style
	return (wxListItemAttr*)(&defaultItemAttr);
}
/////////////////////////////////////////////////////////////
void CncGCodeSequenceListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
		
	setLastSelection(item);
	
	long ln;
	getRow(item).getItem(COL_SEARCH).ToLong(&ln);
	
	SelectEventBlocker blocker(this);
	APP_PROXY::tryToSelectClientId(ln, ClientIdSelSource::ID::TSS_GCODE_SEQ);
}
/////////////////////////////////////////////////////////////
void CncGCodeSequenceListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	// currently nothing todo
}
/////////////////////////////////////////////////////////////
void CncGCodeSequenceListCtrl::addBlock(const GCodeBlock& gcb) {
/////////////////////////////////////////////////////////////
	#warning
	//return;

	static wxString value;
	static CncColumContainer cc(CncGCodeSequenceListCtrl::TOTAL_COL_COUNT);
	
	cc.updateItem(CncGCodeSequenceListCtrl::COL_ID, 	wxString::Format(globalStrings.gcodeSeqRefFormat, gcb.clientID));
	cc.updateItem(CncGCodeSequenceListCtrl::COL_CMD, 	gcb.getCmdAsString(value));
	
	cc.updateItem(CncGCodeSequenceListCtrl::COL_X, 		(gcb.hasX() ? wxString::Format("% 8.3f", gcb.x) : ""));
	cc.updateItem(CncGCodeSequenceListCtrl::COL_Y, 		(gcb.hasY() ? wxString::Format("% 8.3f", gcb.y) : ""));
	cc.updateItem(CncGCodeSequenceListCtrl::COL_Z, 		(gcb.hasZ() ? wxString::Format("% 8.3f", gcb.z) : ""));
	cc.updateItem(CncGCodeSequenceListCtrl::COL_F, 		(gcb.hasF() ? wxString::Format("% 8.3f", gcb.f) : ""));
	
	cc.updateItem(CncGCodeSequenceListCtrl::COL_MORE, 	gcb.traceMore(value));
	
	appendItem(cc);
}
/////////////////////////////////////////////////////////////
bool CncGCodeSequenceListCtrl::searchReference(const wxString& what) {
/////////////////////////////////////////////////////////////
	long ret = searchRow(what, COL_SEARCH);
	
	if ( ret >= 0 )
		selectItem(ret);
	
	return (ret >= 0 );
}
/////////////////////////////////////////////////////////////
bool CncGCodeSequenceListCtrl::searchReferenceById(const long id) {
/////////////////////////////////////////////////////////////
	wxString what(wxString::Format(globalStrings.gcodeSeqRefFormat, id));
	return searchReference(what);
}
