#ifndef ARDUINO_DECODER_MOVE_SEQUENCE
#define ARDUINO_DECODER_MOVE_SEQUENCE

#include <SoftwareSerial.h>
#include "ArdoVal.h"
#include "ArdoObj.h"
#include "DecBase.h"
#include "MainLoop.h"

// ------------------------------------------------------------------------
class ArduinoCmdDecoderMoveSequence : public ArduinoCmdDecoderBase {

  public:
   
    struct Result {

      ArdoObj::ValueInfo vi        = ArdoObj::ValueInfo(0);

      byte pid                     = PID_UNKNOWN;
      byte ret                     = RET_OK;
      byte portionSize             = 0;
      byte portionCounter          = 0;
  
      int32_t totalLength          = 0; 
      int32_t totalIndex           = 0;
      int32_t totalRemaining       = 0;

      int32_t lengthX              = 0; 
      int32_t lengthY              = 0; 
      int32_t lengthZ              = 0; 
  
      int32_t dx                   = 0;
      int32_t dy                   = 0;
      int32_t dz                   = 0;
      int32_t f                    = 0;

      void reset() {
        *this = Result();
      }

    };
    
    ArduinoCmdDecoderMoveSequence()
    : ArduinoCmdDecoderBase()
    , result()
    {}
    
    virtual ~ArduinoCmdDecoderMoveSequence() 
    {}

  protected:
    virtual byte process(const ArduinoCmdDecoderMoveSequence::Result& seq) = 0;

  private:
    Result result;  

    // ----------------------------------------------------------------------
    void debugValues(unsigned short idx) {
  
        LastErrorCodes::clear();
        LastErrorCodes::register1Byte_A = idx;
        LastErrorCodes::register1Byte_B = result.pid;
        LastErrorCodes::register1Byte_C = result.portionSize;
        LastErrorCodes::register1Byte_D = result.portionCounter;
  
        LastErrorCodes::register1Byte_H = (unsigned char)Serial.available();
  
        LastErrorCodes::register4Byte_A = result.totalIndex;
        LastErrorCodes::register4Byte_B = result.totalLength;
        LastErrorCodes::register4Byte_C = result.totalRemaining;
        
        LastErrorCodes::register4Byte_E = result.dx;
        LastErrorCodes::register4Byte_F = result.dy;
        LastErrorCodes::register4Byte_G = result.dz;
        LastErrorCodes::register4Byte_H = result.f;
    }
  
    // ----------------------------------------------------------------------
    void logInfo(unsigned char idx, unsigned char eid) {
        debugValues(idx);
        ArduinoMainLoop::pushMessage(MT_DEBUG, eid, LastErrorCodes::writeToSerial);
    }
  
    // ----------------------------------------------------------------------
    byte logError(unsigned char idx, unsigned char eid) {
        debugValues(idx);
        ArduinoMainLoop::pushMessage(MT_ERROR, eid, LastErrorCodes::writeToSerial);
        ArduinoMainLoop::clearSerial();
        
        return RET_ERROR;
    }

    // ----------------------------------------------------------------------
    short parsePortion() {
      result.portionCounter++;
  
      int portionIndex = result.portionSize;
      while ( portionIndex > 0 ) {
        
        const int valueSize = parseValues();
        if ( valueSize <= 0 ) {
          logError(30, E_INVALID_MOVE_SEQUENCE);
          return -1;
        }
        
        portionIndex           -= valueSize;  
        result.totalRemaining  -= valueSize;  
      }  
  
      return 0;
    }

    // ------------------------------------------------------------------------------
    int parseValues() {
      
      // determine pid and paring rules
      result.pid = ArduinoMainLoop::readSerialByteWithTimeout();
      result.vi.set(result.pid);
      
      if ( result.vi.isValid() == false ) {
        logError(40, E_INVALID_MOVE_SEQUENCE);
        return -1;
      }
      
      unsigned int byteCount = result.vi.getByteCount();
      unsigned int valCount  = result.vi.getValueCount();
      int totalSize          = 1 + ( byteCount != 0 ? valCount * byteCount : 1 );
      int readIndex          = totalSize - 1;
  
      int32_t v[ArdoObj::ValueInfo::MaxValueCount]; 
      v[0] = v[1] = v[2] = 0; v[3] = 0;
      
      byte b;
      unsigned short count = 0;
      
      while ( count < valCount ) {
        
        // copy bytes
        switch ( byteCount ){
    
          // format:   bit: 78543210    
          //                  zzyyxx
          //                  -+-+-+ 
          //                          bit  +               bit  -     0
          case 0:   b = ArduinoMainLoop::readSerialByteWithTimeout();
                    v[0] = bitRead(b, 6) ? +1 : bitRead(b, 7) ? -1 :  0;
                    v[1] = bitRead(b, 0) ? +1 : bitRead(b, 1) ? -1 :  0;
                    v[2] = bitRead(b, 2) ? +1 : bitRead(b, 3) ? -1 :  0;
                    v[3] = bitRead(b, 4) ? +1 : bitRead(b, 5) ? -1 :  0;
                    
                    readIndex -= 1;
                    count     += ArdoObj::ValueInfo::MaxValueCount; // to break the while loop
                    break;
                    
          case 1:   if ( ArduinoMainLoop::readInt8(v[count]) == false ) {
                      logError(42, E_INVALID_MOVE_SEQUENCE);
                      return -1;
                    }
                    readIndex -= 1;
                    break;
    
          case 2:   if ( ArduinoMainLoop::readInt16(v[count]) == false ) {
                      logError(43, E_INVALID_MOVE_SEQUENCE);
                      return -1;
                    }
                    readIndex -= 2;
                    break;
    
          case 4:   if ( ArduinoMainLoop::readInt32(v[count]) == false ) {
                      logError(44, E_INVALID_MOVE_SEQUENCE);
                      return -1;
                    }
                    readIndex -= 4;
                    break;
                    
          default:  logError(45, E_INVALID_MOVE_SEQUENCE);
                    return -1;
        }
  
        count++;
      }
  
      // assign x, y, z and f depending on given type
      const unsigned short p = result.vi.hasF() ? 1 : 0;
      if ( result.vi.hasF() )  result.f = v[0];
      else                     result.f = 0;
        
      if      ( result.vi.hasXYZ() ) { result.dx = v[p+0]; result.dy = v[p+1]; result.dz = v[p+2]; }
      else if ( result.vi.hasXY()  ) { result.dx = v[p+0]; result.dy = v[p+1]; result.dz = 0;      }
      else if ( result.vi.hasX()   ) { result.dx = v[p+0]; result.dy = 0;      result.dz = 0;      }
      else if ( result.vi.hasY()   ) { result.dx = 0;      result.dy = v[p+0]; result.dz = 0;      }
      else if ( result.vi.hasZ()   ) { result.dx = 0;      result.dy = 0;      result.dz = v[p+0]; }
      else                           { logError(46, E_INVALID_MOVE_SEQUENCE); return -1;  }
  
      if ( readIndex != 0 ) {
        logError(49, E_INVALID_MOVE_SEQUENCE);
        return -1;
      }
  
      // process . . . 
      if ( process(result) != RET_OK ) {
        logError(99, E_INVALID_MOVE_SEQUENCE);
        return -1;
      }
      
      return totalSize;
    }

  public:

    // ----------------------------------------------------------------------
    byte decodeMoveSequence() {
      result.reset();

      // first read header values
      if ( ArduinoMainLoop::readInt32(result.totalLength) == false )  return logError(10, E_INVALID_MOVE_SEQUENCE);
      if ( ArduinoMainLoop::readInt32(result.lengthX)     == false )  return logError(11, E_INVALID_MOVE_SEQUENCE);
      if ( ArduinoMainLoop::readInt32(result.lengthY)     == false )  return logError(12, E_INVALID_MOVE_SEQUENCE);
      if ( ArduinoMainLoop::readInt32(result.lengthZ)     == false )  return logError(13, E_INVALID_MOVE_SEQUENCE);

      result.totalRemaining = result.totalLength;
      result.totalIndex     = result.totalLength;
      result.ret            = RET_OK;

      // over all portions 
      while ( result.totalRemaining > 0 ) {
        
        // wait max an extra frame of one second for the next portion
        ArduinoMainLoop::waitForSerialData(1000L * 1000L);
        
        result.portionSize = ArduinoMainLoop::readSerialByteWithTimeout();
        
        if ( result.portionSize == 0 )
          return logError(20, E_INVALID_MOVE_SEQUENCE);
        
        result.totalRemaining--;
        
        if ( parsePortion() != 0 )
          return logError(21, E_INVALID_MOVE_SEQUENCE);
    
        // flush handshake
        if ( result.totalRemaining > 0 ) {
          Serial.write(RET_MORE);
          Serial.flush();
        }
      }

      return result.ret;
    }
};

#endif
