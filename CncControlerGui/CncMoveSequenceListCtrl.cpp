#include <iostream>
#include <wx/imaglist.h>
#include <wx/stattext.h>
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
	EVT_PAINT(							CncMoveSequenceListCtrl::onPaint)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncMoveSequenceListCtrl::CncMoveSequenceListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, active(true)
, moveSequence(NULL)
, defaultItemAttr()
, initialItemAttr()
, clientIdItemAttr()
, speedItemAttr()
, infoText(new wxStaticText(this, wxID_ANY, ""))
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Type",	 		wxLIST_FORMAT_LEFT, 	34);
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
	
	wxPoint pt;
	pt.x = 100;
	pt.y = 100;

	wxFont infoFont(18, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Segoe UI"));
	SetFont(font);
	infoText->Show(false);
	infoText->SetFont(infoFont);
	infoText->SetLabel("Disabled, because the corresponding\nconfiguration parameter\n'Work::Preprocessor\\Analyse Pathes' isn't active!");
	infoText->SetPosition(pt);
}
/////////////////////////////////////////////////////////////
CncMoveSequenceListCtrl::~CncMoveSequenceListCtrl() {
/////////////////////////////////////////////////////////////
	delete infoText;
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
	#warning
	long ret = searchRow(what, COL_SEARCH);

	if ( ret >= 0 )
		selectItem(ret);

	return (ret >= 0 );
}
/////////////////////////////////////////////////////////////
bool CncMoveSequenceListCtrl::searchReferenceById(const long id) {
/////////////////////////////////////////////////////////////
	#warning
	wxString what(wxString::Format(globalStrings.pathListRefFormat, id));
	return searchReference(what);
}
/////////////////////////////////////////////////////////////
void CncMoveSequenceListCtrl::clearAll() {
/////////////////////////////////////////////////////////////
	moveSequence = NULL;
	clear();
}
/////////////////////////////////////////////////////////////
void CncMoveSequenceListCtrl::addMoveSequence(const CncMoveSequence* seq, const SpeedInfo& si) {
/////////////////////////////////////////////////////////////
	clearAll();
	
	if ( seq == NULL )
		return;
		
	moveSequence = seq;
	SetItemCount(moveSequence->getCount());
}
//////////////////////////////////////////////////
void CncMoveSequenceListCtrl::onPaint(wxPaintEvent& event) {
//////////////////////////////////////////////////
	event.Skip();

	const bool show = GBL_CONFIG->getPreProcessorAnalyseFlag() == false && active;
	infoText->Show(show);
	
	if ( active ) {
		if ( GBL_CONFIG->getPreProcessorAnalyseFlag() == false ) 
			clear();
	}
}
/////////////////////////////////////////////////////////////
wxString CncMoveSequenceListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////
	if ( moveSequence == NULL )
		return _("");

	if ( item < 0 || item > (long)(moveSequence->getCount() -1) )
		return _("");
		
	static const wxString fmt(globalStrings.moveSeqRefFormat);

	auto it = moveSequence->const_begin() + item;
	const CncMoveSequence::SequencePoint& sp = *it;
	
	switch ( column ) {
		case CncMoveSequenceListCtrl::COL_TYPE:			return wxString::Format("%d", 		1);
		case CncMoveSequenceListCtrl::COL_SPEED:		return _("");
		case CncMoveSequenceListCtrl::COL_CLD_ID:		return wxString::Format(fmt, 		sp.clientID);
		case CncMoveSequenceListCtrl::COL_DISTANCE_X:	return wxString::Format("%10ld",	(long)sp.x);
		case CncMoveSequenceListCtrl::COL_DISTANCE_Y:	return wxString::Format("%10ld", 	(long)sp.y);
		case CncMoveSequenceListCtrl::COL_DISTANCE_Z:	return wxString::Format("%10ld", 	(long)sp.z);
	}
	
	return _("");
}


// ----------------------------------------------------------------------------
// CncMoveSequenceListCtrl Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(CncMoveSequenceOverviewListCtrl, CncLargeScaledListCtrl)
	EVT_LIST_ITEM_SELECTED(wxID_ANY, 	CncMoveSequenceOverviewListCtrl::onSelectListItem)
	EVT_LIST_ITEM_ACTIVATED(wxID_ANY, 	CncMoveSequenceOverviewListCtrl::onActivateListItem)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncMoveSequenceOverviewListCtrl::CncMoveSequenceOverviewListCtrl(wxWindow *parent, long style, CncMoveSequenceListCtrl* slave, wxStaticText* label) 
: CncLargeScaledListCtrl(parent, style)
, moveSequences()
, slaveSequenceList(slave)
, contentLabel(label)
/////////////////////////////////////////////////////////////
{
	wxASSERT( slaveSequenceList != NULL );
	wxASSERT( contentLabel != NULL );
	
	// add colums
	AppendColumn("#",	 			wxLIST_FORMAT_RIGHT, 	 44);
	AppendColumn("Count",			wxLIST_FORMAT_RIGHT, 	 44);
	AppendColumn("Sequence ID",		wxLIST_FORMAT_LEFT, 	110);
	AppendColumn("Client ID",		wxLIST_FORMAT_RIGHT,	 60);

	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::NORMAL);

	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(191, 205, 219));
}
/////////////////////////////////////////////////////////////
CncMoveSequenceOverviewListCtrl::~CncMoveSequenceOverviewListCtrl() {
/////////////////////////////////////////////////////////////
	moveSequences.clear();
}
/////////////////////////////////////////////////////////////
void CncMoveSequenceOverviewListCtrl::clearAll() {
/////////////////////////////////////////////////////////////
	moveSequences.clear();
	clear();
}
/////////////////////////////////////////////////////////////
void CncMoveSequenceOverviewListCtrl::addMoveSequence(const CncMoveSequence& seq) {
/////////////////////////////////////////////////////////////
	moveSequences.push_back(seq);
	SetItemCount(moveSequences.size());
}
/////////////////////////////////////////////////////////////
wxString CncMoveSequenceOverviewListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return _("");
		
	const CncMoveSequence& seq = moveSequences.at(item);
	
	switch ( column ) {
		case COL_NUM:		return wxString::Format("%ld", 	item);
		case COL_CNT:		return wxString::Format("%ld", 	seq.getCount());
		case COL_REF:		return wxString::Format("%lld", seq.getReference());
		case COL_CLD_ID:	return wxString::Format("%ld",  seq.getClientId());
	}
	
	return _("");
}
/////////////////////////////////////////////////////////////
bool CncMoveSequenceOverviewListCtrl::isItemValid(long item) const {
/////////////////////////////////////////////////////////////
	return item >= 0 && item < (long)(moveSequences.size());
}
/////////////////////////////////////////////////////////////
void CncMoveSequenceOverviewListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
		
	const CncMoveSequence* seq = &(moveSequences.at(item));
	
	// select client id
	setLastSelection(item);
	long ln = seq->getClientId();
	SelectEventBlocker blocker(this);
	THE_APP->tryToSelectClientId(ln, MainFrame::TemplateSelSource::TSS_MOVE_SEQ_OVW);
	
	// update move sequence list control
	wxASSERT( slaveSequenceList != NULL );
	wxASSERT( contentLabel != NULL);
	
	contentLabel->SetLabel(wxString::Format("Content of MoveSequence with ID = '%lld':", seq->getReference()));

	#warning
	CncMoveSequenceListCtrl::SpeedInfo si;
	slaveSequenceList->addMoveSequence(seq, si);
	slaveSequenceList->Refresh();
}
/////////////////////////////////////////////////////////////
void CncMoveSequenceOverviewListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	// currently nothing todo
}
