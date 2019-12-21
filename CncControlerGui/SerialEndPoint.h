#ifndef SERIAL_END_POINT_H
#define SERIAL_END_POINT_H

#include <stdint.h>
#include "BaseEndPoint.h"

/////////////////////////////////////////////////////
class SerialCircularBuffer;
class SerialEndPoint : public BaseEndPoint {

	private:

		SerialCircularBuffer* rdBuffer;
		SerialCircularBuffer* wtBuffer;

		size_t printNumber(unsigned long n, uint8_t base);
		size_t printFloat(double number, uint8_t digits);

	public:

		typedef unsigned char byte;

		SerialEndPoint(SerialCircularBuffer* rb, SerialCircularBuffer* wb);
		~SerialEndPoint();

		void begin(uint32_t baud);
		void end();

		void setTimeout(uint32_t timeout);

		void purge();
		void flush();

		int available(void);
		int peek(void);
		int read(void);

		int readBytes(byte* b, int size);

		int write(byte b);
		int write(const char* str);
		int write(byte* b, int size);

	    size_t print(const char[]);
	    size_t print(char);
	    size_t print(unsigned char, int = 10);
	    size_t print(int, int = 10);
	    size_t print(unsigned int, int = 10);
	    size_t print(long, int = 10);
	    size_t print(unsigned long, int = 10);
	    size_t print(double, int = 2);

	    size_t println(const char[]);
	    size_t println(char);
	    size_t println(unsigned char, int = 10);
	    size_t println(int, int = 10);
	    size_t println(unsigned int, int = 10);
	    size_t println(long, int = 10);
	    size_t println(unsigned long, int = 10);
	    size_t println(double, int = 2);
	    size_t println(void);


		operator bool() { return true; }

};

#endif
