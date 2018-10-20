#include <SoftwareSerial.h>
#include "CommonFunctions.h"
#include "CncStepper.h"

//////////////////////////////////////////////////////////////////////////////
CncStepper::CncStepper(CncController* crtl, PwmProfile& pwm, char a, byte stpPin, byte dirPin, byte lmtPin)
//////////////////////////////////////////////////////////////////////////////
: INCREMENT_DIRECTION_VALUE(NORMALIZED_INCREMENT_DIRECTION_VALUE)
, pwmProfile(pwm)
, interrupted(false)
, calculateDuration(false)
, minReached(false)
, maxReached(false)
, minLimitCnt(0)
, maxLimitCnt(0)
, directionPin(dirPin)
, stepPin(stpPin)
, limitPin(lmtPin)
, axis(a)
, dirPulseWidth(10)
, lowPulsWidth(500)
, highPulsWidth(500)
, minPulsWidth(highPulsWidth + lowPulsWidth)
, tsPrevStep(0L)
, tsCurrStep(0L)
, pitch(1.0)
, steps(400)
, avgStepDuartion(0L)
, stepDirection(SD_UNKNOWN)
, stepCounter(0L)
, stepCounterOverflow(0L)
, curPos(0L)
, posReplyThresholdCount(0L)
, controller(crtl)
{
}
//////////////////////////////////////////////////////////////////////////////
CncStepper::~CncStepper() {
//////////////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////////////////////
int32_t CncStepper::isReadyToRun() {
/////////////////////////////////////////////////////////////////////////////////////
  int32_t ret = 1;

  if ( interrupted == true ) {
    pushErrorMessage(E_INTERRUPT);
    ret = 0;
  }

  if ( readLimitState() != LimitSwitch::LIMIT_UNSET ) {
    pushErrorMessage(E_LIMIT_SWITCH_ACTIVE);
    ret = 0;
  }
  
  return ret;  
}
//////////////////////////////////////////////////////////////////////////////
void CncStepper::printConfig() {
//////////////////////////////////////////////////////////////////////////////
  unsigned char pidIncrementDirectionValue = PID_UNKNOWN;
  
  switch ( axis ) {
    case 'X': pidIncrementDirectionValue = PID_INCREMENT_DIRECTION_VALUE_X; 
              break;
    case 'Y': pidIncrementDirectionValue = PID_INCREMENT_DIRECTION_VALUE_Y; 
              break;
    case 'Z': pidIncrementDirectionValue = PID_INCREMENT_DIRECTION_VALUE_Z; 
              break;
  }

  #define PRINT_PARAMETER( Pid, value ) \
    Serial.print(BLANK); \
    Serial.print(BLANK); \
    Serial.print(Pid);   Serial.print(TEXT_SEPARATOR); \
    Serial.print(value); Serial.write(TEXT_CLOSE);

  Serial.print(PID_AXIS); Serial.print(TEXT_SEPARATOR); Serial.print(axis); Serial.write(TEXT_CLOSE);

    PRINT_PARAMETER(PID_PULSE_WIDTH_OFFSET_DIR,           dirPulseWidth)
    PRINT_PARAMETER(PID_PULSE_WIDTH_OFFSET_LOW,           lowPulsWidth)
    PRINT_PARAMETER(PID_PULSE_WIDTH_OFFSET_HIGH,          highPulsWidth)
    PRINT_PARAMETER(PID_STEPS,                            getSteps())
    PRINT_PARAMETER(PID_PITCH,                            getPitch())
    PRINT_PARAMETER(PID_AVG_STEP_DURATION,                avgStepDuartion)
    PRINT_PARAMETER(PID_STEP_PIN,                         getStepPin())
    PRINT_PARAMETER(PID_DIR_PIN,                          getDirectionPin())
    PRINT_PARAMETER(pidIncrementDirectionValue,           getIncrementDirectionValue())
    PRINT_PARAMETER(PID_MIN_SWITCH,                       minReached)
    PRINT_PARAMETER(PID_MAX_SWITCH,                       maxReached)
    PRINT_PARAMETER(PID_LIMIT,                            readLimitState())
    PRINT_PARAMETER(PID_LAST_STEP_DIR,                    getStepDirection())
    PRINT_PARAMETER(PID_ACCEL_START_SPEED,                pwmProfile.startSpeed_MM_SEC * 60)
    PRINT_PARAMETER(PID_ACCEL_STOP_SPEED,                 pwmProfile.stopSpeed_MM_SEC * 60)

  #undef PRINT_PARAMETER
}
/////////////////////////////////////////////////////////////////////////////////////
void CncStepper::incStepCounter() { 
/////////////////////////////////////////////////////////////////////////////////////
  // detect overflows
  if ( stepCounter == MAX_LONG ) { 
    stepCounter = MIN_LONG;
    stepCounterOverflow++;
  }
    
  stepCounter++;
}
//////////////////////////////////////////////////////////////////////////////
void CncStepper::reset() {
//////////////////////////////////////////////////////////////////////////////
  stepDirection         = SD_UNKNOWN;
  avgStepDuartion       = 0L;
  interrupted           = false;
  minReached            = false;
  maxReached            = false;

  tsPrevStep            = 0L;
  tsCurrStep            = 0L;

  resetPosReplyThresholdCouter();
  resetStepCounter();
}
//////////////////////////////////////////////////////////////////////////////
void CncStepper::sendCurrentLimitStates(bool force) {
//////////////////////////////////////////////////////////////////////////////
  controller->sendCurrentLimitStates(force);
}
//////////////////////////////////////////////////////////////////////////////
void CncStepper::broadcastInterrupt() {
//////////////////////////////////////////////////////////////////////////////
  interrupted = true;
  controller->broadcastInterrupt();
}
//////////////////////////////////////////////////////////////////////////////
void CncStepper::setMinReached(bool state) {
//////////////////////////////////////////////////////////////////////////////
  // avoid hysteresis
  if ( maxReached == true ) {
    minLimitCnt++;
    
    if ( minLimitCnt == 3 ) {
      minLimitCnt = 0;
      minReached = state;
    }
   
  } else {
    minReached = state;
  }

  sendCurrentLimitStates(FORCE);
}
//////////////////////////////////////////////////////////////////////////////
void CncStepper::setMaxReached(bool state) {
//////////////////////////////////////////////////////////////////////////////  
  // avoid hysteresis
  if ( minReached == true ) {
    maxLimitCnt++;
    
    if ( maxLimitCnt == 3 ) {
      maxLimitCnt = 0;
      maxReached = state;
    }
    
  } else {
    maxReached = state;
  }

  sendCurrentLimitStates(FORCE);
}
//////////////////////////////////////////////////////////////////////////////
int32_t CncStepper::readLimitState(int dir) {
//////////////////////////////////////////////////////////////////////////////
  if ( digitalRead(limitPin) == LimitSwitch::LIMIT_SWITCH_OFF )
    return LimitSwitch::LIMIT_UNSET;

  // determine which one . . .
  switch ( dir ) {
    case DIRECTION_INC:   return LimitSwitch::LIMIT_MAX;
    case DIRECTION_DEC:   return LimitSwitch::LIMIT_MIN;
  }

  // in this case no valid limit information available
  return LimitSwitch::LIMIT_UNKNOWN;
}
//////////////////////////////////////////////////////////////////////////////
int32_t CncStepper::getLimitState() {
//////////////////////////////////////////////////////////////////////////////  
  if ( minReached == true ) return LimitSwitch::LIMIT_MIN;
  if ( maxReached == true ) return LimitSwitch::LIMIT_MAX;
  
  return LimitSwitch::LIMIT_UNSET;
}
//////////////////////////////////////////////////////////////////////////////
bool CncStepper::checkLimit(int dir) {
//////////////////////////////////////////////////////////////////////////////
  int val = digitalRead(limitPin);
  if ( val == LimitSwitch::LIMIT_SWITCH_ON ) {

    // unclear sitiuation avoid movement!
    if ( stepDirection == SD_UNKNOWN ) {
      sendCurrentLimitStates(FORCE);
      broadcastInterrupt();
      return true;
    }

    // enable the move in the opposite direction
    if ( minReached && dir > 0 )
      return false;
      
    // enable the move in the opposite direction
    if ( maxReached && dir < 0 )
      return false;
      
    switch ( dir ) {
      
      case DIRECTION_INC:   setMaxReached(true);
                            return true;
      
      case DIRECTION_DEC:   setMinReached(true);
                            return true;
    }
  } else {
    // reset limit state
    if ( minReached == true || maxReached == true ) {
      setMinReached(false);
      setMaxReached(false);
    }
  }

  return false;
}
//////////////////////////////////////////////////////////////////////////////
void CncStepper::resetDirectionPin() {
//////////////////////////////////////////////////////////////////////////////
  if ( getIncrementDirectionValue() == false )  setDirection(DIRECTION_INC);
  else                                          setDirection(DIRECTION_DEC);            
}
//////////////////////////////////////////////////////////////////////////////
bool CncStepper::setDirection(int32_t steps) {
//////////////////////////////////////////////////////////////////////////////
  if      ( steps <  0 ) return setDirection(SD_DEC);
  else if ( steps >= 0 ) return setDirection(SD_INC);

  return false;
}
//////////////////////////////////////////////////////////////////////////////
bool CncStepper::setDirection(const StepDirection sd) {
//////////////////////////////////////////////////////////////////////////////
  if ( stepDirection == sd )
    return true;
  
  stepDirection = sd;

  if ( controller->isProbeMode() == OFF ) {
    // The functions getIn/DecrementDirectionValue() switches the physical direction of "stepDirection".
    // The rest of the stepper logic isn't affected because this is to overrule the stepper cabling only
    // and the physical min and max position staying unchanged
    const bool dir = (stepDirection == SD_INC ? getIncrementDirectionValue() : getDecrementDirectionValue());
    
    digitalWrite(directionPin, dir);
    delayMicroseconds(dirPulseWidth);
  }
  
  return true;
}
//////////////////////////////////////////////////////////////////////////////
unsigned char CncStepper::performNextStep() {
//////////////////////////////////////////////////////////////////////////////
  uint32_t tsStartStepping = calculateDuration ? micros() : 0;

  // first check
  // -----------------------------------------------------------
  // avoid everything in this states
  
  if ( isInterrupted() )
    return RET_INTERRUPT;

  if ( stepDirection == SD_UNKNOWN ) {
    broadcastInterrupt();
    return RET_INTERRUPT;
  }

  if ( checkLimit(stepDirection) == true ) {
    return RET_LIMIT;
  }

  if ( controller->isSpeedControllerActive() ) {
    // gurantiee total puls width
    tsCurrStep = micros();
  
    // -------
    // |     |
    // |     |-------------------------------------------
    // / hpw /     lpw    / 
    // /      minpw       /  speedDelay  /  accelDelay  / 
    
    unsigned long currPulsWidth = minPulsWidth 
                                + pwmProfile.speedDelay 
                                + pwmProfile.accelDelay;
  
    // micros(): Returns the number of microseconds since the Arduino board began running the current program. 
    // This number will overflow (go back to zero), after approximately 70 minutes. On 16 MHz Arduino boards 
    // (e.g. Duemilanove and Nano), this function has a resolution of four microseconds 
    // (i.e. the value returned is always a multiple of four).
    // Check this . . . 
    if ( tsCurrStep <= tsPrevStep ) {
      delayMicroseconds(minPulsWidth * 2);
      
    } else {
      // normal processing
      unsigned long tsDiff = tsCurrStep - tsPrevStep;
      
      if ( tsDiff < currPulsWidth )
        delayMicroseconds( currPulsWidth - tsDiff );
    }
  }
  
  // then stepping . . .
  if ( controller->isProbeMode() == OFF ) { 
    tsPrevStep = tsCurrStep;
       
    // start the step puls
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(highPulsWidth); 

    // stop the step puls
    digitalWrite(stepPin, LOW);
    
    // dont sleep here, because this period will be guranteed by tsPrecStep and tsCurrStep
    // delayMicroseconds(lowPulsWidth);
    
  } else {
     // simulate step delay
     delayMicroseconds(highPulsWidth);
  }

  // ----------------------------------------------------------
  // position handling -/+1
  curPos += stepDirection;
  incPosReplyThresholdCouter();
  incStepCounter();

  if ( calculateDuration )
    calcStepLoopDuration(micros() - tsStartStepping);
    
  return RET_OK;
}
//////////////////////////////////////////////////////////////////////////////
inline void CncStepper::calcStepLoopDuration(uint32_t lastDuration) {
//////////////////////////////////////////////////////////////////////////////
  if ( avgStepDuartion == 0L )  avgStepDuartion = lastDuration;
  else                          avgStepDuartion = (double)((avgStepDuartion + lastDuration)/2);    
}


