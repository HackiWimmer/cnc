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

  // IncrementDirectionValuePid
  byte                    idvPid      = PID_UNKNOWN;

  StepperSetup(CncArduinoController* c, byte s, byte d, byte ll, byte hl, byte p)
  : controller(c)
  , stpPin(s)
  , dirPin(d)
  , llmPin(ll)
  , hlmPin(hl)
  , idvPid(p)
  {}
};

// ----------------------------------------------------------------
template<bool IMPL>
class CncArduinoStepper {

  typedef bool  (*readPin_funct)  (void);
  typedef void  (*writePin_funct) (bool);

  public:
    enum StepDirection { SD_INC     = DIRECTION_INC, 
                         SD_DEC     = DIRECTION_DEC, 
                         SD_UNKNOWN = DIRECTION_UNKNOWN
                       };

  private:

    CncArduinoStepper(const CncArduinoStepper&);

  protected:

    const char                      axisLabel;
    const readPin_funct             readLmtPins;
    const readPin_funct             readMinLmtPin;
    const readPin_funct             readMaxLmtPin;
    const writePin_funct            writeDirPin;
    const writePin_funct            writeStpPin;

    CncArduinoController*           controller;
    const byte                      stpPin;
    const byte                      dirPin;
    const byte                      llmPin;
    const byte                      hlmPin;
    const byte                      idvPid;

    bool                            INCREMENT_DIRECTION_VALUE;
    bool                            interrupted;
    bool                            stepPhase;
    
    uint32_t                        tsStartStep;

    float                           feedrate;
    
    uint16_t                        highPulsWidth;

    StepDirection                   stepDirection;

    int32_t                         curPos;
    
    inline bool                     isLimitPinRelevant();
    
    CncArduinoStepper
    (
        const StepperSetup&     ss,
        const char              label, 
        readPin_funct           Fp_readLmtPins,
        readPin_funct           Fp_readMinLmtPin,
        readPin_funct           Fp_readMaxLmtPin,
        writePin_funct          Fp_writeDirPin,
        writePin_funct          Fp_writeStpPin
    );
        
    
  public:

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
template<> byte CncArduinoStepper<PESIMISTIC>::initiateStep();
template<> byte CncArduinoStepper<OPTIMISTIC>::initiateStep();

template<> byte CncArduinoStepper<PESIMISTIC>::finalizeStep();
template<> byte CncArduinoStepper<OPTIMISTIC>::finalizeStep();

// ----------------------------------------------------------------
class CncAxisX : public CncArduinoStepper<OPTIMISTIC> {

  private:
    CncAxisX(const CncAxisX&);

  protected:
  
    static bool readLmtPins()                                    { return READ_LMT_PIN_X;     }  
    static bool readMinLmtPin()                                  { return READ_LMT_PIN_X_MIN; }  
    static bool readMaxLmtPin()                                  { return READ_LMT_PIN_X_MAX; }  
    static void writeDirPin(bool value)                          { WRITE_DIR_PIN_X(value)     }
    static void writeStpPin(bool value)                          { WRITE_STP_PIN_X(value)     }  

  public:
    CncAxisX(const StepperSetup& ss) 
    : CncArduinoStepper
    (
        ss,
        'X',
        &CncAxisX::readLmtPins,
        &CncAxisX::readMinLmtPin,
        &CncAxisX::readMaxLmtPin,
        &CncAxisX::writeDirPin,
        &CncAxisX::writeStpPin
    )                                   
    {}
};

// ----------------------------------------------------------------
class CncAxisY : public CncArduinoStepper<OPTIMISTIC> {

  private:
    CncAxisY(const CncAxisY&);

  protected:
  
    static bool readLmtPins()                                    { return READ_LMT_PIN_Y;     }  
    static bool readMinLmtPin()                                  { return READ_LMT_PIN_Y_MIN; }  
    static bool readMaxLmtPin()                                  { return READ_LMT_PIN_Y_MAX; }  
    static void writeDirPin(bool value)                          { WRITE_DIR_PIN_Y(value)     }
    static void writeStpPin(bool value)                          { WRITE_STP_PIN_Y(value)     }  
    
  public:
    CncAxisY(const StepperSetup& ss) 
    : CncArduinoStepper
    (
        ss,
        'Y', 
        &CncAxisY::readLmtPins,
        &CncAxisY::readMinLmtPin,
        &CncAxisY::readMaxLmtPin,
        &CncAxisY::writeDirPin,
        &CncAxisY::writeStpPin
    )                                       
    {}
};

// ----------------------------------------------------------------
class CncAxisZ : public CncArduinoStepper<OPTIMISTIC> {

  private:
    CncAxisZ(const CncAxisZ&);

  protected:
   
    static bool readLmtPins()                                    { return READ_LMT_PIN_Z;     }  
    static bool readMinLmtPin()                                  { return READ_LMT_PIN_Z_MIN; }  
    static bool readMaxLmtPin()                                  { return READ_LMT_PIN_Z_MAX; }  
    static void writeDirPin(bool value)                          { WRITE_DIR_PIN_Z(value)     }      
    static void writeStpPin(bool value)                          { WRITE_STP_PIN_Z(value)     }  

  public:
    CncAxisZ(const StepperSetup& ss) 
    : CncArduinoStepper
    (
        ss,
        'Z',
        &CncAxisZ::readLmtPins,
        &CncAxisZ::readMinLmtPin,
        &CncAxisZ::readMaxLmtPin,
        &CncAxisZ::writeDirPin,
        &CncAxisZ::writeStpPin
    )                                       
    {}
};

// ----------------------------------------------------------------
class CncAxisH : public CncArduinoStepper<PESIMISTIC> {

  private:
    CncAxisH(const CncAxisH&);

  protected:
   
    static bool readLmtPins()                                    { return READ_LMT_PIN_H;     }  
    static bool readMinLmtPin()                                  { return READ_LMT_PIN_H_MIN; }  
    static bool readMaxLmtPin()                                  { return READ_LMT_PIN_H_MAX; }  
    static void writeDirPin(bool value)                          { WRITE_DIR_PIN_H(value)     }      
    static void writeStpPin(bool value)                          { WRITE_STP_PIN_H(value)     }  

  public:
    CncAxisH(const StepperSetup& ss) 
    : CncArduinoStepper
    (
        ss,
        'H',
        &CncAxisH::readLmtPins,
        &CncAxisH::readMinLmtPin,
        &CncAxisH::readMaxLmtPin,
        &CncAxisH::writeDirPin,
        &CncAxisH::writeStpPin
    )                                       
    {}
};

#endif
