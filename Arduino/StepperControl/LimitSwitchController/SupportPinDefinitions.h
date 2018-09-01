#ifndef SUPPORT_PIN_DEFINITIONS_H
#define SUPPORT_PIN_DEFINITIONS_H

#include "CommonDefinitions.h"
#include "ByteState.h"

namespace SupportPin {

  const SHORT PIN_SS_PIN_1                       =  A0;
  const SHORT PIN_SS_PIN_2                       =  A1;
  const SHORT PIN_SS_PIN_3                       =  A2;
  const SHORT PIN_SS_PIN_4                       =  A3;
  const SHORT PIN_SS_PIN_5                       =  10;
  const SHORT PIN_SS_PIN_6                       =  11;
  const SHORT PIN_SS_PIN_7                       =  12;
  const SHORT PIN_SS_PIN_8                       =  13;

  const SHORT BIT_IS_SUPPORT_BUTTON_PRESSED      =   8;
  const SHORT BIT_IS_TOOL_POWERED                =   7;
  const SHORT BIT_IS_CABLE_CONNECTED             =   6;

  //-----------------------------------------------------------------
  class SupportStates : public ByteState {

    public:
      SupportStates() 
      : ByteState()
      {}

      explicit SupportStates(const unsigned int value) 
      : ByteState(value)
      {}

      //-------------------------------------------------------------
      virtual void init() {
        initInputPin(PIN_SS_PIN_1, true);
        initInputPin(PIN_SS_PIN_2, true);
        initInputPin(PIN_SS_PIN_3, true);
        initInputPin(PIN_SS_PIN_4, true);
        initInputPin(PIN_SS_PIN_5, true);
        initInputPin(PIN_SS_PIN_6, true);
        initInputPin(PIN_SS_PIN_7, true);
        initInputPin(PIN_SS_PIN_8, true);
      }

      //-------------------------------------------------------------
      virtual void evaluate() {
        setSupportButtonPressed(  readPin(PIN_SS_PIN_1) == false);
        setToolPowered(           readPin(PIN_SS_PIN_2) == false);
        setCableConnected(        readPin(PIN_SS_PIN_3) == true);
      }

      //-------------------------------------------------------------
      virtual void report() {
        headline();
        Serial.print(CR);
        Serial.print("SP Report:");
        Serial.print(CR);
      
        Serial.print("SP Value: ");
        Serial.print(getValue());
        Serial.print(CR);
        Serial.print(getValueAsString());
        Serial.print(CR);
      
        Serial.print("PIN Values: ");
        Serial.print(PIN_SS_PIN_1); Serial.print(BLANK); Serial.print(readPin(PIN_SS_PIN_1)); Serial.print(CR);
        Serial.print(PIN_SS_PIN_2); Serial.print(BLANK); Serial.print(readPin(PIN_SS_PIN_2)); Serial.print(CR);
        Serial.print(PIN_SS_PIN_3); Serial.print(BLANK); Serial.print(readPin(PIN_SS_PIN_3)); Serial.print(CR);
        Serial.print(PIN_SS_PIN_4); Serial.print(BLANK); Serial.print(readPin(PIN_SS_PIN_4)); Serial.print(CR);
        
        Serial.print(PIN_SS_PIN_5); Serial.print(BLANK); Serial.print(readPin(PIN_SS_PIN_5)); Serial.print(CR); 
        Serial.print(PIN_SS_PIN_6); Serial.print(BLANK); Serial.print(readPin(PIN_SS_PIN_6)); Serial.print(CR);
        Serial.print(PIN_SS_PIN_7); Serial.print(BLANK); Serial.print(readPin(PIN_SS_PIN_7)); Serial.print(CR);
        Serial.print(PIN_SS_PIN_8); Serial.print(BLANK); Serial.print(readPin(PIN_SS_PIN_8)); Serial.print(CR);  
      }

      //-------------------------------------------------------------
      bool isSupportButtonPressed()               { return getBit(BIT_IS_SUPPORT_BUTTON_PRESSED);   }
      bool isToolPowered()                        { return getBit(BIT_IS_TOOL_POWERED);             }
      bool isCableConnected()                     { return getBit(BIT_IS_CABLE_CONNECTED);          }
      // add more named getters on demand . . .

    private:      
    
      //-------------------------------------------------------------
      void setSupportButtonPressed(bool value)    { setBit(BIT_IS_SUPPORT_BUTTON_PRESSED, value);   }
      void setToolPowered(bool value)             { setBit(BIT_IS_TOOL_POWERED, value);             }
      void setCableConnected(bool value)          { setBit(BIT_IS_CABLE_CONNECTED, value);          }
      // add more named setters on demand . . .

 };
    
} // namespace

#endif
