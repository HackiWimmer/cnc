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
  
  template <class T>
  void print(unsigned char pid, T value, int8_t indent=1) {
    for (auto i=0; i<indent; i++) Serial.print(BLANK1); \
    Serial.print(pid);   Serial.print(TEXT_SEPARATOR); \
    Serial.print(value); Serial.write(TEXT_CLOSE);
  }
};
/////////////////////////////////////////////////////////////////////////////////////
ArduinoAccelManager::ArduinoAccelManager()
: initialized     (false)
, active          (false)
, curState        (P_UNDEF)
, fA              ()
, fD              ()
, aRampWidth_IMPL (-1.0)
, dRampWidth_IMPL (-1.0)
, cD_IMPL         (0)
, iD_IMPL         (0)
, cF_MMSec        (0.0)
/////////////////////////////////////////////////////////////////////////////////////
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
  AccelMngParameter::print(PID_ACCEL_START_SPEED,    fA.C, indent);
  AccelMngParameter::print(PID_ACCEL_STOP_SPEED,     fD.C, indent);
  // Todo: add PIDS for fA, fD
}
/////////////////////////////////////////////////////////////////////////////////////  
void ArduinoAccelManager::setupAccelManager(const Function& fA, const Function& fD) {
/////////////////////////////////////////////////////////////////////////////////////  
  this->fA         = fA;
  this->fD         = fD;

  // the start speed must be greater then 0.0 because it's used as default value
  initialized = ( fA.C > 0.0 );
  active      = initialized;
  changeState(P_CONST);
}
/////////////////////////////////////////////////////////////////////////////////////  
void ArduinoAccelManager::changeState(const State s) {
/////////////////////////////////////////////////////////////////////////////////////  
  if ( curState != s ) {
    curState = s;
    notifyACMStateChange(s); 
  }
}
/////////////////////////////////////////////////////////////////////////////////////  
bool ArduinoAccelManager::initMove(uint32_t mD_IMPL, float mF_MMSec) {
/////////////////////////////////////////////////////////////////////////////////////  
  if ( initialized != true )  return false;
  if ( mD_IMPL     == 0    )  return false;
  if ( mF_MMSec    == 0    )  return false;

  const uint8_t minImpulseCount = 32;
  State state     = P_CONST;
  iD_IMPL         = 1;
  cD_IMPL         = mD_IMPL;
  cF_MMSec        = fA.defValue();
  aRampWidth_IMPL = mD_IMPL / 2;
  dRampWidth_IMPL = mD_IMPL / 2;

  // if there is a minimum amount of steps and 
  // the configured speed is greater then the start speed
  if ( cD_IMPL > minImpulseCount && mF_MMSec > fA.C ) { 

    aRampWidth_IMPL = fA.rampWidth(mF_MMSec);
    dRampWidth_IMPL = fD.rampWidth(mF_MMSec);

    // reduce the accelertion and deaccelartion ramp width until 
    // they are fit into the total impulse distance - on demand
    if ( aRampWidth_IMPL > 0.0 && dRampWidth_IMPL > 0.0 ) {
      
      while ( (uint32_t)(aRampWidth_IMPL + dRampWidth_IMPL) > cD_IMPL ) {

        const float ratio = dRampWidth_IMPL ? aRampWidth_IMPL/dRampWidth_IMPL : 1;

        for ( int i=0; i<ratio;   i++) aRampWidth_IMPL--;
        for ( int i=0; i<1/ratio; i++) dRampWidth_IMPL--;
    
        if ( aRampWidth_IMPL <= minImpulseCount / 2 || dRampWidth_IMPL <= minImpulseCount / 2 )
          break;
      }
    }

    if ( aRampWidth_IMPL >= minImpulseCount / 2 && dRampWidth_IMPL >= minImpulseCount / 2 ) {
      state       = P_ACCEL;
      cF_MMSec    = mF_MMSec;
    }
  }

  if ( true ) {
    ARDO_DEBUG_VALUE("ArduinoAccelManager: Total impulse count", cD_IMPL)
    ARDO_DEBUG_VALUE("ArduinoAccelManager: Accel ramp         ", aRampWidth_IMPL)
    ARDO_DEBUG_VALUE("ArduinoAccelManager: Deaccel ramp       ", dRampWidth_IMPL)
  }

  changeState(state);
  notifyACMInitMove();
  
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////  
float ArduinoAccelManager::getNextTargetSpeed_MMSec() {
/////////////////////////////////////////////////////////////////////////////////////  
  if ( initialized   != true    )  changeState(P_UNDEF);
  if ( iD_IMPL        > cD_IMPL )  changeState(P_UNDEF);

  if ( active == false ) 
    return cF_MMSec;

  float ret = 0.0;
  switch ( curState ) {
    // ------------------------------------------------------------------------------
    case P_ACCEL: 
    {
      ret = fA.fctValue(iD_IMPL);
      iD_IMPL++;
      
      // state machine handling
      if ( iD_IMPL > aRampWidth_IMPL ) {
        // if the rest amount of steps is smaller then the deacceleration ramp witdh
        // switch directly to P_DEACCEL
        if ( iD_IMPL + dRampWidth_IMPL >= cD_IMPL ) changeState(P_DEACCEL);
        else                                        changeState(P_TARGET);
      }
        
      break;
    }
    // ------------------------------------------------------------------------------
    case P_TARGET: 
    {   
      ret = cF_MMSec;
      iD_IMPL++;
      
      // state machine handling
      if ( iD_IMPL > cD_IMPL - dRampWidth_IMPL )
        changeState(P_DEACCEL);
        
      break;
    }
    // ------------------------------------------------------------------------------
    case P_DEACCEL:
    {
      // Note: Due to the unsigned characteristic of idxS_ST and curS_ST they have to be 
      // casted to a signed value to avoid overflows
      // ArdoObj::absolute: this with respect to get a falling curve here

      ret = fD.fctValue(ArdoObj::absolute((int32_t)iD_IMPL - (int32_t)cD_IMPL));
      iD_IMPL++;

      // state machine handling
      if ( iD_IMPL > cD_IMPL ) {
        #warning
        ARDO_DEBUG_VALUE("ArduinoAccelManager: Current impulse index", iD_IMPL)  
        changeState(P_UNDEF);
      }

      break;
    }
    // ------------------------------------------------------------------------------
    case P_CONST:
    {
      ret = cF_MMSec;
      break;
    }
    case P_UNDEF:
    default:
    {
      ret = fA.defValue();
    }
  }

  // check bounderies
  ret = ret > cF_MMSec ? cF_MMSec : ret;
  ret = ret < fA.C     ? fA.C     : ret;

  return ret;
}

#ifndef SKETCH_COMPILE 
/////////////////////////////////////////////////////////////////////////////////////  
void ArduinoAccelManager::traceACM(std::ostream& o, int indent) {
/////////////////////////////////////////////////////////////////////////////////////  
  std::string prefix(indent, ' ');
  o << prefix << "ACM.aF.A                = " << fA.A             << " [fact]"      << std::endl;
  o << prefix << "ACM.aF.B                = " << fA.B             << " [fact]"      << std::endl;
  o << prefix << "ACM.aF.C                = " << fA.C             << " [mm/sec]"    << std::endl;
  o << prefix << "ACM.dF.A                = " << fD.A             << " [fact]"      << std::endl;
  o << prefix << "ACM.dF.B                = " << fD.B             << " [fact]"      << std::endl;
  o << prefix << "ACM.dF.C                = " << fD.C             << " [mm/sec]"    << std::endl;

  o << prefix << "ACM.aRampWidth          = " << aRampWidth_IMPL  << " [impulses]"  << std::endl;
  o << prefix << "ACM.dRampWidth          = " << dRampWidth_IMPL  << " [impulses]"  << std::endl;
  o << prefix << "ACM.cD_IMPL             = " << cD_IMPL          << " [impulses]"  << std::endl;

  o << prefix << "ACM.cF_MMSec            = " << cF_MMSec         << " [mm/sec]"    << std::endl;
}
#endif
