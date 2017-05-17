#include "UnitTestFrame.h"
#include "Test_Base.h"

////////////////////////////////////////////////////////////////
void TEST_BASE::FRAMEWORK_CALLBACK(FrameworkCallbackInfo& fci) {
////////////////////////////////////////////////////////////////
	if ( unitTestFrame == NULL )
		return;
		
	unitTestFrame->callbackFromTest(fci);
}


