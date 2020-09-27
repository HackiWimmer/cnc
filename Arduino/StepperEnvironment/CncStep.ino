#include "MainLoop.h"
#include "CncStep.h"

#ifndef SKETCH_COMPILE 
  #define CNC_STEPPER_LOG_FUNCTION()  \
    { \
        const wxString logStr(wxString::Format("Stepper[%c]::%s", axisLabel, __FUNCTION__)); \
        ARDO_DEBUG_MESSAGE('D', logStr); \
    }
#else
  #define CNC_STEPPER_LOG_FUNCTION()
#endif

#define CNC_STEPPER_DELAY_MICROS(us) \
  { \
    if ( false ) { \
      AE::delayMicroseconds(us); \
    } \
  }

#define CNC_STEPPER_WRITE_STP_PIN(value) \
  if ( controller->isProbeMode() == OFF ) { \
      writeStpPin(value); \
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
CncArduinoStepper::CncArduinoStepper(
  const StepperSetup&   ss,
  const char            label,
  readPin_funct         Fp_readLmtPins,
  readPin_funct         Fp_readMinLmtPinF,
  readPin_funct         Fp_readMaxLmtPinF,
  writePin_funct        Fp_writeDirPinF,
  writePin_funct        Fp_writeStpPinF
) 
/////////////////////////////////////////////////////////////////////////////////////
: axisLabel               (label)
, readLmtPins             (Fp_readLmtPins)
, readMinLmtPin           (Fp_readMinLmtPinF)
, readMaxLmtPin           (Fp_readMaxLmtPinF)
, writeDirPin             (Fp_writeDirPinF)
, writeStpPin             (Fp_writeStpPinF)
, controller              (ss.controller)
, stpPin                  (ss.stpPin)
, dirPin                  (ss.dirPin)
, llmPin                  (ss.llmPin)
, hlmPin                  (ss.hlmPin)
, idvPid                  (ss.idvPid)
, optimisticMode          (true)
, interrupted             (false)
, stepPhase               (false)
, tsStartStep             (0L)
, feedrate                (1.0)
, highPulsWidth           (500)
, stepDirection           (SD_UNKNOWN)
, curPos                  (0L)
{
}
/////////////////////////////////////////////////////////////////////////////////////
void  CncArduinoStepper::printConfig() {
/////////////////////////////////////////////////////////////////////////////////////
  const unsigned char pidIncrementDirectionValue = idvPid;
  Serial.print(PID_AXIS); Serial.print(TEXT_SEPARATOR); Serial.print(axisLabel); Serial.write(TEXT_CLOSE);

    StepperParameter::print(PID_PULSE_WIDTH_HIGH,                 highPulsWidth);
    StepperParameter::print(PID_FEEDRATE,                         getFeedrate());
    StepperParameter::print(PID_STEP_PIN,                         getStepPin());
    StepperParameter::print(PID_DIR_PIN,                          getDirectionPin());
    StepperParameter::print(pidIncrementDirectionValue,           getIncrementDirectionValue());
    StepperParameter::print(PID_MIN_SWITCH,                       readMinLmtPin());
    StepperParameter::print(PID_MAX_SWITCH,                       readMaxLmtPin());
    StepperParameter::print(PID_LIMIT,                            readLimitState());
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

  tsStartStep           = 0L;

  ARDO_TRACE_STEPPER_DIR(axisLabel, stepDirection)
  ARDO_TRACE_STEPPER_POS(axisLabel, curPos)
}
//////////////////////////////////////////////////////////////////////////////
bool CncArduinoStepper::resolveLimit() {
//////////////////////////////////////////////////////////////////////////////  
    const int8_t  ls   = readLimitState();
    const int32_t step = ( ls == LimitSwitch::LIMIT_MIN ? +1 : ( ls == LimitSwitch::LIMIT_MAX ? -1 : 0 ) );

    if ( step != 0 ) {
      
      short cnt  = 0;
      setDirection(step);
      
      while ( readLmtPins() == LimitSwitch::LIMIT_SWITCH_ON ) {
        performStep();

        if ( ( ++cnt * feedrate ) > 12.0 ) // max mm - a typical sensor diameter
          break;

        // artificially speed delay to be smooth
        AE::delayMicroseconds(5000);  
      }        
    }

   return ( readLmtPins() == LimitSwitch::LIMIT_SWITCH_OFF );
}   
//////////////////////////////////////////////////////////////////////////////
int8_t CncArduinoStepper::readLimitState() {
//////////////////////////////////////////////////////////////////////////////
  const bool lmtMin = readMinLmtPin() == LimitSwitch::LIMIT_SWITCH_ON; 
  const bool lmtMax = readMaxLmtPin() == LimitSwitch::LIMIT_SWITCH_ON; 
  
  if ( lmtMin && lmtMax )  return LimitSwitch::LIMIT_UNKNOWN;
  if ( lmtMin )            return LimitSwitch::LIMIT_MIN;
  if ( lmtMax )            return LimitSwitch::LIMIT_MAX;

  return LimitSwitch::LIMIT_UNSET;
}
//////////////////////////////////////////////////////////////////////////////
bool CncArduinoStepper::isLimitPinRelevant() {
/////////////////////////////////////////////////////////////////////////////
  if ( readLmtPins() == LimitSwitch::LIMIT_SWITCH_ON ) {

    switch ( stepDirection ) {

      case SD_UNKNOWN:  // unclear sitiuation avoid movement!
                        controller->sendCurrentLimitStates(FORCE);
                        controller->broadcastInterrupt();
                        return true;
                        
      case SD_INC:      // enable the move in the opposite direction
                        if ( readMinLmtPin() == LimitSwitch::LIMIT_SWITCH_ON )
                          return false;
                          
                        controller->sendCurrentLimitStates(FORCE);
                        return true;

      case SD_DEC:      // enable the move in the opposite direction
                        if ( readMaxLmtPin() == LimitSwitch::LIMIT_SWITCH_ON )
                          return false;

                        controller->sendCurrentLimitStates(FORCE);
                        return true;
    }
  }

  return false;
}
//////////////////////////////////////////////////////////////////////////////
bool CncArduinoStepper::setDirection(int32_t steps) {
//////////////////////////////////////////////////////////////////////////////
  return setDirection(steps < 0 ? SD_DEC : SD_INC);
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
    
    writeDirPin(dir);
    CNC_STEPPER_DELAY_MICROS(10);
  }

  ARDO_TRACE_STEPPER_DIR(axisLabel, stepDirection)
  
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
  //if ( isInterrupted() )
    //return 
    /*
  if ( stepDirection == SD_UNKNOWN ) {
    controller->broadcastInterrupt();
    return RET_INTERRUPT;
  }
 */
  if ( isLimitPinRelevant() == true )
    return RET_LIMIT;

  if ( stepPhase == true )
    finalizeStep();

  // generate the step impuls ....
  CNC_STEPPER_WRITE_STP_PIN(PL_HIGH);

  //tsStartStep = ArdoTs::now();
  return RET_OK;
}
//////////////////////////////////////////////////////////////////////////////
byte CncArduinoStepper::finalizeStep() {
//////////////////////////////////////////////////////////////////////////////
  // avoid everything in this states
  //if ( isInterrupted() )
    //return RET_INTERRUPT;

/*
  if ( stepDirection == SD_UNKNOWN ) {
    controller->broadcastInterrupt();
    return RET_INTERRUPT;
  }
*/
  if ( stepPhase == false )
    return RET_OK;
/*
  if ( optimisticMode == false ) {
    const int32_t  tpPuls = highPulsWidth - ArdoTs::timespan(tsStartStep);
  
    // guarantee the min. pulse width
    if ( tpPuls > 0 ) CNC_STEPPER_DELAY_MICROS(tpPuls);
  }
 */
  
  // finish the step impuls
  CNC_STEPPER_WRITE_STP_PIN(PL_LOW);

  // position handling -/+1
  curPos += stepDirection;
  ARDO_TRACE_STEPPER_POS(axisLabel, curPos)
  
  return RET_OK;
}
