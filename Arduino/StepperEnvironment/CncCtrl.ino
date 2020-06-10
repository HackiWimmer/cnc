#include <SoftwareSerial.h>
#include "ArdoVal.h"
#include "ArdoObj.h"
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
namespace CtrlParameter {
  
  template <class T>
  void print(unsigned char pid, T value, int8_t indent=1) {
    for (auto i=0; i<indent; i++) Serial.print(BLANK1); \
    Serial.print(pid);   Serial.print(TEXT_SEPARATOR); \
    Serial.print(value); Serial.write(TEXT_CLOSE);
  }
};

/////////////////////////////////////////////////////////////////////////////////////
namespace CtrlStatistics {
  // The following statistic parameteres are moved from the CncArduinoController and 
  // CncArduinoStepper classe to this global namespace to save sketch space and move it 
  // to the global variable memory

  int32_t positionCounter            = 0L;
  uint8_t positionCounterOverflow    = 0L; 

  int32_t stepCounter[3]             = { 0L, 0L, 0L };
  uint8_t stepCounterOverflow[3]     = { 0L, 0L, 0L };

  void resetPositionCounter()       { positionCounter  = 0L; positionCounterOverflow  = 0; }
  void resetStepCounter(AxisId aid) { stepCounter[aid] = 0L; stepCounterOverflow[aid] = 0; }
  
  void incPositionCounter()         { if ( positionCounter  == MAX_LONG ) { positionCounter  = 0L; positionCounterOverflow++;  } positionCounter++;  }    
  void incStepCounter(AxisId aid)   { if ( stepCounter[aid] == MAX_LONG ) { stepCounter[aid] = 0L; stepCounterOverflow[aid]++; } stepCounter[aid]++; }
};

/////////////////////////////////////////////////////////////////////////////////////
namespace CtrlSpeedValues {

  int16_t   tact           = 0;
  float     maxF_1DX_MMSec   = 0.0;
  float     maxF_1DY_MMSec   = 0.0; 
  float     maxF_1DZ_MMSec   = 0.0;
  float     maxF_2DXY_MMSec  = 0.0;
  float     maxF_3DXYZ_MMSec = 0.0;

  float     cfgF_MMSec       = 0.0;
  float     cmsF_MMSec       = 0.0;
  float     tmsF_MMSec       = 0.0;

  float getMaxF_1DX_MMSec()   { return maxF_1DX_MMSec;   }
  float getMaxF_1DY_MMSec()   { return maxF_1DY_MMSec;   }
  float getMaxF_1DZ_MMSec()   { return maxF_1DZ_MMSec;   }
  float getMaxF_2DXY_MMSec()  { return maxF_2DXY_MMSec;  }
  float getMaxF_3DXYZ_MMSec() { return maxF_3DXYZ_MMSec; }

  void setupTact(int16_t pwX, int16_t pwY, int16_t pwZ) {
    tact  = ArdoObj::maximum(pwZ, ArdoObj::maximum(pwX, pwY));
    tact += CtrlSpeedValues::tact * 0.02;
  }

  void setupMaxSpeed(float frX, float frY, float frZ) {
    maxF_1DX_MMSec   = tact ? frX * 1000.0 * 1000.0 / tact                                             : 0;
    maxF_1DY_MMSec   = tact ? frY * 1000.0 * 1000.0 / tact                                             : 0;
    maxF_1DZ_MMSec   = tact ? frZ * 1000.0 * 1000.0 / tact                                             : 0;
    maxF_2DXY_MMSec  = tact ? sqrt( pow(frX, 2) + pow(frY ,2) ) * 1000.0 * 1000.0 / tact               : 0;
    maxF_3DXYZ_MMSec = tact ? sqrt( pow(frX, 2) + pow(frY ,2) + pow(frZ, 2) ) * 1000.0 * 1000.0 / tact : 0;
  }

};

/////////////////////////////////////////////////////////////////////////////////////
CncArduinoController::CncArduinoController()
/////////////////////////////////////////////////////////////////////////////////////
: ArduinoCmdDecoderGetter       () 
, ArduinoCmdDecoderSetter       ()
, ArduinoCmdDecoderMove         () 
, ArduinoCmdDecoderMoveSequence ()
, ArduinoPositionRenderer       ()
, ArduinoAccelManager           ()
, X                             ( new CncAxisX(StepperSetup( this, PIN_X_STP, PIN_X_DIR, PIN_X_LIMIT )) )
, Y                             ( new CncAxisY(StepperSetup( this, PIN_Y_STP, PIN_Y_DIR, PIN_Y_LIMIT )) )
, Z                             ( new CncAxisZ(StepperSetup( this, PIN_Z_STP, PIN_Z_DIR, PIN_Z_LIMIT )) )
, impulseCalculator             ()
, lastI2CData                   ()
, posReplyState                 (OFF)
, probeMode                     (OFF)
, pause                         (OFF)
, I2CAvailable                  (false)
, posReplyCounter               (0)
, posReplyThreshold             (100)
, tsMoveStart                   (0L)
, tsMoveLast                    (0L)
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
void CncArduinoController::printConfig() {
/////////////////////////////////////////////////////////////////////////////////////
  int limitState = -1, supportState = -1;
  if ( isI2CAvailable() == true ) {
    if ( ArduinoMainLoop::readI2CSlave(lastI2CData) ) {
      limitState   = (int)lastI2CData.limitState;
      supportState = (int)lastI2CData.supportState;
    }
  }

  Serial.print(PID_CONTROLLER); Serial.print(TEXT_SEPARATOR); Serial.write(TEXT_CLOSE);


    CtrlParameter::print(PID_POS_REPLY_THRESHOLD,              getPosReplyThreshold());
    CtrlParameter::print(PID_PROBE_MODE,                       isProbeMode());
    CtrlParameter::print(PID_ENABLE_STEPPERS,                  AE::digitalRead(PIN_STEPPER_ENABLE) == ENABLE_STATE_ON);
    CtrlParameter::print(PID_TOOL_SWITCH,                      AE::digitalRead(PIN_TOOL_ENABLE)    == TOOL_STATE_ON);
    CtrlParameter::print(PID_I2C_AVAILABEL,                    isI2CAvailable());
    CtrlParameter::print(PID_GET_POS_COUNTER,                  CtrlStatistics::positionCounter);
    CtrlParameter::print(PID_GET_STEP_COUNTER_X,               CtrlStatistics::stepCounter[IDX_X]);
    CtrlParameter::print(PID_GET_STEP_COUNTER_Y,               CtrlStatistics::stepCounter[IDX_Y]);
    CtrlParameter::print(PID_GET_STEP_COUNTER_Z,               CtrlStatistics::stepCounter[IDX_Z]);
    CtrlParameter::print(PID_I2C_LIMIT_VALUE,                  limitState);
    CtrlParameter::print(PID_I2C_SUPPORT_VALUE,                supportState);

    X->printConfig();
    Y->printConfig();
    Z->printConfig();
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

    posReplyCounter     = 0;
    posReplyThreshold   = 100;

    posReplyState       = false;
    probeMode           = false;
    
    X->reset();
    Y->reset();
    Z->reset();
    
    X->resetPosition();
    Y->resetPosition();
    Z->resetPosition();

    CtrlStatistics::resetPositionCounter();
    CtrlStatistics::resetStepCounter(IDX_X);
    CtrlStatistics::resetStepCounter(IDX_Y);
    CtrlStatistics::resetStepCounter(IDX_Z);
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::turnOff() {
/////////////////////////////////////////////////////////////////////////////////////
  switchToolState (TOOL_STATE_OFF, FORCE);
  enableStepperPin(false);
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::isReadyToRun() {
/////////////////////////////////////////////////////////////////////////////////////
  CNC_CONTROLLER_LOG_FUNCTION();
  bool ret = true;

  if ( X->isReadyToRun() == false ) { 
    LastErrorCodes::clear(); 
    LastErrorCodes::messageText = "X"; 
    ArduinoMainLoop::pushMessage(MT_ERROR, E_STEPPER_NOT_READY_TO_RUN);    
    ret = false; 
  }
  
  if ( Y->isReadyToRun() == false ){ 
    LastErrorCodes::clear(); 
    LastErrorCodes::messageText = "Y"; 
    ArduinoMainLoop::pushMessage(MT_ERROR, E_STEPPER_NOT_READY_TO_RUN);    
    ret = false; 
  }
  
  if ( Z->isReadyToRun() == false ) { 
    LastErrorCodes::clear(); 
    LastErrorCodes::messageText = "Z"; 
    ArduinoMainLoop::pushMessage(MT_ERROR, E_STEPPER_NOT_READY_TO_RUN);    
    ret = false; 
  }
  
  return ret;
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::broadcastInterrupt() {
/////////////////////////////////////////////////////////////////////////////////////
  // Turn off ...
  switchToolState (TOOL_STATE_OFF, FORCE);
  enableStepperPin(false);

  // Show Interrupt LED
  ArduinoMainLoop::switchOutputPinState(PIN_INTERRUPT_LED, ON);

  X->interrupt();
  Y->interrupt();
  Z->interrupt();
  
  ArduinoMainLoop::pushMessage(MT_ERROR, E_INTERRUPT);
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::sendCurrentLimitStates(bool force) {
/////////////////////////////////////////////////////////////////////////////////////
// This method evaluate the limit states by the current stepper states

  const int8_t x = X->getLimitState();
  const int8_t y = Y->getLimitState();
  const int8_t z = Z->getLimitState();

  // the states will be only sent if one of them is activ or they should be forced
  if ( x != LimitSwitch::LIMIT_UNSET || y != LimitSwitch::LIMIT_UNSET || z != LimitSwitch::LIMIT_UNSET || force == true )
    ArduinoMainLoop::writeLongValues(PID_LIMIT, x, y, z);
    
  return (x != LimitSwitch::LIMIT_UNSET && y != LimitSwitch::LIMIT_UNSET && z != LimitSwitch::LIMIT_UNSET );
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::processHeartbeat(byte /* pid */) {
/////////////////////////////////////////////////////////////////////////////////////
  unsigned char limitState    = 0;
  unsigned char supportState  = 0;

  if ( isI2CAvailable() == true ) {
    if ( ArduinoMainLoop::readI2CSlave(lastI2CData) ) {
      limitState   = lastI2CData.limitState; 
      supportState = lastI2CData.supportState;
    }
  } else {
    int8_t x = X->readLimitState(DIRECTION_UNKNOWN);
    int8_t y = Y->readLimitState(DIRECTION_UNKNOWN);
    int8_t z = Z->readLimitState(DIRECTION_UNKNOWN);
    CncInterface::ILS::States ls(x, y, z);   
    limitState = ls.getValue();
  }
  
  ArduinoMainLoop::sendHeartbeat(limitState, supportState);
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::setSpeedValue_MMMin(double f, bool activateAcceleration) { 
/////////////////////////////////////////////////////////////////////////////////////
  CtrlSpeedValues::cfgF_MMSec = f / 60; 
  activateAccel(activateAcceleration);
  
  ARDO_DEBUG_VALUE("F [mm/sec]", CtrlSpeedValues::cfgF_MMSec)
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::setupAccelProfile(const ArduinoCmdDecoderSetter::Result& st) {
/////////////////////////////////////////////////////////////////////////////////////
  typedef ArduinoAccelManager::Function Function;
  Function fA, fD;

  // FEEDRATE_X,Y,Z         = 3
  // PULSE_WIDTH_HIGH_X,Y,Z = 3
  // FA                     = 3
  // FD                     = 3
  if ( st.valueCount != 3 + 3 + 3 + 3 )
  {
    // Function the members Feedrate as well as HighPulseWidth leave unchanged
    // Function fA, fD was initilaizes with default values
    ArduinoMainLoop::pushMessage(MT_ERROR, E_INVALID_PARAM_STREAM_LEN);       
    
  } else {  

    X->setFeedrate(st.values[0].asFloat());
    Y->setFeedrate(st.values[1].asFloat());
    Z->setFeedrate(st.values[2].asFloat());

    X->setHighPulseWidth(round(st.values[3].asFloat()));
    Y->setHighPulseWidth(round(st.values[4].asFloat()));
    Z->setHighPulseWidth(round(st.values[5].asFloat()));

    fA.A = st.values[ 6].asFloat();
    fA.B = st.values[ 7].asFloat();
    fA.C = st.values[ 8].asFloat();

    fD.A = st.values[ 9].asFloat();
    fD.B = st.values[10].asFloat();
    fD.C = st.values[11].asFloat();
  }

  // Pepare some speed  values  
  CtrlSpeedValues::setupTact    (X->getHighPulseWidth(), Y->getHighPulseWidth(), Z->getHighPulseWidth());
  CtrlSpeedValues::setupMaxSpeed(X->getFeedrate(),       Y->getFeedrate(),       Z->getFeedrate());

  if ( true ) {
    ARDO_DEBUG_VALUE("AccelProfile::FeedrateX", X->getFeedrate())
    ARDO_DEBUG_VALUE("AccelProfile::FeedrateY", Y->getFeedrate())
    ARDO_DEBUG_VALUE("AccelProfile::FeedrateZ", Z->getFeedrate())
    
    ARDO_DEBUG_VALUE("AccelProfile::HighPulseWidthX", X->getHighPulseWidth())
    ARDO_DEBUG_VALUE("AccelProfile::HighPulseWidthY", Y->getHighPulseWidth())
    ARDO_DEBUG_VALUE("AccelProfile::HighPulseWidthZ", Z->getHighPulseWidth())

    ARDO_DEBUG_VALUE("AccelProfile::fA.A", fA.A)
    ARDO_DEBUG_VALUE("AccelProfile::fA.B", fA.B)
    ARDO_DEBUG_VALUE("AccelProfile::fA.C", fA.C)
    ARDO_DEBUG_VALUE("AccelProfile::fD.A", fD.A)
    ARDO_DEBUG_VALUE("AccelProfile::fD.B", fD.B)
    ARDO_DEBUG_VALUE("AccelProfile::fD.C", fD.C)
    
    ARDO_DEBUG_VALUE("CtrlSpeedValues::tact       [hz]",     CtrlSpeedValues::tact ? 1000.0 *1000.0 / CtrlSpeedValues::tact : 1)
    ARDO_DEBUG_VALUE("CtrlSpeedValues::maxF_1DX   [mm/min]", 60.0 * CtrlSpeedValues::maxF_1DX_MMSec)
    ARDO_DEBUG_VALUE("CtrlSpeedValues::maxF_1DY   [mm/min]", 60.0 * CtrlSpeedValues::maxF_1DY_MMSec)
    ARDO_DEBUG_VALUE("CtrlSpeedValues::maxF_1DZ   [mm/min]", 60.0 * CtrlSpeedValues::maxF_1DZ_MMSec)
    ARDO_DEBUG_VALUE("CtrlSpeedValues::maxF_2DXY  [mm/min]", 60.0 * CtrlSpeedValues::maxF_2DXY_MMSec)
    ARDO_DEBUG_VALUE("CtrlSpeedValues::maxF_3DXYZ [mm/min]", 60.0 * CtrlSpeedValues::maxF_3DXYZ_MMSec)
  }
  
  setupAccelManager(fA, fD);  
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::switchToolState(bool state, bool force) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( force == false ) {
    if ( isProbeMode() == ON ) {
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
  if ( isProbeMode() == OFF )   AE::digitalWrite(PIN_STEPPER_ENABLE, state == true ? ENABLE_STATE_ON : ENABLE_STATE_OFF);
  else                          AE::digitalWrite(PIN_STEPPER_ENABLE, ENABLE_STATE_OFF);

  AE::delayMicroseconds(10);
  return state;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::isAnyLimitActive() {
/////////////////////////////////////////////////////////////////////////////////////
  static int8_t xLimit, yLimit, zLimit;
  return evaluateLimitStates(xLimit, yLimit, zLimit);
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::evaluateLimitStates(int8_t& xLimit, int8_t& yLimit, int8_t& zLimit) {
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
       ArduinoMainLoop::pushMessage(MT_ERROR, E_STEPPER_NOT_ENABLED); 
       return false;
    }
  }
  
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::observeToolState() {
/////////////////////////////////////////////////////////////////////////////////////
  if ( isProbeMode() == OFF ) {

    #warning impl. missing observeToolState()
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
byte CncArduinoController::acceptGetter() {
/////////////////////////////////////////////////////////////////////////////////////
  return decodeGetter();  
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::acceptSetter() {
/////////////////////////////////////////////////////////////////////////////////////
  return decodeSetter();    
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::acceptMove(byte cmd) {
/////////////////////////////////////////////////////////////////////////////////////
  setPosReplyState(true);
  const byte ret = decodeMove(cmd);

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
byte CncArduinoController::acceptMoveSequence(byte cmd) {
/////////////////////////////////////////////////////////////////////////////////////
  setPosReplyState(true);
  
  const byte ret = decodeMoveSequence(cmd);
  
  setPosReplyState(false);
  return ret;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::processSignal(byte sig) { byte b; return processSignal(sig, b); }
bool CncArduinoController::processSignal(byte sig, byte& retValue) {
/////////////////////////////////////////////////////////////////////////////////////
  switch ( sig ) {
    case SIG_SOFTWARE_RESET:
                #ifdef SKETCH_COMPILE
                  //softwareReset();   
                #endif  
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

  if ( posReplyCounter >= posReplyThreshold || force == true ) 
  {
    posReplyCounter = 0;

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
                                      CtrlSpeedValues::cmsF_MMSec * 60 * FLT_FACT);
                      break;

      default:        ; // do nothing

    }

    if ( false ) {
      ARDO_DEBUG_VALUE("F'", CtrlSpeedValues::cmsF_MMSec)
    }
    
    //Serial.flush();
  }
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::checkRuntimeEnv() {
/////////////////////////////////////////////////////////////////////////////////////
  // TODO: speed up the performance and observate in intervalls
  // blind flying: e.g. 16 * 0,015 (12 mm / 800 steps) = 0,24 mm
  if ( CtrlStatistics::positionCounter%1 == 0 ) {
    
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
void CncArduinoController::notifyMovePart(int8_t dx, int8_t dy, int8_t dz) {
/////////////////////////////////////////////////////////////////////////////////////
  // speed management
  if ( CtrlSpeedValues::cfgF_MMSec > 0 ) {
    
    // determine the current distance for each axis, 
    // due to the later pow() abs isn't necessary here 
    const float curDistX_MM = dx             * X->getFeedrate();
    const float curDistY_MM = dy             * Y->getFeedrate();
    const float curDistZ_MM = dz             * Z->getFeedrate();
    const float curDistV_MM = sqrt(pow(curDistX_MM, 2) + pow(curDistY_MM, 2) + pow(curDistZ_MM, 2));

    // determine the time deviation between the measured and configured sight
    const int32_t curTimeElapsed_US    = AE::micros() - tsMoveLast;
    const int32_t curTimeDistance_US   = round(( 1000.0 * 1000.0 / getNextTargetSpeed_MMSec() ) * curDistV_MM);
    const int32_t curTimeDifference_US = curTimeDistance_US - curTimeElapsed_US;

    if ( curTimeDifference_US > 0 )
      AE::delayMicroseconds(curTimeDifference_US);

    // measure the current speed again
    CtrlSpeedValues::cmsF_MMSec = ( 1000.0 * 1000.0 / (AE::micros() - tsMoveLast) ) * curDistV_MM;
    tsMoveLast = AE::micros();

  #warning
    if ( false ) {
      // determine the total distance for each axis, 
      // due to the later pow() abs isn't necessary here 
      const float totDistX_MM = RS::xStepCount * X->getFeedrate();
      const float totDistY_MM = RS::yStepCount * Y->getFeedrate();
      const float totDistZ_MM = RS::zStepCount * Z->getFeedrate();
      const float totDistV_MM = sqrt(pow(totDistX_MM, 2) + pow(totDistY_MM, 2) + pow(totDistZ_MM, 2));
  
      const int32_t totTimeElapsed_US    = AE::micros() - tsMoveStart;
      const int32_t totTimeDistance_US   = CtrlSpeedValues::cfgF_MMSec ? round(( 1000.0 * 1000.0 / CtrlSpeedValues::cfgF_MMSec ) * totDistV_MM) : 0;
      const int32_t totTimeDifference_US = totTimeDistance_US - totTimeElapsed_US;

      CtrlSpeedValues::tmsF_MMSec = ( 1000.0 * 1000.0 / totTimeDifference_US ) * totDistV_MM;
    }
  }  
  
  // position management
  posReplyCounter++;
  CtrlStatistics::incPositionCounter();
  sendCurrentPositions(PID_XYZ_POS_DETAIL, false);  
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::notifyACMStateChange(State state) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( true ) {
    switch ( state ) {
      case P_UNDEF:   ARDO_DEBUG_MESSAGE('D', "CncArduinoController::notifyACMStateChange: to 'P_UNDEF'");   break;
      case P_CONST:   ARDO_DEBUG_MESSAGE('D', "CncArduinoController::notifyACMStateChange: to 'P_CONST'");   break;
      case P_ACCEL:   ARDO_DEBUG_MESSAGE('D', "CncArduinoController::notifyACMStateChange: to 'P_ACCEL'");   break;
      case P_TARGET:  ARDO_DEBUG_MESSAGE('D', "CncArduinoController::notifyACMStateChange: to 'P_TARGET'");  break;
      case P_DEACCEL: ARDO_DEBUG_MESSAGE('D', "CncArduinoController::notifyACMStateChange: to 'P_DEACCEL'"); break;
    }
  }
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::notifyACMInitMove() {
/////////////////////////////////////////////////////////////////////////////////////
}  
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::setDirection(AxisId aid, int32_t steps) { 
  switch ( aid ) {
    case IDX_X: return X->setDirection(steps);
    case IDX_Y: return Y->setDirection(steps);
    case IDX_Z: return Z->setDirection(steps);
  }

  return RET_ERROR;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::performStep(AxisId aid) {
/////////////////////////////////////////////////////////////////////////////////////
  switch ( aid ) {
    case IDX_X: return X->performStep();
    case IDX_Y: return Y->performStep();
    case IDX_Z: return Z->performStep();
  }

  return RET_ERROR;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::initiateStep(AxisId aid) {
/////////////////////////////////////////////////////////////////////////////////////
  switch ( aid ) {
    case IDX_X: return X->initiateStep();
    case IDX_Y: return Y->initiateStep();
    case IDX_Z: return Z->initiateStep();
  }

  return RET_ERROR;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::finalizeStep(AxisId aid) {
/////////////////////////////////////////////////////////////////////////////////////
  switch ( aid ) {
    case IDX_X: return X->finalizeStep();
    case IDX_Y: return Y->finalizeStep();
    case IDX_Z: return Z->finalizeStep();
  }

  return RET_ERROR;
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
    int8_t x = LimitSwitch::LIMIT_UNKNOWN;
    int8_t y = LimitSwitch::LIMIT_UNKNOWN;
    int8_t z = LimitSwitch::LIMIT_UNKNOWN;

    evaluateLimitStates(x, y, z);
    writeGetterValue3(PID_LIMIT, x, y, z);
  };

  //---------------------------------------------------------------------------------
  switch ( gt.pid ) {
    
    case PID_QUERY_READY_TO_RUN:      writeGetterValue1(PID_QUERY_READY_TO_RUN,    (int32_t)isReadyToRun()); break;
    
    case PID_X_POS:                   writeGetterValue1(PID_X_POS,                 X->getPosition()); break;
    case PID_Y_POS:                   writeGetterValue1(PID_Y_POS,                 Y->getPosition()); break;
    case PID_Z_POS:                   writeGetterValue1(PID_Z_POS,                 Z->getPosition()); break;

    case PID_XY_POS:                  writeGetterValue2(PID_XY_POS,                X->getPosition(), Y->getPosition()); break;
    case PID_XYZ_POS:                 writeGetterValue3(PID_XYZ_POS,               X->getPosition(), Y->getPosition(), Z->getPosition()); break;

    case PID_GET_POS_COUNTER:         writeGetterValue2(PID_GET_POS_COUNTER,       CtrlStatistics::positionCounter,    (int32_t)CtrlStatistics::positionCounterOverflow); break;
    case PID_GET_STEP_COUNTER_X:      writeGetterValue2(PID_GET_STEP_COUNTER_X,    CtrlStatistics::stepCounter[IDX_X], (int32_t)CtrlStatistics::stepCounterOverflow[IDX_X]); break;
    case PID_GET_STEP_COUNTER_Y:      writeGetterValue2(PID_GET_STEP_COUNTER_Y,    CtrlStatistics::stepCounter[IDX_Y], (int32_t)CtrlStatistics::stepCounterOverflow[IDX_Y]); break;
    case PID_GET_STEP_COUNTER_Z:      writeGetterValue2(PID_GET_STEP_COUNTER_Z,    CtrlStatistics::stepCounter[IDX_Z], (int32_t)CtrlStatistics::stepCounterOverflow[IDX_Z]); break; 

    case PID_LIMIT:                   writeLimitGetter(); break;
    
    default:                          writeGetterValue1(PID_UNKNOWN, 0);
                                      
                                      LastErrorCodes::clear();
                                      LastErrorCodes::register1Byte_A = gt.pid;
                                      AML::pushMessage(MT_ERROR, E_INVALID_GETTER_ID);
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
      if ( st.values[i].isFloat )  { values.append(wxString::Format("[%d]=%f",  i, st.values[i].asFloat())); } 
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

    case PID_RESERT_STEP_COUNTER:     CtrlStatistics::resetStepCounter(IDX_X); CtrlStatistics::resetStepCounter(IDX_Y); CtrlStatistics::resetStepCounter(IDX_Z); break;
  
    case PID_INC_DIRECTION_VALUE_X:   X->setIncrementDirectionValue(st.values[0].l);  break;
    case PID_INC_DIRECTION_VALUE_Y:   Y->setIncrementDirectionValue(st.values[0].l);  break;
    case PID_INC_DIRECTION_VALUE_Z:   Z->setIncrementDirectionValue(st.values[0].l);  break;
    
    case PID_POS_REPLY_THRESHOLD:     setPosReplyThreshold(st.values[0].l);           break;
    
    case PID_SPEED_MM_MIN:            setSpeedValue_MMMin(st.values[0].asFloat());    break;

    case PID_ENABLE_STEPPERS:         enableStepperPin(st.values[0].asBool());        break;
    case PID_RESERT_POS_COUNTER:      CtrlStatistics::resetPositionCounter();         break;

    case PID_PROBE_MODE:              setProbeMode(st.values[0].asBool());            break;
    case PID_TOOL_SWITCH:             switchToolState(st.values[0].asBool());         break;
    
    case PID_ACCEL_PROFILE:           setupAccelProfile(st); 
                                      break;

    default:                          LastErrorCodes::clear();
                                      LastErrorCodes::register1Byte_A = st.pid;
                                      AML::pushMessage(MT_ERROR, E_INVALID_PARAM_ID);
                                      return RET_ERROR;
  }
  
  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::process(const ArduinoCmdDecoderMoveSequence::Result& seq) {
/////////////////////////////////////////////////////////////////////////////////////
  switch ( seq.cmd ) {
    case CMD_MOVE_SEQUENCE:  return directMove(seq.dx, seq.dy, seq.dz);
    default:                 return renderMove(seq.dx, seq.dy, seq.dz);
  }
  
  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::initialize(const ArduinoCmdDecoderMoveSequence::Result& seq) {
/////////////////////////////////////////////////////////////////////////////////////  
  ARDO_DEBUG_MESSAGE('S', "MoveSequence started")
  
  tsMoveStart = AE::micros();
  tsMoveLast  = tsMoveStart;

  const int32_t ic = seq.impulseCount;// impulseCalculator.calculate(seq.lengthX, seq.lengthY, seq.lengthZ);
  if ( ic < 0 )
    return RET_ERROR;
   
  if ( CtrlSpeedValues::cfgF_MMSec && initMove(ic, CtrlSpeedValues::cfgF_MMSec) == false )
    return RET_ERROR;
 
  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::finalize(const ArduinoCmdDecoderMoveSequence::Result&) {
/////////////////////////////////////////////////////////////////////////////////////  
  sendCurrentPositions(PID_XYZ_POS_MAJOR, true);
  ARDO_DEBUG_MESSAGE('S', "MoveSequence finished")

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
byte CncArduinoController::movePosition(int32_t dx, int32_t dy, int32_t dz) {
/////////////////////////////////////////////////////////////////////////////////////
  // avoid empty processing
  if ( dx == 0 && dy == 0 && dz == 0 ) 
    return RET_OK;

  const int32_t ic = impulseCalculator.calculate(dx, dy, dz);
  if ( ic < 0 )
    return RET_ERROR;

  tsMoveStart = AE::micros();
  tsMoveLast  = tsMoveStart;
  
  if ( CtrlSpeedValues::cfgF_MMSec && initMove(ic, CtrlSpeedValues::cfgF_MMSec) == false )
    return RET_ERROR;
  
  return renderMove(dx, dy, dz);
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::moveUntilSignal(int32_t dx, int32_t dy, int32_t dz) {
/////////////////////////////////////////////////////////////////////////////////////
  // speed setup
  const float MAX_SPEED   = CtrlSpeedValues::maxF_1DZ_MMSec * 60;
  const float START_SPEED = getStartSpeed_MMSec() * 60 * 0.5;
  const float DIFF_SPEED  = MAX_SPEED - START_SPEED;

  if ( DIFF_SPEED < 0.0 )
    return RET_ERROR;
    
  setSpeedValue_MMMin(START_SPEED);
  
  unsigned char ret = RET_OK;
  unsigned long tsStart = AE::millis();

  activateAccel(false);
  
  if ( dx != 0 || dy != 0 || dz !=0 ) {
    while ( (ret = movePosition(dx, dy, dz)) == RET_OK ) {
  
      // mormally this loop will be broken by signals
      // like SIG_QUIT, or SIG_HALT and renderAndStepAxisXYZ()
      // return != RET_OK

      unsigned int diff = AE::millis() - tsStart;
      if ( diff > moveUntilAccelPeriod ) {
        setSpeedValue_MMMin(MAX_SPEED, false);
        
      } else {
        setSpeedValue_MMMin(START_SPEED + DIFF_SPEED / moveUntilAccelPeriod * diff, false);
        
      }
    }
  }

  activateAccel(true);
  return ret;
}
