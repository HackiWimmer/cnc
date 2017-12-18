#ifndef CNC_STEPPER_H
#define CNC_STEPPER_H

#include <SoftwareSerial.h>
#include "CncController.h"
#include "LastErrorCodes.h"
#include "CommonValues.h"

class CncStepper {

  private:
  
    bool initialized;
    bool interrupted;
    bool pauseStepping;
    bool minReached;
    bool maxReached;
    short minLimitCnt;
    short maxLimitCnt;
    
    byte directionPin;
    byte stepPin;
    byte limitPin;
    char axis;

    unsigned short stepPulseDurationLow;
    unsigned short stepPulseDurationHigh;
    unsigned short dirPulseDuration;

    unsigned long pulsWidthOffset;
      
    unsigned long avgStepDuartion;

    unsigned long tsLoopRef;
    unsigned long tsLoopEnd;
    
    unsigned long tsStepRef;
    unsigned long tsStepLst;
    
    unsigned long steps;
    unsigned long speedOffset;
    double speedValue_MM_MIN;

    long stepCounter;
    long stepCounterOverflow;

    short lastStepDirection;
    
    long curPos;
    CncController* controller;
    LastErrorCodes& errorInfo;

    bool validPitch;
    double pitch;
      
    void setMinReached(bool state);
    void setMaxReached(bool state);
    bool checkLimit(int dir);

    inline void sendCurrentLimitStates(bool force);
    inline void sendCurrentPositions(unsigned char pid, bool force);
    inline void broadcastInterrupt();

    void recalcSpeedOffset();

    inline void calcStepDuration(unsigned long lastDuration);

  public:
    //////////////////////////////////////////////////////////////////////////////
    CncStepper(CncController* crtl, char a, byte stpPin, byte dirPin, byte lmtPin, LastErrorCodes& lec);
    ~CncStepper();

    //////////////////////////////////////////////////////////////////////////////
    short getLastStepDirection() { return lastStepDirection; }

    //////////////////////////////////////////////////////////////////////////////
    bool isInitialized() { return initialized; }

    //////////////////////////////////////////////////////////////////////////////
    void interrupt() { interrupted = true; }
    bool isInterrupted() { return interrupted; }
    void reset();
    
    //////////////////////////////////////////////////////////////////////////////
    void pause(bool state) { pauseStepping = state; }
    
    //////////////////////////////////////////////////////////////////////////////
    long getLimitState();
    long readLimitState() { return readLimitState(lastStepDirection); }
    long readLimitState(int dir);

    //////////////////////////////////////////////////////////////////////////////
    byte getDirectionPin()  { return directionPin; }
    byte getStepPin()       { return stepPin; }

    //////////////////////////////////////////////////////////////////////////////
    bool isMinReached() { return minReached; }
    bool isMaxReached() { return maxReached; }

    //////////////////////////////////////////////////////////////////////////////
    void setLimitStateManually(long value) {
      if ( value > 0 )  { maxReached = true; lastStepDirection = +1; }
      else              { minReached = true; lastStepDirection = -1; }
    }

    //////////////////////////////////////////////////////////////////////////////
    void setPuslWidthOffset(long pwo) {
      pulsWidthOffset = pwo;
    }

    //////////////////////////////////////////////////////////////////////////////
    unsigned long getSteps() { return steps; }
    void setSteps(unsigned long s) { 
      steps = s;
      recalcSpeedOffset();
    }

    //////////////////////////////////////////////////////////////////////////////
    void setSpeedValue(double s) { 
      speedValue_MM_MIN = s; 
      recalcSpeedOffset();
    }

    //////////////////////////////////////////////////////////////////////////////
    double getSpeed_MM_MIN() { return speedValue_MM_MIN; }

    //////////////////////////////////////////////////////////////////////////////
    unsigned long getSpeedOffset() { return speedOffset; }

    //////////////////////////////////////////////////////////////////////////////
    void resetPosition() { setPosition(0); }
    void setPosition(long val) { curPos = val; }
    long getPosition() { return curPos; }

    //////////////////////////////////////////////////////////////////////////////
    void setPitch(const double p);
    double getPitch() { return pitch; }
    bool isPitchValid() { return validPitch; }

    //////////////////////////////////////////////////////////////////////////////
    long calcStepsForMM(long mm) {
      if ( isPitchValid() == false )
        return 0;

      if ( mm == 0 )
        return 0;

      return mm * steps / pitch;
    }

    //////////////////////////////////////////////////////////////////////////////
    void resetStepCounter() { stepCounter = MIN_LONG; stepCounterOverflow = 0L; }
    long getStepCounter() { return stepCounter; }
    long getStepCounterOverflow() { return stepCounterOverflow; }
    inline void incStepCounter();

    //////////////////////////////////////////////////////////////////////////////
    void printConfig();
    
    bool stepAxis(long steps, bool testActive = false);
    bool enableStepperPin(bool state = true);
    bool disableStepperPin() { return enableStepperPin(false); }

};

#endif

