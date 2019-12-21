#ifndef ARDUINO_DECODER_SETTER
#define ARDUINO_DECODER_SETTER

#include <SoftwareSerial.h>
#include "ArdoVal.h"
#include "DecBase.h"
#include "MainLoop.h"

#define DEC_SETTER_DELAY_MICROS(us) \
  { \
    if ( true ) { \
      AE::delayMicroseconds(us); \
    } \
  }


// ------------------------------------------------------------------------
class ArduinoCmdDecoderSetter : public ArduinoCmdDecoderBase {

  public:
   
    struct Values {
      bool     isDouble = false;
      int32_t  l        =   0;
      double   d        = 0.0;   

      void reset() {
        *this = Values();
      }
    };

    struct Result {
      byte      pid         = PID_UNKNOWN;
      byte      valueCount  = 0;
      Values    values[MAX_SETTER_VALUES];

      void reset() {
        pid         = PID_UNKNOWN;
        valueCount  = 0;

        for( Values n : values )
          n.reset();
      }
    };
    
    ArduinoCmdDecoderSetter()
    : ArduinoCmdDecoderBase()
    , result      ()
    , b           {0,0,0,0}
    , byteCount   (0)
    , tryCount    (0)
    {}
    
    virtual ~ArduinoCmdDecoderSetter() 
    {}

  protected:
    virtual byte process(const ArduinoCmdDecoderSetter::Result& st) = 0;

  private:
    Result          result;  
    byte            b[4];
    unsigned short  byteCount;
    unsigned short  tryCount;

  public:

    // ----------------------------------------------------------------------
    byte decodeSetter() {
      result.reset();

      // Wait a protion of time. This is very importent 
      // to give the serial a chance to take a breath
      DEC_SETTER_DELAY_MICROS(1000);
      
      // error handling: pid + count
      if ( Serial.available() < 2) {
        ArduinoMainLoop::pushErrorMessage(E_INVALID_PARAM_ID);
        return RET_ERROR;
      }
  
      result.pid          = Serial.read();
      result.valueCount   = Serial.read();
      byteCount           = Serial.available();
      tryCount            = 0;
      
      while ( byteCount < result.valueCount * sizeof(int32_t) ) {
        // take the next breath to have something available 
        DEC_SETTER_DELAY_MICROS(1000);
        byteCount = Serial.available();
        
        // error handling
        if ( ++tryCount >= 5 ) {
          ArduinoMainLoop::pushErrorMessage(E_INVALID_PARAM_STREAM);
          return RET_ERROR;      
        }
      }

      // error handling
      if ( byteCount%sizeof(int32_t) != 0 || byteCount/sizeof(int32_t) > MAX_SETTER_VALUES ) {
        LastErrorCodes::clear();
        LastErrorCodes::register4Byte_A = byteCount;
        ArduinoMainLoop::pushErrorMessage(E_INVALID_PARAM_STREAM_LEN);
        return RET_ERROR;
      }

      // over all given values
      for (unsigned short i=0; i<byteCount/sizeof(int32_t); i++) {
        
        // read a 4 byte
        unsigned int size = Serial.readBytes(b, sizeof(int32_t));
        if ( size != sizeof(int32_t) ) {
          ArduinoMainLoop::pushErrorMessage(E_INVALID_PARAM_STREAM);
          return RET_ERROR;
        }

        // order the received bytes the an int32_t value
        if ( convertBytesToInt32(b, result.values[i].l) == false ) {
          ArduinoMainLoop::pushErrorMessage(E_INVALID_PARAM_STREAM);
          return RET_ERROR;
        }
        
        const bool mode = ArduinoMainLoop::isDoubleValue(result.pid);
        if ( mode )  ArduinoMainLoop::convertLongToDouble(result.values[i].l, result.values[i].d);
        else         result.values[i].d = 0.0;      
      }

      // process . . .
      byte ret = process(result);
      
      // And finally wait a protion of time 
      // to give the reader a period to do something
      DEC_SETTER_DELAY_MICROS(1000);
      return ret;
    }
};

#endif
