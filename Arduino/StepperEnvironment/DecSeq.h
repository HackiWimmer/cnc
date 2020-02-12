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

      byte cmd                     = CMD_INVALID;
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

      void reset() {
        *this = Result();
      }
      
      void resetDynValues() {
        pid                  = PID_UNKNOWN;
        ret                  = RET_OK;
        portionSize          = 0;
        dx                   = 0;
        dy                   = 0;
        dz                   = 0;
      }
    };
    
    ArduinoCmdDecoderMoveSequence()
    : ArduinoCmdDecoderBase()
    , result()
    {}
    
    virtual ~ArduinoCmdDecoderMoveSequence() 
    {}

  protected:
    
    virtual byte initialize(const ArduinoCmdDecoderMoveSequence::Result& seq) = 0;
    virtual byte process   (const ArduinoCmdDecoderMoveSequence::Result& seq) = 0;
    virtual byte finalize  (const ArduinoCmdDecoderMoveSequence::Result& seq) = 0;

  private:
    typedef ArduinoMainLoop AML;
    Result result;  

    // ----------------------------------------------------------------------
#ifdef SKETCH_COMPILE
    void debugValues(byte) {
#else
    void debugValues(byte idx) {
#endif  
        LastErrorCodes::clear();

        ARDO_DEBUG_MESSAGE('S',"Debug MoveSequence Values")
        
        ARDO_DEBUG_VALUE(" MVSQ.more available",      (int16_t)Serial.available())
        
        ARDO_DEBUG_VALUE(" MVSQ.idx",                 (int16_t)idx)
        ARDO_DEBUG_VALUE(" MVSQ.pid",                 (int16_t)result.pid)
        ARDO_DEBUG_VALUE(" MVSQ.portionSize",         (int16_t)result.portionSize)
        ARDO_DEBUG_VALUE(" MVSQ.portionCounter",      (int16_t)result.portionCounter)
        
        ARDO_DEBUG_VALUE(" MVSQ.TotalIndex",          result.totalIndex)
        ARDO_DEBUG_VALUE(" MVSQ.TotalLength",         result.totalLength)
        ARDO_DEBUG_VALUE(" MVSQ.TotalRemaining",      result.totalRemaining)
  
        ARDO_DEBUG_VALUE(" MVSQ.dx",                  result.dx)
        ARDO_DEBUG_VALUE(" MVSQ.dy",                  result.dy)
        ARDO_DEBUG_VALUE(" MVSQ.dz",                  result.dz)
    }
  
    // ----------------------------------------------------------------------
    byte logError(byte idx, byte eid) {
        AML::pushMessage(MT_ERROR, eid, LastErrorCodes::writeToSerial);
        AML::clearSerial();
        debugValues(idx);
        
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
      result.pid = AML::readSerialByteWithTimeout();
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
      v[0] = v[1] = v[2] = 0;
      
      byte b;
      unsigned short count = 0;
      
      while ( count < valCount ) {
        
        // copy bytes
        switch ( byteCount ){
    
          // format:   bit: 78543210    
          //                  zzyyxx
          //                  -+-+-+ 
          //                          bit  +               bit  -     0
          case 0:   b = AML::readSerialByteWithTimeout();
                    v[0] = bitRead(b, 0) ? +1 : bitRead(b, 1) ? -1 :  0;
                    v[1] = bitRead(b, 2) ? +1 : bitRead(b, 3) ? -1 :  0;
                    v[2] = bitRead(b, 4) ? +1 : bitRead(b, 5) ? -1 :  0;
                    
                    readIndex -= 1;
                    count     += ArdoObj::ValueInfo::MaxValueCount; // to break the while loop
                    break;
                    
          case 1:   if ( AML::readInt8(v[count]) == false ) {
                      logError(42, E_INVALID_MOVE_SEQUENCE);
                      return -1;
                    }
                    readIndex -= 1;
                    break;
    
          case 2:   if ( AML::readInt16(v[count]) == false ) {
                      logError(43, E_INVALID_MOVE_SEQUENCE);
                      return -1;
                    }
                    readIndex -= 2;
                    break;
    
          case 4:   if ( AML::readInt32(v[count]) == false ) {
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
      if      ( result.vi.hasXYZ() ) { result.dx = v[0]; result.dy = v[1]; result.dz = v[2]; }
      else if ( result.vi.hasXY()  ) { result.dx = v[0]; result.dy = v[1]; result.dz = 0;      }
      else if ( result.vi.hasX()   ) { result.dx = v[0]; result.dy = 0;    result.dz = 0;      }
      else if ( result.vi.hasY()   ) { result.dx = 0;    result.dy = v[0]; result.dz = 0;      }
      else if ( result.vi.hasZ()   ) { result.dx = 0;    result.dy = 0;    result.dz = v[0]; }
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
    byte decodeMoveSequence(byte cmd) {
      result.reset();
      result.cmd = cmd;

      // first read header values
      if ( AML::readInt32(result.totalLength) == false )  return logError(10, E_INVALID_MOVE_SEQUENCE);
      if ( AML::readInt32(result.lengthX)     == false )  return logError(11, E_INVALID_MOVE_SEQUENCE);
      if ( AML::readInt32(result.lengthY)     == false )  return logError(12, E_INVALID_MOVE_SEQUENCE);
      if ( AML::readInt32(result.lengthZ)     == false )  return logError(13, E_INVALID_MOVE_SEQUENCE);

      if ( initialize(result) != RET_OK )
        logError(14, E_INVALID_MOVE_SEQUENCE);
        
      if ( true ) {
        ARDO_DEBUG_VALUE("ArduinoCmdDecoderMoveSequence: result.totalLength", result.totalLength)
        ARDO_DEBUG_VALUE("ArduinoCmdDecoderMoveSequence: result.lengthX    ", result.lengthX)
        ARDO_DEBUG_VALUE("ArduinoCmdDecoderMoveSequence: result.lengthY    ", result.lengthY)
        ARDO_DEBUG_VALUE("ArduinoCmdDecoderMoveSequence: result.lengthZ    ", result.lengthZ)
      }

      result.totalRemaining = result.totalLength;
      result.totalIndex     = result.totalLength;
      result.ret            = RET_OK;

      // over all portions 
      while ( result.totalRemaining > 0 ) {
        
        // wait max an extra frame of one second for the next portion
        AML::waitForSerialData(1000L * 1000L);

        result.resetDynValues();
        result.portionSize = AML::readSerialByteWithTimeout();
        
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

      result.ret = finalize(result);
      return result.ret;
    }
};

#endif
