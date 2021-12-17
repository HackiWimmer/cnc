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

  AE::digitalWrite(PIN_ENABLE_PODIUM_A, es[A]);
  AE::digitalWrite(PIN_ENABLE_PODIUM_B, es[B]);
  AE::digitalWrite(PIN_ENABLE_PODIUM_C, es[C]);
  AE::digitalWrite(PIN_ENABLE_PODIUM_D, es[D]);

  // CL57T 4.0: "ENA signal requires advance DIR signal minimum 200ms in single pulse mode"
  // 
  // Experiments have shown that 500 ms isn't long enough to be definitly save here. 
  // If the period is to short steps will be lost without any error information. 
  // It's very importent to wait long enough here!!!
  AE::delay(800);
  
  return true;
}
