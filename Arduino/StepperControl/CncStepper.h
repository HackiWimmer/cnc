#ifndef CNC_STEPPER_H
#define CNC_STEPPER_H

#include <SoftwareSerial.h>
#include "CncController.h"
#include "LastErrorCodes.h"
#include "CommonValues.h"

class CncStepper {

  private:

    static const bool NORMALIZED_INCREMENT_DIRECTION_VALUE = HIGH;
    static const bool INVERSED_INCREMENT_DIRECTION_VALUE   = LOW;

    bool INCREMENT_DIRECTION_VALUE;

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

    unsigned short dirPulseWidth;

    long pulseWidthOffset;
      
    unsigned long avgStepDuartion;

    unsigned long tsLoopRef;
    unsigned long tsLoopEnd;
    
    unsigned long tsStepRef;
    unsigned long tsStepLst;
    
    unsigned long steps;

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

    inline void calcStepDuration(unsigned long lastDuration);

    inline bool isCancelMoveSignalRelevant(const unsigned char sig);

  public:
    //////////////////////////////////////////////////////////////////////////////
    CncStepper(CncController* crtl, char a, byte stpPin, byte dirPin, byte lmtPin, LastErrorCodes& lec);
    ~CncStepper();

    //////////////////////////////////////////////////////////////////////////////
    long isReadyToRun();

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
      if ( value > 0 )  { maxReached = true; lastStepDirection = DIRECTION_INC; }
      else              { minReached = true; lastStepDirection = DIRECTION_DEC; }
    }

    //////////////////////////////////////////////////////////////////////////////
    long getPulseWidthOffset() { return pulseWidthOffset; }
    void setPulseWidthOffset(long pwo) {
      pulseWidthOffset = pwo;
      controller->setupSpeedManager();
    }

    //////////////////////////////////////////////////////////////////////////////
    unsigned long getSteps() { return steps; }
    void setSteps(unsigned long s) { 
      steps = s;
      controller->setupSpeedManager();
    }

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

    //////////////////////////////////////////////////////////////////////////////
    // The following functionality is to overrule the stepper cabling
    void normalizeIncrementDirectionValue()  { INCREMENT_DIRECTION_VALUE = NORMALIZED_INCREMENT_DIRECTION_VALUE; }
    void inverseIncrementDirectionValue()    { INCREMENT_DIRECTION_VALUE = INVERSED_INCREMENT_DIRECTION_VALUE;   }
    void setIncrementDirectionValue(long v)  { v == NORMALIZED_INCREMENT_DIRECTION ? normalizeIncrementDirectionValue() : inverseIncrementDirectionValue(); }
  
    inline bool getIncrementDirectionValue() { return INCREMENT_DIRECTION_VALUE;  }
    inline bool getDecrementDirectionValue() { return !INCREMENT_DIRECTION_VALUE; }

    //////////////////////////////////////////////////////////////////////////////
};

#endif

