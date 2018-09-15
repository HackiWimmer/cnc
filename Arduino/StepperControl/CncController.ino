#include "CncController.h"
#include "CommonFunctions.h"
#include "CommonValues.h"

/////////////////////////////////////////////////////////////////////////////////////
struct RenderStruct {
  static const short POINT_LENGTH = 3;
  static const short IDX_X        = 0;
  static const short IDX_Y        = 1;
  static const short IDX_Z        = 2;
  
  static int A[POINT_LENGTH];
  static int B[POINT_LENGTH];

  static unsigned short xDelay;
  static unsigned short yDelay;
  static unsigned short zDelay;
  
  static unsigned short xStepCount;
  static unsigned short yStepCount;
  static unsigned short zStepCount;

  static void reset() {
    memset(A, 0, sizeof(A));
    memset(B, 0, sizeof(B));

    xStepCount = 0;
    yStepCount = 0;
    zStepCount = 0;
  }

  static void swap() {
    memcpy(B, A, sizeof(B));
  }

  static bool empty() {
    return (    A[IDX_X] - B[IDX_X] == 0 
             && A[IDX_Y] - B[IDX_Y] == 0
             && A[IDX_Z] - B[IDX_Z] == 0
           );
  }

  static int dx() { return A[IDX_X] - B[IDX_X]; } 
  static int dy() { return A[IDX_Y] - B[IDX_Y]; }
  static int dz() { return A[IDX_Z] - B[IDX_Z]; }
};

int            RenderStruct::A[RenderStruct::POINT_LENGTH] = {0,0,0};
int            RenderStruct::B[RenderStruct::POINT_LENGTH] = {0,0,0};
unsigned short RenderStruct::xDelay                        = 0;
unsigned short RenderStruct::yDelay                        = 0;
unsigned short RenderStruct::zDelay                        = 0;
unsigned short RenderStruct::xStepCount                    = 0;
unsigned short RenderStruct::yStepCount                    = 0;
unsigned short RenderStruct::zStepCount                    = 0;
/////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////
CncController::CncController(LastErrorCodes& lec) 
/////////////////////////////////////////////////////////////////////////////////////
: X(new CncStepper(this, 'X', PIN_X_STP, PIN_X_DIR, PIN_X_LIMIT, lec))
, Y(new CncStepper(this, 'Y', PIN_Y_STP, PIN_Y_DIR, PIN_Y_LIMIT, lec))
, Z(new CncStepper(this, 'Z', PIN_Z_STP, PIN_Z_DIR, PIN_Z_LIMIT, lec))
, speedController()
, errorInfo(&lec)
, posReplyThresholdX(100L)
, posReplyThresholdY(100L)
, posReplyThresholdZ(100L)
, positionCounter(MIN_LONG)
, positionCounterOverflow(0L)
, posReplyState(OFF)
, probeMode(OFF)
, pause(PAUSE_INACTIVE)
, lastHeartbeat(0)
{
}
/////////////////////////////////////////////////////////////////////////////////////
CncController::~CncController() {
/////////////////////////////////////////////////////////////////////////////////////
  delete X;
  delete Y;
  delete Z;
}
/////////////////////////////////////////////////////////////////////////////////////
int32_t CncController::isReadyToRun() {
/////////////////////////////////////////////////////////////////////////////////////
  if ( speedController.isInitialized() == false )   { errorInfo->setNextErrorInfo(E_SPEED_MGMT_NOT_INITIALIZED, BLANK);  return 0L; }
  if ( X->isReadyToRun() == 0L )                    { errorInfo->setNextErrorInfo(E_STEPPER_NOT_READY_TO_RUN, "X");      return 0L; }
  if ( Y->isReadyToRun() == 0L )                    { errorInfo->setNextErrorInfo(E_STEPPER_NOT_READY_TO_RUN, "Y");      return 0L; }
  if ( Z->isReadyToRun() == 0L )                    { errorInfo->setNextErrorInfo(E_STEPPER_NOT_READY_TO_RUN, "Z");      return 0L; }
  
  return 1L;
}
////////////////////////////////////////////////////////////////////////////////////
void CncController::setupSpeedController() {
/////////////////////////////////////////////////////////////////////////////////////
  speedController.setup('X', X->getSteps(), X->getPitch(), SPEED_MANAGER_CONST_STATIC_OFFSET_US, SPEED_MANAGER_CONST_LOOP_OFFSET_US, X->getLowPulseWidth() + X->getHighPulseWidth());
  speedController.setup('Y', Y->getSteps(), Y->getPitch(), SPEED_MANAGER_CONST_STATIC_OFFSET_US, SPEED_MANAGER_CONST_LOOP_OFFSET_US, Y->getLowPulseWidth() + Y->getHighPulseWidth());
  speedController.setup('Z', Z->getSteps(), Z->getPitch(), SPEED_MANAGER_CONST_STATIC_OFFSET_US, SPEED_MANAGER_CONST_LOOP_OFFSET_US, Z->getLowPulseWidth() + Z->getHighPulseWidth());
}
//////////////////////////////////////////////////////////////////////////////
bool CncController::enableStepperPin(bool state){
//////////////////////////////////////////////////////////////////////////////
  if ( probeMode == false )   digitalWrite(PIN_ENABLE, state == true ? ENABLE_STATE_ON : ENABLE_STATE_OFF);
  else                        digitalWrite(PIN_ENABLE, ENABLE_STATE_OFF);

  delayMicroseconds(minEnablePulseWide);
  return state;
}
/////////////////////////////////////////////////////////////////////////////////////
void CncController::printConfig() {
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
    PRINT_PARAMETER(PID_ENABLE_STEPPERS,                  !digitalRead(PIN_ENABLE))

    int limitState = -1, supportState = -1;
    if ( isI2CAvailable() == true ) {
      I2CData data;
      if ( readI2CSlave(data) ) {
        limitState   = (int)data.limitState;
        supportState = (int)data.supportState;
      }
    }
    
    PRINT_PARAMETER(PID_I2C_LIMIT_VALUE,                 limitState)
    PRINT_PARAMETER(PID_I2C_SUPPORT_VALUE,               supportState)

  #undef PRINT_PARAMETER
  
  speedController.printConfig();
      
  X->printConfig();
  Y->printConfig();
  Z->printConfig();
}
/////////////////////////////////////////////////////////////////////////////////////
void CncController::reset() {
/////////////////////////////////////////////////////////////////////////////////////
    posReplyThresholdX = 100L;
    posReplyThresholdY = 100L;
    posReplyThresholdZ = 100L;

    posReplyState     = false;
    probeMode         = false;
    
    X->reset();
    Y->reset();
    Z->reset();
    
    X->resetPosition();
    Y->resetPosition();
    Z->resetPosition();

    resetPositionCounter();
}
/////////////////////////////////////////////////////////////////////////////////////
void CncController::incPositionCounter() { 
/////////////////////////////////////////////////////////////////////////////////////
  // detect overflows
  if ( positionCounter == MAX_LONG ) { 
    positionCounter = MIN_LONG;
    positionCounterOverflow++;
  }
    
  positionCounter++;
}
/////////////////////////////////////////////////////////////////////////////////////  
bool CncController::evaluateSupportButtonState(unsigned short idx) {
// true:  button pressed
// false: button released
/////////////////////////////////////////////////////////////////////////////////////  
  if ( isI2CAvailable() == false )
    return false;
    
  I2CData data;
  CncInterface::ISP::States sp;
  if ( readI2CSlave(data) ) sp = CncInterface::ISP::States(data.supportState);
  else                      return false;
 
  switch ( idx ) {
    case 1: return sp.isSupportButton1Pressed();
    case 2: return sp.isSupportButton2Pressed();
    case 3: return sp.isSupportButton3Pressed();
  }
  
  return false;
}
/////////////////////////////////////////////////////////////////////////////////////  
bool CncController::evaluateSupportSwitchState(unsigned short idx) {
// true:  button pressed
// false: button released
/////////////////////////////////////////////////////////////////////////////////////  
  if ( isI2CAvailable() == false )
    return false;
    
  I2CData data;
  CncInterface::ISP::States sp;
  if ( readI2CSlave(data) ) sp = CncInterface::ISP::States(data.supportState);
  else                      return false;
 
  switch ( idx ) {
    case 1: return sp.isSupportSwitch1Pressed();
    case 2: return sp.isSupportSwitch2Pressed();
  }
  
  return false;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::evaluateAnalogLimitPin(CncInterface::ILS::States& ls) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( isI2CAvailable() == false ) {
    ls.reset(); 
    return false;
  }

  I2CData data;
  if ( readI2CSlave(data) ) ls = CncInterface::ILS::States(data.limitState);
  else                      return false;

  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::evaluateAnalogSupportPin(CncInterface::ISP::States& sp) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( isI2CAvailable() == false )
    return false;
  
  I2CData data;
  if ( readI2CSlave(data) ) sp = CncInterface::ISP::States(data.supportState);
  else                      return false;
  
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::evaluateLimitState(const CncStepper* stepper, int32_t& limit) {
///////////////////////////////////////////////////////////////////////////////////// 
  int32_t x = LimitSwitch::LIMIT_UNKNOWN;
  int32_t y = LimitSwitch::LIMIT_UNKNOWN;
  int32_t z = LimitSwitch::LIMIT_UNKNOWN;

  bool ret = evaluateLimitStates(x, y, z);

  if      ( stepper == X )  limit = x;
  else if ( stepper == Y )  limit = y;
  else if ( stepper == Z )  limit = z;
  else                      limit = LimitSwitch::LIMIT_UNKNOWN;

  return ret;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::evaluateLimitStates(int32_t& xLimit, int32_t& yLimit, int32_t& zLimit) {
/////////////////////////////////////////////////////////////////////////////////////
  // call the steppers to read their individual pins
  xLimit = X->readLimitState(DIRECTION_UNKNOWN);
  yLimit = Y->readLimitState(DIRECTION_UNKNOWN);
  zLimit = Z->readLimitState(DIRECTION_UNKNOWN);

  // try to get a better information for unclear behaviurs
  if ( xLimit == LimitSwitch::LIMIT_UNKNOWN || yLimit == LimitSwitch::LIMIT_UNKNOWN || zLimit == LimitSwitch::LIMIT_UNKNOWN ) {
    if ( isI2CAvailable() == true ) {
      
      // read it from analog pin
      CncInterface::ILS::States ls;
      I2CData data;
      if ( readI2CSlave(data) ) {

        CncInterface::ILS::States ls(data.limitState);
        if ( ls. hasError() == false ) {
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

  return (xLimit != LimitSwitch::LIMIT_UNSET && yLimit != LimitSwitch::LIMIT_UNSET && zLimit != LimitSwitch::LIMIT_UNSET );
}
/////////////////////////////////////////////////////////////////////////////////////
// This method evaluate the limit states by the current stepper states
bool CncController::sendCurrentLimitStates(bool force) {
/////////////////////////////////////////////////////////////////////////////////////
  const int32_t x = X->getLimitState();
  const int32_t y = Y->getLimitState();
  const int32_t z = Z->getLimitState();

  // the states will be only sent if one of them is activ or they should be forced
  if ( x != LimitSwitch::LIMIT_UNSET || y != LimitSwitch::LIMIT_UNSET || z != LimitSwitch::LIMIT_UNSET || force == true )
    writeLongValues(PID_LIMIT, x, y, z);
    
  return (x != LimitSwitch::LIMIT_UNSET && y != LimitSwitch::LIMIT_UNSET && z != LimitSwitch::LIMIT_UNSET );
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::heartbeat() {
/////////////////////////////////////////////////////////////////////////////////////
  unsigned char limitState    = 0;
  unsigned char supportState  = 0;

  if ( isI2CAvailable() == true ) {
    I2CData data;
    if ( readI2CSlave(data) ) {
      limitState   = data.limitState; 
      supportState = data.supportState;
    }
    
  } else {
    int32_t x = X->readLimitState(DIRECTION_UNKNOWN);
    int32_t y = Y->readLimitState(DIRECTION_UNKNOWN);
    int32_t z = Z->readLimitState(DIRECTION_UNKNOWN);
    CncInterface::ILS::States ls(x, y, z);   
    limitState = ls.getValue();
  }
  
  sendHeartbeat(limitState, supportState);
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::idle() {
/////////////////////////////////////////////////////////////////////////////////////
  heartbeat();
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
void CncController::broadcastInterrupt() {
/////////////////////////////////////////////////////////////////////////////////////
  X->interrupt();
  Y->interrupt();
  Z->interrupt();
}
/////////////////////////////////////////////////////////////////////////////////////
void CncController::broadcastPause(bool state) {
/////////////////////////////////////////////////////////////////////////////////////
  pause = state; 
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::observeEnablePin() {
/////////////////////////////////////////////////////////////////////////////////////
  if ( isProbeMode() == OFF ) {
    
    if ( digitalRead(PIN_ENABLE) == ENABLE_STATE_OFF ) {
       errorInfo->setNextErrorInfo(E_STEPPER_NOT_ENALED);
       return false;
    }
  }
  
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::observeToolState() {
/////////////////////////////////////////////////////////////////////////////////////
  if ( isProbeMode() == OFF ) {
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
bool CncController::observeSerialFrontByte() {
/////////////////////////////////////////////////////////////////////////////////////
  unsigned char serialFrontByte = CMD_INVALID;
  
  if ( peakSerial(serialFrontByte) == true ) {
    switch ( serialFrontByte ) {
      
      // interrupt handling
      case SIG_INTERRUPPT:
                  // dont remove the signal from serial, so an explizit reset have to be called by the interface
                  broadcastInterrupt();
  
                  // Signalize an error
                  return false;
  
      case SIG_HALT:
                  // remove the signal from serial
                  Serial.read();
                  
                  // Options:
                  //  - Returning false here signalize an error and the complete run cycle (PC) stopps as a result.
                  //  - Returning true here stopps the current move (while loop), so far so good, but the current run cycle 
                  //    continue with the next existing move command which is not the meaning of HALT.
                  
                  return false;
                  
      case SIG_PAUSE:
                  // remove the signal from serial
                  Serial.read();
                  pause = PAUSE_ACTIVE;
  
                  // Don't leave the wigle loop, so break is used here
                  break;
  
      case SIG_RESUME:
                  // remove the signal from serial
                  Serial.read();
                  pause = PAUSE_INACTIVE;
  
                  // Don't leave the wigle loop, so break is used here
    }
  }

  // ----------------------------------------------------------
  // pause handling
  if ( pause == PAUSE_ACTIVE ) {
     static const short PAUSE_WAIT_DELAY = 50;
     static const short HB_MOD           = 1000 / PAUSE_WAIT_DELAY;
     
     unsigned short counter = 0;
     while ( checkSerialForPauseCommands(pause) == PAUSE_ACTIVE ) {
         
      if ( counter%HB_MOD == 0 )
        sendHeartbeat();
        
      delay(PAUSE_WAIT_DELAY);
      counter ++;
     }
     pause = PAUSE_INACTIVE;
  }
 
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
void CncController::sendCurrentPositions(unsigned char pid, bool force) {
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
    //speedManager.determineFeedSpeed();
    
    switch ( pid ) {
      case PID_X_POS: writeLongValue(pid, X->getPosition());
                      break;
                      
      case PID_Y_POS: writeLongValue(pid, Y->getPosition()); 
                      break;
                      
      case PID_Z_POS: writeLongValue(pid, Z->getPosition());
                      break;

      case PID_XYZ_POS: 
      case PID_XYZ_POS_MAJOR: 
      case PID_XYZ_POS_DETAIL: 
                      writeLongValues(pid, 
                                      X->getPosition(),
                                      Y->getPosition(),
                                      Z->getPosition(),
                                      speedController.getRealtimeFeedSpeed_MM_MIN() * DBL_FACT);
                      break;

      default:        ; // do nothing

    }

    //Serial.flush();
  }
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::stepAxisXYZ() {  
/////////////////////////////////////////////////////////////////////////////////////
  typedef RenderStruct RS;
   
  // avoid empty processing
  if ( RS::empty() )
    return true;

  // speed up the performance and observate in intervalls
  if ( positionCounter%16 == 0 ) {

    if ( observeSerialFrontByte() == false )
      return false;

    if ( observeEnablePin() == false )
      return false;
  
    if ( observeToolState() == false )
      return false;
  }
  
  incPositionCounter();

  if ( RS::dx() != 0) {
    if ( X->performNextStep() == false )
      return false;

    RS::xStepCount++;

    if ( RS::xDelay > 0 )
      delayMicroseconds(RS::xDelay);
  }
  
  if ( RS::dy() != 0 ) {
    if ( Y->performNextStep() == false )
      return false;
      
    RS::yStepCount++;
    
    if ( RS::yDelay > 0 )
      delayMicroseconds(RS::yDelay);
      
  }
    
  if ( RS::dz() != 0 ) {
    if ( Z->performNextStep() == false )
      return false;
      
    RS::zStepCount++;
    
    if ( RS::zDelay > 0 )
      delayMicroseconds(RS::zDelay);
  }

  sendCurrentPositions(PID_XYZ_POS_DETAIL, false);  
  RS::swap();
      
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::renderAndStepAxisXYZ(int32_t dx, int32_t dy, int32_t dz) {
/////////////////////////////////////////////////////////////////////////////////////
  // avoid empty processing
  if ( dx == 0 && dy == 0 && dz == 0 ) 
    return true;

  const unsigned long tsStart = micros();
  
  // initialize
  int i, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
  bool ret = false;

  x_inc = (dx < 0) ? -1 : 1;
  l     = absolute<int32_t>(dx);
  
  y_inc = (dy < 0) ? -1 : 1;
  m     = absolute<int32_t>(dy);
  
  z_inc = (dz < 0) ? -1 : 1;
  n     = absolute<int32_t>(dz);
  
  dx2 = l << 1;
  dy2 = m << 1;
  dz2 = n << 1;

  //------------------------------------------------------
  // first setup the directions - this can be done once
  // because the directions didn't switch during a call of
  // renderAndStepAxisXYZ()
  if ( X->setDirection(dx) == false )
    return false;

  if ( Y->setDirection(dy) == false )
    return false;

  if ( Z->setDirection(dz) == false )
    return false;

  typedef RenderStruct RS;
  RS::reset();
  
  //------------------------------------------------------
  if ((l >= m) && (l >= n)) {
    err_1 = dy2 - l;
    err_2 = dz2 - l;
    
    for (i = 0; i < l; i++) {

      ret = stepAxisXYZ();
      if ( ret == false ) { return false; }
      if (err_1 > 0)      { RS::A[RS::IDX_Y] += y_inc; err_1 -= dx2; }
      if (err_2 > 0)      { RS::A[RS::IDX_Z] += z_inc; err_2 -= dx2; }
      
      err_1            += dy2;
      err_2            += dz2;
      RS::A[RS::IDX_X] += x_inc;
    }

  //------------------------------------------------------  
  } else if ((m >= l) && (m >= n)) {
    err_1 = dx2 - m;
    err_2 = dz2 - m;
    
    for (i = 0; i < m; i++) {

      ret = stepAxisXYZ();
      if ( ret == false ) { return false; }
      if (err_1 > 0)      { RS::A[RS::IDX_X] += x_inc; err_1 -= dy2; }
      if (err_2 > 0)      { RS::A[RS::IDX_Z] += z_inc; err_2 -= dy2; }
      
      err_1            += dx2;
      err_2            += dz2;
      RS::A[RS::IDX_Y] += y_inc;
    }

  //------------------------------------------------------  
  } else {
    err_1 = dy2 - n;
    err_2 = dx2 - n;
    
    for (i = 0; i < n; i++) {

      ret = stepAxisXYZ();
      if ( ret == false ) { return false; }
      if (err_1 > 0)      { RS::A[RS::IDX_Y] += y_inc; err_1 -= dz2; }
      if (err_2 > 0)      { RS::A[RS::IDX_X] += x_inc; err_2 -= dz2; }
      
      err_1            += dy2;
      err_2            += dx2;
      RS::A[RS::IDX_Z] += z_inc;
    }
  }
  
  if ( stepAxisXYZ() == false )
    return false;

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
/*
  if ( isProbeMode() == OFF ) {
    speedManager.finalizeMove();
  }
*/
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
void CncController::setSpeedValue(double fm) { 
/////////////////////////////////////////////////////////////////////////////////////
  speedController.setFeedSpeed_MM_MIN(fm);  

  typedef RenderStruct RS;
  
  if ( speedController.isSpeedConfigured() ) {
    RS::xDelay = speedController.X.synthSpeedDelay;
    RS::yDelay = speedController.Y.synthSpeedDelay;
    RS::zDelay = speedController.Z.synthSpeedDelay;
  } else {
    RS::xDelay = 0;
    RS::yDelay = 0;
    RS::zDelay = 0;
  }
}

