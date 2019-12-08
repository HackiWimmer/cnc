#include <wx/event.h>
#include <wx/filedlg.h>

#include "CncFileDialog.h"

wxBEGIN_EVENT_TABLE(CncOpenDialogLRUList, wxListCtrl)
	EVT_LIST_ITEM_SELECTED	(wxID_ANY, 	CncOpenDialogLRUList::onSelectListItem)
	EVT_LEFT_DCLICK 		(			CncOpenDialogLRUList::onLeftDClick)
	EVT_KEY_DOWN			(			CncOpenDialogLRUList::onKeyDown)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncOpenDialogLRUList::CncOpenDialogLRUList(wxWindow *parent)
: wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT)
, lastSelection(-1)
/////////////////////////////////////////////////////////////
{
	AppendColumn("LRU Filename", wxLIST_FORMAT_LEFT , 600);
}
/////////////////////////////////////////////////////////////
CncOpenDialogLRUList::~CncOpenDialogLRUList() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
void CncOpenDialogLRUList::addFileName(const wxString& fn) {
/////////////////////////////////////////////////////////////
	unsigned int index = GetItemCount();
	InsertItem(index, "",  0);
	SetItem(index, 0, fn);
}
/////////////////////////////////////////////////////////////
void CncOpenDialogLRUList::processSelectedItem() {
////////////////////////////////////////////////////////////
	if ( lastSelection < 0 )
		return;

	//std::cout << GetItemText(item, 0) << std::endl;
	wxFileDialog* fd = static_cast<wxFileDialog*>(GetParent());
	fd->SetPath(GetItemText(lastSelection, 0));
}
/////////////////////////////////////////////////////////////
void CncOpenDialogLRUList::closeParentDialog(int code) {
/////////////////////////////////////////////////////////////

	wxFileDialog* fd = static_cast<wxFileDialog*>(GetParent());
	fd->EndModal(code);
}
/////////////////////////////////////////////////////////////
void CncOpenDialogLRUList::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	const long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;

	lastSelection = item;
	processSelectedItem();
}
/////////////////////////////////////////////////////////////
void CncOpenDialogLRUList::onLeftDClick(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////
	processSelectedItem();
	closeParentDialog(wxID_OK);
}
/////////////////////////////////////////////////////////////
void CncOpenDialogLRUList::onKeyDown(wxKeyEvent& event) {
/////////////////////////////////////////////////////////////
	processSelectedItem();
	closeParentDialog(wxID_OK);
}
