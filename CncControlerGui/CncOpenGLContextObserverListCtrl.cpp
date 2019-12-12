#include "wx/menu.h"
#include "3D/GLContextCncPathBase.h"
#include "CncOpenGLContextObserverListCtrl.h"

/////////////////////////////////////////////////////////////
CncOpenGLCurrentContextObserverListCtrl::CncOpenGLCurrentContextObserverListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Parameter",	wxLIST_FORMAT_LEFT, 	 120);
	AppendColumn("Value",	 	wxLIST_FORMAT_LEFT, 	1800);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::NORMAL);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(191, 205, 219));
}
/////////////////////////////////////////////////////////////
CncOpenGLCurrentContextObserverListCtrl::~CncOpenGLCurrentContextObserverListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
int CncOpenGLCurrentContextObserverListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	return -1;
}
/////////////////////////////////////////////////////////////
wxString CncOpenGLCurrentContextObserverListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////
	return CncGLContextObserver::getInstance()->getCurrentContextItemText(item, column);
}
/////////////////////////////////////////////////////////////
bool CncOpenGLCurrentContextObserverListCtrl::isItemValid(long item) const {
/////////////////////////////////////////////////////////////
	return ( item >= 0 && item < (long)(CncGLContextObserver::getInstance()->getContextValueCount() - 1) );
}



/////////////////////////////////////////////////////////////
CncOpenGLRegisteredContextObserverListCtrl::CncOpenGLRegisteredContextObserverListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Context",	 	wxLIST_FORMAT_LEFT,  140);
	AppendColumn("Parameter",	wxLIST_FORMAT_LEFT, 1800);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::NORMAL);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(191, 205, 219));
}
/////////////////////////////////////////////////////////////
CncOpenGLRegisteredContextObserverListCtrl::~CncOpenGLRegisteredContextObserverListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
int CncOpenGLRegisteredContextObserverListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	return -1;
}
/////////////////////////////////////////////////////////////
wxString CncOpenGLRegisteredContextObserverListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////
	return CncGLContextObserver::getInstance()->getRegisteredContextItemText(item, column);
}
/////////////////////////////////////////////////////////////
bool CncOpenGLRegisteredContextObserverListCtrl::isItemValid(long item) const {
/////////////////////////////////////////////////////////////
	return ( item >= 0 && item < (long)(CncGLContextObserver::getInstance()->getContextCount() - 1) );
}


// ----------------------------------------------------------------------------
// CncOpenGLContextObserverHistoryListCtrl Event Table
// ----------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(CncOpenGLContextObserverHistoryListCtrl, CncLargeScaledListCtrl)
	EVT_RIGHT_DOWN(CncOpenGLContextObserverHistoryListCtrl::onRightDown)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncOpenGLContextObserverHistoryListCtrl::CncOpenGLContextObserverHistoryListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, defaultItemAttr()
, warningItemAttr()
, errorItemAttr()
, switchItemAttr()
, popupMenu(NULL)
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Time",			wxLIST_FORMAT_LEFT,     60);
	AppendColumn("Type",			wxLIST_FORMAT_CENTRE,   40);
	AppendColumn("Mode",			wxLIST_FORMAT_CENTRE,   44);
	AppendColumn("Current Context",	wxLIST_FORMAT_LEFT,    120);
	AppendColumn("Function",		wxLIST_FORMAT_LEFT,    120);
	AppendColumn("Message",			wxLIST_FORMAT_LEFT,   1800);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::REVERSE);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(191, 205, 219));
	
	wxFont boldFont(font);
	boldFont.MakeBold();

	defaultItemAttr.SetBackgroundColour(GetBackgroundColour());
	defaultItemAttr.SetFont(font);
	defaultItemAttr.SetTextColour(GetTextColour());
	
	warningItemAttr = defaultItemAttr;
	warningItemAttr.SetTextColour(wxColour(128, 128, 0));
	
	switchItemAttr = defaultItemAttr;
	switchItemAttr.SetFont(boldFont);
	
	errorItemAttr = defaultItemAttr;
	errorItemAttr.SetFont(boldFont);
	errorItemAttr.SetTextColour(wxColour(128, 0, 0));
	
	unsigned int itemId = 8000;
	popupMenu = new wxMenu("History Tools . . . ");
	popupMenu->Append(itemId, wxT("Clear List"));
	
	//............................................
	popupMenu->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [&](wxCommandEvent& event) {
			//CncOpenGLContextObserverHistoryListCtrl* ctl = reinterpret_cast<CncOpenGLContextObserverHistoryListCtrl*>(event.GetEventUserData());
			//wxASSERT(ctl);
			this->clear();
	 }, itemId, itemId);
}
/////////////////////////////////////////////////////////////
CncOpenGLContextObserverHistoryListCtrl::~CncOpenGLContextObserverHistoryListCtrl() {
/////////////////////////////////////////////////////////////
	delete popupMenu;
}
/////////////////////////////////////////////////////////////
void CncOpenGLContextObserverHistoryListCtrl::onRightDown(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////
	PopupMenu(popupMenu);
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncOpenGLContextObserverHistoryListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	const char type = getItemText(item, COL_TYPE)[0];
	
	switch ( type ) {
		case 'W':	return (wxListItemAttr*)(&warningItemAttr);
		case 'E':	return (wxListItemAttr*)(&errorItemAttr);
		case 'S':	return (wxListItemAttr*)(&switchItemAttr);
	}
	
	// this indicates to use the default style
	return (wxListItemAttr*)(&defaultItemAttr);
}
