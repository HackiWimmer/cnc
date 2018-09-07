#include "CncController.h"
#include "CommonFunctions.h"
#include "CommonValues.h"

int pointA[3], pointB[3];

/////////////////////////////////////////////////////////////////////////////////////
CncController::CncController(const unsigned char alp, 
                             const unsigned char asp, 
                             LastErrorCodes& lec) 
/////////////////////////////////////////////////////////////////////////////////////
: X(new CncStepper(this, 'X', PIN_X_STP, PIN_X_DIR, PIN_X_LIMIT, lec))
, Y(new CncStepper(this, 'Y', PIN_Y_STP, PIN_Y_DIR, PIN_Y_LIMIT, lec))
, Z(new CncStepper(this, 'Z', PIN_Z_STP, PIN_Z_DIR, PIN_Z_LIMIT, lec))
, speedManager()
, errorInfo(&lec)
, analogLimitPin(alp)
, analogSupportPin(asp)
, posReplyThresholdX(100L)
, posReplyThresholdY(100L)
, posReplyThresholdZ(100L)
, lastSendPositionX(0L)
, lastSendPositionY(0L)
, lastSendPositionZ(0L)
, positionCounter(MIN_LONG)
, positionCounterOverflow(0L)
, posReplyState(false)
, probeMode(false)
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
long CncController::isReadyToRun() {
/////////////////////////////////////////////////////////////////////////////////////
  if ( speedManager.isInitialized() == false ) { errorInfo->setNextErrorInfo(E_SPEED_MGMT_NOT_INITIALIZED, BLANK);  return 0L; }
  if ( X->isReadyToRun() == 0L )               { errorInfo->setNextErrorInfo(E_STEPPER_NOT_READY_TO_RUN, "X");      return 0L; }
  if ( Y->isReadyToRun() == 0L )               { errorInfo->setNextErrorInfo(E_STEPPER_NOT_READY_TO_RUN, "Y");      return 0L; }
  if ( Z->isReadyToRun() == 0L )               { errorInfo->setNextErrorInfo(E_STEPPER_NOT_READY_TO_RUN, "Z");      return 0L; }
  
  return 1L;
}
////////////////////////////////////////////////////////////////////////////////////
void CncController::setupSpeedManager() {
/////////////////////////////////////////////////////////////////////////////////////
  speedManager.setup( 40,
                      X->getPitch(), X->getSteps(), X->getPulseWidthOffset() * 2,
                      Y->getPitch(), Y->getSteps(), Y->getPulseWidthOffset() * 2,
                      Z->getPitch(), Z->getSteps(), Z->getPulseWidthOffset() * 2);
}
//////////////////////////////////////////////////////////////////////////////
unsigned int CncController::getPerStepSpeedOffset(char axis) { 
//////////////////////////////////////////////////////////////////////////////
  switch ( axis ) {
    case 'X': return speedManager.getOffsetPerStepX();
    case 'Y': return speedManager.getOffsetPerStepX();
    case 'Z': return speedManager.getOffsetPerStepZ();
  }

  return 1000;
  //return MAX_UINT;
}
//////////////////////////////////////////////////////////////////////////////
unsigned int CncController::getLowPulseWidth(char axis){
//////////////////////////////////////////////////////////////////////////////
  switch ( axis ) {
    case 'X': return speedManager.getLowPulseWidthX(); 
    case 'Y': return speedManager.getLowPulseWidthY();
    case 'Z': return speedManager.getLowPulseWidthZ();
  }
   
  return 1000;
}
//////////////////////////////////////////////////////////////////////////////
unsigned int CncController::getHighPulseWidth(char axis) {
//////////////////////////////////////////////////////////////////////////////
  switch ( axis ) {
    case 'X': return speedManager.getHighPulseWidthX();
    case 'Y': return speedManager.getHighPulseWidthY();
    case 'Z': return speedManager.getHighPulseWidthZ();
  }
   
  return 1000;
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
    PRINT_PARAMETER(PID_MIN_ENABLE_PULSE_WIDTH,           minEnablePulseWide)
    PRINT_PARAMETER(PID_ANALOG_LIMIT_PIN,                 analogLimitPin)
    PRINT_PARAMETER(PID_ANALOG_SUPPORT_PIN,               analogSupportPin)
    PRINT_PARAMETER(PID_SPEED_MGMT_INITIALIZED,           speedManager.isInitialized())

  #undef PRINT_PARAMETER
    
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

    lastSendPositionX = 0L;
    lastSendPositionY = 0L;
    lastSendPositionZ = 0L;    
    
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
void CncController::sendCurrentPositions(unsigned char pid, bool force) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( posReplyState == false )
    return;

  long x = X->getPosition();  
  long y = Y->getPosition();  
  long z = Z->getPosition();  

  long speedValue = speedManager.getMeasurementFeedSpeed() * DBL_FACT;
  
  if ( absolute(x - lastSendPositionX) >= posReplyThresholdX ||
       absolute(y - lastSendPositionY) >= posReplyThresholdY ||
       absolute(z - lastSendPositionZ) >= posReplyThresholdZ ||
       force == true ) {

    switch ( pid ) {
      case PID_X_POS: writeLongValue(pid, x);
                      lastSendPositionX = x;
                      break;
                      
      case PID_Y_POS: writeLongValue(pid, y); 
                      lastSendPositionY = y;
                      break;
                      
      case PID_Z_POS: writeLongValue(pid, z);
                      lastSendPositionZ = z; 
                      break;

      case PID_XYZ_POS: 
      case PID_XYZ_POS_MAJOR: 
      case PID_XYZ_POS_DETAIL: 
                      writeLongValues(pid, x, y, z, speedValue);
                      lastSendPositionX = x; 
                      lastSendPositionY = y; 
                      lastSendPositionZ = z; 
					  break;

      default:        ; // do nothing
    }
  }
}
/////////////////////////////////////////////////////////////////////////////////////  
bool CncController::evaluateToolState() {
/////////////////////////////////////////////////////////////////////////////////////  
  return ( digitalRead(PIN_TOOL_FEEDBACK) == TOOL_STATE_ON );
}
/////////////////////////////////////////////////////////////////////////////////////  
bool CncController::evaluateSupportButton1State(unsigned short idx) {
// true:  button pressed
// false: button released
/////////////////////////////////////////////////////////////////////////////////////  
  if ( isAnalogSupportPinAvailable() == false )
    return false;

  const int msDelay = 5;

  CncInterface::ISP::States sp;
  sp.readAnalogValue(analogSupportPin, msDelay);

  switch ( idx ) {
    case 1: return sp.isSupportButton1Pressed();
    case 2: return sp.isSupportButton1Pressed();
    case 3: return sp.isSupportButton1Pressed();
  }
  
  return false;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::evaluateAnalogLimitPin(CncInterface::ILS::States& ls) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( isAnalogLimitPinAvailable() == false ) {
    ls.reset(); 
    return false;
  }

  const int msDelay = 5;
  ls.readAnalogValue(analogLimitPin, msDelay);
  
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::evaluateAnalogSupportPin(CncInterface::ISP::States& sp) {
/////////////////////////////////////////////////////////////////////////////////////
  if ( isAnalogSupportPinAvailable() == false ) {
    return false;
  }

  const int msDelay = 5;
  sp.readAnalogValue(analogSupportPin, msDelay);
  
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::evaluateLimitState(const CncStepper* stepper, long& limit) {
///////////////////////////////////////////////////////////////////////////////////// 
  long x = LimitSwitch::LIMIT_UNKNOWN;
  long y = LimitSwitch::LIMIT_UNKNOWN;
  long z = LimitSwitch::LIMIT_UNKNOWN;

  bool ret = evaluateLimitStates(x, y, z);

  if      ( stepper == X )  limit = x;
  else if ( stepper == Y )  limit = y;
  else if ( stepper == Z )  limit = z;
  else                      limit = LimitSwitch::LIMIT_UNKNOWN;

  return ret;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::evaluateLimitStates(long& xLimit, long& yLimit, long& zLimit) {
/////////////////////////////////////////////////////////////////////////////////////
  // call the steppers to read their individual pins
  xLimit = X->readLimitState(DIRECTION_UNKNOWN);
  yLimit = Y->readLimitState(DIRECTION_UNKNOWN);
  zLimit = Z->readLimitState(DIRECTION_UNKNOWN);

  // try to get a better information for unclear behaviurs
  if ( xLimit == LimitSwitch::LIMIT_UNKNOWN || yLimit == LimitSwitch::LIMIT_UNKNOWN || zLimit == LimitSwitch::LIMIT_UNKNOWN ) {
    if ( isAnalogLimitPinAvailable() == true ) {
      
      // read it from analog pin
      CncInterface::ILS::States ls;
      evaluateAnalogLimitPin(ls);

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

  return (xLimit != LimitSwitch::LIMIT_UNSET && yLimit != LimitSwitch::LIMIT_UNSET && zLimit != LimitSwitch::LIMIT_UNSET );
}
/////////////////////////////////////////////////////////////////////////////////////
// This method evaluate the limit states by reading the corresponding pins
bool CncController::evaluateAndSendStates() {
/////////////////////////////////////////////////////////////////////////////////////

  if ( isAnalogLimitPinAvailable() == false) {
    long x = LimitSwitch::LIMIT_UNKNOWN;
    long y = LimitSwitch::LIMIT_UNKNOWN;
    long z = LimitSwitch::LIMIT_UNKNOWN;
  
    bool ret = evaluateLimitStates(x, y, z);
    writeLongValues(PID_LIMIT, x, y, z);
    return ret;
  }

  CncInterface::ILS::States ls;
  evaluateAnalogLimitPin(ls);

  if ( isAnalogSupportPinAvailable() == false ) {
    sendHeartbeat(ls.getValue());
    return true;
  }

  CncInterface::ISP::States sp;
  evaluateAnalogSupportPin(sp);
  
  sendHeartbeat(ls.getValue(), sp.getValue());
  
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
// This method evaluate the limit states by the current stepper states
bool CncController::sendCurrentLimitStates(bool force) {
/////////////////////////////////////////////////////////////////////////////////////
  long x = X->getLimitState();
  long y = Y->getLimitState();
  long z = Z->getLimitState();

  // the states will be only sent if one of them is activ or they should be forced
  if ( x != LimitSwitch::LIMIT_UNSET || y != LimitSwitch::LIMIT_UNSET || z != LimitSwitch::LIMIT_UNSET || force == true )
    writeLongValues(PID_LIMIT, x, y, z);
    
  return (x != LimitSwitch::LIMIT_UNSET && y != LimitSwitch::LIMIT_UNSET && z != LimitSwitch::LIMIT_UNSET );
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
  X->pause(state);
  Y->pause(state);
  Z->pause(state); 
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::stepAxisX(long x){
/////////////////////////////////////////////////////////////////////////////////////
  return X->stepAxis(x);
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::stepAxisY(long y){
/////////////////////////////////////////////////////////////////////////////////////
  return Y->stepAxis(y);
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::stepAxisZ(long z){
/////////////////////////////////////////////////////////////////////////////////////
  return Z->stepAxis(z);
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::renderAndStepAxisXY(long x1, long y1) {
/////////////////////////////////////////////////////////////////////////////////////
  //avoid empty steps
  if ( x1 == 0 && y1 == 0)
    return true;
  
  long x0 = 0, y0 = 0;
  long dx =  absolute(x1-x0), sx = x0<x1 ? 1 : -1;
  long dy = -absolute(y1-y0), sy = y0<y1 ? 1 : -1;
  long err = dx+dy, e2; // error value e_xy 

  long xOld = x0, yOld = y0;
  while( true ) {

    if ( X->stepAxis(x0 - xOld) == false )
      return false;

    if ( Y->stepAxis(y0 - yOld) == false )
      return false;
      
    xOld = x0; 
    yOld = y0;
    
    if (x0==x1 && y0==y1) break;
    e2 = 2*err;
    if (e2 > dy) { err += dy; x0 += sx; } // e_xy+e_x > 0
    if (e2 < dx) { err += dx; y0 += sy; } // e_xy+e_y < 0
  }

  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::moveXYZ() {  
/////////////////////////////////////////////////////////////////////////////////////
  incPositionCounter();
  
  if ( X->stepAxis(pointA[0] - pointB[0]) == false )
    return false;
  
  if ( Y->stepAxis(pointA[1] - pointB[1]) == false )
    return false;
  
  if ( Z->stepAxis(pointA[2] - pointB[2]) == false )
    return false;

  sendCurrentPositions(PID_XYZ_POS_DETAIL, false);   

  // copy point A into point B
  memcpy(&pointB, &pointA, sizeof(pointA));
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::renderAndStepAxisXYZ(long dx, long dy, long dz) {
/////////////////////////////////////////////////////////////////////////////////////
  // update speed manager values
  if ( isProbeMode() == false )
    speedManager.setNextMove(dx, dy, dz);
  
  // initialize
  int i, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
  memset(&pointA, 0, sizeof(pointA));
  memset(&pointB, 0, sizeof(pointB));

  x_inc = (dx < 0) ? -1 : 1;
  l = absolute(dx);
  
  y_inc = (dy < 0) ? -1 : 1;
  m = absolute(dy);
  
  z_inc = (dz < 0) ? -1 : 1;
  n = absolute(dz);
  
  dx2 = l << 1;
  dy2 = m << 1;
  dz2 = n << 1;

  //------------------------------------------------------
  if ((l >= m) && (l >= n)) {
    err_1 = dy2 - l;
    err_2 = dz2 - l;
    
    for (i = 0; i < l; i++) {
      
      //output
      if ( moveXYZ() == false )
        return false;
      
      if (err_1 > 0) {
        pointA[1] += y_inc;
        err_1 -= dx2;
      }
      
      if (err_2 > 0) {
        pointA[2] += z_inc;
        err_2 -= dx2;
      }
      
      err_1 += dy2;
      err_2 += dz2;
      pointA[0] += x_inc;
    }

  //------------------------------------------------------  
  } else if ((m >= l) && (m >= n)) {
    err_1 = dx2 - m;
    err_2 = dz2 - m;
    
    for (i = 0; i < m; i++) {
      
      //output
      if ( moveXYZ() == false )
        return false;
    
      for (int j=0; j<3; j++ )
        pointB[j] = pointA[j];
        
      if (err_1 > 0) {
        pointA[0] += x_inc;
        err_1 -= dy2;
      }
      
      if (err_2 > 0) {
        pointA[2] += z_inc;
        err_2 -= dy2;
      }
      
      err_1 += dx2;
      err_2 += dz2;
      pointA[1] += y_inc;
    }

  //------------------------------------------------------  
  } else {
    err_1 = dy2 - n;
    err_2 = dx2 - n;
    
    for (i = 0; i < n; i++) {
      
      //output
      if ( moveXYZ() == false )
        return false;
    
      for (int j=0; j<3; j++ )
        pointB[j] = pointA[j];
        
      if (err_1 > 0) {
        pointA[1] += y_inc;
        err_1 -= dz2;
      }
      
      if (err_2 > 0) {
        pointA[0] += x_inc;
        err_2 -= dz2;
      }
      
      err_1 += dy2;
      err_2 += dx2;
      pointA[2] += z_inc;
    }
  }
  
  //output
  if ( moveXYZ() == false )
    return false;

  return true;
}

