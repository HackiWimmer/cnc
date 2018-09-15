#ifndef COMMON_DEFINITIONS_H
#define COMMON_DEFINITIONS_H

    const int I2C_DEVICE_ID                     =       8;
    const unsigned int I2C_BYTE_COUNT           =       2;
    const unsigned int I2C_BYTE_LIMIT_STATE     =       0;
    const unsigned int I2C_BYTE_SUPPORT_STATE   =       1;
    
#ifdef SKETCH_COMPILE
  namespace {
  
    const char CR                               =    '\n';
    const char MINUS                            =     '-';
    const char EQUAL                            =     '=';
    const char HASH                             =     '#';
    const char COMMA                            =     ',';
    
    const char* EMPTY_TEXT_VAL                  =      "";
    const char* BLANK                           =     " ";
    
    const char* BLANK1                          =   BLANK;
    const char* BLANK2                          =    "  ";
    const char* BLANK3                          =   "   ";
  }
#endif

#endif
