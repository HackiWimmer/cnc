#include "wxCrafterImages.h"
#include "CncUserEvents.h"
#include "GlobalFunctions.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "CncMoveSequenceListCtrl.h"
#include "CncPathListEntryListCtrl.h"
#include "CncParsingSynopsisTrace.h"
#include "CncLoggerListCtrl.h"
#include "CncFileNameService.h"
#include "CncExternalViewBox.h"
#include "CncPreprocessor.h"

// ----------------------------------------------------------------------------
class CncOperatingTrace : public CncExtLoggerListCtrl {
	public:
		
		CncOperatingTrace(wxWindow *parent, long style)
		: CncExtLoggerListCtrl(parent, style)
		{}
		
		virtual ~CncOperatingTrace()
		{}
		
		typedef wxListItemAttr LIA;
		void addMovSeqSep	(const wxString& s) { const LIA lia(*wxBLACK, wxColour(128, 128, 255), GetFont()); add(s, lia); }
		void addPthLstSep	(const wxString& s) { const LIA lia(*wxBLACK, wxColour(255, 140, 198), GetFont()); add(s, lia); }
};

// ----------------------------------------------------------------------------
// CncPreprocessor Event Table
// ----------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(CncPreprocessor, CncPreprocessorBase)
	EVT_COMMAND(wxID_ANY, wxEVT_INDIVIDUAL_CTRL_COMMAND, 	CncPreprocessor::onIndividualCommand)
wxEND_EVENT_TABLE()

//////////////////////////////////////////////////////////////////
CncPreprocessor::CncPreprocessor(wxWindow* parent)
: CncPreprocessorBase(parent)
, useOperatingTrace				(true)
, pathListEntries				(NULL)
, moveSequenceOverview			(NULL)
, moveSequence					(NULL)
, parsingSynopsis				(NULL)
, operatingTrace				(NULL)
, externalPathListEntriesView	(NULL)
, externalMoveSequenceView		(NULL)
, externalParsingSynopsisView	(NULL)
, externalOperatingTraceView	(NULL)
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
	
	// parsing synopisis
	parsingSynopsis = new CncParsingSynopsisTrace(this); 
	GblFunc::replaceControl(m_motionSynopsisPlaceholder, parsingSynopsis);

	// operating trace
	operatingTrace = new CncOperatingTrace(this, wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_operatingTracePlaceholder, operatingTrace);
	
	externalPathListEntriesView	= new CncExternalViewBox(this);
	externalMoveSequenceView	= new CncExternalViewBox(this);
	externalParsingSynopsisView	= new CncExternalViewBox(this);
	externalOperatingTraceView	= new CncExternalViewBox(this);
	externalPathListEntriesView ->setupView(CncExternalViewBox::Default::VIEW1,		m_plPathListView,			"External PathList Entries View. . . ");
	externalMoveSequenceView	->setupView(CncExternalViewBox::Default::VIEW1,		m_plMoveSequences,			"External Move Sequences View. . . ");
	externalParsingSynopsisView	->setupView(CncExternalViewBox::Default::VIEW1,		m_plParsingSynopsis,		"External Move Sequences View. . . ");
	externalOperatingTraceView	->setupView(CncExternalViewBox::Default::VIEW1,		m_plOperatingTrace,			"External Move Sequences View. . . ");
	
	const wxFont font = THE_CONTEXT->outboundListBookFont;
	m_listbookPreProcessor->GetListView()->SetFont(font);
}
//////////////////////////////////////////////////////////////////
CncPreprocessor::~CncPreprocessor() {
//////////////////////////////////////////////////////////////////
	wxDELETE( externalPathListEntriesView );
	wxDELETE( externalMoveSequenceView );
	wxDELETE( externalParsingSynopsisView );
	wxDELETE( externalOperatingTraceView );

	wxDELETE( pathListEntries );
	wxDELETE( moveSequenceOverview );
	wxDELETE( moveSequence );
	wxDELETE( parsingSynopsis );
	wxDELETE( operatingTrace ); 
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::clearAll() {
//////////////////////////////////////////////////////////////////
	clearPathListEntries();
	clearMoveSequences();
	clearOperatingTrace();
	parsingSynopsis->clearAll();
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
void CncPreprocessor::clearOperatingTrace() {
//////////////////////////////////////////////////////////////////
	operatingTrace->clearAll();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::popProcessMode() {
//////////////////////////////////////////////////////////////////
	pathListEntries->freeze();
	moveSequence->freeze();
	moveSequenceOverview->freeze();
	
	parsingSynopsis->popProcessMode();
	operatingTrace->popProcessMode();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::pushUpdateMode() {
//////////////////////////////////////////////////////////////////
	pathListEntries->thaw();
	moveSequence->thaw();
	moveSequenceOverview->thaw();
	
	parsingSynopsis->pushUpdateMode();
	operatingTrace->pushUpdateMode();
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
void CncPreprocessor::enableOperatingTrace(bool state) {
//////////////////////////////////////////////////////////////////
	wxBitmap bmpOn  = ImageLib16().Bitmap("BMP_CONNECTED");
	wxBitmap bmpOff = ImageLib16().Bitmap("BMP_DISCONNECTED");
	
	state == true ? m_btConnectOperatingTrace->SetBitmap(bmpOn) 			: m_btConnectOperatingTrace->SetBitmap(bmpOff);
	state == true ? m_btConnectOperatingTrace->SetToolTip("Disable List")	: m_btConnectOperatingTrace->SetToolTip("Enable List");
	
	m_btConnectOperatingTrace->SetValue(state);
	m_btConnectOperatingTrace->Refresh();
	m_btConnectOperatingTrace->Update();

	useOperatingTrace = state;
	if ( useOperatingTrace == false )
		clearOperatingTrace();
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
void CncPreprocessor::addOperatingTrace(const wxString& s) {
//////////////////////////////////////////////////////////////////
	if ( useOperatingTrace == false )
		return;
		
	operatingTrace->add(s);
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::addOperatingTrace(const std::stringstream& s) {
//////////////////////////////////////////////////////////////////
	addOperatingTrace(s.str().c_str());
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::addOperatingTraceMovSeqSep(const wxString& s) {
//////////////////////////////////////////////////////////////////
	if ( useOperatingTrace == false )
		return;
		
	operatingTrace->addMovSeqSep(s);
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::addOperatingTracePthLstSep(const wxString& s) {
//////////////////////////////////////////////////////////////////
	if ( useOperatingTrace == false )
		return;
		
	operatingTrace->addPthLstSep(s);
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::addOperatingTraceSeparator(const wxString& s) {
//////////////////////////////////////////////////////////////////
	if ( useOperatingTrace == false )
		return;
		
	operatingTrace->addSeparator(s);
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::addOperatingTraceDebugEntry(const wxString& s) {
//////////////////////////////////////////////////////////////////
	if ( useOperatingTrace == false )
		return;
		
	operatingTrace->addDebugEntry(s);
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::addOperatingTraceWarnEntry(const wxString& s) {
//////////////////////////////////////////////////////////////////
	if ( useOperatingTrace == false )
		return;
		
	operatingTrace->addWarnEntry(s);
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::addOperatingTraceErrorEntry(const wxString& s) {
//////////////////////////////////////////////////////////////////
	if ( useOperatingTrace == false )
		return;
		
	operatingTrace->addErrorEntry(s);
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
void CncPreprocessor::connectOperatingTrace(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	enableOperatingTrace(m_btConnectOperatingTrace->GetValue());
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

//////////////////////////////////////////////////////////////////
void CncPreprocessor::clearOperatingTrace(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	clearOperatingTrace();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::copyOperatingTrace(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	operatingTrace->copyToClipboard(true);
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::saveOperatingTrace(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	const wxString& fileName(wxString::Format("%s%s%s%s",	CncFileNameService::getTempDirSession(), 
															"CncPreprocessorOperatingTrace", 
															wxDateTime::Now().Format("%Y-%m-%d.%H-%M-%S"), 
															".txt")
											 );
	const wxFileName fn(fileName);
	
	operatingTrace->writeToFile(fn, true);
	
	if ( fn.Exists() ) {
		wxString tool;
		CncConfig::getGlobalCncConfig()->getEditorTool(tool);
		GblFunc::executeExternalProgram(tool, fileName, true);
	}
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::connectParsingSynopis(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	#warning CncPreprocessor::connectParsingSynopis impl missing
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::clearParsingSynopis(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	parsingSynopsis->clearAll();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::copyParsingSynopis(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	parsingSynopsis->copyToClipboard(true);
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::saveParsingSynopsis(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	const wxString& fileName(wxString::Format("%s%s%s%s",	CncFileNameService::getTempDirSession(), 
															"CncPreprocessorParsingSynopsis", 
															wxDateTime::Now().Format("%Y-%m-%d.%H-%M-%S"), 
															".txt")
											 );
	const wxFileName fn(fileName);
	
	parsingSynopsis->writeToFile(fn, true);
	
	if ( fn.Exists() ) {
		wxString tool;
		CncConfig::getGlobalCncConfig()->getEditorTool(tool);
		GblFunc::executeExternalProgram(tool, fileName, true);
	}
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::detachView(CncExternalViewBox* viewbox) {
//////////////////////////////////////////////////////////////////
	wxASSERT( viewbox != NULL );
	const bool isExtViewActive = !viewbox->IsShown();
	
	viewbox->selectView(CncExternalViewBox::Default::VIEW1);
	viewbox->Show(isExtViewActive);
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::onDetachPathListEntriesView(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	detachView(externalPathListEntriesView);
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::onDetachMoveSequenceView(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	detachView(externalMoveSequenceView);
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::onDetachParsingSynopsisView(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	detachView(externalParsingSynopsisView);
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::onDetachOperatingTraceView(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	detachView(externalOperatingTraceView);
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::onPathListEntryFirst(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	wxASSERT(pathListEntries);
	pathListEntries->skipToFirstReference();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::onPathListEntryPrev(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	wxASSERT(pathListEntries);
	pathListEntries->skipToPrevReference();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::onPathListEntryNext(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	wxASSERT(pathListEntries);
	pathListEntries->skipToNextReference();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::onPathListEntryLast(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	wxASSERT(pathListEntries);
	pathListEntries->skipToLastReference();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::onMoveSequenceEntryFirst(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	wxASSERT(moveSequenceOverview);
	moveSequenceOverview->skipToFirstReference();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::onMoveSequenceEntryPrev(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	wxASSERT(moveSequenceOverview);
	moveSequenceOverview->skipToPrevReference();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::onMoveSequenceEntryNext(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	wxASSERT(moveSequenceOverview);
	moveSequenceOverview->skipToNextReference();
}
//////////////////////////////////////////////////////////////////
void CncPreprocessor::onMoveSequenceEntryLast(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	wxASSERT(moveSequenceOverview);
	moveSequenceOverview->skipToLastReference();
}
