#define SKETCH_COMPILE = TRUE

#include <SoftwareSerial.h>
#include <Wire.h>
#include "CommonValues.h"
#include "CncStepper.h"
#include "CncController.h"
#include "CommonFunctions.h"

// Global Parameters
CncController controller;

/////////////////////////////////////////////////////////////////////////////////////
inline void printSketchVersion() {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOH);
    Serial.write(PID_TEXT);
    Serial.write(FIRMWARE_VERSION);
  Serial.write(MBYTE_CLOSE);
}
/////////////////////////////////////////////////////////////////////////////////////
inline void printConfig() {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOH);
    Serial.write(PID_TEXT);
    Serial.print(PID_COMMON);        Serial.print(TEXT_SEPARATOR);     Serial.print(BLANK);                                             Serial.print(TEXT_CLOSE);
    Serial.print(BLANK);             Serial.print(PID_VERSION);        Serial.print(TEXT_SEPARATOR); Serial.print(FIRMWARE_VERSION);    Serial.print(TEXT_CLOSE);
  
    controller.printConfig();

  Serial.write(MBYTE_CLOSE);
}
/////////////////////////////////////////////////////////////////////////////////////
inline void printPinReport() {
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
inline long isReadyToRun(){
/////////////////////////////////////////////////////////////////////////////////////
  long ret = controller.isReadyToRun();
  return ret;
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
  writeGetterValues(PID_LIMIT, x, y, z);
}
/////////////////////////////////////////////////////////////////////////////////////
inline unsigned char evaluateGetter(unsigned char pid) {
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

                                      LastErrorCodes::gblErrorMessage = "[";
                                      LastErrorCodes::gblErrorMessage.concat(pid);
                                      LastErrorCodes::gblErrorMessage.concat("]");
                                      pushErrorMessage(E_INVALID_GETTER_ID, LastErrorCodes::gblErrorMessage.c_str());
                                      return RET_ERROR;
  }

  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
inline unsigned char processGetter() {
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
inline unsigned char processSetter() {
/////////////////////////////////////////////////////////////////////////////////////
  // Wait a protion of time.
  // This is very importent for the next multibyte read
  delay(1);

  if ( Serial.available() <= 0) {
    pushErrorMessage(E_INVALID_PARAM_ID);
    return RET_ERROR;
  }
  
  unsigned char id = Serial.read();

  byte   b[4];
  long   lValue = 0;
  double dValue = 0.0;
  
  if ( Serial.available() <= 0) {
    pushErrorMessage(E_INVALID_PARAM_STREAM);
    return RET_ERROR;
  }
    
  // read a 4 byte value
  unsigned int size = Serial.readBytes(b, 4);
  if ( size != 4 ) {
    pushErrorMessage(E_INVALID_PARAM_STREAM);
    return RET_ERROR;
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
    case PID_TOOL_SWITCH:         controller.switchToolState(lValue == 0 ? TOOL_STATE_OFF: TOOL_STATE_ON); 
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
    case PID_SPEED_MM_MIN:        controller.setSpeedValue(dValue); 
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
    case PID_PROBE_MODE:          controller.setProbeMode(lValue != 0);
                                  break;
    // processSetter() ............................................
    case PID_ENABLE_STEPPERS:     controller.enableStepperPin(lValue != 0);
                                  break;
    // processSetter() ............................................
    default:                      pushErrorMessage(E_INVALID_PARAM_ID);
                                  return RET_ERROR;
  }

  // Wait a protion of time.
  delay(1);

  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
inline unsigned char decodeMove(unsigned char cmd) {
/////////////////////////////////////////////////////////////////////////////////////
  // Wait a protion of time.
  // This is very importent for the next readBytes above
  delayMicroseconds(1000);

  byte b[4];
  int32_t v[3];
  unsigned short size  = 0;
  unsigned short count = 0;

  static const short MAX_VALUES = 3;

  //fetch 1 to max 3 long values
  while ( (size = Serial.available()) > 0 ) {
    size = Serial.readBytes(b, sizeof(int32_t));
        
    if ( size != 4 ) {
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
inline void processInterrupt() {
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
              pushInfoMessage("This is a test message from type: 'INFO'");
              r = RET_OK;
              break;

        case CMD_TEST_WARN_MESSAGE:
              pushWarningMessage("This is a test message from type: 'WARNING'");
              r = RET_OK;
              break;

        case CMD_TEST_ERROR_MESSAGE:
              pushErrorMessage("This is a test message from type: 'ERROR'");
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
              LastErrorCodes::gblErrorMessage = "";
              LastErrorCodes::gblErrorMessage.concat('[');
              LastErrorCodes::gblErrorMessage.concat(c);
              LastErrorCodes::gblErrorMessage.concat(']');
              
              pushErrorMessage(E_UNKNOW_COMMAND, LastErrorCodes::gblErrorMessage.c_str());
  }

  // hand shake
  if ( r != RET_NULL ) {
    Serial.write(r);
    Serial.flush();
  }
}


