#include <cmath> 
#include <algorithm>
#include <numeric>
#include <iostream>
#include <utility>
#include <vector>

#include "impl.h"
#include <wx/string.h>
#include <wx/datetime.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FtMonitor.h"
#include <thread>
////////////////////////////////////////////////////
void Implementation::run() {
////////////////////////////////////////////////////
	std::cout << "Start . . ." << std::endl;
	
	CCS::FtMonitor::test_All();
	return;
	
	/*
	//---------------------------------------------
	CCS::FtMonitor monitor("Max");
	std::cout << monitor << std::endl << std::endl;
	
	//---------------------------------------------
	monitor.addServerInfo("Max", CCS::FtMonitor::Unknown, CCS::FtMonitor::determineServerRating());
	std::cout << monitor << std::endl << std::endl;
	
	//---------------------------------------------
	std::cout << "evaluate(...)" << std::endl; monitor.evaluate();
	std::cout << monitor << std::endl << std::endl;
	
	//---------------------------------------------
	std::cout << "evaluate(...)" << std::endl; monitor.evaluate();
	std::cout << monitor << std::endl << std::endl;

	//---------------------------------------------
	std::cout << "evaluate(...)" << std::endl; monitor.evaluate();
	std::cout << monitor << std::endl << std::endl;

	//---------------------------------------------
	std::cout << "evaluate(...)" << std::endl; monitor.evaluate();
	std::cout << monitor << std::endl << std::endl;

	monitor.addServerInfo("Mia", CCS::FtMonitor::Unknown, CCS::FtMonitor::determineServerRating());
	std::cout << "evaluate(...)" << std::endl; monitor.evaluate();
	std::cout << monitor << std::endl << std::endl;
	return;
	
	//std::cout << "Sleep ....." << std::endl;
	//std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	
	//---------------------------------------------
	std::cout << "evaluate(...)" << std::endl; monitor.evaluate();
	std::cout << monitor << std::endl << std::endl;
	
	//---------------------------------------------
	std::cout << "---------------------------------------------" << std::endl;
	std::cout << "reset(...)" << std::endl;
	monitor.reset();
	std::cout << monitor << std::endl << std::endl;
	*/
}
