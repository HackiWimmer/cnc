#ifndef COMMON_DEFINITIONS_H
#define COMMON_DEFINITIONS_H

namespace {

  const char CR                        =    '\n';
  const char MINUS                     =     '-';
  
  const char* EMPTY_TEXT_VAL           =      "";
  const char* BLANK                    =     " ";
  
  const char* BLANK1                   =   BLANK;
  const char* BLANK2                   =    "  ";
  const char* BLANK3                   =   "   ";
  const char* BLANK4                   =  "    ";
}

namespace LimitSwitch {
  
  const bool LIMIT_SWITCH_ON          =      LOW;
  const bool LIMIT_SWITCH_OFF         =     HIGH;
      
  const long LIMIT_MIN                =      -1L;
  const long LIMIT_MAX                =      +1L;
  const long LIMIT_UNKNOWN            =    +256L;
  const long LIMIT_UNSET              =       0L;
  
};

#endif
