#define SKETCH_COMPILE = TRUE

#include <SoftwareSerial.h>
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
void printConfig() {
  /////////////////////////////////////////////////////////////////////////////////////
  Serial.write(RET_SOT);
  Serial.print(PID_COMMON);        Serial.print(TEXT_SEPARATOR);     Serial.print(BLANK);                                                            Serial.print(TEXT_CLOSE);
  Serial.print(BLANK);             Serial.print(PID_VERSION);        Serial.print(TEXT_SEPARATOR); Serial.print(FIRMWARE_VERSION);                   Serial.print(TEXT_CLOSE);

  controller.printConfig();
  CncTestSuite::printConfig();

  //Todo append values

  Serial.write(MBYTE_CLOSE);
}
/////////////////////////////////////////////////////////////////////////////////////
void printPinReport() {
  /////////////////////////////////////////////////////////////////////////////////////
  const int d = 'D';
  const int a = 'A';
  const int i = 'I';
  const int o = 'O';

  Serial.write(RET_SOT);

  Serial.print(X_STP);              Serial.print(TEXT_SEPARATOR); Serial.print(d); Serial.print(TEXT_SEPARATOR); Serial.print(o); Serial.print(TEXT_SEPARATOR); Serial.print(digitalRead(X_STP));         Serial.print(TEXT_CLOSE);
  Serial.print(Y_STP);              Serial.print(TEXT_SEPARATOR); Serial.print(d); Serial.print(TEXT_SEPARATOR); Serial.print(o); Serial.print(TEXT_SEPARATOR); Serial.print(digitalRead(Y_STP));         Serial.print(TEXT_CLOSE);
  Serial.print(Z_STP);              Serial.print(TEXT_SEPARATOR); Serial.print(d); Serial.print(TEXT_SEPARATOR); Serial.print(o); Serial.print(TEXT_SEPARATOR); Serial.print(digitalRead(Z_STP));         Serial.print(TEXT_CLOSE);
  Serial.print(X_DIR);              Serial.print(TEXT_SEPARATOR); Serial.print(d); Serial.print(TEXT_SEPARATOR); Serial.print(o); Serial.print(TEXT_SEPARATOR); Serial.print(digitalRead(X_DIR));         Serial.print(TEXT_CLOSE);
  Serial.print(Y_DIR);              Serial.print(TEXT_SEPARATOR); Serial.print(d); Serial.print(TEXT_SEPARATOR); Serial.print(o); Serial.print(TEXT_SEPARATOR); Serial.print(digitalRead(Y_DIR));         Serial.print(TEXT_CLOSE);
  Serial.print(Z_DIR);              Serial.print(TEXT_SEPARATOR); Serial.print(d); Serial.print(TEXT_SEPARATOR); Serial.print(o); Serial.print(TEXT_SEPARATOR); Serial.print(digitalRead(Z_DIR));         Serial.print(TEXT_CLOSE);
  Serial.print(ENABLE_PIN);         Serial.print(TEXT_SEPARATOR); Serial.print(d); Serial.print(TEXT_SEPARATOR); Serial.print(o); Serial.print(TEXT_SEPARATOR); Serial.print(digitalRead(ENABLE_PIN));    Serial.print(TEXT_CLOSE);
  Serial.print(X_LIMIT);            Serial.print(TEXT_SEPARATOR); Serial.print(d); Serial.print(TEXT_SEPARATOR); Serial.print(i); Serial.print(TEXT_SEPARATOR); Serial.print(digitalRead(X_LIMIT));       Serial.print(TEXT_CLOSE);
  Serial.print(Y_LIMIT);            Serial.print(TEXT_SEPARATOR); Serial.print(d); Serial.print(TEXT_SEPARATOR); Serial.print(i); Serial.print(TEXT_SEPARATOR); Serial.print(digitalRead(Y_LIMIT));       Serial.print(TEXT_CLOSE);
  Serial.print(Z_LIMIT);            Serial.print(TEXT_SEPARATOR); Serial.print(d); Serial.print(TEXT_SEPARATOR); Serial.print(i); Serial.print(TEXT_SEPARATOR); Serial.print(digitalRead(Z_LIMIT));       Serial.print(TEXT_CLOSE);
  Serial.print(TOOL_PIN);           Serial.print(TEXT_SEPARATOR); Serial.print(d); Serial.print(TEXT_SEPARATOR); Serial.print(i); Serial.print(TEXT_SEPARATOR); Serial.print(digitalRead(TOOL_PIN));      Serial.print(TEXT_CLOSE);
  Serial.print(SUPPORT_PIN);        Serial.print(TEXT_SEPARATOR); Serial.print(d); Serial.print(TEXT_SEPARATOR); Serial.print(i); Serial.print(TEXT_SEPARATOR); Serial.print(digitalRead(SUPPORT_PIN));   Serial.print(TEXT_CLOSE);
  Serial.print(INTERRUPT_LED_ID);   Serial.print(TEXT_SEPARATOR); Serial.print(a); Serial.print(TEXT_SEPARATOR); Serial.print(o); Serial.print(TEXT_SEPARATOR); Serial.print(analogRead(INTERRUPT_LED));  Serial.print(TEXT_CLOSE);

  Serial.write(MBYTE_CLOSE);  
}
/////////////////////////////////////////////////////////////////////////////////////
long isReadyToRun(){
/////////////////////////////////////////////////////////////////////////////////////
  // return value: '1' always is OK, instead '0'
  long ret = controller.isReadyToRun();
  return ret;
}
/////////////////////////////////////////////////////////////////////////////////////
unsigned char getValue() {
/////////////////////////////////////////////////////////////////////////////////////
  // Wait a protion of time.
  // This is very importent for the next multibyte read
  delay(1);

  unsigned char id = 0;
  if ( Serial.available() > 0 ) {
    id = Serial.read();
  } else {
    writeLongValue(PID_UNKNOWN, 0);
    errorInfo.setNextErrorInfo(E_INVALID_GETTER_ID, EMPTY_TEXT_VAL);
    return RET_ERROR;
  }

  switch ( id ) {
    // getValue() ............................................
    case PID_QUERY_READY_TO_RUN:      writeGetterValues(PID_QUERY_READY_TO_RUN, isReadyToRun());
                                      break;
    // getValue() ............................................
    case PID_SPEED_OFFSET_X:          writeGetterValues(PID_SPEED_OFFSET_X, controller.getPerStepSpeedOffsetX());
                                      break;
    // getValue() ............................................
    case PID_SPEED_OFFSET_Y:          writeGetterValues(PID_SPEED_OFFSET_Y, controller.getPerStepSpeedOffsetY());
                                      break;
    // getValue() ............................................
    case PID_SPEED_OFFSET_Z:          writeGetterValues(PID_SPEED_OFFSET_Z, controller.getPerStepSpeedOffsetZ());
                                      break;
    // getValue() ............................................
    case PID_STEPS_X:                 writeGetterValues(PID_STEPS_X, controller.getStepperX()->getSteps());
                                      break;
    // getValue() ............................................
    case PID_STEPS_Y:                 writeGetterValues(PID_STEPS_Y, controller.getStepperY()->getSteps());
                                      break;
    // getValue() ............................................
    case PID_STEPS_Z:                 writeGetterValues(PID_STEPS_Z, controller.getStepperZ()->getSteps());
                                      break;
    // getValue() ............................................
    case PID_X_POS:                   writeGetterValues(PID_X_POS, controller.getStepperX()->getPosition());
                                      break;
    // getValue() ............................................
    case PID_Y_POS:                   writeGetterValues(PID_Y_POS, controller.getStepperY()->getPosition());
                                      break;
    // getValue() ............................................
    case PID_Z_POS:                   writeGetterValues(PID_Z_POS, controller.getStepperZ()->getPosition());
                                      break;
    // getValue() ............................................
    case PID_XY_POS:                  writeGetterValues(PID_XY_POS, controller.getStepperX()->getPosition(), controller.getStepperY()->getPosition());
                                      break;
    // getValue() ............................................
    case PID_XYZ_POS:                 writeGetterValues(PID_XYZ_POS, controller.getStepperX()->getPosition(), controller.getStepperY()->getPosition(), controller.getStepperZ()->getPosition());
                                      break;
    // getValue() ............................................
    case PID_GET_POS_COUNTER:         writeGetterValues(PID_GET_POS_COUNTER, controller.getPositionCounter(), controller.getPositionCounterOverflow());
                                      break;
    // getValue() ............................................
    case PID_GET_STEP_COUNTER_X:      writeGetterValues(PID_GET_STEP_COUNTER_X, controller.getStepperX()->getStepCounter(), controller.getStepperX()->getStepCounterOverflow());
                                      break;
    // getValue() ............................................
    case PID_GET_STEP_COUNTER_Y:      writeGetterValues(PID_GET_STEP_COUNTER_Y, controller.getStepperY()->getStepCounter(), controller.getStepperY()->getStepCounterOverflow());
                                      break;
    // getValue() ............................................
    case PID_GET_STEP_COUNTER_Z:      writeGetterValues(PID_GET_STEP_COUNTER_Z, controller.getStepperZ()->getStepCounter(), controller.getStepperZ()->getStepCounterOverflow());
                                      break;
    // getValue() ............................................
    case PID_LIMIT:                   writeGetterValues(PID_LIMIT, controller.getStepperX()->readLimitState(), controller.getStepperY()->readLimitState(), controller.getStepperZ()->readLimitState());
                                      break;
    // getValue() ............................................
    case PID_POS_REPLY_THRESHOLD_X:   writeGetterValues(PID_POS_REPLY_THRESHOLD_X, controller.getPosReplyThresholdX());
                                      break;
    // getValue() ............................................
    case PID_POS_REPLY_THRESHOLD_Y:   writeGetterValues(PID_POS_REPLY_THRESHOLD_Y, controller.getPosReplyThresholdY());
                                      break;
    // getValue() ............................................
    case PID_POS_REPLY_THRESHOLD_Z:   writeGetterValues(PID_POS_REPLY_THRESHOLD_Z, controller.getPosReplyThresholdZ());
                                      break;
    // getValue() ............................................
    case PID_ERROR_COUNT:             writeGetterValues(PID_ERROR_COUNT, errorInfo.getErrorCount());
                                      break;

    // getValue() ............................................
    default:                          writeGetterValues(PID_UNKNOWN, 0);
                                      errorInfo.setNextErrorInfo(E_INVALID_GETTER_ID, String((int)id).c_str());
                                      return RET_ERROR;
  }

  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
unsigned char setValue() {
  /////////////////////////////////////////////////////////////////////////////////////
  // Wait a protion of time.
  // This is very importent for the next multibyte read
  delay(10);

  unsigned char id = 0;
  if ( Serial.available() > 0) {
    id = Serial.read();
  } else {
    errorInfo.setNextErrorInfo(E_INVALID_PARAM_ID, EMPTY_TEXT_VAL);
    return RET_ERROR;
  }

  byte b[4];
  long   lValue = 0;
  double dValue = 0.0;
  
  unsigned int size  = 0;
  if ( Serial.available() > 0) {
    size = Serial.readBytes(b, 4);

    if ( size != 4 ) {
      String et("["); et.concat((int)id); et.concat("], ");
      et.concat(String(size).c_str());
      errorInfo.setNextErrorInfo(E_INVALID_PARAM_STREAM, et.c_str());
      return RET_ERROR;
    }

    lValue  = (long)b[0] << 24;
    lValue += (long)b[1] << 16;
    lValue += (long)b[2] << 8;
    lValue += (long)b[3];

    if ( id >= PID_DOUBLE_RANG_START )  
      convertLongToDouble(lValue, dValue);

  } else {
    errorInfo.setNextErrorInfo(E_INVALID_PARAM_STREAM, "0");
    return RET_ERROR;
  }
 
  // process id
  switch ( id ) {
    // setValue() ............................................
    case PID_X_POS:               controller.getStepperX()->setPosition(lValue); break;
    case PID_Y_POS:               controller.getStepperY()->setPosition(lValue); break;
    case PID_Z_POS:               controller.getStepperZ()->setPosition(lValue); break;
    // setValue() ............................................
    case PID_RESERT_POS_COUNTER:  controller.resetPositionCounter(); break;
    case PID_RESERT_STEP_COUNTER: controller.getStepperX()->resetStepCounter(); 
                                  controller.getStepperY()->resetStepCounter();
                                  controller.getStepperZ()->resetStepCounter();   
                                  break;
    // setValue() ............................................
    case PID_STEPS_X:             controller.getStepperX()->setSteps(lValue); break;
    case PID_STEPS_Y:             controller.getStepperY()->setSteps(lValue); break;
    case PID_STEPS_Z:             controller.getStepperZ()->setSteps(lValue); break;
    // setValue() ............................................
    case PID_ROUTER_SWITCH:       switchToolState(lValue > 0); break;
    // setValue() ............................................
    case PID_POS_REPLY_THRESHOLD_X: controller.setPosReplyThresholdX(lValue); break;
    case PID_POS_REPLY_THRESHOLD_Y: controller.setPosReplyThresholdY(lValue); break;
    case PID_POS_REPLY_THRESHOLD_Z: controller.setPosReplyThresholdZ(lValue); break;
    // setValue() ............................................
    case PID_X_LIMIT:             controller.getStepperX()->setLimitStateManually(lValue); break;
    // setValue() ............................................
    case PID_Y_LIMIT:             controller.getStepperY()->setLimitStateManually(lValue); break;
    // setValue() ............................................
    case PID_Z_LIMIT:             controller.getStepperZ()->setLimitStateManually(lValue); break;
    // setValue() ............................................
    case PID_PULSE_WIDTH_OFFSET_X: controller.getStepperX()->setPulseWidthOffset(lValue); break;
    case PID_PULSE_WIDTH_OFFSET_Y: controller.getStepperY()->setPulseWidthOffset(lValue); break;
    case PID_PULSE_WIDTH_OFFSET_Z: controller.getStepperZ()->setPulseWidthOffset(lValue); break;
    // setValue() ............................................
    case PID_TEST_VALUE1:         CncTestSuite::testParam1 = lValue; break;
    case PID_TEST_VALUE2:         CncTestSuite::testParam2 = lValue; break;
    case PID_TEST_VALUE3:         CncTestSuite::testParam3 = lValue; break;
    case PID_TEST_VALUE4:         CncTestSuite::testParam4 = lValue; break;
    case PID_TEST_VALUE5:         CncTestSuite::testParam5 = lValue; break;
    // setValue() ............................................
    case PID_PITCH_X:             controller.getStepperX()->setPitch(dValue); break;
    case PID_PITCH_Y:             controller.getStepperY()->setPitch(dValue); break;
    case PID_PITCH_Z:             controller.getStepperZ()->setPitch(dValue); break;
    // setValue() ............................................
    case PID_SPEED_MM_MIN:        controller.setSpeedValue(dValue);
                                  break;
    // setValue() ............................................
    default: {
        String et("[");
        et.concat(id);
        et.concat(']');
        errorInfo.setNextErrorInfo(E_INVALID_PARAM_ID, et.c_str());
        return RET_ERROR;
      }
  }

  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
char decodeMove() {
  /////////////////////////////////////////////////////////////////////////////////////
  // Wait a protion of time.
  // This is very importent for the next multibyte read
  delay(1);

  byte b[4];
  long v[3];
  unsigned int size  = 0;
  unsigned int count = 0;

  //fetch 1 to max 3 long values
  while ( Serial.available() > 0 ) {
    size = Serial.readBytes(b, 4);
    if ( size != 4 ) {
      errorInfo.setNextErrorInfo(E_INVALID_MOVE_CMD, String(size).c_str());
      return RET_ERROR;
    }

    v[count]  = (long)b[0] << 24;
    v[count] += (long)b[1] << 16;
    v[count] += (long)b[2] << 8;
    v[count] += (long)b[3];
    count++;
  }

  long x=0, y=0, z=0;
  switch ( count ) {
    case 1:   z = v[0]; break;
    case 2:   x = v[0]; y = v[1]; break;
    case 3:   x = v[0]; y = v[1]; z= v[2]; break;
    default:  return RET_ERROR;
  }

  if ( controller.renderAndStepAxisXYZ(x, y, z) == false )
    return RET_ERROR;

  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
char decodeTest() {
  /////////////////////////////////////////////////////////////////////////////////////
  // Wait a protion of time.
  // This is very importent for the next multibyte read
  delay(1);

  byte b[4];
  long v;
  unsigned int size  = 0;

  //fetch long value
  if ( Serial.available() > 0 ) {
    size = Serial.readBytes(b, 4);
    if ( size != 4 ) {
      errorInfo.setNextErrorInfo(E_INVALID_TEST_ID, String(size).c_str());
      return RET_ERROR;
    }

    v  = (long)b[0] << 24;
    v += (long)b[1] << 16;
    v += (long)b[2] << 8;
    v += (long)b[3];
  } else {
    errorInfo.setNextErrorInfo(E_INVALID_TEST_ID, "ID missing");
    return RET_ERROR;
  }

  CncTestSuite ts(v, controller, errorInfo);
  return ts.run();
}
/////////////////////////////////////////////////////////////////////////////////////
void switchToolState(bool state) {
  /////////////////////////////////////////////////////////////////////////////////////
  if ( controller.isProbeMode() == true ) {
    digitalWrite(TOOL_PIN, LOW);
    return;
  }

  if ( state ) digitalWrite(TOOL_PIN, HIGH);
  else         digitalWrite(TOOL_PIN, LOW);
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
char reset() {
  /////////////////////////////////////////////////////////////////////////////////////
  // hide Interrupt LED
  analogWrite(INTERRUPT_LED, 0);

  switchToolState(false);

  // stop running tests
  CncTestSuite::resetInterruptFlag();

  // broadcast reset
  controller.reset();
  clearSerial();

  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
void setup() {
  ////////////////////////////////////////////////////////////////////////////////////
  Serial.begin(BAUD_RATE);
  Serial.setTimeout(1000);
  Serial.flush();

  pinMode(X_DIR, OUTPUT);
  pinMode(Y_DIR, OUTPUT);
  pinMode(Z_DIR, OUTPUT);
  pinMode(X_STP, OUTPUT);
  pinMode(Y_STP, OUTPUT);
  pinMode(Z_STP, OUTPUT);

  pinMode(X_LIMIT, INPUT);
  pinMode(Y_LIMIT, INPUT);
  pinMode(Z_LIMIT, INPUT);

  // HIGH = not activ
  digitalWrite(X_LIMIT, HIGH);
  digitalWrite(Y_LIMIT, HIGH);
  digitalWrite(Z_LIMIT, HIGH);

  // HIGH = not activ
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, HIGH);

  // LOW = tool off
  pinMode(TOOL_PIN, OUTPUT);
  digitalWrite(TOOL_PIN, LOW);

  // HIGH = not activ
  pinMode(SUPPORT_PIN, INPUT);
  digitalWrite(SUPPORT_PIN, HIGH);

  reset();
}
/////////////////////////////////////////////////////////////////////////////////////
void handleInterrupt() {
  /////////////////////////////////////////////////////////////////////////////////////
  digitalWrite(ENABLE_PIN,  HIGH);
  digitalWrite(TOOL_PIN,    LOW);
  digitalWrite(SUPPORT_PIN, HIGH);

  // Show Interrup LED
  analogWrite(INTERRUPT_LED, 255);

  // stop running tests
  CncTestSuite::interruptTest();

  controller.broadcastInterrupt();
  errorInfo.setNextErrorInfo(E_INTERRUPT, EMPTY_TEXT_VAL);
}
/////////////////////////////////////////////////////////////////////////////////////
void loop() {
  ///////////////////////////////////////////////////////////////////////////////////
  if ( Serial.available() > 0 ) {
    char r = RET_OK;
    byte c = Serial.read();

    switch ( c ) {

      // --------------------------------------------------------------------------
      // Signals
      // --------------------------------------------------------------------------

          // Interrupt
          case SIG_INTERRUPPT:
                handleInterrupt();
                return;
  
          // Resume
          case SIG_RESUME:
                controller.broadcastPause(false); // off
                return;

          // Resume
          case SIG_PAUSE:
                controller.broadcastPause(true);  // on
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
                controller.evaluateAndSendLimitStates();
                break;

          // MB command - Movement
          case CMD_MOVE:               // obsolete command
          case CMD_RENDER_AND_MOVE:
                controller.setPosReplyState(true);
                r = decodeMove();
                controller.sendCurrentPositions(PID_XYZ_POS_MAJOR, true);
                controller.setPosReplyState(false);
                break;
  
          // MB command - Parameter getter
          case CMD_GETTER:
                r = getValue();
                break;

          // SB command - Parameter setter
          case CMD_SETTER:
                r = setValue();
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
                return;

          case CMD_TEST_WARN_MESSAGE:
                pushWarningMessage("This is a test message from type: 'WARNING'");
                return;

          case CMD_TEST_ERROR_MESSAGE:
                pushErrorMessage("This is a test message from type: 'ERROR'");
                return;

          // MB command - Print configuration
          case CMD_PRINT_CONFIG:
                printConfig();
                return;
    
          // MB command - Print version
          case CMD_PRINT_VERSION:
                Serial.write(RET_SOT);
                Serial.write(FIRMWARE_VERSION);
                Serial.write(MBYTE_CLOSE);
                return;

          // MB command - Pin report
          case CMD_PRINT_PIN_REPORT:
                printPinReport();
                return;

          // MB command - Send last error
          case CMD_PRINT_ERRORINFO:
                errorInfo.writeErrorInfo();
                return;

          // MB command - Send last error
          case CMD_PRINT_LAST_ERROR_RESPONSE_ID:
                errorInfo.writeLastErrorInfoResponseId();
                return;

      // --------------------------------------------------------------------------
      // Error handling
      // --------------------------------------------------------------------------

          // Unknown commands
          default:
                String et("[");
                et.concat(c);
                et.concat(']');
                errorInfo.setNextErrorInfo(E_UNKNOW_COMMAND, et.c_str());
                r = RET_ERROR;
    }

    // hand shake
    Serial.write(r);
  }
}


