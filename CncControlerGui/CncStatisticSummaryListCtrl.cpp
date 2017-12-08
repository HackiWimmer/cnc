#include <iostream>
#include <wx/imaglist.h>
#include "wxcrafter.h"
#include "CncArduino.h"
#include "CncStatisticSummaryListCtrl.h"

/////////////////////////////////////////////////////////////
CncStatisticSummaryListCtrl::CncStatisticSummaryListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, defaultItemAttr()
, valueItemAttr()
, keyMap()
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Key", 		wxLIST_FORMAT_LEFT, 	150);
	AppendColumn("Values",		wxLIST_FORMAT_LEFT, 	100);
	AppendColumn("A",			wxLIST_FORMAT_RIGHT, 	 80);
	AppendColumn("B",			wxLIST_FORMAT_RIGHT, 	 80);
	AppendColumn("C",			wxLIST_FORMAT_RIGHT, 	 80);
	AppendColumn("D",			wxLIST_FORMAT_RIGHT, 	 80);
	AppendColumn("Unit", 		wxLIST_FORMAT_LEFT,	 	 60);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::NORMAL);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTION));
	
	defaultItemAttr.SetBackgroundColour(GetBackgroundColour());
	defaultItemAttr.SetFont(font);
	defaultItemAttr.SetTextColour(GetTextColour());
	
	wxFont valueFont(font);
	//valueFont.MakeBold();
	valueItemAttr.SetBackgroundColour(wxColour(238, 238, 238));
	valueItemAttr.SetFont(valueFont);
	valueItemAttr.SetTextColour(GetTextColour());
}
/////////////////////////////////////////////////////////////
CncStatisticSummaryListCtrl::~CncStatisticSummaryListCtrl() {
/////////////////////////////////////////////////////////////
	keyMap.clear();
}
/////////////////////////////////////////////////////////////
void CncStatisticSummaryListCtrl::addKey(const wxString& key, const wxString& content, const wxString& unit) {
/////////////////////////////////////////////////////////////
	// search if key already exists
	auto it = keyMap.find(key);
	if ( it != keyMap.end() )
		return;
	
	addKeyIntern(key, content, unit);
}
/////////////////////////////////////////////////////////////
void CncStatisticSummaryListCtrl::addKeyIntern(const wxString& key, const wxString& content, const wxString& unit) {
/////////////////////////////////////////////////////////////
	// add row
	CncColumContainer cc(CncStatisticSummaryListCtrl::TOTAL_COL_COUNT);
	cc.updateItem(CncStatisticSummaryListCtrl::COL_KEY, 	key);
	cc.updateItem(CncStatisticSummaryListCtrl::COL_CONT, 	content);
	cc.updateItem(CncStatisticSummaryListCtrl::COL_UNIT, 	unit);
	cc.updateItem(CncStatisticSummaryListCtrl::COL_VAL1, 	_(""));
	cc.updateItem(CncStatisticSummaryListCtrl::COL_VAL2, 	_(""));
	cc.updateItem(CncStatisticSummaryListCtrl::COL_VAL3, 	_(""));
	cc.updateItem(CncStatisticSummaryListCtrl::COL_VAL4, 	_(""));
	appendItem(cc);
	
	keyMap[key] = getItemCount() - 1;
}
/////////////////////////////////////////////////////////////
void CncStatisticSummaryListCtrl::updateValues(const wxString& key, const wxString& v1, const wxString& v2, const wxString& v3, const wxString& v4) {
/////////////////////////////////////////////////////////////
	// search if key already exists
	auto it = keyMap.find(key);
	if ( it == keyMap.end() )
		addKeyIntern(key, "???", "???");
		
	updateItem(it->second, CncStatisticSummaryListCtrl::COL_VAL1, v1);
	updateItem(it->second, CncStatisticSummaryListCtrl::COL_VAL2, v2);
	updateItem(it->second, CncStatisticSummaryListCtrl::COL_VAL3, v3);
	updateItem(it->second, CncStatisticSummaryListCtrl::COL_VAL4, v4);
}
/////////////////////////////////////////////////////////////
void CncStatisticSummaryListCtrl::resetValues() {
/////////////////////////////////////////////////////////////
	for (long i = 0; i <getItemCount(); i++ ){
		updateItem(i, CncStatisticSummaryListCtrl::COL_VAL1, _T(""));
		updateItem(i, CncStatisticSummaryListCtrl::COL_VAL2, _T(""));
		updateItem(i, CncStatisticSummaryListCtrl::COL_VAL3, _T(""));
		updateItem(i, CncStatisticSummaryListCtrl::COL_VAL4, _T(""));
	}
	
	Refresh();
	Update();
}
/////////////////////////////////////////////////////////////
int CncStatisticSummaryListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	// no icons available
	return -1;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncStatisticSummaryListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	// this indicates to use the default style
	return NULL;
}
///////////////////////////////////////////////////////////////////
wxListItemAttr* CncStatisticSummaryListCtrl::OnGetItemColumnAttr(long item, long column) const {
///////////////////////////////////////////////////////////////////
	switch ( column ) {
		case COL_VAL1:
		case COL_VAL2:
		case COL_VAL3:
		case COL_VAL4:	return (wxListItemAttr*)(&valueItemAttr);
	}
	
	return (wxListItemAttr*)(&defaultItemAttr);
}
