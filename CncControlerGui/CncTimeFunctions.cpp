#include <iostream>
#include <windows.h>
#include "CncTimeFunctions.h"


LARGE_INTEGER 	counterFrequency		= { 0 };
LARGE_INTEGER 	counterReading			= { 0 };
unsigned int 	countsPerMicrosecond	= 0;
unsigned int 	countsPerHalfUSec		= 0;
time_t 			timeOfDayBase;

////////////////////////////////////////////////////////////////
void CncTimeFunctions::init() {
////////////////////////////////////////////////////////////////
	init_gettimeofday();
	
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
}
////////////////////////////////////////////////////////////////
void CncTimeFunctions::init_gettimeofday() {
////////////////////////////////////////////////////////////////
	time_t then;
	HANDLE curProcess;
	DWORD priorityOld;
	int s;
	
	QueryPerformanceFrequency (& counterFrequency);
	countsPerMicrosecond = counterFrequency.QuadPart/1000000;
	countsPerHalfUSec = (counterFrequency.QuadPart + 1)/2000000;
	
	curProcess = GetCurrentProcess();
	priorityOld = GetPriorityClass(curProcess);
	s = SetPriorityClass(curProcess, REALTIME_PRIORITY_CLASS);
	
	if ( !s )
		printError("SetPriorityClass failed");
	
	time(&then);
	do {
		time(&timeOfDayBase);
	} while (then == timeOfDayBase);
	
	QueryPerformanceCounter (&counterReading);
	SetPriorityClass(curProcess, priorityOld);
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
	LARGE_INTEGER now;
	BOOL s;
	long long timeDiff;
	
	if ( countsPerMicrosecond == 0 )
		CncTimeFunctions::init_gettimeofday();
	
	s = QueryPerformanceCounter(&now);
	
	timeDiff = now.QuadPart - counterReading.QuadPart;
	
	tv->tv_sec  = timeOfDayBase + timeDiff/counterFrequency.QuadPart;
	tv->tv_usec = ((timeDiff % counterFrequency.QuadPart) + countsPerHalfUSec) / countsPerMicrosecond;
	
	/*
	if ( tv->tv_usec >= 1000 * 1000 ) {
		//tv->tv_sec++;
		//tv->tv_usec = 0;
	}*/
	
	if ( tv->tv_usec >= 1000 * 1000 ) {
		tv->tv_sec  += tv->tv_usec / (1000 * 1000);
		tv->tv_usec  = tv->tv_usec % (1000 * 1000);
	}
	return !s;
}
////////////////////////////////////////////////////////////////
CncTimestamp CncTimeFunctions::getMicrosecondTimestamp() {
////////////////////////////////////////////////////////////////
	struct CncTimeval t;
	CncTimeFunctions::gettimeofday(&t, NULL);
	
	return t.getAsTimestamp();
}
////////////////////////////////////////////////////////////////
CncTimespan CncTimeFunctions::getTimeSpan(const CncTimeval& a, const CncTimeval& b) {
////////////////////////////////////////////////////////////////
	return (a.tv_sec * 1000 * 1000 + a.tv_usec - (b.tv_sec * 1000 * 1000 + b.tv_usec));
}
////////////////////////////////////////////////////////////////
CncTimespan CncTimeFunctions::getTimeSpan(const CncTimestamp& a, const CncTimestamp& b) {
////////////////////////////////////////////////////////////////
	 return a - b;
}
////////////////////////////////////////////////////////////////
void CncTimeFunctions::busyWaitMircoseconds(unsigned int micros) {
////////////////////////////////////////////////////////////////
	struct timeval t0, t1;
	CncTimeFunctions::gettimeofday(&t0, NULL);
	
	do {
		CncTimeFunctions::gettimeofday(&t1, NULL);
		
	} while ( (t1.tv_sec * 1000 * 1000 + t1.tv_usec - (t0.tv_sec * 1000 * 1000 + t0.tv_usec)) < (int)micros );
}

