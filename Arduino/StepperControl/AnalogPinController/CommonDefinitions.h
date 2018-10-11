#ifndef COMMON_DEFINITIONS_H
#define COMMON_DEFINITIONS_H

    const int I2C_DEVICE_ID                     =       8;
    const unsigned int I2C_BYTE_COUNT           =       2;
    const unsigned int I2C_BYTE_LIMIT_STATE     =       0;
    const unsigned int I2C_BYTE_SUPPORT_STATE   =       1;
    
#ifdef SKETCH_COMPILE
  namespace {

    typedef unsigned char SHORT;

    // Additional Output Pins
    const SHORT PIN_SUPPORT_SWITCH1_STATE       =      8;
    const SHORT PIN_SUPPORT_SWITCH2_STATE       =      9;
    const SHORT PIN_SUPPORT_TOOL_POWERED        =     10;
    const SHORT PIN_SUPPORT_CABLE_CONNECTED     =     11;

    // Limit Input Pins
    const SHORT PIN_LS_Z_MIN                    =      2;
    const SHORT PIN_LS_Z_MAX                    =      3;
    const SHORT PIN_LS_Y_MIN                    =      4;
    const SHORT PIN_LS_Y_MAX                    =      5;
    const SHORT PIN_LS_X_MIN                    =      6;
    const SHORT PIN_LS_X_MAX                    =      7;

    // Support Input Pins
    const SHORT PIN_SS_SUPPORT_BUTTON_1_PRESSED =     A6;
    const SHORT PIN_SS_SUPPORT_BUTTON_2_PRESSED =     A7;
    const SHORT PIN_SS_SUPPORT_BUTTON_3_PRESSED =     12;
    const SHORT PIN_SS_PIN_5                    =     13;
    const SHORT PIN_SS_SUPPORT_SWITCH_1_PRESSED =     A0;
    const SHORT PIN_SS_SUPPORT_SWITCH_2_PRESSED =     A1;
    const SHORT PIN_SS_TOOL_POWERED             =     A2;
    const SHORT PIN_SS_CABLE_CONNECTED          =     A3;

    // Commands
    const SHORT CMD_RESET                       =     'r';

    // Further constants
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
