#include "CncSpeedSimulator.h"

//////////////////////////////////////////////////////////////////////////
CncSpeedSimulator::CncSpeedSimulator(
 unsigned int cStepStaticOffset, unsigned int cStepLoopOffset,
 double pitchX, unsigned int stepsX, unsigned int pulseOffsetX,
 double pitchY, unsigned int stepsY, unsigned int pulseOffsetY,
 double pitchZ, unsigned int stepsZ, unsigned int pulseOffsetZ)
: CncSpeedController() 
, traceFlag(false)
, totalAccumulatedOffsetX(0LL)
, totalAccumulatedOffsetY(0LL)
, totalAccumulatedOffsetZ(0LL)
, currentAccumulatedOffset(0LL)
, stepCounterX(0L)
, stepCounterY(0L)
, stepCounterZ(0L)
, tsLastPerform(0LL)
//////////////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////////////
CncSpeedSimulator::~CncSpeedSimulator()  {
//////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::reset() {
//////////////////////////////////////////////////////////////////////////
	totalAccumulatedOffsetX  = 0LL;
	totalAccumulatedOffsetY  = 0LL;
	totalAccumulatedOffsetZ  = 0LL;
	currentAccumulatedOffset = 0LL;
	
	tsLastPerform            = CncTimeFunctions::getNanoTimestamp();
	
	stepCounterX             = 0L;
	stepCounterY             = 0L;
	stepCounterZ             = 0L;
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::simulateOneStepX()  { 
//////////////////////////////////////////////////////////////////////////
	// .totalOffset              [usec] :
	// .synthSpeedDelay     [usec/step] :
	// getNextAccelDelayX() :
	
	const unsigned int accelDelay = getNextAccelDelayX();
	
	stepCounterX++;
	const uint64_t v = X.totalOffset + X.synthSpeedDelay + accelDelay; 
	totalAccumulatedOffsetX  += v;
	currentAccumulatedOffset += v; 
	
	#warning accel
	if ( accelDelay )
		;;//std::cout << accelDelay << std::endl;
		
		
		

}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::simulateOneStepY()  { 
//////////////////////////////////////////////////////////////////////////
	stepCounterY++;
	const uint64_t v = Y.totalOffset + Y.synthSpeedDelay + getNextAccelDelayY(); 
	totalAccumulatedOffsetY  += v; 
	currentAccumulatedOffset += v; 
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::simulateOneStepZ()  { 
//////////////////////////////////////////////////////////////////////////
	stepCounterZ++;
	const uint64_t v = Z.totalOffset + Z.synthSpeedDelay + getNextAccelDelayZ(); 
	totalAccumulatedOffsetZ  += v; 
	currentAccumulatedOffset += v; 
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::performCurrentOffset(bool force) {
//////////////////////////////////////////////////////////////////////////
	const CncNanoTimespan threshold = force == false ? CncTimeFunctions::minWaitPeriod / 1000 : 0LL;
	
	// check if further accumulation is necessary
	if ( currentAccumulatedOffset < threshold )
		return;

	// calc time elapsed since the last wait
	const CncNanoTimestamp now        = CncTimeFunctions::getNanoTimestamp();
	const CncNanoTimespan elapsedTime = tsLastPerform > 0LL ? now - tsLastPerform : 0LL;
	
	// reduce the accumulated offset by the time which is already gone
//std::cout << currentAccumulatedOffset << "," << elapsedTime / 1000 << std::endl;
	currentAccumulatedOffset -= ( elapsedTime / 1000 );
	
	if ( currentAccumulatedOffset < 0LL) {
		//tsLastPerform = now;
		//return;
		
		//currentAccumulatedOffset = 0LL;
	}
	
	//tsAfterLastWait = CncTimeFunctions::getNanoTimestamp();
	tsLastPerform = now;

	// check if further accumulation is necessary
	if ( currentAccumulatedOffset < threshold )
		return;
	
	// sleep
	//std::cout << currentAccumulatedOffset << std::endl;
	CncTimeFunctions::sleepMircoseconds(currentAccumulatedOffset);
	
	//currentAccumulatedOffset = 0LL;
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::initMove(int32_t dx, int32_t dy, int32_t dz) {
//////////////////////////////////////////////////////////////////////////
	CncSpeedController::initMove(dx, dy, dz);
	
	
	#warning accel
	//std::cout << X.AP << std::endl;
	
	
	
	enableAccelerationXYZ(false);
	
	
	
	reset();
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::completeMove() {
//////////////////////////////////////////////////////////////////////////
	// currently nothing to do
}
