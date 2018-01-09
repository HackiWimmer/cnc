#ifndef CNC_TEST_SUITE_H
#define CNC_TEST_SUITE_H

#include "CommonValues.h"
#include "CncController.h"
#include "LastErrorCodes.h"

class CncTestSuite {

  private:
    unsigned int  testId;
    CncController&  controller;
    LastErrorCodes& errorInfo;
    String parameterInfo;

    bool isParamValid(long val) const { return val != INVALID_PARAM_VAL; }

    const char* getParameterInfo(unsigned int id);

    // test routines 
    char runHeartbeatTest();
    char runMotorConfigInterval();
    char runDurationTest();
    //...
    
   public:
      const unsigned int MAX_TEST_ID = 100;

      static const long INVALID_PARAM_VAL = MIN_LONG;
      static long testParam1;
      static long testParam2;
      static long testParam3;
      static long testParam4;
      static long testParam5;

      static bool interruptFlag;

      //////////////////////////////////////////////////////////////////////////////
      CncTestSuite(unsigned int id, CncController& ctrl, LastErrorCodes& lec) 
      : testId(id)
      , controller(ctrl)
      , errorInfo(lec)
      {
        if ( id > MAX_TEST_ID )
          testId = TID_INVALID;
      }
  
      //////////////////////////////////////////////////////////////////////////////
      ~CncTestSuite() {
      }

      //////////////////////////////////////////////////////////////////////////////
      static bool checkForTestSuiteEndFlag();

       //////////////////////////////////////////////////////////////////////////////
      static void interruptTest();
      static void resetInterruptFlag() { interruptFlag = false; }
      static bool isTestInterrupted() { return interruptFlag; }

      //////////////////////////////////////////////////////////////////////////////
      unsigned int getTestId() const { return testId; }

      //////////////////////////////////////////////////////////////////////////////
      void initTestParemeters() {
        CncTestSuite::testParam1 = INVALID_PARAM_VAL;
        CncTestSuite::testParam2 = INVALID_PARAM_VAL;
        CncTestSuite::testParam3 = INVALID_PARAM_VAL;
        CncTestSuite::testParam4 = INVALID_PARAM_VAL;
        CncTestSuite::testParam5 = INVALID_PARAM_VAL;
      }
      
      static void setTestParameter1(long val) { CncTestSuite::testParam1 = val; }
      static void setTestParameter2(long val) { CncTestSuite::testParam2 = val; }
      static void setTestParameter3(long val) { CncTestSuite::testParam3 = val; }
      static void setTestParameter4(long val) { CncTestSuite::testParam4 = val; }
      static void setTestParameter5(long val) { CncTestSuite::testParam5 = val; }

      //////////////////////////////////////////////////////////////////////////////
      static void printConfig();
  
      //////////////////////////////////////////////////////////////////////////////
      char run();
};

#endif
