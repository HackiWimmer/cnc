#include <system_error>
#include "ArduinoEnvWrapper.h"
#include "GlobalFunctions.h"
#include "MainFrame.h"
#include "CncUserEvents.h"
#include "CncArduino.h"
#include "WireCircularBuffer.h"
#include "WireEndPoint.h"
#include "SerialAdminChannel.h"
#include "SerialCircularBuffer.h"
#include "SerialEndPoint.h"
#include "SerialThread.h"

#include <wx/frame.h>
extern wxFrame* 	THE_FRAME;

// global interface: used at SoftwareSerial.h
SerialThread*   	SERIAL_THREAD					= NULL;

// global interface: used here
AE::ArduinoData* 	ARDUINO_DATA_STORE				= NULL;

// global interface: ArduinoWrappaer.cpp
SerialEndPoint* 	SERIAL_END_PTR_FOR_ARDUINO_ENV 	= NULL;

// global interface: ArduinoWrappaer.cpp
WireEndPoint* 		WIRE_END_PTR_FOR_ARDUINO_ENV	= NULL;


///////////////////////////////////////////////////////////////////
struct SerialInterruptException : public std::system_error {
	SerialInterruptException(const wxString& msg)
	: std::system_error(EIO, std::generic_category(), msg.c_str())
	{}
};
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
SerialThread*	SerialThread::theSerialThread() { return SERIAL_THREAD; }
bool 			SerialThread::isInterruped() 	{ return SERIAL_THREAD ? SERIAL_THREAD->interruped : false; }
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
SerialThread::SerialThread(MainFrame *handler)
: wxThread(wxTHREAD_DETACHED)
, pHandler			( handler )
, exit				( false )
, processAdmChl		( true )
, interruped		( false )
, connected			( false )
, serialPriority	( SerialPriority::DISCONNECTED )
, arduinoMainLoop	()
, tsStartup			( CncTimeFunctions::getNanoTimestamp() )
, tsHbRef			( CncTimeFunctions::getNanoTimestamp() )
, tsDtRef			( CncTimeFunctions::getNanoTimestamp() )
, tsHbInterval		( 1000 * 1000 * 1000 ) 	// 1s
, tsDtInterval		( 2000 * 1000 * 1000 ) 	// 2s
, arduinoDataStore	( new AE::ArduinoData() )
, adminChannel		( new SerialAdminChannel())
, adminSender		( new SerialAdminChannelSender(adminChannel))
, adminReceiver		( new SerialAdminChannelReceiver(adminChannel))
, master2slave      ( new WireCircularBuffer())
, slave2master      ( new WireCircularBuffer())
, master            ( new WireEndPoint(slave2master, master2slave) )
, slave				( new WireEndPoint(master2slave, slave2master) )
, app2ctl			( new SerialCircularBuffer())
, ctl2app			( new SerialCircularBuffer())
, app				( new SerialEndPoint(ctl2app, app2ctl))
, ctl				( new SerialEndPoint(app2ctl, ctl2app))
, i2cStates 		{'\0', '\0'}
///////////////////////////////////////////////////////////////////
{
	// Info:
	// app end point: reads(ctl2app) && writes(app2ctl)
	// ctl end point: reads(app2ctl) && writes(ctl2app)
	
	// Provide a global pointer interface for the arduino environment
	SERIAL_THREAD					= this;
	SERIAL_END_PTR_FOR_ARDUINO_ENV 	= ctl;
	WIRE_END_PTR_FOR_ARDUINO_ENV	= master;
	ARDUINO_DATA_STORE				= arduinoDataStore;
}
///////////////////////////////////////////////////////////////////
SerialThread::~SerialThread() {
///////////////////////////////////////////////////////////////////
	wxCriticalSectionLocker enter(pHandler->pSerialThreadCS);
	
	// the thread is being destroyed; 
	// make sure not to leave dangling pointers around
	SERIAL_THREAD					= NULL;
	SERIAL_END_PTR_FOR_ARDUINO_ENV 	= NULL;
	WIRE_END_PTR_FOR_ARDUINO_ENV	= NULL;
	ARDUINO_DATA_STORE				= NULL;

	wxDELETE ( adminChannel );
	wxDELETE ( adminSender );
	wxDELETE ( adminReceiver );
	wxDELETE ( app );
	wxDELETE ( ctl );
	wxDELETE ( app2ctl );
	wxDELETE ( ctl2app );
	wxDELETE ( arduinoDataStore );

	pHandler->serialThread 			= NULL;
}
///////////////////////////////////////////////////////////////////
void SerialThread::stop() {
///////////////////////////////////////////////////////////////////
	exit = true;
}
///////////////////////////////////////////////////////////////////
bool SerialThread::notifyConnecting() {
///////////////////////////////////////////////////////////////////
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialThread::notifyConnected() {
///////////////////////////////////////////////////////////////////
	connected 		= true;
	serialPriority	= SerialPriority::CONNECTED;
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialThread::notifyDisconnected() {
///////////////////////////////////////////////////////////////////
	connected 		= false;
	serialPriority	= SerialPriority::DISCONNECTED;
	
	return true;
}
///////////////////////////////////////////////////////////////////
wxThread::ExitCode SerialThread::Entry() {
///////////////////////////////////////////////////////////////////
	APPEND_THREAD_IDTO_STACK_TRACE_FILE;
	
	// post complete event
	wxQueueEvent(THE_FRAME, new SerialEvent(wxEVT_SERIAL_THREAD, MainFrame::EventId::INITIALIZED));
	
	tsHbRef = CncTimeFunctions::getNanoTimestamp();
	tsDtRef = CncTimeFunctions::getNanoTimestamp();
	
	try {
		
		arduinoMainLoop.setup();
		
		while ( !TestDestroy() ) {
			// recheck this here after the sleep
			if ( TestDestroy() ) break;
			if ( exit == true  ) break;
			
			idleThread();
			
			if ( adminChannel->size() > 0 )
				;//dispatchAdminChannel();
				
			// running the main loop
			if ( interruped == false ) {
				
				//const bool somethingTodo = app2ctl->size() != 0;
				arduinoMainLoop.loop();
				
				// thread data
				if ( CncTimeFunctions::getTimeSpanToNow(tsDtRef) > tsDtInterval )
					publishData();
			}
			
			// always publish thread heartbeats, but note:
			// if publishData() was triggered before the member tsHbRef 
			// will be modified also. Therefore, no heartbeat will be published 
			// immediately after publishData()
			if ( CncTimeFunctions::getTimeSpanToNow(tsHbRef) > tsHbInterval )
				publishHeartbeat();
				
		} // while
		
	} catch (const SerialInterruptException& e) {
		publishMessage('E', wxString::Format("Caught an interrupt: %s", e.what()), CNC_LOG_FUNCT);
		
		connected  		= false;
		interruped 		= true;
		serialPriority 	= SerialPriority::DISCONNECTED;
		
	} catch (...) {
		publishMessage('E', wxString::Format("Caught an unknown exception:"), CNC_LOG_FUNCT);
		
		connected  		= false;
		interruped 		= true;
		serialPriority 	= SerialPriority::DISCONNECTED;
	}
	
	// post complete event
	wxQueueEvent(THE_FRAME, new SerialEvent(wxEVT_SERIAL_THREAD, MainFrame::EventId::COMPLETED));
	
	return NULL;
}
///////////////////////////////////////////////////////////////////
void SerialThread::idleThread() {
///////////////////////////////////////////////////////////////////
	static unsigned int idleCounter = 0;
	
	// return immediately if something on the serial
	if ( app2ctl->size() != 0 ) { idleCounter = 0; return; }
	if ( ctl2app->size() != 0 ) { idleCounter = 0; return; }
	
	// provide a cool down phase
	if ( ++idleCounter < 1024 )	{ return; }
	
	// and now idle
	switch ( serialPriority ) {
		
		case SerialPriority::CONNECTED:		this->Sleep(50);
											break;
											
		case SerialPriority::DISCONNECTED:
		default:							this->Sleep(250);
	}
	
	//this->Sleep(10);
}
///////////////////////////////////////////////////////////////////
void SerialThread::publishPinNotification() {
///////////////////////////////////////////////////////////////////
	wxQueueEvent(THE_FRAME, new SerialEvent(wxEVT_SERIAL_THREAD, MainFrame::EventId::SERIAL_PIN_NOTIFICATION));
}
///////////////////////////////////////////////////////////////////
void SerialThread::publishData(bool force) {
///////////////////////////////////////////////////////////////////
	static AE::TransferData lastDataSend;
	
	SerialEvent* evt = new SerialEvent(wxEVT_SERIAL_THREAD, MainFrame::EventId::SERIAL_DATA);
	arduinoDataStore->fillTransferData(evt->data);
	
	if ( force == false && evt->data.isSomethingChanged(lastDataSend) == false )
		wxDELETE( evt );
	
	if ( evt != NULL ) {
		lastDataSend = evt->data;
		wxQueueEvent(THE_FRAME, evt);
	}

	tsHbRef = CncTimeFunctions::getNanoTimestamp();
	tsDtRef = tsHbRef;
}
///////////////////////////////////////////////////////////////////
void SerialThread::publishHeartbeat() {
///////////////////////////////////////////////////////////////////
	wxQueueEvent(THE_FRAME, new SerialEvent(wxEVT_SERIAL_THREAD, MainFrame::EventId::SERIAL_HEARTBEAT));
	tsHbRef = CncTimeFunctions::getNanoTimestamp();
}
///////////////////////////////////////////////////////////////////
bool SerialThread::getI2CStateValue(int id, int count, unsigned char& ls, unsigned char& ss) {
///////////////////////////////////////////////////////////////////
	if ( id != I2C_DEVICE_ID ) {
		publishMessage('W', wxString::Format("Invalid I2C request id: %d", id), CNC_LOG_FUNCT);
		return false;
	}
	
	if ( count != I2C_BYTE_COUNT ) {
		publishMessage('W', wxString::Format("Invalid I2C byte count %d", count), CNC_LOG_FUNCT);
		return false;
	}
	
	if ( theSerialThread() == NULL )
		return false;

	// check first if there`s a (states) update pending
	if ( theSerialThread()->adminChannel->size() > 0 )
		theSerialThread()->dispatchAdminChannel();

	ls = theSerialThread()->i2cStates[I2C_BYTE_LIMIT_STATE];
	ss = theSerialThread()->i2cStates[I2C_BYTE_SUPPORT_STATE];
	
	//publishMessage('I', wxString::Format("I2C Responce [id=%d, count=%d] prepared . . .", id, count), CNC_LOG_FUNCT);
	
	return true;
}
///////////////////////////////////////////////////////////////////
void SerialThread::dispatchAdminChannel() {
///////////////////////////////////////////////////////////////////
	typedef AE::ArduinoData AD;
	
	// -----------------------------------------------------------
	bool publishDataNotification = false;
	
	unsigned int counter = 0;
	while ( adminReceiver->size() > 0 ) {
		
		// leave the dispatching . . . 
		if ( ++counter > 32 ) {
			publishMessage('W', "To much admin events! The context will be switched back to main loop first . . . ", CNC_LOG_FUNCT);
			break;
		}
		
		SerialAdminMessage sam;
		if ( adminReceiver->read(sam) ) {
			
			if ( processAdmChl == false )
				continue;
			
			typedef SerialAdminMessage::Mid 			MID;
			typedef SerialAdminMessage::ValueName 		VN;
			
			AE::PinName pinName  = AE::PN_NULL;
			int 		pinValue = -1;
			
			switch ( sam.getMid() ) {
				
				// -----------------------------------------------
				case MID::FORCE_DATA_NOTIFICATION:
				{
					if ( interruped == false ) {
						publishData(true);
						publishDataNotification = false;
					}
					
					break;
				}
				
				// -----------------------------------------------
				case MID::DATA_NOTIFICATION:
				{
					if ( interruped == false ) 
						publishDataNotification = true;
						
					break;
				}

				// -----------------------------------------------
				case MID::NOTIFY_PIN_UPDATE:
				{
					if ( interruped == false ) 
						publishPinNotification();
						
					break;
				}
				
				// -----------------------------------------------
				case MID::SET_DIGITAL_PIN:
				{
					if ( interruped == false ) {
						// example: 10, true
						arduinoDataStore->digitalWrite(	sam.getValue<unsigned int>(VN::VAL1),
														sam.getValue<bool        >(VN::VAL2));
						publishPinNotification();
					}
					
					break;
				}
				
				// -----------------------------------------------
				case MID::SET_ANALOG_PIN:
				{
					// example: 3, 255
					pinName  = AD::convertPinName('A', (int)sam.getValue<unsigned int>(VN::VAL1));
					pinValue = sam.getValue<int>(VN::VAL2);
					
					arduinoDataStore->analogWrite(pinName, pinValue);

					if ( pinName == AE::PN_A0 ) {
						
						if ( pinValue > 0 ) { throw SerialInterruptException("Released by a user event . . ."); }
						else 				{ interruped = false; publishMessage('I', "Interruped reseted . . .", CNC_LOG_FUNCT); }
						
					}
					
					publishPinNotification();
					
					break;
				}
				
				// -----------------------------------------------
				case MID::SET_I2C_STATES:
				{
					if ( interruped == false ) {
						
						i2cStates[I2C_BYTE_SUPPORT_STATE] = sam.getValue<unsigned char>(VN::VAL1);
						i2cStates[I2C_BYTE_LIMIT_STATE]   = sam.getValue<unsigned char>(VN::VAL2);
					}
					break;
				}
				
				// -----------------------------------------------
				case MID::NOTIFY_CONFIG_UPDATE:
				{
					arduinoDataStore->exterConfig.traceGetters = sam.getValue<bool>(VN::VAL1);
					arduinoDataStore->exterConfig.traceSetters = sam.getValue<bool>(VN::VAL2);
					// ... more values
					
					publishMessage('I', "Configuration update provided . . .", CNC_LOG_FUNCT);
					break;
				}
				
				// -----------------------------------------------
				default:
				{
					publishMessage('E', wxString::Format("Unresolved message id: %d", (int)sam.getMid()), CNC_LOG_FUNCT);
				}
			}
		}
	}
	
	if ( publishDataNotification == true ) 
		publishData();
}
///////////////////////////////////////////////////////////////////
void SerialThread::publishMessage(const char type, const wxString& msg, const wxString& ctx) {
///////////////////////////////////////////////////////////////////
	SerialEvent* evt = new SerialEvent(wxEVT_SERIAL_THREAD, MainFrame::EventId::SERIAL_MESSAGE);
	evt->message.setType(type);
	evt->message.setMessage(msg);
	evt->message.setContext(ctx);
	wxQueueEvent(THE_FRAME, evt);
}
////////////////////////////////////////////////////////////////////
const char* SerialThread::ardoGetCmdLabel(unsigned char c) {
////////////////////////////////////////////////////////////////////
	static std::string ret;
	return ArduinoCMDs::getCMDLabel((unsigned int)c, ret);
}
////////////////////////////////////////////////////////////////////
const char* SerialThread::ardoGetPidLabel(unsigned char p) {
////////////////////////////////////////////////////////////////////
	static std::string ret;
	return ArduinoPIDs::getPIDLabel((unsigned int)p, ret);
}
////////////////////////////////////////////////////////////////////
const char* SerialThread::ardoGetErrLabel(unsigned char e) {
////////////////////////////////////////////////////////////////////
	static std::string ret;
	return ArduinoErrorCodes::getECLabel((unsigned int)e, ret);
}
////////////////////////////////////////////////////////////////////
void SerialThread::pinMode(AE::PinName pin, AE::PinMode pm) {
////////////////////////////////////////////////////////////////////
	if ( SerialThread::isInterruped() == true )
		return;
		
	// check first if there`s a (pin) update pending
	if ( theSerialThread() && theSerialThread()->adminChannel->size() > 0 )
		theSerialThread()->dispatchAdminChannel();

	if ( ARDUINO_DATA_STORE != NULL )
		ARDUINO_DATA_STORE->pinMode(pin, pm);
}
////////////////////////////////////////////////////////////////////
void SerialThread::digitalWrite(AE::PinName pin, AE::PinLevel pl) {
////////////////////////////////////////////////////////////////////
	if ( SerialThread::isInterruped() == true )
		return;
		
	if ( ARDUINO_DATA_STORE != NULL )
		ARDUINO_DATA_STORE->digitalWrite(pin, pl);
}
////////////////////////////////////////////////////////////////////
AE::PinLevel SerialThread::digitalRead(AE::PinName pin) {
////////////////////////////////////////////////////////////////////
	if ( SerialThread::isInterruped() == true )
		return PL_UNDEFINED;
	
	// check first if there`s a (pin) update pending
	if ( theSerialThread() && theSerialThread()->adminChannel->size() > 0 )
		theSerialThread()->dispatchAdminChannel();
		
	return ARDUINO_DATA_STORE ? ARDUINO_DATA_STORE->digitalRead(pin) : PL_UNDEFINED;
}
////////////////////////////////////////////////////////////////////
void SerialThread::analogWrite(AE::PinName pin, int value) {
////////////////////////////////////////////////////////////////////
	if ( SerialThread::isInterruped() == true )
		return;
	
	if ( ARDUINO_DATA_STORE != NULL )
		ARDUINO_DATA_STORE->analogWrite(pin, value);
}
////////////////////////////////////////////////////////////////////
int16_t SerialThread::analogRead(AE::PinName pin) {
////////////////////////////////////////////////////////////////////
	if ( SerialThread::isInterruped() == true )
		return 0;
	
	// check first if there`s a (pin) update pending
	if ( theSerialThread() && theSerialThread()->adminChannel->size() > 0 )
		theSerialThread()->dispatchAdminChannel();
		
	return ARDUINO_DATA_STORE ? ARDUINO_DATA_STORE->analogRead(pin) : 0;
}
////////////////////////////////////////////////////////////////////
uint32_t SerialThread::millis() {
////////////////////////////////////////////////////////////////////
	// Returns the number of milliseconds passed since the Arduino board 
	// began running the current program. This number will overflow 
	// (go back to zero), after approximately 50 days.
	
	const CncNanoTimestamp base = theSerialThread() ? theSerialThread()->tsStartup : 0;
	const CncNanoTimespan ts    = CncTimeFunctions::getTimeSpan(CncTimeFunctions::getNanoTimestamp(), base);
	
	//            nano2milli            rollover
	return ( ( ts / std::mega::num ) % 4294967295 );
} 
////////////////////////////////////////////////////////////////////
uint32_t SerialThread::micros() {
////////////////////////////////////////////////////////////////////
	// Returns the number of microseconds since the Arduino board began running 
	// the current program. This number will overflow (go back to zero), after 
	// approximately 70 minutes. [ 70 * 60 * 1000 * 1000 ~ 4294967295 ]
	
	const CncNanoTimestamp base = theSerialThread() ? theSerialThread()->tsStartup : 0;
	const CncNanoTimespan ts    = CncTimeFunctions::getTimeSpan(CncTimeFunctions::getNanoTimestamp(), base);
	
	//            nano2micro            rollover
	return ( ( ts / std::kilo::num ) % 4294967295 );
}
////////////////////////////////////////////////////////////////////
void SerialThread::delay(uint32_t milliSeconds) {
////////////////////////////////////////////////////////////////////
	if ( SerialThread::isInterruped() == true )
		return;
	
	CncTimeFunctions::sleepMilliseconds(milliSeconds);
}
////////////////////////////////////////////////////////////////////
void SerialThread::delayMicroseconds(int16_t microsSeconds) {
////////////////////////////////////////////////////////////////////
	if ( SerialThread::isInterruped() == true )
		return;
	
	CncTimeFunctions::sleepMircoseconds(microsSeconds);
}
////////////////////////////////////////////////////////////////////
uint8_t SerialThread::getDigitalPinToPort(uint8_t pin) {
////////////////////////////////////////////////////////////////////
	if ( SerialThread::isInterruped() == true )
		return AE::PN_NOT_A_PIN;
		
	if ( AE::ArduinoData::isPin((AE::PinName)pin) == true ) {
		if ( AE::ArduinoData::isAnalogPin((AE::PinName)pin) == true )
			return pin - AE::minAPinIndex;
		else
			return pin;
	}
	
	return AE::PN_NOT_A_PIN;
}
////////////////////////////////////////////////////////////////////
bool SerialThread::ardoConfigGetTraceGetters() {
////////////////////////////////////////////////////////////////////
	return ARDUINO_DATA_STORE ? ARDUINO_DATA_STORE->exterConfig.traceGetters : false;
}
////////////////////////////////////////////////////////////////////
bool SerialThread::ardoConfigGetTraceSetters() {
////////////////////////////////////////////////////////////////////
	return ARDUINO_DATA_STORE ? ARDUINO_DATA_STORE->exterConfig.traceSetters : false;
}
////////////////////////////////////////////////////////////////////
void SerialThread::ardoTraceStepperDir(char sid, int32_t dir) {
////////////////////////////////////////////////////////////////////
	if ( ARDUINO_DATA_STORE == NULL )
		return; 
		
	switch ( sid ) {
		case 'X': ARDUINO_DATA_STORE->traceInfo.stepperDirX = dir; break;
		case 'Y': ARDUINO_DATA_STORE->traceInfo.stepperDirY = dir; break;
		case 'Z': ARDUINO_DATA_STORE->traceInfo.stepperDirZ = dir; break;
	}
}
////////////////////////////////////////////////////////////////////
void SerialThread::ardoTraceStepperPos(char sid, int32_t pos) {
////////////////////////////////////////////////////////////////////
	if ( ARDUINO_DATA_STORE == NULL )
		return; 

	switch ( sid ) {
		case 'X': ARDUINO_DATA_STORE->traceInfo.stepperPosX = pos; break;
		case 'Y': ARDUINO_DATA_STORE->traceInfo.stepperPosY = pos; break;
		case 'Z': ARDUINO_DATA_STORE->traceInfo.stepperPosZ = pos; break;
	}
}
