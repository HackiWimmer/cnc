#ifndef COMMON_VALUES_H
#define COMMON_VALUES_H

//////////////////////////////////////////////////////////////////////////////////
template <class T>
inline T absolute(T val) {
  if ( val < 0.0 )
    val *= -1;
    
  return val;
}

//////////////////////////////////////////////////////////////////////////////////
// Version
#define FIRMWARE_VERSION                              "0.7.7"

// Pin setup                                             
#define MAX_PINS                                      32 
#define X_STP                                          2 
#define Y_STP                                          3 
#define Z_STP                                          4 
#define X_DIR                                          5 
#define Y_DIR                                          6 
#define Z_DIR                                          7 
                                                         
#define ENABLE_PIN                                     8 
                                                         
#define X_LIMIT                                        9 
#define Y_LIMIT                                       10 
#define Z_LIMIT                                       11 
                                                         
#define TOOL_PIN                                      12 
#define SUPPORT_PIN                                   13 
                                                         
#define INTERRUPT_LED                                 A3 
#define INTERRUPT_LED_ID                               3 


// Signals
#define SIG_INTERRUPPT                                'I'
#define SIG_HALT                                      'H'
#define SIG_PAUSE                                     'P'
#define SIG_RESUME                                    'p'

// Commands
#define MAX_CMDS                                      255 
#define CMD_INVALID                                   '\0'

#define CMD_IDLE                                      'i'
#define CMD_RESET_CONTROLLER                          'R'
#define CMD_RESET_ERRORINFO                           'r'

#define CMD_SETTER                                    'S'
#define CMD_GETTER                                    'G'

#define CMD_ENABLE_STEPPER_PIN                        'E'
#define CMD_DISABLE_STEPPER_PIN                       'e'

#define CMD_POS_STEP_X                                'X'
#define CMD_NEG_STEP_X                                'x'
#define CMD_POS_STEP_Y                                'Y'
#define CMD_NEG_STEP_Y                                'y'
#define CMD_POS_STEP_Z                                'Z'
#define CMD_NEG_STEP_Z                                'z'

#define CMD_MOVE                                      'm'
#define CMD_RENDER_AND_MOVE                           'M'

#define CMD_TEST_START                                'T'

#define CMD_PRINT_CONFIG                              'c'
#define CMD_PRINT_VERSION                             'V'
#define CMD_PRINT_PIN_REPORT                          'Q'
#define CMD_PRINT_ERRORINFO                           '?'

#define CMD_ENABLE_PROBE_MODE                         'K'
#define CMD_DISABLE_PROBE_MODE                        'k'

#define CMD_TEST_INFO_MESSAGE                        129
#define CMD_TEST_WARN_MESSAGE                        130
#define CMD_TEST_ERROR_MESSAGE                       131

//Separator ids
#define SEPARARTOR_SETUP                               1
#define SEPARARTOR_RESET                               2
#define SEPARARTOR_RUN                                 3
                                                        
// Parameter Ids                                        
#define MAX_PIDS                                     255
#define PID_UNKMOWN                                    0
#define PID_ERROR_COUNT                                1
#define PID_VERSION                                    2
#define PID_HEARTBEAT                                  3
#define PID_STEPPER_INITIALIZED                        4
#define PID_SEPARATOR                                  5
                                                        
#define PID_SPEED_OFFSET                              10
#define PID_SPEED_OFFSET_X                            11
#define PID_SPEED_OFFSET_Y                            12
#define PID_SPEED_OFFSET_Z                            13
#define PID_STEPS_X                                   14 
#define PID_STEPS_Y                                   15 
#define PID_STEPS_Z                                   16 
                                                         
#define PID_MIN_SWITCH                                20 
#define PID_MAX_SWITCH                                21 
#define PID_LIMIT                                     22 
#define PID_LAST_STEP_DIR                             23 
#define PID_X_LIMIT                                   24 
#define PID_Y_LIMIT                                   25 
#define PID_Z_LIMIT                                   26 

#define PID_CONTROLLER                                30
#define PID_ROUTER_SWITCH                             31
#define PID_MIN_ENABLE_PULSE_WIDTH                    32
#define PID_POS_REPLY_THRESHOLD_X                     33
#define PID_POS_REPLY_THRESHOLD_Y                     34
#define PID_POS_REPLY_THRESHOLD_Z                     35
                                                        
#define PID_XYZ_POS                                   40
#define PID_XYZ_POS_MAJOR                             41
#define PID_XYZ_POS_DETAIL                            42
#define PID_XY_POS                                    43
#define PID_X_POS                                     44
#define PID_Y_POS                                     45
#define PID_Z_POS                                     46
#define PID_PROBE_MODE                                47
                                                        
#define PID_AXIS                                      50 
#define PID_COMMON                                    51 
#define PID_STEPS                                     52 
#define PID_STEP_MULTIPLIER                           53 
#define PID_STEP_PIN                                  54 
#define PID_DIR_PIN                                   55 
#define PID_ENABLE_STEPPERS                           56 
#define PID_AVG_STEP_DURRATION                        57 
                                                         
#define PID_TEST_SUITE                                70 
#define PID_TEST_VALUE1                               71 
#define PID_TEST_VALUE2                               72 
#define PID_TEST_VALUE3                               73 
#define PID_TEST_VALUE4                               74 
#define PID_TEST_VALUE5                               75 
#define PID_TEST_INTERRUPT                            76 

#define PID_CURRENT_STEP_PULSE_WIDTH_LOW             100
#define PID_CURRENT_STEP_PULSE_WIDTH_HIGH            101
#define PID_CURRENT_DIR_PULSE_WIDTH                  102
#define PID_PULSE_WIDTH_OFFSET                       103
#define PID_PULSE_WIDTH_OFFSET_X                     104
#define PID_PULSE_WIDTH_OFFSET_Y                     105
#define PID_PULSE_WIDTH_OFFSET_Z                     106

#define PID_RESERT_POS_COUNTER                       110
#define PID_GET_POS_COUNTER                          111
#define PID_RESERT_STEP_COUNTER                      112
#define PID_GET_STEP_COUNTER_X                       113
#define PID_GET_STEP_COUNTER_Y                       114
#define PID_GET_STEP_COUNTER_Z                       115

////////////////////////////////////////////////////////
// start double pid range
#define PID_DOUBLE_RANG_START                        200 
#define PID_PITCH                                    200 
#define PID_PITCH_X                                  201 
#define PID_PITCH_Y                                  202 
#define PID_PITCH_Z                                  203 
#define PID_SPEED_MM_MIN                             204
// end double pid range
////////////////////////////////////////////////////////

                                                         
//Test Ids                                               
#define TID_INVALID                                    0
#define TID_HEARTBEAT                                 10
#define TID_MOTOR_CFG_INTERVAL                        11
#define TID_DURATION                                  12
                                                         
//Message types                                          
#define MT_INFO                                       'I'
#define MT_WARNING                                    'W'
#define MT_ERROR                                      'E'
                                                         
// error codes                                           
#define MAX_ERROR_CODES                               255
#define E_NO_ERROR                                      0
#define E_UNKNOW_COMMAND                                1
#define E_INVALID_PARAM_ID                              2
#define E_INVALID_PARAM_STREAM                          3
#define E_GETTER_ID_NOT_FOUND                           4
#define E_INVALID_GETTER_ID                             5
#define E_INVALID_MOVE_CMD                              6
                                                         
#define E_INVALID_TEST_ID                              10
#define E_NOT_KNOWN_TEST_ID                            11
#define E_INVALID_PARAMETER                            12

#define E_STEPPER_NOT_ENALED                           20
#define E_STEPPER_NOT_INITIALIZED                      21

#define E_STEPPER_PULS_WIDTH_TO_LARGE                  30
#define E_STEPPER_PULS_WIDTH_OFFSET_TO_LARGE           31
                                                         
                                                         
#define E_INTERRUPT                                   253
#define E_TOTAL_COUNT                                 254



// const values
#define EMPTY_TEXT_VAL                                 ""

const long BAUD_RATE                                   =  115200; //300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, or 115200
const int  MAX_PARAM_SIZE                              =  64;
const int  MAX_MOVE_CMD_SIZE                           =  16;

const unsigned char MBYTE_CLOSE                        =  '\0';
const unsigned char TEXT_CLOSE                         =  '\n';
const unsigned char MSG_CLOSE                          =  '\r';
const unsigned char BLANK                              =  ' ';
const unsigned char TEXT_SEPARATOR                     =  ':';

const unsigned char RET_NULL                           =   0;   // Invalid ret code
const unsigned char RET_SOH                            =   1;   // Start of header
const unsigned char RET_SOT                            =   2;   // Start of text
const unsigned char RET_OK                             =   6;   // Ack
const unsigned char RET_MSG                            =  17;   // free
const unsigned char RET_ERROR                          =  21;   // Nack
const unsigned char RET_SENT                           = 255;   // Pseudo ret code for sent data, only reserved and not used by the Arduino

const long DBL_FACT                                    = 1000;
const long MIN_LONG                                    = -2147483648L;
const long MAX_LONG                                    = +2147483647L;
const long LIMIT_MIN                                   = -1L;
const long LIMIT_MAX                                   = +1L;
const long LIMIT_SET_BUT_MIN_MAX_UNKNOWN               = MAX_LONG;
const long LIMIT_UNSET                                 = 0L;
 
#endif

