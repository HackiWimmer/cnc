#include "SerialThread.h"
#include "ArduinoEnvWrapper.h"

// support the arduino runtime code
#include "../Arduino/StepperEnvironment/MainLoop.ino"
#include "../Arduino/StepperEnvironment/CncCtrl.ino"
#include "../Arduino/StepperEnvironment/CncStep.ino"
#include "../Arduino/StepperEnvironment/CncRndr.ino"
#include "../Arduino/StepperEnvironment/CncPwmr.ino"
#include "../Arduino/StepperEnvironment/ArdoErr.ino"

////////////////////////////////////////////////////////////////////
// Arduino Emulator Function Interface:
//
// The following functions are defined at:
// "../Arduino/StepperEnvironment/ArdoEnv.h"
////////////////////////////////////////////////////////////////////
typedef AE::ArduinoData AD;

// -----------------------------------------------------------------
// Additional helper functions

// .................................................................
void 			AE::ardoDebugMessage(const char type, const char* msg, const char* ctx)
				{ SerialThread::ardoDebugMessage(type, msg, ctx); }

// .................................................................
template<typename T>
void 			AE::ardoDebugValue(	const char* name, T value, const char* ctx)
				{ SerialThread::ardoDebugValue(name, value, ctx); }

// .................................................................
bool			AE::ardoConfigGetTraceGetters()
				{ return SerialThread::ardoConfigGetTraceGetters(); }
				
// .................................................................
bool			AE::ardoConfigGetTraceSetters()
				{ return SerialThread::ardoConfigGetTraceSetters(); }
				
// .................................................................
void			AE::ardoTraceStepperDir(char id, int32_t dir)
				{ SerialThread::ardoTraceStepperDir(id, dir); }
				
// .................................................................
void			AE::ardoTraceStepperPos(char id, int32_t pos)
				{ SerialThread::ardoTraceStepperPos(id, pos); }
				
// .................................................................
const char*		AE::ardoGetCmdLabel(unsigned char c)
				{ return SerialThread::ardoGetCmdLabel(c); }
				
// .................................................................
const char*		AE::ardoGetPidLabel(unsigned char p)
				{ return SerialThread::ardoGetPidLabel(p); }
				
// .................................................................
const char*		AE::ardoGetErrLabel(unsigned char e)
				{ return SerialThread::ardoGetErrLabel(e); }

// -----------------------------------------------------------------
// Arduino runtime interface

// .................................................................
void 			AE::pinMode(PinName pin, PinMode pm)
				{ SerialThread::pinMode(pin, pm); }

// .................................................................
void 			AE::pinMode(unsigned char pin, PinMode pm)
				{ SerialThread::pinMode(AD::convertPinName(pin), pm); }

// .................................................................
void 			AE::digitalWrite(PinName pin, PinLevel pl)
				{ SerialThread::digitalWrite(pin, pl); }

// .................................................................
void 			AE::digitalWrite(unsigned char pin, PinLevel pl)
				{ SerialThread::digitalWrite(AD::convertPinName(pin), pl); }

// .................................................................
void			AE::digitalWrite(unsigned char pin, bool pl)
				{ SerialThread::digitalWrite(AD::convertPinName(pin), AD::convertPinLevel(pl)); }

// .................................................................
void            AE::analogWrite(PinName pin, int value)
				{ SerialThread::analogWrite(pin, value); }

// .................................................................
void            AE::analogWrite(unsigned char pin, int value)
				{ SerialThread::analogWrite(AD::convertPinName(pin), value); }

// .................................................................
void 			AE::delay(uint32_t milliSeconds)
				{ SerialThread::delay(milliSeconds); }

// .................................................................
void 			AE::delayMicroseconds(uint16_t microsSeconds)
				{ SerialThread::delayMicroseconds(microsSeconds); }
	
// .................................................................
AE::PinLevel 	AE::digitalRead(PinName pin)
				{ return SerialThread::digitalRead(pin); }

// .................................................................
AE::PinLevel 	AE::digitalRead(unsigned char pin)
				{ return SerialThread::digitalRead(AD::convertPinName(pin)); }

// .................................................................
int16_t         AE::analogRead(PinName pin)
				{ return SerialThread::analogRead(pin); }

// .................................................................
int16_t         AE::analogRead(unsigned char pin)
				{ return SerialThread::analogRead(AD::convertPinName('A', (int)pin)); }
	
// .................................................................
uint32_t 		AE::millis()
				{ return SerialThread::millis(); }

// .................................................................
uint32_t 		AE::micros()
				{ return SerialThread::micros(); }

// .................................................................
uint8_t			AE::getDigitalPinToPort(uint8_t pin)
				{ return SerialThread::getDigitalPinToPort(pin); }

