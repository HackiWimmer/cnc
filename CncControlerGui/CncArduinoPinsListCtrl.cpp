#include <iostream>
#include <wx/imaglist.h>
#include "CncCommon.h"
#include "CncArduino.h"
#include "wxCrafterImages.h"
#include "CncArduinoPinsListCtrl.h"

// ----------------------------------------------------------------------------
// CncLruFileViewListCtrl Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(CncArduinoPinsListCtrl, CncLargeScaledListCtrl)
	EVT_LIST_ITEM_SELECTED	(wxID_ANY, 	CncArduinoPinsListCtrl::onSelectListItem	)
	EVT_LIST_ITEM_ACTIVATED	(wxID_ANY, 	CncArduinoPinsListCtrl::onActivateListItem	)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncArduinoPinsListCtrl::CncArduinoPinsListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Typ:", 	wxLIST_FORMAT_CENTRE, 	 40);
	AppendColumn("Num",		wxLIST_FORMAT_CENTRE, 	 40);
	AppendColumn("Label", 	wxLIST_FORMAT_LEFT, 	132);
	AppendColumn("Value", 	wxLIST_FORMAT_LEFT, 	 80);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::NORMAL);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(0, 0, 0));
	SetTextColour(wxColour(255, 255, 255));
	
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibPins().Bitmap("BMP_LOW"));
	imageList->Add(ImageLibPins().Bitmap("BMP_HIGH"));
	imageList->Add(ImageLibPins().Bitmap("BMP_ANALOG"));
	imageList->Add(ImageLibPins().Bitmap("BMP_INPUT"));
	imageList->Add(ImageLibPins().Bitmap("BMP_OUTPUT"));
	SetImageList(imageList, wxIMAGE_LIST_SMALL);
	
	// build up data structure
	for ( unsigned int i=AE::minDPinIndex; i<=AE::maxDPinIndex; i++ )
		pins.push_back(AE::PinData('D', i, i));
		
	for ( unsigned int i=AE::minAPinIndex; i<=AE::maxAPinIndex; i++ )
		pins.push_back(AE::PinData('A', i - AE::minAPinIndex, i));
	
	// build up index
	for ( auto it = pins.begin(); it != pins.end(); ++it )
		pinIndex[AE::ArduinoData::buildDislpayName(it->type, it->name)] = std::distance(pins.begin(), it);
	
	SetItemCount(pins.size());
}
/////////////////////////////////////////////////////////////
CncArduinoPinsListCtrl::~CncArduinoPinsListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
void CncArduinoPinsListCtrl::updatePinValue(const char type, unsigned int name, const char mode, int value) {
/////////////////////////////////////////////////////////////
	auto it = pinIndex.find(AE::ArduinoData::buildDislpayName(type, name));
	if ( it == pinIndex.end() ) 
		return;
		
	AE::PinData& p = pins.at(it->second);
	
	p.value = value;
	p.mode 	= mode;
	Refresh();
}
/////////////////////////////////////////////////////////////////////
bool CncArduinoPinsListCtrl::isItemValid(long item) const {
/////////////////////////////////////////////////////////////////////
	if ( item < 0 || item > (long)(pins.size() - 1) )
		return false;
	
	return true;
}
/////////////////////////////////////////////////////////////
int CncArduinoPinsListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return -1;
		
	const AE::PinData& p = pins.at(item);
	
	const unsigned int BMP_LOW 		= 0;
	const unsigned int BMP_HIGH 	= 1;
	const unsigned int BMP_ANALOG 	= 2;
	const unsigned int BMP_INPUT 	= 3;
	const unsigned int BMP_QUTPUT 	= 4;
	
	if ( column == COL_VAL ) {
		
		if ( p.type == 'D' ) 
			return p.value == 0 ? BMP_LOW : BMP_HIGH;

		if ( p.type == 'A' ) 
			return BMP_ANALOG;
			
	} else if ( column == COL_TYP ) {
		
		return p.mode == 'I' ? BMP_INPUT : BMP_QUTPUT;
	}
	
	return -1;
}
/////////////////////////////////////////////////////////////////////
wxString CncArduinoPinsListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return _("");
		
	const AE::PinData& p = pins.at(item);
	const wxString label(p.type == 'D' ? ArduinoDigitalPins::getPinLabel(p.name) : ArduinoAnalogPins::getPinLabel(p.name));

	switch ( column ) {
		case COL_TYP:	return _(wxString::Format("%c", 	p.type));
		case COL_NUM:	return _(wxString::Format("%02u", 	p.name));
		case COL_LABEL: return _(label.Contains("Unknown") ? "" : label);
	}
	
	if ( column != COL_VAL )
		return _("");
		
	if ( p.value < 0 )
		return _("");

	if ( p.type == 'D' ) 
		return p.value != 0 ? "HIGH" : "LOW";

	if ( p.type == 'A' ) 
		return _(wxString::Format("%03d", p.value));
	
	return _("");
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncArduinoPinsListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	return (wxListItemAttr*)(NULL);
}
/////////////////////////////////////////////////////////////////////
void CncArduinoPinsListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////
void CncArduinoPinsListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////////////
}
