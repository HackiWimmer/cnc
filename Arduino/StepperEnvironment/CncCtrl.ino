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
CncArduinoController::SpindleInterface::SpindleInterface(byte pp, byte sp, byte op)
: pwrPin      (pp)
, splPin      (sp)
, ovlPin      (op)
, enabled     (false)
, speedRange  (255)
, speedValue  (255)
/////////////////////////////////////////////////////////////////////////////////////
{
}
/////////////////////////////////////////////////////////////////////////////////////
CncArduinoController::SpindleInterface::~SpindleInterface() {
/////////////////////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::SpindleInterface::reset() {
/////////////////////////////////////////////////////////////////////////////////////
  *this = CncArduinoController::SpindleInterface(pwrPin, splPin, ovlPin);  
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::SpindleInterface::enable(bool state ) {
/////////////////////////////////////////////////////////////////////////////////////
  enabled = state;
  AE::digitalWrite(pwrPin, state ? PL_HIGH : PL_LOW);
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::SpindleInterface::isOverloaded() const {
/////////////////////////////////////////////////////////////////////////////////////
  if ( enabled == false )
    return false;

  // for more details see getRemainingSeconds()
  return getRemainingSeconds() > 0;
}
/////////////////////////////////////////////////////////////////////////////////////
int CncArduinoController::SpindleInterface::getRemainingSeconds() const {
// a negative return value represents infinity 
/////////////////////////////////////////////////////////////////////////////////////
  if ( enabled == false )
    return -1;

  // 0 - 1023
  const int max = 1023;
  const int lv0 = 0.5/5.0 * max;
  const int lv1 = 1.5/5.0 * max;
  const int lv2 = 2.5/5.0 * max;
  const int lv3 = 3.0/5.0 * max;
  const int lv4 = 4.0/5.0 * max;
  const int lv5 = 4.5/5.0 * max;
  
  const int val = AE::analogRead(ovlPin);
  int ret = -1;

  // Following block is a Mafel FM 1000 specific implementation
  if      ( val <  lv0 ) ret =  -1;
  else if ( val <= lv1 ) ret = 160;
  else if ( val <= lv2 ) ret =  80;
  else if ( val <= lv3 ) ret =  40;
  else if ( val <= lv4 ) ret =  20;
  else if ( val <= lv5 ) ret =  10;
  else                   ret =   5;

  return ret;
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::SpindleInterface::setSpeedFactor(int32_t ssf) { 
/////////////////////////////////////////////////////////////////////////////////////
  speedRange = ArdoObj::SpindleTuple::decodeRange(ssf); 
  speedValue = ArdoObj::SpindleTuple::decodeValue(ssf);
  
  ARDO_DEBUG_VALUE("Spindle speed range", wxString::Format("%d, %d", speedRange, speedValue))

  enable(speedRange != 0 ? true : false);

  if ( enabled == false )
    return;

  const float fact    = (float)(speedRange) / (float)ArdoObj::SpindleTuple::ardoRange;

  #define maxVolt 10
  ARDO_DEBUG_VALUE("Spindle speed to write [PWM, V]", 
                    wxString::Format("%d, %f", 
                    (int)round(speedValue * fact), 
                    (float)((speedValue * fact)/(float)ArdoObj::SpindleTuple::ardoRange * maxVolt)));
  #undef maxVolt
      
  AE::analogWrite(splPin, round(speedValue * fact));
}


/////////////////////////////////////////////////////////////////////////////////////
CncArduinoController::CncArduinoController()
/////////////////////////////////////////////////////////////////////////////////////
: ArduinoCmdDecoderGetter       () 
, ArduinoCmdDecoderSetter       ()
, ArduinoCmdDecoderMove         () 
, ArduinoCmdDecoderMovePodium   ()
, ArduinoCmdDecoderMoveSequence ()
, ArduinoPositionRenderer       ()
, ArduinoAccelManager           ()
, ArduinoPodiumManager          ()
, X                             ( new CncAxisX(StepperSetup( this, PIN_X_STP, PIN_X_DIR, PIN_X_MIN_LIMIT, PIN_X_MAX_LIMIT, PID_INC_DIRECTION_VALUE_X )) )
, Y                             ( new CncAxisY(StepperSetup( this, PIN_Y_STP, PIN_Y_DIR, PIN_Y_MIN_LIMIT, PIN_Y_MAX_LIMIT, PID_INC_DIRECTION_VALUE_Y )) )
, Z                             ( new CncAxisZ(StepperSetup( this, PIN_Z_STP, PIN_Z_DIR, PIN_Z_MIN_LIMIT, PIN_Z_MAX_LIMIT, PID_INC_DIRECTION_VALUE_Z )) )
, H                             ( new CncAxisH(StepperSetup( this, PIN_H_STP, PIN_H_DIR, PIN_H_MIN_LIMIT, PIN_H_MAX_LIMIT, PID_INC_DIRECTION_VALUE_H )) )
, testManager                   (NULL)
, impulseCalculator             ()
, spindleInterface              (PIN_SPINDEL_SUPPORT, PIN_SPINDEL_SPEED_INF, PIN_IS_SPINDEL_OVRLD)
, lastI2CData                   ()
, transactionState              (OFF)
, podiumHardwareState           (OFF)
, posReplyState                 (OFF)
, probeMode                     (OFF)
, pauseState                    (PAUSE_INACTIVE)
, I2CAvailable                  (false)
, cfgF1000_MMSEC                (0)
, cmsF1000_MMMin                (0)
, posReplyCounter               (0)
, posReplyThreshold             (100)
, tsMoveStart                   (0L)
, tsMoveLast                    (0L)
, podiumStillOpenSteps          (0L)
, interactiveMove               ()
{  
  ArduinoPositionRenderer::setupSteppers(X, Y, Z);

  #warning - remove this again if the podium limit switches for H are installed
  CncAxisH::doConsiderLimitPins(false);
}
/////////////////////////////////////////////////////////////////////////////////////
CncArduinoController::~CncArduinoController() {
/////////////////////////////////////////////////////////////////////////////////////
  delete X; X = NULL;
  delete Y; Y = NULL;
  delete Z; Z = NULL;
  delete H; H = NULL;
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::printConfig() {
/////////////////////////////////////////////////////////////////////////////////////
  int limitState = -1, supportState = -1;
  if ( isI2CAvailable() == true ) 
  {
    if ( ArduinoMainLoop::readI2CSlave(lastI2CData) ) 
    {
      limitState   = (int)lastI2CData.limitState;
      supportState = (int)lastI2CData.supportState;
    }
  }

  Serial.print(PID_CONTROLLER); Serial.print(TEXT_SEPARATOR); Serial.write(TEXT_CLOSE);

    CtrlParameter::print(PID_SETUP_ID,                         SETUP_ID);
    CtrlParameter::print(PID_POS_REPLY_THRESHOLD,              getPosReplyThreshold());
    CtrlParameter::print(PID_PROBE_MODE,                       isProbeMode());
    CtrlParameter::print(PID_ENABLE_STEPPERS,                  AE::digitalRead(PIN_ENABLE_STEPPER));
    CtrlParameter::print(PID_SPINDLE_SWITCH,                   AE::digitalRead(PIN_ENABLE_SPINDLE));
    CtrlParameter::print(PID_I2C_AVAILABEL,                    isI2CAvailable());
    CtrlParameter::print(PID_I2C_LIMIT_VALUE,                  limitState);
    CtrlParameter::print(PID_I2C_SUPPORT_VALUE,                supportState);

    X->printConfig();
    Y->printConfig();
    Z->printConfig();
    H->printConfig();
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
bool CncArduinoController::evaluatePodiumSwitches() {
/////////////////////////////////////////////////////////////////////////////////////
  if ( podiumHardwareState == OFF )
    return true;

  // these pins are optionally
  if ( PIN_H_MOVE_UP == 0 || PIN_H_MOVE_DOWN == 0 )
    return true;

  const bool btUp   = ( AE::analogRead(PIN_H_MOVE_UP)    == PODIUM_SWITCH_ON );
  const bool btDown = ( AE::analogRead(PIN_H_MOVE_DOWN)  == PODIUM_SWITCH_ON );
  
  if ( btUp == PL_HIGH || btDown == PL_HIGH )
  { 
    byte b = movePodium(CMD_MOVE_PODIUM,  btUp ? +1 : -1, CncArduinoController::stopMovePodiumBySwitch);  
    return b == RET_OK;
  }

  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::reset() {
/////////////////////////////////////////////////////////////////////////////////////
    CNC_CONTROLLER_LOG_FUNCTION();

    posReplyCounter     = 0;
    posReplyThreshold   = 100;

    transactionState    = OFF;
    posReplyState       = OFF;
    probeMode           = OFF;

    interactiveMove.reset();
    spindleInterface.reset();
    
    X->reset();
    Y->reset();
    Z->reset();
    H->reset();
    
    X->resetPosition();
    Y->resetPosition();
    Z->resetPosition();
    H->resetPosition();
    
    ArduinoAccelManager::reset();
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::turnOff() {
/////////////////////////////////////////////////////////////////////////////////////
  switchSpindleState (SPINDLE_STATE_OFF, FORCE);
  enableStepperPin(ENABLE_STATE_OFF);
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::isReadyToRun() {
/////////////////////////////////////////////////////////////////////////////////////
  CNC_CONTROLLER_LOG_FUNCTION();
  bool ret = true;

  if ( READ_EXT_INNTERRUPT_PIN == EXTERNAL_INTERRRUPT_ON )
  {
    LastErrorCodes::clear(); 
    ArduinoMainLoop::pushMessage(MT_ERROR, E_INTERRUPT); 
    ret = false; 
  }

  if ( READ_IS_CTRL_POWERED_PIN == POWER_STATE_OFF )
  {
    ArduinoMainLoop::pushMessage(MT_ERROR, E_POWER_OFF, "X");  
    ret = false; 
  }

  if ( X->isReadyToRun() == false )
  { 
    ArduinoMainLoop::pushMessage(MT_ERROR, E_STEPPER_NOT_READY_TO_RUN, "X");    
    ret = false; 
  }
  
  if ( Y->isReadyToRun() == false )
  { 
    ArduinoMainLoop::pushMessage(MT_ERROR, E_STEPPER_NOT_READY_TO_RUN, "Y");    
    ret = false; 
  }
  
  if ( Z->isReadyToRun() == false )
  { 
    ArduinoMainLoop::pushMessage(MT_ERROR, E_STEPPER_NOT_READY_TO_RUN, "Z");
    ret = false; 
  }

  if ( isReadyToRender() == false) 
  {
    // message already created by class ArduinoPositionRenderer
    ret = false;
  }
  
  return ret;
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::broadcastInterrupt() {
/////////////////////////////////////////////////////////////////////////////////////
  // Turn off ...
  switchSpindleState (SPINDLE_STATE_OFF, FORCE);
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
void CncArduinoController::setSpeedValue_MMSec1000(int32_t f, bool activateAcceleration) {
/////////////////////////////////////////////////////////////////////////////////////
  f = ArdoObj::SpeedTuple::decodeValue_MMSec1000(f);
  cfgF1000_MMSEC = ArdoObj::absolute(f);
  
  ArduinoAccelManager::activate(activateAcceleration);

  ARDO_DEBUG_VALUE("F [mm/sec]", cfgF1000_MMSEC / 1000.0)
  ARDO_TRACE_SPEED('C', cfgF1000_MMSEC / 1000.0)
  
  if ( activateAcceleration == false ) {
    ARDO_DEBUG_VALUE("ArduinoAccelManager", "Is deactivated!")
  }
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::setupAccelProfile(const ArduinoCmdDecoderSetter::Result& st) {
/////////////////////////////////////////////////////////////////////////////////////
  typedef ArduinoAccelManager::Function Function;
  Function fA(st.values[ 6].asFloat(), st.values[ 7].asFloat(), st.values[ 8].asFloat());
  Function fD(st.values[ 9].asFloat(), st.values[10].asFloat(), st.values[11].asFloat());

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

    X->setFeedrate(st.values[0].asFloat(FEEDRATE_FACT));
    Y->setFeedrate(st.values[1].asFloat(FEEDRATE_FACT));
    Z->setFeedrate(st.values[2].asFloat(FEEDRATE_FACT));

    X->setHighPulseWidth(round(st.values[3].asFloat()));
    Y->setHighPulseWidth(round(st.values[4].asFloat()));
    Z->setHighPulseWidth(round(st.values[5].asFloat()));
  }

  // Pepare some speed  values  
  ArduinoAccelManager::Setup(X->getFeedrate(), Y->getFeedrate(), Z->getFeedrate());
  ArduinoAccelManager::initialize(fA, fD);  

  if ( true ) {
    ARDO_DEBUG_VALUE("AccelProfile::FeedrateX", X->getFeedrate())
    ARDO_DEBUG_VALUE("AccelProfile::FeedrateY", Y->getFeedrate())
    ARDO_DEBUG_VALUE("AccelProfile::FeedrateZ", Z->getFeedrate())
    
    ARDO_DEBUG_VALUE("AccelProfile::HighPulseWidthX", X->getHighPulseWidth())
    ARDO_DEBUG_VALUE("AccelProfile::HighPulseWidthY", Y->getHighPulseWidth())
    ARDO_DEBUG_VALUE("AccelProfile::HighPulseWidthZ", Z->getHighPulseWidth())

    ARDO_DEBUG_VALUE("AccelProfile::fA.A_1000", fA.getA_1000())
    ARDO_DEBUG_VALUE("AccelProfile::fA.B_1000", fA.getB_1000())
    ARDO_DEBUG_VALUE("AccelProfile::fA.C_1000", fA.getC_1000())
    ARDO_DEBUG_VALUE("AccelProfile::fD.A_1000", fD.getA_1000())
    ARDO_DEBUG_VALUE("AccelProfile::fD.B_1000", fD.getB_1000())
    ARDO_DEBUG_VALUE("AccelProfile::fD.C_1000", fD.getC_1000())
  }
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::switchSpindleState(bool state, bool force) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( force == false ) {
    // Don't enable the tool during a probe mode phase
    if ( isProbeMode() == ON ) {
      AE::digitalWrite(PIN_ENABLE_SPINDLE, SPINDLE_STATE_OFF);
      return;
    }
  }
  
  AE::digitalWrite(PIN_ENABLE_SPINDLE, state);

  // give the tool a portion of time to run properly
  if ( state == SPINDLE_STATE_ON )
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
byte CncArduinoController::acceptTransaction(byte cmd) {
/////////////////////////////////////////////////////////////////////////////////////
  transactionState = ( cmd == CMD_PUSH_TRANSACTION ? ON : OFF );
  ARDO_DEBUG_MESSAGE('S', transactionState == ON ? "Push Transaction" : "Pop Transaction")
  
  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::activatePodiumHardware(byte cmd) {
/////////////////////////////////////////////////////////////////////////////////////
  podiumHardwareState = ( cmd == CMD_ACTIVATE_PODIUM_HW ? ON : OFF );
  ARDO_DEBUG_MESSAGE('S', podiumHardwareState == ON ? "Switch Podium Harware 'ON'" : "Switch Podium Harware 'OFF'")

  AE::digitalWrite(PIN_LED_PODIUM, podiumHardwareState ? PL_HIGH : PL_LOW);
   
  return RET_OK;
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
byte CncArduinoController::acceptPodiumMove(byte cmd) {
/////////////////////////////////////////////////////////////////////////////////////
  return decodeMovePodium(cmd);    
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::process(const ArduinoCmdDecoderMovePodium::Result& mv) {
/////////////////////////////////////////////////////////////////////////////////////
  byte ret = RET_ERROR;

  // first set generally 
  podiumStillOpenSteps = ArdoObj::absolute(mv.dh);  
  
  switch ( mv.cmd )
  {
    case CMD_MOVE_PODIUM_EXACT:
          podiumStillOpenSteps = ArdoObj::absolute(mv.dh);  
          ret = movePodium(mv.cmd, mv.dh, CncArduinoController::stopMovePodiumExact);
          break;
          
    case CMD_MOVE_PODIUM:
          podiumStillOpenSteps = MAX_LONG;  
          ret = movePodium(mv.cmd, mv.dh, CncArduinoController::stopMovePodiumBySignal);
          break;
  }

  return ret;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::movePodium(unsigned char cmd, int32_t stepDir, stopPodiumHardware_funct stopFunct) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( stopFunct == NULL )
    return RET_ERROR;

  if ( podiumStillOpenSteps <= 0 )
    return RET_ERROR;

  // Create this instance to enable the podium stepper pin and release the brake.
  // The corresponding dtor will inverse this again
  ArduinoPodiumEnabler pe;

  if ( H->setDirection(stepDir) == false ) 
  {
    ArduinoMainLoop::pushMessage(MT_ERROR, E_PODIUM_DIR_CHANGE_FAILED, CMD_MOVE_PODIUM);
    return RET_ERROR;
  }

  // ------------------------------------------------------------------------------- 
  // Determine acceleration
  const uint32_t maxSpeedDelay = cmd == CMD_MOVE_PODIUM_EXACT ? 1100 : 1000;
  const uint32_t minSpeedDelay = cmd == CMD_MOVE_PODIUM_EXACT ?   10 :  500;
  
  const uint32_t difSpeedDelay = maxSpeedDelay - minSpeedDelay;
  uint32_t       curSpeedDelay = maxSpeedDelay;

  // attention: down counter
  int32_t p1 = podiumStillOpenSteps - difSpeedDelay;
  int32_t p2 = difSpeedDelay;

  if ( p1 < 0 || p2 > p1 )
    p1 = p2 = podiumStillOpenSteps / 2;    

  // ------------------------------------------------------------------------------- 
  // run ...
  setPosReplyState(true);
  sendCurrentPositions(PID_H_POS, true);
  
  while ( stopFunct(this) == false ) 
  {
    // ---------------------------------------------------------------------------
    // error message already published on demand
    byte retValue = checkRuntimeEnv();
    if ( retValue != RET_OK )
      return retValue;

    if ( ArduinoPodiumManager::isEnabled() == false )
    {
      ArduinoMainLoop::pushMessage(MT_ERROR, E_STEPPER_NOT_ENABLED, ARDO_LOG_FUNCT);
      return RET_ERROR;
    }
    
    // ---------------------------------------------------------------------------
    {
      byte b = H->initiateStep();
      if ( b != RET_OK ) 
      {
        ArduinoMainLoop::pushMessage(MT_ERROR, E_PODIUM_MOVE_FAILED, ARDO_LOG_FUNCT);
        return b;
      }
    }
    
    // ---------------------------------------------------------------------------
    if ( podiumStillOpenSteps > p1 )
    {
      if ( curSpeedDelay >= minSpeedDelay ) 
        curSpeedDelay--;
    }
    else if ( podiumStillOpenSteps < p2 )
    {
      if ( curSpeedDelay <= maxSpeedDelay ) 
        curSpeedDelay++;
    }
     
    AE::delayMicroseconds(curSpeedDelay);
    podiumStillOpenSteps--;
    
    // ---------------------------------------------------------------------------
    {
      byte b = H->finalizeStep();
      if ( b != RET_OK ) 
      {
        ArduinoMainLoop::pushMessage(MT_ERROR, E_PODIUM_MOVE_FAILED, ARDO_LOG_FUNCT);
        return b;
      }
    }
    
    if ( podiumStillOpenSteps % 8 == 0 )
      sendCurrentPositions(PID_H_POS, true);
  }

  sendCurrentPositions(PID_H_POS, true);
  setPosReplyState(false);
  
  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::stopMovePodiumExact(CncArduinoController* ctrl) {
/////////////////////////////////////////////////////////////////////////////////////
  return ctrl->podiumStillOpenSteps <= 0;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::stopMovePodiumBySignal(CncArduinoController* ctrl){
/////////////////////////////////////////////////////////////////////////////////////
  byte serialFrontByte = CMD_INVALID;
  if ( ArduinoMainLoop::peakSerial(serialFrontByte) == true ) {

    if ( serialFrontByte == SIG_QUIT_MOVE) {
      
      // read signal from serial
      Serial.read();
      return true;
    }
    
    if ( serialFrontByte == CMD_DEACTIVATE_PODIUM_HW) {
      ctrl->activatePodiumHardware(serialFrontByte);

      // read command from serial
      Serial.read();

      // handshake
      Serial.write(RET_OK);
      return true;
    }
   }

  return false;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::stopMovePodiumBySwitch(CncArduinoController* ctrl) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( CncArduinoController::stopMovePodiumBySignal(ctrl) == true )
    return true;
  
  if ( ctrl->podiumHardwareState == OFF )
    return true;

  // these pins are optionally  
  if ( PIN_H_MOVE_UP == 0 || PIN_H_MOVE_DOWN == 0 )
    return false;

  if ( AE::analogRead(PIN_H_MOVE_UP)   == PODIUM_SWITCH_ON )
    return false;
    
  if ( AE::analogRead(PIN_H_MOVE_DOWN) == PODIUM_SWITCH_ON )
    return false;

  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::initInteractiveSpeed() {
/////////////////////////////////////////////////////////////////////////////////////
  ArduinoAccelManager::finalize();
  
  // For interactive moves the impulse count at the move start isn't defined - by concept.
  // Therefore, to get a fully supported acceleration at the begining a value of 0 is used here,
  // which initialzes the accel manager to the interactive mode. The deacceleration phase isn't much
  // imported here, because we can loose steps . . . 
  const uint32_t defaultImpulses = 0;
  
  if ( cfgF1000_MMSEC && ArduinoAccelManager::initMove(defaultImpulses, cfgF1000_MMSEC) == false )
    return RET_ERROR;
    
  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::updateInteractiveMoveValues(int8_t dx, int8_t dy, int8_t dz) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( ArduinoAccelManager::isInteractiveMoveType() == true ) 
  {
    const bool bx = interactiveMove.valueX == dx;
    const bool by = interactiveMove.valueY == dy;
    const bool bz = interactiveMove.valueZ == dz;

    // check if any direction is changed
    if ( ( bx && by && bz ) != true )
      initInteractiveSpeed();
    
    interactiveMove.valueX  = dx;
    interactiveMove.valueY  = dy;
    interactiveMove.valueZ  = dz;
    
    interactiveMove.tsLast  = ArdoTs::now();
  }
  else
  {
    // ignore the given values if no interactive move is active
    interactiveMove.valueX  = 0;
    interactiveMove.valueY  = 0;
    interactiveMove.valueZ  = 0;

    interactiveMove.tsLast  = 0;
  }
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::updateInteractiveMove() {
/////////////////////////////////////////////////////////////////////////////////////
  interactiveMove.tsLast  = ArdoTs::now();
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::cancelInteractiveMove() {
/////////////////////////////////////////////////////////////////////////////////////  
  cmsF1000_MMMin = FLT_FACT;
  ArduinoAccelManager::finalize();

  interactiveMove.active  = false;
  interactiveMove.tsLast  = 0;
  
  interactiveMove.valueX  = 0;
  interactiveMove.valueY  = 0;
  interactiveMove.valueZ  = 0;
  
  return RET_OK;  
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::acceptInteractiveMove(byte) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( ArduinoAccelManager::isInteractiveMoveType() == true ) 
  {    
    // this appears from time to time due to the totally asynchron gui usage,
    // but this isn't really a problem. Therefore, return positive in this case  . . . 
    
    ArduinoMainLoop::pushMessage(MT_ERROR, E_OTHER_MOVE_CMD_ACTIVE);
    
    //return RET_ERROR;
    return RET_OK;
  }

  byte ret = RET_OK;

  // initialize movement
  ret = initInteractiveSpeed();
  if ( ret != RET_OK )
    return ret;
    
  updateInteractiveMoveValues(0, 0, 0);

  tsMoveStart = ArdoTs::now();
  tsMoveLast  = tsMoveStart;

  setPosReplyState(true);
  setPosReplyThreshold(posReplyThreshold);
  setProbeMode(OFF);
  enableStepperPin(ENABLE_STATE_ON);

  const uint32_t maxMicrosWithoutUpdate = 1000L * 1000L; // 1s
  int8_t timeoutCounter = 3;
  
  interactiveMove.tsLast = ArdoTs::now();
  interactiveMove.active = true;
  
  while ( interactiveMove.active == true ) 
  {
    ret = directMove(interactiveMove.valueX, interactiveMove.valueY, interactiveMove.valueZ);

    if ( ret != RET_OK )
      break;

    const uint32_t lastTsn = ArdoTs::timespan(interactiveMove.tsLast);
    if ( lastTsn > maxMicrosWithoutUpdate ) 
    {
      if ( timeoutCounter <= 0 )
      {
        //ARDO_DEBUG_MESSAGE('D', wxString::Format("ArdoTs::timespan(interactiveMove.tsLast) %ld > %ld [%ld](%ld)", (int32_t)lastTsn, (int32_t)maxMicrosWithoutUpdate, (int32_t)interactiveMove.tsLast, (int32_t)ArdoTs::now()))
  
        // check if any move parameters are active
        const bool b = ( interactiveMove.valueX == 0 && interactiveMove.valueY == 0 && interactiveMove.valueZ == 0);
  
        // reset move parameters
        interactiveMove.reset();
  
        // It's only a HALT if a movement was still active
        ret = b ? RET_OK : RET_HALT;
  
        // stop move loop
        break;
      }
      else
      {
        // Therefore, the first timeout query is stretched to maxMicrosWithoutUpdate * x
        timeoutCounter--;
      }
    }  
  }

  cancelInteractiveMove();
  sendCurrentPositions(PID_XYZ_POS_MAJOR, true);
  setPosReplyState(false);
  
  return ret;  
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::acceptMove(byte cmd) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( ArduinoAccelManager::isInteractiveMoveType() == true ) {
    ArduinoMainLoop::pushMessage(MT_ERROR, E_OTHER_MOVE_CMD_ACTIVE);
    return RET_ERROR;
  }

  setPosReplyState(true);
  const byte ret = decodeMove(cmd);

  if ( ret == RET_QUIT ) {
    // 0.0 isn't valid here because it will be ignored
    cmsF1000_MMMin = FLT_FACT;
    ArduinoAccelManager::finalize();
  }
  
  sendCurrentPositions(PID_XYZ_POS_MAJOR, true);
  setPosReplyState(false);
  return ret;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::acceptMoveSequence(byte cmd) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( ArduinoAccelManager::isInteractiveMoveType() == true ) {
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
                switchSpindleState(SPINDLE_STATE_OFF, FORCE);
                
                retValue = RET_HALT;
                return false;
                
    case SIG_QUIT_MOVE:
                cancelInteractiveMove();
                retValue = RET_QUIT;
                return false; 

    case SIG_UPDATE:
                // retValue will be determined as reference of processSignalUpdate()
                return processSignalUpdate(retValue);

    case SIG_PAUSE:
                pauseState = PAUSE_ACTIVE;
                
                retValue = RET_OK;
                return true;

    case SIG_RESUME:
                pauseState = PAUSE_INACTIVE;

                retValue = RET_OK;
                return true;
  }

  retValue = RET_OK;
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoController::processSignalUpdate(byte& retValue) {
/////////////////////////////////////////////////////////////////////////////////////
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
      int32_t dx; if ( ArduinoMainLoop::readInt8(dx) == false )    dx =   0;
      int32_t dy; if ( ArduinoMainLoop::readInt8(dy) == false )    dy =   0;
      int32_t dz; if ( ArduinoMainLoop::readInt8(dz) == false )    dz =   0;
      int32_t ds; if ( ArduinoMainLoop::readInt8(ds) == false )    ds = '+';
      int32_t df; if ( ArduinoMainLoop::readInt8(df) == false )    df =   0;

      // Check the serail again
      byte serialFrontByte = CMD_INVALID;
      if ( ArduinoMainLoop::peakSerial(serialFrontByte) == true ) 
      {
        // Skip this command if one of the following commands are present as next
        switch ( serialFrontByte ) {
          case SIG_QUIT_MOVE:
          case SIG_UPDATE:    Serial.read();
                              return processSignal(serialFrontByte, retValue);
        }
      }

      // Interactive speed management
      changeInteractiveFactor(df);

      // init the movment update
      // As Int8 to resolve also negative values
      updateInteractiveMoveValues((int8_t)dx, (int8_t)dy, (int8_t)dz);
      
      ret       = true;
      retValue  = RET_OK;
      
      break;
    }
    case PID_HEARTBEAT:
    {
      // this updates the interaction timestamp only
      updateInteractiveMove();
      
      ret       = true;
      retValue  = RET_OK;
      
      break;  
    }
    default:
    {
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

  if ( isReplyDue() || force == true ) 
  {
    posReplyCounter = 0;

    switch ( pid ) 
    {
      case PID_H_POS: ArduinoMainLoop::writeLongValue(pid, H->getPosition());
                      break;

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
                                      cmsF1000_MMMin);
                      break;

      default:        ; // do nothing

    }
    
    //Serial.flush();
  }
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::checkRuntimeEnv() {
/////////////////////////////////////////////////////////////////////////////////////
  static byte retValue = RET_ERROR;
  if ( Serial.available() ) {
    if ( observeSerialFrontByte(retValue) == false )
      return retValue;
  }

  if ( READ_EXT_INNTERRUPT_PIN == EXTERNAL_INTERRRUPT_ON ) {
    ArduinoMainLoop::pushMessage(MT_ERROR, E_INTERRUPT); 
    return RET_ERROR;
  }

  /*
  if ( READ_LMT_PINS == LimitSwitch::LIMIT_SWITCH_ON ) {
    ArduinoMainLoop::pushMessage(MT_ERROR, E_LIMIT_SWITCH_ACTIVE);
    return RET_LIMIT;
  }
  */

  if ( isProbeMode() == OFF ) {
    #warning reactivate this again if the corresponding pin info is available
    /*
    if ( READ_ENABLE_TOOL_PIN != READ_IS_TOOL_POWERED_PIN ) {
      ArduinoMainLoop::pushMessage(MT_ERROR, E_TOOL_NOT_ENALED); 
      return RET_ERROR;
    }
    */    
  }
  
  return RET_OK;   
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::notifyMovePartBefore() {
/////////////////////////////////////////////////////////////////////////////////////
  RS::stepSignatureIndex = GET_AxisSignatureIndex(RS::stepSignature);
  ArduinoAccelManager::initNextImpulse(RS::stepSignatureIndex);
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoController::notifyMovePartAfter() {
/////////////////////////////////////////////////////////////////////////////////////
  // *******************************************************************************
  // Speed measurement
  //                      cmsF1000_MMMin   = FLT_FACT * 60 * 1000 * [um/us] 
  //                                     --> FLT_FACT * 60 *        [nm/us] 
  //                                     --> FLT_FACT * 60 *        [mm/sec] 
  //                                     --> FLT_FACT *             [mm/min] 
  #define MEASURE_SPEED  \
        if ( bReply ) \
          cmsF1000_MMMin = 60L * FLT_FACT * 1000L * curDistV_UM / ArdoTs::timespan(tsMoveLast); 
  // *******************************************************************************

  // *******************************************************************************
  // delay management
  // #define NO_SPEED_DELAY
  #ifdef NO_SPEED_DELAY
                        #define WAIT_US(us)   {}          
  #else
                        #define WAIT_US(us)   { AE::delayMicroseconds(us); }        
  #endif  
  // *******************************************************************************

  // initalize position management
  posReplyCounter++;
  
  // speed management
  const bool bReply = isReplyDue();
  if ( cfgF1000_MMSEC > 0 ) {
    const int32_t curDistV_UM = ArduinoAccelManager::Setup::feedRate_UM[RS::stepSignatureIndex];
    
    // determine the time deviation between the measured and configured sight
    const int32_t currentTimeDistance_US = getCurrentTargetSpeedDelay_US();

    // don't put anything between the lines of the section below
    {
      // determine the rest delay and perform it
      const int32_t tsShould_US   = tsMoveLast  + currentTimeDistance_US;
      const int32_t rest_US       = tsShould_US - ArdoTs::now();
      if ( rest_US > 3 )
        WAIT_US(rest_US)

      // measure the current speed again on demand
      MEASURE_SPEED
      ARDO_TRACE_SPEED('M', cmsF1000_MMMin / (60 * 1000.0))
    }

    // this has to be the last action within this function
    tsMoveLast = ArdoTs::now();
  }
  
  // finalize position management
  if ( bReply )
    sendCurrentPositions(PID_XYZ_POS_DETAIL, false);

  #undef MEASURE_SPEED
  #undef WAIT_US
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
    case IDX_H: H->setHighPulseWidth(width); break;
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
    case IDX_H: return H->setDirection(steps);
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
    case IDX_H: return H->performStep();
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
    case IDX_H: return H->initiateStep();
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
    case IDX_H: return H->finalizeStep();
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

    case PID_PODIUM_POS:
    case PID_H_POS:                       writeGetterValue1(PID_H_POS,                        H->getPosition()); break;
    
    case PID_X_POS:                       writeGetterValue1(PID_X_POS,                        X->getPosition()); break;
    case PID_Y_POS:                       writeGetterValue1(PID_Y_POS,                        Y->getPosition()); break;
    case PID_Z_POS:                       writeGetterValue1(PID_Z_POS,                        Z->getPosition()); break;

    case PID_XY_POS:                      writeGetterValue2(PID_XY_POS,                       X->getPosition(), Y->getPosition()); break;
    case PID_XYZ_POS:                     writeGetterValue3(PID_XYZ_POS,                      X->getPosition(), Y->getPosition(), Z->getPosition()); break;

    case PID_LIMIT:                       writeLimitGetter(); break;

    case PID_TOUCH_CONTACT_STATE:         writeGetterValue1(PID_TOUCH_CONTACT_STATE,          (int32_t)(AE::digitalRead(PIN_TOUCH_CONTACT) == PL_LOW)); break;
    
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
    case PID_X_POS:                   X->setPosition(st.values[0].asInt32());                 break;
    case PID_Y_POS:                   Y->setPosition(st.values[0].asInt32());                 break;
    case PID_Z_POS:                   Z->setPosition(st.values[0].asInt32());                 break;

    case PID_INC_DIRECTION_VALUE_X:   X->setIncrementDirectionValue(st.values[0].asInt32());  break;
    case PID_INC_DIRECTION_VALUE_Y:   Y->setIncrementDirectionValue(st.values[0].asInt32());  break;
    case PID_INC_DIRECTION_VALUE_Z:   Z->setIncrementDirectionValue(st.values[0].asInt32());  break;
    case PID_INC_DIRECTION_VALUE_H:   H->setIncrementDirectionValue(st.values[0].asInt32());  break;
    
    case PID_POS_REPLY_THRESHOLD:     setPosReplyThreshold(st.values[0].asInt32());           break;
    
    case PID_SPEED_MM_SEC:            setSpeedValue_MMSec1000(st.values[0].asInt32());        break;

    case PID_SPINDLE_SPEED:           spindleInterface.setSpeedFactor(st.values[0].asInt32());
                                      break;

    case PID_SPINDLE_SWITCH:          switchSpindleState(st.values[0].asBool());              break;

    case PID_ENABLE_STEPPERS:         enableStepperPin(st.values[0].asBool());                break;

    case PID_PROBE_MODE:              setProbeMode(st.values[0].asBool());                    break;
    
    case PID_ACCEL_PROFILE:           setupAccelProfile(st); 
                                      break;
                                      
    case PID_PODIUM_POS:              H->setPosition(st.values[0].asInt32());                 break;

    case PID_PULSE_WIDTH_HIGH_H:      H->setHighPulseWidth(st.values[0].asInt32());           break;
    case PID_FEEDRATE_H:              H->setFeedrate(st.values[0].asFloat(FEEDRATE_FACT));    break;

    default:                          AML::pushMessage(MT_ERROR, E_INVALID_PARAM_ID, st.pid);
                                      return RET_ERROR;
  }
  
  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::process(const ArduinoCmdDecoderMove::Result& mv) {
/////////////////////////////////////////////////////////////////////////////////////
  // select underlying mechanism 
  switch ( mv.cmd ) {
    case CMD_MOVE_UNTIL_CONTACT:        return moveUntilContact     (mv.dx, mv.dy, mv.dz);
    case CMD_MOVE_UNTIL_LIMIT_IS_FREE:  return moveUntilLimitIsFree (mv.dx, mv.dy, mv.dz);
    default:                            return movePosition         (mv.dx, mv.dy, mv.dz);
  }

  return RET_ERROR;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::process(const ArduinoCmdDecoderMoveSequence::Result& seq) {
/////////////////////////////////////////////////////////////////////////////////////
  const byte ret = renderMove(seq.dx, seq.dy, seq.dz);
  
  // renderMove() processes the linear distance betwenn two points. Due to a performance 
  // improvement the position reply isn't continious active for each step. Therefore, to
  // get straight lines a the monitoring the current position has to be reported at the 
  // end of each linear distance.
  sendCurrentPositions(PID_XYZ_POS_MAJOR, true);  
  
  return ret;
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
  
  if ( cfgF1000_MMSEC && initMove(ic, cfgF1000_MMSEC) == false )
    return RET_ERROR;

  return renderMove(dx, dy, dz);
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::initialize(const ArduinoCmdDecoderMoveSequence::Result& seq) {
/////////////////////////////////////////////////////////////////////////////////////  
  ARDO_DEBUG_MESSAGE('S', "MoveSequence started")
  
  tsMoveStart = ArdoTs::now();
  tsMoveLast  = tsMoveStart;

  const int32_t ic = seq.impulseCount;
  if ( ic < 0 )
    return RET_ERROR;
   
  if ( cfgF1000_MMSEC && ArduinoAccelManager::initMove(ic, cfgF1000_MMSEC) == false )
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
byte CncArduinoController::moveUntilContact(int32_t dx, int32_t dy, int32_t dz) {
/////////////////////////////////////////////////////////////////////////////////////
  ARDO_DEBUG_MESSAGE('S', "Move until contact")

  const int8_t x = dx ? (dx / ArdoObj::absolute(dx)) : 0;
  const int8_t y = dy ? (dy / ArdoObj::absolute(dy)) : 0;
  const int8_t z = dz ? (dz / ArdoObj::absolute(dz)) : 0;

  int32_t     cx = 0;
  int32_t     cy = 0;
  int32_t     cz = 0;

  byte ret = RET_OK;
  
  tsMoveStart = ArdoTs::now();
  tsMoveLast  = tsMoveStart;

  setPosReplyState(true);
  setPosReplyThreshold(posReplyThreshold);
  setProbeMode(OFF);
  enableStepperPin(ENABLE_STATE_ON);
  
  while ( AE::digitalRead(PIN_TOUCH_CONTACT) == PL_HIGH ) {
    ret = directMove(x, y, z);

    cx += ArdoObj::absolute(x);  
    cy += ArdoObj::absolute(y);
    cz += ArdoObj::absolute(z);

    if ( ret != RET_OK )               break;
    if ( cx > ArdoObj::absolute(dx) )  break;
    if ( cy > ArdoObj::absolute(dy) )  break;
    if ( cz > ArdoObj::absolute(dz) )  break;
  }

  sendCurrentPositions(PID_XYZ_POS_MAJOR, true);
  setPosReplyState(false);
  
  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
byte CncArduinoController::moveUntilLimitIsFree(int32_t dx, int32_t dy, int32_t dz) {
/////////////////////////////////////////////////////////////////////////////////////
  ARDO_DEBUG_MESSAGE('S', "Move until Limit is free")

  bool retX = true;
  bool retY = true;
  bool retZ = true;
  
  if ( dx != 0 ) { retX = X->resolveLimit(); }
  if ( dy != 0 ) { retY = Y->resolveLimit(); }
  if ( dz != 0 ) { retZ = Z->resolveLimit(); }

  return (retX == true && retY == true && retZ == true) ? RET_OK : RET_LIMIT;
}
