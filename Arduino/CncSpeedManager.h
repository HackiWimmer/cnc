#ifndef CNC_SPEED_MANAGER
#define CNC_SPEED_MANAGER

#ifndef SKETCH_COMPILE
	#include <iostream>
	#include "CncCommon.h"
	using namespace cnc;
#endif

//////////////////////////////////////////////////////////////////////////////////
class CncSpeedManager {
	
	protected:
		
		double configedFeedSpeed;
		double currentDistance;
		
		double maxFeedSpeedX, maxFeedSpeedY, maxFeedSpeedZ;
		double gearingX, gearingY, gearingZ;
		
		int currentCorrectionValue;
		unsigned int constOffset;
		
		unsigned int tPulseOffsetX, tPulseOffsetY, tPulseOffsetZ;
		unsigned int maxStepsX, maxStepsY, maxStepsZ;
		unsigned int offsetX, offsetY, offsetZ;
		
		int32_t lStepsX, lStepsY, lStepsZ;
		
		//////////////////////////////////////////////////////////////////////////
		void reset() {
			lStepsX = lStepsY = lStepsZ = 0;
			offsetX = offsetY = offsetZ = 0;
		}
		
		//////////////////////////////////////////////////////////////////////////
		void calculate() {
			if ( dblCompareNull(configedFeedSpeed) == true ) {
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
			// offsetX           [us] = fx * restDuration;
			// offsetY           [us] = fy * restDuration;
			// offsetZ           [us] = fz * restDuration;
			
			if ( dblCompareNull(configedFeedSpeed) == true ) { offsetX = offsetY = offsetZ = 0; return; }
			
			long tSteps = lStepsX + lStepsY + lStepsZ;
			if ( tSteps == 0 ) { offsetX = offsetY = offsetZ = 0; return; }
			
			double fx = (double)(lStepsX) / tSteps;
			double fy = (double)(lStepsY) / tSteps;
			double fz = (double)(lStepsZ) / tSteps;
			
			double distanceX = lStepsX * gearingX;
			double distanceY = lStepsY * gearingY;
			double distanceZ = lStepsZ * gearingZ;
			
			currentDistance           = sqrt(distanceX * distanceX + distanceY * distanceY + distanceZ * distanceZ);
			double totalTime          = (currentDistance * 1000.0 * 1000.0) / configedFeedSpeed;
			double totalLoopDuration  = lStepsX * tPulseOffsetX + lStepsY * tPulseOffsetY + lStepsZ * tPulseOffsetZ;
			double restDuration       = totalTime - totalLoopDuration;
			
			if ( restDuration < 0 ) { offsetX = offsetY = offsetZ = 0; return; }
			
			// consider a processing overhead
			const unsigned int processingCorrectionValue = 10;
			offsetX                   = (fx * restDuration) / lStepsX - processingCorrectionValue;
			offsetY                   = (fy * restDuration) / lStepsY - processingCorrectionValue;
			offsetZ                   = (fz * restDuration) / lStepsZ - processingCorrectionValue;
			
		}
		
	public:
		
		//////////////////////////////////////////////////////////////////////////
		CncSpeedManager(unsigned int cOffset,
						double pitchX, unsigned int stepsX, unsigned int pulseOffsetX,
						double pitchY, unsigned int stepsY, unsigned int pulseOffsetY,
						double pitchZ, unsigned int stepsZ, unsigned int pulseOffsetZ) 
		: configedFeedSpeed(0.0)
		, currentDistance(0.0)
		, maxFeedSpeedX(0.0), maxFeedSpeedY(0.0), maxFeedSpeedZ(0.0)
		, gearingX(0.0), gearingY(0.0), gearingZ(0.0)
		, currentCorrectionValue(0)
		, constOffset(cOffset)
		, tPulseOffsetX(0), tPulseOffsetY(0), tPulseOffsetZ(0)
		, maxStepsX(0), maxStepsY(0), maxStepsZ(0)
		, offsetX(0), offsetY(0), offsetZ(0)
		, lStepsX(0), lStepsY(0), lStepsZ(0)
		{
			setup(	cOffset,
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
		}

		//////////////////////////////////////////////////////////////////////////
		// input mm/min
		void setFeedSpeed(double fm) {
			if ( fm > 0.0 )	configedFeedSpeed = fm / 60.0;
			else 			configedFeedSpeed = 0.0;
				
			reset();
		}
				
		//////////////////////////////////////////////////////////////////////////
		// input are steps
		void setNextMove(int32_t dx, int32_t dy, int32_t dz) {
			lStepsX = absolute<int32_t>(dx);
			lStepsY = absolute<int32_t>(dy);
			lStepsZ = absolute<int32_t>(dz);
			
			// calculate the variable getters - below
			calculate();
		}
		
		//////////////////////////////////////////////////////////////////////////
		// inut is mm/min
		void adjustOffset(double currentFeedSpeed) {
			static const double threshold = 5.0;
			
			currentFeedSpeed /= 60;
			
			double diff = configedFeedSpeed - currentFeedSpeed;
			
			if ( diff < 0 ) { if ( absolute<double>(diff) > threshold ) currentCorrectionValue += threshold; } 
			else			{ if ( absolute<double>(diff) > threshold ) currentCorrectionValue -= threshold; }
		}
		
		double getConfiguredFeedSpeed()     { return configedFeedSpeed; }
		
		//////////////////////////////////////////////////////////////////////////
		unsigned int getOffsetX()           { return offsetX += currentCorrectionValue; }
		unsigned int getOffsetY()           { return offsetY += currentCorrectionValue; }
		unsigned int getOffsetZ()           { return offsetZ += currentCorrectionValue; }
		
		//////////////////////////////////////////////////////////////////////////
		double getCurrentDistance()         { return  currentDistance; }
		
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
