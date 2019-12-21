#ifndef ARDUINO_ERROR_H
#define ARDUINO_ERROR_H

struct LastErrorCodes {

    static const char*    messageText;
    
    static unsigned char  register1Byte_A;
    static unsigned char  register1Byte_B;
    static unsigned char  register1Byte_C;
    static unsigned char  register1Byte_D;
    static unsigned char  register1Byte_E;
    static unsigned char  register1Byte_F;
    static unsigned char  register1Byte_G;
    static unsigned char  register1Byte_H;

    static int32_t        register4Byte_A;
    static int32_t        register4Byte_B;
    static int32_t        register4Byte_C;
    static int32_t        register4Byte_D;
    static int32_t        register4Byte_E;
    static int32_t        register4Byte_F;
    static int32_t        register4Byte_G;
    static int32_t        register4Byte_H;
    
    static void writeToSerial();

    static void clear() {
      messageText      = 0;
      
      register1Byte_A  = 0;
      register1Byte_B  = 0;
      register1Byte_C  = 0;
      register1Byte_D  = 0;
      register1Byte_E  = 0;
      register1Byte_F  = 0;

      register4Byte_A = 0L;
      register4Byte_B = 0L;
      register4Byte_C = 0L;
      register4Byte_D = 0L;
      register4Byte_E = 0L;
      register4Byte_F = 0L;
    }

    static bool has1TextInfo() {
      return messageText != 0;
    }

    static bool has1ByteInfos() {
      return (
             register1Byte_A  != 0
          || register1Byte_B  != 0
          || register1Byte_C  != 0
          || register1Byte_D  != 0
          || register1Byte_E  != 0
          || register1Byte_F  != 0
          || register1Byte_G  != 0
          || register1Byte_H  != 0
      );    
    }
    
    static bool has4ByteInfos() {
      return (
             register4Byte_A != 0L
          || register4Byte_B != 0L
          || register4Byte_C != 0L
          || register4Byte_D != 0L
          || register4Byte_E != 0L
          || register4Byte_F != 0L
          || register4Byte_G != 0L
          || register4Byte_H != 0L
      );
    }

    static bool hasByteInfos() {
      return ( has4ByteInfos() || has1ByteInfos() );
    }
};

#endif
