#define SKETCH_COMPILE = TRUE

#include <SoftwareSerial.h>
#include <Wire.h>
#include "CommonValues.h"
#include "CncStepper.h"
#include "CncController.h"
#include "CncTestSuite.h"
#include "CommonFunctions.h"
#include "LastErrorCodes.h"

// Global Parameters
LastErrorCodes errorInfo;
CncController controller(errorInfo);

/////////////////////////////////////////////////////////////////////////////////////
inline void printSketchVersion() {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOT);
    Serial.write(FIRMWARE_VERSION);
  Serial.write(MBYTE_CLOSE);
}
/////////////////////////////////////////////////////////////////////////////////////
inline void printConfig() {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOT);

    Serial.print(PID_COMMON);        Serial.print(TEXT_SEPARATOR);     Serial.print(BLANK);                                             Serial.print(TEXT_CLOSE);
    Serial.print(BLANK);             Serial.print(PID_VERSION);        Serial.print(TEXT_SEPARATOR); Serial.print(FIRMWARE_VERSION);    Serial.print(TEXT_CLOSE);
  
    controller.printConfig();
    CncTestSuite::printConfig();

  Serial.write(MBYTE_CLOSE);
}
/////////////////////////////////////////////////////////////////////////////////////
inline void printPinReport() {
/////////////////////////////////////////////////////////////////////////////////////
  const int I = 'I';
  const int O = 'O';

  #define PRINT_DIGITAL_PIN( Pin, Mode ) \
    { \
      int Type = (int)'D'; \
      Serial.print(Pin);  Serial.print(TEXT_SEPARATOR); \
      Serial.print(Type); Serial.print(TEXT_SEPARATOR); \
      Serial.print(Mode); Serial.print(TEXT_SEPARATOR); \
      Serial.print(digitalRead(Pin)); \
      Serial.print(TEXT_CLOSE); \
    }
    
  #define PRINT_ANALOG_PIN( Pin, Mode ) \
    { \
      int Type = (int)'A'; \
      Serial.print(Pin);  Serial.print(TEXT_SEPARATOR); \
      Serial.print(Type); Serial.print(TEXT_SEPARATOR); \
      Serial.print(Mode); Serial.print(TEXT_SEPARATOR); \
      Serial.print(analogRead(Pin)); \
      Serial.print(TEXT_CLOSE); \
    }

  Serial.write(RET_SOT);

    PRINT_DIGITAL_PIN(PIN_X_STP,                O)
    PRINT_DIGITAL_PIN(PIN_Y_STP,                O)
    PRINT_DIGITAL_PIN(PIN_Z_STP,                O)
  
    PRINT_DIGITAL_PIN(PIN_X_DIR,                O)
    PRINT_DIGITAL_PIN(PIN_Y_DIR,                O)
    PRINT_DIGITAL_PIN(PIN_Z_DIR,                O)
  
    PRINT_DIGITAL_PIN(PIN_STEPPER_ENABLE ,      O)
    
    PRINT_DIGITAL_PIN(PIN_X_LIMIT,              I)
    PRINT_DIGITAL_PIN(PIN_Y_LIMIT,              I)
    PRINT_DIGITAL_PIN(PIN_Z_LIMIT,              I)

    PRINT_DIGITAL_PIN(PIN_TOOL_ENABLE,          I)
    PRINT_DIGITAL_PIN(PIN_TOOL_FEEDBACK,        I)

    PRINT_ANALOG_PIN(PIN_INTERRUPT_LED_ID,      I)

  Serial.write(MBYTE_CLOSE);  

  #undef PRINT_DIGITAL_PIN  
  #undef PRINT_ANALOG_PIN  
  #undef PRINT_ANALOG_PIN_EXT
}
/////////////////////////////////////////////////////////////////////////////////////
inline long isReadyToRun(){
/////////////////////////////////////////////////////////////////////////////////////
  // return value: '1' always is OK, instead '0'
  long ret = controller.isReadyToRun();
  return ret;
}
/////////////////////////////////////////////////////////////////////////////////////
inline void switchToolState(bool state, bool force = false) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( force == false ) {
    if ( controller.isProbeMode() == true ) {
      digitalWrite(PIN_TOOL_ENABLE, TOOL_STATE_OFF);
      return;
    }
  }
  
  digitalWrite(PIN_TOOL_ENABLE, state == true ? TOOL_STATE_ON : TOOL_STATE_OFF);
}
/////////////////////////////////////////////////////////////////////////////////////
inline void switchStepperState(bool state) {
/////////////////////////////////////////////////////////////////////////////////////
  digitalWrite(PIN_STEPPER_ENABLE,  state);
}
/////////////////////////////////////////////////////////////////////////////////////
inline void switchOutputPinState(int pin, bool state) {
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
inline void writeLimitGetter() {
/////////////////////////////////////////////////////////////////////////////////////  
  long x = LimitSwitch::LIMIT_UNKNOWN;
  long y = LimitSwitch::LIMIT_UNKNOWN;
  long z = LimitSwitch::LIMIT_UNKNOWN;

  controller.evaluateLimitStates(x, y, z);
  writeGetterListValues(PID_LIMIT, x, y, z);
}
/////////////////////////////////////////////////////////////////////////////////////
inline unsigned char evaluateGetter(unsigned char pid) {
/////////////////////////////////////////////////////////////////////////////////////

  switch ( pid ) {
    // processGetter() ............................................
    case PID_QUERY_READY_TO_RUN:      writeGetterListValue(PID_QUERY_READY_TO_RUN, isReadyToRun());
        /*                              break;
    // processGetter() ............................................
    case PID_SPEED_OFFSET_X:          writeGetterListValue(PID_SPEED_OFFSET_X, controller.getPerStepSpeedOffsetX());
                                      break;
    // processGetter() ............................................
    case PID_SPEED_OFFSET_Y:          writeGetterListValue(PID_SPEED_OFFSET_Y, controller.getPerStepSpeedOffsetY());
                                      break;
    // processGetter() ............................................
    case PID_SPEED_OFFSET_Z:          writeGetterListValue(PID_SPEED_OFFSET_Z, controller.getPerStepSpeedOffsetZ());
                 */                     break;
    // processGetter() ............................................
    case PID_STEPS_X:                 writeGetterListValue(PID_STEPS_X, controller.getStepperX()->getSteps());
                                      break;
    // processGetter() ............................................
    case PID_STEPS_Y:                 writeGetterListValue(PID_STEPS_Y, controller.getStepperY()->getSteps());
                                      break;
    // processGetter() ............................................
    case PID_STEPS_Z:                 writeGetterListValue(PID_STEPS_Z, controller.getStepperZ()->getSteps());
                                      break;
    // processGetter() ............................................
    case PID_X_POS:                   writeGetterListValue(PID_X_POS, controller.getStepperX()->getPosition());
                                      break;
    // processGetter() ............................................
    case PID_Y_POS:                   writeGetterListValue(PID_Y_POS, controller.getStepperY()->getPosition());
                                      break;
    // processGetter() ............................................
    case PID_Z_POS:                   writeGetterListValue(PID_Z_POS, controller.getStepperZ()->getPosition());
                                      break;
    // processGetter() ............................................
    case PID_XY_POS:                  writeGetterListValues(PID_XY_POS, controller.getStepperX()->getPosition(), controller.getStepperY()->getPosition());
                                      break;
    // processGetter() ............................................
    case PID_XYZ_POS:                 writeGetterListValues(PID_XYZ_POS, controller.getStepperX()->getPosition(), controller.getStepperY()->getPosition(), controller.getStepperZ()->getPosition());
                                      break;
    // processGetter() ............................................
    case PID_GET_POS_COUNTER:         writeGetterListValues(PID_GET_POS_COUNTER, controller.getPositionCounter(), controller.getPositionCounterOverflow());
                                      break;
    // processGetter() ............................................
    case PID_GET_STEP_COUNTER_X:      writeGetterListValues(PID_GET_STEP_COUNTER_X, controller.getStepperX()->getStepCounter(), controller.getStepperX()->getStepCounterOverflow());
                                      break;
    // processGetter() ............................................
    case PID_GET_STEP_COUNTER_Y:      writeGetterListValues(PID_GET_STEP_COUNTER_Y, controller.getStepperY()->getStepCounter(), controller.getStepperY()->getStepCounterOverflow());
                                      break;
    // processGetter() ............................................
    case PID_GET_STEP_COUNTER_Z:      writeGetterListValues(PID_GET_STEP_COUNTER_Z, controller.getStepperZ()->getStepCounter(), controller.getStepperZ()->getStepCounterOverflow());
                                      break;
    // processGetter() ............................................
    case PID_LIMIT:                   writeLimitGetter();
                                      break;
    // processGetter() ............................................
    case PID_AVG_STEP_DURATION:  writeGetterListValues(PID_AVG_STEP_DURATION, controller.getStepperX()->getAvgStepDuration(), 
                                                                              controller.getStepperY()->getAvgStepDuration(), 
                                                                              controller.getStepperZ()->getAvgStepDuration());
                                      break;
    // processGetter() ............................................
    case PID_POS_REPLY_THRESHOLD_X:   writeGetterListValue(PID_POS_REPLY_THRESHOLD_X, controller.getPosReplyThresholdX());
                                      break;
    // processGetter() ............................................
    case PID_POS_REPLY_THRESHOLD_Y:   writeGetterListValue(PID_POS_REPLY_THRESHOLD_Y, controller.getPosReplyThresholdY());
                                      break;
    // processGetter() ............................................
    case PID_POS_REPLY_THRESHOLD_Z:   writeGetterListValue(PID_POS_REPLY_THRESHOLD_Z, controller.getPosReplyThresholdZ());
                                      break;
    // processGetter() ............................................
    case PID_ERROR_COUNT:             writeGetterListValue(PID_ERROR_COUNT, errorInfo.getErrorCount());
                                      break;
    // processGetter() ............................................
    default:                          writeGetterListValue(PID_UNKNOWN, 0);
                                      return errorInfo.setNextErrorInfo(E_INVALID_GETTER_ID, String((int)pid).c_str());
  }

  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
// provides information to the serial
//   Serial.write(RET_SOH);
//   Serial.write(PID_GETTER_LIST);
//   Serial.write(<GETTER_COUNT>);
//   foreach 
//       writeGetterListValue(PID, ....)  
inline unsigned char processGetterList() {
/////////////////////////////////////////////////////////////////////////////////////
  // Wait a protion of time.
  // This is very importent for the next multibyte read
  delay(1);

  Serial.write(RET_SOH);
  Serial.write(PID_GETTER_LIST);

  if ( Serial.available() <= 0 ) {
    Serial.write((unsigned char)1);
    writeGetterSingleValue(PID_UNKNOWN, 0);
    
    return errorInfo.setNextErrorInfo(E_INVALID_GETTER_LIST_COUNT, EMPTY_TEXT_VAL);
  }
  
  unsigned char count = Serial.read();
  Serial.write(count);
  
  // over all getter ids
  for ( unsigned char i=0; i<count; i++ ) {
    
    // check if more available
    if ( Serial.available() <= 0 ) {
      writeGetterListValue(PID_UNKNOWN, 0);
      continue;
    }

    // append the next getter
    unsigned char pid = Serial.read();
    evaluateGetter(pid);
  }

  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
// provides information to the serial
//   Serial.write(RET_SOH);
//   writeGetterListValue(PID, ....)  
inline unsigned char processGetter() {
/////////////////////////////////////////////////////////////////////////////////////
  // Wait a protion of time.
  // This is very importent for the next multibyte read
  delay(1);

  Serial.write(RET_SOH);

  if ( Serial.available() <= 0 ) {
    writeGetterSingleValue(PID_UNKNOWN, 0);
    return errorInfo.setNextErrorInfo(E_INVALID_GETTER_ID, EMPTY_TEXT_VAL);
  }
  
  unsigned char pid = Serial.read();
  return evaluateGetter(pid);
}
/////////////////////////////////////////////////////////////////////////////////////
// provides information from the serial
inline unsigned char processSetter() {
/////////////////////////////////////////////////////////////////////////////////////
  // Wait a protion of time.
  // This is very importent for the next multibyte read
  delay(1);

  if ( Serial.available() <= 0)
    return errorInfo.setNextErrorInfo(E_INVALID_PARAM_ID, EMPTY_TEXT_VAL);

  unsigned char id = Serial.read();

  byte   b[4];
  long   lValue = 0;
  double dValue = 0.0;
  
  if ( Serial.available() <= 0)
    return errorInfo.setNextErrorInfo(E_INVALID_PARAM_STREAM, "0");
    
  // read a 4 byte value
  unsigned int size = Serial.readBytes(b, 4);
  if ( size != 4 ) {
    String et("["); et.concat((int)id); et.concat("], ");
    et.concat(String(size).c_str());

    return errorInfo.setNextErrorInfo(E_INVALID_PARAM_STREAM, et.c_str());
  }

  // order the bytes
  lValue  = (long)b[0] << 24;
  lValue += (long)b[1] << 16;
  lValue += (long)b[2] << 8;
  lValue += (long)b[3];

  // double conversion
  if ( id >= PID_DOUBLE_RANG_START )  
    convertLongToDouble(lValue, dValue);

   // process id
  switch ( id ) {
    // processSetter() ............................................
    case PID_X_POS:               controller.getStepperX()->setPosition(lValue); 
                                  break;
    case PID_Y_POS:               controller.getStepperY()->setPosition(lValue); 
                                  break;
    case PID_Z_POS:               controller.getStepperZ()->setPosition(lValue); 
                                  break;
    // processSetter() ............................................
    case PID_RESERT_POS_COUNTER:  controller.resetPositionCounter(); 
                                  break;
    case PID_RESERT_STEP_COUNTER: controller.getStepperX()->resetStepCounter(); 
                                  controller.getStepperY()->resetStepCounter();
                                  controller.getStepperZ()->resetStepCounter();   
                                  break;
    // processSetter() ............................................
    case PID_TOOL_SWITCH:         switchToolState(lValue > 0); 
                                  break;
    // processSetter() ............................................
    case PID_POS_REPLY_THRESHOLD_X: 
                                  controller.setPosReplyThresholdX(lValue); 
                                  break;
    case PID_POS_REPLY_THRESHOLD_Y: 
                                  controller.setPosReplyThresholdY(lValue); 
                                  break;
    case PID_POS_REPLY_THRESHOLD_Z: 
                                  controller.setPosReplyThresholdZ(lValue); 
                                  break;
    // processSetter() ............................................
    case PID_X_LIMIT:             controller.getStepperX()->setLimitStateManually(lValue); 
                                  break;
    case PID_Y_LIMIT:             controller.getStepperY()->setLimitStateManually(lValue); 
                                  break;
    case PID_Z_LIMIT:             controller.getStepperZ()->setLimitStateManually(lValue); 
                                  break;
    // processSetter() ............................................
    case PID_STEPS_X:             controller.getStepperX()->setSteps(lValue); 
                                  controller.setupSpeedController();
                                  break;
    case PID_STEPS_Y:             controller.getStepperY()->setSteps(lValue); 
                                  controller.setupSpeedController();
                                  break;
    case PID_STEPS_Z:             controller.getStepperZ()->setSteps(lValue); 
                                  controller.setupSpeedController();
                                  break;
    // processSetter() ............................................
    case PID_PITCH_X:             controller.getStepperX()->setPitch(dValue); 
                                  controller.setupSpeedController();
                                  break;
    case PID_PITCH_Y:             controller.getStepperY()->setPitch(dValue); 
                                  controller.setupSpeedController();
                                  break;
    case PID_PITCH_Z:             controller.getStepperZ()->setPitch(dValue); 
                                  controller.setupSpeedController();
                                  break;
    // processSetter() ............................................
    case PID_PULSE_WIDTH_LOW_X:   controller.getStepperX()->setLowPulseWidth(lValue); 
                                  controller.setupSpeedController();
                                  break;
    case PID_PULSE_WIDTH_LOW_Y:   controller.getStepperY()->setLowPulseWidth(lValue); 
                                  controller.setupSpeedController();
                                  break;
    case PID_PULSE_WIDTH_LOW_Z:   controller.getStepperZ()->setLowPulseWidth(lValue); 
                                  controller.setupSpeedController();
                                  break;
    // processSetter() ............................................
    case PID_PULSE_WIDTH_HIGH_X:  controller.getStepperX()->setHighPulseWidth(lValue); 
                                  controller.setupSpeedController();
                                  break;
    case PID_PULSE_WIDTH_HIGH_Y:  controller.getStepperY()->setHighPulseWidth(lValue); 
                                  controller.setupSpeedController();
                                  break;
    case PID_PULSE_WIDTH_HIGH_Z:  controller.getStepperZ()->setHighPulseWidth(lValue); 
                                  controller.setupSpeedController();
                                  break;
    // processSetter() ............................................
    case PID_TEST_VALUE1:         CncTestSuite::testParam1 = lValue; 
                                  break;
    case PID_TEST_VALUE2:         CncTestSuite::testParam2 = lValue; 
                                  break;
    case PID_TEST_VALUE3:         CncTestSuite::testParam3 = lValue; 
                                  break;
    case PID_TEST_VALUE4:         CncTestSuite::testParam4 = lValue; 
                                  break;
    case PID_TEST_VALUE5:         CncTestSuite::testParam5 = lValue; 
                                  break;
    // processSetter() ............................................
    case PID_SPEED_MM_MIN:        controller.setSpeedValue(dValue); 
                                  //TODO
                                   // controller.setSpeedValue(0.0); 
                                  break;
    // processSetter() ............................................
    // call it with lValue = NORMALIZED_INCREMENT_DIRECTION || INVERSED_INCREMENT_DIRECTION
    case PID_INCREMENT_DIRECTION_VALUE_X:   
                                  controller.getStepperX()->setIncrementDirectionValue(lValue); 
                                  break;
    case PID_INCREMENT_DIRECTION_VALUE_Y:   
                                  controller.getStepperY()->setIncrementDirectionValue(lValue); 
                                  break;
    case PID_INCREMENT_DIRECTION_VALUE_Z:   
                                  controller.getStepperZ()->setIncrementDirectionValue(lValue); 
                                  break;

    // processSetter() ............................................
    default: {
        String et("[");
        et.concat(id);
        et.concat(']');

        return errorInfo.setNextErrorInfo(E_INVALID_PARAM_ID, et.c_str());
      }
  }

  // Wait a protion of time.
  delay(1);

  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
inline unsigned char decodeMove() {
/////////////////////////////////////////////////////////////////////////////////////
  // Wait a protion of time.
  // This is very importent for the next readBytes above
  delayMicroseconds(700);

  byte b[4];
  int32_t v[3];
  unsigned short size  = 0;
  unsigned short count = 0;

  static const short MAX_VALUES = 3;

  //fetch 1 to max 3 long values
  while ( (size = Serial.available()) > 0 ) {
    size = Serial.readBytes(b, sizeof(int32_t));
        
    if ( size != 4 )
      return errorInfo.setNextErrorInfo(E_INVALID_MOVE_CMD, String(size).c_str());

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

  return ( controller.renderAndStepAxisXYZ(x, y, z) == false ? RET_ERROR : RET_OK);
}
/////////////////////////////////////////////////////////////////////////////////////
inline unsigned char decodeTest() {
/////////////////////////////////////////////////////////////////////////////////////
  // Wait a protion of time.
  // This is very importent for the next multibyte read
  delay(1);

  byte b[4];
  long v;
  
  if ( Serial.available() <= 0 )
    return errorInfo.setNextErrorInfo(E_INVALID_TEST_ID, "ID missing");
    
  // fetch a 4 byte value
  const unsigned int size = Serial.readBytes(b, 4);
    
  if ( size != 4 )
    return errorInfo.setNextErrorInfo(E_INVALID_TEST_ID, String(size).c_str());

  // order bytes
  v  = (long)b[0] << 24;
  v += (long)b[1] << 16;
  v += (long)b[2] << 8;
  v += (long)b[3];
    
  CncTestSuite ts(v, controller, errorInfo);
  return ts.run();
}
/////////////////////////////////////////////////////////////////////////////////////
inline void clearSerial() {
/////////////////////////////////////////////////////////////////////////////////////
  delay(1);

  while ( Serial.available() > 0 ) {
    Serial.read();
    delay(1);
  }
}
/////////////////////////////////////////////////////////////////////////////////////
inline char reset() {
/////////////////////////////////////////////////////////////////////////////////////
  // Turn off ...
  switchToolState(TOOL_STATE_OFF, FORCE);
  switchStepperState(ENABLE_STATE_OFF);
  
  // Hide the Interrupt LED
  switchOutputPinState(PIN_INTERRUPT_LED, OFF);

  // stop running tests
  CncTestSuite::resetInterruptFlag();

  // broadcast reset
  controller.reset();
  clearSerial();

  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
inline void processInterrupt() {
/////////////////////////////////////////////////////////////////////////////////////
  // Turn off ...
  switchToolState(TOOL_STATE_OFF, FORCE);
  switchStepperState(ENABLE_STATE_OFF);

  // Show Interrup LED
  switchOutputPinState(PIN_INTERRUPT_LED, ON);

  // stop running tests
  CncTestSuite::interruptTest();

  controller.broadcastInterrupt();
  errorInfo.setNextErrorInfo(E_INTERRUPT, EMPTY_TEXT_VAL);

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
  
    pinMode(PIN_STEPPER_ENABLE,     OUTPUT);  digitalWrite(PIN_STEPPER_ENABLE,    ENABLE_STATE_OFF); 
    pinMode(PIN_TOOL_ENABLE,        OUTPUT);  digitalWrite(PIN_TOOL_ENABLE,       TOOL_STATE_OFF);

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

        // Interrupt
        case SIG_INTERRUPPT:
              processInterrupt();
              return;

        // Resume
        case SIG_RESUME:
              controller.broadcastPause(PAUSE_INACTIVE); 
              return;

        // Pause
        case SIG_PAUSE:
              controller.broadcastPause(PAUSE_ACTIVE); 
              return;

        // Cancel running moves
        case SIG_HALT:
               // In this case here the signal does nothing and goes away.
              return;

    // --------------------------------------------------------------------------
    // Commands - which requires a return value
    // - return default --> r = RET_OK | RET_ERROR
    // --------------------------------------------------------------------------

        // SB command -  Reset
        case CMD_RESET_CONTROLLER:
              r = reset();
              break;

        // SB command -  Reset error info
        case CMD_RESET_ERRORINFO:
              errorInfo.resetErrorInfo();
              break;
  
        // SB command - Idle handling
        case CMD_IDLE:
              controller.idle();
              break;

        // SB command - Heartbeat handling
        case CMD_HEARTBEAT:
              controller.heartbeat();
              break;

        // MB command - Movement
        case CMD_MOVE:               // obsolete command
        case CMD_RENDER_AND_MOVE:
              controller.setPosReplyState(true);
              r = decodeMove();
              controller.getStepperX()->resetDirectionPin();
              controller.getStepperY()->resetDirectionPin();
              controller.getStepperZ()->resetDirectionPin();
              controller.sendCurrentPositions(PID_XYZ_POS_MAJOR, true);
              controller.setPosReplyState(false);
              break;

        // MB command - Parameter getter
        case CMD_GETTER_LIST:
              r = processGetterList();
              break;
          
        // MB command - Parameter getter
        case CMD_GETTER:
              r = processGetter();
              break;

        // SB command - Parameter setter
        case CMD_SETTER:
              r = processSetter();
              break;
  
        // SB command - Enable stepper motors
        case CMD_ENABLE_STEPPER_PIN:
              controller.enableStepperPin(true);
              break;
  
        // SB command - Disable stepper motors
        case CMD_DISABLE_STEPPER_PIN:
              controller.enableStepperPin(false);
              break;
  
        // SB command - Test suite
        case CMD_TEST_START:
              r = decodeTest();
              break;
  
        // SB command - Probe
        case CMD_ENABLE_PROBE_MODE:
              controller.enableProbeMode();
              r = RET_OK;
              break;
              
        // SB command - Probe
        case CMD_DISABLE_PROBE_MODE:
              controller.disableProbeMode();
              r = RET_OK;
              break;

    // --------------------------------------------------------------------------
    // Commands - multi byte return
    // - must always return directly
    // --------------------------------------------------------------------------

        case CMD_TEST_INFO_MESSAGE:
              pushInfoMessage("This is a test message from type: 'INFO'");
              Serial.flush();
              return;

        case CMD_TEST_WARN_MESSAGE:
              pushWarningMessage("This is a test message from type: 'WARNING'");
              Serial.flush();
              return;

        case CMD_TEST_ERROR_MESSAGE:
              pushErrorMessage("This is a test message from type: 'ERROR'");
              Serial.flush();
              return;

        // MB command - Print configuration
        case CMD_PRINT_CONFIG:
              printConfig();
              Serial.flush();
              return;
  
        // MB command - Print version
        case CMD_PRINT_VERSION:
              printSketchVersion();
              Serial.flush();
              return;

        // MB command - Pin report
        case CMD_PRINT_PIN_REPORT:
              printPinReport();
              Serial.flush();
              return;

        // MB command - Send last error
        case CMD_PRINT_ERRORINFO:
              errorInfo.writeErrorInfo();
              Serial.flush();
              return;

        // MB command - Send last error
        case CMD_PRINT_LAST_ERROR_RESPONSE_ID:
              errorInfo.writeLastErrorInfoResponseId();
              Serial.flush();
              return;

    // --------------------------------------------------------------------------
    // Error handling
    // --------------------------------------------------------------------------

        // Unknown commands
        default:
              String et("[");
              et.concat(c);
              et.concat(']');
              
              r = errorInfo.setNextErrorInfo(E_UNKNOW_COMMAND, et.c_str());
  }

  // hand shake
  Serial.write(r);
  Serial.flush();
}


