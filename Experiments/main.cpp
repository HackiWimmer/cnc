#include <iostream>
#include <chrono>
#include <sys/time.h>

#include <wx/init.h>
#include <wx/string.h>


///////////////////////////////////////////////////////////////////
int main( int argc, char** argv ) {
///////////////////////////////////////////////////////////////////
	// initialize wxWidgets
	wxInitializer init;
	
	//CncNanoTimestamp ts0 = CncTimeFunctions::getNanoTimestamp()
	
	LARGE_INTEGER count;
	while ( true ) {
		QueryPerformanceCounter(&count);
	}

	
	
	
	std::cout << "\nBye bye"<< std::endl; 
	return 0;
	
}
