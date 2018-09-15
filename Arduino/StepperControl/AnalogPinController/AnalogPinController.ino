#define SKETCH_COMPILE = TRUE

#include <Wire.h>
#include "CommonDefinitions.h"
#include "LimitStates.h"
#include "SupportStates.h"

// ------------------------------------------------------------
// globals
bool                        debugMode;
LimitSwitch::LimitStates    limitStates;
SupportPin::SupportStates   supportStates;

//-------------------------------------------------------------
void requestEvent() {
  byte data[I2C_BYTE_COUNT];
  data[I2C_BYTE_SUPPORT_STATE] = (byte)supportStates.getValue();
  data[I2C_BYTE_LIMIT_STATE]   = (byte)limitStates.getValue();
  
  Wire.write(data, I2C_BYTE_COUNT);    
}

//-------------------------------------------------------------
void setup() {
  Serial.begin(57600);
  Serial.setTimeout(1000);
  Serial.flush();

  Wire.begin(I2C_DEVICE_ID);          
  Wire.onRequest(requestEvent);

  debugMode = false;

  supportStates.init();
  supportStates.reset();

  limitStates.init();
  limitStates.reset();

 //TODO
  pinMode(13, INPUT);
}
//-------------------------------------------------------------
void loop() {

  // For debugging only
  if ( Serial.available() > 0 ) {
    byte c = Serial.read();

    switch ( c ) {
      case 'd':
      case 'D':   debugMode = true; 
                  break;
      //default:    debugMode = false;
    }
  }

  limitStates.evaluate();
  supportStates.evaluate();

  // For debugging only
  if ( debugMode == true ) {
      limitStates.trace();
      supportStates.trace();
      delay(1000);
  }

  delay(1);
}

