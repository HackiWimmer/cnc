#ifndef WIRE_END_POINT_H
#define WIRE_END_POINT_H

#include <stdint.h>
#include "BaseEndPoint.h"

/////////////////////////////////////////////////////
class SerialThread;
class WireCircularBuffer;
class WireEndPoint : public BaseEndPoint {

	private:

		WireCircularBuffer* rdBuffer;
		WireCircularBuffer* wtBuffer;

		// hide this functions, because they are currently not 
		// implemented by the arduino reference
		int peek(void);
		int readBytes(byte* b, int size);
		int write(const char* str);
		void flush();
		
	public:

		typedef unsigned char byte;

		WireEndPoint(WireCircularBuffer* rb, WireCircularBuffer* wb);
		~WireEndPoint();

		void begin();
		void end();
		
		void beginTransmission(int deviceID);
		byte endTransmission(bool stop=false);

		void purge();

		int available(void);
		
		int read(void);
		int write(byte b);
		int write(const byte* b, int size);

		int requestFrom(const int id, const unsigned int bCount);

		operator bool() { return true; }

};


#endif
