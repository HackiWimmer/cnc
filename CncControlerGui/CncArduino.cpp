
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
	
	cmds[CMD_SETTER]                = "Push Setter";
	cmds[CMD_GETTER]                = "Pull Getter";
	
	cmds[CMD_POS_STEP_X]            = "Push Positive X Step";
	cmds[CMD_NEG_STEP_X]            = "Push Negative X Step";
	cmds[CMD_POS_STEP_Y]            = "Push Positive Y Step";
	cmds[CMD_NEG_STEP_Y]            = "Push Negative Y Step";
	cmds[CMD_POS_STEP_Z]            = "Push Positive Z Step";
	cmds[CMD_NEG_STEP_Z]            = "Push Negative Z Step";
	
	cmds[CMD_MOVE]                  = "Push 3D Move";
	cmds[CMD_RENDER_AND_MOVE]       = "Push Render and 3D Move";
	cmds[CMD_MOVE_UNIT_SIGNAL]      = "Push Render and 3D Move until Signal";

	cmds[CMD_PRINT_CONFIG]          = "Pull Configuration";
	cmds[CMD_PRINT_VERSION]         = "Pull Version";
	cmds[CMD_PRINT_PIN_REPORT]      = "Pull PIN Report";
	
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

ArduinoPIDs::PidInfo ArduinoPIDs::pids[MAX_PIDS];
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
		pids[i].setup(s.c_str());
	}
	
	pids[RET_NULL]                            .setup("RET_NULL", "");
	pids[RET_OK]                              .setup("RET_OK", "");
	pids[RET_ERROR]                           .setup("RET_ERROR", "");
	pids[RET_LIMIT]                           .setup("RET_LIMIT", "");
	pids[RET_SOT]                             .setup("RET_SOT", "");
	pids[RET_SOH]                             .setup("RET_SOH", "");
	pids[RET_INTERRUPT]                       .setup("RET_INTERRUPT", ""); 
	pids[RET_HALT]                            .setup("RET_HALT", ""); 
	pids[RET_QUIT]                            .setup("RET_QUIT", "");
	pids[RET_MSG]                             .setup("RET_MSG", "");
	
	pids[PID_UNKNOWN]                         .setup("Default PID", "");
	pids[PID_STEPPER_INITIALIZED]             .setup("Stepper Initialize State", "bool");
	
	pids[PID_QUERY_READY_TO_RUN]              .setup("Ready to run", "bool");
	
	pids[PID_STEPS_X]                         .setup("Steps X Axis", "steps");
	pids[PID_STEPS_Y]                         .setup("Steps Y Axis", "steps");
	pids[PID_STEPS_Z]                         .setup("Steps Z Axis", "steps");
	
	pids[PID_PITCH]                           .setup("Pitch", "mm");
	pids[PID_PITCH_X]                         .setup("Pitch X Axis", "mm");
	pids[PID_PITCH_Y]                         .setup("Pitch Y Axis", "mm");
	pids[PID_PITCH_Z]                         .setup("Pitch Z Axis", "mm");
	
	pids[PID_MIN_SWITCH]                      .setup("Min Switch", "bool");  
	pids[PID_MAX_SWITCH]                      .setup("Max Switch", "bool");
	pids[PID_LIMIT]                           .setup("Limit State", "bool");
	pids[PID_LAST_STEP_DIR]                   .setup("Last Step Direction", "");
	pids[PID_X_LIMIT]                         .setup("X limit State", "bool");
	pids[PID_Y_LIMIT]                         .setup("Y limit State", "bool");
	pids[PID_Z_LIMIT]                         .setup("Z limit State", "bool");

	pids[PID_I2C_AVAILABEL  ]                 .setup("I2C Availabel", "bool");
	pids[PID_I2C_LIMIT_VALUE]                 .setup("I2C Limit Value", "-");
	pids[PID_I2C_SUPPORT_VALUE]               .setup("I2C Support Value", "-");
	
	pids[PID_CONTROLLER]                      .setup("Cnc Controller", "");
	pids[PID_TOOL_SWITCH]                     .setup("Tool Enabled State", "bool");
	pids[PID_POS_REPLY_THRESHOLD_X]           .setup("Position Reply Threshold X Axis", "steps");
	pids[PID_POS_REPLY_THRESHOLD_Y]           .setup("Position Reply Threshold Y Axis", "steps");
	pids[PID_POS_REPLY_THRESHOLD_Z]           .setup("Position Reply Threshold Z Axis", "steps");
	pids[PID_PROBE_MODE]                      .setup("Probe Mode State", "bool");

	pids[PID_XYZ_POS_MAJOR]                   .setup("XYZ Pos - Type Major", "steps");
	pids[PID_XYZ_POS_DETAIL]                  .setup("XYZ Pos - Type Detail", "steps");
	pids[PID_XYZ_POS]                         .setup("XYZ Pos", "steps");
	pids[PID_XY_POS]                          .setup("XY Pos", "steps");
	pids[PID_X_POS]                           .setup("X Pos", "steps");
	pids[PID_Y_POS]                           .setup("Y Pos", "steps");
	pids[PID_Z_POS]                           .setup("Z Pos", "steps");

	pids[PID_AXIS]                            .setup("Stepper Axis", "");
	pids[PID_COMMON]                          .setup("Common", "");
	pids[PID_SPEED_MM_MIN]                    .setup("Speed", "mm/min");
	pids[PID_SPEED_FEED_MODE]                 .setup("Speed Mode", "-");
	pids[PID_STEPS]                           .setup("Steps", "steps");
	pids[PID_STEP_PIN]                        .setup("Step Pin", "");
	pids[PID_DIR_PIN]                         .setup("Direction Pin", "");
	pids[PID_ENABLE_STEPPERS]                 .setup("Stepper Enable State", "");
	pids[PID_AVG_STEP_DURATION]               .setup("AVG Step Loop Duration", "us");

	pids[PID_PULSE_WIDTH_OFFSET_DIR]          .setup("Direction Pulse Width", "us");
	pids[PID_PULSE_WIDTH_OFFSET_LOW]          .setup("Step Pulse Width Low", "us");
	pids[PID_PULSE_WIDTH_OFFSET_HIGH]         .setup("Step Pulse Width High", "us");
	pids[PID_PULSE_WIDTH_LOW_X]               .setup("Step Pulse Width Low X", "us");
	pids[PID_PULSE_WIDTH_LOW_Y]               .setup("Step Pulse Width Low Y", "us");
	pids[PID_PULSE_WIDTH_LOW_Z]               .setup("Step Pulse Width Low Z", "us");
	pids[PID_PULSE_WIDTH_HIGH_X]              .setup("Step Pulse Width High X", "us");
	pids[PID_PULSE_WIDTH_HIGH_Y]              .setup("Step Pulse Width High Y", "us");
	pids[PID_PULSE_WIDTH_HIGH_Z]              .setup("Step Pulse Width High Z", "us");
	
	pids[PID_ACCEL_PROFILE]                   .setup("Accel Profile X,Y,Z(startSpeed, stopSpeed)", "mm/min");
	pids[PID_ACCEL_START_SPEED]               .setup("Accel start speed", "mm/min");
	pids[PID_ACCEL_STOP_SPEED]                .setup("Accel stop speed", "mm/min");
	
	pids[PID_VERSION]                         .setup("Firmware Version", "-");
	pids[PID_SEPARATOR]                       .setup("Separator", "");
	
	pids[PID_RESERT_POS_COUNTER]              .setup("Reset Position Counter", "");
	pids[PID_GET_POS_COUNTER]                 .setup("Get Position Counter", "#");
	pids[PID_RESERT_STEP_COUNTER]             .setup("Reset Step Counter", "");
	pids[PID_GET_STEP_COUNTER_X]              .setup("Get Step Counter X", "#");
	pids[PID_GET_STEP_COUNTER_Y]              .setup("Get Step Counter Y", "#");
	pids[PID_GET_STEP_COUNTER_Z]              .setup("Get Step Counter Z", "#");
	
	pids[PID_SPEED_CONTROLLER]                .setup("Feed Speed Controller", "");
	pids[PID_SPEED_CTRL_INITIALIZED]          .setup("Initialized State", "bool");
	pids[PID_SPEED_CTRL_RPM]                  .setup("Rounds Per Minute", "rpm");
	pids[PID_SPEED_CTRL_TOTAL_OFFSET]         .setup("Total Speed Offset", "us");
	pids[PID_SPEED_CTRL_SYNTH_SPEED_DELAY]    .setup("Synthetically Speed Delay", "us");
	pids[PID_SPEED_CTRL_MAX_SPEED]            .setup("Max Speed", "mm/min");
	pids[PID_SPEED_CTRL_CFG_SPEED]            .setup("Configured Speed", "mm/min");
	
	pids[PID_MAX_DIMENSION_X]                 .setup("Max Dimension X", "steps");
	pids[PID_MAX_DIMENSION_Y]                 .setup("Max Dimension Y", "steps");
	pids[PID_MAX_DIMENSION_Z]                 .setup("Max Dimension Z", "steps");
	
	pids[PID_INCREMENT_DIRECTION_VALUE_X]     .setup("Invert Dircetion X", "bool");
	pids[PID_INCREMENT_DIRECTION_VALUE_Y]     .setup("Invert Dircetion Y", "bool");
	pids[PID_INCREMENT_DIRECTION_VALUE_Z]     .setup("Invert Dircetion Z", "bool");
} 
/////////////////////////////////////////////////////////////////////////
const char* ArduinoPIDs::getPIDLabel(unsigned int pid) {
// not thread safe version - use instead getPIDLabel(unsigned int id, std::string& retVal) 
/////////////////////////////////////////////////////////////////////////
	return getPIDLabel(pid, ret);
}
/////////////////////////////////////////////////////////////////////////
const char* ArduinoPIDs::getPIDLabel(unsigned int pid, std::string& retVal) {
// thread safe version
/////////////////////////////////////////////////////////////////////////
	if ( pid >= 0 && pid < MAX_PIDS ) 
		return pids[pid].label.c_str();
	
	retVal.assign("PID [");
	retVal.append(std::to_string(pid));
	retVal.append("] is out of range");
	
	return retVal.c_str();
}
/////////////////////////////////////////////////////////////////////////
const char* ArduinoPIDs::getPIDLabelWithDefault(unsigned int id, const std::string& defaultValue) {
/////////////////////////////////////////////////////////////////////////
	if ( exists(id) == true )
		return pids[id].label.c_str();
	
	return defaultValue.c_str();
}
/////////////////////////////////////////////////////////////////////////
bool ArduinoPIDs::exists(unsigned int pid) {
/////////////////////////////////////////////////////////////////////////
	if ( pid >= 0 && pid < MAX_PIDS ) {
		if ( pids[pid].label.find(UNKNOWN_PID) == std::string::npos )
			return true;
	}
	
	return false;
}
/////////////////////////////////////////////////////////////////////////
const char* ArduinoPIDs::getPIDUnit(unsigned int pid) {
/////////////////////////////////////////////////////////////////////////
	if ( exists(pid) ) {
		return pids[pid].unit.c_str();
	}
	
	return "";
}
/////////////////////////////////////////////////////////////////////////
const char* ArduinoPIDs::getPIDUnit(unsigned int pid, std::string& retVal) {
/////////////////////////////////////////////////////////////////////////
	if ( exists(pid) ) {
		retVal = pids[pid].unit;
		return pids[pid].unit.c_str();
	}
	
	retVal = "";
	return "";
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
	errorCodes[E_INVALID_PARAM_STREAM_LEN]           = "Arduino::setValue(): Invalid stream length";
	errorCodes[E_GETTER_ID_NOT_FOUND]                = "Arduino::getValue(): Cant read getter id";
	errorCodes[E_INVALID_GETTER_ID]                  = "Arduino::getValue(): Getter id not known"; 
	errorCodes[E_INVALID_GETTER_LIST_COUNT]          = "Arduino::getValues(): Getter list count not available";
	errorCodes[E_INVALID_MOVE_CMD]                   = "Arduino::decodeMove(): Cant read long from Serial: invalid size: ";
	
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
