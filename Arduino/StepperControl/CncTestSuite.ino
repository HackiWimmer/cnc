#include "CncTestSuite.h"

//////////////////////////////////////////////////////////////////////////////
long CncTestSuite::testParam1 = INVALID_PARAM_VAL;
long CncTestSuite::testParam2 = INVALID_PARAM_VAL;
long CncTestSuite::testParam3 = INVALID_PARAM_VAL;
long CncTestSuite::testParam4 = INVALID_PARAM_VAL;
long CncTestSuite::testParam5 = INVALID_PARAM_VAL;

bool CncTestSuite::interruptFlag = false;

//////////////////////////////////////////////////////////////////////////////
void CncTestSuite::printConfig() {
//////////////////////////////////////////////////////////////////////////////
  Serial.print(PID_TEST_SUITE); Serial.print(TEXT_SEPARATOR);     Serial.print(""); Serial.write(TEXT_CLOSE);
  
  Serial.print(BLANK); Serial.print(PID_TEST_VALUE1);     Serial.print(TEXT_SEPARATOR); Serial.print(CncTestSuite::testParam1);     Serial.write(TEXT_CLOSE);
  Serial.print(BLANK); Serial.print(PID_TEST_VALUE2);     Serial.print(TEXT_SEPARATOR); Serial.print(CncTestSuite::testParam2);     Serial.write(TEXT_CLOSE);
  Serial.print(BLANK); Serial.print(PID_TEST_VALUE3);     Serial.print(TEXT_SEPARATOR); Serial.print(CncTestSuite::testParam3);     Serial.write(TEXT_CLOSE);
  Serial.print(BLANK); Serial.print(PID_TEST_VALUE4);     Serial.print(TEXT_SEPARATOR); Serial.print(CncTestSuite::testParam4);     Serial.write(TEXT_CLOSE);
  Serial.print(BLANK); Serial.print(PID_TEST_VALUE5);     Serial.print(TEXT_SEPARATOR); Serial.print(CncTestSuite::testParam5);     Serial.write(TEXT_CLOSE);
  Serial.print(BLANK); Serial.print(PID_TEST_INTERRUPT);  Serial.print(TEXT_SEPARATOR); Serial.print(CncTestSuite::interruptFlag);  Serial.write(TEXT_CLOSE);
}
//////////////////////////////////////////////////////////////////////////////
bool CncTestSuite::checkForTestSuiteEndFlag() {
//////////////////////////////////////////////////////////////////////////////
  if ( interruptFlag == true )
    return true;

  unsigned char frontSerialByte = CMD_INVALID;
  
  if ( peakSerial(frontSerialByte) == true ) {
    if ( frontSerialByte == SIG_HALT ) {
      Serial.read();
      return true;
    }
  }
   
  return false; 
}
//////////////////////////////////////////////////////////////////////////////
void  CncTestSuite::interruptTest() {
//////////////////////////////////////////////////////////////////////////////
  interruptFlag = true;
}
//////////////////////////////////////////////////////////////////////////////
const char* CncTestSuite::getParameterInfo(unsigned int id) {
//////////////////////////////////////////////////////////////////////////////
  parameterInfo  = "Parameter";
  parameterInfo += String(id);
  parameterInfo += " = ";
  
  switch ( id ) {
    case 1:   parameterInfo += CncTestSuite::testParam1; break;
    case 2:   parameterInfo += CncTestSuite::testParam2; break;
    case 3:   parameterInfo += CncTestSuite::testParam3; break;
    case 4:   parameterInfo += CncTestSuite::testParam4; break;
    case 5:   parameterInfo += CncTestSuite::testParam5; break;
    default:  parameterInfo += "Invalid";
  }
  
  return parameterInfo.c_str();
}
//////////////////////////////////////////////////////////////////////////////
unsigned char CncTestSuite::run() {
//////////////////////////////////////////////////////////////////////////////
  char ret = RET_OK;

  switch( testId ) {
    //todo

    case TID_HEARTBEAT:           ret = runHeartbeatTest();
                                  break;

    case TID_MOTOR_CFG_INTERVAL:  ret = runMotorConfigInterval();
                                  break;
                                  
    case TID_DURATION:            ret = runDurationTest();
                                  break;                          

    case TID_INVALID:             ret = errorInfo.setNextErrorInfo(E_INVALID_TEST_ID);
                                  break;
                      
    default:                      ret = errorInfo.setNextErrorInfo(E_NOT_KNOWN_TEST_ID, String(testId).c_str());
                                  break; 
  }

  initTestParemeters();
  return ret;
}
//////////////////////////////////////////////////////////////////////////////
unsigned char CncTestSuite::runHeartbeatTest() {
//////////////////////////////////////////////////////////////////////////////
  long maxValue   = 10;
  long delayValue = 1000;
  
  if ( isParamValid(CncTestSuite::testParam1) && CncTestSuite::testParam1 > 0 )
    maxValue = CncTestSuite::testParam1;

  if ( isParamValid(CncTestSuite::testParam2) && CncTestSuite::testParam2 > 0 )
    delayValue = CncTestSuite::testParam2;
  
  // run the test
  for ( long i=0; i<maxValue; i++ ) {
    if ( CncTestSuite::checkForTestSuiteEndFlag() == true )
      break;
      
    sendHeartbeat();
    delay(delayValue);
  }

  return RET_OK;
}
//////////////////////////////////////////////////////////////////////////////
unsigned char CncTestSuite::runMotorConfigInterval() {
//////////////////////////////////////////////////////////////////////////////
  long axis           = 0;
  long distance       = 0;
  long supportButton  = 0;
  
  if ( isParamValid(CncTestSuite::testParam1) && CncTestSuite::testParam1 > 0 && CncTestSuite::testParam1 < 4 )
    axis = CncTestSuite::testParam1;

  if ( axis == 0 ) {
    errorInfo.setNextErrorInfo(E_INVALID_PARAMETER, getParameterInfo(1));
    return RET_ERROR;
  }

  if ( isParamValid(CncTestSuite::testParam2) && CncTestSuite::testParam2 > 0 && CncTestSuite::testParam2 < 300 )
    distance = CncTestSuite::testParam2;

  if ( distance == 0 ) {
    errorInfo.setNextErrorInfo(E_INVALID_PARAMETER, getParameterInfo(1));
    return RET_ERROR;
  }

  if ( isParamValid(CncTestSuite::testParam3) && CncTestSuite::testParam3 >= 0 && CncTestSuite::testParam3 <= 1 )
    supportButton = CncTestSuite::testParam3;
  else
    pushWarningMessage("Parameter3: out of range [0,1]");

  if ( axis == 3 && distance > 50 )
    distance = 50; 
    
  CncStepper* stepper = NULL;
  switch ( axis ) {
    case 1:   stepper = controller.getStepperX(); break;
    case 2:   stepper = controller.getStepperY(); break;
    case 3:   stepper = controller.getStepperZ(); break;
    
    default:  return errorInfo.setNextErrorInfo(E_INVALID_PARAMETER, String(1).c_str());
  }

  long steps = stepper->calcStepsForMM(distance);

  // run the test
  while ( true ) {
    sendHeartbeat();

    if ( CncTestSuite::checkForTestSuiteEndFlag() == true )
      break;

    if ( supportButton == 1 && controller.evaluateSupportButtonState() == false  ) {
      stepper->enableStepperPin(false);
      delay(500);
      continue;
    }
    
    stepper->enableStepperPin(true);
      
    if ( stepper->stepAxis(steps, true) == false )
      break;
      
    steps *= -1;
  }

  return RET_OK;
}
//////////////////////////////////////////////////////////////////////////////
unsigned char CncTestSuite::runDurationTest() {
//////////////////////////////////////////////////////////////////////////////
  long durations = 0;
  
  if ( isParamValid(CncTestSuite::testParam1) && CncTestSuite::testParam1 > 0 && CncTestSuite::testParam1 < MAX_LONG )
    durations = CncTestSuite::testParam1;

  if ( durations == 0 ) {
    return errorInfo.setNextErrorInfo(E_INVALID_PARAMETER, getParameterInfo(1));
  }

  unsigned long sTime = millis();

  for ( long i=0; i< durations; i++) {
    //writeLongValues(PID_XYZ_POS, i, i, i);
    //writeLongValue(PID_X_POS, i);
  }

  unsigned long eTime = millis();
  unsigned long total = eTime - sTime;

  String summary("Durations: ");
  summary += durations;
  summary += "\n";
  summary += "Total [ms]: ";
  summary += total;
  summary += "\n";
  summary += "Step [us]: ";
  summary += (total * 1000.0 / durations);

  pushErrorMessage(summary.c_str());

  return RET_OK;
}

