#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "wxCrafterMainFrame.h"
#include <wx/wx.h>
#include <wx/notifmsg.h>
#include <wx/generic/notifmsg.h>
#include <wx/event.h>
#include <wx/infobar.h>
#include "GamepadEvent.h"
#include "SerialEvent.h"
#include "NotebookInfo.h"
#include "FileParser.h"
#include "BinaryFileParser.h"
#include "CncControl.h"
#include "CncPerspective.h"
#include "CncMotionMonitor.h"
#include "CncFileView.h"
#include "CncToolMagazine.h"
#include "CncPosSpyListCtrl.h"
#include "CncSetterListCtrl.h"
#include "CncSerialSpyListCtrl.h"
#include "CncGamepadControllerState.h"
#include "CncMotionMonitorVertexTrace.h"
#include "CncParsingSynopsisTrace.h"
#include "CncSummaryListCtrl.h"
#include "Codelite/wxPNGAnimation.h"
#include "CncNavigatorPanel.h"
#include "CncMessageDialog.h"

////////////////////////////////////////////////////////////////////
// forward declarations
class MainFrameProxy;
class wxMenu;
class wxMenuItem;
class wxFileConfig;

class GL3DOptionPane;
class GL3DDrawPane;

class CncTransactionLock;

class GamepadThread;

class SerialThread;
class SerialThreadStub;
class CncLoggerProxy;

class CncSourceEditor;
class CncOutboundEditor;
class CncFilePreviewWnd;
class CncFilePreview;
class CncReferencePosition;
class CncMonitorVSplitterWindow;
class CncMonitorHSplitterWindow;
class CncTemplateObserver;
class CncStatisticsPane;
class CncSvgViewer;
class CncGameportController;
class CncSpeedMonitor;
class CncPreprocessor;
class CncGCodeSequenceListCtrl;
class CncLruFileViewListCtrl;
class CncMotionVertexTrace;
class CncOpenGLContextObserver;
class CncOSEnvironmentDialog;
class CncExternalViewBox;
class CncArduinoEnvironment;
class CncLCDPositionPanel;
class CncManuallyMoveCoordinates;
class CncSpeedPlayground;
class CncGamepadControllerSpy;
class CncGamepadControllerState;

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// app defined events
	wxDECLARE_EVENT(wxEVT_PERSPECTIVE_TIMER, 					wxTimerEvent);
	wxDECLARE_EVENT(wxEVT_DEBUG_USER_NOTIFICATION_TIMER, 		wxTimerEvent);
	wxDECLARE_EVENT(wxEVT_TRACE_FROM_THREAD,	 				wxThreadEvent);
	wxDECLARE_EVENT(wxEVT_DISPATCH_ALL,							wxThreadEvent);
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// global definitions
namespace CncApp {
	
	struct WindowInfo {
		bool 		lastEnableState = true;
		wxWindow* 	ctrl			= NULL;
	};
	typedef std::vector<WindowInfo> GuiControls;
	
	struct MenuInfo {
		bool 		lastEnableState = true;
	};
	
	typedef std::map<wxMenuItem*, MenuInfo> MenuItems;
	
	
	class LocalContext {
		bool* locked;
		
		public:
			LocalContext(bool* value)
			: locked(value)
			{ 
				*locked = true; 
			}
			
			~LocalContext()
			{ 
				*locked = false; 
			}
	};
	
};


class MainFrame;

class GlobalConfigManager {
	
	public:
		GlobalConfigManager(MainFrame* mf, wxPropertyGridManager* pgMgrSetup, wxFileConfig* globalConfig);
		~GlobalConfigManager();
};

///////////////////////////////////////////////////////////////////
// This class is established because the containing controls have to be 
// created as most as possible first. 
// If we do so the creation is after the ctor of MainFrameBClass 
// and before the ctor of MainFrame
class MainFrameBase : public MainFrameBClass {
	
	private:
		CncLoggerProxy* logger;
		CncTextCtrl* 	startupTrace;
		CncTextCtrl* 	tmpTraceInfo;
		CncTextCtrl* 	controllerMsgHistory;
		
		virtual void traceTextUpdated(wxCommandEvent& event) { event.Skip(); }
		
	public:
		explicit MainFrameBase(wxWindow* parent);
		virtual ~MainFrameBase();
		
		// global trace controls
		CncLoggerProxy* getLogger() 			{ return logger; }
		CncTextCtrl* 	getStartupTrace() 		{ return startupTrace; }
		CncTextCtrl* 	getTrace() 				{ return tmpTraceInfo; }
		CncTextCtrl* 	getCtrlMessageHistory() { return controllerMsgHistory; }
		
};

////////////////////////////////////////////////////////////////////
class MainFrame : public MainFrameBase, public GlobalConfigManager {

	// User commands
	protected:
		virtual void requestResolveLimitStates(wxCommandEvent& event);
		virtual void requestToolTest(wxCommandEvent& event);
		virtual void onEvaluateHardwareXYPlane(wxCommandEvent& event);
		virtual void onEvaluateHardwareZAxis(wxCommandEvent& event);
		virtual void onTakeoverHardwareDimensions(wxCommandEvent& event);
		virtual void onResetHardwareReference(wxCommandEvent& event);
		virtual void onEvaluateHardwareReference(wxCommandEvent& event);
		virtual void openSpeedPlayground(wxCommandEvent& event);
		virtual void onChangePreviewMode(wxCommandEvent& event);
		virtual void connectSec(wxCommandEvent& event);
		virtual void selectPortSec(wxCommandEvent& event);
		virtual void onCloseSecureRunAuiPane(wxCommandEvent& event);
		virtual void onOpenGLContextObserver(wxCommandEvent& event);
		virtual void onShowLoggerOnDemand(wxCommandEvent& event);
		virtual void freezeLogger(wxCommandEvent& event);
		virtual void changeMonitorListBook(wxListbookEvent& event);
		virtual void showStacktraceStore(wxCommandEvent& event);
		virtual void onSelectSpyInboundDetails(wxDataViewEvent& event);
		virtual void onSelectSpyOutboundDetails(wxDataViewEvent& event);
		virtual void onSelectSpyUnknownDetails(wxDataViewEvent& event);
		virtual void openSpyDetailWindow(wxCommandEvent& event);
		virtual void dclickHeartbeatState(wxMouseEvent& event);
		virtual void showOSEnvironment(wxCommandEvent& event);
		virtual void toggleMotionMonitorReplayPane(wxCommandEvent& event);
		virtual void selectMetricUnitFrom(wxCommandEvent& event);
		virtual void selectMetricUnitFromValue(wxCommandEvent& event);
		virtual void selectMetricUnitTo(wxCommandEvent& event);
		virtual void toggleMotionMonitorOptionPane(wxCommandEvent& event);
		virtual void toggleMotionMonitorStatisticPane(wxCommandEvent& event);
		virtual void motionMonitorBoundBox(wxCommandEvent& event);
		virtual void motionMonitorHelpLines(wxCommandEvent& event);
		virtual void motionMonitorOrigin(wxCommandEvent& event);
		virtual void motionMonitorRuler(wxCommandEvent& event);
		virtual void toggleOutboundEditorWordWrap(wxCommandEvent& event);
		virtual void extractSourceAsNewTpl(wxCommandEvent& event);
		virtual void saveOutboundAsNewTplFromButton(wxCommandEvent& event);
		virtual void saveOutboundAsNewTplFromMenu(wxCommandEvent& event);
		virtual void selectBinaryEditorViewMode(wxCommandEvent& event);
		virtual void traceSessionId(wxCommandEvent& event);
		virtual void openSessionDialog(wxCommandEvent& event);
		virtual void motionMonitorPostionMarker(wxCommandEvent& event);
		virtual void motionMonitorFlyPath(wxCommandEvent& event);
		virtual void updateRenderResolution(wxCommandEvent& event);
		virtual void toggleIdleRequests(wxCommandEvent& event);
		virtual void cncMainViewChanged(wxNotebookEvent& event);
		virtual void rcSecureDlg(wxCommandEvent& event);
		virtual void leaveSerialSpy(wxMouseEvent& event);
		virtual void changeConfigToolbook(wxToolbookEvent& event);
		virtual void warmStartController(wxCommandEvent& event);
		virtual void setReferencePosition(wxCommandEvent& event);
		virtual void testCaseBookChanged(wxListbookEvent& event);
		virtual void requestInterrupt(wxCommandEvent& event);
		virtual void requestHeartbeat(wxCommandEvent& event);
		virtual void renameTemplateFromButton(wxCommandEvent& event);
		virtual void removeTemplateFromButton(wxCommandEvent& event);
		virtual void onSelectCncMonitor(wxCommandEvent& event);
		virtual void onSelectInboundPreview(wxCommandEvent& event);
		virtual void onSelectTemplatePreview(wxCommandEvent& event);
		virtual void onSelectTestPage(wxCommandEvent& event);
		virtual void refreshSetterList(wxCommandEvent& event);
		virtual void onPaintSpeedPanel(wxPaintEvent& event);
		virtual void clickProbeMode(wxCommandEvent& event);
		virtual void displayIntervalKeyDown(wxKeyEvent& event);
		virtual void displayIntervalChanged(wxScrollEvent& event);
		virtual void displayIntervalThumbtrack(wxScrollEvent& event);
		virtual void searchPosSpy(wxCommandEvent& event);
		virtual void goPosSypFirstId(wxCommandEvent& event);
		virtual void goPosSypLastId(wxCommandEvent& event);
		virtual void goPosSypNextId(wxCommandEvent& event);
		virtual void goPosSypPrevId(wxCommandEvent& event);
		virtual void selectSerialSpyMode(wxCommandEvent& event);
		virtual void clearSetterList(wxCommandEvent& event);
		virtual void loopRepeatTest(wxCommandEvent& event);
		virtual void selectPositionSpyContent(wxCommandEvent& event);
		virtual void copyPositionSpy(wxCommandEvent& event);
		virtual void togglePositionSpy(wxCommandEvent& event);
		virtual void activateAuiPane(wxAuiManagerEvent& event);
		virtual void buttonAuiPane(wxAuiManagerEvent& event);
		virtual void viewZAxis(wxCommandEvent& event);
		virtual void loadPerspective(wxCommandEvent& event);
		virtual void savePerspective(wxCommandEvent& event);
		virtual void addUserPerspective(wxCommandEvent& event);
		virtual void renameUserPerspective(wxCommandEvent& event);
		virtual void removeUserPerspective(wxCommandEvent& event);
		virtual void viewDebugger(wxCommandEvent& event);
		virtual void viewPosistionMonitor(wxCommandEvent& event);
		virtual void toggleTemplateManager(wxCommandEvent& event);
		virtual void clearPositionSpy(wxCommandEvent& event);
		virtual void onSelectManuallyMove(wxCommandEvent& event);
		virtual void onSelectReferences(wxCommandEvent& event);
		virtual void onSelectSetup(wxCommandEvent& event);
		virtual void onSelectTemplate(wxCommandEvent& event);
		virtual void mainViewSelectorSelected(wxCommandEvent& event);
		virtual void monitorViewSelectorSelected(wxCommandEvent& event);
		virtual void viewStatusbar(wxCommandEvent& event);
		virtual void searchAvailiablePorts(wxCommandEvent& event);
		virtual void unitTestFramework(wxCommandEvent& event);
		virtual void selectUCChangeFrom(wxCommandEvent& event);
		virtual void selectUCUnitFrom(wxCommandEvent& event);
		virtual void selectUCUnitTo(wxCommandEvent& event);
		virtual void closeUnitCalculator(wxCommandEvent& event);
		virtual void viewUnitCalculator(wxCommandEvent& event);
		virtual void markSerialSpy(wxCommandEvent& event);
		virtual void viewSpy(wxCommandEvent& event);
		virtual void paintDrawPaneWindow(wxPaintEvent& event);
		virtual void cfgStepDelayDropDown(wxAuiToolBarEvent& event);
		virtual void cfgStepDelayArduino(wxCommandEvent& event);
		virtual void cfgStepDelayMax(wxCommandEvent& event);
		virtual void cfgStepDelayMin(wxCommandEvent& event);
		virtual void enableSerialSpy(wxCommandEvent& event);
		virtual void clearSerialSpy(wxCommandEvent& event);
		virtual void nootebookConfigChanged(wxListbookEvent& event);
		virtual void cancelRun(wxCommandEvent& event);
		virtual void confirmRun(wxCommandEvent& event);
		virtual void clearControllerMsgHistory(wxCommandEvent& event);
		virtual void requestControllerPinsFromButton(wxCommandEvent& event);
		virtual void requestPins(wxCommandEvent& event);
		virtual void traceTextUpdated(wxCommandEvent& event);
		virtual void openPyCam(wxCommandEvent& event);
		virtual void moveXToMid(wxCommandEvent& event);
		virtual void moveYToMid(wxCommandEvent& event);
		virtual void moveZToMid(wxCommandEvent& event);
		virtual void refreshMotionMonitor(wxCommandEvent& event);
		virtual void clearMotionMonitor(wxCommandEvent& event);
		virtual void show3D(wxCommandEvent& event);
		virtual void showFromBottom3D(wxCommandEvent& event);
		virtual void showFromFront3D(wxCommandEvent& event);
		virtual void showFromLeft3D(wxCommandEvent& event);
		virtual void showFromRear3D(wxCommandEvent& event);
		virtual void showFromRight3D(wxCommandEvent& event);
		virtual void showFromTop3D(wxCommandEvent& event);
		virtual void testSwitchToolOnOff(wxCommandEvent& event);
		virtual void testCountXSpinCtl(wxSpinEvent& event);
		virtual void testCountXUpdated(wxCommandEvent& event);
		virtual void selectTestIntervalMode(wxCommandEvent& event);
		virtual void requestCurrentLimitStateIcon(wxMouseEvent& event);
		virtual void rcDebugConfig(wxCommandEvent& event);
		virtual void rcFinish(wxCommandEvent& event);
		virtual void rcNextBreakpoint(wxCommandEvent& event);
		virtual void rcNextStep(wxCommandEvent& event);
		virtual void rcPause(wxCommandEvent& event);
		virtual void rcReset(wxCommandEvent& event);
		virtual void rcRun(wxCommandEvent& event);
		virtual void rcDebug(wxCommandEvent& event);
		virtual void rcStop(wxCommandEvent& event);
		virtual void switchMonitoring(wxCommandEvent& event);
		virtual void viewToolbar(wxCommandEvent& event);
		virtual void renderAuiPane(wxAuiManagerEvent& event);
		virtual void restoreAuiPane(wxAuiManagerEvent& event);
		virtual void maximizeAuiPane(wxAuiManagerEvent& event);
		virtual void saveTemplateFromButton(wxCommandEvent& event);
		virtual void toggleTemplateWordWrapMode(wxCommandEvent& event);
		virtual void copyLogger(wxCommandEvent& event);
		virtual void closeAuiPane(wxAuiManagerEvent& evt);
		virtual void toogleSvgEditSearchFlag(wxCommandEvent& event);
		virtual void svgEditSelected(wxCommandEvent& event);
		virtual void svgEditSearchTextChanged(wxCommandEvent& event);
		virtual void svgEditFind(wxCommandEvent& event);
		virtual void svgEditFindPrev(wxCommandEvent& event);
		virtual void dclickDurationCount(wxMouseEvent& event);
		virtual void stepDelayThumbtrack(wxScrollEvent& event);
		virtual void stepDelayChanged(wxScrollEvent& event);
		virtual void displayUserAgent(wxCommandEvent& event);
		virtual void updateToolControls(wxCommandEvent& event);
		virtual void onSerialTimer(wxTimerEvent& event);
		virtual void onStartupTimer(wxTimerEvent& event);
		virtual void onTraceTimer(wxTimerEvent& event);
		virtual void moveHome(wxCommandEvent& event);
		virtual void moveToZeroXY(wxCommandEvent& event);
		virtual void moveToZeroXYZ(wxCommandEvent& event);
		virtual void moveToZeroZ(wxCommandEvent& event);
		virtual void moveXToMax(wxCommandEvent& event);
		virtual void moveXToMin(wxCommandEvent& event);
		virtual void moveYToMax(wxCommandEvent& event);
		virtual void moveYToMin(wxCommandEvent& event);
		virtual void moveZToMax(wxCommandEvent& event);
		virtual void moveZToMin(wxCommandEvent& event);
		virtual void moveZToBottom(wxCommandEvent& event);
		virtual void moveZToTop(wxCommandEvent& event);
		virtual void openConfigurationFile(wxCommandEvent& event);
		virtual void openExternalEditor(wxCommandEvent& event);
		virtual void openCalculator(wxCommandEvent& event);
		virtual void selectPort(wxCommandEvent& event);
		virtual void requestVersion(wxCommandEvent& event);
		virtual void requestTimestamp(wxCommandEvent& event);
		virtual void requestCurrentPos(wxCommandEvent& event);
		virtual void requestCurrentLimitState(wxCommandEvent& event);
		virtual void requestConfig(wxCommandEvent& event);
		virtual void requestControllerConfigFromButton(wxCommandEvent& event);
		virtual void requestReset(wxCommandEvent& event);
		virtual void requestEnableStepperMotors(wxCommandEvent& event);
		virtual void emergencyStop(wxCommandEvent& event);
		virtual void defineDebugSerial(wxCommandEvent& event);
		virtual void defineMinMonitoring(wxCommandEvent& event);
		virtual void defineNormalMonitoring(wxCommandEvent& event);
		virtual void saveTemplate(wxCommandEvent& event);
		virtual void saveTemplateAs(wxCommandEvent& event);
		virtual void newTemplate(wxCommandEvent& event);
		virtual void openTemplate(wxCommandEvent& event);
		virtual void reloadTemplate(wxCommandEvent& event);
		virtual void openTemplateSourceExtern(wxCommandEvent& event);
		virtual void openTemplateSvgExtern(wxCommandEvent& event);
		virtual void reloadTemplateFromButton(wxCommandEvent& event);
		virtual void defineUpdateCoordinates(wxCommandEvent& event);
		virtual void defineAllowEvents(wxCommandEvent& event);
		virtual void defineOnlineDrawing(wxCommandEvent& event);
		virtual void selectUnit(wxCommandEvent& event);
		virtual void clearLogger(wxCommandEvent& event);
		virtual void connect(wxCommandEvent& event);
		virtual void selectUAInboundPathList(wxDataViewEvent& event);
		virtual void selectUAUseDirectiveList(wxDataViewEvent& event);
		virtual void selectUADetailInfo(wxDataViewEvent& event);
		virtual void activateMainWindow(wxActivateEvent& event);
		virtual void outboundBookChanged(wxNotebookEvent& event);
		virtual void outboundBookChanging(wxNotebookEvent& event);
		virtual void viewMainView(wxCommandEvent& event);
		virtual void viewTemplateManager(wxCommandEvent& event);
		virtual void viewLogger(wxCommandEvent& event);
		virtual void viewMonitor(wxCommandEvent& event);
		virtual void hideAllAuiPanes(wxCommandEvent& event);
		virtual void viewAllAuiPanes(wxCommandEvent& event);
		virtual void testFunction1(wxCommandEvent& event);
		virtual void testFunction2(wxCommandEvent& event);
		virtual void testFunction3(wxCommandEvent& event);
		virtual void testFunction4(wxCommandEvent& event);
		virtual void disableSlider(wxMouseEvent& event);
		virtual void OnExit(wxCommandEvent& event);
		virtual void OnAbout(wxCommandEvent& event);
		virtual void onClose(wxCloseEvent& event);
		
		virtual void loadConfiguration(wxCommandEvent& event);
		virtual void saveConfiguration(wxCommandEvent& event);
		virtual void setupGridChanged(wxPropertyGridEvent& event);
		virtual void setupGridChanging(wxPropertyGridEvent& event);
		virtual void setupGridCommandButton(wxCommandEvent& event);
		virtual void setupGridSelected(wxPropertyGridEvent& event);
		
		void onGamepadThreadInitialized(GamepadEvent& event);
		void onGamepadThreadCompletion(GamepadEvent& event);
		void onGamepadThreadUpadte(GamepadEvent& event);
		void onGamepadThreadHeartbeat(GamepadEvent& event);
		void onGamepadThreadMessage(GamepadEvent& event);
		void onSerialThreadInitialized(SerialEvent& event);
		void onSerialThreadCompletion(SerialEvent& event);
		void onSerialThreadHeartbeat(SerialEvent& event);
		void onSerialThreadMessage(SerialEvent& event);
		void onSerialThreadData(SerialEvent& event);
		void onSerialThreadPinNotification(SerialEvent& event);
		
		void onNavigatorPanel(CncNavigatorPanelEvent& event);
		
		void onPerspectiveTimer(wxTimerEvent& event);
		void onDebugUserNotificationTimer(wxTimerEvent& event);
		void onConfigurationUpdated(wxCommandEvent& event);
		
		bool isGamepadNotificationActive();
		void activateGamepadNotificationsOnDemand(bool state);
		void activateGamepadNotifications(bool state);
		bool filePreviewListLeave();
		
		wxDECLARE_EVENT_TABLE();
		
	public:
		enum EventId { 	INITIALIZED 				=  1,
						COMPLETED 					=  2,
						HEARTBEAT 					=  3,
						DISPATCH_ALL 				=  5, 
						POST_INFO 					=  6, 
						POST_WARNING 				=  7, 
						POST_ERROR 					=  8,
						GAMEPAD_STATE				=  9,
						GAMEPAD_HEARTBEAT			= 10,
						GAMEPAD_MESSAGE				= 11,
						SERIAL_HEARTBEAT			= 12,
						SERIAL_MESSAGE				= 13,
						SERIAL_DATA					= 14,
						SERIAL_PIN_NOTIFICATION		= 15
					  };
					  
		enum class RunConfirmationInfo {Wait, Confirmed, Canceled};
		
		//////////////////////////////////////////////////////////////////////////////////
		MainFrame(wxWindow* parent, wxFileConfig* globalConfig);
		virtual ~MainFrame();
		
		virtual bool Show(bool show);

		void initialize(void);
		bool secureRun() { return processTemplateIntern(); }
		
		wxListCtrl* getCtrlSerialSpy() 			{ return serialSpyListCtrl; }
		
		CncMotionMonitor* getMotionMonitor() 	{ return motionMonitor; }
		wxMenuItem* GetMiMotorEnableState()		{ return m_miMotorEnableState; }
		
		//////////////////////////////////////////////////////////////////////////////////
		void selectMainBookSourcePanel(int sourcePageToSelect = TemplateBookSelection::VAL::SOURCE_PANEL);
		void selectMainBookPreviewPanel();
		void selectMainBookSetupPanel();
		void selectMainBookReferencePanel();
		void selectMainBookManuelPanel();
		void selectMainBookTestPanel();	
		
		void selectMonitorBookCncPanel();
		void selectMonitorBookTemplatePanel();
		
		//////////////////////////////////////////////////////////////////////////////////
		// setup
		void installCustControls();
		
		//////////////////////////////////////////////////////////////////////////////////
		// svg edit popup callbacks
		void openCurrentTemplateInBrowser();
		
		//////////////////////////////////////////////////////////////////////////////////
		void displayNotification(const char type, wxString title, wxString message, unsigned int timeout = 3);
		
		//////////////////////////////////////////////////////////////////////////////////
		virtual void ShowAuiToolMenu(wxAuiToolBarEvent& event);
		
		/////////////////////////////////////////////////////////////////////////////////
		// configuration callbacks
		void releaseControllerSetupFromConfig();
		void notifyConfigUpdate();
		void changeWorkpieceThickness();
		void changeCrossingThickness();
		
		void updateSpyDetailWindow();
		
		void decorateSwitchToolOnOff(bool state);
		
		//////////////////////////////////////////////////////////////////////////////////
#		ifdef __WXMSW__
			virtual WXLRESULT MSWWindowProc(WXUINT, WXWPARAM, WXLPARAM);
#		else
			// currently no equivalent
#		endif
		
		//////////////////////////////////////////////////////////////////////////////////
		void waitActive(unsigned int milliseconds, bool once = true);
		void dispatchNext();
		void dispatchTimerEvents();
		void dispatchAll();
		
		const char* getCurrentPortName(wxString& ret);
		
		void addSetter(unsigned char pid, const cnc::SetterValueList& v);
		void addAppPosition(unsigned char pid, long id, char speedMode, double cfgSpeedValue, double curSpeedValue, const CncLongPosition& pos);
		void addCtlPosition(unsigned char pid, long id, char speedMode, double cfgSpeedValue, double curSpeedValue, const CncLongPosition& pos);
		
		bool isDisplayParserDetails() { return m_menuItemDisplayParserDetails->IsChecked(); }
		void updateAppPositionControls();
		void updateCtlPositionControls();
		void updateSpeedControls();
		
		void updateSpeedSlider(float value);
		void updateAndSetSpeedSlider(float value);
		void updateCncSpeed(float value, CncSpeedMode mode);
		
	protected:
	
		void tryToSelectClientId(long clientId, ClientIdSelSource::ID tss);
		void tryToSelectClientIds(long firstClientId, long lastClientId, ClientIdSelSource::ID tss);
		void selectSourceControlLineNumber(long ln);
		void selectSourceControlLineNumbers(long firstLine, long lastLine);

		// will be bind to this frame
		void globalKeyDownHook(wxKeyEvent& event);
 
		// Interrupt thread handling
		//UpdateManagerThread* updateManagerThread;
		wxCriticalSection pUpdateManagerThreadCS;
		
		GamepadThread* gamepadThread;
		wxCriticalSection pGamepadThreadCS;
		
		SerialThread* serialThread;
		wxCriticalSection pSerialThreadCS;

		CncSpeedPlayground*	cncSpeedPlayground;
		
		CncControl* getCncControl() 						{ return cnc; }
		CncPreprocessor* getCncPreProcessor()				{ return cncPreprocessor; }
		
		CncGCodeSequenceListCtrl* getGCodeSequenceList() 	{ return gCodeSequenceList; }
		CncMotionVertexTrace* getMotionVertexTrace() 		{ return motionVertexCtrl; } 
		CncParsingSynopsisTrace* getParsingSynopsisTrace()	{ return parsingSynopisis; }
		
		bool startInteractiveMove();
		bool updateInteractiveMove(const CncLinearDirection x, const CncLinearDirection y, const CncLinearDirection z);
		bool stopInteractiveMove();
		
		bool connectSerialPort();
		
		void decorateProbeMode(bool probeMode);
		void decorateSecureDlgChoice(bool useDlg);
		void decorateOutboundEditor(const char* fileName = NULL);
		
		void decorateGamepadState(bool state);
		
		void decorateIdleState(bool state);
		
		void enableSourceEditorMenuItems(bool enable);
		
		void startAnimationControl();
		void stopAnimationControl();
		
		void enableControls(bool state = true);
		void disableControls() { enableControls(false); }
		
		SerialThread* getSerialThread(SerialThreadStub* sts);
		
		void cncTransactionLockCallback();
		void cncTransactionReleaseCallback();
		
		friend class MainFrameProxy;
		friend class CncLoggerProxy;
		friend class CncPerspective;

		friend class CncBaseEditor;
		friend class CncSourceEditor;
		friend class CncConfig;
		friend class CncContext;
		friend class CncGampadDeactivator;
		friend class CncTransactionLock;

		friend class UpdateManagerThread;
		friend class GamepadThread;
		friend class SerialThread;
		friend class SerialThreadStub;

		// to remove . . .
			friend class CncFileView;
			friend class CncTemplateObserver;
			friend class CncMotionMonitor;
			friend class PathHandlerBase;
			friend class GCodeFileParser;
			friend class CncOpenGLContextObserver;

		
	private:
		// Member variables
		bool 							isDebugMode;
		bool 							isZeroReferenceValid;
		bool 							canClose;
		bool 							evaluatePositions;
		bool 							ignoreDirControlEvents;
		
		RunConfirmationInfo  			runConfirmationInfo;
		CncTransactionLock*				interactiveTransactionLock;
		
		int 							startTimerTimeout;
		int 							serialTimerTimeout;
		int 							traceTimerTimeout;
		int 							traceTimerCounter;
		
		wxString 						lastPortName;
		wxString 						defaultPortName;
		
		wxInfoBar*						mainInfoBar;
		CncControl* 					cnc;
		
		CncLruFileViewListCtrl*			lruFileView;
		CncSourceEditor* 				sourceEditor;
		CncOutboundEditor* 				outboundEditor;
		CncMotionMonitor* 				motionMonitor;
		CncFileView* 					fileView;
		CncFilePreview* 				mainFilePreview;
		CncFilePreview* 				outboundFilePreview;
		CncFilePreview* 				monitorFilePreview;
		CncToolMagazine* 				toolMagazine;
		CncPosSpyListCtrl* 				positionSpy;
		CncSetterListCtrl* 				setterList;
		CncSpeedMonitor*				speedMonitor;
		CncMotionVertexTrace* 			motionVertexCtrl;
		CncPreprocessor*				cncPreprocessor;
		CncParsingSynopsisTrace*		parsingSynopisis;
		CncGCodeSequenceListCtrl*		gCodeSequenceList;
		CncSummaryListCtrl* 			cncSummaryListCtrl;
		CncSerialSpyListCtrl* 			serialSpyListCtrl;
		CncSvgViewer*					outboundEditorSvgView;
		CncNavigatorPanel*				navigatorPanel;
		GL3DOptionPane* 				optionPane3D;
		GL3DDrawPane* 					drawPane3D;
		CncStatisticsPane*				statisticsPane;
		CncMonitorVSplitterWindow* 		cnc3DVSplitterWindow;
		CncMonitorHSplitterWindow* 		cnc3DHSplitterWindow;
		CncTemplateObserver* 			templateObserver;
		CncMessageDialog*				spyDetailWindow;
		CncOpenGLContextObserver*		openGLContextObserver;
		CncOSEnvironmentDialog* 		cncOsEnvDialog;
		CncExternalViewBox* 			cncExtMainPreview;
		CncArduinoEnvironment*			cncArduinoEnvironment;
		CncLCDPositionPanel*			cncLCDPositionPanel;
		CncManuallyMoveCoordinates*		cncManuallyMoveCoordPanel;
		CncGamepadControllerSpy* 		gamepadControllerSpy;
		CncGamepadControllerState*		gamepadStatusCtl; 
		
		CncPerspective perspectiveHandler;
		wxFileConfig* config;
		wxFileConfig* lruStore;
		
		NotebookInfo* outboundNbInfo;
		NotebookInfo* templateNbInfo;
		
		wxString lastMonitorPreviewFileName;
		
		wxPNGAnimation* pngAnimation;
		wxMenu* stcFileContentPopupMenu;
		wxMenu* stcEmuContentPopupMenu;
		FileParser* inboundFileParser;
		
		wxTimer* perspectiveTimer;
		wxTimer* debugUserNotificationTimer;
		
		CncApp::GuiControls	guiControls;
		CncApp::MenuItems	menuItems;
		
		CncReferencePosition* refPositionDlg;
		
		wxSharedPtr<wxNotificationMessageBase> notificationDialog;
		
		void onIndividualCommand(wxCommandEvent& event);

		const wxString& createCncControl(const wxString& sel, wxString& name);
		
		void setIcons();
		
		void traceGccVersion(std::ostream& out);
		void traceWxWidgetsVersion(std::ostream& out);
		void traceWxSvgVersion(std::ostream& out);
		void traceBoostVersion(std::ostream& out);
		void traceWoodworkingCncVersion(std::ostream& out);
		
		
		void displayReport(int id);
		
		///////////////////////////////////////////////////////////////
		// Secure AUI
		void activateSecureMode(bool state = true);
		void deactivateSecureMode() 					{ activateSecureMode(false); }

		///////////////////////////////////////////////////////////////
		// Path Generator
		void displayPGenErrorInfo(const wxString& errorInfo);
		bool verifyPathGenertorNode(wxXmlDocument& xmlDoc, const wxString& node, wxString& errorInfo);
		
		///////////////////////////////////////////////////////////////
		// File handling
		void reloadTemplate(int sourcePageToSelect = TemplateBookSelection::VAL::SOURCE_PANEL);
		void introduceCurrentFile(int sourcePageToSelect = TemplateBookSelection::VAL::SOURCE_PANEL);
		
		void openPreview(CncFilePreview* ctrl, const wxString& fn);
		void openMainPreview(const wxString& fn);
		void openMonitorPreview(const wxString& fn);
		void openFileFromFileManager(const wxString& fn);
		
		bool openFileExtern(const wxString& tool, const char* file);
		bool openFileExtern(const wxString& tool, wxString& file);
		
		TemplateFormat getTemplateFormat(const wxString& fn);
		const char* getTemplateFormatName(const wxString& fn);

		TemplateFormat getCurrentTemplateFormat();
		const char* getCurrentTemplateFormatName();
		const wxString& getCurrentTemplateFileName();
		const wxString& getCurrentTemplatePathFileName();
		
		void updateHardwareReference();
		void updateHardwareDimensions();
		
		bool saveTemplateOnDemand();

		BinaryFileParser::ViewType getCurrentBinaryViewMode();
		
		bool openFile(int pageToSelect = -1);
		bool saveFile();
		bool saveFileAs();
		
		///////////////////////////////////////////////////////////////
		// monitoring
		void defineMinMonitoring();
		void defineNormalMonitoring();
		void switchMonitorButton(bool state);
		void updateMonitoring();
		
		///////////////////////////////////////////////////////////////
		// configuration
		void updateStepDelay();
		void updateUnit();
		void updateFileContentPosition(long x, long y);
		void updateRenderResolution();
		
		void decorateSearchButton();
		void decorateOutboundSaveControls(bool state);
		void decoratePosSpyConnectButton(bool state);
		
		void registerGuiControls();
		void initializeConnectionSelector();
		void initializeGamepadThread();
		void initializeSerialThread();
		bool initializeLruMenu();
		bool openInitialTemplateFile();
		
		void createAnimationControl();
		int showReferencePositionDlg(wxString msg);
		
		///////////////////////////////////////////////////////////////
		// search handling
		int getSvgEditSearchFlags();
		void updateSvgSearchControl();
		void ensureSvgEditLineIsVisible(int line);
		void searchConditionsChanged();
		wxString& fixRegexParen(wxString& find);
		
		///////////////////////////////////////////////////////////////
		// run template handling
		void rcRun();
		void rcPause();
		
		bool checkIfRunCanBeProcessed(bool confirm=true);
		bool checkReferencePositionState();
		bool processVirtualTemplate();
		bool processTemplateIntern();
		bool processTemplateWrapper(bool confirm=true);
		bool processBinaryTemplate();
		bool processSVGTemplate();
		bool processGCodeTemplate();
		bool processManualTemplate();
		bool processTestTemplate();
		bool processTestInterval();
		
		void logTimeConsumed();
		
		void determineRunMode();
		
		bool isPause()				{ return inboundFileParser != NULL ? inboundFileParser->isPause()            : false; }
		bool isProcessing() 		{ return inboundFileParser != NULL ? inboundFileParser->isProcessing()       : false; }
		
		void startDebugUserNotification();
		void stopDebugUserNotification();
		
		bool showConfigSummaryAndConfirmRun();
		
		void collectSummary();
		void collectSvgSpecificSummary();
		void collectGCodeSpecificSummary();
		
		///////////////////////////////////////////////////////////////
		// control handling
		void toggleMotionMonitorOptionPane(bool forceHide);
		void toggleMotionMonitorStatisticPane(bool forceHide);
		
		void decoratePortSelector(bool list=false);
		
		void updateSetterList();
		
		void decorateRunButton();
		void enableRunControls(bool state = true);
		void disableRunControls() { enableRunControls(false); }
		
		void enableMenu(wxMenu* m, bool state = true);
		void disableMenu(wxMenu* m) { enableMenu(m, false); }
		
		void enableMenuItem(wxMenuItem* m, bool state = true);
		void disableMenuItem(wxMenuItem* m) { enableMenuItem(m, false); }

		void toggleAuiPane(wxWindow* pane, wxMenuItem* menu, bool update=true);
		void showAuiPane(wxWindow* pane, wxMenuItem* menu, bool update=true);
		void hideAuiPane(wxWindow* pane, wxMenuItem* menu, bool update=true);
		
		void toggleAuiPane(const wxString& name, bool update=true);
		void showAuiPane(const wxString& name, bool update=true);
		void hideAuiPane(const wxString& name, bool update=true);
		
		void hideAllAuiPanes(bool update=true);
		void viewAllAuiPanes(bool withSpy=false);
		
		void clearPositionSpy();
		void clearMotionMonitor();
		void prepareMotionMonitorViewType();

		wxWindow* getAUIPaneByName(const wxString& name);
		wxMenuItem* getAUIMenuByName(const wxString& name);
		
		void decorateViewMenu();
		
		void selectEditorToolBox(bool fileLoaded);
		void fillFileDetails(bool fileLoaded, const char* extFileName = NULL);
		void prepareNewTemplateFile();
		void decorateExtTemplatePages(TemplateFormat tf);
		void prepareAndShowMonitorTemplatePreview(bool force=false);
		
		void registerMenuItem(wxMenuItem* item);
		void unregisterMenuItem(wxMenuItem* item);
		void regiterAllMenuItems();
		void registerGuiControl(wxWindow* ctl); 
		void disableGuiControls();
		void enableGuiControls(bool state = true);
		void enableMenuItems(bool state = true);
		
		void enableSerialSpy(bool state = true);
		void disableSerialSpy() { enableSerialSpy(false); }
		void decorateSerialSpy();
		void selectSerialSpyMode();
		
		void clearSerialSpy();
		
		void activate3DPerspectiveButton(wxButton* bt);

		///////////////////////////////////////////////////////////////
		// manually control
		void enableTestControls(bool state);
		void processTestMove(wxStaticText* axis, wxStaticText* counter, 
		                     int c, double xd, double yd, double zd);
		
		///////////////////////////////////////////////////////////////
		// misc
		void traceSessionId();
		void setControllerZero(bool x, bool y, bool z);
		void requestReset();
		
		void navigateX(CncDirection d);
		void navigateY(CncDirection d);
		void navigateZ(CncDirection d);
		
		void resetMinMaxPositions();
		void setRefPostionState(bool state);
		bool connectSerialPortDialog();
		
};

////////////////////////////////////////////////////////////////////
class CncTransactionLock {
	
	private:
		static unsigned int referenceCounter;
		
		MainFrame* parent;
		bool prevCheckState;
		bool prevEnableState;
		bool errorMode;
		
	public:
		explicit CncTransactionLock(MainFrame* p) 
		: parent(p)
		, prevCheckState(false)
		, prevEnableState(false)
		, errorMode(false)
		{
			wxASSERT(parent);
			referenceCounter++;
			
			prevCheckState  = parent->m_miRqtIdleMessages->IsChecked();
			prevEnableState = parent->m_miRqtIdleMessages->IsEnabled();
			parent->m_miRqtIdleMessages->Check(false);
			parent->m_miRqtIdleMessages->Enable(false);
			
			parent->cncTransactionLockCallback();
			parent->decorateIdleState(false);
		}
		
		~CncTransactionLock() {
			parent->m_miRqtIdleMessages->Check(errorMode == true ? false : prevCheckState);
			parent->m_miRqtIdleMessages->Enable(prevEnableState);
			parent->cncTransactionReleaseCallback();
			
			wxASSERT( referenceCounter != 0 );
			referenceCounter--;
		}
		
		void 					setErrorMode() 			{ errorMode = true;	}
		
		static bool 			isLocked() 				{ return referenceCounter != 0; }
		static unsigned int 	getReferenceCount()		{ return referenceCounter; }
};

////////////////////////////////////////////////////////////////////
class CncGampadDeactivator {
	
	private:
		static unsigned int referenceCounter;
		
		MainFrame* parent;
		bool reconstructPrevState;
		bool prevState;
		bool stateDialogShown;
		
	public:
		explicit CncGampadDeactivator(MainFrame* p, bool rps = false)
		: parent(p)
		, reconstructPrevState(rps)
		, prevState(false)
		, stateDialogShown(false)
		{
			wxASSERT(parent);
			referenceCounter++;
			
			prevState = parent->isGamepadNotificationActive();
			parent->activateGamepadNotifications(false);
		}
		
		~CncGampadDeactivator() {
			parent->activateGamepadNotifications(reconstructPrevState == true ? prevState : true);
			
			referenceCounter--;
		}
		
		static bool gamepadNotificationsAllowed() { return referenceCounter == 0; }
};

#endif // MAINFRAME_H
