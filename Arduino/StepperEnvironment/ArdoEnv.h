#ifndef ARDUINO_ENVIRONMENT_H
#define ARDUINO_ENVIRONMENT_H

  #include "ArdoVal.h"

  #define ARDO_LOG_FUNCT __PRETTY_FUNCTION__

  #ifndef SKETCH_COMPILE

    #include "wx/string.h"
    typedef unsigned char  byte;
    #define digitalPinToPort(P) AE::getDigitalPinToPort(P)   
    
    namespace AE {
      // C++ Application Environment
      static const short PN_NOT_A_PIN = 0xFF;
      
      enum PinName   { PN_NULL=PN_NOT_A_PIN, 
                       PN_D0=0,PN_D1=1, PN_D2=3, PN_D3=3, PN_D4=4, PN_D5=5, 
                       PN_D6=6, PN_D7=7, PN_D8=8, PN_D9=9, PN_D10=10, 
                       PN_D11=11, PN_D12=12, PN_D13=13,
                       PN_A0=14, PN_A1=15, PN_A2=16, PN_A3=17, PN_A4=18, PN_A5=19};  
                       
      enum PinLevel  { PL_UNDEFINED = -1, PL_LOW = 0, PL_HIGH = 1 };
      enum PinMode   { PM_INPUT, PM_OUTPUT, PM_INPUT_PULLUP };
      enum PinType   { PT_UNKNOWN = 0, PT_ANALOG = 1, PT_DIGITAL = 2 };
  
      // addition helper functions
      extern inline void              ardoDebugMessage(const char type, const char* msg, const char* context);
  
      template<typename T>
      extern void                     ardoDebugValue(const char* name, T value, const char* context);

      extern bool                     ardoConfigGetTraceGetters();
      extern bool                     ardoConfigGetTraceSetters();

      extern void                     ardoTraceStepperDir(char id, int32_t dir);
      extern void                     ardoTraceStepperPos(char id, int32_t pos);

      extern const char*              ardoGetCmdLabel(unsigned char c);
      extern const char*              ardoGetPidLabel(unsigned char p);
      extern const char*              ardoGetErrLabel(unsigned char e);
      
      // arduino runtime interface
      extern inline void              pinMode(PinName pin, PinMode pm);  
      extern inline void              pinMode(unsigned char pin, PinMode pm);  
      
      extern inline void              digitalWrite(PinName pin, PinLevel pl); 
      extern inline void              digitalWrite(unsigned char pin, PinLevel pl); 
      extern inline void              digitalWrite(unsigned char pin, bool pl); 
      
      extern inline PinLevel          digitalRead(PinName pin);
      extern inline PinLevel          digitalRead(unsigned char pin);
  
      extern inline void              analogWrite(PinName pin, int value); 
      extern inline void              analogWrite(unsigned char pin, int value); 
  
      extern inline int16_t           analogRead(PinName pin);
      extern inline int16_t           analogRead(unsigned char pin);

      extern inline uint32_t          millis();  
      extern inline uint32_t          micros();  
  
      extern inline void              delay(uint32_t milliSeconds);  
      extern inline void              delayMicroseconds(uint16_t microsSeconds);  
  
      extern inline uint8_t           getDigitalPinToPort(uint8_t pin);
   
    }; // namespace AE 
  
    #define PM_INPUT                AE::PM_INPUT
    #define PM_INPUT_PULLUP         AE::PM_INPUT_PULLUP
    #define PM_OUTPUT               AE::PM_OUTPUT
    #define PL_UNDEFINED            AE::PL_UNDEFINED
    #define PL_LOW                  AE::PL_LOW
    #define PL_HIGH                 AE::PL_HIGH
    #define PT_UNKNOWN              AE::PT_UNKNOWN
    #define PT_ANALOG               AE::PT_ANALOG
    #define PT_DIGITAL              AE::PT_DIGITAL

    #define NOT_A_PIN               AE::PN_NOT_A_PIN

    #define PRINT_DEBUG_VALUE(vName, vValue)          AE::ardoDebugValue(vName, vValue, ARDO_LOG_FUNCT);
    #define ARDO_DEBUG_MESSAGE(type, msg)             AE::ardoDebugMessage(type, msg, ARDO_LOG_FUNCT);
    #define ARDO_DEBUG_VALUE(vName, vValue)           AE::ardoDebugValue(vName, vValue, ARDO_LOG_FUNCT);
    #define ARDO_TRACE_STEPPER_DIR(sid, value)        AE::ardoTraceStepperDir(sid, value);
    #define ARDO_TRACE_STEPPER_POS(sid, value)        AE::ardoTraceStepperPos(sid, value);
    
    #define bitRead(value, bit) (((value) >> (bit)) & 0x01)

  #else
    // Arduino Sketch Environment
    #define AE
    
    static const short PN_NOT_A_PIN = NOT_A_PIN;
    enum PinName   { PN_NULL=PN_NOT_A_PIN, 
                     PN_D0=0, PN_D1=1, PN_D2=3, PN_D3=3, PN_D4=4, PN_D5=5, 
                     PN_D6=6, PN_D7=7, PN_D8=8, PN_D9=9, PN_D10=10, 
                     PN_D11=11, PN_D12=12, PN_D13=13,
                     PN_A0=A0, PN_A1=A1, PN_A2=A2, PN_A3=A3, PN_A4=A4, PN_A5=A5};  

    enum PinLevel  { PL_UNDEFINED = -1, PL_LOW = LOW, PL_HIGH = HIGH };
    enum PinMode   { PM_INPUT = INPUT, PM_OUTPUT = OUTPUT, PM_INPUT_PULLUP = INPUT_PULLUP };
    enum PinType   { PT_UNKNOWN = 0, PT_ANALOG = 1, PT_DIGITAL = 2 };

    template<typename T>
    void printDebugValue(const char* vName, T vValue) {
      Serial.write(RET_SOH);
      Serial.write(PID_MSG);
      Serial.write(MT_DEBUG);
  
      Serial.print(vName); 
      Serial.print('='); 
      Serial.print(vValue);
    
      Serial.write(MBYTE_CLOSE);
      Serial.flush();
    }

    // addition helper functions
    #define PRINT_DEBUG_VALUE(vName, vValue)    printDebugValue(vName, vValue);
    #define ARDO_DEBUG_MESSAGE(a,b)             // to eliminate this within the arduino context
    #define ARDO_DEBUG_VALUE(a,b)               // to eliminate this within the arduino context
    #define ARDO_TRACE_STEPPER_DIR(a, b)        // to eliminate this within the arduino context
    #define ARDO_TRACE_STEPPER_POS(a, b)        // to eliminate this within the arduino context

  #endif // #ifndef SKETCH_COMPILE

  const unsigned int MinPinNameValue = (unsigned int)AE::PN_D1;
  const unsigned int MaxPinNameValue = (unsigned int)AE::PN_A5;
  
#endif
