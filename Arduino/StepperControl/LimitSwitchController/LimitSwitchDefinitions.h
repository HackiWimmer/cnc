//-------------------------------------------------------------
// common global definitions

  #define LIMIT_SWITCH_ON        LOW
  #define LIMIT_SWITCH_OFF      HIGH
  
  #define LS_BYTE_COUNT            8
  
  #define LS_BYTE_IDX_X_MIN        7
  #define LS_BYTE_IDX_X_MAX        6
  #define LS_BYTE_IDX_Y_MIN        5
  #define LS_BYTE_IDX_Y_MAX        4
  #define LS_BYTE_IDX_Z_MIN        3
  #define LS_BYTE_IDX_Z_MAX        2
  #define LS_BYTE_IDX_RESERVED     1
  #define LS_BYTE_IDX_ERROR        0

  #define LS_FLAG_X_MIN          128
  #define LS_FLAG_X_MAX           64
  #define LS_FLAG_Y_MIN           32
  #define LS_FLAG_Y_MAX           16
  #define LS_FLAG_Z_MIN            8
  #define LS_FLAG_Z_MAX            4
  #define LS_FLAG_RESERVED_2       2
  #define LS_FLAG_ERROR            1

  // 00000000
  #define LS_INIT_STATE            0
  
  // 11111101
  #define LS_ERROR_STATE         253
    
namespace LimitSwitch {

    const long LIMIT_MIN              =   -1L;
    const long LIMIT_MAX              =   +1L;
    const long LIMIT_UNKNOWN          = +256L;
    const long LIMIT_UNSET            =    0L;
    
    const short analogReadWriteFactor = 1024/256;

#ifdef SKETCH_COMPILE

    //-------------------------------------------------------------
    struct LimitStates {

      private:
        unsigned int limitValue;
        char stringValue[8];

        inline long evaluateLimit(bool minValue, bool maxValue) const {
          if ( minValue && maxValue)  return LIMIT_UNKNOWN;
          if ( minValue )             return LIMIT_MIN;
          if ( maxValue )             return LIMIT_MAX;

          return LIMIT_UNSET;
        }

      public:
        LimitStates() 
        : limitValue(LS_INIT_STATE)
        {}
  
        explicit LimitStates(const int value) 
        : limitValue(value)
        {}
        
        void reset() {
          limitValue = LS_INIT_STATE;
        }

        void resetToError() {
          limitValue = LS_ERROR_STATE;
        }
  
        bool xMin() const       { return limitValue & LS_FLAG_X_MIN; }
        bool xMax() const       { return limitValue & LS_FLAG_X_MAX; }
        bool yMin() const       { return limitValue & LS_FLAG_Y_MIN; }
        bool yMax() const       { return limitValue & LS_FLAG_Y_MAX; }
        bool zMin() const       { return limitValue & LS_FLAG_Z_MIN; }
        bool zMax() const       { return limitValue & LS_FLAG_Z_MAX; }

        long xLimit() const     { return evaluateLimit(xMin(), xMax()); }
        long yLimit() const     { return evaluateLimit(yMin(), yMax()); }
        long zLimit() const     { return evaluateLimit(zMin(), zMax()); }

        unsigned int getLimitValue()        { return limitValue; }        
        void setLimitValue(unsigned int v)  { limitValue = v;    }
        
        bool hasError() const   { 
          if ( xMin() && xMax() ) return true;
          if ( yMin() && yMax() ) return true;
          if ( zMin() && zMax() ) return true;

          return false;
        }

        const char* getLimitValueAsString() {
          memset(&stringValue, '0', sizeof(stringValue));
          
          if ( xMin() )     stringValue[LS_BYTE_COUNT - 1 - LS_BYTE_IDX_X_MIN] = '1';
          if ( xMax() )     stringValue[LS_BYTE_COUNT - 1 - LS_BYTE_IDX_X_MAX] = '1';
        
          if ( yMin() )     stringValue[LS_BYTE_COUNT - 1 - LS_BYTE_IDX_Y_MIN] = '1';
          if ( yMax() )     stringValue[LS_BYTE_COUNT - 1 - LS_BYTE_IDX_Y_MAX] = '1';
         
          if ( zMin() )     stringValue[LS_BYTE_COUNT - 1 - LS_BYTE_IDX_Z_MIN] = '1';
          if ( zMax() )     stringValue[LS_BYTE_COUNT - 1 - LS_BYTE_IDX_Z_MAX] = '1';
         
          if ( hasError() ) stringValue[LS_BYTE_COUNT - 1 - LS_BYTE_IDX_ERROR] = '1';
          
          return stringValue;
        }
    };
    
    //-------------------------------------------------------------
    void readLimitStates(int pin, int msdelay, LimitStates& ls)
    {
      if ( msdelay > 0 )
        delay(msdelay);
        
      // consider analogRead delivers values from 0 to 1023
      int readValue = analogRead(pin);
    
      // recalulate the readValue to a range from 0 ... 255
      //       0:      0,  1,  2,  3
      //       1:      4,  5,  6,  7
      //       2:      8,  9, 10, 11
      //       3:     12, 13, 14, 15
      //       4:     16, 17, 18, 19
      // ...
      // example:   readValue  = 18
      // offset     = 18 % 4 = 2
      // writeValue = (18 - 2) / 4 = 4
       
      short offset      = readValue % analogReadWriteFactor;
      char writtenValue = (readValue - offset) / analogReadWriteFactor;
    
      ls.setLimitValue(writtenValue);
    }
    
#endif
} // namespace
  

