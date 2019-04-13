#ifndef CNC_SPEED_SIMULATOR
#define CNC_SPEED_SIMULATOR

#include <iostream>
#include "OSD/CncTimeFunctions.h"
#include "CncCommon.h"
#include "CncSpeedController.h"

class CncSpeedSimulator : public CncSpeedController {
	
	public:
		
		//////////////////////////////////////////////////////////////////////////
		CncSpeedSimulator(unsigned int cStepStaticOffset, unsigned int cStepLoopOffset,
						  double pitchX, unsigned int stepsX, unsigned int pulseOffsetX,
						  double pitchY, unsigned int stepsY, unsigned int pulseOffsetY,
						  double pitchZ, unsigned int stepsZ, unsigned int pulseOffsetZ);
						  
		virtual ~CncSpeedSimulator();
		
		void simulateOneStepX();
		void simulateOneStepY();
		void simulateOneStepZ();
		
		void performCurrentOffset(bool force=false);
		
		int64_t getTotalAccumulatedOffsetX() { return totalAccumulatedOffsetX; }
		int64_t getTotalAccumulatedOffsetY() { return totalAccumulatedOffsetY; }
		int64_t getTotalAccumulatedOffsetZ() { return totalAccumulatedOffsetZ; }
		
		long getStepCounterX() { return stepCounterX; }
		long getStepCounterY() { return stepCounterY; }
		long getStepCounterZ() { return stepCounterZ; }
		
		virtual void initMove(int32_t dx, int32_t dy, int32_t dz);
		virtual void completeMove();
		
	private:
		
		bool traceFlag;
		
		int64_t totalAccumulatedOffsetX;
		int64_t totalAccumulatedOffsetY;
		int64_t totalAccumulatedOffsetZ;
		int64_t currentAccumulatedOffset;
		
		long stepCounterX;
		long stepCounterY;
		long stepCounterZ;
		
		CncNanoTimestamp tsLastPerform;
		
		void reset();
};

#endif