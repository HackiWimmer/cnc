#include "MainLoop.h"
#include "CncStep.h"

#ifndef SKETCH_COMPILE 
  #define CNC_STEPPER_LOG_FUNCTION()  \
    { \
        const wxString logStr(wxString::Format("Stepper[%c]::%s", getAxisId(), __FUNCTION__)); \
        ARDO_DEBUG_MESSAGE('D', logStr); \
    }
#else
  #define CNC_STEPPER_LOG_FUNCTION()
#endif

#define CNC_STEPPER_DELAY_MICROS(us) \
  { \
    if ( true ) { \
      AE::delayMicroseconds(us); \
    } \
  }
  
/////////////////////////////////////////////////////////////////////////////////////
CncArduinoStepper::CncArduinoStepper(const StepperSetup& ss) 
/////////////////////////////////////////////////////////////////////////////////////
: controller            (ss.controller)
, pwmProfile            ()
, stpPin                (ss.stpPin)
, dirPin                (ss.dirPin)
, lmtPin                (ss.lmtPin)
, interrupted           (false)
, calculateDuration     (false)
, minReached            (false)
, maxReached            (false)
, tsPrevStep            (0L)
, tsCurrStep            (0L)
, pitch                 (1.0)
, dirPulseWidth         (10)
, lowPulsWidth          (500)
, highPulsWidth         (500)
, minPulsWidth          (highPulsWidth + lowPulsWidth)
, stepDirection         (SD_UNKNOWN)
, steps                 (400)
, avgStepDuartion       (0L)
, stepCounter           (0L)
, stepCounterOverflow   (0L)
, curPos                (0L)
, posReplyThresholdCount(0L)
{
}
/////////////////////////////////////////////////////////////////////////////////////
CncArduinoStepper::~CncArduinoStepper() {
/////////////////////////////////////////////////////////////////////////////////////  
}
/////////////////////////////////////////////////////////////////////////////////////
void  CncArduinoStepper::printConfig() {
/////////////////////////////////////////////////////////////////////////////////////
  #define PRINT_PARAMETER( Pid, value ) \
    Serial.print(BLANK); \
    Serial.print(BLANK); \
    Serial.print(Pid);   Serial.print(TEXT_SEPARATOR); \
    Serial.print(value); Serial.write(TEXT_CLOSE);

  const unsigned char pidIncrementDirectionValue = getIncrementDirectionValuePid();
  
  Serial.print(PID_AXIS); Serial.print(TEXT_SEPARATOR); Serial.print(getAxisId()); Serial.write(TEXT_CLOSE);

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
    PRINT_PARAMETER(PID_ACCEL_STOP_SPEED,                 pwmProfile.stopSpeed_MM_SEC  * 60)
  
  #undef PRINT_PARAMETER
}
//////////////////////////////////////////////////////////////////////////////
void CncArduinoStepper::calcStepLoopDuration(int32_t lastDuration) {
//////////////////////////////////////////////////////////////////////////////
  // check micros() oveflow as well as to large durations
  if ( lastDuration <= 0 || lastDuration > 4 * 1000 * 1000 )
    return;
    
  if ( avgStepDuartion == 0L )  avgStepDuartion = lastDuration;
  else                          avgStepDuartion = (double)((avgStepDuartion + lastDuration)/2);    
}
/////////////////////////////////////////////////////////////////////////////////////
int32_t CncArduinoStepper::isReadyToRun() {
/////////////////////////////////////////////////////////////////////////////////////
  CNC_STEPPER_LOG_FUNCTION();
  
  int32_t ret = 1;

  if ( interrupted == true ) {
    ArduinoMainLoop::pushErrorMessage(E_INTERRUPT);
    ret = 0;
  }

  if ( readLimitState() != LimitSwitch::LIMIT_UNSET ) {
    ArduinoMainLoop::pushErrorMessage(E_LIMIT_SWITCH_ACTIVE);
    ret = 0;
  }
  
  return ret;  
}
//////////////////////////////////////////////////////////////////////////////
void CncArduinoStepper::reset() {
//////////////////////////////////////////////////////////////////////////////
  CNC_STEPPER_LOG_FUNCTION();

  curPos                = 0L;
  stepDirection         = SD_UNKNOWN;

  avgStepDuartion       = 0L;
  interrupted           = false;
  minReached            = false;
  maxReached            = false;

  tsPrevStep            = 0L;
  tsCurrStep            = 0L;

  resetPosReplyThresholdCouter();
  resetStepCounter();

  ARDO_TRACE_STEPPER_DIR(getAxisId(), stepDirection)
  ARDO_TRACE_STEPPER_POS(getAxisId(), curPos)
}
/////////////////////////////////////////////////////////////////////////////////////
void CncArduinoStepper::incStepCounter() { 
/////////////////////////////////////////////////////////////////////////////////////
  // detect overflows
  if ( stepCounter == MAX_LONG ) { 
    stepCounter = MIN_LONG;
    stepCounterOverflow++;
  }
    
  stepCounter++;
}
//////////////////////////////////////////////////////////////////////////////
void CncArduinoStepper::setLimitStateManually(int32_t value) {
//////////////////////////////////////////////////////////////////////////////  
  if ( value > 0 )  { maxReached = true; stepDirection = SD_INC; }
  else              { minReached = true; stepDirection = SD_DEC; }
}    
//////////////////////////////////////////////////////////////////////////////
void CncArduinoStepper::resetDirectionPin() {
//////////////////////////////////////////////////////////////////////////////
  if ( getIncrementDirectionValue() == false )  setDirection(DIRECTION_INC);
  else                                          setDirection(DIRECTION_DEC);            
  
  ARDO_TRACE_STEPPER_DIR(getAxisId(), stepDirection)
}
//////////////////////////////////////////////////////////////////////////////
int32_t CncArduinoStepper::getLimitState() {
//////////////////////////////////////////////////////////////////////////////  
  if ( minReached == true ) return LimitSwitch::LIMIT_MIN;
  if ( maxReached == true ) return LimitSwitch::LIMIT_MAX;
  
  return LimitSwitch::LIMIT_UNSET;
}
//////////////////////////////////////////////////////////////////////////////
int32_t CncArduinoStepper::readLimitState(int dir) {
//////////////////////////////////////////////////////////////////////////////
  if ( AE::digitalRead(lmtPin) == LimitSwitch::LIMIT_SWITCH_OFF )
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
bool CncArduinoStepper::checkLimit(int dir) {
//////////////////////////////////////////////////////////////////////////////
  static short minLimitCnt = 0;
  static short maxLimitCnt = 0;

  // ------------------------------------------------------------------------
  auto setMinReached = [&](bool state) {
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
  
    controller->sendCurrentLimitStates(FORCE);
  };

  // ------------------------------------------------------------------------
  auto setMaxReached = [&](bool state) {
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
    
    controller->sendCurrentLimitStates(FORCE);
  };
  
  // ------------------------------------------------------------------------
  const int val = AE::digitalRead(lmtPin);
  if ( val == LimitSwitch::LIMIT_SWITCH_ON ) {

    // unclear sitiuation avoid movement!
    if ( stepDirection == SD_UNKNOWN ) {
      controller->sendCurrentLimitStates(FORCE);
      controller->broadcastInterrupt();
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
bool CncArduinoStepper::setDirection(int32_t steps) {
//////////////////////////////////////////////////////////////////////////////
  if      ( steps <  0 ) return setDirection(SD_DEC);
  else if ( steps >= 0 ) return setDirection(SD_INC);

  return false;
}
//////////////////////////////////////////////////////////////////////////////
bool CncArduinoStepper::setDirection(const StepDirection sd) {
//////////////////////////////////////////////////////////////////////////////
  if ( stepDirection == sd )
    return true;
  
  stepDirection = sd;

  if ( controller->isProbeMode() == OFF ) {
    // The functions get(In/De)crementDirectionValue() switches the physical direction of "stepDirection".
    // The rest of the stepper logic isn't affected because this is to overrule the stepper cabling only
    // and the physical min and max position staying unchanged
    const bool dir = (stepDirection == SD_INC ? getIncrementDirectionValue() : getDecrementDirectionValue());
    
    AE::digitalWrite(dirPin, dir);
    CNC_STEPPER_DELAY_MICROS(dirPulseWidth);
  }

  ARDO_TRACE_STEPPER_DIR(getAxisId(), stepDirection)
  
  return true;
}
//////////////////////////////////////////////////////////////////////////////
byte CncArduinoStepper::performNextStep() {
//////////////////////////////////////////////////////////////////////////////
  uint32_t tsStartStepping = calculateDuration ? AE::micros() : 0;

  // -----------------------------------------------------------
  // avoid everything in this states
  
  if ( isInterrupted() )
    return RET_INTERRUPT;

  if ( stepDirection == SD_UNKNOWN ) {
    controller->broadcastInterrupt();
    return RET_INTERRUPT;
  }

  if ( checkLimit(stepDirection) == true )
    return RET_LIMIT;

  // -----------------------------------------------------------
  // gurantee the current puls width which is separated as:
  // -------
  // |     |
  // |     |-------------------------------------------
  // / hpw /     lpw    / 
  // /      minpw       /  speedDelay  /  accelDelay  / 
  
  const uint16_t extraDelay    = controller->isSpeedControllerActive() ? pwmProfile.speedDelay + pwmProfile.accelDelay : 0;
  const uint16_t currPulsWidth = minPulsWidth + extraDelay;

  // micros(): Returns the number of microseconds since the Arduino board began running the current program. 
  // This number will overflow (go back to zero), after approximately 70 minutes. 
  tsCurrStep = AE::micros();

  // determine the time elapsed since we were last here
  uint16_t tsDiff = 0;                                                          // case tsDiff > 32000: elapsed time to large
  if      ( tsCurrStep - tsPrevStep <     0 ) tsDiff = minPulsWidth * 2;        // micros() was overflowed
  else if ( tsCurrStep - tsPrevStep < 32000 ) tsDiff = tsCurrStep - tsPrevStep; // normal processing 

  // in this case it is not enough time elapsed and we have to wait
  if ( tsDiff < currPulsWidth )
    CNC_STEPPER_DELAY_MICROS( currPulsWidth - tsDiff );

  // swap tsCurrStep
  tsPrevStep = tsCurrStep;

  // then stepping . . .
  {      
    #define DIGITAL_WRITE_STP_PIN(value) \
      if ( controller->isProbeMode() == OFF ) { \
        AE::digitalWrite(stpPin, value); \
      }

    // start the step puls
    DIGITAL_WRITE_STP_PIN(PL_HIGH);
    CNC_STEPPER_DELAY_MICROS(highPulsWidth); 
  
    // stop the step puls
    DIGITAL_WRITE_STP_PIN(PL_LOW);
    // dont sleep here, because this period will be guranteed by tsPrevStep and tsCurrStep
    // CNC_STEPPER_DELAY_MICROS(lowPulsWidth);

    #undef DIGITAL_WRITE_STP_PIN(value)
  }
  
  // ----------------------------------------------------------
  // position handling -/+1
  curPos += stepDirection;
  ARDO_TRACE_STEPPER_POS(getAxisId(), curPos)
  
  incPosReplyThresholdCouter();
  incStepCounter();

  if ( calculateDuration )
    calcStepLoopDuration(AE::micros() - tsStartStepping);
    
  return RET_OK;
}
