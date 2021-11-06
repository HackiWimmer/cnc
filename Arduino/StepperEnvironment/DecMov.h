#ifndef ARDUINO_DECODER_MOVE
#define ARDUINO_DECODER_MOVE

#include <SoftwareSerial.h>
#include "ArdoVal.h"
#include "ArdoObj.h"
#include "DecBase.h"
#include "MainLoop.h"

// ------------------------------------------------------------------------
class ArduinoCmdDecoderMove : public ArduinoCmdDecoderBase {

  public:
    static const uint8_t MAX_MOVE_VALUES = 3;
    
    struct Result {
      byte        cmd = CMD_INVALID;
      int32_t     dx  = 0L;
      int32_t     dy  = 0L;
      int32_t     dz  = 0L;

      void reset() {
        cmd             = CMD_INVALID;
        dx = dy = dz    = 0L;
      }
    };
    
    ArduinoCmdDecoderMove()
    : ArduinoCmdDecoderBase()
    , result  ()
    {}
    
    virtual ~ArduinoCmdDecoderMove() 
    {}

  protected:
    virtual byte process(const ArduinoCmdDecoderMove::Result& mv) = 0;

  private:
  
    Result          result;  

    // ----------------------------------------------------------------------
    void reset() {
      result.reset();
    }

  public:

    // ----------------------------------------------------------------------
    byte decodeMove(byte cmd) {
      typedef ArduinoMainLoop AML;

      reset();
      result.cmd = cmd;  
      
      AML::waitForSerialData();

      const uint8_t valueCount = (uint8_t)AML::readSerialByteWithTimeout();
      if ( valueCount < 1 || valueCount > MAX_MOVE_VALUES ) {
        AML::pushMessage(MT_ERROR, E_INVALID_MOVE_CMD);
        return RET_ERROR;        
      }

      byte b[sizeof(int32_t)];
      for (uint8_t i=0; i<ArdoObj::minimum(valueCount, MAX_MOVE_VALUES); i++) {
        
        const uint8_t size = AML::readSerialBytesWithTimeout(b, sizeof(int32_t));
        if ( size != sizeof(int32_t) ) {
          AML::pushMessage(MT_ERROR, E_INVALID_MOVE_CMD);
          return RET_ERROR;
        }

        // order the received bytes the an int32_t value
        int32_t value = 0L;
        if ( convertBytesToInt32(b, value) == false ) {
          AML::pushMessage(MT_ERROR, E_INVALID_MOVE_CMD);
          return RET_ERROR;
        }

        // delegate values
        switch ( valueCount ) {
          // axis Z Only
          case 1: {
            result.dz = value; 
            break;
          }
          // axis X and Y
          case 2: {   
            switch ( i ) {
              case 0: result.dx = value; break;
              case 1: result.dy = value; break;
            }
            break;
          }
          // axis X, Y and Z
          case 3: {   
            switch ( i ) {
              case 0: result.dx = value; break;
              case 1: result.dy = value; break;
              case 2: result.dz = value; break;
            }
            break;
          }
        }
      }

      // process . . .
      return process(result);
    }
};

// ------------------------------------------------------------------------
class ArduinoCmdDecoderMovePodium : public ArduinoCmdDecoderBase {

  public:
    static const uint8_t MAX_MOVE_VALUES = 1;
    
    struct Result {
      byte     cmd = CMD_INVALID;
      int32_t  dh  = 0L;

      void reset() {
        cmd        = CMD_INVALID;
        dh         = 0L;
      }
    };
    
    ArduinoCmdDecoderMovePodium()
    : ArduinoCmdDecoderBase()
    , result  ()
    {}
    
    virtual ~ArduinoCmdDecoderMovePodium() 
    {}

  protected:
    virtual byte process(const ArduinoCmdDecoderMovePodium::Result& mv) = 0;

  private:
  
    Result          result;  

    // ----------------------------------------------------------------------
    void reset() {
      result.reset();
    }

  public:

    // ----------------------------------------------------------------------
    byte decodeMovePodium(byte cmd) {
      typedef ArduinoMainLoop AML;

      reset();
      result.cmd = cmd;  

      AML::waitForSerialData();

      const uint8_t valueCount = (uint8_t)AML::readSerialByteWithTimeout();
      if ( valueCount < 1 || valueCount > MAX_MOVE_VALUES ) {
        AML::pushMessage(MT_ERROR, E_INVALID_MOVE_CMD);
        return RET_ERROR;        
      }

      byte b[sizeof(int32_t)];
      for (uint8_t i=0; i<ArdoObj::minimum(valueCount, MAX_MOVE_VALUES); i++) {
        
        const uint8_t size = AML::readSerialBytesWithTimeout(b, sizeof(int32_t));
        if ( size != sizeof(int32_t) ) {
          AML::pushMessage(MT_ERROR, E_INVALID_MOVE_CMD);
          return RET_ERROR;
        }

        // order the received bytes the an int32_t value
        int32_t value = 0L;
        if ( convertBytesToInt32(b, value) == false ) {
          AML::pushMessage(MT_ERROR, E_INVALID_MOVE_CMD);
          return RET_ERROR;
        }

        // delegate values
        switch ( valueCount ) {
          // axis H Only
          case 1: {
            result.dh = value; 
            break;
          }
        }
      }

      // process . . .
      return process(result);
    }
};

#endif
