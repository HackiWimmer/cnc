#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>

#include <wx/datetime.h>
#include <wx/debug.h>
#include <wx/msgdlg.h>
#include <wx/aboutdlg.h>
#include <wx/fileconf.h>
#include <wx/filedlg.h>
#include <wx/textentry.h>
#include <wx/valnum.h>
#include <wx/dcclient.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/evtloop.h>
#include <wx/webview.h>
#include <wx/dataview.h>
#include <wx/stc/stc.h>
#include <wx/wfstream.h>
#include <wx/datstrm.h>
#include <wx/txtstrm.h>
#include <wx/vscroll.h>
#include <wx/textdlg.h>
#include <wx/numformatter.h>
#include <wx/clipbrd.h>
#include "GlobalFunctions.h"
#include "SerialPort.h"
#include "CncPosition.h"
#include "CncUsbPortScanner.h"
#include "CncPatternDefinitions.h"
#include "SvgUnitCalculator.h"
#include "CncFileNameService.h"
#include "CncControllerTestSuite.h"
#include "CncFilePreviewWnd.h"
#include "SVGPathHandlerCnc.h"
#include "SVGFileParser.h"
#include "GCodeFileParser.h"
#include "CncArduino.h"
#include "SvgEditPopup.h"
#include "HexDecoder.h"
#include "UnitTestFrame.h"
#include "UpdateManagerThread.h"
#include "MainFrame.h"

// special includes for WindowPoc handling. 
// they have to be at the end of the list to avoid compilation errors
#include <windows.h>
#include <dbt.h>

////////////////////////////////////////////////////////////////////
// global strings
const char* _portEmulatorNULL 	= "<PortEmulator(dev/null)>";
const char* _portEmulatorSVG  	= "<PortEmulator(SVGFile)>";
const char* _programTitel 		= "Woodworking CNC Controller";
const char* _copyRight			= "copyright by Stefan Hoelzer 2016 - 2017";

#ifdef DEBUG
	const char* _programVersion = "0.8.0.d";
#else
	const char* _programVersion = "0.8.0.r";
#endif
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// user defined scintila style options
	enum {
			MARGIN_LINE_NUMBERS, 
			MARGIN_EDIT_TRACKER,
			MARGIN_BREAKPOINT,
			MARGIN_FOLD
	};

	enum {
			TE_DEFAULT_STYLE = 242,
			TE_BREAKPOINT_STYLE,
			TE_LINE_MODIFIED_STYLE,
			TE_LINE_SAVED_STYLE
	};
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// app defined events
	wxDEFINE_EVENT(wxEVT_UPDATE_MANAGER_THREAD, 			UpdateManagerEvent);
	wxDEFINE_EVENT(wxEVT_PERSPECTIVE_TIMER, 				wxTimerEvent);
	wxDEFINE_EVENT(wxEVT_DEBUG_USER_NOTIFICATION_TIMER, 	wxTimerEvent);
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// app defined event table
wxBEGIN_EVENT_TABLE(MainFrame, MainFrameBClass)
	EVT_CLOSE(MainFrame::onClose)
	
	EVT_COMMAND(wxID_ANY, wxEVT_CONFIG_UPDATE_NOTIFICATION, 	MainFrame::configurationUpdated)
	
	EVT_TIMER(wxEVT_PERSPECTIVE_TIMER, 							MainFrame::onPerspectiveTimer)
	EVT_TIMER(wxEVT_DEBUG_USER_NOTIFICATION_TIMER, 				MainFrame::onDebugUserNotificationTimer)
	
wxEND_EVENT_TABLE()
////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
MainFrame::MainFrame(wxWindow* parent, wxFileConfig* globalConfig)
: MainFrameBClass(parent)
, GlobalConfigManager(this, GetPgMgrSetup(), globalConfig)
, updateManagerThread(NULL)
, isDebugMode(false)
, isZeroReferenceValid(false)
, canClose(true)
, evaluatePositions(true)
, templateFileLoading(false)
, ignoreDirControlEvents(false)
, runConfirmationInfo(RunConfirmationInfo::Wait)
, traceTimerCounter(0)
, lastPortName(wxT(""))
, defaultPortName(wxT(""))
, cnc(new CncControl(CncEMU_NULL))
, motionMonitor(NULL)
, serialSpy(NULL)
, fileView(NULL)
, mainFilePreview(NULL)
, monitorFilePreview(NULL)
, toolMagaizne(NULL)
, guiCtlSetup(new GuiControlSetup())
, config(globalConfig)
, lruStore(new wxFileConfig(wxT("CncControllerLruStore"), wxEmptyString, CncFileNameService::getLruFileName(), CncFileNameService::getLruFileName(), wxCONFIG_USE_RELATIVE_PATH | wxCONFIG_USE_NO_ESCAPE_CHARACTERS))
, pathGenerator(new PathGeneratorFrame(this, m_stcFileContent))
, outboundNbInfo(new NotebookInfo(m_outboundNotebook))
, templateNbInfo(new NotebookInfo(m_templateNotebook))
, lruFileList(LruFileList(8))
, lastTemplateModification(wxDateTime::UNow())
, lastSvgEmuModification(wxDateTime::UNow())
, processLastDuartion(0L)
, processStartTime(wxDateTime::UNow())
, processEndTime(wxDateTime::UNow())
, lastTemplateFileNameForPreview(wxT(""))
, pngAnimation(NULL)
, stcFileContentPopupMenu(NULL)
, stcEmuContentPopupMenu(NULL)
, inboundFileParser(NULL)
, perspectiveTimer(this, wxEVT_PERSPECTIVE_TIMER)
, debugUserNotificationTime(this, wxEVT_DEBUG_USER_NOTIFICATION_TIMER)
{
///////////////////////////////////////////////////////////////////
	// determine assert handler
	wxSetDefaultAssertHandler();
			
	// initilazied update mananger thread
	initializeUpdateManagerThread();
	
	// setup aui clear
	hideAllAuiPanes();
	
	// decocate application
	setIcons();
	
	// do this definitely here later it will causes a crash 
	installCustControls();
	
	// debugger configuration
	FileParser::installDebugConfigPage(m_debuggerPropertyManagerGrid);
	
	// define the popup parent frame
	SvgEditPopup::setMainFrame(this);
	
	// bind 
	this->Bind(wxEVT_CHAR_HOOK, 				&MainFrame::globalKeyDownHook, 				this);
	this->Bind(wxEVT_UPDATE_MANAGER_THREAD, 	&MainFrame::onThreadAppPosUpdate, 			this, UpdateManagerThread::EventId::APP_POS_UPDATE);
	this->Bind(wxEVT_UPDATE_MANAGER_THREAD, 	&MainFrame::onThreadCtlPosUpdate, 			this, UpdateManagerThread::EventId::CTL_POS_UPDATE);
	this->Bind(wxEVT_UPDATE_MANAGER_THREAD, 	&MainFrame::onThreadHeartbeat, 				this, UpdateManagerThread::EventId::HEARTBEAT);
	this->Bind(wxEVT_UPDATE_MANAGER_THREAD, 	&MainFrame::onThreadDispatchAll,			this, UpdateManagerThread::EventId::DISPATCH_ALL);
	this->Bind(wxEVT_UPDATE_MANAGER_THREAD, 	&MainFrame::onThreadCompletion, 			this, UpdateManagerThread::EventId::COMPLETED);
}
///////////////////////////////////////////////////////////////////
MainFrame::~MainFrame() {
///////////////////////////////////////////////////////////////////
	// stop the serial timer and wait interval to finish the work behind
	m_serialTimer->Stop();
	if ( cnc != NULL )
		waitActive(m_serialTimer->GetInterval());
	
	// unbind 
	this->Unbind(wxEVT_CHAR_HOOK, 				&MainFrame::globalKeyDownHook, 				this);
	this->Unbind(wxEVT_UPDATE_MANAGER_THREAD, 	&MainFrame::onThreadAppPosUpdate, 			this, UpdateManagerThread::EventId::APP_POS_UPDATE);
	this->Unbind(wxEVT_UPDATE_MANAGER_THREAD, 	&MainFrame::onThreadCtlPosUpdate, 			this, UpdateManagerThread::EventId::CTL_POS_UPDATE);
	this->Unbind(wxEVT_UPDATE_MANAGER_THREAD, 	&MainFrame::onThreadHeartbeat, 				this, UpdateManagerThread::EventId::HEARTBEAT);
	this->Unbind(wxEVT_UPDATE_MANAGER_THREAD, 	&MainFrame::onThreadDispatchAll, 			this, UpdateManagerThread::EventId::DISPATCH_ALL);
	this->Unbind(wxEVT_UPDATE_MANAGER_THREAD, 	&MainFrame::onThreadCompletion, 			this, UpdateManagerThread::EventId::COMPLETED);
	
	// todo
	//this->Unbind(wxEVT_COMMAND_MENU_SELECTED, [](wxCommandEvent& event) {});
	
	wxASSERT(lruStore);
	lruFileList.save(lruStore);
	lruStore->Flush();
	delete lruStore;
	
	wxASSERT(config);
	config->Flush();
	delete config;
	
	wxASSERT(pathGenerator);
	pathGenerator->Destroy();
	delete pathGenerator;

	wxASSERT(guiCtlSetup);
	delete guiCtlSetup;
	
	wxASSERT(outboundNbInfo);
	delete outboundNbInfo;
	
	wxASSERT(templateNbInfo);
	delete templateNbInfo;
	
	SvgEditPopup::destroyMenu(stcFileContentPopupMenu);
	SvgEditPopup::destroyMenu(stcEmuContentPopupMenu);

	wxASSERT (cnc);
	delete cnc;
}
///////////////////////////////////////////////////////////////////
void MainFrame::globalKeyDownHook(wxKeyEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( motionMonitor && motionMonitor->IsShownOnScreen() ) {
		// This is necessary to avoid the default notebook key handling
		if ( motionMonitor->HasFocus() ) {
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
	// overides the from wxcrafter generted method
	event.Skip();
	
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
					
					// dont use the toolbar event handler because this will generate a crash will cnc::waitActive is in action!
					// toolbar->PopupMenu(iter->second, pt);
					this->PopupMenu(iter->second, pt);
					
				}
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////
void MainFrame::configurationUpdated(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////////////
	#warning todo -impl MainFrame::configurationUpdated
	//std::clog << "MainFrame::configurationUpdated(wxCommandEvent& event)" << std::endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::setRefPostionState(bool state) {
///////////////////////////////////////////////////////////////////
	isZeroReferenceValid = state;

	wxBitmap bmp;
	if ( isZeroReferenceValid == true ) bmp = ImageLib24().Bitmap("BMP_TRAFFIC_LIGHT_GREEN"); 
	else 								bmp = ImageLib24().Bitmap("BMP_TRAFFIC_LIGHT_RED");
	
	wxImage img = bmp.ConvertToImage();
	img = img.Rotate90(false);
	
	wxBitmap b(img);
	m_refPosTrafficLight->SetBitmap(b);
	m_statusBar->Refresh();
	m_statusBar->Update();
}
///////////////////////////////////////////////////////////////////
void MainFrame::registerGuiControl(wxWindow* ctl) {
///////////////////////////////////////////////////////////////////
	if ( ctl == NULL )
		return;
		
	guiControls.push_back(ctl);
}		
///////////////////////////////////////////////////////////////////
void MainFrame::disableGuiControls() {
///////////////////////////////////////////////////////////////////
	enableGuiControls(false);
}
///////////////////////////////////////////////////////////////////
void MainFrame::enableGuiControls(bool state) {
///////////////////////////////////////////////////////////////////
	for ( GuiControls::iterator it = guiControls.begin(); it != guiControls.end(); ++it ) {
		if ( (*it) != NULL )
			(*it)->Enable(state);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::installCustControls() {
///////////////////////////////////////////////////////////////////
	// Montion Monitor
	motionMonitor = new CncMotionMonitor(this, NULL);
	GblFunc::replaceControl(m_drawPane3D, motionMonitor);
	activate3DPerspectiveButton(m_3D_Perspective1);
	
	//Spy
	serialSpy = new CncSpyControl(this, wxID_ANY, m_serialSpyDetails);
	GblFunc::replaceControl(m_serialSpy, serialSpy);
	
	// File View
	fileView = new CncFileView(this);
	GblFunc::replaceControl(m_mainFileViewPlaceholder, fileView);
	
	// File Preview
	mainFilePreview = new CncFilePreview(this);
	GblFunc::replaceControl(m_filePreviewPlaceholder, mainFilePreview);
	
	// File Preview
	monitorFilePreview = new CncFilePreview(this);
	GblFunc::replaceControl(m_monitorTemplatePreviewPlaceHolder, monitorFilePreview);
	
	// tool magazine
	toolMagaizne = new CncToolMagazine(this); 
	GblFunc::replaceControl(m_toolMagazinePlaceholder, toolMagaizne);
}
///////////////////////////////////////////////////////////////////
void MainFrame::registerGuiControls() {
///////////////////////////////////////////////////////////////////

	registerGuiControl(m_btSelectReferences);
	registerGuiControl(m_btSelectManuallyMove);
	registerGuiControl(m_btSelectSetup);
	registerGuiControl(m_btSelectTemplate);
	registerGuiControl(m_cbCurveLibResolution);
	registerGuiControl(m_3D_Refreh);
	registerGuiControl(m_3D_Clear);

	registerGuiControl(m_cbContentPosSpy);
	registerGuiControl(m_btPathGenerator);
	registerGuiControl(m_checkManuallyXY);
	registerGuiControl(m_checkManuallyZ);
	registerGuiControl(m_testToggleTool);
	registerGuiControl(m_testToggleEndSwitch);
	registerGuiControl(m_ctrlTestSelection);
	registerGuiControl(m_portSelector);
	registerGuiControl(m_testDimModeX);
	registerGuiControl(m_testDimModeY);
	registerGuiControl(m_testDimModeZ);
	registerGuiControl(m_testDimTakeX);
	registerGuiControl(m_testDimTakeY);
	registerGuiControl(m_testDimTakeZ);
	registerGuiControl(m_testDimTakeAll);
	registerGuiControl(m_connect);
	registerGuiControl(m_zToTop);
	registerGuiControl(m_zToBottom);
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
	registerGuiControl(m_setZero);
	registerGuiControl(m_clearLogger);
	registerGuiControl(m_cbUpdateInterval);
	registerGuiControl(m_svgEmuOpenFileAsSvg);
	registerGuiControl(m_svgEmuOpenFileAsSource);
	registerGuiControl(m_svgEmuReload);
	registerGuiControl(m_svgEmuClear);
	registerGuiControl(m_spinButtonX);
	registerGuiControl(m_spinButtonY);
	registerGuiControl(m_spinButtonZ);
	registerGuiControl(m_moveXYAxisCtl);
	registerGuiControl(m_moveZAxisCtl);
	registerGuiControl(m_reloadTemplate);
	registerGuiControl(m_openSourceExtern);
	registerGuiControl(m_openSvgExtern);
	registerGuiControl(m_btRequestCtlConfig);
	registerGuiControl(m_btRequestCtlPins);
	registerGuiControl(m_btRequestCtlErrorInfo);
	registerGuiControl(m_lruList);
	registerGuiControl(fileView);
	registerGuiControl(m_svgEmuResult);
	registerGuiControl(m_svgEmuOpenFileAsSvg);
	registerGuiControl(m_svgEmuReload);
	registerGuiControl(m_svgEmuClear);
	registerGuiControl(m_svgEmuToggleOrigPath);
	registerGuiControl(m_svgEmuZoomHome);
	registerGuiControl(m_svgEmuZoomMinus);
	registerGuiControl(m_svgEmuZoomPlus);
	registerGuiControl(m_copyLogger);
	registerGuiControl(m_btSvgToggleWordWrap);
	registerGuiControl(m_svgEmuToggleWordWrap);
	registerGuiControl(m_svgEmuToggleOrigPath);
	registerGuiControl(m_switchMonitoing);
	registerGuiControl(m_saveTemplate);
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
void MainFrame::displayNotification(const char type, wxString title, wxString message, unsigned int timeout) {
///////////////////////////////////////////////////////////////////
	wxSharedPtr<wxNotificationMessageBase> dlg;
	dlg = new wxGenericNotificationMessage(title, message, this);
	switch ( type ) {
		case 'E':	dlg->SetFlags(wxICON_ERROR); break;
		case 'W':	dlg->SetFlags(wxICON_WARNING); break;
		default:	dlg->SetFlags(wxICON_INFORMATION); break;
	}
	
	dlg->Show(timeout);
	// important to free the shared Pointer!
	dlg.reset();
}
///////////////////////////////////////////////////////////////////
void MainFrame::testFunction1(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	cnc::trc.logInfoMessage("Test function 1");
	
	
	clog << cnc->getSerial()->getTotalDistanceX() << endl;
	
}
///////////////////////////////////////////////////////////////////
void MainFrame::testFunction2(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	cnc::trc.logInfoMessage("Test function 2");
	
	/*
	cout << "testFunction2"<< endl;
	clog << "testFunction2"<< endl;
	cerr << "testFunction2"<< endl;
	*/
}
///////////////////////////////////////////////////////////////////
void MainFrame::testFunction3(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	cnc::trc.logInfoMessage("Test function 3");
	
	//cnc::cex1 << GetAuimgrMain()->SavePerspective() << endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::testFunction4(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	cnc::trc.logInfoMessage("Test function 4");
	/*
	cnc::trc.green();
	cnc::trc << " Hallo";
	cnc::trc.blue();
	cnc::trc << " Hallo";
	cnc::trc.gray();
	cnc::trc << " Hallo";
	cnc::trc.blue();
	cnc::trc << " Hallo";
	cnc::trc.resetTextAttr();
	cnc::trc << " Hallo";
	*/
	
	cnc::trc.logWarning("This is a test warinig");
}
///////////////////////////////////////////////////////////////////
void MainFrame::startupTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	// Setup AUI Windows menue
	loadPerspective("Run");
	decorateViewMenu();
	
	// Auto connect ?
	if ( CncConfig::getGlobalCncConfig()->getAutoConnectFlag() )
		connectSerialPort();
		
	// Auto process ?
	if ( CncConfig::getGlobalCncConfig()->getAutoProcessFlag() ) {
		defineMinMonitoring();
		processTemplate();
		defineNormalMonitoring();
	}

	//todo - only temp
	//wxCommandEvent dummy;
	//openSVGPathGenerator(dummy);
}
///////////////////////////////////////////////////////////////////
void MainFrame::traceTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	// trace info handling
	if ( m_tmpTraceInfo->GetValue().IsEmpty() ) traceTimerCounter = 0;
	else 										traceTimerCounter += event.GetInterval();
	
	if ( traceTimerCounter > 4000 ) {
		traceTimerCounter = 0;
		m_tmpTraceInfo->Clear();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::serialTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	// idle handling
	if ( m_miRqtIdleMessages->IsChecked() == true ) {
		if ( m_connect->IsEnabled() == false )
			return;
		
		if ( cnc == NULL )
			return;
			
		if ( cnc->isConnected() == false )
			return;
			
		m_serialTimer->Stop();
		cnc->sendIdleMessage();
		m_serialTimer->Start();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::onThreadAppPosUpdate(UpdateManagerEvent& event) {
///////////////////////////////////////////////////////////////////
	CncUnit unit = GBL_CONFIG->getDisplayUnit();
	
	// update position
	switch ( unit ) {
		case CncSteps:	// update application position
						m_xAxis->ChangeValue(wxString::Format("%8ld", event.getCurrentPosition().getX()));
						m_yAxis->ChangeValue(wxString::Format("%8ld", event.getCurrentPosition().getY()));
						m_zAxis->ChangeValue(wxString::Format("%8ld", event.getCurrentPosition().getZ()));
						break;
						
		case CncMetric:	// update application position
						m_xAxis->ChangeValue(wxString::Format("%4.3lf", event.getCurrentPosition().getX() * GBL_CONFIG->getDisplayFactX(unit)));
						m_yAxis->ChangeValue(wxString::Format("%4.3lf", event.getCurrentPosition().getY() * GBL_CONFIG->getDisplayFactY(unit)));
						m_zAxis->ChangeValue(wxString::Format("%4.3lf", event.getCurrentPosition().getZ() * GBL_CONFIG->getDisplayFactZ(unit)));
						break;
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::onThreadCtlPosUpdate(UpdateManagerEvent& event) {
///////////////////////////////////////////////////////////////////
	CncUnit unit = GBL_CONFIG->getDisplayUnit();
	
	// update position
	switch ( unit ) {
		case CncSteps:	// update controller position
						m_xAxisCtl->ChangeValue(wxString::Format("%8ld", event.getCurrentPosition().getX()));
						m_yAxisCtl->ChangeValue(wxString::Format("%8ld", event.getCurrentPosition().getY()));
						m_zAxisCtl->ChangeValue(wxString::Format("%8ld", event.getCurrentPosition().getZ()));
						break;
						
		case CncMetric:	// update controller position
						m_xAxisCtl->ChangeValue(wxString::Format("%4.3lf", event.getCurrentPosition().getX() * GBL_CONFIG->getDisplayFactX(unit)));
						m_yAxisCtl->ChangeValue(wxString::Format("%4.3lf", event.getCurrentPosition().getY() * GBL_CONFIG->getDisplayFactY(unit)));
						m_zAxisCtl->ChangeValue(wxString::Format("%4.3lf", event.getCurrentPosition().getZ() * GBL_CONFIG->getDisplayFactZ(unit)));
						break;
	}
	
	// update z view
	m_zView->updateView(event.getCurrentPosition().getZ() * GBL_CONFIG->getDisplayFactZ(unit));
}
///////////////////////////////////////////////////////////////////
void MainFrame::onThreadHeartbeat(UpdateManagerEvent& event) {
///////////////////////////////////////////////////////////////////
	// rotate heartbeat star
	wxImage img =m_updateManagerUpdate->GetBitmap().ConvertToImage();
	wxPoint p(m_updateManagerUpdate->GetSize().GetWidth()/2, m_updateManagerUpdate->GetSize().GetHeight()/2);

	m_updateManagerUpdate->SetBitmap(wxBitmap(img.Rotate90()));
	m_updateManagerUpdate->Refresh();
	m_updateManagerUpdate->Update();
	
	// update position syp
	static unsigned int lastCount = 0;
	if ( m_btTogglePosSpy->GetValue() == true ) {
		if ( updateManagerThread && m_positionSpy->IsShownOnScreen() ) {
			
			UpdateManagerThread::SpyContent sc = (UpdateManagerThread::SpyContent)m_cbContentPosSpy->GetSelection();
			m_positionSpy->Freeze();
			updateManagerThread->fillPositionSpy(m_positionSpy, sc, *GBL_CONFIG);
			m_positionSpy->Thaw();
		}
	}

	if ( lastCount != m_positionSpy->GetCount() ) {
		//m_positionSpyCount->SetLabel(wxString::Format("# %010u", m_positionSpy->GetCount()));
		m_positionSpyCount->SetLabel(wxNumberFormatter::ToString(((long)(m_positionSpy->GetCount()))));
		m_positionSpyCount->SetToolTip(m_positionSpyCount->GetLabel());
		lastCount = m_positionSpy->GetCount();
	}
	
	// update time consumed
	if ( pngAnimation && pngAnimation->IsRunning() ) {
		processEndTime = wxDateTime::UNow();
		logTimeConsumed();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::onThreadDispatchAll(UpdateManagerEvent& event) {
///////////////////////////////////////////////////////////////////
	dispatchAll();
}
///////////////////////////////////////////////////////////////////
void MainFrame::onThreadCompletion(UpdateManagerEvent& event) {
///////////////////////////////////////////////////////////////////
	std::clog << "OnThreadCompletion" << std::endl;
	updateManagerThread = NULL;
}
///////////////////////////////////////////////////////////////////
void MainFrame::onClose(wxCloseEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( checkIfTemplateIsModified() == false )
		return;
		
	if ( canClose == false) {
		if ( m_svgParseMode->GetLabel().Contains("Debug") ) {
			wxString msg("A debug session is currently active. Should the application yet closed?");
			wxMessageDialog dlg(this, msg, _T("Close application. . . "), 
		                    wxOK|wxCANCEL|wxCENTRE|wxICON_QUESTION);
			
			if ( dlg.ShowModal() == wxID_CANCEL ) {
				cnc::trc.logWarning("Can't close the application due to an active debug session");
				return;
			}
			
			if ( inboundFileParser != NULL )
				inboundFileParser->debugStop();
		} else {
			cnc::trc.logWarning("Can't close the application due to an active run session");
			return;
		}
	}
	
	// Destroy the update manager thread
	if ( updateManagerThread != NULL) {
		updateManagerThread->stop();
		
		while ( true ) {
			{ // was the ~MyThread() function executed?
				wxCriticalSectionLocker enter(pThreadCS);
				if ( !updateManagerThread ) 
					break;
			}
			// wait for thread completion
			wxThread::This()->Sleep(10);
		}
	}
	
	Destroy();
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
	wxDateTime e = wxDateTime::UNow();

	wxEventLoopBase* evtLoop = wxEventLoopBase::GetActive();
	if ( evtLoop == NULL )
		return;

	while ( wxTimeSpan(e - s).GetMilliseconds() < milliseconds ) {
		while ( evtLoop->Pending() ) {
			evtLoop->Dispatch();
			
			e = wxDateTime::UNow();
			if ( wxTimeSpan(e - s).GetMilliseconds() < milliseconds )
				break;
		}
		
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
void MainFrame::dispatchAll() {
///////////////////////////////////////////////////////////////////
	wxEventLoopBase* evtLoop = wxEventLoopBase::GetActive();
	if ( evtLoop == NULL )
		return;
		
	/*
	Please note: This is th fastest version, but evtLoop->Yield() is better then the code below, 
	it also considers timer events, aui-handling etc.
	
	while ( evtLoop->Pending() )
		evtLoop->Dispatch();
	*/

	/*
	wxEVT_CATEGORY_ALL =
		wxEVT_CATEGORY_UI|wxEVT_CATEGORY_USER_INPUT|wxEVT_CATEGORY_SOCKET| \
		wxEVT_CATEGORY_TIMER|wxEVT_CATEGORY_THREAD|wxEVT_CATEGORY_UNKNOWN| \
		wxEVT_CATEGORY_CLIPBOARD
	
	This is the slowest version
	evtLoop->YieldFor(wxEVT_CATEGORY_ALL);
	*/
	
	// the following cod is the best compromise, but aui handling isn't perfect
	if ( wxTheApp->HasPendingEvents() )
		wxTheApp->ProcessPendingEvents();
		
	while ( evtLoop->Pending() )
		evtLoop->Dispatch();
	
}
///////////////////////////////////////////////////////////////////
void MainFrame::initTemplateEditStyle() {
///////////////////////////////////////////////////////////////////
	initTemplateEditStyle(m_stcFileContent, getCurrentTemplateFormat());
	initTemplateEditStyle(m_stcEmuSource, TplSvg);
}
///////////////////////////////////////////////////////////////////
void MainFrame::initTemplateEditStyle(wxStyledTextCtrl* ctl, TemplateFormat format) {
///////////////////////////////////////////////////////////////////
	wxASSERT(ctl);
	
	// Define used fonts
	wxFont defaultFont(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	wxFont staticFont  = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
	wxFont font;
	
	//define colours
	wxColour clDefaultBck(0, 0, 0);
	wxColour clDefaultFgd(175, 175, 175);
	
	wxColour clCaretBck(64, 64, 64);
	wxColour clCaretFgd(*wxWHITE);
	
	wxColour clComment(150, 150, 150);
	wxColour clIdentifier(135, 206, 250);
	wxColour clAttribute(128, 139, 237);
	wxColour clNumber(128,255,128);
	wxColour clOperator(255,128,128);
	
	// Reset all sytles
	ctl->StyleClearAll();

	// setup black background as default
	for ( unsigned int i=0; i<wxSTC_STYLE_MAX; i++) {
		ctl->StyleSetBackground (i, clDefaultBck);
	}
	
	// setup gray foreground as default
	for ( unsigned int i=0; i<wxSTC_STYLE_MAX; i++) {
		ctl->StyleSetForeground (i, clDefaultFgd);
	}
	
	// setup default font
	for ( unsigned int i=0; i<wxSTC_STYLE_MAX; i++) {
		ctl->StyleSetFont(i, defaultFont);
	}

	// Reset folding
	ctl->SetProperty(wxT("xml.auto.close.tags"), 		wxT("0"));
	ctl->SetProperty(wxT("lexer.xml.allow.scripts"), 	wxT("0"));
	ctl->SetProperty(wxT("fold"), 						wxT("0"));
	ctl->SetProperty(wxT("fold.comment"),				wxT("0"));
	ctl->SetProperty(wxT("fold.compact"), 				wxT("0"));
	ctl->SetProperty(wxT("fold.preprocessor"), 			wxT("0"));
	ctl->SetProperty(wxT("fold.html"), 					wxT("0"));
	ctl->SetProperty(wxT("fold.html.preprocessor"), 	wxT("0"));
	ctl->SetMarginMask(MARGIN_FOLD, wxSTC_MASK_FOLDERS);
	ctl->SetMarginWidth(MARGIN_FOLD, 0);
	ctl->SetMarginSensitive(MARGIN_FOLD, false);
	ctl->SetFoldMarginColour(true, wxColour(73, 73, 73));
	ctl->SetFoldMarginHiColour(true, *wxBLACK);
	
	// Set default styles 
	ctl->StyleSetForeground(TE_DEFAULT_STYLE, clDefaultFgd);
	ctl->StyleSetBackground(TE_DEFAULT_STYLE, wxColour(73, 73, 73));
	ctl->StyleSetBackground(TE_BREAKPOINT_STYLE, wxColour(128, 0, 0));
	ctl->StyleSetForeground(TE_BREAKPOINT_STYLE, *wxWHITE);
	ctl->StyleSetBackground(TE_LINE_SAVED_STYLE, wxColour(wxT("FOREST GREEN")));
	ctl->StyleSetBackground(TE_LINE_MODIFIED_STYLE, wxColour(wxT("ORANGE")));
	ctl->StyleSetForeground(wxSTC_STYLE_LINENUMBER, clDefaultFgd);
	ctl->StyleSetBackground(wxSTC_STYLE_LINENUMBER, wxColour(73, 73, 73));
	ctl->SetTabWidth(4);
	ctl->SetWrapMode(wxSTC_WRAP_NONE);
	ctl->SetReadOnly(false);
	
	// Enable line numbers
	ctl->SetMarginWidth(MARGIN_LINE_NUMBERS, 35);
	ctl->SetMarginType(MARGIN_LINE_NUMBERS, wxSTC_MARGIN_NUMBER);
	ctl->SetMarginSensitive(MARGIN_LINE_NUMBERS, true);
	
	// Enable breakpoint
	ctl->SetMarginWidth(MARGIN_BREAKPOINT, 8);
	ctl->SetMarginType(MARGIN_BREAKPOINT, wxSTC_MARGIN_TEXT);
	ctl->SetMarginMask(MARGIN_BREAKPOINT, 0);
	ctl->SetMarginSensitive(MARGIN_BREAKPOINT, true);

	
	// Enable edit style - file content marker
	ctl->SetMarginWidth(MARGIN_EDIT_TRACKER, 3);
	ctl->SetMarginType(MARGIN_EDIT_TRACKER, wxSTC_MARGIN_SYMBOL); 
	ctl->SetMarginMask(MARGIN_EDIT_TRACKER, 0);

	
	// Configure caret style
	ctl->SetCaretForeground(clCaretFgd);
	ctl->SetSelBackground(true, clCaretBck);
	
	// Configure selection colours
	//ctl->SetSelForeground(true, wxColour(255,201,14));
	ctl->SetSelBackground(true, wxColour(83,83,83));
	
	// Set specific styles
	switch ( format ) {
		case TplSvg:

			ctl->SetLexer(wxSTC_LEX_HTML);
			
			// setup highlight colours
			ctl->StyleSetForeground(wxSTC_H_DOUBLESTRING,		wxColour(255, 	205, 	139));
			ctl->StyleSetForeground(wxSTC_H_SINGLESTRING,		wxColour(255,	205, 	139));
			ctl->StyleSetForeground(wxSTC_H_ENTITY,				wxColour(255,	0, 		0));
			ctl->StyleSetForeground(wxSTC_H_TAGUNKNOWN,			wxColour(0,		150, 	0));
			ctl->StyleSetForeground(wxSTC_H_ATTRIBUTEUNKNOWN,	wxColour(0,		0, 		150));
			ctl->StyleSetForeground(wxSTC_H_ATTRIBUTE,			clAttribute);
			ctl->StyleSetForeground(wxSTC_H_TAG,				clIdentifier);
			ctl->StyleSetForeground(wxSTC_H_COMMENT,			clComment);
			
			// setup folding
			ctl->SetProperty(wxT("xml.auto.close.tags"), 		wxT("1"));
			ctl->SetProperty(wxT("lexer.xml.allow.scripts"), 	wxT("1"));
			ctl->SetProperty(wxT("fold"), 						wxT("1"));
			ctl->SetProperty(wxT("fold.comment"),				wxT("1"));
			ctl->SetProperty(wxT("fold.compact"), 				wxT("1"));
			ctl->SetProperty(wxT("fold.preprocessor"), 			wxT("1"));
			ctl->SetProperty(wxT("fold.html"), 					wxT("1"));
			ctl->SetProperty(wxT("fold.html.preprocessor"), 	wxT("1"));
			
			ctl->MarkerDefine(wxSTC_MARKNUM_FOLDER,        		wxSTC_MARK_BOXPLUS, 			clDefaultBck, clDefaultFgd);
			ctl->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN,    		wxSTC_MARK_BOXMINUS, 			clDefaultBck, clDefaultFgd);
			ctl->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB,    		wxSTC_MARK_VLINE,    			clDefaultBck, clDefaultFgd);
			ctl->MarkerDefine(wxSTC_MARKNUM_FOLDEREND,     		wxSTC_MARK_BOXPLUSCONNECTED,	clDefaultBck, clDefaultFgd);
			ctl->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, 		wxSTC_MARK_BOXMINUSCONNECTED, 	clDefaultBck, clDefaultFgd);
			ctl->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL,		wxSTC_MARK_TCORNER,     		clDefaultBck, clDefaultFgd);
			ctl->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL,    		wxSTC_MARK_LCORNER,     		clDefaultBck, clDefaultFgd);

			ctl->SetMarginMask(MARGIN_FOLD, wxSTC_MASK_FOLDERS);
			ctl->SetMarginWidth(MARGIN_FOLD, 32);
			ctl->SetMarginSensitive(MARGIN_FOLD, true);
			ctl->SetFoldFlags(wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED | 16);
			
			break;
			
		case TplGcode:
			ctl->SetLexer(wxSTC_LEX_GCODE);
			
			// setup highlight colours
			ctl->StyleSetForeground (wxSTC_GCODE_OPERATOR,		clOperator);
			ctl->StyleSetForeground (wxSTC_GCODE_NUMBER,		clNumber);
			ctl->StyleSetForeground (wxSTC_GCODE_IDENTIFIER,	clIdentifier);
			ctl->StyleSetForeground (wxSTC_GCODE_PARAM,			clAttribute);
			ctl->StyleSetForeground (wxSTC_GCODE_COMMENT,		clComment);
			ctl->StyleSetForeground (wxSTC_GCODE_COMMENT_LINE,	clComment);
			ctl->StyleSetForeground (wxSTC_GCODE_DIRECTIVE,		clComment);

			font = (ctl->StyleGetFont(wxSTC_GCODE_IDENTIFIER)).Bold();
			ctl->StyleSetFont(wxSTC_GCODE_IDENTIFIER, font);
			
			font = (ctl->StyleGetFont(wxSTC_GCODE_PARAM)).Bold();
			ctl->StyleSetFont(wxSTC_GCODE_PARAM, font);
			break;
			
		default:
			;// do nothing
	}	
}
///////////////////////////////////////////////////////////////////
WXLRESULT MainFrame::MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam) {
///////////////////////////////////////////////////////////////////
	wxString portName("Undefined");
	if ( message == WM_DEVICECHANGE) {
		PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;
		
		// logging
		switch ( wParam ) {
			case DBT_DEVICEARRIVAL:			if ( lpdb->dbch_devicetype == DBT_DEVTYP_PORT ) {
												PDEV_BROADCAST_PORT pPort = (PDEV_BROADCAST_PORT) lpdb;
												wxString n(pPort->dbcp_name);
												portName.assign(n);
											}
											
											cnc::trc.logInfo(wxString("A new COM device was detected on port: ") << portName);
											break;
											
			case DBT_DEVICEREMOVECOMPLETE:	if ( lpdb->dbch_devicetype == DBT_DEVTYP_PORT ) {
												PDEV_BROADCAST_PORT pPort = (PDEV_BROADCAST_PORT) lpdb;
												wxString n(pPort->dbcp_name);
												portName.assign(n);
											}
											
											cnc::trc.logInfo(wxString("The COM device was removed from port: ") << portName);
											break;
											
			default: ;
		}
	}
	
	// do all the default stuff here first
	WXLRESULT ret = wxFrame::MSWWindowProc ( message, wParam, lParam );
	
	// do some more actions 
	if ( message == WM_DEVICECHANGE) {
		// update port selector
		if ( wParam == DBT_DEVICEARRIVAL || wParam == DBT_DEVICEREMOVECOMPLETE )
			decoratePortSelector();
		
		switch ( wParam ) {
			// ask for connect - on demand . . . 
			case DBT_DEVICEARRIVAL:	{
				if ( isProcessing() == false ) {
					wxString msg("Should a connection established to port: ");
					msg.append(portName);
					wxMessageDialog dlg(this, msg, _T("New connection available. Try to connect  . . . "), wxYES|wxNO|wxCENTRE|wxICON_QUESTION);
					if ( dlg.ShowModal() == wxID_YES ) {
						m_portSelector->SetStringSelection(portName);
						connectSerialPort();
					}
				}
			}
			break;
			
			// check if current com connection is effected
			case DBT_DEVICEREMOVECOMPLETE:	{
				// check if the current connection is effected
				if ( lastPortName == portName )
					if ( cnc && cnc->isConnected() ) {
						cnc->interrupt();
						cnc->getSerial()->disconnect();
						lastPortName.clear();
						std::cerr << "Connection brocken" << std::endl;
						cnc::trc.logWarning("Connection broken . . ."); 
					}
				break;
			}
			default: ;
		}
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void MainFrame::searchAvailiablePorts(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	disableControls();
	decoratePortSelector(GetAsyncKeyState(VK_CONTROL) != 0);
	enableControls();
	
	if ( m_portSelector->FindString(lastPortName) == wxNOT_FOUND ) {
		m_portSelector->Popup();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::decoratePortSelector(bool list) {
///////////////////////////////////////////////////////////////////
	startAnimationControl();
	m_portSelector->Clear();
	// add default ports
	if ( lastPortName == _portEmulatorNULL )	m_portSelector->Append(_portEmulatorNULL, ImageLibPortSelector().Bitmap("BMP_PS_CONNECTED"));
	else										m_portSelector->Append(_portEmulatorNULL, ImageLibPortSelector().Bitmap("BMP_PS_AVAILABLE"));
	
	if ( lastPortName == _portEmulatorSVG )		m_portSelector->Append(_portEmulatorSVG, ImageLibPortSelector().Bitmap("BMP_PS_CONNECTED"));
	else										m_portSelector->Append(_portEmulatorSVG, ImageLibPortSelector().Bitmap("BMP_PS_AVAILABLE"));
	
	// add com ports
	int pStart 	= 0;
	int pEnd 	= 256;
	
	if ( list == true ) {
		pStart 	= 1;
		pEnd 	= 11;
	}
	
	for (int i=pStart; i<pEnd; i++) {
		int ret = CncUsbPortScanner::isComPortAvailable(i);
		wxString pn(wxString::Format("COM%d", i));
		
		switch ( ret ) {
			case 0:		if ( cnc && cnc->isConnected() && lastPortName == pn )
							m_portSelector->Append(pn, ImageLibPortSelector().Bitmap("BMP_PS_CONNECTED"));
						else
							m_portSelector->Append(pn, ImageLibPortSelector().Bitmap("BMP_PS_ACCESS_DENIED"));
							
						break;
			case 1:
						m_portSelector->Append(pn, ImageLibPortSelector().Bitmap("BMP_PS_AVAILABLE"));
						break;
						
			default: 	if ( list == true )
							m_portSelector->Append(pn, ImageLibPortSelector().Bitmap("BMP_PS_UNKNOWN"));
		}
	}
	
	// select the last port, if availiable
	if ( m_portSelector->FindString(lastPortName) != wxNOT_FOUND )
		m_portSelector->SetStringSelection(lastPortName);
		
	stopAnimationControl();
}

///////////////////////////////////////////////////////////////////
void MainFrame::setIcons() {
///////////////////////////////////////////////////////////////////
	// Set icon(s) to the application/dialog
	wxIconBundle app_icons;

#ifdef DEBUG
	wxString iconTyp("D");
#else
	wxString iconTyp("R");
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
void MainFrame::initializeUpdateManagerThread() {
///////////////////////////////////////////////////////////////////
	// create the thread
	updateManagerThread = new UpdateManagerThread(this);
	wxThreadError error = updateManagerThread->Create();

	if (error != wxTHREAD_NO_ERROR) {
		wxMessageBox( _("Couldn't create update maanager thread!") );
		abort();
	}
	
	error = updateManagerThread->Run();
	if (error != wxTHREAD_NO_ERROR) {
		wxMessageBox( _("Couldn't run update manager thread!") );
		abort();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::initialize(void) {
///////////////////////////////////////////////////////////////////
	
	lruFileList.setListControl(m_lruList);
	
	createAnimationControl();
	createStcFileControlPopupMenu();
	createStcEmuControlPopupMenu();
	decorateSearchButton();
	switchMonitorButton(true);
	determineRunMode();
	registerGuiControls();
	enableManuallyControls();
	initTemplateEditStyle();
	
	CncControllerTestSuite::fillTestCases(m_ctrlTestSelection);
	decorateTestSuiteParameters();
	
	this->SetTitle(wxString(_programTitel) + " " + _programVersion);
	
	wxString cfgStr;
	
	// setup cnc port selector box
	decoratePortSelector();
	
	wxFloatingPointValidator<float> val8(3, NULL, wxNUM_VAL_DEFAULT );//, wxNUM_VAL_ZERO_AS_BLANK);
	val8.SetRange(0, 100.0);
	m_testDistanceX->SetValidator(val8);
	m_testDistanceY->SetValidator(val8);
	m_testDistanceZ->SetValidator(val8);
	
	wxIntegerValidator<long> val9(NULL);
	m_ctrlTestParam1->SetValidator(val9);
	m_ctrlTestParam2->SetValidator(val9);
	m_ctrlTestParam3->SetValidator(val9); 
	m_ctrlTestParam4->SetValidator(val9);
	m_ctrlTestParam5->SetValidator(val9);
	
	wxTextValidator tVal(wxFILTER_NUMERIC);
	tVal.SetCharIncludes(", ");
	m_cbUCValueFrom->SetValidator(tVal);
	
	wxString comment("");
	comment << "+/-x:           cursor right/left\n";
	comment << "+/-y:           cursor down/up\n";
	comment << "+ Alt           step=0,01\n";
	comment << "+ Ctrl          step=0,10\n";
	comment << "+ Shift         step=2,00\n";
	comment << "+ Ctrl + Shift  step*=10\n";
	comment << "Space           Goto Z\n";
	comment << "Return          Set RefPos\n";
	comment << "Return + Ctrl   Set RefPos with workpiece Thickness\n";
	(*m_moveXYAxisCtl) << comment;

	comment = "";
	comment << "+/-z:           cursor up/down\n";
	comment << "+ Alt           step=0,01\n";
	comment << "+ Ctrl          step=0,10\n";
	comment << "+ Shift         step=2,00\n";
	comment << "+ Ctrl + Shift  step*=10\n";
	comment << "Space           Goto XY\n";
	comment << "Return          Set RefPos\n";
	comment << "Return + Ctrl   Set RefPos with workpiece Thickness\n";
	(*m_moveZAxisCtl) << comment;
	
	if ( CncConfig::getGlobalCncConfig()->getShowTestMenuFlag() == false )
		m_menuBar->Remove(m_menuBar->FindMenu("Test"));
		
	//initilaize debug state
	m_menuItemDebugSerial->Check(false);
	
	decorateSerialSpy();
	
	resetMinMaxPositions();
	initializeLruMenu();
	initializeCncControl();
	
	m_outboundNotebook->SetSelection(OutboundSelection::VAL::MOTION_MONITOR_PANAL);
	m_notebookConfig->SetSelection(OutboundCfgSelection::VAL::SUMMARY_PANEL);
	
	// curve lib resulotion
	CncConfig::gblCurveLibSelector = m_cbCurveLibResolution;
	CncConfig::setCurveLibResolution(CncSvgCurveLib::defaultIncrement);
}
///////////////////////////////////////////////////////////////////
bool MainFrame::initializeCncControl() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);

	//Determine output controls
	determineCncOutputControls();
	
	//Initialize the cnc configuartion
	CncConfig* cncConfig = CncConfig::getGlobalCncConfig();
	
	if ( cncConfig != NULL ) {
		wxString value;
		
		// initialize display unit
		m_unit->SetStringSelection(cncConfig->getDefaultDisplayUnitAsStr());
		updateUnit();
	
		// initialize com port
		cncConfig->getDefaultPort(value);
		m_portSelector->SetStringSelection(value);
		defaultPortName.assign(value);

		// initialize update interval
		unsigned int interval = 1;
		interval = wxAtoi(m_cbUpdateInterval->GetValue());
		cncConfig->setUpdateInterval(interval);
	
		// initialize speed control
		m_speedView->setMaxSpeedX(cncConfig->getMaxSpeedXY());
		m_speedView->setMaxSpeedY(cncConfig->getMaxSpeedXY());
		m_speedView->setMaxSpeedZ(cncConfig->getMaxSpeedZ());

		if ( cnc->isConnected() ) {
			if ( cncConfig->getDefaultSpeedModeXY(value) == "RAPID")	cnc->changeCurrentRpmSpeedXY(CncSpeedRapid);
			else														cnc->changeCurrentRpmSpeedXY(CncSpeedWork);
			
			if ( cncConfig->getDefaultSpeedModeZ(value)  == "RAPID")	cnc->changeCurrentRpmSpeedZ(CncSpeedRapid);
			else														cnc->changeCurrentRpmSpeedZ(CncSpeedWork);
		} else {
			cnc->changeCurrentRpmSpeedZ(CncSpeedRapid);
			cnc->changeCurrentRpmSpeedZ(CncSpeedRapid);
		}
		
		m_speedView->setCurrentSpeedXYZ(cnc->getRpmSpeedX(), cnc->getRpmSpeedY(), cnc->getRpmSpeedZ());
	}
	
	// initialize the postion controls
	cnc->setZeroPos();
	updateCncConfigTrace();
	
	// z slider
	m_zView->updateView(cnc->getControllerPos().getZ() * GBL_CONFIG->getDisplayFactZ(GBL_CONFIG->getDisplayUnit()));
	
	//initilaize debug state
	if ( m_menuItemDebugSerial->IsChecked() ) 	cnc->getSerial()->enableSpyOutput(true);
	else				    					cnc->getSerial()->enableSpyOutput(false); 
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool MainFrame::initializeLruMenu() {
///////////////////////////////////////////////////////////////////
	//load lru list from config file
	lruFileList.load(lruStore);
	
	CncConfig* cncConfig = CncConfig::getGlobalCncConfig();
	
	wxString value;
	cncConfig->getDefaultTplDir(value);
	fileView->setDefaultPath(value);
	fileView->selectDefaultPath();
	
	cncConfig->getDefaultTplFile(value);
	wxFileName fn;
	if ( value.length() > 0 ) 	fn.Assign(value);
	else 						fn.Assign(lruFileList.getFileName(0));
	
	if ( fn.Exists() ) {
		m_inputFileName->SetValue(fn.GetFullName());
		m_inputFileName->SetHint(fn.GetFullPath());
		
		openFile();
		prepareAndShowMonitorTemplatePreview();
		introduceCurrentFile();
	} else {
		cncConfig->getDefaultTplDir(value);
		fileView->openDirectory(value);
		
		selectMainBookSourcePanel();
		return false; 
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
void MainFrame::determineCncOutputControls() {
///////////////////////////////////////////////////////////////////
	//Determine output controls
	guiCtlSetup->mainFrame				= this;
	
	guiCtlSetup->motionMonitor			= motionMonitor;
	
	guiCtlSetup->passingTrace			= m_passingCount;
	guiCtlSetup->toolState 				= m_toolStateTrafficLight;
	
	guiCtlSetup->controllerConfig		= m_dvListCtrlControllerConfig;
	guiCtlSetup->controllerPinReport	= m_dvListCtrlControllerPins;
	guiCtlSetup->controllerErrorInfo	= m_dvListCtrlControllerErrorInfo;
	
	guiCtlSetup->motorState 			= m_miMotorEnableState;
	
	guiCtlSetup->xMinLimit 				= m_xMinLimit;
	guiCtlSetup->xMaxLimit 				= m_xMaxLimit;
	guiCtlSetup->yMinLimit 				= m_yMinLimit;
	guiCtlSetup->yMaxLimit 				= m_yMaxLimit;
	guiCtlSetup->zMinLimit 				= m_zMinLimit;
	guiCtlSetup->zMaxLimit 				= m_zMaxLimit;
	
	cnc->setGuiControls(guiCtlSetup);
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateUnit() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	wxASSERT(m_xAxis); wxASSERT(m_yAxis); wxASSERT(m_zAxis);

	CncConfig* cncConfig = CncConfig::getGlobalCncConfig();
	wxString unit = m_unit->GetValue();

	wxFloatingPointValidator<float> valX(3, NULL, wxNUM_VAL_DEFAULT );//, wxNUM_VAL_ZERO_AS_BLANK);
	wxFloatingPointValidator<float> valY(3, NULL, wxNUM_VAL_DEFAULT );//, wxNUM_VAL_ZERO_AS_BLANK);
	wxFloatingPointValidator<float> valZ(3, NULL, wxNUM_VAL_DEFAULT );//, wxNUM_VAL_ZERO_AS_BLANK);
	
	double xLimit = +cncConfig->getMaxDimensionX()/2;
	double yLimit = +cncConfig->getMaxDimensionY()/2;
	double zLimit = +cncConfig->getMaxDimensionZ()/2;
	
	m_xManuallySlider->SetMin(-xLimit);
	m_xManuallySlider->SetMax(+xLimit);
	m_yManuallySlider->SetMin(-yLimit);
	m_yManuallySlider->SetMax(+yLimit);
	m_zManuallySlider->SetMin(-zLimit);
	m_zManuallySlider->SetMax(+zLimit);
	m_xManuallySlider->SetValue(0);
	m_yManuallySlider->SetValue(0);
	m_zManuallySlider->SetValue(0);
	
	if ( unit == "mm" ) { 
		cncConfig->setDisplayUnit(CncMetric); 
		m_metricX->SetValue("0.000");
		m_metricY->SetValue("0.000");
		m_metricZ->SetValue("0.000");
	} else {
		xLimit *= cncConfig->getStepsX();
		yLimit *= cncConfig->getStepsY();
		zLimit *= cncConfig->getStepsZ();

		cncConfig->setDisplayUnit(CncSteps);
		m_metricX->SetValue("0");
		m_metricY->SetValue("0");
		m_metricZ->SetValue("0");
	}
	
	valX.SetRange(-xLimit, +xLimit);
	valY.SetRange(-yLimit, +yLimit);
	valZ.SetRange(-zLimit, +zLimit);
	m_metricX->SetValidator(valX);
	m_metricY->SetValidator(valY);
	m_metricZ->SetValidator(valZ);
	
	// manual control
	m_mmUnitX->SetLabel(unit);
	m_mmUnitY->SetLabel(unit);
	m_mmUnitZ->SetLabel(unit);

	// update controls
	cnc->forceDisplayPositions();
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
	info.SetName(_programTitel);
	info.SetVersion(_programVersion);
	info.SetLicence(_("GPL v2 or later"));
	info.AddDeveloper("Stefan Hoelzer");
	info.SetDescription(description);
	info.SetIcon(icon);
	info.SetCopyright(_copyRight);
	::wxAboutBox(info);
}
///////////////////////////////////////////////////////////////////
void MainFrame::showSVGEmuResult(bool show) {
///////////////////////////////////////////////////////////////////
	wxASSERT(outboundNbInfo);
	
	if ( show == false ) {
		
		if (m_outboundNotebook->FindPage(m_svgEmuResult) != wxNOT_FOUND )
			m_outboundNotebook->RemovePage(m_outboundNotebook->FindPage(m_svgEmuResult));

		if (m_outboundNotebook->FindPage(m_svgEmuSource) != wxNOT_FOUND )
			m_outboundNotebook->RemovePage(m_outboundNotebook->FindPage(m_svgEmuSource));

	} else {

		if (m_outboundNotebook->FindPage(m_svgEmuResult) == wxNOT_FOUND ) {
			m_outboundNotebook->InsertPage(OutboundSelection::VAL::SVG_OUTPUT_PANEL, m_svgEmuResult,  "", false);
			outboundNbInfo->decorate(OutboundSelection::VAL::SVG_OUTPUT_PANEL);
		}
		
		if (m_outboundNotebook->FindPage(m_svgEmuSource) == wxNOT_FOUND ) {
			m_outboundNotebook->InsertPage(OutboundSelection::VAL::SVG_SOURCE_PANEL, m_svgEmuSource, "", false);
			outboundNbInfo->decorate(OutboundSelection::VAL::SVG_SOURCE_PANEL);
		}
		
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectPort(wxCommandEvent& event) {
	if ( lastPortName != m_portSelector->GetStringSelection() ) {
		connectSerialPort();
	}
}
///////////////////////////////////////////////////////////////////
bool MainFrame::connectSerialPort() {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	
	wxBitmap bmpC = ImageLib16().Bitmap("BMP_CONNECTED");
	wxBitmap bmpD = ImageLib16().Bitmap("BMP_DISCONNECTED");
	m_connect->SetBitmap(bmpD);
	m_connect->Refresh();
	m_connect->Update();
	
	startAnimationControl();
	m_serialTimer->Stop();
	
	if ( m_clearSerialSpyOnConnect->IsChecked() )
		clearSerialSpy();
	
	bool ret = false;
	wxString sel(m_portSelector->GetStringSelection());	
	wxString cs;
	
	disableControls();
	hideSVGEmuResult();
	
	m_miRqtIdleMessages->Check(false);
	
	// disconnect and delete the current cnc control
	if ( cnc != NULL )
		delete cnc;
	
	if ( sel == _portEmulatorNULL ) {
		cnc = new CncControl(CncEMU_NULL);
		cs.assign("dev/null");
		
	} else if ( sel == _portEmulatorSVG ) {
		cnc = new CncControl(CncEMU_SVG);
		wxString val;
		cs.assign(CncFileNameService::getCncOutboundSvgFileName());
		showSVGEmuResult();
		enableMenuItem(m_miSaveEmuOutput, true);
		
	} else {
		cnc = new CncControl(CncPORT);
		cs.assign("\\\\.\\");
		cs.append(sel);
		m_miRqtIdleMessages->Check(true);
	}

	initializeCncControl();
	lastPortName.clear();
	
	if ( (ret = cnc->connect(cs)) == true )  {
		cnc->setup();
		cnc->getSerial()->isEmulator() ? setRefPostionState(true) : setRefPostionState(false);
		updateCncConfigTrace();
		lastPortName.assign(sel);
		m_connect->SetBitmap(bmpC);
		m_serialTimer->Start();
	}
	
	decoratePortSelector();
	m_connect->Refresh();
	m_connect->Update();
	stopAnimationControl();
	enableControls();

	return ret;
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
	
	// all buttons of the run control have to be enabled/disabeled here
	// every time
	
	isPause() ? m_rcDebugConfig->Enable(false) : m_rcRun->Enable(state);
	
	if ( isDebugMode == false ) {
		
		isPause() ? m_rcRun->Enable(true)           : m_rcRun->Enable(state);
		isPause() ? m_rcDebug->Enable(false)        : m_rcDebug->Enable(state);
		isPause() ? m_rcPause->Enable(false)        : m_rcPause->Enable(!state);
		isPause() ? m_rcStop->Enable(true)          : m_rcStop->Enable(!state);
		isPause() ? m_btnEmergenyStop->Enable(true) : m_btnEmergenyStop->Enable(!state);
		
		m_rcReset->Enable(isPause() == false && state);
		m_rcNextStep->Enable(false);
		m_rcNextBreakpoint->Enable(false);
		m_rcFinish->Enable(false);
		
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
	
	// enable all relevant controls
	enableGuiControls(state);
	
	//enable manually controls
	enableManuallyControls(!state);

	// enable menu bar
	for (unsigned int i=0; i<m_menuBar->GetMenuCount();i++)
		m_menuBar->EnableTop(i, state);
		
	// enable template editor
	m_stcFileContent->SetReadOnly(!state);
	if ( state == true )	m_editMode->SetLabel("Edit mode");
	else					m_editMode->SetLabel("Readonly");
	
	// run control
	enableRunControls(state);
	
	// test suite controls
	enableTestParameter(state);
}
///////////////////////////////////////////////////////////////////
void MainFrame::connect(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	connectSerialPort();
}
///////////////////////////////////////////////////////////////////
void MainFrame::clearLogger(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_logger->Clear();
}
///////////////////////////////////////////////////////////////////
void MainFrame::freezeLogger() {
///////////////////////////////////////////////////////////////////
	if ( m_menuItemFreezeLogger->IsChecked() == false )
		m_logger->Freeze();
}
///////////////////////////////////////////////////////////////////
void MainFrame::unfreezeLogger() {
///////////////////////////////////////////////////////////////////
	if ( m_logger->IsFrozen() ) {
		m_logger->Thaw();
		// Trick: This scrolls to the end of content
		std::cout << ' ';
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::setZero(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	setZero();
}
///////////////////////////////////////////////////////////////////
void MainFrame::setZero() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	bool ret = cnc->reset();
	if ( ret == false  ) {
		std::cerr << "MainFrame::setZero: Unable to reset the controller!"<< std::endl;
	}
	
	if ( ret == true )
		setRefPostionState(true);
	
	cnc->forceDisplayPositions();
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectUnit(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	updateUnit();
}
///////////////////////////////////////////////////////////////////
const char* MainFrame::getSvgEmuFileName(wxString& ret) {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc && cnc->getSerial() );
	ret = cnc->getSerial()->getPortName();
	return ret.c_str();
}
///////////////////////////////////////////////////////////////////
void MainFrame::svgEmuReload(wxCommandEvent& event) {
	refreshSvgEmuFile(false);
}
///////////////////////////////////////////////////////////////////
void MainFrame::svgEmuOpenFileAsSource(wxCommandEvent& event) {
	wxString tool, svgFile;
	CncConfig::getGlobalCncConfig()->getEditorTool(tool);
	getSvgEmuFileName(svgFile);
	openFileExtern(tool, svgFile);
}
///////////////////////////////////////////////////////////////////
void MainFrame::svgEmuOpenFileAsSvg(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString tool, svgFile;
		CncConfig::getGlobalCncConfig()->getSVGFileViewer(tool);
	getSvgEmuFileName(svgFile);
	openFileExtern(tool, svgFile);
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateInclWpt(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	CncConfig::getGlobalCncConfig()->setReferenceIncludesWpt(m_includingWpt->IsChecked());
	updateCncConfigTrace();
}
///////////////////////////////////////////////////////////////////
int MainFrame::showSetReferencePositionDlg(wxString msg) {
///////////////////////////////////////////////////////////////////
	wxMessageDialog dlg(this, msg, _T("Action required  . . . "), 
				        wxCANCEL|wxYES|wxNO|wxCENTRE|wxICON_INFORMATION);
	dlg.SetYesNoCancelLabels("Set with workpiece thickness ", "Set without workpiece thickness", "Do it later . . . ");
	
	int ret = dlg.ShowModal();
	switch ( ret ) {
		case  wxID_YES:  	m_includingWpt->SetValue(true);
							selectMainBookReferencePanel();
							setZero();
							break;
							
		case  wxID_NO:		m_includingWpt->SetValue(false);
							selectMainBookReferencePanel();
							setZero();
							break;
							
		default:			;//m_crossingThickness->SetFocus(); 
							//do nothing
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateCncConfigTrace() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	cnc->updateCncConfigTrace();
	m_infoToolDiameter->SetLabel(wxString::Format("%.3lf", CncConfig::getGlobalCncConfig()->getToolDiameter()));
	
	m_zView->updateView(cnc->getControllerPos().getZ() * GBL_CONFIG->getDisplayFactZ(GBL_CONFIG->getDisplayUnit()));
	
	collectSummary();
}
///////////////////////////////////////////////////////////////////
void MainFrame::changeWorkpieceThickness() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	double wpt = CncConfig::getGlobalCncConfig()->getWorkpieceThickness();
	
	if ( cnc::dblCompareNull(wpt) == true )	m_lableWorkpieceThickness->SetBitmap(ImageLib16().Bitmap("BMP_NO_WPT"));
	else 									m_lableWorkpieceThickness->SetBitmap(ImageLib16().Bitmap("BMP_WPT"));
		
	m_lableWorkpieceThickness->SetToolTip(wxString::Format("Workpiece thickness: %.3lf mm", wpt));
	m_lableWorkpieceThickness->Refresh();
	setRefPostionState(false);
	
	wxString msg("A workpiece thickness change requires a redefinition of the CNC reference position.\n\n");
	msg << "This have to be done before the next CNC run.\n\n";
	msg << "The set function below can be used to set it directly.";
	
	showSetReferencePositionDlg(msg);
	m_crossings->ChangeValue(wxString() << CncConfig::getGlobalCncConfig()->getDurationCount());
}
///////////////////////////////////////////////////////////////////
void MainFrame::changeCrossingThickness() {
///////////////////////////////////////////////////////////////////
	m_crossings->ChangeValue(wxString() << CncConfig::getGlobalCncConfig()->getDurationCount());
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
	m_menuItemDebugSerial->Check(false);
	m_menuItemFreezeLogger->Check(false);
	m_menuItemDisplayUserAgent->Check(false);
	m_menuItemToolControls->Check(false);
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
	m_menuItemDebugSerial->Check(false);
	m_menuItemFreezeLogger->Check(true);
	m_menuItemDisplayUserAgent->Check(true);
	m_menuItemToolControls->Check(true);
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
void MainFrame::defineDebugSerial(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	updateMonitoring();
	decorateSerialSpy();
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateMonitoring() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	if ( motionMonitor != NULL ) {
		motionMonitor->enable(m_menuItemUpdDraw->IsChecked());
		motionMonitor->display();
	}
	
	CncConfig::getGlobalCncConfig()->setOnlineUpdateCoordinates(m_menuItemUpdCoors->IsChecked());
	CncConfig::getGlobalCncConfig()->setAllowEventHandling(m_menuItemAllowEvents->IsChecked());
	CncConfig::getGlobalCncConfig()->setOnlineUpdateDrawPane(m_menuItemUpdDraw->IsChecked());
	CncConfig::getGlobalCncConfig()->setAllowEventHandling(m_menuItemDebugSerial->IsChecked());
	cnc->getSerial()->enableSpyOutput(m_menuItemDebugSerial->IsChecked());
	cnc->setUpdateToolControlsState(m_menuItemToolControls->IsChecked());
	
	if ( m_menuItemDisplayUserAgent->IsChecked() == false ) {
		m_dvListCtrlSvgUAInboundPathList->DeleteAllItems();
		m_dvListCtrlSvgUAUseDirective->DeleteAllItems();
		m_dvListCtrlSvgUADetailInfo->DeleteAllItems();
		
		m_dvListCtrlSvgUAInboundPathList->Update();
		m_dvListCtrlSvgUAUseDirective->Update();
		m_dvListCtrlSvgUADetailInfo->Update();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::refreshSvgEmuFile(bool blank) {
///////////////////////////////////////////////////////////////////
	wxString svgFile(getBlankHtmlPage());
	
	if ( cnc->getPortType() != CncEMU_SVG )
		return;
		
	if ( blank == false ) {
		getSvgEmuFileName(svgFile);
		
		wxFileName check(svgFile);
		if ( !check.Exists() ) {
			wxString ei(wxString::Format("MainFrame::refreshSvgEmuFile: File: %s does not exists!", svgFile));
			svgFile = getErrorHtmlPage(ei);
		}
	}
	
	refreshSvgEmuSourceFile(blank);
	m_svgView->LoadURL(svgFile);
	m_svgView->Update();
}

///////////////////////////////////////////////////////////////////
void MainFrame::refreshSvgEmuSourceFile(bool blank) {
///////////////////////////////////////////////////////////////////
	wxString svgFile;
	getSvgEmuFileName(svgFile);
	unsigned int cl = m_stcEmuSource->GetCurrentLine();
	
	if ( blank == true ) {
		m_stcEmuSource->ClearAll();
		m_stcEmuSource->LoadFile(getBlankHtmlPage());
		m_stcEmuSource->SaveFile(svgFile);
		evaluateSvgEmuModificationTimeStamp();
		return;
	}
	
	wxFileName emuFile(svgFile);
	if ( emuFile.Exists() == true ) {
		wxDateTime dt = emuFile.GetModificationTime();
		
		if ( dt != lastSvgEmuModification ) {
			m_stcEmuSource->LoadFile(getSvgEmuFileName(svgFile));
			m_stcEmuSource->GotoLine(cl);
		}
	}
}
///////////////////////////////////////////////////////////////////
TemplateFormat MainFrame::getCurrentTemplateFormat(const char* fileName) {
///////////////////////////////////////////////////////////////////
	unsigned int sel = m_mainViewSelector->GetSelection();
	if ( sel == (unsigned int)MainBookSelection::VAL::MANUEL_PANEL )
		return TplManual;
		
	if ( sel == (unsigned int)MainBookSelection::VAL::TEST_PANEL )
		return TplTest;
		
	wxFileName fn;
	if ( fileName == NULL )	fn.Assign(getCurrentTemplatePathFileName());
	else					fn.Assign(fileName);
	
	wxString ext(fn.GetExt());
	ext.MakeUpper();
	
	if      ( ext == "SVG" )	return TplSvg;
	else if ( ext == "GCODE") 	return TplGcode;

	return TplUnknown;
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
void MainFrame::showSvgExtPages(bool show) {
///////////////////////////////////////////////////////////////////
	if ( show == false ) {
		if (m_templateNotebook->FindPage(m_panelTplUserAgent) != wxNOT_FOUND )
			m_templateNotebook->RemovePage(m_templateNotebook->FindPage(m_panelTplUserAgent));
			
	} else {
		// correct insert order is very important
		if (m_templateNotebook->FindPage(m_panelTplUserAgent) == wxNOT_FOUND ) {
			m_templateNotebook->InsertPage(TemplateBookSelection::VAL::USER_AGENT_PANEL, m_panelTplUserAgent, "", false);
			templateNbInfo->decorate(TemplateBookSelection::VAL::USER_AGENT_PANEL);
		}
	}
}
///////////////////////////////////////////////////////////////////
bool MainFrame::openFile(int pageToSelect) {
///////////////////////////////////////////////////////////////////
	// First select the template page to get the rigth result 
	// from getCurrentTemplateFormat
	selectMainBookSourcePanel();

	templateFileLoading = true;
	bool ret = false;
	switch ( getCurrentTemplateFormat() ) {
		
		case TplSvg:	ret = openTextFile();
						if ( ret == true )
							showSvgExtPages();
						break;
		
		case TplGcode:	ret = openTextFile();
						hideSvgExtPages();
						break;
						
		default:		hideSvgExtPages();
						std::cerr << "MainFrame::openFile(): Unknown Type: " << getCurrentTemplateFormat() << std::endl;
						ret = false;
	}
	
	if ( ret == true ) {
		evaluateTemplateModificationTimeStamp();
		
		initTemplateEditStyle();
		m_stcFileContent->DiscardEdits();
		m_stcFileContent->EmptyUndoBuffer();
		
		if ( inboundFileParser != NULL )
			inboundFileParser->clearControls();
		
		cnc->clearDrawControl();
		cnc->getSerial()->clearSVG();
		
		introduceCurrentFile();
	}
	
	updateFileContentPosition();
	templateFileLoading = false;
	return ret;
}
///////////////////////////////////////////////////////////////////
void MainFrame::introduceCurrentFile() {
///////////////////////////////////////////////////////////////////
	lruFileList.addFile(getCurrentTemplatePathFileName());
	fileView->selectFileInList(getCurrentTemplatePathFileName());
	selectMainBookSourcePanel();
	
	// publish model type
	GLContextBase::ModelType mt = GLContextBase::ModelType::MT_RIGHT_HAND;
	if ( getCurrentTemplateFormat() == TplSvg )
		mt = GLContextBase::ModelType::MT_LEFT_HAND;
		
	motionMonitor->setModelType(mt);
}
///////////////////////////////////////////////////////////////////
bool MainFrame::openTextFile() {
///////////////////////////////////////////////////////////////////
	wxASSERT(m_inputFileName);
	
	wxFileInputStream input(getCurrentTemplatePathFileName());
	wxTextInputStream text(input, wxT("\x09"), wxConvUTF8 );
	
	if ( input.IsOk() ) {
		m_stcFileContent->ClearAll();

		while( input.IsOk() && !input.Eof() ) {
			wxString line=text.ReadLine();
			m_stcFileContent->AppendText(line);
			m_stcFileContent->AppendText(_T("\r\n"));
		}
		
		return true;
	}
	
	std::cerr << "Error while open file: " << getCurrentTemplatePathFileName().c_str() << std::endl;
	return false;
}
///////////////////////////////////////////////////////////////////
void MainFrame::prepareNewTemplateFile() {
///////////////////////////////////////////////////////////////////
	m_stcFileContent->ClearAll();
	
	wxFileName fn(getCurrentTemplateFileName());
	
	if ( fn.GetExt().MakeUpper() == "SVG") {
		m_stcFileContent->AppendText("<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>\r\n");
		m_stcFileContent->AppendText("<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\"\r\n");
		m_stcFileContent->AppendText("\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\r\n");
		m_stcFileContent->AppendText("<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"100mm\" height=\"100mm\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\r\n");
		m_stcFileContent->AppendText("<title>CncControl SerialEmulatorSVG output</title>\r\n");
		m_stcFileContent->AppendText("<desc>....</desc>\r\n");
		
		m_stcFileContent->AppendText(SvgNodeTemplates::getSamplesAsString());
	
		m_stcFileContent->AppendText("\r\n");
		m_stcFileContent->AppendText("</svg>\r\n");
	} else {
		//Curently do nothing
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::newTemplate(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( checkIfTemplateIsModified() == false )
		return;
	
	wxString templateName("..\\Templates\\");
    wxFileDialog newFileDialog(this, 
								_("New Template File"), 
								templateName,
								"",
                                "SVG Files (*.svg)|*.svg|GCode Files (*.gcode)|*.gcode", 
								wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

	if ( newFileDialog.ShowModal() == wxID_CANCEL ) 
        return; 

	wxString ov = getCurrentTemplateFileName();
	wxString oh = getCurrentTemplatePathFileName();

	m_inputFileName->SetValue(newFileDialog.GetFilename());
	m_inputFileName->SetHint(newFileDialog.GetPath());
	
	prepareNewTemplateFile();

	if ( !saveFile() ) {
		m_inputFileName->SetValue(ov);
		m_inputFileName->SetHint(oh);
	} else {
		if ( !openFile() ) {
			m_inputFileName->SetValue(ov);
			m_inputFileName->SetHint(oh);
		} else {
			prepareAndShowMonitorTemplatePreview(true);
		}
	}
	
	introduceCurrentFile();
}
///////////////////////////////////////////////////////////////////
void MainFrame::openTemplate(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(m_inputFileName);
	wxString templateName("..\\Templates\\");
    wxFileDialog openFileDialog(this, 
								_("Open Template File"), 
								templateName,
								"",
                                "SVG Files (*.svg)|*.svg|GCode Files (*.gcode)|*.gcode", 
								wxFD_OPEN|wxFD_FILE_MUST_EXIST);

	if ( openFileDialog.ShowModal() == wxID_CANCEL ) 
        return; 

	wxString ov = getCurrentTemplateFileName();
	wxString oh = getCurrentTemplatePathFileName();

	m_inputFileName->SetValue(openFileDialog.GetFilename());
	m_inputFileName->SetHint(openFileDialog.GetPath());
	
	if ( !openFile() ) {
		m_inputFileName->SetValue(ov);
		m_inputFileName->SetHint(oh);
	} else {
		prepareAndShowMonitorTemplatePreview(true);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::reloadTemplate(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( !openFile() ) {
		std::cerr << "Error while reloding template: " << getCurrentTemplateFileName().c_str() << std::endl;
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::reloadTemplateFromButton(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( !openFile(TemplateBookSelection::VAL::SOURCE_PANEL) ) {
		std::cerr << "Error while reloding template: " << getCurrentTemplateFileName().c_str() << std::endl;
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::openTemplateSourceExtern(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	saveFile();
	
	wxString tool;
	CncConfig::getGlobalCncConfig()->getEditorTool(tool);
	openFileExtern(tool, getCurrentTemplatePathFileName());
}
///////////////////////////////////////////////////////////////////
void MainFrame::openTemplateSvgExtern(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString tool;
	
	switch ( getCurrentTemplateFormat() ) {
		case TplSvg:
					saveFile();
					CncConfig::getGlobalCncConfig()->getSVGFileViewer(tool);
					openFileExtern(tool, getCurrentTemplatePathFileName());
					break;
		case TplGcode:
					saveFile();
					CncConfig::getGlobalCncConfig()->getGCodeFileViewer(tool);
					openFileExtern(tool, getCurrentTemplatePathFileName());
					break;
		default:
					std::clog << "No external editor availiable for current file: " << getCurrentTemplatePathFileName().c_str() << std::endl;
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::evaluateTemplateModificationTimeStamp() {
///////////////////////////////////////////////////////////////////
	wxString fn(getCurrentTemplatePathFileName());
	wxFileName tplFile(fn);
	
	if ( tplFile.Exists() == false )
		return;
		
	lastTemplateModification = tplFile.GetModificationTime();
}
///////////////////////////////////////////////////////////////////
void MainFrame::evaluateSvgEmuModificationTimeStamp() {
///////////////////////////////////////////////////////////////////
	wxString svgFile;
	getSvgEmuFileName(svgFile);
	wxFileName emuFile(svgFile);
	
	if ( emuFile.Exists() == false )
		return;
		
	lastSvgEmuModification = emuFile.GetModificationTime();
}
///////////////////////////////////////////////////////////////////
void MainFrame::activateMainWindow(wxActivateEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString fn(getCurrentTemplatePathFileName());
	wxFileName tplFile(fn);
	
	if ( tplFile.Exists() == true ) {
		wxDateTime dt = tplFile.GetModificationTime();
		
		if ( dt != lastTemplateModification ) {
			cnc::trc << "An externally template change is detected . . . template: " << fn.c_str() << " will be reloaded";
			if ( !openFile() ) {
					std::cerr << "MainFrame::activateMainWindow: Error while open file: " << fn.c_str() << std::endl;
			}
			prepareAndShowMonitorTemplatePreview(true);
		}
	}
	
	wxString svgFile;
	getSvgEmuFileName(svgFile);
	wxFileName emuFile(svgFile);
	
	if ( emuFile.Exists() == true ) {
		wxDateTime dt = emuFile.GetModificationTime();
		
		if ( dt != lastSvgEmuModification ) {
			refreshSvgEmuFile();
		}
	}
	
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
bool MainFrame::saveFile() {
///////////////////////////////////////////////////////////////////
	// First select the template page to get the rigth result 
	// by getCurrentTemplateFormat
	selectMainBookSourcePanel();
	
	bool ret = false;
	switch ( getCurrentTemplateFormat() ) {
		case TplSvg:
		case TplGcode:		ret = saveTextFile();
							break;
							
		default:			std::cerr << "MainFrame::saveFile(): Unknown Type: " << getCurrentTemplateFormat() << std::endl;
	}
	
	if( ret == true ) {
		
		// update tab label
		wxString name(m_templateNotebook->GetPageText(TemplateBookSelection::VAL::SOURCE_PANEL));
		if ( name.StartsWith("*") == true ) {
			name = name.SubString(1, name.length() -1 );
			m_templateNotebook->SetPageText(TemplateBookSelection::VAL::SOURCE_PANEL, name);
		}
		
		m_stcFileContent->DiscardEdits();
		m_stcFileContent->EmptyUndoBuffer();
		evaluateTemplateModificationTimeStamp();
		prepareAndShowMonitorTemplatePreview(true);
	}
		
	return ret;
}
///////////////////////////////////////////////////////////////////
bool MainFrame::saveTextFile() {
///////////////////////////////////////////////////////////////////
	wxASSERT(m_inputFileName);
	wxASSERT(m_stcFileContent);
	
	wxTextFile file(getCurrentTemplatePathFileName());
	if ( !file.Exists() )
		file.Create();
		
	if ( file.Open() ) {
		file.Clear();
		
		for (long i=0; i<m_stcFileContent->GetNumberOfLines(); i++) {
			wxString line = m_stcFileContent->GetLineText(i);
			file.AddLine(line);
		}
		
		file.Write();
		file.Close();
		
		return true;
	} else {
		std::cerr << "Cant save file: " << getCurrentTemplatePathFileName().c_str() << std::endl;
	}
	
	return false;
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
	wxASSERT(m_inputFileName);
	
	wxFileDialog saveFileDialog(this, 
	                            _("Save Template File"), 
								getCurrentTemplatePathFileName(), 
								"",
								"SVG Files (*.svg)|*.svg|GCode Files (*.gcode)|*.gcode",  
								wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
								
	if (saveFileDialog.ShowModal() == wxID_CANCEL) 
		return;

	wxString ov = getCurrentTemplateFileName();
	wxString oh = getCurrentTemplatePathFileName();
	
	m_inputFileName->SetValue(saveFileDialog.GetFilename());
	m_inputFileName->SetHint(saveFileDialog.GetPath());
	
	if ( !saveFile() ) {
		m_inputFileName->SetValue(ov);
		m_inputFileName->SetHint(oh);
	} else {
		m_stcFileContent->DiscardEdits();
		introduceCurrentFile();
	}
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processVirtualTemplate() {
///////////////////////////////////////////////////////////////////
	wxASSERT(m_inputFileName);

	bool ret;
	refreshSvgEmuFile(true);
	cnc->getSerial()->clearSVG();
	
	SvgOutputParameters sop;
	cnc->getSerial()->setSVGOutputParameters(evaluteSvgOutputParameters(sop));

	FileParser::UserAgentOutputControls oc;
	oc.detailInfo 		= m_dvListCtrlSvgUADetailInfo;
	oc.inboundPathList 	= m_dvListCtrlSvgUAInboundPathList;
	oc.useDirectiveList = m_dvListCtrlSvgUAUseDirective;
	
	if ( m_menuItemDisplayUserAgent->IsChecked() == true )
		inboundFileParser->setUserAgentControls(oc);
		
	inboundFileParser->setInboundSourceControl(m_stcFileContent);
	if ( isDebugMode == true ) 	ret = inboundFileParser->processDebug();
	else 						ret = inboundFileParser->processRelease();
	
	refreshSvgEmuFile();
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processSVGTemplate() {
///////////////////////////////////////////////////////////////////
	if ( inboundFileParser != NULL )
		delete inboundFileParser;
		
	inboundFileParser = new SVGFileParser(getCurrentTemplatePathFileName().c_str(), cnc);
	return processVirtualTemplate();
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processGCodeTemplate() {
///////////////////////////////////////////////////////////////////
	if ( inboundFileParser != NULL )
		delete inboundFileParser;
		
	inboundFileParser = new GCodeFileParser(getCurrentTemplatePathFileName().c_str(), new GCodePathHandlerCnc(cnc));
	return processVirtualTemplate();
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processManualTemplate() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	// SVG Serial Emulator Support
	cnc->getSerial()->beginSVG(mm, 
							   CncConfig::getGlobalCncConfig()->getMaxDimensionX(), 
							   CncConfig::getGlobalCncConfig()->getMaxDimensionY());
	cnc->getSerial()->beginPath(cnc->getCurPos().getX() * CncConfig::getGlobalCncConfig()->getDisplayFactX(), 
								cnc->getCurPos().getY() * CncConfig::getGlobalCncConfig()->getDisplayFactY());
		
	if ( m_checkManuallyXY->GetValue() == true ) {
		unsigned int sel = m_mmRadioCoordinates->GetSelection();

		wxString xs = m_metricX->GetValue();
		double xd; 
		if ( xs != "" )	xs.ToCDouble(&xd);
		else			xd = 0.0;
		
		wxString ys = m_metricY->GetValue();
		double yd; 
		if ( ys != "" ) ys.ToCDouble(&yd);
		else			yd = 0.0;
		
		cnc->resetDurationCounter();

		// transform to mm
		if ( m_unit->GetValue() == "steps" ) {
			xd *= CncConfig::getGlobalCncConfig()->getDisplayFactX();
			yd *= CncConfig::getGlobalCncConfig()->getDisplayFactY();
		}

		double moveX = xd, moveY = yd;
		// transfer to absolute coordinates
		if ( sel == 0 ) {
			moveX = xd - cnc->getCurPos().getX() * CncConfig::getGlobalCncConfig()->getDisplayFactX();
			moveY = yd - cnc->getCurPos().getY() * CncConfig::getGlobalCncConfig()->getDisplayFactY();
		}

		//cnc->initNextDuration(); will be done by manualSimpleMoveMetric
		cnc->manualSimpleMoveMetric(moveX, moveY, 0.0);
		
		cnc->resetDurationCounter();
	} else {
		wxString zs = m_metricZ->GetValue();
		double zd; 
		if ( zs != "" )	zs.ToCDouble(&zd);
		else			zd = 0.0;
		
		// transform to mm
		if ( m_unit->GetValue() == "steps" ) 
			zd *= CncConfig::getGlobalCncConfig()->getDisplayFactX();
			
		//cnc->initNextDuration(); will be done by manualSimpleMoveMetric
		cnc->manualSimpleMoveMetric(0.0, 0.0, zd);
		cnc->resetDurationCounter();
	}
	
	// SVG Serial Emulator Support
	cnc->getSerial()->closePath();
	cnc->getSerial()->closeSVG();
	
	refreshSvgEmuFile();

	return true;
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processTestTemplate() {
///////////////////////////////////////////////////////////////////
	wxWindow* page = m_treebookTest->GetCurrentPage();
	if ( page != NULL ) {
		if 		( page == m_testIntervalPage )	return processTestInterval();
		else if ( page == m_testDimensions )	return processTestDimensions();
		else if ( page == m_ctrlTestSuite )		return processControllerTestSuite();
		//...
		
		wxString msg;
		msg << "This Test (\"";
		msg << m_treebookTest->GetPageText(m_treebookTest->GetSelection()); 
		msg << "\") isn't assigned to the run mode. May be there is an action control on the test page itself.\n\n";
		msg << "Nothing will be done.\n" ;
		
		wxMessageDialog dlg(this, msg, _T("MainFrame::processTestTemplate(): Not registered test . . . "), 
							wxOK||wxCENTRE|wxICON_QUESTION);

		dlg.ShowModal();
		return true;
	}
	
	std::cerr << "MainFrame::processTestTemplate(): Invalid page selection!" << std::endl;
	return false;
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processControllerTestSuite() {
///////////////////////////////////////////////////////////////////
	long id = atol(m_ctrlTestSelectedId->GetValue());
	std::clog << "Run Test: " << CncControllerTestSuite::getTestCaseName(id) << endl;
	
	if ( cnc->isConnected() == false ) {
		std::cerr << "Not connetced, nothing will be processed." << std::endl;
		std::clog << "Test finished (" << CncControllerTestSuite::getTestCaseName(id) << ")" << endl;
		return false;
	}

	if ( cnc->getSerial()->isEmulator() == true ) {
		std::cerr << "No controller connected, nothing will be processed." << std::endl;
		std::clog << "Test finished (" << CncControllerTestSuite::getTestCaseName(id) << ")" << endl;
		return false;
	}
	
	cnc->processSetter(PID_TEST_VALUE1, atol(m_ctrlTestParam1->GetValue()));
	cnc->processSetter(PID_TEST_VALUE2, atol(m_ctrlTestParam2->GetValue()));
	cnc->processSetter(PID_TEST_VALUE3, atol(m_ctrlTestParam3->GetValue()));
	cnc->processSetter(PID_TEST_VALUE4, atol(m_ctrlTestParam4->GetValue()));
	cnc->processSetter(PID_TEST_VALUE5, atol(m_ctrlTestParam5->GetValue()));
	updateSetterList();
	
	// run test
	bool ret = false;

	ret =  cnc->getSerial()->processTest(id);
	
	std::clog << "Test finished (" << CncControllerTestSuite::getTestCaseName(id) << ")" << endl;
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
	
	// SVG Serial Emulator Support
	cnc->getSerial()->beginSVG(mm, 
							   CncConfig::getGlobalCncConfig()->getMaxDimensionX(), 
							   CncConfig::getGlobalCncConfig()->getMaxDimensionY());
	cnc->getSerial()->beginPath(cnc->getCurPos().getX() * CncConfig::getGlobalCncConfig()->getDisplayFactX(), 
								cnc->getCurPos().getY() * CncConfig::getGlobalCncConfig()->getDisplayFactY());
								
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
	
	firstX == "First +" ? xd = xd : xd *= -1;
	firstY == "First +" ? yd = yd : yd *= -1;
	firstZ == "First +" ? zd = zd : zd*= -1;
	
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
	
	// SVG Serial Emulator Support
	cnc->getSerial()->closePath();
	cnc->getSerial()->closeSVG();
	
	refreshSvgEmuFile();
	
	return true;

}
///////////////////////////////////////////////////////////////////
bool MainFrame::processTestDimensions() {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	
	if ( cnc->isConnected() == false ) {
		std::cerr << "Not connetced, nothing will be processed." << std::endl;
		return false;
	}
	
	cnc->activatePositionCheck(false);
	
	double result = -DBL_MAX;
	bool ret = true;
	
	// meassure X Axis
	if ( m_testDimModeX->GetValue() == true  && ret == true ) {
		m_testDimResultX->SetValue(wxString::Format("%4.3f", 0.0));
		ret = cnc->meassureXDimension(m_testDimMinX, m_testDimMaxX, result);
		if ( ret ) {
			m_testDimMinX->SetValue(false);
			m_testDimMaxX->SetValue(false);
			m_testDimResultStateX->SetValue(true);
			m_testDimResultX->SetValue(wxString::Format("%4.3f", result));
			
			ret = cnc->moveXToMid();
		}
	}
	// meassure z Axis
	if ( m_testDimModeY->GetValue() == true && ret == true ) {
		m_testDimResultY->SetValue(wxString::Format("%4.3f", 0.0));
		ret = cnc->meassureYDimension(m_testDimMinY, m_testDimMaxY, result);
		if ( ret ) {
			m_testDimMinY->SetValue(false);
			m_testDimMaxY->SetValue(false);
			m_testDimResultStateY->SetValue(true);
			m_testDimResultY->SetValue(wxString::Format("%4.3f", result));
			
			ret = cnc->moveYToMid();
		}
	}
	// meassure z Axis
	if ( m_testDimModeZ->GetValue() == true  && ret == true ) {
		m_testDimResultZ->SetValue(wxString::Format("%4.3f", 0.0));
		ret = cnc->meassureZDimension(m_testDimMinZ, m_testDimMaxZ, result);
		if ( ret ) {
			m_testDimMinZ->SetValue(false);
			m_testDimMaxZ->SetValue(false);
			m_testDimResultStateZ->SetValue(true);
			m_testDimResultZ->SetValue(wxString::Format("%4.3f", result));
			
			ret = cnc->moveZToMid();
		}
	}
	
	if ( ret == false ) {
		std::cerr << "MainFrame::processTestDimensions(): Invalid Test!" << std::endl;
	}
	
	cnc->activatePositionCheck(true);
	return ret;
}
///////////////////////////////////////////////////////////////////
void MainFrame::testDimTakeOverAll(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	testDimTakeOverX(event);
	testDimTakeOverY(event);
	testDimTakeOverZ(event);
}
///////////////////////////////////////////////////////////////////
void MainFrame::testDimTakeOverX(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	double v;
	wxString val = m_testDimResultX->GetValue();
	val.ToDouble(&v);
	
	if ( v > 0.0 )
		CncConfig::getGlobalCncConfig()->setMaxDimensionX(v);
}
///////////////////////////////////////////////////////////////////
void MainFrame::testDimTakeOverY(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	double v;
	wxString val = m_testDimResultY->GetValue();
	val.ToDouble(&v);
	
	if ( v > 0.0 )
		CncConfig::getGlobalCncConfig()->setMaxDimensionY(v);
}
///////////////////////////////////////////////////////////////////
void MainFrame::testDimTakeOverZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	double v;
	wxString val = m_testDimResultZ->GetValue();
	val.ToDouble(&v);

	if ( v > 0.0 )
		CncConfig::getGlobalCncConfig()->setMaxDimensionZ(v);
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
	
	if ( mode != 'A' ) {
		int val = m_testCountX->GetValue();
		
		m_testCountY->SetValue(val);
		m_testCountZ->SetValue(val);
	}
}
///////////////////////////////////////////////////////////////////
bool MainFrame::checkIfRunCanBeProcessed() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	// select summary page
	m_outboundNotebook->SetSelection(OutboundSelection::VAL::SUMMARY_PANEL);
	m_notebookConfig->SetSelection(OutboundCfgSelection::VAL::SUMMARY_PANEL);
	
	if ( isZeroReferenceValid == false ) {
		
		TemplateFormat tf = getCurrentTemplateFormat();
		
		if ( tf != TplManual && tf != TplTest ) {
			wxString msg("The current reference position isn't valid due to a setup change or it isn't not initialized yet.\n\n");
			msg << "How to fix it:\n";
			msg << "Please use the \"Set Current Position to Zero\" functionality on the \"References\" tab.";
			
			showAuiPane("SourceView");
			selectMainBookReferencePanel();
			
			int ret = showSetReferencePositionDlg(msg);
			// means reference postion isn't set
			if ( ret != wxID_YES && ret != wxID_NO)
				return false;
		}
	}

	if ( evaluatePositions == true && cnc->validatePositions() == false ) {
		
		wxString msg("Validate positions failed\n");
		msg << "\nPC pos        : ";
		msg << cnc->getCurPos().getX(); msg << ",";
		msg << cnc->getCurPos().getY(); msg << ",";
		msg << cnc->getCurPos().getZ();
		msg << "\nController pos: ";
		msg << cnc->getControllerPos().getX();  msg << ",";
		msg << cnc->getControllerPos().getY();  msg << ",";
		msg << cnc->getControllerPos().getZ();
		msg << "\n\nThe run command will be aborted!";

		wxMessageDialog dlg(this, msg, _T("CNC Position check . . . "), 
		                    wxOK|wxCENTRE|wxICON_ERROR);
 	
		dlg.ShowModal();
		setRefPostionState(false);
		return false;
	}
	
	cnc->evaluateLimitState();
	if ( cnc->isALimitSwitchActive() ) {
		// always return false to reconfigure zero in this sitiuation
		return false;
	}
 
	return showConfigSummaryAndConfirmRun();
}
///////////////////////////////////////////////////////////////////
bool MainFrame::showConfigSummaryAndConfirmRun() {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	
	wxString mode;
	CncConfig::getGlobalCncConfig()->getRunConfirmationMode(mode);
	if ( mode.IsEmpty() == true )
		mode.assign("Always");
		
	switch ( (char)mode[0] ) {
		// Always
		case 'A':	break;
		
		// Serial Port only
		case 'S': 	if ( cnc->getSerial()->isEmulator() )
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
void MainFrame::collectSummary() {
///////////////////////////////////////////////////////////////////
	if ( cnc == NULL )
		return;
		
	CncConfig* cc = CncConfig::getGlobalCncConfig();
	
	DcmItemList rows;
	DataControlModel::addNumParameterValueUnitRow(rows, "Tool Diameter",					wxString::Format(" %.3f", 	cc->getToolDiameter()), 			" mm"); 
	DataControlModel::addNumParameterValueUnitRow(rows, "Workpiece thickness", 				wxString::Format(" %4.3f", 	cc->getWorkpieceThickness()), 		" mm"); 
	DataControlModel::addNumParameterValueUnitRow(rows, "Max thickness per crossing", 		wxString::Format(" %4.3f", 	cc->getMaxDurationThickness()), 	" mm"); 
	DataControlModel::addNumParameterValueUnitRow(rows, "Work speed XY", 					wxString::Format(" %d", 	cc->getWorkSpeedXY()), 				" rpm"); 
	DataControlModel::addNumParameterValueUnitRow(rows, "Work speed Z", 					wxString::Format(" %d", 	cc->getWorkSpeedZ()), 				" rpm");
	// ...

	m_dvListCtrlConfigSummary->DeleteAllItems();
	for (wxVector<wxVector<wxVariant>>::iterator it = rows.begin(); it != rows.end(); ++it) {
		m_dvListCtrlConfigSummary->AppendItem(*it);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateSetterList() {
///////////////////////////////////////////////////////////////////
	if ( updateManagerThread == NULL )
		return;
		
	startAnimationControl();
	if ( m_setterList->IsFrozen() == false )
		m_setterList->Freeze();
		
	updateManagerThread->fillSetterList(m_setterList);
	
	if ( m_setterList->IsFrozen() == true )
		m_setterList->Thaw();
	stopAnimationControl();
}
///////////////////////////////////////////////////////////////////
void MainFrame::nootebookConfigChanged(wxListbookEvent& event) {
///////////////////////////////////////////////////////////////////
	// check if currentla a run is active and return if so to avoid 
	// a controller request during this mode
	bool runActive = !m_rcRun->IsEnabled();

	m_dvListCtrlControllerConfig->DeleteAllItems();
	m_dvListCtrlControllerPins->DeleteAllItems();
	m_dvListCtrlControllerErrorInfo->DeleteAllItems();
	
	unsigned int sel = event.GetSelection();
	if ( (wxWindow*)event.GetEventObject() == m_notebookConfig ) {
		
		wxCommandEvent dummyEvent;
		
		switch ( sel ) {
			case OutboundCfgSelection::VAL::SUMMARY_PANEL:
					collectSummary();
					break;
					
			case OutboundCfgSelection::VAL::CNC_SETTER_PANEL:
					updateSetterList();
					break;
					
			case OutboundCfgSelection::VAL::CNC_PIN_PANEL:
					if ( runActive == true ) {
						cnc::trc.logWarning("During an active run no controller requests are possible! Ty it later again.");
						return;
					}
					requestControllerPinsFromButton(dummyEvent);
					break;
					
			case OutboundCfgSelection::VAL::CNC_CONFIG_PANEL:
					if ( runActive == true ) {
						cnc::trc.logWarning("During an active run no controller requests are possible! Ty it later again.");
						return;
					}
					requestControllerConfigFromButton(dummyEvent);
					break;
					
			case OutboundCfgSelection::VAL::CNC_ERROR_PANEL:
					if ( runActive == true ) {
						cnc::trc.logWarning("During an active run no controller requests are possible! Ty it later again.");
						return;
					}
					requestControllerErrorInfoFromButton(dummyEvent);
					break;
		}
	}
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processTemplate() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	if ( cnc->isInterrupted() ) {
		std::cerr << "Cnc controller state is interrupted. Before it can run next time a reset is required!" << std::endl;
		return false;
	}
	
	if( cnc->isConnected() == false) {
		if ( !connectSerialPort() )
			return false;
	}
	
	showAuiPane("Outbound");
	selectMonitorBookCncPanel();
		
	if ( checkIfRunCanBeProcessed() == false )
		return false;
		 
	startAnimationControl();
	
	clearPositionSpy();
	
	typedef UpdateManagerThread::Event Event;
	static Event evt;
	
	// select draw pane
	m_outboundNotebook->SetSelection(OutboundSelection::VAL::MOTION_MONITOR_PANAL);
		
	// select template Page
	if ( m_mainViewSelector->GetSelection() != MainBookSelection::VAL::MANUEL_PANEL && 
	     m_mainViewSelector->GetSelection() != MainBookSelection::VAL::TEST_PANEL && 
	     m_mainViewSelector->GetSelection() != MainBookSelection::VAL::SOURCE_PANEL &&
		 m_mainViewSelector->GetSelection() != MainBookSelection::VAL::SETUP_PANEL) {
		selectMainBookSourcePanel();
	}
	
	processStartTime = wxDateTime::UNow();
		
	motionMonitor->pushProcessMode();

	updateStepDelay();
	disableControls();
	resetMinMaxPositions();
	updateCncConfigTrace();
	CncConfig::getGlobalCncConfig()->setAllowEventHandling(true);
	cnc->processSetter(PID_SEPARATOR, SEPARARTOR_RUN);
	cnc->processCommand("r", std::cout);
	cnc->getSerial()->resetPostionCounter();
	cnc->getSerial()->resetStepCounter();
	cnc->logProcessingStart();
	cnc->enableStepperMotors(true);
	freezeLogger();

	bool ret = false;
	switch ( getCurrentTemplateFormat() ) {
		case TplSvg:
			if ( checkIfTemplateIsModified() == false )
				break;
			cnc->clearDrawControl();
			//umPostEvent(evt.ZViewResetEvent());
			ret = processSVGTemplate();
			break;
		case TplGcode:
			if ( checkIfTemplateIsModified() == false )
				break;
			cnc->clearDrawControl();
			//umPostEvent(evt.ZViewResetEvent());
			ret = processGCodeTemplate();
			break;
		case TplManual: 
			ret = processManualTemplate();
			break;
		case TplTest:
			cnc->clearDrawControl();
			//umPostEvent(evt.ZViewResetEvent());
			ret = processTestTemplate();
			break;
		default:
			; // do nothing
	}
	
	// Check positions
	setMinMaxPositions();
	if ( cnc->validatePositions() == false ) {
		if ( cnc->isInterrupted() == false ) {
			std::cerr << "Validate positions failed" << std::endl;
			std::cerr << "PC pos        : " << cnc->getCurPos() << std::endl;
			std::cerr << "Controller pos: " << cnc->getControllerPos() << std::endl;
			setRefPostionState(false);
		}
	}
	
	// Check error count
	int32_t cnt = -1;
	if ( (cnt = cnc->getControllerErrorCount() ) != 0 ) {
		if ( cnc->isInterrupted() == false ) {
			wxString msg("Controller Error Count: ");
			msg << cnt;
			displayNotification('E', "Controller Error Check", msg, 5);
		}
	}
	
	cnc->enableStepperMotors(false);
	cnc->logProcessingEnd();
	
	motionMonitor->popProcessMode();
	
	if ( ret )
		cnc->updateDrawControl();
		
	processEndTime = wxDateTime::UNow();
	logTimeConsumed();
		
	unfreezeLogger();
	enableControls();
	stopAnimationControl();
	
	clog << "Position Counter: " << cnc->getSerial()->getPostionCounter() << endl;
	clog << "Step Counter:     " << cnc->getSerial()->getStepCounter() << endl;
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void MainFrame::logTimeConsumed() {
///////////////////////////////////////////////////////////////////
	processLastDuartion = (processEndTime - processStartTime).GetMilliseconds().ToLong();
	
	int n = (int) ( processLastDuartion % 1000);
	int s = (int) ( processLastDuartion / 1000) % 60 ;
	int m = (int) ((processLastDuartion / (1000 * 60)) % 60);
	int h = (int) ((processLastDuartion / (1000 * 60 * 60)) % 24);
	
	m_cmdDuration->ChangeValue(wxString::Format("%02d:%02d:%02d.%03d", h, m, s, n));
}
///////////////////////////////////////////////////////////////////
void MainFrame::resetMinMaxPositions() {
///////////////////////////////////////////////////////////////////	
	m_minPosX->SetLabel(wxString::Format(wxT("%4.4f"), 0.0));
	m_minPosY->SetLabel(wxString::Format(wxT("%4.4f"), 0.0));
	m_minPosZ->SetLabel(wxString::Format(wxT("%4.4f"), 0.0));

	m_maxPosX->SetLabel(wxString::Format(wxT("%4.4f"), 0.0));
	m_maxPosY->SetLabel(wxString::Format(wxT("%4.4f"), 0.0));
	m_maxPosZ->SetLabel(wxString::Format(wxT("%4.4f"), 0.0));
	
	cnc->resetWatermarks();
}
///////////////////////////////////////////////////////////////////
void MainFrame::setMinMaxPositions() {
///////////////////////////////////////////////////////////////////
	CncDoublePosition min = cnc->getMinPositionsMetric();
	CncDoublePosition max = cnc->getMaxPositionsMetric();
	
	SVGUnit unit = mm;
	//todo display unit in tab page

	m_minPosX->SetLabel(wxString::Format(wxT("%4.4f"), min.getX() * SvgUnitCalculator::getFactorMM2Unit(unit)));
	m_minPosY->SetLabel(wxString::Format(wxT("%4.4f"), min.getY() * SvgUnitCalculator::getFactorMM2Unit(unit)));
	m_minPosZ->SetLabel(wxString::Format(wxT("%4.4f"), min.getZ() * SvgUnitCalculator::getFactorMM2Unit(unit)));

	m_maxPosX->SetLabel(wxString::Format(wxT("%4.4f"), max.getX() * SvgUnitCalculator::getFactorMM2Unit(unit)));
	m_maxPosY->SetLabel(wxString::Format(wxT("%4.4f"), max.getY() * SvgUnitCalculator::getFactorMM2Unit(unit)));
	m_maxPosZ->SetLabel(wxString::Format(wxT("%4.4f"), max.getZ() * SvgUnitCalculator::getFactorMM2Unit(unit)));
}
///////////////////////////////////////////////////////////////////
bool MainFrame::checkIfTemplateIsModified() {
///////////////////////////////////////////////////////////////////
	wxString msg(wxString::Format("The current template file\n\n '%s'\n\nwas modified. Save it?", getCurrentTemplatePathFileName()));
	
	if ( m_stcFileContent->IsModified() == true ) {
		wxMessageDialog dlg(this, msg, _T("Save template . . . "), 
		                    wxYES|wxNO|wxCANCEL|wxICON_QUESTION|wxCENTRE);
 	
		int ret = dlg.ShowModal();
	
		if ( ret == wxID_YES ) {
			saveFile();
		} else if ( ret == wxID_CANCEL ) {
			return false;
		}
	}

	return true;
}
///////////////////////////////////////////////////////////////////
void MainFrame::prepareAndShowMonitorTemplatePreview(bool force) {
///////////////////////////////////////////////////////////////////
	if ( force == true )
		lastTemplateFileNameForPreview.clear();

	// check if a preview update is necessary
	if ( lastTemplateFileNameForPreview == getCurrentTemplatePathFileName() && m_stcFileContent->GetModify() == false)
		return;
		
	lastTemplateFileNameForPreview = getCurrentTemplatePathFileName();

	// wxString tfn(CncFileNameService::getCncTemplatePreviewFileName(getCurrentTemplateFormat()));
	// this causes file access errors between this app and the internet explorer
	// write a temp file instead to have anytime a new one
	wxString tfn(CncFileNameService::getTempFileName(getCurrentTemplateFormat()));
	
	TemplateFormat tf = getCurrentTemplateFormat();
	switch ( tf ) {
		case TplSvg:		tfn.append(".svg");
							break;
							
		case TplGcode:		tfn.append(".gcode");
							break;
		
		default:			// do nothing;
							break;
	}
		
	// create a copy to avoid a modification of m_stcFileContent
	wxTextFile file(tfn);
	if ( !file.Exists() )
		file.Create();
	
	if ( file.Open() ) {
		file.Clear();
		
		for (long i=0; i<m_stcFileContent->GetNumberOfLines(); i++) {
			wxString line = m_stcFileContent->GetLineText(i);
			file.AddLine(line);
		}
		
		file.Write();
		file.Close();
		
		openMonitorPreview(tfn);

	} else {
		std::cerr << "Error creating a temp file: " << tfn.c_str() << std::endl;
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::emergencyStop(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	std::cerr << "Emergency Stop detected" << std::endl;
	cnc->interrupt();
	setRefPostionState(false);
}
///////////////////////////////////////////////////////////////////
void MainFrame::changeUpdateInterval(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	unsigned int interval = 1;
	wxString val = m_cbUpdateInterval->GetValue();
	interval = wxAtoi(val);
	CncConfig::getGlobalCncConfig()->setUpdateInterval(interval);
}
///////////////////////////////////////////////////////////////////
void MainFrame::keyDownXY(wxKeyEvent& event) {
///////////////////////////////////////////////////////////////////
	// disconnect this event handler to avoid the processing of buffered events
	m_moveXYAxisCtl->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(keyDownXY), NULL, this);
	
	wxASSERT(cnc);
	int c = event.GetKeyCode();
	switch ( c ) {
		case WXK_UP:		navigateY(CncAnticlockwise); 	break;
		case WXK_DOWN:		navigateY(CncClockwise); 		break;
		case WXK_LEFT:		navigateX(CncAnticlockwise);	break;
		case WXK_RIGHT:		navigateX(CncClockwise); 		break;
		case WXK_SPACE:		m_moveZAxisCtl->SetFocus();		break;
		
		case WXK_RETURN: 	
							if ( GetAsyncKeyState(VK_CONTROL) != 0 ) 	m_includingWpt->SetValue(true);
							else										m_includingWpt->SetValue(false);
							setZero();
							event.Skip(false);
							break;
							
		default:		event.Skip(false);
	}
	
	// clear bufferd events and reconnect this event handler
	dispatchAll();
	m_moveXYAxisCtl->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(keyDownXY), NULL, this);
}
///////////////////////////////////////////////////////////////////
void MainFrame::keyDownZ(wxKeyEvent& event) {
///////////////////////////////////////////////////////////////////
	// disconnect this event handler to avoid the processing of buffered events
	m_moveZAxisCtl->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(keyDownZ), NULL, this);
	
	wxASSERT(cnc);
	int c = event.GetKeyCode();
	switch ( c ) {
		case WXK_DOWN:		navigateZ(CncAnticlockwise);
							break;
							
		case WXK_UP:		navigateZ(CncClockwise);
							break;
							
		case WXK_SPACE:		m_moveXYAxisCtl->SetFocus();
							break;
		
		case WXK_RETURN: 	
							if ( GetAsyncKeyState(VK_CONTROL) != 0 ) 	m_includingWpt->SetValue(true);
							else										m_includingWpt->SetValue(false);
							setZero();
							event.Skip(false);
							break;
							
		default:			event.Skip(false);
	}
	
	// clear bufferd events and reconnect this event handler
	dispatchAll();
	m_moveZAxisCtl->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(keyDownZ), NULL, this);
}
///////////////////////////////////////////////////////////////////
void MainFrame::mvSpinDownX(wxSpinEvent& event) {
///////////////////////////////////////////////////////////////////
	// disconnect this event handler to avoid the processing of buffered events
	m_spinButtonX->Disconnect(wxEVT_SPIN_DOWN, wxSpinEventHandler(mvSpinDownX), NULL, this);

	wxASSERT(cnc);
	navigateX(CncAnticlockwise);
	
	// clear bufferd events and reconnect this event handler
	dispatchAll();
	m_spinButtonX->Connect(wxEVT_SPIN_DOWN, wxSpinEventHandler(mvSpinDownX), NULL, this);
} 
///////////////////////////////////////////////////////////////////
void MainFrame::mvSpinUpX(wxSpinEvent& event) {
///////////////////////////////////////////////////////////////////
	// disconnect this event handler to avoid the processing of buffered events
	m_spinButtonX->Disconnect(wxEVT_SPIN_UP, wxSpinEventHandler(mvSpinUpX), NULL, this);

	wxASSERT(cnc);
	navigateX(CncClockwise);
	
	// clear bufferd events and reconnect this event handler
	dispatchAll();
	m_spinButtonX->Connect(wxEVT_SPIN_UP, wxSpinEventHandler(mvSpinUpX), NULL, this);
}
///////////////////////////////////////////////////////////////////
void MainFrame::mvSpinDownY(wxSpinEvent& event) {
///////////////////////////////////////////////////////////////////
	// disconnect this event handler to avoid the processing of buffered events
	m_spinButtonY->Disconnect(wxEVT_SPIN_DOWN, wxSpinEventHandler(mvSpinDownY), NULL, this);

	wxASSERT(cnc);
	navigateY(CncAnticlockwise);
	
	// clear bufferd events and reconnect this event handler
	dispatchAll();
	m_spinButtonY->Connect(wxEVT_SPIN_DOWN, wxSpinEventHandler(mvSpinDownY), NULL, this);
}
///////////////////////////////////////////////////////////////////
void MainFrame::mvSpinUpY(wxSpinEvent& event) {
///////////////////////////////////////////////////////////////////
	// disconnect this event handler to avoid the processing of buffered events
	m_spinButtonY->Disconnect(wxEVT_SPIN_UP, wxSpinEventHandler(mvSpinUpY), NULL, this);

	wxASSERT(cnc);
	navigateY(CncClockwise);
	
	// clear bufferd events and reconnect this event handler
	dispatchAll();
	m_spinButtonY->Connect(wxEVT_SPIN_UP, wxSpinEventHandler(mvSpinUpY), NULL, this);
}
///////////////////////////////////////////////////////////////////
void MainFrame::mvSpinDownZ(wxSpinEvent& event) {
///////////////////////////////////////////////////////////////////
	// disconnect this event handler to avoid the processing of buffered events
	m_spinButtonZ->Disconnect(wxEVT_SPIN_DOWN, wxSpinEventHandler(mvSpinDownZ), NULL, this);

	wxASSERT(cnc);
	navigateZ(CncAnticlockwise);
	
	// clear bufferd events and reconnect this event handler
	dispatchAll();
	m_spinButtonZ->Connect(wxEVT_SPIN_DOWN, wxSpinEventHandler(mvSpinDownZ), NULL, this);
}
///////////////////////////////////////////////////////////////////
void MainFrame::mvSpinUpZ(wxSpinEvent& event) {
///////////////////////////////////////////////////////////////////
	// disconnect this event handler to avoid the processing of buffered events
	m_spinButtonZ->Disconnect(wxEVT_SPIN_UP, wxSpinEventHandler(mvSpinUpZ), NULL, this);

	wxASSERT(cnc);
	navigateZ(CncClockwise);

	// clear bufferd events and reconnect this event handler
	dispatchAll();
	m_spinButtonZ->Connect(wxEVT_SPIN_UP, wxSpinEventHandler(mvSpinUpZ), NULL, this);
}
///////////////////////////////////////////////////////////////////
void MainFrame::navigateX(CncDirection d) {
///////////////////////////////////////////////////////////////////
	double fact = 1;
	bool shtKey = (GetAsyncKeyState(VK_SHIFT) 	!= 0);
	bool ctlKey = (GetAsyncKeyState(VK_CONTROL) != 0);
	bool altKey = (GetAsyncKeyState(VK_MENU) != 0);

	if ( altKey ) {
		fact /= 100;
	} else {
		if 		( shtKey && !ctlKey ) fact *= 2;
		else if ( ctlKey && !shtKey ) fact /= 10;
		else if ( ctlKey &&  shtKey ) fact *= 10;
	}

	switch ( d ) {
		case CncClockwise: 	fact *= +1; break;
		case CncAnticlockwise: fact *= -1; break;
		default: ;
	}

	if ( cnc->isInterrupted() ) {
		std::cerr << "CNC controller is interrupted, nothing will be done."<< std::endl;
		return;
	}
	
	cnc->manualSimpleMoveMetric(fact, 0.0, 0.0);
}
///////////////////////////////////////////////////////////////////
void MainFrame::navigateY(CncDirection d) {
///////////////////////////////////////////////////////////////////
	double fact = 1;
	bool shtKey = (GetAsyncKeyState(VK_SHIFT) 	!= 0);
	bool ctlKey = (GetAsyncKeyState(VK_CONTROL) != 0);
	bool altKey = (GetAsyncKeyState(VK_MENU) != 0);

	if ( altKey ) {
		fact /= 100;
	} else {
		if 		( shtKey && !ctlKey ) fact *= 2;
		else if ( ctlKey && !shtKey ) fact /= 10;
		else if ( ctlKey &&  shtKey ) fact *= 10;
	}

	switch ( d ) {
		case CncClockwise: 	fact *= +1; break;
		case CncAnticlockwise: fact *= -1; break;
		default: ;
	}
	
	if ( cnc->isInterrupted() ) {
		std::cerr << "CNC controller is interrupted, nothing will be done."<< std::endl;
		return;
	}
	
	cnc->manualSimpleMoveMetric(0.0, fact, 0.0);
}
///////////////////////////////////////////////////////////////////
void MainFrame::navigateZ(CncDirection d) {
///////////////////////////////////////////////////////////////////
	double fact = 1;
	bool shtKey = (GetAsyncKeyState(VK_SHIFT) 	!= 0);
	bool ctlKey = (GetAsyncKeyState(VK_CONTROL) != 0);
	bool altKey = (GetAsyncKeyState(VK_MENU) != 0);

	if ( altKey ) {
		fact /= 100;
	} else {
		if 		( shtKey && !ctlKey ) fact *= 2;
		else if ( ctlKey && !shtKey ) fact /= 10;
		else if ( ctlKey &&  shtKey ) fact *= 10;
	}

	switch ( d ) {
		case CncClockwise: 	fact *= +1; break;
		case CncAnticlockwise: fact *= -1; break;
		default: ;
	}

	if ( cnc->isInterrupted() ) {
		std::cerr << "CNC controller is interrupted, nothing will be done."<< std::endl;
		return;
	}

	cnc->manualSimpleMoveMetric(0.0, 0.0, fact);
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestVersion(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	std::stringstream ss;
	cnc->processCommand("V", ss);
	cnc::trc.logInfoMessage(ss);
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestConfig(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	cnc->processCommand("c", std::clog);
	m_outboundNotebook->SetSelection(OutboundSelection::VAL::SUMMARY_PANEL);
	m_notebookConfig->SetSelection(OutboundCfgSelection::VAL::CNC_CONFIG_PANEL);
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestControllerConfigFromButton(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	requestConfig(event);
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestErrorInfo(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	cnc->processCommand("?", std::clog);
	m_outboundNotebook->SetSelection(OutboundSelection::VAL::SUMMARY_PANEL);
	m_notebookConfig->SetSelection(OutboundCfgSelection::VAL::CNC_ERROR_PANEL);
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestControllerErrorInfoFromButton(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	requestErrorInfo(event);
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestErrorCount(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	int32_t cnt = cnc->getControllerErrorCount();
	std::stringstream ss;
	ss << "Current controller error count: " << cnt;
	cnc::trc.logInfoMessage(ss.str().c_str());
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestCurrentPos(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	CncLongPosition pos = cnc->getControllerPos();
	std::stringstream ss;
	ss << "Current controller position: " << pos;
	cnc::trc.logInfoMessage(ss.str().c_str());
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestCurrentLimitState(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	cnc->evaluateLimitState();
	cnc::trc.logInfoMessage("Limit controles were updated . . .");
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestCurrentLimitStateIcon(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	cnc->evaluateLimitState();
	cnc::trc.logInfoMessage("Limit controles were updated . . .");
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestReset() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	m_logger->Clear();
	cnc->processSetter(PID_SEPARATOR, SEPARARTOR_RESET);
	//cnc->reset();
	cnc->setup(true);
	cnc->clearDrawControl();
	
	updateSetterList();
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestReset(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	requestReset();
}
///////////////////////////////////////////////////////////////////
void MainFrame::saveEmuOutput(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString filePattern;
	
	switch ( cnc->getPortType() ) {
		case CncEMU_SVG: 	filePattern = "SVG Files (*.svg)|*.svg";  
						break;
		default:		std::clog << "No Emulator output available . . ." << std::endl;
						return;
	}

	wxFileDialog saveFileDialog(this, 
	                            _("Save Emulator Output as Template"), 
								getCurrentTemplatePathFileName(), 
								"",
								filePattern,
								wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
								
	if ( saveFileDialog.ShowModal() == wxID_CANCEL ) { 
		return;
	}

	if ( wxCopyFile(cnc->getSerial()->getPortName(), saveFileDialog.GetPath(), true) == false ) {
		std::cerr << "File copy failed:" << std::endl;
		std::cerr << " from:" << cnc->getSerial()->getPortName()<< std::endl;
		std::cerr << " to:" << saveFileDialog.GetPath() << std::endl;
		return;
	}
	
	wxString msg;
	msg << "Should it opened directly as template?";
	wxMessageDialog dlg(this, msg, _T("Save Emulator Output as Template"), 
						wxYES|wxNO|wxICON_QUESTION|wxCENTRE);

	if ( dlg.ShowModal() == wxID_YES ) {
		m_inputFileName->SetValue(saveFileDialog.GetFilename());
		m_inputFileName->SetHint(saveFileDialog.GetPath());
		
		if ( !openFile() ) {
			std::cerr << "Error while open file: " << saveFileDialog.GetPath().c_str() << std::endl;
		}
		
		prepareAndShowMonitorTemplatePreview(true);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::svgEmuClear(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc && cnc->getSerial());
	cnc->getSerial()->clearSVG();
	refreshSvgEmuFile(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::fileContentKeyDown(wxKeyEvent& event){
///////////////////////////////////////////////////////////////////
	updateFileContentPosition();
	
	// update tab label
	wxString name(m_templateNotebook->GetPageText(TemplateBookSelection::VAL::SOURCE_PANEL));
	if ( name.StartsWith("*") == false ) {
		name.Prepend("*");
		m_templateNotebook->SetPageText(TemplateBookSelection::VAL::SOURCE_PANEL, name);
	}
	
	bool ctlKey = (GetAsyncKeyState(VK_CONTROL) != 0);
	int c = event.GetUnicodeKey();
	
	wxString find(m_stcFileContent->GetSelectedText());
	
	// find
	if ( c == 'F' && ctlKey == true ) {
		event.Skip(false);
		
		if ( find.IsEmpty() == false ) 
			m_svgEditSearch->SetValue(find);
			
		m_svgEditSearch->SetFocus();
		return;
		
	// goto line
	} else if ( c == 'G' && ctlKey == true ) {
		wxTextEntryDialog dlg(this, "Line Number:", "Go to line . . .", "");
		dlg.SetMaxLength(8);
		dlg.SetTextValidator(wxFILTER_DIGITS);
		if ( dlg.ShowModal() == wxID_OK  ) {
			wxString s = dlg.GetValue();
			s.Trim(true).Trim(false);
			if ( s.IsEmpty() == false ) {
				long ln;
				s.ToLong(&ln);
				if ( ln >= 0 && ln <= m_stcFileContent->GetNumberOfLines() )
					m_stcFileContent->GotoLine(ln-1);
				else
					std::clog << "Template Source: Invalid line numer: " << ln << std::endl;
			}
		}
		
	// Undo
	} else if ( c == 'Z' && ctlKey == true) {
		m_stcFileContent->Undo();
	
	// Redo
	} else if ( c == 'Y' && ctlKey == true) {
		m_stcFileContent->Redo();
		
	// save
	} else if ( c == 'S' && ctlKey == true) {
		saveFile();
	}
	
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::fileContentKeyUp(wxKeyEvent& event) {
///////////////////////////////////////////////////////////////////
	updateFileContentPosition();
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::fileContentLeftUp(wxMouseEvent& event){
///////////////////////////////////////////////////////////////////
	event.Skip(true);
	updateFileContentPosition();
}
///////////////////////////////////////////////////////////////////
void MainFrame::fileContentLeftDown(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip(true);
	updateFileContentPosition();
	
	wxASSERT(pathGenerator);
	pathGenerator->updateEditControlCanReplaceState(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateFileContentPosition() {
///////////////////////////////////////////////////////////////////
	long x, y;
	m_stcFileContent->PositionToXY(m_stcFileContent->GetInsertionPoint(), &x, &y);
	wxString label = "Column: ";
	label << x + 1;
	m_filePosition->SetLabel(label);
	
	// try to select current  line as client id
	if ( motionMonitor != NULL ) {
		motionMonitor->setCurrentClientId(y);
		motionMonitor->display();
	}
	
	// display gcode help hint
	m_svgEditStatus->SetValue("");
	if ( getCurrentTemplateFormat() == TplGcode ) {
		int col = m_stcFileContent->GetColumn(m_stcFileContent->GetCurrentPos());
		wxString cl = m_stcFileContent->GetLine(y);
		
		// find first blank on left
		for ( int i=col - 1; i>=0; i-- ) {
			if ( cl[i] == ' ' || cl[i] == '\t' ) {
				cl = cl.Mid(i+1);
				break;
			}
		}
		m_svgEditStatus->SetValue(GCodeCommands::getGCodeHelpHint(cl));
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::openSVGPathGenerator(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( pathGenerator->IsIconized() )
		pathGenerator->Maximize(false);
	
	pathGenerator->Show(!pathGenerator->IsShown());
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestEnableStepperMotors(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	cnc->enableStepperMotors(m_miMotorEnableState->IsChecked());
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveHome(wxCommandEvent& event) {
/*
a) X(min), Y(min), Z(Top)
b) X(max), Y(max), Z(Top)
c) X(mid), Y(mid), Z(Top)
d) X(mid), Y(mid), Z(mid)
*/
///////////////////////////////////////////////////////////////////
	disableControls();
	selectMonitorBookCncPanel();

	wxString sel = m_homeDefintion->GetStringSelection();
	const char c = sel[0];
	switch (c) {
		case 'a':	cnc->moveZToMaxLimit();
					cnc->moveXToMinLimit();
					cnc->moveYToMinLimit();
					break;
		case 'b':	cnc->moveZToMaxLimit();
					cnc->moveXToMaxLimit();
					cnc->moveYToMaxLimit();
					break;
		case 'c':	cnc->moveZToMaxLimit();
					cnc->moveXToMid();
					cnc->moveYToMid();
					break;
		case 'd':	cnc->moveZToMid();
					cnc->moveXToMid();
					cnc->moveYToMid();
					break;
		default:std::cerr << "MainFrame::moveHome: invalid selection!" << std::endl;
	}
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveXToMid(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->moveXToMid();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveYToMid(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	selectMonitorBookCncPanel();
	
	
	disableControls();
	cnc->moveYToMid();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveZToMid(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->moveZToMid();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveToZeroXY(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	selectMonitorBookCncPanel();
	
	wxString sel = m_zeroMoveModeXY->GetStringSelection();
	char mode = sel[0];

	CncDimensions dim = CncDimension1D;
	switch ( mode ) {
		case '2':	dim = CncDimension2D; break;
	}

	disableControls();
	cnc->simpleMoveXYToZeroPos(dim);
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveToZeroXYZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	selectMonitorBookCncPanel();
	
	wxString sel = m_zeroMoveModeXYZ->GetStringSelection();
	char mode = sel[0];

	CncDimensions dim = CncDimension1D;
	switch ( mode ) {
		case '2':	dim = CncDimension2D; break;
		case '3':	dim = CncDimension3D; break;
	}

	disableControls();
	cnc->simpleMoveXYZToZeroPos(dim);
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveToZeroZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->simpleMoveZToZeroPos();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveXToMin(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->moveXToMinLimit();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveXToMax(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->moveXToMaxLimit();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveYToMin(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->moveYToMinLimit();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveYToMax(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->moveYToMaxLimit();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveZToMin(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->moveZToMinLimit();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveZToMax(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->moveZToMaxLimit();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveZToTop(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->moveZToTop();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveZToBottom(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->moveZToBottom();
	enableControls();
}
///////////////////////////////////////////////////////////////////
SvgOutputParameters& MainFrame::evaluteSvgOutputParameters(SvgOutputParameters& sop) {
///////////////////////////////////////////////////////////////////
	sop.zoomFactor 			= CncConfig::getGlobalCncConfig()->getSvgEmulatorCopyFactor();
	sop.copyOrigPath 		= CncConfig::getGlobalCncConfig()->getSvgResultWithOrigPathFlag();
	sop.onlyFirstCrossing 	= CncConfig::getGlobalCncConfig()->getSvgResultOnlyFirstCrossingFlag();
	
	return sop;
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
	openFileExtern(cmd, _(""));
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveManuallySliderX(wxScrollEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString val;
	if ( m_unit->GetValue() == "steps" ) {
		val = wxString::Format(wxT("%d"), (int)(m_xManuallySlider->GetValue() * CncConfig::getGlobalCncConfig()->getCalculationFactX()));
	} else {
		val = wxString::Format(wxT("%4.3f"), (double)(m_xManuallySlider->GetValue()));
	}
	m_metricX->SetValue(val);
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveManuallySliderY(wxScrollEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString val;
	if ( m_unit->GetValue() == "steps" ) {
		val = wxString::Format(wxT("%d"), (int)(m_yManuallySlider->GetValue() * CncConfig::getGlobalCncConfig()->getCalculationFactY()));
	} else {
		val = wxString::Format(wxT("%4.3f"), (double)(m_yManuallySlider->GetValue()));
	}
	m_metricY->SetValue(val);
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveManuallySliderZ(wxScrollEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString val;
	if ( m_unit->GetValue() == "steps" ) {
		val = wxString::Format(wxT("%d"), (int)(m_zManuallySlider->GetValue() * CncConfig::getGlobalCncConfig()->getCalculationFactZ()));
	} else {
		val = wxString::Format(wxT("%4.3f"), (double)(m_zManuallySlider->GetValue()));
	}
	m_metricZ->SetValue(val);
}
///////////////////////////////////////////////////////////////////
void MainFrame::enableManuallyControls(bool force) {
///////////////////////////////////////////////////////////////////
	bool xyFlag = m_checkManuallyXY->GetValue();
	bool zFlag  = !xyFlag;
	
	if ( force == true ) 
		xyFlag = zFlag = false;
	
	m_xManuallySlider->Enable(xyFlag);
	m_yManuallySlider->Enable(xyFlag);
	m_minManuallyXSlider->Enable(xyFlag);
	m_minManuallyYSlider->Enable(xyFlag);
	m_metricX->Enable(xyFlag);
	m_metricY->Enable(xyFlag);
	m_maxManuallyXSlider->Enable(xyFlag);
	m_maxManuallyYSlider->Enable(xyFlag);
	m_zeroManuallyXSlider->Enable(xyFlag);
	m_zeroManuallyYSlider->Enable(xyFlag);
	m_signManuallyXSlider->Enable(xyFlag);
	m_signManuallyYSlider->Enable(xyFlag);
	m_metricZ->Enable(xyFlag);
	m_mmRadioCoordinates->Enable(xyFlag);
		
	m_zManuallySlider->Enable(zFlag);
	m_minManuallyZSlider->Enable(zFlag);
	m_metricZ->Enable(zFlag);
	m_maxManuallyZSlider->Enable(zFlag);
	m_zeroManuallyZSlider->Enable(zFlag);
	m_signManuallyZSlider->Enable(zFlag);
	
	// handle interval mode
	if ( force == false ) {
		wxString sel = m_testIntervalMode->GetStringSelection();
		char mode = sel[0];

		m_testCountY->Enable(mode == 'A');
		m_testCountZ->Enable(mode == 'A');
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::checkManuallyXY(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_checkManuallyZ->SetValue(!m_checkManuallyXY->GetValue());
	enableManuallyControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::checkManuallyZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_checkManuallyXY->SetValue(!m_checkManuallyZ->GetValue());
	enableManuallyControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::changeManuallySliderX(wxScrollEvent& event) {
///////////////////////////////////////////////////////////////////
	moveManuallySliderX(event);
}
///////////////////////////////////////////////////////////////////
void MainFrame::changeManuallySliderY(wxScrollEvent& event) {
///////////////////////////////////////////////////////////////////
	moveManuallySliderY(event);
}
///////////////////////////////////////////////////////////////////
void MainFrame::changeManuallySliderZ(wxScrollEvent& event) {
///////////////////////////////////////////////////////////////////
	moveManuallySliderZ(event);
}
///////////////////////////////////////////////////////////////////
void MainFrame::minManuallyXSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_xManuallySlider->SetValue(m_xManuallySlider->GetMin());
	
	wxString val;
	if ( m_unit->GetValue() == "steps" ) {
		val = wxString::Format(wxT("%d"), (int)(m_xManuallySlider->GetValue() * CncConfig::getGlobalCncConfig()->getCalculationFactX()));
	} else {
		val = wxString::Format(wxT("%4.3f"), (double)(m_xManuallySlider->GetValue()));
	}
	m_metricX->SetValue(val);
}
///////////////////////////////////////////////////////////////////
void MainFrame::maxManuallyXSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_xManuallySlider->SetValue(m_xManuallySlider->GetMax());
	
	wxString val;
	if ( m_unit->GetValue() == "steps" ) {
		val = wxString::Format(wxT("%d"), (int)(m_xManuallySlider->GetValue() * CncConfig::getGlobalCncConfig()->getCalculationFactX()));
	} else {
		val = wxString::Format(wxT("%4.3f"), (double)(m_xManuallySlider->GetValue()));
	}
	m_metricX->SetValue(val);
}
///////////////////////////////////////////////////////////////////
void MainFrame::minManuallyYSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_yManuallySlider->SetValue(m_yManuallySlider->GetMin());
	
	wxString val;
	if ( m_unit->GetValue() == "steps" ) {
		val = wxString::Format(wxT("%d"), (int)(m_yManuallySlider->GetValue() * CncConfig::getGlobalCncConfig()->getCalculationFactY()));
	} else {
		val = wxString::Format(wxT("%4.3f"), (double)(m_yManuallySlider->GetValue()));
	}
	m_metricY->SetValue(val);
}
///////////////////////////////////////////////////////////////////
void MainFrame::maxManuallyYSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_yManuallySlider->SetValue(m_yManuallySlider->GetMax());
	
	wxString val;
	if ( m_unit->GetValue() == "steps" ) {
		val = wxString::Format(wxT("%d"), (int)(m_yManuallySlider->GetValue() * CncConfig::getGlobalCncConfig()->getCalculationFactY()));
	} else {
		val = wxString::Format(wxT("%4.3f"), (double)(m_yManuallySlider->GetValue()));
	}
	m_metricY->SetValue(val);
}
///////////////////////////////////////////////////////////////////
void MainFrame::minManuallyZSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_zManuallySlider->SetValue(m_zManuallySlider->GetMin());
	
	wxString val;
	if ( m_unit->GetValue() == "steps" ) {
		val = wxString::Format(wxT("%d"), (int)(m_zManuallySlider->GetValue() * CncConfig::getGlobalCncConfig()->getCalculationFactZ()));
	} else {
		val = wxString::Format(wxT("%4.3f"), (double)(m_zManuallySlider->GetValue()));
	}
	m_metricZ->SetValue(val);
}
///////////////////////////////////////////////////////////////////
void MainFrame::maxManuallyZSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_zManuallySlider->SetValue(m_zManuallySlider->GetMax());
	
	wxString val;
	if ( m_unit->GetValue() == "steps" ) {
		val = wxString::Format(wxT("%d"), (int)(m_zManuallySlider->GetValue() * CncConfig::getGlobalCncConfig()->getCalculationFactZ()));
	} else {
		val = wxString::Format(wxT("%4.3f"), (double)(m_zManuallySlider->GetValue()));
	}
	m_metricZ->SetValue(val);
}
///////////////////////////////////////////////////////////////////
void MainFrame::zeroManuallyXSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_xManuallySlider->SetValue(0);
	wxString val;
	if ( m_unit->GetValue() == "steps" ) {
		val = wxString::Format(wxT("%d"), 0);
	} else {
		val = wxString::Format(wxT("%4.3f"), 0.0);
	}
	m_metricX->SetValue(val);
}
///////////////////////////////////////////////////////////////////
void MainFrame::zeroManuallyYSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_yManuallySlider->SetValue(0);
	wxString val;
	if ( m_unit->GetValue() == "steps" ) {
		val = wxString::Format(wxT("%d"), 0);
	} else {
		val = wxString::Format(wxT("%4.3f"), 0.0);
	}
	m_metricY->SetValue(val);
}
///////////////////////////////////////////////////////////////////
void MainFrame::zeroManuallyZSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_zManuallySlider->SetValue(0);
	wxString val;
	if ( m_unit->GetValue() == "steps" ) {
		val = wxString::Format(wxT("%d"), 0);
	} else {
		val = wxString::Format(wxT("%4.3f"), 0.0);
	}
	m_metricZ->SetValue(val);
}
///////////////////////////////////////////////////////////////////
void MainFrame::signManuallyXSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString val = m_metricX->GetValue();
	double v;
	val.ToDouble(&v);
	if ( v != 0.0 )
		v *= -1;
	
	if ( m_unit->GetValue() == "steps" ) {
		val = wxString::Format(wxT("%6.0f"), v);
		m_xManuallySlider->SetValue(v / CncConfig::getGlobalCncConfig()->getCalculationFactX());
	} else {
		val = wxString::Format(wxT("%4.3f"), v);
		m_xManuallySlider->SetValue(v);
	}
	
	m_metricX->SetValue(val);
}
///////////////////////////////////////////////////////////////////
void MainFrame::signManuallyYSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString val = m_metricY->GetValue();
	double v;
	val.ToDouble(&v);
	if ( v != 0.0 )
		v *= -1;
	
	if ( m_unit->GetValue() == "steps" ) {
		val = wxString::Format(wxT("%6.0f"), v);
		m_yManuallySlider->SetValue(v / CncConfig::getGlobalCncConfig()->getCalculationFactY());
	} else {
		val = wxString::Format(wxT("%4.3f"), v);
		m_yManuallySlider->SetValue(v);
	}
	
	m_metricY->SetValue(val);
}
///////////////////////////////////////////////////////////////////
void MainFrame::signManuallyZSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString val = m_metricZ->GetValue();
	double v;
	val.ToDouble(&v);
	if ( v != 0.0 )
		v *= -1;
	
	if ( m_unit->GetValue() == "steps" ) {
		val = wxString::Format(wxT("%6.0f"), v);
		m_zManuallySlider->SetValue(v / CncConfig::getGlobalCncConfig()->getCalculationFactZ());
	} else {
		val = wxString::Format(wxT("%4.3f"), v);
		m_zManuallySlider->SetValue(v);
	}
	
	m_metricZ->SetValue(val);
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateMetricX(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString val = m_metricX->GetValue();
	double v;
	val.ToDouble(&v);
	
	if ( m_unit->GetValue() == "steps" ) {
		m_xManuallySlider->SetValue(v / CncConfig::getGlobalCncConfig()->getCalculationFactX());
	} else {
		m_xManuallySlider->SetValue(v);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateMetricY(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString val = m_metricY->GetValue();
	double v;
	val.ToDouble(&v);
	
	if ( m_unit->GetValue() == "steps" ) {
		m_yManuallySlider->SetValue(v / CncConfig::getGlobalCncConfig()->getCalculationFactY());
	} else {
		m_yManuallySlider->SetValue(v);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateMetricZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString val = m_metricZ->GetValue();
	double v;
	val.ToDouble(&v);
	
	if ( m_unit->GetValue() == "steps" ) {
		m_zManuallySlider->SetValue(v / CncConfig::getGlobalCncConfig()->getCalculationFactZ());
	} else {
		m_zManuallySlider->SetValue(v);
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
			//inboundFileParser->selectSourceControl(val.GetInteger() - 1);
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
void MainFrame::killFocusMoveXYAxis(wxFocusEvent& event) {
	m_moveXYAxisCtl->SetBackgroundColour(*wxWHITE);
	m_moveXYAxisCtl->Refresh();
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::setFocusMoveXYAxis(wxFocusEvent& event)  {
	wxColor c(191,205,219);
	m_moveXYAxisCtl->SetBackgroundColour(c);
	m_moveXYAxisCtl->Refresh();
	m_moveXYAxisCtl->SetInsertionPoint(0);
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::killFocusMoveZAxis(wxFocusEvent& event) {
	m_moveZAxisCtl->SetBackgroundColour(*wxWHITE);
	m_moveZAxisCtl->Refresh();
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::setFocusMoveZAxis(wxFocusEvent& event) {
	wxColor c(191,205,219);
	m_moveZAxisCtl->SetBackgroundColour(c);
	m_moveZAxisCtl->Refresh();
	m_moveZAxisCtl->SetInsertionPoint(0);
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateCurveLibResolution(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	updateCurveLibResolution();
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateCurveLibResolution() {
///////////////////////////////////////////////////////////////////
	wxString sel = m_cbCurveLibResolution->GetStringSelection();
	double v = 1.0;
	sel.ToDouble(&v);
	
	CncConfig::setCurveLibResolution(v);
	updateCncConfigTrace();
}
///////////////////////////////////////////////////////////////////
void MainFrame::openPreview(CncFilePreview* ctrl, const wxString& fn) {
///////////////////////////////////////////////////////////////////
	wxASSERT(ctrl);
	
	TemplateFormat tf = getCurrentTemplateFormat(fn);
	switch ( tf ) {
		case TplSvg:		ctrl->selectSVGPreview(fn);
							break;
							
		case TplGcode:		ctrl->selectGCodePreview(fn);
							break;
		
		default:			cnc::trc.logError(wxString("Cant preview: ") + fn );
							break;
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::openMainPreview(const wxString& fn) {
///////////////////////////////////////////////////////////////////
	selectMainBookPreviewPanel();
	openPreview(mainFilePreview, fn);
}
///////////////////////////////////////////////////////////////////
void MainFrame::openMonitorPreview(const wxString& fn) {
///////////////////////////////////////////////////////////////////
	// the call below creates an endlos loop
	// because selectMonitorBookTemplatePanel 
	// calls inderetly openMonitorPreview
	// selectMonitorBookTemplatePanel();
	
	openPreview(monitorFilePreview, fn);
}
///////////////////////////////////////////////////////////////////
void MainFrame::openFileFromFileManager(const wxString& f) {
///////////////////////////////////////////////////////////////////
	selectMainBookSourcePanel();
	selectMonitorBookTemplatePanel();
	
	if ( checkIfTemplateIsModified() == false ) {
		prepareAndShowMonitorTemplatePreview(true);
		return;
	}

	wxFileName fn(f);
	m_inputFileName->SetValue(fn.GetFullName());
	m_inputFileName->SetHint(fn.GetFullPath());

	openFile();
	prepareAndShowMonitorTemplatePreview(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::lruListItemLeave(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_keepFileManagerPreview->IsChecked() )
		return;
		
	if ( m_mainViewBook->GetSelection() != MainBookSelection::VAL::PREVIEW_PANEL )
		return;
		
	selectMainBookSourcePanel();
	
	int n = m_lruList->GetItemCount();
	for (int i = 0; i < n; i++)
		m_lruList->SetItemState(i, 0, wxLIST_STATE_SELECTED);
}
///////////////////////////////////////////////////////////////////
void MainFrame::lruListItemActivated(wxListEvent& event) {
///////////////////////////////////////////////////////////////////
	wxListItem info;
	info.m_itemId = event.m_itemIndex;
	info.m_col = 1;
	info.m_mask = wxLIST_MASK_TEXT;
	
	if ( m_lruList->GetItem(info) )
		openFileFromFileManager(wxString(lruFileList.getFileName(info.m_itemId)));
}
///////////////////////////////////////////////////////////////////
void MainFrame::lruListItemSelected(wxListEvent& event) {
///////////////////////////////////////////////////////////////////
	wxListItem info;
	info.m_itemId = event.m_itemIndex;
	info.m_col = 1;
	info.m_mask = wxLIST_MASK_TEXT;
	
	if ( m_lruList->GetItem(info) )
		openMainPreview(wxString(lruFileList.getFileName(info.m_itemId)));
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectMainBookSourcePanel() {
///////////////////////////////////////////////////////////////////
	m_mainViewSelector->SetSelection(MainBookSelection::VAL::SOURCE_PANEL);
	m_mainViewBook->SetSelection(MainBookSelection::VAL::SOURCE_PANEL);
	
	m_templateNotebook->SetSelection( TemplateBookSelection::VAL::SOURCE_PANEL);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectMainBookPreviewPanel() {
///////////////////////////////////////////////////////////////////
	m_mainViewSelector->SetSelection(MainBookSelection::VAL::PREVIEW_PANEL);
	m_mainViewBook->SetSelection(MainBookSelection::VAL::PREVIEW_PANEL);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectMainBookSetupPanel() {
	m_mainViewSelector->SetSelection(MainBookSelection::VAL::SETUP_PANEL);
	m_mainViewBook->SetSelection(MainBookSelection::VAL::SETUP_PANEL);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectMainBookReferencePanel() {
	m_mainViewSelector->SetSelection(MainBookSelection::VAL::REFERENCE_PANEL);
	m_mainViewBook->SetSelection(MainBookSelection::VAL::REFERENCE_PANEL);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectMainBookManuelPanel() {
	m_mainViewSelector->SetSelection(MainBookSelection::VAL::MANUEL_PANEL);
	m_mainViewBook->SetSelection(MainBookSelection::VAL::MANUEL_PANEL);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectMainBookTestPanel() {
	m_mainViewSelector->SetSelection(MainBookSelection::VAL::TEST_PANEL);
	m_mainViewBook->SetSelection(MainBookSelection::VAL::TEST_PANEL);
}
///////////////////////////////////////////////////////////////////
void MainFrame::mainViewSelectorSelected(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_mainViewBook->SetSelection(m_mainViewSelector->GetSelection());
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectMonitorBookCncPanel() {
///////////////////////////////////////////////////////////////////
	m_monitorViewSelector->SetSelection(MonitorBookSelection::VAL::CNC_PANEL);
	m_monitorViewBook->SetSelection(MonitorBookSelection::VAL::CNC_PANEL);
	
	m_outboundNotebook->SetSelection(OutboundSelection::VAL::MOTION_MONITOR_PANAL);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectMonitorBookTemplatePanel() {
///////////////////////////////////////////////////////////////////
	prepareAndShowMonitorTemplatePreview(true);
	
	m_monitorViewSelector->SetSelection(MonitorBookSelection::VAL::TEMPLATE_PANEL);
	m_monitorViewBook->SetSelection(MonitorBookSelection::VAL::TEMPLATE_PANEL);
}
///////////////////////////////////////////////////////////////////
void MainFrame::monitorViewSelectorSelected(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_monitorViewSelector->GetSelection() == MonitorBookSelection::VAL::TEMPLATE_PANEL)
		prepareAndShowMonitorTemplatePreview(true);
		
	m_monitorViewBook->SetSelection(m_monitorViewSelector->GetSelection());
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
		
	if ( GetAuimgrMain()->GetPane(pane).IsShown() == false ) {
		GetAuimgrMain()->GetPane(pane).Show();
		
		if ( menu != NULL )
			menu->Check(true);
			
		if ( update )
			GetAuimgrMain()->Update();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::hideAuiPane(wxWindow* pane, wxMenuItem* menu, bool update) {
///////////////////////////////////////////////////////////////////
	if ( pane == NULL )
		return;

	if ( GetAuimgrMain()->GetPane(pane).IsShown() == true ) {
		GetAuimgrMain()->GetPane(pane).Hide();
		GetAuimgrMain()->GetPane(pane).DestroyOnClose(false);
		pane->Close(true);
		
		if ( pane == getAUIPaneByName("SerialSpy") )
			enableSerialSpy(false);
		
		if ( menu != NULL )
			menu->Check(false);
			
		if ( update )
			GetAuimgrMain()->Update();
	}
}
///////////////////////////////////////////////////////////////////
wxWindow* MainFrame::getAUIPaneByName(const wxString& name) {
///////////////////////////////////////////////////////////////////
	if      ( name == "Toolbar" ) 			return m_auibarMain;
	else if ( name == "SourceView")			return m_scrollWinMain;
	else if ( name == "Logger")				return m_scrollWinLogger;
	else if ( name == "Outbound")			return m_scrollWinMonitor;
	else if ( name == "TemplateManager")	return m_scrollWinFile;
	else if ( name == "StatusBar")			return m_statusBar;
	else if ( name == "SerialSpy")			return m_serialSpyView;
	else if ( name == "SpeedView")			return m_panelSpeed;
	else if ( name == "UnitCalculator")		return m_svgUnitCalulator;
	else if ( name == "Debugger")			return m_debuggerView;
	else if ( name == "PositionMonitor")	return m_positionMonitorView;
	else if ( name == "ZView")				return m_panelZView;
	
	return NULL;
}
///////////////////////////////////////////////////////////////////
wxMenuItem* MainFrame::getAUIMenuByName(const wxString& name) {
///////////////////////////////////////////////////////////////////
	if      ( name == "Toolbar" ) 			return m_miToolbar;
	else if ( name == "StatusBar")			return m_miViewStatusbar;
	else if ( name == "SourceView")			return m_miViewMainView;
	else if ( name == "Logger")				return m_miViewLogger;
	else if ( name == "Outbound")			return m_miViewMonitor;
	else if ( name == "TemplateManager")	return m_miViewTemplateManager;
	else if ( name == "SerialSpy")			return m_miViewSpy;
	else if ( name == "SpeedView")			return m_miViewSpeed;
	else if ( name == "UnitCalculator")		return m_miViewUnitCalculator;
	else if ( name == "Debugger")			return m_miViewDebugger;
	else if ( name == "PositionMonitor")	return m_miViewPosMonitor;
	else if ( name == "ZView")				return m_miViewZAxis;
	
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
	if ( w == NULL ) {
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
void MainFrame::viewZAxis(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	toggleAuiPane("ZView");
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewSpy(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	toggleAuiPane("SerialSpy");
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewSpeed(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	toggleAuiPane("SpeedView");
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
void MainFrame::viewPosistionMonitor(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	toggleAuiPane("PositionMonitor");
}
///////////////////////////////////////////////////////////////////
void MainFrame::closeAuiPane(wxAuiManagerEvent& evt) {
///////////////////////////////////////////////////////////////////
	wxMenuItem* menu = getAUIMenuByName(evt.pane->name);
	if ( menu != NULL )
		menu->Check(!evt.pane->window->IsShown());

	if ( evt.pane->window == m_serialSpyView )
		enableSerialSpy(false);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onPerspectiveTimer(wxTimerEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( perspectiveTimer.IsRunning() == true )
		perspectiveTimer.Stop();
		
	if ( m_miToolbar->IsChecked() )
		showAuiPane("Toolbar");
		
	if ( m_miViewStatusbar->IsChecked() )
		showAuiPane("StatusBar");
		
	//GetAuimgrMain()->Update();
}
///////////////////////////////////////////////////////////////////
void MainFrame::maximizeAuiPane(wxAuiManagerEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( event.pane->window == m_scrollWinMonitor || event.pane->window == m_scrollWinMain )
		perspectiveTimer.Start(20);
		
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::activateAuiPane(wxAuiManagerEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::buttonAuiPane(wxAuiManagerEvent& event) {
///////////////////////////////////////////////////////////////////
	#define AUI_CLOSE_BTN_ID 	= 101;
	#define AUI_MIN_MAX_BTN_ID 	= 102;
	#define AUI_PIN_BTN_ID 		= 104;
	
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::restoreAuiPane(wxAuiManagerEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip(true);
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
void MainFrame::hideAllAuiPanes() {
///////////////////////////////////////////////////////////////////
	wxAuiPaneInfoArray panes = m_auimgrMain->GetAllPanes();
	for (unsigned int i = 0; i < panes.GetCount(); ++i)
		hideAuiPane( panes.Item(i).name);
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewAllAuiPanes(bool withSpy) {
///////////////////////////////////////////////////////////////////
	wxAuiPaneInfoArray panes = m_auimgrMain->GetAllPanes();
	for (unsigned int i = 0; i < panes.GetCount(); ++i)
		showAuiPane( panes.Item(i).name);

	if ( withSpy )
		showAuiPane(m_serialSpyView, m_miViewSpy);
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
			
			case OutboundSelection::VAL::SVG_OUTPUT_PANEL:
									m_svgEmuToggleOrigPath->Enable( CncConfig::getGlobalCncConfig()->getSvgResultWithOrigPathFlag() );
									break;
									
			case OutboundSelection::VAL::MOTION_MONITOR_PANAL:
									if ( cnc )
										cnc->updatePreview3D();
									break;
		}
	} 
}
///////////////////////////////////////////////////////////////////
void MainFrame::outboundBookChanging(wxNotebookEvent& event) {
///////////////////////////////////////////////////////////////////
	unsigned int sel = event.GetSelection();
	wxString svgFile;
	
	if ( (wxWindow*)event.GetEventObject() == m_outboundNotebook ) {
		switch ( sel ) {
			case OutboundSelection::VAL::SVG_SOURCE_PANEL: 
			
					if ( m_stcEmuSource->IsModified() ) {
						m_stcEmuSource->SaveFile(getSvgEmuFileName(svgFile));
						evaluateSvgEmuModificationTimeStamp();
						refreshSvgEmuFile();
					}
					break;
		}
	}
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
void MainFrame::svgEmuToogleOriginalPath(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_svgView->RunScript("toggleOrigPath();");
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateStepDelay() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	cnc->setStepDelay(m_stepDelay->GetValue());
	
	wxString val;
	val << m_stepDelay->GetValue();
	val << " ms";
	m_stepDelayValue->SetLabel(val);
}
///////////////////////////////////////////////////////////////////
void MainFrame::stepDelayChanged(wxScrollEvent& event) {
///////////////////////////////////////////////////////////////////
	m_miCfgCustom->Check();
	updateStepDelay();
}
///////////////////////////////////////////////////////////////////
void MainFrame::stepDelayThumbtrack(wxScrollEvent& event) {
///////////////////////////////////////////////////////////////////
	m_miCfgCustom->Check();
	updateStepDelay();
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
	if ( pngAnimation != NULL ) {
		if ( pngAnimation->IsRunning() )
			pngAnimation->Stop();
			
		pngAnimation->Start(50);
		pngAnimation->Update();
	}
	
	wxColor color(255,128,128);
	m_cmdDuration->SetForegroundColour(color);
	m_cmdDuration->GetParent()->Update();
	processStartTime = wxDateTime::UNow();
}
///////////////////////////////////////////////////////////////////
void MainFrame::stopAnimationControl() {
///////////////////////////////////////////////////////////////////
	m_cmdDuration->SetForegroundColour(*wxWHITE);
	
	if ( pngAnimation != NULL ) {
		if ( pngAnimation->IsRunning() ) {
			pngAnimation->Stop();
			pngAnimation->Update();
			processEndTime = wxDateTime::UNow();
		}
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::dclickDurationCount(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString msg(m_cmdDuration->GetToolTipText());
	if ( msg != "" ) {
		wxMessageDialog dlg(this, msg, _T("Last processing timestamps . . . "), 
						wxOK|wxCENTRE|wxICON_INFORMATION);
		
		dlg.ShowModal();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::fileContentRightDown(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( getCurrentTemplateFormat() != TplSvg )
		return;
	
	wxPoint point = event.GetPosition();
	// If from keyboard
	if (point.x == -1 && point.y == -1) {
		wxSize size = GetSize();
		point.x = size.x / 2;
		point.y = size.y / 2;
	} else {
		point = ScreenToClient(point);
	}
	
	// Show popupmenu at position
	if ( stcFileContentPopupMenu != NULL ) {
		SvgEditPopup::enablePathGeneratorMenuItem(stcFileContentPopupMenu);
		m_stcFileContent->PopupMenu(stcFileContentPopupMenu, point);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::emuContentRightDown(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	wxPoint point = event.GetPosition();
	// If from keyboard
	if (point.x == -1 && point.y == -1) {
		wxSize size = GetSize();
		point.x = size.x / 2;
		point.y = size.y / 2;
	} else {
		point = ScreenToClient(point);
	}
	
	// Show popupmenu at position
	if ( stcEmuContentPopupMenu != NULL ) {
		SvgEditPopup::enablePathGeneratorMenuItem(stcEmuContentPopupMenu);
		m_stcEmuSource->PopupMenu(stcEmuContentPopupMenu, point);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::createStcEmuControlPopupMenu() {
///////////////////////////////////////////////////////////////////
	if ( stcEmuContentPopupMenu != NULL )
		return;

	stcEmuContentPopupMenu = SvgEditPopup::createMenu(m_stcEmuSource, stcEmuContentPopupMenu, false);
}
///////////////////////////////////////////////////////////////////
void MainFrame::createStcFileControlPopupMenu() {
///////////////////////////////////////////////////////////////////
	if ( stcFileContentPopupMenu != NULL )
		return;

	stcFileContentPopupMenu = SvgEditPopup::createMenu(m_stcFileContent, stcFileContentPopupMenu, true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::decorateSearchButton() {
///////////////////////////////////////////////////////////////////
	wxBitmap bmp = ImageLib16().Bitmap("BMP_OK16");
	if ( bmp.IsOk() )
		m_svgEditSearchState->SetBitmap(bmp);
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
	wxString find = m_svgEditSearch->GetValue();
	
	int pos = m_svgEditSearch->FindString(find);
	if ( pos >= 0 ) {
		m_svgEditSearch->Delete((unsigned int)pos);
	}
	
	m_svgEditSearch->Insert(find, 0);
	m_svgEditSearch->ChangeValue(find);
}
///////////////////////////////////////////////////////////////////
void MainFrame::svgEditFind(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_svgEditSearch->GetValue().IsEmpty() )
		return;

	wxString find = m_svgEditSearch->GetValue();
	fixRegexParen(find);
	
	updateSvgSearchControl();
	
	m_stcFileContent->SetCurrentPos(m_stcFileContent->GetCurrentPos() + m_svgEditSearch->GetValue().Len() + 1);
	m_stcFileContent->SearchAnchor();
	int ret = m_stcFileContent->SearchNext(getSvgEditSearchFlags(), find);
	
	if ( ret == wxNOT_FOUND ) {
		m_stcFileContent->ClearSelections();
		m_stcFileContent->SearchAnchor();
	} else {
		ensureSvgEditLineIsVisible(ret);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::svgEditFindPrev(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_svgEditSearch->GetValue().IsEmpty() )
		return;
		
	wxString find = m_svgEditSearch->GetValue();
	fixRegexParen(find);

	updateSvgSearchControl();
	
	int ncp = m_stcFileContent->GetCurrentPos() - m_svgEditSearch->GetValue().Len() - 1;
	if( ncp < 0 )
		ncp = m_stcFileContent->GetLastPosition();
		
	m_stcFileContent->SetCurrentPos(ncp);
	m_stcFileContent->SearchAnchor();
	int ret = m_stcFileContent->SearchPrev(getSvgEditSearchFlags(), find);
	
	if ( ret == wxNOT_FOUND ) {
		m_stcFileContent->ClearSelections();
		m_stcFileContent->SearchAnchor();
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
	wxString find = m_svgEditSearch->GetValue();
	wxString toolTip("");
	wxBitmap bmp = ImageLib16().Bitmap("BMP_OK16");
	m_svgEditSearchState->SetBitmap(bmp);
	
	fixRegexParen(find);
	m_stcFileContent->SearchAnchor();
	int ret = m_stcFileContent->SearchNext(getSvgEditSearchFlags(), find);
	
	if( ret == wxNOT_FOUND ) {
		m_stcFileContent->ClearSelections();
		bmp = ImageLib16().Bitmap("BMP_WARNING16");
		m_svgEditSearchState->SetBitmap(bmp);

		toolTip << "'";
		toolTip << find;
		toolTip << "' not found";
		
	} else {
		ensureSvgEditLineIsVisible(ret);
	}
	m_svgEditSearchState->Show();
	m_svgEditSearchState->Refresh();
	m_svgEditStatus->SetLabel(toolTip);
	m_svgEditSearchState->SetToolTip(toolTip);
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
		line = m_stcFileContent->LineFromPosition(m_stcFileContent->GetSelectionStart());
	}
	
	int linesOnScreen = m_stcFileContent->LinesOnScreen();
	if(!((line > m_stcFileContent->GetFirstVisibleLine()) && (line < (m_stcFileContent->GetFirstVisibleLine() + linesOnScreen)))) {
		// To place our line in the middle, the first visible line should be
		// the: line - (linesOnScreen / 2)
		int firstVisibleLine = line - (linesOnScreen / 2);
		if( firstVisibleLine < 0 ) {
			firstVisibleLine = 0;
		}
		m_stcFileContent->SetFirstVisibleLine(firstVisibleLine);
	}
	
	m_stcFileContent->EnsureVisible(line);
	m_stcFileContent->ScrollToColumn(0);
	
	int xScrollPosBefore = m_stcFileContent->GetScrollPos(wxHORIZONTAL);
	m_stcFileContent->EnsureCaretVisible();
	int xScrollPosAfter = m_stcFileContent->GetScrollPos(wxHORIZONTAL);
	
	if( xScrollPosBefore != xScrollPosAfter ) {
		// EnsureCaretVisible scrolled the page
		// scroll it a bit more
		int scrollToPos = m_stcFileContent->GetSelectionStart();
		if(	scrollToPos != wxNOT_FOUND	) {
			m_stcFileContent->ScrollToColumn(m_stcFileContent->GetColumn(scrollToPos));
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
void MainFrame::svgEmuZoomMinus(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_svgView->SetZoomType(wxWEBVIEW_ZOOM_TYPE_LAYOUT);

	switch ( m_svgView->GetZoom() ) {
		case wxWEBVIEW_ZOOM_TINY: 		// Do nothing
										break;
		case wxWEBVIEW_ZOOM_SMALL:		m_svgView->SetZoom(wxWEBVIEW_ZOOM_TINY);
										break;
		case wxWEBVIEW_ZOOM_MEDIUM:		m_svgView->SetZoom(wxWEBVIEW_ZOOM_SMALL);
										break;
		case wxWEBVIEW_ZOOM_LARGE:		m_svgView->SetZoom(wxWEBVIEW_ZOOM_MEDIUM);
										break;
		case wxWEBVIEW_ZOOM_LARGEST:	m_svgView->SetZoom(wxWEBVIEW_ZOOM_LARGE);
										break;
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::svgEmuZoomPlus(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_svgView->SetZoomType(wxWEBVIEW_ZOOM_TYPE_LAYOUT);
	
	switch ( m_svgView->GetZoom() ) {
		case wxWEBVIEW_ZOOM_TINY: 		m_svgView->SetZoom(wxWEBVIEW_ZOOM_SMALL);
										break;
		case wxWEBVIEW_ZOOM_SMALL:		m_svgView->SetZoom(wxWEBVIEW_ZOOM_MEDIUM);
										break;
		case wxWEBVIEW_ZOOM_MEDIUM:		m_svgView->SetZoom(wxWEBVIEW_ZOOM_LARGE);
										break;
		case wxWEBVIEW_ZOOM_LARGE:		m_svgView->SetZoom(wxWEBVIEW_ZOOM_LARGEST);
										break;
		case wxWEBVIEW_ZOOM_LARGEST:	// Do nothing
										break;
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::svgEmuZoomHome(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	clog << "Currently not supported" << endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::copyLogger(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_logger->SelectAll();
	m_logger->Copy();
	m_logger->SelectNone();
}
///////////////////////////////////////////////////////////////////
bool MainFrame::openFileExtern(const wxString& tool, const char* file) {
///////////////////////////////////////////////////////////////////
	wxString f(file);
	return openFileExtern(tool, f);
}
///////////////////////////////////////////////////////////////////
bool MainFrame::openFileExtern(const wxString& tool, wxString& file) {
///////////////////////////////////////////////////////////////////
	startAnimationControl();
	
	wxString cmd(tool);
	if ( file.IsEmpty() == false ) {
		
		if ( wxFileName(file).Exists() == false ) {
			std::cerr << "MainFrame::openFileExtern: Failed:" << std::endl;
			std::cerr << " Can't open:" << file << std::endl;
			stopAnimationControl();
			return false;
		}
		
		cmd += " \"";
		cmd += file;
		cmd += "\"";
	}
	
	wxExecute(cmd);
	wxASSERT(cnc);
	waitActive(1500);

	stopAnimationControl();
	return true;
}
///////////////////////////////////////////////////////////////////
void MainFrame::toggleEmuWordWrapMode(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_stcEmuSource->SetWrapMode(!m_svgEmuToggleWordWrap->GetValue());
}
///////////////////////////////////////////////////////////////////
void MainFrame::toggleTemplateWordWrapMode(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_stcFileContent->SetWrapMode(!m_btSvgToggleWordWrap->GetValue());
}
///////////////////////////////////////////////////////////////////
void MainFrame::marginClickFileContent(wxStyledTextEvent& event) {
///////////////////////////////////////////////////////////////////
	//folding
	if ( event.GetMargin() == MARGIN_FOLD ) {
		int lineClick = m_stcFileContent->LineFromPosition(event.GetPosition());
		int levelClick = m_stcFileContent->GetFoldLevel(lineClick);
		
		if ( (levelClick & wxSTC_FOLDLEVELHEADERFLAG ) > 0) {
			m_stcFileContent->ToggleFold(lineClick);
		}
	}
	
	// break points
	if ( event.GetMargin() == MARGIN_BREAKPOINT || event.GetMargin() == MARGIN_LINE_NUMBERS ) {
		int lineClick = m_stcFileContent->LineFromPosition(event.GetPosition());

		if ( m_stcFileContent->MarginGetText(lineClick) == "B" ) {
			m_stcFileContent->MarginSetText(lineClick, wxT(" "));
			m_stcFileContent->MarginSetStyle(lineClick, TE_DEFAULT_STYLE);
		} else {
			m_stcFileContent->MarginSetText(lineClick, wxT("B"));
			m_stcFileContent->MarginSetStyle(lineClick, TE_BREAKPOINT_STYLE);
		}
	}
	
}
///////////////////////////////////////////////////////////////////
void MainFrame::marginClickEmuSource(wxStyledTextEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( event.GetMargin() == MARGIN_FOLD ) {
		int lineClick = m_stcEmuSource->LineFromPosition(event.GetPosition());
		int levelClick = m_stcEmuSource->GetFoldLevel(lineClick);
		
		if ( (levelClick & wxSTC_FOLDLEVELHEADERFLAG ) > 0) {
			m_stcEmuSource->ToggleFold(lineClick);
		}
	}
}
///////////////////////////////////////////////////////////////////
const char* MainFrame::getBlankHtmlPage() {
///////////////////////////////////////////////////////////////////
	wxFileName fn(CncFileNameService::getBlankHtmlPageFileName());
	fstream html;
	
	html.open(fn.GetFullPath(), std::ios::out | std::ios::trunc);
	if ( html.is_open() ) {
		
		html << "<HTML>" << std::endl;
		html << "<HEAD>" << std::endl;
		html << "<TITLE>Blank Page</TITLE>" << std::endl;
		html << "</HEAD>" << std::endl;
		html << "<BODY BGCOLOR=\"FFFFFF\">" << std::endl;
		html << "<HR>" << std::endl;
		html << "Default HTML Page" << std::endl;
		html << "<H1>Empty Content (about:blank)</H1>" << std::endl;
		html << "<H2>No further information available</H2>" << std::endl;
		html << "<HR>" << std::endl;
		html << "</BODY>" << std::endl;
		html << "</HTML>" << std::endl;
		
		html.flush();
		html.close();
		
	} else {
		return "about:blank";
	}

	return fn.GetFullPath();
}
///////////////////////////////////////////////////////////////////
const char* MainFrame::getErrorHtmlPage(const wxString& errorInfo) {
///////////////////////////////////////////////////////////////////
	wxFileName fn(CncFileNameService::getErrorHtmlPageFileName());
	fstream html;
	
	wxString ei(errorInfo);
	if ( errorInfo.IsEmpty() )
		ei = "No further information available";
	
	html.open(fn.GetFullPath(), std::ios::out | std::ios::trunc);
	if ( html.is_open() ) {
		
		html << "<HTML>" << std::endl;
		html << "<HEAD>" << std::endl;
		html << "<TITLE>Error Page</TITLE>" << std::endl;
		html << "</HEAD>" << std::endl;
		html << "<BODY BGCOLOR=\"FFFFFF\">" << std::endl;
		html << "<HR>" << std::endl;
		html << "Default HTML Page" << std::endl;
		html << "<H1>";
		html << ei;
		html << "/H1>" << std::endl;
		html << "<H2>No further information availiable</H2>" << std::endl;
		html << "<HR>" << std::endl;
		html << "</BODY>" << std::endl;
		html << "</HTML>" << std::endl;
		
		html.flush();
		html.close();
		
	} else {
		return "about:blank";
	}

	return fn.GetFullPath();
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
		m_svgParseMode->SetForegroundColour(wxColor(128,128,255));
	}

	decorateRunButton();
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcDebugConfig(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	ensureDebugPerspectiveMinimal();
	
	if ( m_debuggerPropertyManagerGrid->GetPageCount() > 0 )
		m_debuggerPropertyManagerGrid->SelectPage(0);
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcRun(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( event.GetEventObject() == m_rcRun )	isDebugMode = false;
	else										isDebugMode = true;
	determineRunMode();
	
	// toggle only the pause flag
	if ( isPause() == true ) {
		rcPause(event);
		return;
	}
	
	// perform a run
	// Store the current interval
	int interval = CncConfig::getGlobalCncConfig()->getUpdateInterval();
	
	if ( isDebugMode == true ) {
		
		// check if the cuurent port is a cnc and no emulator port
		if ( cnc->getSerial()->getPortType() == CncPORT ) {
			
			wxString msg("Do you realy want to debug a COM port?");
			wxMessageDialog dlg(this, msg, _T("Port Check . . . "), 
			                    wxOK|wxCANCEL|wxCENTRE|wxICON_QUESTION);
			
			if ( dlg.ShowModal() == wxID_CANCEL )
				return;
		}
		
		// bring the debug controls in front
		ensureDebugPerspectiveMinimal();
		
		// to see each line during the debug session
		CncConfig::getGlobalCncConfig()->setUpdateInterval(1);
	} else {
		
		ensureRunPerspectiveMinimal();
	}

	// process
	processTemplate();
	
	// restore the interval
	CncConfig::getGlobalCncConfig()->setUpdateInterval(interval);
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcPause(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( isPause() )	cnc->sendResume();
	else				cnc->sendPause();
	
	if ( inboundFileParser != NULL ) {
		inboundFileParser->togglePause();
		enableRunControls(inboundFileParser->isPause());
	}
	
	decorateRunButton();
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
	wxASSERT(cnc);
	
	// toggle only the pause flag
	if ( isPause() == true )
		rcPause(event);
	
	if ( inboundFileParser != NULL )
		inboundFileParser->debugStop();

	cnc->sendHalt();
	cnc::trc.logInfo("Current session is stopped");
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcReset(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	requestReset();
	setRefPostionState(false);
}
///////////////////////////////////////////////////////////////////
void MainFrame::testSwitchToolOnOff(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	if ( cnc->isConnected() == false ) {
		std::cerr << "Not connetced, nothing will be processed." << std::endl;
		return;
	}
	
	if ( m_testToggleTool->GetValue() == true ) {
		m_testToggleTool->SetLabel("Switch Tool Off");
		m_testToggleTool->SetBackgroundColour(*wxRED);
		m_testToggleTool->SetForegroundColour(*wxWHITE);
		
		disableControls();
		m_testToggleEndSwitch->Enable(false);
		m_testToggleTool->Enable(true);
		
		cnc->switchToolOn();
		startAnimationControl();
		
	} else {
		m_testToggleTool->SetLabel("Switch Tool On");
		m_testToggleTool->SetBackgroundColour(*wxGREEN);
		m_testToggleTool->SetForegroundColour(*wxBLACK);
		
		enableControls();
		
		cnc->switchToolOff();
		stopAnimationControl();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::testEndSwitchEvaluation(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	if ( cnc->isConnected() == false ) {
		std::cerr << "Not connetced, nothing will be processed." << std::endl;
		return;
	}
	
	if ( m_testToggleEndSwitch->GetValue() == true ) {
		m_testToggleEndSwitch->SetLabel("Stop End Switch Evaluation");
		m_testToggleEndSwitch->SetBackgroundColour(*wxRED);
		m_testToggleEndSwitch->SetForegroundColour(*wxWHITE);
		
		disableControls();
		m_testToggleEndSwitch->Enable(true);
		
		startAnimationControl();
		while ( m_testToggleEndSwitch->GetValue() == true ) {
			dispatchAll();
			
			if ( cnc->isInterrupted() ) {
				m_testToggleEndSwitch->SetValue(false);
				break;
			}
			
			cnc->evaluateLimitState();
		}
		
		enableControls();
		stopAnimationControl();
		
	} 
	
	if ( m_testToggleEndSwitch->GetValue() == false ) {
		m_testToggleEndSwitch->SetLabel("Start End Switch Evaluation");
		m_testToggleEndSwitch->SetBackgroundColour(*wxGREEN);
		m_testToggleEndSwitch->SetForegroundColour(*wxBLACK);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::activate3DPerspectiveButton(wxButton* bt) {
///////////////////////////////////////////////////////////////////
	static wxColour active(171, 171, 171);
	static wxColour inactive(240, 240, 240);

	m_3D_Top->SetBackgroundColour(inactive);
	m_3D_Bottom->SetBackgroundColour(inactive);
	m_3D_Front->SetBackgroundColour(inactive);
	m_3D_Rear->SetBackgroundColour(inactive);
	m_3D_Left->SetBackgroundColour(inactive);
	m_3D_Right->SetBackgroundColour(inactive);
	m_3D_Perspective1->SetBackgroundColour(inactive);
	m_3D_Perspective2->SetBackgroundColour(inactive);
	m_3D_Perspective3->SetBackgroundColour(inactive);
	m_3D_Perspective4->SetBackgroundColour(inactive);
	
	if ( bt != NULL ) {
		bt->SetBackgroundColour(active);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::showFromFront3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	activate3DPerspectiveButton((wxButton*)event.GetEventObject());
	motionMonitor->viewFront();
}
///////////////////////////////////////////////////////////////////
void MainFrame::showFromRear3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	activate3DPerspectiveButton((wxButton*)event.GetEventObject());
	motionMonitor->viewRear();
}
///////////////////////////////////////////////////////////////////
void MainFrame::showFromTop3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	activate3DPerspectiveButton((wxButton*)event.GetEventObject());
	motionMonitor->viewTop();
}
///////////////////////////////////////////////////////////////////
void MainFrame::showFromBottom3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	activate3DPerspectiveButton((wxButton*)event.GetEventObject());
	motionMonitor->viewBottom();
}
///////////////////////////////////////////////////////////////////
void MainFrame::showFromLeft3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	activate3DPerspectiveButton((wxButton*)event.GetEventObject());
	motionMonitor->viewLeft();
}
///////////////////////////////////////////////////////////////////
void MainFrame::showFromRight3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	activate3DPerspectiveButton((wxButton*)event.GetEventObject());
	motionMonitor->viewRight();
}
///////////////////////////////////////////////////////////////////
void MainFrame::show3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxButton* bt = (wxButton*)event.GetEventObject();

	if ( bt == m_3D_Perspective1 ) {
		activate3DPerspectiveButton(bt);
		motionMonitor->viewIso1();
		
	} else if ( bt == m_3D_Perspective2 ) {
		activate3DPerspectiveButton(bt);
		motionMonitor->viewIso2();
		
	} else if ( bt == m_3D_Perspective3 ) {
		activate3DPerspectiveButton(bt);
		motionMonitor->viewIso3();
		
	} else if ( bt == m_3D_Perspective4 ) {
		activate3DPerspectiveButton(bt);
		motionMonitor->viewIso4();
		
	} 
}
///////////////////////////////////////////////////////////////////
void MainFrame::runOpenGLTest(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	//todo
	//motionMonitor->runOpenGLTest();
}
///////////////////////////////////////////////////////////////////
void MainFrame::trace3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	std::clog << "Motion Monitor Data - ";
	
	std::stringstream ss;
	motionMonitor->tracePathData(ss);
	
	m_logger->Freeze();
	m_logger->AppendText(ss.str().c_str());
	m_logger->Thaw();
}
///////////////////////////////////////////////////////////////////
void MainFrame::clear3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	motionMonitor->clear();
}
///////////////////////////////////////////////////////////////////
void MainFrame::refresh3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( cnc )
		cnc->updatePreview3D();
}
///////////////////////////////////////////////////////////////////
void MainFrame::openMotionMonitorOptionDlg(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	motionMonitor->showOptionDialog();
}
///////////////////////////////////////////////////////////////////
void MainFrame::enableTestParameter(bool state) {
///////////////////////////////////////////////////////////////////
	long id = 0;
	m_ctrlTestSelectedId->GetValue().ToLong(&id);
	int count = CncControllerTestSuite::getParameterCount(id);
	
 	m_ctrlTestParam1->Enable(false);
	m_ctrlTestParam2->Enable(false);
	m_ctrlTestParam3->Enable(false);
	m_ctrlTestParam4->Enable(false);
	m_ctrlTestParam5->Enable(false);
					
	switch ( count ) {
		case 1: 	m_ctrlTestParam1->Enable(state);
					break;
					
		case 2: 	m_ctrlTestParam1->Enable(state);
					m_ctrlTestParam2->Enable(state);
					break;
					
		case 3: 	m_ctrlTestParam1->Enable(state);
					m_ctrlTestParam2->Enable(state);
					m_ctrlTestParam3->Enable(state);
					break;
					
		case 4: 	m_ctrlTestParam1->Enable(state);
					m_ctrlTestParam2->Enable(state);
					m_ctrlTestParam3->Enable(state);
					m_ctrlTestParam4->Enable(state);
					break;
					
		case 5: 	m_ctrlTestParam1->Enable(state);
					m_ctrlTestParam2->Enable(state);
					m_ctrlTestParam3->Enable(state);
					m_ctrlTestParam4->Enable(state);
					m_ctrlTestParam5->Enable(state);
					break;
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::decorateTestSuiteParameters() {
///////////////////////////////////////////////////////////////////
	int sel = m_ctrlTestSelection->GetSelection();
	wxString item = m_ctrlTestSelection->GetString(sel);
	
	m_ctrlTestParam1->Enable(false);
	m_ctrlTestParam2->Enable(false);
	m_ctrlTestParam3->Enable(false);
	m_ctrlTestParam4->Enable(false);
	m_ctrlTestParam5->Enable(false);
	m_ctrlTestParam1->SetValue("");
	m_ctrlTestParam2->SetValue("");
	m_ctrlTestParam3->SetValue("");
	m_ctrlTestParam4->SetValue("");
	m_ctrlTestParam5->SetValue("");
	
	m_ctrlTestDescription->SetValue("");

	if ( item.IsEmpty() == true )
		return;
	
	long id = 0;
	item.BeforeFirst(':').ToLong(&id);
	
	if ( id <= 0 || id > 100 )
		return;
		
	m_ctrlTestSelectedId->SetValue(wxString::Format("%d", id));
	m_ctrlTestDescription->SetValue(CncControllerTestSuite::getTestCaseDescrption(id));
	
	cnc->activatePositionCheck(false);
	
	int count = CncControllerTestSuite::getParameterCount(id);
	switch ( count ) {
		case 1:		m_ctrlTestParam1->Enable(true);
					m_ctrlTestParam1->SetValue(wxString::Format("%d", CncControllerTestSuite::getDefaultValue1(id)));
					break;
					
		case 2:		m_ctrlTestParam1->Enable(true);
					m_ctrlTestParam2->Enable(true);
					m_ctrlTestParam1->SetValue(wxString::Format("%d", CncControllerTestSuite::getDefaultValue1(id)));
					m_ctrlTestParam2->SetValue(wxString::Format("%d", CncControllerTestSuite::getDefaultValue2(id)));
					break;
					
		case 3:		m_ctrlTestParam1->Enable(true);
					m_ctrlTestParam2->Enable(true); 
					m_ctrlTestParam3->Enable(true);
					m_ctrlTestParam1->SetValue(wxString::Format("%d", CncControllerTestSuite::getDefaultValue1(id)));
					m_ctrlTestParam2->SetValue(wxString::Format("%d", CncControllerTestSuite::getDefaultValue2(id)));
					m_ctrlTestParam3->SetValue(wxString::Format("%d", CncControllerTestSuite::getDefaultValue3(id)));
					break;
					
		case 4:		m_ctrlTestParam1->Enable(true);
					m_ctrlTestParam2->Enable(true); 
					m_ctrlTestParam3->Enable(true); 
					m_ctrlTestParam4->Enable(true); 
					m_ctrlTestParam1->SetValue(wxString::Format("%d", CncControllerTestSuite::getDefaultValue1(id)));
					m_ctrlTestParam2->SetValue(wxString::Format("%d", CncControllerTestSuite::getDefaultValue2(id)));
					m_ctrlTestParam3->SetValue(wxString::Format("%d", CncControllerTestSuite::getDefaultValue3(id)));
					m_ctrlTestParam4->SetValue(wxString::Format("%d", CncControllerTestSuite::getDefaultValue4(id)));
					break;
					
		case 5:		m_ctrlTestParam1->Enable(true);
					m_ctrlTestParam2->Enable(true); 
					m_ctrlTestParam3->Enable(true); 
					m_ctrlTestParam4->Enable(true); 
					m_ctrlTestParam5->Enable(true); 
					m_ctrlTestParam1->SetValue(wxString::Format("%d", CncControllerTestSuite::getDefaultValue1(id)));
					m_ctrlTestParam2->SetValue(wxString::Format("%d", CncControllerTestSuite::getDefaultValue2(id)));
					m_ctrlTestParam3->SetValue(wxString::Format("%d", CncControllerTestSuite::getDefaultValue3(id)));
					m_ctrlTestParam4->SetValue(wxString::Format("%d", CncControllerTestSuite::getDefaultValue4(id)));
					m_ctrlTestParam5->SetValue(wxString::Format("%d", CncControllerTestSuite::getDefaultValue5(id)));
					break;
					
		default:	; // do nothing
	}
	
	cnc->activatePositionCheck(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::treebookTestChanged(wxTreebookEvent& event) {
///////////////////////////////////////////////////////////////////
	decorateTestSuiteParameters();
}
///////////////////////////////////////////////////////////////////
void MainFrame::ctrlTestCaseSelected(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	decorateTestSuiteParameters();
}
///////////////////////////////////////////////////////////////////
void MainFrame::traceTextUpdated(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	traceTimerCounter = 0;
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestControllerPinsFromButton(wxCommandEvent& event) {
	requestPins(event);
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestPins(wxCommandEvent& event) {
	wxASSERT(cnc);
	cnc->processCommand("Q", std::clog);
	m_outboundNotebook->SetSelection(OutboundSelection::VAL::SUMMARY_PANEL);
	m_notebookConfig->SetSelection(OutboundCfgSelection::VAL::CNC_PIN_PANEL);
}
///////////////////////////////////////////////////////////////////
void MainFrame::clearControllerMsgHistory(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_controllerMsgHistory->Clear();
}
///////////////////////////////////////////////////////////////////
void MainFrame::fileContentChange(wxStyledTextEvent& event) {
///////////////////////////////////////////////////////////////////
	// todo currently not tested
	event.Skip();
	
	bool isInsert = event.GetModificationType() & wxSTC_MOD_INSERTTEXT;
	bool isDelete = event.GetModificationType() & wxSTC_MOD_DELETETEXT;
	
	if ( isInsert || isDelete) {
		int numlines(event.GetLinesAdded());
		
		// ignore this event incase we are in the middle of file reloading
		if( templateFileLoading == false/*GetReloadingFile() == false && GetMarginWidth(EDIT_TRACKER_MARGIN_ID */) /* margin is visible */ {
			int curline(m_stcFileContent->LineFromPosition(event.GetPosition()));

			if(numlines == 0) {
				// probably only the current line was modified
				m_stcFileContent->MarginSetText(curline, wxT(" "));
				m_stcFileContent->MarginSetStyle(curline, TE_LINE_MODIFIED_STYLE);
			} else {
				for(int i = 0; i <= numlines; i++) {
					m_stcFileContent->MarginSetText(curline + i, wxT(" "));
					m_stcFileContent->MarginSetStyle(curline + i, TE_LINE_MODIFIED_STYLE);
				}
			}
			 
		}
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::clearSerialSpy() {
///////////////////////////////////////////////////////////////////
	serialSpy->Clear();
}
///////////////////////////////////////////////////////////////////
void MainFrame::clearSerialSpy(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	clearSerialSpy();
}
///////////////////////////////////////////////////////////////////
void MainFrame::markSerialSpy(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString defaultValue(wxString::Format("Marker::%06d", serialSpy->GetNumberOfLines()));
	wxTextEntryDialog dlg(this, "Marker Label:", "Add Spy Marker . . .", defaultValue);
	dlg.SetMaxLength(64);
	dlg.ShowModal();
	serialSpy->addMarker(dlg.GetValue());
}
///////////////////////////////////////////////////////////////////
void MainFrame::freezeLogger(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_logger->IsFrozen() ) {
		m_freezeLogger->SetBitmap(ImageLib16().Bitmap("BMP_NOT_FROZEN")); 
		m_freezeLogger->SetToolTip("Freeze Logger");
		m_logger->Thaw();
	} else {
		m_freezeLogger->SetBitmap(ImageLib16().Bitmap("BMP_FROZEN")); 
		m_freezeLogger->SetToolTip("Unfreeze Logger");
		m_logger->Freeze();
	}
	
	m_freezeLogger->Refresh();
	m_freezeLogger->Update();
}
///////////////////////////////////////////////////////////////////
void MainFrame::enableSerialSpy(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_menuItemDebugSerial->Check(!m_menuItemDebugSerial->IsChecked());
	
	updateMonitoring();
	decorateSerialSpy();
}
///////////////////////////////////////////////////////////////////
void MainFrame::enableSerialSpy(bool state) {
///////////////////////////////////////////////////////////////////
	m_menuItemDebugSerial->Check(state);
	updateMonitoring();
	decorateSerialSpy();
}
///////////////////////////////////////////////////////////////////
void MainFrame::decorateSerialSpy() {
///////////////////////////////////////////////////////////////////
	if ( m_menuItemDebugSerial->IsChecked() ) {
		m_enableSerialSpy->SetBitmap(ImageLib16().Bitmap("BMP_CONNECTED")); 
		m_enableSerialSpy->SetToolTip("Disable Serial Spy");
		cnc::spy.enableMessage(); 
	} else {
		m_enableSerialSpy->SetBitmap(ImageLib16().Bitmap("BMP_DISCONNECTED")); 
		m_enableSerialSpy->SetToolTip("Enable Serial Spy");
		cnc::spy.disableMessage();
	}
	
	m_enableSerialSpy->Refresh();
	m_enableSerialSpy->Update();
}
///////////////////////////////////////////////////////////////////
void MainFrame::cfgStepDelayArduino(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_stepDelay->SetValue(int(m_stepDelay->GetMax() * 0.4));
	updateStepDelay();
}
///////////////////////////////////////////////////////////////////
void MainFrame::cfgStepDelayMax(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_stepDelay->SetValue(m_stepDelay->GetMax());
	updateStepDelay();
}
///////////////////////////////////////////////////////////////////
void MainFrame::cfgStepDelayMin(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_stepDelay->SetValue(0);
	updateStepDelay();
}
///////////////////////////////////////////////////////////////////
void MainFrame::cfgStepDelayDropDown(wxAuiToolBarEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_miCfgSimulateArduino->IsChecked() )
		cfgStepDelayArduino(event);
	
	if ( m_miCfgStepDelayMin->IsChecked() )
		cfgStepDelayMin(event);
		
	if ( m_miCfgStepDelayMax->IsChecked() )
		cfgStepDelayMax(event);

	event.Skip();
}
///////////////////////////////////////////////////////////////////
void MainFrame::UpdateLogger(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_showLoggerOnDemand->IsChecked() == false )
		return;
	
	if ( m_logger->IsShownOnScreen() == false )
		showAuiPane("Logger");
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
void MainFrame::openPathGen() {
///////////////////////////////////////////////////////////////////
	wxASSERT(pathGenerator);
	if ( pathGenerator->IsShown() == false ) 
		pathGenerator->Show();
}
///////////////////////////////////////////////////////////////////
void MainFrame::openPathGenWithCurrentSvgNodeFromPopup(wxStyledTextCtrl* ctl, const wxString& node) {
///////////////////////////////////////////////////////////////////
	if ( pathGenerator == NULL )
		return;
	
	wxXmlDocument xmlDoc;
	wxString errorInfo;
	
	// check
	if ( verifyPathGenertorNode(xmlDoc, node, errorInfo) == false ) {
		displayPGenErrorInfo(errorInfo);
		return;
	}
	
	wxXmlNode* root = xmlDoc.GetRoot();
	wxASSERT(root);
	
	// setup
	wxASSERT(pathGenerator);
	if ( pathGenerator->IsShown() == false ) 
		pathGenerator->Show();
	
	//process
	wxASSERT(cnc);
	PathGeneratorStore::RegenerateParameter rp;
	rp.in.editControl  = ctl;
	rp.in.toolDiameter = CncConfig::getGlobalCncConfig()->getToolDiameter();
	rp.in.cncPattern.assign(root->GetAttribute(CncPatternRootName, ""));
	
	if ( pathGenerator->regenerateSvgBlock(rp) == false ) {
		displayPGenErrorInfo(rp.out.errorInfo);
		return;
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::regenerateCurrentSvgNodeFromPopup(wxStyledTextCtrl* ctl, const wxString& node) {
///////////////////////////////////////////////////////////////////
	if ( ctl == NULL )
		return;
		
	wxXmlDocument xmlDoc;
	wxString errorInfo;
	
	// check
	if ( verifyPathGenertorNode(xmlDoc, node, errorInfo) == false ) {
		displayPGenErrorInfo(errorInfo);
		return;
	}
	
	wxXmlNode* root = xmlDoc.GetRoot();
	wxASSERT(root);
	
	// setup
	wxASSERT(cnc);
	PathGeneratorStore::RegenerateParameter rp;
	rp.in.editControl  = ctl;
	rp.in.toolDiameter =CncConfig::getGlobalCncConfig()->getToolDiameter();
	rp.in.cncPattern.assign(root->GetAttribute(CncPatternRootName, ""));
	
	// process
	PathGeneratorStore store;
	if ( store.regenerateSvgBlock(rp) == false ) {
		displayPGenErrorInfo(rp.out.errorInfo);
		return;
	}
	
	// Replace select text
	ctl->ReplaceSelection(rp.out.resultigSvgFragment);
}
///////////////////////////////////////////////////////////////////
bool MainFrame::verifyPathGenertorNode(wxXmlDocument& xmlDoc, const wxString& node, wxString& errorInfo) {
///////////////////////////////////////////////////////////////////
	if ( node.IsEmpty() == true ) {
		errorInfo << "MainFrame::regenerateCurrentSvgNodeFromPopup: Empty node received, nothing will be done!\n";
		return false;
	}
	
	if ( node.Find(CncPatternRootName) == wxNOT_FOUND ) {
		errorInfo << "MainFrame::regenerateCurrentSvgNodeFromPopup: Current SVG block didn't contain a CncPattern, nothing will be done!\n";
		return false;
	}
	
	wxStringInputStream xmlStream(node);
	if ( xmlDoc.Load(xmlStream) == false ) {
		errorInfo << "MainFrame::regenerateCurrentSvgNodeFromPopup: Cant create an XML document from received node!\n";
		errorInfo << "Please check the selection.\n";
		errorInfo << "Nothing will be done!\n";
		return false;
	}
	
	wxXmlNode* root = xmlDoc.GetRoot();
	if ( root == NULL ) {
		errorInfo << "MainFrame::regenerateCurrentSvgNodeFromPopup: Cant evaluate a root node.\n";
		errorInfo << "Please check the selection.\n";
		errorInfo << "Nothing will be done!\n";
		return false;
	}
	
	if ( root->GetName() != "g" ) {
		errorInfo << "MainFrame::regenerateCurrentSvgNodeFromPopup: Selected svg fragment didn't start with a <g> element.\n";
		errorInfo << "Please check the selection.\n";
		errorInfo << "Nothing will be done!\n";
		return false;
	}
	
	if ( root->HasAttribute(CncPatternRootName) == false ) {
		errorInfo << "MainFrame::regenerateCurrentSvgNodeFromPopup: Current <g> element didn't contain a " << CncPatternRootName << ".\n";
		errorInfo << "Please check the selection.\n";
		errorInfo << "Nothing will be done!\n";
		return false;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
void MainFrame::decodeSvgFragment(wxMouseEvent& event, wxStyledTextCtrl* ctl) {
///////////////////////////////////////////////////////////////////
	if ( event.CmdDown() == false ) {
		event.Skip(true);
		return;
	}
	
	if ( ctl == NULL ) {
		event.Skip(true);
		return;
	}

	int prevCp = ctl->GetCurrentPos();
	int curPos = prevCp;
	
	// find left numeric fence
	char c = (char)ctl->GetCharAt(curPos);
	while ( isdigit(c) != 0 || c == '.' || c == ',' || c == ' ' || c == '+' || c == '-' )
		c = (char)ctl->GetCharAt(curPos--);
	
	int start = curPos + 2;
	curPos = prevCp;
	
	// find right numeric fence
	c = (char)ctl->GetCharAt(curPos--);
	while ( isdigit(c) != 0 || c == '.' || c == ',' || c == ' ' || c == '+' || c == '-' )
		c = (char)ctl->GetCharAt(curPos++);
	
	int end = curPos - 1;
	
	// check is something numeric is found
	if ( start >= end ) {
		event.Skip(true);
		return;
	}
		
	// process
	event.Skip(false);
	ctl->SetSelection(start, end);
	m_cbUCValueFrom->SetValue(ctl->GetTextRange(start, end));
}
///////////////////////////////////////////////////////////////////
void MainFrame::fileContentDClick(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	decodeSvgFragment(event, m_stcFileContent);
}
///////////////////////////////////////////////////////////////////
void MainFrame::emuContentDClick(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	decodeSvgFragment(event, m_stcEmuSource);
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
	SVGUnit from 	= SvgUnitCalculator::determineUnit(m_cbUCUnitFrom->GetStringSelection());
	SVGUnit to		= SvgUnitCalculator::determineUnit(m_cbUCUnitTo->GetStringSelection());
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
		
		vTo = SvgUnitCalculator::convertUnit2Unit(from, to, vFrom);
		
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
/////////////////////////////////////////////////////////////////////
void MainFrame::onDebugUserNotificationTimer(wxTimerEvent& event) {
/////////////////////////////////////////////////////////////////////
	const wxColour c1(227, 227, 227);
	const wxColour c2(255, 201, 14);
	
	if ( inboundFileParser ) {
		if ( inboundFileParser->isWaitingForUserEvents() == false ) {
			if ( m_rcNextBreakpoint->GetBackgroundColour() != c1 )	m_rcNextBreakpoint->SetBackgroundColour(c1);
			if ( m_rcNextStep->GetBackgroundColour() != c1 )		m_rcNextStep->SetBackgroundColour(c1);
			if ( m_rcFinish->GetBackgroundColour() != c1 )			m_rcFinish->SetBackgroundColour(c1);
			
			return;
		}
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
	
	if ( debugUserNotificationTime.IsRunning() == false )
		debugUserNotificationTime.Start(1000);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::stopDebugUserNotification() {
/////////////////////////////////////////////////////////////////////
	const wxColour c(227, 227, 227);
	m_rcNextBreakpoint->SetBackgroundColour(c);
	m_rcNextStep->SetBackgroundColour(c);
	m_rcFinish->SetBackgroundColour(c);
	
	if ( debugUserNotificationTime.IsRunning() == true )
		debugUserNotificationTime.Stop();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::clearPositionSpy() {
/////////////////////////////////////////////////////////////////////
	typedef UpdateManagerThread::Event Event;
	static Event evt;
	umPostEvent(evt.PosSpyResetEvent());
	
	m_positionSpy->Clear();
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
void MainFrame::ensureRunPerspectiveMinimal() {
/////////////////////////////////////////////////////////////////////
	if ( m_scrollWinMonitor->IsShown() == false )
		ensureAllPanesFromPerspectiveAreShown("Run");
}
/////////////////////////////////////////////////////////////////////
void MainFrame::ensureDebugPerspectiveMinimal() {
/////////////////////////////////////////////////////////////////////
	if ( m_scrollWinMonitor->IsShown() == false || m_debuggerView->IsShown() == false )
		ensureAllPanesFromPerspectiveAreShown("Debug");
}
/////////////////////////////////////////////////////////////////////
void MainFrame::ensureAllPanesFromPerspectiveAreShown(const wxString& name) {
/////////////////////////////////////////////////////////////////////
	wxString id(wxString::Format("Perspectives/%s.PaneList", name));
	wxString paneList;
	
	config->Read(id, &paneList, "");
	if ( paneList.IsEmpty() == false ) {
		
		wxStringTokenizer tokenizer(paneList, "|");
		while ( tokenizer.HasMoreTokens() ) {
			wxString name = tokenizer.GetNextToken();
			
			name.Trim(true).Trim(false);
			if ( name.IsEmpty() == false ) {
				showAuiPane(name, false);
			}
		}
		
		decorateViewMenu();
		GetAuimgrMain()->Update();
	}
}
/////////////////////////////////////////////////////////////////////
bool MainFrame::loadPerspective(const wxString& name) {
/////////////////////////////////////////////////////////////////////
	wxString id(wxString::Format("Perspectives/%s", name));
	wxString perspective;
	
	config->Read(id, &perspective, "");
	bool ret = (perspective.IsEmpty() == false);
	
	if ( ret == false )	viewAllAuiPanes();
	else				m_auimgrMain->LoadPerspective(perspective);
	
	decorateViewMenu();
	
	return ret;
}
/////////////////////////////////////////////////////////////////////
void MainFrame::savePerspective(const wxString& name) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(config);
	
	wxString msg(wxString::Format("Do you really want to update the '%s' perspective?", name));
	wxMessageDialog dlg(this, msg, _T("Perspective save. . . "), 
	                    wxOK|wxCANCEL|wxCENTRE|wxICON_QUESTION);
	
	if ( dlg.ShowModal() == wxID_OK ) {
		wxString id(wxString::Format("Perspectives/%s", name));
		config->Write(id, m_auimgrMain->SavePerspective());
		
		// additional store a list of shown panes
		wxAuiPaneInfoArray panes = m_auimgrMain->GetAllPanes();
		wxString list;
		for (unsigned int i = 0; i < panes.GetCount(); ++i) {
			if ( panes.Item(i).window->IsShown() )
				list.append(wxString::Format("%s|", panes.Item(i).name));
		}
		config->Write(wxString::Format("%s.PaneList", id), list);
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::loadPerspectiveRun(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	loadPerspective("Run");
}
/////////////////////////////////////////////////////////////////////
void MainFrame::loadPerspectiveDebug(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	loadPerspective("Debug");
}
/////////////////////////////////////////////////////////////////////
void MainFrame::loadPerspectiveSource(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	loadPerspective("Source");
}
/////////////////////////////////////////////////////////////////////
void MainFrame::savePerspectiveRun(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	savePerspective("Run");
}
/////////////////////////////////////////////////////////////////////
void MainFrame::savePerspectiveDebug(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	savePerspective("Debug");
}
/////////////////////////////////////////////////////////////////////
void MainFrame::savePerspectiveSource(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	savePerspective("Source");
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
	wxASSERT(CncConfig::getGlobalCncConfig());
	CncConfig::getGlobalCncConfig()->setupGridChanged(event);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::setupGridChanging(wxPropertyGridEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(CncConfig::getGlobalCncConfig());
	CncConfig::getGlobalCncConfig()->setupGridChanging(event);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::setupGridCommandButton(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(CncConfig::getGlobalCncConfig());
	CncConfig::getGlobalCncConfig()->setupGridCommandButton(event);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::setupGridSelected(wxPropertyGridEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(CncConfig::getGlobalCncConfig());
	CncConfig::getGlobalCncConfig()->setupGridSelected(event);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::saveConfiguration(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(CncConfig::getGlobalCncConfig());
	CncConfig::getGlobalCncConfig()->saveConfiguration(*config);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::loadConfiguration(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(CncConfig::getGlobalCncConfig());
	CncConfig::getGlobalCncConfig()->loadConfiguration(*config);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::copyPositionSpy(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( m_positionSpy->GetCount() == 0 )
		return;

	// Write some text to the clipboard
	if ( wxTheClipboard->Open() ) {
		startAnimationControl();

		wxString content;
		content.reserve(64 * 10000);
		for ( unsigned int i=0; i<m_positionSpy->GetCount(); i++ ) {
			content.append(wxString::Format("%s\n", m_positionSpy->GetString(i)));
		}
		
		// This data objects are held by the clipboard,
		// so do not delete them in the app.
		wxTheClipboard->SetData( new wxTextDataObject(content) );
		wxTheClipboard->Close();
		
		stopAnimationControl();
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::togglePositionSpy(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxBitmap bmpOn  = ImageLibPosSpy().Bitmap("BMP_POS_SPY_ON");
	wxBitmap bmpOff = ImageLibPosSpy().Bitmap("BMP_POS_SPY_OFF");
	
	m_btTogglePosSpy->GetValue() ? m_btTogglePosSpy->SetBitmap(bmpOn) : m_btTogglePosSpy->SetBitmap(bmpOff);
	m_btTogglePosSpy->Refresh();
	m_btTogglePosSpy->Update();
	
	if ( m_btTogglePosSpy->GetValue() == false )
		clearPositionSpy();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::selectPositionSpy(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	int item = m_positionSpy->GetSelection();
	if ( item == wxNOT_FOUND )
		return;
	
	wxString content(m_positionSpy->GetString(item));
	wxString lineNumber(content.BeforeFirst(' '));
	
	long ln =0L;
	lineNumber.ToLong(&ln);
	ln -= 1L;
	
	if ( ln <= 0L )
		return;
	
	// select source control - try to find ref as line number
	if ( inboundFileParser != NULL ) {
		inboundFileParser->selectSourceControl(ln);
	} else {
		// select editor directly
		m_stcFileContent->GotoLine(ln);
		
		if ( ln == 0 ) {
			m_stcFileContent->SetSelectionStart(0);
			m_stcFileContent->SetSelectionEnd(0);
		} else {
			m_stcFileContent->SetSelectionStart(m_stcFileContent->GetCurrentPos());
			m_stcFileContent->SetSelectionEnd(m_stcFileContent->GetLineEndPosition(ln));
		}
	}
	
	// mark this id
	if ( motionMonitor != NULL ) {
		motionMonitor->setCurrentClientId(ln + 1);
		motionMonitor->display();
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::selectPositionSpyContent(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	clearPositionSpy();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::loopRepeatTest(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxTextEntryDialog dlg(this, "Loop Repeat Test:", "Loop count . . .", "");
	dlg.SetMaxLength(8);
	dlg.SetTextValidator(wxFILTER_DIGITS);
	
	unsigned int loopCount = 0;
	if ( dlg.ShowModal() == wxID_OK  ) {
		wxString s = dlg.GetValue();
		s.Trim(true).Trim(false);
		
		if ( s.IsEmpty() == false ) {
			long num;
			s.ToLong(&num);
			if ( num > 0 && num < 10000 )
				loopCount = (unsigned int)num;
		}
	}
	
	if ( loopCount == 0 )
		return;
		
	wxString title(GetTitle());
	wxString info;
	long duration = 0;

	// loop
	for ( unsigned int i=0; i<loopCount; i++) {
		
		bool ret = processTemplate();
		duration += processLastDuartion;

		info.Printf("Loop Counter : % 6d [#]; AVG duration: % 10ld [ms]", i + 1, duration / ( i + 1 ));
		SetTitle(wxString::Format("%s         [%s]", title, info));
		cnc::trc.logInfoMessage(info);

		if ( ret == false )
			break;
			
		waitActive(5);
	}
	
	// sumary
	cnc::trc.logInfoMessage("");
	std::clog << wxString::Format("Loop Repeat Test Sumary: Count: % 6d [#]; AVG Duration: % 10ld [ms]", loopCount, duration / loopCount) << std::endl;
	SetTitle(title);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::clearSetterList(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	typedef UpdateManagerThread::Event Event;
	static Event evt;
	umPostEvent(evt.SetterListResetEvent());
	
	m_setterList->DeleteAllItems();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::sizeSetterList(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( m_setterList->IsFrozen() == false )
		m_setterList->Freeze();
		
	// first set default sizes depending on content
	m_setterList->SetColumnWidth(UpdateManagerThread::UMT_SETLST_NUM, wxLIST_AUTOSIZE);
	m_setterList->SetColumnWidth(UpdateManagerThread::UMT_SETLST_KEY, wxLIST_AUTOSIZE);
	m_setterList->SetColumnWidth(UpdateManagerThread::UMT_SETLST_VAL, wxLIST_AUTOSIZE);
	
	// try to strech the second (key) column
	const int scrollbarWidth = 26;
	int size = m_setterList->GetSize().GetWidth() 
	           - m_setterList->GetColumnWidth(UpdateManagerThread::UMT_SETLST_NUM) 
			   - m_setterList->GetColumnWidth(UpdateManagerThread::UMT_SETLST_VAL) 
			   - scrollbarWidth;
			   
	if ( size > m_setterList->GetColumnWidth(UpdateManagerThread::UMT_SETLST_KEY) )
		m_setterList->SetColumnWidth(UpdateManagerThread::UMT_SETLST_KEY, size);
		
	if ( m_setterList->IsFrozen() == true )
		m_setterList->Thaw();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::requestInfoMessage(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( cnc )
		cnc->processCommand(CMD_TEST_INFO_MESSAGE, std::clog);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::requestWarningMessage(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( cnc )
		cnc->processCommand(CMD_TEST_WARN_MESSAGE, std::clog);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::requestErrorMessage(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( cnc )
		cnc->processCommand(CMD_TEST_ERROR_MESSAGE, std::clog);
}
