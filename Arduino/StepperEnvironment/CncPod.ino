#include "CncPod.h"


/////////////////////////////////////////////////////////////////////////////////////
ArduinoPodiumManager::ArduinoPodiumManager()
: curMode   (M_CornerABCD)
/////////////////////////////////////////////////////////////////////////////////////
{
}
/////////////////////////////////////////////////////////////////////////////////////
ArduinoPodiumManager::~ArduinoPodiumManager() {
/////////////////////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////////////////////
bool ArduinoPodiumManager::enable(Mode m) {
/////////////////////////////////////////////////////////////////////////////////////
  curMode = m;

  bool es[4];
  const short A = 0, B = 1, C = 2, D = 3;
  es[A] = es[B] = es[C] = es[D] = ENABLE_STATE_OFF;

  switch ( curMode )
  {
    case M_Off:                                                          break;                
    case M_CornerA:    es[A] = ENABLE_STATE_ON;                          break;
    case M_CornerB:    es[B] = ENABLE_STATE_ON;                          break;
    case M_CornerC:    es[C] = ENABLE_STATE_ON;                          break;
    case M_CornerD:    es[D] = ENABLE_STATE_ON;                          break;
    case M_CornerABCD: es[A] = es[B] = es[C] = es[D] = ENABLE_STATE_ON;  break;
  }

  #warning change this to real four pins
  const char pinEnableStepperA = PIN_ENABLE_PODIUM;
  const char pinEnableStepperB = PIN_ENABLE_PODIUM;
  const char pinEnableStepperC = PIN_ENABLE_PODIUM;
  const char pinEnableStepperD = PIN_ENABLE_PODIUM;
  
  AE::digitalWrite(pinEnableStepperA, es[A]);
  AE::digitalWrite(pinEnableStepperB, es[B]);
  AE::digitalWrite(pinEnableStepperC, es[C]);
  AE::digitalWrite(pinEnableStepperD, es[D]);
      
  AE::delayMicroseconds(100);
  return true;
}
