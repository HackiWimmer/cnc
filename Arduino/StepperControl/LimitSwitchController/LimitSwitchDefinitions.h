#ifndef LIMIT_SWITCH_DEFINITIONS_H
#define LIMIT_SWITCH_DEFINITIONS_H

#include "CommonDefinitions.h"
#include "ByteState.h"

namespace LimitSwitch {

  const SHORT PIN_LS_X_MIN            =  2;
  const SHORT PIN_LS_X_MAX            =  3;
  const SHORT PIN_LS_Y_MIN            =  4;
  const SHORT PIN_LS_Y_MAX            =  5;
  const SHORT PIN_LS_Z_MIN            =  6;
  const SHORT PIN_LS_Z_MAX            =  7;

  // 11111101
  const SHORT LS_ERROR_STATE          = 253;

  //-----------------------------------------------------------------
  class LimitStates : public ByteState {

    private:

      //-------------------------------------------------------------
      inline long evaluateLimit(bool minValue, bool maxValue) const {
        if ( minValue && maxValue)  return LIMIT_UNKNOWN;
        if ( minValue )             return LIMIT_MIN;
        if ( maxValue )             return LIMIT_MAX;

        return LIMIT_UNSET;
      }

    public:
      LimitStates() 
      : ByteState()
      {}

      explicit LimitStates(const int value) 
      : ByteState(value)
      {}

      //-------------------------------------------------------------
      virtual void init() {
        initInputPin(PIN_LS_X_MIN, LIMIT_SWITCH_OFF);
        initInputPin(PIN_LS_X_MAX, LIMIT_SWITCH_OFF);
        initInputPin(PIN_LS_Y_MIN, LIMIT_SWITCH_OFF);
        initInputPin(PIN_LS_Y_MAX, LIMIT_SWITCH_OFF);
        initInputPin(PIN_LS_Z_MIN, LIMIT_SWITCH_OFF);
        initInputPin(PIN_LS_Z_MAX, LIMIT_SWITCH_OFF);
      }

      //-------------------------------------------------------------
      virtual void evaluate() {
        setBit8(readPin(PIN_LS_X_MIN) == LIMIT_SWITCH_ON);
        setBit7(readPin(PIN_LS_X_MAX) == LIMIT_SWITCH_ON);
      
        setBit6(readPin(PIN_LS_Y_MIN) == LIMIT_SWITCH_ON);
        setBit5(readPin(PIN_LS_Y_MIN) == LIMIT_SWITCH_ON);
        setBit4(readPin(PIN_LS_Z_MIN) == LIMIT_SWITCH_ON);
        setBit3(readPin(PIN_LS_Z_MAX) == LIMIT_SWITCH_ON);
      }
      
      //-------------------------------------------------------------
      virtual void report() {
        headline();
        Serial.print(CR);
        Serial.print("LS Report:");
        Serial.print(CR);
      
        Serial.print("LS Value: ");
        Serial.print(getValue());
        Serial.print(CR);
        Serial.print(getValueAsString());
        Serial.print(CR);
      
        Serial.print("PIN Values: ");
        Serial.print(PIN_LS_X_MIN); Serial.print(BLANK); Serial.print(readPin(PIN_LS_X_MIN)); Serial.print(CR);
        Serial.print(PIN_LS_X_MAX); Serial.print(BLANK); Serial.print(readPin(PIN_LS_X_MAX)); Serial.print(CR);
        Serial.print(PIN_LS_Y_MIN); Serial.print(BLANK); Serial.print(readPin(PIN_LS_Y_MIN)); Serial.print(CR);
        Serial.print(PIN_LS_Y_MAX); Serial.print(BLANK); Serial.print(readPin(PIN_LS_Y_MAX)); Serial.print(CR);
        Serial.print(PIN_LS_Z_MIN); Serial.print(BLANK); Serial.print(readPin(PIN_LS_Z_MIN)); Serial.print(CR);
        Serial.print(PIN_LS_Z_MAX); Serial.print(BLANK); Serial.print(readPin(PIN_LS_Z_MAX)); Serial.print(CR);
      }
      
      //-------------------------------------------------------------
      void resetToError()     { setValue(LS_ERROR_STATE); }

      //-------------------------------------------------------------
      bool xMin() const       { return getBit8(); }
      bool xMax() const       { return getBit7(); }
      bool yMin() const       { return getBit6(); }
      bool yMax() const       { return getBit5(); }
      bool zMin() const       { return getBit4(); }
      bool zMax() const       { return getBit3(); }

      long xLimit() const     { return evaluateLimit(xMin(), xMax()); }
      long yLimit() const     { return evaluateLimit(yMin(), yMax()); }
      long zLimit() const     { return evaluateLimit(zMin(), zMax()); }

      //-------------------------------------------------------------
      bool hasError() const   { 
        if ( xMin() && xMax() ) return true;
        if ( yMin() && yMax() ) return true;
        if ( zMin() && zMax() ) return true;

        return false;
      }

  private:
  
      //-------------------------------------------------------------
      void setLimitAsValue(unsigned int value) { setValue(value); }

      //-------------------------------------------------------------
      virtual const char* getValueAsString() {
        memset(&stringValue, '0', sizeof(stringValue));
        
        if ( xMin() )     stringValue[0] = '1';
        if ( xMax() )     stringValue[1] = '1';
      
        if ( yMin() )     stringValue[2] = '1';
        if ( yMax() )     stringValue[3] = '1';
       
        if ( zMin() )     stringValue[4] = '1';
        if ( zMax() )     stringValue[5] = '1';
       
        if ( hasError() ) stringValue[7] = '1';
        
        return stringValue;
      }
      
  };
} // namespace
  
#endif
