#ifndef ARDUINO_CNC_STEPPER_H
#define ARDUINO_CNC_STEPPER_H

#include "ArdoObj.h"

// ----------------------------------------------------------------
class CncArduinoController;

struct StepperSetup {
  CncArduinoController*   controller  = NULL;
  byte                    stpPin      = AE::PN_NOT_A_PIN;
  byte                    dirPin      = AE::PN_NOT_A_PIN;
  byte                    lmtPin      = AE::PN_NOT_A_PIN;

  StepperSetup(CncArduinoController* c, byte s, byte d, byte l)
  : controller(c)
  , stpPin(s)
  , dirPin(d)
  , lmtPin(l)
  {}
};

// ----------------------------------------------------------------
class CncArduinoStepper {

  public:
    enum StepDirection { SD_INC     = DIRECTION_INC, 
                         SD_DEC     = DIRECTION_DEC, 
                         SD_UNKNOWN = DIRECTION_UNKNOWN
                       };

  private:

    CncArduinoStepper(const CncArduinoStepper&);

  protected:
    CncArduinoController*           controller;
    ArdoObj::PwmProfile             pwmProfile;
    byte                            stpPin;
    byte                            dirPin;
    byte                            lmtPin;
    
    bool                            INCREMENT_DIRECTION_VALUE;
    bool                            interrupted;
    bool                            calculateDuration;
    bool                            minReached;
    bool                            maxReached;
    
    uint32_t                        tsPrevStep;
    uint32_t                        tsCurrStep;

    double                          pitch;
    
    uint16_t                        dirPulseWidth;
    uint16_t                        lowPulsWidth;
    uint16_t                        highPulsWidth;
    uint16_t                        minPulsWidth;

    StepDirection                   stepDirection;

    uint32_t                        steps;
    uint32_t                        avgStepDuartion;

    int32_t                         stepCounter;
    int32_t                         stepCounterOverflow;
    
    int32_t                         curPos;
    int32_t                         posReplyThresholdCount;
    
    inline void                     incStepCounter();
    inline bool                     checkLimit(int dir);
    inline void                     calcStepLoopDuration(int32_t lastDuration);
    
    explicit CncArduinoStepper(const StepperSetup& ss);
    
  public:
    virtual ~CncArduinoStepper();

    virtual char                    getAxisId()                         = 0;
    virtual unsigned char           getIncrementDirectionValuePid()     = 0;

    ArdoObj::PwmProfile&            getPwnProfile()                      { return pwmProfile; }

    void                            printConfig();
    void                            reset();
    void                            resetDirectionPin();
    void                            interrupt()                          { interrupted = true; }
    bool                            isInterrupted()               const  { return interrupted; }
    
    byte                            getDirectionPin()             const  { return dirPin; }
    byte                            getStepPin()                  const  { return stpPin; }
    byte                            getLimitPin()                 const  { return lmtPin; }
    
    bool                            isMinReached()                const  { return minReached; }
    bool                            isMaxReached()                const  { return maxReached; }
    
    uint16_t                        getLowPulseWidth()            const  { return lowPulsWidth;  }
    uint16_t                        getHighPulseWidth()           const  { return highPulsWidth; }
    void                            setLowPulseWidth(int lpw)            { lowPulsWidth  = lpw;  minPulsWidth = highPulsWidth + lowPulsWidth; }
    void                            setHighPulseWidth(int hpw)           { highPulsWidth = hpw;  minPulsWidth = highPulsWidth + lowPulsWidth; }
    
    uint32_t                        getSteps()                     const { return steps; }
    void                            setSteps(uint32_t s)                 { steps = s;    }
    
    double                          getPitch()                     const { return pitch; }
    void                            setPitch(double p)                   { pitch = p;    }
    
    void                            resetPosition()                      { setPosition(0); }
    void                            setPosition(int32_t val)             { curPos = val;   }
    int32_t                         getPosition()                  const { return curPos;  }

    void                            resetStepCounter()                   { stepCounter = MIN_LONG; stepCounterOverflow = 0L; }
    int32_t                         getStepCounter()               const { return stepCounter; }
    int32_t                         getStepCounterOverflow()       const { return stepCounterOverflow; }

    int32_t                         getPosReplyThresholdCouter()   const { return posReplyThresholdCount; }
    inline void                     incPosReplyThresholdCouter()         { posReplyThresholdCount++;      }
    void                            resetPosReplyThresholdCouter()       { posReplyThresholdCount = 0;    }

    int32_t                         getLimitState();
    int32_t                         readLimitState()                     { return readLimitState(stepDirection); }
    int32_t                         readLimitState(int dir);

    void                            setLimitStateManually(int32_t value);

    uint32_t                        getAvgStepDuration()           const  { return avgStepDuartion; }

    StepDirection                   getStepDirection()             const  { return stepDirection; }
    bool                            setDirection(int32_t steps);
    bool                            setDirection(const StepDirection stepDirection);
    byte                            performNextStep();
    
    int32_t                         isReadyToRun();    


    // The following functionality is to overrule the stepper cabling
    void                            normalizeIncrementDirectionValue()   { INCREMENT_DIRECTION_VALUE = NORMALIZED_INCREMENT_DIRECTION_VALUE; }
    void                            inverseIncrementDirectionValue()     { INCREMENT_DIRECTION_VALUE = INVERSED_INCREMENT_DIRECTION_VALUE;   }
    void                            setIncrementDirectionValue(int32_t v){ v == NORMALIZED_INCREMENT_DIRECTION ? normalizeIncrementDirectionValue() : inverseIncrementDirectionValue(); }
  
    inline bool                     getIncrementDirectionValue()   const { return INCREMENT_DIRECTION_VALUE;  }
    inline bool                     getDecrementDirectionValue()   const { return !INCREMENT_DIRECTION_VALUE; }

};

// ----------------------------------------------------------------
class CncAxisX : public CncArduinoStepper {

  private:
    CncAxisX(const CncAxisX&);

  public:
    CncAxisX(const StepperSetup& ss) 
    : CncArduinoStepper(ss)                                       {}
    virtual ~CncAxisX()                                           {}
    virtual char              getAxisId()                         { return 'X'; }
    virtual unsigned char     getIncrementDirectionValuePid()     { return PID_INC_DIRECTION_VALUE_X; }
};

// ----------------------------------------------------------------
class CncAxisY : public CncArduinoStepper {

  private:
    CncAxisY(const CncAxisY&);

  public:
    CncAxisY(const StepperSetup& ss) 
    : CncArduinoStepper(ss)                                       {}
    virtual ~CncAxisY()                                           {}
    virtual char              getAxisId()                         { return 'Y'; }
    virtual unsigned char     getIncrementDirectionValuePid()     { return PID_INC_DIRECTION_VALUE_Y; }
};

// ----------------------------------------------------------------
class CncAxisZ : public CncArduinoStepper {

  private:
    CncAxisZ(const CncAxisZ&);

  public:
    CncAxisZ(const StepperSetup& ss) 
    : CncArduinoStepper(ss)                                       {}
    virtual ~CncAxisZ()                                           {}
    virtual char              getAxisId()                         { return 'Z'; }
    virtual unsigned char     getIncrementDirectionValuePid()     { return PID_INC_DIRECTION_VALUE_Z; }
};

#endif
