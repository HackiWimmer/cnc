#ifndef COMMON_OBJECTS_H
#define COMMON_OBJECTS_H

#include "ArdoVal.h"

///////////////////////////////////////////////////////////////////////////////////
namespace CtrlSpeedValues {

  #ifndef SKETCH_COMPILE
  
    float getMaxF_1DX_MMSec();
    float getMaxF_1DY_MMSec();
    float getMaxF_1DZ_MMSec();
    float getMaxF_2DXY_MMSec();
    float getMaxF_3DXYZ_MMSec();
    
  #endif
 
  void setupTact(int16_t pwX, int16_t pwY, int16_t pwZ);
  void setupMaxSpeed(float frX, float frY, float frZ);
};

///////////////////////////////////////////////////////////////////////////////////
namespace ArdoTs {

  const uint32_t MAX_VALUE = UINT32_MAX;

  inline uint32_t now();
  inline uint32_t timespan(uint32_t from, uint32_t to);
  inline uint32_t timespan(uint32_t from);
  
};

///////////////////////////////////////////////////////////////////////////////////
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
  inline uint32_t sqrt(uint32_t x) {
    register uint32_t xr;   // result register
    register uint32_t q2;   // scan-bit register
    register unsigned char f; // flag (one bit)

    xr = 0;             // clear result
    q2 = 0x40000000L;       // higest possible result bit
    do {
      if ( (xr + q2) <= x ) {
        x -= xr + q2;
        f = 1;          // set flag
      }
      else {
        f = 0;          // clear flag
      }
      xr >>= 1;
      
      if ( f ) 
        xr += q2;         // test flag
      
    } while ( q2 >>= 2 );     // shift twice
    
    // xr + 1: add for rounding
    return xr < x ? xr + 1 : xr;
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
      case SIG_UPDATE: 
      
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
  // Stores value information, 
  // Used within the MoveSequence processing
  struct ValueInfo {
    public:  
      enum Byte { L0=1, L1=2, L2=3, L4=4, Q=5, X=6, Y=7, Z=8 };
      
    private:
      unsigned char type;
  
      //-------------------------------------------------------------
      bool getBit(unsigned char idx) const {
        switch (idx) {
          case Byte::Z:     return type & 128;
          case Byte::Y:     return type &  64;
          case Byte::X:     return type &  32;
          case Byte::Q:     return type &  16;
          case Byte::L4:    return type &   8;
          case Byte::L2:    return type &   4;
          case Byte::L1:    return type &   2;
          case Byte::L0:    return type &   1;
        }
  
        return false;
      }
      
      // -------------------------------------------------------------
      void setBit(unsigned char idx, bool value) {
        if ( value == true ) {
          switch ( idx ) {
            case Byte::Z:     type |= (1 << 7); break;
            case Byte::Y:     type |= (1 << 6); break;
            case Byte::X:     type |= (1 << 5); break;
            case Byte::Q:     type |= (1 << 4); break;
            case Byte::L4:    type |= (1 << 3); break;
            case Byte::L2:    type |= (1 << 2); break;
            case Byte::L1:    type |= (1 << 1); break;
            case Byte::L0:    type |= (1 << 0); break;
          }
        } 
        else {
           switch (idx) {
            case Byte::Z:     type &= ~(1 << 7); break;
            case Byte::Y:     type &= ~(1 << 6); break;
            case Byte::X:     type &= ~(1 << 5); break;
            case Byte::Q:     type &= ~(1 << 4); break;
            case Byte::L4:    type &= ~(1 << 3); break;
            case Byte::L2:    type &= ~(1 << 2); break;
            case Byte::L1:    type &= ~(1 << 1); break;
            case Byte::L0:    type &= ~(1 << 0); break;
          }       
        }
      }
      
    public:  
      enum Size { One = 0, Int8 = 1, Int16 = 2, Int32 = 4, IntXX = -1};
  
      static const unsigned short MaxValueCount = 3;
      
      explicit ValueInfo(unsigned char t)
      : type(t)
      {}
  
      ValueInfo(Size s, int32_t x, int32_t y, int32_t z)
      : type(0)
      {
        setBit(Byte::L0, s == Size::One);
        setBit(Byte::L1, s == Size::Int8);
        setBit(Byte::L2, s == Size::Int16);
        setBit(Byte::L4, s == Size::Int32);
  
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
  
      bool notX()   const { return !hasX(); }
      bool notY()   const { return !hasY(); }
      bool notZ()   const { return !hasZ(); }
  
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
        return (short)getBit(Byte::X) + (short)getBit(Byte::Y) + (short)getBit(Byte::Z);
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
  
      static const char* getBitDeclaration() { return "ZYX?4210"; }
     
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
