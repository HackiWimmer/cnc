#include <iostream>
#include "CncConfig.h"
#include "MainFrame.h"
#include "CncTimeFunctions.h"

// this have to be done after #include "CncConfig.h"
#include <Windows.h>

bool initialized 						= false;
unsigned int 	countsPerMicrosecond	= 0;
unsigned int 	countsPerHalfUSec		= 0;
unsigned int	tickInterval			= 0;
LARGE_INTEGER 	counterFrequency		= { 0 };
LARGE_INTEGER 	counterReading			= { 0 };
time_t 			timeOfDayBase;

////////////////////////////////////////////////////////////////
void CncTimeFunctions::init() {
////////////////////////////////////////////////////////////////
	initialized = true;
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

	QueryPerformanceFrequency(&counterFrequency);
	
	countsPerMicrosecond 	= counterFrequency.QuadPart/1000000;
	countsPerHalfUSec 		= (counterFrequency.QuadPart + 1)/2000000;
	
	tickInterval = ((1000.0 * 1000 * 1000)/counterFrequency.QuadPart);
	
	LARGE_INTEGER t0, t1;
	unsigned int diff = 0;
	unsigned int counter = 0;
	for ( int i = 0; i < 1000; i++ ) {
		QueryPerformanceCounter(&t0);
		QueryPerformanceCounter(&t1);
		
		if ( (t1.QuadPart - t0.QuadPart) > 0 ) {
			diff += (t1.QuadPart - t0.QuadPart);
			counter++;
		}
	}
	
	diff /= counter;
	tickInterval = std::max(tickInterval, tickInterval * diff);
	
	init_gettimeofday();
}
////////////////////////////////////////////////////////////////
void CncTimeFunctions::init_gettimeofday() {
////////////////////////////////////////////////////////////////
	time_t then;
	time(&then);
	do {
		time(&timeOfDayBase);
	} while (then == timeOfDayBase);
	
	QueryPerformanceCounter (&counterReading);
}
////////////////////////////////////////////////////////////////
int64_t CncTimeFunctions::getOPCFrequency() {
////////////////////////////////////////////////////////////////
	if ( initialized == false )
		CncTimeFunctions::init();

	return (int64_t)counterFrequency.QuadPart;
}
////////////////////////////////////////////////////////////////
unsigned int CncTimeFunctions::geMaxtQPCResolutionNS() {
////////////////////////////////////////////////////////////////
	if ( initialized == false )
		CncTimeFunctions::init();
		
	return tickInterval;
}
////////////////////////////////////////////////////////////////
void CncTimeFunctions::printError(const char *tag) {
////////////////////////////////////////////////////////////////
	LPVOID msg;
	int errorCode = GetLastError();
	
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER
				  | FORMAT_MESSAGE_FROM_SYSTEM
				  | FORMAT_MESSAGE_IGNORE_INSERTS,
				  0, errorCode, 0, (LPTSTR)&msg, 0, 0);
				  
	std::cerr << tag << ": " << "Error code: " << errorCode << "Error msg: " << (LPTSTR)msg << std::endl;
	
	LocalFree(msg);
}
////////////////////////////////////////////////////////////////
int CncTimeFunctions::gettimeofday(struct timeval *tv, void *tz_unused) {
////////////////////////////////////////////////////////////////
	if ( initialized == false )
		CncTimeFunctions::init();

	LARGE_INTEGER now;
	CncNanoTimestamp timeDiff;
	
	QueryPerformanceCounter(&now);
	timeDiff = now.QuadPart - counterReading.QuadPart;
	
	tv->tv_sec  = timeOfDayBase + timeDiff/counterFrequency.QuadPart;
	tv->tv_usec = ((timeDiff % counterFrequency.QuadPart) + countsPerHalfUSec) / countsPerMicrosecond;
	
	if ( tv->tv_usec >= 1000 * 1000 ) {
		tv->tv_sec  += tv->tv_usec / (1000 * 1000);
		tv->tv_usec  = tv->tv_usec % (1000 * 1000);
	}
	 
	return 0;
}
////////////////////////////////////////////////////////////////
CncNanoTimestamp CncTimeFunctions::getNanoTimestamp() {
////////////////////////////////////////////////////////////////
	if ( initialized == false )
		CncTimeFunctions::init();

	LARGE_INTEGER count;
	QueryPerformanceCounter(&count);
	
	time_point tp(duration(count.QuadPart * static_cast<CncNanoTimestamp>(period::den) / counterFrequency.QuadPart));
	return tp.time_since_epoch().count();
}
////////////////////////////////////////////////////////////////
CncNanoTimespan CncTimeFunctions::getTimeSpan(const CncTimeval& a, const CncTimeval& b) {
////////////////////////////////////////////////////////////////
	return (a.tv_sec * 1000 * 1000 + a.tv_usec - (b.tv_sec * 1000 * 1000 + b.tv_usec));
}
////////////////////////////////////////////////////////////////
CncNanoTimespan CncTimeFunctions::getTimeSpan(const CncNanoTimestamp& a, const CncNanoTimestamp& b) {
////////////////////////////////////////////////////////////////
	 return a - b;
}
////////////////////////////////////////////////////////////////
void CncTimeFunctions::busyWaitMircoseconds(unsigned int micros) {
////////////////////////////////////////////////////////////////
	if ( micros < 0LL )
		return;

	struct timeval t0, t1;
	CncTimeFunctions::gettimeofday(&t0, NULL);
	
	do {
		CncTimeFunctions::gettimeofday(&t1, NULL);
		
	} while ( (t1.tv_sec * 1000 * 1000 + t1.tv_usec - (t0.tv_sec * 1000 * 1000 + t0.tv_usec)) < (int)micros );
}
////////////////////////////////////////////////////////////////
void CncTimeFunctions::activeWaitMircoseconds(int64_t micros) {
////////////////////////////////////////////////////////////////
	if ( micros < 0LL )
		return;
	
	static const int64_t threshold = 1625LL;
	
	if ( micros > 3000 ) {
		int64_t x = (micros - threshold) / 1000;
		micros    = threshold + (micros - threshold) % 1000;
		
		GBL_CONFIG->getTheApp()->waitActive(x);
	}
	
	sleepMircoseconds(micros);
}

////////////////////////////////////////////////////////////////
void CncTimeFunctions::sleepMircoseconds(int64_t micros) {
////////////////////////////////////////////////////////////////
	if ( micros < 0LL )
		return;
	
	if ( micros < 1625LL )
		busyWaitMircoseconds(micros);
	
	HANDLE timer; 
	LARGE_INTEGER ft; 
	
	// Convert to 100 nanosecond interval, negative value indicates relative time
	ft.QuadPart = -( 10 * (__int64)micros); 
	timer = CreateWaitableTimer(NULL, TRUE, NULL); 
	SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0); 
	WaitForSingleObject(timer, INFINITE); 
	CloseHandle(timer); 
}
