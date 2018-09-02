#define epsilon ((double)0.000999)

extern inline void sleepMicroseconds(unsigned long usec);

extern inline bool dblCompare(double a, double b);
extern inline bool dblCompareNull(double a);

extern inline bool convertLongToDouble(const long val, double& ret);

extern inline void sendHeartbeat();
extern inline void sendHeartbeat(unsigned char limitState);
extern inline void sendHeartbeat(unsigned char limitState, unsigned char supportState);

extern inline void writeGetterValue(unsigned char pid, long val);
extern inline void writeGetterValues(unsigned char pid, long val1, long val2);
extern inline void writeGetterValues(unsigned char pid, long val1, long val2, long val3);

extern inline void writeByteValue(unsigned char b);
extern inline void writeByteValue(unsigned char pid, unsigned char b);
extern inline void writeByteValues(unsigned char pid, unsigned char b[], unsigned short size);

extern inline void writeLongValue(long val);
extern inline void writeLongValue(unsigned char pid, long val);
extern inline void writeLongValues(unsigned char pid, long val1, long val2);
extern inline void writeLongValues(unsigned char pid, long val1, long val2, long val3);
extern inline void writeLongValues(unsigned char pid, long val1, long val2, long val3, long val4);

extern inline void pushMessage(const char type, const char* msg);
extern inline void pushInfoMessage(const char* msg);
extern inline void pushWarningMessage(const char* msg);
extern inline void pushErrorMessage(const char* msg);

