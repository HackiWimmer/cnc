//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
namespace ArdoTs {

  // --------------------------------------------------------------
  inline uint32_t now() { 
    return AE::micros() % MAX_VALUE; 
  }
  
  // --------------------------------------------------------------
  inline uint32_t timespan(uint32_t from, uint32_t to) {
    
    // The return value of micros() will overflow (go back to zero), 
    // after approximately 70 minutes.
    // In this case ....
    if ( to < from )
      return MAX_VALUE - (to - from);
    
    return to - from;
  }
  
  // --------------------------------------------------------------
  inline uint32_t timespan(uint32_t from) {
    return timespan(from, now());
  }
  
}; // namespace ArdoTs



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
namespace ArdoObj {

  // --------------------------------------------------------------
  int32_t SpeedTuple::encode(char mode, int32_t value_MMSec1000)
  {
    const int32_t C_2_24 = 16777216;
    value_MMSec1000 = minimum(value_MMSec1000, C_2_24);

    int32_t ret = mode;
    ret = (ret << 24);
    return ret | value_MMSec1000;      
  }
    
  // --------------------------------------------------------------
  char SpeedTuple::decodeMode (int32_t eValue)
  {
    if ( eValue != 0 )
      return (eValue >> 24);

    return '?';
  }
  
  // --------------------------------------------------------------
  int32_t SpeedTuple::decodeValue_MMSec1000(int32_t eValue) {
    if ( eValue != 0 )
      return ((eValue <<  8) >> 8);

    return 0;
  }

  // --------------------------------------------------------------
  int32_t SpeedTuple::decodeValue_MMSec(int32_t eValue)
  {
    if ( eValue != 0 )
      return SpeedTuple::decodeValue_MMSec1000(eValue) / 1000;

    return 0;
  }
  
  // --------------------------------------------------------------
  int32_t SpeedTuple::decodeValue_MMMin(int32_t eValue)
  {
    if ( eValue != 0 )
      return SpeedTuple::decodeValue_MMSec(eValue) * 60;

    return 0;
  }

  // --------------------------------------------------------------
  int32_t SpindleTuple::encode(int16_t value, int16_t range)
  {
    int32_t ret = range;
    ret = (ret << 16);
    
    return ret | value;      
  }
  
  // --------------------------------------------------------------
  int16_t SpindleTuple::decodeValue(int32_t eValue)
  {
    if ( eValue != 0 )
      return ((eValue << 16) >> 16);

    return 0;
  }

  // --------------------------------------------------------------
  int16_t SpindleTuple::decodeRange(int32_t eValue)
  {
    if ( eValue != 0 )
      return (eValue >> 16);

    return 0;
  }

  // --------------------------------------------------------------
  float SpindleTuple::getPwmFact(int16_t speedRange)
  {
    return (float)(speedRange) / (float)ardoRange;
  }
  
  // --------------------------------------------------------------
  int16_t SpindleTuple::getPwmValue(int32_t value)
  {
    const int16_t speedRange = decodeRange(value); 
    const int16_t speedValue = decodeValue(value);
    const float fact    = (float)(speedRange) / (float)ardoRange;

    return round(speedValue * fact);
  }

  
}; // namespace ArdoObj
