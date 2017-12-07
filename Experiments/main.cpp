#include <iostream>
#include <wx/init.h>
#include <wx/string.h>

//#include "CncArduino.h"

int main( int argc, char** argv ) {
	// initialize wxWidgets
	wxInitializer init;
	//wxPrintf( wxT("Hello in wxWidgets World!\n\n") )
	
	
	long values = -100;
	const double factorX = 0.015;
	long double t = 0.0;
	
	double x = (abs(values) * factorX) * 2;
	t += x; std::clog << t << ", " << x << std::endl;
	x = (abs(values) * factorX);
	t += x; std::clog << t << ", " << x << std::endl;
	t += x; std::clog << t << ", " << x << std::endl;
	
	return 0;
	
	/*
	CncSpeedCalculator scx(12.0, 800, 250);
	std::clog << "Max Speed MM SEC:  " << scx.getMaxSpeed_MM_SEC() << std::endl;
	std::clog << "Max Speed MM MIN:  " << scx.getMaxSpeed_MM_MIN() << std::endl;
	
	double s = 3000.0;
	std::clog << "Speed (" << s << ") " << scx.calcSpeedOffset(s) << std::endl;
	s = 300.0;
	std::clog << "Speed (" << s << ") " << scx.calcSpeedOffset(s) << std::endl;
	s = 100.0;
	std::clog << "Speed (" << s << ") " << scx.calcSpeedOffset(s) << std::endl;
	s = 4100.0;
	std::clog << "Speed (" << s << ") " << scx.calcSpeedOffset(s) << std::endl;


	CncSpeedCalculator scz(2.0, 400, 250);
	std::clog << "Max Speed MM SEC:  " << scz.getMaxSpeed_MM_SEC() << std::endl;
	std::clog << "Max Speed MM MIN:  " << scz.getMaxSpeed_MM_MIN() << std::endl;
	
	
	std::cout << "Bye bye"<< std::endl; 
	return 0;
	 */
}
