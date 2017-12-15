#include <iostream>
#include <chrono>
#include <sys/time.h>

#include <wx/init.h>
#include <wx/string.h>
#include "CncArduino.h"
#include "CncTimeFunctions.h"


struct timespec req = {0};

//////////////////////////////////////////////////////////////////
void stepSleep(unsigned int width) {
///////////////////////////////////////////////////////////////////
	CncTimeFunctions::busyWaitMircoseconds(width);
}
//////////////////////////////////////////////////////////////////
void digitalWrite(unsigned int width) {
///////////////////////////////////////////////////////////////////
	stepSleep(width);
}
///////////////////////////////////////////////////////////////////
void test1(CncSpeedCalculator& scx) {
///////////////////////////////////////////////////////////////////
	std::clog << "::Test1 start ----------------------------------------" << std::endl;
	
	double s = 0.0;									std::clog << "Speed (" << wxString::Format("%10.1lf", s) << ") = " << wxString::Format("% 10d", scx.calcSpeedOffset(s)) << " [us]" << std::endl;
	s =  500.0;										std::clog << "Speed (" << wxString::Format("%10.1lf", s) << ") = " << wxString::Format("% 10d", scx.calcSpeedOffset(s)) << " [us]" << std::endl;
	s = 1000.0;										std::clog << "Speed (" << wxString::Format("%10.1lf", s) << ") = " << wxString::Format("% 10d", scx.calcSpeedOffset(s)) << " [us]" << std::endl;
	s = 2000.0;										std::clog << "Speed (" << wxString::Format("%10.1lf", s) << ") = " << wxString::Format("% 10d", scx.calcSpeedOffset(s)) << " [us]" << std::endl;
	s = 3000.0;										std::clog << "Speed (" << wxString::Format("%10.1lf", s) << ") = " << wxString::Format("% 10d", scx.calcSpeedOffset(s)) << " [us]" << std::endl;
	s = scx.getMaxSpeed_MM_MIN() - 100.0;			std::clog << "Speed (" << wxString::Format("%10.1lf", s) << ") = " << wxString::Format("% 10d", scx.calcSpeedOffset(s)) << " [us]" << std::endl;
	s = scx.getMaxSpeed_MM_MIN() -  10.0;			std::clog << "Speed (" << wxString::Format("%10.1lf", s) << ") = " << wxString::Format("% 10d", scx.calcSpeedOffset(s)) << " [us]" << std::endl;
	s = scx.getMaxSpeed_MM_MIN() -   1.0;			std::clog << "Speed (" << wxString::Format("%10.1lf", s) << ") = " << wxString::Format("% 10d", scx.calcSpeedOffset(s)) << " [us]" << std::endl;
	s = scx.getMaxSpeed_MM_MIN() +   0.0;			std::clog << "Speed (" << wxString::Format("%10.1lf", s) << ") = " << wxString::Format("% 10d", scx.calcSpeedOffset(s)) << " [us]" << std::endl;
	s = scx.getMaxSpeed_MM_MIN() +   1.0;			std::clog << "Speed (" << wxString::Format("%10.1lf", s) << ") = " << wxString::Format("% 10d", scx.calcSpeedOffset(s)) << " [us]" << std::endl;
	

	std::clog << "::Test1 end   ----------------------------------------" << std::endl;
}
///////////////////////////////////////////////////////////////////
void test2(CncSpeedCalculator& scx) {
///////////////////////////////////////////////////////////////////
	std::clog << "::Test2 start ----------------------------------------" << std::endl;
	
	// speed mm/min
	double s = 600.0;
	// one step
	double os = scx.getPitch() / scx.getSteps360();
	// distance
	double d = 10.0; // mm
	// steps to go
	unsigned int stg = (unsigned int)(d/os);
	clog << "Steps to go     = " << stg << endl;
		
	unsigned int offset = scx.calcSpeedOffset(s);
	clog << "Offset          = " << offset << endl;
	
	CncNanoTimestamp tsStart = CncTimeFunctions::getNanoTimestamp();
	
	for (unsigned int i = 0; i < stg; i++ ) {
		digitalWrite(scx.getSinglePulseWidth());
		
		stepSleep(offset);
		
		digitalWrite(scx.getSinglePulseWidth());
	}
	CncNanoTimestamp tsStop = CncTimeFunctions::getNanoTimestamp();
	long duration = (tsStop - tsStart);
	
	clog << "Duration        = " << duration << ", " << ((double)(duration) / 1000) << ", " << ((double)duration) / (1000 * 1000) << endl;
	clog << "Expected        = " << (double)(offset * stg) + (double)(scx.getTotalPulseWidth() * stg) << endl;
	
	
	std::clog << "::Test2 end   ----------------------------------------" << std::endl;
}
///////////////////////////////////////////////////////////////////
void test3(CncSpeedCalculator& scx) {
///////////////////////////////////////////////////////////////////
	std::clog << "::Test3 start ----------------------------------------" << std::endl;
	
	CncNanoTimestamp tsStart = CncTimeFunctions::getNanoTimestamp();
	CncTimeFunctions::busyWaitMircoseconds(100);
	CncNanoTimestamp tsStop  = CncTimeFunctions::getNanoTimestamp();
	
	clog << "Start           = " << tsStart << endl;
	clog << "Stop            = " << tsStop  << endl;
	
	long duration = (tsStop - tsStart);
	clog << "Duration        = " << duration << ", " << ((double)(duration) / 1000) << ", " << ((double)duration) / (1000 * 1000) << endl;
	
	std::clog << "::Test3 end   ----------------------------------------" << std::endl;
}

///////////////////////////////////////////////////////////////////
int main( int argc, char** argv ) {
///////////////////////////////////////////////////////////////////
	// initialize wxWidgets
	wxInitializer init;
	
	CncSpeedCalculator scx(12.0, 800, 100);
	scx.trace(std::clog); std::clog << endl;
	
	test1(scx);
	test2(scx);
	//test3(scx);
	
	
	std::cout << "\nBye bye"<< std::endl; 
	return 0;
	
}
