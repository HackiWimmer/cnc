#ifndef SERIAL_CIRCULAR_BUFFER_H
#define SERIAL_CIRCULAR_BUFFER_H

#include "CircularBuffer.h"
typedef unsigned char 			byte;
typedef CircularBuffer<byte> 	CircularByteBuffer;

class SerialCircularBuffer : public CircularByteBuffer {
	
	public:
		
		SerialCircularBuffer();
		virtual ~SerialCircularBuffer();
		
		void	setTimeout(unsigned int timeout)
		{ circularBuffer.setTimeout(timeout); }

		void 	clear()		{ circularBuffer.clear(); }
		int 	capacity()	{ return circularBuffer.capacity(); }
		int 	size()		{ return circularBuffer.size(); }

		int 	peak();
		int 	read();
		int 	write(byte b);

		bool 	write(byte* buffer, int size);
		int 	read(byte* buffer, int size);
		
	private:
	
		CircularByteBuffer circularBuffer;
		
	public: 
		static void test(int testId);
		static void testR1();
		static void testR2();

};

#endif

