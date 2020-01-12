#include "CncArduino.h"
#include "CncArduinoPinsListCtrl.h"
#include "CncMessageListCtrl.h"
#include "GlobalFunctions.h"
#include "SerialThread.h"
#include "SerialAdminChannel.h"
#include "../Arduino/StepperEnvironment/ArdoEnv.h"
#include "wxCrafterImages.h"
#include "CncArduinoEnvironment.h"

///////////////////////////////////////////////////////////////////
CncArduinoEnvironment::CncArduinoEnvironment(wxWindow* parent)
: CncArduinoEnvironmentBase(parent)
, reConnectRequired(false)
, showDebugMessages(true)
, canClose(false)
, pinList (NULL)
, logger  (NULL)
, lsOn    (NULL)
, lsOff   (NULL)
, lsiMinX (m_btMinX,   'D', PIN_X_LIMIT, -1)
, lsiMaxX (m_btMaxX,   'D', PIN_X_LIMIT, -1)
, lsiMinY (m_btMinY,   'D', PIN_Y_LIMIT, -1)
, lsiMaxY (m_btMaxY,   'D', PIN_Y_LIMIT, -1)
, lsiMinZ (m_btMinZ,   'D', PIN_Z_LIMIT, -1)
, lsiMaxZ (m_btMaxZ,   'D', PIN_Z_LIMIT, -1)
, ssiBit8 (m_btSSBit8, 'B', -1)
, ssiBit7 (m_btSSBit7, 'B', -1)
, ssiBit6 (m_btSSBit6, 'B', -1)
, ssiBit5 (m_btSSBit5, 'B', -1)
, ssiBit4 (m_btSSBit4, 'S', -1)
, ssiBit3 (m_btSSBit3, 'S', -1)
, ssiBit2 (m_btSSBit2, 'S', -1)
, ssiBit1 (m_btSSBit1, 'S', -1)
, limitStates  ('\0')
, supportStates('\0')
///////////////////////////////////////////////////////////////////
{
	// pin list  control
	pinList = new CncArduinoPinsListCtrl(this, wxLC_SINGLE_SEL|wxALWAYS_SHOW_SB|wxVSCROLL ); 
	GblFunc::replaceControl(m_pinListPlaceholder, pinList);
	
	// logger control
	logger = new CncMessageListCtrl(this, wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_loggerPlaceholder, logger);
	
	wxBitmap bmp = ImageLibSwitch().Bitmap("BMP_TOGGLE_SWITCH_1");
	
	wxRect rectOn;
	rectOn.SetX(0);								rectOn.SetY(0);
	rectOn.SetWidth ( bmp.GetWidth()     );		rectOn.SetHeight( bmp.GetHeight() / 2 );

	wxRect rectOff;
	rectOff.SetX(0);							rectOff.SetY(bmp.GetHeight() / 2);
	rectOff.SetWidth ( bmp.GetWidth()     );	rectOff.SetHeight( bmp.GetHeight() / 2 );
	
	lsOn  = new wxBitmap(bmp.GetSubBitmap(rectOn));
	lsOff = new wxBitmap(bmp.GetSubBitmap(rectOff));
	irOn  = new wxBitmap(m_btEmergency->GetBitmap().ConvertToImage().Rotate90());
	irOff = new wxBitmap(m_btEmergency->GetBitmap().ConvertToImage());
	
	m_btEmergency->SetValue(false); decorateEmergencySwitch();
	
	// deco
	auto setupBtn = [&](wxButton* btn, const wxBitmap& bmp, bool enable) {
		btn->SetBitmap(bmp);
		btn->SetBitmapDisabled(btn->GetBitmap().ConvertToDisabled()); 
		btn->Enable(enable); 
	};
	
	setupBtn(lsiMinX.button, *lsOff, true); 
	setupBtn(lsiMaxX.button, *lsOff, true);
	setupBtn(lsiMinY.button, *lsOff, true);
	setupBtn(lsiMaxY.button, *lsOff, true);
	setupBtn(lsiMinZ.button, *lsOff, true);
	setupBtn(lsiMaxZ.button, *lsOff, true);

	setupBtn(ssiBit8.button, *lsOff, true);
	setupBtn(ssiBit7.button, *lsOff, true);
	setupBtn(ssiBit6.button, *lsOff, true);
	setupBtn(ssiBit5.button, *lsOff, false);
	setupBtn(ssiBit4.button, *lsOff, true);
	setupBtn(ssiBit3.button, *lsOff, true);
	setupBtn(ssiBit2.button, *lsOff, false);
	setupBtn(ssiBit1.button, *lsOff, false);
	
	// events
	lsiMinX.button->Connect(wxEVT_LEFT_DOWN, 		wxMouseEventHandler(CncArduinoEnvironment::onLsLeftDown), 		NULL, this);
	lsiMinX.button->Connect(wxEVT_LEFT_UP, 			wxMouseEventHandler(CncArduinoEnvironment::onLsLeftUp), 		NULL, this);
	lsiMaxX.button->Connect(wxEVT_LEFT_DOWN, 		wxMouseEventHandler(CncArduinoEnvironment::onLsLeftDown), 		NULL, this);
	lsiMaxX.button->Connect(wxEVT_LEFT_UP, 			wxMouseEventHandler(CncArduinoEnvironment::onLsLeftUp), 		NULL, this);

	lsiMinY.button->Connect(wxEVT_LEFT_DOWN, 		wxMouseEventHandler(CncArduinoEnvironment::onLsLeftDown), 		NULL, this);
	lsiMinY.button->Connect(wxEVT_LEFT_UP, 			wxMouseEventHandler(CncArduinoEnvironment::onLsLeftUp), 		NULL, this);
	lsiMaxY.button->Connect(wxEVT_LEFT_DOWN, 		wxMouseEventHandler(CncArduinoEnvironment::onLsLeftDown), 		NULL, this);
	lsiMaxY.button->Connect(wxEVT_LEFT_UP, 			wxMouseEventHandler(CncArduinoEnvironment::onLsLeftUp), 		NULL, this);
	
	lsiMinZ.button->Connect(wxEVT_LEFT_DOWN, 		wxMouseEventHandler(CncArduinoEnvironment::onLsLeftDown), 		NULL, this);
	lsiMinZ.button->Connect(wxEVT_LEFT_UP, 			wxMouseEventHandler(CncArduinoEnvironment::onLsLeftUp), 		NULL, this);
	lsiMaxZ.button->Connect(wxEVT_LEFT_DOWN, 		wxMouseEventHandler(CncArduinoEnvironment::onLsLeftDown), 		NULL, this);
	lsiMaxZ.button->Connect(wxEVT_LEFT_UP, 			wxMouseEventHandler(CncArduinoEnvironment::onLsLeftUp), 		NULL, this);

	ssiBit8.button->Connect(wxEVT_LEFT_DOWN, 		wxMouseEventHandler(CncArduinoEnvironment::onSptBtnLeftDown1), 	NULL, this);
	ssiBit8.button->Connect(wxEVT_LEFT_UP, 			wxMouseEventHandler(CncArduinoEnvironment::onSptBtnLeftUp1), 	NULL, this);
	ssiBit7.button->Connect(wxEVT_LEFT_DOWN, 		wxMouseEventHandler(CncArduinoEnvironment::onSptBtnLeftDown2), 	NULL, this);
	ssiBit7.button->Connect(wxEVT_LEFT_UP, 			wxMouseEventHandler(CncArduinoEnvironment::onSptBtnLeftUp2), 	NULL, this);
	ssiBit6.button->Connect(wxEVT_LEFT_DOWN, 		wxMouseEventHandler(CncArduinoEnvironment::onSptBtnLeftDown3), 	NULL, this);
	ssiBit6.button->Connect(wxEVT_LEFT_UP, 			wxMouseEventHandler(CncArduinoEnvironment::onSptBtnLeftUp3), 	NULL, this);

	ssiBit4.button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CncArduinoEnvironment::onSupportSwt1), NULL, this);
	ssiBit3.button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CncArduinoEnvironment::onSupportSwt2), NULL, this);
	
	// initial values
	initValues();
	enableControls(false, true);
	
	
	m_bmpDirectionX->SetBitmap(ImageLibStepper().Bitmap("BMP_DIR_CCW"));
	m_bmpDirectionY->SetBitmap(ImageLibStepper().Bitmap("BMP_DIR_CCW"));
	m_bmpDirectionZ->SetBitmap(ImageLibStepper().Bitmap("BMP_DIR_CCW"));
	
	m_posStepperX->ChangeValue("0");
	m_posStepperY->ChangeValue("0");
	m_posStepperZ->ChangeValue("0");
	
	m_pgPropTraceGetters->SetValue(false);
	m_pgPropTraceSetters->SetValue(false);
}
///////////////////////////////////////////////////////////////////
CncArduinoEnvironment::~CncArduinoEnvironment() {
///////////////////////////////////////////////////////////////////
	lsiMinX.button->Disconnect(wxEVT_LEFT_DOWN, 	wxMouseEventHandler(CncArduinoEnvironment::onLsLeftDown), 		NULL, this);
	lsiMinX.button->Disconnect(wxEVT_LEFT_UP, 		wxMouseEventHandler(CncArduinoEnvironment::onLsLeftUp), 		NULL, this);
	lsiMaxX.button->Disconnect(wxEVT_LEFT_DOWN, 	wxMouseEventHandler(CncArduinoEnvironment::onLsLeftDown), 		NULL, this);
	lsiMaxX.button->Disconnect(wxEVT_LEFT_UP, 		wxMouseEventHandler(CncArduinoEnvironment::onLsLeftUp), 		NULL, this);

	lsiMinY.button->Disconnect(wxEVT_LEFT_DOWN, 	wxMouseEventHandler(CncArduinoEnvironment::onLsLeftDown), 		NULL, this);
	lsiMinY.button->Disconnect(wxEVT_LEFT_UP, 		wxMouseEventHandler(CncArduinoEnvironment::onLsLeftUp), 		NULL, this);
	lsiMaxY.button->Disconnect(wxEVT_LEFT_DOWN, 	wxMouseEventHandler(CncArduinoEnvironment::onLsLeftDown), 		NULL, this);
	lsiMaxY.button->Disconnect(wxEVT_LEFT_UP, 		wxMouseEventHandler(CncArduinoEnvironment::onLsLeftUp), 		NULL, this);

	lsiMinZ.button->Disconnect(wxEVT_LEFT_DOWN, 	wxMouseEventHandler(CncArduinoEnvironment::onLsLeftDown), 		NULL, this);
	lsiMinZ.button->Disconnect(wxEVT_LEFT_UP, 		wxMouseEventHandler(CncArduinoEnvironment::onLsLeftUp), 		NULL, this);
	lsiMaxZ.button->Disconnect(wxEVT_LEFT_DOWN, 	wxMouseEventHandler(CncArduinoEnvironment::onLsLeftDown), 		NULL, this);
	lsiMaxZ.button->Disconnect(wxEVT_LEFT_UP, 		wxMouseEventHandler(CncArduinoEnvironment::onLsLeftUp), 		NULL, this);
	 
	ssiBit8.button->Disconnect(wxEVT_LEFT_DOWN, 	wxMouseEventHandler(CncArduinoEnvironment::onSptBtnLeftDown1), 	NULL, this);
	ssiBit8.button->Disconnect(wxEVT_LEFT_UP, 		wxMouseEventHandler(CncArduinoEnvironment::onSptBtnLeftUp1), 	NULL, this);
	ssiBit7.button->Disconnect(wxEVT_LEFT_DOWN, 	wxMouseEventHandler(CncArduinoEnvironment::onSptBtnLeftDown2), 	NULL, this);
	ssiBit7.button->Disconnect(wxEVT_LEFT_UP, 		wxMouseEventHandler(CncArduinoEnvironment::onSptBtnLeftUp2), 	NULL, this);
	ssiBit6.button->Disconnect(wxEVT_LEFT_DOWN, 	wxMouseEventHandler(CncArduinoEnvironment::onSptBtnLeftDown3), 	NULL, this);
	ssiBit6.button->Disconnect(wxEVT_LEFT_UP, 		wxMouseEventHandler(CncArduinoEnvironment::onSptBtnLeftUp3), 	NULL, this);

	ssiBit4.button->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CncArduinoEnvironment::onSupportSwt1), NULL, this);
	ssiBit3.button->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CncArduinoEnvironment::onSupportSwt2), NULL, this);

	wxDELETE( pinList );
	wxDELETE( logger  );
	wxDELETE( lsOn    );
	wxDELETE( lsOff   );
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::initValues() {
///////////////////////////////////////////////////////////////////
	reConnectRequired = false;

	lsiMinX.setValueOff();
	lsiMaxX.setValueOff();
	lsiMinY.setValueOff();
	lsiMaxY.setValueOff();
	lsiMinZ.setValueOff();
	lsiMaxZ.setValueOff();
	
	ssiBit8.setValueOff();
	ssiBit7.setValueOff();
	ssiBit6.setValueOff();
	ssiBit5.setValueOff();
	ssiBit4.setValueOff();
	ssiBit3.setValueOff();
	ssiBit2.setValueOff();
	ssiBit1.setValueOff();
	
	updateLimitStates();
	updateSupportStates();
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::enableControls(bool state, bool all) {
///////////////////////////////////////////////////////////////////
	if ( all == true ) {
		m_btEmergency->Enable(state);
	} else {
		m_btEmergency->Enable(true);
	}
	
	m_btForceUpdate->Enable(state);
	m_btClearTrace->Enable(state);
	m_btPeriphery->Enable(state);
	m_btConfiguration->Enable(state);
	
	lsiMinX.button->Enable(state);
	lsiMaxX.button->Enable(state);
	lsiMinY.button->Enable(state);
	lsiMaxY.button->Enable(state);
	lsiMinZ.button->Enable(state);
	lsiMaxZ.button->Enable(state);

	ssiBit8.button->Enable(state);
	ssiBit7.button->Enable(state);
	ssiBit6.button->Enable(state);
	ssiBit4.button->Enable(state);
	ssiBit3.button->Enable(state);
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::appendMessage(char type, const wxString& msg, const wxString& ctx) {
///////////////////////////////////////////////////////////////////
	if ( type == 'D' && showDebugMessages == false )
		return;

	logger->appendMessage(type, msg, ctx);
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::notifyStarted() {
///////////////////////////////////////////////////////////////////
	appendSep();
	appendInfo("Controller main loop is running . . .", CNC_LOG_FUNCT);
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::notifyConnecting() {
///////////////////////////////////////////////////////////////////
	appendSep();
	
	if ( m_btEmergency->GetValue() == true ) {
		// alway release the emergency button
		wxCommandEvent dummy;
		m_btEmergency->SetValue(false);
		onEmergencyButton(dummy);

		appendInfo("Releasing the Emergency Button . . .", CNC_LOG_FUNCT);
	}
	
	appendInfo("Controller is connecting . . .", CNC_LOG_FUNCT);
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::notifyConnected() {
///////////////////////////////////////////////////////////////////
	initValues();
	enableControls(true, true);
	
	appendInfo("Controller is connected . . .", CNC_LOG_FUNCT);
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::notifyDisconnected() {
///////////////////////////////////////////////////////////////////
	enableControls(false, true);
	
	appendInfo("Controller is disconnected . . .", CNC_LOG_FUNCT);
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::notifyPaused() {
///////////////////////////////////////////////////////////////////
	appendInfo("Paused . . .", CNC_LOG_FUNCT);
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::notifyResumed() {
///////////////////////////////////////////////////////////////////
	appendInfo("Resumed . . . ", CNC_LOG_FUNCT);
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onClose(wxCloseEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip(canClose);
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onShow(wxShowEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onStartupTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	// fixing layout problems
	wxSize size = GetClientSize();
	size.IncBy(1);
	
	SetClientSize(size);
	Refresh();
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onContinuousTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	// currently nothing to do
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::updatePinValue(const char type, unsigned int num, const char mode, int value) {
///////////////////////////////////////////////////////////////////
	wxASSERT( pinList );
	pinList->updatePinValue(type, num, mode, value);
	m_lastUpdate->ChangeValue(wxDateTime::UNow().FormatISOTime());
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::update(const AE::TransferData& data) {
///////////////////////////////////////////////////////////////////
	for ( auto i=AE::minPinIndex; i<=AE::maxPinIndex; i++ ) {
		const AE::PinData& pd = data.pins[i];
		updatePinValue(pd.type, pd.name, pd.mode, pd.value );
	}

	m_posStepperX->ChangeValue(wxString::Format("%ld", (long)data.stepperPosX));
	m_posStepperY->ChangeValue(wxString::Format("%ld", (long)data.stepperPosY));
	m_posStepperZ->ChangeValue(wxString::Format("%ld", (long)data.stepperPosZ));
	
	auto updateDirectionBitmap = [&](int32_t dir) {
		switch (dir) {
			case DIRECTION_INC: return ImageLibStepper().Bitmap("BMP_DIR_CW");
			case DIRECTION_DEC: return ImageLibStepper().Bitmap("BMP_DIR_CCW");
		}
		
		return ImageLibStepper().Bitmap("BMP_DIR_UNKNOWN");
	};

	m_bmpDirectionX->SetBitmap(updateDirectionBitmap(data.stepperDirX));
	m_bmpDirectionY->SetBitmap(updateDirectionBitmap(data.stepperDirY));
	m_bmpDirectionZ->SetBitmap(updateDirectionBitmap(data.stepperDirZ));
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::notifyPinUpdate() {
///////////////////////////////////////////////////////////////////
	if ( reConnectRequired == true )
		appendWarning("Reconnect required!", CNC_LOG_FUNCT);
	
}
///////////////////////////////////////////////////////////////////
CncArduinoEnvironment::LimitSwitchInfo* CncArduinoEnvironment::findLimitSwichInfo(wxButton* bt) {
///////////////////////////////////////////////////////////////////
	if      ( bt == lsiMinX.button )	return &lsiMinX;
	else if ( bt == lsiMaxX.button )	return &lsiMaxX;
	else if ( bt == lsiMinY.button )	return &lsiMinY;
	else if ( bt == lsiMaxY.button )	return &lsiMaxY;
	else if ( bt == lsiMinZ.button )	return &lsiMinZ;
	else if ( bt == lsiMaxZ.button )	return &lsiMaxZ;
	
	return NULL;
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::decorateEmergencySwitch() {
///////////////////////////////////////////////////////////////////
	if ( m_btEmergency->GetValue() == true ) m_btEmergency->SetBitmap(*irOn);
	else									 m_btEmergency->SetBitmap(*irOff);
	
	if ( m_btEmergency->GetValue() == true ) m_emergencyLabel->SetForegroundColour(*wxRED);
	else									 m_emergencyLabel->SetForegroundColour(*wxWHITE);
	
	m_emergencyLabel->Refresh();
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::decorateSwitch(wxButton* bt, bool state) {
///////////////////////////////////////////////////////////////////
	if ( bt == NULL )
		return;
		
	if ( state == false ) 	bt->SetBitmap(*lsOff);
	else 					bt->SetBitmap(*lsOn );
	
	bt->Refresh();
}
///////////////////////////////////////////////////////////////////
bool CncArduinoEnvironment::publishLimitSwitchUpdate(int name, bool state) {
///////////////////////////////////////////////////////////////////
	SerialThread* ss = SerialThread::theSerialThread();
	if ( ss == NULL )
		return false;
		
	typedef SerialAdminMessage::Mid 			MID;
	typedef SerialAdminMessage::ValueName 		VN;

	SerialAdminMessage pinUpdate;
	pinUpdate.setMid(MID::SET_DIGITAL_PIN);
	pinUpdate.setValue<unsigned int>(VN::VAL1, name);
	pinUpdate.setValue<bool        >(VN::VAL2, state);
	
	if ( ss->IsRunning() == false )
		ss->Resume();
		
	if ( ss->IsRunning() == false ) {
		appendError("Can't resume the serial thread!", CNC_LOG_FUNCT);
		return false;
	}
	
	ss->getAdminChannelSenderEndPoint()->write(pinUpdate);
	publishForceUpdate();
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncArduinoEnvironment::publishStatesUpdate() {
///////////////////////////////////////////////////////////////////
	SerialThread* ss = SerialThread::theSerialThread();
	if ( ss == NULL )
		return false;
		
	typedef SerialAdminMessage::Mid 			MID;
	typedef SerialAdminMessage::ValueName 		VN;

	SerialAdminMessage iscUpdate;
	iscUpdate.setMid(MID::SET_I2C_STATES);
	iscUpdate.setValue<unsigned char>(VN::VAL1, supportStates);
	iscUpdate.setValue<unsigned char>(VN::VAL2, limitStates);
	
	if ( ss->IsRunning() == false )
		ss->Resume();
		
	if ( ss->IsRunning() == false ) {
		appendError("Can't resume the serial thread!", CNC_LOG_FUNCT);
		return false;
	}
	
	ss->getAdminChannelSenderEndPoint()->write(iscUpdate);
	publishForceUpdate();
	
	return true;
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onLsLeftDown(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	wxButton* bt = static_cast<wxButton*>(event.GetEventObject());
	if ( bt == NULL )
		return;
		
	LimitSwitchInfo* lsi = findLimitSwichInfo(bt);
	if ( lsi == NULL )
		return;
		
	lsi->setValueOn();
	
	decorateSwitch(lsi->button, 			lsi->getBoolValue());
	publishLimitSwitchUpdate(lsi->name, 	lsi->getBoolValue());
	updateLimitStates();
	publishStatesUpdate();
	
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onLsLeftUp(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	wxButton* bt = static_cast<wxButton*>(event.GetEventObject());
	if ( bt == NULL )
		return;
		
	LimitSwitchInfo* lsi = findLimitSwichInfo(bt);
	if ( lsi == NULL )
		return;
		
	lsi->setValueOff();
		
	decorateSwitch(lsi->button,				lsi->getBoolValue());
	publishLimitSwitchUpdate(lsi->name, 	lsi->getBoolValue());
	updateLimitStates();
	publishStatesUpdate();
	
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onSelectArduinoPeriphery(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_contextBook->SetSelection(BookSelection::BS_Periphery);
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onSelectConfiguration(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_contextBook->SetSelection(BookSelection::BS_Configuration);
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onClearTrace(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	logger->clear();
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onForceUpdate(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	publishForceUpdate();
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::publishForceUpdate() {
///////////////////////////////////////////////////////////////////
	SerialThread* ss = SerialThread::theSerialThread();
	if ( ss == NULL )
		return;
	
	typedef SerialAdminMessage::Mid 			MID;
	SerialAdminMessage forceData;
	forceData.setMid(MID::FORCE_DATA_NOTIFICATION);
	ss->getAdminChannelSenderEndPoint()->write(forceData);
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onEmergencyButton(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	decorateEmergencySwitch();
	
	SerialThread* ss = SerialThread::theSerialThread();
	if ( ss == NULL )
		return;
		
	typedef SerialAdminMessage::Mid 			MID;
	typedef SerialAdminMessage::ValueName 		VN;

	SerialAdminMessage pinUpdate;
	pinUpdate.setMid(MID::SET_ANALOG_PIN);
	pinUpdate.setValue<unsigned int>(VN::VAL1, PIN_INTERRUPT_ID);
	pinUpdate.setValue<int         >(VN::VAL2, m_btEmergency->GetValue() ? 255 : 0);
	
	if ( ss->IsRunning() == false )
		ss->Resume();
		
	if ( ss->IsRunning() == false ) {
		appendError("Can't resume the serial thread!", CNC_LOG_FUNCT);
		return;
	}
	
	if ( m_btEmergency->GetValue() == true ) {
		reConnectRequired = true;
		enableControls(false);
	}
	
	ss->getAdminChannelSenderEndPoint()->write(pinUpdate);
	publishForceUpdate();
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onSptBtnLeftDown1(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	ssiBit8.setValueOn();
	decorateSwitch(ssiBit8.button, ssiBit8.getBoolValue());
	updateSupportStates();
	publishStatesUpdate();
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onSptBtnLeftUp1(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	ssiBit8.setValueOff();
	decorateSwitch(ssiBit8.button, ssiBit8.getBoolValue());
	updateSupportStates();
	publishStatesUpdate();
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onSptBtnLeftDown2(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	ssiBit7.setValueOn();
	decorateSwitch(ssiBit7.button, ssiBit7.getBoolValue());
	updateSupportStates();
	publishStatesUpdate();
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onSptBtnLeftUp2(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	ssiBit7.setValueOff();
	decorateSwitch(ssiBit7.button, ssiBit7.getBoolValue());
	updateSupportStates();
	publishStatesUpdate();
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onSptBtnLeftDown3(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	ssiBit6.setValueOn();
	decorateSwitch(ssiBit6.button, ssiBit6.getBoolValue());
	updateSupportStates();
	publishStatesUpdate();
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onSptBtnLeftUp3(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	ssiBit6.setValueOff();
	decorateSwitch(ssiBit6.button, ssiBit6.getBoolValue());
	updateSupportStates();
	publishStatesUpdate();
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onSupportSwt1(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	ssiBit4.toogleValue();
	decorateSwitch(ssiBit4.button, ssiBit4.getBoolValue());
	updateSupportStates();
	publishStatesUpdate();
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onSupportSwt2(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	ssiBit3.toogleValue();
	decorateSwitch(ssiBit3.button, ssiBit3.getBoolValue());
	updateSupportStates();
	publishStatesUpdate();
}
///////////////////////////////////////////////////////////////////
bool CncArduinoEnvironment::getBit(const unsigned char states, const unsigned char idx) const {
///////////////////////////////////////////////////////////////////
	switch (idx) {
		case 8:     return states & 128;
		case 7:     return states &  64;
		case 6:     return states &  32;
		case 5:     return states &  16;
		case 4:     return states &   8;
		case 3:     return states &   4;
		case 2:     return states &   2;
		case 1:     return states &   1;
	}

	return false;
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::setBit(unsigned char& states, const unsigned char idx, const bool value) { 
///////////////////////////////////////////////////////////////////
	if ( value == true ) {
		switch (idx) {
			case 8:     states |= (1 << 7); break;
			case 7:     states |= (1 << 6); break;
			case 6:     states |= (1 << 5); break;
			case 5:     states |= (1 << 4); break;
			case 4:     states |= (1 << 3); break;
			case 3:     states |= (1 << 2); break;
			case 2:     states |= (1 << 1); break;
			case 1:     states |= (1 << 0); break;
		}
	} else {
		switch (idx) {
			case 8:     states &= ~(1 << 7); break;
			case 7:     states &= ~(1 << 6); break;
			case 6:     states &= ~(1 << 5); break;
			case 5:     states &= ~(1 << 4); break;
			case 4:     states &= ~(1 << 3); break;
			case 3:     states &= ~(1 << 2); break;
			case 2:     states &= ~(1 << 1); break;
			case 1:     states &= ~(1 << 0); break;
		 }
	}
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::updateStateCtrl(wxTextCtrl* ctrl, const unsigned char states) {
///////////////////////////////////////////////////////////////////
	if ( ctrl == NULL )
		return;
	
	char byteStr[9];  
	byteStr[0] = getBit(states, 8) == true ? '1' : '0';
	byteStr[1] = getBit(states, 7) == true ? '1' : '0';
	byteStr[2] = getBit(states, 6) == true ? '1' : '0';
	byteStr[3] = getBit(states, 5) == true ? '1' : '0';
	byteStr[4] = getBit(states, 4) == true ? '1' : '0';
	byteStr[5] = getBit(states, 3) == true ? '1' : '0';
	byteStr[6] = getBit(states, 2) == true ? '1' : '0';
	byteStr[7] = getBit(states, 1) == true ? '1' : '0';
	byteStr[8] = '\0';
	
	ctrl->ChangeValue(byteStr);
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::updateLimitStates() {
///////////////////////////////////////////////////////////////////
	setBit(limitStates, LimitSwitch::BIT_LS_X_MIN, 	lsiMinX.getBoolValue());
	setBit(limitStates, LimitSwitch::BIT_LS_X_MAX, 	lsiMaxX.getBoolValue());
	
	setBit(limitStates, LimitSwitch::BIT_LS_Y_MIN, 	lsiMinY.getBoolValue());
	setBit(limitStates, LimitSwitch::BIT_LS_Y_MAX, 	lsiMaxY.getBoolValue());
	
	setBit(limitStates, LimitSwitch::BIT_LS_Z_MIN, 	lsiMinZ.getBoolValue());
	setBit(limitStates, LimitSwitch::BIT_LS_Z_MAX, 	lsiMaxZ.getBoolValue());
	
	setBit(limitStates, LimitSwitch::BIT_2, 		false);
	setBit(limitStates, LimitSwitch::BIT_1, 		false);

	updateStateCtrl(m_curLimitStates, limitStates);
}
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::updateSupportStates() {
///////////////////////////////////////////////////////////////////
	setBit(supportStates, SupportPin::BIT_IS_SUPPORT_BUTTON_1_PRESSED, 	ssiBit8.getBoolValue());
	setBit(supportStates, SupportPin::BIT_IS_SUPPORT_BUTTON_2_PRESSED, 	ssiBit7.getBoolValue());
	setBit(supportStates, SupportPin::BIT_IS_SUPPORT_BUTTON_3_PRESSED, 	ssiBit6.getBoolValue());
	setBit(supportStates, SupportPin::BIT_5, 							ssiBit5.getBoolValue());
	setBit(supportStates, SupportPin::BIT_IS_SUPPORT_SWITCH_1_PRESSED, 	ssiBit4.getBoolValue());
	setBit(supportStates, SupportPin::BIT_IS_SUPPORT_SWITCH_2_PRESSED, 	ssiBit3.getBoolValue());
	setBit(supportStates, SupportPin::BIT_IS_TOOL_POWERED,				ssiBit2.getBoolValue());
	setBit(supportStates, SupportPin::BIT_IS_CABLE_CONNECTED, 			ssiBit1.getBoolValue());

	updateStateCtrl(m_curSupportStates, supportStates);
}


///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onConfigChanged(wxPropertyGridEvent& event) {
///////////////////////////////////////////////////////////////////
	SerialThread* ss = SerialThread::theSerialThread();
	if ( ss == NULL )
		return;
	
	typedef SerialAdminMessage::Mid 		MID;
	typedef SerialAdminMessage::ValueName 	VN;
	
	SerialAdminMessage configUpdate;
	configUpdate.setMid(MID::NOTIFY_CONFIG_UPDATE);
	
	configUpdate.setValue(VN::VAL1, m_pgPropTraceGetters->GetValue());
	configUpdate.setValue(VN::VAL2, m_pgPropTraceSetters->GetValue());
	
	ss->getAdminChannelSenderEndPoint()->write(configUpdate);
} 
///////////////////////////////////////////////////////////////////
void CncArduinoEnvironment::onConfigChanging(wxPropertyGridEvent& event) {
///////////////////////////////////////////////////////////////////
	// currently nothing to do 
}
