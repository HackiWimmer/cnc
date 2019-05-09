/*
 * FYI: codelite win build options
 * g++.exe -c  "...."
 * -Wno-deprecated-declarations -O3 -std=c++17 -std=c++14 -std=c++11 -Wall -mthreads -DHAVE_W32API_H -D__WXMSW__ -DNDEBUG -D_UNICODE 
 * -IC:/@Development/wxWidgets-3.1.0/lib/gcc_dll/mswu 
 * -IC:/@Development/wxWidgets-3.1.0/include 
 * -DWXUSINGDLL 
 * -Wno-ctor-dtor-privacy 
 * -pipe 
 * -fmessage-length=0    
 * -DNDEBUG 
 * -DAPP_USE_SPLASH
 * -I. -IC:\@Development\boost\include\boost-1_64 
 * -I. 
 * -IC:\@Development\boost_1_65_0 
 * -IC:\@Development\freeglut\include

C:/@Development/Compilers/TDM-GCC-64/bin/g++.exe -o "..." 
  * -L. -LC:\@Development\boost\lib -LC:\@Development\freeglut\lib\x64  -lwxmsw31u_stc -lwxmsw31u_webview -lwxmsw31u_propgrid -lwxmsw31u_adv -lwxmsw31u_gl 
  * -lopengl32 -lglu32 -lfreeglut  -mwindows  -mthreads -LC:/@Development/wxWidgets-3.1.0/lib/gcc_dll -lwxmsw31u_richtext -lwxmsw31u_xrc -lwxmsw31u_aui 
  * -lwxmsw31u_html -lwxmsw31u_adv -lwxmsw31u_core -lwxbase31u_xml -lwxbase31u_net -lwxbase31u -lwxscintilla -lwxtiff -lwxjpeg -lwxpng -lwxzlib -lwxregexu -lwxexpat 
  * -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lwxregexu -lwinspool -lwinmm -lshell32 -lcomctl32 -lversion -lshlwapi -lole32 -loleaut32 -luuid -lrpcrt4 -ladvapi32 -lwsock32
*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
#include <math.h>
#include <wx/datetime.h>
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
#include <boost/version.hpp>
#include "OSD/CncUsbPortScanner.h"
#include "OSD/CncAsyncKeyboardState.h"
#include "OSD/webviewOSD.h"
#include "CncExceptions.h"
#include "CncSourceEditor.h"
#include "CncOutboundEditor.h"
#include "CncGameportController.h"
#include "CncNumberFormatter.h"
#include "GlobalFunctions.h"
#include "SerialPort.h"
#include "CncPosition.h"
#include "CncPatternDefinitions.h"
#include "CncUnitCalculator.h"
#include "CncStartPositionResolver.h"
#include "CncFileNameService.h"
#include "CncFilePreviewWnd.h"
#include "SVGPathHandlerCnc.h"
#include "ManuallyParser.h"
#include "SVGFileParser.h"
#include "GCodeFileParser.h"
#include "BinaryPathHandlerCnc.h"
#include "CncArduino.h"
#include "SvgEditPopup.h"
#include "HexDecoder.h"
#include "UnitTestFrame.h"
#include "UpdateManagerThread.h"
#include "GamepadThread.h"
#include "CncConfigProperty.h"
#include "CncSecureRun.h"
#include "CncReferencePosition.h"
#include "CncSpeedMonitor.h"
#include "CncUsbConnectionDetected.h"
#include "CncConnectProgress.h"
#include "CncSha1Wrapper.h"
#include "CncMonitorSplitterWindow.h"
#include "CncTemplateObserver.h"
#include "GL3DOptionPane.h"
#include "GL3DDrawPane.h"
#include "CncPreprocessor.h"
#include "CncMonitorReplayPane.h"
#include "CncGCodeSequenceListCtrl.h"
#include "CncStatisticsPane.h"
#include "CncSvgControl.h"
#include "CncOSEnvironmentDialog.h"
#include "CncContext.h"
#include "GlobalStrings.h"
#include "MainFrame.h"

#ifdef __WXMSW__
	// special includes for WindowPoc handling. 
	// they have to be at the end of the list to avoid compilation errors
	#include <windows.h>
	#include <dbt.h>
	
#endif

////////////////////////////////////////////////////////////////////
extern GlobalConstStringDatabase 	globalStrings;
extern void GlobalStreamRedirectionReset();

////////////////////////////////////////////////////////////////////
unsigned int CncGampadDeactivator::referenceCounter = 0;
unsigned int CncTransactionLock::referenceCounter   = 0;

////////////////////////////////////////////////////////////////////
// app defined events
	wxDEFINE_EVENT(wxEVT_UPDATE_MANAGER_THREAD, 			UpdateManagerEvent);
	wxDEFINE_EVENT(wxEVT_GAMEPAD_THREAD, 					GamepadEvent);
	wxDEFINE_EVENT(wxEVT_PERSPECTIVE_TIMER, 				wxTimerEvent);
	wxDEFINE_EVENT(wxEVT_DEBUG_USER_NOTIFICATION_TIMER, 	wxTimerEvent);
	wxDEFINE_EVENT(wxEVT_TRACE_FROM_THREAD,					wxThreadEvent);
	wxDEFINE_EVENT(wxEVT_DISPATCH_ALL,						wxThreadEvent);
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

////////////////////////////////////////////////////////////////////
class CncRunEventFilter : public wxEventFilter {
	public:
		CncRunEventFilter() {
			wxEvtHandler::AddFilter(this);
		}
		virtual ~CncRunEventFilter() {
			wxEvtHandler::RemoveFilter(this);
		}
		
		virtual int FilterEvent(wxEvent& event) {
			// Update the last user activity
			//const wxEventType t = event.GetEventType();
			//const wxWindow* wnd = (wxWindow*)event.GetEventObject();
			
			return Event_Skip;
			//return Event_Ignore;
		}
};
////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
MainFrame::MainFrame(wxWindow* parent, wxFileConfig* globalConfig)
: MainFrameBClass(parent)
, GlobalConfigManager(this, GetPgMgrSetup(), globalConfig)
, updateManagerThread(NULL)
, gamepadThread(NULL)
, isDebugMode(false)
, isZeroReferenceValid(false)
, canClose(true)
, useSecureRunDlg(true)
, evaluatePositions(true)
, ignoreDirControlEvents(false)
, runConfirmationInfo(RunConfirmationInfo::Wait)
, traceTimerCounter(0)
, lastPortName(wxT(""))
, defaultPortName(wxT(""))
, cnc(new CncControl(CncEMU_NULL))
, sourceEditor(NULL)
, outboundEditor(NULL)
, motionMonitor(NULL)
, fileView(NULL)
, mainFilePreview(NULL)
, outboundFilePreview(NULL)
, monitorFilePreview(NULL)
, toolMagaizne(NULL)
, positionSpy(NULL)
, setterList(NULL)
, speedMonitor(NULL)
, cncPreprocessor(NULL)
, gCodeSequenceList(NULL)
, cncSummaryListCtrl(NULL)
, accelGraphPanel(NULL)
, cncGameportDlg(new CncGameportController(this))
, optionPane3D(NULL)
, drawPane3D(NULL)
, statisticsPane(NULL)
, cnc3DVSplitterWindow(NULL)
, cnc3DHSplitterWindow(NULL)
, templateObserver(NULL)
, spyDetailWindow(NULL)
, perspectiveHandler(globalConfig, m_menuPerspective)
, config(globalConfig)
, lruStore(new wxFileConfig(wxT("CncControllerLruStore"), wxEmptyString, CncFileNameService::getLruFileName(), CncFileNameService::getLruFileName(), wxCONFIG_USE_RELATIVE_PATH | wxCONFIG_USE_NO_ESCAPE_CHARACTERS))
, outboundNbInfo(new NotebookInfo(m_outboundNotebook))
, templateNbInfo(new NotebookInfo(m_templateNotebook))
, lruFileList(LruFileList(16))
, lastTemplateFileNameForPreview(wxT(""))
, pngAnimation(NULL)
, stcFileContentPopupMenu(NULL)
, stcEmuContentPopupMenu(NULL)
, inboundFileParser(NULL)
, perspectiveTimer(this, wxEVT_PERSPECTIVE_TIMER)
, debugUserNotificationTimer(this, wxEVT_DEBUG_USER_NOTIFICATION_TIMER)
, guiControls()
, menuItems()
, secureRunDlg(new CncSecureRun(this))
, refPositionDlg(new CncReferencePosition(this))
{
///////////////////////////////////////////////////////////////////
	// init the specialized wxGrid editor
	CncTextCtrlEditor::init();
			
	// initilazied update mananger thread
	initializeUpdateManagerThread();
	
	// initilazied gamepad thread
	if ( true )
		initializeGamepadThread();
	
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
	this->Bind(wxEVT_CHAR_HOOK, 					&MainFrame::globalKeyDownHook, 				this);
	this->Bind(wxEVT_UPDATE_MANAGER_THREAD, 		&MainFrame::onThreadAppPosUpdate, 			this, MainFrame::EventId::APP_POS_UPDATE);
	this->Bind(wxEVT_UPDATE_MANAGER_THREAD, 		&MainFrame::onThreadCtlPosUpdate, 			this, MainFrame::EventId::CTL_POS_UPDATE);
	this->Bind(wxEVT_UPDATE_MANAGER_THREAD, 		&MainFrame::onThreadHeartbeat, 				this, MainFrame::EventId::HEARTBEAT);
	this->Bind(wxEVT_UPDATE_MANAGER_THREAD, 		&MainFrame::onThreadCompletion, 			this, MainFrame::EventId::COMPLETED);
	this->Bind(wxEVT_DISPATCH_ALL, 					&MainFrame::onThreadDispatchAll,			this, MainFrame::EventId::DISPATCH_ALL);
	this->Bind(wxEVT_TRACE_FROM_THREAD, 			&MainFrame::onThreadPostInfo,	 			this, MainFrame::EventId::POST_INFO);
	this->Bind(wxEVT_TRACE_FROM_THREAD, 			&MainFrame::onThreadPostWarning, 			this, MainFrame::EventId::POST_WARNING);
	this->Bind(wxEVT_TRACE_FROM_THREAD, 			&MainFrame::onThreadPostError,	 			this, MainFrame::EventId::POST_ERROR);
	this->Bind(wxEVT_GAMEPAD_THREAD, 				&MainFrame::onGamepdThreadInitialized, 		this, MainFrame::EventId::INITIALIZED);
	this->Bind(wxEVT_GAMEPAD_THREAD, 				&MainFrame::onGamepdThreadCompletion, 		this, MainFrame::EventId::COMPLETED);
	this->Bind(wxEVT_GAMEPAD_THREAD, 				&MainFrame::onGamepdThreadUpadte, 			this, MainFrame::EventId::GAMEPAD_STATE);
	this->Bind(wxEVT_GAMEPAD_THREAD, 				&MainFrame::onGamepdThreadHeartbeat, 		this, MainFrame::EventId::GAMEPAD_HEARTBEAT);
	this->Bind(wxEVT_CNC_NAVIGATOR_PANEL, 			&MainFrame::onNavigatorPanel, 				this);
	
	const wxFont font = GBL_CONTEXT->outboundListBookFont;
	m_notebookConfig->GetListView()->SetFont(font);
	m_listbookMonitor->GetListView()->SetFont(font);
	m_listbookPostProcessor->GetListView()->SetFont(font);
}
///////////////////////////////////////////////////////////////////
MainFrame::~MainFrame() {
///////////////////////////////////////////////////////////////////
	// stop the serial timer and wait interval to finish the work behind
	m_serialTimer->Stop();
	if ( cnc != NULL )
		waitActive(m_serialTimer->GetInterval());
	
	perspectiveHandler.destroyUserPerspectives();
	
	// unbind 
	this->Unbind(wxEVT_CHAR_HOOK, 					&MainFrame::globalKeyDownHook, 				this);
	this->Unbind(wxEVT_UPDATE_MANAGER_THREAD, 		&MainFrame::onThreadAppPosUpdate, 			this, MainFrame::EventId::APP_POS_UPDATE);
	this->Unbind(wxEVT_UPDATE_MANAGER_THREAD, 		&MainFrame::onThreadCtlPosUpdate, 			this, MainFrame::EventId::CTL_POS_UPDATE);
	this->Unbind(wxEVT_UPDATE_MANAGER_THREAD, 		&MainFrame::onThreadHeartbeat, 				this, MainFrame::EventId::HEARTBEAT);
	this->Unbind(wxEVT_UPDATE_MANAGER_THREAD, 		&MainFrame::onThreadCompletion, 			this, MainFrame::EventId::COMPLETED);
	this->Unbind(wxEVT_DISPATCH_ALL, 				&MainFrame::onThreadDispatchAll, 			this, MainFrame::EventId::DISPATCH_ALL);
	this->Unbind(wxEVT_TRACE_FROM_THREAD, 			&MainFrame::onThreadPostInfo,	 			this, MainFrame::EventId::POST_INFO);
	this->Unbind(wxEVT_TRACE_FROM_THREAD, 			&MainFrame::onThreadPostWarning, 			this, MainFrame::EventId::POST_WARNING);
	this->Unbind(wxEVT_TRACE_FROM_THREAD,	 		&MainFrame::onThreadPostError,	 			this, MainFrame::EventId::POST_ERROR);
	this->Unbind(wxEVT_GAMEPAD_THREAD, 				&MainFrame::onGamepdThreadInitialized, 		this, MainFrame::EventId::INITIALIZED);
	this->Unbind(wxEVT_GAMEPAD_THREAD,	 			&MainFrame::onGamepdThreadCompletion,	 	this, MainFrame::EventId::COMPLETED);
	this->Unbind(wxEVT_CNC_NAVIGATOR_PANEL, 		&MainFrame::onNavigatorPanel, 				this);
	
	// explicit delete the motion monitor pointer here, beacause the motion monitor class
	// considers the Mainframe GBL_CONFIG->getTheApp() pointer in its dtor 
	// and this crashes definitly if the MainFame dtor is already passed
	// the delete below avoid this behaviour
	if ( motionMonitor != NULL )
		delete motionMonitor;
	
	if ( cncGameportDlg != NULL )
		delete cncGameportDlg;

	wxASSERT(lruStore);
	lruFileList.save(lruStore);
	lruStore->Flush();
	delete lruStore;
	
	wxASSERT(config);
	config->Flush();
	delete config;
	
	wxASSERT(secureRunDlg);
	delete secureRunDlg;
	
	wxASSERT(outboundNbInfo);
	delete outboundNbInfo;
	
	wxASSERT(templateNbInfo);
	delete templateNbInfo;
	
	if ( cnc != NULL )
		delete cnc;
	
	DeletePendingEvents();
	GBL_CONFIG->destroyTheApp();
	
	GlobalStreamRedirectionReset();
}
///////////////////////////////////////////////////////////////////
void MainFrame::globalKeyDownHook(wxKeyEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( refPositionDlg && refPositionDlg->IsShownOnScreen() ) {
		wxPostEvent(refPositionDlg, event);
		event.Skip(false);
		return;
	}
	
	if ( secureRunDlg && secureRunDlg->IsShownOnScreen() ) {
		wxPostEvent(secureRunDlg, event);
		event.Skip(false);
		return;
	}
	
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
void MainFrame::umPostEvent(const UpdateManagerThread::Event& evt) { 
///////////////////////////////////////////////////////////////////
	if ( updateManagerThread == NULL ) 
		return;
		
	if ( updateManagerThread->IsPaused() == true ) {
		wxCriticalSectionLocker enter(pUpdateManagerThreadCS);
		updateManagerThread->Resume();
	}

	updateManagerThread->postEvent(evt); 
	
	// update speed monitor
	if ( evt.hasFeedSpeedInfo() ) {
		
		static CncSpeedMonitor::SpeedData sd;
		sd.configured_MM_MIN = evt.pos.configuredSpeedValue;
		sd.received_MM_MIN	 = evt.pos.currentSpeedValue;
		sd.measured_MM_MIN	 = cnc != NULL ? cnc->getMeasuredFeedSpeed_MM_MIN() : 0.0; 
		
		speedMonitor->setCurrentFeedSpeedValue(sd);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::ShowAuiToolMenu(wxAuiToolBarEvent& event) {
///////////////////////////////////////////////////////////////////
	// overides the wxcrafter generted method
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
					
					// dont use the toolbar event handler because this will generate a crash while cnc::waitActive is in action!
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
	// currently nothing to do
	//std::clog << "MainFrame::configurationUpdated(wxCommandEvent& event)" << std::endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::setRefPostionState(bool state) {
///////////////////////////////////////////////////////////////////
	isZeroReferenceValid = state;

	wxBitmap bmp;
	if ( isZeroReferenceValid == true ) bmp = ImageLib24().Bitmap("BMP_TRAFFIC_LIGHT_GREEN"); 
	else 								bmp = ImageLib24().Bitmap("BMP_TRAFFIC_LIGHT_RED");
	
	wxString tip("Reference position state: ");
	// display ref pos mode too
	if ( isZeroReferenceValid == true ) {
		wxMemoryDC mdc(bmp);
		mdc.SetFont(wxFontInfo(8).FaceName("Arial"));
		mdc.SetTextForeground(wxColor(0, 0, 0));
		mdc.DrawText(wxString::Format("%d", (int)GBL_CONFIG->getReferencePositionMode()), {5,1});
		bmp = mdc.GetAsBitmap();
		
		tip.append("Valid\n");
		tip.append(wxString::Format("Reference position mode: %d", (int)GBL_CONFIG->getReferencePositionMode()));
		
	} else {
		tip.append("Not Valid");
		
	}
	
	m_refPosState->SetToolTip(tip);
	m_refPosState->SetBitmap(bmp);
	m_statusBar->Refresh();
	m_statusBar->Update();
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
		CncApp::MenuInfo mi = it->second;
		
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
		CncApp::WindowInfo wi = *it;
		
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
	
	if ( state == true )
		m_btSpeedControl->Enable(GBL_CONTEXT->isProbeMode());
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
	
	// Montion Monitor
	motionMonitor = new CncMotionMonitor(this);
	GblFunc::replaceControl(drawPane3D->GetDrawPane3DPlaceHolder(), motionMonitor);
	drawPane3D->setMotionMonitor(motionMonitor);
	optionPane3D->setMotionMonitor(motionMonitor);
	statisticsPane->setMotionMonitor(motionMonitor);
	activate3DPerspectiveButton(m_3D_Perspective1);
	
	// Template observer
	templateObserver = new CncTemplateObserver(this);
	GblFunc::replaceControl(m_panelTemplateObserverPlaceholder, templateObserver);
	
	// Source Editor
	sourceEditor = new CncSourceEditor(this);
	GblFunc::replaceControl(m_stcFileContent, sourceEditor);
	
	// Outbound Editor
	outboundEditor = new CncOutboundEditor(this);
	GblFunc::replaceControl(m_outboundFileSource, outboundEditor);
	
	// File View
	fileView = new CncFileView(this);
	GblFunc::replaceControl(m_mainFileViewPlaceholder, fileView);
	
	// Inbound File Preview
	mainFilePreview = new CncFilePreview(this);
	GblFunc::replaceControl(m_filePreviewPlaceholder, mainFilePreview);
	
	// Outbound File Preview
	outboundFilePreview = new CncFilePreview(this);
	GblFunc::replaceControl(m_outboundPreviewPlaceholder, outboundFilePreview);

	// File Preview
	monitorFilePreview = new CncFilePreview(this);
	GblFunc::replaceControl(m_monitorTemplatePreviewPlaceHolder, monitorFilePreview);
	
	// tool magazine
	toolMagaizne = new CncToolMagazine(this); 
	GblFunc::replaceControl(m_toolMagazinePlaceholder, toolMagaizne);
	
	// pos spy control
	positionSpy = new CncPosSpyListCtrl(this, wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_positionSpy, positionSpy);

	// pos spy control
	setterList = new CncSetterListCtrl(this, wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_setterList, setterList);
	
	// speed monitor control
	speedMonitor = new CncSpeedMonitor(this); 
	GblFunc::replaceControl(m_speedMonitorPlaceholder, speedMonitor);
	
	// preprocessor control
	cncPreprocessor = new CncPreprocessor(this); 
	GblFunc::replaceControl(m_preprocessorPlaceholder, cncPreprocessor);
	
	// summary list
	cncSummaryListCtrl = new CncSummaryListCtrl(this, wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_cncSummaryListCtrl, cncSummaryListCtrl);
	
	// serial spy list control
	serialSpyListCtrl = new CncSerialSpyListCtrl(this, wxLC_VRULES | wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_serialSpyPlaceholder, serialSpyListCtrl);
	
	// acceleration graph
	accelGraphPanel = new CfgAccelerationGraph(this); 
	GblFunc::replaceControl(m_accelGraphPanel, accelGraphPanel);
	
	// Outbound editor svg viewer
	outboundEditorSvgView = new CncSvgViewer(this); 
	GblFunc::replaceControl(m_outboundEditorSvgViewPlaceholder, outboundEditorSvgView);
	
	// GCode Sequence control
	gCodeSequenceList = new CncGCodeSequenceListCtrl(this, wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_gCodeSequenceListPlaceholder, gCodeSequenceList);

	// navigator panel
	CncNavigatorPanel::Config cfg;
	cfg.innerCircle = true;
	cfg.shortFormat = false;
	cfg.margin		= 0;
	cfg.alignment	= wxALIGN_CENTER_HORIZONTAL;
	cfg.initToolTipMapAsCoordSytem();
	navigatorPanel = new CncNavigatorPanel(this, cfg); 
	GblFunc::replaceControl(m_navigationPanelPlaceholder, navigatorPanel);
}
///////////////////////////////////////////////////////////////////
void MainFrame::registerGuiControls() {
///////////////////////////////////////////////////////////////////
	registerGuiControl(navigatorPanel);
	registerGuiControl(sourceEditor);
	registerGuiControl(outboundEditor);
	registerGuiControl(fileView);
	
	registerGuiControl(m_searchConnections);
	registerGuiControl(m_btnOrigin);
	registerGuiControl(m_btnRuler);
	registerGuiControl(m_btnHelpLines);
	registerGuiControl(m_btnBoundBox);
	registerGuiControl(m_btnPosMarker);
	registerGuiControl(m_btnFlyPath);
	registerGuiControl(m_btToggleOutboundEditorWordWrap);
	registerGuiControl(m_checkBoxToolEnabled);
	registerGuiControl(m_manuallyCorrectLimitPos);
	registerGuiControl(m_rcSecureDlg);
	registerGuiControl(m_btSpeedControl);
	registerGuiControl(m_btProbeMode);
	registerGuiControl(m_btSelectReferences);
	registerGuiControl(m_btSelectManuallyMove);
	registerGuiControl(m_btSelectSetup);
	registerGuiControl(m_btSelectTemplate);
	registerGuiControl(m_btSelectInboundTest);
	registerGuiControl(m_btSelectInboundPreview);
	registerGuiControl(m_btSelectCncPreview);
	registerGuiControl(m_btSelectTemplatePreview);
	registerGuiControl(m_cbRenderResolution);
	registerGuiControl(m_3D_Refreh);
	registerGuiControl(m_3D_Clear);
	registerGuiControl(m_cbContentPosSpy);
	registerGuiControl(m_testToggleTool);
	registerGuiControl(m_testToggleEndSwitch);
	registerGuiControl(m_portSelector);
	registerGuiControl(m_testDimModeX);
	registerGuiControl(m_testDimModeY);
	registerGuiControl(m_testDimModeZ);
	registerGuiControl(m_testDimTakeX);
	registerGuiControl(m_testDimTakeY);
	registerGuiControl(m_testDimTakeZ);
	registerGuiControl(m_testDimTakeAll);
	registerGuiControl(m_connect);
	registerGuiControl(m_refPosition);
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
	registerGuiControl(m_clearLogger);
	registerGuiControl(m_displayInterval);
	registerGuiControl(m_btRequestCtlConfig);
	registerGuiControl(m_btRequestControllerPins);
	registerGuiControl(m_lruList);
	registerGuiControl(m_copyLogger);
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
	registerGuiControl(m_xManuallySlider);
	registerGuiControl(m_yManuallySlider);
	registerGuiControl(m_zManuallySlider);
	registerGuiControl(m_minManuallyXSlider);
	registerGuiControl(m_minManuallyYSlider);
	registerGuiControl(m_minManuallyZSlider);
	registerGuiControl(m_metricX);
	registerGuiControl(m_metricY);
	registerGuiControl(m_metricZ);
	registerGuiControl(m_maxManuallyXSlider);
	registerGuiControl(m_maxManuallyYSlider);
	registerGuiControl(m_maxManuallyZSlider);
	registerGuiControl(m_zeroManuallyXSlider);
	registerGuiControl(m_zeroManuallyYSlider);
	registerGuiControl(m_zeroManuallyZSlider);
	registerGuiControl(m_signManuallyXSlider);
	registerGuiControl(m_signManuallyYSlider);
	registerGuiControl(m_signManuallyZSlider);
	registerGuiControl(m_manuallyToolId);
	registerGuiControl(m_manuallySpeedSlider);
	registerGuiControl(m_manuallySpeedValue);
	registerGuiControl(m_mmRadioCoordinates);
	
	//...
	
	// already managed by sourceEditor
	//registerGuiControl(m_openSourceExtern);
	//registerGuiControl(m_openSvgExtern);
	//registerGuiControl(m_saveTemplate);
	//registerGuiControl(m_reloadTemplate);
	//registerGuiControl(m_removeTemplate);
	//registerGuiControl(m_renameTemplate);

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
void MainFrame::testFunction1(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	cnc::trc.logInfoMessage("Test function 1");

	CncNanoTimestamp ts1 =  CncTimeFunctions::getNanoTimestamp();
	CncTimeFunctions::sleepMircoseconds(300);
	CncNanoTimestamp ts2 =  CncTimeFunctions::getNanoTimestamp();
	std::cout << ( ts2 - ts1 ) / 1000 << std::endl;

}
///////////////////////////////////////////////////////////////////
void MainFrame::testFunction2(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	cnc::trc.logInfoMessage("Test function 2");

std::cout << wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTION).GetAsString() << std::endl;

	std::cout << GBL_CONFIG->connvert_MM_SEC_TO_STP_SEC_X(45.0) << std::endl;
	std::cout << GBL_CONFIG->connvert_STP_SEC_TO_MM_MIN_X(3000) << std::endl;

}
///////////////////////////////////////////////////////////////////
void MainFrame::testFunction3(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	cnc::trc.logInfoMessage("Test function 3");
	//GblFunc::stacktrace(std::clog);
	
	if ( cnc == NULL )
		return;
	
	wxDateTime ts = wxDateTime::UNow();
	std::cout << ts.FormatISOTime() << "." << ts.GetMillisecond() << std::endl;
		bool ret = cnc->getSerialExtern()->test();
		std::cout << "cnc->getSerial()->test() = " << ret  << std::endl;
	ts = wxDateTime::UNow();
	std::cout << ts.FormatISOTime() << "." << ts.GetMillisecond() << std::endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::testFunction4(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	cnc::trc.logInfoMessage("Test function 4");
		
	GblFunc::stacktrace(std::clog);
	GblFunc::stacktraceOnlyApp(std::clog, false);
	GblFunc::stacktraceOnlyApp(std::clog, true);
	
}
///////////////////////////////////////////////////////////////////
void MainFrame::traceGccVersion(std::ostream& out) {
///////////////////////////////////////////////////////////////////
	out << " :: g++ version info: " 
		<< __GNUC__
		<< "."
		<< __GNUC_MINOR__
		<< "."
		<< __GNUC_PATCHLEVEL__
		<< std::endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::traceWxWidgetsVersion(std::ostream& out) {
///////////////////////////////////////////////////////////////////
	out << " :: wxWidgets version info: " 
		<< wxMAJOR_VERSION
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
void MainFrame::traceBoostVersion(std::ostream& out) {
///////////////////////////////////////////////////////////////////
	out << " :: Boost version info: " 
		<< BOOST_VERSION / 100000
		<< "."
		<< BOOST_VERSION / 100 % 1000
		<< "."
		<< BOOST_VERSION % 100 
		<< std::endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::traceWoodworkingCncVersion(std::ostream& out) {
///////////////////////////////////////////////////////////////////
	out << " :: Programm version info: " 
		<< globalStrings.programVersion
		<< std::endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::startupTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	// Setup AUI Windows menue
	perspectiveHandler.loadPerspective("Default");
	decorateViewMenu();
	
	// Version infos
	std::clog << "Version Information:" << std::endl;
	traceGccVersion(std::cout);
	traceWxWidgetsVersion(std::cout);
	traceBoostVersion(std::cout);
	
	GLContextBase::traceOpenGLVersionInfo(std::cout);
	traceWoodworkingCncVersion(std::cout);
	traceSessionId();
	std::cout << std::endl;
	
	// Auto connect ?
	if ( CncConfig::getGlobalCncConfig()->getAutoConnectFlag() )
		connectSerialPortDialog();
		
	// wait intil the main windows is shown
	// this is with respect to the toggle* calls below
	{
		while ( IsShown() == false )
			dispatchAll();
			
		// GTK specific: This is to hide the correponding windows
		toggleMotionMonitorOptionPane(true);
		toggleMotionMonitorStatisticPane(true);
	}
	
	// Auto process ?
	if ( CncConfig::getGlobalCncConfig()->getAutoProcessFlag() ) {
		defineMinMonitoring();
		processTemplateWrapper();
		defineNormalMonitoring();
	}
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
	if ( cnc == NULL )
		return;
		
	if ( cnc->isConnected() == false )
		return;
	
	// give the cnc or serial port a change to do
	// something periodically e. g. pause a thread
	cnc->onPeriodicallyAppEvent();
	
	// pause the update manager thread if nothing more is to do
	static unsigned int counter = 0;
	if ( updateManagerThread != NULL ) {
		if ( updateManagerThread->IsPaused() == false ) {
			
			if ( positionSpy != NULL && positionSpy->IsShownOnScreen() == true ) {
				
				if ( updateManagerThread->somethingLeftToDo() == false ) {
					// a counter of 10 means 5 seconds
					if ( ++counter > 10 ) {
						wxCriticalSectionLocker enter(pUpdateManagerThreadCS);
						updateManagerThread->Pause();
						
						counter = 0;
					}
				}
			}
			else {
				// put it to bed if the posSpyList isn't visible
				updateManagerThread->Pause();
			}
		}
	}
	
	// idle handling
	if ( m_miRqtIdleMessages->IsChecked() == true ) {
		
		// stop the time to avoid overlapping idle request
		m_serialTimer->Stop();
		
		// it's very import to deactivate the notifications during idle processing
		// because instead every config change (sc()) will release a notification
		// as a result the processing slows down significantly.
		CncConfig::NotificationDeactivator cfgNotDeactivation(false);
		
		// it's very important to avoid event handling during the idle processing
		// to prevent the start of furter commands
		GBL_CONTEXT->setAllowEventHandling(false);
		
		// request the idle information
		cnc->sendIdleMessage();
		
		// reconstructed the previous event handling mode
		GBL_CONTEXT->setAllowEventHandling(m_menuItemAllowEvents->IsChecked());
		
		// restart due to the previous Stop() command
		m_serialTimer->Start();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::onPaintSpeedPanel(wxPaintEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( cnc == NULL )
		return;
	
	static wxColour col(0, 128, 255);
	static wxBrush  brush(col);
	static wxPen    barPen(col, 1, wxSOLID);
	static wxPen    wpPen(*wxRED, 1, wxSOLID);
	
	const wxSize size   = m_speedPanel->GetSize();
	unsigned int pos    = size.GetWidth();
	unsigned int height = size.GetHeight();
	
	pos *= (cnc->getRealtimeFeedSpeed_MM_MIN() / GBL_CONFIG->getMaxSpeedXYZ_MM_MIN());
	
	// bar
	wxPaintDC dc(m_speedPanel);
	dc.SetPen(barPen);
	dc.SetBrush(brush);
	
	wxRect rect(0, 0, pos, height);
	dc.DrawRectangle(rect);
	
	// watermark for current config
	unsigned int wp = size.GetWidth() 
	                * cnc->getConfiguredFeedSpeed_MM_MIN() 
					/ GBL_CONFIG->getMaxSpeedXYZ_MM_MIN();
					
	dc.SetPen(wpPen);
	
	// move wp 2 pixel to the left so see valu = max also.
	dc.DrawLine(wp - 2, 0, wp - 2, height);
	dc.DrawLine(wp - 1, 0, wp - 1, height);
}
///////////////////////////////////////////////////////////////////
void MainFrame::onThreadAppPosUpdate(UpdateManagerEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( cnc == NULL )
		return;
		
	CncUnit unit = GBL_CONFIG->getDisplayUnit();
	
	// update position
	switch ( unit ) {
		case CncSteps:	// update application position
						m_xAxis->ChangeValue(wxString::Format("%8ld", cnc->getCurAppPos().getX()));
						m_yAxis->ChangeValue(wxString::Format("%8ld", cnc->getCurAppPos().getY()));
						m_zAxis->ChangeValue(wxString::Format("%8ld", cnc->getCurAppPos().getZ()));
						break;
						
		case CncMetric:	// update application position
						m_xAxis->ChangeValue(wxString::Format("%4.3lf", cnc->getCurAppPos().getX() * GBL_CONFIG->getDisplayFactX(unit)));
						m_yAxis->ChangeValue(wxString::Format("%4.3lf", cnc->getCurAppPos().getY() * GBL_CONFIG->getDisplayFactY(unit)));
						m_zAxis->ChangeValue(wxString::Format("%4.3lf", cnc->getCurAppPos().getZ() * GBL_CONFIG->getDisplayFactZ(unit)));
						break;
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::onThreadCtlPosUpdate(UpdateManagerEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( cnc == NULL )
		return;
		
	CncUnit unit = GBL_CONFIG->getDisplayUnit();
	
	// update position
	switch ( unit ) {
		case CncSteps:	// update controller position
						m_xAxisCtl->ChangeValue(wxString::Format("%8ld", cnc->getCurCtlPos().getX()));
						m_yAxisCtl->ChangeValue(wxString::Format("%8ld", cnc->getCurCtlPos().getY()));
						m_zAxisCtl->ChangeValue(wxString::Format("%8ld", cnc->getCurCtlPos().getZ()));
						break;
						
		case CncMetric:	// update controller position
						m_xAxisCtl->ChangeValue(wxString::Format("%4.3lf", cnc->getCurCtlPos().getX() * GBL_CONFIG->getDisplayFactX(unit)));
						m_yAxisCtl->ChangeValue(wxString::Format("%4.3lf", cnc->getCurCtlPos().getY() * GBL_CONFIG->getDisplayFactY(unit)));
						m_zAxisCtl->ChangeValue(wxString::Format("%4.3lf", cnc->getCurCtlPos().getZ() * GBL_CONFIG->getDisplayFactZ(unit)));
						break;
	}
	
	// update z view
	m_zView->updateView(cnc->getCurCtlPos().getZ() * GBL_CONFIG->getDisplayFactZ(unit));
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
	
	// update time consumed
	if ( pngAnimation && pngAnimation->IsRunning() ) {
		logTimeConsumed();
	}
	
	// statistic
	if ( isProcessing() )
		statisticsPane->logStatistics(false);
	
	// feed speed control + feed speed panel
	m_speedPanel->Refresh();

	wxString sValue(_maxSpeedLabel);

	if ( GBL_CONTEXT->isProbeMode() == false ) {
		if ( cnc != NULL ) {
			const double dValue = cnc->getRealtimeFeedSpeed_MM_MIN();
			if ( dValue  < 0.0 ) sValue.assign(_maxSpeedLabel);
			else 				 sValue.assign(wxString::Format("%.1lf", dValue));
		}
	}

	m_realtimeFeedSpeed->ChangeValue(sValue);
	
	// update position syp
	if ( updateManagerThread == NULL )
		return;
		
	if ( updateManagerThread->IsPaused() == true )
		return;
		
	bool updatePosCount = false;
	static long lastCount = 0;
	if ( m_btTogglePosSpy->GetValue() == true ) {
		if ( positionSpy->IsShownOnScreen() ) {
			if ( updateManagerThread->fillPositionSpy(positionSpy) > 0 ) {
				positionSpy->Refresh();
			}
		}
		
		if ( lastCount != positionSpy->GetItemCount() ) {
			lastCount = positionSpy->GetItemCount();
			updatePosCount = true;
		}
		
	} else {
		if ( lastCount != positionSpy->GetItemCount() ) {
			lastCount = 0;
			updatePosCount = true;
		}
	}
	
	if ( updatePosCount == true ) {
		m_positionSpyCount->ChangeValue(CncNumberFormatter::toString(((long)(positionSpy->GetItemCount()))));
		m_positionSpyCount->SetToolTip(m_positionSpyCount->GetLabel());
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::onThreadDispatchAll(wxThreadEvent& event) {
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
void MainFrame::onThreadPostInfo(wxThreadEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( event.GetString().IsEmpty() == false )
		std::cout << event.GetString() << std::endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::onThreadPostWarning(wxThreadEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( event.GetString().IsEmpty() == false )
		cnc::cex1 << event.GetString() << std::endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::onThreadPostError(wxThreadEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( event.GetString().IsEmpty() == false )
		std::cerr << event.GetString() << std::endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::onGamepdThreadInitialized(GamepadEvent& event) {
///////////////////////////////////////////////////////////////////
	//std::cout << "MainFrame::onGamepdThreadInitilaized" << std::endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::onGamepdThreadCompletion(GamepadEvent& event) {
///////////////////////////////////////////////////////////////////
	//std::cout << "MainFrame::onGamepdThreadCompletion" << std::endl;
	gamepadThread = NULL;
}
///////////////////////////////////////////////////////////////////
void MainFrame::onGamepdThreadHeartbeat(GamepadEvent& event) {
///////////////////////////////////////////////////////////////////
	decorateGamepadState(event.data.connected);
	
	if ( event.data.connectionStateChanged == true ) {
		if ( event.data.connected == false )	cnc::trc.logWarning(" Gamepad disconnected");
		else									cnc::trc.logWarning(" Gamepad connected");
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::onGamepdThreadUpadte(GamepadEvent& event) {
///////////////////////////////////////////////////////////////////
	onGamepdThreadHeartbeat(event);
	
	if ( event.data.connected == false )
		return;
	
	if ( cncGameportDlg )
		cncGameportDlg->update(event);
}
///////////////////////////////////////////////////////////////////
void MainFrame::onClose(wxCloseEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( checkIfTemplateIsModified() == false )
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
			cnc->disconnect();
			
			// this will crash if a run  or debug is active
			// this have to be avoided - see can close above
			delete cnc;
			cnc = NULL;
		}
	}
	
	// Destroy the update manager thread
	if ( updateManagerThread != NULL) {
		if ( updateManagerThread->IsRunning() == false) {
			wxCriticalSectionLocker enter(pUpdateManagerThreadCS);
			updateManagerThread->Resume();
		}
		
		updateManagerThread->stop();
		
		while ( true ) {
			{ // was the ~UpdateManagerThreadThread() function executed?
				wxCriticalSectionLocker enter(pUpdateManagerThreadCS);
				if ( !updateManagerThread ) 
					break;
			}
			// wait for thread completion
			wxThread::This()->Sleep(10);
		}
	}
	
	// Destroy the gamepad thread
	if ( gamepadThread != NULL) {
		if ( gamepadThread->IsRunning() == false) {
			wxCriticalSectionLocker enter(pGamepadThreadCS);
			gamepadThread->Resume();
		}
		
		gamepadThread->stop();
		
		while ( true ) {
			{ // was the ~GamepadThread() function executed?
				wxCriticalSectionLocker enter(pGamepadThreadCS);
				if ( !gamepadThread ) 
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
	
	// the following code is the best compromise, but aui handling isn't perfect
	if ( wxTheApp->HasPendingEvents() )
		wxTheApp->ProcessPendingEvents();
		
	while ( evtLoop->Pending() )
		evtLoop->Dispatch();
}
#ifdef __WXMSW__
///////////////////////////////////////////////////////////////////
WXLRESULT MainFrame::MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam) {
///////////////////////////////////////////////////////////////////
	wxString portName("Undefined");
	PDEV_BROADCAST_HDR lpdb = NULL;
	
	if ( message == WM_DEVICECHANGE) {
		// logging
		switch ( wParam ) {
			case DBT_DEVICEARRIVAL:			lpdb = (PDEV_BROADCAST_HDR)lParam;
											if ( lpdb->dbch_devicetype == DBT_DEVTYP_PORT ) {
												PDEV_BROADCAST_PORT pPort = (PDEV_BROADCAST_PORT) lpdb;
												wxString n(pPort->dbcp_name);
												portName.assign(n);
											}
											
											cnc::trc.logInfo(wxString("A new COM device was detected on port: ") << portName);
											break;
											
			case DBT_DEVICEREMOVECOMPLETE:	lpdb = (PDEV_BROADCAST_HDR)lParam;
											if ( lpdb->dbch_devicetype == DBT_DEVTYP_PORT ) {
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
		
		static CncUsbConnectionDetected* dlg = new CncUsbConnectionDetected(this);
		switch ( wParam ) {
			// ask for connect - on demand . . . 
			case DBT_DEVICEARRIVAL:	{
				if ( isProcessing() == false ) {
					
					if ( dlg->IsShown() ) {
						dlg->setPortName(portName);
						return ret;
					}
					
					dlg->setPortName(portName);
					if ( dlg->ShowModal() == wxID_YES ) {
						m_portSelector->SetStringSelection(portName);
						connectSerialPortDialog();
					}
				}
			}
			break;
			
			// check if current com connection is effected
			case DBT_DEVICEREMOVECOMPLETE:	{
				// check if the current connection is effected
				if ( lastPortName == portName ) {
					if ( cnc && cnc->isConnected() ) {
						cnc->interrupt("Serial Device Removed");
						cnc->disconnect();
						lastPortName.clear();
						std::cerr << "Connection brocken" << std::endl;
						cnc::trc.logWarning("Connection broken . . .");
					}
					
					if ( dlg->IsShown() ) {
						dlg->EndModal(wxID_NO);
					}
				}
				break;
			}
			
			case DBT_DEVNODES_CHANGED: {
				activateGamepadNotificationsOnDemand(true);
				break;
			}
			
			default: ;
		} 
	}
	
	return ret;
}
#else
	// currently no equivalent
#endif
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
	startAnimationControl();
	m_portSelector->Clear();
	
	// add default ports
	if ( lastPortName == _portEmulatorNULL )	m_portSelector->Append(_portEmulatorNULL,  ImageLibPortSelector().Bitmap("BMP_PS_CONNECTED"));
	else										m_portSelector->Append(_portEmulatorNULL,  ImageLibPortSelector().Bitmap("BMP_PS_AVAILABLE"));
	
	if ( lastPortName == _portEmulatorTEXT )	m_portSelector->Append(_portEmulatorTEXT,  ImageLibPortSelector().Bitmap("BMP_PS_CONNECTED"));
	else										m_portSelector->Append(_portEmulatorTEXT,  ImageLibPortSelector().Bitmap("BMP_PS_AVAILABLE"));
	
	if ( lastPortName == _portEmulatorSVG )		m_portSelector->Append(_portEmulatorSVG,   ImageLibPortSelector().Bitmap("BMP_PS_CONNECTED"));
	else										m_portSelector->Append(_portEmulatorSVG,   ImageLibPortSelector().Bitmap("BMP_PS_AVAILABLE"));
	
	if ( lastPortName == _portEmulatorGCODE )	m_portSelector->Append(_portEmulatorGCODE, ImageLibPortSelector().Bitmap("BMP_PS_CONNECTED"));
	else										m_portSelector->Append(_portEmulatorGCODE, ImageLibPortSelector().Bitmap("BMP_PS_AVAILABLE"));
	
	if ( lastPortName == _portEmulatorBIN )		m_portSelector->Append(_portEmulatorBIN,   ImageLibPortSelector().Bitmap("BMP_PS_CONNECTED"));
	else										m_portSelector->Append(_portEmulatorBIN,   ImageLibPortSelector().Bitmap("BMP_PS_AVAILABLE"));
	
	// add com ports
	int pStart = list == true ?  1 :  0;
	int pEnd   = list == true ? 11 : 32;//256;
	
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
		wxMessageBox( _("Couldn't create update manager thread!") );
		abort();
	}
	
	error = updateManagerThread->Run();
	if (error != wxTHREAD_NO_ERROR) {
		wxMessageBox( _("Couldn't run update manager thread!") );
		abort();
	}
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
	
	activateGamepadNotifications(true);
}
/*
///////////////////////////////////////////////////////////////////
bool MainFrame::Show(bool show) {
///////////////////////////////////////////////////////////////////
	MainFrameBClass::Show(show);
		
	std::cout << "adsadasdasdasd\n";
}*/
///////////////////////////////////////////////////////////////////
void MainFrame::initialize(void) {
///////////////////////////////////////////////////////////////////
	lruFileList.setListControl(m_lruList);
	
	createAnimationControl();
	decorateSearchButton();
	decorateSpeedControlBtn(true);
	switchMonitorButton(true);
	determineRunMode();
	decoratePosSpyConnectButton(true);
	decorateSecureDlgChoice(true);
	registerGuiControls();
	changeManuallySpeedValue();
	initSpeedConfigPlayground();
	decorateOutboundSaveControls(false);
	toggleMotionMonitorOptionPane(true);
	toggleMotionMonitorStatisticPane(true);

	// init config pages
	GetConfigurationToolbook()->SetSelection(0);
	m_pgMgrSetup->SelectPage(GetConfigurationToolbook()->GetSelection());

	speedMonitor->init(GBL_CONFIG->getMaxSpeedXYZ_MM_MIN());
	m_speedPanel->SetBackgroundColour(wxColour(234, 234, 234));
	
	m_loggerNotebook->SetSelection(LoggerSelection::VAL::CNC);
	
	perspectiveHandler.setupUserPerspectives();
	
	this->SetTitle(wxString(globalStrings.programTitel) + " " + globalStrings.programVersion);
	
	// setup cnc port selector box
	decoratePortSelector();
	
	wxFloatingPointValidator<float> val(3, NULL, wxNUM_VAL_DEFAULT );//, wxNUM_VAL_ZERO_AS_BLANK);
	val.SetRange(0, 100.0);
	val.SetPrecision(3);
	m_testDistanceX->SetValidator(val);
	m_testDistanceY->SetValidator(val);
	m_testDistanceZ->SetValidator(val);
	
	val.SetRange(0, 50000.0);
	val.SetPrecision(1);
	m_manuallySpeedValue->SetValidator(val);
	
	val.SetRange(0, 50000);
	val.SetPrecision(0);
	m_speedConfigStepsX->SetValidator(val);
	m_speedConfigStepsY->SetValidator(val);
	m_speedConfigStepsZ->SetValidator(val);
	
	wxTextValidator tVal(wxFILTER_NUMERIC);
	tVal.SetCharIncludes(", ");
	m_cbUCValueFrom->SetValidator(tVal);
	m_metricValueFrom->SetValidator(tVal);
	
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
	GBL_CONFIG->setupSelectorRenderResolution();
}
///////////////////////////////////////////////////////////////////
bool MainFrame::initializeCncControl() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	if ( GBL_CONFIG != NULL ) {
		wxString value;
		
		// initialize display unit
		m_unit->SetStringSelection(GBL_CONFIG->getDefaultDisplayUnitAsStr());
		updateUnit();
		
		// initialize com port
		GBL_CONFIG->getDefaultPort(value);
		m_portSelector->SetStringSelection(value);
		defaultPortName.assign(value);
		
		// initialize update interval
		GBL_CONTEXT->setUpdateInterval(m_displayInterval->GetValue());
	}
	 
	// initialize the postion controls
	setControllerZero(true, true, true);
	updateCncConfigTrace();
	
	// z slider
	m_zView->updateView(cnc->requestControllerPos().getZ() * GBL_CONFIG->getDisplayFactZ(GBL_CONFIG->getDisplayUnit()));
	
	//initilaize debug state
	if ( m_menuItemDebugSerial->IsChecked() ) 	cnc->enableSpyOutput(true);
	else				    					cnc->enableSpyOutput(false); 
	
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
void MainFrame::updateUnit() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	CncConfig* cncConfig = CncConfig::getGlobalCncConfig();
	wxString unit = m_unit->GetValue();

	wxFloatingPointValidator<float> val(3, NULL, wxNUM_VAL_DEFAULT );//, wxNUM_VAL_ZERO_AS_BLANK);
	
	double xLimit = +cncConfig->getMaxDimensionX()/2; // [mm]
	double yLimit = +cncConfig->getMaxDimensionY()/2; // [mm]
	double zLimit = +cncConfig->getMaxDimensionZ()/2; // [mm]
	
	double valueX = m_xManuallySlider->GetValue();
	double valueY = m_yManuallySlider->GetValue();
	double valueZ = m_zManuallySlider->GetValue();
	
	wxString xAppPos(m_xAxis->GetValue());
	wxString yAppPos(m_yAxis->GetValue());
	wxString zAppPos(m_zAxis->GetValue());
	wxString xCtlPos(m_xAxisCtl->GetValue());
	wxString yCtlPos(m_yAxisCtl->GetValue());
	wxString zCtlPos(m_zAxisCtl->GetValue());
	
	int precision = 0;

	if ( unit == "mm" ) { 
		cncConfig->setDisplayUnit(CncMetric); 
		precision = 3;
		
		valueX *= GBL_CONFIG->getDisplayFactX();
		valueY *= GBL_CONFIG->getDisplayFactY();
		valueZ *= GBL_CONFIG->getDisplayFactZ();
		
		m_metricX->SetValue(wxString::Format("%4.3lf", valueX));
		m_metricY->SetValue(wxString::Format("%4.3lf", valueY));
		m_metricZ->SetValue(wxString::Format("%4.3lf", valueZ));
		
		long v; 
		xAppPos.ToLong(&v);
		m_xAxis->ChangeValue(wxString::Format("%4.3lf", (double)(v * GBL_CONFIG->getDisplayFactX())));
		
		yAppPos.ToLong(&v);
		m_yAxis->ChangeValue(wxString::Format("%4.3lf", (double)(v * GBL_CONFIG->getDisplayFactY())));
		
		zAppPos.ToLong(&v);
		m_zAxis->ChangeValue(wxString::Format("%4.3lf", (double)(v * GBL_CONFIG->getDisplayFactZ())));
		
		xCtlPos.ToLong(&v);
		m_xAxisCtl->ChangeValue(wxString::Format("%4.3lf", (double)(v * GBL_CONFIG->getDisplayFactX())));
		
		yCtlPos.ToLong(&v);
		m_yAxisCtl->ChangeValue(wxString::Format("%4.3lf", (double)(v * GBL_CONFIG->getDisplayFactY())));
		
		zCtlPos.ToLong(&v);
		m_zAxisCtl->ChangeValue(wxString::Format("%4.3lf", (double)(v * GBL_CONFIG->getDisplayFactZ())));
		
	} else {
		xLimit *= GBL_CONFIG->getCalculationFactX();
		yLimit *= GBL_CONFIG->getCalculationFactY();
		zLimit *= GBL_CONFIG->getCalculationFactZ();
		
		cncConfig->setDisplayUnit(CncSteps);
		
		valueX *= GBL_CONFIG->getCalculationFactX();
		valueY *= GBL_CONFIG->getCalculationFactY();
		valueZ *= GBL_CONFIG->getCalculationFactZ();
		
		m_metricX->SetValue(wxString::Format("%ld", (long)valueX));
		m_metricY->SetValue(wxString::Format("%ld", (long)valueY));
		m_metricZ->SetValue(wxString::Format("%ld", (long)valueZ));
		
		double v; 
		xAppPos.ToDouble(&v);
		m_xAxis->ChangeValue(wxString::Format("%ld",    (long)(v * GBL_CONFIG->getCalculationFactX())));
		
		yAppPos.ToDouble(&v);
		m_yAxis->ChangeValue(wxString::Format("%ld",    (long)(v * GBL_CONFIG->getCalculationFactY())));
		
		zAppPos.ToDouble(&v);
		m_zAxis->ChangeValue(wxString::Format("%ld",    (long)(v * GBL_CONFIG->getCalculationFactZ())));
		
		xCtlPos.ToDouble(&v);
		m_xAxisCtl->ChangeValue(wxString::Format("%ld", (long)(v * GBL_CONFIG->getCalculationFactX())));
		
		yCtlPos.ToDouble(&v);
		m_yAxisCtl->ChangeValue(wxString::Format("%ld", (long)(v * GBL_CONFIG->getCalculationFactY())));
		
		zCtlPos.ToDouble(&v);
		m_zAxisCtl->ChangeValue(wxString::Format("%ld", (long)(v * GBL_CONFIG->getCalculationFactZ())));
		
		precision = 0;
	}
	
	m_xManuallySlider->SetMin(-xLimit);
	m_xManuallySlider->SetMax(+xLimit);
	m_yManuallySlider->SetMin(-yLimit);
	m_yManuallySlider->SetMax(+yLimit);
	m_zManuallySlider->SetMin(-zLimit);
	m_zManuallySlider->SetMax(+zLimit);
	
	m_xManuallySlider->SetValue(valueX);
	m_yManuallySlider->SetValue(valueY);
	m_zManuallySlider->SetValue(valueZ);
	
	val.SetPrecision(precision);
	val.SetRange(-xLimit, +xLimit);
	
	m_metricX->SetValidator(val);
	m_metricY->SetValidator(val);
	m_metricZ->SetValidator(val);
	
	// manual control
	m_mmUnitX->SetLabel(unit);
	m_mmUnitY->SetLabel(unit);
	m_mmUnitZ->SetLabel(unit);

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
	info.SetVersion(globalStrings.programVersion);
	info.SetLicence(_("GPL v2 or later"));
	info.AddDeveloper("Hacki Wimmmer");
	info.SetDescription(description);
	info.SetIcon(icon);
	info.SetCopyright(globalStrings.copyRight);
	::wxAboutBox(info);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectPort(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( lastPortName != m_portSelector->GetStringSelection() ) {
		connectSerialPortDialog();
	}
}
///////////////////////////////////////////////////////////////////
bool MainFrame::connectSerialPortDialog() {
///////////////////////////////////////////////////////////////////
	CncConnectProgress dlg(this);
	
	return ( dlg.ShowModal() == wxID_OK);
}
///////////////////////////////////////////////////////////////////
bool MainFrame::connectSerialPort() {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	
	const wxBitmap bmpC = ImageLib16().Bitmap("BMP_CONNECTED");
	const wxBitmap bmpD = ImageLib16().Bitmap("BMP_DISCONNECTED");
	m_connect->SetBitmap(bmpD);
	m_connect->Refresh();
	m_connect->Update();
	
	startAnimationControl();
	m_serialTimer->Stop();
	
	if ( m_clearSerialSpyOnConnect->GetValue() )
		clearSerialSpy();
		
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
		return false;
		
	statisticsPane->setCncControl(cnc);
	
	initializeCncControl();
	selectSerialSpyMode();
	clearPositionSpy();
	
	lastPortName.clear();
	
	bool ret = false;
	if ( (ret = cnc->connect(serialFileName)) == true )  {

		lastPortName.assign(sel);
		clearMotionMonitor();
		
		if ( (ret = cnc->setup()) == true ) {
			
			cnc->isEmulator() ? setRefPostionState(true) : setRefPostionState(false);
			updateCncConfigTrace();
			decorateSwitchToolOnOff(cnc->getToolState());
			selectSerialSpyMode();
			m_connect->SetBitmap(bmpC);
			m_serialTimer->Start();
			
			if ( cnc->canProcessIdle() ) {
				m_miRqtIdleMessages->Check(GBL_CONFIG->getRequestIdleRequestFlag());
				m_miRqtIdleMessages->Enable(true);
			}
		}
		
		setControllerZero(true, true, true);
	}
	
	updateSetterList();
	decoratePortSelector();
	m_connect->Refresh();
	m_connect->Update();
	
	stopAnimationControl();
	enableControls();

	return ret;
}
///////////////////////////////////////////////////////////////////
const wxString& MainFrame::createCncControl(const wxString& sel, wxString& serialFileName) {
///////////////////////////////////////////////////////////////////
	// disconnect and delete the current cnc control
	if ( cnc != NULL ) {
		cnc->disconnect();
		delete cnc;
	}
	
	serialFileName.clear();
	cnc = NULL;
	
	struct InitialSetup {
		wxString serialFileName		= "";
		bool probeMode				= false;
		bool secureDlg				= false;
		bool speedControl			= false;
		bool pathListEntries		= false;
		bool moveSequences			= false;
	} setup;
	
	if ( sel == _portEmulatorNULL ) {
		cnc = new CncControl(CncEMU_NULL);
		
		setup.serialFileName.assign("dev/null");
		setup.probeMode			= true;
		setup.secureDlg			= false;
		setup.speedControl		= false;
		setup.pathListEntries	= true;
		setup.moveSequences		= true;
	} 
	else if ( sel == _portEmulatorTEXT ) {
		cnc = new CncControl(CncEMU_TXT);
		
		setup.serialFileName.assign(CncFileNameService::getCncOutboundTxtFileName());
		setup.probeMode			= true;
		setup.secureDlg			= false;
		setup.speedControl		= true;
		setup.pathListEntries	= true;
		setup.moveSequences		= true;
	} 
	else if ( sel == _portEmulatorSVG ) {
		cnc = new CncControl(CncEMU_SVG);
		
		setup.serialFileName.assign(CncFileNameService::getCncOutboundSvgFileName());
		setup.probeMode			= true;
		setup.secureDlg			= false;
		setup.speedControl		= true;
		setup.pathListEntries	= true;
		setup.moveSequences		= true;
	}
	else if ( sel == _portEmulatorGCODE ) {
		cnc = new CncControl(CncEMU_GCODE);
		
		setup.serialFileName.assign(CncFileNameService::getCncOutboundGCodeFileName());
		setup.probeMode			= true;
		setup.secureDlg			= false;
		setup.speedControl		= true;
		setup.pathListEntries	= true;
		setup.moveSequences		= true;
	} 
	else if ( sel == _portEmulatorBIN) {
		cnc = new CncControl(CncEMU_BIN);
		
		setup.serialFileName.assign(CncFileNameService::getCncOutboundBinFileName());
		setup.probeMode			= true;
		setup.secureDlg			= false;
		setup.speedControl		= true;
		setup.pathListEntries	= true;
		setup.moveSequences		= true;
	} 
	else {
		cnc = new CncControl(CncPORT);
		
		setup.serialFileName.assign(wxString::Format("\\\\.\\%s", sel));
		setup.probeMode			= false;
		setup.secureDlg			= true;
		setup.speedControl		= true;
		setup.pathListEntries	= false;
		setup.moveSequences		= false;
	}
	
	// config setup
	serialFileName.assign(setup.serialFileName);
	GBL_CONTEXT->setProbeMode(setup.probeMode);
	decorateSecureDlgChoice(setup.secureDlg);
	decorateSpeedControlBtn(setup.speedControl);
	
	if ( cncPreprocessor != NULL ) {
		cncPreprocessor->enablePathListEntries(setup.pathListEntries);
		cncPreprocessor->enableMoveSequences(setup.moveSequences);
	}
	
	// add on
	const bool probeMode = GBL_CONTEXT->isProbeMode();
	if ( speedMonitor )
		speedMonitor->activate(!probeMode);
	
	return serialFileName;
}
///////////////////////////////////////////////////////////////////
void MainFrame::decorateOutboundEditor(const char* fileName) {
///////////////////////////////////////////////////////////////////
	wxString url(wxString::Format("%s%s", CncFileNameService::getDatabaseDir(), "NoSerialContentAvailable.svg"));
	if ( wxFileName::Exists(url) == false )
		url.assign("about:blank");

	if ( fileName == NULL ) {
		outboundEditorSvgView->loadFile(url);
		m_simpleBookOutBoundEditor->SetSelection(1);
		
		outboundEditor->clearContent();
		outboundFilePreview->selectEmptyPreview();
		
		return;
	}
	
	if ( wxFileName::Exists(fileName) == false ) {
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
	
	// enable menu bar
	enableMenuItems(state);
	
	// enable all relevant controls
	enableGuiControls(state);
	
	//enable manually controls
	enableTestControls(state);
	
	// run control
	enableRunControls(state);
	
	// at least update motion monitor
	if ( state == true )
		motionMonitor->display();
}
///////////////////////////////////////////////////////////////////
void MainFrame::connect(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	connectSerialPortDialog();
}
///////////////////////////////////////////////////////////////////
void MainFrame::clearLogger(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_logger->Clear();
}
///////////////////////////////////////////////////////////////////
void MainFrame::setControllerZero(bool x, bool y, bool z) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	clearPositionSpy();
	cnc->resetClientId();
	cnc->setZeroPos(x, y, z);
	setRefPostionState(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectUnit(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	updateUnit();
}
///////////////////////////////////////////////////////////////////
int MainFrame::showReferencePositionDlg(wxString msg) {
///////////////////////////////////////////////////////////////////
	wxASSERT(refPositionDlg);
	secureRunDlg->enableControls(false);
	
	refPositionDlg->setMessage(msg);
	refPositionDlg->setMeasurePlateThickness(GBL_CONFIG->getMeasurePlateThickness());
	
	activateGamepadNotifications(true);
	int ret = refPositionDlg->ShowModal();
	
	if ( ret == wxID_OK ) {
		
		double wpt = refPositionDlg->getWorkpieceThickness();
		GBL_CONFIG->setReferenceIncludesWpt(cnc::dblCompareNull(wpt) == false);
		GBL_CONFIG->setWorkpieceThickness(wpt);
		GBL_CONFIG->setReferencePositionMode(refPositionDlg->getReferenceMode());
		updateCncConfigTrace();
		
		motionMonitor->clear();
		
		setControllerZero(refPositionDlg->shouldZeroX(), 
		                  refPositionDlg->shouldZeroY(), 
						  refPositionDlg->shouldZeroZ()
						 );
						 
	} 
	else {
		cnc::cex1 << " Set reference position aborted . . . " << std::endl;
	}
	
	//selectMonitorBookCncPanel();
	secureRunDlg->enableControls(true);
	return ret;
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateCncConfigTrace() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	cnc->updateCncConfigTrace();
	
	m_infoToolDiameter->SetLabel(wxString::Format("%.3lf", GBL_CONFIG->getToolDiameter(GBL_CONFIG->getCurrentToolId())));
	m_zView->updateView(cnc->requestControllerPos().getZ() * GBL_CONFIG->getDisplayFactZ(GBL_CONFIG->getDisplayUnit()));
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
	
	showReferencePositionDlg(msg);
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
	
	GBL_CONTEXT->setOnlineUpdateCoordinates(m_menuItemUpdCoors->IsChecked());
	GBL_CONTEXT->setAllowEventHandling(m_menuItemAllowEvents->IsChecked());
	GBL_CONTEXT->setOnlineUpdateDrawPane(m_menuItemUpdDraw->IsChecked());
	
	cnc->enableSpyOutput(m_menuItemDebugSerial->IsChecked());
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
const char* MainFrame::getCurrentTemplateFormatName(const char* fileName) {
///////////////////////////////////////////////////////////////////
	switch ( getCurrentTemplateFormat(fileName) ) {
		case TplManual:
		case TplTest:	return "";
		
		default: 	{
						wxFileName fn;
						if ( fileName == NULL )	fn.Assign(getCurrentTemplatePathFileName());
						else					fn.Assign(fileName);
						
						wxString ext(fn.GetExt());
						ext.MakeUpper();
						return ext;
					}
	}
	
	return "";
}
///////////////////////////////////////////////////////////////////
TemplateFormat MainFrame::getCurrentTemplateFormat(const char* fileName) {
///////////////////////////////////////////////////////////////////
	unsigned int sel = m_mainViewSelector->GetSelection();
	if ( sel == (unsigned int)MainBookSelection::VAL::MANUEL_PANEL )
		return TplManual;
		
	if ( sel == (unsigned int)MainBookSelection::VAL::TEST_PANEL )
		return TplTest;
		
	wxString fn;
	if ( fileName == NULL )	fn.assign(getCurrentTemplatePathFileName());
	else					fn.assign(fileName);
	
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
void MainFrame::decorateExtTemplatePages(TemplateFormat tf) {
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
								show = false;
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
	// First select the template page to get the rigth result 
	// from getCurrentTemplateFormat
	selectMainBookSourcePanel(sourcePageToSelect);
	
	bool ret = false;
	switch ( getCurrentTemplateFormat() ) {
		
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
	
	if ( ret == true ) {
		decorateExtTemplatePages(getCurrentTemplateFormat());
		outboundFilePreview->selectEmptyPreview();
		
		if ( inboundFileParser != NULL )
			inboundFileParser->clearControls();
		
		clearMotionMonitor();
		
		introduceCurrentFile(sourcePageToSelect);
	}
	
	updateFileContentPosition(0, 0);
	return ret;
}
///////////////////////////////////////////////////////////////////
void MainFrame::prepareMotionMonitorViewType(const CncDimensions type) {
///////////////////////////////////////////////////////////////////
	switch ( type ) {
		case CncDimensions::CncDimension2D:		motionMonitor->viewTop();
												break;
												
		default:								motionMonitor->viewIso1();
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::introduceCurrentFile(int sourcePageToSelect) {
///////////////////////////////////////////////////////////////////
	lruFileList.addFile(getCurrentTemplatePathFileName());
	fileView->selectFileInList(getCurrentTemplatePathFileName());
	
	selectMainBookSourcePanel(sourcePageToSelect);
	
	// publish model type
	const GLContextBase::ModelType mt = sourceEditor->getModelType();
	motionMonitor->setModelType(mt);
	
	prepareMotionMonitorViewType(sourceEditor->getModelViewType());
}
///////////////////////////////////////////////////////////////////
void MainFrame::prepareNewTemplateFile() {
///////////////////////////////////////////////////////////////////
	const TemplateFormat tf = getCurrentTemplateFormat();

	sourceEditor->SetReadOnly(false);
	sourceEditor->clearContent();
	sourceEditor->prepareNewTemplateFile(tf);
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
                                "SVG Files (*.svg)|*.svg|GCode Files (*.ngc;*.gcode)|*.ngc;*.gcode", 
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
                                "SVG Files (*.svg)|*.svg|GCode Files (*.ngc;*.gcode)|*.ngc;*.gcode|Binary Tpl Files (*.bct)|*.bct", 
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
void MainFrame::reloadTemplate(int sourcePageToSelect) {
///////////////////////////////////////////////////////////////////
	if ( !openFile(sourcePageToSelect) ) {
		std::cerr << "Error while reloding template: " << getCurrentTemplateFileName().c_str() << std::endl;
	}
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
	if ( ret == wxID_YES ) {
		if ( wxRemoveFile(fn) == false ) {
			std::cerr << "MainFrame::removeTemplateFromButton: failed!" << std::endl;
			return;
		}
		
		std::clog << "Template: " << fn << " was removed." << std::endl;
		
		// update file lists
		lruFileList.removeFile(fn);
		lruFileList.save(lruStore);
		
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
	lruFileList.removeFile(oldFileName);
	lruFileList.addFile(newFileName);
	lruFileList.save(lruStore);
	
	fileView->update();
	
	m_inputFileName->SetValue(newTplFile.GetFullName());
	m_inputFileName->SetHint(tplFile.GetPathWithSep());
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
void MainFrame::openTemplateSvgExtern(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString tool;
	
	switch ( getCurrentTemplateFormat() ) {
		case TplBinary:
					// binary files are readonly, thereforenow saveFile() necessary here
					CncConfig::getGlobalCncConfig()->getBINFileViewer(tool);
					openFileExtern(tool, getCurrentTemplatePathFileName());
					break;
					
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
bool MainFrame::saveFile() {
///////////////////////////////////////////////////////////////////
	wxASSERT(sourceEditor);
	
	// First select the template page to get the rigth result 
	// by getCurrentTemplateFormat
	selectMainBookSourcePanel();
	
	// Deactivate observer
	CncTemplateObserver::Deactivator observerDeactivator(templateObserver);
	
	bool ret = sourceEditor->saveFile();
	if ( ret == true )
		prepareAndShowMonitorTemplatePreview(true);
	
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
bool MainFrame::processVirtualTemplate() {
///////////////////////////////////////////////////////////////////
	wxASSERT(m_inputFileName);

	bool ret;
	FileParser::UserAgentOutputControls oc;
	oc.detailInfo 		= m_dvListCtrlSvgUADetailInfo;
	oc.inboundPathList 	= m_dvListCtrlSvgUAInboundPathList;
	oc.useDirectiveList = m_dvListCtrlSvgUAUseDirective;
	
	if ( m_menuItemDisplayUserAgent->IsChecked() == true )
		inboundFileParser->setUserAgentControls(oc);
		
	inboundFileParser->setInboundSourceControl(sourceEditor);
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
bool MainFrame::isGamepadDialogShown() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncGameportDlg);
	return cncGameportDlg->IsShown();
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
		
	bool changed = false;
	if ( state == false ) {
		if ( gamepadThread->IsRunning() ) {
			gamepadThread->Pause();
			changed = true;
		}
	} else {
		if ( gamepadThread->IsPaused() ) {
			gamepadThread->Resume();
			changed = true;
		}
	}
	
	decorateGamepadState(gamepadThread->IsRunning());
	
	if ( changed == false)
		;
	/*
	//if ( gamepadThread->IsPaused() )	cnc::trc.logInfoMessage( changed ? "Gamepad thread deactivated" : "Gamepad thread stays deactivated");
	//else								cnc::trc.logInfoMessage( changed ? "Gamepad thread activated"   : "Gamepad thread stays activated");
	if ( gamepadThread->IsPaused() )	std::cout << ( changed ? "Gamepad thread deactivated" : "Gamepad thread stays deactivated") << std::endl;
	else								std::cout << ( changed ? "Gamepad thread activated"   : "Gamepad thread stays activated") << std::endl;
	*/
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processBinaryTemplate() {
///////////////////////////////////////////////////////////////////
	if ( inboundFileParser != NULL )
		delete inboundFileParser;
		
	CncGampadDeactivator cpd(this);
		
	inboundFileParser = new BinaryFileParser(getCurrentTemplatePathFileName().c_str(), new BinaryPathHandlerCnc(cnc));
	return processVirtualTemplate();
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processSVGTemplate() {
///////////////////////////////////////////////////////////////////
	if ( inboundFileParser != NULL )
		delete inboundFileParser;
		
	CncGampadDeactivator cpd(this);
		
	inboundFileParser = new SVGFileParser(getCurrentTemplatePathFileName().c_str(), cnc);
	return processVirtualTemplate();
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processGCodeTemplate() {
///////////////////////////////////////////////////////////////////
	if ( inboundFileParser != NULL )
		delete inboundFileParser;
		
	CncGampadDeactivator cpd(this);
	
	inboundFileParser = new GCodeFileParser(getCurrentTemplatePathFileName().c_str(), new GCodePathHandlerCnc(cnc));
	return processVirtualTemplate();
}
///////////////////////////////////////////////////////////////////
void MainFrame::changeManuallySpeedSlider(wxScrollEvent& event) {
///////////////////////////////////////////////////////////////////
	changeManuallySpeedValue();
}
///////////////////////////////////////////////////////////////////
void MainFrame::changeManuallySpeedValue() {
///////////////////////////////////////////////////////////////////
	double fact = m_manuallySpeedSlider->GetValue()/100.0;
	m_manuallySpeedValue->ChangeValue(wxString::Format("%5.1lf", fact * GBL_CONFIG->getMaxSpeedXYZ_MM_MIN()));
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processManualTemplate() {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc != NULL );
	
	if ( inboundFileParser != NULL )
		delete inboundFileParser;
		
	ManuallyParser* p = new ManuallyParser(new ManuallyPathHandlerCnc(cnc));
	inboundFileParser = p;

	ManuallyPathHandlerCnc::MoveDefinition move;
	move.speedMode 		= CncSpeedUserDefined;
	move.absoluteMove	= ( m_mmRadioCoordinates->GetSelection() == 0 );
	move.toolState		= m_checkBoxToolEnabled->GetValue();
	move.correctLimit   = m_manuallyCorrectLimitPos->GetValue();
	
	m_manuallySpeedValue->GetValue().ToDouble(&move.f);
	m_metricX->GetValue().ToDouble(&move.x);
	m_metricY->GetValue().ToDouble(&move.y);
	m_metricZ->GetValue().ToDouble(&move.z);
	
	if ( m_unit->GetValue() == "steps" ) {
		move.x *= GBL_CONFIG->getDisplayFactX();
		move.y *= GBL_CONFIG->getDisplayFactY();
		move.z *= GBL_CONFIG->getDisplayFactZ();
	} 
	
	p->reset(cnc->getCurAppPosMetric());
	p->addMove(move);
	
	bool ret = false;
	if ( isDebugMode == true ) 	ret = p->processDebug();
	else 						ret = p->processRelease();
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processTestTemplate() {
///////////////////////////////////////////////////////////////////
	wxWindow* page = m_testCaseBook->GetCurrentPage();
	if ( page != NULL ) {
		if 		( page == m_testIntervalPage )	return processTestInterval();
		else if ( page == m_testDimensions )	return processTestDimensions();
		//...
		
		wxString msg;
		msg << "This Test (\"";
		msg << m_testCaseBook->GetPageText(m_testCaseBook->GetSelection()); 
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
bool MainFrame::checkIfRunCanBeProcessed(bool confirm) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	if ( evaluatePositions == true && cnc->validateAppAgainstCtlPosition() == false ) {
		
		wxString msg("Validate positions failed\n");
		msg << "\nPC pos        : ";
		msg << cnc->getCurAppPos().getX(); msg << ",";
		msg << cnc->getCurAppPos().getY(); msg << ",";
		msg << cnc->getCurAppPos().getZ();
		msg << "\nController pos: ";
		msg << cnc->requestControllerPos().getX();  msg << ",";
		msg << cnc->requestControllerPos().getY();  msg << ",";
		msg << cnc->requestControllerPos().getZ();
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
		std::clog << "MainFrame::checkIfRunCanBeProcessed(): Limit switch detected" << std::endl;
		return false;
	}
	
	return (confirm == true ? showConfigSummaryAndConfirmRun() : true);
}
///////////////////////////////////////////////////////////////////
bool MainFrame::checkReferencePositionState() {
///////////////////////////////////////////////////////////////////
	const CncDoublePosition refPos(CncStartPositionResolver::getReferencePosition());
	const bool zero = ( cnc->getCurAppPosMetric() != refPos );

	if ( isZeroReferenceValid == false ) {
		const TemplateFormat tf = getCurrentTemplateFormat();
		if ( tf != TplManual && tf != TplTest ) {
			wxString msg("The current reference position isn't valid due to a setup change or it isn't not initialized yet.\n");
			
			const int ret = showReferencePositionDlg(msg);
			if ( ret == wxID_OK && zero == false ) {
				cnc::trc.logInfoMessage("Reference Position is fixed now. Please restart");
				
				// Safety: Always return false in this case because this will
				// stopp the current startet run. 
				return false;
			}
		}
	}
	
	// check current position
	if ( zero ) {
		wxASSERT( cnc != NULL );

		bool openDlg = false;
		switch ( GBL_CONFIG->getRunConfirmationModeAsChar() ) {
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
		else 					ret = dlg.resolve();

		if ( ret == wxID_OK )
			cnc::trc.logInfoMessage("Reference Position is fixed now. Please restart");
		
		// Safety: Always return false in this case because this will
		// stopp the current startet run. 
		if ( openDlg == true )
			return false;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool MainFrame::showConfigSummaryAndConfirmRun() {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
		
	switch ( GBL_CONFIG->getRunConfirmationModeAsChar() ) {
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
	typedef CncSummaryListCtrl::ParameterType PT;
	CncConfig* cc = CncConfig::getGlobalCncConfig();
	
	cncSummaryListCtrl->addHeadline(CncSummaryListCtrl::ParameterType::PT_HEADLINE, "SVG specific Settings");

	cncSummaryListCtrl->addParameter(PT::PT_SVG, "Z axis values:", "", "");
	cncSummaryListCtrl->addParameter(PT::PT_SVG, "  Max durations", 				wxString::Format("%u",		cc->getMaxDurations()),							"#");
	cncSummaryListCtrl->addParameter(PT::PT_SVG, "  Workpiece offset", 				wxString::Format("%4.3f",	cc->getWorkpieceOffset()),						"mm");
	cncSummaryListCtrl->addParameter(PT::PT_SVG, "  Max duration thickness", 		wxString::Format("%4.3f",	cc->getMaxDurationThickness()),					"mm");
	cncSummaryListCtrl->addParameter(PT::PT_SVG, "  Calculated durations", 			wxString::Format("%u",		cc->getDurationCount()),						"#");
	cncSummaryListCtrl->addParameter(PT::PT_SVG, "  Current Z distance", 			wxString::Format("%4.3f",	cc->getCurZDistance()),							"mm");
	cncSummaryListCtrl->addParameter(PT::PT_SVG, "  Wpt is included", 				wxString::Format("%d",		cc->getReferenceIncludesWpt()),					"-");
	
	for (unsigned int i=0; i<cc->getMaxDurations(); i++) {
		if ( cc->getDurationThickness(i) != 0.0 ) {
			cncSummaryListCtrl->addParameter(PT::PT_SVG, 
			                                 wxString::Format("   Duration step[%02u]", i), 
			                                 wxString::Format("%4.3f",	cc->getDurationThickness(i)),
			                                 "mm");
		}
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::collectGCodeSpecificSummary() {
///////////////////////////////////////////////////////////////////
	typedef CncSummaryListCtrl::ParameterType PT;
	CncConfig* cc = CncConfig::getGlobalCncConfig();
	
	cncSummaryListCtrl->addHeadline(CncSummaryListCtrl::ParameterType::PT_HEADLINE, "GCode specific Settings");
	cncSummaryListCtrl->addParameter(PT::PT_GCODE, "Sample . . . .", 					wxString::Format("%4.3f",	cc->getReplyThresholdMetric()),						"xx");
}
///////////////////////////////////////////////////////////////////
void MainFrame::collectSummary() {
///////////////////////////////////////////////////////////////////
	if ( cnc == NULL )
		return;
		
	typedef CncSummaryListCtrl::ParameterType PT;
		
	CncConfig* cc = CncConfig::getGlobalCncConfig();
	cncSummaryListCtrl->clear();
	
	cncSummaryListCtrl->addHeadline(PT::PT_HEADLINE, "Common Settings");
	cncSummaryListCtrl->addParameter(PT::PT_COMMON, "Default Tool", 					cc->getDefaultToolParamAsString(),												"-");
	cncSummaryListCtrl->addParameter(PT::PT_COMMON, "Workpiece thickness", 				wxString::Format("%4.3f", 	cc->getWorkpieceThickness()),						"mm");
	//cncSummaryListCtrl->addParameter(PT::PT_COMMON, "Curve lib resolution", 			wxString::Format("%0.3f", 	cc->getRenderResolution()),							"-");
	cncSummaryListCtrl->addParameter(PT::PT_COMMON, "Default Papid speed", 				wxString::Format("%4.3f", 	cc->getDefaultRapidSpeed_MM_MIN()),					"mm/min");
	cncSummaryListCtrl->addParameter(PT::PT_COMMON, "Default Work speed", 				wxString::Format("%4.3f", 	cc->getDefaultWorkSpeed_MM_MIN()),					"mm/min");
	cncSummaryListCtrl->addParameter(PT::PT_COMMON, "Reply Threshold", 					wxString::Format("%4.3f",	cc->getReplyThresholdMetric()),						"mm");

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
	if ( updateManagerThread == NULL )
		return;
		
	if ( updateManagerThread->IsPaused() == true ) {
		wxCriticalSectionLocker enter(pUpdateManagerThreadCS);
		updateManagerThread->Resume();
	}
	
	if ( setterList->IsFrozen() == false )
		setterList->Freeze();
		
	int sizeBefore = setterList->getItemCount();
	if ( updateManagerThread->fillSetterList(setterList) > 0 ) {
		if ( sizeBefore == 0 )
			setterList->updateColumnWidth();
		
		if ( setterList->IsShownOnScreen() ) {
			setterList->Refresh();
			setterList->EnsureVisible(0L);
		}
	}

	if ( setterList->IsFrozen() == true )
		setterList->Thaw();

	m_setterListCount->SetValue(wxString::Format("# %ld", (long)setterList->GetItemCount()));
}
///////////////////////////////////////////////////////////////////
void MainFrame::nootebookConfigChanged(wxListbookEvent& event) {
///////////////////////////////////////////////////////////////////
	// check if currentla a run is active and return if so to avoid 
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
					
			case OutboundCfgSelection::VAL::CNC_SETTER_PANEL:
					updateSetterList();
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
bool MainFrame::processTemplateWrapper(bool confirm) {
///////////////////////////////////////////////////////////////////
	try {
		wxASSERT(cnc);
		if ( cnc->isInterrupted() == true ) {
			std::cerr << "MainFrame::processTemplateWrapper(): Run aborted - Interrupt detected!" << std::endl;
			return false;
		}
		
		bool ret = true;
		
		CncRunEventFilter cef;
		
		// deactivate idle requests
		CncTransactionLock ctl(this);
		
		// Deactivate observer
		CncTemplateObserver::Deactivator observerDeactivator(templateObserver);
		
		// it's very import to deactivate the notifications during a run
		// because instead every config change (sc()) will release a notification
		// this will be the case for example if the SVG path handler changes
		// the z -axis values . . .
		// as a result the processing slows down significantly.
		CncConfig::NotificationDeactivator cfgNotDeactivation;
		
		
		wxString fn (getCurrentTemplatePathFileName());
		if ( fn.IsEmpty() == true )
			return false;

		clearMotionMonitor();
		decorateOutboundSaveControls(false);
		cncPreprocessor->clearAll();
		
		if ( checkReferencePositionState() == false )
			return false;
		
		if ( cnc->isReadyToRun() == false ) {
			std::cerr << "MainFrame::processTemplateWrapper: Controller isn't ready to run: Run was rejected!" << std::endl;
			ret = false;
		}
		
		if ( ret == true )
			ret = checkIfRunCanBeProcessed(confirm);
			
		if ( ret == true ) {
			
			// process
			if ( useSecureRunDlg == true && isDebugMode == false ) {
				wxASSERT(secureRunDlg);
				if ( secureRunDlg->IsShown() )
					secureRunDlg->Show(false);
					
				ret = (secureRunDlg->ShowModal() == wxID_OK);
				
			} 
			else {
				CncSpeedMonitorRunner smr(speedMonitor, GBL_CONFIG->getMaxSpeedXYZ_MM_MIN());
				ret = processTemplateIntern();
			}
			
			motionMonitor->updateMonitorAndOptions();
			statisticsPane->updateReplayPane();
		}
		else {
			std::cerr << "MainFrame::processTemplateWrapper(): checkIfRunCanBeProcessed() failed"<< std::endl;
		}
		
		// prepare final statements
		wxString probeMode(GBL_CONTEXT->isProbeMode() ? "ON" :"OFF");
		if ( ret == false) {
			wxString hint("not successfully");
			cnc::cex1 << wxString::Format("%s - Processing(probe mode = %s) finished %s . . .", wxDateTime::UNow().FormatISOTime(), probeMode, hint) << std::endl;
			ctl.setErrorMode();
			
		} 
		else {
			std::clog << wxString::Format("%s - Processing(probe mode = %s) finished successfully . . .", wxDateTime::UNow().FormatISOTime(), probeMode) << std::endl;
		}
		
		cnc->traceSpeedInformation();
		
		Serial::Trigger::EndRun endRun;
		endRun.succcess = ret;
		cnc->processTrigger(endRun);

		decorateOutboundSaveControls(cnc->isOutputAsTemplateAvailable());

		return ret;
	}
	catch (const CncInterruption& ex) {
		std::cerr << "MainFrame::processTemplateWrapper(): Exception received:" 
		          << std::endl
				  << ex.what()
				  << std::endl;
	}
	catch (...) {
		std::cerr << "MainFrame::processTemplateWrapper(): Unhandled Exception!" << std::endl;
	}
	
	return false;
}
///////////////////////////////////////////////////////////////////
// don't call this method directly, instead use processTemplateWrapper
bool MainFrame::processTemplateIntern() {
///////////////////////////////////////////////////////////////////
	startAnimationControl();
	
	Serial::Trigger::BeginRun begRun;
		begRun.parameter.SRC.fileName		= getCurrentTemplatePathFileName();
		begRun.parameter.SRC.fileType		= getCurrentTemplateFormatName();
		begRun.parameter.SET.hardwareResX 	= GBL_CONFIG->getDisplayFactX();
		begRun.parameter.SET.hardwareResY 	= GBL_CONFIG->getDisplayFactY();
		begRun.parameter.SET.hardwareResZ 	= GBL_CONFIG->getDisplayFactZ();
		begRun.parameter.PRC.user			= "Hacki Wimmer";
	cnc->processTrigger(begRun);
	
	if ( m_clearSerialSpyBeforNextRun->GetValue() )
		clearSerialSpy();
	
	clearPositionSpy();
	
	showAuiPane("Outbound");
	selectMonitorBookCncPanel();
	
	// select draw pane
	m_outboundNotebook->SetSelection(OutboundSelection::VAL::MOTION_MONITOR_PANAL);
		
	// select template Page
	if ( m_mainViewSelector->GetSelection() != MainBookSelection::VAL::MANUEL_PANEL && 
	     m_mainViewSelector->GetSelection() != MainBookSelection::VAL::TEST_PANEL && 
	     m_mainViewSelector->GetSelection() != MainBookSelection::VAL::SOURCE_PANEL &&
	     m_mainViewSelector->GetSelection() != MainBookSelection::VAL::SETUP_PANEL) {
		selectMainBookSourcePanel();
	}
	
	GBL_CONTEXT->timestamps.logTotalTimeStart();
	motionMonitor->pushProcessMode();
	
	updateStepDelay();
	disableControls();
	resetMinMaxPositions();
	updateCncConfigTrace();
	
	GBL_CONTEXT->setAllowEventHandling(true);
	cnc->resetSetterMap();
	cnc->processSetter(PID_SEPARATOR, SEPARARTOR_RUN);
	cnc->enableStepperMotors(true);
	
	bool ret = false;
	switch ( getCurrentTemplateFormat() ) {
		
		case TplBinary:
			if ( checkIfTemplateIsModified() == false )
				break;
			clearMotionMonitor();
			// measurement handling will be done by the corespondinf file parser
			ret = processBinaryTemplate();
			break;
			
		case TplSvg:
			if ( checkIfTemplateIsModified() == false )
				break;
			clearMotionMonitor();
			// measurement handling will be done by the corespondinf file parser
			ret = processSVGTemplate();
			break;
			
		case TplGcode:
			if ( checkIfTemplateIsModified() == false )
				break;
			clearMotionMonitor();
			// measurement handling will be done by the corespondinf file parser
			ret = processGCodeTemplate();
			break;
			
		case TplManual:
			ret = processManualTemplate();
			break;
			
		case TplTest:
			clearMotionMonitor();
			cnc->startSerialMeasurement();
			ret = processTestTemplate();
			cnc->stopSerialMeasurement();
			break;
			
		default:
			; // do nothing
	}
	
	// Check positions
	if ( cnc->validateAppAgainstCtlPosition() == false ) {
		if ( cnc->isInterrupted() == false ) {
			std::cerr << "Validate positions failed" << std::endl;
			std::cerr << "PC pos        : " << cnc->getCurAppPos() << std::endl;
			std::cerr << "Controller pos: " << cnc->requestControllerPos() << std::endl;
			setRefPostionState(false);
		}
		
		ret = false;
	}
	
	if ( cnc->getPositionOutOfRangeFlag() == true ) {
		//if ( cnc->isInterrupted() == false ) {
			std::cerr << "Out of range: During the last run the position limits were exceeded." << std::endl;
			CncLongPosition::Watermarks wm = cnc->getWaterMarks();
			CncLongPosition min(wm.xMin, wm.yMin, wm.zMin);
			CncLongPosition max(wm.xMax, wm.yMax, wm.zMax);
			
			cnc->isPositionOutOfRange(min, true);
			cnc->isPositionOutOfRange(max, true);
			
			setRefPostionState(false);
		//}
	}
	
	cnc->enableStepperMotors(false);
	
	motionMonitor->popProcessMode();
	
	if ( ret )
		cnc->updateDrawControl();
		
	logTimeConsumed();
	statisticsPane->logStatistics();
	displayReport(1);
		
	enableControls();
	stopAnimationControl();
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void MainFrame::logTimeConsumed() {
///////////////////////////////////////////////////////////////////
	GBL_CONTEXT->timestamps.logTotalTimeEnd();
	wxString value;
	m_cmdDuration->ChangeValue(GBL_CONTEXT->timestamps.getTotalTimeConsumedFormated(value));

	std::stringstream ss;
	ss << GBL_CONTEXT->timestamps;
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
bool MainFrame::checkIfTemplateIsModified() {
///////////////////////////////////////////////////////////////////
	wxString msg(wxString::Format("The current template file\n\n '%s'\n\nwas modified. Save it?", getCurrentTemplatePathFileName()));
	
	if ( sourceEditor->IsModified() == true ) {
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
	if ( lastTemplateFileNameForPreview == getCurrentTemplatePathFileName() && sourceEditor->GetModify() == false)
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
							
		case TplBinary:		if ( BinaryFileParser::extractSourceContentAsFile(getCurrentTemplatePathFileName(), tfn) == true ) {
								openMonitorPreview(tfn);
								// no further copy necessay
								return;
								
							} else {
								std::cerr << "MainFrame::prepareAndShowMonitorTemplatePreview(): Can't create preview!" << std::endl;
								return;
								
							}
							
		default:			// do nothing;
							break;
	}
	
	if ( tfn.IsEmpty() ) {
		std::cerr << "MainFrame::prepareAndShowMonitorTemplatePreview(): Invalid file name!" << std::endl;
		return;
	}
	
	// create a copy to avoid a modification of sourceEditor
	wxTextFile file(tfn);
	if ( !file.Exists() )
		file.Create();
	
	if ( file.Open() ) {
		file.Clear();
		
		for (long i=0; i<sourceEditor->GetNumberOfLines(); i++) {
			wxString line = sourceEditor->GetLineText(i);
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
	cnc->interrupt("Emergency Stop detected");
	setRefPostionState(false);
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
	cnc->processCommand(CMD_PRINT_VERSION, ss);
	cnc::trc.logInfoMessage(ss);
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestHeartbeat(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	std::stringstream dummy;
	cnc->processCommand(CMD_HEARTBEAT, dummy);
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestConfig(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
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
	CncLongPosition pos = cnc->requestControllerPos();
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
void MainFrame::requestReset(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	requestReset();
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestReset() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	m_logger->Clear();
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
	if ( motionMonitor != NULL ) {
		motionMonitor->setCurrentClientId(y);
		motionMonitor->display();
	}
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
	wxASSERT( cnc );
	CncTransactionLock ctl(this);
	
	if ( isZeroReferenceValid == false ) {
		cnc::trc.logError("The current reference position isn't valid. Therefore, can't move home");
		return;
	}
	
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
		default:	std::cerr << "MainFrame::moveHome: invalid selection!" << std::endl;
	}
	
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveXToMid(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	CncTransactionLock ctl(this);
	
	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->moveXToMid();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveYToMid(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	CncTransactionLock ctl(this);
	
	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->moveYToMid();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveZToMid(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	CncTransactionLock ctl(this);
	
	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->moveZToMid();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveToZeroXY(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	CncTransactionLock ctl(this);

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
	wxASSERT( cnc );
	CncTransactionLock ctl(this);

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
	wxASSERT( cnc );
	CncTransactionLock ctl(this);

	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->simpleMoveZToZeroPos();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveXToMin(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	CncTransactionLock ctl(this);

	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->moveXToMinLimit();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveXToMax(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	CncTransactionLock ctl(this);

	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->moveXToMaxLimit();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveYToMin(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	CncTransactionLock ctl(this);

	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->moveYToMinLimit();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveYToMax(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	CncTransactionLock ctl(this);

	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->moveYToMaxLimit();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveZToMin(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	CncTransactionLock ctl(this);

	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->moveZToMinLimit();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveZToMax(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	CncTransactionLock ctl(this);
	
	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->moveZToMaxLimit();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveZToTop(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	CncTransactionLock ctl(this);

	selectMonitorBookCncPanel();
	
	disableControls();
	cnc->moveZToTop();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveZToBottom(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	CncTransactionLock ctl(this);

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
		val = wxString::Format(wxT("%d"),    (int)(m_xManuallySlider->GetValue()));
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
		val = wxString::Format(wxT("%d"),    (int)(m_yManuallySlider->GetValue()));
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
		val = wxString::Format(wxT("%d"),    (int)(m_zManuallySlider->GetValue()));
	} else {
		val = wxString::Format(wxT("%4.3f"), (double)(m_zManuallySlider->GetValue()));
	}
	m_metricZ->SetValue(val);
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
		val = wxString::Format(wxT("%d"),    (int)(m_xManuallySlider->GetValue()));
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
		val = wxString::Format(wxT("%d"),    (int)(m_xManuallySlider->GetValue()));
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
		val = wxString::Format(wxT("%d"),    (int)(m_yManuallySlider->GetValue()));
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
		val = wxString::Format(wxT("%d"),    (int)(m_yManuallySlider->GetValue()));
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
		val = wxString::Format(wxT("%d"),    (int)(m_zManuallySlider->GetValue() ));
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
		val = wxString::Format(wxT("%d"),    (int)(m_zManuallySlider->GetValue()));
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
		m_xManuallySlider->SetValue(v);
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
		m_yManuallySlider->SetValue(v);
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
		m_zManuallySlider->SetValue(v);
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

	if ( m_unit->GetValue() == "steps" ) {
		long v;
		val.ToLong(&v);
		m_xManuallySlider->SetValue(v);
	} else {
		double v;
		val.ToDouble(&v);
		m_xManuallySlider->SetValue(v);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateMetricY(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString val = m_metricY->GetValue();
	
	if ( m_unit->GetValue() == "steps" ) {
		long v;
		val.ToLong(&v);
		m_yManuallySlider->SetValue(v);
	} else {
		double v;
		val.ToDouble(&v);
		m_yManuallySlider->SetValue(v);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateMetricZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString val = m_metricZ->GetValue();
	
	if ( m_unit->GetValue() == "steps" ) {
		long v;
		val.ToLong(&v);
		m_zManuallySlider->SetValue(v);
	} else {
		double v;
		val.ToDouble(&v);
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
void MainFrame::updateRenderResolution() {
///////////////////////////////////////////////////////////////////
	GBL_CONFIG->setRenderResolution(m_cbRenderResolution->GetStringSelection());
	updateCncConfigTrace();
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateRenderResolution(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	updateRenderResolution();
}
///////////////////////////////////////////////////////////////////
void MainFrame::openPreview(CncFilePreview* ctrl, const wxString& fn) {
///////////////////////////////////////////////////////////////////
	wxASSERT(ctrl);
	
	if      ( ctrl == mainFilePreview )		m_currentFileMangerPreviewFileName->ChangeValue(fn);
	else if ( ctrl == monitorFilePreview)	m_currentInboundFilePreviewFileName->ChangeValue(fn);
		
	TemplateFormat tf = getCurrentTemplateFormat(fn);
	switch ( tf ) {
		case TplSvg:		ctrl->selectPreview(fn);
							break;
							
		case TplGcode:		ctrl->selectPreview(fn);
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
	
	if ( CncAsyncKeyboardState::isControlPressed() )
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
	openFileFromFileManager(wxString(lruFileList.getFileName(event.GetIndex())));
}
///////////////////////////////////////////////////////////////////
void MainFrame::lruListItemSelected(wxListEvent& event) {
///////////////////////////////////////////////////////////////////
	openMainPreview(wxString(lruFileList.getFileName(event.GetIndex())));
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectMainBookSourcePanel(int sourcePageToSelect) {
///////////////////////////////////////////////////////////////////
	m_mainViewSelector->SetSelection(MainBookSelection::VAL::SOURCE_PANEL);
	m_mainViewBook->SetSelection(MainBookSelection::VAL::SOURCE_PANEL);
	
	if ( sourcePageToSelect < 0 || sourcePageToSelect > (int)(m_templateNotebook->GetPageCount() - 1) )
		sourcePageToSelect = TemplateBookSelection::VAL::SOURCE_PANEL;
	
	m_templateNotebook->SetSelection(sourcePageToSelect);
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
	
	if ( pane == getAUIPaneByName("SerialSpy") )
		enableSerialSpy(false);
	
	if ( menu != NULL )
		menu->Check(false);
		
	if ( update )
		GetAuimgrMain()->Update();
}
///////////////////////////////////////////////////////////////////
wxWindow* MainFrame::getAUIPaneByName(const wxString& name) {
///////////////////////////////////////////////////////////////////
	if      ( name == "Toolbar" ) 			return m_auibarMain;
	else if ( name == "SourceView")			return m_winMainView;
	else if ( name == "Logger")				return m_winLoggerView;
	else if ( name == "Outbound")			return m_winMonitorView;
	else if ( name == "TemplateManager")	return m_winFileView;
	else if ( name == "StatusBar")			return m_statusBar;
	else if ( name == "SerialSpy")			return m_serialSpyView;
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
	// always do this
	perspectiveHandler.logCurrentPerspective();
	
	if ( event.pane->window == m_winMonitorView || event.pane->window == m_winMainView ) {
		perspectiveTimer.Start(20);
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
			
			case OutboundSelection::VAL::MOTION_MONITOR_PANAL:
			{
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
	m_cmdDuration->Refresh();
	
	GBL_CONTEXT->timestamps.logTotalTimeStart();
}
///////////////////////////////////////////////////////////////////
void MainFrame::stopAnimationControl() {
///////////////////////////////////////////////////////////////////
	m_cmdDuration->SetForegroundColour(*wxBLACK);
	m_cmdDuration->Refresh();
	
	if ( pngAnimation != NULL ) {
		if ( pngAnimation->IsRunning() ) {
			pngAnimation->Stop();
			pngAnimation->Update();

			GBL_CONTEXT->timestamps.logTotalTimeEnd();
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
	
	cnc::trc.logInfoMessage(wxString::Format("Open: %s", cmd));
	
	wxExecute(cmd);
	wxASSERT(cnc);
	waitActive(1500);

	stopAnimationControl();
	return true;
}
///////////////////////////////////////////////////////////////////
void MainFrame::toggleTemplateWordWrapMode(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	sourceEditor->SetWrapMode(!m_btSvgToggleWordWrap->GetValue());
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
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcDebugConfig(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	perspectiveHandler.ensureDebugPerspectiveMinimal();
	
	if ( m_debuggerPropertyManagerGrid->GetPageCount() > 0 )
		m_debuggerPropertyManagerGrid->SelectPage(0);
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcRun() {
///////////////////////////////////////////////////////////////////
	determineRunMode();
	
	// ensure the monitor is visible, especally if isPause == true
	// because then the processing should be resume
	showAuiPane("Outbound");
	selectMonitorBookCncPanel();

	// toggle only the pause flag
	if ( isPause() == true ) {
		rcPause();
		return;
	}
	
	// perform a run
	// Store the current interval
	int interval = GBL_CONTEXT->getUpdateInterval();
	
	if ( isDebugMode == true ) {
		
		// check if the cuurent port is a cnc and no emulator port
		if ( cnc->getPortType() == CncPORT ) {
			
			wxString msg("Do you really want to debug a COM port?");
			wxMessageDialog dlg(this, msg, _T("Port Check . . . "), 
			                    wxOK|wxCANCEL|wxCENTRE|wxICON_QUESTION);
			
			if ( dlg.ShowModal() == wxID_CANCEL )
				return;
		}
		
		// bring the debug controls in front
		perspectiveHandler.ensureDebugPerspectiveMinimal();
		
		// to see each line during the debug session
		GBL_CONTEXT->setUpdateInterval(1);
	} else {
		
		perspectiveHandler.ensureRunPerspectiveMinimal();
	}
	
	// process
	processTemplateWrapper();
	
	// restore the interval
	GBL_CONTEXT->setUpdateInterval(interval);
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
	if ( isPause() )	cnc->sendResume();
	else				cnc->sendPause();
	
	if ( inboundFileParser != NULL ) {
		inboundFileParser->togglePause();
		enableRunControls(inboundFileParser->isPause());
	}
	
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
void MainFrame::decorateSwitchToolOnOff(bool state) {
///////////////////////////////////////////////////////////////////
	if ( state == true ) {
		m_testToggleTool->SetLabel("Switch Tool Off");
		m_testToggleTool->SetBackgroundColour(wxColour(255,128,128));
		m_testToggleTool->SetForegroundColour(*wxWHITE);
	} else {
		m_testToggleTool->SetLabel("Switch Tool On");
		m_testToggleTool->SetBackgroundColour(*wxGREEN);
		m_testToggleTool->SetForegroundColour(*wxBLACK);
	}
	
	m_testToggleTool->Refresh(true);
	m_testToggleTool->Update();
}
///////////////////////////////////////////////////////////////////
void MainFrame::testSwitchToolOnOff(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	if ( cnc->isConnected() == false ) {
		std::cerr << "Not connetced, nothing will be processed." << std::endl;
		return;
	}
	
	decorateSwitchToolOnOff(m_testToggleTool->GetValue());
	m_testToggleTool->Enable(false);
	
	if ( m_testToggleTool->GetValue() == true ) {
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
	
	m_testToggleTool->Enable(true);
	updateSetterList();
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
void MainFrame::clearMotionMonitor() {
///////////////////////////////////////////////////////////////////
	motionMonitor->clear();
	statisticsPane->clear();
	
	decorateOutboundEditor();
}
///////////////////////////////////////////////////////////////////
void MainFrame::clearMotionMonitor(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	clearMotionMonitor();
}
///////////////////////////////////////////////////////////////////
void MainFrame::refreshMotionMonitor(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( cnc )
		cnc->updatePreview3D();
}
///////////////////////////////////////////////////////////////////
void MainFrame::testCaseBookChanged(wxListbookEvent& event) {
///////////////////////////////////////////////////////////////////
	unsigned int sel = event.GetSelection();
	
	switch ( sel ) {
		case TestBookSelection::VAL::INTERVAL:
		case TestBookSelection::VAL::DIMENSION:
		case TestBookSelection::VAL::LIMIT:		break;
		
		case TestBookSelection::VAL::TOOL:		if ( cnc != NULL )
													m_testToggleTool->SetValue(cnc->getToolState());
													
												decorateSwitchToolOnOff(m_testToggleTool->GetValue());
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
	m_btRequestControllerPins->Enable(false);
	requestPins(event);
	m_btRequestControllerPins->Enable(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::requestPins(wxCommandEvent& event) {
	wxASSERT(cnc);
	cnc->processCommand(CMD_PRINT_PIN_REPORT, std::clog);
	m_outboundNotebook->SetSelection(OutboundSelection::VAL::SUMMARY_PANEL);
	m_notebookConfig->SetSelection(OutboundCfgSelection::VAL::CNC_PIN_PANEL);
}
///////////////////////////////////////////////////////////////////
void MainFrame::clearControllerMsgHistory(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_controllerMsgHistory->Clear();
}
///////////////////////////////////////////////////////////////////
void MainFrame::clearSerialSpy() {
///////////////////////////////////////////////////////////////////
	wxASSERT(serialSpyListCtrl);
	serialSpyListCtrl->clear();
	serialSpyListCtrl->clearDetails();
}
///////////////////////////////////////////////////////////////////
void MainFrame::clearSerialSpy(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	clearSerialSpy();
}
///////////////////////////////////////////////////////////////////
void MainFrame::markSerialSpy(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(serialSpyListCtrl);
	wxString defaultValue(wxString::Format("Marker::%06d", serialSpyListCtrl->getItemCount()));
	wxTextEntryDialog dlg(this, "Marker Label:", "Add Spy Marker . . .", defaultValue);
	dlg.SetMaxLength(64);
	
	if ( dlg.ShowModal() == wxID_OK )
		serialSpyListCtrl->addLine(dlg.GetValue(), CncSerialSpyListCtrl::LineType::LT_Marker);
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
void MainFrame::updateLogger(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_showLoggerOnDemand->GetValue() == false )
		return;
	
	if ( m_logger->IsShownOnScreen() == false ) {
		showAuiPane("Logger");
		m_loggerNotebook->SetSelection(LoggerSelection::VAL::CNC);
	}
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
	openFileExtern(GBL_CONFIG->getBrowser(ret), getCurrentTemplatePathFileName());
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
	selectMonitorBookTemplatePanel();
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
	
	if ( debugUserNotificationTimer.IsRunning() == false )
		debugUserNotificationTimer.Start(1000);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::stopDebugUserNotification() {
/////////////////////////////////////////////////////////////////////
	const wxColour c(227, 227, 227);
	m_rcNextBreakpoint->SetBackgroundColour(c);
	m_rcNextStep->SetBackgroundColour(c);
	m_rcFinish->SetBackgroundColour(c);
	
	if ( debugUserNotificationTimer.IsRunning() == true )
		debugUserNotificationTimer.Stop();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::clearPositionSpy() {
/////////////////////////////////////////////////////////////////////
	typedef UpdateManagerThread::Event Event;
	static Event evt;
	umPostEvent(evt.PosSpyResetEvent());
	
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
	if ( positionSpy->getItemCount() == 0 )
		return;

	// Write some text to the clipboard
	if ( wxTheClipboard->Open() ) {
		startAnimationControl();

		wxString content;
		content.reserve(1024 * 1024);
		
		for ( long i=0; i<positionSpy->getItemCount(); i++ )
			positionSpy->getRow(i).trace(content);
		
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
	decoratePosSpyConnectButton(m_btTogglePosSpy->GetValue());
}
///////////////////////////////////////////////////////////////////
void MainFrame::decoratePosSpyConnectButton(bool state) {
///////////////////////////////////////////////////////////////////
	wxBitmap bmpOn  = ImageLib16().Bitmap("BMP_CONNECTED");
	wxBitmap bmpOff = ImageLib16().Bitmap("BMP_DISCONNECTED");
	
	state == true ? m_btTogglePosSpy->SetBitmap(bmpOn) 						: m_btTogglePosSpy->SetBitmap(bmpOff);
	state == true ? m_btTogglePosSpy->SetToolTip("Disable Position Spy")	: m_btTogglePosSpy->SetToolTip("Enable Position Spy");
	
	m_btTogglePosSpy->Refresh();
	m_btTogglePosSpy->Update();
	
	if ( state == false )
		clearPositionSpy();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::selectSourceControlLineNumber(long ln) {
/////////////////////////////////////////////////////////////////////
	if ( ln <= 0L )
		return;
	
	sourceEditor->selectLineNumber(ln);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::tryToSelectClientId(long clientId, TemplateSelSource tss) {
/////////////////////////////////////////////////////////////////////
	//std::cout << clientId << " from: " << tss << std::endl;
	
	if ( tss != TSS_POS_SPY ) {
		if ( positionSpy != NULL )
			positionSpy->searchReferenceById(clientId);
	}
	
	if ( tss != TSS_EDITOR ) {
		selectSourceControlLineNumber(clientId - 1 );
	}
	
	if ( tss != TSS_MONITOR ) {
		if ( motionMonitor != NULL ) {
			motionMonitor->setCurrentClientId(clientId);
			motionMonitor->display();
		}
	}
	
	if ( tss != TSS_PATH_LIST ) {
		if ( cncPreprocessor != NULL )
			cncPreprocessor->selectClientId(clientId, CncPreprocessor::LT_PATH_LIST);
	}
	
	if ( tss != TSS_MOVE_SEQ ) {
		if ( cncPreprocessor != NULL )
			cncPreprocessor->selectClientId(clientId, CncPreprocessor::LT_MOVE_SEQUENCE);
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::selectPositionSpyContent(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	typedef UpdateManagerThread::Event Event;
	static Event evt;
	umPostEvent(evt.PosSypContentEvent((UpdateManagerThread::SpyContent)m_cbContentPosSpy->GetSelection()));

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
		
		bool ret = processTemplateWrapper( i == 0 );
		duration += GBL_CONTEXT->timestamps.getTotalDurationMillis();

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
void MainFrame::refreshSetterList(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	updateSetterList();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::clearSetterList(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	typedef UpdateManagerThread::Event Event;
	static Event evt;
	umPostEvent(evt.SetterListResetEvent());
	
	setterList->clear();
	m_setterListCount->SetValue("# 0");
}
/////////////////////////////////////////////////////////////////////
void MainFrame::requestInfoMessage(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( cnc == NULL )
		return;
		
	cnc->processCommand(CMD_TEST_INFO_MESSAGE, std::clog);
	m_outboundNotebook->SetSelection(OutboundSelection::VAL::SUMMARY_PANEL);
	m_notebookConfig->SetSelection(OutboundCfgSelection::VAL::CNC_MSG_HIST_PANEL);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::requestWarningMessage(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( cnc == NULL )
		return;
		
	cnc->processCommand(CMD_TEST_WARN_MESSAGE, std::clog);
	m_outboundNotebook->SetSelection(OutboundSelection::VAL::SUMMARY_PANEL);
	m_notebookConfig->SetSelection(OutboundCfgSelection::VAL::CNC_MSG_HIST_PANEL);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::requestErrorMessage(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( cnc == NULL )
		return;
		
	cnc->processCommand(CMD_TEST_ERROR_MESSAGE, std::clog);
	m_outboundNotebook->SetSelection(OutboundSelection::VAL::SUMMARY_PANEL);
	m_notebookConfig->SetSelection(OutboundCfgSelection::VAL::CNC_MSG_HIST_PANEL);
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
void MainFrame::selectSerialSpyMode() {
/////////////////////////////////////////////////////////////////////
	int sm = m_cbSerialSpyMode->GetSelection();
	switch ( sm ) {
		case 0:		cnc->setSpyMode(Serial::SypMode::SM_READ); 		break;
		case 1:		cnc->setSpyMode(Serial::SypMode::SM_WRITE); 	break;
		default:	cnc->setSpyMode(Serial::SypMode::SM_ALL);		break;
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::selectSerialSpyMode(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	selectSerialSpyMode();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::displayIntervalChanged(wxScrollEvent& event) {
/////////////////////////////////////////////////////////////////////
	m_displayInterval->SetToolTip(wxString::Format("%d", m_displayInterval->GetValue()));
	GBL_CONTEXT->setUpdateInterval(m_displayInterval->GetValue());
}
/////////////////////////////////////////////////////////////////////
void MainFrame::displayIntervalThumbtrack(wxScrollEvent& event) {
/////////////////////////////////////////////////////////////////////
	m_displayInterval->SetToolTip(wxString::Format("%d", m_displayInterval->GetValue()));
	GBL_CONTEXT->setUpdateInterval(m_displayInterval->GetValue());
}
/////////////////////////////////////////////////////////////////////
void MainFrame::displayIntervalKeyDown(wxKeyEvent& event) {
/////////////////////////////////////////////////////////////////////
	event.Skip(true);

	m_displayInterval->SetToolTip(wxString::Format("%d", m_displayInterval->GetValue()));
	GBL_CONTEXT->setUpdateInterval(m_displayInterval->GetValue());
}
/////////////////////////////////////////////////////////////////////
void MainFrame::decorateProbeMode(bool probeMode) {
/////////////////////////////////////////////////////////////////////
	if ( probeMode == true ) {
		m_btProbeMode->SetBitmap(ImageLibProbe().Bitmap("BMP_PROBE"));
		m_btProbeMode->SetToolTip("Probe mode on");
		
		m_probeModeState->SetBitmap(ImageLib24().Bitmap("BMP_TRAFFIC_LIGHT_YELLOW"));
		m_probeModeState->SetToolTip("Probe mode is ON");
		m_probeModeStateLabel->SetLabel(" ON");
		
	} else {
		m_btProbeMode->SetBitmap(ImageLibProbe().Bitmap("BMP_RELEASE"));
		m_btProbeMode->SetToolTip("Probe mode off");
		
		m_probeModeState->SetBitmap(ImageLib24().Bitmap("BMP_TRAFFIC_LIGHT_DEFAULT"));
		m_probeModeState->SetToolTip("Probe mode is OFF");
		m_probeModeStateLabel->SetLabel("OFF");
		
	}
	
	m_btProbeMode->SetValue(probeMode);
	m_btProbeMode->Refresh();
	m_btProbeMode->Update();
	
	if ( motionMonitor != NULL ) {
		motionMonitor->decorateProbeMode(GBL_CONTEXT->isProbeMode());
		motionMonitor->display();
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::clickProbeMode(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	GBL_CONTEXT->setProbeMode(m_btProbeMode->GetValue());
	
	// update depending controls
	const bool probeMode = GBL_CONTEXT->isProbeMode();
	
	cnc->enableProbeMode(probeMode);

	m_btSpeedControl->Enable(probeMode);
	decorateSpeedControlBtn(true);

	if ( speedMonitor )
		speedMonitor->activate(!probeMode);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::clickSpeedControl(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	decorateSpeedControlBtn(m_btSpeedControl->GetValue());
}
/////////////////////////////////////////////////////////////////////
void MainFrame::decorateSpeedControlBtn(bool useSpeedCfg) {
/////////////////////////////////////////////////////////////////////
	if ( useSpeedCfg == true )	{
		m_btSpeedControl->SetBitmap((ImageLibSpeed().Bitmap("BMP_SPEED_CTRL_ON")));
		m_btSpeedControl->SetToolTip("Speed Contoller is active");
		
		m_speedCtrlState->SetBitmap(ImageLib24().Bitmap("BMP_TRAFFIC_LIGHT_YELLOW"));
		m_speedCtrlState->SetToolTip("Speed Controller is ON");
		m_speedCtrlStateLabel->SetLabel(" ON");
		
	} else {
		m_btSpeedControl->SetBitmap((ImageLibSpeed().Bitmap("BMP_SPEED_CTRL_OFF")));
		m_btSpeedControl->SetToolTip("Speed Contoller is inactive");
		
		m_speedCtrlState->SetBitmap(ImageLib24().Bitmap("BMP_TRAFFIC_LIGHT_DEFAULT"));
		m_speedCtrlState->SetToolTip("Speed Controller is OFF");
		m_speedCtrlStateLabel->SetLabel(" OFF");
	}
	
	m_btSpeedControl->SetValue(useSpeedCfg);
	m_btSpeedControl->Refresh();
	
	m_speedCtrlState->Refresh();
	m_speedCtrlStateLabel->Refresh();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::dclickUpdateManagerThreadSymbol(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( updateManagerThread != NULL ) {
		if ( updateManagerThread->IsPaused() == true ) {
			wxCriticalSectionLocker enter(pUpdateManagerThreadCS);
			updateManagerThread->Resume();
		}
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::keyDownLogger(wxKeyEvent& event) {
/////////////////////////////////////////////////////////////////////
	bool ctlKey = CncAsyncKeyboardState::isControlPressed();
	const int c = event.GetUnicodeKey();
	
	if ( c == 'C' && ctlKey == true ) {
		bool unselect = false;
		
		if ( m_logger->HasSelection() == false ) {
			m_logger->SelectAll();
			unselect = true;
		}
		
		m_logger->Copy();
		
		if ( unselect == true )
			m_logger->SelectNone();
			
		m_logger->Refresh();
	}
	
	event.Skip(true);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::dclickLogger(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	long pos = m_logger->GetInsertionPoint(), col = -1, row = -1;
	if ( m_logger->PositionToXY(pos, &col, &row) == false )
		return;
		
	wxString line(m_logger->GetLineText(row));
	line.Trim(false);
	line.MakeUpper();
	
	if ( line.StartsWith('[') == true ) {
		line.assign(line.BeforeFirst(']'));
		line.assign(line.AfterFirst('['));
		
	} else if ( line.Contains("LINE") == true ) {
		int p = line.Find("LINE");
		bool start = false;
		wxString ln;
		for (unsigned int i=p; i<line.length(); i++ ) {
			
			if ( start == false && isdigit((char)line[i]) != 0 )
				start = true;
			
			if ( start == true && isdigit((char)line[i]) == 0 )
				break;
				
			if ( start == true )
				ln.append(line[i]);
		}
		
		line.assign(ln);
		
	} else {
		
		return;
	}
	
	long lineNumber = -1;
	if ( line.ToLong(&lineNumber) == false )
		return;
	
	selectSourceControlLineNumber(lineNumber - 1);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::keyDownLruList(wxKeyEvent& event) {
/////////////////////////////////////////////////////////////////////
	bool ctlKey = CncAsyncKeyboardState::isControlPressed();
	int c = event.GetUnicodeKey();
	
	// save
	if ( c == 'S' && ctlKey == true ) {
		lruFileList.save(lruStore);
		std::clog << "LRU List saved . . . " << std::endl;
		return;
	}
	
	event.Skip(true);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::manualContinuousMoveStart(const CncLinearDirection x, const CncLinearDirection y, const CncLinearDirection z) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	if ( cnc->isInterrupted() == true ) {
		std::cerr << "Interrupt active. Nothing will be done . . ." << std::endl;
		return;
	}
	
	if ( cnc->isContinuousMoveActive() )
		return;
	
	CncControl::StepSensitivity stepSensitivity = CncControl::StepSensitivity::FINEST;
	unsigned int sel = m_rbStepSensitivity->GetSelection();
	switch ( sel ) {
		case 0: stepSensitivity = CncControl::StepSensitivity::FINEST;		break;
		case 1: stepSensitivity = CncControl::StepSensitivity::FINE; 		break;
		case 2: stepSensitivity = CncControl::StepSensitivity::MEDIUM; 		break;
		case 3: stepSensitivity = CncControl::StepSensitivity::ROUGH; 		break;
		case 4: stepSensitivity = CncControl::StepSensitivity::ROUGHEST; 	break;
	}
	
	CncTransactionLock ctl(this);
	
	cnc->manualContinuousMoveStop();
	
	motionMonitor->pushProcessMode();
	cnc->manualContinuousMoveStart(stepSensitivity, x, y, z);
	motionMonitor->popProcessMode();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::manualContinuousMoveStop() {
/////////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	cnc->manualContinuousMoveStop();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onNavigatorPanel(CncNavigatorPanelEvent& event) {
/////////////////////////////////////////////////////////////////////
	typedef CncNavigatorPanelEvent::Id Id;
	const Id eventId = (Id)event.GetId();
	
	auto moveStart = [&]() {
		CncLinearDirection x = CncLinearDirection::CncNoneDir;
		CncLinearDirection y = CncLinearDirection::CncNoneDir;
		CncLinearDirection z = CncLinearDirection::CncNoneDir;
		
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
		
		if ( move == true )
			manualContinuousMoveStart(x, y, z);
	};
	
	auto moveStop = [&]() {
		manualContinuousMoveStop();
	};
		
	switch ( eventId ) {
		case Id::CNP_ACTIVATE_REGION:	moveStart();
										break;
										
		case Id::CNP_DEACTIVATE_REGION:
		case Id::CNP_LEAVE_PANEL:
		case Id::CNP_KILL_FOCUS:
		case Id::CNP_LEAVE_REGION:
										moveStop();
										break;
		default: ;
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::setReferencePosition(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	showReferencePositionDlg("");
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
}
/////////////////////////////////////////////////////////////////////
void MainFrame::leaveSerialSpy(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(serialSpyListCtrl);
	// currently nothing more to do
	//serialSpyListCtrl->clearDetails();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::initSpeedConfigPlayground() {
/////////////////////////////////////////////////////////////////////
	if ( m_speedConfigSlider ) {
		m_speedConfigSlider->SetRange(0, GBL_CONFIG->getMaxSpeedXYZ_MM_MIN());
		m_speedConfigSlider->SetValue(GBL_CONFIG->getMaxSpeedXYZ_MM_MIN() * 0.7);
	}
	
	updateSpeedConfigPlayground();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::updateSpeedConfigPlayground() {
/////////////////////////////////////////////////////////////////////
	if ( m_speedConfigSlider ) {
		
		CncSpeedController csc;
		csc.setup('X', GBL_CONFIG->getStepsX(), GBL_CONFIG->getPitchX(), 
		          SPEED_MANAGER_CONST_STATIC_OFFSET_US, SPEED_MANAGER_CONST_LOOP_OFFSET_US, 
				  GBL_CONFIG->getLowPulsWidthX() + GBL_CONFIG->getHighPulsWidthX(),
				  GBL_CONFIG->getAccelStartSpeedX_MM_MIN()/60, GBL_CONFIG->getAccelStopSpeedX_MM_MIN()/60
				 );
		csc.setup('Y', GBL_CONFIG->getStepsY(), GBL_CONFIG->getPitchY(), 
		          SPEED_MANAGER_CONST_STATIC_OFFSET_US, SPEED_MANAGER_CONST_LOOP_OFFSET_US, 
				  GBL_CONFIG->getLowPulsWidthY() + GBL_CONFIG->getHighPulsWidthY(),
				  GBL_CONFIG->getAccelStartSpeedY_MM_MIN()/60, GBL_CONFIG->getAccelStopSpeedY_MM_MIN()/60
				 );
		csc.setup('Z', GBL_CONFIG->getStepsZ(), GBL_CONFIG->getPitchZ(), 
		          SPEED_MANAGER_CONST_STATIC_OFFSET_US, SPEED_MANAGER_CONST_LOOP_OFFSET_US, 
				  GBL_CONFIG->getLowPulsWidthZ() + GBL_CONFIG->getHighPulsWidthZ(),
				  GBL_CONFIG->getAccelStartSpeedZ_MM_MIN()/60, GBL_CONFIG->getAccelStopSpeedZ_MM_MIN()/60
				  );
		
		csc.setFeedSpeed_MM_MIN(m_speedConfigSlider->GetValue());
		
		long x; m_speedConfigStepsX->GetValue().ToLong(&x);
		long y; m_speedConfigStepsY->GetValue().ToLong(&y);
		long z; m_speedConfigStepsZ->GetValue().ToLong(&z);
		csc.initMove(x, y, z);
		
		std::stringstream ssX, ssY, ssZ, ssAX, ssAY, ssAZ, ssC;
		ssC << csc;  		m_speedConfigTrace->ChangeValue(ssC.str());
		
		ssX << csc.X;  		m_speedConfigTraceX->ChangeValue(ssX.str());
		ssY << csc.Y;  		m_speedConfigTraceY->ChangeValue(ssY.str());
		ssZ << csc.Z;  		m_speedConfigTraceZ->ChangeValue(ssZ.str());
		
		ssAX << csc.X.AP;  	m_accelConfigTraceX->ChangeValue(ssAX.str());
		ssAY << csc.Y.AP;  	m_accelConfigTraceY->ChangeValue(ssAY.str());
		ssAZ << csc.Z.AP;  	m_accelConfigTraceZ->ChangeValue(ssAZ.str());
		
		wxASSERT(accelGraphPanel);
		char axis   = m_speedConfigAccelAxis->GetStringSelection()[0];
		int32_t stm = 0;
		switch ( axis ) {
			case 'X': stm = x; break;
			case 'Y': stm = y; break;
			case 'Z': stm = z; break;
			default : axis = '\0';
		}
		
		if ( axis != '\0' ) {
			accelGraphPanel->init(&csc);
			accelGraphPanel->calculate(axis, stm);
			accelGraphPanel->display();
		}
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::changeSpeedConfigSlider(wxScrollEvent& event) {
/////////////////////////////////////////////////////////////////////
	updateSpeedConfigPlayground();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::updatedSpeedConfigSteps(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	updateSpeedConfigPlayground();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::updatedSpeedConfigAccelAxis(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	updateSpeedConfigPlayground();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::decorateGamepadState(bool state) {
/////////////////////////////////////////////////////////////////////
	if ( state == true )	m_gamepadState->SetBitmap((ImageLibGamepad().Bitmap("BMP_ACTIVATED")));
	else 					m_gamepadState->SetBitmap((ImageLibGamepad().Bitmap("BMP_DEACTIVATED")));
	
	if ( state == false ) {
		m_gpBmp1->Show(false);
		m_gpBmp2->Show(false);
		m_gpBmp3->Show(false);
		m_gpBmp4->Show(false);
		
		if ( cncGameportDlg )
			cncGameportDlg->trace("Gamepad state not available");
	}
	
	m_gamepadState->Refresh();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::decorateSecureDlgChoice(bool useDlg) {
/////////////////////////////////////////////////////////////////////
	useSecureRunDlg = useDlg;
	if ( useSecureRunDlg == true )	m_rcSecureDlg->SetBitmap((ImageLibSecureRun().Bitmap("BMP_SECURE_DLG_YES")));
	else 							m_rcSecureDlg->SetBitmap((ImageLibSecureRun().Bitmap("BMP_SECURE_DLG_NO")));
	
	m_rcSecureDlg->Refresh();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::rcSecureDlg(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	decorateSecureDlgChoice(!useSecureRunDlg);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::cncMainViewChanged(wxNotebookEvent& event) {
/////////////////////////////////////////////////////////////////////
	activateGamepadNotificationsOnDemand(true);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::decorateIdleState(bool state) {
/////////////////////////////////////////////////////////////////////
	if ( state == false )
		m_heartbeatState->SetBitmap(ImageLibHeartbeat().Bitmap("BMP_HEART_INACTIVE"));
		
	m_heartbeatState->Refresh();
	
	// in case the state is true, here is nothing to do, because this state will be handled by 
	// SerialControllerCallback(const ContollerInfo& ci) and ci == CITHeartbeat
}
/////////////////////////////////////////////////////////////////////
void MainFrame::toggleIdleRequests(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	decorateIdleState(event.IsChecked());
}
/////////////////////////////////////////////////////////////////////
void MainFrame::motionMonitorFlyPath(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
		
	motionMonitor->getContextOptions().toggleOption(motionMonitor->getContextOptions().showFlyPath);
	motionMonitor->reconstruct();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::motionMonitorPostionMarker(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
	
	motionMonitor->getContextOptions().toggleOption(motionMonitor->getContextOptions().showPosMarker);
	motionMonitor->updateMonitorAndOptions();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::motionMonitorBoundBox(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
		
	motionMonitor->getContextOptions().toggleOption(motionMonitor->getContextOptions().showBoundBox);
	motionMonitor->updateMonitorAndOptions();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::motionMonitorOrigin(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
		
	motionMonitor->getContextOptions().toggleOption(motionMonitor->getContextOptions().showOrigin);
	motionMonitor->updateMonitorAndOptions();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::motionMonitorRuler(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
	
	motionMonitor->getContextOptions().toggleOption(motionMonitor->getContextOptions().showRuler);
	motionMonitor->updateMonitorAndOptions();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::motionMonitorHelpLines(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
		
	motionMonitor->getContextOptions().toggleOption(motionMonitor->getContextOptions().showHelpLines);
	motionMonitor->updateMonitorAndOptions();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::openSessionDialog(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxString ret;
	openFileExtern(GBL_CONFIG->getFileBrowser(ret), CncFileNameService::getTempDir());
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
	wxString outboundFile(cnc->getPortName());
	
	if ( wxFile::Exists(outboundFile) == false ) {
		std::cerr << "MainFrame::saveOutboundAsNewTemplate: Can't found '" << outboundFile << "'" << std::endl;
		return;
	}
	
	wxString headline("Save Outbound as new Template");
	wxString inboundFile(getCurrentTemplatePathFileName());
	inboundFile.append(".");
	inboundFile.append(outboundEditor->getExtention());
	
	wxString newFile;
	while ( true ) {
		wxFileDialog saveFileDialog(this, 
									headline, 
									inboundFile, 
									inboundFile,
									"",
									wxFD_SAVE);
									
		if ( saveFileDialog.ShowModal() == wxID_CANCEL ) { 
			return;
		}
		
		newFile.assign(saveFileDialog.GetPath());
		if ( wxFile::Exists(newFile) == true ) {
			
			wxString msg(wxString::Format("Template '%s'\nalready exists. Overide it?", newFile));
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
	
	wxString msg("Should the new template opened directly into the editor?");
	wxMessageDialog dlg(this, msg, headline, wxYES|wxNO|wxICON_QUESTION|wxCENTRE);

	if ( dlg.ShowModal() == wxID_YES ) {
		
		wxFileName tpl(newFile);
		m_inputFileName->SetValue(tpl.GetFullName());
		m_inputFileName->SetHint(tpl.GetFullPath());
		
		if ( !openFile() ) {
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
	if ( parser.preface() == false ) {
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
	GBL_CONFIG->getDefaultTplDir(defaultDir);
		
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

	if ( !openFile() ) {
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
void MainFrame::toggleMotionMonitorStatisticPane(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	toggleMotionMonitorStatisticPane(false);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::toggleMotionMonitorOptionPane(bool forceHide) {
/////////////////////////////////////////////////////////////////////
	if ( cnc3DVSplitterWindow != NULL )
		forceHide == true ? cnc3DVSplitterWindow->hideRightWindow() : cnc3DVSplitterWindow->toggleRightWindow();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::toggleMotionMonitorStatisticPane(bool forceHide) {
/////////////////////////////////////////////////////////////////////
	if ( cnc3DHSplitterWindow != NULL ) {
		
		const int nc = MontiorBottomContextSelection::VAL::STATISTIC_PANEL;
		const int cc = cnc3DHSplitterWindow->getCurrentButtomContext();
		
		cnc3DHSplitterWindow->selectBottomContext(nc);
		
		if ( cnc3DHSplitterWindow->isBottomWindowShown() ) {
			cnc3DHSplitterWindow->hideBottomWindow();
			
			if ( nc != cc)
				forceHide == true ? cnc3DHSplitterWindow->hideBottomWindow() : cnc3DHSplitterWindow->showBottomWindow();
				
		} else {
			forceHide == true ? cnc3DHSplitterWindow->hideBottomWindow() : cnc3DHSplitterWindow->toggleBottomWindow();
			
		}
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::toggleMotionMonitorReplayPane(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( cnc3DHSplitterWindow != NULL ) {
		
		const int nc = MontiorBottomContextSelection::VAL::REPLAY_PANEL;
		const int cc = cnc3DHSplitterWindow->getCurrentButtomContext();
		
		cnc3DHSplitterWindow->selectBottomContext(nc);
		
		if ( cnc3DHSplitterWindow->isBottomWindowShown() ) {
			cnc3DHSplitterWindow->hideBottomWindow();
			
			if ( nc != cc)
				cnc3DHSplitterWindow->showBottomWindow();
				
		} else {
			cnc3DHSplitterWindow->toggleBottomWindow();
			
		}
	}
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
	else if ( uFrom == "mm"      && uTo == "stepsX" ) 		m_metricValueTo->ChangeValue(wxString::Format("%.6lf", GBL_CONFIG->convertX(CncMetric, CncSteps,  value)));
	else if ( uFrom == "mm"      && uTo == "stepsY" ) 		m_metricValueTo->ChangeValue(wxString::Format("%.6lf", GBL_CONFIG->convertY(CncMetric, CncSteps,  value)));
	else if ( uFrom == "mm"      && uTo == "stepsZ" ) 		m_metricValueTo->ChangeValue(wxString::Format("%.6lf", GBL_CONFIG->convertZ(CncMetric, CncSteps,  value)));
	
	else if ( uFrom == "stepsX"  && uTo == "stepsX" ) 		m_metricValueTo->ChangeValue(valueFrom);
	else if ( uFrom == "stepsX"  && uTo == "mm"     ) 		m_metricValueTo->ChangeValue(wxString::Format("%.6lf", GBL_CONFIG->convertX(CncSteps,  CncMetric, value)));

	else if ( uFrom == "stepsY"  && uTo == "stepsY" ) 		m_metricValueTo->ChangeValue(valueFrom);
	else if ( uFrom == "stepsY"  && uTo == "mm"     ) 		m_metricValueTo->ChangeValue(wxString::Format("%.6lf", GBL_CONFIG->convertY(CncSteps,  CncMetric, value)));
	
	else if ( uFrom == "stepsZ"  && uTo == "stepsZ" ) 		m_metricValueTo->ChangeValue(valueFrom);
	else if ( uFrom == "stepsZ"  && uTo == "mm"     ) 		m_metricValueTo->ChangeValue(wxString::Format("%.6lf", GBL_CONFIG->convertZ(CncSteps,  CncMetric, value)));
	
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
	CncOSEnvironmentDialog dlg(this);
	dlg.ShowModal();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::openGameportController(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	showGameportController(true);
}
/////////////////////////////////////////////////////////////////////
void MainFrame::showGameportController(bool show) {
/////////////////////////////////////////////////////////////////////
	if ( cncGameportDlg == NULL )
		cncGameportDlg = new CncGameportController(this);
	
	if ( show == true ) {
		if ( cncGameportDlg->IsShown() == false )
			cncGameportDlg->Show(true);
	}
	else {
		if ( cncGameportDlg->IsShown() == true )
			cncGameportDlg->Show(false);
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::dclickHeartbeatState(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( m_miRqtIdleMessages->IsEnabled() == false )
		return;
	
	m_miRqtIdleMessages->Check(!m_miRqtIdleMessages->IsChecked());
	decorateIdleState(m_miRqtIdleMessages->IsChecked());
}
/////////////////////////////////////////////////////////////////////
void MainFrame::updateSpyDetailWindow() {
/////////////////////////////////////////////////////////////////////
	wxDataViewListCtrl* listCtrl = NULL;
	wxString headline;
	if      ( m_spyUnknownDetails->IsShownOnScreen() )		{ listCtrl = m_spyUnknownDetails;  headline.assign("Info"); }
	else if ( m_spyInboundDetails->IsShownOnScreen() ) 		{ listCtrl = m_spyInboundDetails;  headline.assign("Inbound Details"); }
	else if ( m_spyOutboundDetails->IsShownOnScreen() )		{ listCtrl = m_spyOutboundDetails; headline.assign("Outbound Details"); }
	
	if ( listCtrl != NULL && listCtrl->GetItemCount() > 0) {
		
		const int row = 0;
		wxString details;
		for ( unsigned int col = 0; col < listCtrl->GetColumnCount(); col++ ) {
			
			wxDataViewColumn* dvc = listCtrl->GetColumn(col);
			wxVariant value;
			listCtrl->GetValue(value, row, col);
			
			if ( listCtrl->GetColumnCount() > 1 && col == listCtrl->GetColumnCount() - 1 ) {
				details.append(wxString::Format("\n%s:\n%s", dvc->GetTitle(), value.GetString()));
			}
			else {
				const unsigned int size = 24;
				wxString title(dvc->GetTitle().Left(size));
				wxString spacer(' ',  ( listCtrl->GetColumnCount() > 1 ? size - title.Length() : 0 ));
				details.append(wxString::Format("%s%s: %s\n", title, spacer, value.GetString()));
			}
		}
		
		details.Replace(" | ", "\n", true);
		details.Replace(" |", "\n",  true);
		details.Replace("| ", "\n",  true);
		details.Replace("|", "\n", 	 true);
		
		if ( spyDetailWindow != NULL && spyDetailWindow->IsShownOnScreen() == true ) {
			spyDetailWindow->setHeadline(headline);
			spyDetailWindow->setMessage(details);
		}
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::openSpyDetailWindow(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( spyDetailWindow == NULL ) {
		spyDetailWindow = new CncMessageDialog(this, "", "", "Serial Spy Details");
		spyDetailWindow->SetSize(800, 900);
		spyDetailWindow->setWordWrap(true);
	}
	
	if ( spyDetailWindow->IsShownOnScreen() == false )
		spyDetailWindow->Show();
	
	updateSpyDetailWindow();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onSelectSpyInboundDetails(wxDataViewEvent& event) {
/////////////////////////////////////////////////////////////////////
	m_spyInboundDetails->UnselectAll();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onSelectSpyOutboundDetails(wxDataViewEvent& event) {
/////////////////////////////////////////////////////////////////////
	m_spyOutboundDetails->UnselectAll();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::onSelectSpyUnknownDetails(wxDataViewEvent& event) {
/////////////////////////////////////////////////////////////////////
	m_spyUnknownDetails->UnselectAll();
}
/////////////////////////////////////////////////////////////////////
void MainFrame::showStacktraceStore(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	CncFileContentDialog dlg(this, CncFileNameService::getStackTraceFileName(), "All collected stacktraces", "Stacktrace Store");
	dlg.SetSize(800, 900);
	dlg.setWordWrap(false);
	dlg.ShowModal();
}

