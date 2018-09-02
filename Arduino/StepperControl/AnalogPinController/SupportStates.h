#ifndef SUPPORT_PIN_DEFINITIONS_H
#define SUPPORT_PIN_DEFINITIONS_H

#include "CommonDefinitions.h"
#include "CncInterface.h"
#include "StateBuffer.h"

namespace SupportPin {

  const SHORT PIN_SS_SUPPORT_BUTTON_1_PRESSED    =  10;
  const SHORT PIN_SS_SUPPORT_BUTTON_2_PRESSED    =  11;
  const SHORT PIN_SS_SUPPORT_BUTTON_3_PRESSED    =  12;
  const SHORT PIN_SS_PIN_5                       =  13;
  const SHORT PIN_SS_PIN_4                       =  A0;
  const SHORT PIN_SS_PIN_3                       =  A1;
  const SHORT PIN_SS_TOOL_POWERED                =  A2;
  const SHORT PIN_SS_CABLE_CONNECTED             =  A3;

  //-----------------------------------------------------------------
  class SupportStates : public StateBuffer {

    public:
      SupportStates() 
      : StateBuffer()
      {}

      explicit SupportStates(const unsigned int value) 
      : StateBuffer(value)
      {}

      //-------------------------------------------------------------
      virtual void init() {
        initInputPin(PIN_SS_SUPPORT_BUTTON_1_PRESSED,   true);
        initInputPin(PIN_SS_SUPPORT_BUTTON_2_PRESSED,   true);
        initInputPin(PIN_SS_SUPPORT_BUTTON_3_PRESSED,   true);
        initInputPin(PIN_SS_PIN_5,                      true);
        initInputPin(PIN_SS_PIN_4,                      true);
        initInputPin(PIN_SS_PIN_3,                      true);
        initInputPin(PIN_SS_TOOL_POWERED,               true);
        initInputPin(PIN_SS_CABLE_CONNECTED,            true);
      }

      //-------------------------------------------------------------
      virtual void evaluate() {

        setBit(BIT_IS_SUPPORT_BUTTON_1_PRESSED, readPin(PIN_SS_SUPPORT_BUTTON_1_PRESSED));
        setBit(BIT_IS_SUPPORT_BUTTON_2_PRESSED, readPin(PIN_SS_SUPPORT_BUTTON_2_PRESSED));
        setBit(BIT_IS_SUPPORT_BUTTON_3_PRESSED, readPin(PIN_SS_SUPPORT_BUTTON_3_PRESSED));
        setBit(BIT_5,                           true);
        setBit(BIT_4,                           true);
        setBit(BIT_3,                           true);
        setBit(BIT_IS_TOOL_POWERED,             readPin(PIN_SS_TOOL_POWERED));
        setBit(BIT_IS_CABLE_CONNECTED,          readPin(PIN_SS_CABLE_CONNECTED));
      }

      //-------------------------------------------------------------
      virtual void report() {
        headline();
        Serial.print(CR);
        Serial.print("SP Report:");
        Serial.print(CR);
      
        Serial.print("SP Value: ");
        Serial.print(getValue());
        Serial.print(BLANK3);
        Serial.print("'");
        Serial.print(getValueAsString());
        Serial.print("'");
        Serial.print(CR);
      
        Serial.print("PIN Values: ");
        Serial.print(CR);
        Serial.print(PIN_SS_SUPPORT_BUTTON_1_PRESSED);  Serial.print(EQUAL); Serial.print(readPin(PIN_SS_SUPPORT_BUTTON_1_PRESSED));  Serial.print(CR);
        Serial.print(PIN_SS_SUPPORT_BUTTON_2_PRESSED);  Serial.print(EQUAL); Serial.print(readPin(PIN_SS_SUPPORT_BUTTON_2_PRESSED));  Serial.print(CR);
        Serial.print(PIN_SS_SUPPORT_BUTTON_3_PRESSED);  Serial.print(EQUAL); Serial.print(readPin(PIN_SS_SUPPORT_BUTTON_3_PRESSED));  Serial.print(CR);
        Serial.print(PIN_SS_PIN_5);                     Serial.print(EQUAL); Serial.print(readPin(PIN_SS_PIN_5));                     Serial.print(CR);
        Serial.print(PIN_SS_PIN_4);                     Serial.print(EQUAL); Serial.print(readPin(PIN_SS_PIN_4));                     Serial.print(CR); 
        Serial.print(PIN_SS_PIN_3);                     Serial.print(EQUAL); Serial.print(readPin(PIN_SS_PIN_3));                     Serial.print(CR);
        Serial.print(PIN_SS_TOOL_POWERED);              Serial.print(EQUAL); Serial.print(readPin(PIN_SS_TOOL_POWERED));              Serial.print(CR);
        Serial.print(PIN_SS_CABLE_CONNECTED);           Serial.print(EQUAL); Serial.print(readPin(PIN_SS_CABLE_CONNECTED));           Serial.print(CR);  
      }

 };
    
} // namespace

#endif
