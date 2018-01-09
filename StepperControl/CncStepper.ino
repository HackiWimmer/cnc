#include "CommonFunctions.h"
#include "CncStepper.h"

//////////////////////////////////////////////////////////////////////////////
CncStepper::CncStepper(CncController* crtl, char a, byte stpPin, byte dirPin, byte lmtPin, LastErrorCodes& lec)
//////////////////////////////////////////////////////////////////////////////
: initialized(false)
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
, lastStepDirection(0)
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
  //todo
  return 1L;
}
//////////////////////////////////////////////////////////////////////////////
void CncStepper::printConfig() {
//////////////////////////////////////////////////////////////////////////////
  Serial.print(PID_AXIS); Serial.print(TEXT_SEPARATOR); Serial.print(axis); Serial.write(TEXT_CLOSE);

    Serial.print(BLANK); Serial.print(PID_STEPPER_INITIALIZED);            Serial.print(TEXT_SEPARATOR); Serial.print(initialized);                   Serial.write(TEXT_CLOSE);
    Serial.print(BLANK); Serial.print(PID_CURRENT_DIR_PULSE_WIDTH);        Serial.print(TEXT_SEPARATOR); Serial.print(dirPulseWidth);                 Serial.write(TEXT_CLOSE);
    Serial.print(BLANK); Serial.print(PID_PULSE_WIDTH_OFFSET);             Serial.print(TEXT_SEPARATOR); Serial.print(pulseWidthOffset);              Serial.write(TEXT_CLOSE);
    Serial.print(BLANK); Serial.print(PID_STEPS);                          Serial.print(TEXT_SEPARATOR); Serial.print(getSteps());                    Serial.write(TEXT_CLOSE);
    Serial.print(BLANK); Serial.print(PID_PITCH);                          Serial.print(TEXT_SEPARATOR); Serial.print(getPitch());                    Serial.write(TEXT_CLOSE);
    Serial.print(BLANK); Serial.print(PID_AVG_STEP_DURRATION);             Serial.print(TEXT_SEPARATOR); Serial.print(avgStepDuartion);               Serial.write(TEXT_CLOSE);
    Serial.print(BLANK); Serial.print(PID_STEP_PIN);                       Serial.print(TEXT_SEPARATOR); Serial.print(getStepPin());                  Serial.write(TEXT_CLOSE);
    Serial.print(BLANK); Serial.print(PID_DIR_PIN);                        Serial.print(TEXT_SEPARATOR); Serial.print(getDirectionPin());             Serial.write(TEXT_CLOSE);
    Serial.print(BLANK); Serial.print(PID_MIN_SWITCH);                     Serial.print(TEXT_SEPARATOR); Serial.print(minReached);                    Serial.write(TEXT_CLOSE);
    Serial.print(BLANK); Serial.print(PID_MAX_SWITCH);                     Serial.print(TEXT_SEPARATOR); Serial.print(maxReached);                    Serial.write(TEXT_CLOSE);
    Serial.print(BLANK); Serial.print(PID_LIMIT);                          Serial.print(TEXT_SEPARATOR); Serial.print(readLimitState());              Serial.write(TEXT_CLOSE);
    Serial.print(BLANK); Serial.print(PID_LAST_STEP_DIR);                  Serial.print(TEXT_SEPARATOR); Serial.print(getLastStepDirection());        Serial.write(TEXT_CLOSE);
 
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
  lastStepDirection = 0;
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
  int val = digitalRead(limitPin);
  
  if ( val == LOW ) {
    switch ( dir ) {
      case -1:  return LIMIT_MIN;
      case +1:  return LIMIT_MAX;
      default:  return LIMIT_SET_BUT_MIN_MAX_UNKNOWN;
    }
  }
 
  return LIMIT_UNSET;
}
//////////////////////////////////////////////////////////////////////////////
long CncStepper::getLimitState() {
//////////////////////////////////////////////////////////////////////////////  
  if ( minReached == true ) return LIMIT_MIN;
  if ( maxReached == true ) return LIMIT_MAX;
  
  return LIMIT_UNSET;
}
//////////////////////////////////////////////////////////////////////////////
bool CncStepper::checkLimit(int dir) {
//////////////////////////////////////////////////////////////////////////////
  int val = digitalRead(limitPin);
  if ( val == LOW ) {

    // unclear sitiuation avoid movement!
    if ( lastStepDirection == 0 ) {
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
      
      case +1:  setMaxReached(true);
                return true;
      
      case -1:  setMinReached(true);
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
    if ( digitalRead(ENABLE_PIN) == HIGH ) {
      // Possibly an error ?
      if ( testActive == false )
        errorInfo.setNextErrorInfo(E_STEPPER_NOT_ENALED, String(String(axis) + " Axis"));
        
      return testActive;
    }
  }
  
  // -----------------------------------------------------------
  // determine direction and init driver
  short stepDirection = (stepsToMove < 0 ? -1 : +1);
  digitalWrite(directionPin, (stepsToMove > 0));
  delayMicroseconds(dirPulseWidth);
  
  // -----------------------------------------------------------
  // start step loop 
  unsigned long stepsLeft = absolute(stepsToMove);
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
      if ( testActive == true )
        return false;
        
      return true;
    }

    // ----------------------------------------------------------
    // observe the serial port for signals
    if ( peakSerial(frontSerialByte) == true ) {
      switch ( frontSerialByte ) {
        
        // interrupt handling
        case SIG_INTERRUPPT:
                    // dont remove the signal from serial, so an explizit reset have to be called by the interface
                    broadcastInterrupt();
                    return false;

        case SIG_HALT:
                    // dont remove the signal from serial, so an explizit reset have to be called by the interface
                    return false;
                    
        case SIG_PAUSE:
                    Serial.read();
                    pauseStepping = true;
                    break;

        case SIG_RESUME:
                    Serial.read();
                    pauseStepping = false;
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
    // position handling
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
  
  return true;
}
//////////////////////////////////////////////////////////////////////////////
inline void CncStepper::calcStepDuration(unsigned long lastDuration) {
  if ( avgStepDuartion == 0L )  avgStepDuartion = lastDuration;
  else                          avgStepDuartion = (double)((avgStepDuartion + lastDuration)/2);    
}

