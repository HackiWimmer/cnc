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
            else                  configedFeedSpeed = 0.0;

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
        double getConfiguredFeedSpeed()     { return configedFeedSpeed; }
        
        //////////////////////////////////////////////////////////////////////////
        unsigned int getTotalOffsetX()      { return totalOffsetX; }
        unsigned int getTotalOffsetY()      { return totalOffsetY; }
        unsigned int getTotalOffsetZ()      { return totalOffsetZ; }
    
        unsigned int getOffsetPerStepX()    { return perStepOffsetX; }
        unsigned int getOffsetPerStepY()    { return perStepOffsetY; }
        unsigned int getOffsetPerStepZ()    { return perStepOffsetZ; }
        
        //////////////////////////////////////////////////////////////////////////
        double getCurrentDistance()         { return currentDistance; }
        
        //////////////////////////////////////////////////////////////////////////
        unsigned int getConstOffset()       { return constOffset; }
        
        unsigned int getTotalPulseOffsetX() { return tPulseOffsetX; }
        unsigned int getTotalPulseOffsetY() { return tPulseOffsetY; }
        unsigned int getTotalPulseOffsetZ() { return tPulseOffsetZ; }
        
        unsigned int getLowPulseWidthX()    { return (tPulseOffsetX - constOffset) / 2; }
        unsigned int getLowPulseWidthY()    { return (tPulseOffsetX - constOffset) / 2; }
        unsigned int getLowPulseWidthZ()    { return (tPulseOffsetX - constOffset) / 2; }
        
        unsigned int getHighPulseWidthX()   { return (tPulseOffsetX - constOffset) / 2; }
        unsigned int getHighPulseWidthY()   { return (tPulseOffsetX - constOffset) / 2; }
        unsigned int getHighPulseWidthZ()   { return (tPulseOffsetX - constOffset) / 2; }
    
        //////////////////////////////////////////////////////////////////////////
        double getMaxSpeedX_MM_MIN()        { return maxFeedSpeedX * 60; }
        double getMaxSpeedY_MM_MIN()        { return maxFeedSpeedY * 60; }
        double getMaxSpeedZ_MM_MIN()        { return maxFeedSpeedZ * 60; }
    
        double getMaxSpeedY_MM_SEC()        { return maxFeedSpeedY; }
        double getMaxSpeedX_MM_SEC()        { return maxFeedSpeedX; }
        double getMaxSpeedT_MM_SEC()        { return maxFeedSpeedZ; }
        
        //////////////////////////////////////////////////////////////////////////
        unsigned int getMaxStepsX()         { return maxStepsX; }
        unsigned int getMaxStepsY()         { return maxStepsY; }
        unsigned int getMaxStepsZ()         { return maxStepsZ; }
    
};

#endif

