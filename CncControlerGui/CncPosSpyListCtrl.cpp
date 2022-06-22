#include <iostream>
#include <wx/imaglist.h>
#include "wxCrafterImages.h"
#include "GlobalStrings.h"
#include "CncArduino.h"
#include "CncCommon.h"
#include "CncNumberFormatter.h"
#include "MainFrame.h"
#include "MainFrameProxy.h"
#include "CncPosSpyListCtrl.h"

extern GlobalConstStringDatabase globalStrings;

// ----------------------------------------------------------------------------
// CncPathListEntryListCtrl Event Table
// ----------------------------------------------------------------------------
wxDEFINE_EVENT(wxEVT_POS_SPY_DISPLAY_TIMER,  wxTimerEvent);

wxBEGIN_EVENT_TABLE(CncPosSpyListCtrl, CncLargeScaledListCtrl)
	EVT_PAINT(								CncPosSpyListCtrl::onPaint)
	EVT_LIST_ITEM_SELECTED(wxID_ANY, 		CncPosSpyListCtrl::onSelectListItem)
	EVT_LIST_ITEM_ACTIVATED(wxID_ANY, 		CncPosSpyListCtrl::onActivateListItem)
	EVT_TIMER(wxEVT_POS_SPY_DISPLAY_TIMER,	CncPosSpyListCtrl::onDisplayTimer)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncPosSpyListCtrl::CncPosSpyListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, posType						(PT_CTL)
, cncUnit						(CncMetric)
, appPositionEntries			()
, ctlPositionEntries			()
, displayFactX					(1.0)
, displayFactY					(1.0)
, displayFactZ					(1.0)
, posFormat						()
, majorPosPid					(wxString::Format("%d", (int)PID_XYZ_POS_MAJOR))
, majorPosItemAttr				()
, displayTimer					(this, wxEVT_POS_SPY_DISPLAY_TIMER)
, displayTimerInterval			(333)
/////////////////////////////////////////////////////////////
{
	posType	= THE_APP->GetCbContentPosSpy()->GetSelection() == 0 ? PT_APP : PT_CTL;
	setUnit(cncUnit);
	
	// add colums
	AppendColumn("Type", 			wxLIST_FORMAT_LEFT, 	 44);
	AppendColumn("Reference", 		wxLIST_FORMAT_RIGHT, 	 70);
	AppendColumn("Mode",			wxLIST_FORMAT_CENTER, 	 50);
	AppendColumn("Speed [mm/min]", 	wxLIST_FORMAT_LEFT, 	100);
	AppendColumn("X-Target", 		wxLIST_FORMAT_RIGHT,	wxLIST_AUTOSIZE);
	AppendColumn("Y-Target", 		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Z-Target", 		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::REVERSE);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(191, 205, 219));
	
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibPosSpy().Bitmap("BMP_PID_MAJOR"));			// 0
	imageList->Add(ImageLibPosSpy().Bitmap("BMP_PID_DETAIL"));			// 1
	imageList->Add(ImageLibPosSpy().Bitmap("BMP_TYPE_RAPID"));			// 2
	imageList->Add(ImageLibPosSpy().Bitmap("BMP_TYPE_WORK"));			// 3
	imageList->Add(ImageLibPosSpy().Bitmap("BMP_TYPE_MAX"));			// 4
	imageList->Add(ImageLibPosSpy().Bitmap("BMP_TYPE_USER_DEFINED"));	// 5
	imageList->Add(ImageLibPosSpy().Bitmap("BMP_POS"));					// 6
	
	AssignImageList(imageList, wxIMAGE_LIST_SMALL);
	
	wxFont majorPosFont(font);
	majorPosFont.MakeBold();
	majorPosItemAttr.SetBackgroundColour(GetBackgroundColour());
	majorPosItemAttr.SetFont(majorPosFont);
	majorPosItemAttr.SetTextColour(wxColour(0, 0, 64));
}
/////////////////////////////////////////////////////////////
CncPosSpyListCtrl::~CncPosSpyListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
void CncPosSpyListCtrl::setUnit(CncUnit u) { 
/////////////////////////////////////////////////////////////
	cncUnit 		= u;
	posFormat 		= cncUnit == CncMetric ? "%.3lf"                       : "%ld";
	displayFactX	= cncUnit == CncMetric ? THE_CONFIG->getDisplayFactX() : 1.0;
	displayFactY	= cncUnit == CncMetric ? THE_CONFIG->getDisplayFactY() : 1.0;
	displayFactZ	= cncUnit == CncMetric ? THE_CONFIG->getDisplayFactZ() : 1.0;
}
/////////////////////////////////////////////////////////////
void CncPosSpyListCtrl::setPositionType(CncPosSpyListCtrl::PosType pt) {
/////////////////////////////////////////////////////////////
	posType = pt;
	
	switch( posType )
	{
		case PT_APP:	SetItemCount(appPositionEntries.size()); break;
		case PT_CTL:	SetItemCount(ctlPositionEntries.size()); break;
	}
	
	if ( IsShownOnScreen() == true )
		if ( displayTimer.IsRunning() == false )
			displayTimer.Start(displayTimerInterval, wxTIMER_ONE_SHOT);
}
/////////////////////////////////////////////////////////////
void CncPosSpyListCtrl::addAppPosition(unsigned char pid, long id, char speedMode, double cfgSpeedValue, double curSpeedValue, const CncLongPosition& pos) {
/////////////////////////////////////////////////////////////
	appPositionEntries.push_back(std::move(PositionEntry(pid, id, speedMode, cfgSpeedValue, curSpeedValue, pos)));
	
	// To minimize the performance impact of SetItemCount(...)
	// for large list content, it will be called ones at onPaint(...).
	//if ( posType == PT_APP )	SetItemCount(appPositionEntries.size());
	//else						SetItemCount(ctlPositionEntries.size());
	
	if ( IsShownOnScreen() == true )
		if ( displayTimer.IsRunning() == false )
			displayTimer.Start(displayTimerInterval, wxTIMER_ONE_SHOT);
}
/////////////////////////////////////////////////////////////
void CncPosSpyListCtrl::addCtlPosition(unsigned char pid, long id, char speedMode, double cfgSpeedValue, double curSpeedValue, const CncLongPosition& pos) {
/////////////////////////////////////////////////////////////
	ctlPositionEntries.push_back(std::move(PositionEntry(pid, id, speedMode, cfgSpeedValue, curSpeedValue, pos)));
	
	// To minimize the performance impact of SetItemCount(...)
	// for large list content, it will be called ones at onPaint(...).
	//if ( posType == PT_APP )	SetItemCount(appPositionEntries.size());
	//else						SetItemCount(ctlPositionEntries.size());
	
	if ( IsShownOnScreen() == true )
		if ( displayTimer.IsRunning() == false )
			displayTimer.Start(displayTimerInterval, wxTIMER_ONE_SHOT);
}
/////////////////////////////////////////////////////////////
void CncPosSpyListCtrl::clearAll() {
/////////////////////////////////////////////////////////////
	appPositionEntries.clear();
	ctlPositionEntries.clear();
	clear();
}
/////////////////////////////////////////////////////////////
long CncPosSpyListCtrl::translateItem(long item) const {
/////////////////////////////////////////////////////////////
	if ( isItemValid(item) == true ) {
		switch( posType ) {
			case PT_APP:	return appPositionEntries.size() - 1 - item;
			case PT_CTL:	return ctlPositionEntries.size() - 1 - item;
		}
	}
	
	return -1;
}
/////////////////////////////////////////////////////////////
bool CncPosSpyListCtrl::isItemValid(long item) const {
/////////////////////////////////////////////////////////////
	switch( posType ) {
		case PT_APP:	return item >= 0 && item < (long)(appPositionEntries.size());
		case PT_CTL:	return item >= 0 && item < (long)(ctlPositionEntries.size());
	}
	
	return false;
}
/////////////////////////////////////////////////////////////
wxString CncPosSpyListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////
	static wxString speedInfo;

	if ( isItemValid(item) == false )
		return _("");
		
	const long tItem 		= translateItem(item);
	const PositionEntry& pe = posType == PT_APP ? appPositionEntries.at(tItem) : ctlPositionEntries.at(tItem);
	
	if ( pe.currentSpeedValue < 0.0 )	speedInfo.assign(wxString::Format("%.1lf / %s",    pe.configuredSpeedValue, _maxSpeedLabel));
	else								speedInfo.assign(wxString::Format("%.1lf / %.1lf", pe.configuredSpeedValue, pe.currentSpeedValue));

	switch ( column ) {
		case COL_PID:	return wxString::Format("%d", 							pe.pid);
		case COL_REF:	return wxString::Format(globalStrings.posSpyRefFormat, 	pe.id);
		case COL_T:		return wxString::Format("%c", 							pe.speedMode);
		case COL_F:		return speedInfo;
		case COL_X:		return wxString::Format(posFormat, 						pe.pos.getX() * displayFactX);
		case COL_Y:		return wxString::Format(posFormat, 						pe.pos.getY() * displayFactY);
		case COL_Z:		return wxString::Format(posFormat, 						pe.pos.getZ() * displayFactZ);
	}
	
	return _("");
}
/////////////////////////////////////////////////////////////
int CncPosSpyListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	switch ( column ) {
		case COL_PID: 	if ( GetItemText(translateItem(item), column) == majorPosPid )	return 0;
						else															return 1;
						
		case COL_REF:	return -1;
		
		case COL_T:		switch( (char)(GetItemText(translateItem(item), column))[0] ) {
							case 'R':	return 2;
							case 'W':	return 3;
							case 'M':	return 4;
							case 'U':	return 5;
						}
						
		case COL_F:		return -1;
		
		case COL_X:
		case COL_Y:
		case COL_Z:		return 6;
	}

	return -1;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncPosSpyListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	if ( GetItemText(translateItem(item), COL_PID) == majorPosPid )
		return (wxListItemAttr*)(&majorPosItemAttr);
	
	// this indicates to use the default style
	return NULL;
}
/////////////////////////////////////////////////////////////
bool CncPosSpyListCtrl::searchReference(const wxString& what) {
/////////////////////////////////////////////////////////////
	long ret = searchRow(what, COL_SEARCH);
	
	if ( ret >= 0 )
		selectItem(ret);
	
	return (ret >= 0 );
}
/////////////////////////////////////////////////////////////
bool CncPosSpyListCtrl::searchReferenceById(const long id) {
/////////////////////////////////////////////////////////////
	wxString what(wxString::Format(globalStrings.posSpyRefFormat, id));
	return searchReference(what);
}
/////////////////////////////////////////////////////////////
void CncPosSpyListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
	
	if ( isItemValid(item) == false )
		return;
	
	setLastSelection(item);
	
	const long tItem 		= translateItem(item);
	const PositionEntry& pe = posType == PT_APP ? appPositionEntries.at(tItem) : ctlPositionEntries.at(tItem);
	
	SelectEventBlocker blocker(this);
	APP_PROXY::tryToSelectClientId(pe.id, ClientIdSelSource::ID::TSS_POS_SPY);
}
/////////////////////////////////////////////////////////////
void CncPosSpyListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	// currently nothing todo
}
//////////////////////////////////////////////////
void CncPosSpyListCtrl::onDisplayTimer(wxTimerEvent& event) {
//////////////////////////////////////////////////
	Refresh();
}
//////////////////////////////////////////////////
void CncPosSpyListCtrl::onPaint(wxPaintEvent& event) {
//////////////////////////////////////////////////
	if ( posType == PT_APP )	SetItemCount(appPositionEntries.size());
	else						SetItemCount(ctlPositionEntries.size());

	long items = 0;
	if ( THE_APP->GetBtTogglePosSpy() )
		items = GetItemCount();
	
	THE_APP->GetPositionSpyCount()->ChangeValue(CncNumberFormatter::toString(items));
	THE_APP->GetPositionSpyCount()->SetToolTip(THE_APP->GetPositionSpyCount()->GetLabel());
	
	event.Skip();
}
