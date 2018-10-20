#ifndef CNC_SPEED_CONTROLLER
#define CNC_SPEED_CONTROLLER

#ifndef SKETCH_COMPILE

    #include <iostream>
    #include "CncCommon.h"
    #include "OSD/CncTimeFunctions.h"
    using namespace cnc;

#else

  #include "CommonValues.h"
    
#endif


    
//////////////////////////////////////////////////////////////////////
#define  startPeriodStepsMin    250
#define  stopPeriodStepsMin     250
#define  periodStepsMin         startPeriodStepsMin + stopPeriodStepsMin
  
class AccelerationProfile {

  private:

    enum Period { P_INACTIVE, P_ACCEL, P_TARGET, P_DEACCEL };
    
    bool enabled;
    
    unsigned int startSpeedDelay;
    unsigned int stopSpeedDelay;
    unsigned int feedSpeedDelay;

    unsigned int startDelayDelta;
    unsigned int stopDelayDelta;

    int32_t startStepCountMark;
    int32_t stopStepCountMark;

    int32_t stepsToMove;
    int32_t stepCounter;

    Period period;

  public:
  
    //////////////////////////////////////////////////////////////////
    explicit AccelerationProfile() 
    : enabled(true)
    , startSpeedDelay(0)
    , stopSpeedDelay(0)
    , feedSpeedDelay(0)
    , startDelayDelta(0)
    , stopDelayDelta(0)
    , startStepCountMark(-1)
    , stopStepCountMark(-1)
    , stepsToMove(0)
    , stepCounter(0)
    , period(P_ACCEL)
    {}    

    /*
    //////////////////////////////////////////////////////////////////
    explicit AccelerationProfile(const AccelerationProfile& ap) 
    : active(ap.isActive())
    , startSpeedDelay(ap.getStartSpeedDelay())
    , stopSpeedDelay(ap.getStopSpeedDelay())
    , feedSpeedDelay(ap.getFeedSpeedDelay())
    , stepCounter(ap.getStepCounter())
    {} */   
    
    //////////////////////////////////////////////////////////////////
    ~AccelerationProfile(){
    }

    //////////////////////////////////////////////////////////////////
    double getStartSpeedDelay() const { return startSpeedDelay; }
    double getStopSpeedDelay()  const { return stopSpeedDelay;  }
    double getFeedSpeedDelay()  const { return feedSpeedDelay;  }

    int32_t getStepCounter()    const { return stepCounter;  }

    //////////////////////////////////////////////////////////////////
    void enable(bool state = true)  { enabled = state; }
    void disable()                  { enable(false);   }
    bool isEnabled()                { return enabled;  }

    //////////////////////////////////////////////////////////////////
    #ifndef SKETCH_COMPILE
      friend std::ostream &operator<< (std::ostream &ostr, const AccelerationProfile &a) {
        
        ostr << "AccelerationProfile:" << std::endl;

        ostr << " startSpeedDelay      : " << a.startSpeedDelay         << std::endl;
        ostr << " stopSpeedDelay       : " << a.stopSpeedDelay          << std::endl;
        ostr << " feedSpeedDelay       : " << a.feedSpeedDelay          << std::endl;
    
        ostr << " startDelayDelta      : " << a.startDelayDelta         << std::endl;
        ostr << " stopDelayDelta       : " << a.stopDelayDelta          << std::endl;
    
        ostr << " startStepCountMark   : " << a.startStepCountMark      << std::endl;
        ostr << " stopStepCountMark    : " << a.stopStepCountMark       << std::endl;
      
        ostr << " stepToMove           : " << a.stepsToMove             << std::endl;
        ostr << " stepCounter          : " << a.stepCounter             << std::endl;   

        ostr << std::endl;
        return ostr;
      }
    #endif
      
    //////////////////////////////////////////////////////////////////
    void setup(double startDelay, double stopDelay) {
      startSpeedDelay = startDelay;
      stopSpeedDelay  = stopDelay;
    }
    
    //////////////////////////////////////////////////////////////////
    void initFeedSpeedDelay(const unsigned int d) {
      feedSpeedDelay = d;
    }
    
    //////////////////////////////////////////////////////////////////
    bool calculate(const int32_t stm) {
      stepsToMove = absolute(stm);
      
      // always reset index
      stepCounter = 0;

      // total period long enough?
      bool active = (stepsToMove > periodStepsMin && feedSpeedDelay > 0); 
      if ( active == true ) {
        
        // determine periods
        startStepCountMark = startPeriodStepsMin;
        stopStepCountMark  = stepsToMove - stopPeriodStepsMin;

        // determin deltas
        startDelayDelta = feedSpeedDelay <= startSpeedDelay ? (startSpeedDelay - feedSpeedDelay) / startStepCountMark : 0;
        stopDelayDelta  = feedSpeedDelay <= stopSpeedDelay  ? (stopSpeedDelay  - feedSpeedDelay) / (stepsToMove - stopStepCountMark) : 0;

        if ( startDelayDelta <= 0 || stopDelayDelta <= 0 )
          active = false;
      } 

      // is total period to small?
      if ( active == false ) {
        
        // determine periods
        startStepCountMark = 0;
        stopStepCountMark  = 0;

        // determin deltas
        startDelayDelta = 0;
        stopDelayDelta  = 0; 
        
        period = P_INACTIVE;
        
      } else {
        period = P_ACCEL;
        
      }
    
      return active;
    }

    //////////////////////////////////////////////////////////////////
    unsigned int getNextAccelDelay() {

      // deactivated - then nothing to add
      if ( enabled == false ) 
        return stepCounter++, 0; 
      
      // is configured speed smaller then startSpeedDelay - then nothing to add
      if ( feedSpeedDelay >= startSpeedDelay )
        return stepCounter++, 0; 

      // increment index
      stepCounter++;
      
      switch ( period ) {

        case P_INACTIVE: {      return (startSpeedDelay - feedSpeedDelay);
          
        }
        case P_ACCEL: {         if ( stepCounter >= startStepCountMark ) 
                                  period = P_TARGET;

                                return startSpeedDelay - ((stepCounter -1) * startDelayDelta);
        }                         
        case P_TARGET: {        if ( stepCounter >= stopStepCountMark )
                                  period = P_DEACCEL;

                                return 0;
        }  
        case P_DEACCEL: {       if ( stepCounter >= stepsToMove )
                                  period = P_INACTIVE;
          
                                return ( stepCounter - stopStepCountMark ) * stopDelayDelta;             
        }
      }

      return (startSpeedDelay - feedSpeedDelay);
    }
};

//////////////////////////////////////////////////////////////////////
const int32_t MAX_FEED_SPEED_VALUE = MIN_LONG;

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
      Axis()
      : axis('#')
      , initialized(false)
      , speedConfigured(false)
      {}
      
      //////////////////////////////////////////////////////////////////
      Axis(const char a)
      : axis(a)
      , initialized(false)
      , speedConfigured(false)
      {}

      Axis(const Axis& a)
      : axis(a.axis)
      , initialized(a.initialized)
      , speedConfigured(a.speedConfigured)
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
          const char* o = " ";
          ostr << "Axis: "                                                                 << std::endl;
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
          ostr << std::endl;
          
          return ostr;
        }
      #endif
    };

    bool initialized;
    double configuredFeedSpeed_MM_SEC;
    double realtimeFeedSpeed_MM_SEC;

   //////////////////////////////////////////////////////////////////
   void setupAccelerationProfile(const char axis, unsigned int s, double p, unsigned int oR, unsigned int oS, unsigned int tPW,
                                 double startSpeed, double stopSpeed) 
   {
      Axis AccelCalcStart, AccelCalcStop;
      AccelCalcStart.setup(s, p, oR, oS, tPW);
      AccelCalcStop.setup(s, p, oR, oS, tPW);

      switch ( axis ) {
        case 'X': AccelCalcStart.calculateStatics(startSpeed);
                  AccelCalcStop.calculateStatics(stopSpeed);
                  APX.setup(AccelCalcStart.synthSpeedDelay, AccelCalcStop.synthSpeedDelay); 
                  break;
                  
        case 'Y': AccelCalcStart.calculateStatics(startSpeed);
                  AccelCalcStop.calculateStatics(stopSpeed);
                  APY.setup(AccelCalcStart.synthSpeedDelay, AccelCalcStop.synthSpeedDelay); 
                  break;
                  
        case 'Z': AccelCalcStart.calculateStatics(startSpeed);
                  AccelCalcStop.calculateStatics(stopSpeed);
                  APZ.setup(AccelCalcStart.synthSpeedDelay, AccelCalcStop.synthSpeedDelay);  
                  break;
      }
    }
    
  public:
    
    Axis X, Y, Z;
    AccelerationProfile APX, APY, APZ;

    //////////////////////////////////////////////////////////////////
    CncSpeedController()
    : initialized(false)
    , configuredFeedSpeed_MM_SEC(0.0)
    , realtimeFeedSpeed_MM_SEC(0.0)
    , X('X')
    , Y('Y')
    , Z('Z')
    , APX() 
    , APY() 
    , APZ()
    {}

    virtual ~CncSpeedController()
    {}

    //////////////////////////////////////////////////////////////////////////
    virtual void initMove(int32_t dx, int32_t dy, int32_t dz) {
      APX.calculate(dx);
      APY.calculate(dy);
      APZ.calculate(dz);
    }

    //////////////////////////////////////////////////////////////////////////
    virtual void completeMove() {}

    //////////////////////////////////////////////////////////////////////////
    unsigned int getNextAccelDelayX()            { return APX.getNextAccelDelay(); }
    unsigned int getNextAccelDelayY()            { return APY.getNextAccelDelay(); }
    unsigned int getNextAccelDelayZ()            { return APZ.getNextAccelDelay(); }

    void enableAccelerationX(bool state)         { APX.enable(state); }
    void enableAccelerationY(bool state)         { APY.enable(state); }
    void enableAccelerationZ(bool state)         { APZ.enable(state); }

    void enableAccelerationXYZ(bool state)       { enableAccelerationX(state); enableAccelerationY(state); enableAccelerationZ(state);}
    
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
      
      #ifdef SKETCH_COMPILE
        #warning move this to config
      #endif
      const double startSpeed = 15.0; // mm/sec
      const double stopSpeed  = 15.0;

      setupAccelerationProfile(axis, s, p, oR, oS, tPW, startSpeed, stopSpeed);
      
      switch ( axis ) {
        case 'X': X.setup(s, p, oR, oS, tPW); break;
        case 'Y': Y.setup(s, p, oR, oS, tPW); break;
        case 'Z': Z.setup(s, p, oR, oS, tPW); break;
      }
    }
    
    //////////////////////////////////////////////////////////////////
    void setFeedSpeed_MM_SEC(double s) {
      configuredFeedSpeed_MM_SEC = ( s < 0.0 ? 0.0 : s );
      realtimeFeedSpeed_MM_SEC   = configuredFeedSpeed_MM_SEC;
      
      X.calculateStatics(configuredFeedSpeed_MM_SEC);
      Y.calculateStatics(configuredFeedSpeed_MM_SEC);
      Z.calculateStatics(configuredFeedSpeed_MM_SEC);

      APX.initFeedSpeedDelay(X.synthSpeedDelay);
      APY.initFeedSpeedDelay(Y.synthSpeedDelay);
      APZ.initFeedSpeedDelay(Z.synthSpeedDelay);      
    }
    
    //////////////////////////////////////////////////////////////////
    void setFeedSpeed_MM_MIN(double s) {
      setFeedSpeed_MM_SEC(s/60);
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
          PRINT_PARAMETER(PID_SPEED_CTRL_CFG_SPEED,         getConfiguredFeedSpeed_MM_MIN())
        
          X.printConfig();
          Y.printConfig();
          Z.printConfig();        
  
        #undef PRINT_PARAMETER    
      }
      
    #endif 

    //////////////////////////////////////////////////////////////////
    uint32_t getTimeStamp() {
      #ifndef SKETCH_COMPILE
        return CncTimeFunctions::getNanoTimestamp() / 1000;
      #else
        return micros();
      #endif
    }       
    
    //////////////////////////////////////////////////////////////////
    #ifndef SKETCH_COMPILE
      friend std::ostream &operator<< (std::ostream &ostr, const CncSpeedController &c) {
       
        ostr << "CncSpeedController"                                                                    << std::endl;
        ostr << " Configured FeedSpeed XYZ:         [mm/sec] : " << c.getConfiguredFeedSpeed_MM_SEC()   << std::endl;
        ostr << " Configured FeedSpeed XYZ:         [mm/min] : " << c.getConfiguredFeedSpeed_MM_MIN()   << std::endl;
        ostr << " Max FeedSpeed XYZ:                [mm/sec] : " << c.getMaxFeedSpeed_MM_SEC()          << std::endl;
        ostr << " Max FeedSpeed XYZ:                [mm/min] : " << c.getMaxFeedSpeed_MM_MIN()          << std::endl;
        
        return ostr;
      }
    #endif
    
};

#endif
