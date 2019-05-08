#include <iostream>
#include <wx/imaglist.h>
#include "wxcrafter.h"
#include "MainFrame.h"
#include "GlobalStrings.h"
#include "CncConfig.h"
#include "CncPathListEntryListCtrl.h"

extern GlobalConstStringDatabase globalStrings;

// ----------------------------------------------------------------------------
// CncPathListEntryListCtrl Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(CncPathListEntryListCtrl, CncLargeScaledListCtrl)
	EVT_SIZE(							CncPathListEntryListCtrl::onSize)
	EVT_LIST_ITEM_SELECTED(wxID_ANY, 	CncPathListEntryListCtrl::onSelectListItem)
	EVT_LIST_ITEM_ACTIVATED(wxID_ANY, 	CncPathListEntryListCtrl::onActivateListItem)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncPathListEntryListCtrl::CncPathListEntryListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, defaultItemAttr()
, initialItemAttr()
, clientIdItemAttr()
, speedItemAttr()
, pathLists()
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Type",	 		wxLIST_FORMAT_LEFT, 	34);
	AppendColumn("PathList ID",		wxLIST_FORMAT_LEFT, 	120);
	AppendColumn("Client ID", 		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("F [mm/min]",		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	
	AppendColumn("X-Distance", 		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Y-Distance", 		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Z-Distance",		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	
	AppendColumn("X-Target", 		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Y-Target", 		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Z-Target",		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	
	AppendColumn("Total Length",	wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::NORMAL);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(191, 205, 219));
	
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibPathList().Bitmap("BMP_NO_CHG"));	// 0
	imageList->Add(ImageLibPathList().Bitmap("BMP_CLIENT_ID"));	// 1
	imageList->Add(ImageLibPathList().Bitmap("BMP_SPEED"));		// 2
	imageList->Add(ImageLibPathList().Bitmap("BMP_POSITION"));	// 3
	
	SetImageList(imageList, wxIMAGE_LIST_SMALL);
	
	defaultItemAttr.SetBackgroundColour(GetBackgroundColour());
	defaultItemAttr.SetFont(font);
	defaultItemAttr.SetTextColour(GetTextColour());

	clientIdItemAttr.SetBackgroundColour(GetBackgroundColour().ChangeLightness(80));
	clientIdItemAttr.SetFont(font.Bold());
	clientIdItemAttr.SetTextColour(GetTextColour());

	initialItemAttr.SetBackgroundColour(wxColour(112, 146, 190));
	initialItemAttr.SetFont(font);
	initialItemAttr.SetTextColour(GetTextColour());

	speedItemAttr.SetBackgroundColour(wxColour(255, 225, 121));
	speedItemAttr.SetFont(font);
	speedItemAttr.SetTextColour(GetTextColour());
}
/////////////////////////////////////////////////////////////
CncPathListEntryListCtrl::~CncPathListEntryListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
int CncPathListEntryListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	//enum Type {CHG_NOTHING=0, CHG_CLIENTID=1, CHG_SPEED=2, CHG_POSITION=3};
	//imageList->Add(ImageLibPathList().Bitmap("BMP_NO_CHG"));		// 0
	//imageList->Add(ImageLibPathList().Bitmap("BMP_CLIENT_ID"));	// 1
	//imageList->Add(ImageLibPathList().Bitmap("BMP_SPEED"));		// 2
	//imageList->Add(ImageLibPathList().Bitmap("BMP_POSITION"));	// 3
	
	if ( column == COL_TYPE ) {
		long type = 0; 
		getItemText(item, COL_TYPE).ToLong(&type);
		
		switch ( type ) {
			case 0:			return 0;
			case 1:			return 1;
			case 2:			return 2;
			case 3:			return 3;
		}
	}
	
	return -1;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncPathListEntryListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	//enum Type {CHG_NOTHING=0, CHG_CLIENTID=1, CHG_SPEED=2, CHG_POSITION=3};

	if 		( getItemText(item, COL_TYPE) == "0" ) return (wxListItemAttr*)(&initialItemAttr);
	else if	( getItemText(item, COL_TYPE) == "1" ) return (wxListItemAttr*)(&clientIdItemAttr);
	else if ( getItemText(item, COL_TYPE) == "2" ) return (wxListItemAttr*)(&speedItemAttr);

	// this indicates to use the default style
	return (wxListItemAttr*)(&defaultItemAttr);
}
/////////////////////////////////////////////////////////////
void CncPathListEntryListCtrl::onSize(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////
	updateColumnWidth();

	event.Skip(true);
}
/////////////////////////////////////////////////////////////////////
void CncPathListEntryListCtrl::updateColumnWidth() {
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
void CncPathListEntryListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
	
	setLastSelection(item);
	
	long ln;
	getRow(item).getItem(COL_SEARCH).ToLong(&ln);
	
	SelectEventBlocker blocker(this);
	THE_APP->tryToSelectClientId(ln, MainFrame::TemplateSelSource::TSS_PATH_LIST);
}
/////////////////////////////////////////////////////////////
void CncPathListEntryListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	// currently nothing todo
}
/////////////////////////////////////////////////////////////
bool CncPathListEntryListCtrl::searchReference(const wxString& what) {
/////////////////////////////////////////////////////////////
	long ret = searchRow(what, COL_SEARCH);
	
	if ( ret >= 0 )
		selectItem(ret);
	
	return (ret >= 0 );
}
/////////////////////////////////////////////////////////////
bool CncPathListEntryListCtrl::searchReferenceById(const long id) {
/////////////////////////////////////////////////////////////
	wxString what(wxString::Format(globalStrings.pathListRefFormat, id));
	return searchReference(what);
}
/////////////////////////////////////////////////////////////
void CncPathListEntryListCtrl::clearAll() {
/////////////////////////////////////////////////////////////
	pathLists.clear();
	clear();
}
/////////////////////////////////////////////////////////////
void CncPathListEntryListCtrl::addPathListEntry(const CncPathListEntry& cpe) {
/////////////////////////////////////////////////////////////
	pathLists.push_back(cpe);
}
/////////////////////////////////////////////////////////////
void CncPathListEntryListCtrl::updateContent(const UpdateContentInfo& uci, bool force) {
/////////////////////////////////////////////////////////////
	if ( force == false && getItemCount() != 0 )
		return;
	
	THE_APP->startAnimationControl();
	freeze();
	
		clear();
		for ( auto it = pathLists.begin(); it != pathLists.end(); ++it) {
			const CncPathListEntry& cpe = *it;

			if ( cpe.isNothingChange() 	== true && uci.format					== true )	continue;
			if ( cpe.isClientIdChange()	== true && uci.considerClientIdChanges	== false )	continue;
			if ( cpe.isSpeedChange() 	== true && uci.considerSpeedChanges		== false )	continue;
			if ( cpe.isPositionChange() == true && uci.considerPositionChnages	== false )	continue;

			CncColumContainer cc(CncPathListEntryListCtrl::TOTAL_COL_COUNT);
			static const wxString fmt(globalStrings.pathListRefFormat);

			const bool displaySpeed    = ( cpe.isSpeedChange()    == true || cpe.isNothingChange() == true );
			const bool displayPosition = ( cpe.isPositionChange() == true || cpe.isNothingChange() == true );

			cc.updateItem(CncPathListEntryListCtrl::COL_TYPE, 				wxString::Format("%d", 			cpe.type));
			cc.updateItem(CncPathListEntryListCtrl::COL_REF, 				wxString::Format("%lld", 		cpe.pathListReference));
			cc.updateItem(CncPathListEntryListCtrl::COL_CLD_ID,				wxString::Format(fmt, 			cpe.clientId));

			if ( displaySpeed == true ) {
				cc.updateItem(CncPathListEntryListCtrl::COL_F, 				wxString::Format("%4.1lf %c", 	cpe.feedSpeed_MM_MIN, cnc::getCncSpeedTypeAsCharacter(cpe.feedSpeedMode)));
			}

			if ( displayPosition == true ) {
				cc.updateItem(CncPathListEntryListCtrl::COL_DISTANCE_X, 	wxString::Format("%10.3lf", 	cpe.entryDistance.getX()));
				cc.updateItem(CncPathListEntryListCtrl::COL_DISTANCE_Y, 	wxString::Format("%10.3lf", 	cpe.entryDistance.getY()));
				cc.updateItem(CncPathListEntryListCtrl::COL_DISTANCE_Z, 	wxString::Format("%10.3lf", 	cpe.entryDistance.getZ()));
				cc.updateItem(CncPathListEntryListCtrl::COL_TARGET_X, 		wxString::Format("%10.3lf", 	cpe.entryTarget.getX()));
				cc.updateItem(CncPathListEntryListCtrl::COL_TARGET_Y, 		wxString::Format("%10.3lf", 	cpe.entryTarget.getY()));
				cc.updateItem(CncPathListEntryListCtrl::COL_TARGET_Z, 		wxString::Format("%10.3lf", 	cpe.entryTarget.getZ()));
				cc.updateItem(CncPathListEntryListCtrl::COL_TOTAL_DISTANCE, wxString::Format("%10.3lf", 	cpe.totalDistance));
			}
			
			appendItem(cc);
		}
		
	thaw();
	THE_APP->stopAnimationControl();
}
