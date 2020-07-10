#ifndef CNC_INTERFACE_H
#define CNC_INTERFACE_H

namespace LimitSwitch {
  
  const bool EXTERNAL_INTERRUPT_ON                       = false;
  const bool EXTERNAL_INTERRUPT_OFF                      =  true;

  const bool EMERGENCY_SWITCH_ON                         = false;
  const bool EMERGENCY_SWITCH_OFF                        =  true;

  const bool LIMIT_SWITCH_ON                             = false;  // LOW;
  const bool LIMIT_SWITCH_OFF                            =  true;  // HIGH;
      
  const int8_t LIMIT_MIN                                 =   -1;
  const int8_t LIMIT_MAX                                 =   +1;
  const int8_t LIMIT_UNKNOWN                             = +127;
  const int8_t LIMIT_UNSET                               =    0;


  const unsigned char BIT_LS_X_MIN                       =     8;
  const unsigned char BIT_LS_X_MAX                       =     7;
  const unsigned char BIT_LS_Y_MIN                       =     6;
  const unsigned char BIT_LS_Y_MAX                       =     5;
  const unsigned char BIT_LS_Z_MIN                       =     4;
  const unsigned char BIT_LS_Z_MAX                       =     3;
  const unsigned char BIT_EMERGENCY_SWITCH               =     2;
  const unsigned char BIT_1                              =     1;

}; // namespace LimitSwitch

namespace SupportPin {
 
  const unsigned char BIT_IS_SUPPORT_BUTTON_1_PRESSED    =     8;
  const unsigned char BIT_IS_SUPPORT_BUTTON_2_PRESSED    =     7;
  const unsigned char BIT_IS_SUPPORT_BUTTON_3_PRESSED    =     6;
  const unsigned char BIT_5                              =     5;
  const unsigned char BIT_IS_SUPPORT_SWITCH_1_PRESSED    =     4;
  const unsigned char BIT_IS_SUPPORT_SWITCH_2_PRESSED    =     3;
  const unsigned char BIT_IS_TOOL_POWERED                =     2;
  const unsigned char BIT_IS_CABLE_CONNECTED             =     1;

}; // namespace SupportPin

namespace CncInterface {

  class StatesBase {
  
    private:

      static const unsigned short ANALOG_READ_WRITE_FACTOR  = 1024/256;
      unsigned char states;
      char stringValue[9];
     
    public:
      //-------------------------------------------------------------
      StatesBase()
      : states(0) {}
      
      explicit StatesBase(unsigned char s)
      : states(s) {}

      //-------------------------------------------------------------
      void reset() { states = 0; }

      //-------------------------------------------------------------
      unsigned char getValue() const { return states; }
      
      //-------------------------------------------------------------
      bool getBit(unsigned char idx) const {
        switch (idx) {
          case 8:     return states & 128;
          case 7:     return states &  64;
          case 6:     return states &  32;
          case 5:     return states &  16;
          case 4:     return states &   8;
          case 3:     return states &   4;
          case 2:     return states &   2;
          case 1:     return states &   1;
        }

        return false;
      }
      
      //-------------------------------------------------------------
      void setBit(unsigned char idx, bool value) { 
        if ( value == true ) {
          switch (idx) {
            case 8:     states |= (1 << 7); break;
            case 7:     states |= (1 << 6); break;
            case 6:     states |= (1 << 5); break;
            case 5:     states |= (1 << 4); break;
            case 4:     states |= (1 << 3); break;
            case 3:     states |= (1 << 2); break;
            case 2:     states |= (1 << 1); break;
            case 1:     states |= (1 << 0); break;
          }
        } else {
           switch (idx) {
            case 8:     states &= ~(1 << 7); break;
            case 7:     states &= ~(1 << 6); break;
            case 6:     states &= ~(1 << 5); break;
            case 5:     states &= ~(1 << 4); break;
            case 4:     states &= ~(1 << 3); break;
            case 3:     states &= ~(1 << 2); break;
            case 2:     states &= ~(1 << 1); break;
            case 1:     states &= ~(1 << 0); break;
          }       
        }
      }

      //-------------------------------------------------------------
      virtual const char* getValueAsString() {
        stringValue[0] = getBit(8) == true ? '1' : '0';
        stringValue[1] = getBit(7) == true ? '1' : '0';
        stringValue[2] = getBit(6) == true ? '1' : '0';
        stringValue[3] = getBit(5) == true ? '1' : '0';
        stringValue[4] = getBit(4) == true ? '1' : '0';
        stringValue[5] = getBit(3) == true ? '1' : '0';
        stringValue[6] = getBit(2) == true ? '1' : '0';
        stringValue[7] = getBit(1) == true ? '1' : '0';
        stringValue[8] = '\0';
        
        return stringValue;
      }

      //-------------------------------------------------------------
      virtual const char* getValueAsReport(char* ret) {
        return ret;
      }
  };

  namespace ISP {

    class States : public StatesBase {

      public:
        //-------------------------------------------------------------
        States()
        : StatesBase() {}

        explicit States(const States& s)
        : StatesBase(s.getValue()) {}

        explicit States(unsigned char s)
        : StatesBase(s) {}
        
        //-------------------------------------------------------------
        bool isSupportButton1Pressed()        const  { return getBit(SupportPin::BIT_IS_SUPPORT_BUTTON_1_PRESSED);   }
        bool isSupportButton2Pressed()        const  { return getBit(SupportPin::BIT_IS_SUPPORT_BUTTON_2_PRESSED);   }
        bool isSupportButton3Pressed()        const  { return getBit(SupportPin::BIT_IS_SUPPORT_BUTTON_3_PRESSED);   }
        
        bool isSupportSwitch1Pressed()        const  { return getBit(SupportPin::BIT_IS_SUPPORT_SWITCH_1_PRESSED);   }
        bool isSupportSwitch2Pressed()        const  { return getBit(SupportPin::BIT_IS_SUPPORT_SWITCH_2_PRESSED);   }
        
        bool isToolPowered()                  const  { return getBit(SupportPin::BIT_IS_TOOL_POWERED);               }
        bool isCableConnected()               const  { return getBit(SupportPin::BIT_IS_CABLE_CONNECTED);            }
        // add more named getters on demand . . .
        
    }; // States
  }; // SP

  namespace ILS {
    
    class States : public StatesBase {

      private:
      
        //-------------------------------------------------------------
        inline long evaluateLimit(bool minValue, bool maxValue) const {
          if ( minValue && maxValue)  return LimitSwitch::LIMIT_UNKNOWN;
          if ( minValue )             return LimitSwitch::LIMIT_MIN;
          if ( maxValue )             return LimitSwitch::LIMIT_MAX;
  
          return LimitSwitch::LIMIT_UNSET;
        }

      public:
        //-------------------------------------------------------------
        States()
        : StatesBase() {}

        explicit States(unsigned char s)
        : StatesBase(s) {}
  
        explicit States(const States& s)
        : StatesBase(s.getValue()) {}

        States(bool xMin, bool xMax, bool yMin, bool yMax, bool zMin, bool zMax) 
        : StatesBase()
        {
          setBit(LimitSwitch::BIT_LS_X_MAX, xMax); setBit(LimitSwitch::BIT_LS_X_MIN, xMin);
          setBit(LimitSwitch::BIT_LS_Y_MAX, yMax); setBit(LimitSwitch::BIT_LS_Y_MIN, yMin);
          setBit(LimitSwitch::BIT_LS_Z_MAX, zMax); setBit(LimitSwitch::BIT_LS_Z_MIN, zMin);
        }

        States(const int8_t x, const int8_t y, const int8_t z) 
        : StatesBase()
        {
          switch ( x ) {
              case LimitSwitch::LIMIT_UNKNOWN:   setBit(LimitSwitch::BIT_LS_X_MAX, true);  setBit(LimitSwitch::BIT_LS_X_MIN, true);  break;
              case LimitSwitch::LIMIT_UNSET:     setBit(LimitSwitch::BIT_LS_X_MAX, false); setBit(LimitSwitch::BIT_LS_X_MIN, false); break;
              case LimitSwitch::LIMIT_MIN:       setBit(LimitSwitch::BIT_LS_X_MAX, false); setBit(LimitSwitch::BIT_LS_X_MIN, true);  break;
              case LimitSwitch::LIMIT_MAX:       setBit(LimitSwitch::BIT_LS_X_MAX, true);  setBit(LimitSwitch::BIT_LS_X_MIN, false); break;
          }
  
          switch ( y ) {
              case LimitSwitch::LIMIT_UNKNOWN:   setBit(LimitSwitch::BIT_LS_Y_MAX, true);  setBit(LimitSwitch::BIT_LS_Y_MIN, true);  break;
              case LimitSwitch::LIMIT_UNSET:     setBit(LimitSwitch::BIT_LS_Y_MAX, false); setBit(LimitSwitch::BIT_LS_Y_MIN, false); break;
              case LimitSwitch::LIMIT_MIN:       setBit(LimitSwitch::BIT_LS_Y_MAX, false); setBit(LimitSwitch::BIT_LS_Y_MIN, true);  break;
              case LimitSwitch::LIMIT_MAX:       setBit(LimitSwitch::BIT_LS_Y_MAX, true);  setBit(LimitSwitch::BIT_LS_Y_MIN, false); break;
          }
  
          switch ( z ) {
              case LimitSwitch::LIMIT_UNKNOWN:   setBit(LimitSwitch::BIT_LS_Z_MAX, true);  setBit(LimitSwitch::BIT_LS_Z_MIN, true);  break;
              case LimitSwitch::LIMIT_UNSET:     setBit(LimitSwitch::BIT_LS_Z_MAX, false); setBit(LimitSwitch::BIT_LS_Z_MIN, false); break;
              case LimitSwitch::LIMIT_MIN:       setBit(LimitSwitch::BIT_LS_Z_MAX, false); setBit(LimitSwitch::BIT_LS_Z_MIN, true);  break;
              case LimitSwitch::LIMIT_MAX:       setBit(LimitSwitch::BIT_LS_Z_MAX, true);  setBit(LimitSwitch::BIT_LS_Z_MIN, false); break;
          }
        }
        
        //-------------------------------------------------------------
        bool emergency() const  { return getBit(LimitSwitch::BIT_EMERGENCY_SWITCH); }
        
        //-------------------------------------------------------------
        bool xMin() const       { return getBit(LimitSwitch::BIT_LS_X_MIN); }
        bool xMax() const       { return getBit(LimitSwitch::BIT_LS_X_MAX); }
        bool yMin() const       { return getBit(LimitSwitch::BIT_LS_Y_MIN); }
        bool yMax() const       { return getBit(LimitSwitch::BIT_LS_Y_MAX); }
        bool zMin() const       { return getBit(LimitSwitch::BIT_LS_Z_MIN); }
        bool zMax() const       { return getBit(LimitSwitch::BIT_LS_Z_MAX); }
  
        long xLimit() const     { return evaluateLimit(xMin(), xMax()); }
        long yLimit() const     { return evaluateLimit(yMin(), yMax()); }
        long zLimit() const     { return evaluateLimit(zMin(), zMax()); }
        
        bool hasLimits() const   {
          if ( xMin() || xMax() ) return true;
          if ( yMin() || yMax() ) return true;
          if ( zMin() || zMax() ) return true;
          
          return false;
        }
  
        bool hasErrors() const   {
          if ( emergency()      ) return true;
          if ( hasLimits()       ) return true;
          
          return false;
        }
        
    }; // States
  }; // namespace LS 
}; // namespace CncInterface


#endif
