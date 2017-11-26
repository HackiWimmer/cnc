#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "wxcrafter.h"
#include <wx/wx.h>
#include <wx/notifmsg.h>
#include <wx/generic/notifmsg.h>
#include "NotebookInfo.h"
#include "LruFileList.h"
#include "FileParser.h"
#include "PathGeneratorFrame.h"
#include "UpdateManagerThread.h"
#include "CncControl.h"
#include "CncMotionMonitor.h"
#include "CncSpyControl.h"
#include "CncFileView.h"
#include "CncToolMagazine.h"
#include "CncLargeScaleListCtrl.h"
#include "codelite/wxPNGAnimation.h"

////////////////////////////////////////////////////////////////////
// forward declarations
class wxFileConfig;
class CncFilePreviewWnd;
class CncFilePreview;
class wxMenu;
class wxMenuItem;
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// app defined events
	wxDECLARE_EVENT(wxEVT_PERSPECTIVE_TIMER, 					wxTimerEvent);
	wxDECLARE_EVENT(wxEVT_DEBUG_USER_NOTIFICATION_TIMER, 		wxTimerEvent);
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// global definitions
typedef std::vector<wxWindow*> GuiControls;
class MainFrame;

class GlobalConfigManager {
	
	public:
		GlobalConfigManager(MainFrame* mf, wxPropertyGridManager* pgMgrSetup, wxFileConfig* globalConfig) {
			wxASSERT(globalConfig);
			wxASSERT(pgMgrSetup);
			wxASSERT(mf);
			
			// setup configuration
			CncConfig::setupGlobalConfigurationGrid(pgMgrSetup, *globalConfig);
			CncConfig::globalCncConfig = new CncConfig(mf);
			
			// at least load the file configuration
			CncConfig::globalCncConfig->loadConfiguration(*globalConfig);
		}
		
		~GlobalConfigManager() {
			if ( CncConfig::globalCncConfig != NULL )
				delete CncConfig::globalCncConfig;
		}
};


////////////////////////////////////////////////////////////////////
class MainFrame : public MainFrameBClass, public GlobalConfigManager {

	// User command
	protected:
    virtual void searchPosSpy(wxCommandEvent& event);
		virtual void goPosSypFirstId(wxCommandEvent& event);
		virtual void goPosSypLastId(wxCommandEvent& event);
		virtual void goPosSypNextId(wxCommandEvent& event);
		virtual void goPosSypPrevId(wxCommandEvent& event);
		virtual void selectSerialSpyMode(wxCommandEvent& event);
		virtual void requestErrorMessage(wxCommandEvent& event);
		virtual void requestWarningMessage(wxCommandEvent& event);
		virtual void requestInfoMessage(wxCommandEvent& event);
		virtual void clearSetterList(wxCommandEvent& event);
		virtual void sizeSetterList(wxSizeEvent& event);
		virtual void loopRepeatTest(wxCommandEvent& event);
		virtual void selectPositionSpyContent(wxCommandEvent& event);
		virtual void selectPositionSpy(wxListEvent& event);
		virtual void copyPositionSpy(wxCommandEvent& event);
		virtual void togglePositionSpy(wxCommandEvent& event);
		virtual void activateAuiPane(wxAuiManagerEvent& event);
		virtual void buttonAuiPane(wxAuiManagerEvent& event);
		virtual void trace3D(wxCommandEvent& event);
		virtual void viewZAxis(wxCommandEvent& event);
		virtual void loadPerspectiveDebug(wxCommandEvent& event);
		virtual void loadPerspectiveRun(wxCommandEvent& event);
		virtual void loadPerspectiveSource(wxCommandEvent& event);
		virtual void savePerspectiveDebug(wxCommandEvent& event);
		virtual void savePerspectiveRun(wxCommandEvent& event);
		virtual void savePerspectiveSource(wxCommandEvent& event);
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
		virtual void lruListItemLeave(wxMouseEvent& event);
		virtual void lruListItemActivated(wxListEvent& event);
		virtual void lruListItemSelected(wxListEvent& event);
		virtual void openMotionMonitorOptionDlg(wxCommandEvent& event);
		virtual void viewStatusbar(wxCommandEvent& event);
		virtual void searchAvailiablePorts(wxCommandEvent& event);
		virtual void unitTestFramework(wxCommandEvent& event);
		virtual void traceTimer(wxTimerEvent& event);
		virtual void selectUCChangeFrom(wxCommandEvent& event);
		virtual void selectUCUnitFrom(wxCommandEvent& event);
		virtual void selectUCUnitTo(wxCommandEvent& event);
		virtual void closeUnitCalculator(wxCommandEvent& event);
		virtual void emuContentDClick(wxMouseEvent& event);
		virtual void fileContentDClick(wxMouseEvent& event);
		virtual void viewSpeed(wxCommandEvent& event);
		virtual void viewUnitCalculator(wxCommandEvent& event);
		virtual void markSerialSpy(wxCommandEvent& event);
		virtual void viewSpy(wxCommandEvent& event);
		virtual void requestErrorCount(wxCommandEvent& event);
		virtual void paintDrawPaneWindow(wxPaintEvent& event);
		virtual void UpdateLogger(wxCommandEvent& event);
		virtual void cfgStepDelayDropDown(wxAuiToolBarEvent& event);
		virtual void cfgStepDelayArduino(wxCommandEvent& event);
		virtual void cfgStepDelayMax(wxCommandEvent& event);
		virtual void cfgStepDelayMin(wxCommandEvent& event);
		virtual void enableSerialSpy(wxCommandEvent& event);
		virtual void clearSerialSpy(wxCommandEvent& event);
		virtual void freezeLogger(wxCommandEvent& event);
		virtual void nootebookConfigChanged(wxListbookEvent& event);
		virtual void cancelRun(wxCommandEvent& event);
		virtual void confirmRun(wxCommandEvent& event);
		virtual void clearControllerMsgHistory(wxCommandEvent& event);
		virtual void requestControllerPinsFromButton(wxCommandEvent& event);
		virtual void requestPins(wxCommandEvent& event);
		virtual void traceTextUpdated(wxCommandEvent& event);
		virtual void ctrlTestCaseSelected(wxCommandEvent& event);
		virtual void treebookTestChanged(wxTreebookEvent& event);
		virtual void openPyCam(wxCommandEvent& event);
		virtual void moveXToMid(wxCommandEvent& event);
		virtual void moveYToMid(wxCommandEvent& event);
		virtual void moveZToMid(wxCommandEvent& event);
   		virtual void testDimTakeOverAll(wxCommandEvent& event);
		virtual void testDimTakeOverX(wxCommandEvent& event);
		virtual void testDimTakeOverY(wxCommandEvent& event);
		virtual void testDimTakeOverZ(wxCommandEvent& event);
		virtual void refresh3D(wxCommandEvent& event);
		virtual void clear3D(wxCommandEvent& event);
		virtual void runOpenGLTest(wxCommandEvent& event);
		virtual void show3D(wxCommandEvent& event);
		virtual void showFromBottom3D(wxCommandEvent& event);
		virtual void showFromFront3D(wxCommandEvent& event);
		virtual void showFromLeft3D(wxCommandEvent& event);
		virtual void showFromRear3D(wxCommandEvent& event);
		virtual void showFromRight3D(wxCommandEvent& event);
		virtual void showFromTop3D(wxCommandEvent& event);
		virtual void testEndSwitchEvaluation(wxCommandEvent& event);
		virtual void testSwitchToolOnOff(wxCommandEvent& event);
		virtual void testCountXSpinCtl(wxSpinEvent& event);
		virtual void testCountXUpdated(wxCommandEvent& event);
		virtual void selectTestIntervalMode(wxCommandEvent& event);
		virtual void checkManuallyXY(wxCommandEvent& event);
		virtual void checkManuallyZ(wxCommandEvent& event);
		virtual void changeManuallySliderZ(wxScrollEvent& event);
		virtual void maxManuallyZSlider(wxCommandEvent& event);
		virtual void minManuallyZSlider(wxCommandEvent& event);
		virtual void moveManuallySliderZ(wxScrollEvent& event);
		virtual void signManuallyZSlider(wxCommandEvent& event);
		virtual void updateMetricZ(wxCommandEvent& event);
		virtual void zeroManuallyZSlider(wxCommandEvent& event);
		virtual void requestCurrentLimitStateIcon(wxMouseEvent& event);
		virtual void rcDebugConfig(wxCommandEvent& event);
		virtual void rcFinish(wxCommandEvent& event);
		virtual void rcNextBreakpoint(wxCommandEvent& event);
		virtual void rcNextStep(wxCommandEvent& event);
		virtual void rcPause(wxCommandEvent& event);
		virtual void rcReset(wxCommandEvent& event);
		virtual void rcRun(wxCommandEvent& event);
		virtual void rcStop(wxCommandEvent& event);
		virtual void switchMonitoring(wxCommandEvent& event);
		virtual void fileContentChange(wxStyledTextEvent& event);
		virtual void viewToolbar(wxCommandEvent& event);
		virtual void renderAuiPane(wxAuiManagerEvent& event);
		virtual void restoreAuiPane(wxAuiManagerEvent& event);
		virtual void maximizeAuiPane(wxAuiManagerEvent& event);
		virtual void fileContentKeyUp(wxKeyEvent& event);
		virtual void saveTemplateFromButton(wxCommandEvent& event);
		virtual void marginClickEmuSource(wxStyledTextEvent& event);
		virtual void marginClickFileContent(wxStyledTextEvent& event);
		virtual void emuContentRightDown(wxMouseEvent& event);
		virtual void toggleEmuWordWrapMode(wxCommandEvent& event);
		virtual void toggleTemplateWordWrapMode(wxCommandEvent& event);
		virtual void copyLogger(wxCommandEvent& event);
		virtual void closeAuiPane(wxAuiManagerEvent& evt);
		virtual void svgEmuZoomHome(wxCommandEvent& event);
		virtual void svgEmuZoomMinus(wxCommandEvent& event);
		virtual void svgEmuZoomPlus(wxCommandEvent& event);
		virtual void toogleSvgEditSearchFlag(wxCommandEvent& event);
		virtual void svgEditSelected(wxCommandEvent& event);
		virtual void svgEditSearchTextChanged(wxCommandEvent& event);
		virtual void svgEditFind(wxCommandEvent& event);
		virtual void svgEditFindPrev(wxCommandEvent& event);
		virtual void fileContentRightDown(wxMouseEvent& event);
		virtual void dclickDurationCount(wxMouseEvent& event);
		virtual void stepDelayThumbtrack(wxScrollEvent& event);
		virtual void stepDelayChanged(wxScrollEvent& event);
		virtual void svgEmuToogleOriginalPath(wxCommandEvent& event);
		virtual void displayUserAgent(wxCommandEvent& event);
		virtual void updateToolControls(wxCommandEvent& event);
		virtual void serialTimer(wxTimerEvent& event);
		virtual void startupTimer(wxTimerEvent& event);
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
		virtual void openSVGPathGenerator(wxCommandEvent& event);
		virtual void openConfigurationFile(wxCommandEvent& event);
		virtual void openExternalEditor(wxCommandEvent& event);
		virtual void openCalculator(wxCommandEvent& event);
		virtual void fileContentLeftUp(wxMouseEvent& event);
		virtual void fileContentLeftDown(wxMouseEvent& event);
		virtual void fileContentKeyDown(wxKeyEvent& event);
		virtual void svgEmuClear(wxCommandEvent& event);
		virtual void selectPort(wxCommandEvent& event);
		virtual void saveEmuOutput(wxCommandEvent& event);
		virtual void requestVersion(wxCommandEvent& event);
		virtual void requestCurrentPos(wxCommandEvent& event);
		virtual void requestCurrentLimitState(wxCommandEvent& event);
		virtual void requestConfig(wxCommandEvent& event);
		virtual void requestControllerConfigFromButton(wxCommandEvent& event);
		virtual void requestErrorInfo(wxCommandEvent& event);
		virtual void requestControllerErrorInfoFromButton(wxCommandEvent& event);
		virtual void requestReset(wxCommandEvent& event);
		virtual void requestEnableStepperMotors(wxCommandEvent& event);
		virtual void keyDownZ(wxKeyEvent& event);
		virtual void keyDownXY(wxKeyEvent& event);
		virtual void changeUpdateInterval(wxCommandEvent& event);
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
		virtual void mvSpinDownY(wxSpinEvent& event);
		virtual void mvSpinDownZ(wxSpinEvent& event);
		virtual void mvSpinUpZ(wxSpinEvent& event);
		virtual void mvSpinDownX(wxSpinEvent& event);
		virtual void mvSpinUpX(wxSpinEvent& event);
		virtual void mvSpinUpY(wxSpinEvent& event);
		virtual void updateInclWpt(wxCommandEvent& event);
		virtual void svgEmuOpenFileAsSource(wxCommandEvent& event);
		virtual void svgEmuOpenFileAsSvg(wxCommandEvent& event);
		virtual void svgEmuReload(wxCommandEvent& event);
		virtual void selectUnit(wxCommandEvent& event);
		virtual void setZero(wxCommandEvent& event);
		virtual void clearLogger(wxCommandEvent& event);
		virtual void connect(wxCommandEvent& event);
		virtual void selectUAInboundPathList(wxDataViewEvent& event);
		virtual void selectUAUseDirectiveList(wxDataViewEvent& event);
		virtual void selectUADetailInfo(wxDataViewEvent& event);
		virtual void activateMainWindow(wxActivateEvent& event);
		virtual void outboundBookChanged(wxNotebookEvent& event);
		virtual void outboundBookChanging(wxNotebookEvent& event);
		virtual void moveManuallySliderX(wxScrollEvent& event);
		virtual void moveManuallySliderY(wxScrollEvent& event);
		virtual void changeManuallySliderX(wxScrollEvent& event);
		virtual void changeManuallySliderY(wxScrollEvent& event);
		virtual void zeroManuallyXSlider(wxCommandEvent& event);
		virtual void zeroManuallyYSlider(wxCommandEvent& event);
		virtual void signManuallyXSlider(wxCommandEvent& event);
		virtual void signManuallyYSlider(wxCommandEvent& event);
		virtual void minManuallyXSlider(wxCommandEvent& event);
		virtual void maxManuallyXSlider(wxCommandEvent& event);
		virtual void minManuallyYSlider(wxCommandEvent& event);
		virtual void maxManuallyYSlider(wxCommandEvent& event);
		virtual void updateMetricX(wxCommandEvent& event);
		virtual void updateMetricY(wxCommandEvent& event);
		virtual void killFocusMoveZAxis(wxFocusEvent& event);
		virtual void killFocusMoveXYAxis(wxFocusEvent& event);
		virtual void setFocusMoveXYAxis(wxFocusEvent& event);
		virtual void setFocusMoveZAxis(wxFocusEvent& event);
		virtual void viewMainView(wxCommandEvent& event);
		virtual void viewTemplateManager(wxCommandEvent& event);
		virtual void viewLogger(wxCommandEvent& event);
		virtual void viewMonitor(wxCommandEvent& event);
		virtual void hideAllAuiPanes(wxCommandEvent& event);
		virtual void viewAllAuiPanes(wxCommandEvent& event);
		virtual void updateCurveLibResolution(wxCommandEvent& event);
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
		
		void onThreadAppPosUpdate(UpdateManagerEvent& event);
		void onThreadCtlPosUpdate(UpdateManagerEvent& event);
		void onThreadHeartbeat(UpdateManagerEvent& event);
		void onThreadDispatchAll(UpdateManagerEvent& event);
		void onThreadCompletion(UpdateManagerEvent& event);
		
		void onPerspectiveTimer(wxTimerEvent& event);
		void onDebugUserNotificationTimer(wxTimerEvent& event);
		void configurationUpdated(wxCommandEvent& event);
		
		wxDECLARE_EVENT_TABLE();
		
	public:
		
		enum class RunConfirmationInfo {Wait, Confirmed, Canceled};
		
		//////////////////////////////////////////////////////////////////////////////////
		MainFrame(wxWindow* parent, wxFileConfig* globalConfig);
		virtual ~MainFrame();
		
		//////////////////////////////////////////////////////////////////////////////////
		void initialize(void);
		
		//////////////////////////////////////////////////////////////////////////////////
		// global trace controls
		wxTextCtrl* getLogger() 				{ return m_logger; }
		wxTextCtrl* getTrace() 					{ return m_tmpTraceInfo; }
		wxTextCtrl* getCtrlMessageHistory() 	{ return m_controllerMsgHistory; }
		wxTextCtrl* getCtrlPathGeneratorTrace() { return pathGenerator->getPathTrace(); }
		wxTextCtrl* getCtrlSerialSpy() 			{ return serialSpy; }
		
		//////////////////////////////////////////////////////////////////////////////////
		// this call is alread locked by the ConcurrentQueue class
		#define UPD_THREAD  if ( updateManagerThread == NULL ) return
		void umPostEvent(const UpdateManagerThread::Event& evt) 		{ UPD_THREAD; updateManagerThread->postEvent(evt); }
		
		//////////////////////////////////////////////////////////////////////////////////
		void selectMainBookSourcePanel();
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
		void openPathGen();
		void regenerateCurrentSvgNodeFromPopup(wxStyledTextCtrl* ctl, const wxString& node);
		void openPathGenWithCurrentSvgNodeFromPopup(wxStyledTextCtrl* ctl, const wxString& node);
		
		//////////////////////////////////////////////////////////////////////////////////
		void displayNotification(const char type, wxString title, wxString message, unsigned int timeout = 3);
		
		//////////////////////////////////////////////////////////////////////////////////
		virtual void ShowAuiToolMenu(wxAuiToolBarEvent& event);
		
		/////////////////////////////////////////////////////////////////////////////////
		// configuration callbacks
		void releaseControllerSetupFromConfig();
		void updateCncConfigTrace();
		void changeWorkpieceThickness();
		void changeCrossingThickness();
		
		//////////////////////////////////////////////////////////////////////////////////
		virtual WXLRESULT MSWWindowProc(WXUINT, WXWPARAM, WXLPARAM);
		
		//////////////////////////////////////////////////////////////////////////////////
		void waitActive(unsigned int milliseconds, bool once = true);
		void dispatchNext();
		void dispatchAll();

	protected:
 
		// will be bind to this frame
		void globalKeyDownHook(wxKeyEvent& event);
 
		// Interrupt thread handling
		UpdateManagerThread* updateManagerThread;
		wxCriticalSection pThreadCS;
		
		friend class UpdateManagerThread;
		friend class CncFileView;
		
	private:
		// Member variables
		bool isDebugMode;
		bool isZeroReferenceValid;
		bool canClose;
		bool evaluatePositions;
		bool templateFileLoading;
		bool ignoreDirControlEvents;
		
		RunConfirmationInfo  runConfirmationInfo;
		
		int traceTimerCounter;
		
		wxString lastPortName;
		wxString defaultPortName;
	
		CncControl* cnc;
		CncMotionMonitor* motionMonitor;
		CncSpyControl* serialSpy;
		CncFileView* fileView;
		CncFilePreview* mainFilePreview;
		CncFilePreview* monitorFilePreview;
		CncToolMagazine* toolMagaizne;
		CncLargeScaledListCtrl* positionSpy;
		GuiControlSetup* guiCtlSetup;
		wxFileConfig* config;
		wxFileConfig* lruStore;
		
		PathGeneratorFrame* pathGenerator;
		
		NotebookInfo* outboundNbInfo;
		NotebookInfo* templateNbInfo;

		LruFileList lruFileList;
		
		wxDateTime lastTemplateModification;
		wxDateTime lastSvgEmuModification;
		
		long processLastDuartion;
		wxDateTime processStartTime;
		wxDateTime processEndTime;
		
		wxString lastTemplateFileNameForPreview;
		
		wxPNGAnimation* pngAnimation;
		wxMenu* stcFileContentPopupMenu;
		wxMenu* stcEmuContentPopupMenu;
		FileParser* inboundFileParser;
		
		wxTimer perspectiveTimer;
		wxTimer debugUserNotificationTime;
		
		GuiControls	guiControls;
		
		wxSharedPtr<wxNotificationMessageBase> notificationDialog;
		
		void setIcons();
		
		///////////////////////////////////////////////////////////////
		// Path Generator
		void displayPGenErrorInfo(const wxString& errorInfo);
		bool verifyPathGenertorNode(wxXmlDocument& xmlDoc, const wxString& node, wxString& errorInfo);
		
		///////////////////////////////////////////////////////////////
		// File handling
		void evaluateTemplateModificationTimeStamp();
		void evaluateSvgEmuModificationTimeStamp();
		
		void introduceCurrentFile();
		
		void openPreview(CncFilePreview* ctrl, const wxString& fn);
		void openMainPreview(const wxString& fn);
		void openMonitorPreview(const wxString& fn);
		void openFileFromFileManager(const wxString& fn);
		
		const char* getSvgEmuFileName(wxString& ret);
		
		bool openFileExtern(const wxString& tool, const char* file);
		bool openFileExtern(const wxString& tool, wxString& file);
		
		const char* getBlankHtmlPage();
		const char* getErrorHtmlPage(const wxString& errorInfo);
		
		TemplateFormat getCurrentTemplateFormat(const char* fn = NULL);
		const wxString& getCurrentTemplateFileName();
		const wxString& getCurrentTemplatePathFileName();
		
		
		bool checkIfTemplateIsModified();
		
		bool openFile(int pageToSelect = -1);
		bool openTextFile();
		bool saveFile();
		bool saveTextFile();
		
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
		void updateFileContentPosition();
		void updateCurveLibResolution();
		
		void decorateSearchButton();
		
		void decoratePosSpyControl();
		void decoratePosSpyConnectButton(bool state);

		void registerGuiControls();
		bool initializeCncControl();
		void initializeUpdateManagerThread();
		bool initializeLruMenu();
		void initTemplateEditStyle();
		void initTemplateEditStyle(wxStyledTextCtrl* ctl, TemplateFormat format);
		
		void createAnimationControl();
		void createStcFileControlPopupMenu();
		void createStcEmuControlPopupMenu();
		
		int showSetReferencePositionDlg(wxString msg);
		
		///////////////////////////////////////////////////////////////
		// search handling
		int getSvgEditSearchFlags();
		void updateSvgSearchControl();
		void ensureSvgEditLineIsVisible(int line);
		void searchConditionsChanged();
		wxString& fixRegexParen(wxString& find);
		
		///////////////////////////////////////////////////////////////
		// run template handling
		bool checkIfRunCanBeProcessed();
		bool processVirtualTemplate();
		bool processTemplate();
		bool processSVGTemplate();
		bool processGCodeTemplate();
		bool processManualTemplate();
		bool processTestTemplate();
		bool processTestInterval();
		bool processTestDimensions();
		bool processControllerTestSuite();
		
		void logTimeConsumed();
		
		void determineRunMode();
		
		bool isPause()				{ return inboundFileParser != NULL ? inboundFileParser->isPause()            : false; }
		bool isProcessing() 		{ return inboundFileParser != NULL ? inboundFileParser->isProcessing()       : false; }
		
		void startDebugUserNotification();
		void stopDebugUserNotification();
		
		bool showConfigSummaryAndConfirmRun();
		
		void collectSummary();
		
		///////////////////////////////////////////////////////////////
		// control handling
		void decoratePortSelector(bool list=false);
		
		void updateSetterList();
		
		void determineCncOutputControls();
		
		void decorateRunButton();
		void enableRunControls(bool state = true);
		void disableRunControls() { enableRunControls(false); }
		
		void enableControls(bool state = true);
		void disableControls() { enableControls(false); }

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
		
		void hideAllAuiPanes();
		void viewAllAuiPanes(bool withSpy=false);
		
		bool loadPerspective(const wxString& name);
		void savePerspective(const wxString& name);
		
		void ensureRunPerspectiveMinimal();
		void ensureDebugPerspectiveMinimal();
		void ensureAllPanesFromPerspectiveAreShown(const wxString& name);
		
		void clearPositionSpy();
		
		wxWindow* getAUIPaneByName(const wxString& name);
		wxMenuItem* getAUIMenuByName(const wxString& name);
		
		void decorateViewMenu();
		
		void startAnimationControl();
		void stopAnimationControl();
		
		void freezeLogger();
		void unfreezeLogger();
		
		void prepareNewTemplateFile();
		void showSvgExtPages(bool show = true);
		void hideSvgExtPages() { showSvgExtPages(false); }
		void prepareAndShowMonitorTemplatePreview(bool force=false);
		void refreshSvgEmuFile(bool blank=false);
		void refreshSvgEmuSourceFile(bool blank=false);
		void showSVGEmuResult(bool show = true);
		void hideSVGEmuResult() { showSVGEmuResult(false); }
		
		void registerGuiControl(wxWindow* ctl); 
		void disableGuiControls();
		void enableGuiControls(bool state = true);
		
		void decorateTestSuiteParameters();
		void enableTestParameter(bool state);
		void disableTestParameter() { enableTestParameter(false); }
		
		void enableSerialSpy(bool state = true);
		void disableSerialSpy() { enableSerialSpy(false); }
		void decorateSerialSpy();
		void selectSerialSpyMode();
		
		void clearSerialSpy();
		
		void activate3DPerspectiveButton(wxButton* bt);

		///////////////////////////////////////////////////////////////
		// manually control
		void enableManuallyControls(bool force = false);
		void processTestMove(wxStaticText* axis, wxStaticText* counter, 
		                     int c, double xd, double yd, double zd);
		
		///////////////////////////////////////////////////////////////
		// misc
				
		void setZero();
		void requestReset();
		
		void navigateX(CncDirection d);
		void navigateY(CncDirection d);
		void navigateZ(CncDirection d);
		
		bool connectSerialPort();
		
		SvgOutputParameters& evaluteSvgOutputParameters(SvgOutputParameters& sop);
		
		void resetMinMaxPositions();
		void setMinMaxPositions();
		
		void setRefPostionState(bool state);
		
		void decodeSvgFragment(wxMouseEvent& event, wxStyledTextCtrl* ctl);
		
};

#endif // MAINFRAME_H
