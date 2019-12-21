#ifndef WIRE_CIRCULAR_BUFFER_H
#define WIRE_CIRCULAR_BUFFER_H

#include "CircularBuffer.h"
typedef unsigned char 			byte;
typedef CircularBuffer<byte> 	CircularByteBuffer;

class WireCircularBuffer : public CircularByteBuffer {
public:

	WireCircularBuffer();
	virtual ~WireCircularBuffer();

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
};

#endif
