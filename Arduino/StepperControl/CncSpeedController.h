#ifndef CNC_SPEED_CONTROLLER
#define CNC_SPEED_CONTROLLER

class CncSpeedController {
  
  protected:
  
    struct Axis {
      char            axis                = '?';
      bool            initialized         = false;
      bool            speedConfigured     = false;
      
      unsigned int    steps               =   1;
      double          pitch               = 1.0;
      
      unsigned int    offsetRendering     =   0;
      unsigned int    offsetStepping      =   0;
      unsigned int    totalPulsWidth      =   0;
      
      unsigned int    totalOffset         =   0;
      unsigned int    maxStepsPerSecond   =   0;
      unsigned int    maxDistPerSecond    =   0;
      unsigned int    maxDistPerMinute    =   0;
      
      unsigned int    synthSpeedDelay     =   0;
      
      double          gearing             = 0.0;
      
      unsigned int    stepsPerSec         =   0;
      unsigned int    microSecPerStep     =   0;
      unsigned int    rpm                 =   0;
      
      //////////////////////////////////////////////////////////////////
      Axis(const char a)
      : axis(a)
      , initialized(false)
      , speedConfigured(false)
      {}

      //////////////////////////////////////////////////////////////////
      bool isInitialized()     const { return initialized;     }
      bool isSpeedConfigured() const { return speedConfigured; }
      
      //////////////////////////////////////////////////////////////////
      void setup(unsigned int s, double p, unsigned int oR, unsigned int oS, unsigned int tPW) {
        steps             = (s ==  0 ?   1 : s);
        pitch             = (p < 0.0 ? 0.0 : p);
        offsetRendering   = oR;
        offsetStepping    = oS;
        totalPulsWidth    = tPW;
        gearing           = pitch / steps;
        totalOffset       = offsetRendering + offsetStepping + totalPulsWidth;
        maxStepsPerSecond = (1000L * 1000L) / totalOffset;
        maxDistPerSecond  = maxStepsPerSecond * gearing;
        maxDistPerMinute  = maxDistPerSecond * 60;

        // only these values are relevant, the rest can be default
        initialized = ( steps > 0 && pitch > 0.0 && totalPulsWidth > 0);
      }
      
      //////////////////////////////////////////////////////////////////
      bool calculateStatics(double speed_MM_SEC) {
        speedConfigured = false;
        
        if ( speed_MM_SEC > 0.0 ) {
           speedConfigured = true;
          
          stepsPerSec         = speed_MM_SEC       / gearing; 
          microSecPerStep     = (1000L * 1000L)    / stepsPerSec;
          rpm                 = (stepsPerSec * 60) / steps;
          synthSpeedDelay     = (microSecPerStep > totalOffset ? microSecPerStep - totalOffset : 0);
        } else {
          stepsPerSec         = 0; 
          microSecPerStep     = 0;
          rpm                 = 0;
          synthSpeedDelay     = 0;            
        }
        
        return speedConfigured;
      }
      
      //////////////////////////////////////////////////////////////////
      #ifdef SKETCH_COMPILE
        void printConfig() {

          #define PRINT_PARAMETER_L1( Pid, value ) \
              Serial.print(BLANK); Serial.print(BLANK); \
              Serial.print(Pid);   Serial.print(TEXT_SEPARATOR); \
              Serial.print(value); Serial.write(TEXT_CLOSE); 
              
          #define PRINT_PARAMETER_L2( Pid, value ) \
              Serial.print(BLANK); Serial.print(BLANK); Serial.print(BLANK); \
              Serial.print(Pid);   Serial.print(TEXT_SEPARATOR); \
              Serial.print(value); Serial.write(TEXT_CLOSE); 

         #define PRINT_PARAMETER_L3( Pid, value ) \
              Serial.print(BLANK); Serial.print(BLANK); Serial.print(BLANK); Serial.print(BLANK); \
              Serial.print(Pid);   Serial.print(TEXT_SEPARATOR); \
              Serial.print(value); Serial.write(TEXT_CLOSE); 
              

              PRINT_PARAMETER_L1(PID_AXIS,                              axis)
              PRINT_PARAMETER_L2(PID_STEPS,                             steps)
              PRINT_PARAMETER_L2(PID_SPEED_CTRL_TOTAL_OFFSET,           totalOffset)
              PRINT_PARAMETER_L2(PID_SPEED_CTRL_INITIALIZED,            isInitialized())
              PRINT_PARAMETER_L2(PID_SPEED_CTRL_MAX_SPEED,              maxDistPerMinute)

              PRINT_PARAMETER_L3(PID_SPEED_CTRL_RPM,                    rpm)
              PRINT_PARAMETER_L3(PID_SPEED_CTRL_SYNTH_SPEED_DELAY,      synthSpeedDelay)

          #undef PRINT_PARAMETER_L1
          #undef PRINT_PARAMETER_L2
          #undef PRINT_PARAMETER_L3
        }
      #endif
      //////////////////////////////////////////////////////////////////
      #ifndef SKETCH_COMPILE
        friend std::ostream &operator<< (std::ostream &ostr, const Axis &a) {
          const char* o = "   ";
          ostr << " Axis: " << a.axis << ':'                                                << std::endl;
          ostr << o << "Steps                            [steps] : " << a.steps             << std::endl;
          ostr << o << "Pitch                           [mm/360] : " << a.pitch             << std::endl;
          ostr << o << "Gearing                        [mm/step] : " << a.gearing           << std::endl;
          ostr << o << "Offset Rendering                  [usec] : " << a.offsetRendering   << std::endl;
          ostr << o << "Offset Stepping                   [usec] : " << a.offsetStepping    << std::endl;
          ostr << o << "Total Puls Width                  [usec] : " << a.totalPulsWidth    << std::endl;
          ostr << o << "Total Offset                      [usec] : " << a.totalOffset       << std::endl;
          ostr << o << "Max steps per Second (theo.) [steps/sec] : " << a.maxStepsPerSecond << std::endl;
          ostr << o << "Max Feed Speed       (theo.)    [mm/sec] : " << a.maxDistPerSecond  << std::endl;
          ostr << o << "Max Feed Speed       (theo.)    [mm/min] : " << a.maxDistPerMinute  << std::endl;
          ostr << o                                                                         << std::endl;
          ostr << o << "As a function of configured speed:"                                 << std::endl;
          ostr << o << "   Rounds per Minute               [rpm] : " << a.rpm               << std::endl;
          ostr << o << "   Steps per Second           [step/sec] : " << a.stepsPerSec       << std::endl;
          ostr << o << "   Microseconds per Step     [usec/step] : " << a.microSecPerStep   << std::endl;
          ostr << o << "   Synthetically Speed Delay [usec/step] : " << a.synthSpeedDelay   << std::endl;
          
          return ostr;
        }
      #endif
    };

    bool initialized;
    double configuredFeedSpeed_MM_SEC;
    double realtimeFeedSpeed_MM_SEC;
    
  public:
    
    Axis X, Y, Z;
    
    //////////////////////////////////////////////////////////////////
    CncSpeedController()
    : initialized(false)
    , configuredFeedSpeed_MM_SEC(0.0)
    , realtimeFeedSpeed_MM_SEC(0.0)
    , X('X')
    , Y('Y')
    , Z('Z')
    {}

    //////////////////////////////////////////////////////////////////
    bool isInitialized()                   const { return X.isInitialized() && Y.isInitialized() && Z.isInitialized(); }
    bool isSpeedConfigured()               const { return X.isSpeedConfigured() && Y.isSpeedConfigured() && Z.isSpeedConfigured(); }
  
    //////////////////////////////////////////////////////////////////
    double getMaxFeedSpeed_MM_MIN() const { return minimum(X.maxDistPerMinute, Y.maxDistPerMinute, Z.maxDistPerMinute); }
    double getMaxFeedSpeed_MM_SEC() const { return minimum(X.maxDistPerSecond, Y.maxDistPerSecond, Z.maxDistPerSecond); }
    
    //////////////////////////////////////////////////////////////////
    double getConfiguredFeedSpeed_MM_MIN() const { return configuredFeedSpeed_MM_SEC * 60; }
    double getConfiguredFeedSpeed_MM_SEC() const { return configuredFeedSpeed_MM_SEC; }

    //////////////////////////////////////////////////////////////////  
    double getRealtimeFeedSpeed_MM_MIN()   const { return realtimeFeedSpeed_MM_SEC * 60; }
    double getRealtimeFeedSpeed_MM_SEC()   const { return realtimeFeedSpeed_MM_SEC; }
    void setRealtimeFeedSpeed_MM_SEC(const double s) { realtimeFeedSpeed_MM_SEC = (s > 0 ? s : 0.0); } 
       
    //////////////////////////////////////////////////////////////////
    void setup(const char axis, unsigned int s, double p, unsigned int oR, unsigned int oS, unsigned int tPW) {
      switch ( axis ) {
        case 'X': X.setup(s, p, oR, oS, tPW); break;
        case 'Y': Y.setup(s, p, oR, oS, tPW); break;
        case 'Z': Z.setup(s, p, oR, oS, tPW); break;
      }
    }
    
    //////////////////////////////////////////////////////////////////
    void setFeedSpeed_MM_MIN(double s) {
      configuredFeedSpeed_MM_SEC = ( s < 0.0 ? 0.0 : s / 60 );
      realtimeFeedSpeed_MM_SEC   = configuredFeedSpeed_MM_SEC;
      
      X.calculateStatics(configuredFeedSpeed_MM_SEC);
      Y.calculateStatics(configuredFeedSpeed_MM_SEC);
      Z.calculateStatics(configuredFeedSpeed_MM_SEC);
    }

    //////////////////////////////////////////////////////////////////
    void simulateSteps(int32_t , int32_t , int32_t ) {
      /*
      const uint32_t ax       = absolute<int32_t>(dx);
      const uint32_t ay       = absolute<int32_t>(dy);
      const uint32_t az       = absolute<int32_t>(dz);
      
      const uint32_t tx       = ax * (X.synthSpeedDelay + X.totalOffset);
      const uint32_t ty       = ay * (Y.synthSpeedDelay + Y.totalOffset);
      const uint32_t tz       = az * (Z.synthSpeedDelay + Z.totalOffset);
      const uint32_t t        = tx + ty + tz;
      
      const uint32_t totalCountOfSteps    = ax + ay + az;
      
      const double totalDistance          = ax * X.gearing
                                          + ay * Y.gearing
                                          + az * Z.gearing;
      
      //const double speed                  = (1000L *1000L * totalDistance) / t * 60; 
      */         
    }

    //////////////////////////////////////////////////////////////////
    #ifdef SKETCH_COMPILE
      void printConfig() {
        
        #define PRINT_PARAMETER( Pid, value ) \
            Serial.print(BLANK); \
            Serial.print(Pid);   Serial.print(TEXT_SEPARATOR); \
            Serial.print(value); Serial.write(TEXT_CLOSE);    
              
        Serial.print(PID_SPEED_CONTROLLER); Serial.print(TEXT_SEPARATOR); Serial.write(TEXT_CLOSE);

          PRINT_PARAMETER(PID_SPEED_CTRL_INITIALIZED,       isInitialized())
          PRINT_PARAMETER(PID_SPEED_CTRL_MAX_SPEED,         getMaxFeedSpeed_MM_MIN())
        
          X.printConfig();
          Y.printConfig();
          Z.printConfig();   
      
        #undef PRINT_PARAMETER    
      }
    #endif 
    
    //////////////////////////////////////////////////////////////////
    #ifndef SKETCH_COMPILE
      friend std::ostream &operator<< (std::ostream &ostr, const CncSpeedController &c) {
        ostr << "CncSpeedController Axis: " << std::endl;
        ostr << c.X;
        ostr << c.Y;
        ostr << c.Z;
        
        ostr                                                                                            << std::endl;
        ostr << "CncSpeedController Parameters: "                                                       << std::endl;
        ostr << " Configured FeedSpeed XYZ:         [mm/sec] : " << c.getConfiguredFeedSpeed_MM_SEC()   << std::endl;
        ostr << " Configured FeedSpeed XYZ:         [mm/min] : " << c.getConfiguredFeedSpeed_MM_MIN()   << std::endl;
        ostr << " Max FeedSpeed XYZ:                [mm/sec] : " << c.getMaxFeedSpeed_MM_SEC()          << std::endl;
        ostr << " Max FeedSpeed XYZ:                [mm/min] : " << c.getMaxFeedSpeed_MM_MIN()          << std::endl;
        
        return ostr;
      }
    #endif
    
};

#endif
