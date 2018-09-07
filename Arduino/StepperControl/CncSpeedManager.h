#ifndef CNC_SPEED_MANAGER
#define CNC_SPEED_MANAGER

#ifndef SKETCH_COMPILE

    #include <iostream>
    #include "CncCommon.h"
    #include "OSD/CncTimeFunctions.h"
    using namespace cnc;

#else

    #include "CommonFunctions.h"
    
#endif

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
  
        Measurement() { reset(); }
        ~Measurement() {}
  
        void reset() {
          tsStart     = 0L;
          tsEnd       = 0L;
          
          timeElapsed = 0L;
          distance    = 0.0;
          feedSpeed   = MIN_LONG;
        }
      };
      
      //////////////////////////////////////////////////////////////////////////
        Measurement measurement; 
   
        bool initialized;

        double configedFeedSpeed;
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
            if ( dblCompareNull(configedFeedSpeed) == true ) {
                reset();
                return;
            }
            
            // Quality check
            if ( initialized == false ) {
                reset();
                return;
            }
            
            // Formulas:
            // totalTime [us]     = lstepsX * tPulseOffsetX + lstepsY * tPulseOffsetY + lstepsZ * tPulseOffsetZ +
            //                      lstepsX * tX            + lstepsY * tY            + lstepsZ * tZ;
            //
            // -->                + lstepsX * (tPulseOffsetX + tX)
            //                    + lstepsY * (tPulseOffsetY + tY)
            //                    + lstepsZ * (tPulseOffsetZ + tZ)
            //
            // distanceX         [mm] = lStepsX * gearingX;
            // distanceY         [mm] = lStepsY * gearingY;
            // distanceZ         [mm] = lStepsZ * gearingZ;
            //
            // currentDistance   [mm] = sqrt(distanceX * distanceX + distanceY * distanceY + distanceZ * distanceZ )
            // totalTime         [us] = (currentDistance * 1000 * 1000) / configedFeedSpeed);
            // totalLoopDuration [us] = lStepsX * tPulseOffsetX + lStepsY * tPulseOffsetY + lStepsZ * tPulseOffsetZ;
            // restDuration      [us] = totalTime - totalLoopDuration;
            //
            // tSteps             [#] = lStepsX + lStepsY + lStepsX;
            // fx                 [%] = lStepsX/tSteps;
            // fy                 [%] = lStepsY/tSteps;
            // fz                 [%] = lStepsZ/tSteps;
            //
            // toptalOffsetX     [us] = fx * restDuration;
            // toptalOffsetY     [us] = fy * restDuration;
            // toptalOffsetZ     [us] = fz * restDuration;
            //
            // perStepOffsetX    [us] = toptalOffsetX / lstepsX;
            // perStepOffsetY    [us] = toptalOffsetY / lstepsY;
            // perStepOffsetZ    [us] = toptalOffsetZ / lstepsZ;

        if ( dblCompareNull(configedFeedSpeed) == true ) { totalOffsetX = totalOffsetY = totalOffsetZ = 0; return; }
            
            long tSteps = lStepsX + lStepsY + lStepsZ;
            if ( tSteps == 0 ) { resetOffset(); return; }
            
            double fx = (double)(lStepsX) / tSteps;
            double fy = (double)(lStepsY) / tSteps;
            double fz = (double)(lStepsZ) / tSteps;
            
            double distanceX = lStepsX * gearingX;
            double distanceY = lStepsY * gearingY;
            double distanceZ = lStepsZ * gearingZ;
            
            currentDistance           = sqrt(distanceX * distanceX + distanceY * distanceY + distanceZ * distanceZ);
            double totalTime          = (currentDistance / configedFeedSpeed) * 1000.0 * 1000.0;
            double totalLoopDuration  = lStepsX * tPulseOffsetX + lStepsY * tPulseOffsetY + lStepsZ * tPulseOffsetZ;
            double restDuration       = totalTime - totalLoopDuration;

            measurement.distance     += currentDistance;

            if ( restDuration <= 0.0 ) { resetOffset(); return; }
            
            totalOffsetX              = ( lStepsX > 0 ? (fx * restDuration) : 0 );
            totalOffsetY              = ( lStepsY > 0 ? (fy * restDuration) : 0 );
            totalOffsetZ              = ( lStepsZ > 0 ? (fz * restDuration) : 0 );
            
            perStepOffsetX            = ( lStepsX > 0 ? totalOffsetX / lStepsX : 0 );
            perStepOffsetY            = ( lStepsY > 0 ? totalOffsetY / lStepsY : 0 );
            perStepOffsetZ            = ( lStepsZ > 0 ? totalOffsetZ / lStepsZ : 0 );
            
            #ifndef SKETCH_COMPILE
              if ( false ) {
                std::cout << "T: " << tSteps << ", " << restDuration << ", " << currentDistance << ", " <<  totalLoopDuration << ", " << totalTime << ", " << configedFeedSpeed << endl;
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
        , configedFeedSpeed(0.0)
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
        , configedFeedSpeed(0.0)
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
            gearingX = (double)(pitchX / stepsX);
            gearingY = (double)(pitchY / stepsY);
            gearingZ = (double)(pitchZ / stepsZ);
            
            tPulseOffsetX = pulseOffsetX + cOffset;
            tPulseOffsetY = pulseOffsetY + cOffset;
            tPulseOffsetZ = pulseOffsetZ + cOffset;
            
            maxStepsX = (unsigned int)(1.0 / ( tPulseOffsetX / 1000.0 / 1000.0 ));
            maxStepsY = (unsigned int)(1.0 / ( tPulseOffsetY / 1000.0 / 1000.0 ));
            maxStepsZ = (unsigned int)(1.0 / ( tPulseOffsetZ / 1000.0 / 1000.0 ));
            
            maxFeedSpeedX = gearingX * maxStepsX;
            maxFeedSpeedY = gearingY * maxStepsY;
            maxFeedSpeedZ = gearingZ * maxStepsZ;
            
            initialized = true;
        }
        
        //////////////////////////////////////////////////////////////////////////
        bool isInitialized() { return initialized; }
        
        //////////////////////////////////////////////////////////////////////////
        // input mm/min
        void setFeedSpeed(double fm) {
            // do nothing if the value isn't changed 
            if (  dblCompare(fm, configedFeedSpeed) == true )
                return;

            // set
            if ( fm > 0.0 ) configedFeedSpeed = fm / 60;
            else            configedFeedSpeed = 0.0;

            reset();
            measurement.reset();
        }
                
        //////////////////////////////////////////////////////////////////////////
        // input are steps
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
        virtual void initMove() {}
    
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
            // Imput values:
            //    currentDistance  = (alreay calculated)    [mm]
            //    timeElapsed      = tsEnd - tsStart        [us] 
           
            measurement.timeElapsed  = measurement.tsEnd - measurement.tsStart;
            measurement.feedSpeed    = ((1000L * 1000L * measurement.distance) / measurement.timeElapsed) * 60;
          } else {
            
            measurement.feedSpeed = configedFeedSpeed;
          }
        }
            
        //////////////////////////////////////////////////////////////////////////
        virtual void completeMove() {}

        //////////////////////////////////////////////////////////////////////////
        unsigned long getMeasurementTimeElapsed() { return measurement.timeElapsed; }
        double getMeasurementDistance()           { return measurement.distance; }
        double getMeasurementFeedSpeed()          { return measurement.feedSpeed; }
        
        //////////////////////////////////////////////////////////////////////////
        double getConfiguredFeedSpeed()     const { return configedFeedSpeed; }
        
        //////////////////////////////////////////////////////////////////////////
        unsigned int getTotalOffsetX()      const { return totalOffsetX; }
        unsigned int getTotalOffsetY()      const { return totalOffsetY; }
        unsigned int getTotalOffsetZ()      const { return totalOffsetZ; }
    
        unsigned int getOffsetPerStepX()    const { return perStepOffsetX; }
        unsigned int getOffsetPerStepY()    const { return perStepOffsetY; }
        unsigned int getOffsetPerStepZ()    const { return perStepOffsetZ; }
        
        //////////////////////////////////////////////////////////////////////////
        double getCurrentDistance()         const { return currentDistance; }
        
        //////////////////////////////////////////////////////////////////////////
        unsigned int getConstOffset()       const { return constOffset; }
        
        unsigned int getTotalPulseOffsetX() const { return tPulseOffsetX; }
        unsigned int getTotalPulseOffsetY() const { return tPulseOffsetY; }
        unsigned int getTotalPulseOffsetZ() const { return tPulseOffsetZ; }
        
        unsigned int getLowPulseWidthX()    const { return (tPulseOffsetX - constOffset) / 2; }
        unsigned int getLowPulseWidthY()    const { return (tPulseOffsetX - constOffset) / 2; }
        unsigned int getLowPulseWidthZ()    const { return (tPulseOffsetX - constOffset) / 2; }
        
        unsigned int getHighPulseWidthX()   const { return (tPulseOffsetX - constOffset) / 2; }
        unsigned int getHighPulseWidthY()   const { return (tPulseOffsetX - constOffset) / 2; }
        unsigned int getHighPulseWidthZ()   const { return (tPulseOffsetX - constOffset) / 2; }
    
        //////////////////////////////////////////////////////////////////////////
        double getMaxSpeedX_MM_MIN()        const { return maxFeedSpeedX * 60; }
        double getMaxSpeedY_MM_MIN()        const { return maxFeedSpeedY * 60; }
        double getMaxSpeedZ_MM_MIN()        const { return maxFeedSpeedZ * 60; }
    
        double getMaxSpeedY_MM_SEC()        const { return maxFeedSpeedY; }
        double getMaxSpeedX_MM_SEC()        const { return maxFeedSpeedX; }
        double getMaxSpeedT_MM_SEC()        const { return maxFeedSpeedZ; }
        
        //////////////////////////////////////////////////////////////////////////
        unsigned int getMaxStepsX()         const { return maxStepsX; }
        unsigned int getMaxStepsY()         const { return maxStepsY; }
        unsigned int getMaxStepsZ()         const { return maxStepsZ; }
    
};

#endif

