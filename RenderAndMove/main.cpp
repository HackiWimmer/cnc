#include <stdio.h>
#include <cstdlib>
#include <stdint.h>
#include <iostream>
#include <cstring>


static const short POINT_LENGTH = 3;
static const short PIX  = 0;
static const short PIY  = 1;
static const short PIZ  = 2;

int pointA[POINT_LENGTH], pointB[POINT_LENGTH];

///////////////////////////////////////////////////////////////////
bool provideMove(int32_t dx , int32_t dy , int32_t dz, unsigned char *buffer, unsigned int nbByte, bool force=false) {
///////////////////////////////////////////////////////////////////
	std::cout << "   provideMove: " << nbByte << " - " << dx << ", " << dy << ", " << dz << std::endl;
	
	// copy point A into point B
	memcpy(pointB, pointA, sizeof(pointA));
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool renderMove(int32_t dx , int32_t dy , int32_t dz, unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( dx == 0 && dy == 0 && dz == 0 ) 
		return true;
	
	
	
	// update speed simulator values
	/*
	if ( GBL_CONFIG->isProbeMode() == false ) {
		wxASSERT( speedSimulator != NULL );
		speedSimulator->setNextMove(dx, dy, dz);
	}*/
	
	bool render = true;
	
	//if ( dx == 0 && dz == 0 )  render = false;
	//if ( dy == 0 && dz == 0 )  render = false;
	//if ( dx == 0 && dy == 0 )  render = false;
	
	
	if ( render == true ) {
		// initialize
		int i, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
		memset(pointA, 0, sizeof(pointA));
		memset(pointB, 0, sizeof(pointB));
		
		x_inc = (dx < 0) ? -1 : 1;
		l = abs(dx);
		
		y_inc = (dy < 0) ? -1 : 1;
		m = abs(dy);
		
		z_inc = (dz < 0) ? -1 : 1;
		n = abs(dz);
		
		dx2 = l << 1;
		dy2 = m << 1;
		dz2 = n << 1;

		// -------------------------------------------------------------
		if ((l >= m) && (l >= n)) {
			err_1 = dy2 - l;
			err_2 = dz2 - l;
			for (i = 0; i < l; i++) {
				
				if ( provideMove(pointA[PIX] - pointB[PIX], pointA[PIY] - pointB[PIY], pointA[PIZ] - pointB[PIZ], buffer, 1) == false )
					return false;
				
				if (err_1 > 0) { pointA[1] += y_inc; err_1 -= dx2; }
				if (err_2 > 0) { pointA[2] += z_inc; err_2 -= dx2; }
				
				err_1     += dy2;
				err_2     += dz2;
				pointA[0] += x_inc;
			}
		
		// -------------------------------------------------------------
		} else if ((m >= l) && (m >= n)) {
			err_1 = dx2 - m;
			err_2 = dz2 - m;
			for (i = 0; i < m; i++) {
				
				if ( provideMove(pointA[PIX] - pointB[PIX], pointA[PIY] - pointB[PIY], pointA[PIZ] - pointB[PIZ], buffer, 2) == false )
					return false;
				
				if (err_1 > 0) { pointA[0] += x_inc; err_1 -= dy2; }
				if (err_2 > 0) { pointA[2] += z_inc; err_2 -= dy2; }
				
				err_1     += dx2;
				err_2     += dz2;
				pointA[1] += y_inc;
			}
			
		// -------------------------------------------------------------
		} else {
			err_1 = dy2 - n;
			err_2 = dx2 - n;
			for (i = 0; i < n; i++) {
				
				if ( provideMove(pointA[PIX] - pointB[PIX], pointA[PIY] - pointB[PIY], pointA[PIZ] - pointB[PIZ], buffer, 3) == false )
					return false;
				
				if (err_1 > 0) { pointA[1] += y_inc; err_1 -= dz2; }
				if (err_2 > 0) { pointA[0] += x_inc; err_2 -= dz2; }
				
				err_1     += dy2;
				err_2     += dx2;
				pointA[2] += z_inc;
			}
		}
		
		dx = pointA[PIX] - pointB[PIX];
		dy = pointA[PIY] - pointB[PIY];
		dz = pointA[PIZ] - pointB[PIZ];
	}
	
	// -------------------------------------------------------------
	if ( provideMove(dx, dy, dz, buffer, 4, true) == false )
		return false;
	
	// perform any rest offset
	/*
	if ( GBL_CONFIG->isProbeMode() == false ) {
		wxASSERT( speedSimulator != NULL );
		speedSimulator->performCurrentOffset(true);
		speedSimulator->finalizeMove();
	}*/
	return true;
}

///////////////////////////////////////////////////////////////////
bool W_renderMove(int32_t dx , int32_t dy , int32_t dz, unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	std::cout << "renderMove(" << dx << ", " << dy << ", " << dz << ")\n{" << std::endl;
		bool ret = renderMove(dx , dy , dz, buffer, nbByte);
	std::cout << "}" << std::endl;
	
	return ret;
}
///////////////////////////////////////////////////////////////////
int mainX(int argc, char **argv) {
///////////////////////////////////////////////////////////////////

	unsigned char *buffer = NULL;
	unsigned int nbByte   = 0;

	
	W_renderMove(   0,   0,   0, buffer, nbByte);
	W_renderMove(   5,   0,   0, buffer, nbByte);
	W_renderMove(   0,   5,   0, buffer, nbByte);
	W_renderMove(   0,   0,   5, buffer, nbByte);
	
	W_renderMove(   5,   5,   0, buffer, nbByte);
	W_renderMove(   5,   5,   5, buffer, nbByte);
	W_renderMove(  10,   5,   5, buffer, nbByte);
	
	W_renderMove( -10,   5,  -5, buffer, nbByte);
	
	W_renderMove(-100,   5,  -5, buffer, nbByte);
	
	
	
	
	
	printf("\nbye bye\n");
	return 0;
}

template <class T>
T minimum ( T v1, T v2, T v3) {
	return std::min( v3, std::min(v1, v2));
}

template <class T>
T maximum ( T v1, T v2, T v3) {
	return std::max( v3, std::max(v1, v2));
}

template <class T>
inline T absolute(T val) {
	if ( val < 0.0 )
	  val *= -1;
	  
	return val;
}

template <>
inline int8_t absolute(int8_t x) {
	const int8_t a = x >> 7;
	return ((x ^ a) - a);
}

template <>
inline int16_t absolute(int16_t x) {
	const int16_t a = x >> 15;
	return ((x ^ a) - a);
}

template <>
inline int32_t absolute(int32_t x) {
	const int32_t a = x >> 31;
	return ((x ^ a) - a);
}

//////////////////////////////////////////////////////////////////////
#define  startPeriodFactor      0.2
#define  stopPeriodFactor       0.2
#define  targetPeriodFactor     1.0 - startPeriodFactor - stopPeriodFactor

#define  startPeriodStepsMin    100
#define  stopPeriodStepsMin     100
#define  periodStepsMin         startPeriodStepsMin + stopPeriodStepsMin
  
class AccelerationProfile {

  private:
    bool active;
    unsigned int startSpeedDelay;
    unsigned int stopSpeedDelay;
    unsigned int feedSpeedDelay;

    unsigned int startDelayDelta;
    unsigned int stopDelayDelta;

    int32_t startStepCountMark;
    int32_t stopStepCountMark;
  
    int32_t stepCounter;

  public:
  
    //////////////////////////////////////////////////////////////////
    explicit AccelerationProfile() 
    : active(true)
    , startSpeedDelay(0)
    , stopSpeedDelay(0)
    , feedSpeedDelay(0)
    , startDelayDelta(0)
    , stopDelayDelta(0)
    , startStepCountMark(-1)
    , stopStepCountMark(-1)
    , stepCounter(0)
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
    virtual ~AccelerationProfile(){
    }

    //////////////////////////////////////////////////////////////////
    bool isActive()             const { return active; }
    double getStartSpeedDelay() const { return startSpeedDelay; }
    double getStopSpeedDelay()  const { return stopSpeedDelay;  }
    double getFeedSpeedDelay()  const { return feedSpeedDelay;  }

    int32_t getStepCounter()    const { return stepCounter;  }
    
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
    bool calculate(const int32_t stepsToMove) {
      int32_t stm = absolute(stepsToMove);
      
      // reset index
      stepCounter = 0;

      // total period long enough?
      active = (stm > periodStepsMin); 
      if ( active == true ) {
        
        // determine periods
        startStepCountMark = stm * startPeriodFactor;
        stopStepCountMark  = stm - startStepCountMark - (stm * stopPeriodFactor);

        // determin deltas
        startDelayDelta = (startSpeedDelay - feedSpeedDelay) / startStepCountMark;
        stopDelayDelta  = (stopSpeedDelay  - feedSpeedDelay) / (stm - stopStepCountMark);

        if ( startDelayDelta <= 0 || stopDelayDelta <= 0 )
          active = false;
        
      } 
      
      if ( active == false ) {
        
        // determine periods
        startStepCountMark = 0;
        stopStepCountMark  = 0;

        // determin deltas
        startDelayDelta = 0;
        stopDelayDelta  = 0;       
      }
      
      
      return active;
    }

    //////////////////////////////////////////////////////////////////
    unsigned int getNextAccelDelay() {

      // is configured speed to low - then nothing to add
      if ( feedSpeedDelay >= startSpeedDelay )      { return stepCounter++, 0; }

      // is total period to small - then add a delay to archive the start speed  
      if ( active == false )                        { return stepCounter++, (startSpeedDelay - feedSpeedDelay); }
      
      // is targed period active - then nothing to add
      if (        stepCounter > startStepCountMark && stepCounter < stopStepCountMark ) { 
        return stepCounter++, 0; 
        
      } else if ( stepCounter <= startStepCountMark ) {
        stepCounter++;
        return startSpeedDelay - ((stepCounter -1) * startDelayDelta);
        
      } else if ( stepCounter >= stopStepCountMark ) {
        stepCounter++;
        return ( stepCounter - stopStepCountMark )* stopDelayDelta;
      }

      // error
      
      return 0;
    }
  
};

//////////////////////////////////////////////////////////////////////
const int32_t MAX_FEED_SPEED_VALUE = -1;

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

    unsigned int ax() { return APX.getNextAccelDelay(); }
    
    //////////////////////////////////////////////////////////////////////////
    virtual void completeMove() {}

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
      
      const double startSpeed = 5.0; // mm/sec
      const double stopSpeed  = 5.0;

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


///////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
///////////////////////////////////////////////////////////////////

	CncSpeedController sc;
	sc.X.setup(800, 12.0, 12+50, 30, 50);
	sc.Y.setup(800, 12.0, 12+50, 30, 50);
	sc.Z.setup(800, 12.0, 12+50, 30, 50);
	
	sc.setFeedSpeed_MM_MIN(3600);
	std::cout << sc;
	
	sc.simulateSteps(10, 5, 2);
	
	printf("\nbye bye\n");
	return 0;
}
