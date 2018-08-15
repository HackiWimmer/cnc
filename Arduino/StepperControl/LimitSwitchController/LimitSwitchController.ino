#define SKETCH_COMPILE = TRUE
#include "LimitSwitchDefinitions.h"

// ------------------------------------------------------------
// Limit Pins

  #define PIN_LS_START              3
  #define PIN_LS_END                8
  // --> results to  PIN_LS_X_MIN   3
  // --> results to  PIN_LS_X_MAX   4
  // --> results to  PIN_LS_Y_MIN   5
  // --> results to  PIN_LS_Y_MAX   6
  // --> results to  PIN_LS_Z_MIN   7
  // --> results to  PIN_LS_Z_MAX   8
 
  // only analog output pins are valid here
  #define PIN_LS_ANALOG_OUT         9

// ------------------------------------------------------------
// globals
 
  LimitSwitch::LimitStates limitStates;
 
//-------------------------------------------------------------
// for debugging only
//
inline void reportLimitPins()
{
  Serial.print("------------------------------------------------\n");
  Serial.print("Limit Switch Report:");
  Serial.print("\n");

  Serial.print("LimitStates Value: ");
  Serial.print(limitStates.getLimitValue());
  Serial.print("\n");
  Serial.print(limitStates.getLimitValueAsString());
  Serial.print("\n");

  short byteIndex = LS_BYTE_COUNT - 1;
  for (int pin = PIN_LS_START; pin <= PIN_LS_END; pin++)
  {
    // safty check
    if ( byteIndex < LS_BYTE_IDX_Z_MAX )
      break;
   
    Serial.print(pin);
    Serial.print(" ");
    
    switch ( pin ) {
      case PIN_LS_START + 0:  Serial.print("Xmin: "); break;
      case PIN_LS_START + 1:  Serial.print("Xmax: "); break;
      case PIN_LS_START + 2:  Serial.print("Ymin: "); break;
      case PIN_LS_START + 3:  Serial.print("Ymax: "); break;
      case PIN_LS_START + 4:  Serial.print("Zmin: "); break;
      case PIN_LS_START + 5:  Serial.print("Zmax: "); break;
      default:                Serial.print("Unknown pin: ");
    }
  
    Serial.print(digitalRead(pin) == LIMIT_SWITCH_ON);
    Serial.print("\n");
      
    byteIndex--;
  }   
}

//-------------------------------------------------------------
inline void encodeLimitStates()
{
  limitStates.reset();
  
  int states      = 0; 
  short byteIndex = LS_BYTE_COUNT - 1;
 
  // over all limit switch pins
  for (int pin = PIN_LS_START; pin <= PIN_LS_END; pin++)
  {
    // safty check
    if ( byteIndex < LS_BYTE_IDX_Z_MAX )
      break;
 
    if ( digitalRead(pin) == LIMIT_SWITCH_ON )  
      states |= 1 << byteIndex;
   
    byteIndex--;
  }

  limitStates.setLimitValue(states);
 
  // now the bits of LimitStates contaning the following information
  // for more information plese see LimitSwitchDefinitions::LS_FLAG_*
  //
  //  128 = Xmin
  //   64 = Xmax
  //   32 = Ymin
  //   16 = Ymax
  //    8 = Zmin
  //    4 = Zmax
  //    2 = 0
  //    1 = 0
}

//-------------------------------------------------------------
inline void writeLimitStates()
{
  analogWrite(PIN_LS_ANALOG_OUT, limitStates.getLimitValue());
}

//-------------------------------------------------------------
void setup()
{
  Serial.begin(57600);
  Serial.setTimeout(1000);
  Serial.flush();
  
  for (int pin = PIN_LS_START; pin <= PIN_LS_END; pin++) {
    pinMode(pin, INPUT);
    digitalWrite(pin, LIMIT_SWITCH_OFF);    
  }

  limitStates.reset();
  analogWrite(PIN_LS_ANALOG_OUT, limitStates.getLimitValue());
}

//-------------------------------------------------------------
void loop()
{
  // For debugging only
  if ( Serial.available() > 0 ) {
    byte c = Serial.read();

    if ( c != '\0' ) {
      reportLimitPins();
    }
  }

  // Normal processing
  encodeLimitStates();
  writeLimitStates();
}

