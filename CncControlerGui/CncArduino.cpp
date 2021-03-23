
#include "CncArduino.h"

std::string ArduinoCMDs::cmds[MAX_CMDS];
std::string ArduinoCMDs::ret;

const char* UNKNOWN_CMD = "Unknown CMD: ";
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
		std::string s(UNKNOWN_CMD);
		s += std::to_string(i);
		cmds[i]  = s.c_str();
	}
	
	cmds[CMD_INVALID]                          = "Invalid Command";
	
	cmds[CMD_PUSH_TRANSACTION]                 = "Push Transaction";
	cmds[CMD_POP_TRANSACTION]                  = "Pop Transaction";
	
	cmds[SIG_INTERRUPPT]                       = "Push Signal Interrupt";
	cmds[SIG_HALT]                             = "Push Signal Pause";
	cmds[SIG_PAUSE]                            = "Push Signal Pause";
	cmds[SIG_RESUME]                           = "Push Signal Resume";
	cmds[SIG_QUIT_MOVE]                        = "Push Signal Quit Move";
	cmds[SIG_UPDATE]                           = "Push Signal Update";
	
	cmds[CMD_HEARTBEAT]                        = "Pull Heartbeat Callback";
	cmds[CMD_IDLE]                             = "Pull Idle Callback";
	cmds[CMD_POP_SERIAL]                       = "Pop Serial";
	cmds[CMD_RESET_CONTROLLER]                 = "Push Reset Controller";
	
	cmds[CMD_SETTER]                           = "Push Setter";
	cmds[CMD_GETTER]                           = "Pull Getter";
	
	cmds[CMD_MOVE]                             = "Push Move";
	cmds[CMD_RENDER_AND_MOVE]                  = "Push Render and Move";
	cmds[CMD_MOVE_UNTIL_CONTACT]               = "Push Move until contact reached";
	cmds[CMD_MOVE_UNTIL_LIMIT_IS_FREE]         = "Push Move until limit switches are free";
	
	cmds[CMD_MOVE_INTERACTIVE]                 = "Push Move Interactive";

	cmds[CMD_MOVE_SEQUENCE]                    = "Push Move Sequence";
	cmds[CMD_RENDER_AND_MOVE_SEQUENCE]         = "Push Render and Move Sequence";
	
	cmds[CMD_ACTIVATE_PODEST_HW]               = "Activate Podest Hardware";
	cmds[CMD_DEACTIVATE_PODEST_HW]             = "Deactivate Podest Hardware";
	cmds[CMD_MOVE_PODEST]                      = "Move Podest";
	cmds[CMD_MOVE_PODEST_EXACT]                = "Move Podest Exact";
	
	cmds[CMD_PRINT_CONFIG]                     = "Pull Configuration";
	cmds[CMD_PRINT_VERSION]                    = "Pull Version";
	cmds[CMD_PRINT_PIN_REPORT]                 = "Pull PIN Report";
	
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
/////////////////////////////////////////////////////////////////////////
const char* ArduinoCMDs::getCMDLabelWithDefault(unsigned int id, const std::string& defaultValue) {
/////////////////////////////////////////////////////////////////////////
	if ( exists(id) == true )
		return cmds[id].c_str();
	
	return defaultValue.c_str();
}
/////////////////////////////////////////////////////////////////////////
bool ArduinoCMDs::exists(unsigned int cmd) {
/////////////////////////////////////////////////////////////////////////
	if ( cmd >= 0 && cmd < MAX_CMDS ) {
		if ( cmds[cmd].find(UNKNOWN_CMD) == std::string::npos )
			return true;
	}
	
	return false;
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
	pids[RET_MORE]                            .setup("RET_MORE", "");
	pids[RET_ERROR]                           .setup("RET_ERROR", "");
	pids[RET_LIMIT]                           .setup("RET_LIMIT", "");
	pids[RET_SOH]                             .setup("RET_SOH", "");
	pids[RET_INTERRUPT]                       .setup("RET_INTERRUPT", ""); 
	pids[RET_HALT]                            .setup("RET_HALT", ""); 
	pids[RET_QUIT]                            .setup("RET_QUIT", "");
	
	pids[PID_UNKNOWN]                         .setup("Default PID", "");

	pids[PID_HEARTBEAT]                       .setup("Heartbeat", "");
	pids[PID_MSG]                             .setup("Message", "");
	pids[PID_TEXT]                            .setup("Plain Text", "");
	pids[PID_GETTER]                          .setup("Getter", "");
	
	pids[PID_STEPPER_INITIALIZED]             .setup("Stepper Initialize State", "bool");
	pids[PID_QUERY_READY_TO_RUN]              .setup("Ready to run", "bool");
	
	pids[PID_FEEDRATE]                        .setup("Feedrate", "mm/step");
	pids[PID_FEEDRATE_H]                      .setup("Feedrate h Axis", "mm/step");

	
	pids[PID_MIN_SWITCH]                      .setup("Min Switch", "bool");  
	pids[PID_MAX_SWITCH]                      .setup("Max Switch", "bool");
	pids[PID_LIMIT]                           .setup("Limit State", "bool");
	pids[PID_X_LIMIT]                         .setup("X limit State", "bool");
	pids[PID_Y_LIMIT]                         .setup("Y limit State", "bool");
	pids[PID_Z_LIMIT]                         .setup("Z limit State", "bool");

	pids[PID_I2C_AVAILABEL  ]                 .setup("I2C Availabel", "bool");
	pids[PID_I2C_LIMIT_VALUE]                 .setup("I2C Limit Value", "BIN");
	pids[PID_I2C_SUPPORT_VALUE]               .setup("I2C Support Value", "BIN");
	
	pids[PID_CONTROLLER]                      .setup("Cnc Controller", "");
	pids[PID_SETUP_ID]                        .setup("Cnc Controller Setup ID", "");
	pids[PID_SPINDLE_SWITCH]                  .setup("Spindle Enabled State", "bool");
	pids[PID_POS_REPLY_THRESHOLD]             .setup("Position Reply Threshold", "impulses");
	pids[PID_PROBE_MODE]                      .setup("Probe Mode State", "bool");
	
	pids[PID_TOUCH_CONTACT_STATE]             .setup("Touch Contact State", "bool");

	pids[PID_XYZ_INTERACTIVE_POS]             .setup("XYZ Interactive Pos", "steps");
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
	pids[PID_SPEED_MM_SEC]                    .setup("Speed", "mm/sec");
	pids[PID_STEP_PIN]                        .setup("Step Pin", "");
	pids[PID_DIR_PIN]                         .setup("Direction Pin", "");
	pids[PID_ENABLE_STEPPERS]                 .setup("Stepper Enable State", "");
	pids[PID_SPINDLE_SPEED]                   .setup("Spindle Speed", "U/mm");
	
	pids[PID_PODEST_POS]                      .setup("Podest Position", "steps");
	
	pids[PID_PULSE_WIDTH_HIGH]                .setup("Step Pulse Width High", "us");
	pids[PID_PULSE_WIDTH_HIGH_H]              .setup("Step Pulse Width High H-Axis", "us");
	pids[PID_ACCEL_PROFILE]                   .setup("Accel Profile", "<mixed>");
	pids[PID_ACCEL_START_SPEED]               .setup("Accel start speed", "mm/min");
	pids[PID_ACCEL_STOP_SPEED]                .setup("Accel stop speed", "mm/min");
	
	pids[PID_VERSION]                         .setup("Firmware Version", "-");
	pids[PID_SEPARATOR]                       .setup("Separator", "");
	
	pids[PID_MAX_DIMENSION_X]                 .setup("Max Dimension X", "steps");
	pids[PID_MAX_DIMENSION_Y]                 .setup("Max Dimension Y", "steps");
	pids[PID_MAX_DIMENSION_Z]                 .setup("Max Dimension Z", "steps");
	
	pids[PID_INC_DIRECTION_VALUE_X]           .setup("Invert Dircetion X", "bool");
	pids[PID_INC_DIRECTION_VALUE_Y]           .setup("Invert Dircetion Y", "bool");
	pids[PID_INC_DIRECTION_VALUE_Z]           .setup("Invert Dircetion Z", "bool");
	pids[PID_INC_DIRECTION_VALUE_H]           .setup("Invert Dircetion H", "bool");
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
		errorCodes[i]  = "Unknown error code: ";
	}

	errorCodes[E_NO_ERROR]                           = "Current Error Count";
	errorCodes[E_UNKNOW_COMMAND]                     = "Arduino::loop(): Unknown Command";
	errorCodes[E_INVALID_PARAM_ID]                   = "Arduino::setValue(): Invalid parameter ID"; 
	errorCodes[E_INVALID_PARAM_STREAM]               = "Arduino::setValue(): Noting to read";
	errorCodes[E_INVALID_PARAM_STREAM_LEN]           = "Arduino::setValue(): Invalid stream length";
	errorCodes[E_GETTER_ID_NOT_FOUND]                = "Arduino::getValue(): Can't read getter id";
	errorCodes[E_INVALID_GETTER_ID]                  = "Arduino::getValue(): Getter id not known"; 
	errorCodes[E_INVALID_GETTER_LIST_COUNT]          = "Arduino::getValues(): Getter list count not available";
	errorCodes[E_INVALID_MOVE_CMD]                   = "Arduino::decodeMove(): Can't read a int32_t value from Serial: invalid size: ";
	errorCodes[E_OTHER_MOVE_CMD_ACTIVE]              = "Arduino::decodeMove(): Move rejected because a other move statement is currently active";
	errorCodes[E_INVALID_MOVE_SEQUENCE]              = "Arduino::decodeMoveSequence(): Invalid byte chain";
	errorCodes[E_INVALID_PARAM_SIZE]                 = "Arduino::decodeMoveSequence(): Can't read size (int32_t) from Serial: invalid size: ";
	errorCodes[E_AVOID_READY_TO_RUN]                 = "Arduino::avoidReadyToRun == true";
	
	errorCodes[E_INVALID_TRANSACTION_STATE]          = "Arduino::processPodest: Invalid Transaction state";
	errorCodes[E_PODEST_DIR_CHANGE_FAILED]           = "Arduino::processPodest: Direction change failed";
	errorCodes[E_PODEST_MOVE_FAILED]                 = "Arduino::processPodest: Move failed";
	
	errorCodes[E_STEPPER_NOT_ENABLED]                = "Arduino::stepAxisXYZ: Stepper not enabled";
	errorCodes[E_STEPPER_NOT_INITIALIZED]            = "Arduino::stepAxisXYZ: Stepper not initialized";
	errorCodes[E_SPINDLE_NOT_ENALED]                 = "Arduino::stepAxisXYZ: Spindle not enabled";
	
	errorCodes[E_STEPPER_PULS_WIDTH_TO_LARGE]        = "Arduino::recalcDriverConfig(): Value to large";
	errorCodes[E_STEPPER_PULS_WIDTH_OFFSET_TO_LARGE] = "Arduino::setPulsWidthOffset(): Value to large";
	
	errorCodes[E_LIMIT_SWITCH_ACTIVE]                = "Limit switch active";
	errorCodes[E_RMDR_INVALID_STP_POINTER]           = "ArduinoPositionRenderer: Invalid stepper pointers";
	
	errorCodes[E_INTERRUPT]                          = "Interrupt received";
	errorCodes[E_EXTERNEL_INTERRUPT]                 = "External Interrupt received";
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

	pins[PIN_X_STP]                 = "STEP PIN X";
	pins[PIN_Y_STP]                 = "STEP PIN Y";
	pins[PIN_Z_STP]                 = "STEP PIN Z";
	pins[PIN_X_DIR]                 = "DIR PIN X";
	pins[PIN_Y_DIR]                 = "DIR PIN Y";
	pins[PIN_Z_DIR]                 = "DIR PIN Z";
	
	pins[PIN_ENABLE_STEPPER]        = "STEPPER ENABLE PIN";
	
	pins[PIN_X_MIN_LIMIT]           = "LIMIT PIN X Min";
	pins[PIN_X_MAX_LIMIT]           = "LIMIT PIN X Max";
	pins[PIN_Y_MIN_LIMIT]           = "LIMIT PIN Y Min";
	pins[PIN_Y_MAX_LIMIT]           = "LIMIT PIN Y Max";
	pins[PIN_Z_MIN_LIMIT]           = "LIMIT PIN Z Min";
	pins[PIN_Z_MAX_LIMIT]           = "LIMIT PIN Z Max";
	pins[PIN_H_MIN_LIMIT]           = "LIMIT PIN H Min";
	pins[PIN_H_MAX_LIMIT]           = "LIMIT PIN H Max";
	
	pins[PIN_ENABLE_SPINDLE]        = "SPINDLE ENABLE PIN";
	
	pins[PIN_IS_CTRL_POWERED]       = "IS CONTROLLER POWERED PIN";
	pins[PIN_IS_SPINDLE_POWERED]    = "IS SPINDLE POWERED PIN";
	
	pins[PIN_EXTERNAL_INTERRUPT]    = "EXTERNAL INTERRUPT PIN";
	
	pins[PIN_H_MOVE_UP]             = "MOVE PODEST UP PIN";
	pins[PIN_H_MOVE_DOWN]           = "MOVE PODEST DOWN PIN";

	pins[PIN_H_DIR]                 = "MOVE PODEST DIR PIN";
	pins[PIN_H_STP]                 = "MOVE PODEST STEP PIN";
	pins[PIN_LED_PODEST]            = "PODEST LED PIN";
	pins[PIN_ENABLE_PODEST]         = "PODEST ENABLE PIN";
	
	pins[PIN_SPINDEL_SUPPORT]       = "SPINDLE SUPPORT PIN";
	pins[PIN_SPINDEL_SPEED_INF]     = "SPINDLE SPEED PIN";
	
	pins[PIN_IS_SPINDEL_OVRLD_ID]   = "SPINDLE OVERLOAD PIN";


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

	pins[0]   = "Abort/Reset";
	pins[0]   = "Abort/Reset";
	pins[0]   = "Abort/Reset";

	pins[PIN_IS_SPINDEL_OVRLD_ID]	= "SPINDLE OVERLOAD PIN";
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
