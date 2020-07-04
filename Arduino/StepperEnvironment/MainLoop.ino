#include <SoftwareSerial.h>
#include <Wire.h>
#include "ArdoErr.h"
#include "ByteBuf.h"
#include "DecBase.h"
#include "CncCtrl.h"
#include "MainLoop.h"

ByteBuffer            SWB;

#ifndef SKETCH_COMPILE 
  #define CNC_MAIN_LOOP_LOG_FUNCTION()  \
    { \
        const wxString logStr(wxString::Format("MainLoop::%s", __FUNCTION__)); \
        ARDO_DEBUG_MESSAGE('D', logStr); \
    }
#else
  #define CNC_MAIN_LOOP_LOG_FUNCTION()
#endif


/////////////////////////////////////////////////////////////////////////////////////
ArduinoMainLoop::ArduinoMainLoop() 
/////////////////////////////////////////////////////////////////////////////////////
: controller( new CncArduinoController() )
{
}
/////////////////////////////////////////////////////////////////////////////////////
ArduinoMainLoop::~ArduinoMainLoop() {
/////////////////////////////////////////////////////////////////////////////////////
  delete controller; 
  controller = NULL;
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoMainLoop::printSketchVersion() {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOH);
    Serial.write(PID_TEXT);
    Serial.write(FIRMWARE_VERSION);
  Serial.write(MBYTE_CLOSE);
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoMainLoop::printSketchTimestamp() {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOH);
    Serial.write(PID_TEXT);
    Serial.write(__TIMESTAMP__);
  Serial.write(MBYTE_CLOSE);
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoMainLoop::printConfig() {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOH);
    Serial.write(PID_TEXT);
    Serial.print(PID_COMMON);        Serial.print(TEXT_SEPARATOR);     Serial.print(BLANK1);                                            Serial.print(TEXT_CLOSE);
    Serial.print(BLANK1);            Serial.print(PID_VERSION);        Serial.print(TEXT_SEPARATOR); Serial.print(FIRMWARE_VERSION);    Serial.print(TEXT_CLOSE);

    controller->printConfig();

  Serial.write(MBYTE_CLOSE);
}
//////////////////////////////////////////////////////////////
void ArduinoMainLoop::printDigitalPin(const unsigned char pin, const int mode) {
//////////////////////////////////////////////////////////////
  int type = (int)'D'; 
  Serial.print(pin);  Serial.print(TEXT_SEPARATOR); 
  Serial.print(type); Serial.print(TEXT_SEPARATOR);
  Serial.print(mode); Serial.print(TEXT_SEPARATOR);
  Serial.print(AE::digitalRead(pin));
  Serial.print(TEXT_CLOSE); 
}
//////////////////////////////////////////////////////////////    
void ArduinoMainLoop::printAnalogPin(const unsigned char pin, const int mode) {
//////////////////////////////////////////////////////////////
  int type = (int)'A'; 
  Serial.print(pin);  Serial.print(TEXT_SEPARATOR); 
  Serial.print(type); Serial.print(TEXT_SEPARATOR); 
  Serial.print(mode); Serial.print(TEXT_SEPARATOR); 
  Serial.print(AE::analogRead(pin)); 
  Serial.print(TEXT_CLOSE); 
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoMainLoop::printPinReport() {
/////////////////////////////////////////////////////////////////////////////////////
  const int I = 'I';
  const int O = 'O';

  Serial.write(RET_SOH);
    Serial.write(PID_TEXT);
    printDigitalPin(PIN_X_STP,                O);
    printDigitalPin(PIN_Y_STP,                O);
    printDigitalPin(PIN_Z_STP,                O);
  
    printDigitalPin(PIN_X_DIR,                O);
    printDigitalPin(PIN_Y_DIR,                O);
    printDigitalPin(PIN_Z_DIR,                O);
  
    printDigitalPin(PIN_STEPPER_ENABLE ,      O);
    
    printDigitalPin(PIN_X_LIMIT,              I);
    printDigitalPin(PIN_Y_LIMIT,              I);
    printDigitalPin(PIN_Z_LIMIT,              I);

    printDigitalPin(PIN_TOOL_ENABLE,          I);
    printDigitalPin(PIN_EXTERNAL_INTERRUPT,   I);
                   
    printAnalogPin(PIN_INTERRUPT_LED_ID,      I);

  Serial.write(MBYTE_CLOSE);  
}
//////////////////////////////////////////////////////////////
AE::PinType ArduinoMainLoop::getPinType(uint8_t pin) {
//////////////////////////////////////////////////////////////
  uint8_t port = digitalPinToPort(pin);

  if ( port == NOT_A_PIN ) 
    return PT_UNKNOWN;

  // this is uno specific!
  switch ( pin ) {
      case AE::PN_A0: 
      case AE::PN_A1: 
      case AE::PN_A2:
      case AE::PN_A3:
      case AE::PN_A4: 
      case AE::PN_A5: return PT_ANALOG;
  }

  return PT_DIGITAL;
}
//////////////////////////////////////////////////////////////
uint8_t ArduinoMainLoop::getPinMode(uint8_t pin) {
//////////////////////////////////////////////////////////////
#ifdef SKETCH_COMPILE

  uint8_t bit  = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);

  if ( port == NOT_A_PIN ) 
    return NOT_A_PIN;

  // Is there a bit we can check?
  if ( bit == 0 ) 
    return NOT_A_PIN;

  // Is there only a single bit set?
  if ( bit & (bit - 1) ) 
    return NOT_A_PIN;

  volatile uint8_t *reg, *out;
  reg = portModeRegister(port);
  out = portOutputRegister(port);

  if      (*reg & bit)  return OUTPUT;
  else if (*out & bit)  return INPUT_PULLUP;
  else                  return INPUT;

  return NOT_A_PIN;
  
#else

  #warning impl. missing getPinMode()
  return NOT_A_PIN;
  
#endif
}
//////////////////////////////////////////////////////////////
void ArduinoMainLoop::sleepMicroseconds(unsigned long usec) {
//////////////////////////////////////////////////////////////
  long milli = usec / 1000;
  long micro = usec % 1000;

  if ( milli )
    AE::delay(milli);
    
  AE::delayMicroseconds(micro);
}
//////////////////////////////////////////////////////////////
bool ArduinoMainLoop::fltCompare(float a, float b) {
//////////////////////////////////////////////////////////////
  static const float epsilon = ((float)0.000999);
  return (abs(a - b) < epsilon);
}
/////////////////////////////////////////////////////////////////////////////////////
bool ArduinoMainLoop::fltCompareNull(float a) {
/////////////////////////////////////////////////////////////////////////////////////
   return fltCompare(a, 0.0);
}
/////////////////////////////////////////////////////////////////////////////////////
bool ArduinoMainLoop::isFloatValue(const unsigned char pid) {
/////////////////////////////////////////////////////////////////////////////////////
  return  pid >= PID_FLOAT_RANG_START; 
}
/////////////////////////////////////////////////////////////////////////////////////
bool ArduinoMainLoop::convertLongToFloat(const int32_t val, float& ret) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( val == MIN_LONG || val == MAX_LONG ) {
    ret = 0.0;
    return false;
  }
  
  ret = ((float)val)/FLT_FACT; 
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoMainLoop::sendHeartbeat(unsigned char limitState, unsigned char supportState) {
/////////////////////////////////////////////////////////////////////////////////////
  const uint8_t bCnt = limitState != '\0' && supportState != '\0' ? 2 : 1;
  const unsigned char byteCount = bCnt * sizeof(int32_t);
  SWB.init();
  SWB.put(RET_SOH);
  SWB.put(PID_HEARTBEAT);
  SWB.put(byteCount);
  SWB.put((int32_t)(AE::millis() % MAX_LONG));

  if ( bCnt > 1 ) {
    for (unsigned int i=0; i<I2C_BYTE_COUNT; i++) {
      switch ( i ) {
        case I2C_BYTE_LIMIT_STATE:    SWB.put(limitState);   break;
        case I2C_BYTE_SUPPORT_STATE:  SWB.put(supportState); break;
      }
    }
  
    SWB.put((unsigned char)255); // reserved
    SWB.put((unsigned char)255); // reserved
  }
  
  SWB.write();
}
/////////////////////////////////////////////////////////////////////////////////////
bool ArduinoMainLoop::peakSerial(unsigned char& c) {
/////////////////////////////////////////////////////////////////////////////////////
  c = CMD_INVALID;
  
  if ( Serial.available() == 0 )
    return false;
  
  c = Serial.peek();
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool ArduinoMainLoop::checkSerialForPauseCommands(bool currentPauseState) {
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
byte ArduinoMainLoop::readSerialByteWithTimeout(uint32_t timeoutMicros) {
////////////////////////////////////////////////////////////////
  byte b[1];
  return readSerialBytesWithTimeout(b, 1, timeoutMicros) == 1 ? b[0] : 0;
}
///////////////////////////////////////////////////////////////
int8_t ArduinoMainLoop::readSerialBytesWithTimeout(byte* buffer, int8_t length, uint32_t timeoutMicros) {
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
      const int toReadNow       = ArdoObj::minimum(stillToRead, bytesAvailable);

      // read operation
      const int currentlyPicked = Serial.readBytes(internalPointer, toReadNow);
      
      // position / counter management
      internalPointer += currentlyPicked;
      totallyPicked   += currentlyPicked;
    }
  }
    
  return totallyPicked;
}
////////////////////////////////////////////////////////////////
bool ArduinoMainLoop::readInt8(int32_t& ret) {
////////////////////////////////////////////////////////////////  
  static byte b[sizeof(int8_t)];
  const unsigned int size      = sizeof(int8_t);
  const unsigned int byteCount = readSerialBytesWithTimeout(b, size);
  
  if ( size != byteCount )
    return false;

  int8_t value = 0;
  ArduinoCmdDecoderBase::convertBytesToInt8(b, value);
  ret = value;

  return true;  
}
////////////////////////////////////////////////////////////////
bool ArduinoMainLoop::readInt16(int32_t& ret) {
////////////////////////////////////////////////////////////////  
  static byte b[sizeof(int16_t)];
  const unsigned int size      = sizeof(int16_t);
  const unsigned int byteCount = readSerialBytesWithTimeout(b, size);
  
  if ( size != byteCount )
    return false;

  int16_t value = 0;
  ArduinoCmdDecoderBase::convertBytesToInt16(b, value);
  ret = value;

  return true;  
}
////////////////////////////////////////////////////////////////
bool ArduinoMainLoop::readInt32(int32_t& ret) {
////////////////////////////////////////////////////////////////  
  static byte b[sizeof(int32_t)];
  const unsigned int size      = sizeof(int32_t);
  const unsigned int byteCount = readSerialBytesWithTimeout(b, size);
  
  if ( size != byteCount )
    return false;

  ArduinoCmdDecoderBase::convertBytesToInt32(b, ret);
  return true;  
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoMainLoop::writeByteValue(unsigned char b) {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(b);
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoMainLoop::writeByteValues(unsigned char pid, unsigned char b[], unsigned short size) {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOH);
  Serial.write(pid);
  
  for ( unsigned short i=0; i<size; i++ ) {
    writeByteValue(b[i]);
  }
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoMainLoop::writeByteValue(unsigned char pid, unsigned char b) {
/////////////////////////////////////////////////////////////////////////////////////  
  SWB.init();
  SWB.put(RET_SOH);
  SWB.put(pid);
  SWB.put(b);
  SWB.write();
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoMainLoop::writeLongValue(int32_t val) {
/////////////////////////////////////////////////////////////////////////////////////
  SWB.init();
  SWB.put(val);
  SWB.write();
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoMainLoop::writeLongValue(unsigned char pid, int32_t val) {
/////////////////////////////////////////////////////////////////////////////////////
  SWB.init();
  SWB.put(RET_SOH);
  SWB.put(pid);
  SWB.put(val);
  SWB.write();
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoMainLoop::writeLongValues(unsigned char pid, int32_t val1, int32_t val2) {
/////////////////////////////////////////////////////////////////////////////////////
  SWB.init();
  SWB.put(RET_SOH);
  SWB.put(pid);
  SWB.put(val1);
  SWB.put(val2);
  SWB.write();
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoMainLoop::writeLongValues(unsigned char pid, int32_t val1, int32_t val2, int32_t val3) {
/////////////////////////////////////////////////////////////////////////////////////
  SWB.init();
  SWB.put(RET_SOH);
  SWB.put(pid);
  SWB.put(val1);
  SWB.put(val2);
  SWB.put(val3);
  SWB.write();
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoMainLoop::writeLongValues(unsigned char pid, int32_t val1, int32_t val2, int32_t val3, int32_t val4) {
/////////////////////////////////////////////////////////////////////////////////////
  SWB.init();
  SWB.put(RET_SOH);
  SWB.put(pid);
  SWB.put(val1);
  SWB.put(val2);
  SWB.put(val3);
  SWB.put(val4);
  SWB.write();
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoMainLoop::writeGetterValue(unsigned char pid, int32_t val1) {
/////////////////////////////////////////////////////////////////////////////////////
  SWB.init();
  SWB.put(RET_SOH);
  SWB.put(PID_GETTER);
  SWB.put(pid);
  SWB.put((unsigned char)1);
  SWB.put(val1);
  SWB.write();
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoMainLoop::writeGetterValues(unsigned char pid, int32_t val1, int32_t val2) {
/////////////////////////////////////////////////////////////////////////////////////
  SWB.init();
  SWB.put(RET_SOH);
  SWB.put(PID_GETTER);
  SWB.put(pid);
  SWB.put((unsigned char)2);
  SWB.put(val1);
  SWB.put(val2);
  SWB.write();
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoMainLoop::writeGetterValues(unsigned char pid, int32_t val1, int32_t val2, int32_t val3) {
/////////////////////////////////////////////////////////////////////////////////////
  SWB.init();
  SWB.put(RET_SOH);
  SWB.put(PID_GETTER);
  SWB.put(pid);
  SWB.put((unsigned char)3);
  SWB.put(val1);
  SWB.put(val2);
  SWB.put(val3);
  SWB.write();
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoMainLoop::pushMessage(const char type, const unsigned char eid, WriteFunctionType function) {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOH);
  
    Serial.write(PID_MSG);
    switch ( type ) {
      case MT_WARNING:  Serial.write(MT_WARNING); break;
      case MT_ERROR:    Serial.write(MT_ERROR);   break;
      case MT_DEBUG:    Serial.write(MT_DEBUG);   break;
      default:          Serial.write(MT_INFO);    break; 
    }

    if ( eid != E_NO_ERROR ) {
      Serial.write(MT_MID_FLAG);
      Serial.write(eid);
    }

    if ( function ) 
      function();

  Serial.write(MBYTE_CLOSE);
  Serial.flush();

#ifndef SKETCH_COMPILE 

  const wxString errText(wxString::Format("%s %s", AE::ardoGetErrLabel(eid), LastErrorCodes::messageText));
  ARDO_DEBUG_MESSAGE(type, errText);    

#endif
}
/////////////////////////////////////////////////////////////////////////////////////
byte ArduinoMainLoop::reset() {
/////////////////////////////////////////////////////////////////////////////////////
  CNC_MAIN_LOOP_LOG_FUNCTION();
  
  // Turn off ...
  controller->turnOff();

  // Hide the Interrupt LED
  switchOutputPinState(PIN_INTERRUPT_LED, OFF);

  // broadcast reset
  controller->reset();
  clearSerial();

  Wire.end();
  Wire.begin(); 
 
/*
  // only a try . . .
  Serial.end();
  Serial.begin(BAUD_RATE);
  Serial.setTimeout(1000);
  Serial.flush();
*/

  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoMainLoop::switchOutputPinState(int pin, bool state) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( getPinMode(pin) != PM_OUTPUT )
    return;

  AE::PinType pt = getPinType(pin);
  if ( pt == PT_UNKNOWN )
    return;

  if ( pt == PT_ANALOG )   AE::analogWrite(pin,  state == true ? ANALOG_HIGH : ANALOG_LOW);
  else                     AE::digitalWrite(pin, state == true ? PL_HIGH     : PL_LOW);
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoMainLoop::clearSerial() {
/////////////////////////////////////////////////////////////////////////////////////
  //delay(1);

  while ( waitForSerialData() > 0 )
    Serial.read();
}
///////////////////////////////////////////////////////////////
void ArduinoMainLoop::waitActiveMilliseconds(uint32_t milliseconds) {
///////////////////////////////////////////////////////////////
  if ( milliseconds == 0 )
    return;
    
  uint32_t ts = AE::millis();  
  while ( true ) {

    // The return value of millis() will overflow (go back to zero), 
    // after approximately 50 days
    // In this case restart 
    if ( AE::millis() < ts ) {
      waitActiveMilliseconds(milliseconds);
      break;
    }

    if ( AE::millis() - ts > milliseconds )
      break;
  }
}
///////////////////////////////////////////////////////////////
void ArduinoMainLoop::waitActiveMicroseconds(uint32_t microseconds) {
///////////////////////////////////////////////////////////////  
  if ( microseconds == 0 )
    return;

  const uint32_t threshold  = 1000;
  uint32_t milliseconds     = microseconds / threshold;
 
  if ( milliseconds > 0 )
    waitActiveMilliseconds(milliseconds);

  uint32_t us = microseconds % threshold;
  uint32_t ts = AE::micros();  

  if ( us <= 10 )
    return;
  
  while ( true ) {

    // The return value of millis() will overflow (go back to zero), 
    // after approximately 50 days
    // In this case restart 
    if ( AE::micros() < ts ) {
      waitActiveMicroseconds(us);
      break;
    }

    if ( AE::micros() - ts > us )
      break;
  }
}
///////////////////////////////////////////////////////
int ArduinoMainLoop::waitForSerialData(uint32_t timeoutMicros) {
///////////////////////////////////////////////////////
  if ( Serial.available() > 0 )
    return Serial.available();

  const uint32_t timeout = ArdoObj::maximum(timeoutMicros, minSerialReadTimeoutMicros);
  const uint32_t ts      = AE::micros();  
  
  while ( Serial.available() == 0 ) {

    // The return value of micros() will overflow (go back to zero), 
    // after approximately 70 minutes.
    // In this case restart 
    if ( AE::micros() < ts )
      return waitForSerialData(timeoutMicros);

    if ( AE::micros() - ts > timeout )
      return 0;
  }

  return Serial.available();
 
}
//////////////////////////////////////////////////////////////
bool ArduinoMainLoop::readI2CSlave(ArdoObj::I2CData& data) {
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
  byte ret  = Wire.endTransmission();  

  /* ret = 
    0: success
    1: data too long to fit in transmit buffer
    2: received NACK on transmit of address
    3: received NACK on transmit of data
    4: other error 
  */
  
  return ( ret == 0 && bytes == (int)len );
}
///////////////////////////////////////////////////////
void ArduinoMainLoop::setup() {
///////////////////////////////////////////////////////
  CNC_MAIN_LOOP_LOG_FUNCTION();
  Wire.begin(); 
  
  Serial.begin(BAUD_RATE);
  Serial.setTimeout(1000);
  Serial.flush();

  // digital pins
  AE::pinMode(PIN_X_STP,            PM_OUTPUT);  AE::digitalWrite(PIN_X_STP,       PL_LOW);
  AE::pinMode(PIN_Y_STP,            PM_OUTPUT);  AE::digitalWrite(PIN_Y_STP,       PL_LOW);
  AE::pinMode(PIN_Z_STP,            PM_OUTPUT);  AE::digitalWrite(PIN_Z_STP,       PL_LOW);
  AE::pinMode(PIN_X_DIR,            PM_OUTPUT);  AE::digitalWrite(PIN_X_DIR,       PL_LOW);
  AE::pinMode(PIN_Y_DIR,            PM_OUTPUT);  AE::digitalWrite(PIN_Y_DIR,       PL_LOW);
  AE::pinMode(PIN_Z_DIR,            PM_OUTPUT);  AE::digitalWrite(PIN_Z_DIR,       PL_LOW);

  AE::pinMode(PIN_X_LIMIT,          PM_INPUT);   AE::digitalWrite(PIN_X_LIMIT,     LimitSwitch::LIMIT_SWITCH_OFF);
  AE::pinMode(PIN_Y_LIMIT,          PM_INPUT);   AE::digitalWrite(PIN_Y_LIMIT,     LimitSwitch::LIMIT_SWITCH_OFF);
  AE::pinMode(PIN_Z_LIMIT,          PM_INPUT);   AE::digitalWrite(PIN_Z_LIMIT,     LimitSwitch::LIMIT_SWITCH_OFF);
  AE::pinMode(PIN_STEPPER_ENABLE,   PM_OUTPUT);  // state will be managed the reset below 
  AE::pinMode(PIN_TOOL_ENABLE,      PM_OUTPUT);  // state will be managed the reset below 

  // analog pins
  AE::pinMode(PIN_INTERRUPT_LED,    PM_OUTPUT);  AE::analogWrite(PIN_INTERRUPT_LED,      ANALOG_LOW);

  reset();
  controller->evaluateI2CAvailable();

 #if (0)
  #ifdef SKETCH_COMPILE 
    #warning  ilegal use of pin 2  - temp for testing only!
    pinMode(PIN_X_STP, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_X_STP), externalInterrupt, CHANGE);
  #endif
 #endif
}

void externalInterrupt() {
  PRINT_DEBUG_VALUE("Interrupt", "hallo")
}

///////////////////////////////////////////////////////
void ArduinoMainLoop::loop() {
///////////////////////////////////////////////////////
  if ( Serial.available() <= 0 ) {
    controller->evaluateI2CData();
    return;
  }    
  
  byte       r  = RET_NULL;
  const byte c  = Serial.read();

  // --------------------------------------------------------------------------
  // Signals
  // --------------------------------------------------------------------------
  if ( ArdoObj::isSignal(c) == true ) {
    controller->processSignal(c);
    return;
  }

  // --------------------------------------------------------------------------
  // Commands
  // --------------------------------------------------------------------------
  switch ( c ) {

    // SB command -  Reset
    case CMD_RESET_CONTROLLER:
          r = reset();
          break;

    // SB command - Idle handling
    // SB command - Heartbeat handling
    case CMD_IDLE:
    case CMD_HEARTBEAT:
          controller->processHeartbeat(c);
          r = RET_OK;
          break;
          
    // MB command - Parameter getter
    case CMD_GETTER:
          r = controller->acceptGetter();
          break;
          
    // SB command - Parameter setter
    case CMD_SETTER:
          r = controller->acceptSetter();
          break;
          
    // MB command - Movement
    case CMD_MOVE: // obsolete command
    case CMD_RENDER_AND_MOVE:
    case CMD_MOVE_UNIT_SIGNAL:
          r = controller->acceptMove(c);
          break;
          
    case CMD_MOVE_SEQUENCE:
    case CMD_RENDER_AND_MOVE_SEQUENCE:
          r = controller->acceptMoveSequence(c);
          break;

    // --------------------------------------------------------------------------
    // Commands - multi byte return
    // --------------------------------------------------------------------------

     // MB command - Print configuration
    case CMD_PRINT_CONFIG:
          printConfig();
          r = RET_OK;
          break;

    // MB command - Print version
    case CMD_PRINT_VERSION:
          printSketchVersion();
          r = RET_OK;
          break;

    case CMD_PRINT_TIMESTAMP:      
          printSketchTimestamp();
          r = RET_OK;
          break;

    // MB command - Pin report
    case CMD_PRINT_PIN_REPORT:
          printPinReport();
          r = RET_OK;
          break;
   
    // --------------------------------------------------------------------------
    // Commands - for tests only
    // MB command - Pin report
    case CMD_PERFORM_TEST:
          r =  controller->performTest();
          break;
    
    // --------------------------------------------------------------------------
    default: 
          LastErrorCodes::clear();
          LastErrorCodes::register1Byte_A = c;
          ArduinoMainLoop::pushMessage(MT_ERROR, E_UNKNOW_COMMAND);

  } // switch ( c )

  // hand shake
  if ( r != RET_NULL ) {
    Serial.write(r);
    Serial.flush();
  }
}
