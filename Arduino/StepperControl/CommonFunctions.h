#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

	#define epsilon ((double)0.000999)

  /////////////////////////////////////////////////////////////////////////////////////
  class LastErrorCodes {
    public:
    
      static unsigned char  register1Byte_A;
      static unsigned char  register1Byte_B;
      static unsigned char  register1Byte_C;
      static unsigned char  register1Byte_D;

      static int32_t        register4Byte_A;
      static int32_t        register4Byte_B;
      static int32_t        register4Byte_C;
      static int32_t        register4Byte_D;
      
      static void clear() {
        register1Byte_A  = 255;
        register1Byte_B  = 255;
        register1Byte_C  = 255;
        register1Byte_D  = 255;

        register4Byte_A = MIN_LONG;
        register4Byte_B = MIN_LONG;
        register4Byte_C = MIN_LONG;
        register4Byte_D = MIN_LONG;
      }

      static bool has1ByteInfos() {
        return (
               register1Byte_A  != 255
            || register1Byte_B  != 255
            || register1Byte_C  != 255
            || register1Byte_D  != 255
        );    
      }
      
      static bool has4ByteInfos() {
        return (
               register4Byte_A != MIN_LONG
            || register4Byte_B != MIN_LONG
            || register4Byte_C != MIN_LONG
            || register4Byte_D != MIN_LONG
        );
      }

      static bool hasInfos() {
        return ( has4ByteInfos() || has1ByteInfos() );
      }
  };

  /////////////////////////////////////////////////////////////////////////////////////
  const uint32_t minSerialReadTimeoutMicros = 1000;

  
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

	extern INLINE void pushMessage(const char type, const unsigned char mid, const char* msg=NULL);
 
	extern INLINE void pushInfoMessage(const unsigned char mid,    const char* msg=NULL);
	extern INLINE void pushWarningMessage(const unsigned char mid, const char* msg=NULL);
	extern INLINE void pushErrorMessage(const unsigned char mid,   const char* msg=NULL);
	
	extern INLINE void pushInfoMessage(const char* msg);
	extern INLINE void pushWarningMessage(const char* msg);
	extern INLINE void pushErrorMessage(const char* msg);

#endif
