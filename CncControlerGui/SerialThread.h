#ifndef SerialThread_H
#define SerialThread_H

#include <wx/thread.h>
#include <wx/event.h>
#include "SerialEvent.h"
#include "OSD/CncTimeFunctions.h"

typedef void (wxEvtHandler::*SerialEventFunction)(SerialEvent&);
#define SerialEventHandler(func) wxEVENT_HANDLER_CAST(SerialEventFunction, func)

////////////////////////////////////////////////////////////////////////////////////
class MainFrame;
class WireCircularBuffer;
class WireEndPoint;
class SerialCircularBuffer;
class SerialEndPoint;
class SerialAdminChannel; 
class SerialAdminChannelSender; 
class SerialAdminChannelReceiver; 

class SerialThread : public wxThread {
	
	protected:
		
		enum class SerialState		{ DISCONNECTED=0, CONNECTED=1 };

		MainFrame* 					pHandler;
		bool 						exit;
		bool						processAdmChl;
		bool						interruped;
		SerialState					serialState;
		
		ArduinoMainLoop				arduinoMainLoop;
		
		CncNanoTimestamp 			tsStartup;
		
		CncNanoTimestamp 			tsHbRef;
		CncNanoTimestamp 			tsDtRef;

		CncNanoTimespan 			tsHbInterval;

		AE::ArduinoData*			arduinoDataStore;
		
		SerialAdminChannel*			adminChannel; 
		SerialAdminChannelSender*	adminSender; 
		SerialAdminChannelReceiver*	adminReceiver; 

		WireCircularBuffer* 		master2slave;
		WireCircularBuffer* 		slave2master;

		WireEndPoint* 				master;
		WireEndPoint* 				slave;

		SerialCircularBuffer* 		app2ctl;
		SerialCircularBuffer* 		ctl2app;

		SerialEndPoint* 			app;
		SerialEndPoint* 			ctl;
		
		unsigned char 				i2cStates[I2C_BYTE_COUNT];

		virtual ExitCode Entry();
		
		void idleThread();
		void publishPinNotification();
		void publishData(bool force = false);
		void publishHeartbeat();
		void dispatchAdminChannel();
		
	public:

		explicit SerialThread(MainFrame *handler);
		virtual ~SerialThread();
		
		// thread interface
		void 						stop();
		bool						transferData(AE::TransferData& td);
		bool						notifyConnecting();
		bool						notifyConnected();
		bool						notifyDisconnected();
		SerialEndPoint* 			getSerialAppEndPoint() 			{ return app; }
		WireEndPoint* 				getWireSlaveEndPoint() 			{ return slave; }
		SerialAdminChannelSender*	getAdminChannelSenderEndPoint() { return adminSender; }
		
		
		static SerialThread*		theSerialThread();
		static void 				publishMessage(const char type, const wxString& msg, const wxString& context);
		static bool					getI2CStateValue(int id, int count, unsigned char& ls, unsigned char& ss);
		
		// additional interface
		static bool 				isInterruped();
		static void 				ardoDebugMessage(const char type, const char* msg, const char* context) {
										SerialThread::publishMessage(type, msg ? msg : "", context ? context : "");
		}
		
		template<typename T1>
		static void 				ardoDebugValue(const char* name, T1 v1, const char* context) {
										wxString strValue1; strValue1 << v1;
										SerialThread::publishMessage('D', wxString::Format("%s = %s", name ? name : "Unknown", strValue1), context ? context : "");
		}
		
		template<typename T1, typename T2>
		static void 				ardoDebugValue(const char* name, T1 v1, T2 v2, const char* context) {
										wxString strValue1; strValue1 << v1;
										wxString strValue2; strValue2 << v2;
										SerialThread::publishMessage('D', wxString::Format("%s = %s, %s", name ? name : "Unknown", strValue1, strValue2), context ? context : "");
		}
		
		template<typename T1, typename T2, typename T3>
		static void 				ardoDebugValue(const char* name, T1 v1, T2 v2, T3 v3, const char* context) {
										wxString strValue1; strValue1 << v1;
										wxString strValue2; strValue2 << v2;
										wxString strValue3; strValue3 << v3;
										SerialThread::publishMessage('D', wxString::Format("%s = %s, %s, %s", name ? name : "Unknown", strValue1, strValue2, strValue3), context ? context : "");
		}
		
		template<typename T1, typename T2, typename T3, typename T4>
		static void 				ardoDebugValue(const char* name, T1 v1, T2 v2, T3 v3, T4 v4, const char* context) {
										wxString strValue1; strValue1 << v1;
										wxString strValue2; strValue2 << v2;
										wxString strValue3; strValue3 << v3;
										wxString strValue4; strValue4 << v4;
										SerialThread::publishMessage('D', wxString::Format("%s = %s, %s, %s, %s", name ? name : "Unknown", strValue1, strValue2, strValue3, strValue4), context ? context : "");
		}
		
		template<typename T1, typename T2, typename T3, typename T4, typename T5>
		static void 				ardoDebugValue(const char* name, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, const char* context) {
										wxString strValue1; strValue1 << v1;
										wxString strValue2; strValue2 << v2;
										wxString strValue3; strValue3 << v3;
										wxString strValue4; strValue4 << v4;
										wxString strValue5; strValue4 << v5;
										SerialThread::publishMessage('D', wxString::Format("%s = %s, %s, %s, %s, %s", name ? name : "Unknown", strValue1, strValue2, strValue3, strValue4, strValue5), context ? context : "");
		}
		
		static bool 				ardoConfigGetTraceGetters();
		static bool 				ardoConfigGetTraceSetters();
		
		static void 				ardoTraceStepperDir(char sid, int32_t dir);
		static void 				ardoTraceStepperPos(char sid, int32_t pos);
		static void 				ardoTraceSpeed(char sid, int32_t val);

		static const char*			ardoGetCmdLabel(unsigned char c);
		static const char*			ardoGetPidLabel(unsigned char p);
		static const char*			ardoGetErrLabel(unsigned char e);
		
		// arduino interface
		static void 				pinMode(AE::PinName pin, AE::PinMode pm);
		static void 				digitalWrite(AE::PinName pin, AE::PinLevel pl);
		static AE::PinLevel 		digitalRead(AE::PinName pin);
		static void 				analogWrite(AE::PinName pin, int value);
		static int16_t				analogRead(AE::PinName pin);
		static uint32_t		 		millis();
		static uint32_t		 		micros();
		static void 				delay(uint32_t milliSeconds);
		static void 				delayMicroseconds(int16_t microsSeconds);
		static uint8_t				getDigitalPinToPort(uint8_t pin);
		static uint8_t				getPinMode(uint8_t pin);
		
		wxDECLARE_NO_COPY_CLASS(SerialThread);
};

#endif
