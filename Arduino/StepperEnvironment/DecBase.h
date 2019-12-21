#ifndef ARDUINO_DECODER_BASE
#define ARDUINO_DECODER_BASE

class ArduinoCmdDecoderBase {

  public:
    ArduinoCmdDecoderBase() {}
    virtual ~ArduinoCmdDecoderBase() {}


    // ----------------------------------------------------------------
    static bool convertBytesToInt8(byte* b, int8_t& ret ) {
      if ( b == NULL )
        return false;

      ret  = (int8_t)b[0];
      return true;
    }

    // ----------------------------------------------------------------
    static bool convertBytesToInt16(byte* b, int16_t& ret ) {
      if ( b == NULL )
        return false;

      // --------------------------------------------------------------
      // order bytes:
      // expect little endian on the sender side
      // 1 0 --> ============ -->        1        0 
      //                          --> b[1] = 1 b[0] = 0
      ret  = (int32_t)b[1] <<  8;
      ret += (int32_t)b[0];

      return true;
    }

    // ----------------------------------------------------------------
    static bool convertBytesToInt32(byte* b, int32_t& ret ) {
      if ( b == NULL )
        return false;

      // --------------------------------------------------------------
      // order bytes:
      // expect little endian on the sender side
      // 3 2 1 0 --> ============ -->        3        2        1        0 
      //                          --> b[3] = 3 b[2] = 2 b[1] = 1 b[0] = 0
      ret  = (int32_t)b[3] << 24;
      ret += (int32_t)b[2] << 16;
      ret += (int32_t)b[1] <<  8;
      ret += (int32_t)b[0];

      return true;
    }
};

#endif
