#ifndef COMMON_VALUES_H
#define COMMON_VALUES_H

#ifndef SKETCH_COMPILE   
  #include <iostream>
  typedef unsigned char byte;  
#endif

// .....................................................................
// Pin setup
// .....................................................................
#include "ArdoPin.h"

// .....................................................................
// 
// .....................................................................
#include "AnalogPinController/CommonDefinitions.h"
#include "AnalogPinController/CncInterface.h"
  
// .....................................................................
// Signals
// .....................................................................

  const unsigned char SIG_INTERRUPPT                      =  'I';
  const unsigned char SIG_HALT                            =  'H';
  const unsigned char SIG_PAUSE                           =  'P';
  const unsigned char SIG_RESUME                          =  'p';
  const unsigned char SIG_QUIT_MOVE                       =  'q';
  const unsigned char SIG_UPDATE                          =  'U';

  const unsigned char SIG_SOFTWARE_RESET                  =  '\\';

// .....................................................................
// Commands
// .....................................................................

  const unsigned char MAX_CMDS                            =  255;
  const unsigned char CMD_INVALID                         = '\0'; 
  const unsigned char CMD_POP_SERIAL                      = '~'; 

  const unsigned char CMD_IDLE                            =  'i'; // 105 0x69
  const unsigned char CMD_HEARTBEAT                       =  'h'; // 104 0x68
  
  const unsigned char CMD_RESET_CONTROLLER                =  'R'; //  82 0x52
    
  const unsigned char CMD_PUSH_TRANSACTION                =  '('; //  40 0x28
  const unsigned char CMD_POP_TRANSACTION                 =  ')'; //  41 0x29
  
  const unsigned char CMD_SETTER                          =  'S'; //  83 0x53
  const unsigned char CMD_GETTER                          =  'G'; //  71 0x47
  
  const unsigned char CMD_MOVE                            =  'm'; // 155 0x9B
  const unsigned char CMD_RENDER_AND_MOVE                 =  'M'; //  77 0x4D

  const unsigned char CMD_MOVE_SEQUENCE                   =  '.'; //  46 0x2E
  const unsigned char CMD_RENDER_AND_MOVE_SEQUENCE        =  ':'; //  58 0x3A

  const unsigned char CMD_MOVE_UNTIL_CONTACT              =  '+'; //  43 0x2B
  const unsigned char CMD_MOVE_UNTIL_LIMIT_IS_FREE        =  '>'; //  62 0x3E

  const unsigned char CMD_MOVE_INTERACTIVE                =  '%'; //  37 0x25

  const unsigned char CMD_ACTIVATE_PODEST_HW              =  '['; //  91 0x5B
  const unsigned char CMD_DEACTIVATE_PODEST_HW            =  ']'; //  93 0x5E
  const unsigned char CMD_MOVE_PODEST                     =  '$'; //  36 0x24
 
  const unsigned char CMD_PRINT_CONFIG                    =  'c'; //  99 0x63
  const unsigned char CMD_PRINT_TIMESTAMP                 =  'T'; //  84 0x54
  const unsigned char CMD_PRINT_VERSION                   =  'V'; //  86 0x56
  const unsigned char CMD_PRINT_PIN_REPORT                =  'Q'; //  81 0x51
    
  const unsigned char CMD_PERFORM_TEST                    =  't'; // 116 0x74
  
  // Ranges reserved via above definitions:
  //   48 -  57 --> '0'..'9'
  //   65 -  90 --> 'A'..'Z'
  //   97 - 122 --> 'a'..'z'
  //
  // start numbered definition from 129 on (above 128)

// .....................................................................
// Separator ids
// .....................................................................

  const unsigned char SEPARARTOR_SETUP                    =   1;
  const unsigned char SEPARARTOR_RESET                    =   2;
  const unsigned char SEPARARTOR_RUN                      =   3;

// .....................................................................
// Parameter Ids - boundings
// .....................................................................

  const unsigned char MAX_PIDS                            = 255;
  const unsigned char PID_UNKNOWN                         = 241;
  
  const unsigned char PID_RESERVED_RANGE_START            =   1;
  const unsigned char PID_RESERVED_RANGE_END              =  19;
  const unsigned char PID_LONG_RANGE_START                =  20;
  const unsigned char PID_LONG_RANGE_END                  = 199;
  const unsigned char PID_FLOAT_RANG_START                = 200;
  const unsigned char PID_FLOAT_RANG_END                  = 240;
  
// .....................................................................
// PIDs
//
// Please consider
// PID 1 ... 19 are reserved for special commands
// .....................................................................

  const unsigned char RET_MBYTE_CLOSE                     =   0;   // reserved - see also MBYTE_CLOSE
  const unsigned char RET_NULL                            =   1;   // Invalid ret code
  const unsigned char RET_SOH                             =   2;   // Start of header
  const unsigned char RET_MORE                            =   3;   // 
  const unsigned char RET_OK                              =   4;   // Ack
  const unsigned char RET_ERROR                           =   5;   // Nack
  const unsigned char RET_LIMIT                           =   6;   //
  const unsigned char RET_INTERRUPT                       =   7;   // 
  const unsigned char RET_HALT                            =   8;   // 
  const unsigned char RET_QUIT                            =   9;   // 
// .....................................................................
// Please consider
//
// PID_LONG_RANG_START ... PID_LONG_RANG_END 
// are reserved for long values
//
// .....................................................................
// start long pid range
// .....................................................................

  const unsigned char PID_VERSION                         =  20;
  const unsigned char PID_HEARTBEAT                       =  21;
  const unsigned char PID_TEXT                            =  22;
  const unsigned char PID_MSG                             =  23;
  const unsigned char PID_GETTER                          =  24;
  const unsigned char PID_STEPPER_INITIALIZED             =  25;
  const unsigned char PID_SEPARATOR                       =  26;
  const unsigned char PID_QUERY_READY_TO_RUN              =  27;
  
  const unsigned char PID_COMMON                          =  30;
  const unsigned char PID_CONTROLLER                      =  31;
  const unsigned char PID_SETUP_ID                        =  32;
  const unsigned char PID_AXIS                            =  33;
  const unsigned char PID_ACCEL_START_SPEED               =  34;
  const unsigned char PID_ACCEL_STOP_SPEED                =  35;
     
  const unsigned char PID_MIN_SWITCH                      =  40;
  const unsigned char PID_MAX_SWITCH                      =  41;
  const unsigned char PID_LIMIT                           =  42;
  const unsigned char PID_X_LIMIT                         =  43;
  const unsigned char PID_Y_LIMIT                         =  44;
  const unsigned char PID_Z_LIMIT                         =  45;
  const unsigned char PID_I2C_AVAILABEL                   =  46;
  const unsigned char PID_I2C_LIMIT_VALUE                 =  47;
  const unsigned char PID_I2C_SUPPORT_VALUE               =  48;
 
  const unsigned char PID_TOOL_SWITCH                     =  50;
  const unsigned char PID_POS_REPLY_THRESHOLD             =  51;
  const unsigned char PID_MAX_DIMENSION_X                 =  52;
  const unsigned char PID_MAX_DIMENSION_Y                 =  53;
  const unsigned char PID_MAX_DIMENSION_Z                 =  54;
  const unsigned char PID_TOUCH_CONTACT_STATE             =  55;
  const unsigned char PID_SPINDLE_SPEED                   =  56;

  const unsigned char PID_XYZ_INTERACTIVE_POS             =  60;
  const unsigned char PID_XYZ_POS                         =  61;
  const unsigned char PID_XYZ_POS_MAJOR                   =  62;
  const unsigned char PID_XYZ_POS_DETAIL                  =  63;
  const unsigned char PID_XY_POS                          =  64;
  const unsigned char PID_XZ_POS                          =  65;
  const unsigned char PID_YZ_POS                          =  66;
  const unsigned char PID_X_POS                           =  67;
  const unsigned char PID_Y_POS                           =  68;
  const unsigned char PID_Z_POS                           =  69;

  const unsigned char PID_STEP_PIN                        =  70;
  const unsigned char PID_DIR_PIN                         =  71;
  const unsigned char PID_ENABLE_STEPPERS                 =  72;

  const unsigned char PID_INC_DIRECTION_VALUE_X           =  80;
  const unsigned char PID_INC_DIRECTION_VALUE_Y           =  81;
  const unsigned char PID_INC_DIRECTION_VALUE_Z           =  82;
  const unsigned char PID_INC_DIRECTION_VALUE_H           =  83;

  const unsigned char PID_PULSE_WIDTH_HIGH                = 100;
  const unsigned char PID_PROBE_MODE                      = 101;
  
// .....................................................................
// end long pid range [PID_FLOAT_RANG_END] 
// .....................................................................

// .....................................................................
// Please consider
//
// PID_FLOAT_RANG_START ... PID_FLOAT_RANG_END 
//
// are reserved for double values
//
// .....................................................................
// start float pid range
// .....................................................................

  const unsigned char PID_FEEDRATE                        = 200;
  const unsigned char PID_SPEED_MM_MIN                    = 201;
  const unsigned char PID_SPEED_MM_SEC                    = 202;
  const unsigned char PID_ACCEL_PROFILE                   = 203;
  

// .....................................................................
// end float pid range
// .....................................................................

  const unsigned char RET_SENT                            = 255;   // Pseudo ret code for sent data, only reserved and not used by the Arduino

// .....................................................................
// Test Ids
// .....................................................................

  const unsigned char TID_INVALID                         =   0;
  const unsigned char TID_HEARTBEAT                       =  10;
  const unsigned char TID_MOTOR_CFG_INTERVAL              =  11;
  const unsigned char TID_DURATION                        =  12;

// .....................................................................
// Message types
// .....................................................................

  const unsigned char MT_MID_FLAG                         =  '#';
  
  const unsigned char MT_INFO                             =  'I';
  const unsigned char MT_WARNING                          =  'W';
  const unsigned char MT_ERROR                            =  'E';
  const unsigned char MT_DEBUG                            =  'D';

// .....................................................................
// Error codes
// .....................................................................

  const unsigned char MAX_ERROR_CODES                     = 255;
  const unsigned char E_NO_ERROR                          =   0;
  const unsigned char E_UNKNOW_COMMAND                    =   1;
  const unsigned char E_INVALID_PARAM_ID                  =   2;
  const unsigned char E_INVALID_PARAM_STREAM              =   3;
  const unsigned char E_INVALID_PARAM_STREAM_LEN          =   4;
  const unsigned char E_GETTER_ID_NOT_FOUND               =   5;
  const unsigned char E_INVALID_GETTER_ID                 =   6;
  const unsigned char E_INVALID_GETTER_LIST_COUNT         =   7;
  const unsigned char E_INVALID_MOVE_CMD                  =   8;
  const unsigned char E_OTHER_MOVE_CMD_ACTIVE             =   9;
  const unsigned char E_INVALID_MOVE_SEQUENCE             =  10;
  const unsigned char E_INVALID_PARAM_SIZE                =  11;
  const unsigned char E_INVALID_TRANSACTION_STATE         =  12;
  const unsigned char E_PODEST_DIR_CHANGE_FAILED          =  13;
  const unsigned char E_PODEST_MOVE_FAILED                =  14;
  const unsigned char E_AVOID_READY_TO_RUN                =  15;

  const unsigned char E_STEPPER_NOT_ENABLED               =  20;
  const unsigned char E_STEPPER_NOT_INITIALIZED           =  21;
  const unsigned char E_TOOL_NOT_ENALED                   =  22;
  
  const unsigned char E_STEPPER_PULS_WIDTH_TO_LARGE       =  30;
  const unsigned char E_STEPPER_PULS_WIDTH_OFFSET_TO_LARGE=  31;
  
  const unsigned char E_SPEED_MGMT_NOT_INITIALIZED        =  40;
  const unsigned char E_STEPPER_NOT_READY_TO_RUN          =  41;

  const unsigned char E_LIMIT_SWITCH_ACTIVE               =  51;
  const unsigned char E_RMDR_INVALID_STP_POINTER          =  52;

  const unsigned char E_PURE_TEXT_VALUE_ERROR             = 251;
  const unsigned char E_INTERRUPT                         = 252;
  const unsigned char E_EXTERNEL_INTERRUPT                = 253;
  const unsigned char E_TOTAL_COUNT                       = 254;

// .........................................................

// .....................................................................
// Common values
//
// They are here defined because they are shared between C++ and sketch
//
// .....................................................................

  #define FIRMWARE_VERSION                              "0.9.7"

  #define NORMALIZED_INCREMENT_DIRECTION                 0
  #define INVERSED_INCREMENT_DIRECTION                   1

  #define ANALOG_LOW                                     0
  #define ANALOG_HIGH                                    255

  #define FORCE                                          true
  
  #define ON                                             true
  #define OFF                                            false
  
  #define EXTERNAL_INTERRRUPT_OFF                        LimitSwitch::EXTERNAL_INTERRUPT_OFF
  #define EXTERNAL_INTERRRUPT_ON                         LimitSwitch::EXTERNAL_INTERRUPT_ON

  #define PAUSE_ACTIVE                                   true
  #define PAUSE_INACTIVE                                 false

  #define OPTIMISTIC                                     true
  #define PESIMISTIC                                     false

  // ------------------------------------------------------------------------------
  // Don't modify the values above because it has to coresponde with RS::A and RS::B
  enum AxisId
  { 
      IDX_X = 0,
      IDX_Y = 1,
      IDX_Z = 2 
  };

  // ------------------------------------------------------------------------------
  // Don't modify the values above because it has to coresponde with AxisSignatureIndex
  enum AxisSignature  
  {  
      ASG_X    = 0x02,                    //  2
      ASG_Y    = 0x04,                    //  4
      ASG_XY   = ASG_X | ASG_Y,           //  6
      ASG_Z    = 0x08,                    //  8
      ASG_XZ   = ASG_X | ASG_Z,           // 10
      ASG_YZ   = ASG_Y | ASG_Z,           // 12
      ASG_XYZ  = ASG_X | ASG_Y | ASG_Z    // 14
  };

  #define IS_AxisSignature(s) ( s >= ASG_X && s <= ASG_XYZ && s%2 == 0 )
  
  enum AxisSignatureIndex
  {
      ASGI_MASTER = 0,
      ASGI_X      = 1,
      ASGI_Y      = 2,
      ASGI_XY     = 3,
      ASGI_Z      = 4,
      ASGI_XZ     = 5,
      ASGI_YZ     = 6,
      ASGI_XYZ    = 7
  };
  
  // calculates an zero-based index of the signature
  #define GET_AxisSignatureIndex(s)  ( IS_AxisSignature(s) ? (AxisSignatureIndex)((s / 2) - 1) : ASGI_MASTER )
  #define AXIS_SIGNATURE_COUNT       ( ASGI_XYZ + 1 )

  // ------------------------------------------------------------------------------
  
  const char      MBYTE_CLOSE                            = RET_MBYTE_CLOSE;
  const char      TEXT_CLOSE                             = '\n';
  const char      TEXT_SEPARATOR                         = ':';

  const uint32_t  minSerialReadTimeoutMicros             = 10L * 1000L;
  const int16_t   moveUntilAccelPeriod                   = 2500; // ms

  const int8_t    NORMALIZED_INCREMENT_DIRECTION_VALUE   = 1;
  const int8_t    INVERSED_INCREMENT_DIRECTION_VALUE     = 0;

  const int16_t   MIN_DURATION_PER_STEP_US               = 225;

  // MAX_SERIAL_BUFFER_SIZE:
  // - 64 byte is the standard UNO buffer sizes
  // - Max 255
  // - Attention: If the value is > than 64 possibly a data loss occures, because the reader is always slower the the writer
  const int8_t    MAX_SERIAL_BUFFER_SIZE                 = 64; 
  const int32_t   BAUD_RATE                              = 115200; //300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, or 115200
    
  const int8_t    DIRECTION_UNKNOWN                      =  0;
  const int8_t    DIRECTION_INC                          = +1;
  const int8_t    DIRECTION_DEC                          = -1;
  
  const int8_t    MAX_SETTER_VALUES                      = 16;
  const int8_t    MAX_MOVE_CMD_SIZE                      = 16;
    
  const int16_t   FLT_FACT                               = 1000;
  const int32_t   MIN_LONG                               = -2147483648L;
  const int32_t   MAX_LONG                               = +2147483647L;

  // constant values for fixed point arithmetics
  const int32_t   C_2_08                                 =     256;
  const int32_t   C_2_09                                 =     512;
  const int32_t   C_2_10                                 =    1024;
  const int32_t   C_2_11                                 =    2048;
  const int32_t   C_2_12                                 =    4096;
  const int32_t   C_2_13                                 =    8192;
  const int32_t   C_2_14                                 =   16384;
  const int32_t   C_2_15                                 =   32768;
  const int32_t   C_2_16                                 =   65536;

  // Position store trace ids
  const uint8_t   POS_STORE_SID_CTL_POS                  =  1;
  const uint8_t   POS_STORE_SID_APP_POS                  =  2;
  const uint8_t   POS_STORE_SID_DIRECT_MOVE              =  3;
  const uint8_t   POS_STORE_SID_RENDER_AND_MOVE          =  4;
  const uint8_t   POS_STORE_SID_STEPPING                 =  5;
  const uint8_t   POS_STORE_SID_PATH_HANDLER_OUTPUT      =  6;
  const uint8_t   POS_STORE_SID_PATH_LIST_RUNNER         =  7;
  const uint8_t   POS_STORE_SID_MOV_SEQ_ADD              =  8;
  const uint8_t   POS_STORE_SID_SERIAL_NULL              =  9;
  
#endif
