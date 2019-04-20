#define INLINE inline
#define SKETCH_COMPILE = TRUE

#include <SoftwareSerial.h>
#include <Wire.h>
#include "CommonValues.h"
#include "CncStepper.h"
#include "CncController.h"
#include "CommonFunctions.h"

// Global Parameters
const char* testMessageText = "This is a test message.";
CncController controller;

struct MoveSequenceData {
  
  ValueInfo vi                 = ValueInfo(0);
  unsigned char pid            = 0;
  unsigned char portionSize    = 0;
  unsigned char portionCounter = 0;
  
  int32_t totalLength          = 0; 
  int32_t totalIndex           = 0;
  int32_t totalRemaining       = 0;

  int32_t lengthX              = 0; 
  int32_t lengthY              = 0; 
  int32_t lengthZ              = 0; 
  
  int32_t dx                   = 0;
  int32_t dy                   = 0;
  int32_t dz                   = 0;
  int32_t f                    = 0;

  void reset() {
    *this = MoveSequenceData();
  }
  
} msd;

/////////////////////////////////////////////////////////////////////////////////////
INLINE void printSketchVersion() {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOH);
    Serial.write(PID_TEXT);
    Serial.write(FIRMWARE_VERSION);
  Serial.write(MBYTE_CLOSE);
}
/////////////////////////////////////////////////////////////////////////////////////
INLINE void printConfig() {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOH);
    Serial.write(PID_TEXT);
    Serial.print(PID_COMMON);        Serial.print(TEXT_SEPARATOR);     Serial.print(BLANK);                                             Serial.print(TEXT_CLOSE);
    Serial.print(BLANK);             Serial.print(PID_VERSION);        Serial.print(TEXT_SEPARATOR); Serial.print(FIRMWARE_VERSION);    Serial.print(TEXT_CLOSE);
  
    controller.printConfig();

  Serial.write(MBYTE_CLOSE);
}
/////////////////////////////////////////////////////////////////////////////////////
INLINE void printPinReport() {
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
    printDigitalPin(PIN_TOOL_FEEDBACK,        I);

    printAnalogPin(PIN_INTERRUPT_LED_ID,      I);

  Serial.write(MBYTE_CLOSE);  
}
/////////////////////////////////////////////////////////////////////////////////////
INLINE long isReadyToRun(){
/////////////////////////////////////////////////////////////////////////////////////
  long ret = controller.isReadyToRun();
  return ret;
}
/////////////////////////////////////////////////////////////////////////////////////
INLINE void switchOutputPinState(int pin, bool state) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( getPinMode(pin) != OUTPUT )
    return;

  PinType pt = getPinType(pin);
  if ( pt == PT_UNKNOWN )
    return;

  if ( pt == PT_ANALOG )   analogWrite(pin,  state == true ? ANALOG_HIGH : ANALOG_LOW);
  else                     digitalWrite(pin, state == true ? HIGH        : LOW);
}
/////////////////////////////////////////////////////////////////////////////////////
INLINE void writeLimitGetter() {
/////////////////////////////////////////////////////////////////////////////////////  
  long x = LimitSwitch::LIMIT_UNKNOWN;
  long y = LimitSwitch::LIMIT_UNKNOWN;
  long z = LimitSwitch::LIMIT_UNKNOWN;

  controller.evaluateLimitStates(x, y, z);
  writeGetterValues(PID_LIMIT, x, y, z);
}
/////////////////////////////////////////////////////////////////////////////////////
INLINE unsigned char evaluateGetter(unsigned char pid) {
/////////////////////////////////////////////////////////////////////////////////////
  
  switch ( pid ) {
    // evaluateGetter() ............................................
    case PID_QUERY_READY_TO_RUN:      writeGetterValue(PID_QUERY_READY_TO_RUN, isReadyToRun());
                                      break;
    // evaluateGetter() ............................................
    case PID_STEPS_X:                 writeGetterValue(PID_STEPS_X, controller.getStepperX()->getSteps());
                                      break;
    // evaluateGetter() ............................................
    case PID_STEPS_Y:                 writeGetterValue(PID_STEPS_Y, controller.getStepperY()->getSteps());
                                      break;
    // evaluateGetter() ............................................
    case PID_STEPS_Z:                 writeGetterValue(PID_STEPS_Z, controller.getStepperZ()->getSteps());
                                      break;
    // evaluateGetter() ............................................
    case PID_X_POS:                   writeGetterValue(PID_X_POS, controller.getStepperX()->getPosition());
                                      break;
    // evaluateGetter() ............................................
    case PID_Y_POS:                   writeGetterValue(PID_Y_POS, controller.getStepperY()->getPosition());
                                      break;
    // evaluateGetter() ............................................
    case PID_Z_POS:                   writeGetterValue(PID_Z_POS, controller.getStepperZ()->getPosition());
                                      break;
    // evaluateGetter() ............................................
    case PID_XY_POS:                  writeGetterValues(PID_XY_POS, controller.getStepperX()->getPosition(), controller.getStepperY()->getPosition());
                                      break;
    // evaluateGetter() ............................................
    case PID_XYZ_POS:                 writeGetterValues(PID_XYZ_POS, controller.getStepperX()->getPosition(), controller.getStepperY()->getPosition(), controller.getStepperZ()->getPosition());
                                      break;
    // evaluateGetter() ............................................
    case PID_GET_POS_COUNTER:         writeGetterValues(PID_GET_POS_COUNTER, controller.getPositionCounter(), controller.getPositionCounterOverflow());
                                      break;
    // evaluateGetter() ............................................
    case PID_GET_STEP_COUNTER_X:      writeGetterValues(PID_GET_STEP_COUNTER_X, controller.getStepperX()->getStepCounter(), controller.getStepperX()->getStepCounterOverflow());
                                      break;
    // evaluateGetter() ............................................
    case PID_GET_STEP_COUNTER_Y:      writeGetterValues(PID_GET_STEP_COUNTER_Y, controller.getStepperY()->getStepCounter(), controller.getStepperY()->getStepCounterOverflow());
                                      break;
    // evaluateGetter() ............................................
    case PID_GET_STEP_COUNTER_Z:      writeGetterValues(PID_GET_STEP_COUNTER_Z, controller.getStepperZ()->getStepCounter(), controller.getStepperZ()->getStepCounterOverflow());
                                      break;
    // evaluateGetter() ............................................
    case PID_LIMIT:                   writeLimitGetter();
                                      break;
    // evaluateGetter() ............................................
    case PID_AVG_STEP_DURATION:       writeGetterValues(PID_AVG_STEP_DURATION, controller.getStepperX()->getAvgStepDuration(), 
                                                                               controller.getStepperY()->getAvgStepDuration(), 
                                                                               controller.getStepperZ()->getAvgStepDuration());
                                      break;
    // evaluateGetter() ............................................
    case PID_POS_REPLY_THRESHOLD_X:   writeGetterValue(PID_POS_REPLY_THRESHOLD_X, controller.getPosReplyThresholdX());
                                      break;
    // evaluateGetter() ............................................
    case PID_POS_REPLY_THRESHOLD_Y:   writeGetterValue(PID_POS_REPLY_THRESHOLD_Y, controller.getPosReplyThresholdY());
                                      break;
    // evaluateGetter() ............................................
    case PID_POS_REPLY_THRESHOLD_Z:   writeGetterValue(PID_POS_REPLY_THRESHOLD_Z, controller.getPosReplyThresholdZ());
                                      break;
    // evaluateGetter() ............................................
    default:                          writeGetterValue(PID_UNKNOWN, 0);

                                      LastErrorCodes::clear();
                                      LastErrorCodes::register1Byte_A = pid;
                                      pushErrorMessage(E_INVALID_GETTER_ID);
                                      return RET_ERROR;
  }

  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
INLINE unsigned char processGetter() {
/////////////////////////////////////////////////////////////////////////////////////
  // Wait a protion of time.
  // This is very importent for the next multibyte read
  delay(1);

  // error handling
  if ( Serial.available() <= 0 ) {
    writeGetterValue(PID_UNKNOWN, 0);
    pushErrorMessage(E_INVALID_GETTER_LIST_COUNT);
    return RET_ERROR;
  }
  
  // append the getter
  unsigned char pid = Serial.read();
  return evaluateGetter(pid);
}
/////////////////////////////////////////////////////////////////////////////////////
INLINE unsigned char processSetter() {
/////////////////////////////////////////////////////////////////////////////////////
  // Wait a protion of time.
  // This is very importent for the next multibyte read
  delay(1);

  struct Values {
    bool isDouble = false;
    long   l      = 0;
    double d      = 0.0;   
  };

  Values values[MAX_SETTER_VALUES];
  byte b[4];
  
  // error handling: pid + count
  if ( Serial.available() < 2) {
    pushErrorMessage(E_INVALID_PARAM_ID);
    return RET_ERROR;
  }
  
  unsigned char pid        = Serial.read();
  unsigned char valueCount = Serial.read();
  
  unsigned short byteCount = Serial.available();
  unsigned short tryCount  = 0;
  while ( byteCount < valueCount * sizeof(int32_t) ) {
    delay(1);
    byteCount = Serial.available();
    
    // error handling
    if ( ++tryCount >= 5 ) {
      pushErrorMessage(E_INVALID_PARAM_STREAM);
      return RET_ERROR;      
    }
  }

  // error handling
  if ( byteCount%sizeof(int32_t) != 0 || byteCount/sizeof(int32_t) > MAX_SETTER_VALUES ) {
    LastErrorCodes::clear();
    LastErrorCodes::register4Byte_A = byteCount;
    pushErrorMessage(E_INVALID_PARAM_STREAM_LEN);
    return RET_ERROR;
  }

  // over all given values
  for (unsigned short i=0; i<byteCount/sizeof(int32_t); i++) {
    
    // read a 4 byte
    unsigned int size = Serial.readBytes(b, sizeof(int32_t));
    if ( size != sizeof(int32_t) ) {
      pushErrorMessage(E_INVALID_PARAM_STREAM);
      return RET_ERROR;
    }
    
    // order the bytes
    values[i].l  = (long)b[0] << 24;
    values[i].l += (long)b[1] << 16;
    values[i].l += (long)b[2] << 8;
    values[i].l += (long)b[3];

    if ( isDoubleValue(pid) )  convertLongToDouble(values[i].l, values[i].d);
    else                       values[i].d = 0.0;      
  }

  // process pid
  switch ( pid ) {
    // processSetter() ............................................
    case PID_X_POS:               controller.getStepperX()->setPosition(values[0].l); 
                                  break;
    case PID_Y_POS:               controller.getStepperY()->setPosition(values[0].l); 
                                  break;
    case PID_Z_POS:               controller.getStepperZ()->setPosition(values[0].l); 
                                  break;
    // processSetter() ............................................
    case PID_RESERT_POS_COUNTER:  controller.resetPositionCounter(); 
                                  break;
    case PID_RESERT_STEP_COUNTER: controller.getStepperX()->resetStepCounter(); 
                                  controller.getStepperY()->resetStepCounter();
                                  controller.getStepperZ()->resetStepCounter();   
                                  break;
    // processSetter() ............................................
    case PID_TOOL_SWITCH:         controller.switchToolState(values[0].l == 0 ? TOOL_STATE_OFF: TOOL_STATE_ON); 
                                  break;
    // processSetter() ............................................
    case PID_POS_REPLY_THRESHOLD_X: 
                                  controller.setPosReplyThresholdX(values[0].l); 
                                  break;
    case PID_POS_REPLY_THRESHOLD_Y: 
                                  controller.setPosReplyThresholdY(values[0].l); 
                                  break;
    case PID_POS_REPLY_THRESHOLD_Z: 
                                  controller.setPosReplyThresholdZ(values[0].l); 
                                  break;
    // processSetter() ............................................
    case PID_X_LIMIT:             controller.getStepperX()->setLimitStateManually(values[0].l); 
                                  break;
    case PID_Y_LIMIT:             controller.getStepperY()->setLimitStateManually(values[0].l); 
                                  break;
    case PID_Z_LIMIT:             controller.getStepperZ()->setLimitStateManually(values[0].l); 
                                  break;
    // processSetter() ............................................
    case PID_STEPS_X:             controller.getStepperX()->setSteps(values[0].l); 
                                  controller.setupSpeedController();
                                  break;
    case PID_STEPS_Y:             controller.getStepperY()->setSteps(values[0].l); 
                                  controller.setupSpeedController();
                                  break;
    case PID_STEPS_Z:             controller.getStepperZ()->setSteps(values[0].l); 
                                  controller.setupSpeedController();
                                  break;
    // processSetter() ............................................
    case PID_PITCH_X:             controller.getStepperX()->setPitch(values[0].d); 
                                  controller.setupSpeedController();
                                  break;
    case PID_PITCH_Y:             controller.getStepperY()->setPitch(values[0].d); 
                                  controller.setupSpeedController();
                                  break;
    case PID_PITCH_Z:             controller.getStepperZ()->setPitch(values[0].d); 
                                  controller.setupSpeedController();
                                  break;
    // processSetter() ............................................
    case PID_PULSE_WIDTH_LOW_X:   controller.getStepperX()->setLowPulseWidth(values[0].l); 
                                  controller.setupSpeedController();
                                  break;
    case PID_PULSE_WIDTH_LOW_Y:   controller.getStepperY()->setLowPulseWidth(values[0].l); 
                                  controller.setupSpeedController();
                                  break;
    case PID_PULSE_WIDTH_LOW_Z:   controller.getStepperZ()->setLowPulseWidth(values[0].l); 
                                  controller.setupSpeedController();
                                  break;
    // processSetter() ............................................
    case PID_PULSE_WIDTH_HIGH_X:  controller.getStepperX()->setHighPulseWidth(values[0].l); 
                                  controller.setupSpeedController();
                                  break;
    case PID_PULSE_WIDTH_HIGH_Y:  controller.getStepperY()->setHighPulseWidth(values[0].l); 
                                  controller.setupSpeedController();
                                  break;
    case PID_PULSE_WIDTH_HIGH_Z:  controller.getStepperZ()->setHighPulseWidth(values[0].l); 
                                  controller.setupSpeedController();
                                  break;

    // processSetter() ............................................
    case PID_SPEED_MM_MIN:        controller.setSpeedValue(values[0].d); 
                                  break;

    // processSetter() ............................................
    case PID_SPEED_FEED_MODE:     // currently nothing to do
                                  break;

    // processSetter() ............................................
    case PID_ACCEL_PROFILE:       // todo
                                  controller.setupAccelProfile(values[0].l,
                                                               values[1].l,
                                                               values[2].l,
                                                               values[3].l,
                                                               values[4].l,
                                                               values[5].l                                                               
                                                               );
                                  controller.setupSpeedController();
                                  break;
    // processSetter() ............................................
    // call it with lValue = NORMALIZED_INCREMENT_DIRECTION || INVERSED_INCREMENT_DIRECTION
    case PID_INCREMENT_DIRECTION_VALUE_X:   
                                  controller.getStepperX()->setIncrementDirectionValue(values[0].l); 
                                  break;
    case PID_INCREMENT_DIRECTION_VALUE_Y:   
                                  controller.getStepperY()->setIncrementDirectionValue(values[0].l); 
                                  break;
    case PID_INCREMENT_DIRECTION_VALUE_Z:   
                                  controller.getStepperZ()->setIncrementDirectionValue(values[0].l); 
                                  break;

    // processSetter() ............................................
    case PID_PROBE_MODE:          controller.setProbeMode(values[0].l != 0);
                                  break;
    // processSetter() ............................................
    case PID_ENABLE_STEPPERS:     controller.enableStepperPin(values[0].l != 0);
                                  break;
    // processSetter() ............................................
    default:                      LastErrorCodes::clear();
                                  LastErrorCodes::register1Byte_A = pid;
                                  pushErrorMessage(E_INVALID_PARAM_ID);
                                  return RET_ERROR;
  }

  // Wait a protion of time.
  delay(1);

  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
INLINE unsigned char decodeMoveSequence() {
/////////////////////////////////////////////////////////////////////////////////////

  // ------------------------------------------------------------------------------
  auto debugValues = [&](unsigned short idx) {

      LastErrorCodes::clear();
      LastErrorCodes::register1Byte_A = idx;
      LastErrorCodes::register1Byte_B = msd.pid;
      LastErrorCodes::register1Byte_C = msd.portionSize;
      LastErrorCodes::register1Byte_D = msd.portionCounter;

      LastErrorCodes::register1Byte_H = (unsigned char)Serial.available();

      LastErrorCodes::register4Byte_A = msd.totalIndex;
      LastErrorCodes::register4Byte_B = msd.totalLength;
      LastErrorCodes::register4Byte_C = msd.totalRemaining;
      
      LastErrorCodes::register4Byte_E = msd.dx;
      LastErrorCodes::register4Byte_F = msd.dy;
      LastErrorCodes::register4Byte_G = msd.dz;
      LastErrorCodes::register4Byte_H = msd.f;
  };

  // ------------------------------------------------------------------------------
  auto logInfo = [&](unsigned char idx, unsigned char eid) {
      debugValues(idx);
      pushMessage(MT_DEBUG, eid, LastErrorCodes::writeToSerial);
  };

  // ------------------------------------------------------------------------------
  auto logError = [&](unsigned char idx, unsigned char eid) {
      debugValues(idx);
      pushMessage(MT_ERROR, eid, LastErrorCodes::writeToSerial);
      clearSerial();
      return RET_ERROR;
  };

  // ------------------------------------------------------------------------------
  auto doMove = [&]() {
    #warning
    logInfo(100, E_NO_ERROR);
    //controller.sendCurrentPositions(PID_XYZ_POS_MAJOR, true);
  };
  
  // ------------------------------------------------------------------------------
  auto parseValues = [&]() {
    
    // determine pid and paring rules
    msd.pid = readSerialByteWithTimeout();
    msd.vi.set(msd.pid);
    
    if ( msd.vi.isValid() == false ) {
      logError(40, E_INVALID_MOVE_SEQUENCE);
      return -1;
    }
    
    unsigned int byteCount = msd.vi.getByteCount();
    unsigned int valCount  = msd.vi.getValueCount();
    int totalSize          = 1 + ( byteCount != 0 ? valCount * byteCount : 1 );
    int readIndex          = totalSize - 1;

    int32_t v[ValueInfo::MaxValueCount]; 
    v[0] = v[1] = v[2] = 0; v[3] = 0;
    
    byte b;
    unsigned short count = 0;
    
    while ( count < valCount ) {
      
      // copy bytes
      switch ( byteCount ){
  
        // format:   bit: 78543210    
        //                  zzyyxx
        //                  -+-+-+ 
        //                          bit  +               bit  -     0
        case 0:   b = readSerialByteWithTimeout();
                  v[0] = bitRead(b, 6) ? +1 : bitRead(b, 7) ? -1 :  0;
                  v[1] = bitRead(b, 0) ? +1 : bitRead(b, 1) ? -1 :  0;
                  v[2] = bitRead(b, 2) ? +1 : bitRead(b, 3) ? -1 :  0;
                  v[3] = bitRead(b, 4) ? +1 : bitRead(b, 5) ? -1 :  0;
                  
                  readIndex -= 1;
                  count     += ValueInfo::MaxValueCount; // to break the while loop
                  break;
                  
        case 1:   if ( readInt8(v[count]) == false ) {
                    logError(42, E_INVALID_MOVE_SEQUENCE);
                    return -1;
                  }
                  readIndex -= 1;
                  break;
  
        case 2:   if ( readInt16(v[count]) == false ) {
                    logError(43, E_INVALID_MOVE_SEQUENCE);
                    return -1;
                  }
                  readIndex -= 2;
                  break;
  
        case 4:   if ( readInt32(v[count]) == false ) {
                    logError(44, E_INVALID_MOVE_SEQUENCE);
                    return -1;
                  }
                  readIndex -= 4;
                  break;
                  
        default:  logError(45, E_INVALID_MOVE_SEQUENCE);
                  return -1;
      }

      count++;
    }

    // assign x, y, z and f depending on given type
    const unsigned short p = msd.vi.hasF() ? 1 : 0;
    if ( msd.vi.hasF() )  msd.f = v[0];
    else                  msd.f = 0;
      
    if      ( msd.vi.hasXYZ() ) { msd.dx = v[p+0]; msd.dy = v[p+1]; msd.dz = v[p+2]; }
    else if ( msd.vi.hasXY()  ) { msd.dx = v[p+0]; msd.dy = v[p+1]; msd.dz = 0;      }
    else if ( msd.vi.hasX()   ) { msd.dx = v[p+0]; msd.dy = 0;      msd.dz = 0;      }
    else if ( msd.vi.hasY()   ) { msd.dx = 0;      msd.dy = v[p+0]; msd.dz = 0;      }
    else if ( msd.vi.hasZ()   ) { msd.dx = 0;      msd.dy = 0;      msd.dz = v[p+0]; }
    else                        { logError(46, E_INVALID_MOVE_SEQUENCE); return -1;  }

    if ( readIndex != 0 ) {
      logError(49, E_INVALID_MOVE_SEQUENCE);
      return -1;
    }

    doMove();
    return totalSize;
  };
  
  // ------------------------------------------------------------------------------
  auto parsePortion = [&]() {
    msd.portionCounter++;

    int portionIndex = msd.portionSize;
    while ( portionIndex > 0 ) {
      
      const int valueSize = parseValues();
      if ( valueSize <= 0 ) {
        logError(30, E_INVALID_MOVE_SEQUENCE);
        return -1;
      }
      
      portionIndex        -= valueSize;  
      msd.totalRemaining  -= valueSize;  
    }  

    return 0;
  };

  // ------------------------------------------------------------------------------
  // start of parsing  
  msd.reset();
  
  // first read header values
  if ( readInt32(msd.totalLength) == false )  return logError(10, E_INVALID_MOVE_SEQUENCE);
  if ( readInt32(msd.lengthX)     == false )  return logError(11, E_INVALID_MOVE_SEQUENCE);
  if ( readInt32(msd.lengthY)     == false )  return logError(12, E_INVALID_MOVE_SEQUENCE);
  if ( readInt32(msd.lengthZ)     == false )  return logError(13, E_INVALID_MOVE_SEQUENCE);

  msd.totalRemaining = msd.totalLength;
  msd.totalIndex     = msd.totalLength;
  
  // over all portions 
  while ( msd.totalRemaining > 0 ) {
    // wait max an extra frame of one second for the next portion
    waitForSerialData(1000L * 1000L);
    
    msd.portionSize = readSerialByteWithTimeout();
    
    if ( msd.portionSize == 0 )
      return logError(20, E_INVALID_MOVE_SEQUENCE);
    
    msd.totalRemaining--;
    
    if ( parsePortion() != 0 )
      return logError(21, E_INVALID_MOVE_SEQUENCE);

    // flush handshake
    if ( msd.totalRemaining > 0 ) {
      Serial.write(RET_MORE);
      Serial.flush();
    }
  }

  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
INLINE unsigned char decodeMove(unsigned char cmd) {
/////////////////////////////////////////////////////////////////////////////////////
  // Wait a protion of time.
  // This is very importent for the next readBytes above
  delayMicroseconds(1000);

  byte b[4];
  int32_t v[3];
  unsigned short size  = 0;
  unsigned short count = 0;

  const short MAX_VALUES = 3;

  //fetch 1 to max 3 long values
  while ( (size = Serial.available()) > 0 ) {
    size = Serial.readBytes(b, sizeof(int32_t));
        
    if ( size != sizeof(int32_t) ) {
      pushErrorMessage(E_INVALID_MOVE_CMD);
      return RET_ERROR;
    }

    // order bytes
    v[count]  = (long)b[0] << 24;
    v[count] += (long)b[1] << 16;
    v[count] += (long)b[2] << 8;
    v[count] += (long)b[3];

    if ( ++count == MAX_VALUES )
      break;
  }

  static int32_t x=0, y=0, z=0;
  switch ( count ) {
    case 1:   x = 0;    y = 0;    z = v[0]; break;
    case 2:   x = v[0]; y = v[1]; z = 0;    break;
    case 3:   x = v[0]; y = v[1]; z= v[2];  break;
    
    default:  return RET_ERROR;
  }

  // select underlying mechanism 
  switch ( cmd ) {
    case CMD_MOVE_UNIT_SIGNAL:  return controller.moveUntilSignal(x, y, z);
    default:                    return controller.renderAndStepAxisXYZ(x, y, z);
  }

  return RET_ERROR;
}
/////////////////////////////////////////////////////////////////////////////////////
INLINE void clearSerial() {
/////////////////////////////////////////////////////////////////////////////////////
  //delay(1);

  while ( waitForSerialData() > 0 )
    Serial.read();
}
/////////////////////////////////////////////////////////////////////////////////////
INLINE void printSerial() {
/////////////////////////////////////////////////////////////////////////////////////
  LastErrorCodes::clear();
  pushMessage(MT_DEBUG, E_NO_ERROR, LastErrorCodes::writeSerialToSerial);
}
/////////////////////////////////////////////////////////////////////////////////////
INLINE char reset() {
/////////////////////////////////////////////////////////////////////////////////////
  // Turn off ...
  controller.switchToolState(TOOL_STATE_OFF, FORCE);
  controller.switchStepperState(ENABLE_STATE_OFF);

  // Hide the Interrupt LED
  switchOutputPinState(PIN_INTERRUPT_LED, OFF);

  // broadcast reset
  controller.reset();
  
  clearSerial();

  Wire.end();
  Wire.begin(); 
  
/*
  Serial.end();
  Serial.begin(BAUD_RATE);
  Serial.setTimeout(1000);
  Serial.flush();
*/

  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
INLINE void processInterrupt() {
/////////////////////////////////////////////////////////////////////////////////////
  // Turn off ...
  controller.switchToolState(TOOL_STATE_OFF, FORCE);
  controller.switchStepperState(ENABLE_STATE_OFF);

  // Show Interrup LED
  switchOutputPinState(PIN_INTERRUPT_LED, ON);

  controller.broadcastInterrupt();
  pushErrorMessage(E_INTERRUPT);
}
/////////////////////////////////////////////////////////////////////////////////////
void setup() {
/////////////////////////////////////////////////////////////////////////////////////
  Wire.begin(); 
  
  Serial.begin(BAUD_RATE);
  Serial.setTimeout(1000);
  Serial.flush();

  // digital pins
    pinMode(PIN_X_DIR,              OUTPUT);  digitalWrite(PIN_X_DIR,             LOW);
    pinMode(PIN_Y_DIR,              OUTPUT);  digitalWrite(PIN_Y_DIR,             LOW);
    pinMode(PIN_Z_DIR,              OUTPUT);  digitalWrite(PIN_Z_DIR,             LOW);
    pinMode(PIN_X_STP,              OUTPUT);  digitalWrite(PIN_X_STP,             LOW);
    pinMode(PIN_Y_STP,              OUTPUT);  digitalWrite(PIN_Y_STP,             LOW);
    pinMode(PIN_Z_STP,              OUTPUT);  digitalWrite(PIN_Z_STP,             LOW);
    
    pinMode(PIN_X_LIMIT,            INPUT);   digitalWrite(PIN_X_LIMIT,           LimitSwitch::LIMIT_SWITCH_OFF);
    pinMode(PIN_Y_LIMIT,            INPUT);   digitalWrite(PIN_Y_LIMIT,           LimitSwitch::LIMIT_SWITCH_OFF);
    pinMode(PIN_Z_LIMIT,            INPUT);   digitalWrite(PIN_Z_LIMIT,           LimitSwitch::LIMIT_SWITCH_OFF);
  
    pinMode(PIN_STEPPER_ENABLE,     OUTPUT);  // state will be managed the reset below 
    pinMode(PIN_TOOL_ENABLE,        OUTPUT);  // state will be managed the reset below 

  // analog pins
    pinMode(PIN_INTERRUPT_LED,      OUTPUT);  analogWrite(PIN_INTERRUPT_LED,      ANALOG_LOW);

  reset();
  controller.evaluateI2CAvailable();
}
/////////////////////////////////////////////////////////////////////////////////////
void loop() {
/////////////////////////////////////////////////////////////////////////////////////
  
  if ( Serial.available() <= 0 ) {
    controller.evaluateI2CData();
    return;
  }

  char r = RET_OK;
  byte c = Serial.read();

  switch ( c ) {

    // --------------------------------------------------------------------------
    // Signals
    // --------------------------------------------------------------------------

        // Software reset
        case SIG_SOFTWARE_RESET:
              softwareReset();        
              r = RET_NULL;
              break;
              
        // Interrupt
        case SIG_INTERRUPPT:
              processInterrupt();
              r = RET_NULL;
              break;

        // Resume
        case SIG_RESUME:
              controller.broadcastPause(PAUSE_INACTIVE); 
              r = RET_NULL;
              break;

        // Pause
        case SIG_PAUSE:
              controller.broadcastPause(PAUSE_ACTIVE); 
              r = RET_NULL;
              break;

        // Cancel running moves
        case SIG_HALT:
              controller.broadcastHalt(); 
              r = RET_NULL;
              break;

        // quit moves until signal
       case SIG_QUIT_MOVE:
              //TODO  
              r = RET_NULL;
              break;

    // --------------------------------------------------------------------------
    // Commands
    // --------------------------------------------------------------------------

        // SB command -  Reset
        case CMD_RESET_CONTROLLER:
              r = reset();
              break;

        // SB command - Idle handling
        case CMD_IDLE:
              controller.idle();
              r = RET_OK;
              break;

        // SB command - Heartbeat handling
        case CMD_HEARTBEAT:
              controller.heartbeat();
              r = RET_OK;
              break;

        // MB command - Movement
        case CMD_MOVE: // obsolete command
        case CMD_RENDER_AND_MOVE:
        case CMD_MOVE_UNIT_SIGNAL:
              controller.setPosReplyState(true);

              r = decodeMove(c);
              
              controller.getStepperX()->resetDirectionPin();
              controller.getStepperY()->resetDirectionPin();
              controller.getStepperZ()->resetDirectionPin();
              controller.sendCurrentPositions(PID_XYZ_POS_MAJOR, true);
              controller.setPosReplyState(false);
              break;

        case CMD_MOVE_SEQUENCE:
        case CMD_RENDER_AND_MOVE_SEQUENCE:
              controller.setPosReplyState(true);
              
              r = decodeMoveSequence();
              
              controller.getStepperX()->resetDirectionPin();
              controller.getStepperY()->resetDirectionPin();
              controller.getStepperZ()->resetDirectionPin();
              controller.sendCurrentPositions(PID_XYZ_POS_MAJOR, true);
              controller.setPosReplyState(false);
              break;
              
        // MB command - Parameter getter
        case CMD_GETTER:
              r = processGetter();
              break;
          
        // SB command - Parameter setter
        case CMD_SETTER:
              r = processSetter();
              break;
              
    // --------------------------------------------------------------------------
    // Commands - multi byte return
    // --------------------------------------------------------------------------

        case CMD_TEST_INFO_MESSAGE:
              pushInfoMessage(testMessageText);
              r = RET_OK;
              break;

        case CMD_TEST_WARN_MESSAGE:
              pushWarningMessage(testMessageText);
              r = RET_OK;
              break;

        case CMD_TEST_ERROR_MESSAGE:
              pushErrorMessage(testMessageText);
              r = RET_OK;
              break;

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

        // MB command - Pin report
        case CMD_PRINT_PIN_REPORT:
              printPinReport();
              r = RET_OK;
              break;

    // --------------------------------------------------------------------------
    // Error handling
    // --------------------------------------------------------------------------

        // Unknown commands
        default:
              LastErrorCodes::clear();
              LastErrorCodes::register1Byte_A = c;
              pushErrorMessage(E_UNKNOW_COMMAND);
  }

  // hand shake
  if ( r != RET_NULL ) {
    Serial.write(r);
    Serial.flush();
  }
}
