#ifndef CNC_CONTROLLER_H
#define CNC_CONTROLLER_H

#include "LastErrorCodes.h"

class CncStepper;
class CncController {

  private:
    
    CncStepper* X;
    CncStepper* Y;
    CncStepper* Z;

    LastErrorCodes* errorInfo;

    int minEnablePulseWide;

    long posReplyThresholdX;
    long posReplyThresholdY;
    long posReplyThresholdZ;

    long lastSendPositionX;
    long lastSendPositionY;
    long lastSendPositionZ;

    long positionCounter;
    long positionCounterOverflow;
    
    bool posReplyState;
    bool probeMode;

    inline bool renderAndStepAxisXY(long x1, long y1);
    inline bool renderAndStepAxisXYZ(long x1, long y1, long z1);
    inline bool moveXYZ();

  public:
    //////////////////////////////////////////////////////////////////////////////
    CncController(LastErrorCodes& lec);
    ~CncController();

    //////////////////////////////////////////////////////////////////////////////
    // this method id used by the CNC stepper to broadbast the current stepper limit information
    bool sendCurrentLimitStates(bool force = false);
    
    //////////////////////////////////////////////////////////////////////////////
    // this method id used by the main loop
    bool evaluateAndSendLimitStates();
    
    //////////////////////////////////////////////////////////////////////////////
    void sendCurrentPositions(unsigned char pid, bool force);
    void broadcastInterrupt();

    //////////////////////////////////////////////////////////////////////////////
    void broadcastPause(bool state);

    //////////////////////////////////////////////////////////////////////////////
    CncStepper* getStepperX() { return X; }
    CncStepper* getStepperY() { return Y; }
    CncStepper* getStepperZ() { return Z; }

    //////////////////////////////////////////////////////////////////////////////
    void reset();

    //////////////////////////////////////////////////////////////////////////////
    void setPosReplyState(bool s) { posReplyState = s; }
    bool getPosReplyState() { return posReplyState; }

    void setPosReplyThresholdX(long t) { posReplyThresholdX = t; }
    void setPosReplyThresholdY(long t) { posReplyThresholdY = t; }
    void setPosReplyThresholdZ(long t) { posReplyThresholdZ = t; }
    
    long getPosReplyThresholdX() { return posReplyThresholdX; }
    long getPosReplyThresholdY() { return posReplyThresholdY; }
    long getPosReplyThresholdZ() { return posReplyThresholdZ; }

    //////////////////////////////////////////////////////////////////////////////
    // to get the whole unsigned int32_t range the controller starts counting with MIN_LONG
    void resetPositionCounter() { positionCounter = MIN_LONG; positionCounterOverflow = 0L; }
    long getPositionCounter() { return positionCounter; }
    long getPositionCounterOverflow() { return positionCounterOverflow; }
    inline void incPositionCounter();

    //////////////////////////////////////////////////////////////////////////////
    

    //////////////////////////////////////////////////////////////////////////////
    bool stepAxisX(long x);
    bool stepAxisY(long y);
    bool stepAxisZ(long z);
    bool stepAxisXYZ(long x1, long y1, long z1, bool render);

    //////////////////////////////////////////////////////////////////////////////
    void printConfig();

    //////////////////////////////////////////////////////////////////////////////
    bool isProbeMode() { return probeMode; }
    void setProbeMode(bool state = true) { probeMode = state; }
    void enableProbeMode() { setProbeMode(true); }
    void disableProbeMode() { setProbeMode(false); }
    
    //////////////////////////////////////////////////////////////////////////////
    bool enableStepperPin(bool state = true);
    bool disableStepperPin() { return enableStepperPin(false); }
};

#endif

