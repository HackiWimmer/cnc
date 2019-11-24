#include <iostream>
#include <wx/imaglist.h>
#include "wxCrafterMainFrame.h"
#include "wxCrafterImages.h"
#include "CncParsingSynopsisTraceListCtrl.h"

// ----------------------------------------------------------------------------
// CncGCodeSequenceListCtrl Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(CncParsingSynopsisListCtrl, CncLargeScaledListCtrl)
	EVT_SIZE(							CncParsingSynopsisListCtrl::onSize)
	EVT_LIST_ITEM_SELECTED(wxID_ANY, 	CncParsingSynopsisListCtrl::onSelectListItem)
	EVT_LIST_ITEM_ACTIVATED(wxID_ANY, 	CncParsingSynopsisListCtrl::onActivateListItem)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncParsingSynopsisListCtrl::CncParsingSynopsisListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, defaultItemAttr()
, infoItemAttr()
, warningItemAttr()
, errorItemAttr()
, separatorItemAttr()
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("", 		wxLIST_FORMAT_LEFT,  0);
	AppendColumn("Trace", 	wxLIST_FORMAT_LEFT,  wxLIST_AUTOSIZE);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::NORMAL);
	
	wxFont font(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));
	SetFont(font);
	
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibSynopsisTrace().Bitmap("BMP_SYNOPSIS_INFO")		.ConvertToDisabled(60));	// 0
	imageList->Add(ImageLibSynopsisTrace().Bitmap("BMP_SYNOPSIS_WARNING")	.ConvertToDisabled(60));	// 1
	imageList->Add(ImageLibSynopsisTrace().Bitmap("BMP_SYNOPSIS_ERROR")		.ConvertToDisabled(60));	// 2
	imageList->Add(ImageLibSynopsisTrace().Bitmap("BMP_SYNOPSIS_SEPARATOR")	.ConvertToDisabled(60));	// 3
	
	SetImageList(imageList, wxIMAGE_LIST_SMALL);
	
	SetBackgroundColour(wxColour(0, 0, 0));
	
	defaultItemAttr.SetBackgroundColour(GetBackgroundColour());
	defaultItemAttr.SetFont(font);
	defaultItemAttr.SetTextColour(GetTextColour());
	
	infoItemAttr  		= defaultItemAttr;
	warningItemAttr		= defaultItemAttr;
	errorItemAttr		= defaultItemAttr;
	separatorItemAttr	= defaultItemAttr;
	
	infoItemAttr		.SetTextColour(wxColour(242, 242, 242));
	warningItemAttr		.SetTextColour(wxColour(242, 242,   0));
	errorItemAttr		.SetTextColour(wxColour(255, 128, 128));
	
	separatorItemAttr	.SetTextColour(infoItemAttr.GetTextColour());
	separatorItemAttr	.SetBackgroundColour(wxColour(25,   25,  25));
	defaultItemAttr		.SetFont(font.Scaled(0.5));
}
/////////////////////////////////////////////////////////////
CncParsingSynopsisListCtrl::~CncParsingSynopsisListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
int CncParsingSynopsisListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	if ( column == COL_TYPE) {
		if ( getItemText(item, COL_TYPE).length() > 0 ) {
			
			switch ( (char)getItemText(item, COL_TYPE)[0] ) {
				case 'W':		return 1;
				case 'E':		return 2;
				case 'S':		return -1;
				default:		return 0;
			}
		}
	}
	
	return -1;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncParsingSynopsisListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	if ( getItemText(item, COL_TYPE).length() > 0 ) {
		
		switch ( (char)getItemText(item, COL_TYPE)[0] ) {
			case 'W':		return (wxListItemAttr*)(&warningItemAttr);
			case 'E':		return (wxListItemAttr*)(&errorItemAttr);
			case 'S':		return (wxListItemAttr*)(&separatorItemAttr);
			default:		return (wxListItemAttr*)(&infoItemAttr);
		}
	}
	
	// this indicates to use the default style
	return (wxListItemAttr*)(&defaultItemAttr);
}
/////////////////////////////////////////////////////////////
void CncParsingSynopsisListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
		
	setLastSelection(item);
}
/////////////////////////////////////////////////////////////
void CncParsingSynopsisListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	// currently nothing todo
}
/////////////////////////////////////////////////////////////
void CncParsingSynopsisListCtrl::onSize(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////
	updateColumnWidth();

	event.Skip(true);
}
/////////////////////////////////////////////////////////////////////
void CncParsingSynopsisListCtrl::updateColumnWidth() {
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
void CncParsingSynopsisListCtrl::addEntry(const char type, const wxString& entry) {
/////////////////////////////////////////////////////////////
	CncColumContainer cc(CncParsingSynopsisListCtrl::TOTAL_COL_COUNT);

	switch ( type ) {
		case 'W':		cc.updateItem(CncParsingSynopsisListCtrl::COL_TYPE, "W"); 
						break;
		case 'E':		cc.updateItem(CncParsingSynopsisListCtrl::COL_TYPE, "E");
						break;
		case 'S':		cc.updateItem(CncParsingSynopsisListCtrl::COL_TYPE, "S");
						break;
		default:		cc.updateItem(CncParsingSynopsisListCtrl::COL_TYPE, "I");
	}

	cc.updateItem(CncParsingSynopsisListCtrl::COL_TRACE, 	entry);
	appendItem(cc);
}
