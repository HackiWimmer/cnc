#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

	#define epsilon ((double)0.000999)

  /////////////////////////////////////////////////////////////////////////////////////
  class LastErrorCodes {
    public:
      static String gblErrorMessage;

      static void clear() {
        gblErrorMessage = "";
      }
  };

  #define INLINE

  /////////////////////////////////////////////////////////////////////////////////////
  // "Software Reset" function
  extern void (*softwareReset)(void);

  extern inline uint8_t getPinMode(uint8_t pin);
  extern PinType getPinType(uint8_t pin);

  extern void printDigitalPin(const unsigned char Pin, const int charMode);
  extern void PRINT_ANALOG_PIN(const unsigned char Pin, const int charMode);
  extern void printParameter(const unsigned char Pid, void* value );

  extern INLINE bool readI2CSlave(I2CData& data);

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
