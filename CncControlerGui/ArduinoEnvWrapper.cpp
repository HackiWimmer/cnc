#include "SerialThread.h"
#include "ArduinoEnvWrapper.h"

// support the arduino runtime code
#include "../Arduino/StepperEnvironment/MainLoop.ino"
#include "../Arduino/StepperEnvironment/CncCtrl.ino"
#include "../Arduino/StepperEnvironment/CncStep.ino"
#include "../Arduino/StepperEnvironment/CncRndr.ino"
#include "../Arduino/StepperEnvironment/CncAcmr.ino"
#include "../Arduino/StepperEnvironment/CncPod.ino"
#include "../Arduino/StepperEnvironment/ArdoObj.ino"
#include "../Arduino/StepperEnvironment/ArdoErr.ino"

////////////////////////////////////////////////////////////////////
// Arduino Emulator Function Interface:
//
// The following functions are defined at:
// "../Arduino/StepperEnvironment/ArdoEnv.h"
////////////////////////////////////////////////////////////////////
typedef AE::ArduinoData 		AD;
#define JOIN_SERIAL_THREAD       1
// -----------------------------------------------------------------
// Additional helper functions

// .................................................................
void 			AE::ardoDebugMessage(const char type, const char* msg, const char* ctx)
#ifdef JOIN_SERIAL_THREAD
				{ SerialThread::ardoDebugMessage(type, msg, ctx); }
#else
				{}
#endif

// .................................................................
template<typename T1>
void 			AE::ardoDebugValue(	const char* name, T1 v1, const char* ctx)
#ifdef JOIN_SERIAL_THREAD
				{ SerialThread::ardoDebugValue(name, v1, ctx); }
#else
				{}
#endif

// .................................................................
template<typename T1, typename T2>
void 			AE::ardoDebugValue(	const char* name, T1 v1, T2 v2, const char* ctx)
#ifdef JOIN_SERIAL_THREAD
				{ SerialThread::ardoDebugValue(name, v1, v2, ctx); }
#else
				{}
#endif

// .................................................................
template<typename T1, typename T2, typename T3>
void 			AE::ardoDebugValue(	const char* name, T1 v1, T2 v2, T3 v3, const char* ctx)
#ifdef JOIN_SERIAL_THREAD
				{ SerialThread::ardoDebugValue(name, v1, v2, v3, ctx); }
#else
				{}
#endif

// .................................................................
template<typename T1, typename T2, typename T3, typename T4>
void 			AE::ardoDebugValue(	const char* name, T1 v1, T2 v2, T3 v3, T4 v4, const char* ctx)
#ifdef JOIN_SERIAL_THREAD
				{ SerialThread::ardoDebugValue(name, v1, v2, v3, v4, ctx); }
#else
				{}
#endif

// .................................................................
template<typename T1, typename T2, typename T3, typename T4, typename T5>
void 			AE::ardoDebugValue(	const char* name, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, const char* ctx)
#ifdef JOIN_SERIAL_THREAD
				{ SerialThread::ardoDebugValue(name, v1, v2, v3, v4, v5, ctx); }
#else
				{}
#endif

// .................................................................
bool			AE::ardoConfigGetTraceGetters()
#ifdef JOIN_SERIAL_THREAD
				{ return SerialThread::ardoConfigGetTraceGetters(); }
#else
				{ return false; }
#endif

// .................................................................
bool			AE::ardoConfigGetTraceSetters()
#ifdef JOIN_SERIAL_THREAD
				{ return SerialThread::ardoConfigGetTraceSetters(); }
#else
				{ return false; }
#endif

// .................................................................
void			AE::ardoTraceStepperDir(char id, int32_t dir)
#ifdef JOIN_SERIAL_THREAD
				{ SerialThread::ardoTraceStepperDir(id, dir); }
#else
				{}
#endif

// .................................................................
void			AE::ardoTraceStepperPos(char id, int32_t pos)
#ifdef JOIN_SERIAL_THREAD
				{ SerialThread::ardoTraceStepperPos(id, pos); }
#else
				{}
#endif

// .................................................................
void			AE::ardoTraceSpeed(char id, int32_t val)
#ifdef JOIN_SERIAL_THREAD
				{ SerialThread::ardoTraceSpeed(id, val); }
#else
				{}
#endif

// .................................................................
void			AE::ardoTraceMove(uint8_t sid, int32_t dx, int32_t dy, int32_t dz)
#ifdef JOIN_SERIAL_THREAD
				{ SerialThread::ardoTraceMove(sid, dx, dy, dz); }
#else
				{}
#endif

// .................................................................
const char*		AE::ardoGetCmdLabel(unsigned char c)
#ifdef JOIN_SERIAL_THREAD
				{ return SerialThread::ardoGetCmdLabel(c); }
#else
				{ return "???"; }
#endif

// .................................................................
const char*		AE::ardoGetPidLabel(unsigned char p)
#ifdef JOIN_SERIAL_THREAD
				{ return SerialThread::ardoGetPidLabel(p); }
#else
				{ return "???"; }
#endif

// .................................................................
const char*		AE::ardoGetErrLabel(unsigned char e)
#ifdef JOIN_SERIAL_THREAD
				{ return SerialThread::ardoGetErrLabel(e); }
#else
				{ return "???"; }
#endif

// -----------------------------------------------------------------
// Arduino runtime interface

// .................................................................
void 			AE::pinMode(PinName pin, PinMode pm)
#ifdef JOIN_SERIAL_THREAD
				{ SerialThread::pinMode(pin, pm); }
#else
				{}
#endif

// .................................................................
void 			AE::pinMode(unsigned char pin, PinMode pm)
#ifdef JOIN_SERIAL_THREAD
				{ SerialThread::pinMode(AD::convertPinName(pin), pm); }
#else
				{}
#endif

// .................................................................
void 			AE::digitalWrite(PinName pin, PinLevel pl)
#ifdef JOIN_SERIAL_THREAD
				{ SerialThread::digitalWrite(pin, pl); }
#else
				{}
#endif

// .................................................................
void 			AE::digitalWrite(unsigned char pin, PinLevel pl)
#ifdef JOIN_SERIAL_THREAD
				{ SerialThread::digitalWrite(AD::convertPinName(pin), pl); }
#else
				{}
#endif

// .................................................................
void			AE::digitalWrite(unsigned char pin, bool pl)
#ifdef JOIN_SERIAL_THREAD
				{ SerialThread::digitalWrite(AD::convertPinName(pin), AD::convertPinLevel(pl)); }
#else
				{}
#endif

// .................................................................
void            AE::analogWrite(PinName pin, int value)
#ifdef JOIN_SERIAL_THREAD
				{ SerialThread::analogWrite(pin, value); }
#else
				{}
#endif

// .................................................................
void            AE::analogWrite(unsigned char pin, int value)
#ifdef JOIN_SERIAL_THREAD
				{ SerialThread::analogWrite(AD::convertPinName(pin), value); }
#else
				{}
#endif

// .................................................................
void 			AE::delay(uint32_t milliSeconds)
#ifdef JOIN_SERIAL_THREAD
				{ SerialThread::delay(milliSeconds); }
#else
				{}
#endif

// .................................................................
void 			AE::delayMicroseconds(uint16_t microsSeconds)
#ifdef JOIN_SERIAL_THREAD
				{ SerialThread::delayMicroseconds(microsSeconds); }
#else
				{}
#endif

// .................................................................
AE::PinLevel 	AE::digitalRead(PinName pin)
#ifdef JOIN_SERIAL_THREAD
				{ return SerialThread::digitalRead(pin); }
#else
				{ return PL_LOW; }
#endif

// .................................................................
AE::PinLevel 	AE::digitalRead(unsigned char pin)
#ifdef JOIN_SERIAL_THREAD
				{ return SerialThread::digitalRead(AD::convertPinName(pin)); }
#else
				{ return PL_LOW; }
#endif

// .................................................................
int16_t         AE::analogRead(PinName pin)
#ifdef JOIN_SERIAL_THREAD
				{ return SerialThread::analogRead(pin); }
#else
				{ return 0; }
#endif

// .................................................................
int16_t         AE::analogRead(unsigned char pin)
#ifdef JOIN_SERIAL_THREAD
				{ return SerialThread::analogRead(AD::convertPinName('A', (int)pin)); }
#else
				{ return 0; }
#endif

// .................................................................
uint32_t 		AE::millis()
#ifdef JOIN_SERIAL_THREAD
				{ return SerialThread::millis(); }
#else
				{ return 0; }
#endif

// .................................................................
uint32_t 		AE::micros()
#ifdef JOIN_SERIAL_THREAD
				{ return SerialThread::micros(); }
#else
				{ return 0; }
#endif

// .................................................................
uint8_t			AE::getDigitalPinToPort(uint8_t pin)
#ifdef JOIN_SERIAL_THREAD
				{ return SerialThread::getDigitalPinToPort(pin); }
#else
				{ return 0; }
#endif

// .................................................................
uint8_t			AE::getPinMode(uint8_t pin)
#ifdef JOIN_SERIAL_THREAD
				{ return SerialThread::getDigitalPinToPort(pin); }
#else
				{ return 0; }
#endif

