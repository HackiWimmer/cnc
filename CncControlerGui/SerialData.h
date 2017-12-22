#ifndef SERIAL_DATA
#define SERIAL_DATA

#include <iostream>
#include "CncArduino.h"

/////////////////////////////////////////////////////////////////////////
struct SerialByte {
	
	// Do not add more members here as one unsignd char!
	// A SerialByte array will be uses to support
	// a memcpy(buffer, serialByte[], size)
	unsigned char c;
	
	SerialByte() 
	: c(0)
	{}
	
	explicit SerialByte(unsigned char b)
	: c(b)
	{}

	SerialByte(const SerialByte& b)
	: c(b.c)
	{}
	
	~SerialByte()
	{}
};

/////////////////////////////////////////////////////////////////////////
static const unsigned int maxSerialDataBufferSize = 128;
struct SerialData {
	
	unsigned char buffer[maxSerialDataBufferSize];
	unsigned int nbByte;
	
	SerialData()
	: nbByte(0)
	{
	}
	
	SerialData(void *b, unsigned int n)
	: nbByte(n)
	{
		if ( nbByte > 0 )
			memcpy(buffer, b, std::min(nbByte, maxSerialDataBufferSize));
	}
	
	SerialData(const SerialData& swd)
	: nbByte(swd.nbByte)
	{
		if ( nbByte > 0 )
			memcpy(buffer, swd.buffer, std::min(nbByte, maxSerialDataBufferSize));
	}
	
	~SerialData() {
	}
};

/////////////////////////////////////////////////////////////////////////
class SerialDataReader {
	
	private:
		const SerialData* p;
		unsigned int size;
		unsigned int pos;
		
	public:
		SerialDataReader()
		: p(NULL)
		, size(0)
		, pos(0)
		{
		}

		explicit SerialDataReader(const SerialData& sd)
		: p(&sd)
		, size(sd.nbByte)
		, pos(0)
		{
		}
		
		~SerialDataReader() {
		}
		
		void init(const SerialData& sd) {
			p    = &sd;
			size = p->nbByte;
			pos  = 0;
		}
		
		unsigned int available() {
			return size - pos;
		}
		
		unsigned char previewByte(unsigned int i) {
			unsigned int availableSize = available();
			
			if ( availableSize == 0 )
				return RET_NULL;
				
			if ( i > availableSize);
				return RET_NULL;
				
			return p->buffer[i];
		}
		
		unsigned char read() {
			if ( available() == 0 )
				return RET_NULL;
				
			unsigned char ret = p->buffer[pos++];
			return ret;
		}
		
		unsigned int readBytes(unsigned char* ret, unsigned int size) {
			unsigned int availableSize = available();
			
			if ( availableSize == 0 )
				return RET_NULL;
				
			size = std::min(availableSize, size);
				
			memcpy(ret, &(p->buffer[pos]), size);
			pos += size;
			
			return size;
		}
};

/////////////////////////////////////////////////////////////////////////
static const unsigned int maxSerialDataWriterBufferSize = 1024;
class SerialDataWriter {
	
	private:
		SerialByte buffer[maxSerialDataWriterBufferSize];
		SerialByte* p;
		unsigned int pos;
		
	public:
		SerialDataWriter() 
		: p(buffer)
		, pos(0)
		{
		}
	
		~SerialDataWriter() {
		}
		
		void reset() {
			p   = buffer;
			pos = 0;
		}
		
		unsigned int size() const {
			return pos;
		}
		
		const SerialByte* getBuffer() const {
			return buffer;
		}
		
		
		unsigned int available() const {
			if ( pos < maxSerialDataWriterBufferSize )
				return maxSerialDataWriterBufferSize - pos;
			
			return 0;
		}
		
		bool write(unsigned char c) {
			if ( available() < 1 )
				return false;
				
			buffer[pos].c = c;
			p++,
			pos++;
			return true;
		}
		
		bool write(const char* str) {
			if ( str == NULL )
				return false;
				
			unsigned int size = strlen(str);
			if ( available() < size )
				return false;
				
			memcpy(p, str, size);
			p   += size;
			pos += size;
			return true;
		}
		
		bool write(int32_t val) {
			const unsigned int size = sizeof(int32_t);
			
			if ( available() < size )
				return false;
			
			memcpy(p, &val, size);
			p   += size;
			pos += size;
			return true;
		}
};

#endif