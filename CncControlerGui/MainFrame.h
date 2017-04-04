#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "wxcrafter.h"
#include <wx/notifmsg.h>
#include <wx/generic/notifmsg.h>
#include "NotebookInfo.h"
#include "LruFileList.h"
#include "SVGFileParser.h"
#include "PathGeneratorFrame.h"
#include "InterruptThread.h"
#include "CncControl.h"
#include "CncDrawPane.h"
#include "CncSpyControl.h"
#include "codelite/wxPNGAnimation.h"

// declare a new type of event, to be used by our MyThread class:
wxDECLARE_EVENT(wxEVT_COMMAND_MYTHREAD_COMPLETED, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_MYTHREAD_UPDATE, wxThreadEvent);

class wxFileConfig;
class wxMenu;
class wxMenuItem;

enum class RunConfirmationInfo {Wait, Confirmed, Canceled};

////////////////////////////////////////////////////////////////////
typedef std::vector<wxWindow*> GuiControls;

////////////////////////////////////////////////////////////////////
class MainFrame : public MainFrameBClass {
	
	public:
		MainFrame(wxWindow* parent);
		virtual ~MainFrame();
		
		void initialize(void);
		wxTextCtrl* getLogger() { return m_logger; }
		wxTextCtrl* getTrace() { return m_tmpTraceInfo; }
		wxTextCtrl* getCtrlMessageHistory() { return m_controllerMsgHistory; }
		wxTextCtrl* getCtrlSerialSpy() { return serialSpy; }
		
		void install3DPane();
		void installSypControl();
		
		void displayNotification(const char type, wxString title, wxString message, unsigned int timeout = 3);

protected:
    virtual void selectPreconfiguredSpeedSetups(wxCommandEvent& event);
	virtual void requestErrorCount(wxCommandEvent& event);
    virtual void paintXAxisMarkerBottom(wxPaintEvent& event);
    virtual void paintXAxisMarkerTop(wxPaintEvent& event);
    virtual void paintYAxisMarker(wxPaintEvent& event);
    virtual void paintDrawPaneWindow(wxPaintEvent& event);
    virtual void UpdateLogger(wxCommandEvent& event);
 
		// will be bind to this frame
		void globalKeyDownHook(wxKeyEvent& event);
 
		// Interrupt thread handling
		InterruptThread* pThread;
		wxCriticalSection pThreadCS;
		friend class InterruptThread;
		
		
	private:
		// Member variables
		bool isCncControlInitialized;
		bool isZeroReferenceValid;
		bool canClose;
		bool evaluatePositions;
		bool svgDebugger;
		bool templateFileLoading;
		bool ignoreDirControlEvents;
		
		RunConfirmationInfo  runConfirmationInfo;
		
		int traceTimerCounter;
		
		wxString lastPortName;
	
		CncControl* cnc;
		CncOpenGLDrawPane* drawPane3D;
		CncSpyControl* serialSpy;
		GuiControlSetup* guiCtlSetup;
		wxFileConfig* config;
		wxFileConfig* lruStore;
		
		PathGeneratorFrame* pathGenerator;
		
		NotebookInfo* outboundNbInfo;
		NotebookInfo* templateNbInfo;

		LruFileList lruFileList;
		
		wxDateTime lastTemplateModification;
		wxDateTime lastSvgEmuModification;
		wxDateTime processStartTime;
		wxDateTime processEndTime;
		
		wxString lastTemplateFileNameForPreview;
		
		wxPNGAnimation* pngAnimation;
		wxMenu* stcFileContentPopupMenu;
		wxMenu* stcEmuContentPopupMenu;
		SVGFileParser* svgFileParser;
		
		wxTimer perspectiveTimer;
		
		GuiControls	guiControls;
		
		wxSharedPtr<wxNotificationMessageBase> notificationDialog;
		
		///////////////////////////////////////////////////////////////
		// File handling
		void evaluateTemplateModificationTimeStamp();
		void evaluateSvgEmuModificationTimeStamp();
		
		void introduceCurrentFile();
		
		void openSvgPreview(const wxString& fn, TemplateFormat format);
		void updateSvgPreview(const wxString& fn);
		void openFileFromFileManager(const wxString& fn);
		void highlightTplPreview(bool state);
		const char* getSvgEmuFileName(wxString& ret);
		
		bool openFileExtern(wxString& tool, const char* file);
		bool openFileExtern(wxString& tool, wxString& file);
		
		const char* getBlankHtmlPage();
		const char* getErrorHtmlPage(const wxString& errorInfo);
		
		TemplateFormat getCurrentTemplateFormat(const char* fn = NULL);
		
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
		void updateSpeedValues();
		void updateFileContentPosition();
		void updateCurveLibResolution();
		
		void decorateSearchButton();
		void decorateTemplateListBook();

		void registerGuiControls();
		bool initializeCncControl();
		bool initializeLruMenu();
		void initTemplateEditStyle();
		void initTemplateEditStyle(wxStyledTextCtrl* ctl, TemplateFormat format);
		
		void buildCommandDurationToolTip();
		
		void createAnimationControl();
		void createStcFileControlPopupMenu();
		void createStcEmuControlPopupMenu();
		
		int showSetReferencePositionDlg(wxString msg);
		
		void initializePreconfiguredSpeedSetups();
		
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
		void processTemplate();
		bool processSVGTemplate();
		bool processGCodeTemplate();
		bool processTextTemplate();
		bool processManualTemplate();
		bool processTestTemplate();
		bool processTestInterval();
		bool processTestDimensions();
		bool processControllerTestSuite();
		
		void determineRunMode();
		
		bool isPause() { return svgFileParser != NULL ? svgFileParser->isPause() : false; }
		
		bool showConfigSummaryAndConfirmRun();
		
		void collectSummary();
		void updateCncConfigTrace();
		
		///////////////////////////////////////////////////////////////
		// control handling
		void determineCncOutputControls();
		
		void decorateRunButton();
		void enableRunControls(bool state = true);
		void enableAllRunControls(bool state = true);
		void disableRunControls() { enableRunControls(false); }
		void disableAllRunControls() { enableAllRunControls(false); }
		
		void enableControls(bool state = true);
		void disableControls() { enableControls(false); }

		void enableMenu(wxMenu* m, bool state = true);
		void disableMenu(wxMenu* m) { enableMenu(m, false); }
		
		void enableMenuItem(wxMenuItem* m, bool state = true);
		void disableMenuItem(wxMenuItem* m) { enableMenuItem(m, false); }

		void toggleAuiPane(wxWindow* pane, wxMenuItem* menu);
		void showAuiPane(wxWindow* pane, wxMenuItem* menu);
		void hideAuiPane(wxWindow* pane, wxMenuItem* menu);
		
		void toggleAuiPane(const wxString& name);
		void showAuiPane(const wxString& name);
		void hideAuiPane(const wxString& name);
		
		void hideAllAuiPanes();
		void viewAllAuiPanes();
		
		wxWindow* getAUIPaneByName(const wxString& name);
		wxMenuItem* getAUIMenuByName(const wxString& name);
		
		void startAnimationControl();
		void stopAnimationControl();
		
		void freezeLogger();
		void unfreezeLogger();
		
		void prepareNewTemplateFile();
		void showTplPreview(bool show = true);
		void hideTplPreview() { showTplPreview(false); }
		void prepareTplPreview(bool force=false);
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
		
		void freezeSerialSpy(bool state = true); 
		void unfreezeSerialSpy() { freezeSerialSpy(false); }
		void clearSerialSpy();

		///////////////////////////////////////////////////////////////
		// manually control
		void enableManuallyControls(bool force = false);
		void processTestMove(wxStaticText* axis, wxStaticText* counter, 
		                     int c, double xd, double yd, double zd);
		
		///////////////////////////////////////////////////////////////
		// misc
				
		CoordinateSytemType getCoordinateSytemType();
		void setCoordinateSystemType();
		double getDrawPaneFactor();
		
		void dispatch();
		
		void setZero();
		
		void manuallyDissolveEndSwitchDlg();
		
		void requestReset();
		
		void selectSvgDebuggerInfo(wxDataViewListCtrl* ctl);

		void navigateX(CncDirection d);
		void navigateY(CncDirection d);
		void navigateZ(CncDirection d);
		
		bool connectSerialPort();
		
		SvgOutputParameters& evaluteSvgOutputParameters(SvgOutputParameters& sop);
		
		void resetMinMaxPositions();
		void setMinMaxPositions();
		
		void setRefPostionState(bool state);
		
		void setDisplayAngels3D();
		
	protected:
		// User command
		virtual void perspectiveDefault(wxCommandEvent& event);
		virtual void perspectiveRun(wxCommandEvent& event);
		virtual void perspectiveTemplate(wxCommandEvent& event);
		virtual void cfgStepDelayDropDown(wxAuiToolBarEvent& event);
		virtual void cfgStepDelayArduino(wxCommandEvent& event);
		virtual void cfgStepDelayMax(wxCommandEvent& event);
		virtual void cfgStepDelayMin(wxCommandEvent& event);
		virtual void enableSerialSpy(wxCommandEvent& event);
		virtual void clearSerialSpy(wxCommandEvent& event);
		virtual void freezeSerialSpy(wxCommandEvent& event);
		virtual void freezeLogger(wxCommandEvent& event);
		virtual void show3DPaneHelp(wxCommandEvent& event);
		virtual void selectedPlane3D(wxCommandEvent& event);
		virtual void spin3DAngelX(wxSpinEvent& event);
		virtual void spin3DAngelY(wxSpinEvent& event);
		virtual void spin3DAngelZ(wxSpinEvent& event);
		virtual void update3DAngelX(wxCommandEvent& event);
		virtual void update3DAngelY(wxCommandEvent& event);
		virtual void update3DAngelZ(wxCommandEvent& event);
		virtual void nootebookConfigChanged(wxListbookEvent& event);
		virtual void cancelRun(wxCommandEvent& event);
		virtual void confirmRun(wxCommandEvent& event);
		virtual void clearControllerMsgHistory(wxCommandEvent& event);
		virtual void requestControllerPinsFromButton(wxCommandEvent& event);
		virtual void requestPins(wxCommandEvent& event);
		virtual void traceTextUpdated(wxCommandEvent& event);
		virtual void ctrlTestCaseSelected(wxCommandEvent& event);
		virtual void treebookTestChanged(wxTreebookEvent& event);
		virtual void controlerPause(wxCommandEvent& event);
		virtual void openPyCam(wxCommandEvent& event);
		virtual void moveXToMid(wxCommandEvent& event);
		virtual void moveYToMid(wxCommandEvent& event);
		virtual void moveZToMid(wxCommandEvent& event);
   		virtual void testDimTakeOverAll(wxCommandEvent& event);
		virtual void testDimTakeOverX(wxCommandEvent& event);
		virtual void testDimTakeOverY(wxCommandEvent& event);
		virtual void testDimTakeOverZ(wxCommandEvent& event);
		virtual void thumbRelease3D(wxScrollWinEvent& event);
		virtual void thumbtrack3D(wxScrollWinEvent& event);
		virtual void refresh3D(wxCommandEvent& event);
		virtual void animate3D(wxCommandEvent& event);
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
		virtual void rcSelectRunMode(wxCommandEvent& event);
		virtual void togglePreviewErrorInfo(wxCommandEvent& event);
		virtual void rcFinish(wxCommandEvent& event);
		virtual void rcNextPath(wxCommandEvent& event);
		virtual void rcNextStep(wxCommandEvent& event);
		virtual void rcPause(wxCommandEvent& event);
		virtual void rcReset(wxCommandEvent& event);
		virtual void rcRun(wxCommandEvent& event);
		virtual void rcStop(wxCommandEvent& event);
		virtual void switchMonitoring(wxCommandEvent& event);
		virtual void fileContentChange(wxStyledTextEvent& event);
		virtual void switchCoordinateSystemTypeSvgOut(wxCommandEvent& event);
		virtual void viewToolbar(wxCommandEvent& event);
		virtual void renderAuiPane(wxAuiManagerEvent& event);
		virtual void restoreAuiPane(wxAuiManagerEvent& event);
		virtual void maximizeAuiPane(wxAuiManagerEvent& event);
		virtual void selectGridPosUnit(wxCommandEvent& event);
		virtual void switchCoordinateSystemType(wxCommandEvent& event);
		virtual void fileContentKeyUp(wxKeyEvent& event);
		virtual void saveTemplateFromButton(wxCommandEvent& event);
		virtual void marginClickEmuSource(wxStyledTextEvent& event);
		virtual void marginClickFileContent(wxStyledTextEvent& event);
		virtual void emuContentRightDown(wxMouseEvent& event);
		virtual void toggleEmuWordWrapMode(wxCommandEvent& event);
		virtual void toggleTemplateWordWrapMode(wxCommandEvent& event);
		virtual void copySvgTrace(wxCommandEvent& event);
		virtual void copyLogger(wxCommandEvent& event);
		virtual void OpenXmlTrace(wxCommandEvent& event);
		virtual void openXMLTraceAsText(wxCommandEvent& event);
		virtual void closeAuiPane(wxAuiManagerEvent& evt);
		virtual void gridDrawPane(wxCommandEvent& event);
		virtual void traceDrawPane(wxCommandEvent& event);
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
		virtual void onLeaveDrawPane(wxMouseEvent& event);
		virtual void onMotionDrawPane(wxMouseEvent& event);
		virtual void updateFlySpeedXY(wxCommandEvent& event);
		virtual void updateWorkSpeedXY(wxCommandEvent& event);
		virtual void updateWorkSpeedZ(wxCommandEvent& event);
		virtual void configureXYSpeedWithZValues(wxCommandEvent& event);
		virtual void configureZSpeedWithXYValues(wxCommandEvent& event);
		virtual void updateCurrentSpeedZ(wxCommandEvent& event);
		virtual void updateFlySpeedZ(wxCommandEvent& event);
		virtual void updateCurrentSpeedXY(wxCommandEvent& event);
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
		virtual void clearDrawPane(wxCommandEvent& event);
		virtual void zoomDrawPane(wxCommandEvent& event);
		virtual void changeUpdateInterval(wxCommandEvent& event);
		virtual void emergencyStop(wxCommandEvent& event);
		virtual void defineDebugSerial(wxCommandEvent& event);
		virtual void defineMinMonitoring(wxCommandEvent& event);
		virtual void defineNormalMonitoring(wxCommandEvent& event);
		virtual void saveTemplate(wxCommandEvent& event);
		virtual void saveTemplateAs(wxCommandEvent& event);
		virtual void mainBookPageChanged(wxNotebookEvent& event);
		virtual void mainBookPageChanging(wxNotebookEvent& event);
		virtual void newTemplate(wxCommandEvent& event);
		virtual void openTemplate(wxCommandEvent& event);
		virtual void reloadTemplate(wxCommandEvent& event);
		virtual void openTemplateSourceExtern(wxCommandEvent& event);
		virtual void openTemplateSvgExtern(wxCommandEvent& event);
		virtual void reloadTemplateFromButton(wxCommandEvent& event);
		virtual void defineUpdateCoordinates(wxCommandEvent& event);
		virtual void defineAllowEvents(wxCommandEvent& event);
		virtual void defineOnlineDrawing(wxCommandEvent& event);
		virtual void onPaintDrawPane(wxPaintEvent& event);
		virtual void mvSpinDownY(wxSpinEvent& event);
		virtual void mvSpinDownZ(wxSpinEvent& event);
		virtual void mvSpinUpZ(wxSpinEvent& event);
		virtual void mvSpinDownX(wxSpinEvent& event);
		virtual void mvSpinUpX(wxSpinEvent& event);
		virtual void mvSpinUpY(wxSpinEvent& event);
		virtual void updateInclWpt(wxCommandEvent& event);
		virtual void killFocusWorkpieceThickness(wxFocusEvent& event);
		virtual void killFocusCrossingThickness(wxFocusEvent& event);
		virtual void killFocusMaxDimensionX(wxFocusEvent& event);
		virtual void killFocusMaxDimensionY(wxFocusEvent& event);
		virtual void killFocusMaxDimensionZ(wxFocusEvent& event);
		virtual void killFocusMaxSpeedXY(wxFocusEvent& event);
		virtual void killFocusMaxSpeedZ(wxFocusEvent& event);
		virtual void killFocusRouterDiameter(wxFocusEvent& event);
		virtual void killFocusReplyThreshold(wxFocusEvent& event);
		virtual void svgEmuOpenFileAsSource(wxCommandEvent& event);
		virtual void svgEmuOpenFileAsSvg(wxCommandEvent& event);
		virtual void svgEmuReload(wxCommandEvent& event);
		virtual void selectUnit(wxCommandEvent& event);
		virtual void setZero(wxCommandEvent& event);
		virtual void clearLogger(wxCommandEvent& event);
		virtual void connect(wxCommandEvent& event);
		virtual void reinit(wxCommandEvent& event);
		virtual void clearProcessedSetterList(wxCommandEvent& event);
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
		virtual void dissolveEndSwitchStates(wxCommandEvent& event);
		virtual void updateReverseStepSignX(wxCommandEvent& event);
		virtual void updateReverseStepSignY(wxCommandEvent& event);
		virtual void killFocusMoveZAxis(wxFocusEvent& event);
		virtual void killFocusMoveXYAxis(wxFocusEvent& event);
		virtual void setFocusMoveXYAxis(wxFocusEvent& event);
		virtual void setFocusMoveZAxis(wxFocusEvent& event);
		virtual void dirCtrlActivated(wxTreeEvent& event);
		virtual void dirCtrlChanged(wxTreeEvent& event);
		virtual void selectCurrentFile(wxCommandEvent& event);
		virtual void selectDefaultDirectory(wxCommandEvent& event);
		virtual void leaveLruList(wxMouseEvent& event);
		virtual void lruListItemActivated(wxCommandEvent& event);
		virtual void lruListItemSelected(wxCommandEvent& event);
		virtual void leaveEnterFileManagerControl(wxMouseEvent& event);
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
		virtual void clearDebugControls();
		virtual void disableSlider(wxMouseEvent& event);
		virtual void selectSvgDebuggerInfoBase(wxDataViewEvent& event);
		virtual void selectSvgDebuggerInfoPath(wxDataViewEvent& event);
		virtual void selectSvgDebuggerInfoDetail(wxDataViewEvent& event);
		virtual void OnExit(wxCommandEvent& event);
		virtual void OnAbout(wxCommandEvent& event);
		virtual void OnClose(wxCloseEvent& event);
		virtual void OnThreadUpdate(wxThreadEvent& event) {}
		virtual void OnThreadCompletion(wxThreadEvent& event) {}
		virtual void OnPerspectiveTimer(wxTimerEvent& WXUNUSED(event));
		wxDECLARE_EVENT_TABLE();
};

#endif // MAINFRAME_H
