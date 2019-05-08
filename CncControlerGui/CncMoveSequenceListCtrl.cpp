#include <iostream>
#include <wx/imaglist.h>
#include "wxcrafter.h"
#include "MainFrame.h"
#include "GlobalStrings.h"
#include "CncConfig.h"
#include "CncMoveSequenceListCtrl.h"

extern GlobalConstStringDatabase globalStrings;

// ----------------------------------------------------------------------------
// CncMoveSequenceListCtrl Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(CncMoveSequenceListCtrl, CncLargeScaledListCtrl)
	EVT_SIZE(							CncMoveSequenceListCtrl::onSize)
	EVT_LIST_ITEM_SELECTED(wxID_ANY, 	CncMoveSequenceListCtrl::onSelectListItem)
	EVT_LIST_ITEM_ACTIVATED(wxID_ANY, 	CncMoveSequenceListCtrl::onActivateListItem)
wxEND_EVENT_TABLE()


/////////////////////////////////////////////////////////////
CncMoveSequenceListCtrl::CncMoveSequenceListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, defaultItemAttr()
, initialItemAttr()
, clientIdItemAttr()
, speedItemAttr()
, moveSequences()
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Type",	 		wxLIST_FORMAT_LEFT, 	34);
	AppendColumn("Sequence ID",		wxLIST_FORMAT_LEFT, 	120);
	AppendColumn("Speed [mm/min]",	wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Client ID",	 	wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("X-Distance", 		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Y-Distance", 		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Z-Distance",		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);

	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::NORMAL);

	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(191, 205, 219));

	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibPathList().Bitmap("BMP_NO_CHG"));		// 0
	imageList->Add(ImageLibPathList().Bitmap("BMP_CLIENT_ID"));		// 1
	imageList->Add(ImageLibPathList().Bitmap("BMP_SPEED"));			// 2
	imageList->Add(ImageLibPathList().Bitmap("BMP_POSITION"));		// 3
	imageList->Add(ImageLibPathList().Bitmap("BMP_POS_OPTIMIZED"));	// 4

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
CncMoveSequenceListCtrl::~CncMoveSequenceListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
int CncMoveSequenceListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	//enum Type {CHG_NOTHING=0, CHG_CLIENTID=1, CHG_SPEED=2, CHG_POSITION=3};
	//imageList->Add(ImageLibPathList().Bitmap("BMP_NO_CHG"));			// 0
	//imageList->Add(ImageLibPathList().Bitmap("BMP_CLIENT_ID"));		// 1
	//imageList->Add(ImageLibPathList().Bitmap("BMP_SPEED"));			// 2
	//imageList->Add(ImageLibPathList().Bitmap("BMP_POSITION"));		// 3
	//imageList->Add(ImageLibPathList().Bitmap("BMP_POS_OPTIMIZED"));	// 4

	if ( column == COL_TYPE ) {
		long type = 0;
		getItemText(item, COL_TYPE).ToLong(&type);

		switch ( type ) {
			case 0:			return 0;
			case 1:			return 1;
			case 2:			return 2;
			case 3:			return 3;
			case 4:			return 4;
		}
	}

	return -1;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncMoveSequenceListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	//enum Type {CHG_NOTHING=0, CHG_CLIENTID=1, CHG_SPEED=2, CHG_POSITION=3};

	if 		( getItemText(item, COL_TYPE) == "0" ) return (wxListItemAttr*)(&initialItemAttr);
	else if	( getItemText(item, COL_TYPE) == "1" ) return (wxListItemAttr*)(&clientIdItemAttr);
	else if ( getItemText(item, COL_TYPE) == "2" ) return (wxListItemAttr*)(&speedItemAttr);

	// this indicates to use the default style
	return (wxListItemAttr*)(&defaultItemAttr);
}
/////////////////////////////////////////////////////////////
void CncMoveSequenceListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;

	setLastSelection(item);

	long ln;
	getRow(item).getItem(COL_SEARCH).ToLong(&ln);

	SelectEventBlocker blocker(this);
	THE_APP->tryToSelectClientId(ln, MainFrame::TemplateSelSource::TSS_MOVE_SEQ);
}
/////////////////////////////////////////////////////////////
void CncMoveSequenceListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	// currently nothing todo
}
/////////////////////////////////////////////////////////////
void CncMoveSequenceListCtrl::onSize(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////
	updateColumnWidth();

	event.Skip(true);
}
/////////////////////////////////////////////////////////////////////
void CncMoveSequenceListCtrl::updateColumnWidth() {
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
bool CncMoveSequenceListCtrl::searchReference(const wxString& what) {
/////////////////////////////////////////////////////////////
	long ret = searchRow(what, COL_SEARCH);

	if ( ret >= 0 )
		selectItem(ret);

	return (ret >= 0 );
}
/////////////////////////////////////////////////////////////
bool CncMoveSequenceListCtrl::searchReferenceById(const long id) {
/////////////////////////////////////////////////////////////
	wxString what(wxString::Format(globalStrings.pathListRefFormat, id));
	return searchReference(what);
}
/////////////////////////////////////////////////////////////
void CncMoveSequenceListCtrl::clearAll() {
/////////////////////////////////////////////////////////////
	moveSequences.clear();
	clear();
}
/////////////////////////////////////////////////////////////
void CncMoveSequenceListCtrl::addMoveSequenceClietId(const CncMoveSequence& seq, const SpeedInfo& si) {
	const RowInfo rpi(RowInfo::Type::RIT_ID);
	moveSequences.push_back(rpi);
	moveSequences.back().seq 	= seq;
	moveSequences.back().speed 	= si;
}
/////////////////////////////////////////////////////////////
void CncMoveSequenceListCtrl::addMoveSequencePositions(const CncMoveSequence& seq) {
/////////////////////////////////////////////////////////////
	const RowInfo rpi(RowInfo::Type::RIT_POS);
	moveSequences.push_back(rpi);
	moveSequences.back().seq = seq;
}
/////////////////////////////////////////////////////////////
void CncMoveSequenceListCtrl::updateContent(bool force) {
/////////////////////////////////////////////////////////////
	if ( force == false && getItemCount() != 0 )
		return;
	
	THE_APP->startAnimationControl();
	freeze();
		
		clear();
		for ( auto it = moveSequences.begin(); it != moveSequences.end(); ++it)
			it->appendTo(this);
		
	thaw();
	THE_APP->stopAnimationControl();

}
/////////////////////////////////////////////////////////////
void CncMoveSequenceListCtrl::RowInfo::appendTo(CncMoveSequenceListCtrl* ctrl) {
/////////////////////////////////////////////////////////////
	if ( ctrl == NULL )
		return;

	static const wxString fmt(globalStrings.moveSeqRefFormat);

	switch ( type ) {
		case RIT_ID:
		{
			CncColumContainer cc(CncMoveSequenceListCtrl::TOTAL_COL_COUNT);

			cc.updateItem(CncMoveSequenceListCtrl::COL_TYPE, 			wxString::Format("%d",       RIT_ID));
			cc.updateItem(CncMoveSequenceListCtrl::COL_REF,  			wxString::Format("%lld", 	 seq.getReference()));
			cc.updateItem(CncMoveSequenceListCtrl::COL_SPEED,  			wxString::Format("%.1lf %c", speed.value, speed.mode));

			ctrl->appendItem(cc);

			break;
		}
		case RIT_SPEED:
		{
			//todo
			break;
		}
		case RIT_POS:
		{
			for ( auto it = seq.const_begin(); it != seq.const_end(); ++it) {

				CncColumContainer cc(CncMoveSequenceListCtrl::TOTAL_COL_COUNT);
				cc.updateItem(CncMoveSequenceListCtrl::COL_TYPE, 		wxString::Format("%d", 		RIT_POS));
				cc.updateItem(CncMoveSequenceListCtrl::COL_REF, 		wxString::Format("%lld", 	seq.getReference()));

				const CncMoveSequence::SequencePoint sp = *it;
				cc.updateItem(CncMoveSequenceListCtrl::COL_CLD_ID,		wxString::Format(fmt, 		sp.clientID));

				cc.updateItem(CncMoveSequenceListCtrl::COL_DISTANCE_X,	wxString::Format("%10ld",	(long)sp.x));
				cc.updateItem(CncMoveSequenceListCtrl::COL_DISTANCE_Y,	wxString::Format("%10ld", 	(long)sp.y));
				cc.updateItem(CncMoveSequenceListCtrl::COL_DISTANCE_Z,	wxString::Format("%10ld", 	(long)sp.z));

				ctrl->appendItem(cc);
			}

			break;
		}

		default: ;
	}
}


