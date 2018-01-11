#ifdef __WXGTK__
	#include <time.h>
#endif

#include <iostream>
#include "CncConfig.h"
#include "MainFrame.h"
#include "CncTimeFunctions.h"

#ifdef __WXMSW__
	// this have to be done after #include "CncConfig.h"
	#include <Windows.h>

	bool initialized 						= false;
	unsigned int 	countsPerMicrosecond	= 0;
	unsigned int 	countsPerHalfUSec		= 0;
	unsigned int	tickInterval			= 0;
	LARGE_INTEGER 	counterFrequency		= { 0 };
	LARGE_INTEGER 	counterReading			= { 0 };
	time_t 			timeOfDayBase;
	
#endif

////////////////////////////////////////////////////////////////
void CncTimeFunctions::init() {
////////////////////////////////////////////////////////////////
#ifdef __WXMSW__
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
#endif
}
////////////////////////////////////////////////////////////////
void CncTimeFunctions::init_gettimeofday() {
////////////////////////////////////////////////////////////////
#ifdef __WXMSW__
	time_t then;
	time(&then);
	do {
		time(&timeOfDayBase);
	} while (then == timeOfDayBase);
	
	QueryPerformanceCounter (&counterReading);
#endif
}
////////////////////////////////////////////////////////////////
int64_t CncTimeFunctions::getOPCFrequency() {
////////////////////////////////////////////////////////////////
#ifdef __WXMSW__
	if ( initialized == false )
		CncTimeFunctions::init();
	
	return (int64_t)counterFrequency.QuadPart;
#else
	return 0LL;
#endif
}
////////////////////////////////////////////////////////////////
unsigned int CncTimeFunctions::geMaxtQPCResolutionNS() {
////////////////////////////////////////////////////////////////
#ifdef __WXMSW__
	if ( initialized == false )
		CncTimeFunctions::init();
		
	return tickInterval;
#else
	return 0;
#endif
}
////////////////////////////////////////////////////////////////
void CncTimeFunctions::printError(const char *tag) {
////////////////////////////////////////////////////////////////
#ifdef __WXMSW__
	LPVOID msg;
	int errorCode = GetLastError();
	
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER
				  | FORMAT_MESSAGE_FROM_SYSTEM
				  | FORMAT_MESSAGE_IGNORE_INSERTS,
				  0, errorCode, 0, (LPTSTR)&msg, 0, 0);
				  
	std::cerr << tag << ": " << "Error code: " << errorCode << "Error msg: " << (LPTSTR)msg << std::endl;
	
	LocalFree(msg);
#endif
}
////////////////////////////////////////////////////////////////
int CncTimeFunctions::gettimeofday(struct timeval *tv, void *tz) {
////////////////////////////////////////////////////////////////
#ifdef __WXMSW__
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
# endif

#ifdef __WXGTK__
	return gettimeofday(tv, tz);
#endif
}
////////////////////////////////////////////////////////////////
CncNanoTimestamp CncTimeFunctions::getNanoTimestamp() {
////////////////////////////////////////////////////////////////
#ifdef __WXMSW__
	if ( initialized == false )
		CncTimeFunctions::init();
	
	LARGE_INTEGER count;
	QueryPerformanceCounter(&count);
	
	time_point tp(duration(count.QuadPart * (static_cast<CncNanoTimestamp>(period::den) / counterFrequency.QuadPart)));
	return tp.time_since_epoch().count();
#endif

#ifdef __WXGTK__
	struct timeval tv;
	gettimeofday(&tv, NULL);
	
	return tv.tv_sec * 1000 * 1000 + tv.tv_usec;
#endif
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
#ifdef __WXMSW__
	if ( micros <= 0 )
		return;
	
	CncNanoTimestamp tsStart = getNanoTimestamp();
	CncNanoTimestamp tsEnd   = 0LL;
	do {
		tsEnd  = getNanoTimestamp();
	} while ( (tsEnd - tsStart)/1000 < micros );
#endif

#ifdef __WXGTK__
	sleepMircoseconds((int64_t) micros)
#endif
}
////////////////////////////////////////////////////////////////
void CncTimeFunctions::sleepMircoseconds(int64_t micros) {
////////////////////////////////////////////////////////////////
	if ( micros <= 0LL )
		return;

#ifdef __WXMSW__
	
	if ( micros <= 16250LL ) {
		busyWaitMircoseconds(micros);
		return;
	}
	
	HANDLE timer; 
	LARGE_INTEGER ft; 
	
	// Convert to 100 nanosecond interval, negative value indicates relative time
	ft.QuadPart = -( 10 * (__int64)micros); 
	timer = CreateWaitableTimer(NULL, TRUE, NULL); 
	SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0); 
	WaitForSingleObject(timer, INFINITE); 
	CloseHandle(timer); 
#endif

#ifdef __WXGTK__
	struct timespec req, rem;
	req.tv_sec  = micros/1000;
	req.tv_nsec = micros%1000;
	
	nanosleep(req, rem);
#endif
}
////////////////////////////////////////////////////////////////
void CncTimeFunctions::activeWaitMircoseconds(int64_t micros, bool active) {
////////////////////////////////////////////////////////////////
	if ( micros <= 0LL )
		return;
	
	static const int64_t threshold = 16250LL;
	
	if ( micros > 30000 ) {
		int64_t x = (micros - threshold) / 1000;
		micros    = threshold + (micros - threshold) % 1000;
		
		if ( active == true ) 	GBL_CONFIG->getTheApp()->waitActive(x);
		else					sleepMircoseconds(x * 1000);
	}
		
	sleepMircoseconds(micros);
}
////////////////////////////////////////////////////////////////
void CncTimeFunctions::sleep(unsigned int milliseconds) {
////////////////////////////////////////////////////////////////
#ifdef __WXMSW__
	Sleep(milliseconds);
#endif

#ifdef __WXGTK__
	sleep(milliseconds);
#endif
}