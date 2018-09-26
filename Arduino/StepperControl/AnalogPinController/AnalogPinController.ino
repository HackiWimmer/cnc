#define SKETCH_COMPILE = TRUE

#include <Wire.h>
#include "CommonDefinitions.h"
#include "CncInterface.h"
#include "LimitStates.h"
#include "SupportStates.h"

// ------------------------------------------------------------
// globals
bool                        debugMode;
LimitSwitch::LimitStates    limitStates;
SupportPin::SupportStates   supportStates;

//-------------------------------------------------------------
void receiveEvent(int bytes) {
  if ( bytes == 0 )
    return;

  byte cmd = Wire.read();
  switch ( cmd ) {
    case CMD_RESET:   reset();
                      break;
    default:          ;
  }
}

//-------------------------------------------------------------
void requestEvent() {
  byte data[I2C_BYTE_COUNT];
  data[I2C_BYTE_SUPPORT_STATE] = (byte)supportStates.getValue();
  data[I2C_BYTE_LIMIT_STATE]   = (byte)limitStates.getValue();
  
  Wire.write(data, I2C_BYTE_COUNT);    
}

//-------------------------------------------------------------
inline void debugOnDemand() {
  if ( debugMode == false )
  return;
  
  limitStates.trace();
  supportStates.trace();
  delay(1000);
}

//-------------------------------------------------------------
void reset() {
   supportStates.reset();
   limitStates.reset();
}

//-------------------------------------------------------------
void setup() {
  Serial.begin(57600);
  Serial.setTimeout(1000);
  Serial.flush();

  if ( PIN_SUPPORT_SWITCH1_STATE > 0 )    pinMode(PIN_SUPPORT_SWITCH1_STATE,    OUTPUT);
  if ( PIN_SUPPORT_SWITCH2_STATE > 0 )    pinMode(PIN_SUPPORT_SWITCH2_STATE,    OUTPUT);
  if ( PIN_SUPPORT_TOOL_POWERED > 0 )     pinMode(PIN_SUPPORT_TOOL_POWERED,     OUTPUT);
  if ( PIN_SUPPORT_CABLE_CONNECTED > 0 )  pinMode(PIN_SUPPORT_CABLE_CONNECTED,  OUTPUT);

  Wire.begin(I2C_DEVICE_ID);          
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

  debugMode = false;

  supportStates.init();
  supportStates.reset();

  limitStates.init();
  limitStates.reset();
}
//-------------------------------------------------------------
void loop() {

  // For debugging only
  if ( Serial.available() > 0 ) {
    byte c = Serial.read();

    // For debugging only
    switch ( c ) {
      case 'd':
      case 'D':   debugMode = true; 
                  break;
                  
      case '\n':  // to handle <CR> too
      case '\r':  // to handle <CR> too
                  break;
                  
      default:    debugMode = false;
    }
  }

  // evaluate states for the I2C communication
  limitStates.evaluate();
  supportStates.evaluate();

  // create additional output information
  CncInterface::ISP::States states(supportStates.getValue());
  if ( PIN_SUPPORT_SWITCH1_STATE > 0 )    digitalWrite(PIN_SUPPORT_SWITCH1_STATE,   states.isSupportSwitch1Pressed());
  if ( PIN_SUPPORT_SWITCH2_STATE > 0 )    digitalWrite(PIN_SUPPORT_SWITCH2_STATE,   states.isSupportSwitch2Pressed()); 
  if ( PIN_SUPPORT_TOOL_POWERED > 0 )     digitalWrite(PIN_SUPPORT_TOOL_POWERED,    states.isToolPowered());
  if ( PIN_SUPPORT_CABLE_CONNECTED > 0 )  digitalWrite(PIN_SUPPORT_CABLE_CONNECTED, states.isCableConnected());

  debugOnDemand();

  //some relaxation
  delay(1);
}

