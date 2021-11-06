#include "GlobalStrings.h"
#include "CncSecureCtrlPanel.h"
#include "wxCrafterImages.h"
#include "CncSecurePortListCtrl.h"

/////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(CncSecurePortListCtrl, CncLargeScaledListCtrl)
	EVT_SIZE				(			CncSecurePortListCtrl::onSize				)
	EVT_LIST_ITEM_ACTIVATED	(wxID_ANY, 	CncSecurePortListCtrl::onActivateListItem	)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
CncSecurePortListCtrl::CncSecurePortListCtrl(wxWindow *parent, CncSecureCtrlPanel* panel, long style)
: CncLargeScaledListCtrl	(parent, style)
, portEntries				()
, securePanel				(panel)
, defaultItemAttr			()
, selectedItemAttr			()
/////////////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("", wxLIST_FORMAT_LEFT, 30);
	AppendColumn("", wxLIST_FORMAT_LEFT, 30);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::REVERSE);
	
	wxFont font(16, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetTextColour(wxColour(32, 32, 32));
	SetBackgroundColour(wxColour(255, 255, 255));
	
	defaultItemAttr.SetBackgroundColour(GetBackgroundColour());
	defaultItemAttr.SetFont(font);
	defaultItemAttr.SetTextColour(GetTextColour());
	
	selectedItemAttr 	= defaultItemAttr;
	selectedItemAttr	.SetTextColour(wxColour(255, 128, 128));
	selectedItemAttr	.SetFont(font.Bold());
	
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibPortSelector().Bitmap(PortSelector::BMP_PS_CONNECTED));
	imageList->Add(ImageLibPortSelector().Bitmap(PortSelector::BMP_PS_AVAILABLE));
	imageList->Add(ImageLibPortSelector().Bitmap(PortSelector::BMP_PS_ACCESS_DENIED));
	imageList->Add(ImageLibPortSelector().Bitmap(PortSelector::BMP_PS_UNKNOWN));
	SetImageList(imageList, wxIMAGE_LIST_SMALL);
}
/////////////////////////////////////////////////////////////////////
CncSecurePortListCtrl::~CncSecurePortListCtrl() {
/////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////
void CncSecurePortListCtrl::updateColumnWidth() {
/////////////////////////////////////////////////////////////////////
	// try to stretch the second (key) column
	const int size = GetSize().GetWidth() - 26; 
	
	if ( size > GetColumnWidth(COL_STRECH) )
		SetColumnWidth(COL_STRECH, size);
}
/////////////////////////////////////////////////////////////////////
void CncSecurePortListCtrl::deleteAllEntries() {
/////////////////////////////////////////////////////////////////////
	SetItemCount(portEntries.size());
	
	CncLargeScaledListCtrl::clear();
	portEntries.clear();
	
	Refresh();
}
/////////////////////////////////////////////////////////////////////
void CncSecurePortListCtrl::addPortEntry(const wxString& name, PortListImage pii) {
/////////////////////////////////////////////////////////////////////
	portEntries.push_back(PortEntry(name, pii));
	
	SetItemCount(portEntries.size());
	Refresh();
}
/////////////////////////////////////////////////////////////////////
void CncSecurePortListCtrl::addPortEntry(const wxString& name, const wxString& image) {
/////////////////////////////////////////////////////////////////////
	addPortEntry(name, getImgaeIndex(image));
}
/////////////////////////////////////////////////////////////////////
bool CncSecurePortListCtrl::selectPortInList(const wxString& portName) {
/////////////////////////////////////////////////////////////////////
	for ( auto it = portEntries.begin(); it != portEntries.end(); ++it ) {
		
		if ( it->portName == portName ) {
			selectItem(std::distance(portEntries.begin(), it), true);
			return true;
		}
	}
	
	return false;
}
/////////////////////////////////////////////////////////////////////
void CncSecurePortListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////////////
	long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
	
	if ( isItemValid(item) == false )
		return;
		
	if ( securePanel ) {
		securePanel->setPortSelection(portEntries.at(item).portName);
		setLastSelection(item);
	}
}
/////////////////////////////////////////////////////////////////////
const CncSecurePortListCtrl::PortListImage CncSecurePortListCtrl::getImgaeIndex(const wxString& name) {
/////////////////////////////////////////////////////////////////////
	if      (name.IsSameAs(PortSelector::BMP_PS_CONNECTED))     return PTI_CONNECTED;
	else if (name.IsSameAs(PortSelector::BMP_PS_AVAILABLE))     return PTI_AVAILABLE;
	else if (name.IsSameAs(PortSelector::BMP_PS_ACCESS_DENIED)) return PTI_UNKNOWN;
	else if (name.IsSameAs(PortSelector::BMP_PS_UNKNOWN)) 		return PTI_ACCESS_DENIED;
	
	return PTI_EMPTY;
}
/////////////////////////////////////////////////////////////////////
wxString CncSecurePortListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return _("");

	if ( column != COL_NAME )
		return _("");
	
	return portEntries.at(item).portName;
}
/////////////////////////////////////////////////////////////////////
int CncSecurePortListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return -1;
		
	if ( column != COL_STATE )
		return -1;
		
	return portEntries.at(item).imageIdx;
}
/////////////////////////////////////////////////////////////////////
wxListItemAttr* CncSecurePortListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////////////
	return (wxListItemAttr*)(item != getLastSelection() ? &defaultItemAttr : &selectedItemAttr);
}
/////////////////////////////////////////////////////////////////////
void CncSecurePortListCtrl::onSize(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////////////
	updateColumnWidth();
	event.Skip(true);
}
