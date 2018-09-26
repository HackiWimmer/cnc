#ifndef CNC_STEPPER_H
#define CNC_STEPPER_H

#include "CncController.h"
#include "LastErrorCodes.h"
#include "CommonValues.h"

class CncStepper {
  public:
    enum StepDirection { SD_INC     = DIRECTION_INC, 
                         SD_DEC     = DIRECTION_DEC, 
                         SD_UNKNOWN = DIRECTION_UNKNOWN
                       };
    
  private:
    bool INCREMENT_DIRECTION_VALUE;

    bool interrupted;
    bool calculateDuration;
    bool minReached;
    bool maxReached;
    
    short minLimitCnt;
    short maxLimitCnt;
    
    byte directionPin;
    byte stepPin;
    byte limitPin;
    
    char axis;

    uint32_t steps;
    double pitch;
    bool validPitch;
    
    unsigned int dirPulseWidth;
    unsigned int lowPulsWidth;
    unsigned int highPulsWidth;
      
    uint32_t avgStepDuartion;

    StepDirection stepDirection;
    int32_t stepCounter;
    int32_t stepCounterOverflow;
    int32_t curPos;

    int32_t posReplyThresholdCount;
    
    CncController* controller;
    LastErrorCodes& errorInfo;

    inline void incStepCounter();
        
    inline void setMinReached(bool state);
    inline void setMaxReached(bool state);
    inline bool checkLimit(int dir);

    inline void sendCurrentLimitStates(bool force);
    inline void broadcastInterrupt();

    inline void calcStepLoopDuration(uint32_t lastDuration);

  public:
    //////////////////////////////////////////////////////////////////////////////
    CncStepper(CncController* crtl, char a, byte stpPin, byte dirPin, byte lmtPin, LastErrorCodes& lec);
    ~CncStepper();

    //////////////////////////////////////////////////////////////////////////////
    int32_t isReadyToRun();    
    
    //////////////////////////////////////////////////////////////////////////////
    void setLimitStateManually(int32_t value) {
      if ( value > 0 )  { maxReached = true; stepDirection = SD_INC; }
      else              { minReached = true; stepDirection = SD_DEC; }
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void printConfig();

    //////////////////////////////////////////////////////////////////////////////
    uint32_t getAvgStepDuration()           const  { return avgStepDuartion; }

    //////////////////////////////////////////////////////////////////////////////
    StepDirection getStepDirection()        const  { return stepDirection; }

    //////////////////////////////////////////////////////////////////////////////
    void reset();
    void resetDirectionPin();
    void interrupt()                               { interrupted = true; }
    bool isInterrupted()                    const  { return interrupted; }

    //////////////////////////////////////////////////////////////////////////////
    bool isDuartionCalcActive()             const  { return calculateDuration; }
    void activateDurationCalc(bool state = true)   { calculateDuration = state; }
    void deactivateDurationCalc()                  { activateDurationCalc(false); }
    
    //////////////////////////////////////////////////////////////////////////////
    int32_t getLimitState();
    int32_t readLimitState()                       { return readLimitState(stepDirection); }
    int32_t readLimitState(int dir);

    //////////////////////////////////////////////////////////////////////////////
    byte getDirectionPin()                  const  { return directionPin; }
    byte getStepPin()                       const  { return stepPin; }

    //////////////////////////////////////////////////////////////////////////////
    bool isMinReached()                     const  { return minReached; }
    bool isMaxReached()                     const  { return maxReached; }

    //////////////////////////////////////////////////////////////////////////////
    unsigned int getLowPulseWidth()         const  { return lowPulsWidth;  }
    unsigned int getHighPulseWidth()        const  { return highPulsWidth; }
    void setLowPulseWidth(int lpw)                 { lowPulsWidth  = lpw;  }
    void setHighPulseWidth(int hpw)                { highPulsWidth = hpw;  }

    //////////////////////////////////////////////////////////////////////////////
    uint32_t getSteps()                     const  { return steps; }
    void setSteps(uint32_t s)                      { steps = s;    }

    //////////////////////////////////////////////////////////////////////////////
    void resetPosition()                           { setPosition(0); }
    void setPosition(int32_t val)                  { curPos = val;   }
    int32_t getPosition()                   const  { return curPos;  }

    //////////////////////////////////////////////////////////////////////////////
    void setPitch(const double p);
    double getPitch()                       const  { return pitch; }
    bool isPitchValid()                     const  { return validPitch; }

    //////////////////////////////////////////////////////////////////////////////
    int32_t calcStepsForMM(int32_t mm); 

    //////////////////////////////////////////////////////////////////////////////
    void resetStepCounter()                       { stepCounter = MIN_LONG; stepCounterOverflow = 0L; }
    int32_t getStepCounter()                const { return stepCounter; }
    int32_t getStepCounterOverflow()        const { return stepCounterOverflow; }

    //////////////////////////////////////////////////////////////////////////////
    bool setDirection(int32_t steps);
    bool setDirection(const StepDirection stepDirection);
    bool performNextStep();

    //////////////////////////////////////////////////////////////////////////////
    int32_t getPosReplyThresholdCouter()     const { return posReplyThresholdCount; }
    inline void incPosReplyThresholdCouter()       { posReplyThresholdCount++;      }
    void resetPosReplyThresholdCouter()            { posReplyThresholdCount = 0;    }

    //////////////////////////////////////////////////////////////////////////////
    // The following functionality is to overrule the stepper cabling
    void normalizeIncrementDirectionValue()        { INCREMENT_DIRECTION_VALUE = NORMALIZED_INCREMENT_DIRECTION_VALUE; }
    void inverseIncrementDirectionValue()          { INCREMENT_DIRECTION_VALUE = INVERSED_INCREMENT_DIRECTION_VALUE;   }
    void setIncrementDirectionValue(int32_t v)     { v == NORMALIZED_INCREMENT_DIRECTION ? normalizeIncrementDirectionValue() : inverseIncrementDirectionValue(); }
  
    inline bool getIncrementDirectionValue() const { return INCREMENT_DIRECTION_VALUE;  }
    inline bool getDecrementDirectionValue() const { return !INCREMENT_DIRECTION_VALUE; }

    //////////////////////////////////////////////////////////////////////////////
};

#endif

