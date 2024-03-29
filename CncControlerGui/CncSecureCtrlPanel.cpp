#include <wx/richmsgdlg.h>
#include "MainFrame.h"
#include "wxCrafterImages.h"
#include "GlobalStrings.h"
#include "GlobalFunctions.h"
#include "CncCommon.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "CncFilePreview.h"
#include "CncTemplateContext.h"
#include "CncMessageDialog.h"
#include "CncSecureManuallyMovePanel.h"
#include "CncPolarRegionDetector.h"
#include "CncSecureRotateModelPanel.h"
#include "CncSecurePortListCtrl.h"
#include "CncSecureCtrlPanel.h"

/////////////////////////////////////////////////////////////////////
CncSecureCtrlPanel::CncSecureCtrlPanel(wxWindow* parent)
: CncSecureCtrlPanelBase						(parent)
, CncSecureSlidepad::CallbackInterface			()
, CncReferenceEvaluation::CallbackInterface		()
, CncPodiumMgmtMovement::CallbackInterface		()
, portSelectorList								(NULL)
, manuallyMovePanel								(NULL)
, rotateModelPanel								(NULL)
, interactiveMoveX								(NULL)
, interactiveMoveY								(NULL)
, interactiveMoveZ								(NULL)
, interactiveTouchpadXYZ						(NULL)
, podiumPanel									(NULL)
, referencePanel								(NULL)
, speedpad										(NULL)
, pageVector									()
/////////////////////////////////////////////////////////////////////
{
	// ---------------------------------------------------------------
	auto setPageWidth = [&](wxWindow* page, int width){
		int pos = m_rightBook->FindPage(page);
		if ( pos >=0 && pos < (int)pageVector.size() )
			pageVector.at(pos).width = width;
	};

	// setup additional page information
	wxASSERT( m_leftBook->GetPageCount() ==  m_rightBook->GetPageCount() );
	for (unsigned int i = 0; i < m_leftBook->GetPageCount(); i++ ) {
		PageInfo pi;
		pageVector.push_back(pi);
	}
	
	setPageWidth(m_rpManually,	800);
	setPageWidth(m_rpRef,		800);
	setPageWidth(m_rpMisc,		800);
	
	portSelectorList = new CncSecurePortListCtrl(this, this, wxLC_HRULES | wxLC_SINGLE_SEL | wxLC_NO_HEADER); 
	GblFunc::replaceControl(m_portSelectorPlaceholder, portSelectorList);
	
	manuallyMovePanel = new CncSecureManuallyMovePanel(this); 
	GblFunc::replaceControl(m_manuallyMovePlaceholder, manuallyMovePanel);
	
	rotateModelPanel  = new CncSecureRotateModelPanel(this);
	GblFunc::replaceControl(THE_APP->GetSecRotateSliderPlaceholder(), rotateModelPanel);
	
	interactiveMoveX = new CncSecureGesturesPanel(this, wxVERTICAL, CncSecureGesturesPanel::Type::T_BUTTON, CncSecureGesturesPanel::Mode::M_BOTH, 5);
	GblFunc::replaceControl(m_interactiveMoveXPlaceholder, interactiveMoveX);
	interactiveMoveX->setCallbackId(CallbackID_SPX);
	interactiveMoveX->SetBackgroundColour(wxColour(255, 128, 128));
	interactiveMoveX->setCenterBitmap(ImageLibSecure().Bitmap("BMP_NAVI_UP_DOWN32"));
	
	interactiveMoveY = new CncSecureGesturesPanel(this, wxVERTICAL, CncSecureGesturesPanel::Type::T_BUTTON, CncSecureGesturesPanel::Mode::M_BOTH, 5);
	GblFunc::replaceControl(m_interactiveMoveYPlaceholder, interactiveMoveY);
	interactiveMoveY->setCallbackId(CallbackID_SPY);
	interactiveMoveY->SetBackgroundColour(wxColour(  0, 120, 215));
	interactiveMoveY->setCenterBitmap(ImageLibSecure().Bitmap("BMP_NAVI_UP_DOWN32"));
	
	interactiveMoveZ = new CncSecureGesturesPanel(this, wxVERTICAL, CncSecureGesturesPanel::Type::T_BUTTON, CncSecureGesturesPanel::Mode::M_BOTH, 5);
	GblFunc::replaceControl(m_interactiveMoveZPlaceholder, interactiveMoveZ);
	interactiveMoveZ->setCallbackId(CallbackID_SPZ);
	interactiveMoveZ->SetBackgroundColour(wxColour(  0, 128,  0));
	interactiveMoveZ->setCenterBitmap(ImageLibSecure().Bitmap("BMP_NAVI_UP_DOWN32"));
	
	interactiveTouchpadXYZ = new CncSecureGesturesPanel(this, wxBOTH, CncSecureGesturesPanel::Type::T_BUTTON, CncSecureGesturesPanel::Mode::M_BOTH, 5);
	GblFunc::replaceControl(m_interactiveTouchpadXYZ, interactiveTouchpadXYZ);
	interactiveTouchpadXYZ->setCallbackId(CallbackID_TPXY);
	interactiveTouchpadXYZ->SetBackgroundColour(wxColour(255, 255, 184));
	
	podiumPanel = new CncPodiumMgmtMovement(this); 
	GblFunc::replaceControl(m_podiumPlaceholder, podiumPanel);
	podiumPanel->setCallbackInterface(this);
	
	referencePanel = new CncReferenceEvaluation(this); 
	GblFunc::replaceControl(m_evaluateReferencePlaceholder, referencePanel);
	referencePanel->setCallbackInterface(this);
	
	Bind(wxEVT_CNC_SECURE_GESTURES_PANEL, &CncSecureCtrlPanel::onInteractiveMove,	this);
	
	THE_APP->GetSecureVersionInfo()->SetLabel(CNC_VERSION_LONG_STR);
}
/////////////////////////////////////////////////////////////////////
CncSecureCtrlPanel::~CncSecureCtrlPanel() {
/////////////////////////////////////////////////////////////////////
	wxDELETE(portSelectorList);
	wxDELETE(manuallyMovePanel);
	wxDELETE(rotateModelPanel);
	wxDELETE(podiumPanel);
	wxDELETE(referencePanel);
	wxDELETE(speedpad);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::notifyResetMonitorView() {
/////////////////////////////////////////////////////////////////////
	rotateModelPanel->reset();
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onInteractiveMove(CncSecureGesturesPanelEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( THE_APP->getCncControl()->isConnected() == false )
		return;
		
	wxWindow* evtWnd = ((wxWindow*)event.GetEventObject());
	const bool isShownOnScreen = ( evtWnd && evtWnd->IsShownOnScreen() );
	
	if ( false )
	{
		// debug only
		if ( event.data.cbId == 1 )
			cnc::cex2 << event << std::endl;
	}
	
	switch( event.GetId() )
	{
		case CncSecureGesturesPanelEvent::Id::CSGP_STARTING:
		{
			if ( isShownOnScreen == true )
			{
				if ( THE_APP->isInteractiveMoveActive() == false )
				{
					const int pos = cnc::getSpeedStepSensitivityIndex(FINE);
					THE_APP->selectStepSensitivity(pos);
					
					if ( THE_APP->startInteractiveMove(CncInteractiveMoveDriver::IMD_NAVIGATOR) == false )
						CNC_CERR_FUNCT_A("startInteractiveMove() failed")
				}
			}
			break;
		}
		case CncSecureGesturesPanelEvent::Id::CSGP_POS_HELD:
		{
			if ( isShownOnScreen == true )
			{
				// is a interactive move active?
				if ( THE_APP->isInteractiveMoveActive() == false )
				{
					CNC_CERR_FUNCT_A("THE_APP->isInteractiveMoveActive() == false")
					return;
				}
			
				if ( THE_APP->updateInteractiveMove() == false )
					CNC_CERR_FUNCT_A("updateInteractiveMove() failed")
			}
			break;
		}
		case CncSecureGesturesPanelEvent::Id::CSGP_POS_CHANGED:
		{
			// stop (always!!!)
			if ( event.data.isZero() )
			{
				if ( THE_APP->stopInteractiveMove() == false )
					CNC_CERR_FUNCT_A("stopInteractiveMove() failed")
					
				if ( THE_APP->isInteractiveMoveActive() == true )
					CNC_CERR_FUNCT_A("InteractiveMove isn't stopped")
					
				return;
			}
			
			// may be nothing to to
			if ( event.data.isChanged() == false )
				return;
				
			// is a interactive move active?
			if ( THE_APP->isInteractiveMoveActive() == false )
			{
				//CNC_CERR_FUNCT_A("THE_APP->isInteractiveMoveActive() == false")
				return;
			}
				
			// determine direction(s)
			CncLinearDirection dx = CncNoneDir;
			CncLinearDirection dy = CncNoneDir;
			CncLinearDirection dz = CncNoneDir;
			
			bool move = false;
			switch ( event.data.cbId ) 
			{
				case CallbackID_SPX:
				{
					dx = ( event.data.range != 0 ? ( event.data.range < 0  ? CncNegDir : CncPosDir ) : CncNoneDir );
					move = true;
					break;
				}
				case CallbackID_SPY:
				{	
					dy = ( event.data.range != 0 ? ( event.data.range < 0  ? CncNegDir : CncPosDir ) : CncNoneDir );
					move = true;
					break;
				}
				case CallbackID_SPZ:
				case CallbackID_TPZ:
				{
					dz = ( event.data.range != 0 ? ( event.data.range < 0  ? CncNegDir : CncPosDir ) : CncNoneDir );
					move = true;
					break;
				}
				case CallbackID_TPXY:
				{
					CncPolarRegionDetector prd(event.data.xVal, event.data.yVal);
					dx = prd.getDirectionX();
					dy = prd.getDirectionY();
					move = true;
					break;
				}
			}
			
			if ( move && isShownOnScreen )
			{
				const int modifySpeed = abs(event.data.range);
				
				if ( THE_APP->updateInteractiveMove(dx, dy, dz, modifySpeed) == false )
					CNC_CERR_FUNCT_A("updateInteractiveMove(dx, dy, dz) failed")
			}
			
			break;
		}
	}
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::sliderValueChanged(int pos, int value) {
/////////////////////////////////////////////////////////////////////
	// Notification from navigator speed slider
	THE_APP->selectStepSensitivity(pos);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onLeftBookPageChanged(int oldSel) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(m_leftBook->GetPageCount() == m_rightBook->GetPageCount() );
	
	const int newSel = m_leftBook->GetSelection();
	m_rightBook->SetSelection(newSel);
	
	if ( newSel >=0 && newSel < (int)pageVector.size() )
		THE_APP->GetSecureSplitterMainV()->SetSashPosition(pageVector.at(newSel).width);
	
	performRightHeadline();
	
	//---------------------------------------------------------------
	switch ( oldSel )
	{
		case PAGE_PODIUM:	THE_APP->applyPodiumDistance();
							podiumPanel->close();
							THE_APP->waitActive(500);
							
							CncIdleCheckDeactivator::activate(true);
							break;
	}
	
	//---------------------------------------------------------------
	switch ( newSel ) 
	{
		case PAGE_PODIUM:	CncIdleCheckDeactivator::activate(false);
		
							THE_APP->resetPodiumDistance();
							podiumPanel->init();
							break;
							
		case PAGE_REF:		referencePanel->init();
							break;
	}
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onLeftBookPageChanged(wxListbookEvent& event) {
/////////////////////////////////////////////////////////////////////
	onLeftBookPageChanged(event.GetOldSelection());
}
/////////////////////////////////////////////////////////////////////
int CncSecureCtrlPanel::aktivateReferencePanel() {
/////////////////////////////////////////////////////////////////////
	m_leftBook->SetSelection(PAGE_REF);
	
	return wxID_OK;
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onEmergencySec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->emergencyStop(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onRunSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	// first some questionnaires . . . 
	CncControl* cnc = THE_APP->getCncControl();
	if ( cnc && cnc->dryRunAvailable() )
	{
		if ( THE_TPL_CTX->isValid() == false )
		{
			wxString msg(wxString::Format(	"The current Template status is not valid!\n" \
											" - The count of (dry)runs is %u\n" \
											" - The count of valid (dry)runs is %u\n\n" \
											"Choose what you really want?",
											THE_TPL_CTX->getRunCount(),
											THE_TPL_CTX->getValidRunCount()
										  )
			);
			
			wxRichMessageDialog dlg(this, msg, _T("Template State . . . "), 
								wxYES|wxNO|wxCANCEL|wxCENTRE);
			
			dlg.SetFooterText("It is strictly recommend to perform a Template Run only based on a Dry Run with a valid state!");
			dlg.SetFooterIcon(wxICON_WARNING);
			dlg.SetYesNoCancelLabels("Start a Dry Run", "Start a Run", "Cancel");
			
			const int ret = dlg.ShowModal();
			
			if      ( ret == wxID_CANCEL )
			{ 
				// still return 
				return; 
			}
			else if ( ret == wxID_YES )
			{
				// start a dry run and return
				THE_APP->rcDryRun(event); 
				return; 
			}
			// else
			// ... starting run
		}
		else
		{
			wxString msg("The current Template is ready to run.");
			
			wxRichMessageDialog dlg(this, msg, _T("Template State . . . "), 
								wxOK|wxCANCEL|wxCENTRE);
								
			dlg.SetFooterText("The final starting shot . . .");
			dlg.SetFooterIcon(wxICON_QUESTION);
			dlg.SetOKCancelLabels("Start Run", "Cancel");
			const int ret = dlg.ShowModal();
			
			if ( ret == wxID_CANCEL )
			{
				CNC_CLOG_A("The run was cancelled by user")
				return;
			}
			// else
			// ... starting run
		}
	}
	
	// then starting run . . . 
	THE_APP->rcRun(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onPauseSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->rcPause(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onStopSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->rcStop(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onConnectSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	setPortSelection(wxEmptyString);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::setPortSelection(const wxString& portName) {
/////////////////////////////////////////////////////////////////////
	GblGuiCtrlDisabler gcd(portSelectorList);
	
	if ( portName.IsEmpty() )
	{
		THE_APP->connectSerialPortDialog();
	}
	else
	{
		THE_APP->GetPortSelector()->SetStringSelection(portName);
		THE_APP->selectPort();
	}
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onEvaluateHardwareReference(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->onEvaluateHardwareReference(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onResetSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->rcReset(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onDetermineAnchorPositionsSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->setAnchorPosition(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::lockSelection(bool b) {
/////////////////////////////////////////////////////////////////////
	m_leftBook->Enable(b == false);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::performRightHeadline() {
/////////////////////////////////////////////////////////////////////
	const int sel = m_leftBook->GetSelection();
	wxString hl(wxString::Format("%s:\n", m_leftBook->GetPageText(sel)));
	hl.Replace("\n", " ", true);
	
	m_rightHeadline->SetLabel(hl);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::tryToProvideTemplate() {
/////////////////////////////////////////////////////////////////////
	wxFileName fn(THE_APP->getCurrentTemplateFileName());
	if ( fn.Exists() == false ) {
		
		wxString firstLruFile;
		if ( THE_APP->getFirstLruFile(firstLruFile) )
			fn.Assign(firstLruFile);
			
		bool fileOpened = false;
		if ( fn.Exists() == true ) {
			THE_APP->setTemplateName(fn);
			fileOpened = THE_APP->openFile();
		}
		
		if ( fileOpened == false )
			THE_APP->openTemplate();
	}
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::updatePortSelection(const wxString& portName) {
/////////////////////////////////////////////////////////////////////
	portSelectorList->selectPortInList(portName);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::addPortName(const wxString& portName, const wxString& imageName) {
/////////////////////////////////////////////////////////////////////
	portSelectorList->addPortEntry(portName, imageName);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::clearPortSelection() {
/////////////////////////////////////////////////////////////////////
	portSelectorList->deleteAllEntries();
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::notifyConnection(bool state, const wxString& portName) {
/////////////////////////////////////////////////////////////////////
	const wxBitmap bmpC = ImageLib16().Bitmap("BMP_OK16");
	const wxBitmap bmpD = ImageLib16().Bitmap("BMP_WARNING16");
	
	m_portName->SetLabel(portName);
	m_portName->Refresh();
	
	m_bmpConnectionStateSecure->SetBitmap( state == true ? bmpC : bmpD);
	m_bmpConnectionStateSecure->Refresh();
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onOpenTemplateSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->openTemplate(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onAppEnvironmentSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->showOSEnvironment(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onSessionDirSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->openSessionDialog(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onTestFunction1Sec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->testFunction1(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onTestFunction2Sec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->testFunction2(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onTestFunction3Sec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->testFunction3(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onTestFunction4Sec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->testFunction4(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onEditTemplateSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->detachTemplateSource(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onOpenConfigurationDlgSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->detachConfiguration(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onOpenConfigurationFileSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->openConfigurationFile(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onStackTraceStoreSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->showStacktraceStore(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onSerialSpySec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxAuiPaneInfo& pane = THE_APP->GetAuimgrMain()->GetPane("SerialSpy");
	if ( pane.IsFloating() == false )
		pane.Float();
		
	if ( pane.IsShown() == false )
		pane.Show(true);
		
	const wxSize size =  THE_APP->GetClientSize();
	pane.FloatingSize(size.GetX() / 3, size.GetY() * 0.8 );
		
	THE_APP->GetAuimgrMain()->Update();
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onRequestResolveLimitStates(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->requestResolveLimitStates(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::referenceNotifyMessage(const wxString& msg, int flags) {
//////////// /////////////////////////////////////////////////////////
	wxString m(msg);
	while ( m.EndsWith("\n") )
		m.RemoveLast();
	
	m_referenceInfobar->ShowMessage(m, flags);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::referenceDismissMessage() {
/////////////////////////////////////////////////////////////////////
	m_referenceInfobar->Dismiss();
	Layout();
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::cameraNotifyPreview(bool show) {
/////////////////////////////////////////////////////////////////////
	if ( show == true )	THE_APP->GetSecurePreviewBook()->SetSelection(SecurePreviewBookSelection::VAL::CAMERA_PREVIEW);
	else				THE_APP->GetSecurePreviewBook()->SetSelection(SecurePreviewBookSelection::VAL::RIGHT_PREVIEW);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::podiumNotifyEnable(bool state)  {
/////////////////////////////////////////////////////////////////////
	m_leftBook->Enable(state);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::podiumNotifyInit(bool state) {
/////////////////////////////////////////////////////////////////////
	if ( state == false )
		CNC_PRINT_FUNCT
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::podiumNotifyClose(bool state) {
/////////////////////////////////////////////////////////////////////
	if ( state == false )
		CNC_PRINT_FUNCT
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onReferenceSet(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	referencePanel->set();
	
	RefPosResult parameter;
	referencePanel->getResult(parameter);
	THE_APP->updateReferencePosition(&parameter);
	
	cameraNotifyPreview(false);
	m_leftBook->SetSelection(PAGE_RUN);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onDryRunSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->rcDryRun(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onTemplateContextSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->openTemplateContextView();
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onToggleTouchpadPane(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	interactiveTouchpadXYZ->init();
	
	const int id = interactiveTouchpadXYZ->getCallbackId();
	switch ( id ) 
	{
		case CallbackID_TPXY:
			((wxToggleButton*)event.GetEventObject())->SetLabel("Z Axis");
			interactiveTouchpadXYZ->setCallbackId(CallbackID_TPZ);
			interactiveTouchpadXYZ->setOrientation(wxVERTICAL);
			interactiveTouchpadXYZ->SetBackgroundColour(wxColour(  0, 128,  0));
			interactiveTouchpadXYZ->setCenterBitmap(ImageLibSecure().Bitmap("BMP_NAVI_UP_DOWN32"));
			break;
			
		case CallbackID_TPZ:
			((wxToggleButton*)event.GetEventObject())->SetLabel("XY Plane");
			interactiveTouchpadXYZ->setCallbackId(CallbackID_TPXY);
			interactiveTouchpadXYZ->setOrientation(wxBOTH);
			interactiveTouchpadXYZ->SetBackgroundColour(wxColour(255, 255, 184));
			interactiveTouchpadXYZ->setCenterBitmap(ImageLibSecure().Bitmap("BMP_CROSSHAIR"));
			break;
	}
	
	interactiveTouchpadXYZ->update();
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onToggleHeartbeatsSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->toggleIdleRequests(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::activate(bool activating) {
/////////////////////////////////////////////////////////////////////
	if ( activating ) 
	{
		// the secure mode will be activated
		
		if ( THE_CONTEXT->secureModeInfo.isActivatedByStartup == true ) 
			tryToProvideTemplate();
		
		const int sel = PAGE_CONNECT;
		m_leftBook->SetSelection(sel);
		m_rightBook->SetSelection(sel);
		
		THE_APP->GetSecureSplitterMainV()->SetSashPosition(pageVector.at(sel).width);
		performRightHeadline();
	}
	else 
	{
		// the secure mode will be deactivated
		// ...
	}
	
	// This has to be done independently from activate, because
	// it has to be done in both directions to eliminate 
	// view modifications and fit a good presentation isf the view
	// swaps from "secure" to "normal" and back
	THE_APP->mainFilePreview->normalizeView();
	THE_APP->monitorFilePreview->normalizeView();
	
	// ... define more here 
}
