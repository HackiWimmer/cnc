#ifndef ARDUINO_CNC_CONTROLLER_H
#define ARDUINO_CNC_CONTROLLER_H

#include "DecGet.h"
#include "DecSet.h"
#include "DecMov.h"
#include "DecSeq.h"
#include "CncRndr.h"
#include "CncPwmr.h"

///////////////////////////////////////////////////////////////////////////////////
class CncAxisX;
class CncAxisY;
class CncAxisZ;

class CncArduinoController : public ArduinoCmdDecoderGetter, 
                             public ArduinoCmdDecoderSetter,
                             public ArduinoCmdDecoderMove,
                             public ArduinoCmdDecoderMoveSequence,
                             public ArduinoPositionRenderer,
                             public ArduinoPulsWidthManager
{                                    
  private:

    CncAxisX*           X;
    CncAxisY*           Y;
    CncAxisZ*           Z;
    
    ArdoObj::I2CData    lastI2CData;

    bool                posReplyState;
    bool                probeMode;
    bool                pause;
    bool                I2CAvailable;
    
    int32_t             posReplyThresholdX;
    int32_t             posReplyThresholdY;
    int32_t             posReplyThresholdZ;

    int32_t             positionCounter;
    int32_t             positionCounterOverflow;

    uint32_t            lastHeartbeat;
    
    CncArduinoController(const CncArduinoController&);

    void                setProbeMode(bool state = true)                     { probeMode = state; }
    void                enableProbeMode()                                   { setProbeMode(true); }
    void                disableProbeMode()                                  { setProbeMode(false); }

    void                switchToolState(bool state, bool force = false);
    void                switchStepperState(bool state);
    
    void                setupAccelProfile(int32_t v1, int32_t v2, int32_t v3, int32_t v4, int32_t v5, int32_t v6);
    
    void                setSpeedValue(double fm, bool activateAcceleration=true);

    void                setPosReplyState(bool s)                   { posReplyState = s; }
    bool                getPosReplyState()                  const  { return posReplyState; }

    void                setPosReplyThresholdX(long t)              { posReplyThresholdX = t; }
    void                setPosReplyThresholdY(long t)              { posReplyThresholdY = t; }
    void                setPosReplyThresholdZ(long t)              { posReplyThresholdZ = t; }
    
    int32_t             getPosReplyThresholdX()             const  { return posReplyThresholdX; }
    int32_t             getPosReplyThresholdY()             const  { return posReplyThresholdY; }
    int32_t             getPosReplyThresholdZ()             const  { return posReplyThresholdZ; }

    void                resetPositionCounter()                     { positionCounter = MIN_LONG; positionCounterOverflow = 0L; }
    int32_t             getPositionCounter()                const  { return positionCounter; }
    int32_t             getPositionCounterOverflow()        const  { return positionCounterOverflow; }
    inline void         incPositionCounter();

    bool                enableStepperPin(bool state = true);
    bool                disableStepperPin()                        { return enableStepperPin(false); }

    bool                isAnyLimitActive();
    bool                evaluateLimitStates(int32_t& xLimit, int32_t& yLimit, int32_t& zLimit);

    void                sendCurrentPositions(unsigned char pid, bool force);

    byte                movePosition   (int32_t dx, int32_t dy, int32_t dz);
    byte                moveUntilSignal(int32_t dx, int32_t dy, int32_t dz);

  protected:

    friend class        CncArduinoStepper;
    friend class        CncAxisX;
    friend class        CncAxisY;
    friend class        CncAxisZ;

    // decoder interface
    virtual byte        process(const ArduinoCmdDecoderGetter::Result& gt);
    virtual byte        process(const ArduinoCmdDecoderSetter::Result& st);
    virtual byte        process(const ArduinoCmdDecoderMove::Result& mv);
    virtual byte        process(const ArduinoCmdDecoderMoveSequence::Result& seq);

    // render interface
    virtual void        notifyPositionChange();
    virtual byte        checkRuntimeEnv();

    virtual byte        setDirectionX(int32_t steps);
    virtual byte        setDirectionY(int32_t steps);
    virtual byte        setDirectionZ(int32_t steps);
    
    virtual byte        performNextStepX();
    virtual byte        performNextStepY();
    virtual byte        performNextStepZ();

    // common functions
    bool                sendCurrentLimitStates(bool force);

    inline bool         observeSerialFrontByte(byte& retValue);
    inline bool         observeEnablePin();
    inline bool         observeToolState();

    bool                processSignal(byte sig, byte& retValue);
    void                broadcastInterrupt();

  public:
    
    CncArduinoController();
    virtual ~CncArduinoController();

    void                printConfig();
    int32_t             isReadyToRun();

    void                reset();
    
    bool                isProbeMode()                       const  { return probeMode; }
    bool                isI2CAvailable()                    const  { return I2CAvailable; }
    bool                isSpeedControllerActive()           const  { return false /*speedController.isSpeedConfigured()*/; }
    
    bool                evaluateI2CData();
    bool                evaluateI2CAvailable();

    bool                processSignal(byte sig);
    bool                processHeartbeat();
    bool                processIdle();
    
    byte                processGetter();
    byte                processSetter();
    byte                processMove(byte cmd);
};

#endif
