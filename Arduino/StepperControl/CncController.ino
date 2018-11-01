#include "CncController.h"
#include "CommonFunctions.h"
#include "CommonValues.h"

/////////////////////////////////////////////////////////////////////////////////////
struct RenderStruct {

  static PwmProfile pwmPX, pwmPY, pwmPZ;
  
  static const int POINT_LENGTH = 3;
  static const int IDX_X        = 0;
  static const int IDX_Y        = 1;
  static const int IDX_Z        = 2;
  
  static int32_t A[POINT_LENGTH];
  static int32_t B[POINT_LENGTH];

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

/////////////////////////////////////////////////////////////////////////////////////
int32_t        RenderStruct::A[RenderStruct::POINT_LENGTH] = {0,0,0};
int32_t        RenderStruct::B[RenderStruct::POINT_LENGTH] = {0,0,0};

PwmProfile     RenderStruct::pwmPX; 
PwmProfile     RenderStruct::pwmPY; 
PwmProfile     RenderStruct::pwmPZ;

unsigned short RenderStruct::xStepCount                    = 0;
unsigned short RenderStruct::yStepCount                    = 0;
unsigned short RenderStruct::zStepCount                    = 0;
/////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////
CncController::CncController() 
/////////////////////////////////////////////////////////////////////////////////////
: X(new CncStepper(this, RenderStruct::pwmPX, 'X', PIN_X_STP, PIN_X_DIR, PIN_X_LIMIT))
, Y(new CncStepper(this, RenderStruct::pwmPY, 'Y', PIN_Y_STP, PIN_Y_DIR, PIN_Y_LIMIT))
, Z(new CncStepper(this, RenderStruct::pwmPZ, 'Z', PIN_Z_STP, PIN_Z_DIR, PIN_Z_LIMIT))
, speedController()
, posReplyThresholdX(100L)
, posReplyThresholdY(100L)
, posReplyThresholdZ(100L)
, positionCounter(MIN_LONG)
, positionCounterOverflow(0L)
, posReplyState(OFF)
, probeMode(OFF)
, pause(PAUSE_INACTIVE)
, I2CAvailable(false)
, lastHeartbeat(0)
, lastI2CData()
{}
/////////////////////////////////////////////////////////////////////////////////////
CncController::~CncController() {
/////////////////////////////////////////////////////////////////////////////////////
  delete X;
  delete Y;
  delete Z;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::evaluateI2CAvailable() {
/////////////////////////////////////////////////////////////////////////////////////
  // try to request data
  I2CAvailable = readI2CSlave(lastI2CData);

  // On demand try a second one
  if ( I2CAvailable == false ) {
    delay(1000);
    I2CAvailable = readI2CSlave(lastI2CData);
  }

  return I2CAvailable;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::evaluateI2CData() {
/////////////////////////////////////////////////////////////////////////////////////
  if ( isI2CAvailable() == false )
    return false;
  
  // request data
  return readI2CSlave(lastI2CData);
}
/////////////////////////////////////////////////////////////////////////////////////
int32_t CncController::isReadyToRun() {
/////////////////////////////////////////////////////////////////////////////////////
  if ( speedController.isInitialized() == false )   { pushErrorMessage(E_SPEED_MGMT_NOT_INITIALIZED);       return 0L; }
  if ( X->isReadyToRun() == 0L )                    { pushErrorMessage(E_STEPPER_NOT_READY_TO_RUN, "X");    return 0L; }
  if ( Y->isReadyToRun() == 0L )                    { pushErrorMessage(E_STEPPER_NOT_READY_TO_RUN, "Y");    return 0L; }
  if ( Z->isReadyToRun() == 0L )                    { pushErrorMessage(E_STEPPER_NOT_READY_TO_RUN, "Z");    return 0L; }
  
  return 1L;
}
////////////////////////////////////////////////////////////////////////////////////
void CncController::setupSpeedController() {
/////////////////////////////////////////////////////////////////////////////////////
  typedef RenderStruct RS;
  
  speedController.setup('X', X->getSteps(), X->getPitch(), 
                        SPEED_MANAGER_CONST_STATIC_OFFSET_US, SPEED_MANAGER_CONST_LOOP_OFFSET_US, X->getLowPulseWidth() + X->getHighPulseWidth(),
                        RS::pwmPX.startSpeed_MM_SEC, RS::pwmPX.stopSpeed_MM_SEC
                       );
                       
  speedController.setup('Y', Y->getSteps(), Y->getPitch(), 
                        SPEED_MANAGER_CONST_STATIC_OFFSET_US, SPEED_MANAGER_CONST_LOOP_OFFSET_US, Y->getLowPulseWidth() + Y->getHighPulseWidth(),
                        RS::pwmPY.startSpeed_MM_SEC, RS::pwmPY.stopSpeed_MM_SEC
                       );
                       
  speedController.setup('Z', Z->getSteps(), Z->getPitch(), 
                        SPEED_MANAGER_CONST_STATIC_OFFSET_US, SPEED_MANAGER_CONST_LOOP_OFFSET_US, Z->getLowPulseWidth() + Z->getHighPulseWidth(),
                        RS::pwmPZ.startSpeed_MM_SEC, RS::pwmPZ.stopSpeed_MM_SEC
                       );
}
/////////////////////////////////////////////////////////////////////////////////////
void CncController::setupAccelProfile(int32_t v1, int32_t v2, int32_t v3, int32_t v4, int32_t v5, int32_t v6) {
/////////////////////////////////////////////////////////////////////////////////////
  typedef RenderStruct RS;
  
  RS::pwmPX.startSpeed_MM_SEC = v1;
  RS::pwmPX.stopSpeed_MM_SEC  = v2;
  RS::pwmPY.startSpeed_MM_SEC = v3;
  RS::pwmPY.stopSpeed_MM_SEC  = v4;
  RS::pwmPZ.startSpeed_MM_SEC = v5;
  RS::pwmPZ.stopSpeed_MM_SEC  = v6;
}
//////////////////////////////////////////////////////////////////////////////
bool CncController::enableStepperPin(bool state){
//////////////////////////////////////////////////////////////////////////////
  if ( probeMode == false )   digitalWrite(PIN_STEPPER_ENABLE, state == true ? ENABLE_STATE_ON : ENABLE_STATE_OFF);
  else                        digitalWrite(PIN_STEPPER_ENABLE, ENABLE_STATE_OFF);

  delayMicroseconds(10);
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
    PRINT_PARAMETER(PID_ENABLE_STEPPERS,                  digitalRead(PIN_STEPPER_ENABLE) == ENABLE_STATE_ON)
    PRINT_PARAMETER(PID_TOOL_SWITCH,                      digitalRead(PIN_TOOL_ENABLE)    == TOOL_STATE_ON)
    PRINT_PARAMETER(PID_I2C_AVAILABEL,                    isI2CAvailable())

    int limitState = -1, supportState = -1;
    if ( isI2CAvailable() == true ) {
      if ( readI2CSlave(lastI2CData) ) {
        limitState   = (int)lastI2CData.limitState;
        supportState = (int)lastI2CData.supportState;
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
    
  CncInterface::ISP::States sp;
  if ( readI2CSlave(lastI2CData) )  sp = CncInterface::ISP::States(lastI2CData.supportState);
  else                              return false;
 
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
    
  CncInterface::ISP::States sp;
  if ( readI2CSlave(lastI2CData) )  sp = CncInterface::ISP::States(lastI2CData.supportState);
  else                              return false;
 
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

  if ( readI2CSlave(lastI2CData) )  ls = CncInterface::ILS::States(lastI2CData.limitState);
  else                              return false;

  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::evaluateAnalogSupportPin(CncInterface::ISP::States& sp) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( isI2CAvailable() == false )
    return false;
  
  if ( readI2CSlave(lastI2CData) )  sp = CncInterface::ISP::States(lastI2CData.supportState);
  else                              return false;
  
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::isAnyLimitActive() {
/////////////////////////////////////////////////////////////////////////////////////
  int32_t xLimit, yLimit, zLimit;
  return evaluateLimitStates(xLimit, yLimit, zLimit);
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
      if ( readI2CSlave(lastI2CData) ) {

        CncInterface::ILS::States ls(lastI2CData.limitState);
        if ( ls.hasError() == false ) {
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
    if ( readI2CSlave(lastI2CData) ) {
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
void CncController::switchToolState(bool state, bool force) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( force == false ) {
    if ( isProbeMode() == true ) {
      digitalWrite(PIN_TOOL_ENABLE, TOOL_STATE_OFF);
      return;
    }
  }
  
  digitalWrite(PIN_TOOL_ENABLE, state);

  // give the tool a portion of time to run properly
  if ( state == TOOL_STATE_ON )
    delay(500);
}
/////////////////////////////////////////////////////////////////////////////////////
void CncController::switchStepperState(bool state) {
/////////////////////////////////////////////////////////////////////////////////////
  digitalWrite(PIN_STEPPER_ENABLE,  state);
}
/////////////////////////////////////////////////////////////////////////////////////
void CncController::broadcastPause(bool state) {
/////////////////////////////////////////////////////////////////////////////////////
  pause = state; 
  switchToolState( pause == PAUSE_ACTIVE ? TOOL_STATE_OFF : TOOL_STATE_ON, FORCE);
}
/////////////////////////////////////////////////////////////////////////////////////
void CncController::broadcastHalt() {
/////////////////////////////////////////////////////////////////////////////////////
  switchToolState(TOOL_STATE_OFF, FORCE);
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::observeEnablePin() {
/////////////////////////////////////////////////////////////////////////////////////
  if ( isProbeMode() == OFF ) {
    
    if ( digitalRead(PIN_STEPPER_ENABLE) == ENABLE_STATE_OFF ) {
       pushErrorMessage(E_STEPPER_NOT_ENALED); 
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
 * TODO
    if ( digitalRead(PIN_TOOL_FEEDBACK) == TOOL_STATE_OFF ) {
      errorInfo->setNextErrorInfo(E_TOOL_NOT_ENALED);
      return false;
    }
    */
  }
  
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::observeSerialFrontByte(unsigned char& retValue) {
/////////////////////////////////////////////////////////////////////////////////////
  unsigned char serialFrontByte = CMD_INVALID;
  
  if ( peakSerial(serialFrontByte) == true ) {
    switch ( serialFrontByte ) {

      case SIG_SOFTWARE_RESET:
                  softwareReset();   
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
                  broadcastHalt();
                  
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
  }

  // ----------------------------------------------------------
  // pause handling
  if ( pause == PAUSE_ACTIVE ) {

     broadcastPause(pause);
    
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
     broadcastPause(pause);
  }
  
  retValue = RET_OK;
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
unsigned char CncController::stepAxisXYZ() {  
/////////////////////////////////////////////////////////////////////////////////////
  typedef RenderStruct RS;

  // if stepAxisXYZ() is called from renderAndStepAxisXYZ - normal processing -
  // RS conatins dx, dy and dz with in range [-1, 0, +1] only
   
  // avoid empty processing
  if ( RS::empty() )
    return RET_OK;

  unsigned char retValue = RET_ERROR;

  // speed up the performance and observate in intervalls
  // blind flying: e.g. 16 * 0,015 (12 mm / 800 steps) = 0,24 mm
  if ( positionCounter%1 == 0 ) {
    if ( observeSerialFrontByte(retValue) == false )
      return retValue;

    if ( observeEnablePin() == false )
      return RET_ERROR;
  
    if ( observeToolState() == false )
      return RET_ERROR;
  }
  
  incPositionCounter();

  if ( RS::dx() != 0) {

    RS::pwmPX.accelDelay = speedController.getNextAccelDelayX();
    
    if ( (retValue = X->performNextStep()) != RET_OK ) 
      return retValue;

    RS::xStepCount++;
  }
  
  if ( RS::dy() != 0 ) {
    
     RS::pwmPY.accelDelay = speedController.getNextAccelDelayY();
     
     if ( (retValue = Y->performNextStep()) != RET_OK )
      return retValue;
      
    RS::yStepCount++;
  }
    
  if ( RS::dz() != 0 ) {
    
     RS::pwmPZ.accelDelay = speedController.getNextAccelDelayZ();
     
     if ( (retValue = Z->performNextStep()) != RET_OK )
      return retValue;
      
    RS::zStepCount++;
  }

  sendCurrentPositions(PID_XYZ_POS_DETAIL, false);  
  RS::swap();
      
  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
unsigned char CncController::renderAndStepAxisXYZ(int32_t dx, int32_t dy, int32_t dz) {
/////////////////////////////////////////////////////////////////////////////////////
  // avoid empty processing
  if ( dx == 0 && dy == 0 && dz == 0 ) 
    return RET_OK;

  //const unsigned long tsStart = micros();
  const unsigned long tsStart = speedController.getTimeStamp();
  
  if ( isProbeMode() == OFF )
    speedController.initMove(dx, dy, dz);
  
  // initialize
  int32_t i, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
  unsigned char ret = RET_ERROR;

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
    return RET_ERROR;

  if ( Y->setDirection(dy) == false )
    return RET_ERROR;

  if ( Z->setDirection(dz) == false )
    return RET_ERROR;

  typedef RenderStruct RS;
  RS::reset();
  
  //------------------------------------------------------
  if ((l >= m) && (l >= n)) {
    err_1 = dy2 - l;
    err_2 = dz2 - l;
    
    for (i = 0; i < l; i++) {

      ret = stepAxisXYZ();
      if ( ret != RET_OK ) { return ret; }
      if (err_1 > 0)       { RS::A[RS::IDX_Y] += y_inc; err_1 -= dx2; }
      if (err_2 > 0)       { RS::A[RS::IDX_Z] += z_inc; err_2 -= dx2; }
      
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
      if ( ret != RET_OK ) { return ret; }
      if (err_1 > 0)       { RS::A[RS::IDX_X] += x_inc; err_1 -= dy2; }
      if (err_2 > 0)       { RS::A[RS::IDX_Z] += z_inc; err_2 -= dy2; }
      
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
      if ( ret != RET_OK ) { return ret; }
      if (err_1 > 0)       { RS::A[RS::IDX_Y] += y_inc; err_1 -= dz2; }
      if (err_2 > 0)       { RS::A[RS::IDX_X] += x_inc; err_2 -= dz2; }
      
      err_1            += dy2;
      err_2            += dx2;
      RS::A[RS::IDX_Z] += z_inc;
    }
  }
  
  if ( (ret = stepAxisXYZ()) != RET_OK )
    return ret;

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

  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
unsigned char CncController::moveUntilSignal(const int32_t dx, const int32_t dy, const int32_t dz) {
/////////////////////////////////////////////////////////////////////////////////////
  // speed setup
  const double START_SPEED = speedController.getStartSpeed_MM_MIN() * 0.5;
  const double MAX_SPEED   = speedController.getMaxFeedSpeed_MM_MIN();
  const double DIFF_SPEED  = MAX_SPEED - START_SPEED;

  const unsigned short accelPeriod = 2500; // ms

  if ( DIFF_SPEED < 0.0 )
    return RET_ERROR;
    
  setSpeedValue(START_SPEED);
  
  unsigned char ret = RET_OK;
  unsigned long tsStart = millis();

  speedController.enableAccelerationXYZ(false);
  
  if ( dx != 0 || dy != 0 || dz !=0 ) {
    while ( (ret = renderAndStepAxisXYZ(dx, dy, dz)) == RET_OK ) {
  
      // break by interrupt
      if (    X->isInterrupted() 
           || Y->isInterrupted() 
           || Z->isInterrupted() 
         )
      { 
        ret = RET_INTERRUPT; 
        break;
      }

      // break by limit
      if (    X->getLimitState() != LimitSwitch::LIMIT_UNSET 
           || Y->getLimitState() != LimitSwitch::LIMIT_UNSET 
           || Z->getLimitState() != LimitSwitch::LIMIT_UNSET 
         )
      { 
        ret =  RET_LIMIT; 
        break;
      }

      // mormally this loop will be broken by signals
      // like SIG_QUIT, or SIG_HALT and renderAndStepAxisXYZ()
      // return != RET_OK

      unsigned diff = millis() - tsStart;
      if ( diff > accelPeriod ) {
        setSpeedValue(MAX_SPEED, false);
        
      } else {
        setSpeedValue(START_SPEED + DIFF_SPEED / accelPeriod * diff, false);
        
      }
    }
  }

  speedController.enableAccelerationXYZ(true);
  return ret;
 
}
/////////////////////////////////////////////////////////////////////////////////////
void CncController::setSpeedValue(double fm, bool activateAcceleration) { 
/////////////////////////////////////////////////////////////////////////////////////
  speedController.setFeedSpeed_MM_MIN(fm);  

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
}


