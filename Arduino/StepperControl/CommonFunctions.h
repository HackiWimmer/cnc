#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

#define epsilon ((double)0.000999)

typedef void (*WriteFunctionType)(void);

/////////////////////////////////////////////////////////////////////////////////////
struct LastErrorCodes {

    static const char*    messageText;
    
    static unsigned char  register1Byte_A;
    static unsigned char  register1Byte_B;
    static unsigned char  register1Byte_C;
    static unsigned char  register1Byte_D;
    static unsigned char  register1Byte_E;
    static unsigned char  register1Byte_F;
    static unsigned char  register1Byte_G;
    static unsigned char  register1Byte_H;

    static int32_t        register4Byte_A;
    static int32_t        register4Byte_B;
    static int32_t        register4Byte_C;
    static int32_t        register4Byte_D;
    static int32_t        register4Byte_E;
    static int32_t        register4Byte_F;
    static int32_t        register4Byte_G;
    static int32_t        register4Byte_H;
    
    static void writeSerialToSerial();
    static void writeToSerial();

    static void clear() {
      messageText      = NULL;
      
      register1Byte_A  = 0;
      register1Byte_B  = 0;
      register1Byte_C  = 0;
      register1Byte_D  = 0;
      register1Byte_E  = 0;
      register1Byte_F  = 0;

      register4Byte_A = 0L;
      register4Byte_B = 0L;
      register4Byte_C = 0L;
      register4Byte_D = 0L;
      register4Byte_E = 0L;
      register4Byte_F = 0L;
    }

    static bool has1TextInfo() {
      return messageText != NULL;
    }

    static bool has1ByteInfos() {
      return (
             register1Byte_A  != 0
          || register1Byte_B  != 0
          || register1Byte_C  != 0
          || register1Byte_D  != 0
          || register1Byte_E  != 0
          || register1Byte_F  != 0
          || register1Byte_G  != 0
          || register1Byte_H  != 0
      );    
    }
    
    static bool has4ByteInfos() {
      return (
             register4Byte_A != 0L
          || register4Byte_B != 0L
          || register4Byte_C != 0L
          || register4Byte_D != 0L
          || register4Byte_E != 0L
          || register4Byte_F != 0L
          || register4Byte_G != 0L
          || register4Byte_H != 0L
      );
    }

    static bool hasByteInfos() {
      return ( has4ByteInfos() || has1ByteInfos() );
    }
};

/////////////////////////////////////////////////////////////////////////////////////
const uint32_t minSerialReadTimeoutMicros = 10L * 1000L;


/////////////////////////////////////////////////////////////////////////////////////
// "Software Reset" function
extern void (*softwareReset)(void);

extern inline uint8_t getPinMode(uint8_t pin);
extern PinType getPinType(uint8_t pin);

extern void waitActiveMilliseconds(uint32_t milli);
extern void waitActiveMicroseconds(uint32_t micro);
extern int waitForSerialData(uint32_t timeout=minSerialReadTimeoutMicros);

extern void printDigitalPin(const unsigned char Pin, const int charMode);
extern void PRINT_ANALOG_PIN(const unsigned char Pin, const int charMode);
extern void printParameter(const unsigned char Pid, void* value );

extern INLINE bool readI2CSlave(I2CData& data);

extern INLINE byte readSerialByteWithTimeout(uint32_t timeoutMicros=minSerialReadTimeoutMicros);
extern INLINE int  readSerialBytesWithTimeout(byte* buffer, int length, uint32_t timeoutMicros=minSerialReadTimeoutMicros);

extern INLINE void sleepMicroseconds(unsigned long usec);

extern INLINE bool dblCompare(double a, double b);
extern INLINE bool dblCompareNull(double a);

extern INLINE bool isDoubleValue(const unsigned char pid);
extern INLINE bool convertLongToDouble(const long val, double& ret);

extern INLINE void sendHeartbeat();
extern INLINE void sendHeartbeat(unsigned char limitState, unsigned char supportState);

extern INLINE void writeGetterValue(unsigned char pid, int32_t val1);
extern INLINE void writeGetterValues(unsigned char pid, int32_t val1, int32_t val2);
extern INLINE void writeGetterValues(unsigned char pid, int32_t val1, int32_t val2, int32_t val3);

extern INLINE void writeByteValue(unsigned char b);
extern INLINE void writeByteValue(unsigned char pid, unsigned char b);
extern INLINE void writeByteValues(unsigned char pid, unsigned char b[], unsigned short size);

extern INLINE void writeLongValue(long val);
extern INLINE void writeLongValue(unsigned char pid, long val);
extern INLINE void writeLongValues(unsigned char pid, long val1, long val2);
extern INLINE void writeLongValues(unsigned char pid, long val1, long val2, long val3);
extern INLINE void writeLongValues(unsigned char pid, long val1, long val2, long val3, long val4);

extern INLINE void pushMessage(const char type, const unsigned char eid, WriteFunctionType func=NULL);
extern INLINE void pushTextMessage(const char type, const char* msg);

extern INLINE void pushInfoMessage(const unsigned char eid)       { pushMessage(MT_INFO, eid);    }
extern INLINE void pushWarningMessage(const unsigned char eid)    { pushMessage(MT_WARNING, eid); }
extern INLINE void pushErrorMessage(const unsigned char eid)      { pushMessage(MT_ERROR, eid);   }
extern INLINE void pushDebugMessage(const unsigned char eid)      { pushMessage(MT_DEBUG, eid);   }

extern INLINE void pushInfoMessage(const char* msg)               { pushTextMessage(MT_INFO, msg);    }
extern INLINE void pushWarningMessage(const char* msg)            { pushTextMessage(MT_WARNING, msg); }
extern INLINE void pushErrorMessage(const char* msg)              { pushTextMessage(MT_ERROR, msg);   }
extern INLINE void pushDebugMessage(const char* msg)              { pushTextMessage(MT_DEBUG, msg);   }

extern INLINE bool readInt8(int32_t& ret);
extern INLINE bool readInt16(int32_t& ret);
extern INLINE bool readInt32(int32_t& ret);

#endif
