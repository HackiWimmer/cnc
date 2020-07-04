#ifndef ARDUINO_CNC_CONTROLLER_H
#define ARDUINO_CNC_CONTROLLER_H

#include "DecGet.h"
#include "DecSet.h"
#include "DecMov.h"
#include "DecSeq.h"
#include "CncAcmr.h"
#include "CncRndr.h"

///////////////////////////////////////////////////////////////////////////////////
class CncAxisX;
class CncAxisY;
class CncAxisZ;

class CncArduinoController : public ArduinoCmdDecoderGetter, 
                             public ArduinoCmdDecoderSetter,
                             public ArduinoCmdDecoderMove,
                             public ArduinoCmdDecoderMoveSequence,
                             public ArduinoPositionRenderer,
                             public ArduinoAccelManager
{                                    
  private:

   typedef ArduinoImpulseCalculator ImpulseCalculator;


    CncAxisX*           X;
    CncAxisY*           Y;
    CncAxisZ*           Z;

    ImpulseCalculator   impulseCalculator;
    
    ArdoObj::I2CData    lastI2CData;

    bool                posReplyState;
    bool                probeMode;
    bool                pause;
    bool                I2CAvailable;

    uint16_t            posReplyCounter;
    uint16_t            posReplyThreshold;

    uint32_t            tsMoveStart;
    uint32_t            tsMoveLast;

    CncArduinoController(const CncArduinoController&);

    void                setProbeMode(bool state = true)                     { probeMode = state; }
    void                enableProbeMode()                                   { setProbeMode(true); }
    void                disableProbeMode()                                  { setProbeMode(false); }

    void                switchToolState(bool state, bool force = false);

    void                setupAccelProfile(const ArduinoCmdDecoderSetter::Result& st);
    
    void                setSpeedValue_MMMin(double f, bool activateAcceleration=true);

    void                setPosReplyState(bool s)                   { posReplyState = s; }
    bool                getPosReplyState()                  const  { return posReplyState; }

    void                setPosReplyThreshold(uint16_t t)           { posReplyThreshold = t; }
    uint16_t            getPosReplyThreshold()              const  { return posReplyThreshold; }

    bool                enableStepperPin(bool state = true);
    bool                disableStepperPin()                        { return enableStepperPin(false); }
    bool                enableStepperPinState();

    bool                isAnyLimitActive();
    bool                evaluateLimitStates(int8_t& xLimit, int8_t& yLimit, int8_t& zLimit);

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

    virtual byte        initialize(const ArduinoCmdDecoderMoveSequence::Result& seq);
    virtual byte        finalize  (const ArduinoCmdDecoderMoveSequence::Result& seq);

    // render interface
    virtual byte        checkRuntimeEnv();

    virtual void        notifyMovePart(int8_t dx, int8_t dy, int8_t dz);
    virtual byte        setDirection  (AxisId aid, int32_t steps);
    virtual byte        performStep   (AxisId aid);
    virtual byte        initiateStep  (AxisId aid);
    virtual byte        finalizeStep  (AxisId aid);

    // accelartor interface
    virtual void        notifyACMStateChange(State s);
    virtual void        notifyACMInitMove();

    // common functions
    bool                sendCurrentLimitStates(bool force);

    inline bool         observeSerialFrontByte(byte& retValue);

    bool                processSignal(byte sig, byte& retValue);
    void                broadcastInterrupt();

  public:
    
    CncArduinoController();
    virtual ~CncArduinoController();

    void                printConfig();
    bool                isReadyToRun();

    void                reset();

    void                turnOff();
    
    bool                isProbeMode()                       const  { return probeMode; }
    bool                isI2CAvailable()                    const  { return I2CAvailable; }
    
    bool                evaluateI2CData();
    bool                evaluateI2CAvailable();

    bool                processSignal(byte sig);
    bool                processHeartbeat(byte pid);
    
    byte                acceptGetter();
    byte                acceptSetter();
    byte                acceptMove(byte cmd);
    byte                acceptMoveSequence(byte cmd);

    // test interface
    byte                performTest();
    
};

#endif
