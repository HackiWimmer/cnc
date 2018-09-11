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

const long MAX_FEED_SPEED_VALUE = MIN_LONG;

//////////////////////////////////////////////////////////////////////////////////
class CncSpeedManager {
    
    protected:

      //////////////////////////////////////////////////////////////////////////
      struct Measurement {
        unsigned long tsStart     = 0L;
        unsigned long tsEnd       = 0L;
        
        unsigned long timeElapsed = 0L;
        double distance           = 0.0;
        double feedSpeed          = 0.0;
  
        Measurement()  { reset(); }
        ~Measurement() {}
  
        void reset() {
          tsStart     = 0L;
          tsEnd       = 0L;
          
          timeElapsed = 0L;
          distance    = 0.0;
          feedSpeed   = MAX_FEED_SPEED_VALUE;
        }
      };
      
      //////////////////////////////////////////////////////////////////////////
        Measurement measurement; 
   
        bool initialized;

        double configuredFeedSpeed;
        double currentDistance;
        
        double maxFeedSpeedX, maxFeedSpeedY, maxFeedSpeedZ;
        double gearingX, gearingY, gearingZ;
        
        int currentCorrectionValue;
        unsigned int constOffset;
        
        unsigned int tPulseOffsetX, tPulseOffsetY, tPulseOffsetZ;
        unsigned int maxStepsX, maxStepsY, maxStepsZ;
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
        void calculate() {
            // Quality check
            if ( dblCompareNull(configuredFeedSpeed) == true )  { reset(); return; }
            if ( initialized == false )                         { reset(); return; }

            // Calculate
            const long tSteps = lStepsX + lStepsY + lStepsZ; 							// Total [steps]
            if ( tSteps == 0 ) { resetOffset(); return; }
            
            const double fx = (double)(lStepsX) / tSteps; 								// Factor [%]
            const double fy = (double)(lStepsY) / tSteps; 								// Factor [%]
            const double fz = (double)(lStepsZ) / tSteps; 								// Factor [%]
            
            const double distanceX = lStepsX * gearingX; 								  // [mm]
            const double distanceY = lStepsY * gearingY; 							  	// [mm]
            const double distanceZ = lStepsZ * gearingZ; 								  // [mm]
            
            currentDistance                 = sqrt(  distanceX * distanceX
            		                                   + distanceY * distanceY
												                           + distanceZ * distanceZ
												                          );     								  // [mm]

            const double totalTime          = (currentDistance / configuredFeedSpeed)
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

            measurement.distance     += currentDistance;								// [mm]

            // Is syntactically wait required?
            if ( restLoopDuration < 0.0 || dblCompareNull(restLoopDuration) ) { resetOffset(); return; }
            
            totalOffsetX              = ( lStepsX > 0 ? (fx * restLoopDuration) : 0 ); 	// [us]
            totalOffsetY              = ( lStepsY > 0 ? (fy * restLoopDuration) : 0 ); 	// [us]
            totalOffsetZ              = ( lStepsZ > 0 ? (fz * restLoopDuration) : 0 ); 	// [us]
            
            perStepOffsetX            = ( lStepsX > 0 ? totalOffsetX / lStepsX : 0 );  	// [us/step]
            perStepOffsetY            = ( lStepsY > 0 ? totalOffsetY / lStepsY : 0 );  	// [us/step]
            perStepOffsetZ            = ( lStepsZ > 0 ? totalOffsetZ / lStepsZ : 0 );  	// [us/step]
            
            #ifndef SKETCH_COMPILE
              if ( false ) {
                std::cout << "T: " << tSteps << ", " << restLoopDuration << ", " << currentDistance << ", " <<  totalLoopDuration << ", " << totalTime << ", " << configuredFeedSpeed << endl;
                std::cout << "X: " << totalOffsetX << ", " << perStepOffsetX << ", " << lStepsX << ", " << fx << endl;
                std::cout << "Y: " << totalOffsetY << ", " << perStepOffsetY << ", " << lStepsY << ", " << fy << endl;
                std::cout << "Z: " << totalOffsetZ << ", " << perStepOffsetZ << ", " << lStepsZ << ", " << fz << endl;
              }
            #endif
        }

    //////////////////////////////////////////////////////////////////////////
    inline unsigned long getTimeStamp() {
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
        , configuredFeedSpeed(0.0)
        , currentDistance(0.0)
        , maxFeedSpeedX(0.0), maxFeedSpeedY(0.0), maxFeedSpeedZ(0.0)
        , gearingX(0.0), gearingY(0.0), gearingZ(0.0)
        , constOffset(0)
        , tPulseOffsetX(0), tPulseOffsetY(0), tPulseOffsetZ(0)
        , maxStepsX(0), maxStepsY(0), maxStepsZ(0)
        , totalOffsetX(0), totalOffsetY(0), totalOffsetZ(0)
        , perStepOffsetX(0), perStepOffsetY(0), perStepOffsetZ(0)
        , lStepsX(0), lStepsY(0), lStepsZ(0)
        {
        }
        
        //////////////////////////////////////////////////////////////////////////
        CncSpeedManager(unsigned int cOffset,
                        double pitchX, unsigned int stepsX, unsigned int pulseOffsetX,
                        double pitchY, unsigned int stepsY, unsigned int pulseOffsetY,
                        double pitchZ, unsigned int stepsZ, unsigned int pulseOffsetZ) 
        : measurement()
        , initialized(false)
        , configuredFeedSpeed(0.0)
        , currentDistance(0.0)
        , maxFeedSpeedX(0.0), maxFeedSpeedY(0.0), maxFeedSpeedZ(0.0)
        , gearingX(0.0), gearingY(0.0), gearingZ(0.0)
        , constOffset(cOffset)
        , tPulseOffsetX(0), tPulseOffsetY(0), tPulseOffsetZ(0)
        , maxStepsX(0), maxStepsY(0), maxStepsZ(0)
        , totalOffsetX(0), totalOffsetY(0), totalOffsetZ(0)
        , perStepOffsetX(0), perStepOffsetY(0), perStepOffsetZ(0)
        , lStepsX(0), lStepsY(0), lStepsZ(0)
        {
            setup(  cOffset,
                    pitchX, stepsX, pulseOffsetX,
                    pitchY, stepsY, pulseOffsetY,
                    pitchZ, stepsZ, pulseOffsetZ);
        }
        
        //////////////////////////////////////////////////////////////////////////
        virtual ~CncSpeedManager() {
        }
        
        //////////////////////////////////////////////////////////////////////////
        void setup(unsigned int cOffset,
                    double pitchX, unsigned int stepsX, unsigned int pulseOffsetX,
                    double pitchY, unsigned int stepsY, unsigned int pulseOffsetY,
                    double pitchZ, unsigned int stepsZ, unsigned int pulseOffsetZ) {
                    
            initialized = false;
                        
            if ( stepsX == 0 || stepsY == 0 || stepsZ == 0 )
                return;
            
            if ( dblCompareNull(pitchX) || dblCompareNull(pitchY) | dblCompareNull(pitchZ) )
                return;
            
            if ( pulseOffsetX == 0 || pulseOffsetY == 0 || pulseOffsetZ == 0 )
                return;
            
            // preconfig/setup values
            gearingX = (double)(pitchX / stepsX); // [mm/steps]
            gearingY = (double)(pitchY / stepsY); // [mm/steps]
            gearingZ = (double)(pitchZ / stepsZ); // [mm/steps]
            
            tPulseOffsetX = pulseOffsetX + cOffset; // [us]
            tPulseOffsetY = pulseOffsetY + cOffset; // [us]
            tPulseOffsetZ = pulseOffsetZ + cOffset; // [us]
            
            maxStepsX = (unsigned int)(1.0 / ( tPulseOffsetX / 1000.0 / 1000.0 )); // [steps/s]
            maxStepsY = (unsigned int)(1.0 / ( tPulseOffsetY / 1000.0 / 1000.0 )); // [steps/s]
            maxStepsZ = (unsigned int)(1.0 / ( tPulseOffsetZ / 1000.0 / 1000.0 )); // [steps/s]
            
            maxFeedSpeedX = gearingX * maxStepsX; // [mm/s]
            maxFeedSpeedY = gearingY * maxStepsY; // [mm/s]
            maxFeedSpeedZ = gearingZ * maxStepsZ; // [mm/s]
            
            initialized = true;
        }
        
        //////////////////////////////////////////////////////////////////////////
        bool isInitialized() { return initialized; }
        
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
            measurement.reset();
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
        virtual void initMove() {
        }
    
        //////////////////////////////////////////////////////////////////////////
        void finalizeMove() {
          // give inherited classes a change to do something
          completeMove();
    
          // log end point
          if ( measurement.tsStart != 0L)
            measurement.tsEnd = getTimeStamp();
    
          // calculate current feed speed - consider time stamp overflow
          // for more details please see micros()
          if ( measurement.tsEnd > measurement.tsStart ) {

        	measurement.timeElapsed  = measurement.tsEnd
            						 - measurement.tsStart;							// [us]

            measurement.feedSpeed    = ((1000L * 1000L * measurement.distance)
            		                 / measurement.timeElapsed)
            		                 * 60;											// [mm/min]

          } else {
            
            measurement.feedSpeed    = configuredFeedSpeed
            						 * 60;											// [mm/min]
          }
        }
            
        //////////////////////////////////////////////////////////////////////////
        virtual void completeMove() {}

        //////////////////////////////////////////////////////////////////////////
        unsigned long getMeasurementTimeElapsed() 	const { return measurement.timeElapsed; }
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
        double getCurrentDistance()         		const { return currentDistance; }
        
        //////////////////////////////////////////////////////////////////////////
        unsigned int getConstOffset()       		const { return constOffset; }
        
        unsigned int getTotalPulseOffsetX() 		const { return tPulseOffsetX; }
        unsigned int getTotalPulseOffsetY() 		const { return tPulseOffsetY; }
        unsigned int getTotalPulseOffsetZ() 		const { return tPulseOffsetZ; }
        
        unsigned int getLowPulseWidthX()    		const { return (tPulseOffsetX - constOffset) / 2; }
        unsigned int getLowPulseWidthY()    		const { return (tPulseOffsetY - constOffset) / 2; }
        unsigned int getLowPulseWidthZ()    		const { return (tPulseOffsetZ - constOffset) / 2; }
        
        unsigned int getHighPulseWidthX()   		const { return (tPulseOffsetX - constOffset) / 2; }
        unsigned int getHighPulseWidthY()   		const { return (tPulseOffsetY - constOffset) / 2; }
        unsigned int getHighPulseWidthZ()   		const { return (tPulseOffsetZ - constOffset) / 2; }
    
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
    
};

#endif

