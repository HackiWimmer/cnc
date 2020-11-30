#ifndef CNCCONTROLERGUI_MAINFRAMEPROXY_H_
#define CNCCONTROLERGUI_MAINFRAMEPROXY_H_

#include <wx/gdicmn.h>
#include <wx/fileconf.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/dataview.h>
#include <wx/radiobox.h>
#include <wx/checkbox.h>
#include <wx/bmpbuttn.h>
#include <wx/simplebook.h>
#include <wx/menuitem.h>
#include "CncCommon.h"

// ----------------------------------------------------------------
// This class establish a proxy between the MainFrame and the
// the rest of the program cosmos.
// This will be done to save compile time because the MainFrame.h
// have not to be included everywhere
// ----------------------------------------------------------------

class CncPreprocessor;
class CncReferencePosition;
class CncMotionMonitor;
class CncControl;
class SerialThread;
class SerialThreadStub;

class MainFrameProxy {

	public:

		static void enableBtnEmergenyStop(bool state);
		static void enableBtnRcPause(bool state);
		static void enableBtnRcStop(bool state);
		static void enableControls(bool state);

		static void tryToSelectClientId(long clientId, ClientIdSelSource::ID tss);
		static void tryToSelectClientIds(long firstClientId, long lastClientId, ClientIdSelSource::ID tss);

		static void postEvent(wxEvent* evt);
		
		static void dispatchAll();
		static void dispatchNext();
		static void waitActive(unsigned int millis);

		static bool isAppPointerAvailable();
		static bool connectSerialPort();
		static bool isProcessing();

		static void parsingSynopsisTraceAddSeparator(const wxString& entry);
		static void parsingSynopsisTraceAddEntry(const char type, const wxString& entry);
		static void parsingSynopsisTraceAddInfo(const wxString& info);
		static void parsingSynopsisTraceAddWarning(const wxString& entry);
		static void parsingSynopsisTraceAddError(const wxString& entry);
		static bool parsingSynopsisTraceHasDebugEntries();
		static bool parsingSynopsisTraceHasWarnEntries();
		static bool parsingSynopsisTraceHasErrorEntries();
		
		static bool startStepwiseMovement(CncLinearDirection x, CncLinearDirection y, CncLinearDirection z);
		static bool startInteractiveMove(CncInteractiveMoveDriver imd);
		static bool updateInteractiveMove();
		static bool updateInteractiveMove(const CncLinearDirection x, const CncLinearDirection y, const CncLinearDirection z);
		static bool stopInteractiveMove();
		
		static void releaseControllerSetupFromConfig();
		static void changeCrossingThickness();
		static void prepareMotionMonitorViewType();
		static void openMainPreview(const wxString& fn);
		static void openMonitorPreview(const wxString& fn);
		static void openFileFromFileManager(const wxString& fn);
		static void selectMainBookSourcePanel(int sourcePageToSelect);
		static void decorateProbeMode(bool probeMode);
		static void decorateSpeedControl(bool probeMode);
		static int  showReferencePositionDlg(wxString msg);
		static void newTemplate();
		static void openTemplate();
		static void openNavigatorFromGamepad();
		static bool filePreviewListLeave();
		static void updateCncSpeed(float value, CncSpeedMode mode);
		static void updateSpeedSlider(float value);
		static void updateAndSetSpeedSlider(float value);
		
		static SerialThread* getSerialThread(SerialThreadStub* sts);
		
		static wxFileConfig*			getLruStore();
		
		static wxPoint 					GetScreenPosition();

		static wxStaticText* 			GetOutboundEditMode();
		static wxStaticText* 			GetOutboundPosition();

		static wxTextCtrl* 				GetSvgRootNode();
		static wxTextCtrl* 				GetOutboundEditStatus();

		static wxBitmapButton* 			GetBtSelectReferences();

		static wxDataViewListCtrl* 		GetDvListCtrlSvgUADetailInfo();
		static wxDataViewListCtrl* 		GetDvListCtrlSvgUAInboundPathList();
		static wxDataViewListCtrl* 		GetDvListCtrlSvgUAUseDirective();

		static wxRadioBox* 				GetRbStepSensitivity();
		static wxRadioBox* 				GetRbStepMode();
		
		static wxSimplebook* 			GetMainViewBook();
		static wxCheckBox* 				GetKeepFileManagerPreview();
		
		static wxMenuItem* 				GetMiOpenGLContextObserver();

		static CncControl* 				getCncControl();
		static CncPreprocessor* 		getCncPreProcessor();
		static CncReferencePosition*	getRefPositionDlg();
		static CncMotionMonitor*		getMotionMonitor();

	private:
		MainFrameProxy() {}
		~MainFrameProxy() {}
};

typedef MainFrameProxy APP_PROXY;


#endif /* CNCCONTROLERGUI_MAINFRAMEPROXY_H_ */
