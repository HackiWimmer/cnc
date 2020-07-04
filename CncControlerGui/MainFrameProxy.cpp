#include "MainFrame.h"
#include "MainFrameProxy.h"

	bool APP_PROXY::isAppPointerAvailable()
	{ return THE_APP != NULL; }

	void APP_PROXY::enableBtnEmergenyStop(bool state)
	{ THE_APP->GetBtnEmergenyStop()->Enable(state); }

	void APP_PROXY::enableBtnRcPause(bool state)
	{ THE_APP->GetRcPause()->Enable(state); }

	void APP_PROXY::enableBtnRcStop(bool state)
	{ THE_APP->GetRcStop()->Enable(state); }

	void APP_PROXY::enableControls(bool state)
	{ THE_APP->enableControls(state); }

	void APP_PROXY::tryToSelectClientId(long clientId, ClientIdSelSource::ID tss)
	{ THE_APP->tryToSelectClientId(clientId, tss); }

	void APP_PROXY::tryToSelectClientIds(long firstClientId, long lastClientId, ClientIdSelSource::ID tss)
	{ THE_APP->tryToSelectClientIds(firstClientId, lastClientId, tss); }

	void APP_PROXY::manualContinuousMoveStart(const CncLinearDirection x, const CncLinearDirection y, const CncLinearDirection z)
	{ THE_APP->manualContinuousMoveStart(x, y, z); }

	void APP_PROXY::manualContinuousMoveStop()
	{ THE_APP->manualContinuousMoveStop(); }

	void APP_PROXY::dispatchAll()
	{ THE_APP->dispatchAll(); }

	void APP_PROXY::dispatchNext()
	{ THE_APP->dispatchNext(); }

	void APP_PROXY::waitActive(unsigned int millis)
	{ THE_APP->waitActive(millis); }

	bool APP_PROXY::connectSerialPort()
	{ return THE_APP->connectSerialPort(); }

	bool APP_PROXY::isProcessing()
	{ return THE_APP->isProcessing(); }

	wxPoint APP_PROXY::GetScreenPosition()
	{ return THE_APP->GetScreenPosition(); }

	CncControl* APP_PROXY::getCncControl()
	{ return THE_APP->getCncControl(); }

	CncPreprocessor* APP_PROXY::getCncPreProcessor()
	{ return THE_APP->getCncPreProcessor(); }

	CncReferencePosition* APP_PROXY::getRefPositionDlg()
	{ return THE_APP->refPositionDlg; }

	wxStaticBitmap* APP_PROXY::GetGpBmp1()
	{ return THE_APP->GetGpBmp1(); }

	wxStaticBitmap* APP_PROXY::GetGpBmp2()
	{ return THE_APP->GetGpBmp2(); }

	wxStaticBitmap* APP_PROXY::GetGpBmp3()
	{ return THE_APP->GetGpBmp3(); }

	wxStaticBitmap* APP_PROXY::GetGpBmp4()
	{ return THE_APP->GetGpBmp4(); }

	wxBitmapButton* APP_PROXY::GetBtSelectReferences()
	{ return THE_APP->GetBtSelectReferences(); }

	wxRadioBox* APP_PROXY::GetRbStepSensitivity()
	{ return THE_APP->GetRbStepSensitivity(); }

	wxTextCtrl* APP_PROXY::GetSvgRootNode()
	{ return THE_APP->GetSvgRootNode(); }

	wxStaticText* APP_PROXY::GetOutboundEditMode()
	{ return THE_APP->GetOutboundEditMode(); }

	wxStaticText* APP_PROXY::GetOutboundPosition()
	{ return THE_APP->GetOutboundPosition(); }

	wxTextCtrl* APP_PROXY::GetOutboundEditStatus()
	{ return THE_APP->GetOutboundEditStatus(); }

	wxDataViewListCtrl* APP_PROXY::GetDvListCtrlSvgUADetailInfo()
	{ return THE_APP->GetDvListCtrlSvgUADetailInfo(); }

	wxDataViewListCtrl* APP_PROXY::GetDvListCtrlSvgUAInboundPathList()
	{ return THE_APP->GetDvListCtrlSvgUAInboundPathList(); }

	wxDataViewListCtrl* APP_PROXY::GetDvListCtrlSvgUAUseDirective()
	{ return THE_APP->GetDvListCtrlSvgUAUseDirective(); }

	wxSimplebook* APP_PROXY::GetMainViewBook()
	{ return THE_APP->GetMainViewBook(); }

	wxCheckBox* APP_PROXY::GetKeepFileManagerPreview()
	{ return THE_APP->GetKeepFileManagerPreview(); }

	wxMenuItem* APP_PROXY::GetMiOpenGLContextObserver()
	{ return THE_APP->m_miOpenGLContextObserver; }

	void APP_PROXY::parsingSynopsisTraceAddSeparator()
	{ THE_APP->getParsingSynopsisTrace()->addSeparator(); }

	void APP_PROXY::parsingSynopsisTraceAddEntry(const char type, const wxString& info)
	{ THE_APP->getParsingSynopsisTrace()->addEntry(type, info); }

	void APP_PROXY::parsingSynopsisTraceAddInfo(const wxString& info)
	{ THE_APP->getParsingSynopsisTrace()->addInfo(info); }

	void APP_PROXY::parsingSynopsisTraceAddWarning(const wxString& info)
	{ THE_APP->getParsingSynopsisTrace()->addWarning(info); }

	void APP_PROXY::parsingSynopsisTraceAddError(const wxString& info)
	{ THE_APP->getParsingSynopsisTrace()->addError(info); }

	int APP_PROXY::showReferencePositionDlg(wxString msg)
	{ return THE_APP->showReferencePositionDlg(msg); }

	void APP_PROXY::releaseControllerSetupFromConfig()
	{ THE_APP->releaseControllerSetupFromConfig(); }

	void APP_PROXY::changeCrossingThickness()
	{ THE_APP->changeCrossingThickness(); }

	void APP_PROXY::prepareMotionMonitorViewType()
	{ THE_APP->prepareMotionMonitorViewType(); }

	void APP_PROXY::openMainPreview(const wxString& fn)
	{ THE_APP->openMainPreview(fn); }

	void APP_PROXY::openMonitorPreview(const wxString& fn)
	{ THE_APP->openMonitorPreview(fn); }

	void APP_PROXY::openFileFromFileManager(const wxString& fn)
	{ THE_APP->openFileFromFileManager(fn); }

	void APP_PROXY::selectMainBookSourcePanel(int sourcePageToSelect)
	{ THE_APP->selectMainBookSourcePanel(sourcePageToSelect); }

	void APP_PROXY::newTemplate()
	{ wxCommandEvent dummy; THE_APP->newTemplate(dummy); }

	void APP_PROXY::openTemplate()
	{ wxCommandEvent dummy; THE_APP->openTemplate(dummy); }

	void APP_PROXY::decorateProbeMode(bool state)
	{ THE_APP->decorateProbeMode(state); }

	wxFileConfig* APP_PROXY::getLruStore()
	{ return THE_APP->lruStore; }
	
	bool APP_PROXY::filePreviewListLeave()
	{ return THE_APP->filePreviewListLeave(); }
	
	SerialThread* APP_PROXY::getSerialThread(SerialThreadStub* sts)
	{ return THE_APP->getSerialThread(sts); }
