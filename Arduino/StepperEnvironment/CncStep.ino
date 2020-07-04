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
  
#define CNC_STEPPER_WRITE_STP_PIN(value) \
  if ( controller->isProbeMode() == OFF ) { \
    AE::digitalWrite(stpPin, value); \
  } \
  stepPhase = (value == PL_HIGH);

namespace StepperParameter {
  
  template <class T>
  void print(unsigned char pid, T value, int8_t indent=2) {
    for (auto i=0; i<indent; i++) Serial.print(BLANK1); \
    Serial.print(pid);   Serial.print(TEXT_SEPARATOR); \
    Serial.print(value); Serial.write(TEXT_CLOSE);
  }
  
  void print(unsigned char pid, float value, int8_t indent=2) {
    for (auto i=0; i<indent; i++) Serial.print(BLANK1); \
    Serial.print(pid);      Serial.print(TEXT_SEPARATOR); \
    Serial.print(value, 3); Serial.write(TEXT_CLOSE);
  }
};

/////////////////////////////////////////////////////////////////////////////////////
CncArduinoStepper::CncArduinoStepper(const StepperSetup& ss) 
/////////////////////////////////////////////////////////////////////////////////////
: controller              (ss.controller)
, stpPin                  (ss.stpPin)
, dirPin                  (ss.dirPin)
, lmtPin                  (ss.lmtPin)
, interrupted             (false)
, minReached              (false)
, maxReached              (false)
, stepPhase               (false)
, tsStartStep             (0L)
, feedrate                (1.0)
, highPulsWidth           (500)
, stepDirection           (SD_UNKNOWN)
, curPos                  (0L)
{
  CNC_STEPPER_WRITE_STP_PIN(PL_LOW);
}
/////////////////////////////////////////////////////////////////////////////////////
CncArduinoStepper::~CncArduinoStepper() {
/////////////////////////////////////////////////////////////////////////////////////  
}
/////////////////////////////////////////////////////////////////////////////////////
void  CncArduinoStepper::printConfig() {
/////////////////////////////////////////////////////////////////////////////////////
  const unsigned char pidIncrementDirectionValue = getIncrementDirectionValuePid();
  Serial.print(PID_AXIS); Serial.print(TEXT_SEPARATOR); Serial.print(getAxisId()); Serial.write(TEXT_CLOSE);

    StepperParameter::print(PID_PULSE_WIDTH_HIGH,                 highPulsWidth);
    StepperParameter::print(PID_FEEDRATE,                         getFeedrate());
    StepperParameter::print(PID_STEP_PIN,                         getStepPin());
    StepperParameter::print(PID_DIR_PIN,                          getDirectionPin());
    StepperParameter::print(pidIncrementDirectionValue,           getIncrementDirectionValue());
    StepperParameter::print(PID_MIN_SWITCH,                       minReached);
    StepperParameter::print(PID_MAX_SWITCH,                       maxReached);
    StepperParameter::print(PID_LIMIT,                            readLimitState());
    StepperParameter::print(PID_LAST_STEP_DIR,                    getStepDirection());
}
/////////////////////////////////////////////////////////////////////////////////////
bool CncArduinoStepper::isReadyToRun() {
/////////////////////////////////////////////////////////////////////////////////////
  CNC_STEPPER_LOG_FUNCTION();
  
  bool ret = true;

  if ( interrupted == true ) {
    ArduinoMainLoop::pushMessage(MT_ERROR, E_INTERRUPT);
    ret = false;
  }

  if ( readLimitState() != LimitSwitch::LIMIT_UNSET ) {
    ArduinoMainLoop::pushMessage(MT_ERROR, E_LIMIT_SWITCH_ACTIVE);
    ret = false;
  }

  return ret;  
}
//////////////////////////////////////////////////////////////////////////////
void CncArduinoStepper::reset() {
//////////////////////////////////////////////////////////////////////////////
  CNC_STEPPER_LOG_FUNCTION();

  curPos                = 0L;
  stepDirection         = SD_UNKNOWN;

  interrupted           = false;
  minReached            = false;
  maxReached            = false;

  tsStartStep           = 0L;

  ARDO_TRACE_STEPPER_DIR(getAxisId(), stepDirection)
  ARDO_TRACE_STEPPER_POS(getAxisId(), curPos)
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
int8_t CncArduinoStepper::getLimitState() {
//////////////////////////////////////////////////////////////////////////////  
  if ( minReached == true ) return LimitSwitch::LIMIT_MIN;
  if ( maxReached == true ) return LimitSwitch::LIMIT_MAX;
  
  return LimitSwitch::LIMIT_UNSET;
}
//////////////////////////////////////////////////////////////////////////////
int8_t CncArduinoStepper::readLimitState(int dir) {
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
    CNC_STEPPER_DELAY_MICROS(10);
  }

  ARDO_TRACE_STEPPER_DIR(getAxisId(), stepDirection)
  
  return true;
}
//////////////////////////////////////////////////////////////////////////////
byte CncArduinoStepper::performStep() {
//////////////////////////////////////////////////////////////////////////////
  byte ret = initiateStep();
  if ( ret != RET_OK )
    return ret;

  return finalizeStep();
}
//////////////////////////////////////////////////////////////////////////////
byte CncArduinoStepper::initiateStep() {
//////////////////////////////////////////////////////////////////////////////
  // avoid everything in this states
  if ( isInterrupted() )
    return RET_INTERRUPT;

  if ( stepDirection == SD_UNKNOWN ) {
    controller->broadcastInterrupt();
    return RET_INTERRUPT;
  }

  if ( checkLimit(stepDirection) == true )
    return RET_LIMIT;

  if ( stepPhase == true )
    finalizeStep();

  // generate the step impuls ....
  CNC_STEPPER_WRITE_STP_PIN(PL_HIGH);

  tsStartStep = AE::micros() % UINT32_MAX;
  return RET_OK;
}
//////////////////////////////////////////////////////////////////////////////
byte CncArduinoStepper::finalizeStep() {
//////////////////////////////////////////////////////////////////////////////
  // avoid everything in this states
  if ( isInterrupted() )
    return RET_INTERRUPT;

  if ( stepDirection == SD_UNKNOWN ) {
    controller->broadcastInterrupt();
    return RET_INTERRUPT;
  }

  if ( stepPhase == false )
    return RET_OK;

  const uint32_t tsNow  = AE::micros();
  const int32_t  tpPuls = highPulsWidth - (tsNow - tsStartStep);

  // guarantee the min. pulse width
  if ( tpPuls > 0 ) CNC_STEPPER_DELAY_MICROS(tpPuls);

  // finish the step impuls
  CNC_STEPPER_WRITE_STP_PIN(PL_LOW);

  // position handling -/+1
  curPos += stepDirection;
  ARDO_TRACE_STEPPER_POS(getAxisId(), curPos)
  
  return RET_OK;
}
