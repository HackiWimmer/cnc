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
: initialized           (false)
, active                (false)
, curState              (P_UNDEF)
, fA                    ()
, fD                    ()
, aRampWidth_IMPL       (-1.0)
, dRampWidth_IMPL       (-1.0)
, cD_IMPL               (0)
, iD_IMPL               (0)
, cF_MMSec              (0.0)
, currentTargetF_MMSec  (0.0)

#ifndef SKETCH_COMPILE 
  , cFT1_IMPL             (0)
  , cFT2_IMPL             (0)
  , cFT3_IMPL             (0)
  , cFT1_MMSec            (0.0)
  , cFT2_MMSec            (0.0)
  , cFT3_MMSec            (0.0)
#endif

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
void ArduinoAccelManager::setup(const Function& fA, const Function& fD) {
/////////////////////////////////////////////////////////////////////////////////////  
  this->fA         = fA;
  this->fD         = fD;

  // the start speed must be greater then 0.0 because it's used as default value
  initialized = ( fA.C > 0.0 );
  active      = initialized;
  
  changeState(P_UNDEF);
}
/////////////////////////////////////////////////////////////////////////////////////  
void ArduinoAccelManager::changeState(const State s) {
/////////////////////////////////////////////////////////////////////////////////////  
  if ( curState != s ) {
    curState = s;
    notifyACMStateChange(s); 

    if ( true ) {
      switch ( s ) {
        case P_UNDEF:   ARDO_DEBUG_MESSAGE('D', wxString::Format("ArduinoAccelManager::changeState: to 'P_UNDEF';   Current impulse count: %ld", getCurrentImpulseCount())); break;
        case P_CONST:   ARDO_DEBUG_MESSAGE('D', wxString::Format("ArduinoAccelManager::changeState: to 'P_CONST';   Current impulse count: %ld", getCurrentImpulseCount())); break;
        case P_ACCEL:   ARDO_DEBUG_MESSAGE('D', wxString::Format("ArduinoAccelManager::changeState: to 'P_ACCEL';   Current impulse count: %ld", getCurrentImpulseCount())); break;
        case P_TARGET:  ARDO_DEBUG_MESSAGE('D', wxString::Format("ArduinoAccelManager::changeState: to 'P_TARGET';  Current impulse count: %ld", getCurrentImpulseCount())); break;
        case P_DEACCEL: ARDO_DEBUG_MESSAGE('D', wxString::Format("ArduinoAccelManager::changeState: to 'P_DEACCEL'; Current impulse count: %ld", getCurrentImpulseCount())); break;
      }
    }
  }
}
/////////////////////////////////////////////////////////////////////////////////////  
bool ArduinoAccelManager::initMove(uint32_t mD_IMPL, float mF_MMSec) {
/////////////////////////////////////////////////////////////////////////////////////  
  const uint8_t minImpulseCount = 32;

  if ( initialized != true )  return false;
  if ( mD_IMPL     == 0    )  return false;
  if ( mF_MMSec    == 0    )  return false;

  if ( false ) {
    ARDO_DEBUG_MESSAGE('S', "ArduinoAccelManager::initMove previous parameters:")
    ARDO_DEBUG_VALUE("ArduinoAccelManager: curState           ", getStateAsString(curState))
    ARDO_DEBUG_VALUE("ArduinoAccelManager: Total impulse count", cD_IMPL)
    ARDO_DEBUG_VALUE("ArduinoAccelManager: iD_IMPL            ", iD_IMPL)
    ARDO_DEBUG_VALUE("ArduinoAccelManager: cF_MMSec           ", cF_MMSec)
    ARDO_DEBUG_VALUE("ArduinoAccelManager: mF_MMSec           ", mF_MMSec)
    ARDO_DEBUG_VALUE("ArduinoAccelManager: Accel   c()        ", fA.c())
    ARDO_DEBUG_VALUE("ArduinoAccelManager: Deaccel c()        ", fD.c())
    ARDO_DEBUG_VALUE("ArduinoAccelManager: Accel   ramp       ", aRampWidth_IMPL)
    ARDO_DEBUG_VALUE("ArduinoAccelManager: Deaccel ramp       ", dRampWidth_IMPL)
  }

  State newState  = P_UNDEF;
  iD_IMPL         = 1;
  cD_IMPL         = mD_IMPL;
  cF_MMSec        = fA.defValue();
  aRampWidth_IMPL = mD_IMPL / 2;
  dRampWidth_IMPL = mD_IMPL / 2;


  fA.refValue = mF_MMSec;
  fA.cSlider  = curState == P_UNDEF ? 0.0 : currentTargetF_MMSec - fA.C;
  fD.refValue = fD.C;
  fD.cSlider  = 0.0;


  auto constSetup = [&]() {
    newState = curState == P_UNDEF ? P_CONST : P_ACCEL;
    cF_MMSec = mF_MMSec;
  };


  if ( curState == P_UNDEF && mF_MMSec <= fA.c() ) {
    ARDO_DEBUG_MESSAGE('D', "1")
    constSetup();
  }
  else {
    
    // if there is a minimum amount of steps 
    if ( cD_IMPL > minImpulseCount ) { 
  
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
        newState    = P_ACCEL;
        cF_MMSec    = mF_MMSec;
        
      } else {
        ARDO_DEBUG_MESSAGE('D', "2")
        constSetup();
      }
    } 
    else {
      ARDO_DEBUG_MESSAGE('D', "1")
      constSetup();
    }
    
  }
  
  if ( true ) {
    ARDO_DEBUG_MESSAGE('S', "ArduinoAccelManager::initMove resolved parameters:")
    ARDO_DEBUG_VALUE("ArduinoAccelManager: newState           ", getStateAsString(newState))
    ARDO_DEBUG_VALUE("ArduinoAccelManager: Total impulse count", cD_IMPL)
    ARDO_DEBUG_VALUE("ArduinoAccelManager: iD_IMPL            ", iD_IMPL)
    ARDO_DEBUG_VALUE("ArduinoAccelManager: cF_MMSec           ", cF_MMSec)
    ARDO_DEBUG_VALUE("ArduinoAccelManager: mF_MMSec           ", mF_MMSec)
    ARDO_DEBUG_VALUE("ArduinoAccelManager: Accel   c()        ", fA.c())
    ARDO_DEBUG_VALUE("ArduinoAccelManager: Deaccel c()        ", fD.c())
    ARDO_DEBUG_VALUE("ArduinoAccelManager: Accel   ramp       ", aRampWidth_IMPL)
    ARDO_DEBUG_VALUE("ArduinoAccelManager: Deaccel ramp       ", dRampWidth_IMPL)
  }

  changeState(newState);
  notifyACMInitMove();
  
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////  
bool ArduinoAccelManager::updateSpeedConfig(float mF_MMSec) {
/////////////////////////////////////////////////////////////////////////////////////  
  if ( initialized != true )  return false;
  if ( mF_MMSec    == 0    )  return false;

  ARDO_DEBUG_MESSAGE('S', "ArduinoAccelManager::updateSpeedConfig:")

  switch ( curState ) {
    // ------------------------------------------------------------------------------
    case P_CONST:
    case P_ACCEL: 
    case P_TARGET: 
    {   
      // no change, nothing to do leave unchanged
      if ( ArduinoMainLoop::fltCompare(currentTargetF_MMSec, mF_MMSec) == true )
        break;

      // 
      if ( cD_IMPL - iD_IMPL <= 0 )
        break;
     
      initMove(cD_IMPL - iD_IMPL + 1, mF_MMSec);
      break;
    }
    // ------------------------------------------------------------------------------
    case P_DEACCEL:
    {
      // to late, nothing to do leave unchanged
      break;
    }
    // ------------------------------------------------------------------------------
    case P_UNDEF:
    default:
    {
      // generally nothing to do leave unchanged
    }
  }
      
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////  
float ArduinoAccelManager::initNextTargetSpeed_MMSec() {
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
      if ( iD_IMPL >= aRampWidth_IMPL ) {
        // if the rest amount of steps is smaller then the deacceleration ramp witdh
        // switch directly to P_DEACCEL
        // -1 to avoid problems raised by rounding
        if ( iD_IMPL + dRampWidth_IMPL >= cD_IMPL - 1 ) changeState(P_DEACCEL);
        else                                            changeState(P_TARGET);
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
    // ------------------------------------------------------------------------------
    case P_UNDEF:
    default:
    {
      ret = fA.defValue();
    }
  }

  // check bounderies
  ret = ret < 0        ? fA.C     : ret;

  currentTargetF_MMSec = ret;

  // *********************************************************************************
  // for testing only
  #ifndef SKETCH_COMPILE 
    if  ( cFT1_IMPL > 0 && cFT1_MMSec > 0 && cFT1_IMPL == iD_IMPL) { 
      if ( cFT2_IMPL > 0 )
        cFT2_IMPL -= cFT1_IMPL;
        
      if ( cFT3_IMPL > 0 )
        cFT3_IMPL -= cFT1_IMPL;
        
      updateSpeedConfig(cFT1_MMSec); 
      cFT1_IMPL  = 0; 
      cFT1_MMSec = 0.0; 
    }
    
    if ( cFT2_IMPL > 0 && cFT2_MMSec > 0 && cFT2_IMPL == iD_IMPL) { 
      if ( cFT3_IMPL > 0 )
        cFT3_IMPL -= cFT2_IMPL;
      
      updateSpeedConfig(cFT2_MMSec); 
      cFT2_IMPL = 0; 
      cFT2_MMSec = 0.0; 
    }
    
    if ( cFT3_IMPL > 0 && cFT3_MMSec > 0 && cFT3_IMPL == iD_IMPL) { 
      updateSpeedConfig(cFT3_MMSec); 
      cFT3_IMPL = 0; 
      cFT3_MMSec = 0.0; 
    }
  #endif
  // *********************************************************************************
  
  return ret;
}

#ifndef SKETCH_COMPILE 
/////////////////////////////////////////////////////////////////////////////////////  
const char* ArduinoAccelManager::getStateAsString(State s) {
/////////////////////////////////////////////////////////////////////////////////////  
  switch ( s ) {
    case P_UNDEF:   return "P_UNDEF";
    case P_CONST:   return "P_CONST";
    case P_ACCEL:   return "P_ACCEL";
    case P_TARGET:  return "P_TARGET";
    case P_DEACCEL: return "P_DEACCEL";
  }

  return "Unkonwn ArduinoAccelManager State";
}
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
