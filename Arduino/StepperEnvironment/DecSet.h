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
      bool     isFloat  = false;
      int32_t  l        =   0;
      float    f        = 0.0;   

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
      typedef ArduinoMainLoop AML;
      
      result.reset();

      if ( AML::readSerialBytesWithTimeout(b, 2) != 2 ) {
        ArduinoMainLoop::pushMessage(MT_ERROR, E_INVALID_PARAM_ID);
        return RET_ERROR;
      }

      result.pid          = b[0];
      result.valueCount   = b[1];
      
      // over all given values
      for (unsigned short i=0; i<result.valueCount; i++) {
        // read a 4 byte
        if ( AML::readSerialBytesWithTimeout(b, sizeof(int32_t)) != sizeof(int32_t) ) {
          ArduinoMainLoop::pushMessage(MT_ERROR, E_INVALID_PARAM_STREAM);
          return RET_ERROR;
        }
        
        // order the received bytes the an int32_t value
        if ( convertBytesToInt32(b, result.values[i].l) == false ) {
          ArduinoMainLoop::pushMessage(MT_ERROR, E_INVALID_PARAM_STREAM);
          return RET_ERROR;
        }
        
        const bool mode = ArduinoMainLoop::isFloatValue(result.pid);
        if ( mode )  ArduinoMainLoop::convertLongToFloat(result.values[i].l, result.values[i].f);
        else         result.values[i].f = 0.0;      
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
