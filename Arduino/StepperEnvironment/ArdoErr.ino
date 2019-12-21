#include <SoftwareSerial.h>
#include "ArdoErr.h"
#include "MainLoop.h"

///////////////////////////////////////////////////////
// static initialization:
  const char*    LastErrorCodes::messageText    =    "";
  
  unsigned char  LastErrorCodes::register1Byte_A = '\0';
  unsigned char  LastErrorCodes::register1Byte_B = '\0';
  unsigned char  LastErrorCodes::register1Byte_C = '\0';
  unsigned char  LastErrorCodes::register1Byte_D = '\0';
  unsigned char  LastErrorCodes::register1Byte_E = '\0';
  unsigned char  LastErrorCodes::register1Byte_F = '\0';
  unsigned char  LastErrorCodes::register1Byte_G = '\0';
  unsigned char  LastErrorCodes::register1Byte_H = '\0';
  
  int32_t        LastErrorCodes::register4Byte_A =    0;
  int32_t        LastErrorCodes::register4Byte_B =    0;
  int32_t        LastErrorCodes::register4Byte_C =    0;
  int32_t        LastErrorCodes::register4Byte_D =    0;
  int32_t        LastErrorCodes::register4Byte_E =    0;
  int32_t        LastErrorCodes::register4Byte_F =    0;
  int32_t        LastErrorCodes::register4Byte_G =    0;
  int32_t        LastErrorCodes::register4Byte_H =    0;
  
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void LastErrorCodes::writeToSerial() {
///////////////////////////////////////////////////////
  if ( has1ByteInfos() == true ) {
      Serial.print('[');
      Serial.print(register1Byte_A); Serial.print(COMMA);
      Serial.print(register1Byte_B); Serial.print(COMMA);
      Serial.print(register1Byte_C); Serial.print(COMMA);
      Serial.print(register1Byte_D); Serial.print(COMMA);
      Serial.print(register1Byte_E); Serial.print(COMMA);
      Serial.print(register1Byte_F); Serial.print(COMMA);
      Serial.print(register1Byte_G); Serial.print(COMMA);
      Serial.print(register1Byte_H);
      Serial.print(']');
  }
    
  if ( has4ByteInfos() == true ) {
      Serial.print('[');
      Serial.print(register4Byte_A); Serial.print(COMMA);
      Serial.print(register4Byte_B); Serial.print(COMMA);
      Serial.print(register4Byte_C); Serial.print(COMMA);
      Serial.print(register4Byte_D); Serial.print(COMMA);
      Serial.print(register4Byte_E); Serial.print(COMMA);
      Serial.print(register4Byte_F); Serial.print(COMMA);
      Serial.print(register4Byte_G); Serial.print(COMMA);
      Serial.print(register4Byte_H);
      Serial.print(']');
  }

  if ( has1TextInfo() == true ) {
    Serial.print(BLANK);
    Serial.print(messageText);
  }
}
