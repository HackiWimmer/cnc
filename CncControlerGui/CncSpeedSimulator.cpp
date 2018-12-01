#include "CncSpeedSimulator.h"

//////////////////////////////////////////////////////////////////////////
CncSpeedSimulator::TimeStampInfo::TimeStampInfo() {
//////////////////////////////////////////////////////////////////////////
	reset();
}
//////////////////////////////////////////////////////////////////////////
CncSpeedSimulator::TimeStampInfo::~TimeStampInfo() {
//////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::TimeStampInfo::reset() {
//////////////////////////////////////////////////////////////////////////
	force                    = false;
	waitPeriod               = 0LL;
	beforeWait               = 0LL;
	afterWait                = 0LL;

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
unsigned int CncSpeedSimulator::TimeStampInfo::getColumnCount() {
//////////////////////////////////////////////////////////////////////////
	std::stringstream ss;
	headline(ss);
	wxString s(ss.str().c_str());
	
	if ( s.length() == 0 )
		return 0;
	
	unsigned int ret = 1;
	for (unsigned int i=0; i<s.length(); i++ ) {
		if ( (const char)s[i] == delim )
			ret++;
	}
	
	return ret;
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::TimeStampInfo::headline(std::ostream& out) {
//////////////////////////////////////////////////////////////////////////
	out 
	<< "force"
	<< delim
	<< "waitPeriod"
	<< delim
	<< "beforeWait"
	<< delim
	<< "afterWait"
	<< delim
	<< "diff"
	<< delim
	<< "diff-period"
	<< delim
	<< "totalAccumulatedOffset"
	<< delim
	<< "currentAccumulatedOffset"
	<< delim
	<< "totalAccumulatedOffsetX" 
	<< delim
	<< "totalAccumulatedOffsetY" 
	<< delim
	<< "totalAccumulatedOffsetZ"
	<< delim
	<< "stepCounterX" 
	<< delim
	<< "stepCounterY" 
	<< delim
	<< "stepCounterZ"
	<< std::endl;
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::TimeStampInfo::trace(std::ostream& out) {
//////////////////////////////////////////////////////////////////////////
	out
	<< force
	<< delim
	<< waitPeriod
	<< delim
	<< beforeWait
	<< delim
	<< afterWait
	<< delim
	<< (afterWait - beforeWait) / 1000
	<< delim
	<< ((afterWait - beforeWait) / 1000) - waitPeriod
	<< delim
	<< totalAccumulatedOffset 
	<< delim
	<< currentAccumulatedOffset 
	<< delim
	<< totalAccumulatedOffsetX 
	<< delim 
	<< totalAccumulatedOffsetY 
	<< delim 
	<< totalAccumulatedOffsetZ
	<< delim
	<< stepCounterX
	<< delim
	<< stepCounterY 
	<< delim
	<< stepCounterZ
	<< std::endl;
}
//////////////////////////////////////////////////////////////////////////
CncSpeedSimulator::CncSpeedSimulator(
 unsigned int cStepStaticOffset, unsigned int cStepLoopOffset,
 double pitchX, unsigned int stepsX, unsigned int pulseOffsetX,
 double pitchY, unsigned int stepsY, unsigned int pulseOffsetY,
 double pitchZ, unsigned int stepsZ, unsigned int pulseOffsetZ)
: CncSpeedController() 
, traceFlag(false)
, traceInfo()
, curTsInfo()
, totalAccumulatedOffset(0LL)
, totalAccumulatedOffsetX(0LL)
, totalAccumulatedOffsetY(0LL)
, totalAccumulatedOffsetZ(0LL)
, currentAccumulatedOffset(0LL)
, stepCounterX(0L)
, stepCounterY(0L)
, stepCounterZ(0L)
, tsAfterLastWait(0LL)
//////////////////////////////////////////////////////////////////////////
{
	traceInfo.reserve(1024 * 1024);
}
//////////////////////////////////////////////////////////////////////////
CncSpeedSimulator::~CncSpeedSimulator()  {
//////////////////////////////////////////////////////////////////////////
	traceInfo.clear();
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::reset() {
//////////////////////////////////////////////////////////////////////////
	// totalAccumulatedOffsetstay still grows over 
	// the whole life time of this object
	
	totalAccumulatedOffsetX  = 0LL;
	totalAccumulatedOffsetY  = 0LL;
	totalAccumulatedOffsetZ  = 0LL;
	currentAccumulatedOffset = 0LL;
	
	stepCounterX             = 0L;
	stepCounterY             = 0L;
	stepCounterZ             = 0L;
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::simulateSteppingX(unsigned int dx)  { 
//////////////////////////////////////////////////////////////////////////
	stepCounterX += dx;
	uint64_t v = dx * (X.synthSpeedDelay + X.totalOffset); 
	totalAccumulatedOffsetX  += v;
	currentAccumulatedOffset += v; 
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::simulateSteppingY(unsigned int dy)  { 
//////////////////////////////////////////////////////////////////////////
	stepCounterY += dy;
	uint64_t v = dy * (Y.synthSpeedDelay + Y.totalOffset); 
	totalAccumulatedOffsetY  += v; 
	currentAccumulatedOffset += v; 
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::simulateSteppingZ(unsigned int dz)  { 
//////////////////////////////////////////////////////////////////////////
	stepCounterZ += dz;
	uint64_t v = dz * (Z.synthSpeedDelay + Z.totalOffset); 
	totalAccumulatedOffsetZ  += v; 
	currentAccumulatedOffset += v; 
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::performCurrentOffset(bool force) {
//////////////////////////////////////////////////////////////////////////
	long threshold = 20000L;
	if ( force == true )
		threshold = 0L;
	
	// check if further accumulation is necessary
	if ( currentAccumulatedOffset < threshold ) 
		return;

	// calc time elapsed since the last wait
	CncNanoTimespan elapsedTime = 0LL;
	if ( tsAfterLastWait > 0LL )
		elapsedTime = (CncTimeFunctions::getNanoTimestamp() - tsAfterLastWait) / 1000;
	
	// reduce the accumulated offset by the time which is already gone
	currentAccumulatedOffset -= elapsedTime;
	tsAfterLastWait = CncTimeFunctions::getNanoTimestamp();

	// check if further accumulation is necessary
	if ( currentAccumulatedOffset < threshold )
		return;
	
	// sleep
	CncNanoTimestamp tsBeforeWait = CncTimeFunctions::getNanoTimestamp();
	CncTimeFunctions::sleepMircoseconds(currentAccumulatedOffset);
	CncNanoTimestamp tsAfterWait = CncTimeFunctions::getNanoTimestamp();
	
	// accumulate
	totalAccumulatedOffset += currentAccumulatedOffset;
	
	// trace
	if ( traceFlag == true ) {
		curTsInfo.force                    = force;
		curTsInfo.waitPeriod               = currentAccumulatedOffset;
		curTsInfo.beforeWait               = tsBeforeWait;
		curTsInfo.afterWait                = tsAfterWait;
		
		curTsInfo.totalAccumulatedOffset   = totalAccumulatedOffset;
		curTsInfo.currentAccumulatedOffset = currentAccumulatedOffset; 
		curTsInfo.stepCounterX             = stepCounterX;
		curTsInfo.totalAccumulatedOffsetX  = totalAccumulatedOffsetX;
		curTsInfo.stepCounterY             = stepCounterY;
		curTsInfo.totalAccumulatedOffsetY  = totalAccumulatedOffsetY;
		curTsInfo.stepCounterZ             = stepCounterZ;
		curTsInfo.totalAccumulatedOffsetZ  = totalAccumulatedOffsetZ;
	}
	
	// reset the current offset - but consider the evaluated sleep deviation
	currentAccumulatedOffset = (tsAfterWait - tsBeforeWait) / 1000;
	
	// trace
	if ( traceFlag == true )
		registerNextTsInfo();
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::registerNextTsInfo() {
//////////////////////////////////////////////////////////////////////////
	traceInfo.push_back(curTsInfo);
	curTsInfo.reset();
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::trace(std::ostream& out) {
//////////////////////////////////////////////////////////////////////////
	CncSpeedSimulator::TimeStampInfo::headline(out);
	for ( auto it = traceInfo.begin(); it != traceInfo.end(); ++it ) {
		it->trace(out);
	}
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::initMove(int32_t dx, int32_t dy, int32_t dz) {
//////////////////////////////////////////////////////////////////////////
	// currently noting to do
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::completeMove() {
//////////////////////////////////////////////////////////////////////////
	reset();
}
