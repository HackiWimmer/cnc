#include <SoftwareSerial.h>
#include "ArdoErr.h"
#include "MainLoop.h"

///////////////////////////////////////////////////////
// static initialization:
  const char*    LastErrorCodes::messageText    =    "";
  unsigned char  LastErrorCodes::register1Byte_A = '\0';
  int32_t        LastErrorCodes::register4Byte_A =    0;
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void LastErrorCodes::writeToSerial() {
///////////////////////////////////////////////////////
  if ( has1ByteInfos() == true ) {
      Serial.print('[');
      Serial.print(register1Byte_A); Serial.print(COMMA);
      Serial.print(']');
  }
    
  if ( has4ByteInfos() == true ) {
      Serial.print('[');
      Serial.print(register4Byte_A); Serial.print(COMMA);
      Serial.print(']');
  }

  if ( has1TextInfo() == true ) {
    Serial.print(BLANK);
    Serial.print(messageText);
  }
}
