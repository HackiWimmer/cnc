#include "GlobalFunctions.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "CncMoveSequenceListCtrl.h"
#include "CncPathListEntryListCtrl.h"
#include "CncPreprocessor.h"

//////////////////////////////////////////////////////////////////
CncPreprocessor::CncPreprocessor(wxWindow* parent)
: CncPreprocessorBase(parent)
, pathListEntries(NULL)
//////////////////////////////////////////////////////////////////
{
	// path list entries control
	pathListEntries = new CncPathListEntryListCtrl(this, wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_pathListEntriesPlaceholder, pathListEntries);
	
	// move sequences control
	moveSequences = new CncMoveSequenceListCtrl(this, wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL);
	GblFunc::replaceControl(m_moveSequencesPlaceholder, moveSequences);

	const wxFont font = GBL_CONTEXT->outboundListBookFont;
	m_listbookPreProcessor->GetListView()->SetFont(font);
}
//////////////////////////////////////////////////////////////////
CncPreprocessor::~CncPreprocessor() {
//////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////
	moveSequences->clearAll();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::freeze() {
//////////////////////////////////////////////////////////////////
	pathListEntries->freeze();
	moveSequences->freeze();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::thaw() {
//////////////////////////////////////////////////////////////////
	pathListEntries->thaw();
	moveSequences->thaw();
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
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::addMoveSequenceStart(const CncMoveSequence& seq, double value_MM_MIN, char mode) {
//////////////////////////////////////////////////////////////////
	if ( m_btConnectMoveSequences->GetValue() == false ) 
		return;

	CncMoveSequenceListCtrl::SpeedInfo si;
	si.value = value_MM_MIN;
	si.mode  = mode;

	moveSequences->addMoveSequenceClietId(seq, si);
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::addMoveSequence(const CncMoveSequence& seq) {
//////////////////////////////////////////////////////////////////
	if ( m_btConnectMoveSequences->GetValue() == false ) 
		return;
	
	moveSequences->addMoveSequencePositions(seq);
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
		case LT_PATH_LIST: 		pathListEntries->searchReferenceById(id); 	break;
		case LT_MOVE_SEQUENCE:	moveSequences->searchReferenceById(id); 	break;
	}
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::updateContent() {
//////////////////////////////////////////////////////////////////
	updatePathListContent();
	updateMoveSequenceListContent();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::moveSequenceChangeContent(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	const bool force = true;
	updatePathListContent(force);
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::updateMoveSequenceListContent(bool force) {
//////////////////////////////////////////////////////////////////
	moveSequences->updateContent(force);
	m_moveSequenceRowCount->ChangeValue(wxString::Format("%ld", moveSequences->getItemCount()));
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::updatePathListContent(bool force) {
//////////////////////////////////////////////////////////////////
	CncPathListEntryListCtrl::UpdateContentInfo uci;
	uci.format					= m_btToogleFormat->GetValue();
	uci.considerClientIdChanges	= m_btConsiderClientIdChanges->GetValue();
	uci.considerSpeedChanges	= m_btConsiderFeedSpeedChanges->GetValue();
	uci.considerPositionChnages	= m_btConsiderPositionChanges->GetValue();
	
	pathListEntries->updateContent(uci, force);
	m_pathListRowCount->ChangeValue(wxString::Format("%ld", pathListEntries->getItemCount()));
}
