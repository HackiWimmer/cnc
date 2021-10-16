#include "CncAcmr.h"

#ifndef SKETCH_COMPILE 
  #define CNC_ACM_LOG_FUNCTION()  \
    { \
        const wxString logStr(wxString::Format("ArduinoAccelManager::%s", __FUNCTION__)); \
        ARDO_DEBUG_MESSAGE('D', logStr); \
    }
#else
  #define CNC_ACM_LOG_FUNCTION()
#endif

/////////////////////////////////////////////////////////////////////////////////////
namespace AccelMngParameter {
#ifdef SKETCH_COMPILE 
  
  template <class T>
  void print(unsigned char pid, T value, int8_t indent=1) {
    for (auto i=0; i<indent; i++) Serial.print(BLANK1); \
    Serial.print(pid);   Serial.print(TEXT_SEPARATOR); \
    Serial.print(value); Serial.write(TEXT_CLOSE);
  }
  
#else

  template <class T>
  void print(unsigned char pid, T value, int8_t indent=1) {
    ARDO_DEBUG_VALUE(wxString::Format("PID=%u", pid), value); \
  }
  
#endif  
};

/////////////////////////////////////////////////////////////////////////////////////
uint32_t ArduinoAccelManager::Setup::feedRate_FT[]        = {C_2_08, C_2_08, C_2_08, C_2_08, C_2_08, C_2_08, C_2_08, C_2_08};
uint32_t ArduinoAccelManager::Setup::feedRate_UM[]        = {0,      0,      0,      0,      0,      0,      0,      0};
uint32_t ArduinoAccelManager::Setup::feedRate_Master1000  = 0;
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////  
void ArduinoAccelManager::Function::initMinSpeedDelay() { 
/////////////////////////////////////////////////////////////////////////////////////  
  minSpeedDelay_US  = ( C_1000 > 0 ? 1000L * 1000 * Setup::feedRate_Master1000 / C_1000 : 0 );
  
  // this indicates a const speed and no acceleration
  stdRampWidth_IMPL = 0;
  relRampWidth_IMPL = 0;
}
/////////////////////////////////////////////////////////////////////////////////////  
void ArduinoAccelManager::Function::initTrgSpeedDelay(const uint32_t trgF1000_MMSEC) {
/////////////////////////////////////////////////////////////////////////////////////  
  // this indicates a const speed and no acceleration
  stdRampWidth_IMPL = 0;
  relRampWidth_IMPL = 0;
  
  if ( trgF1000_MMSEC > 0 ) {
    const uint32_t trgSpeedDelay_US = 1000L * 1000 * Setup::feedRate_Master1000 / trgF1000_MMSEC;
    if ( trgSpeedDelay_US < minSpeedDelay_US ) {
      // this indicates acceleration ( if minSpeedDelay_US != 0 )
      stdRampWidth_IMPL = ( minSpeedDelay_US == 0 ? 0 : rampWidth(trgF1000_MMSEC) );
      relRampWidth_IMPL = 0;
    }
  }
}
/////////////////////////////////////////////////////////////////////////////////////  
uint32_t ArduinoAccelManager::Function::rampWidth(uint32_t trgF1000_MMSEC) {
/////////////////////////////////////////////////////////////////////////////////////  
  if ( A_1000 == 0 ) {
    // linear function: F(impulse) = B * impulse + C
    return B_1000 ? ( trgF1000_MMSEC - C_1000 ) / B_1000 + 1 : C_1000;
  }
  else {
    // quadratic functions
    if ( B_1000 == 0 ) {
      // simple quadratic: F(impulse) = A * (impulse)^2 + C
      return sqrt( ( trgF1000_MMSEC - C_1000) / A_1000 ) + 1;
    }
    else {
      // quadratic
      const uint32_t p = B_1000 / A_1000 * 0.5;
      const uint32_t q = ( C_1000 - trgF1000_MMSEC ) / A_1000;
      const uint32_t s = sqrt( p * p - q );
      
      return ( s >= p ? -p + s : -p - s );
    }
  }
  
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////  
ArduinoAccelManager::ArduinoAccelManager() 
/////////////////////////////////////////////////////////////////////////////////////  
: initialized             (false)
, active                  (false)
, curState                (P_UNDEF)
, curType                 (T_MODEL_DRIVEN)
, interactiveFactor       (1)
, curSpeedDelay           (0)
, curImplIdx              (0)
, curImplCnt              (0)
, cfgSpeedDelay           (0)
, fA                      (0.0, 0.0, 0.0)
, fD                      (0.0, 0.0, 0.0)
{
}
/////////////////////////////////////////////////////////////////////////////////////  
ArduinoAccelManager::~ArduinoAccelManager() {
/////////////////////////////////////////////////////////////////////////////////////  
}
/////////////////////////////////////////////////////////////////////////////////////  
void ArduinoAccelManager::printAccelConfig() const {
/////////////////////////////////////////////////////////////////////////////////////  
  const int8_t indent = 2;  
  AccelMngParameter::print(PID_ACCEL_START_SPEED,    (float)fA.getC_1000() / 1000, indent);
  AccelMngParameter::print(PID_ACCEL_STOP_SPEED,     (float)fD.getC_1000() / 1000, indent);
  // Todo: add PIDS for fA, fD
}
/////////////////////////////////////////////////////////////////////////////////////  
void ArduinoAccelManager::initialize(const Function& fA, const Function& fD) {
/////////////////////////////////////////////////////////////////////////////////////  
  this->fA    = fA;
  this->fD    = fD;
  
  this->fA.initMinSpeedDelay();
  this->fD.initMinSpeedDelay();
  
  // the start speed must be greater then 0.0 because it's used as default value
  // and a valid feedRate has to exist
  initialized = ( fA.C_1000 > 0.0 && Setup::feedRate_Master1000 > 0);
  active    = initialized;
  
  changeState(P_UNDEF);
}
/////////////////////////////////////////////////////////////////////////////////////  
void ArduinoAccelManager::changeState(const State s) {
/////////////////////////////////////////////////////////////////////////////////////  
  if ( curState != s ) {
    curState = s;

    notifyACMStateChange(s); 

    //PRINT_DEBUG_VALUE1("s", s)

    #ifndef SKETCH_COMPILE 
      if ( true ) {
        switch ( s ) {
          case P_UNDEF:   ARDO_DEBUG_MESSAGE('D', wxString::Format("ArduinoAccelManager::changeState: to 'P_UNDEF';   Current impulse count: %ld", getCurrentImpulseCount())); break;
          case P_CONST:   ARDO_DEBUG_MESSAGE('D', wxString::Format("ArduinoAccelManager::changeState: to 'P_CONST';   Current impulse count: %ld", getCurrentImpulseCount())); break;
          case P_ACCEL:   ARDO_DEBUG_MESSAGE('D', wxString::Format("ArduinoAccelManager::changeState: to 'P_ACCEL';   Current impulse count: %ld", getCurrentImpulseCount())); break;
          case P_TARGET:  ARDO_DEBUG_MESSAGE('D', wxString::Format("ArduinoAccelManager::changeState: to 'P_TARGET';  Current impulse count: %ld", getCurrentImpulseCount())); break;
          case P_DEACCEL: ARDO_DEBUG_MESSAGE('D', wxString::Format("ArduinoAccelManager::changeState: to 'P_DEACCEL'; Current impulse count: %ld", getCurrentImpulseCount())); break;
        }
      }
    #endif  
  }
}
/////////////////////////////////////////////////////////////////////////////////////  
void ArduinoAccelManager::finalize() { 
/////////////////////////////////////////////////////////////////////////////////////  
  changeState(P_UNDEF);
  curType               = T_MODEL_DRIVEN; 
}
/////////////////////////////////////////////////////////////////////////////////////  
void ArduinoAccelManager::reset() {
/////////////////////////////////////////////////////////////////////////////////////  
  curState              = P_UNDEF;
  curType               = T_MODEL_DRIVEN; 
}
/////////////////////////////////////////////////////////////////////////////////////  
bool ArduinoAccelManager::initSpeed(uint32_t mF1000_MMSec) {
/////////////////////////////////////////////////////////////////////////////////////  
  fA.initTrgSpeedDelay(mF1000_MMSec);
  fD.initTrgSpeedDelay(mF1000_MMSec);

  cfgSpeedDelay = cnvSpeedToDelay(mF1000_MMSec);
 
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////  
bool ArduinoAccelManager::initMove(uint32_t mD_IMPL) {
/////////////////////////////////////////////////////////////////////////////////////  
  
  // --------------------------------------------------------------------------------
  // Init model driven movement
  if ( mD_IMPL > 0 ) {
    const uint32_t MIN_IMPL_COUNT = 3;
    const bool b = fA.stdRampWidth_IMPL > 0  && fD.stdRampWidth_IMPL > 0;
  
    curType     = T_MODEL_DRIVEN;
    curImplIdx  = 0;
    curImplCnt  = mD_IMPL;
    
    if ( b && curImplCnt >= MIN_IMPL_COUNT ) {
      
      if ( fA.stdRampWidth_IMPL + fD.stdRampWidth_IMPL < curImplCnt ) {
        fA.relRampWidth_IMPL = fA.stdRampWidth_IMPL;
        fD.relRampWidth_IMPL = fD.stdRampWidth_IMPL;
        changeState(P_ACCEL);
      }
      else {
        fA.relRampWidth_IMPL = curImplCnt / 2;
        fD.relRampWidth_IMPL = curImplCnt - fA.relRampWidth_IMPL;
        changeState(P_ACCEL);
      }
    }
    else {
      fA.relRampWidth_IMPL = 0;
      fD.relRampWidth_IMPL = 0;
      changeState(P_CONST);
    }
  }
  // --------------------------------------------------------------------------------
  // Init interactive movement
  else {
    curType     = T_INTERACTIVE;
    curImplIdx  = 0;
    curImplCnt  = 0;
    
    if ( fA.stdRampWidth_IMPL > 0 ) {
        fA.relRampWidth_IMPL = fA.stdRampWidth_IMPL;
        fD.relRampWidth_IMPL = fD.stdRampWidth_IMPL;
        changeState(P_ACCEL);
    }
    else {
      fA.relRampWidth_IMPL = 0;
      fD.relRampWidth_IMPL = 0;
      changeState(P_CONST);
    }

    #ifndef SKETCH_COMPILE 
      if ( false ) {
        ARDO_DEBUG_MESSAGE('D', wxString::Format("ArduinoAccelManager::initMove: fA.stdRampWidth_IMPL = %ld", fA.stdRampWidth_IMPL));
        ARDO_DEBUG_MESSAGE('D', wxString::Format("ArduinoAccelManager::initMove: fA.relRampWidth_IMPL = %ld", fA.relRampWidth_IMPL));
      }
    #endif  
  
  }
  // --------------------------------------------------------------------------------

  notifyACMInitMove();
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////  
bool ArduinoAccelManager::initMove(uint32_t mD_IMPL, uint32_t mF1000_MMSec) {
/////////////////////////////////////////////////////////////////////////////////////  
  if ( initSpeed(mF1000_MMSec) == false )
    return false;
  
  return initMove(mD_IMPL);
}
/////////////////////////////////////////////////////////////////////////////////////  
uint32_t ArduinoAccelManager::initNextImpulse(AxisSignatureIndex axisSignatureIdx) {
/////////////////////////////////////////////////////////////////////////////////////  
  if ( initialized != true ) 
    changeState(P_UNDEF);

  // interactive moves didn't have a curImplCnt value > 0  
  if ( curType == T_MODEL_DRIVEN && curImplIdx > curImplCnt )  
    changeState(P_UNDEF);

  if ( active == false ) 
    return cfgSpeedDelay;

  uint32_t ret = 0;
  switch ( curState ) {
    
    // ------------------------------------------------------------------------------
    case P_ACCEL: 
    {
      ret = cnvSpeedToDelay( fA.fctValue1000(curImplIdx) );

      if (curType == T_INTERACTIVE )
        ret /= (interactiveFactor);
      
      curImplIdx++;
      
      // state machine handling
      if ( curImplIdx >= fA.relRampWidth_IMPL ) {
        // if the rest amount of steps is smaller then the deacceleration ramp witdh
        // switch directly to P_DEACCEL
        // -1 to avoid problems raised by rounding
        // Further, interactive moves didn't have a curImplCnt value > 0  
        const bool b = curImplIdx + fD.relRampWidth_IMPL >= curImplCnt - 1;
        
        // interactive moves don't reach the mode P_DEACCEL
        changeState( b && curType == T_MODEL_DRIVEN ? P_DEACCEL : P_TARGET );
      }
      
      break;
    }
    // ------------------------------------------------------------------------------
    case P_TARGET: 
    {   
      ret = cfgSpeedDelay;
      curImplIdx++;
      
      if (curType == T_INTERACTIVE )
         ret /= (interactiveFactor);
        
      // state machine handling
      // interactive moves stay in mode P_TARGET until they stops
      if ( curType == T_MODEL_DRIVEN ) {
        if ( curImplIdx > curImplCnt - fD.relRampWidth_IMPL )
          changeState(P_DEACCEL);
      }
      
      break;
    }
    // ------------------------------------------------------------------------------
    case P_DEACCEL:
    {
      curImplIdx++;
      // Note: Due to the unsigned characteristic of curImplIdx and curImplCnt they have to be 
      // casted to a signed value to avoid overflows
      // ArdoObj::absolute: this with respect to get a falling curve here
      ret = cnvSpeedToDelay(fD.fctValue1000( ArdoObj::absolute((int32_t)curImplIdx - (int32_t)curImplCnt)) );
      
      // state machine handling
      if ( curImplIdx > curImplCnt )
        finalize();
      
      break;
    }
    // ------------------------------------------------------------------------------
    case P_CONST:
    {
      ret = cfgSpeedDelay;
      break;
    }
    // ------------------------------------------------------------------------------
    case P_UNDEF:
    default:
    {
      ret = getDefaultDelay();
    }
  }

  // shift the master feed rate to the concrete movement situation
  // Setup::feedRate_FT[axisSignatureIdx] stored with factor C_2_08
  curSpeedDelay = ( ret * Setup::feedRate_FT[axisSignatureIdx] ) >> 8;

  return curSpeedDelay;
}
