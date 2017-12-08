#include <iostream>
#include <stdio.h>
#include <windows.h>
#include "CncTimeFunctions.h"

int main() {
	
	struct CncTimeval t0, t1;
	CncTimeFunctions::init();
	
	//SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	
	CncTimeFunctions::gettimeofday(&t0, NULL);
	do {
		CncTimeFunctions::gettimeofday(&t1, NULL);
	} while (t0.tv_sec == t1.tv_sec && t0.tv_usec == t1.tv_usec);
	t0 = t1;

	int cnt = 0;
	do {
		CncTimeFunctions::gettimeofday(&t1, NULL);
		cnt++;
	} while (t0.tv_sec == t1.tv_sec && t0.tv_usec == t1.tv_usec);

    int d = t1.tv_usec - t0.tv_usec + (t1.tv_sec - t0.tv_sec)*1000000;

    // resolution test
    printf("t0: %ld %ld\n", t0.tv_sec, t0.tv_usec);
    printf("t1: %ld %ld\n", t1.tv_sec, t1.tv_usec);
    printf("resolution: %d usec after %d calls\n", d, cnt);

	const int MX_CNT = 20;
	
	long  sec[MX_CNT];
	long usec[MX_CNT];

	for (int i=0; i < MX_CNT; i++) {
		CncTimeFunctions::busyWaitMircoseconds(10);
		CncTimeFunctions::gettimeofday(&t0, NULL);
		
		sec[i]  = t0.tv_sec;
		usec[i] = t0.tv_usec;
	}

	for (int i=0; i < MX_CNT; i++) {
		printf("secs: %ld usecs %3ld\n", sec[i], usec[i]);
	}



}
