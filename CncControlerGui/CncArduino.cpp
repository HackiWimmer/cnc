
#include "CncArduino.h"

std::string ArduinoCMDs::cmds[MAX_CMDS];
std::string ArduinoCMDs::ret;

/////////////////////////////////////////////////////////////////////////
class ArduinoCMDsInitializer {
	public:
		ArduinoCMDsInitializer() {
			ArduinoCMDs::init();
		}
}; ArduinoCMDsInitializer aci;

/////////////////////////////////////////////////////////////////////////
void ArduinoCMDs::init() {
/////////////////////////////////////////////////////////////////////////
	for (int i=0; i<MAX_CMDS -1;i++){
		std::string s("Unknown Command: ");
		s += std::to_string(i);
		cmds[i]  = s.c_str();
	}
	
	cmds[CMD_INVALID]               = "Invalid Command";
	
	cmds[SIG_INTERRUPPT]            = "Push Signal Interrupt";
	cmds[SIG_HALT]                  = "Push Signal Pause";
	cmds[SIG_PAUSE]                 = "Push Signal Pause";
	cmds[SIG_RESUME]                = "Push Signal Resume";
	cmds[SIG_QUIT_MOVE]             = "Push Signal Quit Move";
	
	cmds[CMD_IDLE]                  = "Pull Idle Callback";
	cmds[CMD_RESET_CONTROLLER]      = "Push Reset Controller";
	cmds[CMD_RESET_ERRORINFO]       = "Push Reset Error Info";
	
	cmds[CMD_SETTER]                = "Push Setter";
	cmds[CMD_GETTER]                = "Pull Getter";
	cmds[CMD_GETTER_LIST]           = "Pull Getter List";
	
	cmds[CMD_ENABLE_STEPPER_PIN]    = "Push Enable Stepper Motors";
	cmds[CMD_DISABLE_STEPPER_PIN]   = "Push Disable Stepper Motors";
	
	cmds[CMD_POS_STEP_X]            = "Push Positive X Step";
	cmds[CMD_NEG_STEP_X]            = "Push Negative X Step";
	cmds[CMD_POS_STEP_Y]            = "Push Positive Y Step";
	cmds[CMD_NEG_STEP_Y]            = "Push Negative Y Step";
	cmds[CMD_POS_STEP_Z]            = "Push Positive Z Step";
	cmds[CMD_NEG_STEP_Z]            = "Push Negative Z Step";
	
	cmds[CMD_MOVE]                  = "Push 3D Move";
	cmds[CMD_RENDER_AND_MOVE]       = "Push Render and 3D Move";
	
	cmds[CMD_TEST_START]            = "Push Pause Start";
	
	cmds[CMD_PRINT_CONFIG]          = "Pull Configuration";
	cmds[CMD_PRINT_VERSION]         = "Pull Version";
	cmds[CMD_PRINT_PIN_REPORT]      = "Pull PIN Report";
	cmds[CMD_PRINT_ERRORINFO]       = "Pull Error Info";
	
	cmds[CMD_ENABLE_PROBE_MODE]     = "Push Enable Probe Mode";
	cmds[CMD_DISABLE_PROBE_MODE]    = "Push Disable Probe Mode";
	
	cmds[CMD_TEST_INFO_MESSAGE]     = "Pull Version";
	cmds[CMD_TEST_WARN_MESSAGE]     = "Pull PIN Report";
	cmds[CMD_TEST_ERROR_MESSAGE]    = "Pull Error Info";
	
} 
/////////////////////////////////////////////////////////////////////////
const char* ArduinoCMDs::getCMDLabel(unsigned int id) {
// not thread safe version - use instead getCMDLabel(unsigned int id, std::string& retVal) 
/////////////////////////////////////////////////////////////////////////
	return getCMDLabel(id, ArduinoCMDs::ret);
}
/////////////////////////////////////////////////////////////////////////
const char* ArduinoCMDs::getCMDLabel(unsigned int id, std::string& retVal) {
// thread safe version
/////////////////////////////////////////////////////////////////////////
	if ( id >= 0 && id < MAX_CMDS )
		return cmds[id].c_str();

	retVal.assign("C [");
	retVal.append(std::to_string(id));
	retVal.append("] is out of range");
	
	return ret.c_str();
}

std::string ArduinoPIDs::pids[MAX_PIDS];
std::string ArduinoPIDs::ret;

/////////////////////////////////////////////////////////////////////////
class ArduinoPIDsInitializer {
	public:
		ArduinoPIDsInitializer() {
			ArduinoPIDs::init();
		}
}; ArduinoPIDsInitializer api;

const char* UNKNOWN_PID = "Unknown PID: ";
/////////////////////////////////////////////////////////////////////////
void ArduinoPIDs::init() {
/////////////////////////////////////////////////////////////////////////
	for (int i=0; i<MAX_PIDS -1;i++){
		std::string s(UNKNOWN_PID);
		s += std::to_string(i);
		pids[i]  = s.c_str();
	}
	
	pids[RET_NULL]                            = "RET_NULL";
	pids[RET_OK]                              = "RET_OK";
	pids[RET_ERROR]                           = "RET_ERROR";
	pids[RET_SOT]                             = "RET_SOT";
	pids[RET_SOH]                             = "RET_SOH";
	pids[RET_INTERRUPT]                       = "RET_INTERRUPT"; 
	pids[RET_HALT]                            = "RET_HALT"; 
	pids[RET_QUIT]                            = "RET_QUIT";
	pids[RET_MSG]                             = "RET_MSG";
	
	pids[PID_UNKNOWN]                         = "Default PID";
	pids[PID_STEPPER_INITIALIZED]             = "Stepper Initialize State";
	
	pids[PID_QUERY_READY_TO_RUN]              = "Ready to run";
	
	pids[PID_SPEED_OFFSET]                    = "Speed Step Offset [us]";
	pids[PID_SPEED_OFFSET_X]                  = "Speed Step Offset X Axis [us]";
	pids[PID_SPEED_OFFSET_Y]                  = "Speed Step Offset Y Axis [us]";
	pids[PID_SPEED_OFFSET_Z]                  = "Speed Step Offset Z Axis [us]";
	pids[PID_STEPS_X]                         = "Steps X Axis";
	pids[PID_STEPS_Y]                         = "Steps Y Axis";
	pids[PID_STEPS_Z]                         = "Steps Z Axis";
	
	pids[PID_PITCH]                           = "Pitch";
	pids[PID_PITCH_X]                         = "Pitch X Axis";
	pids[PID_PITCH_Y]                         = "Pitch Y Axis";
	pids[PID_PITCH_Z]                         = "Pitch Z Axis";
	
	pids[PID_MIN_SWITCH]                      = "Min Switch";  
	pids[PID_MAX_SWITCH]                      = "Max Switch";
	pids[PID_LIMIT]                           = "Limit State";
	pids[PID_LAST_STEP_DIR]                   = "Last Step Direction";
	pids[PID_X_LIMIT]                         = "X limit State";
	pids[PID_Y_LIMIT]                         = "Y limit State";
	pids[PID_Z_LIMIT]                         = "Z limit State";

	pids[PID_I2C_AVAILABEL  ]                 = "I2C Availabel";
	pids[PID_I2C_LIMIT_VALUE]                 = "I2C Limit Value";
	pids[PID_I2C_SUPPORT_VALUE]               = "I2C Support Value";
	
	pids[PID_CONTROLLER]                      = "Cnc Controller";
	pids[PID_TOOL_SWITCH]                     = "Tool Enabled State";
	pids[PID_POS_REPLY_THRESHOLD_X]           = "Position Reply Threshold X Axis";
	pids[PID_POS_REPLY_THRESHOLD_Y]           = "Position Reply Threshold Y Axis";
	pids[PID_POS_REPLY_THRESHOLD_Z]           = "Position Reply Threshold Z Axis";
	pids[PID_PROBE_MODE]                      = "Probe Mode";

	pids[PID_XYZ_POS_MAJOR]                   = "XYZ Pos - Type Major";
	pids[PID_XYZ_POS_DETAIL]                  = "XYZ Pos - Type Detail";
	pids[PID_XYZ_POS]                         = "XYZ Pos";
	pids[PID_XY_POS]                          = "XY Pos";
	pids[PID_X_POS]                           = "X Pos";
	pids[PID_Y_POS]                           = "Y Pos";
	pids[PID_Z_POS]                           = "Z Pos";

	pids[PID_AXIS]                            = "Stepper Axis";
	pids[PID_COMMON]                          = "Common";
	pids[PID_SPEED_MM_MIN]                    = "Speed [mm/minute]";
	pids[PID_STEPS]                           = "Steps";
	pids[PID_STEP_MULTIPLIER]                 = "Multiplier";
	pids[PID_STEP_PIN]                        = "Step Pin";
	pids[PID_DIR_PIN]                         = "Direction Pin";
	pids[PID_ENABLE_STEPPERS]                 = "Stepper Enable State";
	pids[PID_AVG_STEP_DURATION]               = "AVG Step Loop Duration [us]";

	pids[PID_PULSE_WIDTH_OFFSET_DIR]          = "Direction Pulse Width";
	pids[PID_PULSE_WIDTH_OFFSET_LOW]          = "Step Pulse Width Low";
	pids[PID_PULSE_WIDTH_OFFSET_HIGH]         = "Step Pulse Width High";
	pids[PID_PULSE_WIDTH_LOW_X]               = "Step Pulse Width Low X";
	pids[PID_PULSE_WIDTH_LOW_Y]               = "Step Pulse Width Low Y";
	pids[PID_PULSE_WIDTH_LOW_Z]               = "Step Pulse Width Low Z";
	pids[PID_PULSE_WIDTH_HIGH_X]              = "Step Pulse Width High X";
	pids[PID_PULSE_WIDTH_HIGH_Y]              = "Step Pulse Width High Y";
	pids[PID_PULSE_WIDTH_HIGH_Z]              = "Step Pulse Width High Z";
	
	pids[PID_TEST_SUITE]                      = "Test Suite";
	pids[PID_TEST_VALUE1]                     = "Parmeter1";
	pids[PID_TEST_VALUE2]                     = "Parmeter2";
	pids[PID_TEST_VALUE3]                     = "Parmeter3";
	pids[PID_TEST_VALUE4]                     = "Parmeter4";
	pids[PID_TEST_VALUE5]                     = "Parmeter5";
	
	pids[PID_TEST_INTERRUPT]                  = "Test Interrupt Flag";
	
	pids[PID_ERROR_COUNT]                     = "Error Count";
	pids[PID_VERSION]                         = "Firmware Version";
	
	pids[PID_SEPARATOR]                       = "Separator";
	
	pids[PID_RESERT_POS_COUNTER]              = "Reset Position Counter";
	pids[PID_GET_POS_COUNTER]                 = "Get Position Counter";
	pids[PID_RESERT_STEP_COUNTER]             = "Reset Step Counter";
	pids[PID_GET_STEP_COUNTER_X]              = "Get Step Counter X";
	pids[PID_GET_STEP_COUNTER_Y]              = "Get Step Counter Y";
	pids[PID_GET_STEP_COUNTER_Z]              = "Get Step Counter Z";
	
	pids[PID_SPEED_CONTROLLER]                = "Feed Speed Controller";
	pids[PID_SPEED_CTRL_INITIALIZED]          = "Initialized State";
	pids[PID_SPEED_CTRL_RPM]                  = "Rounds Per Minute";
	pids[PID_SPEED_CTRL_TOTAL_OFFSET]         = "Total Speed Offset";
	pids[PID_SPEED_CTRL_SYNTH_SPEED_DELAY]    = "Synthetically Speed Delay";
	pids[PID_SPEED_CTRL_MAX_SPEED]            = "Max Speed";
	pids[PID_SPEED_CTRL_CFG_SPEED]            = "Configured Speed";
	
	pids[PID_MAX_DIMENSION_X]                 = "Max Dimension X [Steps]";
	pids[PID_MAX_DIMENSION_Y]                 = "Max Dimension Y [Steps]";
	pids[PID_MAX_DIMENSION_Z]                 = "Max Dimension Z [Steps]";
	
	pids[PID_INCREMENT_DIRECTION_VALUE_X]     = "Invert Dircetion X";
	pids[PID_INCREMENT_DIRECTION_VALUE_Y]     = "Invert Dircetion Y";
	pids[PID_INCREMENT_DIRECTION_VALUE_Z]     = "Invert Dircetion Z";
} 
/////////////////////////////////////////////////////////////////////////
const char* ArduinoPIDs::getPIDLabel(unsigned int id) {
// not thread safe version - use instead getPIDLabel(unsigned int id, std::string& retVal) 
/////////////////////////////////////////////////////////////////////////
	return getPIDLabel(id, ret);
}
/////////////////////////////////////////////////////////////////////////
const char* ArduinoPIDs::getPIDLabel(unsigned int id, std::string& retVal) {
// thread safe version
/////////////////////////////////////////////////////////////////////////
	if ( id >= 0 && id < MAX_PIDS ) 
		return pids[id].c_str();
	
	retVal.assign("PID [");
	retVal.append(std::to_string(id));
	retVal.append("] is out of range");
	
	return retVal.c_str();
}
/////////////////////////////////////////////////////////////////////////
const char* ArduinoPIDs::getPIDLabelWithDefault(unsigned int id, const std::string& defaultValue) {
/////////////////////////////////////////////////////////////////////////
	if ( exists(id) == true )
		return pids[id].c_str();
	
	return defaultValue.c_str();
}
/////////////////////////////////////////////////////////////////////////
bool ArduinoPIDs::exists(unsigned int id) {
/////////////////////////////////////////////////////////////////////////
	if ( id >= 0 && id < MAX_PIDS ) {
		if ( pids[id].find(UNKNOWN_PID) == std::string::npos )
			return true;
	}
	
	return false;
}

std::string ArduinoErrorCodes::errorCodes[MAX_ERROR_CODES];
std::string ArduinoErrorCodes::ret;

/////////////////////////////////////////////////////////////////////////
class ArduinoErrorCodesInitializer {
	public:
		ArduinoErrorCodesInitializer() {
			ArduinoErrorCodes::init();
		}
}; ArduinoErrorCodesInitializer aeci;

/////////////////////////////////////////////////////////////////////////
void ArduinoErrorCodes::init() {
/////////////////////////////////////////////////////////////////////////
	for (int i=0; i<MAX_ERROR_CODES -1;i++){
		errorCodes[i]  = "Unknow error code: ";
	}

	errorCodes[E_NO_ERROR]                           = "Current Error Count";
	errorCodes[E_UNKNOW_COMMAND]                     = "Arduino::loop(): Unknown Command";
	errorCodes[E_INVALID_PARAM_ID]                   = "Arduino::setValue(): Invalid parameter ID"; 
	errorCodes[E_INVALID_PARAM_STREAM]               = "Arduino::setValue(): Noting to read";
	errorCodes[E_GETTER_ID_NOT_FOUND]                = "Arduino::getValue(): Cant read getter id";
	errorCodes[E_INVALID_GETTER_ID]                  = "Arduino::getValue(): Getter id not known"; 
	errorCodes[E_INVALID_GETTER_LIST_COUNT]          = "Arduino::getValues(): Getter list count not available";
	errorCodes[E_INVALID_MOVE_CMD]                   = "Arduino::decodeMove(): Cant read long from Serial: invalid size: ";
	
	errorCodes[E_INVALID_TEST_ID]                    = "Arduino::devodeTest(): Invalid Test id";
	errorCodes[E_NOT_KNOWN_TEST_ID]                  = "Arduino::TestSuite::run(): Not known Test id: ";
	errorCodes[E_INVALID_PARAMETER]                  = "Arduino::TestSuite::run(): Invalid parameter nr: ";
	
	errorCodes[E_STEPPER_NOT_ENALED]                 = "Arduino::stepAxisXYZ: Stepper not enaled";
	errorCodes[E_STEPPER_NOT_INITIALIZED]            = "Arduino::stepAxisXYZ: Stepper not initialized";
	errorCodes[E_TOOL_NOT_ENALED]                    = "Arduino::stepAxisXYZ: Tool not enabled";
	
	errorCodes[E_STEPPER_PULS_WIDTH_TO_LARGE]        = "Arduino::recalcDriverConfig(): Value to large";
	errorCodes[E_STEPPER_PULS_WIDTH_OFFSET_TO_LARGE] = "Arduino::setPulsWidthOffset(): Value to large";
	
	errorCodes[E_INTERRUPT]                          = "Interrupt received";
	errorCodes[E_TOTAL_COUNT]                        = "Total error count";
	
	errorCodes[E_PURE_TEXT_VALUE_ERROR]              = "Pure text message";
	errorCodes[E_SPEED_MGMT_NOT_INITIALIZED]         = "Speed Manager isn't initialized";
	errorCodes[E_STEPPER_NOT_READY_TO_RUN]           = "Stepper not ready to run";
	
}
/////////////////////////////////////////////////////////////////////////
const char* ArduinoErrorCodes::getECLabel(unsigned int id) {
// not thread safe version - use instead getECLabel(unsigned int id, std::string& retVal) 
/////////////////////////////////////////////////////////////////////////
	return getECLabel(id, ArduinoErrorCodes::ret);
}
/////////////////////////////////////////////////////////////////////////
const char* ArduinoErrorCodes::getECLabel(unsigned int id, std::string& retVal) {
// thread safe version
/////////////////////////////////////////////////////////////////////////
	if ( id >= 0 && id < MAX_ERROR_CODES )
		return errorCodes[id].c_str();

	retVal.assign("EC [");
	retVal.append(std::to_string(id));
	retVal.append("] is out of range");
	return retVal.c_str();
}

std::string ArduinoDigitalPins::pins[MAX_PINS];
std::string ArduinoDigitalPins::ret;
std::string ArduinoAnalogPins::pins[MAX_PINS];
std::string ArduinoAnalogPins::ret;

/////////////////////////////////////////////////////////////////////////
class ArduinoPinsInitializer {
	public:
		ArduinoPinsInitializer() {
			ArduinoDigitalPins::init();
			ArduinoAnalogPins::init();
		}
}; ArduinoPinsInitializer apini;

/////////////////////////////////////////////////////////////////////////
void ArduinoDigitalPins::init() {
/////////////////////////////////////////////////////////////////////////
	for (int i=0; i<MAX_PINS -1;i++){
		std::string s("Unknown PIN: ");
		s += std::to_string(i);
		pins[i]  = s.c_str();
	}

	pins[PIN_X_STP]              = "STEP PIN X";
	pins[PIN_Y_STP]              = "STEP PIN Y";
	pins[PIN_Z_STP]              = "STEP PIN Z";
	pins[PIN_X_DIR]              = "DIR PIN X";
	pins[PIN_Y_DIR]              = "DIR PIN Y";
	pins[PIN_Z_DIR]              = "DIR PIN Z";
	
	pins[PIN_STEPPER_ENABLE]     = "STEPPER ENABLE PIN";
	
	pins[PIN_X_LIMIT]            = "LIMIT PIN X";
	pins[PIN_Y_LIMIT]            = "LIMIT PIN Y";
	pins[PIN_Z_LIMIT]            = "LIMIT PIN Z";
	
	pins[PIN_TOOL_ENABLE]        = "TOOL ENABLE PIN";
	pins[PIN_TOOL_FEEDBACK]      = "TOOL FEEDBACK PIN";
}
/////////////////////////////////////////////////////////////////////////
const char* ArduinoDigitalPins::getPinLabel(unsigned int id) {
// not thread safe version - use instead getPinLabel(unsigned int id, std::string& retVal) 
/////////////////////////////////////////////////////////////////////////
	return getPinLabel(id, ArduinoDigitalPins::ret);
}
/////////////////////////////////////////////////////////////////////////
const char* ArduinoDigitalPins::getPinLabel(unsigned int id, std::string& retVal) {
// thread safe version
/////////////////////////////////////////////////////////////////////////
	if ( id >= 0 && id < MAX_PINS )
		return pins[id].c_str();

	retVal.assign("PIN [");
	retVal.append(std::to_string(id));
	retVal.append("] is out of range");
	
	return ret.c_str();
}
/////////////////////////////////////////////////////////////////////////
void ArduinoAnalogPins::init() {
/////////////////////////////////////////////////////////////////////////
	for (int i=0; i<MAX_PINS -1;i++){
		std::string s("Unknown PIN: ");
		s += std::to_string(i);
		pins[i]  = s.c_str();
	}

	pins[PIN_INTERRUPT_LED_ID]   = "INTERRUPT LED PIN";
}
/////////////////////////////////////////////////////////////////////////
const char* ArduinoAnalogPins::getPinLabel(unsigned int id) {
// not thread safe version - use instead getPinLabel(unsigned int id, std::string& retVal) 
/////////////////////////////////////////////////////////////////////////
	return getPinLabel(id, ArduinoAnalogPins::ret);
}
/////////////////////////////////////////////////////////////////////////
const char* ArduinoAnalogPins::getPinLabel(unsigned int id, std::string& retVal) {
// thread safe version
/////////////////////////////////////////////////////////////////////////
	if ( id >= 0 && id < MAX_PINS )
		return pins[id].c_str();

	retVal.assign("PIN [");
	retVal.append(std::to_string(id));
	retVal.append("] is out of range");
	
	return ret.c_str();
}
