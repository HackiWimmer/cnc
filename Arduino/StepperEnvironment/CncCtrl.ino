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
  // to the global memory
  
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

  int16_t   tact             = 0;

  float     cfgF_MMSec       = 0.0;
  float     cmsF_MMSec       = 0.0;
  float     tmsF_MMSec       = 0.0;

  int16_t   distXYZ_NM       = 0;
  int16_t   distXY_NM        = 0;
  int16_t   distYZ_NM        = 0;
  int16_t   distXZ_NM        = 0;
  int16_t   distX_NM         = 0;
  int16_t   distY_NM         = 0;
  int16_t   distZ_NM         = 0;

  #ifndef SKETCH_COMPILE

    float     maxF_1DX_MMSec   = 0.0;
    float     maxF_1DY_MMSec   = 0.0; 
    float     maxF_1DZ_MMSec   = 0.0;
    float     maxF_2DXY_MMSec  = 0.0;
    float     maxF_3DXYZ_MMSec = 0.0;

    float getMaxF_1DX_MMSec()   { return maxF_1DX_MMSec;   }
    float getMaxF_1DY_MMSec()   { return maxF_1DY_MMSec;   }
    float getMaxF_1DZ_MMSec()   { return maxF_1DZ_MMSec;   }
    float getMaxF_2DXY_MMSec()  { return maxF_2DXY_MMSec;  }
    float getMaxF_3DXYZ_MMSec() { return maxF_3DXYZ_MMSec; }
    
  #endif

  void setupTact(int16_t pwX, int16_t pwY, int16_t pwZ) {
    tact  = ArdoObj::maximum(pwZ, ArdoObj::maximum(pwX, pwY));
    tact -= CtrlSpeedValues::tact * 0.2;
  }

  void setupMaxSpeed(float frX, float frY, float frZ) {
    
    #ifndef SKETCH_COMPILE
    
      maxF_1DX_MMSec   = tact ? frX * 1000.0 * 1000.0 / tact                                             : 0;
      maxF_1DY_MMSec   = tact ? frY * 1000.0 * 1000.0 / tact                                             : 0;
      maxF_1DZ_MMSec   = tact ? frZ * 1000.0 * 1000.0 / tact                                             : 0;
      maxF_2DXY_MMSec  = tact ? sqrt( pow(frX, 2) + pow(frY ,2) ) * 1000.0 * 1000.0 / tact               : 0;
      maxF_3DXYZ_MMSec = tact ? sqrt( pow(frX, 2) + pow(frY ,2) + pow(frZ, 2) ) * 1000.0 * 1000.0 / tact : 0;
      
    #endif

    // fw is unit mm  
    const float cdx_MM  = pow(frX, 2);
    const float cdy_MM  = pow(frY, 2);
    const float cdz_MM  = pow(frZ, 2);
    
    const uint32_t factor = 1000000; // 1000 * 1000
    distXYZ_NM       = factor * sqrt(cdx_MM + cdy_MM + cdz_MM);
    distXY_NM        = factor * sqrt(cdx_MM + cdy_MM         );
    distYZ_NM        = factor * sqrt(         cdy_MM + cdz_MM);
    distXZ_NM        = factor * sqrt(cdx_MM +          cdz_MM);
    distX_NM         = factor * sqrt(cdx_MM                  );
    distY_NM         = factor * sqrt(      cdy_MM            );
    distZ_NM         = factor * sqrt(            cdz_MM      );

    if ( true ) {
      ARDO_DEBUG_VALUE("CtrlSpeedValues::distXYZ [nm]", distXYZ_NM)
      ARDO_DEBUG_VALUE("CtrlSpeedValues::distXY  [nm]", distXY_NM )
      ARDO_DEBUG_VALUE("CtrlSpeedValues::distYZ  [nm]", distYZ_NM )
      ARDO_DEBUG_VALUE("CtrlSpeedValues::distXZ  [nm]", distXZ_NM )
      ARDO_DEBUG_VALUE("CtrlSpeedValues::distX   [nm]", distX_NM  )
      ARDO_DEBUG_VALUE("CtrlSpeedValues::distY   [nm]", distY_NM  )
      ARDO_DEBUG_VALUE("CtrlSpeedValues::distZ   [nm]", distZ_NM  )
    }  
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
, X                             ( new CncAxisX(StepperSetup( this, PIN_X_STP, PIN_X_DIR, PIN_X_MIN_LIMIT, PIN_X_MAX_LIMIT )) )
, Y                             ( new CncAxisY(StepperSetup( this, PIN_Y_STP, PIN_Y_DIR, PIN_Y_MIN_LIMIT, PIN_X_MAX_LIMIT )) )
, Z                             ( new CncAxisZ(StepperSetup( this, PIN_Z_STP, PIN_Z_DIR, PIN_Z_MIN_LIMIT, PIN_X_MAX_LIMIT )) )
, testManager                   (NULL)
, impulseCalculator             ()
, lastI2CData                   ()
, posReplyState                 (OFF)
, probeMode                     (OFF)
, pause                         (OFF)
, I2CAvailable                  (false)
, interactiveMode               (false)
, interactiveMoveActive         (false)
, posReplyCounter               (0)
, posReplyThreshold             (100)
, tsMoveStart                   (0L)
, tsMoveLast                    (0L)
, interactiveValueX             (0)
, interactiveValueY             (0)
, interactiveValueZ             (0)
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

    CtrlParameter::print(PID_SETUP_ID,                         SETUP_ID);
    CtrlParameter::print(PID_POS_REPLY_THRESHOLD,              getPosReplyThreshold());
    CtrlParameter::print(PID_PROBE_MODE,                       isProbeMode());
    CtrlParameter::print(PID_ENABLE_STEPPERS,                  AE::digitalRead(PIN_ENABLE_STEPPER));
    CtrlParameter::print(PID_TOOL_SWITCH,                      AE::digitalRead(PIN_ENABLE_TOOL));
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

    interactiveMode     = false;
    interactiveValueX   = 0;
    interactiveValueY   = 0;
    interactiveValueZ   = 0;
    
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
  enableStepperPin(ENABLE_STATE_OFF);
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::isReadyToRun() {
/////////////////////////////////////////////////////////////////////////////////////
  CNC_CONTROLLER_LOG_FUNCTION();
  bool ret = true;

  if ( READ_EXT_INNTERRUPT_PIN == EXTERNAL_INTERRRUPT_ON ) {
    LastErrorCodes::clear(); 
    ArduinoMainLoop::pushMessage(MT_ERROR, E_EXTERNEL_INTERRUPT); 
    ret = false; 
  }

  if ( X->isReadyToRun() == false ) { 
    ArduinoMainLoop::pushMessage(MT_ERROR, E_STEPPER_NOT_READY_TO_RUN, "X");    
    ret = false; 
  }
  
  if ( Y->isReadyToRun() == false ){ 
    ArduinoMainLoop::pushMessage(MT_ERROR, E_STEPPER_NOT_READY_TO_RUN, "Y");    
    ret = false; 
  }
  
  if ( Z->isReadyToRun() == false ) { 
    ArduinoMainLoop::pushMessage(MT_ERROR, E_STEPPER_NOT_READY_TO_RUN, "Z");
    ret = false; 
  }
  
  return ret;
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::broadcastInterrupt() {
/////////////////////////////////////////////////////////////////////////////////////
  // Turn off ...
  switchToolState (TOOL_STATE_OFF, FORCE);
  enableStepperPin(ENABLE_STATE_OFF);

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
  const bool xMin = READ_LMT_PIN_X_MIN == LimitSwitch::LIMIT_SWITCH_ON;
  const bool xMax = READ_LMT_PIN_X_MAX == LimitSwitch::LIMIT_SWITCH_ON;
  const bool yMin = READ_LMT_PIN_Y_MIN == LimitSwitch::LIMIT_SWITCH_ON;
  const bool yMax = READ_LMT_PIN_Y_MAX == LimitSwitch::LIMIT_SWITCH_ON;
  const bool zMin = READ_LMT_PIN_Z_MIN == LimitSwitch::LIMIT_SWITCH_ON;
  const bool zMax = READ_LMT_PIN_Z_MAX == LimitSwitch::LIMIT_SWITCH_ON;

  CncInterface::ILS::States ls( xMin, xMax, yMin, yMax,zMin, zMax );   
  
  // the states will be only sent if one of them is activ or they should be forced
  if ( ls.hasLimits() || force )
    ArduinoMainLoop::writeLongValues(PID_LIMIT, ls.xLimit(), ls.yLimit(), ls.zLimit());
    
  return ls.hasLimits();
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::processHeartbeat() {
/////////////////////////////////////////////////////////////////////////////////////
  unsigned char limitState    = 0;
  unsigned char supportState  = 0;

  const bool xMin = READ_LMT_PIN_X_MIN == LimitSwitch::LIMIT_SWITCH_ON;
  const bool xMax = READ_LMT_PIN_X_MAX == LimitSwitch::LIMIT_SWITCH_ON;
  const bool yMin = READ_LMT_PIN_Y_MIN == LimitSwitch::LIMIT_SWITCH_ON;
  const bool yMax = READ_LMT_PIN_Y_MAX == LimitSwitch::LIMIT_SWITCH_ON;
  const bool zMin = READ_LMT_PIN_Z_MIN == LimitSwitch::LIMIT_SWITCH_ON;
  const bool zMax = READ_LMT_PIN_Z_MAX == LimitSwitch::LIMIT_SWITCH_ON;

  CncInterface::ILS::States ls( xMin, xMax, yMin, yMax,zMin, zMax );   
  limitState = ls.getValue();

  ArduinoMainLoop::sendHeartbeat(limitState, supportState);
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::evaluateLimitStates(int8_t& xLimit, int8_t& yLimit, int8_t& zLimit) {
/////////////////////////////////////////////////////////////////////////////////////
  const bool xMin = READ_LMT_PIN_X_MIN == LimitSwitch::LIMIT_SWITCH_ON;
  const bool xMax = READ_LMT_PIN_X_MAX == LimitSwitch::LIMIT_SWITCH_ON;
  const bool yMin = READ_LMT_PIN_Y_MIN == LimitSwitch::LIMIT_SWITCH_ON;
  const bool yMax = READ_LMT_PIN_Y_MAX == LimitSwitch::LIMIT_SWITCH_ON;
  const bool zMin = READ_LMT_PIN_Z_MIN == LimitSwitch::LIMIT_SWITCH_ON;
  const bool zMax = READ_LMT_PIN_Z_MAX == LimitSwitch::LIMIT_SWITCH_ON;

  CncInterface::ILS::States ls( xMin, xMax, yMin, yMax,zMin, zMax );   
  xLimit = ls.xLimit();
  yLimit = ls.yLimit();
  zLimit = ls.zLimit();
    
  return ls.hasLimits();
}
///////////////////////////////////////////////////////
byte CncArduinoController::performTest() {
///////////////////////////////////////////////////////
  #ifdef USE_CNC_TEST
    if ( testManager == NULL )
      testManager = new ArduinoTestManager(this);
      
    return testManager->performTest();
  #endif

  ArduinoMainLoop::pushMessage(MT_ERROR, E_UNKNOW_COMMAND, CMD_PERFORM_TEST);

  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::setSpeedValue_MMMin(double f, bool activateAcceleration) { 
/////////////////////////////////////////////////////////////////////////////////////
  CtrlSpeedValues::cfgF_MMSec = f / 60; 
  ArduinoAccelManager::activate(activateAcceleration);

  ARDO_DEBUG_VALUE("F [mm/sec]", CtrlSpeedValues::cfgF_MMSec)
  if ( activateAcceleration == false ) {
    ARDO_DEBUG_VALUE("ArduinoAccelManager", "Is deactivated!")
  }
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
  CtrlSpeedValues::setupTact    (MIN_DURATION_PER_STEP_US, MIN_DURATION_PER_STEP_US, MIN_DURATION_PER_STEP_US);
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
  
  ArduinoAccelManager::setup(fA, fD);  
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::switchToolState(bool state, bool force) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( force == false ) {
    // Don't enable the tool during a probe mode phase
    if ( isProbeMode() == ON ) {
      AE::digitalWrite(PIN_ENABLE_TOOL, TOOL_STATE_OFF);
      return;
    }
  }
  
  AE::digitalWrite(PIN_ENABLE_TOOL, state);

  // give the tool a portion of time to run properly
  if ( state == TOOL_STATE_ON )
    AE::delay(1000);
}
//////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::enableStepperPin(bool state){
//////////////////////////////////////////////////////////////////////////////
  if ( isProbeMode() == OFF )   AE::digitalWrite(PIN_ENABLE_STEPPER, state);
  else                          AE::digitalWrite(PIN_ENABLE_STEPPER, ENABLE_STATE_OFF);

  AE::delayMicroseconds(10);
  return state;
}
//////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::getEnableStepperPinState(){
//////////////////////////////////////////////////////////////////////////////
  return AE::digitalRead(PIN_ENABLE_STEPPER);
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::isAnyLimitActive() {
/////////////////////////////////////////////////////////////////////////////////////
  static int8_t xLimit, yLimit, zLimit;
  return evaluateLimitStates(xLimit, yLimit, zLimit);
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
void CncArduinoController::updateInteractiveMoveValues(int8_t dx, int8_t dy, int8_t dz) {
/////////////////////////////////////////////////////////////////////////////////////
  interactiveValueX = dx;
  interactiveValueY = dy;
  interactiveValueZ = dz;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::cancelInteractiveMove(byte) {
/////////////////////////////////////////////////////////////////////////////////////  
  CtrlSpeedValues::cmsF_MMSec = 0.1;
  ArduinoAccelManager::finalize();

  interactiveMoveActive = false;
  updateInteractiveMoveValues(0, 0, 0);
  
  return RET_OK;  
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::acceptInteractiveMove(byte) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( interactiveMode == true ) {
    ArduinoMainLoop::pushMessage(MT_ERROR, E_OTHER_MOVE_CMD_ACTIVE);
    return RET_ERROR;
  }
  
  // For intractive moves the impulse count at the move start isn't defined - by concept.
  // Therefore, here a default impulses count is used with a (very) high value. This is to
  // get a fully supported acceleration at the begining. The deacceleration phase isn't much
  // imported here, because we can loose steps here . . . 
  const uint32_t defaultImpulses = 100000;
  if ( CtrlSpeedValues::cfgF_MMSec && ArduinoAccelManager::initMove(defaultImpulses, CtrlSpeedValues::cfgF_MMSec) == false )
    return RET_ERROR;

  interactiveMode = true;
  updateInteractiveMoveValues(0, 0, 0);

  byte ret    = RET_OK;
  tsMoveStart = ArdoTs::now();
  tsMoveLast  = tsMoveStart;

  setPosReplyState(true);
  setPosReplyThreshold(posReplyThreshold);
  setProbeMode(OFF);
  enableStepperPin(ENABLE_STATE_ON);

  interactiveMoveActive = true;
  while ( interactiveMoveActive == true ) {
    ret = directMove(interactiveValueX, interactiveValueY, interactiveValueZ);
    
    if ( ret != RET_OK )
      break;
  }

  cancelInteractiveMove(0);
  sendCurrentPositions(PID_XYZ_POS_MAJOR, true);
  setPosReplyState(false);
  
  interactiveMode = false;
  return ret;  
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::acceptMove(byte cmd) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( interactiveMode == true ) {
    ArduinoMainLoop::pushMessage(MT_ERROR, E_OTHER_MOVE_CMD_ACTIVE);
    return RET_ERROR;
  }

  setPosReplyState(true);
  const byte ret = decodeMove(cmd);

  if ( ret == RET_QUIT ) {
    // 0.0 isn't valid here because it will be ignored
    CtrlSpeedValues::cmsF_MMSec = 0.1;
    ArduinoAccelManager::finalize();
  }
  
  sendCurrentPositions(PID_XYZ_POS_MAJOR, true);
  setPosReplyState(false);
  return ret;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::acceptMoveSequence(byte cmd) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( interactiveMode == true ) {
    ArduinoMainLoop::pushMessage(MT_ERROR, E_OTHER_MOVE_CMD_ACTIVE);
    return RET_ERROR;
  }

  setPosReplyState(true);
  const byte ret = decodeMoveSequence(cmd);
  
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
                // Options:
                //  - Returning false here signalize an error and the complete run cycle (PC) stopps as a result.
                //  - Returning true here stopps the current move (while loop), so far so good, but the current run cycle 
                //    continue with the next existing move command which is not the meaning of HALT.
                switchToolState(TOOL_STATE_OFF, FORCE);
                
                retValue = RET_HALT;
                return false;
                
    case SIG_QUIT_MOVE:
                cancelInteractiveMove(SIG_QUIT_MOVE);
                retValue = RET_QUIT;
                return false; 

    case SIG_UPDATE:
                // remove the signal from serial
                return processSignalUpdate(retValue);

    case SIG_PAUSE:
                pause = PAUSE_ACTIVE;
                
                // Don't return here - see the pause handling below
                break;

    case SIG_RESUME:
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
        processHeartbeat();
        
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
bool CncArduinoController::processSignalUpdate(byte& retValue) {
/////////////////////////////////////////////////////////////////////////////////////
  //Mode m = ArduinoPositionRenderer::getMode();

  byte b[1];
  
  // read length
  if ( !ArduinoMainLoop::readSerialBytesWithTimeout(b, 1) )
    return false;

  const int8_t cmdLen = (int8_t)b[0];
    
  // read PID
  if ( !ArduinoMainLoop::readSerialBytesWithTimeout(b, 1) )
    b[0] = 0;

  bool ret = false;
  switch ( b[0] ) {
    
    case PID_XYZ_INTERACTIVE_POS: {
      int32_t dx; if ( ArduinoMainLoop::readInt8(dx) == false )    dx = 0;
      int32_t dy; if ( ArduinoMainLoop::readInt8(dy) == false )    dy = 0;
      int32_t dz; if ( ArduinoMainLoop::readInt8(dz) == false )    dz = 0;

      updateInteractiveMoveValues((int8_t)dx, (int8_t)dy, (int8_t)dz);
      
      ret       = true;
      retValue  = RET_OK;
      
      break;
    }
    
    default: {
      
      // remove unknown update
      for( int i = 0; i < ArdoObj::minimum(Serial.available(), (int)cmdLen); i++)
        Serial.read();
        
      ret       = false;
      retValue  = RET_ERROR;
    }
  }

  return ret;
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
  
  //--------------------------------------------------------------------------------
  auto observeExternalInterruptPin = [&]() {
      if ( READ_EXT_INNTERRUPT_PIN == EXTERNAL_INTERRRUPT_ON ) {
         ArduinoMainLoop::pushMessage(MT_ERROR, E_EXTERNEL_INTERRUPT); 
         return false;
    }
    return true;
  };
  
  byte retValue = RET_ERROR;
  if ( observeSerialFrontByte(retValue) == false )
    return retValue;

  if ( observeExternalInterruptPin() == false )
    return RET_ERROR;
    
  if ( isProbeMode() == OFF ) {
    
    if ( READ_EXT_INNTERRUPT_PIN == EXTERNAL_INTERRRUPT_ON ) {
      ArduinoMainLoop::pushMessage(MT_ERROR, E_EXTERNEL_INTERRUPT); 
      return RET_ERROR;
    }
    
    if ( PIN_IS_TOOL_POWERED == TOOL_STATE_OFF ) {
      ArduinoMainLoop::pushMessage(MT_ERROR, E_TOOL_NOT_ENALED); 
      return RET_ERROR;
    }
  }
  
  return RET_OK;   
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::notifyMovePart(int8_t dx, int8_t dy, int8_t dz) {
/////////////////////////////////////////////////////////////////////////////////////
  // position management
  posReplyCounter++;
  CtrlStatistics::incPositionCounter();
  sendCurrentPositions(PID_XYZ_POS_DETAIL, false);  

  // speed management
  if ( CtrlSpeedValues::cfgF_MMSec > 0.0 ) {
    
    // determine the current distance for each axis, 
    int32_t curDistV_NM = 0;
   
    if      ( dx != 0 && dy != 0 && dz != 0 )   { curDistV_NM = CtrlSpeedValues::distXYZ_NM; }
    else if ( dx != 0 && dy != 0 )              { curDistV_NM = CtrlSpeedValues::distXY_NM;  }
    else if ( dx != 0 && dz != 0 )              { curDistV_NM = CtrlSpeedValues::distXZ_NM;  }
    else if ( dy != 0 && dz != 0 )              { curDistV_NM = CtrlSpeedValues::distYZ_NM;  }
    else if ( dx != 0 )                         { curDistV_NM = CtrlSpeedValues::distX_NM;   }
    else if ( dy != 0 )                         { curDistV_NM = CtrlSpeedValues::distY_NM;   }
    else if ( dz != 0 )                         { curDistV_NM = CtrlSpeedValues::distZ_NM;   }

    // determine the time deviation between the measured and configured sight
    const int32_t currentTargetSpeed_MMSec = ArdoObj::maximum((int32_t)1, (int32_t)getCurrentTargetSpeed_MMSec());
    const int32_t currentTimeDistance_US   = curDistV_NM / currentTargetSpeed_MMSec; // [nm/mm/s] -> [10^9/10^3 s] --> [us] 

    // don't put anything between the lines of the section below
    {
      // Here starts the potential delay phase, first use this phase to evaluate 
      // the next target speed (de/accelleration) to safe extern time therefore
      initNextTargetSpeed_MMSec();
      
      const bool asFastAsCan = false;
      if ( asFastAsCan == true ) {
        
        // test condition only . . . 
        // only measure the current speed 
        CtrlSpeedValues::cmsF_MMSec = curDistV_NM / ArdoTs::timespan(tsMoveLast); // [nm/us] -> [um/ms] -> [mm/s]
      } 
      else {

        // determine the rest delay and perform it
        const int32_t tsShould_US   = tsMoveLast + currentTimeDistance_US;
        const int32_t rest_US       = tsShould_US - ArdoTs::now();
        if ( rest_US > 3 ) {
          AE::delayMicroseconds(rest_US);

          // here we are in time it can be asumed the speed is as configured
          CtrlSpeedValues::cmsF_MMSec = currentTargetSpeed_MMSec;
        }
        else {
          // measure the current speed again
          CtrlSpeedValues::cmsF_MMSec = curDistV_NM / ArdoTs::timespan(tsMoveLast); // [nm/us] -> [um/ms] -> [mm/s]
        }
      }
    }

    // this has to be the last action within this function
    tsMoveLast = ArdoTs::now();
  }
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::notifyACMStateChange(State ) {
/////////////////////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::notifyACMInitMove() {
/////////////////////////////////////////////////////////////////////////////////////
}  
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::setHighPulseWidth(AxisId aid, int32_t width) {
/////////////////////////////////////////////////////////////////////////////////////  
  switch ( aid ) {
    case IDX_X: X->setHighPulseWidth(width); break;
    case IDX_Y: Y->setHighPulseWidth(width); break;
    case IDX_Z: Z->setHighPulseWidth(width); break;
  }

  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::setDirection(AxisId aid, int32_t steps) { 
/////////////////////////////////////////////////////////////////////////////////////
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
    
    case PID_QUERY_READY_TO_RUN:          writeGetterValue1(PID_QUERY_READY_TO_RUN,           (int32_t)isReadyToRun()); break;
    case PID_ENABLE_STEPPERS:             writeGetterValue1(PID_ENABLE_STEPPERS,              (int32_t)getEnableStepperPinState()); break;
    
    
    case PID_X_POS:                       writeGetterValue1(PID_X_POS,                        X->getPosition()); break;
    case PID_Y_POS:                       writeGetterValue1(PID_Y_POS,                        Y->getPosition()); break;
    case PID_Z_POS:                       writeGetterValue1(PID_Z_POS,                        Z->getPosition()); break;

    case PID_XY_POS:                      writeGetterValue2(PID_XY_POS,                       X->getPosition(), Y->getPosition()); break;
    case PID_XYZ_POS:                     writeGetterValue3(PID_XYZ_POS,                      X->getPosition(), Y->getPosition(), Z->getPosition()); break;

    case PID_GET_POS_COUNTER:             writeGetterValue2(PID_GET_POS_COUNTER,              CtrlStatistics::positionCounter,    (int32_t)CtrlStatistics::positionCounterOverflow); break;
    case PID_GET_STEP_COUNTER_X:          writeGetterValue2(PID_GET_STEP_COUNTER_X,           CtrlStatistics::stepCounter[IDX_X], (int32_t)CtrlStatistics::stepCounterOverflow[IDX_X]); break;
    case PID_GET_STEP_COUNTER_Y:          writeGetterValue2(PID_GET_STEP_COUNTER_Y,           CtrlStatistics::stepCounter[IDX_Y], (int32_t)CtrlStatistics::stepCounterOverflow[IDX_Y]); break;
    case PID_GET_STEP_COUNTER_Z:          writeGetterValue2(PID_GET_STEP_COUNTER_Z,           CtrlStatistics::stepCounter[IDX_Z], (int32_t)CtrlStatistics::stepCounterOverflow[IDX_Z]); break; 

    case PID_LIMIT:                       writeLimitGetter(); break;
    
    default:                              writeGetterValue1(PID_UNKNOWN, 0);
                                      
                                          AML::pushMessage(MT_ERROR, E_INVALID_GETTER_ID, gt.pid);
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

    default:                          AML::pushMessage(MT_ERROR, E_INVALID_PARAM_ID, st.pid);
                                      return RET_ERROR;
  }
  
  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::process(const ArduinoCmdDecoderMoveSequence::Result& seq) {
/////////////////////////////////////////////////////////////////////////////////////
  switch ( seq.cmd ) {
    case CMD_MOVE_SEQUENCE:  return renderMove(seq.dx, seq.dy, seq.dz);
    default:                 return renderMove(seq.dx, seq.dy, seq.dz);
  }
  
  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::initialize(const ArduinoCmdDecoderMoveSequence::Result& seq) {
/////////////////////////////////////////////////////////////////////////////////////  
  ARDO_DEBUG_MESSAGE('S', "MoveSequence started")
  
  tsMoveStart = ArdoTs::now();
  tsMoveLast  = tsMoveStart;

  const int32_t ic = seq.impulseCount;// impulseCalculator.calculate(seq.lengthX, seq.lengthY, seq.lengthZ);
  if ( ic < 0 )
    return RET_ERROR;
   
  if ( CtrlSpeedValues::cfgF_MMSec && ArduinoAccelManager::initMove(ic, CtrlSpeedValues::cfgF_MMSec) == false )
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
    case CMD_MOVE_UNIT_LIMIT_IS_FREE: return moveUntilLimitIsFree (mv.dx, mv.dy, mv.dz);
    default:                          return movePosition         (mv.dx, mv.dy, mv.dz);
  }

  return RET_ERROR;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::moveUntilLimitIsFree(int32_t dx, int32_t dy, int32_t dz) {
/////////////////////////////////////////////////////////////////////////////////////
  bool retX = true;
  bool retY = true;
  bool retZ = true;
  
  if ( dx != 0 ) { retX = X->resolveLimit(); }
  if ( dy != 0 ) { retY = Y->resolveLimit(); }
  if ( dz != 0 ) { retZ = Z->resolveLimit(); }

  //PRINT_DEBUG_VALUE("retX", retX);
  //PRINT_DEBUG_VALUE("retY", retY);
  //PRINT_DEBUG_VALUE("retZ", retZ);
  
  return (retX == true && retY == true && retZ == true) ? RET_OK : RET_LIMIT;
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

  tsMoveStart = ArdoTs::now();
  tsMoveLast  = tsMoveStart;
  
  if ( CtrlSpeedValues::cfgF_MMSec && initMove(ic, CtrlSpeedValues::cfgF_MMSec) == false )
    return RET_ERROR;

  return renderMove(dx, dy, dz);
}
