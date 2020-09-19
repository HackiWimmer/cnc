#include "CncCtrl.h"
#include "CncTest.h"

/////////////////////////////////////////////////////////////////////////////////////
ArduinoTestManager::ArduinoTestManager(CncArduinoController* ctrl)
: controller(ctrl)
/////////////////////////////////////////////////////////////////////////////////////
{
}
/////////////////////////////////////////////////////////////////////////////////////
ArduinoTestManager::~ArduinoTestManager() {
/////////////////////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoTestManager::summerizeSpeed(float totalMM, uint32_t ts) {
/////////////////////////////////////////////////////////////////////////////////////
    const uint32_t totalSteps = round(totalMM / feedRate);
    const uint32_t timeDiff   = ArdoTs::timespan(ts);
    const float    mmMin      = (float)((totalMM * 60 * 1000 * 1000) / timeDiff);

    Serial.print("D=");   Serial.print((float)timeDiff / 1000 / 1000 ); Serial.print("[s]\n");
    Serial.print("R=");   Serial.print((float)timeDiff / totalSteps  ); Serial.print("[us/step]\n");
    Serial.print("F=");   Serial.print(mmMin);                          Serial.print("[mm/min]\n");
    Serial.print("RPM="); Serial.print(mmMin / resolution);             Serial.print("[rpm]\n");
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoTestManager::stepDist(uint32_t dist, bool x, bool y, bool z) {
/////////////////////////////////////////////////////////////////////////////////////
    const uint32_t tsBeg = ArdoTs::now();
    for ( uint32_t i=0; i<dist; i++) {
      if ( x ) controller->initiateStep(IDX_X);
      if ( y ) controller->initiateStep(IDX_Y);
      if ( z ) controller->initiateStep(IDX_Z);
      
      if ( x ) controller->finalizeStep(IDX_X);
      if ( y ) controller->finalizeStep(IDX_Y);
      if ( z ) controller->finalizeStep(IDX_Z);
    }

    if      ( x && y && z )    summerizeSpeed( 3 * sqrt(pow(dist * feedRate, 2)), tsBeg);
    else if ( x && y )         summerizeSpeed( 2 * sqrt(pow(dist * feedRate, 2)), tsBeg);
    else if ( x && z )         summerizeSpeed( 2 * sqrt(pow(dist * feedRate, 2)), tsBeg);
    else if ( y && z )         summerizeSpeed( 2 * sqrt(pow(dist * feedRate, 2)), tsBeg);
    else if ( x )              summerizeSpeed( 1 * sqrt(pow(dist * feedRate, 2)), tsBeg);
    else if ( y )              summerizeSpeed( 1 * sqrt(pow(dist * feedRate, 2)), tsBeg);
    else if ( z )              summerizeSpeed( 1 * sqrt(pow(dist * feedRate, 2)), tsBeg);
}


/////////////////////////////////////////////////////////////////////////////////////
byte ArduinoTestManager::performTest() {
/////////////////////////////////////////////////////////////////////////////////////
  #ifdef USE_CNC_TEST  
  
    byte b[1];
    if ( !ArduinoMainLoop::readSerialBytesWithTimeout(b, 1) )
      b[0] = 0;

    if ( controller == NULL )
      return RET_ERROR;  
  
    switch ( b[0] ) {
      case '1':   return performTestCase1();
      case '2':   return performTestCase1();
    }    
  #endif

  return RET_OK;
}

/////////////////////////////////////////////////////////////////////////////////////
byte ArduinoTestManager::performTestCase1() {
/////////////////////////////////////////////////////////////////////////////////////
  bool error = false;

  ArduinoCmdDecoderSetter::Result acc;
  acc.pid          = PID_ACCEL_PROFILE;
  acc.valueCount   = 12;
      
  acc.values[ 0].l = FLT_FACT * feedRate;    // steps x
  acc.values[ 1].l = FLT_FACT * feedRate;    // steps y
  acc.values[ 2].l = FLT_FACT * feedRate;    // steps z
  acc.values[ 3].l = FLT_FACT * 5L;          // pulse width x
  acc.values[ 4].l = FLT_FACT * 5L;          // pulse width y
  acc.values[ 5].l = FLT_FACT * 5L;          // pulse width z
  
  acc.values[ 6].l = FLT_FACT * 0.0;         // acc profile 
  acc.values[ 7].l = FLT_FACT * 0.1;         // acc profile 
  acc.values[ 8].l = FLT_FACT * 333.0 / 60;  // acc profile 
  acc.values[ 9].l = FLT_FACT * 0.0;         // acc profile 
  acc.values[10].l = FLT_FACT * 0.1;         // acc profile 
  acc.values[11].l = FLT_FACT * 333.0 / 60;  // acc profile 
  controller->setupAccelProfile(acc);
  
  controller->setProbeMode(OFF);
  controller->enableStepperPin(ENABLE_STATE_ON);
  controller->setSpeedValue_MMSec1000(1000L * 4500 / 60, true);
  controller->setPosReplyThreshold(333);
  
  const uint32_t dist = 700 / feedRate;
  
  ArduinoCmdDecoderMove::Result mv;
  mv.cmd = CMD_RENDER_AND_MOVE;
  mv.dx  = dist;
  mv.dy  = dist;
  mv.dz  = dist;
  
  const uint32_t tsBeg = ArdoTs::now();
  if ( controller->process(mv) != RET_OK )
    error = true;
  
  summerizeSpeed(sqrt(pow(mv.dx * feedRate, 2) + pow(mv.dy * feedRate, 2) + pow(mv.dz * feedRate, 2)), tsBeg);

  Serial.write("finished ");
  if ( error )  Serial.write("failed\n");
  else          Serial.write("succesfull\n");
              
  return error ? RET_ERROR : RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
byte ArduinoTestManager::performTestCase2() {
/////////////////////////////////////////////////////////////////////////////////////  
  bool error = false;

  controller->setProbeMode(OFF);
  controller->enableStepperPin(ENABLE_STATE_ON);
  controller->setPosReplyThreshold(33);

  controller->setHighPulseWidth(IDX_X, 5);
  controller->setDirection(IDX_X, 1);
  
  controller->setHighPulseWidth(IDX_Y, 5);
  controller->setDirection(IDX_Y, 1);
  
  controller->setHighPulseWidth(IDX_Z, 5);
  controller->setDirection(IDX_Z, 1);
  
  const uint32_t dist  = 3500 / feedRate;

  stepDist (dist, true, true,  true );
  stepDist (dist, true, true,  false);
  stepDist (dist, true, false, false);
 
  Serial.write("finished ");
  if ( error )  Serial.write("failed\n");
  else          Serial.write("succesfull\n");
              
  return error ? RET_ERROR : RET_OK;
}
