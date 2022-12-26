#include <iostream>
#include <iomanip>  
#include <sstream>
#include <sstream>
#include <fstream>
#include <cctype>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <wx/datetime.h>
#include <wx/numdlg.h>
#include <wx/debug.h>
#include <wx/msgdlg.h>
#include <wx/aboutdlg.h>
#include <wx/fileconf.h>
#include <wx/filedlg.h>
#include <wx/event.h>
#include <wx/utils.h> 
#include <wx/textentry.h>
#include <wx/valnum.h>
#include <wx/dcclient.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/evtloop.h>
#include <wx/dataview.h>
#include <wx/wfstream.h>
#include <wx/datstrm.h>
#include <wx/txtstrm.h>
#include <wx/vscroll.h>
#include <wx/textdlg.h>
#include <wx/clipbrd.h>
#include <wx/version.h> 
#include <wx/gdicmn.h>
#include <wx/richmsgdlg.h>
#include <wx/richtooltip.h>
#include <wx/uiaction.h>
#include <boost/version.hpp>
#include "Codelite/cl_aui_dock_art.h"
#include "OSD/CncUsbPortScanner.h"
#include "OSD/CncAsyncKeyboardState.h"
#include "OSD/webviewOSD.h"
#include "OSD/CncCameraHelper.h"
#include "CncGamepadSpy.h"
#include "GamepadEvent.h"
#include "CncGamepadFilter.h"
#include "CncGamepadControllerState.h"
#include "SerialThread.h"
#include "CncMillingSoundController.h"
#include "CncSimuHwDimensionSetup.h"
#include "CncExceptions.h"
#include "CncTextCtrl.h"
#include "CncLoggerProxy.h"
#include "CncLoggerView.h"
#include "CncLoggerListCtrl.h"
#include "CncBoundarySpace.h"
#include "CncTemplateContext.h"
#include "CncSourceEditor.h"
#include "CncOutboundEditor.h"
#include "CncNumberFormatter.h"
#include "CncSpeedSlider.h"
#include "GlobalFunctions.h"
#include "SerialThreadStub.h"
#include "SerialPort.h"
#include "CncPosition.h"
#include "CncAnchorPosition.h"
#include "CncPatternDefinitions.h"
#include "CncUnitCalculator.h"
#include "CncStartPositionResolver.h"
#include "CncFileNameService.h"
#include "CncFilePreviewWnd.h"
#include "CncSpeedPlayground.h"
#include "SVGPathHandlerCnc.h"
#include "ManuallyParser.h"
#include "SVGFileParser.h"
#include "GCodeFileParser.h"
#include "BinaryPathHandlerCnc.h"
#include "CncArduino.h"
#include "SvgEditPopup.h"
#include "HexDecoder.h"
#include "UnitTestFrame.h"
#include "CncReferencePosition.h"
#include "CncSpeedMonitor.h"
#include "CncUsbConnectionObserver.h"
#include "CncConnectProgress.h"
#include "CncMonitorSplitterWindow.h"
#include "CncMotionVertexTrace.h"
#include "CncTemplateObserver.h"
#include "CncFileViewLists.h"
#include "CncPodiumManagement.h"
#include "CncExternalViewBox.h"
#include "CncSecureCtrlPanel.h"
#include "GL3DOptionPane.h"
#include "GL3DDrawPane.h"
#include "CncPreprocessor.h"
#include "CncGCodeSequenceListCtrl.h"
#include "CncStatisticsPane.h"
#include "CncSvgControl.h"
#include "CncOpenGLContextObserver.h"
#include "CncOSEnvironmentDialog.h"
#include "CncContext.h"
#include "CncTestRunConfig.h"
#include "CncManuallyMoveCoordinates.h"
#include "CncPositionStorageView.h"
#include "CncLastProcessingTimestampSummary.h"
#include "CncFileDialog.h"
#include "CncArduinoEnvironment.h"
#include "CncTemplateContextSummaryPanel.h"
#include "CncTemplateContextSummaryDialog.h"
#include "CncLCDPositionPanel.h"
#include "CncUserEvents.h"
#include "CncInfoBar.h"
#include "GlobalStrings.h"
#include "wxCrafterLCDPanel.h"
#include "wxCrafterImages.h"
#include "MainFrame.h"

#ifdef __WXMSW__
	// special includes for WindowPoc handling. 
	// they have to be at the end of the list to avoid compilation errors
	#include <windows.h>
	#include <dbt.h>
#endif

////////////////////////////////////////////////////////////////////
extern GlobalConstStringDatabase globalStrings;
extern void GlobalStreamRedirectionReset();

////////////////////////////////////////////////////////////////////
unsigned int CncTransactionLockBase::referenceCounter		= 0;
unsigned int CncGampadDeactivator::referenceCounter			= 0;
unsigned int CncIdleCheckDeactivator::referenceCounter		= 0;

////////////////////////////////////////////////////////////////////
// app defined events
	wxDEFINE_EVENT(wxEVT_GAMEPAD_THREAD, 					GamepadEvent);
	wxDEFINE_EVENT(wxEVT_SERIAL_THREAD, 					SerialEvent);
	wxDEFINE_EVENT(wxEVT_PERSPECTIVE_TIMER, 				wxTimerEvent);
	wxDEFINE_EVENT(wxEVT_DEBUG_USER_NOTIFICATION_TIMER, 	wxTimerEvent);
	wxDEFINE_EVENT(wxEVT_TRACE_FROM_THREAD,					wxThreadEvent);
	wxDEFINE_EVENT(wxEVT_DISPATCH_ALL,						wxThreadEvent);
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// app defined event table
	wxBEGIN_EVENT_TABLE(MainFrame, MainFrameBClass)
		EVT_CLOSE(													MainFrame::onClose)
		EVT_COMMAND(wxID_ANY, wxEVT_INDIVIDUAL_CTRL_COMMAND, 		MainFrame::onIndividualCommand)
		EVT_COMMAND(wxID_ANY, wxEVT_CONFIG_UPDATE_NOTIFICATION, 	MainFrame::onConfigurationUpdated)
		EVT_TIMER(wxEVT_PERSPECTIVE_TIMER, 							MainFrame::onPerspectiveTimer)
		EVT_TIMER(wxEVT_DEBUG_USER_NOTIFICATION_TIMER, 				MainFrame::onDebugUserNotificationTimer)
	wxEND_EVENT_TABLE()
////////////////////////////////////////////////////////////////////

#define MF_PRINT_LOCATION_CTX_FILE			//	CNC_PRINT_LOCATION
#define MF_PRINT_LOCATION_CTX_SOMETHING		//	CNC_PRINT_LOCATION

#define CNC_TRANSACTION_LOCK \
	CncTransactionLock lock(this); \
	if ( lock.isOk() == false ) { \
		std::cerr << CNC_LOG_FUNCT_A(": Creation of CncTransactionLock is failed!\n"); \
		return; \
	}
	
#define CNC_TRANSACTION_LOCK_RET_ON_ERROR(RET_VAL) \
	CncTransactionLock lock(this); \
	if ( lock.isOk() == false ) { \
		std::cerr << CNC_LOG_FUNCT_A(": Creation of CncTransactionLock is failed!\n"); \
		return RET_VAL; \
	}

// The filter below suppressed several cnc navigator events to ensure that a interactive move 
// cant be started twice. The main reason is that the cnc controller call dispatchAll() from time to time 
// to update gui objects and accept user input, but never such events as filtered.
#define CNC_INTERACTIVE_MOVE_FILTER CncInteractiveMoveEventFilter InteractiveMoveFilter;

////////////////////////////////////////////////////////////////////
CncTransactionLockBase::CncTransactionLockBase(MainFrame* p)
: parent			(p)
, state				(false)
, errorMode			(false)
////////////////////////////////////////////////////////////////////
{
	wxASSERT(parent);
	parent->stopSerialTimer();
	
	referenceCounter++;
}
////////////////////////////////////////////////////////////////////
CncTransactionLockBase::~CncTransactionLockBase() {
////////////////////////////////////////////////////////////////////
	wxASSERT_MSG( referenceCounter != 0, wxString::Format("referenceCounter: %u", referenceCounter) );
	
	if ( referenceCounter > 0 )
		referenceCounter--;
	
	if ( parent->isInterrupted() == false ) 
		parent->startSerialTimer();
}
////////////////////////////////////////////////////////////////////
bool CncTransactionLockBase::waitUntilCncIsAvailable() {
////////////////////////////////////////////////////////////////////
	bool ret = false;
	
	if ( parent->cnc ) {
		
		unsigned counter = 0;
		while ( parent->cnc->isCommandActive() ) {
			
			parent->dispatchAll();
			parent->waitActive(10);
			
			if ( counter++ > 15 ) {
				
				//  this should not appear
				const unsigned char lcmd = SerialCommandLocker::getLockedCommand();
				std::cerr << CNC_LOG_FUNCT_A(": Command '%s' still active!", ArduinoCMDs::getCMDLabel(lcmd));
				
				break;
			}
		} 
		
		ret = ( parent->cnc->isCommandActive() == false );
		
		if ( parent->cnc->isConnected() )
		{
			if ( parent->cnc->serialDataAvailable() == true )
				parent->cnc->peekAndTraceReadBuffer(std::cout);
		}
	}
	
	return ret;
}

////////////////////////////////////////////////////////////////////
CncTransactionLock::CncTransactionLock(MainFrame* p) 
: CncTransactionLockBase(p)
////////////////////////////////////////////////////////////////////
{
	state = waitUntilCncIsAvailable();
	
	if ( isOk() ) {
		
		if ( parent->cnc != NULL ) {
			
			if ( parent->cnc->isSpyOutputOn() )
				cnc::spy.addMarker("Transaction initiated . . . ");
		
			if ( parent->cnc->isConnected() )
				parent->cnc->processCommand(CMD_PUSH_TRANSACTION, std::cerr);
				
		}
		
		THE_CONTEXT->registerCncTransaction();
		
		parent->cncTransactionLockCallback(this);
	}
	else 
	{
		std::cerr << CNC_LOG_FUNCT_A(": State isn't OK\n");
	}
}
////////////////////////////////////////////////////////////////////
CncTransactionLock::~CncTransactionLock() {
////////////////////////////////////////////////////////////////////
	if ( isOk() ) 
	{
		if ( waitUntilCncIsAvailable() )
			parent->cncTransactionReleaseCallback(this);
			
		if ( parent->cnc != NULL ) {
			
			if ( parent->cnc->isConnected() )
				parent->cnc->processCommand(CMD_POP_TRANSACTION, std::cerr);
			
			if ( parent->cnc->isSpyOutputOn() )
				cnc::spy.addMarker("Transaction released . . . ");
		}
		
		THE_CONTEXT->unregisterCncTransaction();
	}
	else 
	{
		std::cerr << CNC_LOG_FUNCT_A(": State isn't OK\n");
	}
}

////////////////////////////////////////////////////////////////////
CncGamepadTransactionLock::CncGamepadTransactionLock(MainFrame* p) 
: CncTransactionLockBase(p)
////////////////////////////////////////////////////////////////////
{
	if ( isOk() )
		parent->cncTransactionLockCallback(this);
	
	//std::clog << CNC_LOG_FUNCT_A("\n");
}
////////////////////////////////////////////////////////////////////
CncGamepadTransactionLock::~CncGamepadTransactionLock() {
////////////////////////////////////////////////////////////////////
	if ( isOk() )
		parent->cncTransactionReleaseCallback(this);
	
	//std::clog << CNC_LOG_FUNCT_A("\n");
}

////////////////////////////////////////////////////////////////////
CncGampadDeactivator::CncGampadDeactivator(MainFrame* p, bool rps)
: parent				(p)
, reconstructPrevState	(rps)
, prevState				(false)
////////////////////////////////////////////////////////////////////
{
	wxASSERT(parent);
	referenceCounter++;
	
	prevState = parent->isGamepadNotificationActive();
	parent->activateGamepadNotifications(false);
}
////////////////////////////////////////////////////////////////////
CncGampadDeactivator::~CncGampadDeactivator() {
////////////////////////////////////////////////////////////////////
	parent->activateGamepadNotifications(reconstructPrevState == true ? prevState : true);
	referenceCounter--;
}

////////////////////////////////////////////////////////////////////
class CncRunEventFilter : public wxEventFilter {
	public:
		CncRunEventFilter() {
			wxEvtHandler::AddFilter(this);
		}
		virtual ~CncRunEventFilter() {
			wxEvtHandler::RemoveFilter(this);
		}
		
		// currently no filter active
		virtual int FilterEvent(wxEvent& event) {
			return Event_Skip;
			
			// examples how to use 
			/*
			if ( event.GetEventCategory() == wxEVT_CATEGORY_TIMER )
				return Event_Skip;
				
			const wxWindow* wnd = (wxWindow*)event.GetEventObject();
			if ( wnd == THE_APP->GetBtnEmergenyStop() ) {
				THE_APP->GetBtnEmergenyStop()->SetLabel(wxDateTime::UNow().FormatISOTime());
				return Event_Skip;
			}
			
			//const wxEventType t = event.GetEventType();
			//const wxWindow* wnd = (wxWindow*)event.GetEventObject();
			return Event_Ignore;
			*/
		}
};

////////////////////////////////////////////////////////////////////
class CncInteractiveMoveEventFilter : public wxEventFilter {
	public:
		CncInteractiveMoveEventFilter() {
			wxEvtHandler::AddFilter(this);
		}
		virtual ~CncInteractiveMoveEventFilter() {
			wxEvtHandler::RemoveFilter(this);
		}
		
		// currently no filter active
		virtual int FilterEvent(wxEvent& event) {
			
			const wxEventType et = event.GetEventType();
			
			if ( et == wxEVT_CNC_NAVIGATOR_PANEL ) {
				switch ( event.GetId() ) {
					case CncNavigatorPanelEvent::Id::CNP_ACTIVATE_REGION:
					case CncNavigatorPanelEvent::Id::CNP_CHANGE_SPEED_INDEX:
					{
						return Event_Ignore;
					}
				}
			}
			else if ( et == wxEVT_GAMEPAD_THREAD ) {
				switch ( event.GetId() ) {
					case MainFrame::EventId::GAMEPAD_STATE:
					case MainFrame::EventId::GAMEPAD_MESSAGE:
					{
						//return Event_Ignore;
					}
				}
			}
			
			return Event_Skip;
		}
};

////////////////////////////////////////////////////////////////////
GlobalConfigManager::GlobalConfigManager(MainFrame* mf, wxPropertyGridManager* pgMgrSetup, wxFileConfig* globalConfig) {
////////////////////////////////////////////////////////////////////
	APPEND_LOCATION_TO_STACK_TRACE_FILE
	
	wxASSERT(globalConfig);
	wxASSERT(pgMgrSetup);
	wxASSERT(mf);
	
	// setup configuration
	CncConfig::setupGlobalConfigurationGrid(pgMgrSetup, *globalConfig);
	CncConfig::globalCncConfig = new CncConfig(mf);
	
	// load the file configuration
	CncConfig::globalCncConfig->loadConfiguration(*globalConfig);
	
	// at least initialize
	CncConfig::globalCncConfig->init();
}
////////////////////////////////////////////////////////////////////
GlobalConfigManager::~GlobalConfigManager() {
////////////////////////////////////////////////////////////////////
	cncDELETE( CncConfig::globalCncConfig );
	APPEND_LOCATION_TO_STACK_TRACE_FILE
}

////////////////////////////////////////////////////////////////////
wxFrame* THE_FRAME = NULL;

MainFrameBase::MainFrameBase(wxWindow* parent)
: MainFrameBClass(parent)
, logger				( new CncStandardLoggerProxy	(this))
, startupTrace			( new CncStartupLoggerProxy		(this))
, loggerView			( new CncLoggerView				(this))
, tmpTraceInfo			( new CncTraceProxy				(this))
, controllerMsgHistory	( new CncMsgHistoryLoggerProxy	(this))
, dryRunLoggerProxy		( new CncDryRunLoggerProxy		(this))
, parserSynopsisProxy	( new CncParserSynopsisProxy	(this))
////////////////////////////////////////////////////////////////////
{
	// If this timers are already started stop it here!
	// This is with respect that they should be started only if all corresponding controls
	// are well created, which takes a portion of time which will increase with the
	// amount of controls.
	// Restarting it at a later event (may be Show()) of class MainFrame will ensure that.

	if ( m_startupTimer->IsRunning() )	m_startupTimer->Stop();
	if ( m_serialTimer ->IsRunning() )	m_serialTimer ->Stop();
	if ( m_traceTimer  ->IsRunning() )	m_traceTimer  ->Stop();

	THE_FRAME = this;
	
	logger				->Show(false);
	startupTrace		->Show(false);
	tmpTraceInfo		->Show(false);
	controllerMsgHistory->Show(false);
	dryRunLoggerProxy	->Show(false);
	parserSynopsisProxy	->Show(false);
	
	GblFunc::replaceControl(m_loggerViewPlaceholder, loggerView);
	
	tmpTraceInfo->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MainFrameBase::traceTextUpdated), NULL, this);
}
///////////////////////////////////////////////////////////////////
MainFrameBase::~MainFrameBase() {
///////////////////////////////////////////////////////////////////
	APPEND_LOCATION_TO_STACK_TRACE_FILE_A("Entry . . .")
	
	// housekeeping
	CncFileNameService::sessionHousekeeping();
	APPEND_LOCATION_TO_STACK_TRACE_FILE_A("after Session Housekeeping . . .")

	GlobalStreamRedirectionReset();
	APPEND_LOCATION_TO_STACK_TRACE_FILE_A("after Stream Redirection . . .")
	
	// beautifying only
	logger->GetParent()->SetBackgroundColour(*wxBLACK);
	
	//Unbinding
	tmpTraceInfo->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MainFrameBase::traceTextUpdated), NULL, this);
	
	// clear
	cncDELETE( parserSynopsisProxy );
	cncDELETE( dryRunLoggerProxy );
	cncDELETE( controllerMsgHistory );
	cncDELETE( tmpTraceInfo );
	cncDELETE( startupTrace );
	cncDELETE( loggerView );
	cncDELETE( logger );
	
	THE_FRAME = NULL;
	
	APPEND_LOCATION_TO_STACK_TRACE_FILE_A("Finalized . . .")
}

///////////////////////////////////////////////////////////////////
MainFrame::MainFrame(wxWindow* parent, wxFileConfig* globalConfig)
: MainFrameBase							(parent)
, GlobalConfigManager					(this, GetPgMgrSetup(), globalConfig)
, gamepadThread							(NULL)
, serialThread							(NULL)
, cncSpeedPlayground					(new CncSpeedPlayground(this))
, isDebugMode							(false)
, canClose								(true)
, evaluatePositions						(true)
, ignoreDirControlEvents				(false)
, idleBitmapState						(false)
, toolState								()
, runConfirmationInfo					(RunConfirmationInfo::Wait)
, interactiveTransactionLock			(NULL)
, startTimerTimeout						(250)
, serialTimerTimeout					(1000)
, traceTimerTimeout						(125)
, traceTimerCounter						(0)
, lastPortName							(wxT(""))
, cnc									(new CncControl(CncEMU_NULL))
, lruFileView							(NULL)
, sourceEditor							(NULL)
, outboundEditor						(NULL)
, motionMonitor							(NULL)
, fileView								(NULL)
, mainFilePreview						(NULL)
, outboundFilePreview					(NULL)
, monitorFilePreview					(NULL)
, toolMagazine							(NULL)
, positionSpy							(NULL)
, setterList							(NULL)
, speedMonitor							(NULL)
, defaultSpeedSlider					(new CncDefaultSpeedSlider(m_defaultSpeedSlider, new CncValueCtrl(m_defaultSpeedSliderValue)))
, motionVertexCtrl						(NULL)
, cncPreprocessor						(NULL)
, gCodeSequenceList						(NULL)
, cncSummaryListCtrl					(NULL)
, serialSpyPanel						(NULL)
, outboundEditorSvgView					(NULL)
, navigatorPanel						(NULL)
, optionPane3D							(NULL)
, drawPane3D							(NULL)
, statisticsPane						(NULL)
, cnc3DVSplitterWindow					(NULL)
, cnc3DHSplitterWindow					(NULL)
, templateObserver						(NULL)
, openGLContextObserver					(new CncOpenGLContextObserver(this))
, cncOsEnvDialog						(new CncOSEnvironmentDialog(this))
, cncExtViewBoxCluster					(NULL)
, cncExtMainPreview						(NULL)
, cncArduinoEnvironment					(new CncArduinoEnvironment(this))
, cncLCDPositionPanel					(NULL)
, cncManuallyMoveCoordPanel				(NULL)
, gamepadSpy							(NULL)
, gamepadStatusCtl						(NULL)
, controllersMsgHistoryList				(NULL)
, mainViewInfobar						(new CncMainInfoBar(this))
, monitorViewInfobar					(new CncMainInfoBar(this))
, positionStorage						(new CncPositionStorageView(this))
, usbConnectionObserver					(new CncUsbConnectionObserver(this))
, anchorPositionDlg						(new CncAnchorPosition(this))
, secureCtrlPanel						(NULL)
, contextSummaryPanel					(NULL)
, perspectiveHandler					(globalConfig, m_menuPerspective)
, config								(globalConfig)
, lruStore								(new wxFileConfig(wxT("CncControllerLruStore"), wxEmptyString, CncFileNameService::getLruFileName(), CncFileNameService::getLruFileName(), wxCONFIG_USE_RELATIVE_PATH | wxCONFIG_USE_NO_ESCAPE_CHARACTERS))
, outboundNbInfo						(new NotebookInfo(m_outboundNotebook))
, templateNbInfo						(new NotebookInfo(m_templateNotebook))
, lastTemplateFileName					()
, lastMonitorPreviewFileName			(_(""))
, pngAnimation							(NULL)
, stcFileContentPopupMenu				(NULL)
, stcEmuContentPopupMenu				(NULL)
, inboundFileParser						(NULL)
, perspectiveTimer						(new wxTimer(this, wxEVT_PERSPECTIVE_TIMER))
, debugUserNotificationTimer			(new wxTimer(this, wxEVT_DEBUG_USER_NOTIFICATION_TIMER))
, guiControls							()
, menuItems								()
, refPositionDlg						(new CncReferencePosition(this))
, podiumManagementDlg					(new CncPodiumManagement(this))
{
///////////////////////////////////////////////////////////////////
	APPEND_THREAD_ID_TO_STACK_TRACE_FILE;
	
	THE_CONFIG->updateLoadTrace(m_cfgLoadTrace, m_cfgObsoleteTrace);
	
	m_auimgrMain->SetArtProvider(new clAuiDockArt());
	
	getLoggerView()->initialize();
	
	// initialize gamepad thread
	initializeGamepadThread();
	
	// initialize serial thread
	initializeSerialThread();

	// setup aui clear
	hideAllAuiPanes();
	
	// decocate application
	setIcons();

	// do this definitely here later it will causes a crash 
	installCustControls();

	// debugger configuration
	FileParser::installDebugConfigPage(m_debuggerPropertyManagerGrid);
	
	regiterAllMenuItems();
	
	// bind 
	this->Bind(wxEVT_CHAR_HOOK, 					&MainFrame::onGlobalKeyDownHook, 			this);
	this->Bind(wxEVT_IDLE,							&MainFrame::onIdle,							this);
	this->Bind(wxEVT_GAMEPAD_THREAD, 				&MainFrame::onGamepadThreadInitialized,		this, MainFrame::EventId::INITIALIZED);
	this->Bind(wxEVT_GAMEPAD_THREAD, 				&MainFrame::onGamepadThreadCompletion, 		this, MainFrame::EventId::COMPLETED);
	this->Bind(wxEVT_GAMEPAD_THREAD, 				&MainFrame::onGamepadThreadUpadte, 			this, MainFrame::EventId::GAMEPAD_STATE);
	this->Bind(wxEVT_GAMEPAD_THREAD, 				&MainFrame::onGamepadThreadMessage, 		this, MainFrame::EventId::GAMEPAD_MESSAGE);
	this->Bind(wxEVT_SERIAL_THREAD, 				&MainFrame::onSerialThreadInitialized, 		this, MainFrame::EventId::INITIALIZED);
	this->Bind(wxEVT_SERIAL_THREAD, 				&MainFrame::onSerialThreadCompletion, 		this, MainFrame::EventId::COMPLETED);
	this->Bind(wxEVT_SERIAL_THREAD, 				&MainFrame::onSerialThreadHeartbeat, 		this, MainFrame::EventId::SERIAL_HEARTBEAT);
	this->Bind(wxEVT_SERIAL_THREAD, 				&MainFrame::onSerialThreadMessage, 			this, MainFrame::EventId::SERIAL_MESSAGE);
	this->Bind(wxEVT_SERIAL_THREAD, 				&MainFrame::onSerialThreadDataNotification,	this, MainFrame::EventId::SERIAL_DATA_NOTIFICATION);
	this->Bind(wxEVT_SERIAL_THREAD, 				&MainFrame::onSerialThreadPinNotification, 	this, MainFrame::EventId::SERIAL_PIN_NOTIFICATION);
	this->Bind(wxEVT_CNC_NAVIGATOR_PANEL, 			&MainFrame::onNavigatorPanel, 				this);
	
	{
		const wxFont font = THE_CONTEXT->outboundListBookFont;
		m_notebookConfig->GetListView()->SetFont(font);
		m_listbookMonitor->GetListView()->SetFont(font);
		m_listbookPostProcessor->GetListView()->SetFont(font);
		m_listbookManallyMotionControl->GetListView()->SetFont(font);
		m_listbookSource->GetListView()->SetFont(font);
		m_listbookSetupConfig->GetListView()->SetFont(font);
		m_listbookReferences->GetListView()->SetFont(font);
		m_testCaseBook->GetListView()->SetFont(font);
	}
	
	defaultSpeedSlider->showValue(true);
	defaultSpeedSlider->setToolTipWindow(m_defaultSpeedSliderValue);
	
	toolState.setControl(GetToolState());
}
///////////////////////////////////////////////////////////////////
MainFrame::~MainFrame() {
///////////////////////////////////////////////////////////////////
	GblFunc::appendToStackTraceFile("\nShuting down ......................");
	
	// stop the serial timer and wait interval to finish the work behind
	auto stopTimer = [&](wxTimer* timer) {
		if ( timer->IsRunning() )
			timer->Stop();
	};
	
	stopTimer( perspectiveTimer );
	stopTimer( debugUserNotificationTimer );
	stopTimer( m_startupTimer );
	stopTimer( m_serialTimer );
	stopTimer( m_traceTimer );
	
	if ( cnc != NULL )
		waitActive(m_serialTimer->GetInterval());
	
	perspectiveHandler.destroyUserPerspectives();
	
	// unbind 
	this->Unbind(wxEVT_CHAR_HOOK, 					&MainFrame::onGlobalKeyDownHook, 			this);
	this->Unbind(wxEVT_IDLE,						&MainFrame::onIdle,							this);
	this->Unbind(wxEVT_GAMEPAD_THREAD, 				&MainFrame::onGamepadThreadInitialized, 	this, MainFrame::EventId::INITIALIZED);
	this->Unbind(wxEVT_GAMEPAD_THREAD,	 			&MainFrame::onGamepadThreadCompletion,	 	this, MainFrame::EventId::COMPLETED);
	this->Unbind(wxEVT_GAMEPAD_THREAD, 				&MainFrame::onGamepadThreadUpadte, 			this, MainFrame::EventId::GAMEPAD_STATE);
	this->Unbind(wxEVT_GAMEPAD_THREAD, 				&MainFrame::onGamepadThreadMessage, 		this, MainFrame::EventId::GAMEPAD_MESSAGE);
	this->Unbind(wxEVT_SERIAL_THREAD, 				&MainFrame::onSerialThreadInitialized, 		this, MainFrame::EventId::INITIALIZED);
	this->Unbind(wxEVT_SERIAL_THREAD, 				&MainFrame::onSerialThreadCompletion, 		this, MainFrame::EventId::COMPLETED);
	this->Unbind(wxEVT_SERIAL_THREAD, 				&MainFrame::onSerialThreadHeartbeat, 		this, MainFrame::EventId::SERIAL_HEARTBEAT);
	this->Unbind(wxEVT_SERIAL_THREAD, 				&MainFrame::onSerialThreadMessage, 			this, MainFrame::EventId::SERIAL_MESSAGE);
	this->Unbind(wxEVT_SERIAL_THREAD, 				&MainFrame::onSerialThreadDataNotification,	this, MainFrame::EventId::SERIAL_DATA_NOTIFICATION);
	this->Unbind(wxEVT_SERIAL_THREAD, 				&MainFrame::onSerialThreadPinNotification, 	this, MainFrame::EventId::SERIAL_PIN_NOTIFICATION);

	this->Unbind(wxEVT_CNC_NAVIGATOR_PANEL, 		&MainFrame::onNavigatorPanel, 				this);
	
	// explicit delete the motion monitor pointer here, beacause the motion monitor class
	// considers the Mainframe THE_CONFIG->getTheApp() pointer in its dtor 
	// and this crashes definitly if the MainFame dtor is already passed
	// the delete below avoid this behaviour
	cncDELETE ( motionMonitor );
	cncDELETE ( gamepadStatusCtl );

	wxASSERT(lruStore);
	wxASSERT(lruFileView);
	lruFileView->save(lruStore);
	lruStore->Flush();
	cncDELETE ( lruStore );
	
	cncDELETE( podiumManagementDlg );
	cncDELETE( refPositionDlg );
	cncDELETE( openGLContextObserver );
	cncDELETE( cncExtViewBoxCluster );
	cncDELETE( cncExtMainPreview );
	cncDELETE( cncArduinoEnvironment );
	cncDELETE( cncOsEnvDialog );
	cncDELETE( outboundNbInfo );
	cncDELETE( templateNbInfo );
	cncDELETE( positionStorage );
	cncDELETE( cncSpeedPlayground );
	cncDELETE( cnc );
	cncDELETE( statisticsPane );
	cncDELETE( drawPane3D );
	cncDELETE( optionPane3D );
	cncDELETE( motionMonitor );
	cncDELETE( templateObserver );
	cncDELETE( lruFileView );
	cncDELETE( sourceEditor );
	cncDELETE( outboundEditor );
	cncDELETE( fileView );
	cncDELETE( mainFilePreview );
	cncDELETE( outboundFilePreview );
	cncDELETE( monitorFilePreview );
	cncDELETE( toolMagazine );
	cncDELETE( positionSpy );
	cncDELETE( setterList );
	cncDELETE( motionVertexCtrl );
	cncDELETE( speedMonitor );
	cncDELETE( defaultSpeedSlider );
	cncDELETE( cncPreprocessor );
	cncDELETE( cncSummaryListCtrl );
	cncDELETE( serialSpyPanel );
	cncDELETE( outboundEditorSvgView );
	cncDELETE( gCodeSequenceList );
	cncDELETE( navigatorPanel );
	cncDELETE( cnc3DVSplitterWindow );
	cncDELETE( cnc3DHSplitterWindow );
	cncDELETE( interactiveTransactionLock );
	
	wxASSERT(config);
	config->Flush();
	cncDELETE( config );
	
	DeletePendingEvents();
	
	// log exit dtor
	APPEND_LOCATION_TO_STACK_TRACE_FILE_A("Finalized . . .")
}
///////////////////////////////////////////////////////////////////
void MainFrame::onGlobalKeyDownHook(wxKeyEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( refPositionDlg && refPositionDlg->IsShownOnScreen() )
	{
		wxPostEvent(refPositionDlg, event);
		event.Skip(false);
		return;
	}
	
	if ( podiumManagementDlg && podiumManagementDlg->IsShownOnScreen() )
	{
		wxPostEvent(podiumManagementDlg, event);
		event.Skip(false);
		return;
	}
	
	if ( motionMonitor && motionMonitor->IsShownOnScreen() )
	{
		// This is necessary to avoid the default notebook key handling
		if ( motionMonitor->HasFocus() )
		{
			motionMonitor->onKeyDown(event);
			event.Skip(false);
			return;
		}
	}
	
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::ShowAuiToolMenu(wxAuiToolBarEvent& event) {
///////////////////////////////////////////////////////////////////
	// overides the wxcrafter generted method
	event.Skip();
	/*
	if (event.IsDropDownClicked()) {
		wxAuiToolBar* toolbar = wxDynamicCast(event.GetEventObject(), wxAuiToolBar);
	
		if (toolbar) {
			wxAuiToolBarItem* item = toolbar->FindTool(event.GetId());
			if (item) {
				std::map<int, wxMenu*>::iterator iter = m_dropdownMenus.find(item->GetId());
				if (iter != m_dropdownMenus.end()) {
					event.Skip(false);
					wxPoint pt = event.GetItemRect().GetBottomLeft();
					pt.y++;
					
					// dont use the toolbar event handler because this will generate a crash while cnc::waitActive is in action!
					// toolbar->PopupMenu(iter->second, pt);
					this->PopupMenu(iter->second, pt);
					
				}
			}
		}
	}
	*/ 
}
////////////////////////////////////////////////////////////////////////////
bool MainFrame::startSerialTimer(bool notify) {
////////////////////////////////////////////////////////////////////////////
	const bool ret = m_serialTimer->Start();
	
	if ( notify )
		decorateIdleState();
	
	return ret;
}
////////////////////////////////////////////////////////////////////////////
void MainFrame::stopSerialTimer(bool notify) {
////////////////////////////////////////////////////////////////////////////
	m_serialTimer->Stop();
	
	if ( notify )
		decorateIdleState();
}
////////////////////////////////////////////////////////////////////////////
void MainFrame::onConfigurationUpdated(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////////////
	GetLastConfigNotification()->ChangeValue(wxDateTime::UNow().Format("%H:%M:%S"));

	// log the change is related to
	//CNC_PRINT_FUNCT_A(" %s", event.GetString())
	
	if ( drawPane3D && drawPane3D->GetZView() )
		drawPane3D->GetInfoToolDiameter()->SetLabel(wxString::Format("%.3lf", THE_CONTEXT->getCurrentToolDiameter()));
	
	if ( drawPane3D && drawPane3D->GetInfoToolDiameter() )
		drawPane3D->GetZView()->updateView(cnc->requestControllerPos().getZ() * THE_CONFIG->getDisplayFactZ(THE_CONFIG->getDisplayUnit()));
}
///////////////////////////////////////////////////////////////////
void MainFrame::disableGuiControls() {
///////////////////////////////////////////////////////////////////
	enableGuiControls(false);
}
///////////////////////////////////////////////////////////////////
void MainFrame::registerMenuItem(wxMenuItem* item) {
///////////////////////////////////////////////////////////////////
	if ( item == NULL )
		return;
	
	if ( item->IsSeparator() )
		return;
		
	if ( item->IsSubMenu() ) {
		wxMenu* menu = item->GetSubMenu();
		for ( unsigned int i = 0; i < menu->GetMenuItemCount (); i++ )
			registerMenuItem(menu->FindItemByPosition(i));
	}
	
	CncApp::MenuInfo mi;
	mi.lastEnableState 	= true;
	menuItems[item] = mi;
}
///////////////////////////////////////////////////////////////////
void MainFrame::unregisterMenuItem(wxMenuItem* item) {
///////////////////////////////////////////////////////////////////
	if ( item == NULL )
		return;
		
	auto it = menuItems.find(item);
	if ( it != menuItems.end() )
		menuItems.erase (it);       
}
///////////////////////////////////////////////////////////////////
void MainFrame::regiterAllMenuItems() {
///////////////////////////////////////////////////////////////////
	std::function<void(wxMenu* menu)> registerItem = [&](wxMenu* menu) {
		if ( menu == NULL )
			return;
			
		for ( unsigned int i = 0; i < menu->GetMenuItemCount (); i++ ) {
			wxMenuItem* item = menu->FindItemByPosition(i);
			if ( item->IsSubMenu() )
				registerItem(item->GetSubMenu());
			
			if ( item->IsSeparator() )
				continue;
			
			CncApp::MenuInfo mi;
			mi.lastEnableState 	= true;
			
			menuItems[item] = mi;
		}
	};
	
	menuItems.clear();
	
	for ( unsigned int m = 0; m < m_menuBar->GetMenuCount(); m++) {
		wxMenu* menu = m_menuBar->GetMenu(m);
		registerItem(menu);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::enableMenuItems(bool state) {
///////////////////////////////////////////////////////////////////
	for ( auto it = menuItems.begin(); it != menuItems.end(); ++it ) {
		CncApp::MenuInfo& mi = it->second;
		
		if ( it->first == NULL )
			continue;
			
		if ( state == true ) {
			it->first->Enable(mi.lastEnableState);
			
		} else {
			mi.lastEnableState = it->first->IsEnabled();
			it->first->Enable(false);
			
			menuItems[it->first] = mi;
		}
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::registerGuiControl(wxWindow* ctrl) {
///////////////////////////////////////////////////////////////////
	if ( ctrl == NULL )
		return;
		
	CncApp::WindowInfo wi;
	wi.ctrl 			= ctrl;
	wi.lastEnableState 	= ctrl->IsThisEnabled();
		
	guiControls.push_back(wi);
}
///////////////////////////////////////////////////////////////////
void MainFrame::enableGuiControls(bool state) {
///////////////////////////////////////////////////////////////////
	for ( auto it = guiControls.begin(); it != guiControls.end(); ++it ) {
		CncApp::WindowInfo& wi = *it;
		
		if ( wi.ctrl == NULL )
			continue;
			
		if ( state == true ) {
			wi.ctrl->Enable(wi.lastEnableState);
			
		} else {
			wi.lastEnableState = wi.ctrl->IsThisEnabled();
			wi.ctrl->Enable(false);
			
			*it = wi;
		}
	}
	
	getLoggerView()->enable(state);
}
///////////////////////////////////////////////////////////////////
void MainFrame::installCustControls() {
///////////////////////////////////////////////////////////////////

	// 3D splitter window
	wxWindow* parent 		= m_3DSplitterPlaceholder->GetParent();
	cnc3DHSplitterWindow 	= new CncMonitorHSplitterWindow(parent);
	cnc3DVSplitterWindow 	= new CncMonitorVSplitterWindow(cnc3DHSplitterWindow);
	
	statisticsPane 			= new CncStatisticsPane(cnc3DHSplitterWindow);
	cnc3DHSplitterWindow->SplitHorizontally(cnc3DVSplitterWindow, statisticsPane, 0);
	
	drawPane3D    			= new GL3DDrawPane(cnc3DVSplitterWindow);
	optionPane3D  			= new GL3DOptionPane(cnc3DVSplitterWindow);
	cnc3DVSplitterWindow->SplitVertically(drawPane3D, optionPane3D, 0);
	
	GblFunc::replaceControl(m_3DSplitterPlaceholder, cnc3DHSplitterWindow);
	
	// info bars - aready created
	GblFunc::replaceControl(m_mainViewInfobar,		mainViewInfobar);
	GblFunc::replaceControl(m_monitorViewInfobar,	monitorViewInfobar);
	
	// Motion Monitor
	motionMonitor = new CncMotionMonitor(this);
	GblFunc::replaceControl(drawPane3D->GetDrawPane3DPlaceHolder(), motionMonitor);
	drawPane3D->setMotionMonitor(motionMonitor);
	optionPane3D->setMotionMonitor(motionMonitor);
	statisticsPane->setMotionMonitor(motionMonitor);
	
	activate3DPerspectiveButton(m_3D_Perspective1Sec);
	motionMonitorViewPerspective1();
	
	// Controllers message history
	controllersMsgHistoryList = new CncLoggerListCtrl(this, wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_controllerMsgHistoryPlaceholder, controllersMsgHistoryList);
	
	// Template observer
	templateObserver = new CncTemplateObserver(this);
	GblFunc::replaceControl(m_panelTemplateObserverPlaceholder, templateObserver);
	
	// LRU List 
	lruFileView = new CncLruFileViewListCtrl(this, 24, wxLC_SINGLE_SEL | wxLC_HRULES);
	GblFunc::replaceControl(m_lruListPlaceholder, lruFileView);
	
	// Source Editor
	sourceEditor = new CncSourceEditor(this);
	GblFunc::replaceControl(m_stcFileContent, sourceEditor);
	
	// Outbound Editor
	outboundEditor = new CncOutboundEditor(this);
	GblFunc::replaceControl(m_outboundFileSource, outboundEditor);
	
	// File View
	fileView = new CncFileView(this, false);
	GblFunc::replaceControl(m_mainFileViewPlaceholder, fileView);
	
	// Transfer file list
	transferFileView = new CncTransferFileView(this, true);
	GblFunc::replaceControl(m_mainFileTransferPlaceholder, transferFileView);
	
	// Inbound File Preview
	mainFilePreview = new CncFilePreview(this, "GLMainPreview");
	GblFunc::replaceControl(m_filePreviewPlaceholder, mainFilePreview);
	mainFilePreview->setTitle("File Preview:");
	
	// Outbound File Preview
	outboundFilePreview = new CncFilePreview(this, "GLOutboundPreview");
	GblFunc::replaceControl(m_outboundPreviewPlaceholder, outboundFilePreview);
	outboundFilePreview->setTitle("Serial Outbound Preview:");

	// File Preview
	monitorFilePreview = new CncFilePreview(this, "GLMontiorPreview");
	GblFunc::replaceControl(m_monitorTemplatePreviewPlaceHolder, monitorFilePreview);
	monitorFilePreview->setTitle("Template Preview:");
	
	// External previews
	cncExtMainPreview = new CncExternalViewBox(this);
	cncExtMainPreview->setupView(CncExternalViewBox::Preview::FILE,			mainFilePreview,		"External File Preview . . . ");
	cncExtMainPreview->setupView(CncExternalViewBox::Preview::TEMPLATE,		monitorFilePreview,		"External Template Preview . . . ");
	
	// External views
	cncExtViewBoxCluster = new CncExternalViewBoxCluster(this);
	cncExtViewBoxCluster->setupView1(CncExternalViewBoxCluster::Node::EVB_Config,		m_panelConfiguration,		"External Configuration . . . ");
	cncExtViewBoxCluster->setupView1(CncExternalViewBoxCluster::Node::EVB_Source,		m_panelTemplateSource,		"External Source View . . . ");
	cncExtViewBoxCluster->setupView1(CncExternalViewBoxCluster::Node::EVB_Reference,	m_panelReferences,			"External Reference View . . . ");
	cncExtViewBoxCluster->setupView1(CncExternalViewBoxCluster::Node::EVB_Manually,		m_panelTemplateManually,	"External Manually View . . . ");
	cncExtViewBoxCluster->setupView1(CncExternalViewBoxCluster::Node::EVB_Test,			m_panelMainTest,			"External Test View . . . ");
	cncExtViewBoxCluster->setupView1(CncExternalViewBoxCluster::Node::EVB_Monitor,		m_panelMotionMonitor,		"External Motion Monitor . . . ");
	cncExtViewBoxCluster->setupView1(CncExternalViewBoxCluster::Node::EVB_SetterList,	m_panelSetterList,			"External Setter List View . . . ");
	cncExtViewBoxCluster->setupView1(CncExternalViewBoxCluster::Node::EVB_CtrlMsg,		m_panelControllerMessages,	"External Controller Messages View . . . ");
	
	// tool magazine
	toolMagazine = new CncToolMagazine(this); 
	GblFunc::replaceControl(m_toolMagazinePlaceholder, toolMagazine);
	
	// pos spy control
	positionSpy = new CncPosSpyListCtrl(this, wxLC_HRULES | wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_positionSpy, positionSpy);

	// setter control
	setterList = new CncSetterListCtrl(this, wxLC_HRULES | wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_setterListPlaceholder, setterList);
	setterList->setSelNumCtrl  (m_selSetterNum);
	setterList->setSelPidCtrl  (m_selSetterPid);
	setterList->setSelKeyCtrl  (m_selSetterKey);
	setterList->setSelValCtrl  (m_selSetterValue);
	setterList->setSelUnitCtrl (m_selSetterUnit);
	
	// motion vertex list control
	motionVertexCtrl = new CncMotionVertexTrace(this); 
	GblFunc::replaceControl(m_motionVertexPlaceholder, motionVertexCtrl);
	
	// speed monitor control
	speedMonitor = new CncSpeedMonitor(this); 
	GblFunc::replaceControl(m_speedMonitorPlaceholder, speedMonitor);
	
	// preprocessor control
	cncPreprocessor = new CncPreprocessor(this); 
	GblFunc::replaceControl(m_preprocessorPlaceholder, cncPreprocessor);
	
	// summary list
	cncSummaryListCtrl = new CncSummaryListCtrl(this, wxLC_HRULES | wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_cncSummaryListCtrl, cncSummaryListCtrl);
	
	// serial spy list control
	serialSpyPanel = new CncSerialSpyPanel(this); 
	GblFunc::replaceControl(m_serialSpyPlaceholder, serialSpyPanel);
	
	// Outbound editor svg viewer
	outboundEditorSvgView = new CncSvgViewer(this);
	outboundEditorSvgView->SetBackgroundColour(*wxBLACK);
	outboundEditorSvgView->setContentSizable(false);
	GblFunc::replaceControl(m_outboundEditorSvgViewPlaceholder, outboundEditorSvgView);
	
	// GCode Sequence control
	gCodeSequenceList = new CncGCodeSequenceListCtrl(this, wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_gCodeSequenceListPlaceholder, gCodeSequenceList);
	
	// Gamepad status control
	gamepadStatusCtl = new CncGamepadControllerState(this); 
	GblFunc::replaceControl(m_gamepadStatusPlaceholder, gamepadStatusCtl);
	
	// Gamepad Spy
	gamepadSpy = new CncGamepadSpy(this);
	GblFunc::replaceControl(m_gamepadSpyPlaceholder, gamepadSpy);
	gamepadSpy->setCondHistControl(gamepadStatusCtl->getHistList());
	gamepadSpy->setContext(CncGamepadSpy::GPC_MAIN);
	
	// navigator panel
	CncNavigatorPanel::Config cfg;
	cfg.innerCircle	 = true;
	cfg.shortFormat	 = false;
	cfg.margin			= 0;
	cfg.alignment		= wxALIGN_CENTER_HORIZONTAL;
	cfg.showRegionInfo	= false;
	cfg.initToolTipMapAsCoordSytem();
	navigatorPanel = new CncNavigatorPanel(this, cfg); 
	GblFunc::replaceControl(m_navigationPanelPlaceholder, navigatorPanel);
	
	cncLCDPositionPanel = new CncLCDPositionPanel(this);
	GblFunc::replaceControl(m_lcdPositionPlaceholder, cncLCDPositionPanel);
	
	cncManuallyMoveCoordPanel = new CncManuallyMoveCoordinates(this);
	GblFunc::replaceControl(m_manuallyMoveCoordPlaceholder, cncManuallyMoveCoordPanel);
	
	secureCtrlPanel = new CncSecureCtrlPanel(this);
	GblFunc::replaceControl(m_securityCtrlPanelPlaceholder, secureCtrlPanel);
	
	contextSummaryPanel = new CncTemplateContextSummaryPanel(this);
	GblFunc::replaceControl(m_templateContextPlaceholder, contextSummaryPanel);
}
///////////////////////////////////////////////////////////////////
void MainFrame::registerGuiControls() {
///////////////////////////////////////////////////////////////////
	registerGuiControl(secureCtrlPanel->GetBtConnectSec());
	registerGuiControl(secureCtrlPanel->GetBtResetSec());
	registerGuiControl(secureCtrlPanel->GetBtDryRunSec());
	registerGuiControl(secureCtrlPanel->GetBtTemplateContextSec());
	registerGuiControl(secureCtrlPanel->GetBtHardwareRefSec());
	
	registerGuiControl(m_btCloseSecurePanel);
	
	registerGuiControl(navigatorPanel);
	registerGuiControl(sourceEditor);
	registerGuiControl(outboundEditor);
	registerGuiControl(fileView);
	registerGuiControl(transferFileView);
	registerGuiControl(lruFileView);
	
	registerGuiControl(m_editorToolBox);
	registerGuiControl(m_btSvgToggleTryToSelectClientId);
	registerGuiControl(m_btSvgToggleAutoSaveTplOnProcess);
	registerGuiControl(m_defaultSpeedSlider);
	registerGuiControl(m_searchConnections);
	registerGuiControl(m_btToggleOutboundEditorWordWrap);
	registerGuiControl(m_rcSecureDlg);
	registerGuiControl(m_btProbeMode);
	registerGuiControl(m_btSelectReferences);
	registerGuiControl(m_btSelectManuallyMove);
	registerGuiControl(m_btSelectSetup);
	registerGuiControl(m_btSelectTemplate);
	registerGuiControl(m_btSelectInboundTest);
	registerGuiControl(m_btSelectInboundPreview);
	registerGuiControl(m_btSelectCncPreview);
	registerGuiControl(m_btSelectTemplatePreview);
	registerGuiControl(m_cbContentPosSpy);
	registerGuiControl(m_testSpindlePowerBtn);
	registerGuiControl(m_testSpindleSpeedSlider);
	registerGuiControl(m_portSelector);
	registerGuiControl(m_connect);
	registerGuiControl(m_btAdditionalParameters);
	registerGuiControl(m_btResetHardwareReference);
	registerGuiControl(m_btEvaluateHardwareReference);
	registerGuiControl(m_btTakeOverDimensions);
	registerGuiControl(m_btEvaluateDimensionXYPlane);
	registerGuiControl(m_btEvaluateDimensionZAxis);
	registerGuiControl(m_refPosition);
	registerGuiControl(m_anchorPosition);
	registerGuiControl(m_zToTop);
	registerGuiControl(m_zToTop);
	registerGuiControl(m_xToMin);
	registerGuiControl(m_yToMin);
	registerGuiControl(m_zToMin);
	registerGuiControl(m_xToMax);
	registerGuiControl(m_yToMax);
	registerGuiControl(m_zToMax);
	registerGuiControl(m_xToMid);
	registerGuiControl(m_yToMid);
	registerGuiControl(m_zToMid);
	registerGuiControl(m_homeDefintion);
	registerGuiControl(m_moveHome);
	registerGuiControl(m_moveXYtoZero);
	registerGuiControl(m_moveXYZtoZero);
	registerGuiControl(m_moveZtoZero);
	registerGuiControl(m_zeroMoveModeXY);
	registerGuiControl(m_zeroMoveModeXYZ);
	registerGuiControl(m_zeroMoveModeZ);
	registerGuiControl(m_displayInterval);
	registerGuiControl(m_btRequestCtlConfig);
	registerGuiControl(m_btRequestControllerPins);
	registerGuiControl(m_btSvgToggleWordWrap);
	registerGuiControl(m_switchMonitoing);
	registerGuiControl(m_testCountX);
	registerGuiControl(m_testCountY);
	registerGuiControl(m_testCountZ);
	registerGuiControl(m_testDistanceX);
	registerGuiControl(m_testDistanceY);
	registerGuiControl(m_testDistanceZ);
	registerGuiControl(m_testFirstX);
	registerGuiControl(m_testFirstY);
	registerGuiControl(m_testFirstZ);
	registerGuiControl(m_testModeX);
	registerGuiControl(m_testModeY);
	registerGuiControl(m_testModeZ);
	//...
}
///////////////////////////////////////////////////////////////////
void MainFrame::displayNotification(const Notification& notification) {
///////////////////////////////////////////////////////////////////
	if ( notification.title.IsEmpty() && notification.message.IsEmpty() )
		return;
	
	// msg: initial formating ...
	wxString msg(wxString::Format("%s: %s", notification.title, notification.message));
	msg.Replace("\n", " ");
	
	typedef MainFrame::Notification::Location Loc;
	switch ( notification.location ) {
		case Loc::NL_MainView:		wxBell();
									mainViewInfobar->showMessage(notification.type, msg);
									break;
									
		case Loc::NL_MonitorView:	wxBell();
									monitorViewInfobar->showMessage(notification.type, msg);
									break;
	}
	
	/*
	// obsolete, just for documentation . . 
	wxSharedPtr<wxNotificationMessageBase> dlg;
	dlg = new wxGenericNotificationMessage(notification.title, notification.message, this);
	switch ( notification.type ) {
		case 'E':	dlg->SetFlags(wxICON_ERROR); break;
		case 'W':	dlg->SetFlags(wxICON_WARNING); break;
		default:	dlg->SetFlags(wxICON_INFORMATION); break;
	}
	
	dlg->Show(notification.timeout);
	// important to free the shared Pointer!
	dlg.reset();
	*/
}
///////////////////////////////////////////////////////////////////
void MainFrame::displayReport(int id) {
///////////////////////////////////////////////////////////////////
	return;
	
	PidList pidList;
	
	switch ( id ) {
		case 1:			pidList.push_back(PID_I2C_LIMIT_VALUE);
						pidList.push_back(PID_I2C_SUPPORT_VALUE);
						//pidList.push_back(PID_SPEED_OFFSET_Y);
						//pidList.push_back(PID_SPEED_OFFSET_Z);
						break;
	}
	
	cnc->displayGetterList(pidList);
}
///////////////////////////////////////////////////////////////////
CncTouchBlockDetector::Result MainFrame::processTouchTest(const CncTouchBlockDetector::Parameters& para) {
///////////////////////////////////////////////////////////////////
	CncTouchBlockDetector::Result result;
	
	CNC_TRANSACTION_LOCK_RET_ON_ERROR(result)
	
	wxDateTime tsStart = wxDateTime::Now();
	while ( (wxDateTime::Now() - tsStart).Abs().GetSeconds() < 25 ) {
		
		cnc::trc.logDebugMessage(wxString::Format("Test contact result"));
		
		CncTouchBlockDetector tbd(cnc);
		if ( tbd.touch(para) == false) {
			result.errorInfo = tbd.getErrorCodes();
			break;
		}
		
		result.contactState = tbd.getContactState();
		if ( result.contactState == CncTouchBlockDetector::ContactState::CS_CLOSED ) {
			result.processResult	= true;
			result.errorInfo		= tbd.getErrorCodes();
			break;
		}
		
		if ( CncAsyncKeyboardState::isEscapePressed() ) {
			result.processResult	= false;
			result.errorInfo		= tbd.getErrorCodes();
			break;
		}
		
		waitActive(100);
		dispatchAll();
	}
	
	return result;
}
///////////////////////////////////////////////////////////////////
CncTouchBlockDetector::Result MainFrame::processZTouch(const CncTouchBlockDetector::Parameters& para) {
///////////////////////////////////////////////////////////////////
	CncTouchBlockDetector::Result result;
	selectMonitorBookCncPanel();
	motionMonitor->clear();
	
	CNC_TRANSACTION_LOCK_RET_ON_ERROR(result)
	
	CncTouchBlockDetector tbd(cnc);
	result.processResult	= tbd.touch(para);
	result.errorInfo		= tbd.getErrorCodes();
	
	if ( result.processResult == true )	
		result.newCurrentPos = tbd.getNewCurrentPos();
	
	motionMonitor->update();
	return result;
}
///////////////////////////////////////////////////////////////////
CncTouchBlockDetector::Result MainFrame::processXYZTouch(const CncTouchBlockDetector::Parameters& para) {
///////////////////////////////////////////////////////////////////
	CncTouchBlockDetector::Result result;
	selectMonitorBookCncPanel();
	motionMonitor->clear();
	
	CNC_TRANSACTION_LOCK_RET_ON_ERROR(result)
	
	CncTouchBlockDetector tbd(cnc);
	result.processResult	= tbd.touch(para);
	result.errorInfo		= tbd.getErrorCodes();
	
	if ( result.processResult == true )
		result.newCurrentPos = tbd.getNewCurrentPos();
	
	motionMonitor->update();
	return result;
}
#include "CncAutoProgressDialog.h"
///////////////////////////////////////////////////////////////////
void MainFrame::testFunction1(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	cnc::trc.logInfoMessage("Test function 1");
	
	Refresh();
}
///////////////////////////////////////////////////////////////////
void MainFrame::testFunction2(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	cnc::trc.logDebugMessage("Test function 2");
}
///////////////////////////////////////////////////////////////////
void MainFrame::testFunction3(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	cnc::trc.logDebugMessage("Test function 3");
}
///////////////////////////////////////////////////////////////////
void MainFrame::testFunction4(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	cnc::trc.logErrorMessage("Test function 4");
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onDeactivateSecureRunMode(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_CONTEXT->secureModeInfo.isActivatedByStartup = false;
	THE_CONTEXT->secureModeInfo.isDeactivatedByUser  = true;
	m_btDeactivateSecureRunMode->Enable(false);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onCloseSecureRunAuiPane(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( THE_CONTEXT->secureModeInfo.isActivatedByStartup == true ) 
	{
		Close();
		return;
	}
	
	activateSecureMode(false);
	THE_CONTEXT->secureModeInfo.isDeactivatedByUser  = false;
}
///////////////////////////////////////////////////////////////////
void MainFrame::activateSecureMode(bool state) {
///////////////////////////////////////////////////////////////////
	CncSecureCtrlPanel* scp = secureCtrlPanel;
	wxASSERT( scp != NULL );
	
	//-------------------------------------------------------------
	auto swapControls = [&]()
	{
		GblFunc::swapControls(drawPane3D->GetDrawPanePanel(),		m_secMonitorPlaceholder);
		GblFunc::swapControls(getLoggerView(),						m_secLoggerPlaceholder);
		GblFunc::swapControls(lruFileView,							scp->GetLruFilePlaceholder());
		GblFunc::swapControls(transferFileView->getFileView(),		scp->GetTransferDirPlaceholder());
		GblFunc::swapControls(mainFilePreview,						m_leftTplPreviewPlaceholder);
		GblFunc::swapControls(monitorFilePreview,					m_rightTplPreviewPlaceholder);
		GblFunc::swapControls(cncLCDPositionPanel,					m_cncOverviewsPlaceholder);
		GblFunc::swapControls(gamepadSpy,							m_secGamepadPlaceholder);
		GblFunc::swapControls(m_scrollWinPredefinedPositions,		scp->GetPredefinedPositionsPlaceholder());
		
		//mainFilePreview->normalizeView();
		//monitorFilePreview->normalizeView();
	};
	
	// log the state
	THE_CONTEXT->secureModeInfo.isActive = state;
	
	// some control handling
	const bool useIt   = THE_CONTEXT->secureModeInfo.useIt;
	getLoggerView()->setShowOnDemandState(!useIt);
	
	m_btDeactivateSecureRunMode->Enable(THE_CONTEXT->secureModeInfo.isActivatedByStartup);
	
	// switch the state
	if ( THE_CONTEXT->secureModeInfo.isActive == true ) {
		
		getLoggerView()->setSecureMode(true);
		
		perspectiveHandler.logCurrentPerspective();
		hideAllAuiPanes(true);
		
		lruFileView->setBigTheme(true);
		transferFileView->getFileView()->setBigTheme(true);
		
		if ( IsFullScreen() == false )
			ShowFullScreen(true);
		
		showAuiPane("SecureRunPanel",	false);
		showAuiPane("StatusBar",		false);
		
		if ( cncExtMainPreview->IsShownOnScreen() )
			cncExtMainPreview->Show(false);
			
		// ensure known conditions
		m_keepFileManagerPreview->SetValue(false);
		m_keepFileManagerPreview->Enable(false);
		m_externFileManagerPreview->SetValue(false);
		m_externFileManagerPreview->Enable(false);
		
		cncExtViewBoxCluster->hideAll();
		
		// default show the preview of loaded template
		m_securePreviewBook->SetSelection(SecurePreviewBookSelection::VAL::RIGHT_PREVIEW);
		
		swapControls();
	} 
	else 
	{
		lruFileView->setBigTheme(false);
		transferFileView->getFileView()->setBigTheme(false);
		
		if ( IsFullScreen() == true )
			ShowFullScreen(false);
		
		if ( THE_CONTEXT->secureModeInfo.isDeactivatedByUser == true )	perspectiveHandler.loadDefaultPerspective();
		else 															perspectiveHandler.restoreLoggedPerspective();
		
		swapControls();
		
		getLoggerView()->setSecureMode(false);
	}
	
	GetAuimgrMain()->Update();
	secureCtrlPanel->activate(THE_CONTEXT->secureModeInfo.isActive);
}
///////////////////////////////////////////////////////////////////
bool MainFrame::getFirstLruFile(wxString& ret) {
///////////////////////////////////////////////////////////////////
	if ( lruFileView != NULL && lruFileView->getFileCount() > 0 ) {
		ret.assign(lruFileView->getFileName(0));
		return true;
	}
		
	ret.Clear();
	return false;
}
///////////////////////////////////////////////////////////////////
bool MainFrame::setTemplateName(const wxString& pathFile) {
///////////////////////////////////////////////////////////////////
	wxFileName fn(pathFile);
	return setTemplateName(fn);
}
///////////////////////////////////////////////////////////////////
bool MainFrame::setTemplateName(const wxFileName& pathFile) {
///////////////////////////////////////////////////////////////////
	if ( pathFile.Exists() == true ) {
		m_inputFileName->SetValue(pathFile.GetFullName());
		m_inputFileName->SetHint(pathFile.GetFullPath());
		return true;
	}
	
	return false;
}
///////////////////////////////////////////////////////////////////
void MainFrame::traceGccVersion(std::ostream& out) {
///////////////////////////////////////////////////////////////////
	out << __GNUC__
		<< "."
		<< __GNUC_MINOR__
		<< "."
		<< __GNUC_PATCHLEVEL__
		<< std::endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::traceWxWidgetsVersion(std::ostream& out) {
///////////////////////////////////////////////////////////////////
	out << wxMAJOR_VERSION
		<< "."
		<< wxMINOR_VERSION
		<< "."
		<< wxRELEASE_NUMBER;
		
		if ( wxSUBRELEASE_NUMBER > 0 ) {
			out << " - "
			    << wxSUBRELEASE_NUMBER;
		}
		
		out << std::endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::traceWxSvgVersion(std::ostream& out) {
///////////////////////////////////////////////////////////////////
	out << CncSvgViewer::versionInfo();
}
///////////////////////////////////////////////////////////////////
void MainFrame::traceBoostVersion(std::ostream& out) {
///////////////////////////////////////////////////////////////////
	out << BOOST_VERSION / 100000
		<< "."
		<< BOOST_VERSION / 100 % 1000
		<< "."
		<< BOOST_VERSION % 100 
		<< std::endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::traceWoodworkingCncVersion(std::ostream& out) {
///////////////////////////////////////////////////////////////////
	out << globalStrings.programVersionLong << std::endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::traceOpenCvVersion(std::ostream& out) {
///////////////////////////////////////////////////////////////////
	out	<< CV_VERSION
		/*
		<< "."
		<< CV_MAJOR_VERSION
		<< "."
		<< CV_MINOR_VERSION
		<< "."
		<< CV_SUBMINOR_VERSION
		*/
		<< std::endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::onStartupTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	m_startupTimer->Stop();
	
	
	if ( THE_CONTEXT->secureModeInfo.isActivatedByStartup == false ) {
		// Setup AUI Windows menue
		perspectiveHandler.loadDefaultPerspective();
		
	} else {
		// Setup secure mode
		activateSecureMode(true);
	}
	
	{
		CncNanoTimestamp ts1 = 0;
		
		// force the OpenGL as well as GLEW initialization
		{
			// wait intil the main windows is shown
			// this is with respect to the calls below
			ts1 = CncTimeFunctions::getNanoTimestamp();
			while ( IsShown() == false ) {
				dispatchAll(); 
				
				if ( CncTimeFunctions::getTimeSpanToNow(ts1) > 2000 * 1000 * 1000 ) { 
					std::cerr << "MainFrame::startupTimer(): Timeout reached for 'MainFrame::IsShown()'" << std::endl;
					break; 
				}
			}
		}
		
		// wait until the mainframe is shown on screen
		ts1 = CncTimeFunctions::getNanoTimestamp();
		while ( IsShownOnScreen() == false ) {
			dispatchAll(); 
			
			if ( CncTimeFunctions::getTimeSpanToNow(ts1) > 2000 * 1000 * 1000 ) { 
				std::cerr 	<< "MainFrame::startupTimer(): Timeout reached for 'MainFrame::IsShownOnScreen()'" << std::endl;
				break; 
			}
		}
		
		// select each cnc OpenGL canvas to force the OpenGL as well 
		// as GLEW initialization for each context
		{
			m_monitorViewBook->SetSelection(MonitorBookSelection::VAL::CNC_PANEL);
			
				m_outboundNotebook->SetSelection(OutboundSelection::VAL::POSTPOCESSOR_PANAL);
					m_listbookPostProcessor->SetSelection(PostProcessorSelection::VAL::OUTBOUND_PREVIEW);
				
				m_outboundNotebook->SetSelection(OutboundSelection::VAL::MOTION_MONITOR_PANAL);
				
			m_monitorViewBook->SetSelection(MonitorBookSelection::VAL::TEMPLATE_PANEL);
		}
		
		// Version infos
		std::stringstream ss;
		ss.str(""); traceGccVersion(ss); 			THE_CONTEXT->versionInfoMap["gcc"] 			= ss.str().c_str();
		ss.str(""); traceWxWidgetsVersion(ss); 		THE_CONTEXT->versionInfoMap["wxWidgets"] 	= ss.str().c_str();
		ss.str(""); traceWxSvgVersion(ss); 			THE_CONTEXT->versionInfoMap["wxSVG"] 		= ss.str().c_str();
		ss.str(""); traceBoostVersion(ss); 			THE_CONTEXT->versionInfoMap["boost"] 		= ss.str().c_str();
		ss.str(""); traceWoodworkingCncVersion(ss); THE_CONTEXT->versionInfoMap["program"] 		= ss.str().c_str();
		ss.str(""); traceOpenCvVersion(ss);			THE_CONTEXT->versionInfoMap["opencv"] 		= ss.str().c_str();
		
		traceSessionId();
		std::cout << std::endl;
	}
	
	if ( THE_CONFIG->getAutoOpenOglObserverFlag() == true) {
		wxCommandEvent dummy;
		onOpenGLContextObserver(dummy);
	}
	
	// Check open gl state
	if ( GLCommon::isReady() == false ) GL_CTX_OBS->appendMessage('E', CNC_LOG_FUNCT, wxString::Format("OpenGL interface isn't ready!"));
	else								GL_CTX_OBS->appendMessage('I', CNC_LOG_FUNCT, wxString::Format("Application Startup Marker . . . "));
	
	// Auto connect ?
	if ( THE_CONFIG->getAutoConnectFlag() )
		connectSerialPortDialog();
	
	// default slect the cnc panel view
	selectMonitorBookCncPanel();
	
	// Auto open last ?
	if ( THE_CONFIG->getAutoOpenLastFlag() )
		openInitialTemplateFile();
	
	if ( THE_CONTEXT->secureModeInfo.isActivatedByStartup == false ) {
		// Auto process ?
		if ( THE_CONFIG->getAutoProcessFlag() ) {
			
			// some template available?
			if ( getCurrentTemplateFormat() == TplUnknown )
				openInitialTemplateFile();
				
			defineMinMonitoring();
			processTemplate();
			defineNormalMonitoring();
		}
	}
	
	activateGamepadNotifications(true);
	GblFunc::appendToStackTraceFile("Ready ............................");
}
///////////////////////////////////////////////////////////////////
void MainFrame::onTraceTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	static unsigned int callCounter = 0;
	const CncNanoTimestamp tr = CncTimeFunctions::getNanoTimestamp();
	
	switch ( callCounter ) 
	{
		case 0:
		{
					callCounter = 1;
					updateAppPositionControls();
					THE_CONTEXT->timestamps.measuredDurations.md1.add((long)(CncTimeFunctions::getNanoTimestamp()  - tr));
					break;
		}
		case 1:
		{
					callCounter = 2;
					updateCtlPositionControls();
					THE_CONTEXT->timestamps.measuredDurations.md2.add((long)(CncTimeFunctions::getNanoTimestamp()  - tr));
					break;
		}
		case 2: 
		{
					callCounter = 3;
					updateSpeedControls();
					THE_CONTEXT->timestamps.measuredDurations.md3.add((long)(CncTimeFunctions::getNanoTimestamp()  - tr));
					break;
		}
		case 3: 
		{
					callCounter = 0;
					if ( isProcessing() )
					{
						statisticsPane->logStatistics(false);
						THE_CONTEXT->timestamps.measuredDurations.md4.add((long)(CncTimeFunctions::getNanoTimestamp()  - tr));
					}
					break;
		}
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::onSerialTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( cnc == NULL )
		return;
		
	if ( cnc->isConnected() == false )
		return;
	
	// give the cnc or serial port a change to do
	// something periodically e. g. pause a thread
	cnc->onPeriodicallyAppEvent();
	
	// idle handling
	if ( m_miRqtIdleMessages->IsChecked() == true ) {
		
		// to avoid flicker during this short time
		const bool forceDecorateIdleState = false;
		
		// stop the time to avoid overlapping idle request
		stopSerialTimer(forceDecorateIdleState);
		
		// it's very import to deactivate the notifications during idle processing
		// because instead every config change (sc()) will release a notification
		// as a result the processing slows down significantly.
		CncConfig::NotificationDeactivator cfgNotDeactivation(false);
		
		// it's very important to avoid event handling during the idle processing
		// to prevent the start of further commands
		THE_CONTEXT->setAllowEventHandling(false);
		
		// request the idle information
		const bool ret = cnc->sendIdleMessage();
		if ( ret == false )
		{
			CNC_CERR_FUNCT_A(": Error while processing sendIdleMessage()!")
			stopSerialTimer();
			m_miRqtIdleMessages->Check(false);
			
		}
		
		// reconstructed the previous event handling mode
		THE_CONTEXT->setAllowEventHandling(m_menuItemAllowEvents->IsChecked());
		
		// restart due to the previous stopSerialTimer()
		if ( ret == true )	startSerialTimer(forceDecorateIdleState);
		else				decorateIdleState();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::onPaintSpeedPanel(wxPaintEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( cnc == NULL )
		return;
	
	static wxColour col(0, 128, 255);
	static wxBrush  brush(col);
	static wxPen    barPen(col,    1, wxSOLID);
	static wxPen    wmpPen(*wxRED, 1, wxSOLID);
	
	const wxSize size   		= m_speedPanel->GetSize();
	const unsigned int height 	= size.GetHeight();
	
	const unsigned int pos		= size.GetWidth() 
								* cnc->getRealtimeFeedSpeed_MM_MIN() 
								/ THE_CONFIG->getMaxSpeedXYZ_MM_MIN();
	
	const unsigned int wmp		= size.GetWidth() 
								* cnc->getConfiguredFeedSpeed_MM_MIN() 
								/ THE_CONFIG->getMaxSpeedXYZ_MM_MIN();
	
	// bar
		wxPaintDC dc(m_speedPanel);
		dc.SetPen(barPen);
		dc.SetBrush(brush);

		const wxRect rect(0, 0, pos, height);
		dc.DrawRectangle(rect);
	
	// watermark for current config
		dc.SetPen(wmpPen);
		
		// move wp 2 pixel to the left so see valu = max also.
		dc.DrawLine(wmp - 2, 0, wmp - 2, height);
		dc.DrawLine(wmp - 1, 0, wmp - 1, height);
}
///////////////////////////////////////////////////////////////////
void MainFrame::addSetter(unsigned char pid, const cnc::SetterValueList& v) {
///////////////////////////////////////////////////////////////////
	wxASSERT( setterList );
	setterList->addSetter(pid, v);
}
///////////////////////////////////////////////////////////////////
void MainFrame::addAppPosition(unsigned char pid, long id, char speedMode, double cfgSpeedValue, double curSpeedValue, const CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	if ( m_btTogglePosSpy->GetValue() == true )
	{
		positionSpy->addAppPosition(	pid, 
										id, 
										speedMode, 
										cfgSpeedValue, 
										curSpeedValue, 
										pos
									);
	}
	
	speedMonitor->setCurrentFeedSpeedValues(cfgSpeedValue, curSpeedValue);
}
///////////////////////////////////////////////////////////////////
void MainFrame::addCtlPosition(unsigned char pid, long id, char speedMode, double cfgSpeedValue, double curSpeedValue, const CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	if ( m_btTogglePosSpy->GetValue() == true )
	{
		positionSpy->addCtlPosition(	pid, 
										id, 
										speedMode, 
										cfgSpeedValue, 
										curSpeedValue, 
										pos
									);
	}
	
	speedMonitor->setCurrentFeedSpeedValues(cfgSpeedValue, curSpeedValue);
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateAppPositionControls() {
///////////////////////////////////////////////////////////////////
	if ( cnc == NULL )
		return;
		
	const CncUnit unit 			= THE_CONFIG->getDisplayUnit();
	const CncLongPosition pos	= cnc->getCurAppPos();
	
	// update position
	switch ( unit ) {
		case CncSteps:	// update application position
						m_xAxis->ChangeValue(wxString::Format("%8ld", pos.getX()));
						m_yAxis->ChangeValue(wxString::Format("%8ld", pos.getY()));
						m_zAxis->ChangeValue(wxString::Format("%8ld", pos.getZ()));
						break;
						
		case CncMetric:	// update application position
						m_xAxis->ChangeValue(wxString::Format("%4.3lf", pos.getX() * THE_CONFIG->getDisplayFactX(unit)));
						m_yAxis->ChangeValue(wxString::Format("%4.3lf", pos.getY() * THE_CONFIG->getDisplayFactY(unit)));
						m_zAxis->ChangeValue(wxString::Format("%4.3lf", pos.getZ() * THE_CONFIG->getDisplayFactZ(unit)));
						break;
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateCtlPositionControls() {
///////////////////////////////////////////////////////////////////
	if ( cnc == NULL )
		return;
		
	const CncUnit unit 			= THE_CONFIG->getDisplayUnit();
	const CncLongPosition pos	= cnc->getCurCtlPos();
	
	// update position
	switch ( unit ) 
	{
		case CncSteps:	// update controller position
						m_xAxisCtl->ChangeValue(wxString::Format("%8ld", pos.getX()));
						m_yAxisCtl->ChangeValue(wxString::Format("%8ld", pos.getY()));
						m_zAxisCtl->ChangeValue(wxString::Format("%8ld", pos.getZ()));
						break;
						
		case CncMetric:	// update controller position
						m_xAxisCtl->ChangeValue(wxString::Format("%4.3lf", pos.getX() * THE_CONFIG->getDisplayFactX(unit)));
						m_yAxisCtl->ChangeValue(wxString::Format("%4.3lf", pos.getY() * THE_CONFIG->getDisplayFactY(unit)));
						m_zAxisCtl->ChangeValue(wxString::Format("%4.3lf", pos.getZ() * THE_CONFIG->getDisplayFactZ(unit)));
						break;
	}
	
	// update z view
	if ( drawPane3D && drawPane3D->GetZView() )
		drawPane3D->GetZView()->updateView(pos.getZ() * THE_CONFIG->getDisplayFactZ(unit));
	
	// lcd view
	if ( cncLCDPositionPanel && cncLCDPositionPanel->IsShownOnScreen() )
		cncLCDPositionPanel->updateValues();
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateSpeedControls() {
///////////////////////////////////////////////////////////////////
	if ( cnc == NULL )
		return;
		
	// Configured speed values
	m_configuredFeedSpeed    ->ChangeValue(wxString::Format("%3.1lf", cnc->getConfiguredFeedSpeed_MM_MIN()));
	m_configuredFeedSpeedMode->ChangeValue(wxString::Format("%c",     cnc::getCncSpeedTypeAsCharacter(cnc->getConfiguredSpeedMode())));
	
	speedMonitor->update();
	
	// Realtime speed values
	if ( THE_CONTEXT->isProbeMode() == false && THE_CONTEXT->canSpeedMonitoring() )
	{
		
		if ( m_speedPanel->IsShownOnScreen() )
			m_speedPanel->Refresh();
		
		const double dValue = cnc->getRealtimeFeedSpeed_MM_MIN();
		m_realtimeFeedSpeed->ChangeValue(dValue >= 0.0 ? wxString::Format("%.1lf", dValue) : _maxSpeedLabel);
	} 
	else
	{
		m_realtimeFeedSpeed->ChangeValue(_maxSpeedLabel);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateSpeedSlider(float value) {
///////////////////////////////////////////////////////////////////
	// this is only a preview
	defaultSpeedSlider->previewValue(value);
	
	if ( cnc != NULL )
		cnc->previewConfiguredFeedSpeed_MM_MIN(CncSpeedUserDefined, value);
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateAndSetSpeedSlider(float value) {
///////////////////////////////////////////////////////////////////
	defaultSpeedSlider->setValue(value);
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateCncSpeed(float value, CncSpeedMode mode) {
///////////////////////////////////////////////////////////////////
	if ( cnc == NULL )
		return;
		
	if ( cnc->isConnected() ) 
		cnc->changeCurrentFeedSpeedXYZ_MM_MIN(value, mode);
}
///////////////////////////////////////////////////////////////////
void MainFrame::onGamepadThreadInitialized(GamepadEvent& event) {
///////////////////////////////////////////////////////////////////
	//std::cout << "MainFrame::onGamepdThreadInitilaized" << std::endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::onGamepadThreadCompletion(GamepadEvent& event) {
///////////////////////////////////////////////////////////////////
	std::clog << CNC_LOG_FUNCT << std::endl;
	gamepadThread = NULL;
}
///////////////////////////////////////////////////////////////////
void MainFrame::onGamepadThreadMessage(GamepadEvent& event) {
///////////////////////////////////////////////////////////////////
	std::cout << "GAMEPAD: "<< event.GetString() << std::endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::onGamepadThreadHeartbeat(GamepadEvent& event) {
///////////////////////////////////////////////////////////////////
	static int counter = 0;
	
	if ( (++counter)%3 != 0 )
		return;
		
	counter = 0;
	
	decorateGamepadState(event.data.connected);
	
	if ( event.data.connectionStateChanged == true ) {
		if ( event.data.connected == false )	cnc::trc.logWarning(" Gamepad disconnected");
		else									cnc::trc.logWarning(" Gamepad connected");
	}
	
	// rotate heartbeat image
	wxImage img = m_gamepadThreadHeartbeat->GetBitmap().ConvertToImage();
	if ( img.IsOk() == false )
		return;
		
	m_gamepadThreadHeartbeat->SetBitmap(wxBitmap(img.Rotate90()));
	m_gamepadThreadHeartbeat->Refresh();
	//m_gamepadThreadHeartbeat->Update();
}
///////////////////////////////////////////////////////////////////
void MainFrame::onGamepadThreadUpadte(GamepadEvent& event) {
///////////////////////////////////////////////////////////////////
	// always do the heartbeat handling . . . 
	onGamepadThreadHeartbeat(event);
	
	if ( event.data.connected == false ) {
		// update spy view
		if ( gamepadSpy )
			gamepadSpy->update(&event);
			
		return;
	}
		
	if ( cnc == NULL || cnc->isConnected() == false )
		return;
	
	if ( THE_CONTEXT->canInteractiveMoving() == false ) {
		cnc::trc.logInfo("The gamepad isn't available for this connetion port . . . ");
		return;
	}
	
	// update gamepad state view and process the movement
	if ( gamepadStatusCtl )
		gamepadStatusCtl->update(event);
		
	// update spy view
	if ( gamepadSpy )
		gamepadSpy->update(&event);
}
///////////////////////////////////////////////////////////////////
void MainFrame::onSerialThreadInitialized(SerialEvent& event) {
///////////////////////////////////////////////////////////////////
	//std::cout << CNC_LOG_LOCATION << std::endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::onSerialThreadCompletion(SerialEvent& event) {
///////////////////////////////////////////////////////////////////
	std::clog << CNC_LOG_FUNCT << std::endl;
	serialThread = NULL;
}
///////////////////////////////////////////////////////////////////
void MainFrame::onSerialThreadHeartbeat(SerialEvent& event) {
///////////////////////////////////////////////////////////////////
	// rotate heartbeat image
	wxImage img = m_serialThreadHeartbeat->GetBitmap().ConvertToImage();
	if ( img.IsOk() == false )
		return;
		
	m_serialThreadHeartbeat->SetBitmap(wxBitmap(img.Rotate90()));
	m_serialThreadHeartbeat->Refresh();
}
///////////////////////////////////////////////////////////////////
void MainFrame::onSerialThreadMessage(SerialEvent& event) {
///////////////////////////////////////////////////////////////////
	const char type = (char)event.message.type;
	
	if ( cncArduinoEnvironment->IsShownOnScreen() )
		cncArduinoEnvironment->appendMessage(type, event.message.getMessage(), event.message.getContext());
	
	if ( cncSpeedPlayground->IsShownOnScreen() )
		cncSpeedPlayground->appendArdoMessage(type, event.message.getMessage(), event.message.getContext());
	
	const bool alreadyTraced = cncArduinoEnvironment->IsShownOnScreen() || cncSpeedPlayground->IsShownOnScreen();
	
	//-------------------------------------------------------------
	auto log2Std = [&](std::ostream & o) {
		return;
		
		o << "ArdoMsg: " << type 
						 << " " 
						 << event.message.getMessage() 
						 /*
						 << std::setw(80) 
						 << " [[ " 
						 << event.message.getContext() 
						 << " ]]"
						 */ 
						 << std::endl;
	};
	
	if ( alreadyTraced == false ) {
		switch ( type ) {
			case 'E':	log2Std(std::cerr); break;
			case 'D':
			case 'W':	log2Std(std::clog); break;
			case 'S':	log2Std(cnc::cex1); break;
			default:	log2Std(std::cout);
		}
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::onSerialThreadPinNotification(SerialEvent& event) {
///////////////////////////////////////////////////////////////////
	cncArduinoEnvironment->notifyPinUpdate();
}
///////////////////////////////////////////////////////////////////
void MainFrame::onSerialThreadDataNotification(SerialEvent& event) {
///////////////////////////////////////////////////////////////////
	cncArduinoEnvironment->notifyDataUpdate();
}
/////////////////////////////////////////////////////////////////////
bool MainFrame::readSerialThreadData(AE::TransferData& td) {
/////////////////////////////////////////////////////////////////////
	if ( serialThread == NULL )
		return false;
		
	return serialThread->transferData(td);
}
///////////////////////////////////////////////////////////////////
void MainFrame::onClose(wxCloseEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( saveTemplateOnDemand() == false )
		return;
		
	if ( canClose == false) {
		cnc::trc.logWarning("Can't close the application due to an active run/debug session");
		return;
	}
	
	// Destroy the serial port connection.
	// It's very important to do this here in case  
	// the serial port has threads
	if ( cnc != NULL ) {
		{
			wxCriticalSectionLocker enter(pUpdateManagerThreadCS);
			cnc->sendHalt();
			cnc->reset();
			cnc->disconnect();
			
			// this will crash if a run  or debug is active
			// this have to be avoided - see can close above
			delete cnc;
			cnc = NULL;
		}
	}
	
	// Destroy the gamepad thread
	if ( gamepadThread != NULL) {
		if ( gamepadThread->IsRunning() == false) {
			wxCriticalSectionLocker enter(pGamepadThreadCS);
			gamepadThread->Resume();
		}
		
		gamepadThread->stop();
		
		while ( true )
		{
			{ // was the ~GamepadThread() function executed?
				wxCriticalSectionLocker enter(pGamepadThreadCS);
				if ( !gamepadThread ) 
					break;
			}
			// wait for thread completion
			wxThread::This()->Sleep(10);
		}
		 
	}
	
	// This has to be done before the serial thread stops
	cncArduinoEnvironment->shutdownTimer();
	
	// Destroy the serial thread
	if ( serialThread != NULL) {
		if ( serialThread->IsRunning() == false) {
			wxCriticalSectionLocker enter(pSerialThreadCS);
			serialThread->Resume();
		}
		
		serialThread->stop();
		
		while ( true )
		{
			{ // was the ~GamepadThread() function executed?
				wxCriticalSectionLocker enter(pSerialThreadCS);
				if ( !serialThread ) 
					break;
			}
			// wait for thread completion
			wxThread::This()->Sleep(10);
		}
		 
	}
	
	// hide all external windows 
	cncSpeedPlayground->Show(false);
	positionStorage->Show(false);
	
	cncExtMainPreview->Show(false);
	cncExtViewBoxCluster->hideAll();

	cncArduinoEnvironment->activateOnClose(true);
	
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void MainFrame::waitActive(unsigned int milliseconds, bool once) {
///////////////////////////////////////////////////////////////////
	if ( milliseconds == 0 ) {
		if ( once == true )
			dispatchAll();
		
		return;
	}
	
	wxDateTime s = wxDateTime::UNow();
	wxDateTime e = s;

	while ( wxTimeSpan(e - s).GetMilliseconds() < milliseconds ) {
		dispatchNext();
		e = wxDateTime::UNow();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::dispatchNext() {
///////////////////////////////////////////////////////////////////
	wxEventLoopBase* evtLoop = wxEventLoopBase::GetActive();
	if ( evtLoop == NULL )
		return;
	
	if ( evtLoop->Pending() )
		evtLoop->Dispatch();
}
///////////////////////////////////////////////////////////////////
void MainFrame::dispatchTimerEvents() {
///////////////////////////////////////////////////////////////////
	wxEventLoopBase* evtLoop = wxEventLoopBase::GetActive();
	if ( evtLoop == NULL )
		return;

	evtLoop->YieldFor(wxEVT_CATEGORY_TIMER);
}
///////////////////////////////////////////////////////////////////
void MainFrame::dispatchAll() {
///////////////////////////////////////////////////////////////////
	// this has to be done before SafeYield of the MainFrame
	// because otherwise it has no effect
	if ( podiumManagementDlg->IsShownOnScreen() )
	{
		// this dialogue is opened modal.
		// therefore, nothing more to do in the main event loop 
		// and we can return in this case 
		podiumManagementDlg->dispatchAll();
		return;
	}
	
	wxEventLoopBase* evtLoop = wxEventLoopBase::GetActive();
	if ( evtLoop == NULL )
		return;
	
	// This avoids further doings if the current event loop isn't the main one
	// Otherwise modal dialogues loose teh modal state for example.
	if ( evtLoop->IsMain() == false )
		return;

	// Since wxWidgets 3.1.x: This is the best compromise 
	wxTheApp->SafeYield(this, true);
	//wxTheApp->Yield();

	/*
	Please note: This is the fastest version, but evtLoop->Yield() is better then the code below, 
	it also considers timer events, aui-handling etc.
	
	wxEventLoopBase* evtLoop = wxEventLoopBase::GetActive();
	if ( evtLoop == NULL )
		return;

	while ( evtLoop->Pending() )
		evtLoop->Dispatch();
	*/

	/*
	wxEVT_CATERY_ALL =
		wxEVT_CATEGORY_UI|wxEVT_CATEGORY_USER_INPUT|wxEVT_CATEGORY_SOCKET| \
		wxEVT_CATEGORY_TIMER|wxEVT_CATEGORY_THREAD|wxEVT_CATEGORY_UNKNOWN| \
		wxEVT_CATEGORY_CLIPBOARD
	
	This is the slowest version
	evtLoop->YieldFor(wxEVT_CATEGORY_ALL);
	*/
	
	// the following code is the best compromise, but aui handling isn't perfect
	//if ( wxTheApp->HasPendingEvents() )
		//wxTheApp->ProcessPendingEvents();
	
	/*
	while ( evtLoop->Pending() ) {
		evtLoop->Dispatch();
	}
	*/
}
///////////////////////////////////////////////////////////////////
void MainFrame::handleCommonException() {
///////////////////////////////////////////////////////////////////
	//TODO: May be do more here
	
	// stop idle request
	stopSerialTimer();
	
	disconnectSerialPort();
	
	if ( serialThread == NULL )
		serialThread->notifyInterrupted();
		
	if ( usbConnectionObserver->IsShown() ) 
		usbConnectionObserver->EndModal(wxID_NO);
		
	// to get the full gui control again
	enableRunControls(true);
	
	// go silent again
	CncSpindleSound::stop();
	
	THE_CONTEXT->resetProcessing();
}
///////////////////////////////////////////////////////////////////
void MainFrame::handleCncInterruptException(const CncInterruption& ex) {
///////////////////////////////////////////////////////////////////
	std::cerr	<< std::endl
				<< wxString('!',80)
				<< std::endl
				
				<< CNC_LOG_FUNCT
				<< ":"
				<< std::endl
				<< " Exception received:" 
				<< std::endl
				
				<< "Context: "
				<< std::endl
				<< " " << ex.getCatchLocation()
				<< std::endl
				
				<< "What:"
				<< std::endl
				<< " " << ex.what()
				<< std::endl
				
				<< wxString('!',80)
				<< std::endl
				<< std::endl
	;
	
	handleCommonException();
}
///////////////////////////////////////////////////////////////////
void MainFrame::handleUnhandledException(const wxString& context) {
///////////////////////////////////////////////////////////////////
	std::cerr	<< std::endl
				<< wxString('!',80)
				<< std::endl
				
				<< CNC_LOG_FUNCT 
				<< ":"
				<< std::endl
				<< " Unhandled exception received:"
				<< std::endl
				
				<< "Context: "
				<< std::endl
				<< " " << context
				<< std::endl
				
				<< wxString('!',80)
				<< std::endl
				<< std::endl
	;
	
	handleCommonException();
}
///////////////////////////////////////////////////////////////////
void MainFrame::notifyComPortArrival(const wxString& rawPortName) {
///////////////////////////////////////////////////////////////////
	cnc::trc.logInfo(wxString("A new COM device was detected on port: ") << rawPortName);
	
	wxString portName(rawPortName);
	const int portIdx = isPortNameAvailable(portName);
	if ( portIdx != wxNOT_FOUND ) 
	{
		// override blurred search port names like COM4
		// with its long name
		portName = m_portSelector->GetString(portIdx);
		
		if ( usbConnectionObserver->IsShown() )
		{
			// update port name only
			usbConnectionObserver->setPortName(portName);
		}
		else
		{
			if ( usbConnectionObserver->getSensitivity() == true )
			{
				usbConnectionObserver->setPortName(portName);
				
				if ( usbConnectionObserver->ShowModal() == wxID_YES )
				{
					m_portSelector->SetStringSelection(portName);
					connectSerialPortDialog();
				}
			}
			else
			{
				cnc::trc.logInfoMessage(wxString::Format("New USB connection available, name: %s", portName));
			}
		}
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::notifyComPortRemoval(const wxString& rawPortName) {
///////////////////////////////////////////////////////////////////
	// The content of lastPortName is may be already empty. 
	// This is the case if an corresponding exception was handled before 
	// CNC_PRINT_FUNCT_A("'%s'.Contains(%s)", lastPortName, rawPortName)
	
	// check if the current connection is effected
	if ( lastPortName.Contains(rawPortName) )
	{
		const wxString msg(wxString::Format("The connected COM device was removed from port: %s. Try to disconnect to clean-up the situation", rawPortName));
		cnc::trc.logError(msg);
		
		// try to disconnect
		disconnectSerialPort();
	}
	else
	{
		const wxString msg(wxString::Format("The unused COM device was removed from port: %s", rawPortName));
		cnc::trc.logInfo(msg);
	}
}
#ifdef __WXMSW__
///////////////////////////////////////////////////////////////////
WXLRESULT MainFrame::onDeviceChange(WXUINT message, WXWPARAM wParam, WXLPARAM lParam) {
///////////////////////////////////////////////////////////////////
	wxString portName("Undefined");
	
	// ------------------------------------------------------------
	// logging
	auto traceDeviceEvent = [&](const wxString& context)
	{
		if ( false )
		{
			PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;
			long type = lpdb ? lpdb->dbch_devicetype : -1;
			
			CNC_CLOG_FUNCT_A(": %s: type=%ld", context, type)
		}
	};
	
	// ------------------------------------------------------------
	auto isPortType = [&]()
	{
		PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;
		return ( lpdb && lpdb->dbch_devicetype == DBT_DEVTYP_PORT );
	};
	
	// ------------------------------------------------------------
	auto evaluatePortName = [&]()
	{
		if ( isPortType() )
		{
			PDEV_BROADCAST_PORT pPort = (PDEV_BROADCAST_PORT)lParam;
			const wxString n(pPort->dbcp_name);
			portName.assign(n);
			
			return true;
		}
		
		return false;
	};
	
	// ------------------------------------------------------------
	// evaluation
	switch ( wParam )
	{
		case DBT_DEVICEARRIVAL:
		{
			//if ( isProcessing() == false )
			if ( CncTransactionLockBase::isLocked() == false )
			{
				traceDeviceEvent(wxString::Format("DBT_DEVICEARRIVAL(%d)", (int)wParam));
				
				if ( evaluatePortName() )
				{
					decoratePortSelector();
					notifyComPortArrival(portName);
				}
			}
			
			break;
		}
		case DBT_DEVICEREMOVECOMPLETE:
		{
			traceDeviceEvent(wxString::Format("DBT_DEVICEREMOVECOMPLETE(%d)", (int)wParam));
			
			if ( evaluatePortName() )
			{
				notifyComPortRemoval(portName);
				decoratePortSelector();
			}
			
			break;
		}
		case DBT_DEVNODES_CHANGED: 
		{
			traceDeviceEvent(wxString::Format("DBT_DEVNODES_CHANGED(%d)", (int)wParam));
			
			activateGamepadNotificationsOnDemand(true);
			break;
		}
		default:
		{
			// show also not handled event types
			if ( false )
				traceDeviceEvent(wxString::Format("DBT_XXX(%d)", (int)wParam));
		}
	}
	
	// default system handling
	return MainFrameBase::MSWWindowProc(message, wParam, lParam);
}
///////////////////////////////////////////////////////////////////
WXLRESULT MainFrame::MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam) {
///////////////////////////////////////////////////////////////////
	try 
	{
		switch ( message )
		{
			case WM_DEVICECHANGE:	return onDeviceChange(message, wParam, lParam);
			default:				return MainFrameBase::MSWWindowProc(message, wParam, lParam);
		}
	}
	catch (const CncInterruption& ex) 
	{
		CncInterruption nex(ex);
		nex.addCatchLocation(CNC_LOG_FUNCT);
		handleCncInterruptException(nex);
	}
	catch (...) 
	{
		handleUnhandledException(CNC_LOG_FUNCT);
	}
	
	return 0L;
}
#else
	// currently no equivalent
#endif
///////////////////////////////////////////////////////////////////
int MainFrame::isPortNameAvailable(const wxString& portName, bool exact) {
///////////////////////////////////////////////////////////////////
	const int pos = m_portSelector->FindString(portName);
	
	if ( pos != wxNOT_FOUND )
		return pos;
		
	if ( exact == false )
	{
		for (int i = 0; i < (int)m_portSelector->GetCount(); i++ )
		{
			if ( m_portSelector->GetString(i).Contains(portName) )
				return i;
		}
	}
	
	return wxNOT_FOUND;
}
///////////////////////////////////////////////////////////////////
void MainFrame::searchAvailiablePorts(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	disableControls();
	decoratePortSelector(CncAsyncKeyboardState::isControlPressed());
	enableControls();
	
	if ( m_portSelector->FindString(lastPortName) == wxNOT_FOUND ) {
		m_portSelector->Popup();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::decoratePortSelector(bool list) {
///////////////////////////////////////////////////////////////////
	CncRunAnimationControl rac(this);
	
	m_portSelector ->Clear();
	secureCtrlPanel->clearPortSelection();
	
	//------------------------------------------------------------
	auto appendItem = [&](const wxString& item, const wxString& bitmapName = "") {
		wxBitmap bitmap = bitmapName.IsEmpty() == false ? ImageLibPortSelector().Bitmap(bitmapName) : wxNullBitmap;
	
		m_portSelector->Append      (item,  bitmap);
		secureCtrlPanel->addPortName(item,  bitmapName);
	};
	
	// add default preprocessor ports ports
	if ( lastPortName == _portPreProcMonitor )	appendItem(_portPreProcMonitor,		PortSelector::BMP_PS_CONNECTED);
	else										appendItem(_portPreProcMonitor,		PortSelector::BMP_PS_AVAILABLE);
	
	if ( lastPortName == _portPreProcFile )		appendItem(_portPreProcFile,		PortSelector::BMP_PS_CONNECTED);
	else										appendItem(_portPreProcFile,		PortSelector::BMP_PS_AVAILABLE);
	
	appendItem(_portSeparator);
	
	// add default cnc ports
	if ( lastPortName == _portEmulatorNULL )	appendItem(_portEmulatorNULL,		PortSelector::BMP_PS_CONNECTED);
	else										appendItem(_portEmulatorNULL,		PortSelector::BMP_PS_AVAILABLE);
	
	if ( lastPortName == _portEmulatorTEXT )	appendItem(_portEmulatorTEXT,		PortSelector::BMP_PS_CONNECTED);
	else										appendItem(_portEmulatorTEXT,		PortSelector::BMP_PS_AVAILABLE);
	
	if ( lastPortName == _portEmulatorSVG )		appendItem(_portEmulatorSVG,		PortSelector::BMP_PS_CONNECTED);
	else										appendItem(_portEmulatorSVG,		PortSelector::BMP_PS_AVAILABLE);
	
	if ( lastPortName == _portEmulatorGCODE )	appendItem(_portEmulatorGCODE,		PortSelector::BMP_PS_CONNECTED);
	else										appendItem(_portEmulatorGCODE,		PortSelector::BMP_PS_AVAILABLE);
	
	if ( lastPortName == _portEmulatorBIN )		appendItem(_portEmulatorBIN, 		PortSelector::BMP_PS_CONNECTED);
	else										appendItem(_portEmulatorBIN,		PortSelector::BMP_PS_AVAILABLE);
	
	if ( lastPortName == _portEmulatorArduino )	appendItem(_portEmulatorArduino,	PortSelector::BMP_PS_CONNECTED);
	else										appendItem(_portEmulatorArduino, 	PortSelector::BMP_PS_AVAILABLE);
	
	// add com cnc ports
	bool available	= false;
	
	//------------------------------------------------------------
	auto appendCOMItem = [&](const wxString& item, const wxString& bitmapName = "") {
		
		if ( available == false ) {
			appendItem(_portSeparator);
			available = true;
		}
		
		appendItem(item, bitmapName);
	};
	
	CncAvailableArduionPorts::PortList pl;
	CncAvailableArduionPorts::evaluate(pl);
	
	for ( auto it = pl.begin(); it != pl.end(); ++it ) {
		
		const CncAvailableArduionPorts::Port& port = *it;
	
		const wxString pn(wxString::Format("COM%d - %s", port.num, port.desc));
		const bool b  = cnc && cnc->isConnected() && lastPortName == pn;
		const int ret = CncUsbPortScanner::isComPortAvailable(port.num);
		
		switch ( ret ) {
			case 0:		appendCOMItem(pn, (b ? PortSelector::BMP_PS_CONNECTED : PortSelector::BMP_PS_ACCESS_DENIED));
						break;
						
			case 1:		appendCOMItem(pn, PortSelector::BMP_PS_AVAILABLE);
						break;
						
			default: 	if ( list == true ) 
							appendCOMItem(pn, PortSelector::BMP_PS_UNKNOWN);
		}
	}
	
	// select the last port, if availiable
	if ( m_portSelector->FindString(lastPortName) != wxNOT_FOUND ) {
		m_portSelector->SetStringSelection(lastPortName);
		secureCtrlPanel->updatePortSelection(lastPortName);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::setIcons() {
///////////////////////////////////////////////////////////////////
	// Set icon(s) to the application/dialog
	wxIconBundle app_icons;

	#ifdef DEBUG
			const wxString iconTyp("D");
	#else
		#ifdef GPROF
			const wxString iconTyp("G");
		#else
			const wxString iconTyp("R");
		#endif
	#endif
	
	int sizes[] = {16, 32, 64, 128, 256};

	for ( unsigned int i=0; i<sizeof(sizes)/sizeof(int); i++) {
		wxBitmap iconBmp = ImageLibAppIcons().Bitmap(wxString::Format("BMP_APP_%s%d", iconTyp, sizes[i]));
		wxIcon icn;
		icn.CopyFromBitmap(iconBmp);
		app_icons.AddIcon( icn );
	}
	
	if ( app_icons.GetIconCount() > 0 )
		SetIcons( app_icons );
}
///////////////////////////////////////////////////////////////////
void MainFrame::initializeGamepadThread() {
///////////////////////////////////////////////////////////////////
	// create the thread
	gamepadThread = new GamepadThread(this);
	wxThreadError error = gamepadThread->Create();

	if (error != wxTHREAD_NO_ERROR) {
		wxMessageBox( _("Couldn't create gamepad thread!") );
		abort();
	}
	
	error = gamepadThread->Run();
	if (error != wxTHREAD_NO_ERROR) {
		wxMessageBox( _("Couldn't run gamepad thread!") );
		abort();
	}
	
	// this have to be done later, because the cnc controller 
	// isn't initialized at this early state
	// activateGamepadNotifications(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::initializeSerialThread() {
///////////////////////////////////////////////////////////////////
	// create the thread
	serialThread = new SerialThread(this);
	wxThreadError error = serialThread->Create();

	if (error != wxTHREAD_NO_ERROR) {
		wxMessageBox( _("Couldn't create serial thread!") );
		abort();
	}
	
	error = serialThread->Run();
	if (error != wxTHREAD_NO_ERROR) {
		wxMessageBox( _("Couldn't run serial thread!") );
		abort();
	}
	
	wxASSERT ( cncArduinoEnvironment != NULL );
	if ( serialThread->IsRunning() )
		cncArduinoEnvironment->notifyStarted();
}
///////////////////////////////////////////////////////////////////
bool MainFrame::Show(bool show) {
///////////////////////////////////////////////////////////////////
	bool ret = MainFrameBClass::Show(show);

	if ( show == true )  {
		toggleMotionMonitorOptionPane(true);
		showMotionMonitorReplayPane(true);

		auto startTimer = [](wxTimer* timer, unsigned int value, const char* name) {
			
			if ( timer->IsRunning() == false) {
				timer->Start(value);
				APPEND_LOCATION_TO_STACK_TRACE_FILE_A(wxString::Format("Start timer: Call %s->Start(%d)", name, value));
			
			} else {
				
				if ( timer->GetInterval() != (int)value ) {
					timer->Start(value);
					APPEND_LOCATION_TO_STACK_TRACE_FILE_A(wxString::Format("Restart Timer: Call %s->Start(%d)", name, value));
				}
				else {
					APPEND_LOCATION_TO_STACK_TRACE_FILE_A(wxString::Format("%s [%d] already started", name, timer->GetInterval()));
				}
			}
		};
		
		// Now all controls are well created, this is the perfect time to (re)start these timers
		startTimer(m_startupTimer,	startTimerTimeout,  "m_startupTimer");
		startTimer(m_serialTimer,	serialTimerTimeout, "m_serialTimer" );
		startTimer(m_traceTimer,	traceTimerTimeout,  "m_traceTimer"  );
	}

	APPEND_LOCATION_TO_STACK_TRACE_FILE_A(wxString::Format("Result = %d; show = %d", ret, show));

	return ret;
}
///////////////////////////////////////////////////////////////////
void MainFrame::initialize(void) {
///////////////////////////////////////////////////////////////////
	createAnimationControl();
	decorateSearchButton();
	switchMonitorButton(true);
	determineRunMode();
	decoratePosSpyConnectButton(THE_CONFIG->getUsePositionSpyFlag());
	decorateSecureDlgChoice(true);
	registerGuiControls();
	decorateOutboundSaveControls(false);
	updateHardwareDimensions();

	// init config pages
	GetConfigurationToolbook()->SetSelection(0);
	m_pgMgrSetup->SelectPage(GetConfigurationToolbook()->GetSelection());

	speedMonitor->init();
	m_speedPanel->SetBackgroundColour(wxColour(234, 234, 234));
	
	perspectiveHandler.setupUserPerspectives();
	
	this->SetTitle(globalStrings.programVersionLong);
	
	// setup cnc port selector box
	decoratePortSelector();
	
	wxFloatingPointValidator<float> val(3, NULL, wxNUM_VAL_DEFAULT );//, wxNUM_VAL_ZERO_AS_BLANK);
	val.SetRange(0, 100.0);
	val.SetPrecision(3);
	m_testDistanceX->SetValidator(val);
	m_testDistanceY->SetValidator(val);
	m_testDistanceZ->SetValidator(val);
	
	val.SetRange(0, 50000);
	val.SetPrecision(0);
	
	wxTextValidator tVal(wxFILTER_NUMERIC);
	tVal.SetCharIncludes(", ");
	m_cbUCValueFrom->SetValidator(tVal);
	m_metricValueFrom->SetValidator(tVal);
	
	if ( CncConfig::getGlobalCncConfig()->getShowTestMenuFlag() == false )
		m_menuBar->Remove(m_menuBar->FindMenu("Test"));
		
	serialSpyPanel->enableSerialSpy(false);
	
	resetMinMaxPositions();
	initializeLruMenu();
	initializeConnectionSelector();
	
	m_outboundNotebook->SetSelection(OutboundSelection::VAL::MOTION_MONITOR_PANAL);
	m_listbookMonitor->SetSelection(OutboundMonitorSelection::VAL::MOTION_MONITOR_PANAL);
	m_notebookConfig->SetSelection(OutboundCfgSelection::VAL::SUMMARY_PANEL);
	
	updateUnit();
}
///////////////////////////////////////////////////////////////////
void MainFrame::initializeConnectionSelector() {
///////////////////////////////////////////////////////////////////
	if ( THE_CONFIG == NULL )
		return;
		
	// initialize display unit
	m_unit->SetStringSelection(THE_CONFIG->getDefaultDisplayUnitAsStr());
	updateUnit();
	
	// initialize default serial port
	wxString value;
	const bool exactSearch = false;
	const int portIdx = isPortNameAvailable(THE_CONFIG->getDefaultPort(value), exactSearch);
	
	if ( portIdx != wxNOT_FOUND )
		m_portSelector->SetSelection(portIdx);
	
	// initialize update interval
	THE_CONTEXT->setUpdateInterval(m_displayInterval->GetValue());
}
///////////////////////////////////////////////////////////////////
bool MainFrame::initializeLruMenu() {
///////////////////////////////////////////////////////////////////
	return lruFileView->load(lruStore);
}
///////////////////////////////////////////////////////////////////
bool MainFrame::openTemplateFile(const wxFileName& fn) {
///////////////////////////////////////////////////////////////////
	if ( fn.Exists() == false ) 
		return false; 
	
	m_inputFileName->SetValue(fn.GetFullName());
	m_inputFileName->SetHint(fn.GetFullPath());
	
	const bool ret = openFile();
	prepareAndShowMonitorTemplatePreview(true);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool MainFrame::openInitialTemplateFile() {
///////////////////////////////////////////////////////////////////
	wxASSERT ( lruFileView != NULL );
	
	wxString value;
	THE_CONFIG->getDefaultTplDir(value);
	fileView->setDefaultPath(value);
	fileView->selectDefaultPath();
	
	THE_CONFIG->getDefaultTplFile(value);
	wxFileName fn;
	if ( value.length() > 0 ) 	fn.Assign(value);
	else 						fn.Assign(lruFileView->getFileName(0));
	
	const bool ret = openTemplateFile(fn);
	if ( ret == false )
	{
		THE_CONFIG->getDefaultTplDir(value);
		fileView->openDirectory(value);
		
		selectMainBookSourcePanel();
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateUnit() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	wxASSERT(cncManuallyMoveCoordPanel);
	
	const wxString unit = m_unit->GetValue();
	
	wxString xAppPos(m_xAxis->GetValue());
	wxString yAppPos(m_yAxis->GetValue());
	wxString zAppPos(m_zAxis->GetValue());
	wxString xCtlPos(m_xAxisCtl->GetValue());
	wxString yCtlPos(m_yAxisCtl->GetValue());
	wxString zCtlPos(m_zAxisCtl->GetValue());
	
	if ( unit == "mm" ) { 
		THE_CONFIG->setDisplayUnit(CncMetric); 
		
		long v; 
		xAppPos.ToLong(&v); m_xAxis->ChangeValue   (wxString::Format("%4.3lf", (double)(v * THE_CONFIG->getDisplayFactX())));
		yAppPos.ToLong(&v); m_yAxis->ChangeValue   (wxString::Format("%4.3lf", (double)(v * THE_CONFIG->getDisplayFactY())));
		zAppPos.ToLong(&v); m_zAxis->ChangeValue   (wxString::Format("%4.3lf", (double)(v * THE_CONFIG->getDisplayFactZ())));
		xCtlPos.ToLong(&v); m_xAxisCtl->ChangeValue(wxString::Format("%4.3lf", (double)(v * THE_CONFIG->getDisplayFactX())));
		yCtlPos.ToLong(&v); m_yAxisCtl->ChangeValue(wxString::Format("%4.3lf", (double)(v * THE_CONFIG->getDisplayFactY())));
		zCtlPos.ToLong(&v); m_zAxisCtl->ChangeValue(wxString::Format("%4.3lf", (double)(v * THE_CONFIG->getDisplayFactZ())));
		
	} else {
		THE_CONFIG->setDisplayUnit(CncSteps);
		
		double v; 
		xAppPos.ToDouble(&v); m_xAxis->ChangeValue   (wxString::Format("%ld", (long)(v * THE_CONFIG->getCalculationFactX())));
		yAppPos.ToDouble(&v); m_yAxis->ChangeValue   (wxString::Format("%ld", (long)(v * THE_CONFIG->getCalculationFactY())));
		zAppPos.ToDouble(&v); m_zAxis->ChangeValue   (wxString::Format("%ld", (long)(v * THE_CONFIG->getCalculationFactZ())));
		xCtlPos.ToDouble(&v); m_xAxisCtl->ChangeValue(wxString::Format("%ld", (long)(v * THE_CONFIG->getCalculationFactX())));
		yCtlPos.ToDouble(&v); m_yAxisCtl->ChangeValue(wxString::Format("%ld", (long)(v * THE_CONFIG->getCalculationFactY())));
		zCtlPos.ToDouble(&v); m_zAxisCtl->ChangeValue(wxString::Format("%ld", (long)(v * THE_CONFIG->getCalculationFactZ())));
	}
	
	cncManuallyMoveCoordPanel->updateUnit();
	cncLCDPositionPanel->updateUnit();
}
///////////////////////////////////////////////////////////////////
void MainFrame::OnExit(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxUnusedVar(event);
	Close();
}
///////////////////////////////////////////////////////////////////
void MainFrame::OnAbout(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString description;
	description << "Here you find a simple fun project . . .\n\n";
	description << "It's a combination of:\n";
	description << " - Programming,\n";
	description << " - Build electronics and\n";
	description << " - Mechanical work\n";
	
	wxIcon icon;
	wxBitmap bmp = ImageLibBig().Bitmap("BMP_CNC");
	icon.CopyFromBitmap(bmp);
	
	wxUnusedVar(event);
	wxAboutDialogInfo info;
	info.SetName(globalStrings.programTitel);
	info.SetVersion(globalStrings.programVersionOnly);
	info.SetLicence(_("GPL v2 or later"));
	info.AddDeveloper("Hacki Wimmmer");
	info.SetDescription(description);
	info.SetIcon(icon);
	info.SetCopyright(globalStrings.copyRight);
	::wxAboutBox(info);
}
///////////////////////////////////////////////////////////////////
bool MainFrame::disconnectSerialPort() {
///////////////////////////////////////////////////////////////////
	bool ret = true;
	
	if ( cnc && cnc->isConnected() )
	{
		ret = cnc->disconnect();
		
		m_portSelector->SetSelection(wxNOT_FOUND);
		lastPortName.clear();
		secureCtrlPanel->notifyConnection(false, "");
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectPort(wxCommandEvent& event) {
//////////////////////////////////////////////////
	selectPort();
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectPort() {
///////////////////////////////////////////////////////////////////
	if ( m_portSelector->GetStringSelection().IsSameAs(_portSeparator) ) {
		m_portSelector->SetStringSelection(lastPortName);
		return;
	}
	
	if ( lastPortName != m_portSelector->GetStringSelection() )
		connectSerialPortDialog();
}
///////////////////////////////////////////////////////////////////
bool MainFrame::connectSerialPortDialog() {
///////////////////////////////////////////////////////////////////
	CncConnectProgress dlg(this);
	dlg.GetPortName()->SetLabel(m_portSelector->GetStringSelection());
	
	return ( dlg.ShowModal() == wxID_OK);
}
///////////////////////////////////////////////////////////////////
bool MainFrame::connectSerialPort() {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	wxASSERT( cncManuallyMoveCoordPanel );
	
	CncGampadDeactivator cgd(this);
	
	const wxBitmap bmpC = ImageLib16().Bitmap("BMP_CONNECTED");
	const wxBitmap bmpD = ImageLib16().Bitmap("BMP_DISCONNECTED");
	m_connect->SetBitmap(bmpD);
	m_connect->Refresh();
	m_connect->Update();
	
	mainViewInfobar->Dismiss();
	monitorViewInfobar->Dismiss();
	
	CncRunAnimationControl rac(this);
	stopSerialTimer();
	
	THE_BOUNDS->resetHardwareOffset();
	updateHardwareReference();
	
	const wxString sel(m_portSelector->GetStringSelection());
	if ( sel.IsEmpty() ) 
		return false;
	
	disableControls();
	decorateOutboundSaveControls(false);
	decorateOutboundEditor();
	
	m_miRqtIdleMessages->Check(false);
	m_miRqtIdleMessages->Enable(false);
	
	wxString serialFileName;
	createCncControl(sel, serialFileName);
	
	if ( cnc == NULL )
	{
		CNC_CERR_FUNCT_A(": Can't create a new cnc controller")
		return false;
	}
	
	// initialize the position controls
	setControllerZero(CncRM_Mode5, 0.0, 0.0, 0.0);
	
	statisticsPane->setCncControl(cnc);
	serialSpyPanel->initDuringConnect();

	clearPositionSpy();
	lastPortName.clear();
	
	bool ret = false;
	secureCtrlPanel->notifyConnection(false, "");
	
	if ( (ret = cnc->connect(serialFileName)) == true ) 
	{
		lastPortName.assign(sel);
		clearMotionMonitor();
		clearPositionSpy();
		
		if ( (ret = cnc->setup()) == true )
		{
			setReferencePosEnforceFlag(cnc->isEmulator() == false);
			
			decorateSpindleState(cnc->getSpindlePowerState());
			
			m_connect->SetBitmap(bmpC);
			startSerialTimer();
			
			secureCtrlPanel->notifyConnection(true, lastPortName);
		}
	}
	
	cncManuallyMoveCoordPanel->reset();
	updateSetterList();
	decoratePortSelector();
	decorateGamepadState(false);
	
	defaultSpeedSlider->autoConfigure();
	
	m_connect->Refresh();
	m_connect->Update();

	if ( cncOsEnvDialog && cncOsEnvDialog->IsShownOnScreen() == true )
		cncOsEnvDialog->update();

	enableControls();
	
	// this has to be done after enableControls(), otherwise the state can
	// no be reproduced after the next disableControls()!
	m_rcDryRun->Enable(cnc->dryRunAvailable());
	secureCtrlPanel->GetBtDryRunSec()->Enable(cnc->dryRunAvailable());
	secureCtrlPanel->GetBtTemplateContextSec()->Enable(cnc->dryRunAvailable());
	
	if ( cnc->isEmulator() == true )
		simulateHardwareReference(true);
	
	if ( ret == true && cnc->canProcessIdle() )
	{
		m_miRqtIdleMessages->Check(THE_CONFIG->getRequestIdleRequestFlag());
		m_miRqtIdleMessages->Enable(true);
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
const wxString& MainFrame::createCncControl(const wxString& sel, wxString& serialFileName) {
///////////////////////////////////////////////////////////////////
	// disconnect and delete the current cnc control
	if ( cnc != NULL ) 
	{
		cnc->disconnect();
		delete cnc;
	}
	
	serialFileName.clear();
	cnc = NULL;
	
	struct InitialSetup 
	{
		wxString serialFileName		= "";
		wxString description		= "";
		bool probeMode				= false;
		bool interactiveMoving		= false;
		bool secureDlg				= false;
		bool speedMonitor			= false;
		bool hasHardware			= false;
		bool pathListEntries		= THE_CONFIG->getPreProcessorCntPathListEntries()	&& THE_CONTEXT->secureModeInfo.isActivatedByStartup == false;
		bool moveSequences			= THE_CONFIG->getPreProcessorCntMoveSequneces()		&& THE_CONTEXT->secureModeInfo.isActivatedByStartup == false;
		bool operatingTrace			= THE_CONFIG->getPreProcessorUseOperatingTrace()	&& THE_CONTEXT->secureModeInfo.isActivatedByStartup == false;
		bool vertexTrace			= true												&& THE_CONTEXT->secureModeInfo.isActivatedByStartup == false;

	} setup;
	
	if ( sel == _portEmulatorNULL || sel.IsEmpty() ) {
		cnc = new CncControl(CncEMU_NULL);
		
		setup.serialFileName.assign("dev/null");
		setup.description.assign("This port represents a simple emulator. It simulates the controller interface as best it can and discards all CNC output to dev/null.");
		setup.probeMode			= true;
		setup.interactiveMoving	= true;
		setup.secureDlg			= false;
		setup.speedMonitor		= false;
		setup.hasHardware		= false;
	} 
	else if ( sel == _portPreProcFile ) {
		cnc = new CncControl(CncEMU_NULL);
		
		setup.serialFileName.assign("PreprocessorOnly/File");
		setup.description.assign("Illustrates only the preprocessor output without involving the CNC controller and store it file based.");
		setup.probeMode			= true;
		setup.interactiveMoving	= true;
		setup.secureDlg			= false;
		setup.speedMonitor		= false;
		setup.hasHardware		= false;
	} 
	else if ( sel == _portPreProcMonitor ) {
		cnc = new CncControl(CncEMU_NULL);
		
		setup.serialFileName.assign("PreprocessorOnly/Monitor/");
		setup.description.assign("Illustrates only the preprocessor output without involving the CNC controller.");
		setup.probeMode			= true;
		setup.interactiveMoving	= true;
		setup.secureDlg			= false;
		setup.speedMonitor		= false;
		setup.hasHardware		= false;
	} 
	else if ( sel == _portEmulatorTEXT ) {
		cnc = new CncControl(CncEMU_TXT);
		
		setup.serialFileName.assign(CncFileNameService::getCncOutboundTxtFileName());
		setup.description.assign("This port represents a simple emulator. It simulates the controller interface as best it can and converts all CNC output to a text format.");
		setup.probeMode			= true;
		setup.interactiveMoving	= true;
		setup.secureDlg			= false;
		setup.speedMonitor		= false;
		setup.hasHardware		= false;
	} 
	else if ( sel == _portEmulatorSVG ) {
		cnc = new CncControl(CncEMU_SVG);
		
		setup.serialFileName.assign(CncFileNameService::getCncOutboundSvgFileName());
		setup.description.assign("This port represents a simple emulator. It simulates the controller interface as best it can and converts all CNC output to SVG format.");
		setup.probeMode			= true;
		setup.interactiveMoving	= true;
		setup.secureDlg			= false;
		setup.speedMonitor		= false;
		setup.hasHardware		= false;
	}
	else if ( sel == _portEmulatorGCODE ) {
		cnc = new CncControl(CncEMU_GCODE);
		
		setup.serialFileName.assign(CncFileNameService::getCncOutboundGCodeFileName());
		setup.description.assign("This port represents a simple emulator. It simulates the controller interface as best it can and converts all CNC output to GCode format.");
		setup.probeMode			= true;
		setup.interactiveMoving	= true;
		setup.secureDlg			= false;
		setup.speedMonitor		= false;
		setup.hasHardware		= false;
	} 
	else if ( sel == _portEmulatorBIN) {
		cnc = new CncControl(CncEMU_BIN);
		
		setup.serialFileName.assign(CncFileNameService::getCncOutboundBinFileName());
		setup.description.assign("This port represents a simple emulator. It simulates the controller interface as best it can and converts all CNC output to a binary format.");
		setup.probeMode			= true;
		setup.interactiveMoving	= true;
		setup.secureDlg			= false;
		setup.speedMonitor		= false;
		setup.hasHardware		= false;
	} 
	else if ( sel == _portEmulatorArduino) {
		cnc = new CncControl(CncPORT_EMU_ARDUINO);
		
		setup.serialFileName.assign("::Arduino");
		setup.description.assign("This port represents a complex simulator. It runs the Arduino Sketch in a separate thread of this application without any CNC hardware.");
		setup.probeMode			= true;
		setup.interactiveMoving	= true;
		setup.secureDlg			= false;
		setup.speedMonitor		= true;
		setup.hasHardware		= true;
	} 
	else {
		cnc = new CncControl(CncPORT);
		
		setup.serialFileName.assign(wxString::Format("\\\\.\\%s", sel.BeforeFirst(' ')));
		setup.description.assign("This port stands for a specific hardware USB Arduino stepper environment connection.");
		setup.probeMode			= false;
		setup.interactiveMoving	= true;
		setup.secureDlg			= false;
		setup.speedMonitor		= true;
		setup.hasHardware		= true;
	}
	
	const bool startDisabled = false;
	if ( startDisabled == true ) 
	{
		cnc::cex1 << "MainFrame::createCncControl(): Flag startDisabled is active!" << std::endl;
		setup.pathListEntries	= false;
		setup.moveSequences		= false;
		setup.operatingTrace	= false;
		setup.vertexTrace		= false;
		
		decoratePosSpyConnectButton(false);
		m_menuItemUpdCoors->Check(false);
	}
	
	m_portSelector->SetToolTip(setup.description);

	if ( inboundFileParser != NULL )
		inboundFileParser->changePathListRunnerInterface(sel);
	
	// configuration setup
	serialFileName.assign(setup.serialFileName);
	THE_CONTEXT->setInteractiveMoveingMode(setup.interactiveMoving);
	THE_CONTEXT->setProbeMode(setup.probeMode);
	THE_CONTEXT->setSpeedMonitoring(setup.speedMonitor);
	THE_CONTEXT->setHardwareFlag(setup.hasHardware);
	
	speedMonitor->clear();
	speedMonitor->activate(true);
	
	decorateSecureDlgChoice(setup.secureDlg);
	
	if ( cncPreprocessor != NULL ) 
	{
		cncPreprocessor->enablePathListEntries(setup.pathListEntries);
		cncPreprocessor->enableMoveSequences(setup.moveSequences);
		cncPreprocessor->enableOperatingTrace(setup.operatingTrace);
	}
	
	return serialFileName;
}
///////////////////////////////////////////////////////////////////
void MainFrame::decorateOutboundEditor(const char* fileName) {
///////////////////////////////////////////////////////////////////
	wxString url(wxString::Format("%s%s", CncFileNameService::getDatabaseDir(), "NoSerialContentAvailable.svg"));
	if ( wxFileName::Exists(url) == false )
		url.assign("about:blank");

	if ( fileName == NULL )
	{
		outboundEditorSvgView->loadFile(url);
		m_simpleBookOutBoundEditor->SetSelection(1);
		
		outboundEditor->clearContent();
		outboundFilePreview->selectEmptyPreview();
		
		return;
	}
	
	if ( wxFileName::Exists(fileName) == false )
	{
		outboundEditorSvgView->loadFile(url);
		m_simpleBookOutBoundEditor->SetSelection(1);
		
		std::cerr << "MainFrame::decorateOutboundEditor(): Can't open file: '" 
		          << fileName 
		          << "'" << std::endl;
		return;
	}
	
	m_simpleBookOutBoundEditor->SetSelection(0);
	outboundEditor->openFile(fileName);
	outboundFilePreview->selectPreview(fileName);
}
///////////////////////////////////////////////////////////////////
void MainFrame::enableMenu(wxMenu* m, bool state) {
///////////////////////////////////////////////////////////////////	
	if ( m )
		m_menuBar->EnableTop(m_menuBar->FindMenu(m->GetTitle()), state);
}
///////////////////////////////////////////////////////////////////
void MainFrame::enableMenuItem(wxMenuItem* m, bool state) {
///////////////////////////////////////////////////////////////////
	if ( m ) {
		for (unsigned int i=0; i<m_menuBar->GetMenuCount();i++) {
			wxMenu* menu = m_menuBar->GetMenu(i);
			
			if ( menu->FindItem(m->GetItemLabel()) ) {
				menu->Enable(menu->FindItem(m->GetItemLabel()), state);
				break;
			}
		}
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::enableRunControls(bool state) {
// state == false: processing active
// state == true:  processing inacative
// - triggered by enableControls(state)
///////////////////////////////////////////////////////////////////
	// only for debugging
	if ( false ) {
		std::clog << "state: " 			<< state;
		std::clog << ", debug: " 		<< isDebugMode;
		std::clog << ", pause: " 		<< isPause();
		std::clog << std::endl;
	}
	
	wxASSERT(secureCtrlPanel != NULL);
	
	// all buttons of the run control have to be enabled/disabeled here
	// every time
	
	if ( isPause() ) {
		m_rcDebugConfig->Enable(false);
	}
	else {
		m_rcRun->Enable(state);
		secureCtrlPanel->GetRcRunSec()->Enable(state);
	}
	
	if ( isDebugMode == false ) {
		
		isPause() ? m_rcRun->Enable(true)									: m_rcRun->Enable(state); 
		isPause() ? secureCtrlPanel->GetRcRunSec()->Enable(true)			: secureCtrlPanel->GetRcRunSec()->Enable(state);
		isPause() ? m_rcDebug->Enable(false)								: m_rcDebug->Enable(state);
		isPause() ? m_rcPause->Enable(false)								: m_rcPause->Enable(!state);
		isPause() ? secureCtrlPanel->GetRcPauseSec()->Enable(false)			: secureCtrlPanel->GetRcPauseSec()->Enable(!state);
		isPause() ? m_rcStop->Enable(true)									: m_rcStop->Enable(!state);
		isPause() ? secureCtrlPanel->GetRcStopSec()->Enable(true)			: secureCtrlPanel->GetRcStopSec()->Enable(!state);
		isPause() ? m_btnEmergenyStop->Enable(true)							: m_btnEmergenyStop->Enable(!state);
		isPause() ? secureCtrlPanel->GetBtnEmergenyStopSec()->Enable(true)	: secureCtrlPanel->GetBtnEmergenyStopSec()->Enable(!state);
		
		m_rcReset->Enable(isPause() == false && state);
		m_rcNextStep->Enable(false);
		m_rcNextBreakpoint->Enable(false);
		m_rcFinish->Enable(false);
		secureCtrlPanel->lockSelection(state == false);
		
	} else {
		
		isPause() ? m_rcRun->Enable(false)          : m_rcRun->Enable(state);
		isPause() ? m_rcDebug->Enable(true)         : m_rcDebug->Enable(state);
		isPause() ? m_rcPause->Enable(false)        : m_rcPause->Enable(!state);
		isPause() ? m_rcStop->Enable(true)          : m_rcStop->Enable(!state);
		isPause() ? m_btnEmergenyStop->Enable(true) : m_btnEmergenyStop->Enable(!state);
				
		m_rcReset->Enable(isPause() == false && state);
		
		m_rcNextStep->Enable(!state);
		m_rcNextBreakpoint->Enable(!state);
		m_rcFinish->Enable(!state);
		
		if ( m_rcNextBreakpoint->IsEnabled() )	startDebugUserNotification();
		else									stopDebugUserNotification();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::enableControls(bool state) {
///////////////////////////////////////////////////////////////////
	// set global state
	canClose = state;
	
	m_secureSplitterMainV->SetSashInvisible(!state);
	m_secureSplitterMainH->SetSashInvisible(!state);
	m_secureSplitterLoggerV->SetSashInvisible(!state);
	
	// enable menu bar
	enableMenuItems(state);
	
	// enable all relevant controls
	enableGuiControls(state);
	
	//enable manually controls
	enableTestControls(state);
	
	// run control
	enableRunControls(state);
	
	drawPane3D->enable(state);
	statisticsPane->enable(state);
	
	// special secure mode handling
	const bool sState = state == true ? true : THE_CONTEXT->secureModeInfo.isActive == true ? false : true;
	getLoggerView()->Enable(sState);
	speedMonitor->activate(sState);
}
///////////////////////////////////////////////////////////////////
void MainFrame::connect(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	connectSerialPortDialog();
}
///////////////////////////////////////////////////////////////////
void MainFrame::setControllerZero(CncRefPositionMode m, double x, double y, double z) {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	wxASSERT( refPositionDlg );
	
	CNC_TRANSACTION_LOCK
	
	// log the current position before the zero operation below applies
	const CncLongPosition prevPos = cnc->requestControllerPos();
	
	cnc->resetClientId();
	
	if ( cnc::dblCmp::gt(x, cnc::dbl::MIN) ) cnc->setZeroPosX(THE_CONFIG->convertMetricToStepsX(x));
	if ( cnc::dblCmp::gt(y, cnc::dbl::MIN) ) cnc->setZeroPosY(THE_CONFIG->convertMetricToStepsY(y));
	if ( cnc::dblCmp::gt(z, cnc::dbl::MIN) ) cnc->setZeroPosZ(THE_CONFIG->convertMetricToStepsZ(z));
	
	resetReferencePosTempSetting();
	setReferencePosEnforceFlag(false);
	
	// align the hardware offset ith the new logical software origin
	if ( THE_BOUNDS->getHardwareOffset().isValid() == true )
	{
		CncLongPosition offset(THE_BOUNDS->getHardwareOffset().getAsSteps());
		offset -= prevPos;
		THE_BOUNDS->setHardwareOffset(offset);
		
		cnc::trc.logInfoMessage("The hardware offset was realigned to the new origin . . .");
	}
}
/////////////////////////////////////////////////////////////////////
bool MainFrame::resetPodiumDistance() {
/////////////////////////////////////////////////////////////////////
	if ( cnc->resetPodiumDistance() == false ) 
	{
		std::cerr << CNC_LOG_FUNCT_A(": Can't reset podium position\n");
		return false;
	}
	
	return true;
}
/////////////////////////////////////////////////////////////////////
bool MainFrame::applyPodiumDistance() {
/////////////////////////////////////////////////////////////////////
	CNC_TRANSACTION_LOCK_RET_ON_ERROR(false);
	
	CncIdleCheckDeactivator icd(this);
	
	const double dbl = cnc->getPodiumDistanceMetric() * (-1);
	
	// log the current position before the correction operation below applies
	const CncLongPosition prevPos = cnc->requestControllerPos();
	
	// correct the z position by the podium movement
	cnc->setZeroPosZ(prevPos.getZ() + THE_CONFIG->convertMetricToStepsZ(dbl) );
	
	// align the hardware offset with the new logical software origin
	if ( THE_BOUNDS->getHardwareOffset().isValid() == true )
	{
		CncLongPosition offset(THE_BOUNDS->getHardwareOffset().getAsSteps());
		offset -= prevPos;
		THE_BOUNDS->setHardwareOffset(offset);
		
		cnc::trc.logInfoMessage("The hardware offset was realigned to the new origin . . .");
	}
	
	return true;
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onPodiumManagement(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( podiumManagementDlg == NULL )
		return;
		
	if ( resetPodiumDistance() == false ) 
		return;
	
	const int filter = CncGamepadFilterInstance::FILTER_QUICK_MENU_ACTIVATION | CncGamepadFilterInstance::FILTER_CNC_ACTIVATION;
	CncGamepadFilter gf(THE_CONTEXT->gamepadFilterInstance, filter);
	
	CncIdleCheckDeactivator icd(this);
	
	enableControls(false);
	
	podiumManagementDlg->ShowModal();
	waitActive(500);
	
	applyPodiumDistance();
	
	enableControls(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectUnit(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	updateUnit();
}
///////////////////////////////////////////////////////////////////
int MainFrame::showReferencePositionDlg(wxString msg) {
///////////////////////////////////////////////////////////////////
	wxASSERT ( secureCtrlPanel );
	wxASSERT ( refPositionDlg );
	
	CncUsbConnectionObserver::Deactivator noUsbPopup(usbConnectionObserver);
	
	activateGamepadNotifications(true);
	CncGamepadSpy::ContextSwaper gcs(gamepadSpy, CncGamepadSpy::GPC_REFPOS);
	
	int ret = wxID_CANCEL;
	if ( THE_CONTEXT->secureModeInfo.isActive == true )
	{
		secureCtrlPanel->getReferencePanel()->setMessage(msg);
		ret = secureCtrlPanel->aktivateReferencePanel();
	}
	else
	{
		refPositionDlg->setMessage(msg);
		ret = refPositionDlg->ShowModal();
		if ( ret == wxID_OK )
		{
			RefPosResult parameter;
			getReferencePosResult(parameter);
			updateReferencePosition(&parameter);
		} 
	}

	if ( ret != wxID_OK )
		cnc::cex1 << " Set reference position aborted . . . " << std::endl;
	
	//selectMonitorBookCncPanel();
	return ret;
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateReferencePosition(RefPosResult* parameter) {
///////////////////////////////////////////////////////////////////
	wxASSERT ( parameter != NULL );
	
	THE_BOUNDS->setWorkpieceThickness(parameter->workpieceThickness);
	THE_BOUNDS->setMeasurementOffset (parameter->measurementOffset);
	THE_BOUNDS->setRefPositionMode   (parameter->refMode);
	
	motionMonitor->clear();
	
	setControllerZero(parameter->refMode,
					  parameter->zeroX ? THE_BOUNDS->getCalculatedRefPositionMetric().getX() : cnc::dbl::MIN, 
					  parameter->zeroY ? THE_BOUNDS->getCalculatedRefPositionMetric().getY() : cnc::dbl::MIN, 
					  parameter->zeroZ ? THE_BOUNDS->getCalculatedRefPositionMetric().getZ() : cnc::dbl::MIN 
					 );
						 
	motionMonitor->Refresh();
}
///////////////////////////////////////////////////////////////////
void MainFrame::changeCrossingThickness() {
///////////////////////////////////////////////////////////////////
	if ( cnc != NULL )
		m_crossings->ChangeValue(wxString::Format("%d", cnc->getDurationCount()));
}
///////////////////////////////////////////////////////////////////
void MainFrame::defineMinMonitoring(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	defineMinMonitoring();
}
///////////////////////////////////////////////////////////////////
void MainFrame::defineMinMonitoring() {
///////////////////////////////////////////////////////////////////
	m_menuItemUpdCoors->Check(false);
	m_menuItemUpdDraw->Check(false);
	m_menuItemDisplayParserDetails->Check(false);
	switchMonitorButton(false);
	updateMonitoring();
}
///////////////////////////////////////////////////////////////////
void MainFrame::defineNormalMonitoring(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	defineNormalMonitoring();
}
///////////////////////////////////////////////////////////////////
void MainFrame::defineNormalMonitoring() {
///////////////////////////////////////////////////////////////////
	m_menuItemUpdCoors->Check(true);
	m_menuItemUpdDraw->Check(true);
	m_menuItemDisplayParserDetails->Check(true);
	switchMonitorButton(true);
	updateMonitoring();
}
///////////////////////////////////////////////////////////////////
void MainFrame::switchMonitoring(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_switchMonitoing->GetValue() == true )
		defineNormalMonitoring();
	else
		defineMinMonitoring();
}
///////////////////////////////////////////////////////////////////
void MainFrame::switchMonitorButton(bool state) {
///////////////////////////////////////////////////////////////////
	m_switchMonitoing->SetValue(state);
	
	if ( state == true ) {
		m_switchMonitoing->SetBitmap(ImageLib16().Bitmap("BMP_MONITORING_NORMAL"));
		m_switchMonitoing->SetToolTip("Switch Monitoring to Minimal");
	} else {
		m_switchMonitoing->SetBitmap(ImageLib16().Bitmap("BMP_MONITORING_MINIMAL"));
		m_switchMonitoing->SetToolTip("Switch Monitoring to Normal");
	}
	m_switchMonitoing->Refresh();
}
///////////////////////////////////////////////////////////////////
void MainFrame::defineUpdateCoordinates(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	updateMonitoring();
}
///////////////////////////////////////////////////////////////////
void MainFrame::defineAllowEvents(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	updateMonitoring();
}
///////////////////////////////////////////////////////////////////
void MainFrame::defineOnlineDrawing(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	updateMonitoring();
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateMonitoring() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	if ( motionMonitor != NULL ) {
		motionMonitor->enable(m_menuItemUpdDraw->IsChecked());
		motionMonitor->Refresh();
	}
	
	THE_CONTEXT->setOnlineUpdateCoordinates(m_menuItemUpdCoors->IsChecked());
	THE_CONTEXT->setAllowEventHandling(m_menuItemAllowEvents->IsChecked());
	THE_CONTEXT->setOnlineUpdateDrawPane(m_menuItemUpdDraw->IsChecked());
	
	if ( isDisplayParserDetails() == false )
		clearTemplateEditorExtViews();
}
///////////////////////////////////////////////////////////////////
void MainFrame::clearTemplateEditorExtViews() {
///////////////////////////////////////////////////////////////////
	m_svgRootNode->Clear();
	
	m_dvListCtrlSvgUAInboundPathList->DeleteAllItems();
	m_dvListCtrlSvgUAUseDirective->DeleteAllItems();
	m_dvListCtrlSvgUADetailInfo->DeleteAllItems();
	
	m_dvListCtrlSvgUAInboundPathList->Update();
	m_dvListCtrlSvgUAUseDirective->Update();
	m_dvListCtrlSvgUADetailInfo->Update();
	
	getGCodeSequenceList()->clearAll();
}
///////////////////////////////////////////////////////////////////
const char* MainFrame::getTemplateFormatName(const wxString& fileName) {
///////////////////////////////////////////////////////////////////
	static wxString ext;
	
	wxFileName fn;
	fn.Assign(fileName);
	
	ext.assign(fn.GetExt());
	ext.MakeUpper();
	return ext;
}
///////////////////////////////////////////////////////////////////
const char* MainFrame::getCurrentTemplateFormatName() {
///////////////////////////////////////////////////////////////////
	switch ( getCurrentTemplateFormat() ) {
		case TplManual:
		case TplTest:	return "";
		default: 		return getTemplateFormatName(getCurrentTemplatePathFileName());
	}
	
	return "";
}
///////////////////////////////////////////////////////////////////
CncTemplateFormat MainFrame::getTemplateFormat(const wxString& fn) {
///////////////////////////////////////////////////////////////////
	return cnc::getTemplateFormatFromFileName(fn);
}
///////////////////////////////////////////////////////////////////
CncTemplateFormat MainFrame::getCurrentTemplateFormat() {
///////////////////////////////////////////////////////////////////
	switch ( m_mainViewBook->GetSelection() ) {
		case MainBookSelection::VAL::MANUEL_PANEL:	return TplManual;
		case MainBookSelection::VAL::TEST_PANEL:	return TplTest;
	}
	
	const wxString fn(getCurrentTemplatePathFileName());
	return cnc::getTemplateFormatFromFileName(fn);
}
///////////////////////////////////////////////////////////////////
const wxString& MainFrame::getCurrentTemplateFileName() {
///////////////////////////////////////////////////////////////////
	static wxString ret;
	ret.assign(m_inputFileName->GetValue());
	return ret;
}
///////////////////////////////////////////////////////////////////
const wxString& MainFrame::getCurrentTemplatePathFileName() {
///////////////////////////////////////////////////////////////////
	static wxString ret;
	ret.assign(m_inputFileName->GetHint());
	return ret;
}
///////////////////////////////////////////////////////////////////
void MainFrame::decorateExtTemplatePages(CncTemplateFormat tf) {
///////////////////////////////////////////////////////////////////
	switch ( tf ) {
		case TplSvg:		m_simpleBookSourceExt->SetSelection(SourceExtBookSelection::VAL::USER_AGENT_PANEL);
							break;
							
		case TplBinary:		m_simpleBookSourceExt->SetSelection(SourceExtBookSelection::VAL::NESTED_INFO_PANEL);
							break;
							
		case TplGcode:		m_simpleBookSourceExt->SetSelection(SourceExtBookSelection::VAL::GCODE_SEQUENCE_PANEL);
							break;
							
		default:			m_simpleBookSourceExt->SetSelection(SourceExtBookSelection::VAL::NULL_PANEL);
	}
	
	wxImageList* soureImgList = m_simpleBookSourceExt->GetImageList();
	wxASSERT(soureImgList);
	wxIcon   ico(soureImgList->GetIcon(m_simpleBookSourceExt->GetSelection()));
	wxString txt(m_simpleBookSourceExt->GetPageText(m_simpleBookSourceExt->GetSelection()));
	
	wxImageList* destImgList  = m_templateNotebook->GetImageList();
	wxASSERT(destImgList);
	destImgList->Replace(TemplateBookSelection::VAL::EXT_INFO_PANEL, ico);
	
	m_templateNotebook->SetPageImage(TemplateBookSelection::VAL::EXT_INFO_PANEL, TemplateBookSelection::VAL::EXT_INFO_PANEL);
	m_templateNotebook->SetPageText(TemplateBookSelection::VAL::EXT_INFO_PANEL,  txt);
}
///////////////////////////////////////////////////////////////////
void MainFrame::fillFileDetails(bool fileLoaded, const char* extFileName) {
///////////////////////////////////////////////////////////////////
	wxString details;
	
	if ( fileLoaded == true ) {
		bool evaluate = true;
		
		wxString fileName(getCurrentTemplatePathFileName());
		if ( extFileName != NULL ) {
			if ( wxFileName::Exists(extFileName) == true )	fileName.assign(extFileName);
			else											evaluate = false;
		}
		
		if ( evaluate == true ) {
			
			wxFileName file(fileName);
			details.append(wxString::Format("%s\n", file.GetModificationTime().FormatISOCombined(' ')));
			details.append(wxString::Format("%s",   file.GetHumanReadableSize()));
		}
	}
	
	m_filePreviewDetails->ChangeValue(details);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectEditorToolBox(bool fileLoaded) {
///////////////////////////////////////////////////////////////////
	wxFlexGridSizer* sizer = static_cast<wxFlexGridSizer*>(m_editorToolBox->GetContainingSizer());
	if ( sizer == NULL )
		return;

	bool show = true;
	if ( fileLoaded == false ) {
		show = false;
		
	} else {
		switch ( getCurrentTemplateFormat() ) {
			case TplSvg:		m_editorToolBox->SetSelection(0);
								show = true;
								break;
								
			case TplGcode:		m_editorToolBox->SetSelection(1);
								show = false;
								break;
								
			case TplBinary:		m_editorToolBox->SetSelection(2);
								show = true;
								break;
								
			default:			show = false;
		}
	}
	
	m_editorToolBox->Show(show);
	sizer->Layout();
}
///////////////////////////////////////////////////////////////////
bool MainFrame::openFile(int sourcePageToSelect) {
///////////////////////////////////////////////////////////////////
	// First select the template page to get the right result 
	// from getCurrentTemplateFormat()
	selectMainBookSourcePanel(sourcePageToSelect);
	
	// evaluate if template file was changed
	wxFileName fn(getCurrentTemplatePathFileName());
	if ( fn == lastTemplateFileName )
	{
		cnc::trc.logInfo("Current template wasn't changed. Use the 'Reload' function to force an opening again.");
		return true;
	}
	
	// assign last template file name
	lastTemplateFileName.Assign(getCurrentTemplatePathFileName());
	
	THE_TPL_CTX->reset();
	setAutoSaveMode(THE_TPL_CTX->isAutoSaveMode());

	decorateDryRunState(cncUnknown);
	
	bool ret = false;
	switch ( getCurrentTemplateFormat() ) 
	{
		
		case TplSvg:	ret = sourceEditor->openFile(getCurrentTemplatePathFileName());
						break;
		
		case TplGcode:	ret = sourceEditor->openFile(getCurrentTemplatePathFileName());
						break;
						
		case TplBinary:	ret = sourceEditor->openFile(getCurrentTemplatePathFileName());
						break;
						
		default:		std::cerr << "MainFrame::openFile(): Unknown Type: " << getCurrentTemplateFormat() << std::endl;
						ret = false;
	}
	
	selectEditorToolBox(ret);
	fillFileDetails(ret);
	
	if ( ret == true ) 
	{
		decorateExtTemplatePages(getCurrentTemplateFormat());
		outboundFilePreview->selectEmptyPreview();
		
		if ( inboundFileParser != NULL )
			inboundFileParser->clearControls();
		
		clearMotionMonitor();
		
		introduceCurrentFile(sourcePageToSelect);

		THE_TPL_CTX->init(getCurrentTemplatePathFileName());
		
		ADD_LOGGER_SEPERATOR
		std::clog << "Template: '" << THE_TPL_CTX->getName() << "' successfully loaded" << std::endl;
		SET_RESULT_FOR_LAST_FILLED_LOGGER_ROW_OK
	}
	
	updateFileContentPosition(0, 0);
	return ret;
}
///////////////////////////////////////////////////////////////////
void MainFrame::prepareMotionMonitorViewType() {
///////////////////////////////////////////////////////////////////
	const GLContextBase::ModelType mt = sourceEditor->getModelType();
	motionMonitor->setModelType(mt);

	switch ( sourceEditor->getModelViewType() ) {
		case CncDimensions::CncDimension2D:		motionMonitor->viewTop();
												break;
												
		default:								motionMonitor->viewIso1();
	}
	
	motionMonitor->Refresh();
}
///////////////////////////////////////////////////////////////////
void MainFrame::introduceCurrentFile(int sourcePageToSelect) {
///////////////////////////////////////////////////////////////////
	MF_PRINT_LOCATION_CTX_FILE
	
	lruFileView->addFile(getCurrentTemplatePathFileName());
	fileView->selectFileInList(getCurrentTemplatePathFileName());
	
	selectMainBookSourcePanel(sourcePageToSelect);
	
	// publish model type
	prepareMotionMonitorViewType();
	
	// user agent & co
	clearTemplateEditorExtViews();
}
///////////////////////////////////////////////////////////////////
void MainFrame::newTemplate(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( saveTemplateOnDemand() == false )
		return;
		
		
	wxString defaultTplDir;
	THE_CONFIG->getDefaultTplDir(defaultTplDir);
	CncFileNameService::ensureEndWithPathSep(defaultTplDir);
	
	defaultTplDir.append("NewCncTemplate");
	
	wxFileDialog newFileDialog(this, 
								"New Template File", 
								wxEmptyString,
								defaultTplDir, 
								"SVG Files (*.svg)|*.svg|GCode Files (*.ngc;*.gcode)|*.ngc;*.gcode", 
								wxFD_SAVE/*|wxFD_OVERWRITE_PROMPT*/);
								
	while ( true ) {
		
		if ( newFileDialog.ShowModal() == wxID_CANCEL ) 
			return; 
		
		wxFileName fn(newFileDialog.GetPath());
		if ( fn.Exists() == false )
			break;
		
		const wxString msg(wxString::Format("The file name '%s' already exists. Do you really want to override it?", newFileDialog.GetPath()));
		wxMessageDialog dlg(this, msg, _T("New template . . . "), wxYES|wxNO|wxICON_QUESTION|wxCENTRE);
		
		int ret = dlg.ShowModal();
		if ( ret == wxID_YES )
			break;
	}
	
	// store old template file values
	const wxString ov(getCurrentTemplateFileName());
	const wxString oh(getCurrentTemplatePathFileName());

	// set new template file values
	m_inputFileName->SetValue(newFileDialog.GetFilename());
	m_inputFileName->SetHint(newFileDialog.GetPath());
	
	// prepare editor
	sourceEditor->SetReadOnly(false);
	sourceEditor->clearContent();
	sourceEditor->prepareNewTemplateFile(getCurrentTemplateFormat());
	sourceEditor->setNewTemplateFileName(getCurrentTemplatePathFileName());

	// first save te new template
	if ( saveFile() == false ) 
	{
		m_inputFileName->SetValue(ov);
		m_inputFileName->SetHint(oh);
	} 
	
	// then reopen it . . 
	if ( openFile() == false) 
	{
		m_inputFileName->SetValue(ov);
		m_inputFileName->SetHint(oh);
	} 
	else 
	{
		prepareAndShowMonitorTemplatePreview(true);
	}
	
	introduceCurrentFile();
}
///////////////////////////////////////////////////////////////////
void MainFrame::openTemplate(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	openTemplate();
}
///////////////////////////////////////////////////////////////////
void MainFrame::openTemplate() {
///////////////////////////////////////////////////////////////////
	wxString templateName("..\\Templates\\");
	CncFileDialog openFileDialog(this,
								_("Open Template File"),
								templateName,
								"",
								"SVG Files (*.svg)|*.svg|GCode Files (*.ngc;*.gcode)|*.ngc;*.gcode|Binary Tpl Files (*.bct)|*.bct",
								wxFD_OPEN|wxFD_FILE_MUST_EXIST);

	if ( openFileDialog.ShowModal() == wxID_CANCEL || openFileDialog.GetFilename().IsEmpty() )
		return;

	// store old template file values
	const wxString ov = getCurrentTemplateFileName();
	const wxString oh = getCurrentTemplatePathFileName();
	
	// set new template file values
	m_inputFileName->SetValue(openFileDialog.GetFilename());
	m_inputFileName->SetHint(openFileDialog.GetPath());

	// then open it . . 
	if ( openFile() == false )
	{
		m_inputFileName->SetValue(ov);
		m_inputFileName->SetHint(oh);
	} 
	else 
	{
		prepareAndShowMonitorTemplatePreview(true);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::reloadTemplate(int sourcePageToSelect) {
///////////////////////////////////////////////////////////////////
	if ( sourceEditor->IsModified() )
	{
		wxString msg("The template was locally modified. Do you really want to reload it?\n");
		wxMessageDialog dlg(this, msg, _T("Reload template . . . "), 
		                wxYES|wxNO|wxICON_QUESTION|wxCENTRE);
		
		int ret = dlg.ShowModal();
		if ( ret != wxID_YES )
			return;
	}
	
	// do this otherwise openFile() will return without opening the template
	lastTemplateFileName.Clear();
	const bool ret = openFile(sourcePageToSelect);
	
	if ( ret == true )	prepareAndShowMonitorTemplatePreview(true);
	else				std::cerr	<< "Error while reloading template: " 
									<< getCurrentTemplateFileName().c_str() 
									<< std::endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::reloadTemplate(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	reloadTemplate();
}
///////////////////////////////////////////////////////////////////
void MainFrame::reloadTemplateFromButton(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	reloadTemplate();
}
///////////////////////////////////////////////////////////////////
void MainFrame::removeTemplateFromButton(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString fn (getCurrentTemplatePathFileName());
	wxFileName tplFile(fn);
	
	if ( tplFile.Exists() == false ) {
		std::cerr << "MainFrame::removeTemplateFromButton: Can't find: " << fn << std::endl;
		return;
	}
	
	wxString msg("Do you really want to remove the following template from the disk?\n\nTemplate:\n");
	msg.append(fn);
	wxMessageDialog dlg(this, msg, _T("Remove template . . . "), 
		                wxYES|wxNO|wxCANCEL|wxICON_QUESTION|wxCENTRE);
	
	int ret = dlg.ShowModal();
	if ( ret == wxID_YES )
	{
		if ( wxRemoveFile(fn) == false )
		{
			std::cerr << "MainFrame::removeTemplateFromButton: failed!" << std::endl;
			return;
		}
		
		std::clog << "Template: " << fn << " was removed." << std::endl;
		
		// update file lists
		lruFileView->removeFile(fn);
		lruFileView->save(lruStore);
		fileView->update();
		
		// clear source editor an motion monitor
		sourceEditor->clearContent();
		sourceEditor->DiscardEdits();
		
		clearMotionMonitor();
		m_inputFileName->SetValue("");
		m_inputFileName->SetHint("");
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::renameTemplateFromButton(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString oldFileName (getCurrentTemplatePathFileName());
	wxFileName tplFile(oldFileName);
	
	if ( tplFile.Exists() == false ) {
		std::cerr << "MainFrame::renameTemplateFromButton: Can't find: " << oldFileName << std::endl;
		return;
	}
	
	wxTextEntryDialog dlg(this, "Rename current Template:", "New Name", tplFile.GetFullName());
	dlg.SetMaxLength(256);
	if ( dlg.ShowModal() != wxID_OK )
		return;
	
	// search again - if the dialog was a long time opened the file may be removed otherwise
	if ( tplFile.Exists() == false ) {
		std::cerr << "MainFrame::renameTemplateFromButton: Can't find: " << oldFileName << std::endl;
		return;
	}
	
	// build new file name
	wxString newFileName(tplFile.GetPathWithSep());
	newFileName.append(dlg.GetValue());
	
	wxFileName newTplFile(newFileName);
	if ( newTplFile.Exists() == true ) {
		
		wxString msg("The new template name below already exists. Should it be overwritten?\n\nNew template name:\n");
		msg.append(newFileName);
		wxMessageDialog dlg(this, msg, _T("Template already exists. . . "), 
		                wxYES|wxNO|wxCANCEL|wxICON_QUESTION|wxCENTRE);
		
		if ( dlg.ShowModal() != wxID_YES )
			return;
	}
	
	if ( wxRenameFile(oldFileName, newFileName, true) == false ) {
		std::cerr << "MainFrame::renameTemplateFromButton: wxRenameFile failed: From: " << oldFileName << " to: " << newFileName << std::endl;
		return;
	}
	
	// update file lists
	lruFileView->removeFile(oldFileName);
	lruFileView->addFile(newFileName);
	lruFileView->save(lruStore);
	
	fileView->update();
	
	m_inputFileName->SetValue(newTplFile.GetFullName());
	m_inputFileName->SetHint(newTplFile.GetFullPath());
	
	if ( sourceEditor != NULL )
		sourceEditor->setNewTemplateFileName(getCurrentTemplatePathFileName());
}
///////////////////////////////////////////////////////////////////
void MainFrame::openTemplateSourceExtern(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString externalFile(getCurrentTemplatePathFileName());
	wxString tool; CncConfig::getGlobalCncConfig()->getEditorTool(tool);
	
	if ( getCurrentTemplateFormat() == TplBinary)
		BinaryFileParser::extractSourceContentAsFile(getCurrentTemplatePathFileName(), externalFile);
	else
		saveFile();
	
	openFileExtern(tool, externalFile);
}
///////////////////////////////////////////////////////////////////
void MainFrame::openTemplateExtern(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString tool;
	
	switch ( getCurrentTemplateFormat() ) 
	{
		case TplBinary:
					// binary files are readonly, thereforenow saveFile() necessary here
					THE_CONFIG->getBINFileViewer(tool);
					openFileExtern(tool, getCurrentTemplatePathFileName());
					break;
					
		case TplSvg:
					saveFile(false);
					THE_CONFIG->getSVGFileViewer(tool);
					openFileExtern(tool, getCurrentTemplatePathFileName());
					break;
					
		case TplGcode:
					saveFile(false);
					THE_CONFIG->getGCodeFileViewer(tool);
					openFileExtern(tool, getCurrentTemplatePathFileName());
					break;
					
		default:
					std::clog << "No external editor availiable for current file: " 
							  << getCurrentTemplatePathFileName().c_str() 
							  << std::endl;
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::activateMainWindow(wxActivateEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
bool MainFrame::saveFile(bool interactive) {
///////////////////////////////////////////////////////////////////
	// Deactivate observer
	CncTemplateObserver::Deactivator observerDeactivator(templateObserver);
	
	THE_TPL_CTX->resetValidRuns();
	
	wxASSERT(sourceEditor);
	bool ret = sourceEditor->saveFile();
	
	if ( interactive == true ) {
		
		// First select the template page to get the rigth result 
		// by getCurrentTemplateFormat
		selectMainBookSourcePanel();
		
		if ( ret == true )
			prepareAndShowMonitorTemplatePreview(true);
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool MainFrame::saveFileAs() {
///////////////////////////////////////////////////////////////////
	wxASSERT(m_inputFileName);
	
	wxFileDialog saveFileDialog(this, 
	                            _("Save Template File"), 
								getCurrentTemplatePathFileName(), 
								"",
								"SVG Files (*.svg)|*.svg|GCode Files (*.ngc;*.gcode)|*.ngc;*.gcode",  
								wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
								
	if ( saveFileDialog.ShowModal() == wxID_CANCEL ) 
		return true;
		
	bool ret = sourceEditor->saveFileAs(saveFileDialog.GetPath());
	if ( ret == true ) {
		m_inputFileName->SetValue(saveFileDialog.GetFilename());
		m_inputFileName->SetHint(saveFileDialog.GetPath());
		introduceCurrentFile();
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void MainFrame::saveTemplate(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	saveFile();
}
///////////////////////////////////////////////////////////////////
void MainFrame::saveTemplateFromButton(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	saveFile();
}
///////////////////////////////////////////////////////////////////
void MainFrame::saveTemplateAs(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	saveFileAs();
}
///////////////////////////////////////////////////////////////////
// don't call this method directly, instead use processTemplate()
bool MainFrame::processTemplate_Execute(const SHA1SessionKey& sk) {
///////////////////////////////////////////////////////////////////
	Trigger::ParameterSet ps;
	ps.SRC.fileName		= getCurrentTemplatePathFileName();
	ps.SRC.fileType		= getCurrentTemplateFormatName();
	ps.SET.hardwareResX	= THE_CONFIG->getDisplayFactX();
	ps.SET.hardwareResY	= THE_CONFIG->getDisplayFactY();
	ps.SET.hardwareResZ	= THE_CONFIG->getDisplayFactZ();
	ps.PRC.user			= "Hacki Wimmer";
	
	const Trigger::BeginRun begRun(ps);
	
	wxASSERT( cnc );
	wxASSERT( inboundFileParser );
	
	inboundFileParser->deligateTrigger(begRun);
	inboundFileParser->enableUserAgentControls(isDisplayParserDetails());
	inboundFileParser->setInboundSourceControl(sourceEditor);
	inboundFileParser->initCurrentPos(cnc->getCurAppPosMetric());
	
	bool ret;
	if ( isDebugMode == true ) 	ret = inboundFileParser->processDebug();
	else 						ret = inboundFileParser->processRelease();
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool MainFrame::isGamepadNotificationActive() {
///////////////////////////////////////////////////////////////////
	if ( gamepadThread == NULL )
		return false;
		
	return gamepadThread->IsRunning();
}
///////////////////////////////////////////////////////////////////
void MainFrame::activateGamepadNotificationsOnDemand(bool state) {
///////////////////////////////////////////////////////////////////
	if ( CncGampadDeactivator::gamepadNotificationsAllowed() == false ) {
		activateGamepadNotifications(false);
		return;
	}
	
	activateGamepadNotifications(state);
}
///////////////////////////////////////////////////////////////////
void MainFrame::activateGamepadNotifications(bool state) {
///////////////////////////////////////////////////////////////////
	if ( gamepadThread == NULL )
		return;
		
	if ( state == false )
	{
		if ( gamepadThread->IsRunning() )
			gamepadThread->Pause();
	}
	else
	{
		if ( gamepadThread->IsPaused() )
			gamepadThread->Resume();
	}
	
	decorateGamepadState(gamepadThread->IsRunning());
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processBinaryTemplate(const SHA1SessionKey& sk) {
///////////////////////////////////////////////////////////////////
	if ( inboundFileParser != NULL )
		delete inboundFileParser;
		
	CncGampadDeactivator cpd(this);
		
	inboundFileParser = new BinaryFileParser(getCurrentTemplatePathFileName().c_str(), new BinaryPathHandlerCnc(cnc));
	inboundFileParser->changePathListRunnerInterface(m_portSelector->GetStringSelection());
	
	return processTemplate_Execute(sk);
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processSVGTemplate(const SHA1SessionKey& sk) {
///////////////////////////////////////////////////////////////////
	if ( inboundFileParser != NULL )
		delete inboundFileParser;
		
	CncGampadDeactivator cpd(this);
	
	inboundFileParser = new SVGFileParser(getCurrentTemplatePathFileName().c_str(), cnc);
	inboundFileParser->changePathListRunnerInterface(m_portSelector->GetStringSelection());

	return processTemplate_Execute(sk);
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processGCodeTemplate(const SHA1SessionKey& sk) {
///////////////////////////////////////////////////////////////////
	if ( inboundFileParser != NULL )
		delete inboundFileParser;
		
	CncGampadDeactivator cpd(this);
	
	inboundFileParser = new GCodeFileParser(getCurrentTemplatePathFileName().c_str(), new GCodePathHandlerCnc(cnc));
	inboundFileParser->changePathListRunnerInterface(m_portSelector->GetStringSelection());
	
	return processTemplate_Execute(sk);
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processManualTemplate() {
///////////////////////////////////////////////////////////////////
	wxASSERT( cncManuallyMoveCoordPanel );
	
	CncMoveDefinition move;
	move.x.absolute		= cncManuallyMoveCoordPanel->isAbsoluteMove();
	move.x.value		= cncManuallyMoveCoordPanel->getValueX();
	move.y.absolute		= cncManuallyMoveCoordPanel->isAbsoluteMove();
	move.y.value		= cncManuallyMoveCoordPanel->getValueY();
	move.z.absolute		= cncManuallyMoveCoordPanel->isAbsoluteMove();
	move.z.value		= cncManuallyMoveCoordPanel->getValueZ();
	
	move.speedMode 		= CncSpeedUserDefined;
	move.speedValue		= (double)(defaultSpeedSlider->getValueMM_MIN());
	
	move.moveMode		= CncMoveDefinition::convert(cncManuallyMoveCoordPanel->getMoveMode(), CncMoveDefinition::MoveMode::MM_2D);
	
	move.toolState		= cncManuallyMoveCoordPanel->switchToolOn();
	move.correctLimit	= cncManuallyMoveCoordPanel->correctLimitStates();
	
	if ( m_unit->GetValue() == "steps" ) {
		move.x.value *= THE_CONFIG->getDisplayFactX();
		move.y.value *= THE_CONFIG->getDisplayFactY();
		move.z.value *= THE_CONFIG->getDisplayFactZ();
	} 
	
	return processManualMove(move);
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processTestTemplate() {
///////////////////////////////////////////////////////////////////
	wxWindow* page = m_testCaseBook->GetCurrentPage();
	if ( page != NULL ) {
		if 		( page == m_testIntervalPage )	return processTestInterval();
		//...
		
		wxString msg;
		msg << "This Test (\"";
		msg << m_testCaseBook->GetPageText(m_testCaseBook->GetSelection()); 
		msg << "\") isn't assigned to the run mode. May be there is an action control on the test page itself.\n\n";
		msg << "Nothing will be done.\n" ;
		
		wxMessageDialog dlg(this, msg, _T("MainFrame::processTestTemplate(): Not registered test . . . "), 
							wxOK|wxCENTRE|wxICON_QUESTION);
		
		dlg.ShowModal();
		return true;
	}
	
	std::cerr << "MainFrame::processTestTemplate(): Invalid page selection!" << std::endl;
	return false;
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processManualMove(const CncMoveDefinition& md) {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	
	if ( inboundFileParser != NULL )
		delete inboundFileParser;
	
	ManuallyParser* p = new ManuallyParser(new ManuallyPathHandlerCnc(cnc));
	inboundFileParser = p;
	inboundFileParser->changePathListRunnerInterface(m_portSelector->GetStringSelection());
	
	p->reset(cnc->getCurAppPosMetric());
	p->addMove(md);
	
	CNC_TRANSACTION_LOCK_RET_ON_ERROR(false)
	
	motionMonitor->pushInteractiveProcessMode();
	
	bool ret = false;
	if ( isDebugMode == true ) 	ret = p->processDebug();
	else 						ret = p->processRelease();
	
	motionMonitor->popInteractiveProcessMode();
	motionMonitor->update(true);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processTestInterval() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);

	if ( cnc->isConnected() == false ) {
		std::cerr << "Not connetced, nothing will be processed." << std::endl;
		return false;
	}
	
	wxString sel = m_testIntervalMode->GetStringSelection();
	char mode = sel[0];
	
	int countX = m_testCountX->GetValue();
	int countY = m_testCountY->GetValue(); 
	int countZ = m_testCountZ->GetValue();
	
	double xd = 0.0, yd = 0.0, zd = 0.0;
	
	bool modeX = m_testModeX->GetValue();
	bool modeY = m_testModeY->GetValue();
	bool modeZ = m_testModeZ->GetValue();
	
	if ( modeX == true ) {
		wxString xs = m_testDistanceX->GetValue();
		if ( xs != "" )	xs.ToCDouble(&xd);
	} 
	
	if ( modeY == true ) {
		wxString ys = m_testDistanceY->GetValue();
		if ( ys != "" )	ys.ToCDouble(&yd);
	}
	
	if ( modeZ == true ) {
		wxString zs = m_testDistanceZ->GetValue();
		if ( zs != "" )	zs.ToCDouble(&zd);
	} 
	
	wxString firstX = m_testFirstX->GetStringSelection();
	wxString firstY = m_testFirstY->GetStringSelection();
	wxString firstZ = m_testFirstZ->GetStringSelection();
	
	xd = (firstX == "First +" ? +xd : -xd);
	yd = (firstY == "First +" ? +yd : -yd);
	zd = (firstZ == "First +" ? +zd : -zd);
	
	m_testDurationCounterX->SetLabel("-");
	m_testDurationCounterY->SetLabel("-");
	m_testDurationCounterZ->SetLabel("-");
	
	switch ( mode ) {
		case 'A': 	if ( modeX == true ) {
						for( int i=0; i<countX; i++ ) {
							processTestMove(m_testAxisX, m_testDurationCounterX, i, +xd, 0.0, 0.0);
							processTestMove(m_testAxisX, m_testDurationCounterX, i, -xd, 0.0, 0.0);
						}
					}
					
					if ( modeY == true ) {
						for( int i=0; i<countY; i++ ) {
							processTestMove(m_testAxisY, m_testDurationCounterY, i, 0.0, +yd, 0.0);
							processTestMove(m_testAxisY, m_testDurationCounterY, i, 0.0, -yd, 0.0);
						}
					}
					
					if ( modeZ == true ) {
						for( int i=0; i<countZ; i++ ) {
							processTestMove(m_testAxisZ, m_testDurationCounterZ, i, 0.0, 0.0, +zd);
							processTestMove(m_testAxisZ, m_testDurationCounterZ, i, 0.0, 0.0, -zd);
						}
					}
						
					break;
					
		case 'B': 	for( int i=0; i<countX; i++ ) {
						if ( modeX == true ) processTestMove(m_testAxisX, m_testDurationCounterX, i, +xd, 0.0, 0.0);
						if ( modeY == true ) processTestMove(m_testAxisY, m_testDurationCounterY, i, 0.0, +yd, 0.0);
						if ( modeZ == true ) processTestMove(m_testAxisZ, m_testDurationCounterZ, i, 0.0, 0.0, +zd);
						
						if ( modeX == true ) processTestMove(m_testAxisX, m_testDurationCounterX, i, -xd, 0.0, 0.0);
						if ( modeY == true ) processTestMove(m_testAxisY, m_testDurationCounterY, i, 0.0, -yd, 0.0);
						if ( modeZ == true ) processTestMove(m_testAxisZ, m_testDurationCounterZ, i, 0.0, 0.0, -zd);
					}
					
					break;
					
		case 'C': 	for( int i=0; i<countX; i++ ) {
						processTestMove(NULL, NULL, i, +xd, +yd, +zd);
						processTestMove(NULL, NULL, i, -xd, -yd, -zd);
					}
					
					break;
		default: ;// do nothing
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
void MainFrame::processTestMove(wxStaticText* axis, wxStaticText* counter, int c, double xd, double yd, double zd) {
///////////////////////////////////////////////////////////////////
	if ( axis != NULL ) {
		axis->SetForegroundColour(*wxWHITE);
		axis->SetBackgroundColour(*wxBLACK);
		axis->Refresh();
	} else {
		m_testAxisX->SetForegroundColour(*wxWHITE);
		m_testAxisY->SetForegroundColour(*wxWHITE);
		m_testAxisZ->SetForegroundColour(*wxWHITE);
		m_testAxisX->SetBackgroundColour(*wxBLACK);
		m_testAxisY->SetBackgroundColour(*wxBLACK);
		m_testAxisZ->SetBackgroundColour(*wxBLACK);
		m_testAxisX->Refresh();
		m_testAxisY->Refresh();
		m_testAxisZ->Refresh();
	}
		
	if ( counter != NULL ) {
		counter->SetLabel(wxString::Format("%d", c + 1));
	} else {
		m_testDurationCounterX->SetLabel(wxString::Format("%d", c + 1));
		m_testDurationCounterY->SetLabel(wxString::Format("%d", c + 1));
		m_testDurationCounterZ->SetLabel(wxString::Format("%d", c + 1));
	}
		
	dispatchAll();
	cnc->manualSimpleMoveMetric3D(xd, yd, zd);
	dispatchAll();
	
	if ( axis != NULL ) {
		axis->SetForegroundColour(*wxBLACK);
		axis->SetBackgroundColour(*wxLIGHT_GREY);
		axis->Refresh();
	} else {
		m_testAxisX->SetForegroundColour(*wxBLACK);
		m_testAxisY->SetForegroundColour(*wxBLACK);
		m_testAxisZ->SetForegroundColour(*wxBLACK);
		m_testAxisX->SetBackgroundColour(*wxLIGHT_GREY);
		m_testAxisY->SetBackgroundColour(*wxLIGHT_GREY);
		m_testAxisZ->SetBackgroundColour(*wxLIGHT_GREY);
		m_testAxisX->Refresh();
		m_testAxisY->Refresh();
		m_testAxisZ->Refresh();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::testCountXSpinCtl(wxSpinEvent& event) {
///////////////////////////////////////////////////////////////////
	// do nothing
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectTestIntervalMode(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString sel = m_testIntervalMode->GetStringSelection();
	char mode = sel[0];
	
	m_testDurationCounterX->SetLabel("-");
	m_testDurationCounterY->SetLabel("-");
	m_testDurationCounterZ->SetLabel("-");

	m_testCountY->Enable(mode == 'A');
	m_testCountZ->Enable(mode == 'A');
	
	if ( mode != 'A' ) {
		int val = m_testCountX->GetValue();
		
		m_testCountY->SetValue(val);
		m_testCountZ->SetValue(val);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::testCountXUpdated(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString sel = m_testIntervalMode->GetStringSelection();
	char mode = sel[0];
	
	if ( mode != 'A' )
	{
		int val = m_testCountX->GetValue();
		
		m_testCountY->SetValue(val);
		m_testCountZ->SetValue(val);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::displayPositionSituation(int type, const wxString& m, const wxString& headline, const wxString& appendix) {
///////////////////////////////////////////////////////////////////
	const CncLongPosition ctrlPos(cnc->requestControllerPos());
	wxString msg(m);
	
	msg	<< "\n" << 
		wxString::Format("(X, Y, Z): % +10.3lf, % +10.3lf, % +10.3lf [mm]: Application Pos", 
							cnc->getCurAppPosMetricX(), 
							cnc->getCurAppPosMetricY(), 
							cnc->getCurAppPosMetricZ()
		);
	
	msg << "\n" <<
		wxString::Format("(X, Y, Z): % +10.3lf, % +10.3lf, % +10.3lf [mm]: Controller Pos <requested>", 
							ctrlPos.getX() * THE_CONFIG->getDisplayFactX(), 
							ctrlPos.getY() * THE_CONFIG->getDisplayFactY(), 
							ctrlPos.getZ() * THE_CONFIG->getDisplayFactZ()
		);
		
	if ( true )
	{
		msg << "\n" <<
		wxString::Format("(X, Y, Z): % +10.3lf, % +10.3lf, % +10.3lf [mm]: Controller Pos <locally stored>", 
							cnc->getCurCtlPosMetricX(), 
							cnc->getCurCtlPosMetricY(), 
							cnc->getCurCtlPosMetricZ()
		);
	}
		
	if ( appendix.IsEmpty() == false )
		msg	<< "\n\n" << appendix;
	
	if ( type < 0 )
	{
		auto display = [&](std::ostream& o)
		{
			o	<< headline	<< std::endl
				<< msg		<< std::endl
			;
		};
		
		switch ( type * (-1) )
		{
			case wxICON_ERROR:		display(std::cerr); break;
			case wxICON_WARNING:	display(cnc::cex1); break;
			default:				display(std::cout);
		}
	}
	else
	{
		const int flags =  wxOK | wxCENTRE | type;
		wxMessageDialog(this, msg, headline, flags).ShowModal();
	}
}
///////////////////////////////////////////////////////////////////
bool MainFrame::showConfigSummaryAndConfirmRun() {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
		
	switch ( THE_CONFIG->getRunConfirmationModeAsChar() ) {
		// Always
		case 'A':	break;
		
		// Serial Port only
		case 'S': 	if ( cnc->isEmulator() )
						return true;
						
					break;
		// Never
		default:	return true;
	}
	
	// check template format
	if ( getCurrentTemplateFormat() != TplSvg && getCurrentTemplateFormat() != TplGcode )
		return true;
	
	// control handling
	disableControls();
	m_btCancelRun->Enable(true);
	m_btConfirmRun->Enable(true);
	
	// select summary page
	m_outboundNotebook->SetSelection(OutboundSelection::VAL::SUMMARY_PANEL);
	m_notebookConfig->SetSelection(OutboundCfgSelection::VAL::SUMMARY_PANEL);
	
	// wait for user feedback
	runConfirmationInfo = RunConfirmationInfo::Wait;
	wxDateTime start 	= wxDateTime::UNow();
	wxDateTime now 		= wxDateTime::UNow();
	wxColour c1         = wxColor(227, 227, 227);
	wxColour c2         = wxColor(109, 109, 190);
	wxString msg		= "Waiting for user feedback. Please confirm to run . . . ";
	if ( m_btConfirmRun->GetBackgroundColour() == c1 )	m_btConfirmRun->SetBackgroundColour(c2);
	else												m_btConfirmRun->SetBackgroundColour(c1);
	cnc::trc.logWarning(msg);
	
	while ( runConfirmationInfo == RunConfirmationInfo::Wait ) {
		dispatchAll();
		now = wxDateTime::UNow();
		wxTimeSpan diff = now - start;
		if ( diff.GetSeconds() >= 1 ) {
			start = now;
			if ( m_btConfirmRun->GetBackgroundColour() == c1 ) {
				cnc::trc.logWarning(msg);
				m_btConfirmRun->SetBackgroundColour(c2);
			} else {
				cnc::trc.clear();
				m_btConfirmRun->SetBackgroundColour(c1);
			}
		}
	}
	
	cnc::trc.clear();
	m_btConfirmRun->SetBackgroundColour(c1);
	enableControls();
	m_btCancelRun->Enable(false);
	m_btConfirmRun->Enable(false);
	
	return (runConfirmationInfo == RunConfirmationInfo::Confirmed);
}
///////////////////////////////////////////////////////////////////
void MainFrame::confirmRun(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	runConfirmationInfo = RunConfirmationInfo::Confirmed;
}
///////////////////////////////////////////////////////////////////
void MainFrame::cancelRun(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	runConfirmationInfo = RunConfirmationInfo::Canceled;
}
///////////////////////////////////////////////////////////////////
void MainFrame::collectSvgSpecificSummary() {
///////////////////////////////////////////////////////////////////
	cncSummaryListCtrl->addHeadline(CncSummaryListCtrl::ParameterType::PT_HEADLINE, "SVG specific Settings");
}
///////////////////////////////////////////////////////////////////
void MainFrame::collectGCodeSpecificSummary() {
///////////////////////////////////////////////////////////////////
	typedef CncSummaryListCtrl::ParameterType PT;
	CncConfig* cc = CncConfig::getGlobalCncConfig();
	
	cncSummaryListCtrl->addHeadline(CncSummaryListCtrl::ParameterType::PT_HEADLINE, "GCode specific Settings");
	cncSummaryListCtrl->addParameter(PT::PT_GCODE, "Sample . . . .", wxString::Format("%4.3f",	cc->getReplyThresholdMetric()),						"xx");
}
///////////////////////////////////////////////////////////////////
void MainFrame::collectSummary() {
///////////////////////////////////////////////////////////////////
	if ( cnc == NULL )
		return;
		
	typedef CncSummaryListCtrl::ParameterType PT;
		
	cncSummaryListCtrl->clear();
	cncSummaryListCtrl->addHeadline(PT::PT_HEADLINE, "Common Settings");
	cncSummaryListCtrl->addParameter(PT::PT_COMMON, "Default Tool",					THE_CONFIG->getDefaultToolParamAsInfoStr(),												"-");
	cncSummaryListCtrl->addParameter(PT::PT_COMMON, "Workpiece thickness",				wxString::Format("%4.3f", 	THE_BOUNDS->getWorkpieceThickness()),						"mm");
	//cncSummaryListCtrl->addParameter(PT::PT_COMMON, "Curve lib resolution", 			wxString::Format("%0.3f", 	THE_CONFIG->getRenderResolution()),							"-");
	cncSummaryListCtrl->addParameter(PT::PT_COMMON, "Default Rapid speed",				wxString::Format("%4.3f", 	THE_CONFIG->getDefaultRapidSpeed_MM_MIN()),					"mm/min");
	cncSummaryListCtrl->addParameter(PT::PT_COMMON, "Default Work speed",				wxString::Format("%4.3f", 	THE_CONFIG->getDefaultWorkSpeed_MM_MIN()),					"mm/min");
	cncSummaryListCtrl->addParameter(PT::PT_COMMON, "Reply Threshold",					wxString::Format("%4.3f",	THE_CONFIG->getReplyThresholdMetric()),						"mm");

	// type specific . . .
	switch ( getCurrentTemplateFormat() ) {
		case TplSvg:	collectSvgSpecificSummary();
						break;
		
		case TplGcode:	collectGCodeSpecificSummary();
						break;
						
		default:		; // do nothing
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateSetterList() {
///////////////////////////////////////////////////////////////////
	wxASSERT( setterList != NULL );
	m_setterListCount->SetValue(wxString::Format("# %ld", (long)setterList->GetItemCount()));
}
///////////////////////////////////////////////////////////////////
void MainFrame::nootebookConfigChanged(wxListbookEvent& event) {
///////////////////////////////////////////////////////////////////
	// check if currently a run is active and return if so to avoid 
	// a controller request during this mode
	bool runActive = !m_rcRun->IsEnabled();

	m_dvListCtrlControllerConfig->DeleteAllItems();
	m_dvListCtrlControllerPins->DeleteAllItems();
	
	unsigned int sel = event.GetSelection();
	if ( (wxWindow*)event.GetEventObject() == m_notebookConfig ) {
		
		wxCommandEvent dummyEvent;
		
		switch ( sel ) {
			case OutboundCfgSelection::VAL::SUMMARY_PANEL:
					collectSummary();
					break;
					
			case OutboundCfgSelection::VAL::CNC_PIN_PANEL:
					if ( runActive == true ) {
						cnc::trc.logWarning("During an active run controller requests are not possible! Ty it later again.");
						return;
					}
					requestControllerPinsFromButton(dummyEvent);
					break;
					
			case OutboundCfgSelection::VAL::CNC_CONFIG_PANEL:
					if ( runActive == true ) {
						cnc::trc.logWarning("During an active run controller requests are not possible! Ty it later again.");
						return;
					}
					requestControllerConfigFromButton(dummyEvent);
					break;
		}
	}
}
///////////////////////////////////////////////////////////////////
bool MainFrame::checkReferencePositionState() {
///////////////////////////////////////////////////////////////////
	const CncTemplateFormat tf = getCurrentTemplateFormat();
	if ( tf == TplManual || tf == TplTest )
		return true;
		
	const CncDoublePosition refPos(CncStartPositionResolver::getReferencePosition());
	const bool refPosValid	= isReferenceStateValid();
	const bool zero			= ( cnc->getCurAppPosMetric() != refPos );
	
	if ( refPosValid == false )
	{
		const wxString msg("The current reference position isn't valid due to a setup change or it isn't not initialized yet.\n");
		
		const int ret = showReferencePositionDlg(msg);
		if ( ret == wxID_OK && zero == false )
		{
			cnc::trc.logInfoMessage("Reference Position is fixed now. Please restart");
			
			// Safety: Always return false in this case because this will
			// stop the currently started run. 
			return false;
		}
	}
	
	if ( zero )
	{
		wxASSERT( cnc != NULL );
		
		bool openDlg = false;
		switch ( THE_CONFIG->getRunConfirmationModeAsChar() )
		{
			// Always
			case 'A':	openDlg = true; break;
			// Serial Port only
			case 'S': 	openDlg = !cnc->isEmulator(); break;
			// Never
			default:	openDlg = false;
		}
		
		CncStartPositionResolver dlg(this);
		int ret = wxID_OK;
		
		if ( openDlg == true ) 	ret = dlg.ShowModal();
		else 					ret = dlg.resolve("M[xyz]");

		if ( ret == wxID_OK )
		{
			cnc::trc.logInfoMessage("Reference Position is fixed now. Please restart");
		}
		else
		{
			// Always return false in this case because this will
			// stop the currently started run. 
			return false;
		}
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool MainFrame::checkIfRunCanBeProcessed(bool confirm) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	if ( isInterrupted() == true )
	{
		CNC_CERR_FUNCT_A(": Run aborted - Interrupt detected!")
		return false;
	}
		
	// checks against the loaded template
	const CncTemplateFormat tf = getCurrentTemplateFormat();
	const wxString fn (getCurrentTemplatePathFileName());
	
	if ( tf != TplManual && tf != TplTest )
	{
		if ( fn.IsEmpty() == true )
		{
			CNC_CERR_FUNCT_A("No template loaded!")
			return false;
		}
	}
	
	// Validate current boundaries
	if ( THE_TPL_CTX->getBoundaries().hasBoundaries() ) 
	{
		// check if template fits with the current hardware information
		std::stringstream ss;
		if ( THE_TPL_CTX->fitsIntoCurrentHardwareBoundaries(CncTemplateContext::BT_TEMPLATE, ss) == false ) 
		{
			wxString msg(ss.str().c_str());
			std::clog << "Boundary Failure . . ." << std::endl;
			
			REGISTER_NEXT_LOGGER_ROW
			
			if		( msg.StartsWith("Error") )		{ SET_RESULT_FOR_REGISTERED_LOGGER_ROW_ERROR;   std::cerr << msg; }
			else if	( msg.StartsWith("Warning") )	{ SET_RESULT_FOR_REGISTERED_LOGGER_ROW_WARNING; cnc::cex1 << msg; }
			else									std::cout << msg;
			
			// if hardware is available interact with the user in this case
			if ( THE_CONTEXT->hasHardware() )
			{
				msg.append("Should this run definitely further processed?");
				
				wxMessageDialog dlg(this, msg, _T("Check boundaries . . . "), 
								wxYES|wxNO|wxICON_WARNING|wxCENTRE);
			
				int ret = dlg.ShowModal();
				if ( ret != wxID_YES )
				{
					std::cerr << "Template Run aborted regarding boundary failures" << std::endl;
					return false;
				}
			}
		}
	}
	else
	{
		// if hardware is available interact with the user in this case
		if ( THE_CONTEXT->hasHardware() )
		{
			const wxString msg(
				"There are no boundary information regarding the current template available.\n" \
				"This may be causes limit switch violations!\n\n" \
				"Should this run definitely further processed?"
			);
			
			wxMessageDialog dlg(this, msg, _T("Check boundaries . . . "), 
							wxYES|wxNO|wxICON_WARNING|wxCENTRE);
		
			int ret = dlg.ShowModal();
			if ( ret != wxID_YES )
			{
				std::cerr << "Template Run aborted regarding boundary failures" << std::endl;
				return false;
			}
		}
	}

	// Validate current positions
	if ( evaluatePositions == true && cnc->validateAppAgainstCtlPosition() == false )
	{
		displayPositionSituation(	wxICON_ERROR,
									"Validate positions failed\n", 
									"CNC Position check . . . ", 
									"The run command will be aborted!"
		);
		
		setReferencePosEnforceFlag(true);
		
		CNC_CERR_FUNCT_A("Position validation failed!")
		return false;
	}
	
	// ask controlwer for readiness
	if ( cnc->isReadyToRun() == false ) 
	{
		CNC_CERR_FUNCT_A("Controller isn't ready to run: Run was rejected!")
		return false;
	}
	
	// may be this is already done by cnc->isReadyToRun()
	cnc->evaluateLimitState();
	if ( cnc->isALimitSwitchActive() )
	{
		// always return false to reconfigure zero in this situation
		CNC_CERR_FUNCT_A("Limit switch detected")
		return false;
	}
	
	return (confirm == true ? showConfigSummaryAndConfirmRun() : true);
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processTemplate(bool confirm) {
///////////////////////////////////////////////////////////////////
	try 
	{
		wxASSERT(cnc);
		
		wxDateTime tsStart(wxDateTime::UNow());
		
		SHA1SessionKey runSessionKey;
		CncStringSha1::createSessionKey(
					wxString::Format("%s.%u", tsStart.FormatISOCombined(), tsStart.GetMillisecond()), 
					runSessionKey
		); 
		
		CncRunAnimationControl rac(this);
		
		// deactivate idle requests
		// do this already here because the checks below already perform 
		// cnc commands
		CNC_TRANSACTION_LOCK_RET_ON_ERROR(false)

		//-----------------------------------------------------------------
		// all mandatory checks from here on . . . 
		
		// do this first and return true if it failed to start a second loop 
		// on demand that's much better as the runs starts immediately after 
		// the reference position setup and in secure mode it is essential to
		// stop here because there no modal dialogue.
		if ( checkReferencePositionState() == false )
			return true;
		
		// corresponding error messages are already displayed
		if ( checkIfRunCanBeProcessed(confirm) == false )
		{
			MainFrame::Notification notification;
			notification.location	= MainFrame::Notification::Location::NL_MonitorView;
			notification.type		= 'E';
			notification.title		= "Check if run can be processed";
			notification.message	= "Mandatory Checks failed . . . ";
			displayNotification(notification);
			return false;
		}
		
		// select a template page
		if ( m_mainViewBook->GetSelection() != MainBookSelection::VAL::MANUEL_PANEL && 
			 m_mainViewBook->GetSelection() != MainBookSelection::VAL::TEST_PANEL && 
			 m_mainViewBook->GetSelection() != MainBookSelection::VAL::SOURCE_PANEL &&
			 m_mainViewBook->GetSelection() != MainBookSelection::VAL::SETUP_PANEL)
		{
			selectMainBookSourcePanel();
		}
		
		// select the draw pane
		if ( THE_CONTEXT->secureModeInfo.isActive == false )
		{
			showAuiPane("Outbound");
			selectMonitorBookCncPanel();
		}
		
		//-----------------------------------------------------------------
		// deactivate all relevant behaviours from here on . . . 
		
		CncRunEventFilter cef;
		
		// Deactivate observer
		CncTemplateObserver::Deactivator observerDeactivator(templateObserver);
		
		// it's very import to deactivate the notifications during a run
		// because instead every configure change will release a notification
		// this will be the case for example if the SVG path handler changes
		// the z -axis values . . .
		// as a result the processing slows down significantly.
		CncConfig::NotificationDeactivator cfgNotDeactivation;
		
		//-----------------------------------------------------------------
		// prepare all relevant settings from here on . . . 
		const bool useExistingCncInstructions	 = THE_CONFIG->isModified() == false 
												&& THE_TPL_CTX->isValid() 
												&& cnc::isFileTemplate(getCurrentTemplateFormat())
		;
		
		// if template boundaries available prepare the motion monitor 
		// to the best size (scale) and origin placement
		if ( THE_TPL_CTX->getBoundaries().hasBoundaries() ) 
			motionMonitor->makeCompleteVisibleMetric(THE_TPL_CTX->getBoundaries());

		serialSpyPanel->clearSerialSpyBeforNextRun();
		
		THE_CONTEXT->resetProcessing();
		THE_CONTEXT->initPreparationPhase();
		THE_CONTEXT->setAllowEventHandling(true);
		
		cnc->resetSetterMap();
		cnc->processSetter(PID_SEPARATOR, SEPARARTOR_RUN);
		cnc->enableProbeMode(THE_CONTEXT->isProbeMode());
		cnc->setStepDelay(THE_CONFIG->getArtificallyStepDelay());
		
		decorateOutboundSaveControls(false);
		
		const wxString probeMode(THE_CONTEXT->isProbeMode() ? "ON" :"OFF");
		
		// restart the trace timer using the previous time-out value
		m_traceTimer->Start(-1);
		
		clearPositionSpy();
		disableControls();
		resetMinMaxPositions();
		getTemplateContextSummary()->getParsingSynopsis()->clearAll();
		
		// Overrides the THE_CONTEXT->timestamps.logTotalTimeStart() CncRunAnimationControl mechanism (ctor)
		// to be more closer on the real processing start
		THE_CONTEXT->timestamps.logTotalTimeStart();
		
		//-----------------------------------------------------------------
		// process template . . . 
		
			// *********************
			// This instance starts and stops the speed monitor
			CncSpeedMonitorRunner smr(speedMonitor);
			
			StreamBufferHighlighter sbh(std::clog);
			
			motionMonitor->pushProcessMode();
			
			bool ret = false;
			if ( useExistingCncInstructions )
			{
				CNC_CLOG_A(wxString::Format("~~~ Processing based on existing CNC Instructions started ~~~"))
				INC_LOGGER_INDENT
				
				motionMonitor->clear();
				ret = THE_TPL_CTX->executeCncInterface();
			}
			else
			{
				CNC_CLOG_A(wxString::Format("~~~ Processing(probe mode = %s) started ~~~", probeMode))
				INC_LOGGER_INDENT
					
				if ( THE_TPL_CTX->getRunCount() > 0 )
				{
					wxString msg("Cant use existing Cnc Instructions because: ");
					if ( THE_CONFIG->isModified() == true  ) msg.append(" Configuration is modified");
					if ( THE_TPL_CTX->isValid()   == false ) msg.append(" Template context isn't valid");
					
					CNC_CEX3_A(msg)
				}
				THE_TPL_CTX->registerRun();
				ret = processTemplate_SelectType(runSessionKey);
				
				#warning rethink the reset procedure
				if ( ret )
					THE_CONFIG->resetModificationFlag();
			}
			
			motionMonitor->popProcessMode();
	
			if ( ret )
				cnc->updatePreview3D();
			
			// Do this already here to be to be more closer on the real processing start
			// instead of the dtor of CncRunAnimationControl 
			logTimeConsumed();
			statisticsPane->logStatistics();
			displayReport(1);
			
			DEC_LOGGER_INDENT
			// *********************

		//-----------------------------------------------------------------
		// post settings from here on . . . 
		
		decorateOutboundSaveControls(cnc->isOutputAsTemplateAvailable());
		
		// refresh some periphery
		motionMonitor->updateMonitorAndOptions();
		statisticsPane->updateReplayPane();
		
		// refresh boundaries
		CncDoubleBoundaries bounds;
		bounds.setMinBound(cnc->getMinPositionsMetric());
		bounds.setMaxBound(cnc->getMaxPositionsMetric());
		THE_TPL_CTX->registerBoundaries(bounds, CncTemplateContext::BT_MEASURED);
		
		// if template boundaries available prepare the motion monitor 
		// to the best size (scale) and origin placement
		if ( THE_TPL_CTX->getBoundaries().hasBoundaries() ) 
		{
			std::stringstream ss;
			motionMonitor->makeCompleteVisibleMetric(THE_TPL_CTX->getBoundaries());
			if ( THE_TPL_CTX->fitsIntoCurrentHardwareBoundaries(CncTemplateContext::BT_MEASURED, ss) == false )
			{
				const wxString msg(ss.str().c_str());
				
				REGISTER_NEXT_LOGGER_ROW
				
				if		( msg.StartsWith("Error") )		{ std::cerr << msg; SET_RESULT_FOR_REGISTERED_LOGGER_ROW_ERROR		}
				else if	( msg.StartsWith("Warning") )	{ cnc::cex1 << msg; SET_RESULT_FOR_REGISTERED_LOGGER_ROW_WARNING	}
				else									{ std::cout << msg; SET_RESULT_FOR_REGISTERED_LOGGER_ROW_OK			}
				
				ret = false;
			}
		}
		
		const wxTimeSpan elapsed = wxDateTime::UNow().Subtract(tsStart);
		const wxString timeInfo(wxString::Format("total time: %s", elapsed.Format("%H:%M:%S.%l")));
		
		//-----------------------------------------------------------------
		// post statements from here on . . . 
		// prepare final statements
		if ( ret == false || THE_CONTEXT->parsingSynopsisHasErrorEntries() )
		{
			if ( THE_CONTEXT->parsingSynopsisHasErrorEntries() )
			{
				CNC_CERR_A("Parsing Error(s) detected")
				getTemplateContextSummary()->getParsingSynopsis()->traceErrorEntries(std::cerr);
				CNC_CERR_A("For more details please visit the parsing synopsis trace")
			}
			
			StreamBufferHighlighter sbh(std::cerr);
			
			CNC_CERR_A(wxString::Format("=== Processing(probe mode = %s) finished not successfully, %s ===", probeMode, timeInfo))
			SET_RESULT_FOR_LAST_FILLED_LOGGER_ROW_ERROR
			
			lock.setErrorMode();
			
			THE_TPL_CTX->resetValidRuns();
			decorateDryRunState(cncError);
		} 
		else
		{
			if ( THE_CONTEXT->parsingSynopsisHasNonInfoEntries() )
			{
				CNC_CEX1_A("Parsing Warnings(s) detected")
				getTemplateContextSummary()->getParsingSynopsis()->traceWarnEntries(cnc::cex1);
				CNC_CEX1_A("Parsing Error(s) detected")
				getTemplateContextSummary()->getParsingSynopsis()->traceErrorEntries(std::cerr);
				CNC_CEX1_A("For more details please visit the parsing synopsis trace")
				
				StreamBufferHighlighter sbh(cnc::cex1);
				
				CNC_CEX1_A(wxString::Format("=== Processing(probe mode = %s) finished with warnings, %s ===", probeMode, timeInfo))
				SET_RESULT_FOR_LAST_FILLED_LOGGER_ROW_WARNING
				
				THE_TPL_CTX->registerValidRun();
				decorateDryRunState(cncWarning);
			}
			else
			{
				StreamBufferHighlighter sbh(std::clog);
				
				CNC_CLOG_A(wxString::Format("=== Processing(probe mode = %s) finished successfully, %s ===", probeMode, timeInfo))
				SET_RESULT_FOR_LAST_FILLED_LOGGER_ROW_OK
				
				THE_TPL_CTX->registerValidRun();
				decorateDryRunState(cncOk);
			}
		}
		
		enableControls();
		return ret;
	}
	catch (const CncInterruption& ex) 
	{
		CncInterruption nex(ex);
		nex.addCatchLocation(CNC_LOG_FUNCT);
		handleCncInterruptException(nex);
	}
	catch (...) 
	{
		handleUnhandledException(CNC_LOG_FUNCT);
	}
	
	return false;
}
///////////////////////////////////////////////////////////////////
// don't call this method directly, instead use processTemplate()
bool MainFrame::processTemplate_SelectType(const SHA1SessionKey& sk) {
///////////////////////////////////////////////////////////////////
	if ( getCurrentTemplateFormat() != TplManual )
		cncManuallyMoveCoordPanel->resetClearViewState();
	
	bool ret = false;
	switch ( getCurrentTemplateFormat() )
	{
		case TplBinary:
		{
			if ( saveTemplateOnDemand() == false )
				break;
				
			clearMotionMonitor();
			
			ret = processBinaryTemplate(sk);
			break;
		}
		case TplSvg:
		{
			if ( saveTemplateOnDemand() == false )
				break;
			
			clearMotionMonitor();
			
			ret = processSVGTemplate(sk);
			break;
		}
		case TplGcode:
		{
			if ( saveTemplateOnDemand() == false )
				break;
				
			clearMotionMonitor();
			
			ret = processGCodeTemplate(sk);
			break;
		}
		case TplManual:
		{
			if ( cncManuallyMoveCoordPanel->shouldClearMontionMonitor() )
				clearMotionMonitor();

			ret = processManualTemplate();
			break;
		}
		case TplTest:
		{
			clearMotionMonitor();
			
			cnc->startSerialMeasurement();
			ret = processTestTemplate();
			cnc->stopSerialMeasurement();
			break;
		}
		default: ; // do nothing
	}
	
	// Check positions
	if ( cnc->validateAppAgainstCtlPosition() == false )
	{
		if ( isInterrupted() == false )
		{
			displayPositionSituation(	wxICON_ERROR * (-1),
										"Validate positions failed\n", 
										"CNC Position check . . . ", 
										"The run command will be aborted!"
			);
			
			setReferencePosEnforceFlag(true);
		}
		
		ret = false;
	}
	
	if ( cnc->getPositionOutOfRangeFlag() == true )
	{
		//if ( isInterrupted() == false ) {
			std::cerr << "Out of range: During the last run the position limits were exceeded." << std::endl;
			CncLongPosition::Watermarks wm = cnc->getWaterMarks();
			CncLongPosition min(wm.xMin, wm.yMin, wm.zMin);
			CncLongPosition max(wm.xMax, wm.yMax, wm.zMax);
			
			cnc->isPositionOutOfRange(min, true);
			cnc->isPositionOutOfRange(max, true);
			
			setReferencePosEnforceFlag(true);
		//}
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void MainFrame::logTimeConsumed() {
///////////////////////////////////////////////////////////////////
	THE_CONTEXT->timestamps.logTotalTimeEnd();
	wxString value;
	m_cmdDuration->ChangeValue(THE_CONTEXT->timestamps.getTotalTimeConsumedFormated(value));

	std::stringstream ss;
	ss << THE_CONTEXT->timestamps;
	m_cmdDuration->SetToolTip(ss.str().c_str());
}
///////////////////////////////////////////////////////////////////
void MainFrame::resetMinMaxPositions() {
///////////////////////////////////////////////////////////////////	
	if ( cnc == NULL )
		return;
		
	cnc->resetWatermarks();
}
///////////////////////////////////////////////////////////////////
void MainFrame::setAutoSaveMode(bool as) {
///////////////////////////////////////////////////////////////////
	THE_TPL_CTX->setAutoSaveMode(as);
	
	const bool b = THE_TPL_CTX->isAutoSaveMode();
	m_btSvgToggleAutoSaveTplOnProcess->SetToolTip(b ? "Switch AutoSave off" : "Switch AutoSave on" );
	m_btSvgToggleAutoSaveTplOnProcess->SetValue(b);
}
///////////////////////////////////////////////////////////////////
bool MainFrame::saveTemplateOnDemand() {
///////////////////////////////////////////////////////////////////
	if ( sourceEditor->IsModified() == true )
	{
		const wxString msg(wxString::Format("Save Template?\n\n '%s'", getCurrentTemplatePathFileName()));
		wxRichMessageDialog dlg(this, msg, _T("File Observer . . . "), 
		                    wxYES|wxNO|wxCANCEL|wxCENTRE);
		
		dlg.ShowCheckBox("Don't not ask again for the current template", THE_TPL_CTX->isAutoSaveMode());
		dlg.SetFooterText("The current template was modified.");
		dlg.SetFooterIcon(wxICON_WARNING);
		
		const int ret = THE_TPL_CTX->isAutoSaveMode() ? wxID_YES : dlg.ShowModal();
		setAutoSaveMode(dlg.IsCheckBoxChecked());
		
		if ( ret == wxID_YES )
		{
			saveFile();
			selectMonitorBookCncPanel();
		} 
		else if ( ret == wxID_CANCEL ) 
		{
			return false;
		}
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
void MainFrame::prepareAndShowMonitorTemplatePreview(bool force) {
///////////////////////////////////////////////////////////////////
	if ( force == true )
		lastMonitorPreviewFileName.clear();
	
	// check if a preview update is necessary
	const bool isFileChanged	= lastMonitorPreviewFileName != getCurrentTemplatePathFileName();
	const bool isModified		= sourceEditor->GetModify() || force;
	
	if ( isFileChanged == false && isModified == false )
		return;
		
	// Open ...
	lastMonitorPreviewFileName = getCurrentTemplatePathFileName();
	
	// wxString tfn(CncFileNameService::getCncTemplatePreviewFileName(getCurrentTemplateFormat()));
	// this causes file access errors between this app and the internet explorer
	// write a temp file instead to have any time a new one
	
	// --------------------------------------------------------------
	// First prepare the file name for the temporary file
	wxString tfn(CncFileNameService::getTempFileName(getCurrentTemplateFormat()));
	
	CncTemplateFormat tf = getCurrentTemplateFormat();
	switch ( tf )
	{
		case TplSvg:		tfn.append(".svg");
							break;
							
		case TplGcode:		tfn.append(".gcode");
							break;
							
		case TplBinary:		if ( BinaryFileParser::extractSourceContentAsFile(getCurrentTemplatePathFileName(), tfn) == true )
							{
								openMonitorPreview(tfn);
								// no further copy necessary
								return;
							} 
							else 
							{
								std::cerr << CNC_LOG_FUNCT << ": Can't create preview!" << std::endl;
								return;
							}
							
		default:			// do nothing;
							break;
	}
	
	if ( tfn.IsEmpty() )
	{
		std::cerr << CNC_LOG_FUNCT_A(": Invalid file name!\n");
		return;
	}
	
	// --------------------------------------------------------------
	// Then create a copy to avoid a modification of source editor
	wxTextFile file(tfn);
	if ( !file.Exists() )
		file.Create();
	
	if ( file.Open() ) 
	{
		file.Clear();
		
		for (long i=0; i<sourceEditor->GetNumberOfLines(); i++) 
		{
			wxString line = sourceEditor->GetLineText(i);
			file.AddLine(line);
		}
		
		file.Write();
		file.Close();
		
		openMonitorPreview(tfn);
	} 
	else 
	{
		std::cerr << "Error creating a temp file: " << tfn.c_str() << std::endl;
	}
	
	// --------------------------------------------------------------
	// Now check the boundaries. This will be done here because the relevant 
	// information were evaluated by the openMonitorPreview preview function above 
	
	// Only if boundaries exists check if it fits
	if ( THE_TPL_CTX->hasBoundaries() )
	{
		std::stringstream ss;
		if ( THE_TPL_CTX->fitsIntoCurrentHardwareBoundaries(CncTemplateContext::BT_TEMPLATE, ss) == false )
		{
			const wxString msg(ss.str().c_str());
			
			REGISTER_NEXT_LOGGER_ROW
			
			if		( msg.StartsWith("Error") )		{ std::cerr << msg; SET_RESULT_FOR_REGISTERED_LOGGER_ROW_ERROR		}
			else if	( msg.StartsWith("Warning") )	{ cnc::cex1 << msg; SET_RESULT_FOR_REGISTERED_LOGGER_ROW_WARNING	}
			else									{ std::cout << msg; SET_RESULT_FOR_REGISTERED_LOGGER_ROW_OK			}
		}
	}
	else
	{
		REGISTER_NEXT_LOGGER_ROW
		cnc::cex1 << "Template without boundary information" << std::endl;
		SET_RESULT_FOR_REGISTERED_LOGGER_ROW_WARNING
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::emergencyStop(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	std::cerr << "Emergency Stop detected" << std::endl;
	
	try {
		wxASSERT(cnc);
		cnc->interrupt("Emergency Stop detected");
	}
	catch (const CncInterruption& ex) 
	{
		CncInterruption nex(ex);
		nex.addCatchLocation(CNC_LOG_FUNCT);
		handleCncInterruptException(nex);
	}
	catch (...) 
	{
		handleUnhandledException(CNC_LOG_FUNCT);
	}
	
	setReferencePosEnforceFlag(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::navigateX(CncDirection d) {
///////////////////////////////////////////////////////////////////
	double fact = 1;
	bool shtKey = CncAsyncKeyboardState::isShiftPressed();
	bool ctlKey = CncAsyncKeyboardState::isControlPressed();
	bool altKey = CncAsyncKeyboardState::isAltPressed();

	if ( altKey ) {
		fact /= 100;
	} else {
		if 		( shtKey && !ctlKey ) fact *= 2;
		else if ( ctlKey && !shtKey ) fact /= 10;
		else if ( ctlKey &&  shtKey ) fact *= 10;
	}

	switch ( d ) {
		case CncClockwise: 	fact *= +1; break;
		case CncCounterClockwise: fact *= -1; break;
		default: ;
	}

	if ( isInterrupted() ) {
		std::cerr << "CNC controller is interrupted, nothing will be done."<< std::endl;
		return;
	}
	
	cnc->manualSimpleMoveMetric(fact, 0.0, 0.0);
}
///////////////////////////////////////////////////////////////////
void MainFrame::navigateY(CncDirection d) {
///////////////////////////////////////////////////////////////////
	double fact = 1;
	bool shtKey = CncAsyncKeyboardState::isShiftPressed();
	bool ctlKey = CncAsyncKeyboardState::isControlPressed();
	bool altKey = CncAsyncKeyboardState::isAltPressed();

	if ( altKey ) {
		fact /= 100;
	} else {
		if 		( shtKey && !ctlKey ) fact *= 2;
		else if ( ctlKey && !shtKey ) fact /= 10;
		else if ( ctlKey &&  shtKey ) fact *= 10;
	}

	switch ( d ) {
		case CncClockwise: 	fact *= +1; break;
		case CncCounterClockwise: fact *= -1; break;
		default: ;
	}
	
	if ( isInterrupted() ) {
		std::cerr << "CNC controller is interrupted, nothing will be done."<< std::endl;
		return;
	}
	
	cnc->manualSimpleMoveMetric(0.0, fact, 0.0);
}
///////////////////////////////////////////////////////////////////
void MainFrame::navigateZ(CncDirection d) {
///////////////////////////////////////////////////////////////////
	double fact = 1;
	bool shtKey = CncAsyncKeyboardState::isShiftPressed();
	bool ctlKey = CncAsyncKeyboardState::isControlPressed();
	bool altKey = CncAsyncKeyboardState::isAltPressed();

	if ( altKey ) {
		fact /= 100;
	} else {
		if 		( shtKey && !ctlKey ) fact *= 2;
		else if ( ctlKey && !shtKey ) fact /= 10;
		else if ( ctlKey &&  shtKey ) fact *= 10;
	}

	switch ( d ) {
		case CncClockwise: 	fact *= +1; break;
		case CncCounterClockwise: fact *= -1; break;
		default: ;
	}

	if ( isInterrupted() ) {
		std::cerr << "CNC controller is interrupted, nothing will be done."<< std::endl;
		return;
	}

	cnc->manualSimpleMoveMetric(0.0, 0.0, fact);
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestVersion(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	CNC_TRANSACTION_LOCK
	 
	std::stringstream ss;
	cnc->processCommand(CMD_PRINT_VERSION, ss);
	cnc::trc.logInfoMessage(ss);
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestTimestamp(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	CNC_TRANSACTION_LOCK
	
	std::stringstream ss;
	cnc->processCommand(CMD_PRINT_TIMESTAMP, ss);
	cnc::trc.logInfoMessage(ss);
}

///////////////////////////////////////////////////////////////////
void MainFrame::requestHeartbeat(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	// this function request a single heartbeat
	// a continuous processing is handles by 
	// toggleIdleRequests(wxCommandEvent& event)
	wxASSERT(cnc);
	CNC_TRANSACTION_LOCK

	std::stringstream dummy;
	cnc->processCommand(CMD_HEARTBEAT, dummy);
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestConfig(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	CNC_TRANSACTION_LOCK

	cnc->processCommand(CMD_PRINT_CONFIG, std::clog);
	m_outboundNotebook->SetSelection(OutboundSelection::VAL::SUMMARY_PANEL);
	m_notebookConfig->SetSelection(OutboundCfgSelection::VAL::CNC_CONFIG_PANEL);
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestControllerConfigFromButton(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_btRequestCtlConfig->Enable(false);
	requestConfig(event);
	m_btRequestCtlConfig->Enable(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestCurrentPos(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	CNC_TRANSACTION_LOCK
	
	CncLongPosition   posS = cnc->requestControllerPos();
	CncDoublePosition posM = THE_CONFIG->convertStepsToMetric(posM, posS);
	
	std::stringstream ss;
	ss << "Current controller position: " 
	   << posS << " [steps]"
	   << " | " 
	   << posM << " [mm]";
	
	cnc::trc.logInfoMessage(ss.str().c_str());
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestCurrentLimitState(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	CNC_TRANSACTION_LOCK
	
	cnc->evaluateLimitState();
	cnc::trc.logInfoMessage("Limit controls were updated . . .");
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestCurrentLimitStateIcon(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	CNC_TRANSACTION_LOCK

	cnc->evaluateLimitState();
	cnc::trc.logInfoMessage("Limit controles were updated . . .");
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestReset(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	requestReset();
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestReset() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	CNC_TRANSACTION_LOCK
	
	getLogger()->Clear();
	
	mainViewInfobar->Dismiss();
	monitorViewInfobar->Dismiss();
	
	cnc->processSetter(PID_SEPARATOR, SEPARARTOR_RESET);
	cnc->setup(true);
	
	clearMotionMonitor();
	updateSetterList();
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestInterrupt(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	cnc->sendInterrupt();
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateFileContentPosition(long x, long y) {
///////////////////////////////////////////////////////////////////
	// try to select current  line as client id
	if ( motionMonitor != NULL )
	{
		motionMonitor->setCurrentClientId(y);
		motionMonitor->Refresh();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestEnableStepperMotors(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	if ( cnc->isConnected() )
		cnc->enableStepperMotors(m_miMotorEnableState->IsChecked() ? ENABLE_STATE_ON : ENABLE_STATE_OFF);
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveHome(wxCommandEvent& event) {
//
//	a) X(min), Y(min), Z(Top)
//	b) X(max), Y(max), Z(Top)
//	c) X(mid), Y(mid), Z(Top)
//	d) X(mid), Y(mid), Z(mid)
//
///////////////////////////////////////////////////////////////////
	if ( cnc && cnc->isReadyToRun() )
	{
		selectMonitorBookCncPanel();
		CNC_TRANSACTION_LOCK
		const CncRunAnimationControl rac(this);
		const CncGuiDisabler cgd(this);
		
		#define PCERR	CNC_CERR_FUNCT_A
		
		const wxString sel = m_homeDefintion->GetStringSelection();
		const char c = sel[0];
		switch (c) 
		{
			case 'a':	if ( cnc->moveZToMaxLimit() == false ) { PCERR("a: moveZToMaxLimit() failed"); break; } 
						if ( cnc->moveXToMinLimit() == false ) { PCERR("a: moveXToMaxLimit() failed"); break; } 
						if ( cnc->moveYToMinLimit() == false ) { PCERR("a: moveYToMaxLimit() failed"); break; } 
						break;
						
			case 'b':	if ( cnc->moveZToMaxLimit() == false ) { PCERR("b: moveZToMaxLimit() failed"); break; } 
						if ( cnc->moveXToMaxLimit() == false ) { PCERR("b: moveXToMaxLimit() failed"); break; } 
						if ( cnc->moveYToMaxLimit() == false ) { PCERR("b: moveYToMaxLimit() failed"); break; } 
						break;
						
			case 'c':	if ( cnc->moveZToMaxLimit() == false ) { PCERR("c: moveZToMaxLimit() failed"); break; } 
						if ( cnc->moveXToMid()      == false ) { PCERR("c: moveXToMid() failed"); break; } 
						if ( cnc->moveYToMid()      == false ) { PCERR("c: moveYToMid() failed"); break; } 
						break;
						
			case 'd':	if ( cnc->moveZToMid()      == false ) { PCERR("d: moveZToMid() failed"); break; } 
						if ( cnc->moveXToMid()      == false ) { PCERR("d: moveXToMid() failed"); break; } 
						if ( cnc->moveYToMid()      == false ) { PCERR("d: moveYToMid() failed"); break; }
						break;
						
			default:	CNC_CERR_FUNCT_A(" invalid selection!")
		}
	}
	
	#undef PCERR
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveXToMid(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( cnc && cnc->isReadyToRun() )
	{
		selectMonitorBookCncPanel();
		CNC_TRANSACTION_LOCK
		const CncRunAnimationControl rac(this);
		const CncGuiDisabler cgd(this);
		
		cnc->moveXToMid();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveYToMid(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( cnc && cnc->isReadyToRun() )
	{
		selectMonitorBookCncPanel();
		CNC_TRANSACTION_LOCK
		const CncRunAnimationControl rac(this);
		const CncGuiDisabler cgd(this);
		
		cnc->moveYToMid();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveZToMid(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( cnc && cnc->isReadyToRun() )
	{
		selectMonitorBookCncPanel();
		CNC_TRANSACTION_LOCK
		const CncRunAnimationControl rac(this);
		const CncGuiDisabler cgd(this);
		
		cnc->moveZToMid();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveToZeroXY(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( cnc && cnc->isReadyToRun() )
	{
		selectMonitorBookCncPanel();
		CNC_TRANSACTION_LOCK
		const CncRunAnimationControl rac(this);
		const CncGuiDisabler cgd(this);
	
		const wxString sel      = m_zeroMoveModeXY->GetStringSelection();
		const char mode         = sel[0];
		const CncDimensions dim = mode == 2 ? CncDimension2D : CncDimension1D;
		cnc->simpleMoveXYToZeroPos(dim);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveToZeroXYZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( cnc && cnc->isReadyToRun() )
	{
		selectMonitorBookCncPanel();
		CNC_TRANSACTION_LOCK
		const CncRunAnimationControl rac(this);
		const CncGuiDisabler cgd(this);
	
		const wxString sel = m_zeroMoveModeXYZ->GetStringSelection();
		const char mode    = sel[0];

		CncDimensions dim = CncDimension1D;
		switch ( mode ) 
		{
			case '2':	dim = CncDimension2D; break;
			case '3':	dim = CncDimension3D; break;
		}

		cnc->simpleMoveXYZToZeroPos(dim);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveToZeroZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( cnc && cnc->isReadyToRun() )
	{
		selectMonitorBookCncPanel();
		CNC_TRANSACTION_LOCK
		const CncRunAnimationControl rac(this);
		const CncGuiDisabler cgd(this);
	
		cnc->simpleMoveZToZeroPos();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveXToMin(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	if ( cnc && cnc->isReadyToRun() )
	{
		selectMonitorBookCncPanel();
		CNC_TRANSACTION_LOCK
		const CncRunAnimationControl rac(this);
		const CncGuiDisabler cgd(this);
		
		cnc->moveXToMinLimit();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveXToMax(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( cnc && cnc->isReadyToRun() )
	{
		selectMonitorBookCncPanel();
		CNC_TRANSACTION_LOCK
		const CncRunAnimationControl rac(this);
		const CncGuiDisabler cgd(this);
		
		cnc->moveXToMaxLimit();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveYToMin(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	if ( cnc && cnc->isReadyToRun() )
	{
		selectMonitorBookCncPanel();
		CNC_TRANSACTION_LOCK
		const CncRunAnimationControl rac(this);
		const CncGuiDisabler cgd(this);
		
		cnc->moveYToMinLimit();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveYToMax(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	if ( cnc && cnc->isReadyToRun() )
	{
		selectMonitorBookCncPanel();
		CNC_TRANSACTION_LOCK
		const CncRunAnimationControl rac(this);
		const CncGuiDisabler cgd(this);
		
		cnc->moveYToMaxLimit();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveZToMin(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	if ( cnc && cnc->isReadyToRun() )
	{
		selectMonitorBookCncPanel();
		CNC_TRANSACTION_LOCK
		const CncRunAnimationControl rac(this);
		const CncGuiDisabler cgd(this);
		
		cnc->moveZToMinLimit();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveZToMax(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	if ( cnc && cnc->isReadyToRun() )
	{
		selectMonitorBookCncPanel();
		CNC_TRANSACTION_LOCK
		const CncRunAnimationControl rac(this);
		const CncGuiDisabler cgd(this);
		
		cnc->moveZToMaxLimit();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveZToTop(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( cnc && cnc->isReadyToRun() )
	{
		selectMonitorBookCncPanel();
		CNC_TRANSACTION_LOCK
		const CncRunAnimationControl rac(this);
		const CncGuiDisabler cgd(this);
		
		cnc->moveZToTop();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::openConfigurationFile(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString tool;
	CncConfig::getGlobalCncConfig()->getEditorTool(tool);
	openFileExtern(tool, CncFileNameService::getConfigFileName());
}
///////////////////////////////////////////////////////////////////
void MainFrame::openExternalEditor(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString tool;
	CncConfig::getGlobalCncConfig()->getEditorTool(tool);
	openFileExtern(tool, _(""));
}
///////////////////////////////////////////////////////////////////
void MainFrame::openHexEditor(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString tool;
	CncConfig::getGlobalCncConfig()->getHexEditorTool(tool);
	openFileExtern(tool, _(""));
}
///////////////////////////////////////////////////////////////////
void MainFrame::openPyCam(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString tool;
	CncConfig::getGlobalCncConfig()->getPyCamTool(tool);
	openFileExtern(tool, _(""));
}
///////////////////////////////////////////////////////////////////
void MainFrame::openCalculator(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString cmd("calc");
	openFileExtern(cmd, _(""), false);
}
///////////////////////////////////////////////////////////////////
void MainFrame::enableTestControls(bool state) {
///////////////////////////////////////////////////////////////////
	// handle interval mode
	if ( state == false ) {
		wxString sel = m_testIntervalMode->GetStringSelection();
		char mode = sel[0];

		m_testCountY->Enable(mode == 'A');
		m_testCountZ->Enable(mode == 'A');
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectUAInboundPathList(wxDataViewEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_dvListCtrlSvgUAInboundPathList->HasSelection() ) {
		int sel = m_dvListCtrlSvgUAInboundPathList->GetSelectedRow();
		if ( inboundFileParser != NULL ) {
			m_dvListCtrlSvgUADetailInfo->DeleteAllItems();
			inboundFileParser->displayUserAgentDetailInfo(sel);
			m_dvListCtrlSvgUADetailInfo->Refresh();
			m_dvListCtrlSvgUADetailInfo->Update();
			
			wxVariant val;
			m_dvListCtrlSvgUAInboundPathList->GetValue(val, sel, 0);
		}
	}
	
	m_selectedUAInfo->SetValue("");
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectUADetailInfo(wxDataViewEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_dvListCtrlSvgUADetailInfo->HasSelection() ) {
		int sel = m_dvListCtrlSvgUADetailInfo->GetSelectedRow();
		m_selectedUAInfo->SetValue(m_dvListCtrlSvgUADetailInfo->GetTextValue(sel, 1));
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectUAUseDirectiveList(wxDataViewEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_dvListCtrlSvgUAUseDirective->HasSelection() ) {
		int sel = m_dvListCtrlSvgUAUseDirective->GetSelectedRow();
		m_selectedUAInfo->SetValue(m_dvListCtrlSvgUAUseDirective->GetTextValue(sel, 1));
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::onReloadMonitorPreview(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	const wxString fileName(m_currentInboundFilePreviewFileName->GetValue());
	const wxFileName fn(fileName);
	if ( fn.Exists() )
		openPreview(monitorFilePreview, fileName);
}
///////////////////////////////////////////////////////////////////
void MainFrame::openPreview(CncFilePreview* ctrl, const wxString& fn) {
///////////////////////////////////////////////////////////////////
	wxASSERT(ctrl);
	
	if      ( ctrl == mainFilePreview )			m_currentFileMangerPreviewFileName->ChangeValue(fn);
	else if ( ctrl == monitorFilePreview)		m_currentInboundFilePreviewFileName->ChangeValue(fn);
	
	if ( cncExtMainPreview != NULL ) 
	{
		if      ( ctrl == mainFilePreview )		cncExtMainPreview->setStatusTextRight(wxFileName(fn).GetFullName());
		else if ( ctrl == monitorFilePreview)	cncExtMainPreview->setStatusTextRight(m_inputFileName->GetValue());
	}
	
	//selectSecurePreviewView();
	
	CncTemplateFormat tf = getTemplateFormat(fn);
	switch ( tf )
	{
		case TplSvg:		ctrl->selectPreview(fn);
							ctrl->Refresh();
							break;
							
		case TplGcode:		ctrl->selectPreview(fn);
							ctrl->Refresh();
							break;
							
		case TplBinary:		{
								wxString tmpSourceFileName;
								if ( BinaryFileParser::extractSourceContentAsFile(fn, tmpSourceFileName) ) {
									openPreview(ctrl, tmpSourceFileName);
								}
								
								break;
							}
		
		default:			if ( fn.IsEmpty() == false )
								cnc::trc.logError(wxString::Format("Can't preview: '%s'", fn));
								
							fillFileDetails(false);
							return;
	}
	
	fillFileDetails(true, fn);
}
///////////////////////////////////////////////////////////////////
void MainFrame::openMainPreview(const wxString& fn) {
///////////////////////////////////////////////////////////////////
	MF_PRINT_LOCATION_CTX_FILE
	
	selectMainBookPreviewPanel();
	openPreview(mainFilePreview, fn);
	
	if (THE_CONTEXT->secureModeInfo.isActive == true )
		m_securePreviewBook->SetSelection(SecurePreviewBookSelection::VAL::LEFT_PREVIEW);
}
///////////////////////////////////////////////////////////////////
void MainFrame::openMonitorPreview(const wxString& fn) {
///////////////////////////////////////////////////////////////////
	MF_PRINT_LOCATION_CTX_FILE

	selectMonitorBookTemplatePanel();
	openPreview(monitorFilePreview, fn);
}
///////////////////////////////////////////////////////////////////
void MainFrame::openFileFromFileManager(const wxString& f) {
///////////////////////////////////////////////////////////////////
	selectMainBookSourcePanel();
	selectMonitorBookTemplatePanel();
	
	if ( saveTemplateOnDemand() == false )
	{
		prepareAndShowMonitorTemplatePreview();
		return;
	}
	
	wxFileName fn(f);
	m_inputFileName->SetValue(fn.GetFullName());
	m_inputFileName->SetHint(fn.GetFullPath());

	openFile();
	prepareAndShowMonitorTemplatePreview(sourceEditor->IsModified());
}
///////////////////////////////////////////////////////////////////
void MainFrame::openNavigatorFromGamepad() {
///////////////////////////////////////////////////////////////////
	const bool b1 = m_mainViewBook->GetSelection() == MainBookSelection::VAL::MANUEL_PANEL;
	
	if ( b1 )
	{
		const int max  = m_listbookManallyMotionControl->GetPageCount();
		const int next = ( m_listbookManallyMotionControl->GetSelection() + 1) % max;
		m_listbookManallyMotionControl->SetSelection(next);
	}
	else
	{
		selectMainBookManuelPanel();
	}
}
///////////////////////////////////////////////////////////////////
bool MainFrame::filePreviewListLeave() {
///////////////////////////////////////////////////////////////////
	if (THE_CONTEXT->secureModeInfo.isActive == true )
		m_securePreviewBook->SetSelection(SecurePreviewBookSelection::VAL::RIGHT_PREVIEW);
		
	if ( m_keepFileManagerPreview->IsChecked() )
		return false;
	
	if ( CncAsyncKeyboardState::isControlPressed() )
		return false;
	
	if ( cncExtMainPreview != NULL && cncExtMainPreview->IsShownOnScreen() == true ) {
		cncExtMainPreview->selectView(CncExternalViewBox::Preview::TEMPLATE);
		return false;
	}
	
	if ( m_mainViewBook->GetSelection() != MainBookSelection::VAL::PREVIEW_PANEL )
		return false;
		
	selectMainBookSourcePanel();
	
	return true;
}
///////////////////////////////////////////////////////////////////
void MainFrame::onChangePreviewMode(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	const bool isExtViewActive = m_externFileManagerPreview->IsChecked();
	
	//m_keepFileManagerPreview->SetValue( isExtViewActive);
	//m_keepFileManagerPreview->Enable  (!isExtViewActive);
		
	if ( cncExtMainPreview != NULL ) {
		
		// deselect main preview if active
		if ( m_mainViewBook->GetSelection() == MainBookSelection::VAL::PREVIEW_PANEL )
			selectMainBookSourcePanel();
		
		// deselect monitor preview if active
		if ( m_monitorViewBook->GetSelection() == MonitorBookSelection::VAL::TEMPLATE_PANEL )
			selectMonitorBookCncPanel();
		
		// prepare extern preview
		cncExtMainPreview->selectView(CncExternalViewBox::Preview::TEMPLATE);
		cncExtMainPreview->Show(isExtViewActive);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectMainBookSourcePanel(int sourcePageToSelect) {
///////////////////////////////////////////////////////////////////
	m_mainViewBook->SetSelection(MainBookSelection::VAL::SOURCE_PANEL);
	
	if ( sourcePageToSelect < 0 || sourcePageToSelect > (int)(m_listbookSource->GetPageCount() - 1) )
		sourcePageToSelect = SourceBookSelection::VAL::EDITOR;
		
	m_listbookSource->SetSelection(sourcePageToSelect);
	
	if ( sourcePageToSelect == SourceBookSelection::VAL::EDITOR )
		m_templateNotebook->SetSelection(TemplateBookSelection::VAL::SOURCE_PANEL);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectMainBookPreviewPanel() {
///////////////////////////////////////////////////////////////////
	MF_PRINT_LOCATION_CTX_FILE
	
	if ( m_externFileManagerPreview->IsChecked() == false ) {
		m_mainViewBook->SetSelection(MainBookSelection::VAL::PREVIEW_PANEL);
		
	} else {
		
		cncExtMainPreview->selectView(CncExternalViewBox::Preview::FILE);
		if ( cncExtMainPreview->IsShownOnScreen() == false )
			cncExtMainPreview->Show();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectMainBookSetupPanel() {
///////////////////////////////////////////////////////////////////
	m_mainViewBook->SetSelection(MainBookSelection::VAL::SETUP_PANEL);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectMainBookReferencePanel() {
///////////////////////////////////////////////////////////////////
	m_mainViewBook->SetSelection(MainBookSelection::VAL::REFERENCE_PANEL);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectMainBookManuelPanel() {
///////////////////////////////////////////////////////////////////
	m_mainViewBook->SetSelection(MainBookSelection::VAL::MANUEL_PANEL);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectMainBookTestPanel() {
///////////////////////////////////////////////////////////////////
	m_mainViewBook->SetSelection(MainBookSelection::VAL::TEST_PANEL);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectMonitorBookCncPanel() {
///////////////////////////////////////////////////////////////////
	if ( cncExtViewBoxCluster->isViewDetached(CncExternalViewBoxCluster::Node::EVB_Monitor) == true )
	{
		cncExtViewBoxCluster->bringViewOnTop(CncExternalViewBoxCluster::Node::EVB_Monitor);
	}
	else 
	{
		m_monitorViewBook->SetSelection(MonitorBookSelection::VAL::CNC_PANEL);
		m_outboundNotebook->SetSelection(OutboundSelection::VAL::MOTION_MONITOR_PANAL);
	}
	
	// to release the opengl context switch
	motionMonitor->Update();
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectMonitorBookSetupSummaryPanel() {
///////////////////////////////////////////////////////////////////
	m_monitorViewBook->SetSelection(MonitorBookSelection::VAL::CNC_PANEL);
	m_outboundNotebook->SetSelection(OutboundSelection::VAL::SUMMARY_PANEL);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectMonitorBookTemplatePanel() {
///////////////////////////////////////////////////////////////////
	if ( m_externFileManagerPreview->IsChecked() == false ) 
	{
		m_monitorViewBook->SetSelection(MonitorBookSelection::VAL::TEMPLATE_PANEL);
	}
	else
	{
		selectMonitorBookCncPanel();
		
		cncExtMainPreview->selectView(CncExternalViewBox::Preview::TEMPLATE);
		if ( cncExtMainPreview->IsShownOnScreen() == false )
			cncExtMainPreview->Show();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectParsingSynopsisTrace() {
///////////////////////////////////////////////////////////////////
	m_listbookSource->SetSelection(SourceBookSelection::VAL::CONTEXT);
	contextSummaryPanel->selectParsingSynopsis();
	
	if ( THE_CONTEXT->secureModeInfo.isActive )
	{
		CncTemplateContextSummaryDialog dlg(this);
		dlg.update();
		dlg.ShowModal();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::toggleAuiPane(wxWindow* pane, wxMenuItem* menu, bool update) {
///////////////////////////////////////////////////////////////////
	if ( pane == NULL )
		return;
	
	if ( GetAuimgrMain()->GetPane(pane).IsShown() )	hideAuiPane(pane, menu);
	else											showAuiPane(pane, menu);
	
	if ( update )
		GetAuimgrMain()->Update();
}
///////////////////////////////////////////////////////////////////
void MainFrame::showAuiPane(wxWindow* pane, wxMenuItem* menu, bool update) {
///////////////////////////////////////////////////////////////////
	if ( pane == NULL )
		return;
		
	GetAuimgrMain()->GetPane(pane).Show();
	
	if ( menu != NULL )
		menu->Check(true);
		
	if ( update )
		GetAuimgrMain()->Update();
}
///////////////////////////////////////////////////////////////////
void MainFrame::hideAuiPane(wxWindow* pane, wxMenuItem* menu, bool update) {
///////////////////////////////////////////////////////////////////
	if ( pane == NULL )
		return;
		
	GetAuimgrMain()->GetPane(pane).Hide();
	GetAuimgrMain()->GetPane(pane).DestroyOnClose(false);
	pane->Close(true);
	
	if ( serialSpyPanel && pane == getAUIPaneByName("SerialSpy") )
		serialSpyPanel->enableSerialSpy(false);
	
	if ( menu != NULL )
		menu->Check(false);
		
	if ( update )
		GetAuimgrMain()->Update();
}
///////////////////////////////////////////////////////////////////
wxWindow* MainFrame::getAUIPaneByName(const wxString& name) {
///////////////////////////////////////////////////////////////////
	if      ( name == "Toolbar" ) 				return m_auibarMain;
	else if ( name == "SourceView")				return m_winMainView;
	else if ( name == "Logger")					return m_winLoggerView;
	else if ( name == "Outbound")				return m_winMonitorView;
	else if ( name == "TemplateManager")		return m_winFileView;
	else if ( name == "StatusBar")				return m_statusBar;
	else if ( name == "SerialSpy")				return m_serialSpyView;
	else if ( name == "UnitCalculator")			return m_svgUnitCalulator;
	else if ( name == "Debugger")				return m_debuggerView;
	else if ( name == "AccelerationMonitor")	return m_accelaerationMonitorView;
	else if ( name == "CncCoordinates")			return m_cncCoordinateView;
	else if ( name == "CncGamepadSpy")			return m_cncGamepadSpy;
	else if ( name == "SecureRunPanel")			return m_secureRunPanel;
	
	return NULL;
}
///////////////////////////////////////////////////////////////////
wxMenuItem* MainFrame::getAUIMenuByName(const wxString& name) {
///////////////////////////////////////////////////////////////////
	if      ( name == "Toolbar" ) 				return m_miToolbar;
	else if ( name == "StatusBar")				return m_miViewStatusbar;
	else if ( name == "SourceView")				return m_miViewMainView;
	else if ( name == "Logger")					return m_miViewLogger;
	else if ( name == "Outbound")				return m_miViewMonitor;
	else if ( name == "TemplateManager")		return m_miViewTemplateManager;
	else if ( name == "SerialSpy")				return m_miViewSpy;
	else if ( name == "UnitCalculator")			return m_miViewUnitCalculator;
	else if ( name == "Debugger")				return m_miViewDebugger;
	else if ( name == "AccelerationMonitor")	return m_miViewAccelMonitor;
	else if ( name == "CncCoordinates")			return m_miViewCoordinates;
	else if ( name == "CncGamepadSpy")			return m_miViewGamepadSpy;
	
	return NULL;
}
///////////////////////////////////////////////////////////////////
void MainFrame::decorateViewMenu() {
///////////////////////////////////////////////////////////////////
	wxAuiPaneInfoArray panes = m_auimgrMain->GetAllPanes();
	for (unsigned int i = 0; i < panes.GetCount(); ++i) {
		wxMenuItem* mi = getAUIMenuByName(panes.Item(i).name);
		if ( mi )
			mi->Check(panes.Item(i).window->IsShown());
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::toggleAuiPane(const wxString& name, bool update) {
///////////////////////////////////////////////////////////////////
	wxWindow* w = getAUIPaneByName(name);
	if ( w == NULL ) {
		std::cerr << "MainFrame::toggleAuiPane: Invalid pane window name: " << name << std::endl;
		return;
	}
	wxMenuItem* m = getAUIMenuByName(name);
	if ( m == NULL ) {
		std::cerr << "MainFrame::toggleAuiPane: Invalid pane window name: " << name << std::endl;
		return;
	}
	
	toggleAuiPane(w, m, update);
}
///////////////////////////////////////////////////////////////////
void MainFrame::showAuiPane(const wxString& name, bool update) {
///////////////////////////////////////////////////////////////////
	wxWindow* w = getAUIPaneByName(name);
	if ( w == NULL ) {
		std::cerr << "MainFrame::toggleAuiPane: Invalid pane window name: " << name << std::endl;
		return;
	}
	wxMenuItem* m = getAUIMenuByName(name);
	if ( w == NULL ) {
		std::cerr << "MainFrame::toggleAuiPane: Invalid pane window name: " << name << std::endl;
		return;
	}
	
	showAuiPane(w, m, update);
}
///////////////////////////////////////////////////////////////////
void MainFrame::hideAuiPane(const wxString& name, bool update) {
///////////////////////////////////////////////////////////////////
	wxWindow* w = getAUIPaneByName(name);
	if ( w == NULL ) {
		std::cerr << "MainFrame::toggleAuiPane: Invalid pane window name: " << name << std::endl;
		return;
	}
	wxMenuItem* m = getAUIMenuByName(name);
	if ( w == NULL ) {
		std::cerr << "MainFrame::toggleAuiPane: Invalid pane window name: " << name << std::endl;
		return;
	}
	
	hideAuiPane(w, m, update);
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewToolbar(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	toggleAuiPane(m_auibarMain, m_miToolbar);
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewStatusbar(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	toggleAuiPane("StatusBar");
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewMainView(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	toggleAuiPane("SourceView");
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewTemplateManager(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	toggleAuiPane("TemplateManager");
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewLogger(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	toggleAuiPane("Logger");
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewMonitor(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	toggleAuiPane("Outbound");
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewCoordinates(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	toggleAuiPane("CncCoordinates");
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewGamepadSpy(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	toggleAuiPane("CncGamepadSpy");
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewSpy(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	toggleAuiPane("SerialSpy");
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewUnitCalculator(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	toggleAuiPane("UnitCalculator");
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewDebugger(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	toggleAuiPane("Debugger");
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewAccelerationMonitor(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	toggleAuiPane("AccelerationMonitor");
}
///////////////////////////////////////////////////////////////////
void MainFrame::closeAuiPane(wxAuiManagerEvent& evt) {
///////////////////////////////////////////////////////////////////
	wxMenuItem* menu = getAUIMenuByName(evt.pane->name);
	if ( menu != NULL )
		menu->Check(!evt.pane->window->IsShown());

	if ( evt.pane->window == m_serialSpyView )
		serialSpyPanel->enableSerialSpy(false);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onPerspectiveTimer(wxTimerEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( perspectiveTimer->IsRunning() == true )
		perspectiveTimer->Stop();
		
	if ( m_miToolbar->IsChecked() )
		showAuiPane("Toolbar");
		
	if ( m_miViewStatusbar->IsChecked() )
		showAuiPane("StatusBar");
		
	//GetAuimgrMain()->Update();
}
///////////////////////////////////////////////////////////////////
void MainFrame::maximizeAuiPane(wxAuiManagerEvent& event) {
///////////////////////////////////////////////////////////////////
	// always do this
	perspectiveHandler.logCurrentPerspective();
	
	if ( event.pane->window == m_winMonitorView || event.pane->window == m_winMainView ) {
		perspectiveTimer->Start(20);
	}

	event.Skip(false);
}
///////////////////////////////////////////////////////////////////
void MainFrame::activateAuiPane(wxAuiManagerEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::buttonAuiPane(wxAuiManagerEvent& event) {
///////////////////////////////////////////////////////////////////
	enum { 
		AUI_CLOSE_BTN_ID 	= 101,
		AUI_MIN_MAX_BTN_ID 	= 102,
		AUI_PIN_BTN_ID 		= 104
	};
	
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::restoreAuiPane(wxAuiManagerEvent& event) {
///////////////////////////////////////////////////////////////////
	// prevent the standard handling 
	// the restore handling will be done be the code below
	event.Veto(true);
	
	perspectiveHandler.restoreLoggedPerspective();
}
///////////////////////////////////////////////////////////////////
void MainFrame::renderAuiPane(wxAuiManagerEvent& event) {
///////////////////////////////////////////////////////////////////
	// do nothing else!
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::hideAllAuiPanes(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	hideAllAuiPanes();
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewAllAuiPanes(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	viewAllAuiPanes();
}
///////////////////////////////////////////////////////////////////
void MainFrame::hideAllAuiPanes(bool update) {
///////////////////////////////////////////////////////////////////
	wxAuiPaneInfoArray panes = m_auimgrMain->GetAllPanes();
	for (unsigned int i = 0; i < panes.GetCount(); ++i)
		hideAuiPane( panes.Item(i).name, false);
		
	if ( update == true )
		GetAuimgrMain()->Update();
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewAllAuiPanes() {
///////////////////////////////////////////////////////////////////
	wxAuiPaneInfoArray panes = m_auimgrMain->GetAllPanes();
	for (unsigned int i = 0; i < panes.GetCount(); ++i) {
		const wxString name(panes.Item(i).name);
		if ( name != "SecureRunPanel" )
			showAuiPane(name, false);
	}
	
	GetAuimgrMain()->Update();
}
///////////////////////////////////////////////////////////////////
void MainFrame::disableSlider(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip(false);
}
///////////////////////////////////////////////////////////////////
void MainFrame::outboundBookChanged(wxNotebookEvent& event) {
///////////////////////////////////////////////////////////////////
	unsigned int sel = event.GetSelection();
	
	if ( (wxWindow*)event.GetEventObject() == m_outboundNotebook ) {
		switch ( sel ) {
			case OutboundSelection::VAL::MOTION_MONITOR_PANAL:
			{
				motionMonitor->synchronizeClientId();
				
				if ( cnc )
					cnc->updatePreview3D();
					
				break;
			}
			case OutboundSelection::VAL::PREPOCESSOR_PANAL:
			{
				cncPreprocessor->updateContent();
				break;
			}

		}
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::changeMonitorListBook(wxListbookEvent& event) {
///////////////////////////////////////////////////////////////////
	unsigned int sel = event.GetSelection();
	
	if ( (wxWindow*)event.GetEventObject() == m_listbookMonitor ) {
		switch ( sel ) {
			
			case OutboundMonitorSelection::VAL::CNC_SETTER_PANEL:
			{
				updateSetterList();
				break;
			}
			
			case OutboundMonitorSelection::VAL::MOTION_VERTEX_TRACE:
			{
				CncRunAnimationControl rac(this);
				motionMonitor->initVertexListCtr();
				break;
			}
		}
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::outboundBookChanging(wxNotebookEvent& event) {
///////////////////////////////////////////////////////////////////
	// currently nothing to do
	// unsigned int sel = event.GetSelection();
	// if ( (wxWindow*)event.GetEventObject() == m_outboundNotebook ) {
	// }
}
///////////////////////////////////////////////////////////////////
void MainFrame::displayUserAgent(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	updateMonitoring();
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateToolControls(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	updateMonitoring();
}
///////////////////////////////////////////////////////////////////
void MainFrame::createAnimationControl() {
///////////////////////////////////////////////////////////////////
	if ( pngAnimation != NULL )
		delete pngAnimation;
		
	pngAnimation = NULL;
	
	wxBitmap bmp = ImageLib16().Bitmap("ANIMATION_SPRITE");
	if ( bmp.IsOk() )
		pngAnimation = new wxPNGAnimation(m_activityPanel, bmp, wxHORIZONTAL, wxSize(80, 7), 0);
}
///////////////////////////////////////////////////////////////////
void MainFrame::startAnimationControl() {
///////////////////////////////////////////////////////////////////
	if ( pngAnimation != NULL ) 
	{
		if ( pngAnimation->IsRunning() )
			pngAnimation->Stop();
			
		pngAnimation->Start(50);
		pngAnimation->Update();
	}
	
	wxColor color(255,128,128);
	m_cmdDuration->SetForegroundColour(color);
	m_cmdDuration->Refresh();
	
	THE_CONTEXT->timestamps.logTotalTimeStart();
}
///////////////////////////////////////////////////////////////////
void MainFrame::stopAnimationControl() {
///////////////////////////////////////////////////////////////////
	m_cmdDuration->SetForegroundColour(*wxBLACK);
	m_cmdDuration->Refresh();
	
	if ( pngAnimation != NULL ) 
	{
		if ( pngAnimation->IsRunning() ) 
		{
			pngAnimation->Stop();
			pngAnimation->Update();

			THE_CONTEXT->timestamps.logTotalTimeEnd();
		}
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::dclickDurationCount(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	const wxString msg(m_cmdDuration->GetToolTipText());
	if ( msg.IsEmpty() == false ) {
		
		CncLastProcessingTimestampSummary dlg (this);
		wxStringTokenizer tokenizer(msg, "\n");
		
		// token rest
		while ( tokenizer.HasMoreTokens() ) {
			wxString token = tokenizer.GetNextToken();
			if ( token.IsEmpty() )
				continue;
				
			token.Replace("*", "", true);
			const wxString key(token.BeforeFirst(':').Trim().Trim(true));
			const wxString val(token.AfterFirst(':').Trim().Trim(true));
			
			dlg.addTimestamp(key, val);
		}
		
		dlg.ShowModal();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::decorateOutboundSaveControls(bool state) { 
///////////////////////////////////////////////////////////////////
	m_miSaveEmuOutput->Enable(state);
	m_btSaveOutboundAsTemplate1->Enable(state);
	m_btSaveOutboundAsTemplate2->Enable(state);
	
	if ( state == true ) {
		wxString outboundFile(cnc->getPortName());
		decorateOutboundEditor(outboundFile);
		
	} else {
		decorateOutboundEditor();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::decorateSearchButton() {
///////////////////////////////////////////////////////////////////
	wxBitmap bmp = ImageLib16().Bitmap("BMP_OK16");
	if ( bmp.IsOk() )
		m_sourceEditSearchState->SetBitmap(bmp);
}
///////////////////////////////////////////////////////////////////
int MainFrame::getSvgEditSearchFlags() {
///////////////////////////////////////////////////////////////////
	int ret = 0;
	//wxSTC_FIND_WORDSTART
	if ( m_tbCaseSensitive->GetValue() == true ) ret |= wxSTC_FIND_MATCHCASE;
	if ( m_tbWholeWord->GetValue() == true ) ret |= wxSTC_FIND_WHOLEWORD;
	if ( m_tbRegEx->GetValue() == true ) ret |= wxSTC_FIND_REGEXP;
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateSvgSearchControl() {
///////////////////////////////////////////////////////////////////
	wxString find = m_sourceEditSearch->GetValue();
	
	int pos = m_sourceEditSearch->FindString(find);
	if ( pos >= 0 ) {
		m_sourceEditSearch->Delete((unsigned int)pos);
	}
	
	m_sourceEditSearch->Insert(find, 0);
	m_sourceEditSearch->ChangeValue(find);
}
///////////////////////////////////////////////////////////////////
void MainFrame::svgEditFind(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_sourceEditSearch->GetValue().IsEmpty() )
		return;

	wxString find = m_sourceEditSearch->GetValue();
	fixRegexParen(find);
	
	updateSvgSearchControl();
	
	sourceEditor->SetCurrentPos(sourceEditor->GetCurrentPos() + m_sourceEditSearch->GetValue().Len() + 1);
	sourceEditor->SearchAnchor();
	int ret = sourceEditor->SearchNext(getSvgEditSearchFlags(), find);
	
	if ( ret == wxNOT_FOUND ) {
		sourceEditor->ClearSelections();
		sourceEditor->SearchAnchor();
	} else {
		ensureSvgEditLineIsVisible(ret);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::svgEditFindPrev(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_sourceEditSearch->GetValue().IsEmpty() )
		return;
		
	wxString find = m_sourceEditSearch->GetValue();
	fixRegexParen(find);

	updateSvgSearchControl();
	
	int ncp = sourceEditor->GetCurrentPos() - m_sourceEditSearch->GetValue().Len() - 1;
	if( ncp < 0 )
		ncp = sourceEditor->GetLastPosition();
		
	sourceEditor->SetCurrentPos(ncp);
	sourceEditor->SearchAnchor();
	int ret = sourceEditor->SearchPrev(getSvgEditSearchFlags(), find);
	
	if ( ret == wxNOT_FOUND ) {
		sourceEditor->ClearSelections();
		sourceEditor->SearchAnchor();
	} else {
		ensureSvgEditLineIsVisible(ret);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::svgEditSearchTextChanged(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	searchConditionsChanged();
}
///////////////////////////////////////////////////////////////////
void MainFrame::searchConditionsChanged() {
///////////////////////////////////////////////////////////////////
	wxString find = m_sourceEditSearch->GetValue();
	wxString toolTip("");
	wxBitmap bmp = ImageLib16().Bitmap("BMP_OK16");
	m_sourceEditSearchState->SetBitmap(bmp);
	
	fixRegexParen(find);
	sourceEditor->SearchAnchor();
	int ret = sourceEditor->SearchNext(getSvgEditSearchFlags(), find);
	
	if( ret == wxNOT_FOUND ) {
		sourceEditor->ClearSelections();
		bmp = ImageLib16().Bitmap("BMP_WARNING16");
		m_sourceEditSearchState->SetBitmap(bmp);

		toolTip << "'";
		toolTip << find;
		toolTip << "' not found";
		
	} else {
		ensureSvgEditLineIsVisible(ret);
	}
	
	m_sourceEditSearchState->Show();
	m_sourceEditSearchState->Refresh();
	m_sourceEditSearchState->SetToolTip(toolTip);
	m_sourceEditStatus->SetLabel(toolTip);
}
///////////////////////////////////////////////////////////////////
wxString& MainFrame::fixRegexParen(wxString& find) {
///////////////////////////////////////////////////////////////////
	if( getSvgEditSearchFlags() & wxSTC_FIND_REGEXP) {
		// Scintilla's REGEX group markers are \( and \)
		// while wxRegEx is usig bare ( and ) and the escaped version for
		// non regex manner
		find.Replace("\\(", "/<!@#$");
		find.Replace("\\)", "/>!@#$");
		find.Replace("(", "<!@#$");
		find.Replace(")", ">!@#$");

		find.Replace("/<!@#$", "(");
		find.Replace("/>!@#$", ")");
		find.Replace("<!@#$", "\\(");
		find.Replace(">!@#$", "\\)");
	}
	
	return find;
}
///////////////////////////////////////////////////////////////////
void MainFrame::svgEditSelected(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	// does currently nothing
}
///////////////////////////////////////////////////////////////////
void MainFrame::ensureSvgEditLineIsVisible(int line) {
///////////////////////////////////////////////////////////////////
	if( line == wxNOT_FOUND ) {
		line = sourceEditor->LineFromPosition(sourceEditor->GetSelectionStart());
	}
	
	int linesOnScreen = sourceEditor->LinesOnScreen();
	if(!((line > sourceEditor->GetFirstVisibleLine()) && (line < (sourceEditor->GetFirstVisibleLine() + linesOnScreen)))) {
		// To place our line in the middle, the first visible line should be
		// the: line - (linesOnScreen / 2)
		int firstVisibleLine = line - (linesOnScreen / 2);
		if( firstVisibleLine < 0 ) {
			firstVisibleLine = 0;
		}
		sourceEditor->SetFirstVisibleLine(firstVisibleLine);
	}
	
	sourceEditor->EnsureVisible(line);
	sourceEditor->ScrollToColumn(0);
	
	int xScrollPosBefore = sourceEditor->GetScrollPos(wxHORIZONTAL);
	sourceEditor->EnsureCaretVisible();
	int xScrollPosAfter = sourceEditor->GetScrollPos(wxHORIZONTAL);
	
	if( xScrollPosBefore != xScrollPosAfter ) {
		// EnsureCaretVisible scrolled the page
		// scroll it a bit more
		int scrollToPos = sourceEditor->GetSelectionStart();
		if(	scrollToPos != wxNOT_FOUND	) {
			sourceEditor->ScrollToColumn(sourceEditor->GetColumn(scrollToPos));
		}
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::toogleSvgEditSearchFlag(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxWindow* wnd = (wxWindow*)event.GetEventObject();
	
	if ( wnd == NULL )
		return;
		
	bool state = ((wxBitmapToggleButton*)wnd)->GetValue();
	wxColor color(227,227,227);
	
	if ( state == true )
		color.Set(191,205,219);
		
	wnd->SetBackgroundColour(color);
	wnd->Update();
	wnd->Refresh();
	
	searchConditionsChanged();
}
///////////////////////////////////////////////////////////////////
bool MainFrame::openFileExtern(const wxString& tool, const wxString& file, bool checkToolExists) {
///////////////////////////////////////////////////////////////////
	CncRunAnimationControl rac(this);
	
	const bool ret = GblFunc::executeExternalProgram(tool, file, checkToolExists);
	if ( ret == true )
		waitActive(1500);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void MainFrame::toggleTemplateShowEOL(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	sourceEditor->SetViewEOL(!m_btSvgToggleShowEOL->GetValue());
}
///////////////////////////////////////////////////////////////////
void MainFrame::toggleTemplateShowWs(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	sourceEditor->SetViewWhiteSpace(!m_btSvgToggleShowWS->GetValue());
}
///////////////////////////////////////////////////////////////////
void MainFrame::toggleTemplateWordWrapMode(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	sourceEditor->SetWrapMode(!m_btSvgToggleWordWrap->GetValue());
}
///////////////////////////////////////////////////////////////////
void MainFrame::toggleAutoSaveTplOnProcess(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	setAutoSaveMode(m_btSvgToggleAutoSaveTplOnProcess->GetValue());
}
///////////////////////////////////////////////////////////////////
void MainFrame::toggleTryToSelectClientIdFromEditor(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	const bool flag = m_btSvgToggleTryToSelectClientId->GetValue();
	
	if ( sourceEditor != NULL ) 
		sourceEditor->setTryToSelectFlag(flag);
	
	if ( motionMonitor != NULL ) {
		
		if ( flag == true )			motionMonitor->dimAllVectiesDown();
		else 						motionMonitor->normalizeAllSelectionEffects();
		
		motionMonitor->update(true);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::toggleOutboundEditorWordWrap(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	outboundEditor->SetWrapMode(!m_btToggleOutboundEditorWordWrap->GetValue());
}
///////////////////////////////////////////////////////////////////
const char* MainFrame::getCurrentPortName(wxString& ret) {
///////////////////////////////////////////////////////////////////
	ret.assign("Unknown Port Name");
	if ( cnc != NULL ) {
		wxString cn(cnc->getClassName());
		wxString pn(cnc->getPortName());
		
		ret.assign(wxString::Format("%s", cn));
		
		if ( pn.IsEmpty() == false ) {
			wxFileName fn(pn);
			if ( fn.HasExt() ) {
				pn.assign(fn.GetFullName());
				
			} else {
				pn.Replace("\\","",true);
				pn.Replace(".","",true);
				
			}
			
			ret.append(wxString::Format("::%s", pn));
		}
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void MainFrame::decorateRunButton() {
///////////////////////////////////////////////////////////////////
	wxString toolTip;
	toolTip = isPause() ? "Resume " : "Run";
	m_rcRun->SetToolTip(toolTip);
}
///////////////////////////////////////////////////////////////////
void MainFrame::determineRunMode() {
///////////////////////////////////////////////////////////////////
	if ( isDebugMode ) {
		m_svgParseMode->SetLabel("Debug");
		m_svgParseMode->SetForegroundColour(wxColor(255,128,128));
	} else {
		m_svgParseMode->SetLabel("Release");
		m_svgParseMode->SetForegroundColour(wxColor(0,162,232));
	}
	
	m_svgParseMode->Refresh();

	decorateRunButton();
	
	if ( m_externFileManagerPreview->IsChecked() == true && cncExtMainPreview != NULL )
		cncExtMainPreview->selectView(CncExternalViewBox::Preview::TEMPLATE);
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcDebugConfig(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	perspectiveHandler.ensureDebugPerspectiveMinimal();
	
	if ( m_debuggerPropertyManagerGrid->GetPageCount() > 0 )
		m_debuggerPropertyManagerGrid->SelectPage(0);
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcDryRun(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	// do this already here, because this is true a part of processTemplate(), 
	// but is makes a clearer work flow if the state is checked before anything is started 
	if ( checkReferencePositionState() == false )
		return;
	
	if ( cnc->dryRunAvailable() )
	{
		const unsigned int prevRunCount			= THE_CONTEXT->templateContext->getRunCount();
		const unsigned int prevValidRunCount	= THE_CONTEXT->templateContext->getValidRunCount();
		
		const wxString starMessage (wxString::Format("~~~ Dry run started: %s", wxDateTime::Now().FormatISOTime()));
		wxString       finalMessage(                 "=== Dry run was successful");
		bool ret = false;
		
		CNC_CLOG_A(starMessage)
		// streamer redirect section
		{
			StdStreamRedirector srd(getDryRunLogger());
			CNC_CLOG_A(starMessage)
			
			cnc->switchRunMode(CncControl::RunMode::M_DryRun);
			ret = processTemplate();
			cnc->switchRunMode(CncControl::RunMode::M_RealRun);
			
			// error handling 
			if ( ret == false && THE_CONTEXT->templateContext->getRunCount() <= prevRunCount )
			{
				// in this case processTemplate() fails before the run was started by previous checks
				// no further checks necessary here . . . 
				finalMessage.assign("=== Dry run was failed");
			}
			else
			{
				if ( THE_CONTEXT->templateContext->getRunCount() > prevRunCount ) 
				{
					// only if a new run was registered
					if ( (prevValidRunCount + 1) != THE_CONTEXT->templateContext->getValidRunCount() )
					{
						// the valid count was not increased
						finalMessage.assign("=== Dry run was failed");
						ret           = false;
					}
				}
				else
				{
					// in this case normally the reference position check failed and 
					// a second loop was initiated
					finalMessage.clear();
					ret = true;
				}
			}
			
			// display processing info on try run logger
			if ( finalMessage.IsEmpty() == false )
			{
				if ( ret == false )	{ CNC_CERR_A(finalMessage); SET_RESULT_FOR_LAST_FILLED_LOGGER_ROW( CNC_RESULT_ERROR_STR ) }
				else				{ CNC_CLOG_A(finalMessage); SET_RESULT_FOR_LAST_FILLED_LOGGER_ROW( CNC_RESULT_OK_STR ) }
			}
		}
		
		// display processing info on standard logger
		if ( ret == false )
		{
			decorateDryRunState(cncError);
			CNC_CERR_A(finalMessage)
			SET_RESULT_FOR_LAST_FILLED_LOGGER_ROW( CNC_RESULT_ERROR_STR )
			
			openTemplateContextView();
		}
		else
		{
			if ( finalMessage.IsEmpty() == false )
			{
				decorateDryRunState(cncOk);
				CNC_CLOG_A(finalMessage)
				SET_RESULT_FOR_LAST_FILLED_LOGGER_ROW( CNC_RESULT_OK_STR )
			}
			else
			{
				decorateDryRunState(cncUnknown);
			}
		}
	}
	
	motionMonitor->update(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::openTemplateContextView() {
///////////////////////////////////////////////////////////////////
	// always do this, because the CncTemplateContextSummaryDialog 
	// swap these controls
	m_listbookSource->SetSelection(SourceBookSelection::VAL::CONTEXT);
	contextSummaryPanel->selectSummary();

	if ( THE_CONTEXT->secureModeInfo.isActive )
	{
		CncTemplateContextSummaryDialog dlg(this);
		dlg.update();
		dlg.ShowModal();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::decorateDryRunState(CncState state) {
///////////////////////////////////////////////////////////////////
	wxString bmpName;
	
	switch ( state )
	{ 
		case cncUnknown:	bmpName.assign("BMP_TRAFFIC_LIGHT_DEFAULT");
							break;
		case cncOk:			bmpName.assign("BMP_TRAFFIC_LIGHT_GREEN");
							break;
		case cncWarning:	bmpName.assign("BMP_TRAFFIC_LIGHT_YELLOW");
							break;
		case cncError:		bmpName.assign("BMP_TRAFFIC_LIGHT_RED");
							break;
	}
	
	if ( bmpName.IsEmpty() == false )
	{
		secureCtrlPanel->GetBtTemplateContextSec()->SetBitmap(ImageLib24().Bitmap(bmpName));
		secureCtrlPanel->GetBtTemplateContextSec()->SetBitmapDisabled(ImageLib24().Bitmap("BMP_TRAFFIC_LIGHT_DEFAULT"));
		secureCtrlPanel->GetBtTemplateContextSec()->Refresh();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcRun() {
///////////////////////////////////////////////////////////////////
	determineRunMode();
	
	if ( THE_CONTEXT->secureModeInfo.useIt == true && m_secureRunPanel->IsShownOnScreen() == false )
	{
		activateSecureMode(!THE_CONTEXT->secureModeInfo.isActive);
		return;
	}
	
	// ensure the monitor is visible, especially if isPause == true
	// because then the processing should be resume
	if ( THE_CONTEXT->secureModeInfo.isActive == false )
	{
		showAuiPane("Outbound");
		selectMonitorBookCncPanel();
	}
	else 
	{
		selectSecureMonitorView();
	}

	// toggle only the pause flag
	if ( isPause() == true ) 
	{
		rcPause();
		return;
	}
	
	// perform a run
	// Store the current interval
	const int interval = THE_CONTEXT->getUpdateInterval();
	
	if ( isDebugMode == true )
	{
		// check if the current port is a cnc and no emulator port
		if ( cnc->getPortType() == CncPORT )
		{
			wxString msg("Do you really want to debug a COM port?");
			wxMessageDialog dlg(this, msg, _T("Port Check . . . "), 
			                    wxOK|wxCANCEL|wxCENTRE|wxICON_QUESTION);
			
			if ( dlg.ShowModal() == wxID_CANCEL )
				return;
		}
		
		// bring the debug controls in front
		perspectiveHandler.ensureDebugPerspectiveMinimal();
		
		// to see each line during the debug session
		THE_CONTEXT->setUpdateInterval(1);
	}
	else
	{
		if ( THE_CONTEXT->secureModeInfo.isActive == false ) 
			perspectiveHandler.ensureRunPerspectiveMinimal();
	}
	
	// process
	CNC_CLOG_A("")
	
	processTemplate();
	
	// restore the interval
	THE_CONTEXT->setUpdateInterval(interval);
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcDebug(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	isDebugMode = true;
	rcRun();
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcRun(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	isDebugMode = false;
	rcRun();
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcPause() {
///////////////////////////////////////////////////////////////////
	// first update the state context
	THE_CONTEXT->togglePause();
	
	// update gui
	enableRunControls(THE_CONTEXT->isPause());
	decorateRunButton();
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcPause(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	rcPause();
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcNextBreakpoint(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( inboundFileParser != NULL )
		inboundFileParser->debugNextBreakPoint();
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcNextStep(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( inboundFileParser != NULL )
		inboundFileParser->debugNextStep();
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcFinish(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( inboundFileParser != NULL )
		inboundFileParser->debugFinish();
		
	m_rcNextBreakpoint->Enable(false);
	m_rcNextStep->Enable(false);
	m_rcFinish->Enable(false);
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcStop(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	cnc->sendHalt();
	
	// toggle the pause flag
	if ( isPause() == true )
	{
		THE_CONTEXT->togglePause();
		enableRunControls(THE_CONTEXT->isPause());
		decorateRunButton();
	}
	
	if ( inboundFileParser != NULL )
		inboundFileParser->debugStop();
	
	cnc::trc.logInfo("Current session is stopped");
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcReset(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	requestReset();
	setReferencePosEnforceFlag(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::decorateSpindleState(CncSpindlePowerState state, bool force) {
///////////////////////////////////////////////////////////////////
	// decorate the tool info bitmap
	if ( state == SPINDLE_STATE_ON )	toolState.setState(CncToolStateControl::green);
	else 								toolState.setState(CncToolStateControl::red);
	
	// decorate motion monitor
	motionMonitor->setSpindlePowerState(state);
	
	if ( m_testCaseBook->IsShownOnScreen() || force )
	{
		// decorate the test environment
		m_testSpindlePowerBtn->SetLabel          (state == SPINDLE_STATE_OFF ? "Switch Spindle 'On'"       : "Switch Spindle 'Off'");

		m_testToolPowerState->SetLabel           (state == SPINDLE_STATE_OFF ? "Spindle is switched 'Off'" : "Spindle is switched 'On'");
		m_testToolPowerState->SetBackgroundColour(state == SPINDLE_STATE_OFF ? wxColour(255,128,128)       : *wxGREEN);
		m_testToolPowerState->SetForegroundColour(state == SPINDLE_STATE_OFF ? *wxWHITE                    : *wxBLACK);
		
		m_testToolPowerState->Refresh(true);
		m_testToolPowerState->Update();
		
		if ( m_testToolPowerState->GetParent() )
		{
			m_testToolPowerState->GetParent()->SetBackgroundColour(state == SPINDLE_STATE_OFF ? wxColour(255,128,128) : *wxGREEN);
			m_testToolPowerState->GetParent()->Refresh(true);
			m_testToolPowerState->GetParent()->Update();
		}
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::testSwitchToolOnOff(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	if ( cnc->isConnected() == false ) {
		std::cerr << "Not connected, nothing will be processed." << std::endl;
		return;
	}
	
	CncSpindlePowerState cncToolState = cnc->getSpindlePowerState();
	
	if ( cncToolState == SPINDLE_STATE_OFF ) {
		
		const wxString hdl("Switch tool on . . . ");
		const wxString msg("Do you really want to switch the tool power on?");
		wxMessageDialog dlg(this, msg, hdl, wxYES | wxNO | wxICON_QUESTION | wxCENTRE);
		
		if ( dlg.ShowModal() == wxID_NO ) 
			return;
	}
	
	if ( cncToolState == SPINDLE_STATE_OFF ) {
		cnc->switchSpindleOn();
		startAnimationControl();
		
		m_testSpindleSpeedSlider->SetMin((int)THE_CONFIG->getSpindleSpeedMin());
		m_testSpindleSpeedSlider->SetMax((int)THE_CONFIG->getSpindleSpeedMax());
		
		if ( cnc != NULL ) 
			m_testSpindleSpeedSlider->SetValue((int)cnc->getConfiguredSpindleSpeed());
			
	} else {
		cnc->switchSpindleOff();
		stopAnimationControl();
	}
	
	cncToolState = cnc->getSpindlePowerState();
	enableControls(cncToolState == SPINDLE_STATE_OFF);
	
	if ( m_testSpindlePowerBtn->IsShownOnScreen() ) {
		m_testSpindlePowerBtn->Enable(true);
		m_testSpindleSpeedSlider->Enable(true);
	}
		
	updateSetterList();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::testChangedSpindleSpeed(wxScrollEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( cnc != NULL && cnc->getSpindlePowerState() == SPINDLE_STATE_ON ) {
		cnc->changeCurrentSpindleSpeed_U_MIN((float)m_testSpindleSpeedSlider->GetValue());
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::testChangingSpindleSpeed(wxScrollEvent& event) {
/////////////////////////////////////////////////////////////////////
	m_testSpindleSpeedSlider->SetToolTip(wxString::Format("%d U/min", m_testSpindleSpeedSlider->GetValue()));
}
///////////////////////////////////////////////////////////////////
void MainFrame::activate3DPerspectiveButton(wxButton* bt) {
///////////////////////////////////////////////////////////////////
	static wxColour active  (171, 171, 171);
	static wxColour inactive(240, 240, 240);

	m_3D_TopSec->SetBackgroundColour(inactive);
	m_3D_BottomSec->SetBackgroundColour(inactive);
	m_3D_FrontSec->SetBackgroundColour(inactive);
	m_3D_RearSec->SetBackgroundColour(inactive);
	m_3D_LeftSec->SetBackgroundColour(inactive);
	m_3D_RightSec->SetBackgroundColour(inactive);
	m_3D_Perspective1Sec->SetBackgroundColour(inactive);
	m_3D_Perspective2Sec->SetBackgroundColour(inactive);
	m_3D_Perspective3Sec->SetBackgroundColour(inactive);
	m_3D_Perspective4Sec->SetBackgroundColour(inactive);
	
	if ( bt != NULL ) {
		bt->SetBackgroundColour(active);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::showFromTop3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxButton* bt = (wxButton*)event.GetEventObject();
	activate3DPerspectiveButton(bt);

	motionMonitorViewTop();
}
///////////////////////////////////////////////////////////////////
void MainFrame::showFromBottom3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxButton* bt = (wxButton*)event.GetEventObject();
	activate3DPerspectiveButton(bt);

	motionMonitorViewBottom();
}
///////////////////////////////////////////////////////////////////
void MainFrame::showFromFront3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxButton* bt = (wxButton*)event.GetEventObject();
	activate3DPerspectiveButton(bt);

	motionMonitorViewFront();
}
///////////////////////////////////////////////////////////////////
void MainFrame::showFromRear3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxButton* bt = (wxButton*)event.GetEventObject();
	activate3DPerspectiveButton(bt);

	motionMonitorViewRear();
}
///////////////////////////////////////////////////////////////////
void MainFrame::showFromLeft3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxButton* bt = (wxButton*)event.GetEventObject();
	activate3DPerspectiveButton(bt);

	motionMonitorViewLeft();
}
///////////////////////////////////////////////////////////////////
void MainFrame::showFromRight3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxButton* bt = (wxButton*)event.GetEventObject();
	activate3DPerspectiveButton(bt);
	
	motionMonitorViewRight();
}
///////////////////////////////////////////////////////////////////
void MainFrame::show3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxButton* bt = (wxButton*)event.GetEventObject();
	activate3DPerspectiveButton(bt);
	
	if      ( bt == m_3D_Perspective1Sec )	motionMonitorViewPerspective1();
	else if ( bt == m_3D_Perspective2Sec )	motionMonitorViewPerspective2();
	else if ( bt == m_3D_Perspective3Sec )	motionMonitorViewPerspective3();
	else if ( bt == m_3D_Perspective4Sec )	motionMonitorViewPerspective4();
	
}
///////////////////////////////////////////////////////////////////
void MainFrame::onKeepFrontModeCentred(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	motionMonitor->setFrontCatchingMode(GLContextBase::FrontCatchingMode::FCM_ALWAYS_CENTRED);
}
///////////////////////////////////////////////////////////////////
void MainFrame::onKeepFrontModeInFrame(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	motionMonitor->setFrontCatchingMode(GLContextBase::FrontCatchingMode::FCM_KEEP_IN_FRAME);
}
///////////////////////////////////////////////////////////////////
void MainFrame::onKeepFrontModeOff(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	motionMonitor->setFrontCatchingMode(GLContextBase::FrontCatchingMode::FCM_OFF);
}
///////////////////////////////////////////////////////////////////
void MainFrame::onSelectHardwareSpace(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	motionMonitor->makeHardwareSpaceVisible();
}
///////////////////////////////////////////////////////////////////
void MainFrame::onSelectWorkingSpace(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	motionMonitor->makeWorkingSpaceVisible();
}
///////////////////////////////////////////////////////////////////
void MainFrame::motionMonitorViewTop() {
///////////////////////////////////////////////////////////////////
	drawPane3D->showFromTop();
}
///////////////////////////////////////////////////////////////////
void MainFrame::motionMonitorViewBottom() {
///////////////////////////////////////////////////////////////////
	drawPane3D->showFromBottom();
}
///////////////////////////////////////////////////////////////////
void MainFrame::motionMonitorViewLeft() {
///////////////////////////////////////////////////////////////////
	drawPane3D->showFromLeft();
}
///////////////////////////////////////////////////////////////////
void MainFrame::motionMonitorViewRight() {
///////////////////////////////////////////////////////////////////
	drawPane3D->showFromRight();
}
///////////////////////////////////////////////////////////////////
void MainFrame::motionMonitorViewFront() {
///////////////////////////////////////////////////////////////////
	drawPane3D->showFromFront();
}
///////////////////////////////////////////////////////////////////
void MainFrame::motionMonitorViewRear() {
///////////////////////////////////////////////////////////////////
	drawPane3D->showFromRear();
}
///////////////////////////////////////////////////////////////////
void MainFrame::motionMonitorViewPerspective1() {
///////////////////////////////////////////////////////////////////
	drawPane3D->showFromPerspective1();
}
///////////////////////////////////////////////////////////////////
void MainFrame::motionMonitorViewPerspective2() {
///////////////////////////////////////////////////////////////////
	drawPane3D->showFromPerspective2();
}
///////////////////////////////////////////////////////////////////
void MainFrame::motionMonitorViewPerspective3() {
///////////////////////////////////////////////////////////////////
	drawPane3D->showFromPerspective3();
}
///////////////////////////////////////////////////////////////////
void MainFrame::motionMonitorViewPerspective4() {
///////////////////////////////////////////////////////////////////
	drawPane3D->showFromPerspective4();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onClearMonitor(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(drawPane3D);
	drawPane3D->clearMonitor();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onRefreshMonitor(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(drawPane3D);
	drawPane3D->refreshMonitor();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onToggleObjectBoundBox(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(drawPane3D);
	drawPane3D->toggleObjectBoundBox();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onToggleTotalBoundBox(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(drawPane3D);
	drawPane3D->toggleTotalBoundBox();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onToggleHardwareBox(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(drawPane3D);
	drawPane3D->toggleHardwareBox();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onToggleRuler(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(drawPane3D);
	drawPane3D->toggleRuler();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onToggleFlyPathes(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(drawPane3D);
	drawPane3D->toggleFlyPathes();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onToggleGuidePathes(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(drawPane3D);
	drawPane3D->toggleGuidePathes();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onToggleHelpLines(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(drawPane3D);
	drawPane3D->toggleHelpLines();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onToggleMillingCutter(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(drawPane3D);
	drawPane3D->toggleMillingCutter();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onToggleOrigin(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(drawPane3D);
	drawPane3D->toggleOrigin();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onTogglePosMarker(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(drawPane3D);
	drawPane3D->togglePosMarker();
	
	m_secureShowMillingCutter->Enable(drawPane3D->statePosMarker());
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onResetView(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(drawPane3D);
	drawPane3D->resetView();
	
	if ( secureCtrlPanel )
		secureCtrlPanel->notifyResetMonitorView();
}
///////////////////////////////////////////////////////////////////
void MainFrame::clearMotionMonitor() {
///////////////////////////////////////////////////////////////////
	if ( cnc )
		cnc->resetClientId();
	
	const bool useExistingCncInstructions = THE_TPL_CTX->isValid();
	
	motionMonitor->clear();
	statisticsPane->clear();
	cncPreprocessor->clearAll(useExistingCncInstructions == false);
	
	decorateOutboundEditor();
}
///////////////////////////////////////////////////////////////////
void MainFrame::testCaseBookChanged(wxListbookEvent& event) {
///////////////////////////////////////////////////////////////////
	unsigned int sel = event.GetSelection();
	
	switch ( sel ) {
		case TestBookSelection::VAL::INTERVAL:	break;
		
		case TestBookSelection::VAL::TOOL:		if ( cnc != NULL )
													decorateSpindleState(cnc->getSpindlePowerState(), true);
													
												break;
												
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::traceTextUpdated(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	traceTimerCounter = 0;
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestControllerPinsFromButton(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_btRequestControllerPins->Enable(false);
	requestPins(event);
	m_btRequestControllerPins->Enable(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestPins(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	cnc->processCommand(CMD_PRINT_PIN_REPORT, std::clog);
	m_outboundNotebook->SetSelection(OutboundSelection::VAL::SUMMARY_PANEL);
	m_notebookConfig->SetSelection(OutboundCfgSelection::VAL::CNC_PIN_PANEL);
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewControllerMsgHistory(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	getCtrlMsgHistoryList()->openAsTextView("CNC Message History", true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::clearControllerMsgHistory(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	getCtrlMsgHistoryList()->clearAll();
}
///////////////////////////////////////////////////////////////////
void MainFrame::paintDrawPaneWindow(wxPaintEvent& event) {
///////////////////////////////////////////////////////////////////
	// do nothing
}
///////////////////////////////////////////////////////////////////
void MainFrame::displayPGenErrorInfo(const wxString& errorInfo) {
///////////////////////////////////////////////////////////////////
	wxMessageDialog dlg(this, "Path generation failed!", "Path Generator Error Message", wxOK|wxICON_ERROR);
 	dlg.SetExtendedMessage(errorInfo);
	dlg.ShowModal();
}
///////////////////////////////////////////////////////////////////
void MainFrame::openCurrentTemplateInBrowser() {
///////////////////////////////////////////////////////////////////
	wxString ret;
	openFileExtern(THE_CONFIG->getBrowser(ret), getCurrentTemplatePathFileName());
}
///////////////////////////////////////////////////////////////////
void MainFrame::closeUnitCalculator(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	hideAuiPane("UnitCalculator");
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectUCUnitFrom(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	selectUCChangeFrom(event);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectUCUnitTo(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	selectUCChangeFrom(event);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectUCChangeFrom(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	typedef CncUnitCalculatorBase::Unit Unit;
	
	Unit from = Unit::px;
	Unit to   = Unit::px;
	
	if ( CncUnitCalculatorBase::determineUnit(m_cbUCUnitFrom->GetStringSelection(), from) == false )
		return;
	
	if ( CncUnitCalculatorBase::determineUnit(m_cbUCUnitTo->GetStringSelection(),     to) == false )
		return;

	wxString line	= m_cbUCValueFrom->GetValue();
	
	m_cbUCValueTo->Clear();
	
	if ( line.IsEmpty() == true )
		return;
	
	wxStringTokenizer tokenizer(line, " \t,");
	while ( tokenizer.HasMoreTokens() ) {
		wxString token = tokenizer.GetNextToken();
		if ( token.IsEmpty() )
			continue;
		
		double vFrom, vTo;
		token.ToDouble(&vFrom);
		
		CncUnitCalculator<double> uc(from, to);
		vTo = uc.convert(vFrom);
		
		m_cbUCValueTo->AppendText(wxString::Format("%.3lf", vTo));
		m_cbUCValueTo->AppendText(wxString(tokenizer.GetLastDelimiter()));
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::unitTestFramework(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	UnitTests test(this, 0, true);
	test.ShowModal();
}
///////////////////////////////////////////////////////////////////
void MainFrame::onSelectManuallyMove(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	selectMainBookManuelPanel();
}
///////////////////////////////////////////////////////////////////
void MainFrame::onSelectReferences(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	selectMainBookReferencePanel();
}
///////////////////////////////////////////////////////////////////
void MainFrame::onSelectSetup(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	selectMainBookSetupPanel();
}
///////////////////////////////////////////////////////////////////
void MainFrame::onSelectTemplate(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	selectMainBookSourcePanel();
}
///////////////////////////////////////////////////////////////////
void MainFrame::onSelectInboundPreview(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	selectMainBookPreviewPanel();
}
///////////////////////////////////////////////////////////////////
void MainFrame::onSelectTestPage(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	selectMainBookTestPanel();
}
///////////////////////////////////////////////////////////////////
void MainFrame::onSelectCncMonitor(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	selectMonitorBookCncPanel();
}
///////////////////////////////////////////////////////////////////
void MainFrame::onSelectTemplatePreview(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	prepareAndShowMonitorTemplatePreview(sourceEditor->IsModified());
	selectMonitorBookTemplatePanel();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onDebugUserNotificationTimer(wxTimerEvent& event) {
/////////////////////////////////////////////////////////////////////
	const wxColour c1(227, 227, 227);
	const wxColour c2(255, 201, 14);
	
	if ( THE_CONTEXT->processingInfo->isWaitingForUserEvents() == true )
	{
		if ( m_rcNextBreakpoint->GetBackgroundColour() != c1 )	m_rcNextBreakpoint->SetBackgroundColour(c1);
		if ( m_rcNextStep->GetBackgroundColour() != c1 )		m_rcNextStep->SetBackgroundColour(c1);
		if ( m_rcFinish->GetBackgroundColour() != c1 )			m_rcFinish->SetBackgroundColour(c1);
		
		return;
	}
	
	if ( m_rcNextBreakpoint->GetBackgroundColour() == c1 )	m_rcNextBreakpoint->SetBackgroundColour(c2);
	else 													m_rcNextBreakpoint->SetBackgroundColour(c1);
	
	if ( m_rcNextStep->GetBackgroundColour() == c1 )		m_rcNextStep->SetBackgroundColour(c2);
	else 													m_rcNextStep->SetBackgroundColour(c1);
	
	if ( m_rcFinish->GetBackgroundColour() == c1 )			m_rcFinish->SetBackgroundColour(c2);
	else 													m_rcFinish->SetBackgroundColour(c1);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::startDebugUserNotification() {
/////////////////////////////////////////////////////////////////////
	const wxColour c(255, 201, 14);
	m_rcNextBreakpoint->SetBackgroundColour(c);
	m_rcNextStep->SetBackgroundColour(c);
	m_rcFinish->SetBackgroundColour(c);
	
	if ( debugUserNotificationTimer->IsRunning() == false )
		debugUserNotificationTimer->Start(1000);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::stopDebugUserNotification() {
/////////////////////////////////////////////////////////////////////
	const wxColour c(227, 227, 227);
	m_rcNextBreakpoint->SetBackgroundColour(c);
	m_rcNextStep->SetBackgroundColour(c);
	m_rcFinish->SetBackgroundColour(c);
	
	if ( debugUserNotificationTimer->IsRunning() == true )
		debugUserNotificationTimer->Stop();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::clearPositionSpy() {
/////////////////////////////////////////////////////////////////////
	positionSpy->clear();
	motionMonitor->resetCurrentClientId();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::clearPositionSpy(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	clearPositionSpy();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::toggleTemplateManager(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	toggleAuiPane("TemplateManager");
}
/////////////////////////////////////////////////////////////////////
void MainFrame::loadPerspective(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxMenuItem* mi = m_menuPerspective->FindChildItem(event.GetId());
	if ( mi != NULL ) {
		wxString perspective(mi->GetItemLabelText());
		bool up = CncPerspective::isUserPerspective(perspective);
		
		// strip name
		perspective.assign(perspective.AfterFirst(CncPerspective::getNameEnclose()));
		perspective.assign(perspective.BeforeLast(CncPerspective::getNameEnclose()));
		
		if ( up == true )
			perspective.Prepend(CncPerspective::getNameEnclose());
			
		perspectiveHandler.loadPerspective(perspective);
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::savePerspective(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxMenuItem* mi = m_menuPerspective->FindChildItem(event.GetId());
	if ( mi != NULL ) {
		wxString perspective(mi->GetItemLabelText());
		bool up = CncPerspective::isUserPerspective(perspective);
		
		// strip name
		perspective.assign(perspective.AfterFirst(CncPerspective::getNameEnclose()));
		perspective.assign(perspective.BeforeLast(CncPerspective::getNameEnclose()));

		if ( up == true )
			perspective.Prepend(CncPerspective::getNameEnclose());
			
		perspectiveHandler.savePerspective(perspective);
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::addUserPerspective(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	perspectiveHandler.addUserPerspective();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::renameUserPerspective(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	perspectiveHandler.renameUserPerspective();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::removeUserPerspective(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	perspectiveHandler.removeUserPerspective();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::releaseControllerSetupFromConfig() {
/////////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	cnc->setup(false);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::setupGridChanged(wxPropertyGridEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_CONFIG->setupGridChanged(event);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::setupGridChanging(wxPropertyGridEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_CONFIG->setupGridChanging(event);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::setupGridCommandButton(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_CONFIG->setupGridCommandButton(event);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::setupGridSelected(wxPropertyGridEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_CONFIG->setupGridSelected(event);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::saveConfiguration(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	const bool ask = true;
	THE_CONFIG->saveConfiguration(*config, ask);
	THE_CONFIG->updateSaveTrace(m_cfgSaveTrace);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::loadConfiguration(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_CONFIG->loadConfiguration(*config);
	THE_CONFIG->updateLoadTrace(m_cfgLoadTrace, m_cfgObsoleteTrace);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::copyPositionSpy(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( positionSpy->getItemCount() == 0 )
		return;

	// Write some text to the clipboard
	if ( wxTheClipboard->Open() ) {
		CncRunAnimationControl rac(this);

		wxString content;
		content.reserve(1024 * 1024);
		
		for ( long i=0; i<positionSpy->getItemCount(); i++ )
			positionSpy->getRow(i).trace(content);
		
		// This data objects are held by the clipboard,
		// so do not delete them in the app.
		wxTheClipboard->SetData( new wxTextDataObject(content) );
		wxTheClipboard->Close();
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::togglePositionSpy(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	decoratePosSpyConnectButton(m_btTogglePosSpy->GetValue());
}
///////////////////////////////////////////////////////////////////
void MainFrame::decoratePosSpyConnectButton(bool state) {
///////////////////////////////////////////////////////////////////
	wxBitmap bmpOn  = ImageLib16().Bitmap("BMP_CONNECTED");
	wxBitmap bmpOff = ImageLib16().Bitmap("BMP_DISCONNECTED");
	
	state == true ? m_btTogglePosSpy->SetBitmap(bmpOn) 						: m_btTogglePosSpy->SetBitmap(bmpOff);
	state == true ? m_btTogglePosSpy->SetToolTip("Disable Position Spy")	: m_btTogglePosSpy->SetToolTip("Enable Position Spy");
	
	m_btTogglePosSpy->SetValue(state);
	m_btTogglePosSpy->Refresh();
	m_btTogglePosSpy->Update();
	
	if ( state == false )
		clearPositionSpy();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::selectSourceControlLineNumber(long ln) {
/////////////////////////////////////////////////////////////////////
	ln /= CLIENT_ID.TPL_FACTOR;
	ln--;
	if ( ln <= 0L )
		return;
	
	// debugging only
	// std::cout << "ln: " << ln << std::endl;

	wxASSERT( sourceEditor );
	sourceEditor->selectLineNumber(ln);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::selectSourceControlLineNumbers(long firstLine, long lastLine) {
/////////////////////////////////////////////////////////////////////
	firstLine /= CLIENT_ID.TPL_FACTOR;
	lastLine  /= CLIENT_ID.TPL_FACTOR;
	
	firstLine--;
	lastLine--;
	
	if ( lastLine <= 0L || firstLine <= 0L )
		return;
	
	// debugging only
	// std::cout << "ln: " << firstLine << "," << lastLine << std::endl;
	
	wxASSERT( sourceEditor );
	sourceEditor->selectLineNumbers(firstLine, lastLine);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::tryToSelectClientIds(long firstClientId, long lastClientId, ClientIdSelSource::ID tss) {
/////////////////////////////////////////////////////////////////////
	static bool isRunning = false;
	
	if ( isRunning == true )	return;
	else						isRunning = true;
	
	if ( motionMonitor && motionMonitor->IsShownOnScreen() )
	{
		if ( tss != ClientIdSelSource::TSS_REPLAY )
			cnc::trc.logInfo(wxString::Format("%s->selectClientIds(%ld ... %ld); ", ClientIdSelSource::getTemplateSelSourceAsLongString(tss), firstClientId, lastClientId));
	}
	
	if ( tss != ClientIdSelSource::TSS_POS_SPY )
	{
		if ( positionSpy != NULL )
			positionSpy->searchReferenceById(firstClientId);
	}
	
	if ( tss != ClientIdSelSource::TSS_MONITOR )
	{
		if ( motionMonitor != NULL )
		{
			// replay is a special mode which creates the highlighting by itself
			if ( tss != ClientIdSelSource::TSS_REPLAY )
				motionMonitor->highlightClientId(firstClientId, lastClientId);
			
			motionMonitor->Refresh();
		}
	}
	
	if ( tss != ClientIdSelSource::TSS_PATH_LIST )
	{
		if ( cncPreprocessor != NULL )
			cncPreprocessor->selectClientId(firstClientId, CncPreprocessor::LT_PATH_LIST);
	}
	
	if ( tss != ClientIdSelSource::TSS_MOVE_SEQ_OVW && tss != ClientIdSelSource::TSS_MOVE_SEQ )
	{
		if ( cncPreprocessor != NULL )
		{
			cncPreprocessor->selectClientId(firstClientId, CncPreprocessor::LT_MOVE_SEQ_OVERVIEW);
			cncPreprocessor->selectClientId(firstClientId, CncPreprocessor::LT_MOVE_SEQ_CONTENT);
		}
	}
	
	if ( tss != ClientIdSelSource::TSS_VERTEX_DATA_TRACE)
	{
		if ( motionVertexCtrl != NULL )
			motionVertexCtrl->selectClientId(firstClientId, CncMotionVertexTrace::LT_VERTEX_DATA_TRACE);
	}
	
	if ( tss != ClientIdSelSource::TSS_VERTEX_INDEX_TRACE)
	{
		if ( motionVertexCtrl != NULL )
			motionVertexCtrl->selectClientId(firstClientId, CncMotionVertexTrace::LT_VERTEX_INDEX_TRACE);
	}
	
	// Important!
	// The editor hast to be the last one, otherwise the selection can be overridden by an 
	// other control which calls tryToSelectClientId(long clientId, TemplateSelSource tss) only
	// and the he first line is then selected only
	if ( tss != ClientIdSelSource::TSS_EDITOR )
	{
		selectSourceControlLineNumbers(firstClientId, lastClientId);
	}
	
	isRunning = false;
}
/////////////////////////////////////////////////////////////////////
void MainFrame::tryToSelectClientId(long clientId, ClientIdSelSource::ID tss) {
/////////////////////////////////////////////////////////////////////
	tryToSelectClientIds(clientId, clientId, tss);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::selectPositionSpyContent(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( m_cbContentPosSpy->GetSelection() == 0 )	positionSpy->setPositionType(CncPosSpyListCtrl::PosType::PT_APP);
	else											positionSpy->setPositionType(CncPosSpyListCtrl::PosType::PT_CTL);

	clearPositionSpy();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::loopRepeatTest(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	CncTestRunConfig dlg(this);
	dlg.ShowModal();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::refreshSetterList(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	updateSetterList();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::clearSetterList(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	setterList->clearAll();
	
	cnc::SetterValueList values;
	values.push_back(SEPARARTOR_RESET);
	addSetter(PID_SEPARATOR, values);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::goPosSypFirstId(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( positionSpy->GetItemCount() == 0 )
		return;
		
	positionSpy->selectItem(0L);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::goPosSypLastId(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( positionSpy->GetItemCount() == 0 )
		return;
	
	positionSpy->selectItem(positionSpy->GetItemCount() - 1);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::goPosSypNextId(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	long ls = positionSpy->getLastSelection();
	if ( ls == wxNOT_FOUND )
		positionSpy->selectItem(0L);
	
	ls = positionSpy->getLastSelection();
	if ( ls == wxNOT_FOUND )
		return;
	
	if ( positionSpy->goForwardUnitlColumnChange(ls, CncPosSpyListCtrl::COL_REF) == false )
		cnc::trc.logInfo("Position Monitor: Bottom reached . . . ");
	else
		cnc::trc.logInfo("");
}
/////////////////////////////////////////////////////////////////////
void MainFrame::goPosSypPrevId(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	long ls = positionSpy->getLastSelection();
	if ( ls == wxNOT_FOUND )
		positionSpy->selectItem(positionSpy->GetItemCount() - 1);
	
	ls = positionSpy->getLastSelection();
	if ( ls == wxNOT_FOUND )
		return;
	
	if ( positionSpy->goBackwardUnitlColumnChange(ls, CncPosSpyListCtrl::COL_REF) == false )
		cnc::trc.logInfo("Position Monitor: Top reached . . . ");
	else
		cnc::trc.logInfo("");
}
/////////////////////////////////////////////////////////////////////
void MainFrame::searchPosSpy(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxTextEntryDialog dlg(this, "Reference:", "Search position spy reference:", "");
	dlg.SetTextValidator(wxTextValidator(wxFILTER_NUMERIC));
	dlg.SetMaxLength(16);
	
	if ( dlg.ShowModal() != wxID_OK )
		return;
		
	wxString what(dlg.GetValue());
	what.Trim().Trim(true);
	
	bool ret = false;
	if ( what.IsEmpty() == false ) {
		long ref; what.ToLong(&ref);
		ret = positionSpy->searchReferenceById(ref);
	}
	
	if ( ret == false ) 
		cnc::trc.logWarning(wxString::Format("Position Spy Search: Nothing found for '%s'", what));
}
/////////////////////////////////////////////////////////////////////
void MainFrame::displayIntervalChanged(wxScrollEvent& event) {
/////////////////////////////////////////////////////////////////////
	m_displayInterval->SetToolTip(wxString::Format("%d", m_displayInterval->GetValue()));
	THE_CONTEXT->setUpdateInterval(m_displayInterval->GetValue());
}
/////////////////////////////////////////////////////////////////////
void MainFrame::displayIntervalThumbtrack(wxScrollEvent& event) {
/////////////////////////////////////////////////////////////////////
	m_displayInterval->SetToolTip(wxString::Format("%d", m_displayInterval->GetValue()));
	THE_CONTEXT->setUpdateInterval(m_displayInterval->GetValue());
}
/////////////////////////////////////////////////////////////////////
void MainFrame::displayIntervalKeyDown(wxKeyEvent& event) {
/////////////////////////////////////////////////////////////////////
	event.Skip(true);

	m_displayInterval->SetToolTip(wxString::Format("%d", m_displayInterval->GetValue()));
	THE_CONTEXT->setUpdateInterval(m_displayInterval->GetValue());
}
/////////////////////////////////////////////////////////////////////
void MainFrame::clickProbeMode(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	// this forces decorateProbeMode(...) also
	THE_CONTEXT->setProbeMode(m_btProbeMode->GetValue());
}
/////////////////////////////////////////////////////////////////////
void MainFrame::decorateProbeMode(bool probeMode) {
/////////////////////////////////////////////////////////////////////
	if ( probeMode == true ) {
		const wxString tt("Probe mode is ON\nThis disables the stepper motors and the speed controller");
		m_btProbeMode->SetBitmap(ImageLibProbe().Bitmap("BMP_PROBE"));
		m_btProbeMode->SetToolTip(tt);
		
		m_probeModeState->SetBitmap(ImageLib24().Bitmap("BMP_TRAFFIC_LIGHT_YELLOW"));
		m_probeModeState->SetToolTip(tt);
		m_probeModeStateLabel->SetLabel(" ON");
		
	} else {
		const wxString tt("Probe mode is OFF\nThis enables the stepper motors and the speed controller");
		m_btProbeMode->SetBitmap(ImageLibProbe().Bitmap("BMP_RELEASE"));
		m_btProbeMode->SetToolTip(tt);
		
		m_probeModeState->SetBitmap(ImageLib24().Bitmap("BMP_TRAFFIC_LIGHT_DEFAULT"));
		m_probeModeState->SetToolTip(tt);
		m_probeModeStateLabel->SetLabel("OFF");
		
	}
	
	m_btProbeMode->SetValue(probeMode);
	m_btProbeMode->Refresh();
	
	if ( motionMonitor != NULL ) {
		motionMonitor->decorateProbeMode(THE_CONTEXT->isProbeMode());
		motionMonitor->Refresh();
	}
}
/////////////////////////////////////////////////////////////////////
bool MainFrame::isInteractiveMoveActive() const {
/////////////////////////////////////////////////////////////////////
	if ( cnc )
		return cnc->isInteractiveMoveActive();
		
	return false;
}
/////////////////////////////////////////////////////////////////////
bool MainFrame::startInteractiveMove(CncInteractiveMoveDriver imd) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	const unsigned int sel = m_rbStepSensitivity->GetSelection();
	CncStepSensitivity stepSensitivity = cnc::getStepSensitivityOfIndex(sel);
	
	// cnc->isReadyToRun() isn't sufficient here, because this disables
	// the possibility to resolve a limit situation
	
	// Until a other command is still active do nothing here. 
	// Normally this situation will resolve automatically
	if ( cnc->isCommandActive() == true ) 
		return false;
	
	// create an overarching transaction lock  
	if ( interactiveTransactionLock == NULL )
		interactiveTransactionLock = new CncGamepadTransactionLock(this);
	
	selectMonitorBookCncPanel();
	motionMonitor->pushInteractiveProcessMode();
	
	// start
	const bool ret = cnc->startInteractiveMove(stepSensitivity, imd);
	return ret;
}
/////////////////////////////////////////////////////////////////////
bool MainFrame::updateInteractiveMove() {
/////////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	bool ret = true;
	if ( isInteractiveMoveActive() == true )
	{
		// this is something like a heartbeat
		ret = cnc->updateInteractiveMove();
		
		const bool dispatchUserEvents = false;
		
		if ( SerialCommandLocker::getLockedCommand() != CMD_POP_SERIAL )
			ret = cnc->popSerial(dispatchUserEvents);
	}
	
	return ret;
}
/////////////////////////////////////////////////////////////////////
bool MainFrame::updateInteractiveMove(const CncLinearDirection x, const CncLinearDirection y, const CncLinearDirection z, int modifySpeed) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	bool ret = true;
	if ( isInteractiveMoveActive() == true )
	{
		const bool dispatchUserEvents = false;
		
		if ( SerialCommandLocker::getLockedCommand() != CMD_POP_SERIAL )
			ret = cnc->popSerial(dispatchUserEvents);
			
		if ( ret == true )
			ret = cnc->updateInteractiveMove(x, y, z, modifySpeed);
	}
	
	return ret;
}
/////////////////////////////////////////////////////////////////////
bool MainFrame::stopInteractiveMove() {
/////////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	const bool ret = cnc->stopInteractiveMove();
	
	wxDELETE(interactiveTransactionLock);
	
	motionMonitor->popInteractiveProcessMode();
	motionMonitor->update(true);
	updateSpeedControls();
	
	return ret;
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onNavigatorPanel(CncNavigatorPanelEvent& event) {
/////////////////////////////////////////////////////////////////////
	typedef CncNavigatorPanelEvent::Id Id;
	const Id eventId = (Id)event.GetId();
	
	CncLinearDirection x = CncLinearDirection::CncNoneDir;
	CncLinearDirection y = CncLinearDirection::CncNoneDir;
	CncLinearDirection z = CncLinearDirection::CncNoneDir;
	

	// -------------------------------------------------------------
	auto evaluateMovement = [&]() {
		
		bool move = true;
		switch ( event.direction ) {
			case CncNavigatorPanel::Direction::EE: 	x = CncLinearDirection::CncPosDir; break;
			case CncNavigatorPanel::Direction::WW: 	x = CncLinearDirection::CncNegDir; break;
			
			case CncNavigatorPanel::Direction::NN: 	y = CncLinearDirection::CncPosDir; break;
			case CncNavigatorPanel::Direction::SS: 	y = CncLinearDirection::CncNegDir; break;
			
			case CncNavigatorPanel::Direction::CP: 	z = CncLinearDirection::CncPosDir; break;
			case CncNavigatorPanel::Direction::CN: 	z = CncLinearDirection::CncNegDir; break;
			
			case CncNavigatorPanel::Direction::NW:	x = CncLinearDirection::CncNegDir; 
													y = CncLinearDirection::CncPosDir;
													break;
													
			case CncNavigatorPanel::Direction::NE:	x = CncLinearDirection::CncPosDir; 
													y = CncLinearDirection::CncPosDir;
													break;
													
			case CncNavigatorPanel::Direction::SW:	x = CncLinearDirection::CncNegDir; 
													y = CncLinearDirection::CncNegDir;
													break;
													
			case CncNavigatorPanel::Direction::SE:	x = CncLinearDirection::CncPosDir; 
													y = CncLinearDirection::CncNegDir;
													break;
													
			default:								move = false;
		}
		
		return move;
	};
	
	// -------------------------------------------------------------
	auto interactiveMoveStart = [&]() {
		
		if ( evaluateMovement() == true ) {
			CNC_INTERACTIVE_MOVE_FILTER

			if ( startInteractiveMove(CncInteractiveMoveDriver::IMD_NAVIGATOR) )
				updateInteractiveMove(x, y, z);
			else
				std::cerr << "interactiveMoveStart failed" << std::endl;
		}
	};
	
	// -------------------------------------------------------------
	auto interactiveMoveUpdate = [&]() {
		CNC_INTERACTIVE_MOVE_FILTER
		updateInteractiveMove();
	};
	
	// -------------------------------------------------------------
	auto interactiveMoveStop = [&]() {
		CNC_INTERACTIVE_MOVE_FILTER
		stopInteractiveMove();
	};
	
	// -------------------------------------------------------------
	if ( GetRbStepMode()->GetSelection() == SM_INTERACTIVE ) {
		switch ( eventId ) {
			case Id::CNP_ACTIVATE_REGION:		interactiveMoveStart();
												break;
											
			case Id::CNP_LEFT_DOWN_FOLLOWUP:	interactiveMoveUpdate();
												break;
											
			case Id::CNP_CHANGE_SPEED_INDEX:	selectStepSensitivity(event.speedIndex);
												break;
												
			case Id::CNP_DEACTIVATE_REGION:
			case Id::CNP_LEAVE_PANEL:
			case Id::CNP_KILL_FOCUS:
			case Id::CNP_LEAVE_REGION:			interactiveMoveStop();
												break;
			default: ;
		}
	}
	else {
		
		static CncLinearDirection lastX = CncLinearDirection::CncNoneDir;
		static CncLinearDirection lastY = CncLinearDirection::CncNoneDir;
		static CncLinearDirection lastZ = CncLinearDirection::CncNoneDir;
		
		auto resetLast = [&]() {
			lastX = CncLinearDirection::CncNoneDir;
			lastY = CncLinearDirection::CncNoneDir;
			lastZ = CncLinearDirection::CncNoneDir;
		};
		
		switch ( eventId ) {
			case Id::CNP_ACTIVATE_REGION:		resetLast();
												if ( evaluateMovement() ) {
													if ( startStepwiseMovement(x, y, z) ) {
														lastX = x;
														lastY = y;
														lastZ = z;
													}
												}
												break;
											
			case Id::CNP_LEFT_DOWN_FOLLOWUP:	startStepwiseMovement(lastX, lastY, lastZ);
												break;
											
			case Id::CNP_DEACTIVATE_REGION:
			case Id::CNP_LEAVE_PANEL:
			case Id::CNP_KILL_FOCUS:
			case Id::CNP_LEAVE_REGION:			resetLast();
												break;
			default: ;
		}
	}
}
/////////////////////////////////////////////////////////////////////
bool MainFrame::startStepwiseMovement(CncLinearDirection x, CncLinearDirection y, CncLinearDirection z) {
/////////////////////////////////////////////////////////////////////
	if ( cnc == NULL ) 
		return false;
		
	const double dx = x * THE_CONFIG->getFeedrateX();
	const double dy = y * THE_CONFIG->getFeedrateY();
	const double dz = z * THE_CONFIG->getFeedrateZ();
	
	selectMonitorBookCncPanel();
	
	CNC_TRANSACTION_LOCK_RET_ON_ERROR(false)
	CNC_INTERACTIVE_MOVE_FILTER
	
	return cnc->manualSimpleMoveMetric3D(dx, dy, dz, false);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::setReferencePosition(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	showReferencePositionDlg("");
}
/////////////////////////////////////////////////////////////////////
void MainFrame::setAnchorPosition(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	anchorPositionDlg->ShowModal();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::warmStartController(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( cnc ) {
		cnc->sendSoftwareReset();
		connectSerialPortDialog();
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::changeConfigToolbook(wxToolbookEvent& event) {
/////////////////////////////////////////////////////////////////////
	m_pgMgrSetup->SelectPage(event.GetSelection());
	m_pgMgrSetup->SetDescription("", "");
	//m_pgMgrSetup->GetCurrentPage()->FitColumns();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::decorateGamepadState(bool state) {
/////////////////////////////////////////////////////////////////////
	static bool lastState = true;
	
	if ( state != lastState ) {
		lastState = state;
		
		if ( state == true )	m_gamepadState->SetBitmap((ImageLibGamepad().Bitmap("BMP_ACTIVATED")));
		else 					m_gamepadState->SetBitmap((ImageLibGamepad().Bitmap("BMP_DEACTIVATED")));
		
		gamepadSpy->setActivationState(state);
		
		if ( gamepadStatusCtl != NULL ) {
			if ( THE_CONTEXT->canInteractiveMoving() == false ) {
				gamepadStatusCtl->trace("The gamepad isn't available for the current connection port . . . ");
			}
			else {
				if ( state == false ) {
					gamepadStatusCtl->trace("Gamepad state not available");
				}
			}
		}
		
		m_gamepadState->Refresh();
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::decorateSecureDlgChoice(bool useDlg) {
/////////////////////////////////////////////////////////////////////
	THE_CONTEXT->secureModeInfo.useIt = useDlg;
	if ( THE_CONTEXT->secureModeInfo.useIt == true )	m_rcSecureDlg->SetBitmap((ImageLibSecureRun().Bitmap("BMP_SECURE_DLG_YES")));
	else 												m_rcSecureDlg->SetBitmap((ImageLibSecureRun().Bitmap("BMP_SECURE_DLG_NO")));
	
	m_rcSecureDlg->Refresh();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::rcSecureDlg(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	decorateSecureDlgChoice(!THE_CONTEXT->secureModeInfo.useIt);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::cncMainViewChanged(wxNotebookEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( event.GetOldSelection() == MainBookSelection::VAL::PREVIEW_PANEL ) {
		wxASSERT( lruFileView != NULL );
		lruFileView->selectFirstItem();
	} 
	
	activateGamepadNotificationsOnDemand(true);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::notifyControllerHeartbeat() {
/////////////////////////////////////////////////////////////////////
	// here the idle state indirect always true, because this function
	// call is trigger by the corresponding micro controller feedback
	
	// flag is needed to toggle the bitmap
	if ( idleBitmapState )	{ idleBitmapState = false; m_heartbeatState->SetBitmap(ImageLibHeartbeat().Bitmap("BMP_HEART")); }
	else					{ idleBitmapState = true;  m_heartbeatState->SetBitmap(ImageLibHeartbeat().Bitmap("BMP_HEART_PLUS")); }
	
	m_heartbeatState->Refresh();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::decorateIdleState() {
/////////////////////////////////////////////////////////////////////
	//CNC_PRINT_FUNCT_A("  isIdleStateActive()=%d %d %d",  isIdleStateActive(),  m_miRqtIdleMessages->IsChecked(), m_serialTimer->IsRunning())

	const bool isActive = isIdleStateActive();
	const wxString btnLabel( isActive ? "Turn Off\nController Heartbeats" : "Turn On\nController Heartbeats");
	secureCtrlPanel->GetBtToggleHeartbeats()->SetLabel(btnLabel);
	secureCtrlPanel->GetBtToggleHeartbeats()->SetValue(isActive);
	
	if ( isActive == false )
	{
		// all other states are handles by notifyControllerHeartbeat()
		m_heartbeatState->SetBitmap(ImageLibHeartbeat().Bitmap("BMP_HEART_INACTIVE"));
		m_heartbeatState->Refresh();
	}
}
/////////////////////////////////////////////////////////////////////
bool MainFrame::isIdleStateActive() const {
/////////////////////////////////////////////////////////////////////
	return ( m_miRqtIdleMessages->IsChecked() && m_serialTimer->IsRunning() );
}
/////////////////////////////////////////////////////////////////////
void MainFrame::toggleIdleRequests(bool state) {
/////////////////////////////////////////////////////////////////////
	if ( state )	startSerialTimer();
	else			stopSerialTimer();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::dclickHeartbeatState(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( m_miRqtIdleMessages->IsEnabled() == false )
		return;
		
	toggleIdleRequests(!isIdleStateActive());
}
/////////////////////////////////////////////////////////////////////
void MainFrame::toggleIdleRequests(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	m_miRqtIdleMessages->Check(event.IsChecked());
	toggleIdleRequests(m_miRqtIdleMessages->IsChecked());
}
/////////////////////////////////////////////////////////////////////
void MainFrame::openSessionDialog(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxString ret;
	openFileExtern(THE_CONFIG->getFileBrowser(ret), CncFileNameService::getTempDirSession());
}
/////////////////////////////////////////////////////////////////////
void MainFrame::traceSessionId(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	traceSessionId();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::traceSessionId() {
/////////////////////////////////////////////////////////////////////
	std::clog << "Woodworking Session:" << std::endl;
	std::cout << " :: ID: " << CncFileNameService::getSession() << std::endl;
}
/////////////////////////////////////////////////////////////////////
BinaryFileParser::ViewType MainFrame::getCurrentBinaryViewMode() {
/////////////////////////////////////////////////////////////////////
	unsigned int sel = m_cbBinaryViewMode->GetSelection();
	switch ( sel ) {
		case 0:		return BinaryFileParser::ViewType::HexRaw;
		case 1:		return BinaryFileParser::ViewType::HexCStyle;
		case 2:		return BinaryFileParser::ViewType::ReadableSteps;
		
		default:	return BinaryFileParser::ViewType::ReadableMetric;
	}
	
	return BinaryFileParser::ViewType::ReadableMetric;
}
/////////////////////////////////////////////////////////////////////
void MainFrame::selectBinaryEditorViewMode(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	unsigned int sel = m_cbBinaryViewMode->GetSelection();
	
	BinaryFileParser::ViewType vt;
	switch ( sel ) {
		case 0:		vt = BinaryFileParser::ViewType::HexRaw; 			break;
		case 1:		vt = BinaryFileParser::ViewType::HexCStyle; 		break;
		case 2:		vt = BinaryFileParser::ViewType::ReadableSteps; 	break;
		
		default:	vt = BinaryFileParser::ViewType::ReadableMetric;
	}
	
	sourceEditor->changeBinaryViewType(vt);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::saveOutboundAsNewTplFromMenu(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	saveOutboundAsNewTplFromButton(event);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::saveOutboundAsNewTplFromButton(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	const wxString outboundFile(cnc->getPortName());
	
	if ( wxFile::Exists(outboundFile) == false ) {
		std::cerr << "MainFrame::saveOutboundAsNewTemplate: Can't found '" << outboundFile << "'" << std::endl;
		return;
	}
	
	const wxString headline("Save Outbound as new Template");
	wxString inboundFile(getCurrentTemplatePathFileName());
	inboundFile.append(".");
	inboundFile.append(outboundEditor->getExtention());

	wxString newFile;
	wxFileName fileName(inboundFile);

	while ( true ) {
		wxFileDialog saveFileDialog(this, 
									headline, 
									fileName.GetPath(),
									fileName.GetFullName(),
									"",
									wxFD_SAVE);

		if ( saveFileDialog.ShowModal() == wxID_CANCEL ) { 
			return;
		}
		
		newFile.assign(saveFileDialog.GetPath());
		if ( wxFile::Exists(newFile) == true ) {
			
			const wxString msg(wxString::Format("Template '%s'\nalready exists. Overide it?", newFile));
			wxMessageDialog dlg(this, msg, headline, wxYES|wxNO|wxICON_QUESTION|wxCENTRE);
			
			if ( dlg.ShowModal() == wxID_YES ) 
				break; //while
				
		} else {
			break; //while
			
		}
	}
	
	if ( wxCopyFile(outboundFile, newFile, true) == false ) {
		
		std::cerr << "File copy failed:"            << std::endl;
		std::cerr << " from:" << cnc->getPortName() << std::endl;
		std::cerr << " to:"   << newFile	        << std::endl;
		return;
	}
	
	const wxString msg("Should the new template opened directly into the editor?");
	wxMessageDialog dlg(this, msg, headline, wxYES|wxNO|wxICON_QUESTION|wxCENTRE);

	if ( dlg.ShowModal() == wxID_YES )
	{
		const wxFileName tpl(newFile);
		m_inputFileName->SetValue(tpl.GetFullName());
		m_inputFileName->SetHint(tpl.GetFullPath());
		
		if ( !openFile() ) 
		{
			std::cerr << "Error while open file: " << outboundFile << std::endl;
			return;
		}
		
		prepareAndShowMonitorTemplatePreview(true);
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::extractSourceAsNewTpl(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	BinaryFileParser parser(getCurrentTemplatePathFileName());
	if ( parser.preface() == false )
	{
		std::cerr << "MainFrame::extractSourceAsNewTpl(): Error while prefacing file '" 
				  << getCurrentTemplatePathFileName()
				  << "'" << std::endl;
		return;
	}
	
	wxString origType, origName;
	parser.getSourceParameter(XMLSourceNode_AttribFile, origName);
	parser.getSourceParameter(XMLSourceNode_AttribType, origType);
	
	wxFileName newFile(origName);
	newFile.ClearExt();
	
	wxString newName(newFile.GetFullName());
	newName.append("_Copy.");
	newName.append(origType);
	
	wxString defaultDir;
	THE_CONFIG->getDefaultTplDir(defaultDir);
		
	wxString headline("Save nested source as new Template");
	
	wxFileDialog saveFileDialog(this, 
								headline, 
								defaultDir, 
								newName,
								"",
								wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
								
	if ( saveFileDialog.ShowModal() == wxID_CANCEL ) { 
		return;
	}
	
	std::ofstream out(saveFileDialog.GetPath().c_str().AsChar());
	if ( out.good() == false ) {
		std::cerr << "MainFrame::extractSourceAsNewTpl(): Error while creating file '" 
				  << saveFileDialog.GetPath()
				  << "'" << std::endl;
		return;
	}
	
	wxString content;
	out << parser.getSourceContent(content);
	out.close();
	
	wxFileName x(saveFileDialog.GetPath());
	m_inputFileName->SetValue(x.GetFullName());
	m_inputFileName->SetHint(x.GetFullPath());

	if ( !openFile() ) 
	{
		std::cerr << "Error while open file: " << saveFileDialog.GetPath() << std::endl;
		return;
	}
	
	prepareAndShowMonitorTemplatePreview(true);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::toggleMotionMonitorOptionPane(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	toggleMotionMonitorOptionPane(false);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::toggleMotionMonitorOptionPane(bool forceHide) {
/////////////////////////////////////////////////////////////////////
	if ( cnc3DVSplitterWindow != NULL )
		forceHide == true ? cnc3DVSplitterWindow->hideRightWindow() : cnc3DVSplitterWindow->toggleRightWindow();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::toggleMotionMonitorStatisticPane(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	toggleMotionMonitorStatisticPane(false);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::toggleMotionMonitorStatisticPane(bool forceHide) {
/////////////////////////////////////////////////////////////////////
	const int nc	= MontiorBottomContextSelection::VAL::STATISTIC_PANEL;
	const int cc	= cnc3DHSplitterWindow->getCurrentButtomContext();
	
	const bool show	=	( forceHide ? false : 
						( nc != cc  ? true  : !cnc3DHSplitterWindow->isBottomWindowShown() ) 
						);
						
	showMotionMonitorStatisticPane(show);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::showMotionMonitorStatisticPane(bool show) {
/////////////////////////////////////////////////////////////////////
	if ( cnc3DHSplitterWindow == NULL )
		return;
		
	const int nc = MontiorBottomContextSelection::VAL::STATISTIC_PANEL;
	cnc3DHSplitterWindow->selectBottomContext(nc);
	
	if ( show == true )	cnc3DHSplitterWindow->showBottomWindow();
	else				cnc3DHSplitterWindow->hideBottomWindow();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::toggleMotionMonitorReplayPane(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	toggleMotionMonitorReplayPane(false);
}

/////////////////////////////////////////////////////////////////////
void MainFrame::toggleMotionMonitorReplayPane(bool forceHide) {
/////////////////////////////////////////////////////////////////////
	const int nc	= MontiorBottomContextSelection::VAL::REPLAY_PANEL;
	const int cc	= cnc3DHSplitterWindow->getCurrentButtomContext();
	
	const bool show	=	( forceHide ? false : 
						( nc != cc  ? true  : !cnc3DHSplitterWindow->isBottomWindowShown() ) 
						);
						
	showMotionMonitorReplayPane(show);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::showMotionMonitorReplayPane(bool show) {
/////////////////////////////////////////////////////////////////////
	if ( cnc3DHSplitterWindow == NULL )
		return;
		
	const int nc = MontiorBottomContextSelection::VAL::REPLAY_PANEL;
	cnc3DHSplitterWindow->selectBottomContext(nc);
	
	if ( show == true )	cnc3DHSplitterWindow->showBottomWindow();
	else				cnc3DHSplitterWindow->hideBottomWindow();

}
/////////////////////////////////////////////////////////////////////
void MainFrame::selectMetricUnitFrom(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	selectMetricUnitTo(event);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::selectMetricUnitFromValue(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	selectMetricUnitTo(event);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::selectMetricUnitTo(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	const wxString uFrom(m_cbMetricUnitFrom->GetStringSelection());
	const wxString uTo(m_cbMetricUnitTo->GetStringSelection());
	
	const wxString valueFrom(m_metricValueFrom->GetValue());
	double value;
	valueFrom.ToDouble(&value);
	
	if      ( uFrom == "mm"      && uTo == "mm"     ) 		m_metricValueTo->ChangeValue(valueFrom);
	else if ( uFrom == "mm"      && uTo == "stepsX" ) 		m_metricValueTo->ChangeValue(wxString::Format("%.6lf", THE_CONFIG->convertX(CncMetric, CncSteps,  value)));
	else if ( uFrom == "mm"      && uTo == "stepsY" ) 		m_metricValueTo->ChangeValue(wxString::Format("%.6lf", THE_CONFIG->convertY(CncMetric, CncSteps,  value)));
	else if ( uFrom == "mm"      && uTo == "stepsZ" ) 		m_metricValueTo->ChangeValue(wxString::Format("%.6lf", THE_CONFIG->convertZ(CncMetric, CncSteps,  value)));
	
	else if ( uFrom == "stepsX"  && uTo == "stepsX" ) 		m_metricValueTo->ChangeValue(valueFrom);
	else if ( uFrom == "stepsX"  && uTo == "mm"     ) 		m_metricValueTo->ChangeValue(wxString::Format("%.6lf", THE_CONFIG->convertX(CncSteps,  CncMetric, value)));

	else if ( uFrom == "stepsY"  && uTo == "stepsY" ) 		m_metricValueTo->ChangeValue(valueFrom);
	else if ( uFrom == "stepsY"  && uTo == "mm"     ) 		m_metricValueTo->ChangeValue(wxString::Format("%.6lf", THE_CONFIG->convertY(CncSteps,  CncMetric, value)));
	
	else if ( uFrom == "stepsZ"  && uTo == "stepsZ" ) 		m_metricValueTo->ChangeValue(valueFrom);
	else if ( uFrom == "stepsZ"  && uTo == "mm"     ) 		m_metricValueTo->ChangeValue(wxString::Format("%.6lf", THE_CONFIG->convertZ(CncSteps,  CncMetric, value)));
	
	else													m_metricValueTo->ChangeValue("-");
}
/////////////////////////////////////////////////////////////////////
void MainFrame::enableSourceEditorMenuItems(bool enable) {
/////////////////////////////////////////////////////////////////////
	m_miNewTemplate->Enable(enable);
	m_miOpenTemplate->Enable(enable);
	m_miReloadTemplate->Enable(enable);
	m_miSaveTemplate->Enable(enable);
	m_miSaveTemplateAs->Enable(enable);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::showOSEnvironment(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( cncOsEnvDialog && cncOsEnvDialog->IsShownOnScreen() == false )
		cncOsEnvDialog->Show();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::showStacktraceStore(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	CncFileContentDialog dlg(this, CncFileNameService::getStackTraceFileName(), "All collected stacktraces", "Stacktrace Store");
	dlg.scrollToEOF();
	dlg.SetSize(800, 900);
	dlg.setWordWrap(false);
	dlg.ShowModal();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onOpenGLContextObserver(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxASSERT( openGLContextObserver != NULL );
	
	const bool flag = openGLContextObserver->IsShownOnScreen();
	openGLContextObserver->Show(!flag);
	m_miOpenGLContextObserver->Check(!flag);
	
	Refresh();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onIndividualCommand(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	IndividualCommandEvent* ice = static_cast<IndividualCommandEvent*>(&event);

	typedef IndividualCommandEvent::EvtMainFrame 	EMF_ID;
	typedef IndividualCommandEvent::EvtSerialStub 	ESS_ID;
	typedef IndividualCommandEvent::ValueName 		VN;
	
	//---------------------------------------------------------------
	auto showArduinoEnv = [&](bool state)
	{
		if ( cncArduinoEnvironment && cncArduinoEnvironment->IsShownOnScreen() == !state )
			cncArduinoEnvironment->Show(state);
			
		if ( state == true )
		{
			if ( cncArduinoEnvironment->IsIconized() )
				cncArduinoEnvironment->Restore();
		}
	};
	
	//---------------------------------------------------------------
	auto checkSerialThread = [&]() {
		if ( serialThread == NULL ) {
			return false;
		}
		
		return true;
	};
	
	switch ( ice->GetId() ) {
		//-----------------------------------------------------------
		case EMF_ID::DispatchAll:
		{
			dispatchAll();
			break;
		}
		//-----------------------------------------------------------
		case EMF_ID::DistpatchNext:
		{
			dispatchNext();
			break;
		}
		//-----------------------------------------------------------
		case EMF_ID::WaitActive:
		{
			if ( ice->hasValue(VN::VAL2) )
				waitActive(ice->getValue<unsigned int>(VN::VAL1), ice->getValue<bool>(VN::VAL2));
			else
				waitActive(ice->getValue<unsigned int>(VN::VAL1));
			break;
		}
		//-----------------------------------------------------------
		case EMF_ID::EnableControls:
		{
			enableControls(ice->getValue<bool>(VN::VAL1));
			break;
		}
		//-----------------------------------------------------------
		case EMF_ID::ExtViewBoxChange:
		{
			if ( ice->getValue<CncExternalViewBox*>(VN::VAL1) == cncExtMainPreview ) {
				if ( ice->getValue<int>(VN::VAL2) == CncExternalViewBox::Preview::FILE )
					lruFileView->selectFirstItem();
			}
			break;
		}
		//-----------------------------------------------------------
		case EMF_ID::ExtViewBoxAttach:
		{
			if ( ice->getValue<CncExternalViewBox*>(VN::VAL1) == cncExtMainPreview ) {
				// select any id to to the rst below only once
				if ( ice->getValue<unsigned int>(VN::VAL2) == CncExternalViewBox::Preview::FILE ) {
					
					if ( m_secureRunPanel->IsShownOnScreen() )
						selectSecureMonitorView();
				}
			}
			
			break;
		}
		//-----------------------------------------------------------
		case EMF_ID::ExtViewBoxDetach:
		{
			//std::cout << CNC_LOG_FUNCT_A(": EMF_ID::ExtViewBoxDetach(%d)\n",  ice->getValue<unsigned int>(VN::VAL2));
			break;
		}
		//-----------------------------------------------------------
		case ESS_ID::NotifyFatalError:
		{
			std::cerr << "ESS_ID::NotifyFatalError:" << std::endl;
			std::cerr << ( ice->hasValue(VN::VAL1) ? ice->getValue<const char*>(VN::VAL1) 
												   : " No context info available." ) 
					  << std::endl;
			break;
		}
		//-----------------------------------------------------------
		case ESS_ID::NotifyConneting:
		{
			showArduinoEnv(true);
			
			if ( checkSerialThread() == false )
				return; 
				
			serialThread->notifyConnecting();
			cncArduinoEnvironment->notifyConnecting();
			break;
		}
		//-----------------------------------------------------------
		case ESS_ID::NotifyConneted:
		{
			showArduinoEnv(true);
			
			if ( checkSerialThread() == false )
				return; 
				
			serialThread->notifyConnected();
			cncArduinoEnvironment->notifyConnected();
			break;
		}
		//-----------------------------------------------------------
		case ESS_ID::NotifyDisconnected:
		{
			showArduinoEnv(false);
			
			if ( checkSerialThread() == false )
				return; 
				
			serialThread->notifyDisconnected();
			cncArduinoEnvironment->notifyDisconnected();
			break;
		}
		//-----------------------------------------------------------
		case ESS_ID::NotifyPauseBefore:
		{
			showArduinoEnv(true);
			cncArduinoEnvironment->notifyPaused();
			break;
		}
		//-----------------------------------------------------------
		case ESS_ID::NotifyResumeBefore:
		{
			showArduinoEnv(true);
			break;
		}
		//-----------------------------------------------------------
		case ESS_ID::NotifyResumeAfter:
		{
			showArduinoEnv(true);
			cncArduinoEnvironment->notifyResumed();
			break;
		}
	}
}
/////////////////////////////////////////////////////////////////////
SerialThread* MainFrame::getSerialThread(SerialThreadStub* sts) {
/////////////////////////////////////////////////////////////////////
	if ( dynamic_cast<SerialThreadStub*>(sts) != NULL )
		return serialThread;
	
	return NULL;
}
/////////////////////////////////////////////////////////////////////
void MainFrame::openSpeedPlayground(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( cncSpeedPlayground->IsShown() == false )
		cncSpeedPlayground->Show();

	if ( cncSpeedPlayground->IsIconized() == true )
		cncSpeedPlayground->Restore();
	
	cncSpeedPlayground->Raise();
	cncSpeedPlayground->SetFocus();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::openPositionStorage(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	PositionStorage::storage = positionStorage;
	
	if ( positionStorage->IsShown() == false )
		positionStorage->Show();
	
	if ( positionStorage->IsIconized() == true )
		positionStorage->Restore();
	
	positionStorage->Raise();
	positionStorage->SetFocus();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::updateHardwareReference() {
/////////////////////////////////////////////////////////////////////
	double dx = 0.0;
	double dy = 0.0;
	double dz = 0.0;
	
	if ( THE_BOUNDS->getHardwareOffset().isValid() == true ) 
	{
		dx = THE_BOUNDS->getHardwareOffset().getAsMetricX();
		dy = THE_BOUNDS->getHardwareOffset().getAsMetricY();
		dz = THE_BOUNDS->getHardwareOffset().getAsMetricZ();
	}
	
	m_hardwareOffsetX->ChangeValue(wxString::Format("%.3lf", dx));
	m_hardwareOffsetY->ChangeValue(wxString::Format("%.3lf", dy));
	m_hardwareOffsetZ->ChangeValue(wxString::Format("%.3lf", dz));
	
	if ( serialThread != NULL )
	{
		if ( THE_BOUNDS->hasHardwareOffset() == true ) 
		{
			serialThread->setHardwareOffset(&THE_BOUNDS->getHardwareOffset(), 
											THE_BOUNDS->getMaxDimensionStepsX(),
											THE_BOUNDS->getMaxDimensionStepsY(),
											THE_BOUNDS->getMaxDimensionStepsZ()
			);
		}
	}

}
/////////////////////////////////////////////////////////////////////
void MainFrame::simulateHardwareReference(bool defaultBehaviour) {
/////////////////////////////////////////////////////////////////////
	if ( false )
	{
		cnc::trc.logInfoMessage("No hardware support available for the connected port . . . ");
		return;
	}
	
	CncSimuHwDimensionSetup dlg(this, CncSimuHwDimensionSetup::M_ORIGIN);
	if ( defaultBehaviour == false )
	{
		selectMonitorBookCncPanel();
		dlg.ShowModal();
	}
	else
	{
		dlg.processDefault();
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::simulateHardwareDimensions() {
/////////////////////////////////////////////////////////////////////
	selectMonitorBookCncPanel();
	
	CncSimuHwDimensionSetup dlg(this, CncSimuHwDimensionSetup::M_DIMENSION);
	dlg.ShowModal();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onEvaluateHardwareReference(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	// using isEmulator() instead of hasHardware() because the Arduino emulator 
	// has hardware but no end switch detection
	if ( cnc->isEmulator() == true )
		return simulateHardwareReference(false);
	
	if ( THE_CONTEXT->hasHardware() == false ) {
		cnc::trc.logInfoMessage("No hardware support available for the connected port . . . ");
		return;
	}
	
	// evaluate the physical hardware reference
	wxString msg("Do you really want to evaluate the hardware reference position?\n\n");
	msg.append("Execution Plan:\n\n");
	msg.append(" 1. Moves Z axis to maximum position\n");
	msg.append(" 2. Moves X axis to minimum position\n");
	msg.append(" 3. Moves Y axis to minimum position\n");
	msg.append(" 4. Move X and Y axis to previous position\n");
	msg.append(" 5. Moves Z axis to previous position\n");
	
	wxRichMessageDialog dlg(this, msg, _T("Evaluate Hardware Reference . . . "), 
							wxYES_NO|wxICON_INFORMATION|wxCENTRE);
	
	dlg.SetFooterText("Make sure this path is free . . . ");
	dlg.SetFooterIcon(wxICON_WARNING);
	
	CncGamepadSpy::ContextSwaper gcs(gamepadSpy, CncGamepadSpy::GPC_MSGBOX);
	if ( dlg.ShowModal() == wxID_YES )
	{
		CncGampadDeactivator cgd(this);
		
		selectMonitorBookCncPanel();
		disableControls();
		
		CNC_TRANSACTION_LOCK
		
		motionMonitor->pushInteractiveProcessMode();
		
		if ( cnc->evaluateHardwareReference() == true )
			motionMonitor->clear();
			
		motionMonitor->popInteractiveProcessMode();
		updateHardwareReference();
		enableControls();
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onResetHardwareReference(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_BOUNDS->resetHardwareOffset();
	updateHardwareReference();
	motionMonitor->Update();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::updateHardwareDimensions() {
/////////////////////////////////////////////////////////////////////
	m_hardwareDimensionX->ChangeValue(wxString::Format("%.3lf", THE_BOUNDS->getMaxDimensionMetricX()));
	m_hardwareDimensionY->ChangeValue(wxString::Format("%.3lf", THE_BOUNDS->getMaxDimensionMetricY()));
	m_hardwareDimensionZ->ChangeValue(wxString::Format("%.3lf", THE_BOUNDS->getMaxDimensionMetricZ()));
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onEvaluateHardwareXYPlane(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	// using isEmulator() instead of hasHardware() because the Arduino emulator 
	// has hardware but no end switch detection
	if ( cnc->isEmulator() == true )
		return simulateHardwareDimensions();
		
	if ( THE_CONTEXT->hasHardware() == false ) {
		cnc::trc.logInfoMessage("No hardware support available for the connected port . . . ");
		return;
	}
	
	wxString msg("Do you really want to evaluate the dimensions of the XY plane?\n");
	msg.append("Execution Plan:\n\n");
	msg.append(" 1. Moves Z axis to maximum position\n");
	msg.append(" 2. Moves X axis to minimum position\n");
	msg.append(" 3. Moves Y axis to minimum position\n");
	msg.append(" 4. Moves Y axis to maximum position\n");
	msg.append(" 5. Moves X axis to maximum position\n");
	msg.append(" 6. Moves Y axis to minimum position\n");
	msg.append(" 7. Moves X axis to minimum position\n");
	msg.append(" 8. Move X and Y axis to previous position\n");
	msg.append(" 9. Moves Z axis to previous position\n");
	msg.append("\n Make sure this path is free . . . ");
	
	wxRichMessageDialog dlg(this, msg, _T("Evaluate Hardware Dimension . . . "), 
							wxYES_NO|wxICON_INFORMATION|wxCENTRE);
	
	dlg.SetFooterText("Make sure this path is free . . . ");
	dlg.SetFooterIcon(wxICON_WARNING);
	
	CncGamepadSpy::ContextSwaper gcs(gamepadSpy, CncGamepadSpy::GPC_MSGBOX);
	if ( dlg.ShowModal() == wxID_YES ) {
		CncGampadDeactivator cgd(this);
		
		selectMonitorBookCncPanel();
		disableControls();
		
		CNC_TRANSACTION_LOCK

		motionMonitor->pushInteractiveProcessMode();
		CncControl::DimensionXYPlane result;
		if ( cnc->evaluateHardwareDimensionsXYPlane(result) ) {
			m_cbHardwareDimensionEvaluatedX->SetValue(true);
			m_hardwareDimensionX->ChangeValue(wxString::Format("%.3lf", result.dimensionX));
			
			m_cbHardwareDimensionEvaluatedY->SetValue(true);
			m_hardwareDimensionY->ChangeValue(wxString::Format("%.3lf", result.dimensionY));
		}
		
		motionMonitor->popInteractiveProcessMode();
		enableControls();
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onEvaluateHardwareZAxis(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	// using isEmulator() instead of hasHardware() because the Arduino emulator 
	// has hardware but no end switch detection
	if ( cnc->isEmulator() == true )
		return simulateHardwareDimensions();
	
	if ( THE_CONTEXT->hasHardware() == false ) {
		cnc::trc.logInfoMessage("No hardware support available for the connected port . . . ");
		return;
	}
	
	wxString msg("Do you really want to evaluate the dimensions of the Z axis?\n");
	msg.append("Execution Plan:\n\n");
	msg.append(" 1. Moves Z axis to maximum position\n");
	msg.append(" 2. Moves Z axis to minimum position\n");
	msg.append(" 5. Moves Z axis to previous position\n");
	
	wxRichMessageDialog dlg(this, msg, _T("Evaluate Hardware Dimension . . . "), 
							wxYES_NO|wxICON_INFORMATION|wxCENTRE);
	
	dlg.SetFooterText("Make sure this path is free . . . ");
	dlg.SetFooterIcon(wxICON_WARNING);

	if ( dlg.ShowModal() == wxID_YES )
	{
		CncGampadDeactivator cgd(this);

		selectMonitorBookCncPanel();
		disableControls();
		
		CNC_TRANSACTION_LOCK
		
		CncControl::DimensionZAxis result;
		if ( cnc->evaluateHardwareDimensionsZAxis(result) ) {
			m_cbHardwareDimensionEvaluatedZ->SetValue(true);
			m_hardwareDimensionZ->ChangeValue(wxString::Format("%.3lf", result.dimensionZ));
		}
		
		enableControls();
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onTakeoverHardwareDimensions(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	// No extra question required here because saveConfiguration()
	// below will generate someone . . .
	
	bool somethingChanged = false;
	{
		if ( m_cbHardwareDimensionEvaluatedX->GetValue() == true ) {
			double v; m_hardwareDimensionX->GetValue().ToDouble(&v);
			THE_CONFIG->setMaxDimensionX(v);
			somethingChanged = true;
		}
		
		if ( m_cbHardwareDimensionEvaluatedY->GetValue() == true ) {
			double v; m_hardwareDimensionY->GetValue().ToDouble(&v);
			THE_CONFIG->setMaxDimensionY(v);
			somethingChanged = true;
		}
		
		if ( m_cbHardwareDimensionEvaluatedZ->GetValue() == true ) {
			double v; m_hardwareDimensionZ->GetValue().ToDouble(&v);
			THE_CONFIG->setMaxDimensionZ(v);
			somethingChanged = true;
		}
	}
	
	if ( somethingChanged == true )
	{
		const bool ask = true;
		THE_CONFIG->saveConfiguration(*config, ask);
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::requestResolveLimitStates(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( cnc == NULL )
		return;
		
	CncGampadDeactivator cgd(this);
	CNC_TRANSACTION_LOCK
	
	if ( cnc->getLimitState().hasLimit() == false ) {
		cnc::trc.logInfo("No limit situation present . . . ");
		return;
	}
	
	wxString msg("Do you really want to resolve the current limit situation?\n");
	wxRichMessageDialog dlg(this, msg, _T("Resolve limit situation . . . "), 
							wxYES_NO|wxICON_INFORMATION|wxCENTRE);
	
	msg.append("Execution Plan:\n\n");
	msg.append(" The controller tries to move each relevant axis one step before the limit switch\n");
	
	dlg.SetFooterText("Make sure this path is free . . . ");
	dlg.SetFooterIcon(wxICON_WARNING);

	if ( dlg.ShowModal() == wxID_YES ) {
		if ( cnc->correctLimitPositions() == false )
			std::cerr << "MainFrame::requestResolveLimitStates: The function correctLimitPositions() is failed" << std::endl;
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::requestToolTest(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	m_testCaseBook->SetSelection(TestBookSelection::VAL::TOOL);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::cncTransactionLockCallback(CncTransactionLockBase* tal) {
/////////////////////////////////////////////////////////////////////
	getLoggerView()->popProcessMode(LoggerSelection::VAL::CNC);
	getCncPreProcessor()->popProcessMode();
	positionStorage->popProcessMode();
} 
/////////////////////////////////////////////////////////////////////
void MainFrame::cncTransactionReleaseCallback(CncTransactionLockBase* tal) {
/////////////////////////////////////////////////////////////////////
	positionStorage->pushProcessMode();
	getCncPreProcessor()->pushUpdateMode();
	getLoggerView()->pushUpdateMode(LoggerSelection::VAL::CNC);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::clickWorkingParameters(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	m_mainViewBook->SetSelection(MainBookSelection::VAL::SETUP_PANEL);
	
	m_listbookSetupConfig->SetSelection(0);
	m_notebookSetupConfig->SetSelection(0);
	m_configurationToolbook->SetSelection(0);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::shiftStepSensitivity() {
/////////////////////////////////////////////////////////////////////
	const int sel = m_rbStepSensitivity->GetSelection();
	const int max = m_rbStepSensitivity->GetCount();
	
	selectStepSensitivity( (sel + 1) % max );
}
/////////////////////////////////////////////////////////////////////
void MainFrame::selectStepSensitivity(int sel) {
/////////////////////////////////////////////////////////////////////
	if ( sel < 0 || sel > (int) m_rbStepSensitivity->GetCount() - 1 )
		return;
		
	if ( m_rbStepSensitivity->GetSelection() != sel )
		m_rbStepSensitivity->SetSelection(sel);
	
	if ( refPositionDlg != NULL )
		refPositionDlg->selectStepSensitivity(sel);

	const CncStepSensitivity ss = cnc::getStepSensitivityOfIndex(sel);
	updateSpeedSlider(cnc::getSpeedValue(ss));
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onSelectStepSensitivity(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	selectStepSensitivity(m_rbStepSensitivity->GetSelection());
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onSelectStepMode(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	const CncStepMode sm = m_rbStepMode->GetSelection() == 0 ? SM_INTERACTIVE : SM_STEPWISE;
	
	if ( navigatorPanel != NULL)
		navigatorPanel->setStepMode(sm);
	
	const CncStepSensitivity ss = cnc::getStepSensitivityOfIndex(m_rbStepSensitivity->GetSelection());
	updateSpeedSlider(sm == SM_INTERACTIVE ? cnc::getSpeedValue(ss) : 1.0);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::detachMotionMonitor(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	cncExtViewBoxCluster->detachNode(CncExternalViewBoxCluster::Node::EVB_Monitor, ((wxButton*)event.GetEventObject()));
}
/////////////////////////////////////////////////////////////////////
void MainFrame::detachConfiguration(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	// this method is also called from the secure panel from a different button
	// therefore the concrete button pointer
	
	//cncExtViewBoxCluster->detachNode(CncExternalViewBoxCluster::Node::EVB_Config, ((wxButton*)event.GetEventObject()));
	cncExtViewBoxCluster->detachNode(CncExternalViewBoxCluster::Node::EVB_Config, m_btDetachConfiguration);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::detachTemplateSource(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	// this method is also called from the secure panel from a different button
	// therefore the concrete button pointer
	
	//cncExtViewBoxCluster->detachNode(CncExternalViewBoxCluster::Node::EVB_Source, ((wxButton*)event.GetEventObject()));
	cncExtViewBoxCluster->detachNode(CncExternalViewBoxCluster::Node::EVB_Source, m_btDetachExternalTemplateSource);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::detachManuallyTemplate(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	cncExtViewBoxCluster->detachNode(CncExternalViewBoxCluster::Node::EVB_Manually, ((wxButton*)event.GetEventObject()));
}
/////////////////////////////////////////////////////////////////////
void MainFrame::detachReference(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	cncExtViewBoxCluster->detachNode(CncExternalViewBoxCluster::Node::EVB_Reference, ((wxButton*)event.GetEventObject()));
}
/////////////////////////////////////////////////////////////////////
void MainFrame::detachTest(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	cncExtViewBoxCluster->detachNode(CncExternalViewBoxCluster::Node::EVB_Test, ((wxButton*)event.GetEventObject()));
}
/////////////////////////////////////////////////////////////////////
void MainFrame::detachControllerMessages(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	cncExtViewBoxCluster->detachNode(CncExternalViewBoxCluster::Node::EVB_CtrlMsg, ((wxButton*)event.GetEventObject()));
}
/////////////////////////////////////////////////////////////////////
void MainFrame::detachSetterList(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	cncExtViewBoxCluster->detachNode(CncExternalViewBoxCluster::Node::EVB_SetterList, ((wxButton*)event.GetEventObject()));
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onSelectTemplatePanel(wxListbookEvent& event) {
/////////////////////////////////////////////////////////////////////
	/*
	// already done by the onPaint event of 
	// class CncTemplateContextSummaryPanel
	if ( (wxWindow*)event.GetEventObject() == m_listbookSource ) {
		unsigned int sel = event.GetSelection();

		if ( sel == SourceBookSelection::VAL::CONTEXT )
			THE_TPL_CTX->updateGui(true);
	}
	*/
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onSvgExport(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxString outboundFileName(getCurrentTemplatePathFileName() + ".export.svg");
	
	wxFileDialog newFileDialog(this, 
								"Export current SVG to . . . ", 
								wxEmptyString, 
								wxFileName(outboundFileName).GetFullPath(),
								"SVG Files (*.svg)|*.svg", 
								wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

	if ( newFileDialog.ShowModal() == wxID_CANCEL ) 
		return; 
	
	outboundFileName.assign(newFileDialog.GetPath());
	
	SVGFileFormatter::Setup s;
	s.compact		= m_cbSvgExportCompact->GetValue();
	s.keepCnc		= m_cbSvgExportKeepCncNodes->GetValue();
	s.keepFormat	= m_chSvgExportKeepFormat->GetValue();
																		
	SVGFileFormatter f(getCurrentTemplatePathFileName());
	if ( f.convert(s, outboundFileName) == false )
	{
		std::cerr << CNC_LOG_FUNCT_A(wxString::Format(" SVGFileFormatter::convert('%s' -> '%s') failed!\n", 
														getCurrentTemplatePathFileName(), 
														outboundFileName
													 ));
	}
	else
	{
		//ask to open the exported file
		wxMessageDialog dlg(this, 
							outboundFileName,
							"Open . . .  ", 
							wxYES|wxNO|wxICON_QUESTION|wxCENTRE);
							
		int ret = dlg.ShowModal();
		if ( ret == wxID_YES )
		{
			wxFileName fn(outboundFileName);
			
			if ( openTemplateFile(fn) == false )
			{
				std::cerr << CNC_LOG_FUNCT_A(wxString::Format(" openFile('%s') failed!\n", fn.GetFullPath()));
				return;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onSvgFormatPretty(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxString outboundFileName(getCurrentTemplatePathFileName());
	
	// determine a new file name of overridden == false
	if ( m_cbSvgFormatPrettyOverride->GetValue() == false ) {
		const wxString templateName(outboundFileName + ( m_cbSvgFormatPretty->GetValue() ? ".pretty.svg" : ".compact.svg") );
		
		wxFileDialog newFileDialog(this, 
									"Format current SVG pretty to . . . ", 
									wxEmptyString, 
									wxFileName(templateName).GetFullPath(),
									"SVG Files (*.svg)|*.svg", 
									wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

		if ( newFileDialog.ShowModal() == wxID_CANCEL ) 
			return; 
			
		outboundFileName.assign(newFileDialog.GetPath());
	}
	
	SVGFileFormatter::Setup s;
	s.compact		= m_cbSvgFormatPretty->GetValue() == false;
	s.keepCnc		= true;
	s.keepFormat	= true;

	SVGFileFormatter f(getCurrentTemplatePathFileName());
	if ( f.convert(s, outboundFileName) == false )
	{
		std::cerr << CNC_LOG_FUNCT_A(wxString::Format(" SVGFileFormatter::convert('%s' -> '%s') failed!\n", 
														getCurrentTemplatePathFileName(), 
														outboundFileName
													 ));
		return;
	}
	
	// in case a new file was created, ask to open it
	if ( outboundFileName.IsSameAs(getCurrentTemplatePathFileName()) == false )
	{
		wxMessageDialog dlg(this, 
							outboundFileName,
							"Open . . .  ", 
							wxYES|wxNO|wxICON_QUESTION|wxCENTRE);
							
		int ret = dlg.ShowModal();
		if ( ret == wxID_YES )
		{
			wxFileName fn(outboundFileName);
			
			if ( openTemplateFile(fn) == false )
			{
				std::cerr << CNC_LOG_FUNCT_A(wxString::Format(" openFile('%s') failed!\n", fn.GetFullPath()));
				return;
			}
		}
	}
	// else reload to make the overridden content visible
	else 
	{
		reloadTemplate();
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onLeftDClickTemplateName(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxRichToolTip tip("Full Template file name:", wxString::Format("%s", m_inputFileName->GetHint()));
	tip.SetIcon(wxICON_INFORMATION);
	//tip.SetTipKind(wxTipKind_BottomLeft);
	tip.ShowFor(m_inputFileName);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::setControllerPowerStateBmp(bool state) {
/////////////////////////////////////////////////////////////////////
	m_ctrlPowerStateBmp->SetBitmap(ImageLibPower().Bitmap(state ? "BMP_ON": "BMP_OFF"));
	m_ctrlPowerStateBmp->Refresh();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onToggleSecMainView(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	selectSecureMonitorView();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::selectSecureMonitorView() {
/////////////////////////////////////////////////////////////////////
	m_secureMainBook->SetSelection(0);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onDClickSpeedSliderValue(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxNumberEntryDialog dlg(this, 
							"Update Value:",
							"Feed Speed [mm/min]",
							"Feed Speed Configuration . . . ", 
							defaultSpeedSlider->getValueMM_MIN() , 
							defaultSpeedSlider->getMinValueMM_MIN(), 
							defaultSpeedSlider->getMaxValueMM_MIN()
							);
	
	if ( dlg.ShowModal() == wxID_OK ) {
		dlg.GetValue();
		
		updateSpeedSlider((float)dlg.GetValue());
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::traceAllCameraDevices(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	std::clog << "Evaluate available Camera Devices . . ." << std::endl;
	
	std::vector<OSD::Device> ret;
	if ( OSD::evaluateAvailableCameraDevices(ret) == true ) {
		for ( auto it = ret.begin(); it != ret.end(); ++it ) {
			OSD::Device dv = *it;
			
			std::cout << " Device ID   : " << dv.id			<< std::endl;
			std::cout << " Device Name : " << dv.deviceName	<< std::endl;
			std::cout << " Device Path : " << dv.devicePath	<< std::endl;
			std::cout 										<< std::endl;
		}
											
		std::clog << "Finish . . ." << std::endl;
	}
	else {
		std::cout << "Cant find anything, sorry . . ." << std::endl;
	}
}
/////////////////////////////////////////////////////////////////////
double MainFrame::getConfiguredSpindleSpeed() {
/////////////////////////////////////////////////////////////////////
	if ( cnc )
		return cnc->getConfiguredSpindleSpeed();
	
	return 0.0;
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onExecuteOsk(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	openFileExtern("FreeVK.exe","",true);
}
/////////////////////////////////////////////////////////////////////
bool MainFrame::isReferenceStateValid() const {
/////////////////////////////////////////////////////////////////////
	wxASSERT ( secureCtrlPanel );
	wxASSERT ( refPositionDlg );
	
	if ( THE_CONTEXT->secureModeInfo.isActive == true )
		return secureCtrlPanel->getReferencePanel()->isReferenceStateValid();
		
	return refPositionDlg->isReferenceStateValid();
}
/////////////////////////////////////////////////////////////////////
const RefPosResult& MainFrame::getReferencePosResult(RefPosResult& result)	const {
/////////////////////////////////////////////////////////////////////
	wxASSERT ( secureCtrlPanel );
	wxASSERT ( refPositionDlg );
	
	if ( THE_CONTEXT->secureModeInfo.isActive == true )
		return secureCtrlPanel->getReferencePanel()->getResult(result);
		
	return refPositionDlg->getResult(result);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::setReferencePosEnforceFlag(bool s) {
/////////////////////////////////////////////////////////////////////
	wxASSERT ( secureCtrlPanel );
	wxASSERT ( refPositionDlg );
	
	// change the order
	if ( THE_CONTEXT->secureModeInfo.isActive == true )
	{
		refPositionDlg->setEnforceFlag(s);
		secureCtrlPanel->getReferencePanel()->setEnforceFlag(s);
	}
	else
	{
		secureCtrlPanel->getReferencePanel()->setEnforceFlag(s);
		refPositionDlg->setEnforceFlag(s);
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::resetReferencePosTempSetting() {
/////////////////////////////////////////////////////////////////////
	wxASSERT ( secureCtrlPanel );
	wxASSERT ( refPositionDlg );
	
	secureCtrlPanel->getReferencePanel()->resetTempSetting();
	refPositionDlg->resetTempSetting();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::motionMonitorZoom(float ratio) {
/////////////////////////////////////////////////////////////////////
	motionMonitor->setScaleByRatio(ratio);
	motionMonitor->Refresh();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::motionMonitorRotateX(float angle) {
/////////////////////////////////////////////////////////////////////
	motionMonitor->setAngleX(angle);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::motionMonitorRotateY(float angle) {
/////////////////////////////////////////////////////////////////////
	motionMonitor->setAngleY(angle);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::motionMonitorRotateZ(float angle) {
/////////////////////////////////////////////////////////////////////
	motionMonitor->setAngleZ(angle);
}
/////////////////////////////////////////////////////////////////////
bool MainFrame::evaluateAndPerformProcessingState() {
/////////////////////////////////////////////////////////////////////
	if ( isInterrupted() == true )
		return false;
		
	if ( THE_CONTEXT->processingInfo->getStopFlag() == true )
		return false;
		
	// pause handling
	if (THE_CONTEXT->processingInfo->isPause() ) 
	{
		CncLongPosition curStartingPos(cnc->getCurCtlPos());
		
		// pause
		const CncSpindlePowerState prevSpindleState = cnc->getSpindlePowerState();
		if ( cnc->moveZToTop() == false )
			return false;
			
		if ( cnc->switchSpindleOff(true) == false )
			return false;
			
		cnc->sendPause();
		
		while ( THE_CONTEXT->processingInfo->isPause() == true )
		{
			dispatchAll();
			motionMonitor->update();
			
			if ( isInterrupted() == true )
			{
				THE_CONTEXT->processingInfo->setPauseFlag(false);
				return false;
			}
				
			if ( THE_CONTEXT->processingInfo->getStopFlag() == true )
			{
				THE_CONTEXT->processingInfo->setPauseFlag(false);
				return false;
			}
			
			CncTimeFunctions::sleepMilliseconds(25);
		}
		
		// resume
		cnc->sendResume();
		
		if ( prevSpindleState == SPINDLE_STATE_ON )
		{
			if ( cnc->switchSpindleOn() == false )
				return false;
		}
		
		if ( cnc->moveToPos(curStartingPos) == false )
			return false;
	}
	
	return true;
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onLeftDclickLastConfigTime(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	const CncConfig::ChangeMap& cm = THE_CONFIG->getModificationMap();
	
	if ( cm.size() == 0 )
		return;
		
	ADD_LOGGER_SEPERATOR
	cnc::cex2 << "Collected Configuration Changes: [key: value changed to]" << std::endl;
	
	for ( auto it = cm.begin(); it != cm.end(); )
	{
		std::cout 	<< "  "
					<< std::left << std::setw(64) << std::setfill(' ') 
					<< it->first 
					<< ": "
					<< it->second.MakeString()
		;
		
		if ( ++it != cm.end() ) 
			std::cout << std::endl;
	}
	
	ADD_LOGGER_SEPERATOR
}


/////////////////////////////////////////////////////////////////////
void MainFrame::onIdle(wxIdleEvent& event) {
/////////////////////////////////////////////////////////////////////
	//CNC_PRINT_LOCATION
}

