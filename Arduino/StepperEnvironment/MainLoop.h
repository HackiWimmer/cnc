#ifndef MAIN_LOOP_H
#define MAIN_LOOP_H

#include "ArdoEnv.h"
#include "ArdoErr.h"
#include "ArdoVal.h"
#include "ArdoObj.h"

///////////////////////////////////////////////////////
typedef void (*WriteFunctionType)(void);
class CncArduinoController;
class ArduinoMainLoop {

  private:

    CncArduinoController*  controller;

    ArduinoMainLoop(const ArduinoMainLoop&);
    
    void            printSketchVersion();
    void            printSketchTimestamp();
    byte            reset();
    void            printConfig();
    
  public:
    ArduinoMainLoop();
    ~ArduinoMainLoop();

    void setup();
    void loop();

    // common function interface
    static uint8_t      getPinMode(uint8_t pin);
    static AE::PinType  getPinType(uint8_t pin);

    static bool     readI2CSlave(ArdoObj::I2CData& data);
    static bool     sendDataToI2CSlave(const byte data[], unsigned int len);

    static void     printDigitalPin(const unsigned char Pin, const int Mode);
    static void     printAnalogPin(const unsigned char Pin, const int Mode);
    static void     printPinReport();

    static void     switchOutputPinState(int pin, bool state);
    
    static void     clearSerial();
    static void     waitActiveMilliseconds(uint32_t milli);
    static void     waitActiveMicroseconds(uint32_t micro);
    static int      waitForSerialData(uint32_t timeoutMicros=minSerialReadTimeoutMicros);

    static void     sendHeartbeat();
    static void     sendHeartbeat(unsigned char limitState, unsigned char supportState);

    static void     sleepMicroseconds(unsigned long usec);
    
    static bool     dblCompare(double a, double b);
    static bool     dblCompareNull(double a);
    
    static bool     isDoubleValue(const unsigned char pid);
    static bool     convertLongToDouble(const int32_t val, double& ret);

    static bool     peakSerial(unsigned char& c);
    static bool     checkSerialForPauseCommands(bool currentPauseState);
    static byte     readSerialByteWithTimeout(uint32_t timeoutMicros=minSerialReadTimeoutMicros);
    static int      readSerialBytesWithTimeout(byte* buffer, int length, uint32_t timeoutMicros=minSerialReadTimeoutMicros);

    static bool     readInt8 (int32_t& ret);
    static bool     readInt16(int32_t& ret);
    static bool     readInt32(int32_t& ret);

    static void     writeByteValue (unsigned char b);
    static void     writeByteValue (unsigned char pid, unsigned char b);
    static void     writeByteValues(unsigned char pid, unsigned char b[], unsigned short size);

    static void     writeLongValue (int32_t val);
    static void     writeLongValue (unsigned char pid, int32_t val);
    static void     writeLongValues(unsigned char pid, int32_t val1, int32_t val2);
    static void     writeLongValues(unsigned char pid, int32_t val1, int32_t val2, int32_t val3);
    static void     writeLongValues(unsigned char pid, int32_t val1, int32_t val2, int32_t val3, int32_t val4);

    static void     writeGetterValue (unsigned char pid, int32_t val1);
    static void     writeGetterValues(unsigned char pid, int32_t val1, int32_t val2);
    static void     writeGetterValues(unsigned char pid, int32_t val1, int32_t val2, int32_t val3);

    static void     pushMessage        (const char type, const unsigned char eid, WriteFunctionType func=LastErrorCodes::writeToSerial);
    static void     pushTextMessage    (const char type, const char* msg);
    
    static void     pushInfoMessage    (const unsigned char eid);
    static void     pushWarningMessage (const unsigned char eid);
    static void     pushErrorMessage   (const unsigned char eid);
    static void     pushDebugMessage   (const unsigned char eid);
    
    static void     pushInfoMessage    (const char* msg);
    static void     pushWarningMessage (const char* msg);
    static void     pushErrorMessage   (const char* msg);
    static void     pushDebugMessage   (const char* msg);

    static void     testLastError();
    static byte     testLab();
};

#endif
