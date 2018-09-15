#ifndef CNC_SPEED_MANAGER
#define CNC_SPEED_MANAGER

#ifndef SKETCH_COMPILE

    #include <iostream>
    #include "CncCommon.h"
    #include "OSD/CncTimeFunctions.h"
    using namespace cnc;

#else

	#include "CommonValues.h"
    #include "CommonFunctions.h"
    
#endif

const int32_t MAX_FEED_SPEED_VALUE = MIN_LONG;

//////////////////////////////////////////////////////////////////////////////////
class CncSpeedManager {
    
    protected:

      //////////////////////////////////////////////////////////////////////////
      struct Measurement {
        uint32_t tsStart     = 0L;
        uint32_t tsEnd       = 0L;
        
        uint32_t timeElapsed = 0L;
        double distance           = 0.0;
        double feedSpeed          = 0.0;
  
        Measurement()  { reset(); }
        ~Measurement() {}
  
        void reset(double sValue = 0.0) {
          tsStart     = 0L;
          tsEnd       = 0L;
          
          timeElapsed = 0L;
          distance    = 0.0;
          feedSpeed   = sValue;
        }
      };
      
      //////////////////////////////////////////////////////////////////////////
        Measurement measurement; 
   
        bool initialized;
        bool traceOn;
        
        double configuredFeedSpeed;
        double currentMoveDistance;
        double runningDistance;
        
        double maxFeedSpeedX, maxFeedSpeedY, maxFeedSpeedZ;
        double gearingX, gearingY, gearingZ;
        
        int currentCorrectionValue;
        unsigned int constStepStaticOffset, constStepLoopOffset;
     
        unsigned int tPulseOffsetX, tPulseOffsetY, tPulseOffsetZ;
        unsigned int maxStepsX, maxStepsY, maxStepsZ;
        unsigned int maxRpmX, maxRpmY, maxRpmZ;
        unsigned int totalOffsetX, totalOffsetY, totalOffsetZ;
        unsigned int perStepOffsetX, perStepOffsetY, perStepOffsetZ;
        
        int32_t lStepsX, lStepsY, lStepsZ;
        
        //////////////////////////////////////////////////////////////////////////
        void reset() {
            lStepsX = lStepsY = lStepsZ = 0;
            resetOffset();
        }
        
        //////////////////////////////////////////////////////////////////////////
        void resetOffset() {
            totalOffsetX   = totalOffsetY   = totalOffsetZ   = 0;
            perStepOffsetX = perStepOffsetY = perStepOffsetZ = 0;
        }
        
        //////////////////////////////////////////////////////////////////////////
        bool calculate() {
            #ifndef SKETCH_COMPILE
              if ( traceOn ) {
                std::cout << "SpeedManager::calculate()" << std::endl;
              }
            #endif
            
            // Quality check
            if ( dblCompareNull(configuredFeedSpeed) == true )  { reset(); return false; }
            if ( initialized == false )                         { reset(); return false; }

            // Calculate - steps are absolute
            const int32_t tSteps = lStepsX + lStepsY + lStepsZ; 							// Total [steps]
            if ( tSteps == 0 ) { resetOffset(); return true; }
        
            const double fx = (double)(lStepsX) / tSteps; 								// Factor [%]
            const double fy = (double)(lStepsY) / tSteps; 								// Factor [%]
            const double fz = (double)(lStepsZ) / tSteps; 								// Factor [%]
            
            const double distanceX = lStepsX * gearingX; 								  // [mm]
            const double distanceY = lStepsY * gearingY; 							  	// [mm]
            const double distanceZ = lStepsZ * gearingZ; 								  // [mm]
            
            currentMoveDistance             = sqrt(  distanceX * distanceX
            		                                   + distanceY * distanceY
												                           + distanceZ * distanceZ
												                          );     								  // [mm]

            const double totalTime          = (currentMoveDistance / configuredFeedSpeed)
            		                            * 1000.0 * 1000.0; 						// [us]
            
            const double totalLoopDuration  = lStepsX * tPulseOffsetX
            		                            + lStepsY * tPulseOffsetY
											                      + lStepsZ * tPulseOffsetZ;		// [us]

            const double restLoopDuration   = totalTime 
                                            - totalLoopDuration;			    // [us]

            // ------------------------------------------------------------> totalTime
            // -------------------------------->                             totalLoopDuration
            //                                  ---------------------------> restLoopDuration
            //
            // restLoopDuration is the timespan which has to be syntactically waited
            // to realize the configured feed speed

            measurement.distance     += currentMoveDistance;							// [mm]

            // Is syntactically wait required?
            //if ( restLoopDuration < 0.0 || dblCompareNull(restLoopDuration) ) { resetOffset(); return true; }
            
            totalOffsetX              = ( lStepsX > 0 ? (fx * restLoopDuration) : 0 ); 	// [us]
            totalOffsetY              = ( lStepsY > 0 ? (fy * restLoopDuration) : 0 ); 	// [us]
            totalOffsetZ              = ( lStepsZ > 0 ? (fz * restLoopDuration) : 0 ); 	// [us]
            
            perStepOffsetX            = ( lStepsX > 0 ? totalOffsetX / lStepsX : 0 );  	// [us/step]
            perStepOffsetY            = ( lStepsY > 0 ? totalOffsetY / lStepsY : 0 );  	// [us/step]
            perStepOffsetZ            = ( lStepsZ > 0 ? totalOffsetZ / lStepsZ : 0 );  	// [us/step]
            
            #ifndef SKETCH_COMPILE
              if ( traceOn ) {
				  /*
                std::cout << " T: " << tSteps << ", " << restLoopDuration << ", " << currentDistance << ", " <<  totalLoopDuration << ", " << totalTime << ", " << configuredFeedSpeed << endl;
                std::cout << " X: " << totalOffsetX << ", " << perStepOffsetX << ", " << lStepsX << ", " << fx << endl;
                std::cout << " Y: " << totalOffsetY << ", " << perStepOffsetY << ", " << lStepsY << ", " << fy << endl;
                std::cout << " Z: " << totalOffsetZ << ", " << perStepOffsetZ << ", " << lStepsZ << ", " << fz << endl;
				 * */
              }
            #endif

            return true;
        }

        //////////////////////////////////////////////////////////////////////////
        inline uint32_t getTimeStamp() {
          #ifndef SKETCH_COMPILE
            return CncTimeFunctions::getNanoTimestamp() / 1000;
          #else
            return micros();
          #endif
        }
         
    public:
        
        //////////////////////////////////////////////////////////////////////////
        CncSpeedManager() 
        : measurement()
        , initialized(false)
        , traceOn(false)
        , configuredFeedSpeed(0.0)
        , currentMoveDistance(0.0)
        , runningDistance(0.0)
        , maxFeedSpeedX(0.0), maxFeedSpeedY(0.0), maxFeedSpeedZ(0.0)
        , gearingX(0.0), gearingY(0.0), gearingZ(0.0)
        , constStepStaticOffset(0), constStepLoopOffset(0)
        , tPulseOffsetX(0), tPulseOffsetY(0), tPulseOffsetZ(0)
        , maxStepsX(0), maxStepsY(0), maxStepsZ(0)
        , maxRpmX(0), maxRpmY(0), maxRpmZ(0)
        , totalOffsetX(0), totalOffsetY(0), totalOffsetZ(0)
        , perStepOffsetX(0), perStepOffsetY(0), perStepOffsetZ(0)
        , lStepsX(0), lStepsY(0), lStepsZ(0)
        {
        }
        
        //////////////////////////////////////////////////////////////////////////
        CncSpeedManager(unsigned int cStepStaticOffset, unsigned int cStepLoopOffset, 
                        double pitchX, unsigned int stepsX, unsigned int pulseOffsetX,
                        double pitchY, unsigned int stepsY, unsigned int pulseOffsetY,
                        double pitchZ, unsigned int stepsZ, unsigned int pulseOffsetZ) 
        : measurement()
        , initialized(false)
        , traceOn(false)
        , configuredFeedSpeed(0.0)
        , currentMoveDistance(0.0)
        , runningDistance(0.0)
        , maxFeedSpeedX(0.0), maxFeedSpeedY(0.0), maxFeedSpeedZ(0.0)
        , gearingX(0.0), gearingY(0.0), gearingZ(0.0)
        , constStepStaticOffset(cStepStaticOffset), constStepLoopOffset(cStepLoopOffset)
        , tPulseOffsetX(0), tPulseOffsetY(0), tPulseOffsetZ(0)
        , maxStepsX(0), maxStepsY(0), maxStepsZ(0)
        , maxRpmX(0), maxRpmY(0), maxRpmZ(0)
        , totalOffsetX(0), totalOffsetY(0), totalOffsetZ(0)
        , perStepOffsetX(0), perStepOffsetY(0), perStepOffsetZ(0)
        , lStepsX(0), lStepsY(0), lStepsZ(0)
        {
            setup(  cStepLoopOffset, cStepStaticOffset,
                    pitchX, stepsX, pulseOffsetX,
                    pitchY, stepsY, pulseOffsetY,
                    pitchZ, stepsZ, pulseOffsetZ);
        }
        
        //////////////////////////////////////////////////////////////////////////
        virtual ~CncSpeedManager() {
        }
        
        //////////////////////////////////////////////////////////////////////////
        bool setup( unsigned int cStepStaticOffset, unsigned int cStepLoopOffset,
                    double pitchX, unsigned int stepsX, unsigned int pulseOffsetX,
                    double pitchY, unsigned int stepsY, unsigned int pulseOffsetY,
                    double pitchZ, unsigned int stepsZ, unsigned int pulseOffsetZ) {
                    
            initialized = false;
                        
            if ( stepsX == 0 || stepsY == 0 || stepsZ == 0 )
                return initialized;
            
            if ( dblCompareNull(pitchX) || dblCompareNull(pitchY) | dblCompareNull(pitchZ) )
                return initialized;
            
            if ( pulseOffsetX == 0 || pulseOffsetY == 0 || pulseOffsetZ == 0 )
                return initialized;
            
            // preconfig/setup values
            constStepStaticOffset = cStepStaticOffset;                         // [us]
            constStepLoopOffset   = cStepLoopOffset;                           // [us]
            
            gearingX      = (double)(pitchX / stepsX);                         // [mm/steps]
            gearingY      = (double)(pitchY / stepsY);                         // [mm/steps]
            gearingZ      = (double)(pitchZ / stepsZ);                         // [mm/steps]
            
            tPulseOffsetX = pulseOffsetX + cStepLoopOffset;                    // [us]         - contains HIGH and LOW period + loop offset
            tPulseOffsetY = pulseOffsetY + cStepLoopOffset;                    // [us]         - contains HIGH and LOW period + loop offset
            tPulseOffsetZ = pulseOffsetZ + cStepLoopOffset;                    // [us]         - contains HIGH and LOW period + loop offset

            const double base = 1000L * 1000L - cStepStaticOffset;
            maxStepsX     = (unsigned int)(base / tPulseOffsetX);               // [steps/s]    
            maxStepsY     = (unsigned int)(base / tPulseOffsetY);               // [steps/s]    
            maxStepsZ     = (unsigned int)(base / tPulseOffsetZ);               // [steps/s]  

             maxStepsX = 4000;
              maxStepsY = 4000;
               maxStepsZ = 4000;
        
            maxFeedSpeedX = gearingX * maxStepsX;                              // [mm/s]
            maxFeedSpeedY = gearingY * maxStepsY;                              // [mm/s]
            maxFeedSpeedZ = gearingZ * maxStepsZ;                              // [mm/s]

            maxRpmX       = maxStepsX/stepsX * 60;                             // [rpm]
            maxRpmY       = maxStepsY/stepsY * 60;                             // [rpm]
            maxRpmZ       = maxStepsZ/stepsZ * 60;                             // [rpm]
            
            traceSetup();
            
            initialized = true;
            return initialized;
        }

        //////////////////////////////////////////////////////////////////////////
        void traceSetup() {
          #ifndef SKETCH_COMPILE
              std::cout << "SpeedManager::setup()" << std::endl;
              std::cout << "X: tOffset:" << tPulseOffsetX << ", mSteps: " << maxStepsX << ", mSpeed [mm/min]: " << maxFeedSpeedX * 60 << " mRpm : " << maxRpmX << ", gearing: " << gearingX << endl;
              std::cout << "Y: tOffset:" << tPulseOffsetY << ", mSteps: " << maxStepsY << ", mSpeed [mm/min]: " << maxFeedSpeedY * 60 << " mRpm : " << maxRpmY << ", gearing: " << gearingY << endl;
              std::cout << "Z: tOffset:" << tPulseOffsetZ << ", mSteps: " << maxStepsZ << ", mSpeed [mm/min]: " << maxFeedSpeedZ * 60 << " mRpm : " << maxRpmZ << ", gearing: " << gearingX << endl;
          #endif          
        }
       
        //////////////////////////////////////////////////////////////////////////
        virtual void initMove() {}
    
        //////////////////////////////////////////////////////////////////////////
        virtual void completeMove() {}
        
        //////////////////////////////////////////////////////////////////////////
        // fm	 			  :	[mm/min]
        // configuredFeedSpeed: [mm/s]
        //
        void setFeedSpeed(double fm) {

        	// internal speed values a in [mm/s]
        	fm /= 60.0;

          // do nothing if the value isn't changed 
          if (  dblCompare(fm, configuredFeedSpeed ) == true )
              return;

          // set
          if ( fm > 0.0 ) configuredFeedSpeed = fm;
          else            configuredFeedSpeed = 0.0;

          reset();
          measurement.reset(configuredFeedSpeed * 60.0);
        }
                
        //////////////////////////////////////////////////////////////////////////
        // dx, dy, dz:		[steps]
        //
        void setNextMove(int32_t dx, int32_t dy, int32_t dz) {
            // log start point
            if ( measurement.tsStart == 0L )
                measurement.tsStart = getTimeStamp();
      
            lStepsX = absolute<int32_t>(dx);
            lStepsY = absolute<int32_t>(dy);
            lStepsZ = absolute<int32_t>(dz);
            
            // calculate the variable getters - below
            calculate();

            // give inherited classes a change to do something
            initMove();
        }

        //////////////////////////////////////////////////////////////////////////
        double appendToRunningDistance(short dx, short dy, short dz) {
          const double x = gearingX * dx;
          const double y = gearingY * dy;
          const double z = gearingZ * dz;
          
          runningDistance += sqrt( x * x + y * y + z * z ); // [mm]

          return runningDistance;
        }

        //////////////////////////////////////////////////////////////////////////
        void finalizeMove() {
          // give inherited classes a change to do something
          completeMove();
    
          // log end point
          determineFeedSpeed();
        }

        //////////////////////////////////////////////////////////////////////////
        void determineFeedSpeed() {
           // log current point
          if ( measurement.tsStart != 0L )
            measurement.tsEnd = getTimeStamp();

            measurement.timeElapsed = measurement.tsEnd
                                    - measurement.tsStart;     // [us]
                                     
          // calculate current feed speed - consider time stamp overflow
          // for more details please see micros()
          // and consider timespans > 1 ms only
          if ( measurement.timeElapsed > 1000 ) {

            // To avoid underflows reorganize the formula
            // mm / (us / 1000 * 1000) ==> (mm * 1000 * 1000) / us
            // last but not least convert from [mm/sec] to [mm/min]
            measurement.feedSpeed    = ((1000L * 1000L * measurement.distance)
                                     / measurement.timeElapsed)
                                     * 60;                      // [mm/min]

          } else {
            
            measurement.feedSpeed    = configuredFeedSpeed
                                     * 60;                      // [mm/min]
          }
        }
        //////////////////////////////////////////////////////////////////////////
        bool isInitialized()                        const { return initialized; }
        
        //////////////////////////////////////////////////////////////////////////
        uint32_t getMeasurementTimeElapsed() 	      const { return measurement.timeElapsed; }
        double getMeasurementDistance()           	const { return measurement.distance; }
        double getMeasurementFeedSpeed_MM_MIN()   	const { return measurement.feedSpeed; }
        
        //////////////////////////////////////////////////////////////////////////
        double getConfiguredFeedSpeed_MM_SEC()    	const { return configuredFeedSpeed; }
        double getConfiguredFeedSpeed_MM_MIN()    	const { return configuredFeedSpeed * 60; }
        
        //////////////////////////////////////////////////////////////////////////
        unsigned int getTotalOffsetX()      		const { return totalOffsetX; }
        unsigned int getTotalOffsetY()      		const { return totalOffsetY; }
        unsigned int getTotalOffsetZ()      		const { return totalOffsetZ; }
    
        unsigned int getOffsetPerStepX()    		const { return perStepOffsetX; }
        unsigned int getOffsetPerStepY()    		const { return perStepOffsetY; }
        unsigned int getOffsetPerStepZ()    		const { return perStepOffsetZ; }
        
        //////////////////////////////////////////////////////////////////////////
        double getCurrentMoveDistance()         const { return currentMoveDistance; }
        
        //////////////////////////////////////////////////////////////////////////
        unsigned int getConstStepStaticOffset() const { return constStepStaticOffset; }
        unsigned int getConstStepLoopOffset()   const { return constStepLoopOffset; }
        
        unsigned int getTotalPulseOffsetX() 		const { return tPulseOffsetX; }
        unsigned int getTotalPulseOffsetY() 		const { return tPulseOffsetY; }
        unsigned int getTotalPulseOffsetZ() 		const { return tPulseOffsetZ; }
        
        unsigned int getLowPulseWidthX()    		const { return (tPulseOffsetX - constStepLoopOffset) / 2; }
        unsigned int getLowPulseWidthY()    		const { return (tPulseOffsetY - constStepLoopOffset) / 2; }
        unsigned int getLowPulseWidthZ()    		const { return (tPulseOffsetZ - constStepLoopOffset) / 2; }
        
        unsigned int getHighPulseWidthX()   		const { return (tPulseOffsetX - constStepLoopOffset) / 2; }
        unsigned int getHighPulseWidthY()   		const { return (tPulseOffsetY - constStepLoopOffset) / 2; }
        unsigned int getHighPulseWidthZ()   		const { return (tPulseOffsetZ - constStepLoopOffset) / 2; }
    
        //////////////////////////////////////////////////////////////////////////
        double getMaxSpeedX_MM_MIN()        		const { return maxFeedSpeedX * 60; }
        double getMaxSpeedY_MM_MIN()        		const { return maxFeedSpeedY * 60; }
        double getMaxSpeedZ_MM_MIN()        		const { return maxFeedSpeedZ * 60; }
    
        double getMaxSpeedY_MM_SEC()        		const { return maxFeedSpeedY; }
        double getMaxSpeedX_MM_SEC()        		const { return maxFeedSpeedX; }
        double getMaxSpeedT_MM_SEC()        		const { return maxFeedSpeedZ; }
        
        //////////////////////////////////////////////////////////////////////////
        unsigned int getMaxStepsX()         		const { return maxStepsX; }
        unsigned int getMaxStepsY()         		const { return maxStepsY; }
        unsigned int getMaxStepsZ()         		const { return maxStepsZ; }

        //////////////////////////////////////////////////////////////////////////
        unsigned int getMaxRpmX()               const { return maxRpmX; }
        unsigned int getMaxRpmY()               const { return maxRpmY; }
        unsigned int getMaxRpmZ()               const { return maxRpmZ; } 
};

#endif

