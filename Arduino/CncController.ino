#include "CncController.h"
#include "CommonFunctions.h"
#include "CommonValues.h"

int pointA[3], pointB[3];

/////////////////////////////////////////////////////////////////////////////////////
CncController::CncController(LastErrorCodes& lec) 
/////////////////////////////////////////////////////////////////////////////////////
: X(new CncStepper(this, 'X', X_STP, X_DIR, X_LIMIT, lec))
, Y(new CncStepper(this, 'Y', Y_STP, Y_DIR, Y_LIMIT, lec))
, Z(new CncStepper(this, 'Z', Z_STP, Z_DIR, Z_LIMIT, lec))
, errorInfo(&lec)
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
	wrewr
}
/////////////////////////////////////////////////////////////////////////////////////
CncController::~CncController() {
/////////////////////////////////////////////////////////////////////////////////////
  delete X;
  delete Y;
  delete Z;
}
//////////////////////////////////////////////////////////////////////////////
bool CncController::enableStepperPin(bool state){
//////////////////////////////////////////////////////////////////////////////
  if ( probeMode == false )   digitalWrite(ENABLE_PIN, state == true ? LOW : HIGH);
  else                        digitalWrite(ENABLE_PIN, HIGH);

  delayMicroseconds(minEnablePulseWide);
  return state;
}
/////////////////////////////////////////////////////////////////////////////////////
void CncController::printConfig() {
/////////////////////////////////////////////////////////////////////////////////////
  Serial.print(PID_CONTROLLER); Serial.print(TEXT_SEPARATOR); Serial.write(TEXT_CLOSE);
    
    Serial.print(BLANK); Serial.print(PID_POS_REPLY_THRESHOLD_X);          Serial.print(TEXT_SEPARATOR); Serial.print(controller.getPosReplyThresholdX()); Serial.write(TEXT_CLOSE);
    Serial.print(BLANK); Serial.print(PID_POS_REPLY_THRESHOLD_Y);          Serial.print(TEXT_SEPARATOR); Serial.print(controller.getPosReplyThresholdY()); Serial.write(TEXT_CLOSE);
    Serial.print(BLANK); Serial.print(PID_POS_REPLY_THRESHOLD_Z);          Serial.print(TEXT_SEPARATOR); Serial.print(controller.getPosReplyThresholdZ()); Serial.write(TEXT_CLOSE);
    Serial.print(BLANK); Serial.print(PID_PROBE_MODE);                     Serial.print(TEXT_SEPARATOR); Serial.print(controller.isProbeMode());           Serial.write(TEXT_CLOSE);
    Serial.print(BLANK); Serial.print(PID_ENABLE_STEPPERS);                Serial.print(TEXT_SEPARATOR); Serial.print(!digitalRead(ENABLE_PIN));           Serial.write(TEXT_CLOSE);
    Serial.print(BLANK); Serial.print(PID_MIN_ENABLE_PULSE_WIDTH);         Serial.print(TEXT_SEPARATOR); Serial.print(minEnablePulseWide);                 Serial.write(TEXT_CLOSE);
  
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
                      writeLongValues(pid, x, y, z);
                      lastSendPositionX = x; 
                      lastSendPositionY = y; 
                      lastSendPositionZ = z; 
					  break;

      default:        ; // do nothing
    }
  }
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::evaluateAndSendLimitStates() {
/////////////////////////////////////////////////////////////////////////////////////
  long x = X->readLimitState(0);
  long y = Y->readLimitState(0);
  long z = Z->readLimitState(0);

  writeLongValues(PID_LIMIT, x, y, z);
  return (x != 0 && y != 0 && z != 0 );
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::sendCurrentLimitStates(bool force) {
/////////////////////////////////////////////////////////////////////////////////////
  long x = X->getLimitState();
  long y = Y->getLimitState();
  long z = Z->getLimitState();

  // the states will be only sent if one of them is activ or they should be forced
  if ( x != 0 || y != 0 || z != 0 || force == true )
    writeLongValues(PID_LIMIT, x, y, z);
    
  return (x != 0 && y != 0 && z != 0 );
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
  
  // optimize pure horizontal moves
  if ( y1 == 0 )
    return stepAxisX(x1);

  // optimize pure vertical moves
  if ( x1 == 0 )
    return stepAxisY(y1);

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
/////////////////////////////////////////////////////////////////////////////////////
bool CncController::stepAxisXYZ(long x1, long y1, long z1, bool render){
/////////////////////////////////////////////////////////////////////////////////////
  bool ret = true;
  
  if ( render == true ) {
    // move command = 'M'
    
    ret = renderAndStepAxisXYZ(x1, y1, z1);
    
  } else {
    // move command = 'm'

    if ( ret == true ) {  
      if ( stepAxisX(x1) == false )
        ret = false;
    }
      
    if ( ret == true ) {  
      if ( stepAxisY(y1) == false )
        ret = false;
    }

    if ( ret == true ) {  
      if ( stepAxisZ(z1) == false )
        ret = false;
    }
  }

  return ret;
}


