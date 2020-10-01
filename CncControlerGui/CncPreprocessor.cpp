#include "wxCrafterImages.h"
#include "CncUserEvents.h"
#include "GlobalFunctions.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "CncMoveSequenceListCtrl.h"
#include "CncPathListEntryListCtrl.h"
#include "CncPreprocessor.h"

// ----------------------------------------------------------------------------
// CncPreprocessor Event Table
// ----------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(CncPreprocessor, CncPreprocessorBase)
	EVT_COMMAND(wxID_ANY, wxEVT_INDIVIDUAL_CTRL_COMMAND, 	CncPreprocessor::onIndividualCommand)
wxEND_EVENT_TABLE()

//////////////////////////////////////////////////////////////////
CncPreprocessor::CncPreprocessor(wxWindow* parent)
: CncPreprocessorBase(parent)
, pathListEntries(NULL)
, moveSequenceOverview(NULL)
, moveSequence(NULL)
//////////////////////////////////////////////////////////////////
{
	// path list entries control
	pathListEntries = new CncPathListEntryListCtrl(this, wxLC_HRULES | wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_pathListEntriesPlaceholder, pathListEntries);
	
	// move sequences control
	moveSequence = new CncMoveSequenceListCtrl(this, wxLC_HRULES | wxLC_SINGLE_SEL);
	GblFunc::replaceControl(m_moveSequencesPlaceholder, moveSequence);

	// move sequences control
	moveSequenceOverview = new CncMoveSequenceOverviewListCtrl(this, wxLC_HRULES | wxLC_SINGLE_SEL, moveSequence, m_contentLabel);
	GblFunc::replaceControl(m_moveSequencesListPlaceholder, moveSequenceOverview);

	const wxFont font = THE_CONTEXT->outboundListBookFont;
	m_listbookPreProcessor->GetListView()->SetFont(font);
}
//////////////////////////////////////////////////////////////////
CncPreprocessor::~CncPreprocessor() {
//////////////////////////////////////////////////////////////////
	delete pathListEntries;
	delete moveSequenceOverview;
	delete moveSequence;
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::clearAll() {
//////////////////////////////////////////////////////////////////
	clearPathListEntries();
	clearMoveSequences();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::clearPathListEntries() {
//////////////////////////////////////////////////////////////////
	pathListEntries->clearAll();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::clearMoveSequences() {
///////////////////////////////////////////////////////////
	moveSequenceOverview->clearAll();
	moveSequence->clearAll();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::freeze() {
//////////////////////////////////////////////////////////////////
	pathListEntries->freeze();
	moveSequence->freeze();
	moveSequenceOverview->freeze();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::thaw() {
//////////////////////////////////////////////////////////////////
	pathListEntries->thaw();
	moveSequence->thaw();
	moveSequenceOverview->thaw();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::enablePathListEntries(bool state) {
//////////////////////////////////////////////////////////////////
	wxBitmap bmpOn  = ImageLib16().Bitmap("BMP_CONNECTED");
	wxBitmap bmpOff = ImageLib16().Bitmap("BMP_DISCONNECTED");
	
	state == true ? m_btConnectPathListEntries->SetBitmap(bmpOn) 			: m_btConnectPathListEntries->SetBitmap(bmpOff);
	state == true ? m_btConnectPathListEntries->SetToolTip("Disable List")	: m_btConnectPathListEntries->SetToolTip("Enable List");
	
	m_btConnectPathListEntries->SetValue(state);
	
	m_btConnectPathListEntries->Refresh();
	m_btConnectPathListEntries->Update();
	
	if ( state == false )
		clearPathListEntries();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::enableMoveSequences(bool state) {
//////////////////////////////////////////////////////////////////
	wxBitmap bmpOn  = ImageLib16().Bitmap("BMP_CONNECTED");
	wxBitmap bmpOff = ImageLib16().Bitmap("BMP_DISCONNECTED");
	
	state == true ? m_btConnectMoveSequences->SetBitmap(bmpOn) 				: m_btConnectMoveSequences->SetBitmap(bmpOff);
	state == true ? m_btConnectMoveSequences->SetToolTip("Disable List")	: m_btConnectMoveSequences->SetToolTip("Enable List");
	
	m_btConnectMoveSequences->SetValue(state);
	
	m_btConnectMoveSequences->Refresh();
	m_btConnectMoveSequences->Update();
	
	if ( state == false )
		clearMoveSequences();
		
	moveSequence->setActive(state);
	moveSequence->Refresh();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::addMoveSequence(const CncMoveSequence& seq) {
//////////////////////////////////////////////////////////////////
	if ( m_btConnectMoveSequences->GetValue() == false ) 
		return;
		
	moveSequenceOverview->addMoveSequence(seq);
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::addPathListEntry(const CncPathListEntry& cpe) {
//////////////////////////////////////////////////////////////////
	if ( m_btConnectPathListEntries->GetValue() == false )
		return;
	
	pathListEntries->addPathListEntry(cpe);
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::clearMoveSequences(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	clearMoveSequences();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::clearPathListEntries(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	clearPathListEntries();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::connectMoveSequences(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	enableMoveSequences(m_btConnectMoveSequences->GetValue());
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::connectPathListEntries(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	enablePathListEntries(m_btConnectPathListEntries->GetValue());
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::selectClientId(long id, ListType lt) {
//////////////////////////////////////////////////////////////////
	switch ( lt ) {
		case LT_PATH_LIST: 			pathListEntries->searchReferenceById(id); 		break;
		case LT_MOVE_SEQ_OVERVIEW:	moveSequenceOverview->searchReferenceById(id); 	break;
		case LT_MOVE_SEQ_CONTENT:	moveSequence->searchReferenceById(id); 			break;
	}
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::updateContent() {
//////////////////////////////////////////////////////////////////
	updatePathListContent();
	updateMoveSequenceListContent();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::updateMoveSequenceListContent(bool force) {
//////////////////////////////////////////////////////////////////
	m_moveSequenceRowCount->ChangeValue(wxString::Format("%ld", (long)(moveSequence->GetItemCount())));
	
	if ( moveSequenceOverview->IsShownOnScreen() == true ) 
		moveSequenceOverview->Refresh();
		
	if ( moveSequence->IsShownOnScreen() == true ) 
		moveSequence->Refresh();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::updatePathListContent() {
//////////////////////////////////////////////////////////////////
	m_pathListRowCount->ChangeValue(wxString::Format("%ld", (long)(pathListEntries->GetItemCount())));
	
	if ( pathListEntries->IsShownOnScreen() == true ) 
		pathListEntries->Refresh();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::onIndividualCommand(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	IndividualCommandEvent* ice = static_cast<IndividualCommandEvent*>(&event);
	
	typedef IndividualCommandEvent::EvtPreprocessor ID;

	switch ( ice->GetId() ) {
		case ID::UpdateSelectedClientIds:	m_selectedClientIds->ChangeValue(ice->GetString());
											break;
	}
}

