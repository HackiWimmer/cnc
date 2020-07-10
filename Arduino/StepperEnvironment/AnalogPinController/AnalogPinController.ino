#define SKETCH_COMPILE = TRUE

#include <Wire.h>
#include "CommonDefinitions.h"
#include "CncInterface.h"
#include "LimitStates.h"
#include "SupportStates.h"

// ------------------------------------------------------------
// globals
namespace GBL {
  bool                        debugMode;
  bool                        externalInterrruptState;
  
  unsigned int                lastLimitStatesValue;
  unsigned int                lastSupportStatesValue;

  LimitSwitch::LimitStates    limitStates;
  SupportPin::SupportStates   supportStates;
}

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
  data[I2C_BYTE_SUPPORT_STATE] = (byte)GBL::supportStates.getValue();
  data[I2C_BYTE_LIMIT_STATE]   = (byte)GBL::limitStates.getValue();
  
  Wire.write(data, I2C_BYTE_COUNT);    
}

//-------------------------------------------------------------
inline void debugOnDemand() {
  if ( GBL::debugMode == false )
    return;

  Serial.print("EIS=");
  Serial.print(digitalRead(PIN_BROADCAST_INTERRUPT));
  Serial.print(", I=");
  Serial.println(GBL::externalInterrruptState);
  
  GBL::limitStates.trace();
  //supportStates.trace();
}

//-------------------------------------------------------------
void reset() {
  GBL::supportStates.reset();
  GBL::limitStates.reset();
  
  GBL::lastSupportStatesValue = GBL::supportStates.getValue();
  GBL::lastLimitStatesValue   = GBL::limitStates.getValue();
}

//-------------------------------------------------------------
void setup() {
  Serial.begin(57600);
  Serial.setTimeout(1000);
  Serial.flush();

  Wire.begin(I2C_DEVICE_ID);          
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

  //disable SPI to used this port otherwise
  SPCR &= 0B10111111;  

  GBL::debugMode = false;

  pinMode(PIN_BROADCAST_INTERRUPT,   OUTPUT);  digitalWrite(PIN_BROADCAST_INTERRUPT, LimitSwitch::EXTERNAL_INTERRUPT_OFF);

  GBL::supportStates.init();
  GBL::limitStates.init();
  
  reset();
}

//-------------------------------------------------------------
void loop() {

  // For debugging only
  if ( Serial.available() > 0 ) {
    byte c = Serial.read();

    // For debugging only
    switch ( c ) {
      case 'd':
      case 'D':   GBL::debugMode = true; 
                  break;
                  
      case '\n':  // to handle <CR> too
      case '\r':  // to handle <CR> too
                  break;
                  
      default:    GBL::debugMode = false;
    }
  }

  // evaluate states for the I2C communication
  GBL::limitStates.evaluate();
  GBL::supportStates.evaluate();

  const CncInterface::ILS::States ls(GBL::limitStates.getValue());
  GBL::externalInterrruptState = ls.hasErrors() ? LimitSwitch::EXTERNAL_INTERRUPT_ON 
                                                : LimitSwitch::EXTERNAL_INTERRUPT_OFF;
  
  digitalWrite(PIN_BROADCAST_INTERRUPT, GBL::externalInterrruptState);

  if (   GBL::lastLimitStatesValue   != GBL::limitStates.getValue() 
      || GBL::lastSupportStatesValue != GBL::supportStates.getValue() ) {
    debugOnDemand();
  }

  GBL::lastSupportStatesValue = GBL::supportStates.getValue();
  GBL::lastLimitStatesValue   = GBL::limitStates.getValue();
}
