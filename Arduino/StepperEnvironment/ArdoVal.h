#ifndef COMMON_VALUES_H
#define COMMON_VALUES_H

#ifndef SKETCH_COMPILE   
  #include <iostream>
  typedef unsigned char byte;  
#endif

#include "AnalogPinController/CommonDefinitions.h"
#include "AnalogPinController/CncInterface.h"
  
// .....................................................................
// Pin setup
// .....................................................................

  const unsigned char MAX_PINS                            =  32;
  const unsigned char PIN_X_STP                           =   2;
  const unsigned char PIN_Y_STP                           =   3;
  const unsigned char PIN_Z_STP                           =   4;
  const unsigned char PIN_X_DIR                           =   5;
  const unsigned char PIN_Y_DIR                           =   6;
  const unsigned char PIN_Z_DIR                           =   7;

  const unsigned char PIN_STEPPER_ENABLE                  =   8;

  const unsigned char PIN_X_LIMIT                         =   9;
  const unsigned char PIN_Y_LIMIT                         =  10;
  const unsigned char PIN_Z_LIMIT                         =  11;

  const unsigned char PIN_TOOL_ENABLE                     =  12;
  const unsigned char PIN_TOOL_FEEDBACK                   =  13;

  // A0 CNC Shield: Reset/Abort
  // A1 CNC Shield: Feed Hold
  // A2 CNC Shield: Cycle Start / Resume
  // A3 CNC Shield: Collant Enabled
  // A4 CNC Shield: free
  // A5 CNC Shield: free
  
  #ifndef SKETCH_COMPILE   
    #define PIN_INTERRUPT                              AE::PN_A0   
  #else
    #define PIN_INTERRUPT                                     A0
  #endif  
  const unsigned char PIN_INTERRUPT_ID                     =   0;
  

  #ifndef SKETCH_COMPILE   
    #define PIN_INTERRUPT_LED                          AE::PN_A3   
  #else
    #define PIN_INTERRUPT_LED                                 A3   
  #endif  
  const unsigned char PIN_INTERRUPT_LED_ID                 =   3;
  
// .....................................................................
// Signals
// .....................................................................

  const unsigned char SIG_INTERRUPPT                      =  'I';
  const unsigned char SIG_HALT                            =  'H';
  const unsigned char SIG_PAUSE                           =  'P';
  const unsigned char SIG_RESUME                          =  'p';
  const unsigned char SIG_QUIT_MOVE                       =  'q';

  const unsigned char SIG_SOFTWARE_RESET                  =  '#';

// .....................................................................
// Commands
// .....................................................................

  const unsigned char MAX_CMDS                            =  255;
  const unsigned char CMD_INVALID                         = '\0'; 
  
  const unsigned char CMD_IDLE                            =  'i'; // 105 0x69
  const unsigned char CMD_HEARTBEAT                       =  'h'; // 104 0x68
  
  const unsigned char CMD_RESET_CONTROLLER                =  'R'; //  82 0x52
    
  const unsigned char CMD_SETTER                          =  'S'; //  83 0x53
  const unsigned char CMD_GETTER                          =  'G'; //  71 0x47
  
  const unsigned char CMD_MOVE                            =  'm'; // 155 0x9B
  const unsigned char CMD_RENDER_AND_MOVE                 =  'M'; //  77 0x4D

  const unsigned char CMD_MOVE_SEQUENCE                   =  '.'; //  46 0x2E
  const unsigned char CMD_RENDER_AND_MOVE_SEQUENCE        =  ':'; //  58 0x3A

  const unsigned char CMD_MOVE_UNIT_SIGNAL                =  '>'; //  62 0x3E
  
  const unsigned char CMD_PRINT_CONFIG                    =  'c'; //  99 0x63
  const unsigned char CMD_PRINT_TIMESTAMP                 =  'T'; //  84 0x54
  const unsigned char CMD_PRINT_VERSION                   =  'V'; //  86 0x56
  const unsigned char CMD_PRINT_PIN_REPORT                =  'Q'; //  81 0x51
    
  // Ranges reserved via above definitions:
  //   48 -  57 --> '0'..'9'
  //   65 -  90 --> 'A'..'Z'
  //   97 - 122 --> 'a'..'z'
  //
  // start numbered definition from 129 on (above 128)

  const unsigned char CMD_TEST_INFO_MESSAGE               = 129;
  const unsigned char CMD_TEST_WARN_MESSAGE               = 130;
  const unsigned char CMD_TEST_ERROR_MESSAGE              = 131;
  const unsigned char CMD_TEST_LAST_ERROR                 = 132;
  const unsigned char CMD_TEST_LAB                        = 133;

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
  const unsigned char PID_DOUBLE_RANG_START               = 200;
  const unsigned char PID_DOUBLE_RANG_END                 = 240;
  
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
  
  const unsigned char PID_STEPS_X                         =  30;
  const unsigned char PID_STEPS_Y                         =  31;
  const unsigned char PID_STEPS_Z                         =  32;
  const unsigned char PID_LAST_STEP_DIR                   =  33;
  const unsigned char PID_ACCEL_PROFILE                   =  34;
  const unsigned char PID_ACCEL_START_SPEED               =  35;
  const unsigned char PID_ACCEL_STOP_SPEED                =  36;
     
  const unsigned char PID_MIN_SWITCH                      =  40;
  const unsigned char PID_MAX_SWITCH                      =  41;
  const unsigned char PID_LIMIT                           =  42;
  const unsigned char PID_X_LIMIT                         =  43;
  const unsigned char PID_Y_LIMIT                         =  44;
  const unsigned char PID_Z_LIMIT                         =  45;
  const unsigned char PID_I2C_AVAILABEL                   =  46;
  const unsigned char PID_I2C_LIMIT_VALUE                 =  47;
  const unsigned char PID_I2C_SUPPORT_VALUE               =  48;
 
  const unsigned char PID_CONTROLLER                      =  50;
  const unsigned char PID_TOOL_SWITCH                     =  51;
  const unsigned char PID_POS_REPLY_THRESHOLD_X           =  52;
  const unsigned char PID_POS_REPLY_THRESHOLD_Y           =  53;
  const unsigned char PID_POS_REPLY_THRESHOLD_Z           =  54;
  const unsigned char PID_MAX_DIMENSION_X                 =  55;
  const unsigned char PID_MAX_DIMENSION_Y                 =  56;
  const unsigned char PID_MAX_DIMENSION_Z                 =  57;

  const unsigned char PID_XYZ_POS                         =  60;
  const unsigned char PID_XYZ_POS_MAJOR                   =  61;
  const unsigned char PID_XYZ_POS_DETAIL                  =  62;
  const unsigned char PID_XY_POS                          =  63;
  const unsigned char PID_X_POS                           =  64;
  const unsigned char PID_Y_POS                           =  65;
  const unsigned char PID_Z_POS                           =  66;
  const unsigned char PID_PROBE_MODE                      =  67;

  const unsigned char PID_AXIS                            =  70;
  const unsigned char PID_COMMON                          =  71;
  const unsigned char PID_STEPS                           =  72;
  const unsigned char PID_STEP_PIN                        =  73;
  const unsigned char PID_DIR_PIN                         =  74;
  const unsigned char PID_ENABLE_STEPPERS                 =  75;
  const unsigned char PID_AVG_STEP_DURATION               =  76;

  const unsigned char PID_TEST_INTERRUPT                  =  80;
  const unsigned char PID_INC_DIRECTION_VALUE_X           =  81;
  const unsigned char PID_INC_DIRECTION_VALUE_Y           =  82;
  const unsigned char PID_INC_DIRECTION_VALUE_Z           =  83;

  const unsigned char PID_SPEED_CONTROLLER                =  90;
  const unsigned char PID_SPEED_FEED_MODE                 =  91;
  const unsigned char PID_SPEED_CTRL_INITIALIZED          =  92;
  const unsigned char PID_SPEED_CTRL_TOTAL_OFFSET         =  93;
  const unsigned char PID_SPEED_CTRL_RPM                  =  94;
  const unsigned char PID_SPEED_CTRL_SYNTH_SPEED_DELAY    =  95;
  const unsigned char PID_SPEED_CTRL_MAX_SPEED            =  96;
  const unsigned char PID_SPEED_CTRL_CFG_SPEED            =  97;
  
  const unsigned char PID_PULSE_WIDTH_OFFSET_DIR          = 100;
  const unsigned char PID_PULSE_WIDTH_OFFSET_LOW          = 101;
  const unsigned char PID_PULSE_WIDTH_OFFSET_HIGH         = 102;
  const unsigned char PID_PULSE_WIDTH_LOW_X               = 103;
  const unsigned char PID_PULSE_WIDTH_LOW_Y               = 104;
  const unsigned char PID_PULSE_WIDTH_LOW_Z               = 105;
  const unsigned char PID_PULSE_WIDTH_HIGH_X              = 106;
  const unsigned char PID_PULSE_WIDTH_HIGH_Y              = 107;
  const unsigned char PID_PULSE_WIDTH_HIGH_Z              = 108;
  
  const unsigned char PID_RESERT_POS_COUNTER              = 110;
  const unsigned char PID_GET_POS_COUNTER                 = 111;
  const unsigned char PID_RESERT_STEP_COUNTER             = 112;
  const unsigned char PID_GET_STEP_COUNTER_X              = 113;
  const unsigned char PID_GET_STEP_COUNTER_Y              = 114;
  const unsigned char PID_GET_STEP_COUNTER_Z              = 115;

// .....................................................................
// end long pid range [PID_DOUBLE_RANG_END] 
// .....................................................................

// .....................................................................
// Please consider
//
// PID_DOUBLE_RANG_START ... PID_DOUBLE_RANG_END 
//
// are reserved for double values
//
// .....................................................................
// start double pid range
// .....................................................................

  const unsigned char PID_PITCH                           = 200;
  const unsigned char PID_PITCH_X                         = 201;
  const unsigned char PID_PITCH_Y                         = 202;
  const unsigned char PID_PITCH_Z                         = 203;
  const unsigned char PID_SPEED_MM_MIN                    = 204;

// .....................................................................
// end double pid range
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
  const unsigned char E_INVALID_MOVE_SEQUENCE             =   9;
  const unsigned char E_INVALID_PARAM_SIZE                =  10;

  const unsigned char E_STEPPER_NOT_ENALED                =  20;
  const unsigned char E_STEPPER_NOT_INITIALIZED           =  21;
  const unsigned char E_TOOL_NOT_ENALED                   =  22;
  
  const unsigned char E_STEPPER_PULS_WIDTH_TO_LARGE       =  30;
  const unsigned char E_STEPPER_PULS_WIDTH_OFFSET_TO_LARGE=  31;
  
  const unsigned char E_SPEED_MGMT_NOT_INITIALIZED        =  40;
  const unsigned char E_STEPPER_NOT_READY_TO_RUN          =  41;

  const unsigned char E_LIMIT_SWITCH_ACTIVE               =  51;

  const unsigned char E_TEST_ERROR_CODE                   =  60;

  const unsigned char E_PURE_TEXT_VALUE_ERROR             = 252;
  const unsigned char E_INTERRUPT                         = 253;
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
  
  #define TOOL_STATE_OFF                                 PL_LOW
  #define TOOL_STATE_ON                                  PL_HIGH 

  #define ENABLE_STATE_OFF                               PL_HIGH
  #define ENABLE_STATE_ON                                PL_LOW 

  #define PAUSE_ACTIVE                                   true
  #define PAUSE_INACTIVE                                 false

  const char MBYTE_CLOSE                                 = RET_MBYTE_CLOSE;
  const char TEXT_CLOSE                                  = '\n';
  const char TEXT_SEPARATOR                              = ':';

  const uint32_t minSerialReadTimeoutMicros              = 10L * 1000L;
  
  const unsigned short moveUntilAccelPeriod              = 2500; // ms
  const int16_t cncHeartbeatInterval                     = 2000; // milli seconds

  const int32_t NORMALIZED_INCREMENT_DIRECTION_VALUE     = 1;
  const int32_t INVERSED_INCREMENT_DIRECTION_VALUE       = 0;

  const int16_t SPEED_MANAGER_CONST_STATIC_OFFSET_US     = 0; // [us]
  const int16_t SPEED_MANAGER_CONST_LOOP_OFFSET_US       = 0; // [us]

  // MAX_SERIAL_BUFFER_SIZE:
  // - 64 byte is the standard UNO buffer sizes
  // - Max 255
  // - Attention: If the value is > than 64 possible data loss occures,
  //              always the reader is slower the the writer
  const int16_t MAX_SERIAL_BUFFER_SIZE                   = 64; 
  const int32_t BAUD_RATE                                = 115200; //300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, or 115200
    
  const int16_t DIRECTION_UNKNOWN                        =  0;
  const int16_t DIRECTION_INC                            = +1;
  const int16_t DIRECTION_DEC                            = -1;

  
  const int16_t MAX_SETTER_VALUES                        = 16;
  const int16_t MAX_MOVE_CMD_SIZE                        = 16;
    
  const int32_t DBL_FACT                                 = 1000;
  const int32_t MIN_LONG                                 = -2147483648L;
  const int32_t MAX_LONG                                 = +2147483647L;
  
  const int16_t MIN_INT                                  = -32767;
  const int16_t MAX_INT                                  = +32767;
  
  const uint16_t MAX_UINT                                = +65535;

#endif
