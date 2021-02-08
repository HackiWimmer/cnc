#ifndef ARDUINO_CNC_CONTROLLER_H
#define ARDUINO_CNC_CONTROLLER_H

#include "DecGet.h"
#include "DecSet.h"
#include "DecMov.h"
#include "DecSeq.h"
#include "CncAcmr.h"
#include "CncRndr.h"
#include "CncTest.h"

///////////////////////////////////////////////////////////////////////////////////
class CncAxisX;
class CncAxisY;
class CncAxisZ;
class CncAxisH;

class CncArduinoController : public ArduinoCmdDecoderGetter, 
                             public ArduinoCmdDecoderSetter,
                             public ArduinoCmdDecoderMove,
                             public ArduinoCmdDecoderMovePodest,
                             public ArduinoCmdDecoderMoveSequence,
                             public ArduinoPositionRenderer,
                             public ArduinoAccelManager
{                                    
  private:

    class PodestEnabler {
      public:
        PodestEnabler()  { PodestEnabler::enable();  }
        ~PodestEnabler() { PodestEnabler::disable(); }

        static void enable(bool state = true) { AE::digitalWrite(PIN_ENABLE_PODEST, state); AE::delayMicroseconds(10); }
        static void disable()                 { PodestEnabler::enable(false); }
        static bool isEnabled()               { return AE::digitalRead(PIN_ENABLE_PODEST); }
    };

    struct InteractiveMove {
      bool      active  = false;
      int8_t    valueX  = 0;
      int8_t    valueY  = 0;
      int8_t    valueZ  = 0;

      void reset() {
       *this = InteractiveMove();
      }
    
    };

    typedef bool  (*stopPodestHardware_funct)  (CncArduinoController* ctrl);
    typedef ArduinoImpulseCalculator ImpulseCalculator;

    CncAxisX*           X;
    CncAxisY*           Y;
    CncAxisZ*           Z;
    CncAxisH*           H;
    
    ArduinoTestManager* testManager;

    ImpulseCalculator   impulseCalculator;
    
    ArdoObj::I2CData    lastI2CData;

    bool                transactionState;
    bool                podestHardwareState;
    bool                posReplyState;
    bool                probeMode;
    bool                pause;
    bool                I2CAvailable;

    uint32_t            cfgF1000_MMSEC   = 0;
    uint32_t            cmsF1000_MMMin   = 0;

    uint16_t            posReplyCounter;
    uint16_t            posReplyThreshold;

    uint32_t            tsMoveStart;
    uint32_t            tsMoveLast;

    InteractiveMove     interactiveMove;  
    
    CncArduinoController(const CncArduinoController&);

    byte                initInteractiveSpeed();

    void                setProbeMode(bool state = true)                     { probeMode = state; }
    void                enableProbeMode()                                   { setProbeMode(true); }
    void                disableProbeMode()                                  { setProbeMode(false); }

    void                switchToolState(bool state, bool force = false);

    void                setupAccelProfile(const ArduinoCmdDecoderSetter::Result& st);
    void                setSpeedValue_MMSec1000(int32_t f, bool activateAcceleration=true);

    void                setPosReplyState(bool s)                   { posReplyState = s; }
    bool                getPosReplyState()                  const  { return posReplyState; }
    bool                isReplyDue()                        const  { return posReplyCounter >= posReplyThreshold; }

    void                setPosReplyThreshold(uint16_t t)           { posReplyThreshold = t; }
    uint16_t            getPosReplyThreshold()              const  { return posReplyThreshold; }

    bool                enableStepperPin(bool state = ENABLE_STATE_ON);
    bool                disableStepperPin()                        { return enableStepperPin(ENABLE_STATE_OFF); }
    bool                getEnableStepperPinState();
    
    bool                isAnyLimitActive();
    bool                evaluateLimitStates(int8_t& xLimit, int8_t& yLimit, int8_t& zLimit);

    void                sendCurrentPositions(unsigned char pid, bool force);

    byte                movePosition        (int32_t dx, int32_t dy, int32_t dz);
    byte                moveUntilLimitIsFree(int32_t dx, int32_t dy, int32_t dz);

    byte                movePodest(int32_t stepDir, stopPodestHardware_funct stopFunct);
    static bool         stopMovePodestBySignal(CncArduinoController* ctrl);
    static bool         stopMovePodestBySwitch(CncArduinoController* ctrl);

  protected:

    template<bool IMPL>
    friend class        CncArduinoStepper;
    
    friend class        ArduinoTestManager;
    friend class        CncAxisX;
    friend class        CncAxisY;
    friend class        CncAxisZ;
    friend class        CncAxisH;

    // decoder interface
    virtual byte        process(const ArduinoCmdDecoderGetter::Result& gt);
    virtual byte        process(const ArduinoCmdDecoderSetter::Result& st);
    virtual byte        process(const ArduinoCmdDecoderMove::Result& mv);
    virtual byte        process(const ArduinoCmdDecoderMoveSequence::Result& seq);
    virtual byte        process(const ArduinoCmdDecoderMovePodest::Result& mv);

    virtual byte        initialize(const ArduinoCmdDecoderMoveSequence::Result& seq);
    virtual byte        finalize  (const ArduinoCmdDecoderMoveSequence::Result& seq);

    // render interface
    virtual byte        checkRuntimeEnv();
    virtual void        notifyMovePartInit    (){}
    virtual void        notifyMovePartBefore  ();
    virtual void        notifyMovePartAfter   ();
    virtual byte        setHighPulseWidth     (AxisId aid, int32_t width);
    virtual byte        setDirection          (AxisId aid, int32_t steps);
    virtual byte        performStep           (AxisId aid);
    virtual byte        initiateStep          (AxisId aid);
    virtual byte        finalizeStep          (AxisId aid);

    // accelartor interface
    virtual void        notifyACMStateChange(State s);
    virtual void        notifyACMInitMove();

    // common functions
    bool                sendCurrentLimitStates(bool force);

    inline bool         observeSerialFrontByte(byte& retValue);

    bool                processSignal(byte sig, byte& retValue);
    bool                processSignalUpdate(byte& retValue);
    void                broadcastInterrupt();

    void                updateInteractiveMoveValues(int8_t dx, int8_t dy, int8_t dz);

  public:
    
    CncArduinoController();
    virtual ~CncArduinoController();

    void                printConfig();
    bool                isReadyToRun();

    void                reset();

    void                turnOff();

    bool                isTransactionActive()               const  { return transactionState == ON; }
    bool                isProbeMode()                       const  { return probeMode; }
    bool                isI2CAvailable()                    const  { return I2CAvailable; }
    
    bool                evaluateI2CData();
    bool                evaluateI2CAvailable();
    bool                evaluatePodestSwitch();

    bool                processSignal(byte sig);
    bool                processHeartbeat();

    byte                acceptTransaction(byte c);
    byte                activatePodestHardware(byte c);
    
    byte                acceptGetter();
    byte                acceptSetter();
    byte                acceptMove(byte cmd);
    byte                acceptMoveSequence(byte cmd);

    byte                acceptPodestMove(byte cmd);
    
    byte                acceptInteractiveMove(byte cmd);
    byte                cancelInteractiveMove(byte cmd);

    // test interface
    byte                performTest();
    
};


#endif
