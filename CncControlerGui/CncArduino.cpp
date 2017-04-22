
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
	cmds[CMD_INTERRUPPT]            = "Push Interrupt";
	cmds[CMD_IDLE]                  = "Pull Idle Callback";
	cmds[CMD_RESET_CONTROLLER]      = "Push Reset Congtroller";
	cmds[CMD_RESET_ERRORINFO]       = "Push Reset Error Info";

	cmds[CMD_SETTER]                = "Push Setter";
	cmds[CMD_GETTER]                = "Pull Getter";

	cmds[CMD_ENABLE_STEPPER_PIN]    = "Push Enable Stepper Motors";
	cmds[CMD_DISABLE_STEPPER_PIN]   = "Push Disable Stepper Motors";

	cmds[CMD_PAUSE_START]           = "Push Pause Start";
	cmds[CMD_PAUSE_STOP]            = "Push Pause Stop";

	cmds[CMD_POS_STEP_X]            = "Push Positive X Step";
	cmds[CMD_NEG_STEP_X]            = "Push Negative X Step";
	cmds[CMD_POS_STEP_Y]            = "Push Positive Y Step";
	cmds[CMD_NEG_STEP_Y]            = "Push Negative Y Step";
	cmds[CMD_POS_STEP_Z]            = "Push Positive Z Step";
	cmds[CMD_NEG_STEP_Z]            = "Push Negative Z Step";

	cmds[CMD_MOVE]                  = "Push 3D Move";
	cmds[CMD_RENDER_AND_MOVE]       = "Push Render and 3D Move";

	cmds[CMD_TEST_START]            = "Push Pause Start";
	cmds[CMD_TEST_STOP]             = "Push Pause Stop";

	cmds[CMD_PRINT_CONFIG]          = "Pull Configuration";
	cmds[CMD_PRINT_VERSION]         = "Pull Version";
	cmds[CMD_PRINT_PIN_REPORT]      = "Pull PIN Report";
	cmds[CMD_PRINT_ERRORINFO]       = "Pull Error Info";
	
	cmds[CMD_ENABLE_PROBE_MODE]     = "Push Enable Probe Mode";
	cmds[CMD_DISABLE_PROBE_MODE]    = "Push Disable Probe Mode";
} 
/////////////////////////////////////////////////////////////////////////
const char* ArduinoCMDs::getCMDLabel(unsigned int id) {
/////////////////////////////////////////////////////////////////////////
	if ( id >= 0 && id < MAX_CMDS )
		return cmds[id].c_str();

	ret = "C [";
	ret += std::to_string(id);
	ret += "] is out of range";
	
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

/////////////////////////////////////////////////////////////////////////
void ArduinoPIDs::init() {
/////////////////////////////////////////////////////////////////////////
	for (int i=0; i<MAX_PIDS -1;i++){
		std::string s("Unknown PID: ");
		s += std::to_string(i);
		pids[i]  = s.c_str();
	}

	pids[PID_UNKMOWN]                         = "Default PID";
	pids[PID_STEPPER_INITIALIZED]             = "Stepper initialize state";
	pids[PID_SPEED_X]                         = "Speed X axis";
	pids[PID_SPEED_Y]                         = "Speed Y axis";
	pids[PID_SPEED_Z]                         = "Speed Z axis";
	pids[PID_STEPS_X]                         = "Steps X axis";
	pids[PID_STEPS_Y]                         = "Steps Y axis";
	pids[PID_STEPS_Z]                         = "Steps Z axis";
	
	pids[PID_PITCH]                           = "Pitch";
	pids[PID_PITCH_X]                         = "Pitch X Axis";
	pids[PID_PITCH_Y]                         = "Pitch Y Axis";
	pids[PID_PITCH_Z]                         = "Pitch Z Axis";
	
	pids[PID_STEP_MULTIPLIER_X]               = "Step multipier X axis"; 
	pids[PID_STEP_MULTIPLIER_Y]               = "Step multipier Y axis"; 
	pids[PID_STEP_MULTIPLIER_Z]               = "Step multipier Z axis"; 
	
	pids[PID_MIN_SWITCH]                      = "Min switch";  
	pids[PID_MAX_SWITCH]                      = "Max switch";
	pids[PID_LIMIT]                           = "Limit state";
	pids[PID_LAST_STEP_DIR]                   = "Last step direction";
	pids[PID_X_LIMIT]                         = "X limit state";
	pids[PID_Y_LIMIT]                         = "Y limit state";
	pids[PID_Z_LIMIT]                         = "Z limit state";

	pids[PID_CONTROLLER]                      = "Controller";
	pids[PID_ROUTER_SWITCH]                   = "Tool switch";
	pids[PID_POS_REPLY_THRESHOLD]             = "Position reply threshold";
	pids[PID_POS_REPLY_COUNTER]               = "Position pelay counter";
	pids[PID_MIN_ENABLE_PULSE_WIDTH]          = "Stepper Driver Enbable Pulse Width";
	pids[PID_PROBE_MODE]                      = "Probe Mode";

	pids[PID_XYZ_POS]                         = "XYZ Pos";
	pids[PID_XY_POS]                          = "XY Pos";
	pids[PID_X_POS]                           = "X Pos";
	pids[PID_Y_POS]                           = "Y Pos";
	pids[PID_Z_POS]                           = "Z Pos";

	pids[PID_STEP_SIGN]                       = "Step Sign";
	pids[PID_X_STEP_SIGN]                     = "X Step Sign";
	pids[PID_Y_STEP_SIGN]                     = "Y Step Sign";

	pids[PID_AXIS]                            = "Stepper Axis";
	pids[PID_COMMON]                          = "Common";
	pids[PID_SPEED]                           = "Speed";
	pids[PID_STEPS]                           = "Steps";
	pids[PID_STEP_MULTIPLIER]                 = "Multiplier";
	pids[PID_STEP_PIN]                        = "Step pin";
	pids[PID_DIR_PIN]                         = "Direction pin";
	pids[PID_ENABLE_STEPPERS]                 = "Stepper enable state";
	pids[PID_AVG_STEP_DURRATION]              = "AVG Step duration [us]";
	
	pids[PID_SDRV_TYPE]                       = "Stepper Driver Type";
	pids[PID_SDRV_MIN_STEP_PULSE_WIDTH_LOW]   = "Stepper Driver Min Step Pulse Width [LOW]";
	pids[PID_SDRV_MIN_STEP_PULSE_WIDTH_HIGH]  = "Stepper Driver Min Step Pulse Width [HIGH]";
	pids[PID_SDRV_MIN_DIR_PULSE_WIDTH]        = "Stepper Driver Min Dir Pulse Width";
	pids[PID_SDRV_PULS_WITDH_OFFSET_X]        = "Stepper Driver Puls Width Offset X Axis";
	pids[PID_SDRV_PULS_WITDH_OFFSET_Y]        = "Stepper Driver Puls Width Offset Y Axis";
	pids[PID_SDRV_PULS_WITDH_OFFSET_Z]        = "Stepper Driver Puls Width Offset Z Axis";
	
	pids[PID_CURRENT_STEP_PULS_WIDTH_LOW]     = "Current Step Puls Width Low";
	pids[PID_CURRENT_STEP_PULS_WIDTH_HIGH]    = "Current Step Puls Width High";
	pids[PID_CURRENT_DIR_PULS_WIDTH]          = "Current Dir Puls Width";
	
	pids[PID_SWITCH_MOVE_MODE_STATE]          = "Move Mode Control State";

	pids[PID_TEST_SUITE]                      = "Test Suite";
	pids[PID_TEST_VALUE1]                     = "Parmeter1";
	pids[PID_TEST_VALUE2]                     = "Parmeter2";
	pids[PID_TEST_VALUE3]                     = "Parmeter3";
	pids[PID_TEST_VALUE4]                     = "Parmeter4";
	pids[PID_TEST_VALUE5]                     = "Parmeter5";
	
	pids[PID_TEST_INTERRUPT]                  = "Test Interrupt Flag";
	
	pids[PID_ERROR_COUNT]                     = "Error Count";
	pids[PID_VERSION]                         = "Firmware Version";
} 
/////////////////////////////////////////////////////////////////////////
const char* ArduinoPIDs::getPIDLabel(unsigned int id) {
/////////////////////////////////////////////////////////////////////////
	if ( id >= 0 && id < MAX_PIDS )
		return pids[id].c_str();

	ret = "PID [";
	ret += std::to_string(id);
	ret += "] is out of range";
	
	return ret.c_str();
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

	errorCodes[E_NO_ERROR]                           = "No Error";
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
}
/////////////////////////////////////////////////////////////////////////
const char* ArduinoErrorCodes::getECLabel(unsigned int id) {
/////////////////////////////////////////////////////////////////////////
	if ( id >= 0 && id < MAX_ERROR_CODES )
		return errorCodes[id].c_str();

	ret = "EC [";
	ret += std::to_string(id);
	ret += "] is out of range";
	return ret.c_str();
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
/////////////////////////////////////////////////////////////////////////
	if ( id >= 0 && id < MAX_PINS )
		return pins[id].c_str();

	ret = "PIN [";
	ret += std::to_string(id);
	ret += "] is out of range";
	
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

	pins[PID_UNKMOWN]              = "Default PIN";
	pins[INTERRUPT_LED_ID]         = "Interrupt LED";
}
/////////////////////////////////////////////////////////////////////////
const char* ArduinoAnalogPins::getPinLabel(unsigned int id) {
/////////////////////////////////////////////////////////////////////////
	if ( id >= 0 && id < MAX_PINS )
		return pins[id].c_str();

	ret = "PIN [";
	ret += std::to_string(id);
	ret += "] is out of range";
	
	return ret.c_str();
}