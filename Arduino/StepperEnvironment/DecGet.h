#ifndef ARDUINO_DECODER_GETTER
#define ARDUINO_DECODER_GETTER

#include <SoftwareSerial.h>
#include "ArdoVal.h"
#include "DecBase.h"
#include "MainLoop.h"

// ------------------------------------------------------------------------
class ArduinoCmdDecoderGetter : public ArduinoCmdDecoderBase {

  public:
   
    struct Result {
      byte pid = PID_UNKNOWN;

      void reset() {
        pid = PID_UNKNOWN;
      }
    };
    
    ArduinoCmdDecoderGetter()
    : ArduinoCmdDecoderBase()
    , result()
    {}
    
    virtual ~ArduinoCmdDecoderGetter() 
    {}

  protected:
    virtual byte process(const ArduinoCmdDecoderGetter::Result& gt) = 0;

  private:
    Result result;  

  public:

    // ----------------------------------------------------------------------
    byte decodeGetter() {
      result.reset();

      // Wait a protion of time. This is very importent 
      // to give the serial a chance to take a breath
      AE::delayMicroseconds(1000);

      if ( Serial.available() > 0 )
        result.pid = Serial.read();
        
      // process . . .
      return process(result);
    }
};

#endif
