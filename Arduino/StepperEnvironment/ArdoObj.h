#ifndef COMMON_OBJECTS_H
#define COMMON_OBJECTS_H

#include "ArdoVal.h"

namespace ArdoObj {

  // --------------------------------------------------------------
  template <class T>
  T minimum(T v1, T v2) {
    #ifdef SKETCH_COMPILE
      return min( v1, v2);
    #else
      return std::min(v1, v2);
    #endif
  }
  
  // --------------------------------------------------------------
  template <class T>
  T minimum(T v1, T v2, T v3) {
    #ifdef SKETCH_COMPILE
      return min(v3, min(v1, v2));
    #else
      return std::min(v3, std::min(v1, v2));
    #endif
  }
  
  // --------------------------------------------------------------
  template <class T>
  T maximum(T v1, T v2) {
    #ifdef SKETCH_COMPILE
      return max(v1, v2);
    #else
      return std::max(v1, v2);
    #endif  
  }
  
  // --------------------------------------------------------------
  template <class T>
  T maximum(T v1, T v2, T v3) {
    #ifdef SKETCH_COMPILE
      return max(v3, max(v1, v2));
    #else
      return std::max(v3, std::max(v1, v2));
    #endif  
  }
  
  // --------------------------------------------------------------
  template <class T>
  inline T absolute(T val) {
    if ( val < 0.0 )
      val *= -1;
      
    return val;
  }
  
  // --------------------------------------------------------------
  template <>
  inline int8_t absolute(int8_t x) {
    const int8_t a = x >> 7;
    return ((x ^ a) - a);
  }
  
  // --------------------------------------------------------------
  template <>
  inline int16_t absolute(int16_t x) {
    const int16_t a = x >> 15;
    return ((x ^ a) - a);
  }

  // --------------------------------------------------------------
  template <>
  inline int32_t absolute(int32_t x) {
    const int32_t a = x >> 31;
    return ((x ^ a) - a);
  }

  // --------------------------------------------------------------
  inline bool isSignal(byte cmd) {
    switch ( cmd ) {
      case SIG_SOFTWARE_RESET:
      case SIG_INTERRUPPT:
      case SIG_RESUME:
      case SIG_PAUSE:
      case SIG_HALT:
      case SIG_QUIT_MOVE: 
      
        return true;
    }
    
    return false;
  }
 
  // --------------------------------------------------------------
  // I2C data structure: Stores information about the received 
  // Wire communication 
  struct I2CData {
    unsigned char limitState;
    unsigned char supportState; 
  
    I2CData() 
    : limitState(0)
    , supportState(0)
    {}
  
    explicit I2CData(const I2CData& d) 
    : limitState(d.limitState)
    , supportState(d.limitState)
    {}
  
    void reset() {
      limitState   = 0;
      supportState = 0;     
    }
  };

  // --------------------------------------------------------------
  // 
  struct PwmProfile {
    
    unsigned int speedDelay         = 1;
    unsigned int accelDelay         = 1;
  
    unsigned int startSpeed_MM_SEC  = 1;
    unsigned int stopSpeed_MM_SEC   = 1;  
  };  
  
  // --------------------------------------------------------------
  // Stores value information, 
  // Used within the MoveSequence processing
  struct ValueInfo {
  
    private:
      enum Byte { L0=1, L1=2, L2=3, L4=4, F=5, X=6, Y=7, Z=8 };
      unsigned char type;
  
      //-------------------------------------------------------------
      bool getBit(unsigned char idx) const {
        switch (idx) {
          case 8:     return type & 128;
          case 7:     return type &  64;
          case 6:     return type &  32;
          case 5:     return type &  16;
          case 4:     return type &   8;
          case 3:     return type &   4;
          case 2:     return type &   2;
          case 1:     return type &   1;
        }
  
        return false;
      }
      
      // -------------------------------------------------------------
      void setBit(unsigned char idx, bool value) {
        if ( value == true ) {
          switch ( idx ) {
            case 8:     type |= (1 << 7); break;
            case 7:     type |= (1 << 6); break;
            case 6:     type |= (1 << 5); break;
            case 5:     type |= (1 << 4); break;
            case 4:     type |= (1 << 3); break;
            case 3:     type |= (1 << 2); break;
            case 2:     type |= (1 << 1); break;
            case 1:     type |= (1 << 0); break;
          }
        } 
        else {
           switch (idx) {
            case 8:     type &= ~(1 << 7); break;
            case 7:     type &= ~(1 << 6); break;
            case 6:     type &= ~(1 << 5); break;
            case 5:     type &= ~(1 << 4); break;
            case 4:     type &= ~(1 << 3); break;
            case 3:     type &= ~(1 << 2); break;
            case 2:     type &= ~(1 << 1); break;
            case 1:     type &= ~(1 << 0); break;
          }       
        }
      }
      
    public:  
      enum Size { One = 0, Int8 = 1, Int16 = 2, Int32 = 4, IntXX = -1};
  
      static const unsigned short MaxValueCount = 4;
      
      explicit ValueInfo(unsigned char t)
      : type(t)
      {}
  
      ValueInfo(Size s, int32_t x, int32_t y, int32_t z, int32_t f)
      : type(0)
      {
        setBit(Byte::L0, s == Size::One);
        setBit(Byte::L1, s == Size::Int8);
        setBit(Byte::L2, s == Size::Int16);
        setBit(Byte::L4, s == Size::Int32);
  
        setBit(Byte::F,  f != 0L);
        
        setBit(Byte::X,  x != 0L);
        setBit(Byte::Y,  y != 0L);
        setBit(Byte::Z,  z != 0L);
      }
  
      void set(unsigned char t) { type = t; }
  
      bool hasXYZ() const { return hasX() && hasY() && hasZ(); }
      bool hasXY()  const { return hasX() && hasY(); }
      
      bool hasX()   const { return getBit(Byte::X); }
      bool hasY()   const { return getBit(Byte::Y); }
      bool hasZ()   const { return getBit(Byte::Z); }
  
      bool hasF()   const { return getBit(Byte::F); }
  
      bool notX()   const { return !hasX(); }
      bool notY()   const { return !hasY(); }
      bool notZ()   const { return !hasZ(); }
  
      bool notF()   const { return !hasF(); }
  
      unsigned char getType() const { return type; }
      
      unsigned char getTypeAsPositionPid() const { 
        if      ( hasXYZ() )                    return PID_XYZ_POS;
        else if ( hasXY() && notZ())            return PID_XY_POS;
        else if ( hasX()  && notY() && notZ())  return PID_X_POS;
        else if ( hasY()  && notX() && notZ())  return PID_Y_POS;
        else if ( hasZ()  && notX() && notY())  return PID_Z_POS;
        
        return 0; 
      }
  
      bool isValid() const { 
        const short bc = getByteCount();
        return ( bc == 0 || bc == 1 || bc == 2 || bc == 4 );
      }
  
      short getByteCount() const {
        if      ( getBit(Byte::L0) ) return 0;
        else if ( getBit(Byte::L1) ) return 1;
        else if ( getBit(Byte::L2) ) return 2;
        else if ( getBit(Byte::L4) ) return 4;
  
        return -1;
      }
  
      short getValueCount() const {
        return (short)getBit(Byte::X) + (short)getBit(Byte::Y) + (short)getBit(Byte::Z) + (short)getBit(Byte::F);
      }
  
      short getNecessarySize() const {
        const short bc = getByteCount();
        
        if ( bc < 0 )
          return 0;
          
        if ( bc == 0 )
          return 1 * getValueCount();
          
        return bc * getValueCount();
      }
      
     #ifndef SKETCH_COMPILE
  
      static const char* getBitDeclaration() { return "XYZF4210"; }
     
      friend std::ostream &operator<< (std::ostream& ostr, const ValueInfo& vi) {
        for ( unsigned char i=8; i>=1; i-- ) {
          const char x = vi.getBit(i) ? '1' : '0';
          ostr << x;
        }
        return ostr;
      }
      
     #endif
  };
  
}; // namespace

#endif
