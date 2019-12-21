#include <SoftwareSerial.h>
#include "ArdoVal.h"
#include "ArdoEnv.h"
#include "MainLoop.h"
#include "CncStep.h"
#include "CncCtrl.h"

#ifndef SKETCH_COMPILE 
  #define CNC_CONTROLLER_LOG_FUNCTION()  \
    { \
        const wxString logStr(wxString::Format("Controller::%s", __FUNCTION__)); \
        ARDO_DEBUG_MESSAGE('D', logStr); \
    }
#else
  #define CNC_CONTROLLER_LOG_FUNCTION()
#endif

/////////////////////////////////////////////////////////////////////////////////////
CncArduinoController::CncArduinoController()
/////////////////////////////////////////////////////////////////////////////////////
: ArduinoCmdDecoderGetter       () 
, ArduinoCmdDecoderSetter       ()
, ArduinoCmdDecoderMove         () 
, ArduinoCmdDecoderMoveSequence ()
, ArduinoPositionRenderer       ()
, ArduinoPulsWidthManager       ()
, X                             ( new CncAxisX(StepperSetup( this, PIN_X_STP, PIN_X_DIR, PIN_X_LIMIT )) )
, Y                             ( new CncAxisY(StepperSetup( this, PIN_Y_STP, PIN_Y_DIR, PIN_Y_LIMIT )) )
, Z                             ( new CncAxisZ(StepperSetup( this, PIN_Z_STP, PIN_Z_DIR, PIN_Z_LIMIT )) )
, lastI2CData                   ()
, posReplyState                 (OFF)
, probeMode                     (OFF)
, pause                         (OFF)
, I2CAvailable                  (false)
, posReplyThresholdX            (100L)
, posReplyThresholdY            (100L)
, posReplyThresholdZ            (100L)
, positionCounter               (MIN_LONG)
, positionCounterOverflow       (0L) 
, lastHeartbeat                 (0L)
{  
}
/////////////////////////////////////////////////////////////////////////////////////
CncArduinoController::~CncArduinoController() {
/////////////////////////////////////////////////////////////////////////////////////
  delete X; X = NULL;
  delete Y; Y = NULL;
  delete Z; Z = NULL;
}

/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::setDirectionX(int32_t steps) { return X->setDirection(steps); }
byte CncArduinoController::setDirectionY(int32_t steps) { return Y->setDirection(steps); }
byte CncArduinoController::setDirectionZ(int32_t steps) { return Z->setDirection(steps); }
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::printConfig() {
/////////////////////////////////////////////////////////////////////////////////////
  #define PRINT_PARAMETER( Pid, value ) \
      Serial.print(BLANK); \
      Serial.print(Pid);   Serial.print(TEXT_SEPARATOR); \
      Serial.print(value); Serial.write(TEXT_CLOSE);

  Serial.print(PID_CONTROLLER); Serial.print(TEXT_SEPARATOR); Serial.write(TEXT_CLOSE);

    PRINT_PARAMETER(PID_POS_REPLY_THRESHOLD_X,            getPosReplyThresholdX())
    PRINT_PARAMETER(PID_POS_REPLY_THRESHOLD_Y,            getPosReplyThresholdY())
    PRINT_PARAMETER(PID_POS_REPLY_THRESHOLD_Z,            getPosReplyThresholdZ())
    PRINT_PARAMETER(PID_PROBE_MODE,                       isProbeMode())
    PRINT_PARAMETER(PID_ENABLE_STEPPERS,                  AE::digitalRead(PIN_STEPPER_ENABLE) == ENABLE_STATE_ON)
    PRINT_PARAMETER(PID_TOOL_SWITCH,                      AE::digitalRead(PIN_TOOL_ENABLE)    == TOOL_STATE_ON)
    PRINT_PARAMETER(PID_I2C_AVAILABEL,                    isI2CAvailable())

    int limitState = -1, supportState = -1;
    if ( isI2CAvailable() == true ) {
      if ( ArduinoMainLoop::readI2CSlave(lastI2CData) ) {
        limitState   = (int)lastI2CData.limitState;
        supportState = (int)lastI2CData.supportState;
      }
    }
    
    PRINT_PARAMETER(PID_I2C_LIMIT_VALUE,                 limitState)
    PRINT_PARAMETER(PID_I2C_SUPPORT_VALUE,               supportState)

  printPwmConfig();
  
  X->printConfig();
  Y->printConfig();
  Z->printConfig();
  
  #undef PRINT_PARAMETER
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::evaluateI2CAvailable() {
/////////////////////////////////////////////////////////////////////////////////////
  CNC_CONTROLLER_LOG_FUNCTION();
  
  // first: try to request data
  I2CAvailable = ArduinoMainLoop::readI2CSlave(lastI2CData);

  // On demand try a second one
  if ( I2CAvailable == false ) {
    AE::delay(1000);
    I2CAvailable = ArduinoMainLoop::readI2CSlave(lastI2CData);
  }

  ARDO_DEBUG_VALUE("I2CAvailable", I2CAvailable);
  return I2CAvailable;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::evaluateI2CData() {
/////////////////////////////////////////////////////////////////////////////////////
  if ( isI2CAvailable() == false )
    return false;
  
  // request data
  return ArduinoMainLoop::readI2CSlave(lastI2CData);
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::reset() {
/////////////////////////////////////////////////////////////////////////////////////
    CNC_CONTROLLER_LOG_FUNCTION();
    
    posReplyThresholdX  = 100L;
    posReplyThresholdY  = 100L;
    posReplyThresholdZ  = 100L;

    posReplyState       = false;
    probeMode           = false;
    
    X->reset();
    Y->reset();
    Z->reset();
    
    X->resetPosition();
    Y->resetPosition();
    Z->resetPosition();

    resetPositionCounter();
}
/////////////////////////////////////////////////////////////////////////////////////
int32_t CncArduinoController::isReadyToRun() {
/////////////////////////////////////////////////////////////////////////////////////
  CNC_CONTROLLER_LOG_FUNCTION();
  int32_t ret = 1L;

  if ( isPwmInitialized() == false )   { 
    ArduinoMainLoop::pushErrorMessage(E_SPEED_MGMT_NOT_INITIALIZED);  
    return 0L; 
  }

  if ( X->isReadyToRun() == 0L ) { 
    LastErrorCodes::clear(); 
    LastErrorCodes::messageText = "X"; 
    ArduinoMainLoop::pushErrorMessage(E_STEPPER_NOT_READY_TO_RUN);    
    ret = 0L; 
  }
  
  if ( Y->isReadyToRun() == 0L ){ 
    LastErrorCodes::clear(); 
    LastErrorCodes::messageText = "Y"; 
    ArduinoMainLoop::pushErrorMessage(E_STEPPER_NOT_READY_TO_RUN);    
    ret = 0L; 
  }
  
  if ( Z->isReadyToRun() == 0L ) { 
    LastErrorCodes::clear(); 
    LastErrorCodes::messageText = "Z"; 
    ArduinoMainLoop::pushErrorMessage(E_STEPPER_NOT_READY_TO_RUN);    
    ret = 0L; 
  }
  
  return ret;
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::broadcastInterrupt() {
/////////////////////////////////////////////////////////////////////////////////////
  // Turn off ...
  switchToolState     (TOOL_STATE_OFF, FORCE);
  switchStepperState  (ENABLE_STATE_OFF);

  // Show Interrupt LED
  ArduinoMainLoop::switchOutputPinState(PIN_INTERRUPT_LED, ON);

  X->interrupt();
  Y->interrupt();
  Z->interrupt();
  
  ArduinoMainLoop::pushErrorMessage(E_INTERRUPT);
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::incPositionCounter() { 
/////////////////////////////////////////////////////////////////////////////////////
  // detect overflows
  if ( positionCounter == MAX_LONG ) { 
    positionCounter = MIN_LONG;
    positionCounterOverflow++;
  }
    
  positionCounter++;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::sendCurrentLimitStates(bool force) {
/////////////////////////////////////////////////////////////////////////////////////
// This method evaluate the limit states by the current stepper states

  const int32_t x = X->getLimitState();
  const int32_t y = Y->getLimitState();
  const int32_t z = Z->getLimitState();

  // the states will be only sent if one of them is activ or they should be forced
  if ( x != LimitSwitch::LIMIT_UNSET || y != LimitSwitch::LIMIT_UNSET || z != LimitSwitch::LIMIT_UNSET || force == true )
    ArduinoMainLoop::writeLongValues(PID_LIMIT, x, y, z);
    
  return (x != LimitSwitch::LIMIT_UNSET && y != LimitSwitch::LIMIT_UNSET && z != LimitSwitch::LIMIT_UNSET );
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::processHeartbeat() {
/////////////////////////////////////////////////////////////////////////////////////
  unsigned char limitState    = 0;
  unsigned char supportState  = 0;

  if ( isI2CAvailable() == true ) {
    if ( ArduinoMainLoop::readI2CSlave(lastI2CData) ) {
      limitState   = lastI2CData.limitState; 
      supportState = lastI2CData.supportState;
    }
  } else {
    int32_t x = X->readLimitState(DIRECTION_UNKNOWN);
    int32_t y = Y->readLimitState(DIRECTION_UNKNOWN);
    int32_t z = Z->readLimitState(DIRECTION_UNKNOWN);
    CncInterface::ILS::States ls(x, y, z);   
    limitState = ls.getValue();
  }
  
  ArduinoMainLoop::sendHeartbeat(limitState, supportState);
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::processIdle() {
/////////////////////////////////////////////////////////////////////////////////////
  return processHeartbeat();
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::setSpeedValue(double fm, bool activateAcceleration) { 
/////////////////////////////////////////////////////////////////////////////////////
  #warning impl. missing  
/*  speedController.setFeedSpeed_MM_MIN(fm);  

  typedef RenderStruct RS;
  
  if ( speedController.isSpeedConfigured() ) {
    RS::pwmPX.speedDelay = speedController.X.synthSpeedDelay;
    RS::pwmPY.speedDelay = speedController.Y.synthSpeedDelay;
    RS::pwmPZ.speedDelay = speedController.Z.synthSpeedDelay;
    
    if ( activateAcceleration )
      speedController.enableAccelerationXYZ(true);
    
  } else {
    
    RS::pwmPX.speedDelay = 0;
    RS::pwmPY.speedDelay = 0;
    RS::pwmPZ.speedDelay = 0;
    speedController.enableAccelerationXYZ(false);
  }
*/
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::setupAccelProfile(int32_t v1, int32_t v2, int32_t v3, int32_t v4, int32_t v5, int32_t v6) {
/////////////////////////////////////////////////////////////////////////////////////
  #warning impl. missing  
/*
  typedef RenderStruct RS;
  
  RS::pwmPX.startSpeed_MM_SEC = v1;
  RS::pwmPX.stopSpeed_MM_SEC  = v2;
  RS::pwmPY.startSpeed_MM_SEC = v3;
  RS::pwmPY.stopSpeed_MM_SEC  = v4;
  RS::pwmPZ.startSpeed_MM_SEC = v5;
  RS::pwmPZ.stopSpeed_MM_SEC  = v6;
*/  
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::switchStepperState(bool state) {
/////////////////////////////////////////////////////////////////////////////////////
  AE::digitalWrite(PIN_STEPPER_ENABLE,  state);
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::switchToolState(bool state, bool force) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( force == false ) {
    if ( isProbeMode() == true ) {
      AE::digitalWrite(PIN_TOOL_ENABLE, TOOL_STATE_OFF);
      return;
    }
  }
  
  AE::digitalWrite(PIN_TOOL_ENABLE, state);

  // give the tool a portion of time to run properly
  if ( state == TOOL_STATE_ON )
    AE::delay(500);
}
//////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::enableStepperPin(bool state){
//////////////////////////////////////////////////////////////////////////////
  if ( probeMode == false )   AE::digitalWrite(PIN_STEPPER_ENABLE, state == true ? ENABLE_STATE_ON : ENABLE_STATE_OFF);
  else                        AE::digitalWrite(PIN_STEPPER_ENABLE, ENABLE_STATE_OFF);

  AE::delayMicroseconds(10);
  return state;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::isAnyLimitActive() {
/////////////////////////////////////////////////////////////////////////////////////
  static int32_t xLimit, yLimit, zLimit;
  return evaluateLimitStates(xLimit, yLimit, zLimit);
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::evaluateLimitStates(int32_t& xLimit, int32_t& yLimit, int32_t& zLimit) {
/////////////////////////////////////////////////////////////////////////////////////
  // call the steppers to read their individual pins
  xLimit = X->readLimitState(DIRECTION_UNKNOWN);
  yLimit = Y->readLimitState(DIRECTION_UNKNOWN);
  zLimit = Z->readLimitState(DIRECTION_UNKNOWN);

  // try to get a better information for unclear behaviours
  if (    xLimit == LimitSwitch::LIMIT_UNKNOWN 
       || yLimit == LimitSwitch::LIMIT_UNKNOWN 
       || zLimit == LimitSwitch::LIMIT_UNKNOWN ) 
  {
    // query the I2C channel
    if ( isI2CAvailable() == true ) {
      
      // read it from analog pin
      CncInterface::ILS::States ls;
      if ( ArduinoMainLoop::readI2CSlave(lastI2CData) ) {

        CncInterface::ILS::States ls(lastI2CData.limitState);
        if ( ls.hasError() == false ) {

          // override the stepper states on demand
          if ( xLimit == LimitSwitch::LIMIT_UNKNOWN ) 
            xLimit = ls.xLimit();
          
          if ( yLimit == LimitSwitch::LIMIT_UNKNOWN )
            yLimit = ls.yLimit();
          
          if ( zLimit == LimitSwitch::LIMIT_UNKNOWN )
            zLimit = ls.zLimit();        
        }
      }
    } 
  }

  return ( xLimit != LimitSwitch::LIMIT_UNSET && yLimit != LimitSwitch::LIMIT_UNSET && zLimit != LimitSwitch::LIMIT_UNSET );
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::observeEnablePin() {
/////////////////////////////////////////////////////////////////////////////////////
  if ( isProbeMode() == OFF ) {
    
    if ( AE::digitalRead(PIN_STEPPER_ENABLE) == ENABLE_STATE_OFF ) {
       ArduinoMainLoop::pushErrorMessage(E_STEPPER_NOT_ENALED); 
       return false;
    }
  }
  
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::observeToolState() {
/////////////////////////////////////////////////////////////////////////////////////
  if ( isProbeMode() == OFF ) {

    #warning impl. missign
    /*
    if ( digitalRead(PIN_TOOL_FEEDBACK) == TOOL_STATE_OFF ) {
      errorInfo->setNextErrorInfo(E_TOOL_NOT_ENALED);
      return false;
    }
    */
  }
  
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::observeSerialFrontByte(byte& retValue) {
/////////////////////////////////////////////////////////////////////////////////////
  byte serialFrontByte = CMD_INVALID;
  
  if ( ArduinoMainLoop::peakSerial(serialFrontByte) == false ) {
    // nothing available
    retValue = RET_OK;
    return true;
  }

  if ( ArdoObj::isSignal(serialFrontByte) == true ) {
    // remove the signal from serial
    Serial.read();

    // process the signal
    return processSignal(serialFrontByte, retValue);
  }

  // no signal detected, leave serial unchanged
  retValue = RET_OK;
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::processGetter() {
/////////////////////////////////////////////////////////////////////////////////////
  return decodeGetter();  
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::processSetter() {
/////////////////////////////////////////////////////////////////////////////////////
  return decodeSetter();    
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::processMove(byte cmd) {
/////////////////////////////////////////////////////////////////////////////////////
  setPosReplyState(true);
  byte ret = decodeMove(cmd);

  if ( true ) {
    X->resetDirectionPin();
    Y->resetDirectionPin();
    Z->resetDirectionPin();
  }
  
  sendCurrentPositions(PID_XYZ_POS_MAJOR, true);

  setPosReplyState(false);
  return ret;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::processSignal(byte sig) { byte b; return processSignal(sig, b); }
bool CncArduinoController::processSignal(byte sig, byte& retValue) {
/////////////////////////////////////////////////////////////////////////////////////
  switch ( sig ) {
    case SIG_SOFTWARE_RESET:
                #warning reactivate it again  
                //softwareReset();   
                return false;
    
    // interrupt handling
    case SIG_INTERRUPPT:
                // dont remove the signal from serial, so an explizit reset have to be called by the interface
                broadcastInterrupt();

                // Signalize an error
                retValue = RET_INTERRUPT;
                return false;

    case SIG_HALT:
                // remove the signal from serial
                Serial.read();
                
                // Options:
                //  - Returning false here signalize an error and the complete run cycle (PC) stopps as a result.
                //  - Returning true here stopps the current move (while loop), so far so good, but the current run cycle 
                //    continue with the next existing move command which is not the meaning of HALT.
                switchToolState(TOOL_STATE_OFF, FORCE);
                
                retValue = RET_HALT;
                return false;
                
    case SIG_QUIT_MOVE:
                // remove the signal from serial
                Serial.read();
                
                retValue = RET_QUIT;
                return false; 

    case SIG_PAUSE:
                // remove the signal from serial
                Serial.read();
                pause = PAUSE_ACTIVE;
                
                 // Don't return here - see the pause handling below
                 
                break;

    case SIG_RESUME:
                // remove the signal from serial
                Serial.read();
                pause = PAUSE_INACTIVE;
                
                // Don't return here - see the pause handling below
                break;
  }


  // ----------------------------------------------------------
  // pause handling
  if ( pause == PAUSE_ACTIVE ) {

     static const short PAUSE_WAIT_DELAY = 50;
     static const short HB_MOD           = 1000 / PAUSE_WAIT_DELAY;
     
     switchToolState( pause == PAUSE_ACTIVE ? TOOL_STATE_OFF : TOOL_STATE_ON, FORCE);

     // loop
     unsigned short counter = 0;
     while ( ArduinoMainLoop::checkSerialForPauseCommands(pause) == PAUSE_ACTIVE ) {
         
      if ( counter%HB_MOD == 0 )
        ArduinoMainLoop::sendHeartbeat();
        
      AE::delay(PAUSE_WAIT_DELAY);
      counter ++;
     }
     
     pause = PAUSE_INACTIVE;
     switchToolState( pause == PAUSE_ACTIVE ? TOOL_STATE_OFF : TOOL_STATE_ON, FORCE);
  }
  
  retValue = RET_OK;
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::sendCurrentPositions(unsigned char pid, bool force) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( posReplyState == false )
    return;

  if (   X->getPosReplyThresholdCouter() >= posReplyThresholdX
      || Y->getPosReplyThresholdCouter() >= posReplyThresholdY
      || Z->getPosReplyThresholdCouter() >= posReplyThresholdZ
      || force == true
     ) 
  {
    X->resetPosReplyThresholdCouter();
    Y->resetPosReplyThresholdCouter();
    Z->resetPosReplyThresholdCouter();

    const double speed_MM_MIN = 0.0; //speedController.getRealtimeFeedSpeed_MM_MIN() * DBL_FACT
    
    switch ( pid ) {
      case PID_X_POS: ArduinoMainLoop::writeLongValue(pid, X->getPosition());
                      break;
                      
      case PID_Y_POS: ArduinoMainLoop::writeLongValue(pid, Y->getPosition()); 
                      break;
                      
      case PID_Z_POS: ArduinoMainLoop::writeLongValue(pid, Z->getPosition());
                      break;

      case PID_XYZ_POS: 
      case PID_XYZ_POS_MAJOR: 
      case PID_XYZ_POS_DETAIL: 
                      ArduinoMainLoop::writeLongValues(pid, 
                                      X->getPosition(),
                                      Y->getPosition(),
                                      Z->getPosition(),
                                      speed_MM_MIN);
                      break;

      default:        ; // do nothing

    }

    //Serial.flush();
  }
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::checkRuntimeEnv() {
/////////////////////////////////////////////////////////////////////////////////////
  // TODO: speed up the performance and observate in intervalls
  // blind flying: e.g. 16 * 0,015 (12 mm / 800 steps) = 0,24 mm
  if ( positionCounter%1 == 0 ) {
    
    byte retValue = RET_ERROR;
    if ( observeSerialFrontByte(retValue) == false )
      return retValue;

    if ( observeEnablePin() == false )
      return RET_ERROR;
  
    if ( observeToolState() == false )
      return RET_ERROR;
  }

  return RET_OK;   
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::notifyPositionChange() {
/////////////////////////////////////////////////////////////////////////////////////
  incPositionCounter();
  sendCurrentPositions(PID_XYZ_POS_DETAIL, false);  
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::performNextStepX() { 
/////////////////////////////////////////////////////////////////////////////////////
  X->getPwnProfile().accelDelay = 0; //getNextAccelDelayX();
  return X->performNextStep();   
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::performNextStepY() { 
/////////////////////////////////////////////////////////////////////////////////////
  Y->getPwnProfile().accelDelay = 0; //getNextAccelDelayY();
  return Y->performNextStep();   
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::performNextStepZ() { 
/////////////////////////////////////////////////////////////////////////////////////
  Z->getPwnProfile().accelDelay = 0; //getNextAccelDelayZ();
  return Z->performNextStep();   
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::process(const ArduinoCmdDecoderGetter::Result& gt) {
/////////////////////////////////////////////////////////////////////////////////////
#ifndef SKETCH_COMPILE   

  #define DBG_GETTER1(p, v1) { \
    if ( AE::ardoConfigGetTraceGetters() ) { \
      ARDO_DEBUG_VALUE("Getter PID", wxString::Format("[%03d] - %s(%ld)", (int)gt.pid, AE::ardoGetPidLabel(gt.pid), v1)); \
    } \
  }
    
  #define DBG_GETTER2(p, v1, v2) { \
    if ( AE::ardoConfigGetTraceGetters() ) { \
      ARDO_DEBUG_VALUE("Getter PID", wxString::Format("[%03d] - %s(%ld, %ld)", (int)gt.pid, AE::ardoGetPidLabel(gt.pid), v1, v2)); \
    } \
  }
    
  #define DBG_GETTER3(p, v1, v2, v3) { \
    if ( AE::ardoConfigGetTraceGetters() ) { \
      ARDO_DEBUG_VALUE("Getter PID", wxString::Format("[%03d] - %s(%ld, %ld, %ld)", (int)gt.pid, AE::ardoGetPidLabel(gt.pid), v1, v2, v3)); \
    } \
  }

#else

  #define DBG_GETTER1(p, v1)
  #define DBG_GETTER2(p, v1, v2)
  #define DBG_GETTER3(p, v1, v2, v3)
  
#endif 

  typedef ArduinoMainLoop AML;

  //---------------------------------------------------------------------------------
  auto writeGetterValue1 = [&](unsigned char pid, int32_t val1 )                              { DBG_GETTER1(pid, val1);             AML::writeGetterValue (pid, val1);              };
  auto writeGetterValue2 = [&](unsigned char pid, int32_t val1, int32_t val2)                 { DBG_GETTER2(pid, val1, val2);       AML::writeGetterValues(pid, val1, val2);        };
  auto writeGetterValue3 = [&](unsigned char pid, int32_t val1, int32_t val2, int32_t val3 )  { DBG_GETTER3(pid, val1, val2, val3); AML::writeGetterValues(pid, val1, val2, val3);  };
  
  auto writeLimitGetter = [&]() {
    int32_t x = LimitSwitch::LIMIT_UNKNOWN;
    int32_t y = LimitSwitch::LIMIT_UNKNOWN;
    int32_t z = LimitSwitch::LIMIT_UNKNOWN;

    evaluateLimitStates(x, y, z);
    writeGetterValue3(PID_LIMIT, x, y, z);
  };

  //---------------------------------------------------------------------------------
  switch ( gt.pid ) {
    
    case PID_QUERY_READY_TO_RUN:      writeGetterValue1(PID_QUERY_READY_TO_RUN,    isReadyToRun()); break;
    
    case PID_STEPS_X:                 writeGetterValue1(PID_STEPS_X,               X->getSteps()); break;
    case PID_STEPS_Y:                 writeGetterValue1(PID_STEPS_Y,               Y->getSteps()); break;
    case PID_STEPS_Z:                 writeGetterValue1(PID_STEPS_Z,               Z->getSteps()); break;

    case PID_X_POS:                   writeGetterValue1(PID_X_POS,                 X->getPosition()); break;
    case PID_Y_POS:                   writeGetterValue1(PID_Y_POS,                 Y->getPosition()); break;
    case PID_Z_POS:                   writeGetterValue1(PID_Z_POS,                 Z->getPosition()); break;

    case PID_POS_REPLY_THRESHOLD_X:   writeGetterValue1(PID_POS_REPLY_THRESHOLD_X, getPosReplyThresholdX()); break;
    case PID_POS_REPLY_THRESHOLD_Y:   writeGetterValue1(PID_POS_REPLY_THRESHOLD_Y, getPosReplyThresholdY()); break;
    case PID_POS_REPLY_THRESHOLD_Z:   writeGetterValue1(PID_POS_REPLY_THRESHOLD_Z, getPosReplyThresholdZ()); break;

    case PID_XY_POS:                  writeGetterValue2(PID_XY_POS,                X->getPosition(), Y->getPosition()); break;
    case PID_XYZ_POS:                 writeGetterValue3(PID_XYZ_POS,               X->getPosition(), Y->getPosition(), Z->getPosition()); break;

    case PID_GET_POS_COUNTER:         writeGetterValue2(PID_GET_POS_COUNTER,       getPositionCounter(), getPositionCounterOverflow()); break;
    case PID_GET_STEP_COUNTER_X:      writeGetterValue2(PID_GET_STEP_COUNTER_X,    X->getStepCounter(), X->getStepCounterOverflow()); break;
    case PID_GET_STEP_COUNTER_Y:      writeGetterValue2(PID_GET_STEP_COUNTER_Y,    Y->getStepCounter(), Y->getStepCounterOverflow()); break;
    case PID_GET_STEP_COUNTER_Z:      writeGetterValue2(PID_GET_STEP_COUNTER_Z,    Z->getStepCounter(), Z->getStepCounterOverflow()); break; 

    case PID_AVG_STEP_DURATION:       writeGetterValue3(PID_AVG_STEP_DURATION,     X->getAvgStepDuration(), Y->getAvgStepDuration(), Z->getAvgStepDuration()); break;
    
    case PID_LIMIT:                   writeLimitGetter(); break;
    
    default:                          writeGetterValue1(PID_UNKNOWN, 0);
                                      
                                      LastErrorCodes::clear();
                                      LastErrorCodes::register1Byte_A = gt.pid;
                                      AML::pushErrorMessage(E_INVALID_GETTER_ID);
                                      return RET_ERROR;
  }

  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::process(const ArduinoCmdDecoderSetter::Result& st) {
/////////////////////////////////////////////////////////////////////////////////////
#ifndef SKETCH_COMPILE   
  if ( AE::ardoConfigGetTraceSetters() ) {
    ARDO_DEBUG_VALUE("Setter PID", wxString::Format("[%03d] - %s", (int)st.pid, AE::ardoGetPidLabel(st.pid)));

    wxString values;
    for( int i=0; i<st.valueCount; i++ ) {
      if ( st.values[i].isDouble ) { values.append(wxString::Format("[%d]=%lf", i, st.values[i].d)); } 
      else                         { values.append(wxString::Format("[%d]=%ld", i, st.values[i].l)); }

      if ( i != st.valueCount - 1 )
        values.append(",");
    }

    ARDO_DEBUG_VALUE(" Values", wxString::Format("{%s}", values));
  }
#endif
  
  typedef ArduinoMainLoop AML;
  //---------------------------------------------------------------------------------
  switch ( st.pid ) {
    case PID_X_POS:                   X->setPosition(st.values[0].l); break;
    case PID_Y_POS:                   Y->setPosition(st.values[0].l); break;
    case PID_Z_POS:                   Z->setPosition(st.values[0].l); break;
    
    case PID_X_LIMIT:                 X->setLimitStateManually(st.values[0].l); break;
    case PID_Y_LIMIT:                 Y->setLimitStateManually(st.values[0].l); break;
    case PID_Z_LIMIT:                 Z->setLimitStateManually(st.values[0].l); break;
    
    case PID_STEPS_X:                 X->setSteps(st.values[0].l); setupPwmManager(); break;
    case PID_STEPS_Y:                 Y->setSteps(st.values[0].l); setupPwmManager(); break;
    case PID_STEPS_Z:                 Z->setSteps(st.values[0].l); setupPwmManager(); break;
    
    case PID_PITCH_X:                 X->setPitch(st.values[0].d); setupPwmManager(); break;
    case PID_PITCH_Y:                 Y->setPitch(st.values[0].d); setupPwmManager(); break;
    case PID_PITCH_Z:                 Z->setPitch(st.values[0].d); setupPwmManager(); break;

    case PID_PULSE_WIDTH_LOW_X:       X->setLowPulseWidth(st.values[0].l); setupPwmManager(); break;
    case PID_PULSE_WIDTH_LOW_Y:       Y->setLowPulseWidth(st.values[0].l); setupPwmManager(); break;
    case PID_PULSE_WIDTH_LOW_Z:       Z->setLowPulseWidth(st.values[0].l); setupPwmManager(); break;

    case PID_PULSE_WIDTH_HIGH_X:      X->setHighPulseWidth(st.values[0].l); setupPwmManager(); break;
    case PID_PULSE_WIDTH_HIGH_Y:      Y->setHighPulseWidth(st.values[0].l); setupPwmManager(); break;
    case PID_PULSE_WIDTH_HIGH_Z:      Z->setHighPulseWidth(st.values[0].l); setupPwmManager(); break;

    case PID_RESERT_STEP_COUNTER:     X->resetStepCounter(); Y->resetStepCounter(); Z->resetStepCounter(); break;
  
    case PID_INC_DIRECTION_VALUE_X:   X->setIncrementDirectionValue(st.values[0].l); break;
    case PID_INC_DIRECTION_VALUE_Y:   Y->setIncrementDirectionValue(st.values[0].l); break;
    case PID_INC_DIRECTION_VALUE_Z:   Z->setIncrementDirectionValue(st.values[0].l); break;
    
    case PID_POS_REPLY_THRESHOLD_X:   setPosReplyThresholdX(st.values[0].l); break;
    case PID_POS_REPLY_THRESHOLD_Y:   setPosReplyThresholdY(st.values[0].l); break;
    case PID_POS_REPLY_THRESHOLD_Z:   setPosReplyThresholdZ(st.values[0].l); break;

    case PID_SPEED_MM_MIN:            setSpeedValue(st.values[0].d); break;
    case PID_SPEED_FEED_MODE:         break; // currently nothing to do

    case PID_ENABLE_STEPPERS:         enableStepperPin(st.values[0].l != 0); break;
    case PID_RESERT_POS_COUNTER:      resetPositionCounter(); break;

    case PID_PROBE_MODE:              setProbeMode(st.values[0].l != 0); break;
    case PID_TOOL_SWITCH:             switchToolState(st.values[0].l == 0 ? TOOL_STATE_OFF: TOOL_STATE_ON); break;
    
    case PID_ACCEL_PROFILE:           setupAccelProfile(st.values[0].l, st.values[1].l, st.values[2].l, st.values[3].l, st.values[4].l, st.values[5].l); 
                                      setupPwmManager(); break;

    default:                          LastErrorCodes::clear();
                                      LastErrorCodes::register1Byte_A = st.pid;
                                      AML::pushErrorMessage(E_INVALID_PARAM_ID);
                                      return RET_ERROR;
  }
  
  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::process(const ArduinoCmdDecoderMove::Result& mv) {
/////////////////////////////////////////////////////////////////////////////////////
  // select underlying mechanism 
  switch ( mv.cmd ) {
    case CMD_MOVE_UNIT_SIGNAL:  return moveUntilSignal (mv.dx, mv.dy, mv.dz);
    default:                    return movePosition    (mv.dx, mv.dy, mv.dz);
  }

  return RET_ERROR;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::process(const ArduinoCmdDecoderMoveSequence::Result& seq) {
/////////////////////////////////////////////////////////////////////////////////////
  #warning impl. missing
  return RET_ERROR;
}




/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::movePosition(int32_t dx, int32_t dy, int32_t dz) {
/////////////////////////////////////////////////////////////////////////////////////
  
  #warning impl. missing

  // avoid empty processing
  if ( dx == 0 && dy == 0 && dz == 0 ) 
    return RET_OK;

  //const unsigned long tsStart = micros();
 // const unsigned long tsStart = speedController.getTimeStamp();
  
 // if ( isProbeMode() == OFF )
  //  speedController.initMove(dx, dy, dz);



  //ARDO_DEBUG_VALUE(" dx", dx)
  //ARDO_DEBUG_VALUE(" dy", dy)
  //ARDO_DEBUG_VALUE(" dz", dz)


  byte ret = renderMove(dx, dy, dz);
  
  //ARDO_DEBUG_VALUE(" renderMove", (int)ret)

  
/*  
  if ( speedController.isSpeedConfigured() ) {
    const unsigned long timeElapsed = micros() - tsStart;
  
    // micros(): Returns the number of microseconds since the Arduino board began 
    // running the current program. This number will overflow (go back to zero), 
    // after approximately 70 minutes.
    if ( timeElapsed > 3 ) {
      const double distance  = 0.0
                             + RS::xStepCount * speedController.X.gearing
                             + RS::yStepCount * speedController.Y.gearing
                             + RS::zStepCount * speedController.Z.gearing;
  
      if ( distance > 0.0 )
        speedController.setRealtimeFeedSpeed_MM_SEC((1000.0 * 1000.0 * distance) / ( timeElapsed + SPEED_MANAGER_CONST_STATIC_OFFSET_US ));
    }
  }

  if ( isProbeMode() == OFF )
    speedController.completeMove();

*/  

  return ret;
}

/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::moveUntilSignal(int32_t dx, int32_t dy, int32_t dz) {
/////////////////////////////////////////////////////////////////////////////////////
  #warning impl. missing
  return RET_ERROR;
/*  
  // speed setup
  const double START_SPEED = speedController.getStartSpeed_MM_MIN() * 0.5;
  const double MAX_SPEED   = speedController.getMaxFeedSpeed_MM_MIN();
  const double DIFF_SPEED  = MAX_SPEED - START_SPEED;

  if ( DIFF_SPEED < 0.0 )
    return RET_ERROR;
    
  setSpeedValue(START_SPEED);
  
  unsigned char ret = RET_OK;
  unsigned long tsStart = millis();

  speedController.enableAccelerationXYZ(false);
  
  if ( dx != 0 || dy != 0 || dz !=0 ) {
    while ( (ret = renderAndStepAxisXYZ(dx, dy, dz)) == RET_OK ) {
  
      // mormally this loop will be broken by signals
      // like SIG_QUIT, or SIG_HALT and renderAndStepAxisXYZ()
      // return != RET_OK

      unsigned int diff = millis() - tsStart;
      if ( diff > moveUntilAccelPeriod ) {
        setSpeedValue(MAX_SPEED, false);
        
      } else {
        setSpeedValue(START_SPEED + DIFF_SPEED / moveUntilAccelPeriod * diff, false);
        
      }
    }
  }

  speedController.enableAccelerationXYZ(true);
  return ret;
  */
}
