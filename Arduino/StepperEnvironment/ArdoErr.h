#ifndef ARDUINO_ERROR_H
#define ARDUINO_ERROR_H

struct LastErrorCodes {

    static const char*    messageText;
    
    static unsigned char  register1Byte_A;
    static int32_t        register4Byte_A;
    
    static void writeToSerial();

    static void clear() {
      messageText      = 0;
      register1Byte_A  = 0;
      register4Byte_A = 0L;
    }

    static bool has1TextInfo() {
      return messageText != 0;
    }

    static bool has1ByteInfos() {
      return ( register1Byte_A  != 0 );    
    }
    
    static bool has4ByteInfos() {
      return ( register4Byte_A != 0L );
    }

    static bool hasByteInfos() {
      return ( has4ByteInfos() || has1ByteInfos() );
    }
};

#endif
