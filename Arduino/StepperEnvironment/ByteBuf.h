#ifndef BYTE_BUFFER_H
#define BYTE_BUFFER_H

#include <SoftwareSerial.h>
class ByteBuffer {

  private:
    static const unsigned char MAX_SIZE = 32;

    bool            error;
    unsigned char   size;
    unsigned char   index;

    byte buffer[MAX_SIZE];

    ByteBuffer(const ByteBuffer&);

  public:
    ByteBuffer() : error(false), size(0), index(0), buffer()  {}
    ~ByteBuffer()                                             {}
  
    ByteBuffer& init() {
      error = false;
      size  = 0;
      index = 0;
      return *this;
    }

    bool            isOk()          { return error == false; }
    unsigned char   curIndex()      { return index;          }
    unsigned char   maxSize()       { return MAX_SIZE;       }

    ByteBuffer& put(unsigned char b) {
      if ( error == true || size + sizeof(byte) > MAX_SIZE -1 )
        return error = true, *this;

      buffer[index++] = b;
      return *this;
    }

    ByteBuffer& put(int32_t val) {
      if ( error == true || size + sizeof(int32_t) > MAX_SIZE -1 )
        return error = true, *this;

      byte* p = buffer + index;
      memcpy(p, &val, sizeof(int32_t));
      index += sizeof(int32_t);
      return *this;
    }

    int write() {
      if ( error == true)
        return 0;

      const int ret = Serial.write(buffer, index);
      init();
      return ret;
    }
};

#endif
