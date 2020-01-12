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
      typedef ArduinoMainLoop AML;
      
      result.reset();

      byte b[1];
      if ( AML::readSerialBytesWithTimeout(b, 1) == 1 )
        result.pid = b[0];
        
      // process . . .
      return process(result);
    }
};

#endif
