#include <iostream>
#include <wx/imaglist.h>
#include <wx/stattext.h>
#include "wxCrafterImages.h"
#include "MainFrameProxy.h"
#include "GlobalStrings.h"
#include "CncConfig.h"
#include "CncUserEvents.h"
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
, active					(true)
, moveSequence				(NULL)
, defaultItemAttr			()
, initialItemAttr			()
, clientIdItemAttr			()
, speedItemAttr				()
, defaultItemAttrSelected	()
, initialItemAttrSelected	()
, clientIdItemAttrSelected	()
, speedItemAttrSelected		()
, infoText					(new wxStaticText(this, wxID_ANY, ""))
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Type",	 		wxLIST_FORMAT_LEFT, 	44);
	AppendColumn("Client ID(s)",	wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("X-Distance", 		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Y-Distance", 		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Z-Distance",		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);

	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::NORMAL);

	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(  0,   0,   0));
	SetTextColour(*wxLIGHT_GREY);

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

	clientIdItemAttr.SetBackgroundColour(GetBackgroundColour().ChangeLightness(98));
	clientIdItemAttr.SetFont(font.Bold());
	clientIdItemAttr.SetTextColour(GetTextColour());

	initialItemAttr.SetBackgroundColour(GetBackgroundColour());
	initialItemAttr.SetFont(font.Underlined());
	initialItemAttr.SetTextColour(GetTextColour());

	speedItemAttr.SetBackgroundColour(GetBackgroundColour());
	speedItemAttr.SetFont(font.Italic());
	speedItemAttr.SetTextColour(GetTextColour());
	
	defaultItemAttrSelected		= defaultItemAttr;
	initialItemAttrSelected		= initialItemAttr;
	clientIdItemAttrSelected	= clientIdItemAttr;
	speedItemAttrSelected		= speedItemAttr;
	
	defaultItemAttrSelected.SetTextColour(*wxYELLOW);
	defaultItemAttrSelected.SetFont(defaultItemAttrSelected.GetFont().Bold());

	initialItemAttrSelected.SetTextColour(*wxYELLOW);
	initialItemAttrSelected.SetFont(initialItemAttrSelected.GetFont().Bold());

	clientIdItemAttrSelected.SetTextColour(*wxYELLOW);
	clientIdItemAttrSelected.SetFont(clientIdItemAttrSelected.GetFont().Bold());
	
	speedItemAttrSelected.SetTextColour(*wxYELLOW);
	speedItemAttrSelected.SetFont(speedItemAttrSelected.GetFont().Bold());

	wxPoint pt;
	pt.x = 100;
	pt.y = 100;

	wxFont infoFont(18, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Segoe UI"));
	SetFont(font);
	infoText->Show(false);
	infoText->SetFont(infoFont);
	infoText->SetLabel("Disabled, because the corresponding\nconfiguration parameter\n'Work::Preprocessor\\Analyse Pathes' isn't active!");
	infoText->SetPosition(pt);
	infoText->SetForegroundColour(GetTextColour());
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
	const bool b = (item == getLastSelection());
	
	if 		( getItemText(item, COL_TYPE) == "0" ) return (wxListItemAttr*)( b ? (&initialItemAttrSelected)  : (&initialItemAttr) );
	else if	( getItemText(item, COL_TYPE) == "1" ) return (wxListItemAttr*)( b ? (&clientIdItemAttrSelected) : (&clientIdItemAttr) );
	else if ( getItemText(item, COL_TYPE) == "2" ) return (wxListItemAttr*)( b ? (&speedItemAttrSelected)    : (&speedItemAttr) );

	// this indicates to use the default style
	return (wxListItemAttr*)( b ? (&defaultItemAttrSelected) : (&defaultItemAttr) );
}
/////////////////////////////////////////////////////////////
void CncMoveSequenceListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
	
	if ( item < 0 || item > (long)(moveSequence->getCount() -1) )
		return;
	
	setLastSelection(item);
	
	auto it = moveSequence->const_begin() + item;
	const CncMoveSequence::SequencePoint& sp = *it;
	
	SelectEventBlocker blocker(this);
	APP_PROXY::tryToSelectClientId(sp.clientID, ClientIdSelSource::ID::TSS_MOVE_SEQ);
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
		
	if ( COL_STRECH < 0 )
		return;
		
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
}
/////////////////////////////////////////////////////////////
bool CncMoveSequenceListCtrl::searchReference(const wxString& what) {
/////////////////////////////////////////////////////////////
	long ln;
	
	if ( wxString(what).ToLong(&ln) )
		return searchReferenceById(ln);
	
	return false;
}
/////////////////////////////////////////////////////////////
bool CncMoveSequenceListCtrl::searchReferenceById(const long id) {
/////////////////////////////////////////////////////////////
	if ( moveSequence == NULL )
		return false;
		
	for ( auto it=moveSequence->const_begin(); it != moveSequence->const_end(); ++it ) {
		
		if ( id >= it->clientID ) {
			selectItem(std::distance(moveSequence->const_begin(), it), true);
			return true;
		}
	}
	return false;
}
/////////////////////////////////////////////////////////////
void CncMoveSequenceListCtrl::clearAll() {
/////////////////////////////////////////////////////////////
	moveSequence = NULL;
	clear();
}
/////////////////////////////////////////////////////////////
void CncMoveSequenceListCtrl::addMoveSequence(const CncMoveSequence* seq) {
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

	const bool show = THE_CONFIG->getPreProcessorAnalyseFlag() == false && active && moveSequence == NULL;
	infoText->Show(show);
	
	if ( show ) 
		clearAll();
}
/////////////////////////////////////////////////////////////
wxString CncMoveSequenceListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////
	static const wxString fmt(globalStrings.moveSeqRefFormat);

	if ( moveSequence == NULL )
		return _("");
	
	if ( moveSequence->getCount() == 0 ) {
		switch ( column ) {
			case CncMoveSequenceListCtrl::COL_TYPE:		return wxString::Format("%d",		2); 
		}
		
		return _("");
	}

	if ( item < 0 || item > (long)(moveSequence->getCount() - 1) )
		return _("");

	auto it = moveSequence->const_begin() + item;
	const CncMoveSequence::SequencePoint& sp = *it;
	
	switch ( column ) {
		case CncMoveSequenceListCtrl::COL_TYPE:			return wxString::Format("%d",		3); 
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
	EVT_PAINT(			 				CncMoveSequenceOverviewListCtrl::onPaint)
	EVT_LIST_ITEM_SELECTED(wxID_ANY, 	CncMoveSequenceOverviewListCtrl::onSelectListItem)
	EVT_LIST_ITEM_ACTIVATED(wxID_ANY, 	CncMoveSequenceOverviewListCtrl::onActivateListItem)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncMoveSequenceOverviewListCtrl::CncMoveSequenceOverviewListCtrl(wxWindow *parent, long style, CncMoveSequenceListCtrl* slave, wxStaticText* label) 
: CncLargeScaledListCtrl(parent, style)
, defaultItemAttr			()
, defaultItemAttrSelected	()
, moveSequences				()
, slaveSequenceList			(slave)
, contentLabel				(label)
/////////////////////////////////////////////////////////////
{
	wxASSERT( slaveSequenceList != NULL );
	wxASSERT( contentLabel != NULL );
	
	// add colums
	AppendColumn("#",	 			wxLIST_FORMAT_RIGHT,	 44);
	AppendColumn("Count",			wxLIST_FORMAT_RIGHT,	 44);
	AppendColumn("Sequence ID",		wxLIST_FORMAT_LEFT, 	110);
	AppendColumn("Opt.",			wxLIST_FORMAT_CENTRE,	 44);
	AppendColumn("Speed [mm/min]",	wxLIST_FORMAT_RIGHT,	100);
	AppendColumn("Impulses",		wxLIST_FORMAT_RIGHT,	wxLIST_AUTOSIZE);
	AppendColumn("X-Tot Length",	wxLIST_FORMAT_RIGHT,	wxLIST_AUTOSIZE);
	AppendColumn("Y-Tot Length",	wxLIST_FORMAT_RIGHT,	wxLIST_AUTOSIZE);
	AppendColumn("Z-Tot Length",	wxLIST_FORMAT_RIGHT,	wxLIST_AUTOSIZE);
	AppendColumn("First C.-ID",		wxLIST_FORMAT_RIGHT,	 64);
	AppendColumn("Last C.-ID",		wxLIST_FORMAT_RIGHT,	 64);

	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::NORMAL);

	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(  0,   0,   0));
	SetTextColour(*wxLIGHT_GREY);
	
	defaultItemAttr.SetBackgroundColour(GetBackgroundColour());
	defaultItemAttr.SetFont(font);
	defaultItemAttr.SetTextColour(GetTextColour());
	
	defaultItemAttrSelected = defaultItemAttr;
	defaultItemAttrSelected.SetTextColour(*wxYELLOW);
	defaultItemAttrSelected.SetFont(defaultItemAttrSelected.GetFont().Bold());
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
//////////////////////////////////////////////////////////////////
void CncMoveSequenceOverviewListCtrl::onPaint(wxPaintEvent& event) {
//////////////////////////////////////////////////////////////////
	event.Skip();
	
	if ( moveSequences.size() > 0 && GetSelectedItemCount() == 0 ) {
		selectItem(0, true);
	}
}
/////////////////////////////////////////////////////////////
void CncMoveSequenceOverviewListCtrl::addMoveSequence(const CncMoveSequence& seq) {
/////////////////////////////////////////////////////////////
	moveSequences.push_back(seq);
	SetItemCount(moveSequences.size());
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncMoveSequenceOverviewListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	const bool b = (item == getLastSelection());
	
	// this indicates to use the default style
	return (wxListItemAttr*)( b ? (&defaultItemAttrSelected) : (&defaultItemAttr) );
}
/////////////////////////////////////////////////////////////
wxString CncMoveSequenceOverviewListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////
	static const wxString fmt(globalStrings.moveSeqRefFormat);

	if ( isItemValid(item) == false )
		return _("");
		
	const CncMoveSequence& seq = moveSequences.at(item);
	
	switch ( column ) {
		case COL_NUM:			return wxString::Format("%ld", 	item);
		case COL_CNT:			return wxString::Format("%u", 	seq.getCount());
		case COL_REF:			return wxString::Format("%lld",	seq.getReference());
		case COL_OPTIMIZED:		return                          seq.isOptimized() ? "X" : "";
		case COL_SPEED:			return wxString::Format("%.1lf",seq.getCurrentSpeedInfo().value );
		case COL_IMPULSES:		return wxString::Format("%ld",  seq.getImpulseCount());
		case COL_DISTANCE_X:	return wxString::Format("%ld",  seq.getLengthX());
		case COL_DISTANCE_Y:	return wxString::Format("%ld",  seq.getLengthY());
		case COL_DISTANCE_Z:	return wxString::Format("%ld",  seq.getLengthZ());
		case COL_FIRST_CLD_ID:	return wxString::Format(fmt,	seq.getFirstClientId());
		case COL_LAST_CLD_ID:	return wxString::Format(fmt, 	seq.getLastClientId());
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
	const long firstClientId = seq->getFirstClientId();
	const long lastClientId  = seq->getLastClientId();
	
	SelectEventBlocker blocker(this);
	APP_PROXY::tryToSelectClientIds(firstClientId, lastClientId, ClientIdSelSource::ID::TSS_MOVE_SEQ_OVW);

	// update move sequence list control
	wxASSERT( slaveSequenceList != NULL );
	wxASSERT( contentLabel != NULL);
	
	contentLabel->SetLabel(wxString::Format("Content of MoveSequence with ID = '%lld':", seq->getReference()));
	
	typedef IndividualCommandEvent::EvtPreprocessor ID;
	IndividualCommandEvent evt(ID::UpdateSelectedClientIds);
	evt.SetString(seq->getClientIdsAsString());

	wxPostEvent(GetParent(), evt);
	
	slaveSequenceList->addMoveSequence(seq);
	slaveSequenceList->Refresh();
}
/////////////////////////////////////////////////////////////
void CncMoveSequenceOverviewListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	// currently nothing todo
}
/////////////////////////////////////////////////////////////
bool CncMoveSequenceOverviewListCtrl::searchReference(const wxString& what) {
/////////////////////////////////////////////////////////////
	long ln;
	
	if ( wxString(what).ToLong(&ln) )
		return searchReferenceById(ln);
	
	return false;
}
/////////////////////////////////////////////////////////////
bool CncMoveSequenceOverviewListCtrl::searchReferenceById(const long id) {
/////////////////////////////////////////////////////////////
	for ( auto it=moveSequences.begin(); it != moveSequences.end(); ++it ) {
		
		if ( id >= it->getFirstClientId() && id <= it->getLastClientId() ) {
			selectItem(std::distance(moveSequences.begin(), it), true);
			return true;
		}
	}
	
	return false;
}
/////////////////////////////////////////////////////////////
bool CncMoveSequenceOverviewListCtrl::skipToFirstReference() {
/////////////////////////////////////////////////////////////
	if ( moveSequences.size() == 0 )
		return false;
	
	if ( getLastSelection() == 0 ) 
		{ wxBell(); return false; }

	return selectItem(0, true);
}
/////////////////////////////////////////////////////////////
bool CncMoveSequenceOverviewListCtrl::skipToPrevReference() {
/////////////////////////////////////////////////////////////
	if ( moveSequences.size() == 0 )
		return false;
		
	long item = getLastSelection();
	
	if ( isItemValid(item) == false )
		skipToLastReference();
		
	if ( isItemValid(item) == false )
		return false;
		
	const CncMoveSequence* seq	= &(moveSequences.at(item));
	const long firstClientId	= seq->getFirstClientId();
	const long ritem			= moveSequences.size() - 1 - item;
	
	// skip to previous client id
	for ( auto it = moveSequences.rbegin() + ritem; it != moveSequences.rend(); ++it ) {
		if ( it->getFirstClientId() != firstClientId )
			break;
			
		item--;
	}
	
	if ( isItemValid(item) == false ) 
		{ wxBell(); return false; }
		
	return selectItem(item, true);
}
/////////////////////////////////////////////////////////////
bool CncMoveSequenceOverviewListCtrl::skipToNextReference() {
/////////////////////////////////////////////////////////////
	if ( moveSequences.size() == 0 )
		return false;
		
	long item = getLastSelection();
	
	if ( isItemValid(item) == false )
		skipToFirstReference();
		
	if ( isItemValid(item) == false ) 
		{ wxBell(); return false; }
		
	const CncMoveSequence* seq = &(moveSequences.at(item));
	const long lastClientId    = seq->getLastClientId();
	
	// skip to next client id
	for ( auto it = moveSequences.begin() + item; it != moveSequences.end(); ++it ) {
		if ( it->getLastClientId() != lastClientId )
			break;
		
		item++;
	}
	
	if ( isItemValid(item) == false ) 
		{ wxBell(); return false; }
		
	return selectItem(item, true);
}
/////////////////////////////////////////////////////////////
bool CncMoveSequenceOverviewListCtrl::skipToLastReference() {
/////////////////////////////////////////////////////////////
	if ( moveSequences.size() == 0 )
		return false;
		
	const long item = getLastSelection();
	if ( item == (long)(moveSequences.size() - 1) )
		{ wxBell(); return false; }
	
	return selectItem(moveSequences.size() - 1, true);
}

