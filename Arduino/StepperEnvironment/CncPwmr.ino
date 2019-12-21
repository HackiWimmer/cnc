#include "CncPwmr.h"

#ifndef SKETCH_COMPILE 
  #define CNC_PWM_LOG_FUNCTION()  \
    { \
        const wxString logStr(wxString::Format("PulsWidthManager::%s", __FUNCTION__)); \
        ARDO_DEBUG_MESSAGE('D', logStr); \
    }
#else
  #define CNC_PWM_LOG_FUNCTION()
#endif

/////////////////////////////////////////////////////////////////////////////////////
ArduinoPulsWidthManager::ArduinoPulsWidthManager()
: initialized(false)
/////////////////////////////////////////////////////////////////////////////////////
{
  #warning test only
  initialized = true;
}
/////////////////////////////////////////////////////////////////////////////////////
ArduinoPulsWidthManager::~ArduinoPulsWidthManager() {
/////////////////////////////////////////////////////////////////////////////////////  
}
/////////////////////////////////////////////////////////////////////////////////////  
void ArduinoPulsWidthManager::printPwmConfig() const {
/////////////////////////////////////////////////////////////////////////////////////  

  #warning impl. missing
}
/////////////////////////////////////////////////////////////////////////////////////  
void ArduinoPulsWidthManager::setupPwmManager() {
/////////////////////////////////////////////////////////////////////////////////////  
    #warning impl. missing
}