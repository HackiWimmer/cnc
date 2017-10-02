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
#include "SerialPort.h"
#include "CncPosition.h"
#include "CncUsbPortScanner.h"
#include "CncPatternDefinitions.h"
#include "SvgUnitCalculator.h"
#include "EndSwitchDialog.h"
#include "CncFileNameService.h"
#include "CncControllerTestSuite.h"
#include "SVGPathHandlerCnc.h"
#include "GCodeFileParser.h"
#include "CncArduino.h"
#include "SvgEditPopup.h"
#include "HexDecoder.h"
#include "UnitTestFrame.h"
#include "MainFrame.h"

// special includes for WindowPoc handling. 
//They have to be at the end of the list to avoid compilation errors
#include <windows.h>
#include <dbt.h>

// global strings
const char* _portEmulatorNULL 	= "<PortEmulator(dev/null)>";
const char* _portEmulatorSVG  	= "<PortEmulator(SVGFile)>";
const char* _programTitel 		= "Woodworking CNC Controller";
const char* _copyRight			= "copyright by Stefan Hoelzer 2016 - 2017";
const char* _defaultPerspective = "layout2|name=Toolbar;caption=Main;state=17148;dir=1;layer=0;row=0;pos=0;prop=100000;bestw=40;besth=40;minw=40;minh=40;maxw=40;maxh=40;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=MainView;caption=CNC Main View;state=31459324;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=800;besth=800;minw=10;minh=10;maxw=800;maxh=800;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=TemplateManager;caption=CNC Template Manager;state=31459324;dir=3;layer=1;row=0;pos=0;prop=100000;bestw=100;besth=160;minw=100;minh=160;maxw=100;maxh=160;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=Logger;caption=CNC Logger;state=31459324;dir=3;layer=1;row=0;pos=1;prop=100000;bestw=100;besth=160;minw=100;minh=160;maxw=100;maxh=180;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=StatusBar;caption=;state=1020;dir=3;layer=2;row=0;pos=0;prop=100000;bestw=20;besth=28;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=Outbound;caption=CNC Monitor;state=31459324;dir=2;layer=0;row=1;pos=0;prop=100000;bestw=800;besth=800;minw=10;minh=10;maxw=800;maxh=800;floatx=1462;floaty=216;floatw=400;floath=250|dock_size(1,0,0)=42|dock_size(5,0,0)=205|dock_size(3,1,0)=179|dock_size(3,2,0)=30|dock_size(2,0,1)=799|";
#ifdef DEBUG
const char* _programVersion 	= "0.8.0.d";
#else
const char* _programVersion 	= "0.8.0.r";
#endif

// file content change environment
#define EDIT_TRACKER_MARGIN_ID 1
#define CL_LINE_MODIFIED_STYLE 200
#define CL_LINE_SAVED_STYLE 201
enum {MARGIN_LINE_NUMBERS, MARGIN_FOLD};

wxBEGIN_EVENT_TABLE(MainFrame, MainFrameBClass)
	EVT_CLOSE(MainFrame::OnClose)
	EVT_THREAD(wxEVT_COMMAND_MYTHREAD_UPDATE, MainFrame::OnThreadUpdate)
	EVT_THREAD(wxEVT_COMMAND_MYTHREAD_COMPLETED, MainFrame::OnThreadCompletion)
	EVT_TIMER(PERSPECTIVE_TIMER, MainFrame::OnPerspectiveTimer)
wxEND_EVENT_TABLE()

wxDEFINE_EVENT(wxEVT_COMMAND_MYTHREAD_COMPLETED, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_MYTHREAD_UPDATE, wxThreadEvent);

///////////////////////////////////////////////////////////////////
MainFrame::MainFrame(wxWindow* parent)
: MainFrameBClass(parent) 
, isCncControlInitialized(false)
, isZeroReferenceValid(false)
, canClose(true)
, evaluatePositions(true)
, svgDebugger(false)
, templateFileLoading(false)
, ignoreDirControlEvents(false)
, runConfirmationInfo(RunConfirmationInfo::Wait)
, traceTimerCounter(0)
, lastPortName(wxT(""))
, defaultPortName(wxT(""))
, cnc(new CncControl(CncEMU_NULL))
, motionMonitor(NULL)
, serialSpy(NULL)
, guiCtlSetup(new GuiControlSetup())
, config(new wxFileConfig(wxT("CncController"), wxEmptyString, CncFileNameService::getConfigFileName(), CncFileNameService::getConfigFileName(), wxCONFIG_USE_RELATIVE_PATH | wxCONFIG_USE_NO_ESCAPE_CHARACTERS))
, lruStore(new wxFileConfig(wxT("CncControllerLruStore"), wxEmptyString, CncFileNameService::getLruFileName(), CncFileNameService::getLruFileName(), wxCONFIG_USE_RELATIVE_PATH | wxCONFIG_USE_NO_ESCAPE_CHARACTERS))
, pathGenerator(new PathGeneratorFrame(this, m_stcFileContent))
, outboundNbInfo(new NotebookInfo(m_outboundNotebook))
, templateNbInfo(new NotebookInfo(m_templateNotebook))
, lruFileList(LruFileList(8))
, lastTemplateModification(wxDateTime::UNow())
, lastSvgEmuModification(wxDateTime::UNow())
, processStartTime(wxDateTime::UNow())
, processEndTime(wxDateTime::UNow())
, lastTemplateFileNameForPreview(wxT(""))
, pngAnimation(NULL)
, stcFileContentPopupMenu(NULL)
, stcEmuContentPopupMenu(NULL)
, inboundFileParser(NULL)
, perspectiveTimer(this, wxID_HIGHEST + 1)
{
///////////////////////////////////////////////////////////////////
	// detemine assert handler
	wxSetDefaultAssertHandler();
	
	// decocate application
	setIcons();
	
	// do this definitely here later it will causes a crash 
	install3DPane();
	installSypControl();
	
	//m_templateNotebook->SetSelection(TemplatePreviewPage);
	
	// define the popup parent frame
	SvgEditPopup::setMainFrame(this);
	
	// bind global key down hook
	this->Bind(wxEVT_CHAR_HOOK, &MainFrame::globalKeyDownHook, this);
}
///////////////////////////////////////////////////////////////////
MainFrame::~MainFrame() {
///////////////////////////////////////////////////////////////////
	// stop the serial timer and wait interval to finish the work behind
	m_serialTimer->Stop();
	if ( cnc != NULL )
		cnc->waitActive(m_serialTimer->GetInterval());
	
	// unbind global key down hook
	this->Unbind(wxEVT_CHAR_HOOK, &MainFrame::globalKeyDownHook, this);
	
	// todo
	//this->Unbind(wxEVT_COMMAND_MENU_SELECTED, [](wxCommandEvent& event) {});

	wxASSERT(lruStore);
	lruFileList.save(lruStore);
	lruStore->Flush();
	delete lruStore;
	
	wxASSERT(config);
	config->Flush();
	delete config;
	
	wxASSERT (pathGenerator);
	pathGenerator->Destroy();
	delete pathGenerator;

	wxASSERT (guiCtlSetup);
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
void MainFrame::install3DPane() {
///////////////////////////////////////////////////////////////////
	wxASSERT( m_drawPane3D );
		
	wxWindow* parent = m_drawPane3D->GetParent();
	wxSizer* sizer   = m_drawPane3D->GetContainingSizer();
	
	if ( parent == NULL ) {
		std::cerr << "MainFrame::install3DPane(): Invalid parent pointer." << std::endl;
		return;
	}
	
	if ( sizer == NULL ) {
		std::cerr << "MainFrame::install3DPane(): Invalid sizer pointer." << std::endl;
		return;
	}
	
	std::clog << "Installing 3D draw pane . . . ";
	motionMonitor = new CncMotionMonitor(parent, NULL);
	motionMonitor->SetPosition(m_drawPane3D->GetPosition());
	
	/*
	 * todo
	motionMonitor->setPlayButton(m_3D_Animate);
	motionMonitor->setTraceCtrl(m_trace3D);
	motionMonitor->setSpinCtrls(m_spin3DAngelX, m_spin3DAngelY, m_spin3DAngelZ);
	motionMonitor->view3D();
	*/
	
	activate3DPerspectiveButton(m_3D_Perspective1);

	sizer->Replace(m_drawPane3D, motionMonitor, true);
	sizer->Layout();
	std::clog << "Done" << std::endl;
	
	// remove the placeholder
	m_drawPane3D->Destroy();
	//delete m_drawPane3D;
}
///////////////////////////////////////////////////////////////////
void MainFrame::installSypControl() {
///////////////////////////////////////////////////////////////////
	wxASSERT( m_serialSpy );
		
	wxWindow* parent = m_serialSpy->GetParent();
	wxSizer* sizer   = m_serialSpy->GetContainingSizer();
	
	if ( parent == NULL ) {
		std::cerr << "MainFrame::installSypControl(): Invalid parent pointer." << std::endl;
		return;
	}
	
	if ( sizer == NULL ) {
		std::cerr << "MainFrame::installSypControl(): Invalid sizer pointer." << std::endl;
		return;
	}
	
	std::clog << "Installing Spy Control . . . ";
	serialSpy = new CncSpyControl(parent, wxID_ANY, m_serialSpyDetails);
	serialSpy->SetPosition(m_serialSpy->GetPosition());

	sizer->Replace(m_serialSpy, serialSpy, true);
	sizer->Layout();
	std::clog << "Done" << std::endl;
	
	// remove the placeholder
	m_serialSpy->Destroy();
	//delete m_serialSpy;
}
///////////////////////////////////////////////////////////////////
void MainFrame::registerGuiControls() {
///////////////////////////////////////////////////////////////////
	registerGuiControl(m_btPathGenerator);
	registerGuiControl(m_checkManuallyXY);
	registerGuiControl(m_checkManuallyZ);
	registerGuiControl(m_testToggleTool);
	registerGuiControl(m_testToggleEndSwitch);
	registerGuiControl(m_ctrlTestSelection);
	registerGuiControl(m_testDimTakeOverAndSave);
	registerGuiControl(m_portSelector);
	registerGuiControl(m_maxXDimension);
	registerGuiControl(m_maxYDimension);
	registerGuiControl(m_maxZDimension);
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
	registerGuiControl(m_currentSpeedXY);
	registerGuiControl(m_flySpeedXY);
	registerGuiControl(m_workSpeedXY);
	registerGuiControl(m_workSpeedZ);
	registerGuiControl(m_reloadTemplate);
	registerGuiControl(m_openSourceExtern);
	registerGuiControl(m_openSvgExtern);
	registerGuiControl(m_btRequestCtlConfig);
	registerGuiControl(m_btRequestCtlPins);
	registerGuiControl(m_btRequestCtlErrorInfo);
	registerGuiControl(m_lruList);
	registerGuiControl(m_dirCtrl);
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
	registerGuiControl(m_previewErrorInfo);
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
}
///////////////////////////////////////////////////////////////////
void MainFrame::testFunction2(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	cnc::trc.logInfoMessage("Test function 2");
	
	cout << "testFunction2"<< endl;
	clog << "testFunction2"<< endl;
	cerr << "testFunction2"<< endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::testFunction3(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	cnc::trc.logInfoMessage("Test function 3");
	
	wxAuiPaneInfo pi = GetAuimgrMain()->GetPane("Outbound");
	cnc::cex1 << GetAuimgrMain()->SavePerspective() << endl;
}
///////////////////////////////////////////////////////////////////
void MainFrame::testFunction4(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	cnc::trc.logInfoMessage("Test function 4");
	
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
}
///////////////////////////////////////////////////////////////////
void MainFrame::startupTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	// Setup AUI Windows menues
	hideAuiPane("Spy");
	m_miToolbar->Check(m_auibarMain->IsShown());
	m_miViewStatusbar->Check(m_statusBar->IsShown());
	m_miViewTemplateManager->Check(m_scrollWinFile->IsShown());
	m_miViewLogger->Check(m_scrollWinLogger->IsShown());
	m_miViewMonitor->Check(m_scrollOutbound->IsShown());
	m_miViewMainView->Check(m_scrollWinMain->IsShown());
	m_miViewSpy->Check(m_scrollSpy->IsShown());
	m_miViewSpeed->Check(m_panelSpeed->IsShown());
	m_miViewUnitCalculator->Check(m_svgUnitCalulator->IsShown());
	
	// Show environment information
	std::ostream stream(m_envrionmentInfo);
	CncFileNameService::trace(stream);
	SvgUnitCalculator::trace(stream);
	
	// Auto connect ?
	wxString cfgStr;
	config->Read("App/AutoConnect", &cfgStr, wxT("false"));
	if ( cfgStr == "true")
		connectSerialPort();
	
	// Auto process ?
	config->Read("App/AutoProcess", &cfgStr, wxT("false"));
	if ( cfgStr == "true" ) {
		defineMinMonitoring();
		processTemplate();
		defineNormalMonitoring();
	}
	
	// don't works well
	//GetAuimgrMain()->LoadPerspective(_defaultPerspective, true);
	
	//todo
	wxCommandEvent dummy;
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
void MainFrame::OnClose(wxCloseEvent& event) {
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
	
	Destroy();
}
///////////////////////////////////////////////////////////////////
void MainFrame::dispatch() {
///////////////////////////////////////////////////////////////////
	wxEventLoopBase* evtLoop = wxEventLoopBase::GetActive();
	while (evtLoop->Pending() )
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
	
	// setup gray background as default
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
	ctl->SetFoldMarginColour(true, *wxBLACK);
	ctl->SetFoldMarginHiColour(true, *wxBLACK);

	// Set default styles 
	ctl->StyleSetForeground (wxSTC_STYLE_LINENUMBER, clDefaultFgd);
	ctl->StyleSetBackground (wxSTC_STYLE_LINENUMBER, clDefaultBck);
	ctl->SetTabWidth(4);
	ctl->SetWrapMode(wxSTC_WRAP_NONE);
	ctl->SetReadOnly(false);
	
	// Enable line numbers
	ctl->SetMarginWidth(MARGIN_LINE_NUMBERS, 35);
	ctl->SetMarginType(MARGIN_LINE_NUMBERS, wxSTC_MARGIN_NUMBER);
	
	ctl->SetMarginType(EDIT_TRACKER_MARGIN_ID, 4); // Styled Text margin
	ctl->SetMarginWidth(EDIT_TRACKER_MARGIN_ID, 3);
	ctl->SetMarginMask(EDIT_TRACKER_MARGIN_ID, 0);
	
	// Configure caret style
	ctl->SetCaretForeground(clCaretFgd);
	ctl->SetSelBackground(true, clCaretBck);
	
	// Configure selection colours
	//ctl->SetSelForeground(true, wxColour(255,201,14));
	ctl->SetSelBackground(true, wxColour(83,83,83));
	
	// file content marker
	ctl->StyleSetBackground(CL_LINE_SAVED_STYLE, wxColour(wxT("FOREST GREEN")));
	ctl->StyleSetBackground(CL_LINE_MODIFIED_STYLE, wxColour(wxT("ORANGE")));
	
	// Set specific styles
	switch ( format ) {
		case TplSvg:

#ifndef DEBUG
			ctl->SetLexer(wxSTC_LEX_HTML);
#endif
			
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
			
		case TplText:
			ctl->SetLexer(wxSTC_LEX_NULL);
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
											
											std::clog << "A new COM device was detected on port: " << portName << std::endl;
											break;
											
			case DBT_DEVICEREMOVECOMPLETE:	if ( lpdb->dbch_devicetype == DBT_DEVTYP_PORT ) {
												PDEV_BROADCAST_PORT pPort = (PDEV_BROADCAST_PORT) lpdb;
												wxString n(pPort->dbcp_name);
												portName.assign(n);
											}
											
											std::clog << "The COM device was removed from port: " << portName << std::endl;
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
						
						/*
						m_portSelector->SetStringSelection(defaultPortName);
						connectSerialPort();
						*/
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
void MainFrame::initialize(void) {
///////////////////////////////////////////////////////////////////
	wxASSERT(m_portSelector);
	wxASSERT(m_inputFileName);
	wxASSERT(m_metricX); wxASSERT(m_metricY); wxASSERT(m_metricZ);

	lruFileList.setListControl(m_lruList);
	
	createAnimationControl();
	createStcFileControlPopupMenu();
	createStcEmuControlPopupMenu();
	decorateSearchButton();
	decorateTemplateListBook();
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
	
	config->Read("COMConfig/DefaultPort", &cfgStr, wxString(_portEmulatorNULL));
	m_portSelector->SetStringSelection(cfgStr);
	defaultPortName.assign(cfgStr);

	// decorate dir control
	m_dirCtrl->SetFilter("SVG Files (*.svg)|*.svg|GCode Files (*.gcode)|*.gcode|Text Files (*.txt)|*.txt"); 
	m_dirCtrl->SetFilterIndex(0);

	wxFloatingPointValidator<float> val2(1, NULL,wxNUM_VAL_DEFAULT );//, wxNUM_VAL_ZERO_AS_BLANK);
	val2.SetRange(0, 50.0);
	m_workpieceThickness->SetValidator(val2);

	wxFloatingPointValidator<float> val3(3, NULL,wxNUM_VAL_DEFAULT );//, wxNUM_VAL_ZERO_AS_BLANK);
	val3.SetRange(0, 10.0);
	m_routerBitDiameter->SetValidator(val3);
	
	wxFloatingPointValidator<float> val4(3, NULL,wxNUM_VAL_DEFAULT );//, wxNUM_VAL_ZERO_AS_BLANK);
	val4.SetRange(-1000, 1000);
	m_maxXDimension->SetValidator(val4);
	m_maxYDimension->SetValidator(val4);
	m_maxZDimension->SetValidator(val4);
	
	wxFloatingPointValidator<float> val5(0, NULL, wxNUM_VAL_DEFAULT );//, wxNUM_VAL_ZERO_AS_BLANK);
	val5.SetRange(1, 1000);
	m_maxXYSpeed->SetValidator(val5);
	m_maxZSpeed->SetValidator(val5);
	
	wxFloatingPointValidator<float> val6(1, NULL, wxNUM_VAL_DEFAULT );//, wxNUM_VAL_ZERO_AS_BLANK);
	val2.SetRange(0.0, 4.0);
	m_crossingThickness->SetValidator(val2);
	
	wxIntegerValidator<long> val7(NULL);
	val7.SetRange(1, 4000);
	m_replyThreshold->SetValidator(val7);
	
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
	
	config->Read("App/ShowTestMenu", &cfgStr, "false");
	if ( cfgStr.MakeUpper().Trim() != "TRUE" ) {
		m_menuBar->Remove(m_menuBar->FindMenu("Test"));
	}
	
	//initilaize debug state
	config->Read("CncConfig/Debug", &cfgStr, "false"); 
	if ( cfgStr == "true" ) { m_menuItemDebugSerial->Check(true); }
	else				    { m_menuItemDebugSerial->Check(false); }
	decorateSerialSpy();
	
	resetMinMaxPositions();
	initializeLruMenu();
	initializeCncControl();
	initializePreconfiguredSpeedSetups();
	
	m_outboundNotebook->SetSelection(Outbound3DPage);
	m_notebookConfig->SetSelection(CNCSetterPage);
	
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
	wxString cfgStr, val;
	long cfgLong;
	double cfgDouble;
	CncConfig* cncConfig = cnc->getCncConfig();
	
	if ( cncConfig != NULL ) {
		config->Read("CncConfig/StepsX", &cfgLong, 200); cncConfig->setStepsX(cfgLong);
		config->Read("CncConfig/StepsY", &cfgLong, 200); cncConfig->setStepsY(cfgLong);
		config->Read("CncConfig/StepsZ", &cfgLong, 200); cncConfig->setStepsZ(cfgLong);
		config->Read("CncConfig/MaxSpeedYX", &cfgLong, 100); cncConfig->setMaxSpeedXY(cfgLong);
		config->Read("CncConfig/MaxSpeedZ",  &cfgLong, 100); cncConfig->setMaxSpeedZ(cfgLong);
		config->Read("CncConfig/MultiplierX", &cfgLong, 1); cncConfig->setMultiplierX(cfgLong);
		config->Read("CncConfig/MultiplierY", &cfgLong, 1); cncConfig->setMultiplierY(cfgLong);
		config->Read("CncConfig/MultiplierZ", &cfgLong, 1); cncConfig->setMultiplierZ(cfgLong);
		config->Read("CncConfig/PitchX", &cfgDouble, 2.0); cncConfig->setPitchX(cfgDouble);
		config->Read("CncConfig/PitchY", &cfgDouble, 2.0); cncConfig->setPitchY(cfgDouble);
		config->Read("CncConfig/PitchZ", &cfgDouble, 2.0); cncConfig->setPitchZ(cfgDouble);
		config->Read("CncConfig/PulsWidthOffsetX", &cfgLong, 100); cncConfig->setPulsWidthOffsetX(cfgLong);
		config->Read("CncConfig/PulsWidthOffsetY", &cfgLong, 100); cncConfig->setPulsWidthOffsetY(cfgLong);
		config->Read("CncConfig/PulsWidthOffsetZ", &cfgLong, 100); cncConfig->setPulsWidthOffsetZ(cfgLong);
		
		if ( isCncControlInitialized == false ) {

			// init max dimensions
			config->Read("CncConfig/MaxDimensionsX", &cfgDouble, 1.0); cncConfig->setMaxDimensionX(cfgDouble);
			m_maxXDimension->SetValue(wxString::Format("%4.3f", cncConfig->getMaxDimensionX()));

			config->Read("CncConfig/MaxDimensionsY", &cfgDouble, 1.0); cncConfig->setMaxDimensionY(cfgDouble);
			m_maxYDimension->SetValue(wxString::Format("%4.3f", cncConfig->getMaxDimensionY()));
			
			config->Read("CncConfig/MaxDimensionsZ", &cfgDouble, 1.0); cncConfig->setMaxDimensionZ(cfgDouble);
			m_maxZDimension->SetValue(wxString::Format("%4.3f", cncConfig->getMaxDimensionZ()));

			// init speed config
			m_maxXYSpeed->SetValue(wxString() << cncConfig->getMaxSpeedXY());
			config->Read("CncConfig/FlySpeedXY",  &cfgStr, "90");
			m_flySpeedXY->SetStringSelection(cfgStr);
			config->Read("CncConfig/WorkSpeedXY",  &cfgStr, "80");
			m_workSpeedXY->SetStringSelection(cfgStr);
			
			m_maxZSpeed->SetValue(wxString() << cncConfig->getMaxSpeedZ());
			config->Read("CncConfig/FlySpeedZ",  &cfgStr, "90");
			m_flySpeedZ->SetStringSelection(cfgStr);
			config->Read("CncConfig/WorkSpeedZ",  &cfgStr, "80");
			m_workSpeedZ->SetStringSelection(cfgStr);
			
			m_currentSpeedXY->SetSelection(0);
			m_currentSpeedZ->SetSelection(0);
			updateSpeedValues();
	
			// init interval
			unsigned int interval = 1;
			val = m_cbUpdateInterval->GetValue();
			interval = wxAtoi(val);
			cncConfig->setUpdateInterval(interval);

			//initilaize cnc unit
			config->Read("CncConfig/Unit", &cfgStr, "mm"); 
			if ( cfgStr == "mm" ) { cncConfig->setUnit(CncMetric); m_unit->SetStringSelection("mm"); }
			else				  { cncConfig->setUnit(CncSteps);  m_unit->SetStringSelection("steps"); }
			updateUnit();
		}
	}
	
	//Initialize the postion controls
	cnc->setZeroPos();
	updateCncConfigTrace();
	
	// z slider
	cnc->updateZSlider();
	
	//initilaize debug state
	if ( m_menuItemDebugSerial->IsChecked() ) 	cnc->getSerial()->enableSpyOutput(true);
	else				    					cnc->getSerial()->enableSpyOutput(false); 
	
	isCncControlInitialized = true;
	return isCncControlInitialized;
}
///////////////////////////////////////////////////////////////////
bool MainFrame::initializeLruMenu() {
///////////////////////////////////////////////////////////////////
	//load lru list from config file
	lruFileList.load(lruStore);
	
	wxString cfgStr;
	config->Read("DefaultTemplate/Filename", &cfgStr, wxT(""));
	
	wxFileName fn;
	if ( cfgStr.length() > 0 ) 	fn.Assign(cfgStr);
	else 						fn.Assign(lruFileList.getFileName(0));
	
	if ( fn.Exists() ) {
		m_inputFileName->SetValue(fn.GetFullName());
		m_inputFileName->SetHint(fn.GetFullPath());
		
		openFile();
		prepareTplPreview();
		introduceCurrentFile();
	} else {
		config->Read("DefaultTemplate/DefaultDir", &cfgStr, wxT(""));
		m_dirCtrl->SetPath(cfgStr);
		m_dirCtrl->ExpandPath(cfgStr);
		updateSvgPreview("");
		highlightTplPreview(false);
				
		return false; 
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
void MainFrame::initializePreconfiguredSpeedSetups() {
///////////////////////////////////////////////////////////////////
	wxString fn(CncFileNameService::getSpeedConfigFileName());
	wxFileConfig cfg(wxT("CncController"), wxEmptyString, fn, fn, wxCONFIG_USE_RELATIVE_PATH | wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
	
	m_cbPreconfiguredSpeedSetups->Clear();
	
	wxString str;
	long dummy, cfgLong;
	
	// all groups...
	bool bCont = cfg.GetFirstGroup(str, dummy);
	unsigned int cnt = 0;
	while ( bCont ) {
		cnt++;
		wxString item(wxString::Format("%03d: ", cnt));
		item << str;
		item << wxString(' ', 50 - str.Length());
		
		item << "[";
		cfg.Read(str + "/FlySpeedXY", &cfgLong, 10); item << cfgLong; item << ", ";
		cfg.Read(str + "/FlySpeedZ", &cfgLong, 10); item << cfgLong; item << ", ";
		cfg.Read(str + "/WorkSpeedXY", &cfgLong, 10); item << cfgLong; item << ", ";
		cfg.Read(str + "/WorkSpeedZ", &cfgLong, 10); item << cfgLong; item << "]";
		
		m_cbPreconfiguredSpeedSetups->Append(item);
		bCont = cfg.GetNextGroup(str, dummy);
	}
	
	if ( m_cbPreconfiguredSpeedSetups->GetCount() > 0 ) {
		m_cbPreconfiguredSpeedSetups->Select(0);
		wxCommandEvent dummyEvent;
		selectPreconfiguredSpeedSetups(dummyEvent);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::performSpeedValueConfig(wxComboBox* cb, const wxString& item) {
///////////////////////////////////////////////////////////////////
	if ( cb == NULL )
		return;
		
	long test;
	item.ToLong(&test);
	if ( test < 0 || test > 100 )
		return;
	
	if ( cb->FindString(item, true) == wxNOT_FOUND ) {
		cb->Append(item);
	} 
	
	cb->SetStringSelection(item);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectPreconfiguredSpeedSetups(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString item = m_cbPreconfiguredSpeedSetups->GetStringSelection();
	int p1 = wxNOT_FOUND, p2 = wxNOT_FOUND;
	if ( (p1 = item.Find("[")) != wxNOT_FOUND ) {
		if ( (p2 = item.Find("]")) != wxNOT_FOUND && p2 >= p1 + 2) {
			item = item.SubString(p1 + 1, p2 - 1);
			
			wxStringTokenizer tokenizer(item, ",");
			unsigned int cnt = 0;
			while ( tokenizer.HasMoreTokens() ) {
				wxString token = tokenizer.GetNextToken();
				token.Trim(true).Trim(false);
				cnt++;
				
				switch ( cnt ) {
					case 1:		performSpeedValueConfig(m_flySpeedXY, token);
								break;
					case 2:		performSpeedValueConfig(m_flySpeedZ, token);
								break;
					case 3:		performSpeedValueConfig(m_workSpeedXY, token);
								break;
					case 4:		performSpeedValueConfig(m_workSpeedZ, token);
								break;
					default: 	std::cerr << "MainFrame::selectPreconfiguredSpeedSetups: Invalid token count: " << cnt << std::endl;
				}
			}
			
			updateSpeedValues();
		}
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::determineCncOutputControls() {
///////////////////////////////////////////////////////////////////
	//Determine output controls
	
	guiCtlSetup->mainWnd			= this;
	
	guiCtlSetup->motionMonitor			= motionMonitor;
	
	guiCtlSetup->xAxis 				= m_xAxis;
	guiCtlSetup->yAxis 				= m_yAxis;
	guiCtlSetup->zAxis 				= m_zAxis;
	
	guiCtlSetup->xAxisCtl			= m_xAxisCtl;
	guiCtlSetup->yAxisCtl			= m_yAxisCtl;
	guiCtlSetup->zAxisCtl			= m_zAxisCtl;
	
	guiCtlSetup->cmdCount 			= m_cmdCount;
	guiCtlSetup->cmdDuration 		= m_cmdDuration;
	
	guiCtlSetup->staticCncConfig	= m_dvListCtrlStatic;
	guiCtlSetup->dynamicCncConfig 	= m_dvListCtrlDynamic;
	
	guiCtlSetup->passingTrace		= m_passingCount;
	guiCtlSetup->toolState 			= m_toolStateTrafficLight;
	
	guiCtlSetup->setterValues		= m_dvListCtrlLastSetters;
	guiCtlSetup->processedSetters	= m_dvListCtrlProcessedSetters;
	guiCtlSetup->controllerConfig	= m_dvListCtrlControllerConfig;
	guiCtlSetup->controllerPinReport= m_dvListCtrlControllerPins;
	guiCtlSetup->controllerErrorInfo= m_dvListCtrlControllerErrorInfo;
	
	guiCtlSetup->motorState 		= m_miMotorEnableState;
	guiCtlSetup->zView				= m_zView;
	guiCtlSetup->speedView			= m_speedView;
	
	guiCtlSetup->xMinLimit 			= m_xMinLimit;
	guiCtlSetup->xMaxLimit 			= m_xMaxLimit;
	guiCtlSetup->yMinLimit 			= m_yMinLimit;
	guiCtlSetup->yMaxLimit 			= m_yMaxLimit;
	guiCtlSetup->zMinLimit 			= m_zMinLimit;
	guiCtlSetup->zMaxLimit 			= m_zMaxLimit;
	
	cnc->setGuiControls(guiCtlSetup);
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateUnit() {
///////////////////////////////////////////////////////////////////
	wxASSERT(m_unit);
	wxASSERT(cnc && cnc->getCncConfig());
	wxASSERT(m_xAxis); wxASSERT(m_yAxis); wxASSERT(m_zAxis);

	CncConfig* cncConfig = cnc->getCncConfig();
	wxString unit = m_unit->GetValue();
	CncUnit oldUnit = cncConfig->getUnit();

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
		cnc->getCncConfig()->setUnit(CncMetric); 
		m_metricX->SetValue("0.000");
		m_metricY->SetValue("0.000");
		m_metricZEdit->SetValue("0.000");
	} else {
		xLimit *= cncConfig->getStepsX();
		yLimit *= cncConfig->getStepsY();
		zLimit *= cncConfig->getStepsZ();

		cnc->getCncConfig()->setUnit(CncSteps);
		m_metricX->SetValue("0");
		m_metricY->SetValue("0");
		m_metricZEdit->SetValue("0");
	}
	
	valX.SetRange(-xLimit, +xLimit);
	valY.SetRange(-yLimit, +yLimit);
	valZ.SetRange(-zLimit, +zLimit);
	m_metricX->SetValidator(valX);
	m_metricY->SetValidator(valY);
	m_metricZEdit->SetValidator(valZ);
	
	// manual control
	m_mmUnitX->SetLabel(unit);
	m_mmUnitY->SetLabel(unit);

	CncUnit newUnit = cncConfig->getUnit();
	wxString x = m_xAxis->GetLineText(0);
	wxString y = m_yAxis->GetLineText(0);
	wxString z = m_zAxis->GetLineText(0);
	double xd; x.ToCDouble(&xd);
	double yd; y.ToCDouble(&yd);
	double zd; z.ToCDouble(&zd);
	
	xd = cncConfig->convertX(oldUnit, newUnit, xd);
	m_xAxis->Clear(); m_xAxis->SetValue(wxString() << xd);
	
	yd = cncConfig->convertY(oldUnit, newUnit, yd);
	m_yAxis->Clear(); m_yAxis->SetValue(wxString() << yd);
	
	zd = cncConfig->convertZ(oldUnit, newUnit, zd);
	m_zAxis->Clear(); m_zAxis->SetValue(wxString() << zd);
	
	x = m_xAxisCtl->GetLineText(0);
	y = m_yAxisCtl->GetLineText(0);
	z = m_zAxisCtl->GetLineText(0);
	x.ToCDouble(&xd);
	y.ToCDouble(&yd);
	z.ToCDouble(&zd);

	xd = cncConfig->convertX(oldUnit, newUnit, xd);
	m_xAxisCtl->Clear(); m_xAxisCtl->SetValue(wxString() << xd);
	
	yd = cncConfig->convertY(oldUnit, newUnit, yd);
	m_yAxisCtl->Clear(); m_yAxisCtl->SetValue(wxString() << yd);
	
	zd = cncConfig->convertZ(oldUnit, newUnit, zd);
	m_zAxisCtl->Clear(); m_zAxisCtl->SetValue(wxString() << zd);
	
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
			m_outboundNotebook->InsertPage(OutboundSvgPage, m_svgEmuResult,  "", false);
			outboundNbInfo->decorate(OutboundSvgPage);
		}
		
		if (m_outboundNotebook->FindPage(m_svgEmuSource) == wxNOT_FOUND ) {
			m_outboundNotebook->InsertPage(OutboundSvgSource, m_svgEmuSource, "", false);
			outboundNbInfo->decorate(OutboundSvgSource);
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
	wxASSERT(cnc);
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
	CncConfig cc(*cnc->getCncConfig());
	
	delete cnc;
	wxString cs;
	
	disableControls();
	disableAllRunControls();
	hideSVGEmuResult();
	
	m_miRqtIdleMessages->Check(false);
	
	if ( sel == _portEmulatorNULL ) {
		cnc = new CncControl(CncEMU_NULL);
		cnc->updateCncConfig(cc);
		cs.assign("NULL");
		
	} else if ( sel == _portEmulatorSVG ) {
		cnc = new CncControl(CncEMU_SVG);
		cnc->updateCncConfig(cc);
		wxString val;
		cs.assign(CncFileNameService::getCncOutboundSvgFileName());
		showSVGEmuResult();
		enableMenuItem(m_miSaveEmuOutput, true);
		
	} else {
		cnc = new CncControl(CncPORT);
		cnc->updateCncConfig(cc);
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
		motionMonitor->setCncConfig(*cnc->getCncConfig());
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
///////////////////////////////////////////////////////////////////
	m_rcRun->Enable(state);
		
	m_rcStop->Enable(!state);
	m_btnEmergenyStop->Enable(!state);
	
	if ( isPause() ) {
		m_rcStop->Enable(true);
		m_btnEmergenyStop->Enable(true);
	}

	
	//todo
	m_rcReset->Enable(		isPause() == false 		&& state);
	
	m_rcPause->Enable(		svgDebugger == false 	&& !state);
	m_rcNextStep->Enable(	svgDebugger == true 	&& !state);
	m_rcNextPath->Enable(	svgDebugger == true 	&& !state);
	m_rcFinish->Enable(		svgDebugger == true 	&& !state);
}
///////////////////////////////////////////////////////////////////
void MainFrame::enableAllRunControls(bool state) {
///////////////////////////////////////////////////////////////////
	m_rcRun->Enable(state);
	m_rcPause->Enable(state);
	m_rcStop->Enable(state);
	m_rcReset->Enable(state);
	m_rcNextStep->Enable(state);
	m_rcNextPath->Enable(state);
	m_rcFinish->Enable(state);
	m_btnEmergenyStop->Enable(!state);
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
void MainFrame::reinit(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	isCncControlInitialized = false;
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
	if ( ret == true  ) {
		cnc->enableStepperMotors(true);
		if ( cnc->moveUpZ() == false ) {
			std::cerr << "MainFrame::setZero: Unable to initialize Z axis!"<< std::endl;
			ret = false;
		}
	} else {
		std::cerr << "MainFrame::setZero: Unable to reste the controller!"<< std::endl;
	}
	
	if ( ret == true )
		setRefPostionState(true);
	
	cnc->forceDisplayPositions();
	cnc->enableStepperMotors(false);
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
	wxString cmd, svgFile;
	config->Read("TemplateEditor/ExternalTool", &cmd, wxT("notepad "));
	getSvgEmuFileName(svgFile);
	openFileExtern(cmd, svgFile);
}
///////////////////////////////////////////////////////////////////
void MainFrame::svgEmuOpenFileAsSvg(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString cmd, svgFile;
	config->Read("EmulatorSVG/SVGFileViewer", &cmd, wxT("explorer "));
	getSvgEmuFileName(svgFile);
	openFileExtern(cmd, svgFile);
}
///////////////////////////////////////////////////////////////////
void MainFrame::killFocusMaxDimensionX(wxFocusEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc && cnc->getCncConfig());
	wxASSERT(m_maxXDimension);
	
	wxString val = m_maxXDimension->GetValue();
	double rbd;
	if ( val.length() > 0 )	val.ToDouble(&rbd);
	else					rbd = 0.0;
	
	cnc->getCncConfig()->setMaxDimensionX(rbd);
	updateCncConfigTrace();
	
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::killFocusMaxDimensionY(wxFocusEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc && cnc->getCncConfig());
	wxASSERT(m_maxYDimension);
	
	wxString val = m_maxYDimension->GetValue();
	double rbd;
	if ( val.length() > 0 )	val.ToDouble(&rbd);
	else					rbd = 0.0;
	
	cnc->getCncConfig()->setMaxDimensionY(rbd);
	updateCncConfigTrace();
	
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::killFocusMaxDimensionZ(wxFocusEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc && cnc->getCncConfig());
	wxASSERT(m_maxZDimension);
	
	wxString val = m_maxZDimension->GetValue();
	double rbd;
	if ( val.length() > 0 )	val.ToDouble(&rbd);
	else					rbd = 0.0;
	
	cnc->getCncConfig()->setMaxDimensionZ(rbd);
	updateCncConfigTrace();
	
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::killFocusRouterDiameter(wxFocusEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc && cnc->getCncConfig());
	wxASSERT(m_routerBitDiameter);
	
	wxString val = m_routerBitDiameter->GetValue();
	double rbd;
	if ( val.length() > 0 )	val.ToDouble(&rbd);
	else					rbd = 0.0;

	cnc->getCncConfig()->setRouterBitDiameter(rbd);
	updateCncConfigTrace();
	
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::killFocusReplyThreshold(wxFocusEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc && cnc->getCncConfig());
	
	wxString rt = m_replyThreshold->GetValue();
	long v = 0;
	rt.ToLong(&v);
	
	if ( (unsigned long)v != cnc->getCncConfig()->getRelyThreshold() ) {
		cnc->getCncConfig()->setRelyThreshold(v);
		cnc->setup(false);
		updateCncConfigTrace();
	}
	
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateReverseStepSignX(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc && cnc->getCncConfig() );
	wxString sel = m_cbStepSignX->GetStringSelection();
	if ( sel.MakeUpper() == "YES") 	cnc->getCncConfig()->setStepSignX(-1);
	else							cnc->getCncConfig()->setStepSignX(+1);
	
	cnc->setup(false);
	updateCncConfigTrace();
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateReverseStepSignY(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc && cnc->getCncConfig() );
	wxString sel = m_cbStepSignY->GetStringSelection();
	if ( sel.MakeUpper() == "YES") 	cnc->getCncConfig()->setStepSignY(-1);
	else							cnc->getCncConfig()->setStepSignY(+1);
	
	cnc->setup(false);
	updateCncConfigTrace();
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateInclWpt(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc && cnc->getCncConfig());
	cnc->getCncConfig()->setReferenceIncludesWpt(m_includingWpt->IsChecked());
	updateCncConfigTrace();
}
///////////////////////////////////////////////////////////////////
int MainFrame::showSetReferencePositionDlg(wxString msg) {
///////////////////////////////////////////////////////////////////
	wxMessageDialog dlg(this, msg, _T("Action required  . . . "), 
				wxCANCEL|wxYES|wxNO|wxCENTRE|wxICON_INFORMATION);
	dlg.SetYesNoCancelLabels("Set with workpiece thickness ", "Set without workpiece thieckness", "Do it later . . . ");
	
	int ret = dlg.ShowModal();
	switch ( ret ) {
		case  wxID_YES:  	m_includingWpt->SetValue(true);
							m_mainNotebook->SetSelection(MainReferencePage);
							setZero();
							break;
							
		case  wxID_NO:		m_includingWpt->SetValue(false);
							m_mainNotebook->SetSelection(MainReferencePage);
							setZero();
							break;
							
		default:			m_crossingThickness->SetFocus(); 
							//do nothing
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateCncConfigTrace() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc && cnc->getCncConfig());
	
	cnc->updateCncConfigTrace();
	cnc->updateZSlider();
	collectSummary();
	
	m_infoToolDiameter->SetLabel(wxString::Format("%.3lf", cnc->getCncConfig()->getRouterBitDiameter()));
}
///////////////////////////////////////////////////////////////////
void MainFrame::killFocusWorkpieceThickness(wxFocusEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc && cnc->getCncConfig());
	
	wxString val = m_workpieceThickness->GetValue();
	double wpt;
	if ( val.length() > 0 )	val.ToDouble(&wpt);
	else					wpt = 0.0;
	
	if ( cnc::dblCompareNull(wpt) == true )
		m_lableWorkpieceThickness->SetBitmap(ImageLib16().Bitmap("BMP_NO_WPT"));
	else 
		m_lableWorkpieceThickness->SetBitmap(ImageLib16().Bitmap("BMP_WPT"));
		
	m_lableWorkpieceThickness->SetToolTip(wxString::Format("Workpiece thickness: %.3lf mm", wpt));
	m_lableWorkpieceThickness->Refresh();
	
	if ( cnc->getCncConfig()->getWorkpieceThickness() != wpt ) {
		cnc->getCncConfig()->setWorkpieceThickness(wpt);
		updateCncConfigTrace();
		setRefPostionState(false);
		
		wxString msg("A workpiece thickness change requires a redefinition of the CNC reference position.\n\n");
		msg << "This have to be done before the next CNC run.\n\n";
		msg << "The set function below can be used to set it directly.";
		
		showSetReferencePositionDlg(msg);
	}
	
	m_crossings->ChangeValue(wxString() << cnc->getCncConfig()->getDurationCount());
	
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::killFocusCrossingThickness(wxFocusEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc && cnc->getCncConfig());
	wxASSERT(m_crossingThickness);
	
	wxString val = m_crossingThickness->GetValue();
	double ct;
	if ( val.length() > 0 )	val.ToDouble(&ct);
	else					ct = 2.0;
	
	
	if ( cnc->getCncConfig()->getMaxDurationThickness() != ct ) {
		cnc->getCncConfig()->setMaxDurationThickness(ct);
		updateCncConfigTrace();
	}
	
	m_crossings->ChangeValue(wxString() << cnc->getCncConfig()->getDurationCount());
	
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::killFocusMaxSpeedXY(wxFocusEvent& event) {
///////////////////////////////////////////////////////////////////
	updateSpeedValues();
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateFlySpeedXY(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	updateSpeedValues();
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateWorkSpeedXY(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	updateSpeedValues();
}
///////////////////////////////////////////////////////////////////
void MainFrame::killFocusMaxSpeedZ(wxFocusEvent& event) {
///////////////////////////////////////////////////////////////////
	updateSpeedValues();
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateWorkSpeedZ(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	updateSpeedValues();
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateCurrentSpeedXY(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	updateSpeedValues();
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateCurrentSpeedZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	updateSpeedValues();
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateFlySpeedZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	updateSpeedValues();
}
///////////////////////////////////////////////////////////////////
void MainFrame::configureXYSpeedWithZValues(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_flySpeedXY->SetStringSelection(m_flySpeedZ->GetStringSelection());
	m_workSpeedXY->SetStringSelection(m_workSpeedZ->GetStringSelection());
	m_currentSpeedXY->SetStringSelection(m_currentSpeedZ->GetStringSelection());
	updateSpeedValues();
}
///////////////////////////////////////////////////////////////////
void MainFrame::configureZSpeedWithXYValues(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_flySpeedZ->SetStringSelection(m_flySpeedXY->GetStringSelection());
	m_workSpeedZ->SetStringSelection(m_workSpeedXY->GetStringSelection());
	m_currentSpeedZ->SetStringSelection(m_currentSpeedXY->GetStringSelection());
	updateSpeedValues();
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateSpeedValues() {
///////////////////////////////////////////////////////////////////
	wxString maxXY 	= m_maxXYSpeed->GetValue();
	wxString flyXY  = m_flySpeedXY->GetStringSelection();
	wxString workXY = m_workSpeedXY->GetStringSelection();
	wxString selXY  = m_currentSpeedXY->GetStringSelection();
	
	wxString maxZ 	= m_maxZSpeed->GetValue();
	wxString flyZ   = m_flySpeedZ->GetStringSelection();
	wxString workZ  = m_workSpeedZ->GetStringSelection();
	wxString selZ   = m_currentSpeedZ->GetStringSelection();
	
	long mXY=0, mZ = 0, fXY=0, wXY=0, fZ=0, wZ=0;
	if ( maxXY.length()  > 0 ) 	maxXY.ToLong(&mXY);
	if ( flyXY.length()  > 0 )	flyXY.ToLong(&fXY);
	if ( workXY.length() > 0 )	workXY.ToLong(&wXY);
	
	if ( maxZ.length()   > 0 ) 	maxXY.ToLong(&mZ);
	if ( flyZ.length()   > 0 )	flyZ.ToLong(&fZ);
	if ( workZ.length()  > 0 )	workZ.ToLong(&wZ);

	if ( mXY <= 0 ) mXY = 1;
	if ( mZ  <= 0 ) mZ  = 1;
	
	cnc->getCncConfig()->setMaxSpeedXY(mXY);
	cnc->getCncConfig()->setMaxSpeedZ(mZ);
	
	double val; 
	if ( fXY != 0 && wXY != 0 ) {
		val = cnc->getCncConfig()->getMaxSpeedXY() * fXY/100;
		cnc->getCncConfig()->setFlySpeedXY((int)val);

		val = cnc->getCncConfig()->getMaxSpeedXY() * wXY/100;
		cnc->getCncConfig()->setWorkSpeedXY((int)val);
	} else {
		cnc->getCncConfig()->setFlySpeedXY(1);
		cnc->getCncConfig()->setWorkSpeedXY(1);
	}

	if ( fZ != 0 && wZ != 0 ) {
		val = cnc->getCncConfig()->getMaxSpeedZ() * fZ/100;
		cnc->getCncConfig()->setFlySpeedZ((int)val);

		val = cnc->getCncConfig()->getMaxSpeedZ() * wZ/100;
		cnc->getCncConfig()->setWorkSpeedZ((int)val);
	} else {
		cnc->getCncConfig()->setFlySpeedZ(1);
		cnc->getCncConfig()->setWorkSpeedZ(1);
	}
	
	if ( cnc->isConnected() ) {
		if (selXY.MakeUpper() == "FLY")	cnc->changeWorkSpeedXY(CncSpeedFly, true);
		else							cnc->changeWorkSpeedXY(CncSpeedWork, true);
		
		if (selZ.MakeUpper()  == "FLY")	cnc->changeWorkSpeedZ(CncSpeedFly, true);
		else							cnc->changeWorkSpeedZ(CncSpeedWork, true);
		
	} else {
		cnc->getCncConfig()->setActiveSpeedXY(CncSpeedFly);
		cnc->getCncConfig()->setActiveSpeedZ(CncSpeedFly);
	}

	updateCncConfigTrace();
	
	m_speedView->setMaxSpeedX(cnc->getCncConfig()->getMaxSpeedXY());
	m_speedView->setMaxSpeedY(cnc->getCncConfig()->getMaxSpeedXY());
	m_speedView->setMaxSpeedZ(cnc->getCncConfig()->getMaxSpeedZ());

	if ( m_menuItemToolControls->IsChecked() == true ) {
		m_speedView->setCurrentSpeedX(cnc->getCncConfig()->getSpeedX());
		m_speedView->setCurrentSpeedY(cnc->getCncConfig()->getSpeedY());
		m_speedView->setCurrentSpeedZ(cnc->getCncConfig()->getSpeedZ());
	}
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
	wxASSERT(cnc && cnc->getCncConfig() );
	
	cnc->getCncConfig()->setOnlineUpdateCoordinates(m_menuItemUpdCoors->IsChecked());
	cnc->getCncConfig()->setAllowEventHandling(m_menuItemAllowEvents->IsChecked());
	cnc->getCncConfig()->setOnlineUpdateDrawPane(m_menuItemUpdDraw->IsChecked());
	cnc->getCncConfig()->setAllowEventHandling(m_menuItemDebugSerial->IsChecked());
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
	unsigned int sel = m_mainNotebook->GetSelection();
	if ( sel == MainManuallyPage )
		return TplManual;
		
	if ( sel == MainTestPage )
		return TplTest;
		
	wxFileName fn;
	if ( fileName == NULL )	fn.Assign(getCurrentTemplatePathFileName());
	else					fn.Assign(fileName);
	
	wxString ext(fn.GetExt());
	ext.MakeUpper();
	
	if      ( ext == "SVG" )	return TplSvg;
	else if ( ext == "GCODE") 	return TplGcode;
	else if ( ext == "TXT") 	return TplText;

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
void MainFrame::showTplPreview(bool show) {
///////////////////////////////////////////////////////////////////
	if ( show == false ) {
		if (m_templateNotebook->FindPage(m_panelTplDebugger) != wxNOT_FOUND )
			m_templateNotebook->RemovePage(m_templateNotebook->FindPage(m_panelTplDebugger));

		if (m_templateNotebook->FindPage(m_panelTplUserAgent) != wxNOT_FOUND )
			m_templateNotebook->RemovePage(m_templateNotebook->FindPage(m_panelTplUserAgent));
			
		if (m_templateNotebook->FindPage(m_panelTplPreview) != wxNOT_FOUND )
			m_templateNotebook->RemovePage(m_templateNotebook->FindPage(m_panelTplPreview));
			
	} else {
		// correct insert order is very important
		if (m_templateNotebook->FindPage(m_panelTplUserAgent) == wxNOT_FOUND ) {
			m_templateNotebook->InsertPage(TemplateUserAgentPage, m_panelTplUserAgent, "", false);
			templateNbInfo->decorate(TemplateUserAgentPage);
		}

		if (m_templateNotebook->FindPage(m_panelTplDebugger) == wxNOT_FOUND ) {
			m_templateNotebook->InsertPage(TemplateDebuggerPage, m_panelTplDebugger, "", false);
			templateNbInfo->decorate(TemplateDebuggerPage);
		}

		if (m_templateNotebook->FindPage(m_panelTplPreview) == wxNOT_FOUND ) {
			m_templateNotebook->InsertPage(TemplatePreviewPage, m_panelTplPreview, "", false);
			templateNbInfo->decorate(TemplatePreviewPage);
		}
	}
}
///////////////////////////////////////////////////////////////////
bool MainFrame::openFile(int pageToSelect) {
///////////////////////////////////////////////////////////////////
	// First select the template page to get the rigth result 
	// by getCurrentTemplateFormat
	m_mainNotebook->SetSelection(MainTemplatePage);

	templateFileLoading = true;
	bool ret = false;
	switch ( getCurrentTemplateFormat() ) {
		case TplSvg:
		case TplGcode:
			ret = openTextFile();
			if ( ret == true ) {
				showTplPreview();
				if ( pageToSelect < 0 )	m_templateNotebook->SetSelection(TemplatePreviewPage);
				else					m_templateNotebook->SetSelection(pageToSelect);
			}
			break;
		case TplText: 
			ret = openTextFile();
			if ( ret == true ) {
				hideTplPreview();
				if ( pageToSelect < 0 )	m_templateNotebook->SetSelection(TemplateContentPage);
				else					m_templateNotebook->SetSelection(pageToSelect);
			}
			break;
		default:
			hideTplPreview();
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
	
	ignoreDirControlEvents = true;
	{
		//SVG Files (*.svg)|*.svg|GCode Files (*.gcode)|*.gcode|Text Files (*.txt)|*.txt
		switch ( getCurrentTemplateFormat() ) {
			case TplSvg:	m_dirCtrl->SetFilterIndex(0); break;
			case TplGcode:	m_dirCtrl->SetFilterIndex(1); break;
			case TplText: 	m_dirCtrl->SetFilterIndex(2); break;
			default: std::cerr << "MainFrame::introduceCurrentFile(): Invalid extention!" << std::endl;
		}
		
		if ( m_dirCtrl->GetFilterListCtrl() ) 
			m_dirCtrl->GetFilterListCtrl()->Select(m_dirCtrl->GetFilterIndex());
			
		m_dirCtrl->ReCreateTree();
		m_dirCtrl->SetPath(getCurrentTemplatePathFileName());
		m_dirCtrl->ExpandPath(getCurrentTemplatePathFileName());
	}
	ignoreDirControlEvents = false;
	highlightTplPreview(false);
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
	
	std::cerr << "Error while opne file: " << getCurrentTemplatePathFileName().c_str() << std::endl;
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
                                "SVG Files (*.svg)|*.svg|GCode Files (*.gcode)|*.gcode|Text Files (*.txt)|*.txt", 
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
			prepareTplPreview(true);
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
                                "SVG Files (*.svg)|*.svg|GCode Files (*.gcode)|*.gcode|Text Files (*.txt)|*.txt", 
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
		prepareTplPreview(true);
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
	if ( !openFile(TemplateContentPage) ) {
		std::cerr << "Error while reloding template: " << getCurrentTemplateFileName().c_str() << std::endl;
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::openTemplateSourceExtern(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	saveFile();
		
	wxString cmd;
	config->Read("TemplateEditor/ExternalTool", &cmd, wxT("notepad"));
	openFileExtern(cmd, getCurrentTemplatePathFileName());
}
///////////////////////////////////////////////////////////////////
void MainFrame::openTemplateSvgExtern(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString cmd;
	
	switch ( getCurrentTemplateFormat() ) {
		case TplSvg:
					saveFile();
					config->Read("App/SVGFileEditor", &cmd, wxT("notepad"));
					openFileExtern(cmd, getCurrentTemplatePathFileName());
					break;
		case TplGcode:
					saveFile();
					config->Read("App/GCodeFileEditor", &cmd, wxT("notepad"));
					openFileExtern(cmd, getCurrentTemplatePathFileName());
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
			std::clog << "Template change detected . . . Reload Template file: " << fn.c_str() << std::endl;
			if ( !openFile() ) {
					std::cerr << "MainFrame::activateMainWindow: Error while open file: " << fn.c_str() << std::endl;
			}
			prepareTplPreview(true);
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
	m_mainNotebook->SetSelection(MainTemplatePage);

	bool ret = false;
	switch ( getCurrentTemplateFormat() ) {
		case TplSvg:
		case TplGcode:
		case TplText: 
			ret = saveTextFile();
			break;
		default:
			std::cerr << "MainFrame::saveFile(): Unknown Type: " << getCurrentTemplateFormat() << std::endl;
	}
	
	if( ret == true ) {
		m_stcFileContent->DiscardEdits();
		m_stcFileContent->EmptyUndoBuffer();
		evaluateTemplateModificationTimeStamp();
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
								"SVG Files (*.svg)|*.svg|GCode Files (*.gcode)|*.gcode|Text Files (*.txt)|*.txt",  
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

	if ( svgDebugger == true ) {
		SVGFileParser::DebugControls sdc;
		sdc.debuggerControlBase 	= m_dvListCtrlSvgDebuggerInfoBase;
		sdc.debuggerControlPath		= m_dvListCtrlSvgDebuggerInfoPath;
		sdc.debuggerControlDetail	= m_dvListCtrlSvgDebuggerInfoDetail;
		sdc.debugPreprocessing		= m_miRcPreprocessing;
		sdc.debugUserAgent			= m_miRcUserAgent;
		sdc.debugSpooling			= m_miRcSpooling;
		sdc.debugPhase				= m_debugPhase;
		
		inboundFileParser->setDebuggerControls(sdc);
		
		ret = inboundFileParser->processDebug();
		clearDebugControls();

	} else {
		ret = inboundFileParser->processRelease();
	}
	
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

	inboundFileParser = new GCodeFileParser(getCurrentTemplatePathFileName().c_str(), cnc);
	return processVirtualTemplate();
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processTextTemplate() {
///////////////////////////////////////////////////////////////////
	wxASSERT(m_stcFileContent);

	bool anythingDone = false;

	// SVG Serial Emulator Support
	cnc->getSerial()->beginSVG(mm, 
	                           cnc->getCncConfig()->getMaxDimensionX(), 
							   cnc->getCncConfig()->getMaxDimensionY());
	cnc->getSerial()->beginPath(cnc->getCurPos().getX() * cnc->getCncConfig()->getDisplayFactX(), 
	                            cnc->getCurPos().getY() * cnc->getCncConfig()->getDisplayFactY());
	
	for (long i=0; i<m_stcFileContent->GetNumberOfLines(); i++) {
		m_stcFileContent->ShowPosition(i);
		wxString line = m_stcFileContent->GetLineText(i);
		line = line.Trim(false);
		line = line.Trim(true);
		
		if ( !line.StartsWith("#") ) {
			cnc->moveDownZ();
			
			if ( line.length() > 1 ) {
				anythingDone = true;
				cnc->processCommand(line.c_str(), std::cout);
			} else if (line.length() == 1 ) {
				anythingDone = true;
				unsigned const char c = static_cast<unsigned char>(line[0]);
				cnc->processCommand(c, std::cout);
			}
			
			cnc->moveUpZ();
		}
	}
	
	// SVG Serial Emulator Support
	cnc->getSerial()->closePath();
	cnc->getSerial()->closeSVG();
	
	refreshSvgEmuFile();
	
	return anythingDone;
}
///////////////////////////////////////////////////////////////////
bool MainFrame::processManualTemplate() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	// SVG Serial Emulator Support
	cnc->getSerial()->beginSVG(mm, 
							   cnc->getCncConfig()->getMaxDimensionX(), 
							   cnc->getCncConfig()->getMaxDimensionY());
	cnc->getSerial()->beginPath(cnc->getCurPos().getX() * cnc->getCncConfig()->getDisplayFactX(), 
								cnc->getCurPos().getY() * cnc->getCncConfig()->getDisplayFactY());
		
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

		if ( m_metricZ->GetStringSelection() == "Up" ) {
			cnc->moveUpZ();
		} else {
			cnc->moveDownZ();
		}
		
		// transform to mm
		if ( m_unit->GetValue() == "steps" ) {
			xd *= cnc->getCncConfig()->getDisplayFactX();
			yd *= cnc->getCncConfig()->getDisplayFactY();
		}

		double moveX = xd, moveY = yd;
		// transfer to absolute coordinates
		if ( sel == 0 ) {
			moveX = xd - cnc->getCurPos().getX() * cnc->getCncConfig()->getDisplayFactX();
			moveY = yd - cnc->getCurPos().getY() * cnc->getCncConfig()->getDisplayFactY();
		}

		//cnc->initNextDuration(); will be done by manualSimpleMoveMetric
		cnc->manualSimpleMoveMetric(moveX, moveY, 0.0);
		
		cnc->resetDurationCounter();
		cnc->enableStepperMotors(true);
		cnc->moveUpZ();
		cnc->enableStepperMotors(false);

	} else {
		wxString zs = m_metricZEdit->GetValue();
		double zd; 
		if ( zs != "" )	zs.ToCDouble(&zd);
		else			zd = 0.0;
		
		// transform to mm
		if ( m_unit->GetValue() == "steps" ) 
			zd *= cnc->getCncConfig()->getDisplayFactX();
			
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
							   cnc->getCncConfig()->getMaxDimensionX(), 
							   cnc->getCncConfig()->getMaxDimensionY());
	cnc->getSerial()->beginPath(cnc->getCurPos().getX() * cnc->getCncConfig()->getDisplayFactX(), 
								cnc->getCurPos().getY() * cnc->getCncConfig()->getDisplayFactY());
								
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
	
	if ( v > 0.0 ) {
		m_maxXDimension->SetValue(val);
		cnc->getCncConfig()->setMaxDimensionX(v);
		
		if ( m_testDimTakeOverAndSave->GetValue() == true )
			config->Write("CncConfig/MaxDimensionsX", m_maxXDimension->GetValue());
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::testDimTakeOverY(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	double v;
	wxString val = m_testDimResultY->GetValue();
	val.ToDouble(&v);
	
	if ( v > 0.0 ) {
		m_maxYDimension->SetValue(m_testDimResultY->GetValue());
		cnc->getCncConfig()->setMaxDimensionY(v);
		
		if ( m_testDimTakeOverAndSave->GetValue() == true )
			config->Write("CncConfig/MaxDimensionsY", m_maxYDimension->GetValue());
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::testDimTakeOverZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	double v;
	wxString val = m_testDimResultZ->GetValue();
	val.ToDouble(&v);

	if ( v > 0.0 ) {
		m_maxZDimension->SetValue(m_testDimResultZ->GetValue());
		cnc->getCncConfig()->setMaxDimensionZ(v);
		
		if ( m_testDimTakeOverAndSave->GetValue() == true )
			config->Write("CncConfig/MaxDimensionsZ", m_maxZDimension->GetValue());
	}
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
		
	dispatch();
	cnc->manualSimpleMoveMetric3D(xd, yd, zd);
	dispatch();
	
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
	m_outboundNotebook->SetSelection(OutboundCNCValuesPage);
	m_notebookConfig->SetSelection(ConfigSummaryPage);
	
	if ( isZeroReferenceValid == false ) {
		
		TemplateFormat tf = getCurrentTemplateFormat();
		
		if ( tf != TplManual && tf != TplTest ) {
			wxString msg("The current reference position isn't valid due to a setup change or it isn't not initialized yet.\n\n");
			msg << "How to fix it:\n";
			msg << "Please use the \"Set Current Position to Zero\" functionality on the \"References\" tab.";
			
			showAuiPane("MainView");
			m_mainNotebook->SetSelection(MainReferencePage);
			
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
		manuallyDissolveEndSwitchDlg();
		// always return false to reconfigure zero in this sitiuation
		return false;
	}
 
	return showConfigSummaryAndConfirmRun();
}
///////////////////////////////////////////////////////////////////
bool MainFrame::showConfigSummaryAndConfirmRun() {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	wxString mode = m_cbRunConfirmationMode->GetStringSelection();
	switch ( (char)mode[0] ) {
		// alwyays
		case 'a':	break;
		// Serial Port only
		case 'c': 	if ( cnc->getSerial()->isEmulator() )
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
	disableAllRunControls();
	m_btCancelRun->Enable(true);
	m_btConfirmRun->Enable(true);
	
	// select summary page
	m_outboundNotebook->SetSelection(OutboundCNCValuesPage);
	m_notebookConfig->SetSelection(ConfigSummaryPage);
	
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
		dispatch();
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
		
	if ( cnc->getCncConfig() == NULL )
		return;
		
	CncConfig* cc = cnc->getCncConfig();
	
	DcmItemList rows;
	DataControlModel::addNumParameterValueUnitRow(rows, "Tool Diameter",					wxString::Format(" %.3f", 	cc->getRouterBitDiameter()), 		" mm"); 
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
			case ConfigSummaryPage:
					collectSummary();
					break;
					
			case CNCControllerPinsPage:
					if ( runActive == true ) {
						cnc::trc.logWarning("During an active run no controller requests are possible! Ty it later again.");
						return;
					}
					requestControllerPinsFromButton(dummyEvent);
					break;
					
			case CNCControllerConfigPage:
					if ( runActive == true ) {
						cnc::trc.logWarning("During an active run no controller requests are possible! Ty it later again.");
						return;
					}
					requestControllerConfigFromButton(dummyEvent);
					break;
					
			case CNCControllerErrorPage:
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
void MainFrame::processTemplate() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	if ( cnc->isInterrupted() ) {
		std::cerr << "Cnc controller state is interrupted. Before it can run next time a reset is required!" << std::endl;
		return;
	}
	
	if( cnc->isConnected() == false) {
		if ( !connectSerialPort() )
			return;
	}
	
	showAuiPane("Outbound");
		
	if ( checkIfRunCanBeProcessed() == false )
		return;
		 
	if ( m_cbClearProceesdSetterList->GetStringSelection().MakeUpper() == "YES" ) {
		m_dvListCtrlProcessedSetters->DeleteAllItems();
	}
	
	if ( m_cbUseProceesdSetterList->GetStringSelection().MakeUpper() == "YES" ) {
		 guiCtlSetup->processedSetters = m_dvListCtrlProcessedSetters;
		 cnc->setGuiControls(guiCtlSetup);
	} else {
		m_dvListCtrlProcessedSetters->DeleteAllItems();
		guiCtlSetup->processedSetters = NULL;
		cnc->setGuiControls(guiCtlSetup);
	}
	
	startAnimationControl();

	// select draw pane
	m_outboundNotebook->SetSelection(Outbound3DPage);
		
	// select template Page
	if ( m_mainNotebook->GetSelection() != MainManuallyPage && 
	     m_mainNotebook->GetSelection() != MainTestPage && 
	     m_mainNotebook->GetSelection() != MainTemplatePage ) {
		m_mainNotebook->SetSelection(MainTemplatePage);
	}
	
	motionMonitor->pushProcessMode();

	updateStepDelay();
	disableControls();
	resetMinMaxPositions();
	updateCncConfigTrace();
	cnc->getCncConfig()->setAllowEventHandling(true);
	cnc->processCommand("r", std::cout);
	cnc->logProcessingStart();
	cnc->enableStepperMotors(true);
	freezeLogger();
	

	bool ret = false;
	switch ( getCurrentTemplateFormat() ) {
		case TplSvg:
			if ( checkIfTemplateIsModified() == false )
				break;
			cnc->clearDrawControl();
			cnc->resetZSlider();
			ret = processSVGTemplate();
			break;
		case TplGcode:
			if ( checkIfTemplateIsModified() == false )
				break;
			cnc->clearDrawControl();
			cnc->resetZSlider();
			ret = processGCodeTemplate();
			break;
		case TplText:
			ret = processTextTemplate();
			break;
		case TplManual: 
			ret = processManualTemplate();
			break;
		case TplTest:
			cnc->clearDrawControl();
			cnc->resetZSlider();
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
	
	unfreezeLogger();
	enableControls();
	stopAnimationControl();
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
void MainFrame::mainBookPageChanging(wxNotebookEvent& event) {
///////////////////////////////////////////////////////////////////
	// currently not in use
}
///////////////////////////////////////////////////////////////////
void MainFrame::mainBookPageChanged(wxNotebookEvent& event) {
///////////////////////////////////////////////////////////////////
	unsigned int sel = event.GetSelection();
	
	if ( (wxWindow*)event.GetEventObject() == m_mainNotebook ) {
		// do nothing 
	} else if ( (wxWindow*)event.GetEventObject() == m_templateNotebook ) {
		if ( sel == TemplatePreviewPage ) {
			prepareTplPreview();
		} else if ( sel == TemplateContentPage) {
			if ( m_stcFileContent->HasFocus() == false )
				m_stcFileContent->SetFocus();
		}
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::prepareTplPreview(bool force) {
///////////////////////////////////////////////////////////////////
	// wxString tfn(CncFileNameService::getCncTemplatePreviewFileName(getCurrentTemplateFormat()));
	// this causes file access errors between this app and the internet explorer
	// write a temp file instead to have anytime a new one
	
	if ( force == true )
		lastTemplateFileNameForPreview.clear();

	// check if a preview update is necessary
	if ( lastTemplateFileNameForPreview == getCurrentTemplatePathFileName() && m_stcFileContent->GetModify() == false)
		return;
		
	lastTemplateFileNameForPreview = getCurrentTemplatePathFileName();

	wxString tfn(CncFileNameService::getTempFileName(getCurrentTemplateFormat()));
	
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
		
		openSvgPreview(tfn, getCurrentTemplateFormat());
		
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
	wxASSERT(cnc);
	wxASSERT(cnc->getCncConfig());

	unsigned int interval = 1;
	wxString val = m_cbUpdateInterval->GetValue();
	interval = wxAtoi(val);
	cnc->getCncConfig()->setUpdateInterval(interval);
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
	dispatch();
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
	dispatch();
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
	dispatch();
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
	dispatch();
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
	dispatch();
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
	dispatch();
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
	dispatch();
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
	dispatch();
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
	m_outboundNotebook->SetSelection(OutboundCNCValuesPage);
	m_notebookConfig->SetSelection(CNCControllerConfigPage);
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
	m_outboundNotebook->SetSelection(OutboundCNCValuesPage);
	m_notebookConfig->SetSelection(CNCControllerErrorPage);
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
	cnc->reset();
	cnc->clearDrawControl();
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
		
		prepareTplPreview(true);
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
	
	bool ctlKey = (GetAsyncKeyState(VK_CONTROL) != 0);
	int c = event.GetUnicodeKey();
	
	wxString find(m_stcFileContent->GetSelectedText());
	
	if ( c == 'F' && ctlKey == true ) {
		event.Skip(false);
		
		if ( find.IsEmpty() == false ) 
			m_svgEditSearch->SetValue(find);
			
		m_svgEditSearch->SetFocus();
		return;
		
	} else if ( c == 'G' && ctlKey == true ) {
		wxTextEntryDialog dlg(this, "Line Number:", "Go to line . . .", "");
		dlg.SetMaxLength(32);
		dlg.SetTextValidator(wxFILTER_NUMERIC);
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
	cnc->moveXToMid();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveYToMid(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	cnc->moveYToMid();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveZToMid(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT( cnc );
	cnc->moveZToMid();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveToZeroXY(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
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
	disableControls();
	cnc->simpleMoveZToZeroPos();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveXToMin(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	disableControls();
	cnc->moveXToMinLimit();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveXToMax(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	disableControls();
	cnc->moveXToMaxLimit();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveYToMin(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	disableControls();
	cnc->moveYToMinLimit();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveYToMax(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	disableControls();
	cnc->moveYToMaxLimit();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveZToMin(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	disableControls();
	cnc->moveZToMinLimit();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveZToMax(wxCommandEvent& event){
///////////////////////////////////////////////////////////////////
	disableControls();
	cnc->moveZToMaxLimit();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveZToTop(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	disableControls();
	cnc->moveZToTop();
	enableControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveZToBottom(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	disableControls();
	cnc->moveZToBottom();
	enableControls();
}
///////////////////////////////////////////////////////////////////
SvgOutputParameters& MainFrame::evaluteSvgOutputParameters(SvgOutputParameters& sop) {
///////////////////////////////////////////////////////////////////
	wxString sel = m_cbSvgZoom->GetStringSelection();
	sel.ToDouble(&sop.zoomFactor);
	
	sel = m_cbSvgIncludeOriginalPath->GetStringSelection();
	if (sel.MakeUpper() == "YES" ) 	sop.copyOrigPath = true;
	else							sop.copyOrigPath = false;
	
	sel = m_cbSvgFirstCrossing->GetStringSelection();
	if (sel.MakeUpper() == "YES" ) 	sop.onlyFirstCrossing = true;
	else							sop.onlyFirstCrossing = false;
	
	return sop;
}
///////////////////////////////////////////////////////////////////
void MainFrame::openConfigurationFile(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString cmd;
	config->Read("TemplateEditor/ExternalTool", &cmd, wxT("notepad "));
	openFileExtern(cmd, CncFileNameService::getConfigFileName());
}
///////////////////////////////////////////////////////////////////
void MainFrame::openExternalEditor(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString cmd;
	wxString tool("Tools/ExternalEditor");
	config->Read(tool, &cmd, wxT("notepad "));
	if ( cmd.IsEmpty() ) {
		std::cerr << "Nothing configured for: " << tool << std::endl;
		return;
	}
	openFileExtern(cmd, _(""));
}
///////////////////////////////////////////////////////////////////
void MainFrame::openPyCam(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString cmd;
	wxString tool("Tools/PyCAM");
	config->Read(tool, &cmd, wxT(""));
	if ( cmd.IsEmpty() ) {
		std::cerr << "Nothing configured for: " << tool << std::endl;
		return;
	}
	openFileExtern(cmd, _(""));
}
///////////////////////////////////////////////////////////////////
void MainFrame::openCalculator(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString cmd("calc");
	openFileExtern(cmd, _(""));
}
///////////////////////////////////////////////////////////////////
void MainFrame::clearDebugControls() {
///////////////////////////////////////////////////////////////////
	m_dvListCtrlSvgDebuggerInfoBase->DeleteAllItems();
	m_dvListCtrlSvgDebuggerInfoPath->DeleteAllItems();
	m_dvListCtrlSvgDebuggerInfoDetail->DeleteAllItems();
	m_svgDebuggerKey->Clear();
	m_svgDebuggerValue->Clear();
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectSvgDebuggerInfo(wxDataViewListCtrl* ctl) {
///////////////////////////////////////////////////////////////////
	if ( ctl->HasSelection() ) {
		int sel = ctl->GetSelectedRow();
		m_svgDebuggerKey->SetValue(ctl->GetTextValue(sel, 0));
		m_svgDebuggerValue->SetValue(ctl->GetTextValue(sel, 1));
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectSvgDebuggerInfoBase(wxDataViewEvent& event) {
///////////////////////////////////////////////////////////////////
	selectSvgDebuggerInfo(m_dvListCtrlSvgDebuggerInfoBase);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectSvgDebuggerInfoPath(wxDataViewEvent& event) {
///////////////////////////////////////////////////////////////////
	selectSvgDebuggerInfo(m_dvListCtrlSvgDebuggerInfoPath);
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectSvgDebuggerInfoDetail(wxDataViewEvent& event) {
///////////////////////////////////////////////////////////////////
	selectSvgDebuggerInfo(m_dvListCtrlSvgDebuggerInfoDetail);
}
///////////////////////////////////////////////////////////////////
void MainFrame::moveManuallySliderX(wxScrollEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString val;
	if ( m_unit->GetValue() == "steps" ) {
		val = wxString::Format(wxT("%d"), (int)(m_xManuallySlider->GetValue() * cnc->getCncConfig()->getCalculationFactX()));
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
		val = wxString::Format(wxT("%d"), (int)(m_yManuallySlider->GetValue() * cnc->getCncConfig()->getCalculationFactY()));
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
		val = wxString::Format(wxT("%d"), (int)(m_zManuallySlider->GetValue() * cnc->getCncConfig()->getCalculationFactZ()));
	} else {
		val = wxString::Format(wxT("%4.3f"), (double)(m_zManuallySlider->GetValue()));
	}
	m_metricZEdit->SetValue(val);
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
	m_metricZEdit->Enable(zFlag);
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
		val = wxString::Format(wxT("%d"), (int)(m_xManuallySlider->GetValue() * cnc->getCncConfig()->getCalculationFactX()));
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
		val = wxString::Format(wxT("%d"), (int)(m_xManuallySlider->GetValue() * cnc->getCncConfig()->getCalculationFactX()));
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
		val = wxString::Format(wxT("%d"), (int)(m_yManuallySlider->GetValue() * cnc->getCncConfig()->getCalculationFactY()));
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
		val = wxString::Format(wxT("%d"), (int)(m_yManuallySlider->GetValue() * cnc->getCncConfig()->getCalculationFactY()));
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
		val = wxString::Format(wxT("%d"), (int)(m_zManuallySlider->GetValue() * cnc->getCncConfig()->getCalculationFactZ()));
	} else {
		val = wxString::Format(wxT("%4.3f"), (double)(m_zManuallySlider->GetValue()));
	}
	m_metricZEdit->SetValue(val);
}
///////////////////////////////////////////////////////////////////
void MainFrame::maxManuallyZSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_zManuallySlider->SetValue(m_zManuallySlider->GetMax());
	
	wxString val;
	if ( m_unit->GetValue() == "steps" ) {
		val = wxString::Format(wxT("%d"), (int)(m_zManuallySlider->GetValue() * cnc->getCncConfig()->getCalculationFactZ()));
	} else {
		val = wxString::Format(wxT("%4.3f"), (double)(m_zManuallySlider->GetValue()));
	}
	m_metricZEdit->SetValue(val);
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
	m_metricZEdit->SetValue(val);
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
		m_xManuallySlider->SetValue(v / cnc->getCncConfig()->getCalculationFactX());
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
		m_yManuallySlider->SetValue(v / cnc->getCncConfig()->getCalculationFactY());
	} else {
		val = wxString::Format(wxT("%4.3f"), v);
		m_yManuallySlider->SetValue(v);
	}
	
	m_metricY->SetValue(val);
}
///////////////////////////////////////////////////////////////////
void MainFrame::signManuallyZSlider(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString val = m_metricZEdit->GetValue();
	double v;
	val.ToDouble(&v);
	if ( v != 0.0 )
		v *= -1;
	
	if ( m_unit->GetValue() == "steps" ) {
		val = wxString::Format(wxT("%6.0f"), v);
		m_zManuallySlider->SetValue(v / cnc->getCncConfig()->getCalculationFactZ());
	} else {
		val = wxString::Format(wxT("%4.3f"), v);
		m_zManuallySlider->SetValue(v);
	}
	
	m_metricZEdit->SetValue(val);
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateMetricX(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString val = m_metricX->GetValue();
	double v;
	val.ToDouble(&v);
	
	if ( m_unit->GetValue() == "steps" ) {
		m_xManuallySlider->SetValue(v / cnc->getCncConfig()->getCalculationFactX());
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
		m_yManuallySlider->SetValue(v / cnc->getCncConfig()->getCalculationFactY());
	} else {
		m_yManuallySlider->SetValue(v);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateMetricZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString val = m_metricZEdit->GetValue();
	double v;
	val.ToDouble(&v);
	
	if ( m_unit->GetValue() == "steps" ) {
		m_zManuallySlider->SetValue(v / cnc->getCncConfig()->getCalculationFactZ());
	} else {
		m_zManuallySlider->SetValue(v);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::clearProcessedSetterList(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_dvListCtrlProcessedSetters->DeleteAllItems();
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
			inboundFileParser->selectSourceControl(m_stcFileContent, val.GetInteger() - 1);
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
void MainFrame::manuallyDissolveEndSwitchDlg() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	cnc->evaluateLimitState();
	EndSwitchDialog esd(this, cnc);
	esd.ShowModal();
}
///////////////////////////////////////////////////////////////////
void MainFrame::dissolveEndSwitchStates(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	manuallyDissolveEndSwitchDlg();
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
void MainFrame::openSvgPreview(const wxString& fn, TemplateFormat format) {
///////////////////////////////////////////////////////////////////
	wxString url;
	wxString tmpPreview("Default html page");
	
	if ( fn.IsEmpty() ) {
		url.assign(getBlankHtmlPage());
	} else {
		startAnimationControl();
		
		bool errorInfo = m_previewErrorInfo->GetValue();
		
		if ( format == TplSvg ) {
			SVGFileParser fp(fn, cnc);
			tmpPreview = CncFileNameService::getCncTemplatePreviewFileName(TplSvg);
			fp.createPreview(tmpPreview, errorInfo);
			
		} else if ( format == TplGcode) {
			GCodeFileParser fp(fn, cnc);
			tmpPreview = CncFileNameService::getCncTemplatePreviewFileName(TplGcode);
			fp.createPreview(tmpPreview, errorInfo);
			
		} else {
			// avoid explorer extention handling
			tmpPreview = CncFileNameService::getCncTemplatePreviewFileName(format);
			wxCopyFile(fn, tmpPreview);
		}
		
		url.assign(tmpPreview);
	}
	
	m_svgPreviewFileName1->SetValue(fn);
	m_svgPreviewFileName2->SetValue(tmpPreview);
	m_svgPreview->LoadURL(url);
	m_svgPreview->Update();
	stopAnimationControl();
}
///////////////////////////////////////////////////////////////////
void MainFrame::updateSvgPreview(const wxString& fn) {
///////////////////////////////////////////////////////////////////
	m_mainNotebook->SetSelection(MainTemplatePage);
	
	if ( m_templateNotebook->FindPage(m_panelTplPreview) != wxNOT_FOUND  ) {
		m_templateNotebook->SetSelection(TemplatePreviewPage);
		
		highlightTplPreview(true);
		m_panelTplPreview->Refresh();

		openSvgPreview(fn, getCurrentTemplateFormat(fn));
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::openFileFromFileManager(const wxString& f) {
///////////////////////////////////////////////////////////////////
	if ( checkIfTemplateIsModified() == false ) {
		prepareTplPreview();
		highlightTplPreview(false);
		return;
	}

	wxFileName fn(f);
	m_inputFileName->SetValue(fn.GetFullName());
	m_inputFileName->SetHint(fn.GetFullPath());

	openFile();
}
///////////////////////////////////////////////////////////////////
void MainFrame::dirCtrlChanged(wxTreeEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( ignoreDirControlEvents == true )
		return;

	//event.Skip(true);
	
	wxString sel = m_dirCtrl->GetFilePath();
	if ( sel.length() <= 0 ) 
		return;
		
	// update preview
	updateSvgPreview(m_dirCtrl->GetFilePath());
	
}
///////////////////////////////////////////////////////////////////
void MainFrame::dirCtrlActivated(wxTreeEvent& event) {
///////////////////////////////////////////////////////////////////
	//event.Skip(true);
	
	wxString sel = m_dirCtrl->GetFilePath();
	if ( sel.length() <= 0 ) 
		return;
	
	openFileFromFileManager(m_dirCtrl->GetFilePath());
	highlightTplPreview(false);
}
///////////////////////////////////////////////////////////////////
void MainFrame::leaveLruList(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	m_lruList->DeselectAll();
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectCurrentFile(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_dirCtrl->SetPath(getCurrentTemplatePathFileName());
	m_dirCtrl->ExpandPath(getCurrentTemplatePathFileName());
}
///////////////////////////////////////////////////////////////////
void MainFrame::selectDefaultDirectory(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString cfgStr;
	config->Read("DefaultTemplate/DefaultDir", &cfgStr, wxT(""));
	
	if ( cfgStr != "" ) {
		m_dirCtrl->SetPath(cfgStr);
		m_dirCtrl->ExpandPath(cfgStr);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::lruListItemActivated(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_lruList->GetStringSelection().length() > 0 ) {
		int sel = m_lruList->GetSelection();
		openFileFromFileManager(wxString(lruFileList.getFileName(sel)));

		//Not necessary will be already released by openFileFromFileManager
		//prepareTplPreview();
		//highlightSvgPreview(false);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::lruListItemSelected(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_lruList->GetStringSelection().length() > 0 ) {
		int sel = m_lruList->GetSelection();
		updateSvgPreview(wxString(lruFileList.getFileName(sel)));
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::leaveEnterFileManagerControl(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	prepareTplPreview(m_svgFilePreviewHint->IsShown());
	highlightTplPreview(false);
}
///////////////////////////////////////////////////////////////////
void MainFrame::highlightTplPreview(bool state) {
///////////////////////////////////////////////////////////////////
	wxSizer* fgs = m_panelTplPreview->GetSizer();
	if ( fgs == NULL )
		return;
	
	if ( state == true ) {
		m_panelTplPreview->SetBackgroundColour(wxColor(153,180,209));
		m_svgFilePreviewHint->SetBackgroundColour(wxColor(153,180,209));
		fgs->Show(m_svgFilePreviewHint);
	} else {
		m_panelTplPreview->SetBackgroundColour(wxNullColour);
		fgs->Hide(m_svgFilePreviewHint);
	}
	
	fgs->RecalcSizes();
	fgs->Layout();
	
	m_svgPreview->Refresh();
	m_panelTplPreview->Refresh();
}

///////////////////////////////////////////////////////////////////
void MainFrame::toggleAuiPane(wxWindow* pane, wxMenuItem* menu) {
///////////////////////////////////////////////////////////////////
	if ( pane == NULL )
		return;
	
	if ( GetAuimgrMain()->GetPane(pane).IsShown() )  {
		hideAuiPane(pane, menu);
			
	} else {
		showAuiPane(pane, menu);
	}
	GetAuimgrMain()->Update();
}
///////////////////////////////////////////////////////////////////
void MainFrame::showAuiPane(wxWindow* pane, wxMenuItem* menu) {
///////////////////////////////////////////////////////////////////
	if ( pane == NULL )
		return;
		
	GetAuimgrMain()->GetPane(pane).Show();
	
	if ( pane == getAUIPaneByName("Spy") ) {
		enableSerialSpy(true);
		GetAuimgrMain()->GetPane(pane).Floatable(true);
		GetAuimgrMain()->GetPane(pane).Dockable(false);
		GetAuimgrMain()->GetPane(pane).Float();
		GetAuimgrMain()->GetPane(pane).FloatingSize(600,500);
		
		GetAuimgrMain()->Update();
	}
	
	if ( menu != NULL )
		menu->Check(true);
		
	GetAuimgrMain()->Update();
}
///////////////////////////////////////////////////////////////////
void MainFrame::hideAuiPane(wxWindow* pane, wxMenuItem* menu) {
///////////////////////////////////////////////////////////////////
	if ( pane == NULL )
		return;

	GetAuimgrMain()->GetPane(pane).Hide();
	GetAuimgrMain()->GetPane(pane).DestroyOnClose(false);
	pane->Close(true);
	
	if ( pane == getAUIPaneByName("Spy") )
		enableSerialSpy(false);
	
	if ( menu != NULL )
		menu->Check(false);
		
	GetAuimgrMain()->Update();
}
///////////////////////////////////////////////////////////////////
wxWindow* MainFrame::getAUIPaneByName(const wxString& name) {
///////////////////////////////////////////////////////////////////
	if      ( name == "ToolBar" ) 			return m_auibarMain;
	else if ( name == "MainView")			return m_scrollWinMain;
	else if ( name == "Logger")				return m_scrollWinLogger;
	else if ( name == "Outbound")			return m_scrollOutbound;
	else if ( name == "TemplateManager")	return m_scrollWinFile;
	else if ( name == "StatusBar")			return m_statusBar;
	else if ( name == "Spy")				return m_scrollSpy;
	else if ( name == "Speed")				return m_panelSpeed;
	else if ( name == "UnitCalculator")		return m_svgUnitCalulator;

	return NULL;
}
///////////////////////////////////////////////////////////////////
wxMenuItem* MainFrame::getAUIMenuByName(const wxString& name) {
///////////////////////////////////////////////////////////////////
	if      ( name == "ToolBar" ) 			return m_miToolbar;
	else if ( name == "StatusBar")			return m_miViewStatusbar;
	else if ( name == "MainView")			return m_miViewMainView;
	else if ( name == "Logger")				return m_miViewLogger;
	else if ( name == "Outbound")			return m_miViewMonitor;
	else if ( name == "TemplateManager")	return m_miViewTemplateManager;
	else if ( name == "Spy")				return m_miViewSpy;
	else if ( name == "Speed")				return m_miViewSpeed;
	else if ( name == "UnitCalculator")		return m_miViewUnitCalculator;

	return NULL;
}
///////////////////////////////////////////////////////////////////
void MainFrame::toggleAuiPane(const wxString& name) {
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
	
	toggleAuiPane(w, m);
}
///////////////////////////////////////////////////////////////////
void MainFrame::showAuiPane(const wxString& name) {
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
	
	showAuiPane(w, m);
}
///////////////////////////////////////////////////////////////////
void MainFrame::hideAuiPane(const wxString& name) {
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
	
	hideAuiPane(w, m);
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
	toggleAuiPane("MainView");
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
void MainFrame::viewSpy(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	toggleAuiPane("Spy");
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewSpeed(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	toggleAuiPane("Speed");
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewUnitCalculator(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	toggleAuiPane("UnitCalculator");
}
///////////////////////////////////////////////////////////////////
void MainFrame::perspectiveDefault(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	viewAllAuiPanes();
	//GetAuimgrMain()->LoadPerspective(_defaultPerspective, true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::perspectiveRun(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	hideAllAuiPanes();
	showAuiPane("ToolBar");
	showAuiPane("Outbound");
	showAuiPane("Logger");
	showAuiPane("StatusBar");
}
///////////////////////////////////////////////////////////////////
void MainFrame::perspectiveTemplate(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	hideAllAuiPanes();
	showAuiPane("ToolBar");
	showAuiPane("MainView");
	showAuiPane("Logger");
	showAuiPane("StatusBar");
}
///////////////////////////////////////////////////////////////////
void MainFrame::closeAuiPane(wxAuiManagerEvent& evt) {
///////////////////////////////////////////////////////////////////
	if ( evt.pane->window == m_scrollWinFile ) {
		m_miViewTemplateManager->Check(!m_scrollWinFile->IsShown());
		
	} else if ( evt.pane->window == m_scrollWinLogger ) {
		m_miViewLogger->Check(!m_scrollWinLogger->IsShown());
		
	} else if ( evt.pane->window == m_scrollOutbound ) {
		m_miViewMonitor->Check(!m_scrollOutbound->IsShown());
		
	} else if ( evt.pane->window == m_scrollWinMain ) {
		m_miViewMainView->Check(!m_scrollWinMain->IsShown());
		
	} else if ( evt.pane->window == m_scrollSpy ) {
		m_miViewSpy->Check(!m_scrollSpy->IsShown());
		enableSerialSpy(false);
		
	} else if ( evt.pane->window == m_panelSpeed ) {
		m_miViewSpeed->Check(!m_panelSpeed->IsShown());
		
	} else if ( evt.pane->window == m_statusBar ) {
		m_miViewStatusbar->Check(!m_statusBar->IsShown());
		
	} else if ( evt.pane->window == m_panelSpeed ) {
		m_miViewSpeed->Check(!m_panelSpeed->IsShown());
		
	}
}
/////////////////////////////////////////////////////////////////////
void MainFrame::OnPerspectiveTimer(wxTimerEvent& WXUNUSED(event)) {
/////////////////////////////////////////////////////////////////////
	if ( perspectiveTimer.IsRunning() == true )
		perspectiveTimer.Stop();
		
	if ( m_miToolbar->IsChecked() )
		showAuiPane("ToolBar");
		
	if ( m_miViewStatusbar->IsChecked() )
		showAuiPane("StatusBar");
		
	//GetAuimgrMain()->Update();
}
///////////////////////////////////////////////////////////////////
void MainFrame::maximizeAuiPane(wxAuiManagerEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( event.pane->window == m_scrollOutbound || event.pane->window == m_scrollWinMain )
		perspectiveTimer.Start(20);
		
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::restoreAuiPane(wxAuiManagerEvent& event) {
///////////////////////////////////////////////////////////////////
	// do nothing
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
	hideAuiPane(m_auibarMain,      m_miToolbar);
	hideAuiPane(m_statusBar,       m_miViewStatusbar);
	hideAuiPane(m_scrollWinFile,   m_miViewTemplateManager);
	hideAuiPane(m_scrollWinLogger, m_miViewLogger);
	hideAuiPane(m_scrollOutbound,  m_miViewMonitor);
	hideAuiPane(m_scrollWinMain,   m_miViewMainView);
	hideAuiPane(m_scrollSpy,       m_miViewSpy);
	hideAuiPane(m_panelSpeed,      m_miViewSpeed);
	hideAuiPane(m_svgUnitCalulator,m_miViewUnitCalculator);
}
///////////////////////////////////////////////////////////////////
void MainFrame::viewAllAuiPanes(bool withSpy) {
///////////////////////////////////////////////////////////////////
	showAuiPane(m_auibarMain,      m_miToolbar);
	showAuiPane(m_statusBar,       m_miViewStatusbar);
	showAuiPane(m_scrollWinFile,   m_miViewTemplateManager);
	showAuiPane(m_scrollWinLogger, m_miViewLogger);
	showAuiPane(m_scrollOutbound,  m_miViewMonitor);
	showAuiPane(m_scrollWinMain,   m_miViewMainView);
	showAuiPane(m_panelSpeed,      m_miViewSpeed);
	showAuiPane(m_svgUnitCalulator,m_miViewUnitCalculator);
	
	if ( withSpy )
		showAuiPane(m_scrollSpy,       m_miViewSpy);
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
			case OutboundSvgPage: 	m_svgEmuToggleOrigPath->Enable( m_cbSvgIncludeOriginalPath->GetStringSelection().Upper() == "YES" );
									break;
									
			case Outbound3DPage:	if ( cnc )
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
			case OutboundSvgSource: if ( m_stcEmuSource->IsModified() ) {
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
			
		pngAnimation->Start(10);
		pngAnimation->Update();
	}
	
	wxColor color(255,128,128);
	m_cmdDuration->SetForegroundColour(color);
	m_cmdCount->SetForegroundColour(color);
	m_cmdCount->GetParent()->Update();
	processStartTime = wxDateTime::UNow();
	buildCommandDurationToolTip();
}
///////////////////////////////////////////////////////////////////
void MainFrame::stopAnimationControl() {
///////////////////////////////////////////////////////////////////
	m_cmdDuration->SetForegroundColour(*wxWHITE);
	m_cmdCount->SetForegroundColour(*wxWHITE);
	
	if ( pngAnimation != NULL ) {
		if ( pngAnimation->IsRunning() ) {
			pngAnimation->Stop();
			pngAnimation->Update();
			processEndTime = wxDateTime::UNow();
			buildCommandDurationToolTip();
		}
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::buildCommandDurationToolTip() {
///////////////////////////////////////////////////////////////////
	wxString tt;
	tt << "Processing timestamps  . . .\n\n";
	
	tt << "Started: ";
	if ( processStartTime.IsValid() ) {
		tt << processStartTime.Format("%H:%M:%S.%l", wxDateTime::CET);
	}
	tt << "]\n";
	
	tt << "Finished: ";
	if ( processEndTime.IsValid() ) {
		tt << processEndTime.Format("%H:%M:%S.%l",   wxDateTime::CET);
	}
	tt << "\n\n";

	tt << "Elapsed: ";
	if ( processEndTime.IsValid() &&  processStartTime.IsValid()) {
		wxTimeSpan diff = processEndTime - processStartTime;
		tt << diff.Format("%H:%M:%S.%l");
	}
	tt << "\n";
	
	m_cmdDuration->SetToolTip(tt);
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
void MainFrame::decorateTemplateListBook() {
///////////////////////////////////////////////////////////////////
	m_templateToolbook->SetSelection(0);
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
	cnc->waitActive(1500);

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
	if ( event.GetMargin() == MARGIN_FOLD ) {
		int lineClick = m_stcFileContent->LineFromPosition(event.GetPosition());
		int levelClick = m_stcFileContent->GetFoldLevel(lineClick);
		
		if ( (levelClick & wxSTC_FOLDLEVELHEADERFLAG ) > 0) {
			m_stcFileContent->ToggleFold(lineClick);
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
void MainFrame::togglePreviewErrorInfo(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_previewErrorInfo->GetValue() == true )
		m_previewErrorInfo->SetToolTip("Preview with error information");
	else 
		m_previewErrorInfo->SetToolTip("Preview without error information");
		
	prepareTplPreview(true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::decorateRunButton() {
///////////////////////////////////////////////////////////////////
	wxBitmap bmp;
	wxString toolTip;
	
	toolTip = isPause() ? "Resume " : "Run";
	
	if ( svgDebugger == true ) {
		toolTip += "(Debug)";
		bmp = ImageLib24().Bitmap("BMP_RUN_DEBUG");
	} else {
		toolTip += "(Release)";
		bmp = ImageLib24().Bitmap("BMP_RUN_RELEASE");
	}
	
	m_rcRun->SetToolTip(toolTip);
	m_rcRun->SetBitmapDisabled(bmp);
	m_rcRun->SetBitmap(bmp);
	m_rcRun->Update();
}
///////////////////////////////////////////////////////////////////
void MainFrame::determineRunMode() {
///////////////////////////////////////////////////////////////////
	svgDebugger = m_miRcDebug->IsChecked();
	
	if ( svgDebugger == true ) {
		m_svgParseMode->SetLabel("Debug");
		m_svgParseMode->SetForegroundColour(wxColor(255,128,128));
	} else {
		m_svgParseMode->SetLabel("Release");
		m_svgParseMode->SetForegroundColour(wxColor(128,128,255));
	}

	decorateRunButton();
	
	m_miRcPreprocessing->Enable(svgDebugger == true);
	m_miRcUserAgent->Enable(svgDebugger == true);
	m_miRcSpooling->Enable(svgDebugger == true);
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcSelectRunMode(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	determineRunMode();
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcRun(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( isPause() == false ) {
		if ( m_miRcDebug->IsChecked() == true ) 
			m_templateNotebook->SetSelection(TemplateDebuggerPage);

		processTemplate();
	} else {
		rcPause(event);
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcPause(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
 	if ( inboundFileParser != NULL ) {
		inboundFileParser->pause();
		enableRunControls(inboundFileParser->isPause());
	}
	
	decorateRunButton();
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcNextPath(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( inboundFileParser != NULL )
		inboundFileParser->debugNextPath();
		
	m_svgDebuggerKey->Clear();
	m_svgDebuggerValue->Clear();
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcNextStep(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( inboundFileParser != NULL )
		inboundFileParser->debugNextStep();
		
	clearDebugControls();
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcFinish(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( inboundFileParser != NULL )
		inboundFileParser->debugFinish();
		
	m_svgDebuggerKey->Clear();
	m_svgDebuggerValue->Clear();
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcStop(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cnc);
	
	if ( getCurrentTemplateFormat() == TplTest ) {
		bool ret = cnc->getSerial()->sendTestSuiteEndFlag();
		if ( ret == true ) 	cnc::trc.logInfo("Test was stopped");
		else				cnc::trc.logError("Test stop was failed");
	} else {
		if ( inboundFileParser != NULL )
			inboundFileParser->debugStop();
		 
		clearDebugControls();
		cnc::trc.logInfo("Debug session was stopped");
	}
}
///////////////////////////////////////////////////////////////////
void MainFrame::rcReset(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	requestReset();
	setRefPostionState(false);
}
///////////////////////////////////////////////////////////////////
void MainFrame::controlerPause(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	//todo curently not tested
	if ( cnc->getSerial()->isPauseActive() )
		cnc->processCommand('p', std::clog);
	else
		cnc->processCommand('P', std::clog);
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
		disableAllRunControls();
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
		disableAllRunControls();
		m_testToggleEndSwitch->Enable(true);
		
		startAnimationControl();
		while ( m_testToggleEndSwitch->GetValue() == true ) {
			dispatch();
			
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
	m_outboundNotebook->SetSelection(OutboundCNCValuesPage);
	m_notebookConfig->SetSelection(CNCControllerPinsPage);
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
				m_stcFileContent->MarginSetStyle(curline, CL_LINE_MODIFIED_STYLE);
			} else {
				for(int i = 0; i <= numlines; i++) {
					m_stcFileContent->MarginSetText(curline + i, wxT(" "));
					m_stcFileContent->MarginSetStyle(curline + i, CL_LINE_MODIFIED_STYLE);
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
	m_stepDelay->SetValue(125);
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
	wxASSERT( cnc && cnc->getCncConfig() );
	PathGeneratorStore::RegenerateParameter rp;
	rp.in.editControl  = ctl;
	rp.in.toolDiameter = cnc->getCncConfig()->getRouterBitDiameter();
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
	wxASSERT( cnc && cnc->getCncConfig() );
	PathGeneratorStore::RegenerateParameter rp;
	rp.in.editControl  = ctl;
	rp.in.toolDiameter = cnc->getCncConfig()->getRouterBitDiameter();
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
void MainFrame::openPrevFile1(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString fn(m_svgPreviewFileName1->GetValue());
	openFileExtern(wxT("notepad"), fn);
}
///////////////////////////////////////////////////////////////////
void MainFrame::openPrevFile2(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString fn(m_svgPreviewFileName2->GetValue());
	openFileExtern(wxT("notepad"), fn);
}

