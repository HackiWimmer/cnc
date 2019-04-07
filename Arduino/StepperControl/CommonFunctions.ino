#include <Wire.h>
#include "CommonValues.h"

unsigned char  LastErrorCodes::register1Byte_A;
unsigned char  LastErrorCodes::register1Byte_B;
unsigned char  LastErrorCodes::register1Byte_C;
unsigned char  LastErrorCodes::register1Byte_D;

int32_t        LastErrorCodes::register4Byte_A;
int32_t        LastErrorCodes::register4Byte_B;
int32_t        LastErrorCodes::register4Byte_C;
int32_t        LastErrorCodes::register4Byte_D;

//////////////////////////////////////////////////////////////
// "Software Reset" function
void (*softwareReset)(void)=0;
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
PinType getPinType(uint8_t pin) {
//////////////////////////////////////////////////////////////
uint8_t port = digitalPinToPort(pin);

if ( port == NOT_A_PIN ) 
  return PT_UNKNOWN;

    // this is uno specific!
    switch ( pin ) {
      case A0: 
      case A1: 
      case A2:
      case A3:
      case A4: 
      case A5: return PT_ANALOG;
    }
    
    return PT_DIGITAL;
}
//////////////////////////////////////////////////////////////
uint8_t getPinMode(uint8_t pin) {
//////////////////////////////////////////////////////////////
  uint8_t bit  = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);

  if ( port == NOT_A_PIN ) 
    return UNKNOWN_PIN;

  // Is there a bit we can check?
  if ( bit == 0 ) 
    return UNKNOWN_PIN;

  // Is there only a single bit set?
  if ( bit & (bit - 1) ) 
    return UNKNOWN_PIN;

  volatile uint8_t *reg, *out;
  reg = portModeRegister(port);
  out = portOutputRegister(port);

  if      (*reg & bit)  return OUTPUT;
  else if (*out & bit)  return INPUT_PULLUP;
  else                  return INPUT;

  return UNKNOWN_PIN;
}
//////////////////////////////////////////////////////////////
void printDigitalPin(const unsigned char Pin, const int Mode) {
//////////////////////////////////////////////////////////////
  int Type = (int)'D'; 
  Serial.print(Pin);  Serial.print(TEXT_SEPARATOR); 
  Serial.print(Type); Serial.print(TEXT_SEPARATOR);
  Serial.print(Mode); Serial.print(TEXT_SEPARATOR);
  Serial.print(digitalRead(Pin));
  Serial.print(TEXT_CLOSE); 
}
//////////////////////////////////////////////////////////////    
void printAnalogPin(const unsigned char Pin, const int Mode) {
//////////////////////////////////////////////////////////////
  int Type = (int)'A'; 
  Serial.print(Pin);  Serial.print(TEXT_SEPARATOR); 
  Serial.print(Type); Serial.print(TEXT_SEPARATOR); 
  Serial.print(Mode); Serial.print(TEXT_SEPARATOR); 
  Serial.print(analogRead(Pin)); 
  Serial.print(TEXT_CLOSE); 
}
//////////////////////////////////////////////////////////////
void sleepMicroseconds(unsigned long usec) {
//////////////////////////////////////////////////////////////
  long milli = usec / 1000;
  long micro = usec % 1000;

  if ( milli )
    delay(milli);
    
  delayMicroseconds(micro);
}
//////////////////////////////////////////////////////////////
bool readI2CSlave(I2CData& data) {
//////////////////////////////////////////////////////////////
  Wire.requestFrom(I2C_DEVICE_ID, I2C_BYTE_COUNT);

  if ( Wire.available() <= 0 )
    return false;

  unsigned int counter = 0;
  while ( Wire.available()) {
    switch ( counter ) {
      case I2C_BYTE_LIMIT_STATE:     data.limitState    = Wire.read();  break;
      case I2C_BYTE_SUPPORT_STATE:   data.supportState  = Wire.read();  break;
      default:  return false;
    }
    counter++;
  }

  return true;
}
//////////////////////////////////////////////////////////////
bool sendDataToI2CSlave(const byte data[], unsigned int len) {
//////////////////////////////////////////////////////////////
  if ( len == 0 )
    return false;

  Wire.beginTransmission(I2C_DEVICE_ID);
  int bytes = Wire.write(data, len);            
  byte ret = Wire.endTransmission();  

  /* ret = 
    0: success
    1: data too long to fit in transmit buffer
    2: received NACK on transmit of address
    3: received NACK on transmit of data
    4: other error 
  */
  
  return ( ret == 0 && bytes == (int)len );
}

//////////////////////////////////////////////////////////////
bool dblCompare(double a, double b) {
//////////////////////////////////////////////////////////////
  return (abs(a - b) < epsilon);
}
/////////////////////////////////////////////////////////////////////////////////////
bool dblCompareNull(double a) {
/////////////////////////////////////////////////////////////////////////////////////
   return dblCompare(a, 0.0);
}
/////////////////////////////////////////////////////////////////////////////////////
bool isDoubleValue(const unsigned char pid) {
/////////////////////////////////////////////////////////////////////////////////////
  return  pid >= PID_DOUBLE_RANG_START; 
}
/////////////////////////////////////////////////////////////////////////////////////
bool convertLongToDouble(const long val, double& ret) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( val == MIN_LONG || val == MAX_LONG ) {
    ret = 0.0;
    return false;
  }
  
  ret = ((double)val)/DBL_FACT; 
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
void writeByteValue(unsigned char b) {
/////////////////////////////////////////////////////////////////////////////////////
    Serial.write(b);
}
/////////////////////////////////////////////////////////////////////////////////////
void writeByteValue(unsigned char pid, unsigned char b) {
/////////////////////////////////////////////////////////////////////////////////////  
  Serial.write(RET_SOH);
  Serial.write(pid);
  writeByteValue(b);
}
/////////////////////////////////////////////////////////////////////////////////////
void writeByteValues(unsigned char pid, unsigned char b[], unsigned short size) {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOH);
  Serial.write(pid);
  
  for ( unsigned short i=0; i<size; i++ ) {
    writeByteValue(b[i]);
  }
}
/////////////////////////////////////////////////////////////////////////////////////
void writeGetterValue(unsigned char pid, int32_t val1) {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOH);
  Serial.write(PID_GETTER);
  Serial.write(pid);
  writeByteValue(1);
  writeLongValue(val1);
}
/////////////////////////////////////////////////////////////////////////////////////
void writeGetterValues(unsigned char pid, int32_t val1, int32_t val2) {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOH);
  Serial.write(PID_GETTER);
  Serial.write(pid);
  writeByteValue(2);
  writeLongValue(val1);
  writeLongValue(val2);
}
/////////////////////////////////////////////////////////////////////////////////////
void writeGetterValues(unsigned char pid, int32_t val1, int32_t val2, int32_t val3) {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOH);
  Serial.write(PID_GETTER);
  Serial.write(pid);
  writeByteValue(3);
  writeLongValue(val1);
  writeLongValue(val2);
  writeLongValue(val3);
}
/////////////////////////////////////////////////////////////////////////////////////
void writeUnsignedIntValue(uint16_t val) {
/////////////////////////////////////////////////////////////////////////////////////
  unsigned char buf[sizeof(uint16_t)];
  memcpy(buf, &val, sizeof(uint16_t));
  Serial.write(buf, sizeof(buf));
}
/////////////////////////////////////////////////////////////////////////////////////
void writeDoubleValue(double val) {
/////////////////////////////////////////////////////////////////////////////////////
  unsigned char buf[sizeof(double)];
  memcpy(buf, &val, sizeof(double));
  Serial.write(buf, sizeof(buf));
}
/////////////////////////////////////////////////////////////////////////////////////
void writeLongValue(long val) {
/////////////////////////////////////////////////////////////////////////////////////
  unsigned char buf[sizeof(long)];
  memcpy(buf, &val, sizeof(long));
  Serial.write(buf, sizeof(buf));
}
/////////////////////////////////////////////////////////////////////////////////////
void writeLongValue(unsigned char pid, long val) {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOH);
  Serial.write(pid);
  writeLongValue(val);
}
/////////////////////////////////////////////////////////////////////////////////////
void writeLongValues(unsigned char pid, long val1, long val2) {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOH);
  Serial.write(pid);
  writeLongValue(val1);
  writeLongValue(val2);
}
/////////////////////////////////////////////////////////////////////////////////////
void writeLongValues(unsigned char pid, long val1, long val2, long val3) {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOH);
  Serial.write(pid);
  writeLongValue(val1);
  writeLongValue(val2);
  writeLongValue(val3);
}
/////////////////////////////////////////////////////////////////////////////////////
void writeLongValues(unsigned char pid, long val1, long val2, long val3, long val4) {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOH);
  Serial.write(pid);
  writeLongValue(val1);
  writeLongValue(val2);
  writeLongValue(val3);
  writeLongValue(val4);
}
/////////////////////////////////////////////////////////////////////////////////////
void sendHeartbeat() {
/////////////////////////////////////////////////////////////////////////////////////
  unsigned char byteCount = sizeof(int32_t);
  Serial.write(RET_SOH);
  Serial.write(PID_HEARTBEAT);
  Serial.write(byteCount);
  writeLongValue(millis() % MAX_LONG);
}
/////////////////////////////////////////////////////////////////////////////////////
void sendHeartbeat(unsigned char limitState, unsigned char supportState) {
/////////////////////////////////////////////////////////////////////////////////////
  unsigned char byteCount = 2 * sizeof(int32_t);

  Serial.write(RET_SOH);
  Serial.write(PID_HEARTBEAT);
  Serial.write(byteCount);
  writeLongValue(millis() % MAX_LONG);
  
  for (unsigned int i=0; i<I2C_BYTE_COUNT; i++) {
    switch ( i ) {
      case I2C_BYTE_LIMIT_STATE:    writeByteValue(limitState);   break;
      case I2C_BYTE_SUPPORT_STATE:  writeByteValue(supportState); break;
    }
  }
  
  writeByteValue(255); // reserved
  writeByteValue(255); // reserved
}
/////////////////////////////////////////////////////////////////////////////////////
void pushMessage(const char type, const unsigned char mid, const char* msg) {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOH);
    Serial.write(PID_MSG);
    switch ( type ) {
      case MT_WARNING:  Serial.write(MT_WARNING); break;
      case MT_ERROR:    Serial.write(MT_ERROR);   break;
      default:          Serial.write(MT_INFO);    break; 
    }

    if ( mid != E_NO_ERROR ) {
      Serial.write(MT_MID_FLAG);
      Serial.write(mid);
    }
/*
    if ( LastErrorCodes::hasInfos() == true ) {
      char buf[128];
      
      if ( LastErrorCodes::has1ByteInfos() == true ) {
        sprintf(buf, "\nReg1:[%d,%d,%d,%d]\n",     LastErrorCodes::register1Byte_A, 
                                                   LastErrorCodes::register1Byte_B, 
                                                   LastErrorCodes::register1Byte_C, 
                                                   LastErrorCodes::register1Byte_D);
        Serial.print(buf);
      }
      
      if ( LastErrorCodes::has4ByteInfos() == true ) {
        sprintf(buf, " Reg4:[%ld,%ld,%ld,%ld]\n", LastErrorCodes::register4Byte_A, 
                                                   LastErrorCodes::register4Byte_B, 
                                                   LastErrorCodes::register4Byte_C, 
                                                   LastErrorCodes::register4Byte_D);
        Serial.print(buf);
      }
     
      LastErrorCodes::clear();
    }
*/
    if ( msg != NULL )
      Serial.print(msg);
    
  Serial.write(MBYTE_CLOSE);
  Serial.flush();
}
/////////////////////////////////////////////////////////////////////////////////////
void pushInfoMessage(const unsigned char mid, const char* msg) {
/////////////////////////////////////////////////////////////////////////////////////
  pushMessage(MT_INFO, mid, msg);
}
/////////////////////////////////////////////////////////////////////////////////////
void pushWarningMessage(const unsigned char mid, const char* msg) {
/////////////////////////////////////////////////////////////////////////////////////
  pushMessage(MT_WARNING, mid, msg);
}
/////////////////////////////////////////////////////////////////////////////////////
void pushErrorMessage(const unsigned char mid, const char* msg) {
/////////////////////////////////////////////////////////////////////////////////////
  pushMessage(MT_ERROR, mid, msg);
}
/////////////////////////////////////////////////////////////////////////////////////
void pushInfoMessage(const char* msg) {
/////////////////////////////////////////////////////////////////////////////////////
  pushMessage(MT_INFO, E_NO_ERROR, msg);
}
/////////////////////////////////////////////////////////////////////////////////////
void pushWarningMessage(const char* msg) {
/////////////////////////////////////////////////////////////////////////////////////
  pushMessage(MT_WARNING, E_NO_ERROR, msg);
}
/////////////////////////////////////////////////////////////////////////////////////
void pushErrorMessage(const char* msg) {
/////////////////////////////////////////////////////////////////////////////////////
  pushMessage(MT_ERROR, E_NO_ERROR, msg);
}
/////////////////////////////////////////////////////////////////////////////////////
bool peakSerial(unsigned char& c) {
/////////////////////////////////////////////////////////////////////////////////////
  c = CMD_INVALID;
  
  if ( Serial.available() == 0 )
    return false;
  
  c = Serial.peek();
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool checkSerialForPauseCommands(bool currentPauseState) {
/////////////////////////////////////////////////////////////////////////////////////
    bool ret = currentPauseState;
    
    if ( Serial.available() > 0 ) {
      
      switch ( Serial.peek() ) {
        case 'p': ret = PAUSE_INACTIVE; 
                  // remove the cmd for serial
                  Serial.read(); 
                  break;
                  
        case 'P': ret = PAUSE_ACTIVE;
                  // remove the cmd for serial
                  Serial.read();  
                  break;
      }
    }

    return ret;
}
///////////////////////////////////////////////////////////////
void waitActiveMilliseconds(uint32_t milliseconds) {
///////////////////////////////////////////////////////////////
  if ( milliseconds == 0 )
    return;
    
  uint32_t ts = millis();  
  while ( true ) {

    // The return value of millis() will overflow (go back to zero), 
    // after approximately 50 days
    // In this case restart 
    if ( millis() < ts ) {
      waitActiveMilliseconds(milliseconds);
      break;
    }

    if ( millis() - ts > milliseconds )
      break;
  }
}
///////////////////////////////////////////////////////////////
void waitActiveMicroseconds(uint32_t microseconds) {
///////////////////////////////////////////////////////////////  
  if ( microseconds == 0 )
    return;

  const uint32_t threshold  = 1000;
  uint32_t milliseconds     = microseconds / threshold;
 
  if ( milliseconds > 0 )
    waitActiveMilliseconds(milliseconds);

  uint32_t us = microseconds % threshold;
  uint32_t ts = micros();  

  if ( us <= 10 )
    return;
  
  while ( true ) {

    // The return value of millis() will overflow (go back to zero), 
    // after approximately 50 days
    // In this case restart 
    if ( micros() < ts ) {
      waitActiveMicroseconds(us);
      break;
    }

    if ( micros() - ts > us )
      break;
  }
}
///////////////////////////////////////////////////////////////
int waitForSerialData(uint32_t timeoutMicros) {
///////////////////////////////////////////////////////////////
  if ( Serial.available() > 0 )
    return Serial.available();

  const uint32_t timeout = max(timeoutMicros, minSerialReadTimeoutMicros);

  uint32_t ts = micros();  
  while ( Serial.available() == 0 ) {

    // The return value of micros() will overflow (go back to zero), 
    // after approximately 70 minutes.
    // In this case restart 
    if ( micros() < ts )
      return waitForSerialData(timeoutMicros);

    if ( micros() - ts > timeout )
      return 0;
  }

  return Serial.available();
}
///////////////////////////////////////////////////////////////
byte readSerialByteWithTimeout(uint32_t timeoutMicros) {
////////////////////////////////////////////////////////////////
  byte b[1];
  return readSerialBytesWithTimeout(b, 1, timeoutMicros) == 1 ? b[0] : 0;
}
///////////////////////////////////////////////////////////////
int readSerialBytesWithTimeout(byte* buffer, int length, uint32_t timeoutMicros) {
////////////////////////////////////////////////////////////////
  byte* internalPointer   = buffer;
  int bytesAvailable      = 0;
  int totallyPicked       = 0;

  if ( buffer != NULL ) {
    while ( totallyPicked < length ) {

      // check timeout
      if ( ( bytesAvailable = waitForSerialData(timeoutMicros) ) == 0 )
        break;
        
      // counter management
      const int stillToRead     = length - totallyPicked;
      const int toReadNow       = min(stillToRead, bytesAvailable);

      // read operation
      const int currentlyPicked = Serial.readBytes(internalPointer, toReadNow);
      
      // position / counter management
      internalPointer += currentlyPicked;
      totallyPicked   += currentlyPicked;
    }
  }
    
  return totallyPicked;
}

