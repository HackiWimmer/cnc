
#include "CncControllerTestSuite.h"

CncTestCaseMap CncControllerTestSuite::testCaseList;

/////////////////////////////////////////////////////////////////////////
class CncControllerTestSuiteInitializer {
	public:
		CncControllerTestSuiteInitializer() {
			CncControllerTestSuite::init();
		}
}; CncControllerTestSuiteInitializer tsi;


/////////////////////////////////////////////////////////////////////////
void CncControllerTestSuite::init() {
/////////////////////////////////////////////////////////////////////////
	CncTestCaseConfig tcc;
	
	tcc.init();
	tcc.id 				= TID_HEARTBEAT;
	tcc.name 			= "SendHeartbeatInterval( hbCount[#], interval[ms] )";
	tcc.description		= "Sends <hbCount> heartbeats from the controller to the PC and waits <interval> between each heartbeat.";
	tcc.parameterCount 	= 2;
	tcc.initDefaultValues(10, 1000);
	registerTestCase(tcc);
	
	tcc.init();
	tcc.id 				= TID_MOTOR_CFG_INTERVAL;
	tcc.name 			= "runMotorConfigInterval( axis[1=X, 2=y, 3=z], distance[mm], checkSupportButtonPressed[0=no, 1=yes] )";
	tcc.description		= "Oscillates +- <distance> for the given <axis> until the test was stopped.\n" \
	                      "If parameter <checkSupportButtonPressed> == 1 the test runs only if the hardware support button is pressed.\n" \
	                      "The test can be stopped by the run control stop button.\n" \
						  "Please note: After this test the PC position is invalid and the controller has to be reseted.";
	tcc.parameterCount 	= 3;
	tcc.initDefaultValues(1, 20, 0);
	registerTestCase(tcc);
	
	tcc.init();
	tcc.id 				= 99;
	tcc.name 			= "Not registered test";
	tcc.description		= "Only for debug purpose.";
	tcc.parameterCount 	= 0;
	tcc.initDefaultValues();
	registerTestCase(tcc);
}