#include "CommonFunctions.h"
#include "CncSpeedManager.h"
#include "CncStepper.h"

//////////////////////////////////////////////////////////////////////////////
CncStepper::CncStepper(CncController* crtl, char a, byte stpPin, byte dirPin, byte lmtPin, LastErrorCodes& lec)
//////////////////////////////////////////////////////////////////////////////
: INCREMENT_DIRECTION_VALUE(NORMALIZED_INCREMENT_DIRECTION_VALUE)
, initialized(false)
, interrupted(false)
, pauseStepping(false)
, minReached(false)
, maxReached(false)
, minLimitCnt(0)
, maxLimitCnt(0)
, directionPin(dirPin)
, stepPin(stpPin)
, limitPin(lmtPin)
, axis(a)
, dirPulseWidth(2)
, pulseWidthOffset(100L * 100L)
, avgStepDuartion(0L)
, tsLoopRef(0L)
, tsLoopEnd(0L)
, tsStepRef(0L)
, tsStepLst(0L)
, steps(1L)
, stepCounter(0L)
, stepCounterOverflow(0L)
, lastStepDirection(DIRECTION_UNKNOWN)
, curPos(0L)
, controller(crtl)
, errorInfo(lec)
, validPitch(false)
, pitch(0.0)
{
}
//////////////////////////////////////////////////////////////////////////////
CncStepper::~CncStepper() {
//////////////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////////////////////
long CncStepper::isReadyToRun() {
/////////////////////////////////////////////////////////////////////////////////////
  long ret = 1;

  if ( interrupted == true ) {
    errorInfo.setNextErrorInfo(E_INTERRUPT, BLANK + axis);
    ret = 0;
  }

  if ( readLimitState() != LimitSwitch::LIMIT_UNSET ) {
    errorInfo.setNextErrorInfo(E_LIMIT_SWITCH_ACTIVE, BLANK + axis);
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
    Serial.print(Pid);   Serial.print(TEXT_SEPARATOR); \
    Serial.print(value); Serial.write(TEXT_CLOSE);

  Serial.print(PID_AXIS); Serial.print(TEXT_SEPARATOR); Serial.print(axis); Serial.write(TEXT_CLOSE);

    PRINT_PARAMETER(PID_STEPPER_INITIALIZED,              initialized)
    PRINT_PARAMETER(PID_CURRENT_DIR_PULSE_WIDTH,          dirPulseWidth)
    PRINT_PARAMETER(PID_PULSE_WIDTH_OFFSET,               pulseWidthOffset)
    PRINT_PARAMETER(PID_STEPS,                            getSteps())
    PRINT_PARAMETER(PID_PITCH,                            getPitch())
    PRINT_PARAMETER(PID_AVG_STEP_DURRATION,               avgStepDuartion)
    PRINT_PARAMETER(PID_STEP_PIN,                         getStepPin())
    PRINT_PARAMETER(PID_DIR_PIN,                          getDirectionPin())
    PRINT_PARAMETER(pidIncrementDirectionValue,           getIncrementDirectionValue())
    PRINT_PARAMETER(PID_MIN_SWITCH,                       minReached)
    PRINT_PARAMETER(PID_MAX_SWITCH,                       maxReached)
    PRINT_PARAMETER(PID_LIMIT,                            readLimitState())
    PRINT_PARAMETER(PID_LAST_STEP_DIR,                    getLastStepDirection())

    const CncSpeedManager& sm = controller->getSpeedManager();
    PRINT_PARAMETER(PID_SPEED_MGMT_LOW_PULSE_WIDTH,       sm.getLowPulseWidthX())
    PRINT_PARAMETER(PID_SPEED_MGMT_HIGH_PULSE_WIDTH,      sm.getHighPulseWidthX())
    PRINT_PARAMETER(PID_SPEED_MGMT_TOTAL_OFFSET,          sm.getTotalOffsetX())
    PRINT_PARAMETER(PID_SPEED_MGMT_PER_SETP_OFFSET,       sm.getOffsetPerStepX())
    PRINT_PARAMETER(PID_SPEED_MGMT_MAX_SPEED,             sm.getMaxSpeedX_MM_MIN())
  
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
void CncStepper::setPitch(const double p) { 
//////////////////////////////////////////////////////////////////////////////
  validPitch = ( p > 0.0 ); 
  if ( validPitch ) 
    pitch = p;

  controller->setupSpeedManager();
}
//////////////////////////////////////////////////////////////////////////////
void CncStepper::reset() {
//////////////////////////////////////////////////////////////////////////////
  lastStepDirection = DIRECTION_UNKNOWN;
  tsLoopRef         = 0L;
  tsLoopEnd         = 0L;
  tsStepRef         = 0L;
  tsStepLst         = 0L;
  avgStepDuartion   = 0L;
  interrupted       = false;
  minReached        = false;
  maxReached        = false;
  pauseStepping     = false;

  resetStepCounter();

  initialized = true;
}
//////////////////////////////////////////////////////////////////////////////
bool CncStepper::enableStepperPin(bool state){
//////////////////////////////////////////////////////////////////////////////
  return controller->enableStepperPin(state);
}
//////////////////////////////////////////////////////////////////////////////
void CncStepper::sendCurrentLimitStates(bool force) {
//////////////////////////////////////////////////////////////////////////////
  controller->sendCurrentLimitStates(force);
}
//////////////////////////////////////////////////////////////////////////////
void CncStepper::sendCurrentPositions(unsigned char pid, bool force) {
//////////////////////////////////////////////////////////////////////////////
  controller->sendCurrentPositions(pid, force);  
}
//////////////////////////////////////////////////////////////////////////////
void CncStepper::broadcastInterrupt() {
//////////////////////////////////////////////////////////////////////////////
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

  sendCurrentLimitStates(true);
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

  sendCurrentLimitStates(true);
}
//////////////////////////////////////////////////////////////////////////////
long CncStepper::readLimitState(int dir) {
//////////////////////////////////////////////////////////////////////////////
  if ( digitalRead(limitPin) == LimitSwitch::LIMIT_SWITCH_OFF )
    return LimitSwitch::LIMIT_UNSET;

  // determine which one . . .
  switch ( dir ) {
    case DIRECTION_INC:   return LimitSwitch::LIMIT_MAX;
    case DIRECTION_DEC:   return LimitSwitch::LIMIT_MIN;
    default:              ;
  }

  // in this case the direction is unclear, try to get more information by calling the controller
  long limit = LimitSwitch::LIMIT_UNKNOWN;
  if ( controller->evaluateLimitState(this, limit) == true )
    return limit;

  // in this case no valid limit information available
  return LimitSwitch::LIMIT_UNKNOWN;
}
//////////////////////////////////////////////////////////////////////////////
long CncStepper::getLimitState() {
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
    if ( lastStepDirection == DIRECTION_UNKNOWN ) {
      sendCurrentLimitStates(true);
      broadcastInterrupt();
      return true;
    }

    // enable move the the opposite direction
    if ( minReached && dir > 0 )
      return false;
      
    // enable move the the opposite direction
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
bool CncStepper::isCancelMoveSignalRelevant(const unsigned char sig) {
  bool ret = false;
  
  switch ( axis ) {
    case 'X': ret = (sig == SIG_CANCEL_X_MOVE); break;
    case 'Y': ret = (sig == SIG_CANCEL_Y_MOVE); break;
    case 'Z': ret = (sig == SIG_CANCEL_Z_MOVE); break;
  }

  return ret;
}
//////////////////////////////////////////////////////////////////////////////
bool CncStepper::stepAxis(long stepsToMove, bool testActive) {
//////////////////////////////////////////////////////////////////////////////

  // -----------------------------------------------------------
  // avoid everything in this states
  if ( isInterrupted() )
    return false;

  if ( initialized == false ) {
    errorInfo.setNextErrorInfo(E_STEPPER_NOT_INITIALIZED, String(String(axis) + " Axis"));
    return false;
  }

  // -----------------------------------------------------------
  // nothing to do
  if ( stepsToMove == 0 ) 
    return true;

  // -----------------------------------------------------------
  if ( controller->isProbeMode() == false ) {
    if ( digitalRead(PIN_ENABLE) == ENABLE_STATE_OFF ) {
      // Possibly an error ?
      if ( testActive == false )
        errorInfo.setNextErrorInfo(E_STEPPER_NOT_ENALED, String(String(axis) + " Axis"));
        
      return testActive;
    }
  }
  
  // -----------------------------------------------------------
  // determine direction and init driver
  // stepsToMove == 0 is already checked above
  short stepDirection = (stepsToMove > 0 ? DIRECTION_INC : DIRECTION_DEC);

  // The functions getIn/DecrementDirectionValue() switches the physical direction of "stepsToMove > 0".
  // The rest of the stepper logic isn't affected because this is to overrule the stepper cabling only
  // and the physical min and max position staying unchanged
  digitalWrite(directionPin, (stepDirection > 0 ? getIncrementDirectionValue() : getDecrementDirectionValue()));
  delayMicroseconds(dirPulseWidth);
  
  // -----------------------------------------------------------
  // start step loop 
  unsigned long stepsLeft       = absolute(stepsToMove);
  unsigned char frontSerialByte = CMD_INVALID;
  
  tsLoopEnd = 0L;
  tsStepLst = 0L;
    
  while ( stepsLeft > 0 ) {

    // ----------------------------------------------------------
    // calculate loop duration
    tsLoopRef = micros();
    // with repect to the fact, that micros() overflow (go back to zero) 
    // after approximately 70 minutes - on 16 MHz Arduino boards
    if ( tsLoopEnd > 0L && tsLoopRef > tsLoopEnd)
      calcStepDuration(tsLoopRef - tsLoopEnd);

    // ----------------------------------------------------------
    // limit handling
    if ( checkLimit(stepDirection) == true ) {
      // Case:  A limit is activ

      if ( testActive == true )
        return false;

      // TODO: ALWAYS CREATE AN ERROR
      return true;
    }

    // ----------------------------------------------------------
    // tool observation handling
    if ( controller->evaluateToolState() == false ) {
      // Case: The tool isn't running

      // false will create an error case outside the stepper
      // this will abort the current run
      return false;
    }

    // ----------------------------------------------------------
    // observe the serial port for signals
    if ( peakSerial(frontSerialByte) == true ) {
      switch ( frontSerialByte ) {
        
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
                    pauseStepping = true;

                    // Don't leave the wigle loop, so break is used here
                    break;

        case SIG_RESUME:
                    // remove the signal from serial
                    Serial.read();
                    pauseStepping = false;

                    // Don't leave the wigle loop, so break is used here
                    break;

        case SIG_CANCEL_X_MOVE: 
        case SIG_CANCEL_Y_MOVE: 
        case SIG_CANCEL_Z_MOVE:
                    // first check the signal context
                    if ( isCancelMoveSignalRelevant(frontSerialByte) == true ) {
                      // remove the signal from serial
                      Serial.read();
                      
                      // Controlled cancellation: In this case it should look like a complete 
                      // successful move outside the stepper class 
                      return true;
                    }

                    // in this case nothing to do
                    break;
      }
    }

    // ----------------------------------------------------------
    // pause handling
    if ( pauseStepping == true ) {
       while ( checkSerialForPauseCommands(pauseStepping) == true ) {
           sendHeartbeat();
           delay(50);
       }
       pauseStepping = false;
    }

    // ----------------------------------------------------------
    // step the driver
    if ( controller->isProbeMode() == false ) { 
      digitalWrite(stepPin, HIGH); 
      sleepMicroseconds(controller->getHighPulseWidth(axis)); 
    }
    
    if ( controller->isProbeMode() == false ) {
      //sleepMicroseconds(controller->getPerStepSpeedOffset(axis));
      
      int pulseDelay = 0;
      if ( controller->isProbeMode() == false )
        pulseDelay = controller->getPerStepSpeedOffset(axis);

      if ( pulseDelay > 0 ) {
        // ----------------------------------------------------------
        // speed delay
        tsStepRef = micros();
        // with repect to the fact, that micros() overflow (go back to zero) 
        // after approximately 70 minutes - on 16 MHz Arduino boards
        if ( tsStepLst > 0L && tsStepRef > tsStepLst ) {
          
          long diff = pulseDelay - (tsStepRef - tsStepLst);
          if ( diff > 0 ) delayMicroseconds(diff);
          else            delayMicroseconds(0);
          
        } else {
          delayMicroseconds(pulseDelay);
        }
        tsStepLst = tsStepRef;
      }
    }
    
    if ( controller->isProbeMode() == false ) {
      digitalWrite(stepPin, LOW);  
      sleepMicroseconds(controller->getLowPulseWidth(axis));
    }

    // ----------------------------------------------------------
    // position handling -/+1
    curPos += stepDirection;

    // ----------------------------------------------------------
    // configure next step
    lastStepDirection = stepDirection;
    stepsLeft--;
    incStepCounter();

    // ----------------------------------------------------------
    // calculate the loop duration - ensure the last step of this loop 
    // is always considered
    if ( stepsLeft > 0 )  tsLoopEnd = tsLoopRef;
    else                  calcStepDuration(micros() - tsLoopRef);
    
  }      
  // end step loop ----------------------------------------------
  
  digitalWrite(directionPin, LOW);
  return true;
}
//////////////////////////////////////////////////////////////////////////////
inline void CncStepper::calcStepDuration(unsigned long lastDuration) {
  if ( avgStepDuartion == 0L )  avgStepDuartion = lastDuration;
  else                          avgStepDuartion = (double)((avgStepDuartion + lastDuration)/2);    
}

