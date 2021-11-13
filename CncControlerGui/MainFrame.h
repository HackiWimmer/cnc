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
#include "CncToolStateControl.h"
#include "CncPerspective.h"
#include "CncMotionMonitor.h"
#include "CncFileView.h"
#include "CncToolMagazine.h"
#include "CncSerialSpyPanel.h"
#include "CncPosSpyListCtrl.h"
#include "CncSetterListCtrl.h"
#include "CncMoveDefinition.h"
#include "CncMotionMonitorVertexTrace.h"
#include "CncTouchBlockDetector.h"
#include "CncSummaryListCtrl.h"
#include "Codelite/wxPNGAnimation.h"
#include "CncNavigatorPanel.h"
#include "CncMessageDialog.h"
#include "CncExceptions.h"

////////////////////////////////////////////////////////////////////
// forward declarations
class MainFrameProxy;
class wxMenu;
class wxMenuItem;
class wxFileConfig;

class GL3DOptionPane;
class GL3DDrawPane;

class CncMainInfoBar;

class CncTransactionLockBase;
class CncTransactionLock;
class CncGamepadTransactionLock;

class GamepadThread;

class SerialThread;
class SerialThreadStub;
class CncStartupLoggerProxy;
class CncStandardLoggerProxy;
class CncMsgHistoryLoggerProxy;
class CncTraceProxy;
class CncLoggerView;
class CncLoggerListCtrl;

class CncSourceEditor;
class CncOutboundEditor;
class CncFilePreviewWnd;
class CncFilePreview;
class CncReferencePosition;
class CncParsingSynopsisTrace;
class CncMonitorVSplitterWindow;
class CncMonitorHSplitterWindow;
class CncTemplateObserver;
class CncStatisticsPane;
class CncSvgViewer;
class CncSpeedMonitor;
class CncDefaultSpeedSlider;
class CncPreprocessor;
class CncGCodeSequenceListCtrl;
class CncLruFileViewListCtrl;
class CncMotionVertexTrace;
class CncOpenGLContextObserver;
class CncOSEnvironmentDialog;
class CncExternalViewBoxCluster;
class CncExternalViewBox;
class CncArduinoEnvironment;
class CncLCDPositionPanel;
class CncManuallyMoveCoordinates;
class CncSpeedPlayground;
class CncGamepadSpy;
class CncGamepadControllerState;
class CncPositionStorageView;
class CncPodiumManagement;
class CncUsbConnectionObserver;
class CncAnchorPosition;
class CncSecureCtrlPanel;
class CncSecureManuallyMovePanel;
class CncTouchBlockDetector;

struct RefPosResult;

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
		CncStandardLoggerProxy* 	logger;
		CncStartupLoggerProxy* 		startupTrace;
		CncLoggerView*				loggerView;
		CncTraceProxy* 				tmpTraceInfo;
		CncMsgHistoryLoggerProxy*	controllerMsgHistory;
		
		virtual void traceTextUpdated(wxCommandEvent& event) { event.Skip(); }
		
	public:
		explicit MainFrameBase(wxWindow* parent);
		virtual ~MainFrameBase();
		
		// global trace controls
		CncStandardLoggerProxy* 	getLogger() 			{ return logger; }
		CncStartupLoggerProxy* 		getStartupTrace() 		{ return startupTrace; }
		CncLoggerView*				getLoggerView()			{ return loggerView; }
		CncTraceProxy* 				getTrace() 				{ return tmpTraceInfo; }
		CncMsgHistoryLoggerProxy*	getCtrlMessageHistory() { return controllerMsgHistory; }
};

////////////////////////////////////////////////////////////////////
class MainFrame : public MainFrameBase, public GlobalConfigManager {

	// User commands
	protected:
		virtual void rcTryRun(wxCommandEvent& event);
		virtual void onPodiumManagement(wxCommandEvent& event);
		virtual void onResetView(wxCommandEvent& event);
		virtual void onToggleFlyPathes(wxCommandEvent& event);
		virtual void onToggleGuidePathes(wxCommandEvent& event);
		virtual void onToggleHelpLines(wxCommandEvent& event);
		virtual void onToggleMillingCutter(wxCommandEvent& event);
		virtual void onToggleOrigin(wxCommandEvent& event);
		virtual void onTogglePosMarker(wxCommandEvent& event);
		virtual void onClearMonitor(wxCommandEvent& event);
		virtual void onRefreshMonitor(wxCommandEvent& event);
		virtual void onToggleBoundBox(wxCommandEvent& event);
		virtual void onToggleHardwareBox(wxCommandEvent& event);
		virtual void onToggleRuler(wxCommandEvent& event);
		virtual void onExecuteOsk(wxCommandEvent& event);
		virtual void testChangedSpindleSpeed(wxScrollEvent& event);
		virtual void testChangingSpindleSpeed(wxScrollEvent& event);
		virtual void toggleTemplateShowEOL(wxCommandEvent& event);
		virtual void toggleTemplateShowWs(wxCommandEvent& event);
		virtual void traceAllCameraDevices(wxCommandEvent& event);
		virtual void onDClickSpeedSliderValue(wxMouseEvent& event);
		virtual void onToggleSecMainView(wxCommandEvent& event);
		virtual void viewCoordinates(wxCommandEvent& event);
		virtual void viewGamepadSpy(wxCommandEvent& event);
		virtual void onLeftDClickTemplateName(wxMouseEvent& event);
		virtual void onSvgExport(wxCommandEvent& event);
		virtual void onSvgFormatPretty(wxCommandEvent& event);
		virtual void toggleTryToSelectClientIdFromEditor(wxCommandEvent& event);
		virtual void toggleAutoSaveTplOnProcess(wxCommandEvent& event);
		virtual void onSelectTemplatePanel(wxListbookEvent& event);
		virtual void detachControllerMessages(wxCommandEvent& event);
		virtual void detachSetterList(wxCommandEvent& event);
		virtual void detachManuallyTemplate(wxCommandEvent& event);
		virtual void detachReference(wxCommandEvent& event);
		virtual void detachTemplateSource(wxCommandEvent& event);
		virtual void detachTest(wxCommandEvent& event);
		virtual void detachConfiguration(wxCommandEvent& event);
		virtual void clickWorkingParameters(wxCommandEvent& event);
		virtual void show3D(wxCommandEvent& event);
		virtual void showFromBottom3D(wxCommandEvent& event);
		virtual void showFromFront3D(wxCommandEvent& event);
		virtual void showFromLeft3D(wxCommandEvent& event);
		virtual void showFromRear3D(wxCommandEvent& event);
		virtual void showFromRight3D(wxCommandEvent& event);
		virtual void showFromTop3D(wxCommandEvent& event);
		virtual void detachMotionMonitor(wxCommandEvent& event);
		virtual void openPositionStorage(wxCommandEvent& event);
		virtual void openHexEditor(wxCommandEvent& event);
		virtual void onReloadMonitorPreview(wxCommandEvent& event);
		virtual void viewControllerMsgHistory(wxCommandEvent& event);
		virtual void onSelectStepSensitivity(wxCommandEvent& event);
		virtual void onSelectStepMode(wxCommandEvent& event);
		virtual void onDeactivateSecureRunMode(wxCommandEvent& event);
		virtual void requestResolveLimitStates(wxCommandEvent& event);
		virtual void requestToolTest(wxCommandEvent& event);
		virtual void onEvaluateHardwareXYPlane(wxCommandEvent& event);
		virtual void onEvaluateHardwareZAxis(wxCommandEvent& event);
		virtual void onTakeoverHardwareDimensions(wxCommandEvent& event);
		virtual void onResetHardwareReference(wxCommandEvent& event);
		virtual void onEvaluateHardwareReference(wxCommandEvent& event);
		virtual void openSpeedPlayground(wxCommandEvent& event);
		virtual void onChangePreviewMode(wxCommandEvent& event);
		virtual void onCloseSecureRunAuiPane(wxCommandEvent& event);
		virtual void onOpenGLContextObserver(wxCommandEvent& event);
		virtual void changeMonitorListBook(wxListbookEvent& event);
		virtual void showStacktraceStore(wxCommandEvent& event);
		virtual void dclickHeartbeatState(wxMouseEvent& event);
		virtual void showOSEnvironment(wxCommandEvent& event);
		virtual void toggleMotionMonitorReplayPane(wxCommandEvent& event);
		virtual void selectMetricUnitFrom(wxCommandEvent& event);
		virtual void selectMetricUnitFromValue(wxCommandEvent& event);
		virtual void selectMetricUnitTo(wxCommandEvent& event);
		virtual void toggleMotionMonitorOptionPane(wxCommandEvent& event);
		virtual void toggleMotionMonitorStatisticPane(wxCommandEvent& event);
		virtual void toggleOutboundEditorWordWrap(wxCommandEvent& event);
		virtual void extractSourceAsNewTpl(wxCommandEvent& event);
		virtual void saveOutboundAsNewTplFromButton(wxCommandEvent& event);
		virtual void saveOutboundAsNewTplFromMenu(wxCommandEvent& event);
		virtual void selectBinaryEditorViewMode(wxCommandEvent& event);
		virtual void traceSessionId(wxCommandEvent& event);
		virtual void openSessionDialog(wxCommandEvent& event);
		virtual void toggleIdleRequests(wxCommandEvent& event);
		virtual void cncMainViewChanged(wxNotebookEvent& event);
		virtual void rcSecureDlg(wxCommandEvent& event);
		virtual void changeConfigToolbook(wxToolbookEvent& event);
		virtual void warmStartController(wxCommandEvent& event);
		virtual void setReferencePosition(wxCommandEvent& event);
		virtual void setAnchorPosition(wxCommandEvent& event);
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
		virtual void clearSetterList(wxCommandEvent& event);
		virtual void loopRepeatTest(wxCommandEvent& event);
		virtual void selectPositionSpyContent(wxCommandEvent& event);
		virtual void copyPositionSpy(wxCommandEvent& event);
		virtual void togglePositionSpy(wxCommandEvent& event);
		virtual void activateAuiPane(wxAuiManagerEvent& event);
		virtual void buttonAuiPane(wxAuiManagerEvent& event);
		virtual void loadPerspective(wxCommandEvent& event);
		virtual void savePerspective(wxCommandEvent& event);
		virtual void addUserPerspective(wxCommandEvent& event);
		virtual void renameUserPerspective(wxCommandEvent& event);
		virtual void removeUserPerspective(wxCommandEvent& event);
		virtual void viewDebugger(wxCommandEvent& event);
		virtual void viewAccelerationMonitor(wxCommandEvent& event);
		virtual void toggleTemplateManager(wxCommandEvent& event);
		virtual void clearPositionSpy(wxCommandEvent& event);
		virtual void onSelectManuallyMove(wxCommandEvent& event);
		virtual void onSelectReferences(wxCommandEvent& event);
		virtual void onSelectSetup(wxCommandEvent& event);
		virtual void onSelectTemplate(wxCommandEvent& event);
		virtual void viewStatusbar(wxCommandEvent& event);
		virtual void searchAvailiablePorts(wxCommandEvent& event);
		virtual void unitTestFramework(wxCommandEvent& event);
		virtual void selectUCChangeFrom(wxCommandEvent& event);
		virtual void selectUCUnitFrom(wxCommandEvent& event);
		virtual void selectUCUnitTo(wxCommandEvent& event);
		virtual void closeUnitCalculator(wxCommandEvent& event);
		virtual void viewUnitCalculator(wxCommandEvent& event);
		virtual void viewSpy(wxCommandEvent& event);
		virtual void paintDrawPaneWindow(wxPaintEvent& event);
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
		virtual void closeAuiPane(wxAuiManagerEvent& evt);
		virtual void toogleSvgEditSearchFlag(wxCommandEvent& event);
		virtual void svgEditSelected(wxCommandEvent& event);
		virtual void svgEditSearchTextChanged(wxCommandEvent& event);
		virtual void svgEditFind(wxCommandEvent& event);
		virtual void svgEditFindPrev(wxCommandEvent& event);
		virtual void dclickDurationCount(wxMouseEvent& event);
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
		virtual void defineMinMonitoring(wxCommandEvent& event);
		virtual void defineNormalMonitoring(wxCommandEvent& event);
		virtual void saveTemplate(wxCommandEvent& event);
		virtual void saveTemplateAs(wxCommandEvent& event);
		virtual void newTemplate(wxCommandEvent& event);
		virtual void openTemplate(wxCommandEvent& event);
		virtual void reloadTemplate(wxCommandEvent& event);
		virtual void openTemplateSourceExtern(wxCommandEvent& event);
		virtual void openTemplateExtern(wxCommandEvent& event);
		virtual void reloadTemplateFromButton(wxCommandEvent& event);
		virtual void defineUpdateCoordinates(wxCommandEvent& event);
		virtual void defineAllowEvents(wxCommandEvent& event);
		virtual void defineOnlineDrawing(wxCommandEvent& event);
		virtual void selectUnit(wxCommandEvent& event);
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
		void onSerialThreadDataNotification(SerialEvent& event);
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
		
		// ----------------------------------------------------------
		enum EventId { 	INITIALIZED 				=  1,
						COMPLETED 					=  2,
						HEARTBEAT 					=  3,
						DISPATCH_ALL 				=  5, 
						POST_INFO 					=  6, 
						POST_WARNING 				=  7, 
						POST_ERROR 					=  8,
						GAMEPAD_STATE				=  9,
						GAMEPAD_MESSAGE				= 10,
						SERIAL_HEARTBEAT			= 11,
						SERIAL_MESSAGE				= 12,
						SERIAL_DATA_NOTIFICATION	= 13,
						SERIAL_PIN_NOTIFICATION		= 14
					  };
					  
		enum class RunConfirmationInfo {Wait, Confirmed, Canceled};
		
		// ----------------------------------------------------------
		struct Notification {
			
			enum Location { NL_MainView, NL_MonitorView };
			
			Location		location	= NL_MainView;
			char			type		= 'I'; 
			wxString		title		= ""; 
			wxString		message		= ""; 
		};
		
		//////////////////////////////////////////////////////////////////////////////////
		MainFrame(wxWindow* parent, wxFileConfig* globalConfig);
		virtual ~MainFrame();
		
		virtual bool Show(bool show);

		void initialize(void);
		bool secureRun() { return processTemplateIntern(); }
		
		void handleCncInterruptException(const CncInterruption& ex);
		void handleUnhandledException(const wxString& context);
		
		wxListCtrl* 		getCtrlSerialSpy()			{ return (wxListCtrl* )(serialSpyPanel->getSerialSpyCtrl()); }
		CncMotionMonitor*	getMotionMonitor()			{ return motionMonitor; }
		wxMenuItem*			GetMiMotorEnableState()		{ return m_miMotorEnableState; }
		
		//////////////////////////////////////////////////////////////////////////////////
		void startAnimationControl();
		void stopAnimationControl();
		
		void selectMainBookSourcePanel(int sourcePageToSelect = SourceBookSelection::VAL::EDITOR);
		void selectMainBookPreviewPanel();
		void selectMainBookSetupPanel();
		void selectMainBookReferencePanel();
		void selectMainBookManuelPanel();
		void selectMainBookTestPanel();
		
		void selectMonitorBookCncPanel();
		void selectMonitorBookTemplatePanel();
		
		void selectParsingSynopsisTrace();
		
		void setControllerPowerStateBmp(bool state);
		
		void selectSecureMonitorView();
		
		//////////////////////////////////////////////////////////////////////////////////
		// setup
		void installCustControls();
		
		//////////////////////////////////////////////////////////////////////////////////
		// svg edit popup callbacks
		void openCurrentTemplateInBrowser();
		void openTemplateExtern();
		
		//////////////////////////////////////////////////////////////////////////////////
		void displayNotification(const Notification& notification);
		
		//////////////////////////////////////////////////////////////////////////////////
		virtual void ShowAuiToolMenu(wxAuiToolBarEvent& event);
		
		/////////////////////////////////////////////////////////////////////////////////
		// configuration callbacks
		void releaseControllerSetupFromConfig();
		void notifyConfigUpdate();
		void changeCrossingThickness();
		void decorateSpindleState(CncSpindlePowerState state);
		
		//////////////////////////////////////////////////////////////////////////////////
#		ifdef __WXMSW__
			virtual WXLRESULT MSWWindowProc(WXUINT, WXWPARAM, WXLPARAM);
			WXLRESULT onDeviceChange(WXUINT, WXWPARAM, WXLPARAM);
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
		
		void toggleMotionMonitorOptionPane(bool forceHide);
		void toggleMotionMonitorStatisticPane(bool forceHide);
		void toggleMotionMonitorReplayPane(bool forceHide);
		
		void showMotionMonitorStatisticPane(bool show=true);
		void showMotionMonitorReplayPane(bool show=true);
		void hideMotionMonitorStatisticPane()			{ showMotionMonitorStatisticPane(false); }
		void hideMotionMonitorReplayPane()				{ showMotionMonitorReplayPane(false);    }

		void clearMotionMonitor();
		
		CncTouchBlockDetector::Result processTouchTest(const CncTouchBlockDetector::Parameters& para);
		CncTouchBlockDetector::Result processZTouch(const CncTouchBlockDetector::Parameters& para);
		CncTouchBlockDetector::Result processXYZTouch(const CncTouchBlockDetector::Parameters& para);
		
		double getConfiguredSpindleSpeed();
		
		void clearTemplateEditorExtViews();
		
		void motionMonitorZoom(float fact);
		void motionMonitorRotateX(float angle);
		void motionMonitorRotateY(float angle);
		void motionMonitorRotateZ(float angle);
		
		bool isPause()						{ return THE_CONTEXT->isPause(); }
		bool isProcessing() 				{ return THE_CONTEXT->isProcessing(); }
		bool isInterrupted()				{ return ( cnc ? cnc->isInterrupted() : false); }
		
		bool evaluateAndPerformProcessingState();
		
	protected:
	
		void tryToSelectClientId(long clientId, ClientIdSelSource::ID tss);
		void tryToSelectClientIds(long firstClientId, long lastClientId, ClientIdSelSource::ID tss);
		void selectSourceControlLineNumber(long ln);
		void selectSourceControlLineNumbers(long firstLine, long lastLine);

		// will be bind to this frame
		void onGlobalKeyDownHook(wxKeyEvent& event);
		void onIdle(wxIdleEvent& event);

		void selectStepSensitivity(int sel);
		void shiftStepSensitivity();
 
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
		CncLoggerListCtrl* getCtrlMsgHistoryList()			{ return controllersMsgHistoryList; }
		
		CncGCodeSequenceListCtrl* getGCodeSequenceList() 	{ return gCodeSequenceList; }
		CncMotionVertexTrace* getMotionVertexTrace() 		{ return motionVertexCtrl; } 
		CncParsingSynopsisTrace* getParsingSynopsisTrace();
		
		void updateReferencePosition(RefPosResult* parameter);
		
		int isPortNameAvailable(const wxString& portName, bool exact=false);
		
		bool setTemplateName(const wxString& pathFile);
		bool setTemplateName(const wxFileName& pathFile);
		bool getFirstLruFile(wxString& ret);
		void openTemplate();
		
		bool readSerialThreadData(AE::TransferData& td);
		
		bool startStepwiseMovement(CncLinearDirection x, CncLinearDirection y, CncLinearDirection z);
		bool startInteractiveMove(CncInteractiveMoveDriver imd);
		bool updateInteractiveMove();
		bool updateInteractiveMove(const CncLinearDirection x, const CncLinearDirection y, const CncLinearDirection z, int modifySpeed=0);
		bool stopInteractiveMove();
		bool isInteractiveMoveActive() const;
		
		bool connectSerialPort();
		bool connectSerialPortDialog();
		void selectPort();
		
		void decorateTryRunState(CncState state);
		
		void decorateProbeMode(bool probeMode);
		void decorateSecureDlgChoice(bool useDlg);
		void decorateOutboundEditor(const char* fileName = NULL);
		
		void decorateGamepadState(bool state);
		
		void decorateIdleState(bool state);
		
		void enableSourceEditorMenuItems(bool enable);
		
		void enableControls(bool state = true);
		void disableControls() { enableControls(false); }
		
		SerialThread* getSerialThread(SerialThreadStub* sts);
		
		void cncTransactionLockCallback(CncTransactionLockBase* tal);
		void cncTransactionReleaseCallback(CncTransactionLockBase* tal);
		
		void motionMonitorViewTop();
		void motionMonitorViewBottom();
		void motionMonitorViewLeft();
		void motionMonitorViewRight();
		void motionMonitorViewFront();
		void motionMonitorViewRear();
		
		void motionMonitorViewPerspective1();
		void motionMonitorViewPerspective2();
		void motionMonitorViewPerspective3();
		void motionMonitorViewPerspective4();
		
		bool isReferenceStateValid() const;
		const RefPosResult& getReferencePosResult(RefPosResult& result)	const;
		void setReferencePosEnforceFlag(bool s);
		void resetReferencePosTempSetting();
		
		bool resetPodiumDistance();
		bool applyPodiumDistance();
		
		bool processManualMove(const CncMoveDefinition& md);
		
		friend class MainFrameProxy;
		friend class CncMsgHistoryLoggerProxy;
		friend class CncLoggerListCtrl;
		friend class CncPerspective;

		friend class CncBaseEditor;
		friend class CncSourceEditor;
		friend class CncConfig;
		friend class CncContext;
		friend class CncGampadDeactivator;
		friend class CncIdleCheckDeactivator;
		friend class CncTransactionLockBase;
		friend class CncTransactionLock;
		friend class CncGamepadTransactionLock;
		friend class CncGamepadMenuDlg;
		friend class CncSerialSpyPanel;
		friend class CncSecureCtrlPanel;

		friend class GamepadThread;
		friend class SerialThread;
		friend class SerialThreadStub;
		
		friend class CncArduinoEnvironment;
		friend class CncPodiumMgmtMovement;
		friend class CncSecureManuallyMovePanel;
		
		friend class CncPathListRunner;

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
		bool 							canClose;
		bool 							evaluatePositions;
		bool 							ignoreDirControlEvents;
		
		CncToolStateControl				toolState;
		RunConfirmationInfo  			runConfirmationInfo;
		CncGamepadTransactionLock*		interactiveTransactionLock;
		
		int 							startTimerTimeout;
		int 							serialTimerTimeout;
		int 							traceTimerTimeout;
		int 							traceTimerCounter;
		
		wxString 						lastPortName;
		
		CncControl* 					cnc;
		
		CncLruFileViewListCtrl*			lruFileView;
		CncSourceEditor* 				sourceEditor;
		CncOutboundEditor* 				outboundEditor;
		CncMotionMonitor* 				motionMonitor;
		CncFileView* 					fileView;
		CncFileView*					transferFileView;
		CncFilePreview* 				mainFilePreview;
		CncFilePreview* 				outboundFilePreview;
		CncFilePreview* 				monitorFilePreview;
		CncToolMagazine* 				toolMagazine;
		CncPosSpyListCtrl* 				positionSpy;
		CncSetterListCtrl* 				setterList;
		CncSpeedMonitor*				speedMonitor;
		CncDefaultSpeedSlider*			defaultSpeedSlider;
		CncMotionVertexTrace* 			motionVertexCtrl;
		CncPreprocessor*				cncPreprocessor;
		CncGCodeSequenceListCtrl*		gCodeSequenceList;
		CncSummaryListCtrl* 			cncSummaryListCtrl;
		CncSerialSpyPanel* 				serialSpyPanel;
		CncSvgViewer*					outboundEditorSvgView;
		CncNavigatorPanel*				navigatorPanel;
		GL3DOptionPane* 				optionPane3D;
		GL3DDrawPane* 					drawPane3D;
		CncStatisticsPane*				statisticsPane;
		CncMonitorVSplitterWindow* 		cnc3DVSplitterWindow;
		CncMonitorHSplitterWindow* 		cnc3DHSplitterWindow;
		CncTemplateObserver* 			templateObserver;
		CncOpenGLContextObserver*		openGLContextObserver;
		CncOSEnvironmentDialog* 		cncOsEnvDialog;
		CncExternalViewBoxCluster*		cncExtViewBoxCluster;
		CncExternalViewBox* 			cncExtMainPreview;
		CncArduinoEnvironment*			cncArduinoEnvironment;
		CncLCDPositionPanel*			cncLCDPositionPanel;
		CncManuallyMoveCoordinates*		cncManuallyMoveCoordPanel;
		CncGamepadSpy* 					gamepadSpy;
		CncGamepadControllerState*		gamepadStatusCtl; 
		CncLoggerListCtrl* 				controllersMsgHistoryList;
		CncMainInfoBar*					mainViewInfobar;
		CncMainInfoBar*					monitorViewInfobar;
		CncPositionStorageView*			positionStorage;
		CncUsbConnectionObserver*		usbConnectionObserver;
		CncAnchorPosition*				anchorPositionDlg;
		CncSecureCtrlPanel*				secureCtrlPanel;
		
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
		CncPodiumManagement*  podiumManagementDlg; 
		
		wxSharedPtr<wxNotificationMessageBase> notificationDialog;
		
		void onIndividualCommand(wxCommandEvent& event);

		const wxString& createCncControl(const wxString& sel, wxString& name);
		
		void setIcons();
		
		void traceGccVersion(std::ostream& out);
		void traceWxWidgetsVersion(std::ostream& out);
		void traceWxSvgVersion(std::ostream& out);
		void traceBoostVersion(std::ostream& out);
		void traceWoodworkingCncVersion(std::ostream& out);
		void traceOpenCvVersion(std::ostream& out);
		
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
		void reloadTemplate(int sourcePageToSelect = SourceBookSelection::VAL::EDITOR);
		void introduceCurrentFile(int sourcePageToSelect = SourceBookSelection::VAL::EDITOR);
		
		void openPreview(CncFilePreview* ctrl, const wxString& fn);
		void openMainPreview(const wxString& fn);
		void openMonitorPreview(const wxString& fn);
		void openFileFromFileManager(const wxString& fn);
		void openNavigatorFromGamepad();
		
		bool openFileExtern(const wxString& tool, const wxString& file, bool checkToolExists=true);
		
		CncTemplateFormat getTemplateFormat(const wxString& fn);
		const char* getTemplateFormatName(const wxString& fn);

		CncTemplateFormat getCurrentTemplateFormat();
		const char* getCurrentTemplateFormatName();
		const wxString& getCurrentTemplateFileName();
		const wxString& getCurrentTemplatePathFileName();
		
		void updateHardwareReference();
		void updateHardwareDimensions();
		
		bool saveTemplateOnDemand(bool force);

		BinaryFileParser::ViewType getCurrentBinaryViewMode();
		
		bool openFile(int pageToSelect = -1);
		bool saveFile(bool interactive = true);
		bool saveFileAs();
		
		///////////////////////////////////////////////////////////////
		// monitoring
		void defineMinMonitoring();
		void defineNormalMonitoring();
		void switchMonitorButton(bool state);
		void updateMonitoring();
		
		///////////////////////////////////////////////////////////////
		// configuration
		void updateUnit();
		void updateFileContentPosition(long x, long y);
		
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
		
		void displayPositionSituation(int type, const wxString& msg, const wxString& headLine = "Current Positions:", const wxString& appendix = wxEmptyString);
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
		
		void startDebugUserNotification();
		void stopDebugUserNotification();
		
		bool showConfigSummaryAndConfirmRun();
		
		void collectSummary();
		void collectSvgSpecificSummary();
		void collectGCodeSpecificSummary();
		
		///////////////////////////////////////////////////////////////
		// control handling
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
		void viewAllAuiPanes();
		
		void clearPositionSpy();
		void prepareMotionMonitorViewType();

		wxWindow* getAUIPaneByName(const wxString& name);
		wxMenuItem* getAUIMenuByName(const wxString& name);
		
		void decorateViewMenu();
		
		void selectEditorToolBox(bool fileLoaded);
		void fillFileDetails(bool fileLoaded, const char* extFileName = NULL);
		void decorateExtTemplatePages(CncTemplateFormat tf);
		void prepareAndShowMonitorTemplatePreview(bool force=false);
		
		void registerMenuItem(wxMenuItem* item);
		void unregisterMenuItem(wxMenuItem* item);
		void regiterAllMenuItems();
		void registerGuiControl(wxWindow* ctl); 
		void disableGuiControls();
		void enableGuiControls(bool state = true);
		void enableMenuItems(bool state = true);
		void activate3DPerspectiveButton(wxButton* bt);

		///////////////////////////////////////////////////////////////
		// manually control
		void enableTestControls(bool state);
		void processTestMove(wxStaticText* axis, wxStaticText* counter, 
		                     int c, double xd, double yd, double zd);
		
		///////////////////////////////////////////////////////////////
		// misc
		void traceSessionId();
		void setControllerZero(CncRefPositionMode m, double x, double y, double z);
		void requestReset();
		
		void navigateX(CncDirection d);
		void navigateY(CncDirection d);
		void navigateZ(CncDirection d);
		
		void resetMinMaxPositions();
		bool disconnectSerialPort();
		
		void notifyComPortArrival(const wxString& rawPortName);
		void notifyComPortRemoval(const wxString& rawPortName);
		
		void handleCommonException();
};

////////////////////////////////////////////////////////////////////
class CncTransactionLockBase {
	
	protected:
		static unsigned int	referenceCounter;
		
		MainFrame*			parent;
		bool				state;
		bool				errorMode;
	
		explicit CncTransactionLockBase(MainFrame* p);
		virtual ~CncTransactionLockBase();
		
		bool waitUntilCncIsAvailable();
		
	public:
		
		bool					isOk()				const	{ return state; }
		void 					setErrorMode() 				{ errorMode = true;	}
		
		static bool 			isLocked() 					{ return referenceCounter != 0; }
		static unsigned int 	getReferenceCount()			{ return referenceCounter; }
};

////////////////////////////////////////////////////////////////////
class CncTransactionLock : public CncTransactionLockBase {
	
	public:
		explicit CncTransactionLock(MainFrame* p);
		virtual ~CncTransactionLock();
};

////////////////////////////////////////////////////////////////////
class CncGamepadTransactionLock : public CncTransactionLockBase {
	
	public:
		explicit CncGamepadTransactionLock(MainFrame* p);
		virtual ~CncGamepadTransactionLock();
};

////////////////////////////////////////////////////////////////////
class CncIdleCheckDeactivator {
	
	private:
		static unsigned int	referenceCounter;
		
		MainFrame*	parent;
		bool		prevState;
	
	public:
		CncIdleCheckDeactivator(MainFrame* p)
		: parent		(p)
		, prevState	(false)
		{
			wxASSERT ( parent );
			
			prevState = parent->m_miRqtIdleMessages->IsChecked();
			if ( prevState != false )
				parent->m_miRqtIdleMessages->Check(false);
				
			referenceCounter++;
		}
		
		~CncIdleCheckDeactivator()
		{
			wxASSERT ( parent );
			
			parent->m_miRqtIdleMessages->Check(prevState);
			
			referenceCounter--;
		}
		
		static void activate(bool b) { 
			if ( THE_APP )
				THE_APP->m_miRqtIdleMessages->Check(b); 
		}
};

////////////////////////////////////////////////////////////////////
class CncGampadDeactivator {
	
	private:
		static unsigned int referenceCounter;
		
		MainFrame*	parent;
		bool		reconstructPrevState;
		bool		prevState;
		
	public:
		CncGampadDeactivator(MainFrame* p, bool rps = false);
		~CncGampadDeactivator();
		
		static bool gamepadNotificationsAllowed() { return referenceCounter == 0; }
};

////////////////////////////////////////////////////////////////////
class CncRunAnimationControl {
	
	private:
		
		MainFrame*	parent;
		
	public:
	
		explicit CncRunAnimationControl(MainFrame* p)
		: parent(p)
		{ if ( parent ) parent->startAnimationControl(); }
		
		~CncRunAnimationControl()
		{ if ( parent ) parent->stopAnimationControl(); }
};

#endif // MAINFRAME_H
