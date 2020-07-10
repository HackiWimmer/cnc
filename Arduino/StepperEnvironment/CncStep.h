#ifndef ARDUINO_CNC_STEPPER_H
#define ARDUINO_CNC_STEPPER_H

#include "ArdoObj.h"

// ----------------------------------------------------------------
class CncArduinoController;

struct StepperSetup {
  CncArduinoController*   controller  = NULL;
  byte                    stpPin      = AE::PN_NOT_A_PIN;
  byte                    dirPin      = AE::PN_NOT_A_PIN;
  byte                    llmPin      = AE::PN_NOT_A_PIN;
  byte                    hlmPin      = AE::PN_NOT_A_PIN;

  StepperSetup(CncArduinoController* c, byte s, byte d, byte ll, byte hl)
  : controller(c)
  , stpPin(s)
  , dirPin(d)
  , llmPin(ll)
  , hlmPin(hl)
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
    byte                            stpPin;
    byte                            dirPin;
    byte                            llmPin;
    byte                            hlmPin;

    bool                            INCREMENT_DIRECTION_VALUE;
    bool                            interrupted;
    bool                            stepPhase;
    
    uint32_t                        tsStartStep;

    float                           feedrate;
    
    uint16_t                        highPulsWidth;

    StepDirection                   stepDirection;

    int32_t                         curPos;
    
    inline bool                     isLimitPinRelevant();
    
    explicit CncArduinoStepper(const StepperSetup& ss);

    // Will be overriden by CncAxisX, CncAxisY and CncAxisZ
    virtual bool readMinLmtPin()   = 0;
    virtual bool readMaxLmtPin()   = 0;
    virtual void writeDirPin(bool) = 0;
    virtual void writeStpPin(bool) = 0;
    
  public:
    virtual ~CncArduinoStepper();

    virtual char                    getAxisId()                         = 0;
    virtual unsigned char           getIncrementDirectionValuePid()     = 0;

    void                            printConfig();
    void                            reset();
    void                            interrupt()                          { interrupted = true; }
    bool                            isInterrupted()               const  { return interrupted; }
    
    byte                            getDirectionPin()             const  { return dirPin; }
    byte                            getStepPin()                  const  { return stpPin; }
    byte                            getMinLimitPin()              const  { return llmPin; }
    byte                            getMaxLimitPin()              const  { return hlmPin; }

    bool                            isStepPhase()                 const  { return stepPhase;  }
    
    uint16_t                        getHighPulseWidth()           const  { return highPulsWidth; }
    void                            setHighPulseWidth(int hpw)           { highPulsWidth = hpw;  }
    
    float                           getFeedrate()                  const { return feedrate; }
    void                            setFeedrate(float f)                 { feedrate = f;    }
    
    void                            resetPosition()                      { setPosition(0); }
    void                            setPosition(int32_t val)             { curPos = val;   }
    int32_t                         getPosition()                  const { return curPos;  }

    bool                            resolveLimit();
    int8_t                          readLimitState();
    bool                            readLimitPins();

    StepDirection                   getStepDirection()             const { return stepDirection; }
    bool                            setDirection(int32_t steps);
    bool                            setDirection(const StepDirection stepDirection);

    byte                            performStep();
    byte                            initiateStep(); 
    byte                            finalizeStep();
    
    bool                            isReadyToRun();    


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

  protected:
    virtual bool readMinLmtPin()                                  { return READ_LMT_PIN_X_MIN; }  
    virtual bool readMaxLmtPin()                                  { return READ_LMT_PIN_X_MAX; }  
    virtual void writeDirPin(bool value)                          { WRITE_DIR_PIN_X            }
    virtual void writeStpPin(bool value)                          { WRITE_STP_PIN_X            }  

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

  protected:
    virtual bool readMinLmtPin()                                  { return READ_LMT_PIN_Y_MIN; }  
    virtual bool readMaxLmtPin()                                  { return READ_LMT_PIN_Y_MAX; }  
    virtual void writeDirPin(bool value)                          { WRITE_DIR_PIN_Y            }    
    virtual void writeStpPin(bool value)                          { WRITE_STP_PIN_Y            }  
    
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

  protected:
    virtual bool readMinLmtPin()                                  { return READ_LMT_PIN_Z_MIN; }  
    virtual bool readMaxLmtPin()                                  { return READ_LMT_PIN_Z_MAX; }  
    virtual void writeDirPin(bool value)                          { WRITE_DIR_PIN_Z            }      
    virtual void writeStpPin(bool value)                          { WRITE_STP_PIN_Z            }  

  public:
    CncAxisZ(const StepperSetup& ss) 
    : CncArduinoStepper(ss)                                       {}
    virtual ~CncAxisZ()                                           {}
    virtual char              getAxisId()                         { return 'Z'; }
    virtual unsigned char     getIncrementDirectionValuePid()     { return PID_INC_DIRECTION_VALUE_Z; }
};

#endif
