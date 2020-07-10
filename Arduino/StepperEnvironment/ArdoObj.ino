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
  
};
