
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
	
	cmds[CMD_IDLE]                  = "Pull Idle Callback";
	cmds[CMD_RESET_CONTROLLER]      = "Push Reset Controller";
	cmds[CMD_RESET_ERRORINFO]       = "Push Reset Error Info";
	
	cmds[CMD_SETTER]                = "Push Setter";
	cmds[CMD_GETTER]                = "Pull Getter";
	
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
	pids[RET_MSG]                             = "RET_MSG";
	
	pids[PID_UNKNOWN]                         = "Default PID";
	pids[PID_STEPPER_INITIALIZED]             = "Stepper initialize state";
	
	pids[PID_QUERY_READY_TO_RUN]              = "Ready to run";
	
	pids[PID_SPEED_OFFSET]                    = "Speed step offset [us]";
	pids[PID_SPEED_OFFSET_X]                  = "Speed step offset X axis [us]";
	pids[PID_SPEED_OFFSET_Y]                  = "Speed step offset Y axis [us]";
	pids[PID_SPEED_OFFSET_Z]                  = "Speed step offset Z axis [us]";
	pids[PID_STEPS_X]                         = "Steps X axis";
	pids[PID_STEPS_Y]                         = "Steps Y axis";
	pids[PID_STEPS_Z]                         = "Steps Z axis";
	
	pids[PID_PITCH]                           = "Pitch";
	pids[PID_PITCH_X]                         = "Pitch X Axis";
	pids[PID_PITCH_Y]                         = "Pitch Y Axis";
	pids[PID_PITCH_Z]                         = "Pitch Z Axis";
	
	pids[PID_MIN_SWITCH]                      = "Min switch";  
	pids[PID_MAX_SWITCH]                      = "Max switch";
	pids[PID_LIMIT]                           = "Limit state";
	pids[PID_LAST_STEP_DIR]                   = "Last step direction";
	pids[PID_X_LIMIT]                         = "X limit state";
	pids[PID_Y_LIMIT]                         = "Y limit state";
	pids[PID_Z_LIMIT]                         = "Z limit state";

	pids[PID_CONTROLLER]                      = "Controller";
	pids[PID_ROUTER_SWITCH]                   = "Tool switch";
	pids[PID_POS_REPLY_THRESHOLD_X]           = "Position reply threshold X axis";
	pids[PID_POS_REPLY_THRESHOLD_Y]           = "Position reply threshold Y axis";
	pids[PID_POS_REPLY_THRESHOLD_Z]           = "Position reply threshold Z axis";
	pids[PID_MIN_ENABLE_PULSE_WIDTH]          = "Stepper Driver Enbable Pulse Width";
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
	pids[PID_STEP_PIN]                        = "Step pin";
	pids[PID_DIR_PIN]                         = "Direction pin";
	pids[PID_ENABLE_STEPPERS]                 = "Stepper enable state";
	pids[PID_AVG_STEP_DURRATION]              = "AVG Step duration [us]";
	
	pids[PID_CURRENT_STEP_PULSE_WIDTH_LOW]    = "Current Step Pulse Width Low";
	pids[PID_CURRENT_STEP_PULSE_WIDTH_HIGH]   = "Current Step Pulse Width High";
	pids[PID_CURRENT_DIR_PULSE_WIDTH]         = "Current Dir Pulse Width";
	
	pids[PID_PULSE_WIDTH_OFFSET]              = "Min Pulse Width Offset";
	pids[PID_PULSE_WIDTH_OFFSET_X]            = "Min Pulse Width Offset X";
	pids[PID_PULSE_WIDTH_OFFSET_Y]            = "Min Pulse Width Offset Y";
	pids[PID_PULSE_WIDTH_OFFSET_Z]            = "Min Pulse Width Offset Z";
	
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
	
	pids[PID_SPEED_MGMT]                      = "Speed Manager Parameters";
	pids[PID_SPEED_MGMT_INITIALIZED]          = "Initialized state";
	pids[PID_SPEED_MGMT_LOW_PULSE_WIDTH]      = "Low Pulse Width";
	pids[PID_SPEED_MGMT_HIGH_PULSE_WIDTH]     = "High Pulse Width";
	pids[PID_SPEED_MGMT_TOTAL_OFFSET]         = "Total Speed Offset";
	pids[PID_SPEED_MGMT_PER_SETP_OFFSET]      = "Per Step Speed Offset";
	pids[PID_SPEED_MGMT_MAX_SPEED]            = "Max Speed";
	
	pids[PID_MAX_DIMENSION_X]                 = "Max Dimension X [Steps]";
	pids[PID_MAX_DIMENSION_Y]                 = "Max Dimension Y [Steps]";
	pids[PID_MAX_DIMENSION_Z]                 = "Max Dimension Z [Steps]";
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
		if ( pids[id].find(UNKNOWN_PID) == string::npos )
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
	errorCodes[E_INVALID_MOVE_CMD]                   = "Arduino::decodeMove(): Cant read long from Serial: invalid size: ";
	
	errorCodes[E_INVALID_TEST_ID]                    = "Arduino::devodeTest(): Invalid Test id";
	errorCodes[E_NOT_KNOWN_TEST_ID]                  = "Arduino::TestSuite::run(): Not known Test id: ";
	errorCodes[E_INVALID_PARAMETER]                  = "Arduino::TestSuite::run(): Invalid parameter nr: ";
	
	errorCodes[E_STEPPER_NOT_ENALED]                 = "Arduino::stepAxis: Stepper not enaled: ";
	errorCodes[E_STEPPER_NOT_INITIALIZED]            = "Arduino::stepAxis: Stepper not initialized: ";
	
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

	pins[X_STP]              = "STEP PIN X";
	pins[Y_STP]              = "STEP PIN Y";
	pins[Z_STP]              = "STEP PIN Z";
	pins[X_DIR]              = "DIR PIN X";
	pins[Y_DIR]              = "DIR PIN Y";
	pins[Z_DIR]              = "DIR PIN Z";
	
	pins[ENABLE_PIN]         = "STEPPER MOTOR ENABLE PIN";
	
	pins[X_LIMIT]            = "LIMIT PIN X";
	pins[Y_LIMIT]            = "LIMIT PIN Y";
	pins[Z_LIMIT]            = "LIMIT PIN Z";
	
	pins[TOOL_PIN]           = "TOOL ENABLE PIN";
	pins[SUPPORT_PIN]        = "SUPPORT PIN";
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

	pins[PID_UNKNOWN]              = "Default PIN";
	pins[INTERRUPT_LED_ID]         = "Interrupt LED";
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
