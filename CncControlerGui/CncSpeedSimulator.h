#ifndef CNC_SPEED_SIMULATOR
#define CNC_SPEED_SIMULATOR

#include <iostream>
#include "CncTimeFunctions.h"
#include "CncArduino.h"

class CncSpeedSimulator : public CncSpeedManager {
	
	private:
		int64_t totalAccumulatedOffset;
		int64_t totalAccumulatedOffsetX;
		int64_t totalAccumulatedOffsetY;
		int64_t totalAccumulatedOffsetZ;
		int64_t currentAccumulatedOffset;
		
		long stepCounterX;
		long stepCounterY;
		long stepCounterZ;
		
	public:
		
		//////////////////////////////////////////////////////////////////////////
		CncSpeedSimulator(unsigned int cOffset,
						  double pitchX, unsigned int stepsX, unsigned int pulseOffsetX,
						  double pitchY, unsigned int stepsY, unsigned int pulseOffsetY,
						  double pitchZ, unsigned int stepsZ, unsigned int pulseOffsetZ)
		: CncSpeedManager(cOffset,
						  pitchX, stepsX, pulseOffsetX,
						  pitchY, stepsY, pulseOffsetY,
						  pitchZ, stepsZ, pulseOffsetZ) 
		, totalAccumulatedOffset(0LL)
		, totalAccumulatedOffsetX(0LL)
		, totalAccumulatedOffsetY(0LL)
		, totalAccumulatedOffsetZ(0LL)
		, currentAccumulatedOffset(0LL)
		, stepCounterX(0L)
		, stepCounterY(0L)
		, stepCounterZ(0L)
		{
		}
		
		//////////////////////////////////////////////////////////////////////////
		virtual ~CncSpeedSimulator()  {
		}
		
		//////////////////////////////////////////////////////////////////////////
		void reset() {
			totalAccumulatedOffset   = 0LL;
			totalAccumulatedOffsetX  = 0LL;
			totalAccumulatedOffsetY  = 0LL;
			totalAccumulatedOffsetZ  = 0LL;
			currentAccumulatedOffset = 0LL;
			stepCounterX             = 0L;
			stepCounterY             = 0L;
			stepCounterZ             = 0L;
		}
		
		//////////////////////////////////////////////////////////////////////////
		void simulateSteppingX(unsigned int dx)  { stepCounterX += lStepsX; uint64_t v = dx * (getOffsetPerStepX() + getTotalPulseOffsetX()); currentAccumulatedOffset += v; totalAccumulatedOffsetX += v; }
		void simulateSteppingY(unsigned int dy)  { stepCounterY += lStepsY; uint64_t v = dy * (getOffsetPerStepY() + getTotalPulseOffsetY()); currentAccumulatedOffset += v; totalAccumulatedOffsetY += v; }
		void simulateSteppingZ(unsigned int dz)  { stepCounterZ += lStepsZ; uint64_t v = dz * (getOffsetPerStepZ() + getTotalPulseOffsetZ()); currentAccumulatedOffset += v; totalAccumulatedOffsetZ += v; }
		
		//////////////////////////////////////////////////////////////////////////
		void performCurrentOffset(bool force=false) {
			// consider the windows standard timer resolution of 15,625 ms
			if ( currentAccumulatedOffset >= 16000LL || force == true ) {
				CncTimeFunctions::activeWaitMircoseconds(currentAccumulatedOffset);
				
				totalAccumulatedOffset   += currentAccumulatedOffset;
				currentAccumulatedOffset  = 0LL;
			}
		}
		
		//////////////////////////////////////////////////////////////////////////
		int64_t getTotalAccumulatedOffset()  { return totalAccumulatedOffset; }
		int64_t getTotalAccumulatedOffsetX() { return totalAccumulatedOffsetX; }
		int64_t getTotalAccumulatedOffsetY() { return totalAccumulatedOffsetY; }
		int64_t getTotalAccumulatedOffsetZ() { return totalAccumulatedOffsetZ; }
		
		//////////////////////////////////////////////////////////////////////////
		long getStepCounterX() { return stepCounterX; }
		long getStepCounterY() { return stepCounterY; }
		long getStepCounterZ() { return stepCounterZ; }
};


#endif