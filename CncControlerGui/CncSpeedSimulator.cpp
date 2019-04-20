#include <fstream>
#include <algorithm>
#include "CncFileNameService.h"
#include "CncSpeedSimulator.h"

//#define TRACE_SPEED_FLOW
#define CSE currentSleepEntry
#define STA statistics

/////////////////////////////////////////////////////////////
void DelayAccumulator::SleepFlowEntry::traceHeadline(std::ostream& o) {
/////////////////////////////////////////////////////////////
	const char* delim = ",";
	o << "tsNow" 				<< delim;
	o << "tsRef"				<< delim;
	o << "tsLast" 				<< delim;
	o << "tsTarget" 			<< delim;
	o << "elapsedMicros"		<< delim;
	o << "incrementMicros" 		<< delim;
	o << "accumulatedMicros" 	<< delim;
	o << "sleepIntervalMicros" 	<< delim;
	o << "stepCountX"			<< delim;
	o << "stepCountY"			<< delim;
	o << "stepCountZ"			<< delim;

	o << std::endl;

}
/////////////////////////////////////////////////////////////
void DelayAccumulator::SleepFlowEntry::trace(std::ostream& o) {
/////////////////////////////////////////////////////////////
	const char* delim = ",";
	o << tsNow 					<< delim;
	o << tsRef					<< delim;
	o << tsLast 				<< delim;
	o << tsTarget 				<< delim;
	o << elapsedMicros			<< delim;
	o << incrementMicros 		<< delim;
	o << accumulatedMicros 		<< delim;
	o << sleepIntervalMicros 	<< delim;
	o << stepCountX				<< delim;
	o << stepCountY				<< delim;
	o << stepCountZ				<< delim;

	o << std::endl;
}
/////////////////////////////////////////////////////////////
DelayAccumulator::DelayAccumulator(CncSpeedSimulator* s)
: simulator(s)
, STA()
, CSE()
, sleepStatistics()
/////////////////////////////////////////////////////////////
{
}
/////////////////////////////////////////////////////////////
DelayAccumulator::~DelayAccumulator() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
void DelayAccumulator::trace(std::ostream& o) {
/////////////////////////////////////////////////////////////
	STA.trace(std::cout);

	SleepFlowEntry::traceHeadline(o);
	for ( auto it = sleepStatistics.begin(); it != sleepStatistics.end(); ++it )
		it->trace(o);
}
/////////////////////////////////////////////////////////////
void DelayAccumulator::initialize() {
/////////////////////////////////////////////////////////////
	CSE.reset();
	CSE.accumulatedMicros	= 0LL;
	CSE.tsRef	 			= CncTimeFunctions::getNanoTimestamp();
	CSE.tsLast 				= CSE.tsRef;

	STA.reset();
}
/////////////////////////////////////////////////////////////
int64_t DelayAccumulator::perform(bool force) {
/////////////////////////////////////////////////////////////
	const int64_t ret = performB(force);

	#ifdef TRACE_SPEED_FLOW

		CSE.stepCountX	= simulator->X.AP.stepCounter;
		CSE.stepCountY	= simulator->Y.AP.stepCounter;
		CSE.stepCountZ	= simulator->Z.AP.stepCounter;

		sleepStatistics.push_back(CSE);

	#endif

	CSE.next();

	return ret;
}
/////////////////////////////////////////////////////////////
int64_t DelayAccumulator::performA(bool force) {
/////////////////////////////////////////////////////////////
	return 0;
}
/////////////////////////////////////////////////////////////
int64_t DelayAccumulator::performB(bool force) {
/////////////////////////////////////////////////////////////
	const CncNanoTimespan threshold = force == false ? DefaultThresholdMicros : 0LL;
	CSE.tsNow         				= CncTimeFunctions::getNanoTimestamp();
	CSE.elapsedMicros 				= CSE.tsRef > 0LL ? (CSE.tsNow - CSE.tsRef) / 1000 : 0LL;

	// check if further accumulation is necessary
	if ( CSE.accumulatedMicros < threshold )
		return 0;

	if ( CSE.elapsedMicros == 0LL ) {
		CSE.tsRef = CSE.tsNow;
		return 0;
	}

	// reduce the accumulated offset by the time which is already gone
	CSE.accumulatedMicros -= ( CSE.elapsedMicros );

	//tsRef = CncTimeFunctions::getNanoTimestamp();
	CSE.tsRef = CSE.tsNow;

	// check if further accumulation is necessary
	if ( CSE.accumulatedMicros < threshold )
		return 0;

	// sleep
	sleepMicros(CSE.accumulatedMicros);
	return CSE.accumulatedMicros;
}
/////////////////////////////////////////////////////////////
int64_t DelayAccumulator::performC(bool force) {
/////////////////////////////////////////////////////////////
	const CncNanoTimespan  thresholdMicros 	= force == false ? DefaultThresholdMicros : 0LL;
	CSE.tsNow           		= CncTimeFunctions::getNanoTimestamp();
	CSE.tsTarget	      		= CSE.tsRef  + CSE.accumulatedMicros * 1000;
	CSE.elapsedMicros   		= (CSE.tsNow - CSE.tsLast) / 1000;

	auto log = [&](int64_t ret) {
		CSE.tsLast = CncTimeFunctions::getNanoTimestamp();
		return ret;
	};

	if ( CSE.tsNow >= CSE.tsTarget ) {
		STA.targetSmaller++;

		// In this case the progress is to slow and
		// we have to catch up time
		CSE.accumulatedMicros = 0LL;
		return log(-1LL);
	}
	else {
		//
		STA.targetLarger++;

		const int64_t diffMicros = (CSE.tsTarget - CSE.tsNow) / 1000; //- elapsedMicros;

		if ( diffMicros >= thresholdMicros ) {
			CSE.accumulatedMicros = 0LL;
			return log(sleepMicros(diffMicros));
		}
	}

	return log(0LL);
}
/////////////////////////////////////////////////////////////
bool DelayAccumulator::incMicros(int64_t micros){
/////////////////////////////////////////////////////////////
	if ( CSE.accumulatedMicros == 0LL ) {
		CSE.tsRef  = CncTimeFunctions::getNanoTimestamp();
		CSE.tsLast = CSE.tsRef;
	}

	CSE.incrementMicros    = micros;
	CSE.accumulatedMicros += micros;

	return ( CSE.accumulatedMicros >= DefaultThresholdMicros );
}
/////////////////////////////////////////////////////////////
int64_t DelayAccumulator::sleepMicros(int64_t micros) {
/////////////////////////////////////////////////////////////
	if ( STA.minSleepInterval == 0 ) STA.minSleepInterval = micros;
	else							 STA.minSleepInterval = std::min(STA.minSleepInterval, micros);

	if ( STA.maxSleepInterval == 0 ) STA.maxSleepInterval = micros;
	else							 STA.maxSleepInterval = std::max(STA.maxSleepInterval, micros);

	STA.avgSleepInterval = ( statistics.avgSleepInterval + micros ) / 2;
	STA.sleeped++;

	CSE.sleepIntervalMicros = micros;
	CncTimeFunctions::sleepMircoseconds(micros);

	return micros;
}




//////////////////////////////////////////////////////////////////////////
CncSpeedSimulator::CncSpeedSimulator()
: CncSpeedController()
, delayAccumulator(this)
//////////////////////////////////////////////////////////////////////////
{
	X.AP.startPeriodStepsMin  = 250;
    X.AP.stopPeriodStepsMin   = 250;
	
	delayAccumulator.initialize();
}
//////////////////////////////////////////////////////////////////////////
CncSpeedSimulator::~CncSpeedSimulator()  {
//////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::simulateOneStepX()  { 
//////////////////////////////////////////////////////////////////////////
	const unsigned int accelDelay = getNextAccelDelayX();
	const int64_t d = X.totalOffset + X.synthSpeedDelay + accelDelay;
	
	delayAccumulator.incMicros(d);
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::simulateOneStepY()  { 
//////////////////////////////////////////////////////////////////////////
	const unsigned int accelDelay = getNextAccelDelayY();
	const int64_t d = Y.totalOffset + Y.synthSpeedDelay + accelDelay;

	delayAccumulator.incMicros(d);
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::simulateOneStepZ()  { 
//////////////////////////////////////////////////////////////////////////
	const unsigned int accelDelay = getNextAccelDelayZ();
	const int64_t d = Z.totalOffset + Z.synthSpeedDelay + accelDelay;

	delayAccumulator.incMicros(d);
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::performCurrentOffset(bool force) {
//////////////////////////////////////////////////////////////////////////
	delayAccumulator.perform(force);
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::initMove(int32_t dx, int32_t dy, int32_t dz) {
//////////////////////////////////////////////////////////////////////////
	CncSpeedController::initMove(dx, dy, dz);
	
	#warning accel
	//std::cout << X.AP << std::endl;
	

	enableAccelerationXYZ(true);
	//std::cout << this->X << std::endl;
	
	
	delayAccumulator.initialize();
}
//////////////////////////////////////////////////////////////////////////
void CncSpeedSimulator::completeMove() {
//////////////////////////////////////////////////////////////////////////
	#ifdef TRACE_SPEED_FLOW
	
		wxString fn(wxString::Format("%s/tx.%lld.txt", CncFileNameService::getTempDir(), CncTimeFunctions::getNanoTimestamp()));
		std::ofstream ofs (fn.c_str().AsChar(), std::ofstream::out);
		delayAccumulator.trace(ofs);
		ofs.close();
		
	#endif
}
