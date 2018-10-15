#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

	#define epsilon ((double)0.000999)

  /////////////////////////////////////////////////////////////////////////////////////
  class LastErrorCodes {
    public:
      static String gblErrorMessage;
  };


  /////////////////////////////////////////////////////////////////////////////////////
  // "Software Reset" function
  extern void (*softwareReset)(void);

  extern inline uint8_t getPinMode(uint8_t pin);
  extern PinType getPinType(uint8_t pin);

  extern void printDigitalPin(const unsigned char Pin, const int charMode);
  extern void PRINT_ANALOG_PIN(const unsigned char Pin, const int charMode);
  extern void printParameter(const unsigned char Pid, void* value );

  extern inline bool readI2CSlave(I2CData& data);

	extern inline void sleepMicroseconds(unsigned long usec);

	extern inline bool dblCompare(double a, double b);
	extern inline bool dblCompareNull(double a);

	extern inline bool convertLongToDouble(const long val, double& ret);

  extern inline void sendHeartbeat();
	extern inline void sendHeartbeat(unsigned char limitState, unsigned char supportState);

  extern inline void writeGetterValue(unsigned char pid, int32_t val1);
  extern inline void writeGetterValues(unsigned char pid, int32_t val1, int32_t val2);
	extern inline void writeGetterValues(unsigned char pid, int32_t val1, int32_t val2, int32_t val3);

	extern inline void writeByteValue(unsigned char b);
	extern inline void writeByteValue(unsigned char pid, unsigned char b);
	extern inline void writeByteValues(unsigned char pid, unsigned char b[], unsigned short size);

	extern inline void writeLongValue(long val);
	extern inline void writeLongValue(unsigned char pid, long val);
	extern inline void writeLongValues(unsigned char pid, long val1, long val2);
	extern inline void writeLongValues(unsigned char pid, long val1, long val2, long val3);
	extern inline void writeLongValues(unsigned char pid, long val1, long val2, long val3, long val4);

	extern inline void pushMessage(const char type, const unsigned char mid, const char* msg=NULL);
 
	extern inline void pushInfoMessage(const unsigned char mid,    const char* msg=NULL);
	extern inline void pushWarningMessage(const unsigned char mid, const char* msg=NULL);
	extern inline void pushErrorMessage(const unsigned char mid,   const char* msg=NULL);
	
	extern inline void pushInfoMessage(const char* msg);
	extern inline void pushWarningMessage(const char* msg);
	extern inline void pushErrorMessage(const char* msg);

#endif
