#include <sstream>
#include <ostream>
#include "CncConfig.h"
#include "CncContext.h"
#include "CncContextListCtrl.h"

// ----------------------------------------------------------------------------
// CncContextListCtrl Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(CncContextListCtrl, CncLargeScaledListCtrl)
	EVT_SIZE(							CncContextListCtrl::onSize)
	EVT_LIST_ITEM_SELECTED(wxID_ANY, 	CncContextListCtrl::onSelectListItem)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncContextListCtrl::CncContextListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, keyControl(NULL)
, valControl(NULL)
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Key",	 		wxLIST_FORMAT_LEFT, 	280);
	AppendColumn("Value",		wxLIST_FORMAT_LEFT, 	800);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::NORMAL);
	
	wxFont font(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));
	SetFont(font);
	
	SetTextColour(*wxWHITE);
	SetBackgroundColour(*wxBLACK);
	
	SetItemCount(MAX_ITEM_COUNT);
}
/////////////////////////////////////////////////////////////
CncContextListCtrl::~CncContextListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
void CncContextListCtrl::setControls(wxTextCtrl* key, wxTextCtrl* val) {
/////////////////////////////////////////////////////////////
	keyControl = key;
	valControl = val;
}
/////////////////////////////////////////////////////////////
void CncContextListCtrl::onSize(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////
	updateColumnWidth();
	event.Skip(true);
}
/////////////////////////////////////////////////////////////////////
void CncContextListCtrl::updateColumnWidth() {
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
void CncContextListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
	
	setLastSelection(item);
	
	if ( keyControl != NULL )
		keyControl->ChangeValue(GetItemText(item, COL_KEY));
		
	if ( valControl != NULL )
		valControl->ChangeValue(GetItemText(item, COL_VAL));
}
/////////////////////////////////////////////////////////////
wxString CncContextListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////
	auto eval = [&](long column, const char* k, const char* v) {
		if ( column == COL_KEY )	return _(k);
		else						return _(v);
		
		return _("");
	};
	
	auto formatBool		= [](bool value) 		{ return wxString::Format("%s", value ? "Yes" : "No"); };
	auto formatInt		= [](int value) 		{ return wxString::Format("%d", value); };
	
	wxString s;
	long index = 0;
	if ( false ) {}
	
	else if ( index++ == item ) return eval(column, "CommandLine",          											THE_CONTEXT->traceCommandLineParameter(s));
	
	else if ( index++ == item ) return eval(column, "SecureModeInfo.UseIt",								formatBool		(THE_CONTEXT->secureModeInfo.useIt));
	else if ( index++ == item ) return eval(column, "SecureModeInfo.Active",							formatBool		(THE_CONTEXT->secureModeInfo.isActive));
	else if ( index++ == item ) return eval(column, "SecureModeInfo.ActivatedByStartup",				formatBool		(THE_CONTEXT->secureModeInfo.isActivatedByStartup));
	
	else if ( index++ == item ) return eval(column, "Common.ProbeMode",									formatBool		(THE_CONTEXT->isProbeMode()));
	else if ( index++ == item ) return eval(column, "Common.OnlineUpdateDrawPane",						formatBool		(THE_CONTEXT->isOnlineUpdateDrawPane()));
	else if ( index++ == item ) return eval(column, "Common.AllowEventHandling",						formatBool		(THE_CONTEXT->isAllowEventHandling()));
	else if ( index++ == item ) return eval(column, "Common.OnlineUpdateCoordinates",					formatBool		(THE_CONTEXT->isOnlineUpdateCoordinates()));
	else if ( index++ == item ) return eval(column, "Common.UpdateInterval",							formatInt		(THE_CONTEXT->getUpdateInterval()));
	
	else if ( index++ == item ) return eval(column, "Serial.CanGamePort",								formatBool		(THE_CONTEXT->canInteractiveMoveing()));
	else if ( index++ == item ) return eval(column, "Serial.CanSpeedMonitoring",						formatBool		(THE_CONTEXT->canSpeedMonitoring()));
	else if ( index++ == item ) return eval(column, "Serial.HasHardware",								formatBool		(THE_CONTEXT->hasHardware()));
	
	// ....
	
	wxASSERT ( index < MAX_ITEM_COUNT );
	return _("");
}
