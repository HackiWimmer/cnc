
#include <iostream>
#include <sstream>
#include <cfloat>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/slider.h>
#include <wx/dataview.h>
#include <wx/propgrid/manager.h>
#include <wx/evtloop.h>
#include "DataControlModel.h"
#include "SerialPort.h"
#include "SerialEmulatorNULL.h"
#include "SerialEmulatorFile.h"
#include "SerialEmulatorSVG.h"
#include "CncMotionMonitor.h"
#include "CncCommon.h"
#include "CncControl.h"
#include "CncFileNameService.h"
#include "MainFrame.h"

static CommandTemplates CMDTPL;

///////////////////////////////////////////////////////////////////
CncControl::CncControl(CncPortType pt) 
: currentClientId(-1)
, setterMap()
, portType(pt)
, serialPort(NULL)
, cncConfig(NULL)
, zeroPos(0,0,0)
, startPos(0,0,0)
, curAppPos(0,0,0)
, controllerPos(0,0,0)
, renderMode(CncRenderAtController)
, speedType(CncSpeedRapid)
, rpmSpeedX(500), rpmSpeedY(500), rpmSpeedZ(500)
, feedSpeed(10)
, durationCounter(0)
, interruptState(false)
, powerOn(false)
, toolUpdateState(true)
, stepDelay(0)
, guiCtlSetup(NULL)
, commandCounter(0)
, displayCounter(0)
, positionCheck(true)
, drawPaneMargin(30)
, speedMonitorMode(DM_2D)
{
//////////////////////////////////////////////////////////////////
	if      ( pt == CncPORT ) 		serialPort = new SerialSpyPort(this);
	else if ( pt == CncEMU_NULL )	serialPort = new SerialEmulatorNULL(this);
	else if ( pt == CncEMU_SVG )	serialPort = new SerialEmulatorSVG(this);
	else 							serialPort = new SerialSpyPort(this);
	
	serialPort->enableSpyOutput();
	
	// create default config
	cncConfig = CncConfig::getGlobalCncConfig();
	
	// init pen handler
	penHandler.reset();
}
///////////////////////////////////////////////////////////////////
CncControl::~CncControl() {
///////////////////////////////////////////////////////////////////
	assert(serialPort);
	
	if ( getToolState() == true )
		switchToolOff();
	
	// safty
	if ( serialPort->isConnected() )
		serialPort->disconnect();

	delete serialPort;
}
//////////////////////////////////////////////////////////////////
const CncDoublePosition CncControl::getStartPosMetric() {
//////////////////////////////////////////////////////////////////
	CncDoublePosition retValue;
	retValue.setXYZ(startPos.getX() * cncConfig->getDisplayFactX(),
				    startPos.getY() * cncConfig->getDisplayFactY(),
	                startPos.getZ() * cncConfig->getDisplayFactZ());
	return retValue;
}
//////////////////////////////////////////////////////////////////
const CncDoublePosition CncControl::getCurPosMetric() {
//////////////////////////////////////////////////////////////////
	CncDoublePosition retValue;
	retValue.setXYZ(curAppPos.getX() * cncConfig->getDisplayFactX(),
				    curAppPos.getY() * cncConfig->getDisplayFactY(),
	                curAppPos.getZ() * cncConfig->getDisplayFactZ());
	return retValue;
}
//////////////////////////////////////////////////////////////////
const CncLongPosition CncControl::getMinPositions() {
//////////////////////////////////////////////////////////////////
	CncLongPosition retValue;
	retValue.setXYZ(curAppPos.getXMin(), curAppPos.getYMin(), curAppPos.getZMin());
	return retValue;
}//////////////////////////////////////////////////////////////////
const CncLongPosition CncControl::getMaxPositions() {
//////////////////////////////////////////////////////////////////
	CncLongPosition retValue;
	retValue.setXYZ(curAppPos.getXMax(), curAppPos.getYMax(), curAppPos.getZMax());
	return retValue;
}
//////////////////////////////////////////////////////////////////
const CncDoublePosition CncControl::getMinPositionsMetric() {
//////////////////////////////////////////////////////////////////
	CncDoublePosition retValue;
	retValue.setXYZ(curAppPos.getXMin() * cncConfig->getDisplayFactX(),
					curAppPos.getYMin() * cncConfig->getDisplayFactY(),
					curAppPos.getZMin() * cncConfig->getDisplayFactZ());
	return retValue;
}
//////////////////////////////////////////////////////////////////
const CncDoublePosition CncControl::getMaxPositionsMetric() {
//////////////////////////////////////////////////////////////////
	CncDoublePosition retValue;
	retValue.setXYZ(curAppPos.getXMax() * cncConfig->getDisplayFactX(),
					curAppPos.getYMax() * cncConfig->getDisplayFactY(),
					curAppPos.getZMax() * cncConfig->getDisplayFactZ());
	return retValue;
}
//////////////////////////////////////////////////////////////////
const CncLongPosition::Watermarks CncControl::getWaterMarks() {
//////////////////////////////////////////////////////////////////
	CncLongPosition::Watermarks retValue;
	curAppPos.getWatermarks(retValue);
	return retValue;
}
//////////////////////////////////////////////////////////////////
const CncDoublePosition::Watermarks CncControl::getWaterMarksMetric() {
//////////////////////////////////////////////////////////////////
	CncDoublePosition::Watermarks retValue;

	CncLongPosition::Watermarks xyz = CncControl::getWaterMarks();
	retValue.xMin = xyz.xMin * cncConfig->getDisplayFactX();
	retValue.xMax = xyz.xMax * cncConfig->getDisplayFactX();

	retValue.yMin = xyz.yMin * cncConfig->getDisplayFactY();
	retValue.yMax = xyz.yMax * cncConfig->getDisplayFactY();
	
	retValue.zMin = xyz.zMin * cncConfig->getDisplayFactZ();
	retValue.zMax = xyz.zMax * cncConfig->getDisplayFactZ();

	return retValue;
}
///////////////////////////////////////////////////////////////////
bool CncControl::processSetter(unsigned char id, int32_t value) {
///////////////////////////////////////////////////////////////////
	if ( isInterrupted() )
		return false;

	if ( isConnected() == false )
		return false;
	
	#warning todo - impl setterMap as parameter bool + resetSetterMap() if value change
	if ( true ) {
		auto it = setterMap.find((int)id);
		if ( it != setterMap.end() ) {
			// value dosen't changed
			if ( it->second == value )
				return true;
		}
	}
		
	if ( serialPort->processSetter(id, value) == false ) {
		std::cerr << std::endl << "CncControl::processSetterList: Setter failed." << std::endl;
		std::cerr << " Id:    " << ArduinoPIDs::getPIDLabel((int)id) << std::endl;
		std::cerr << " Value: " << value << std::endl;

		return false;
	}
	
	// store
	setterMap[id] = value;
	
	// publish setter event
	typedef UpdateManagerThread::Event Event;
	static Event evt;
	
	if ( GET_GUI_CTL(mainFrame) )
		GET_GUI_CTL(mainFrame)->umPostEvent(evt.SetterEvent(id, value));

	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::processSetterList(std::vector<SetterTuple>& setup) {
///////////////////////////////////////////////////////////////////
	for ( auto it = setup.begin(); it != setup.end(); ++it) {
		if ( processSetter((*it).id, (*it).value) == false ) {
			return false;
		}
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
void CncControl::resetSetterMap() {
///////////////////////////////////////////////////////////////////
	setterMap.clear();
}
///////////////////////////////////////////////////////////////////
void CncControl::setup(bool doReset) {
///////////////////////////////////////////////////////////////////
	wxASSERT(serialPort);
	wxASSERT(cncConfig);
	
	// always reset the map here to definitly reinitianlize the controller
	resetSetterMap();
	
	if ( serialPort->isConnected() == false) 
		return;

	logProcessingStart();

	if ( doReset == true )
		reset();
		
	evaluateLimitState();

	std::cout << " Starting Controller initialization . . . ";

	std::vector<SetterTuple> setup;
	setup.push_back(SetterTuple(PID_SEPARATOR, SEPARARTOR_SETUP));
	
	setup.push_back(SetterTuple(PID_STEPS_X, cncConfig->getStepsX()));
	setup.push_back(SetterTuple(PID_STEPS_Y, cncConfig->getStepsY()));
	setup.push_back(SetterTuple(PID_STEPS_Z, cncConfig->getStepsZ()));
	
	setup.push_back(SetterTuple(PID_PITCH_X, convertDoubleToCtrlLong(PID_PITCH_X, cncConfig->getPitchX())));
	setup.push_back(SetterTuple(PID_PITCH_Y, convertDoubleToCtrlLong(PID_PITCH_Y, cncConfig->getPitchY())));
	setup.push_back(SetterTuple(PID_PITCH_Z, convertDoubleToCtrlLong(PID_PITCH_Z, cncConfig->getPitchZ())));
	
	setup.push_back(SetterTuple(PID_SPEED_X, getRpmSpeedX()));
	setup.push_back(SetterTuple(PID_SPEED_Y, getRpmSpeedY()));
	setup.push_back(SetterTuple(PID_SPEED_Z, getRpmSpeedZ()));
	
	setup.push_back(SetterTuple(PID_POS_REPLY_THRESHOLD, cncConfig->getReplyThreshold()));
	
	if ( processSetterList(setup) ) {
		changeCurrentRpmSpeedXYZ(CncSpeedRapid);
		
		// reset error info
		processCommand("r", std::cerr);
		
		std::cout << "Ready\n";
	}
	
	// Firmware check
	std::cout << "Firmware:" << std::endl;
	std::cout << " Available:\t";
	std::stringstream ss;
	processCommand(CMD_PRINT_VERSION, ss);
	std::cout << ss.str() << std::endl;
	std::cout << " Required:\t" << FIRMWARE_VERSION << std::endl;
	
	if ( wxString(FIRMWARE_VERSION) != ss.str().c_str() )
		cnc::cex1 << " Firmware is possibly not compatible!" << std::endl;

	logProcessingEnd();
}
///////////////////////////////////////////////////////////////////
long CncControl::convertDoubleToCtrlLong(unsigned char 	id, double d) { 
///////////////////////////////////////////////////////////////////
	if ( d <= MIN_LONG / DBL_FACT ) {
		std::cerr << "CncControl::convertDoubleToCtrlLong(): Invalid double value: '" << d << "' for PID: " << ArduinoPIDs::getPIDLabel(id) << std::endl;
		return MIN_LONG; 
	}
		
	if ( d >= MAX_LONG / DBL_FACT ) {
		std::cerr << "CncControl::convertDoubleToCtrlLong(): Invalid double value: '" << d << "' for PID: " << ArduinoPIDs::getPIDLabel(id) << std::endl;
		return MAX_LONG; 
	}
		
	return d * DBL_FACT; 
}
///////////////////////////////////////////////////////////////////
bool CncControl::connect(const char * portName) {
///////////////////////////////////////////////////////////////////
	wxASSERT(serialPort);
	
	std::cout << "::" << serialPort->getClassName() << ":" << std::endl;
	if ( serialPort->isConnected() ) {
		std::cout << " Disconnecting . . ." << std::endl;
		serialPort->disconnect();
	}

	std::clog << " Try to connect to: " << portName << std::endl;
	bool ret = serialPort->connect(portName);
	if ( ret == true ) {
		std::cout << " Connection established." << std::endl;
	}

	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::isConnected() {
///////////////////////////////////////////////////////////////////
	wxASSERT(serialPort);
	return serialPort->isConnected();
}
///////////////////////////////////////////////////////////////////
bool CncControl::processCommand(const unsigned char c, std::ostream& txtCtl) {
///////////////////////////////////////////////////////////////////
	wxASSERT(serialPort);
	return serialPort->processCommand(c, txtCtl, curAppPos);
}
///////////////////////////////////////////////////////////////////
bool CncControl::processCommand(const char* cmd, std::ostream& txtCtl) {
///////////////////////////////////////////////////////////////////
	wxASSERT(serialPort);
	return serialPort->processCommand(cmd, txtCtl, curAppPos);
}
///////////////////////////////////////////////////////////////////
bool CncControl::processMoveXYZ(int32_t x1, int32_t y1, int32_t z1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	wxASSERT(serialPort);
	return serialPort->processMoveXYZ(x1, y1, z1, alreadyRendered, curAppPos);
}
///////////////////////////////////////////////////////////////////
void CncControl::clearDrawControl() {
///////////////////////////////////////////////////////////////////	
	penHandler.reset();
	
	if ( IS_GUI_CTL_VALID(motionMonitor) )
		GET_GUI_CTL(motionMonitor)->clear();
}
///////////////////////////////////////////////////////////////////
void CncControl::updateDrawControl() {
///////////////////////////////////////////////////////////////////
	updatePreview3D(true); 
}
///////////////////////////////////////////////////////////////////
inline void CncControl::setValue(wxTextCtrl *ctl, int32_t val) {
///////////////////////////////////////////////////////////////////
	if ( ctl != NULL ) {
		ctl->ChangeValue(wxString::Format(wxT("%i"),val));
	}
}
///////////////////////////////////////////////////////////////////
inline void CncControl::setValue(wxTextCtrl *ctl, double val) {
///////////////////////////////////////////////////////////////////
	if ( ctl != NULL ) {
		ctl->ChangeValue(wxString::Format(wxT("%4.3f"),val));
	}
}
///////////////////////////////////////////////////////////////////
inline void CncControl::setValue(wxTextCtrl *ctl, const char* val) {
///////////////////////////////////////////////////////////////////
	if ( ctl != NULL ) {
		ctl->ChangeValue(wxString::Format(wxT("%s"),val));
	}
}
///////////////////////////////////////////////////////////////////
void CncControl::setGuiControls(GuiControlSetup* gcs) {
///////////////////////////////////////////////////////////////////
	assert(gcs);
	guiCtlSetup = gcs;
	toolState.setControl(GET_GUI_CTL(toolState));
	setToolState(true);
}
///////////////////////////////////////////////////////////////////
void CncControl::setZeroPosX() {
///////////////////////////////////////////////////////////////////
	curAppPos.setX(0);
	zeroPos.setX(0);
	startPos.setX(0);
	
	postAppPosition();
}
///////////////////////////////////////////////////////////////////
void CncControl::setZeroPosY() {
///////////////////////////////////////////////////////////////////
	curAppPos.setY(0);
	zeroPos.setY(0);
	startPos.setY(0);
	
	postAppPosition();
}
///////////////////////////////////////////////////////////////////
void CncControl::setZeroPosZ() {
///////////////////////////////////////////////////////////////////
	wxASSERT( guiCtlSetup );
	
	int32_t val = 0L;
	
	if ( cncConfig->getReferenceIncludesWpt() == true )
		val = (long)round(cncConfig->getWorkpieceThickness() * cncConfig->getCalculationFactZ());
	
	curAppPos.setZ(val);
	zeroPos.setZ(val);
	startPos.setZ(val);
	
	if ( isConnected() == true ) {
		if ( processSetter(PID_Z_POS, val)  == false ) {
			std::cerr << "CncControl::setZeroPosZ: processSetter failed!"<< std::endl;
		}
	}
	
	postAppPosition();
}
///////////////////////////////////////////////////////////////////
void CncControl::setZeroPos() {
///////////////////////////////////////////////////////////////////
	setZeroPosX();
	setZeroPosY();
	setZeroPosZ();
}
///////////////////////////////////////////////////////////////////
void CncControl::setStartPos() {
///////////////////////////////////////////////////////////////////
	startPos = curAppPos;
}
///////////////////////////////////////////////////////////////////
void CncControl::interrupt() {
///////////////////////////////////////////////////////////////////
	std::cerr << "CncControl: Interrupted" << std::endl;
	interruptState = true;
	switchToolOff(true);
}
///////////////////////////////////////////////////////////////////
bool CncControl::isInterrupted() {
///////////////////////////////////////////////////////////////////
	return interruptState;
}
///////////////////////////////////////////////////////////////////
void CncControl::resetInterrupt() {
///////////////////////////////////////////////////////////////////
	resetDurationCounter();
	interruptState = false;
}
///////////////////////////////////////////////////////////////////
bool CncControl::resetWatermarks() {
///////////////////////////////////////////////////////////////////
	zeroPos.resetWatermarks();
	startPos.resetWatermarks();
	curAppPos.resetWatermarks();
	curCtlPos.resetWatermarks();
	controllerPos.resetWatermarks();
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::reset() {
///////////////////////////////////////////////////////////////////
	getSerial()->purge();
	resetInterrupt();
	
	std::cout << "Try to reset the controller\n";
	if ( processCommand(CMD_RESET_CONTROLLER, std::cerr) ) {
		std::cout << " Controller reseted\n";
	} else {
		std::cerr << " Controller reset failed\n";
		return false;
	}
	
	// do this after the controller reset, because setZeroPos will determine a new controller position on demand
	setZeroPos();
	
	curCtlPos = getControllerPos();
	postCtlPosition();
	
	evaluateLimitState();
	switchToolOff();
	
	return true;
}
///////////////////////////////////////////////////////////////////
unsigned int CncControl::getDurationCount() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	return cncConfig->getDurationCount();
}
///////////////////////////////////////////////////////////////////
bool CncControl::hasNextDuration() {
///////////////////////////////////////////////////////////////////
	return ( durationCounter < getDurationCount() );
}
///////////////////////////////////////////////////////////////////
void CncControl::resetDurationCounter() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	durationCounter = 0;
	penHandler.reset();
	
	if ( GET_GUI_CTL(passingTrace) && toolUpdateState == true )
		GET_GUI_CTL(passingTrace)->SetValue(wxString() << durationCounter);
}
///////////////////////////////////////////////////////////////////
void CncControl::initNextDuration() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	penHandler.initNextDuration();
	getSerial()->beginDuration(getDurationCounter());
	
	durationCounter++;
	
	if ( GET_GUI_CTL(passingTrace) && toolUpdateState == true )
		GET_GUI_CTL(passingTrace)->SetValue(wxString() << durationCounter);
}
///////////////////////////////////////////////////////////////////
unsigned int CncControl::getDurationCounter() {
///////////////////////////////////////////////////////////////////
	return durationCounter;
}
///////////////////////////////////////////////////////////////////
bool CncControl::isFirstDuration() {
///////////////////////////////////////////////////////////////////
	return (durationCounter == 1);
}
///////////////////////////////////////////////////////////////////
bool CncControl::isLastDuration() {
///////////////////////////////////////////////////////////////////
	return (durationCounter == getDurationCount());
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveZToBottom() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	
	double curZPos = curAppPos.getZ() * cncConfig->getDisplayFactZ(); // we need it as mm
	double moveZ   = curZPos * (-1);
	
	bool ret = true;
	if ( prepareSimpleMove() == true ) {
		if ( moveRelMetricZ(moveZ) == false ) {
			std::cerr << "CncControl: Move Z to bottom error"<< std::endl;
			ret = false;
		}
	} else {
		ret = false;
	}
	reconfigureSimpleMove(false);
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveZToTop() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	
	double topZPos = cncConfig->getCurZDistance(); //cncConfig->getMaxZDistance();
	double curZPos = curAppPos.getZ() * cncConfig->getDisplayFactZ(); // we need it as mm
	double moveZ   = topZPos - curZPos;
	
	bool ret = true;
	if ( prepareSimpleMove() == true ) {
		if ( moveRelMetricZ(moveZ) == false ) {
			std::cerr << "CncControl: Move Z to top error"<< std::endl;
			ret = false;
		}
	} else {
		ret = false;
	}
	
	reconfigureSimpleMove(false);
	return ret;
}
///////////////////////////////////////////////////////////////////
void CncControl::changeCurrentFeedSpeedXYZ(CncSpeed s, double value) {
///////////////////////////////////////////////////////////////////
	if ( setActiveFeedSpeed(s, value) == true ) {
		
		int mmm = getSpeedControlMode() == DM_2D ? PID_SWITCH_MOVE_MODE_STATE_2D : PID_SWITCH_MOVE_MODE_STATE_3D;
		processSetter(mmm, (s == CncSpeedWork));
		processSetter(PID_SPEED_X, getRpmSpeedX());
		processSetter(PID_SPEED_Y, getRpmSpeedY());
		processSetter(PID_SPEED_Z, getRpmSpeedZ());
	}
}
///////////////////////////////////////////////////////////////////
void CncControl::changeCurrentRpmSpeedXYZ(CncSpeed s, unsigned int value) {
///////////////////////////////////////////////////////////////////
	changeCurrentRpmSpeedZ(s, value);
	changeCurrentRpmSpeedXY(s, value);
}
///////////////////////////////////////////////////////////////////
void CncControl::changeCurrentRpmSpeedXY(CncSpeed s, unsigned int value) {
///////////////////////////////////////////////////////////////////
	if ( setActiveRpmSpeedXY(s, value ) == true ) {
		
		int mmm = getSpeedControlMode() == DM_2D ? PID_SWITCH_MOVE_MODE_STATE_2D : PID_SWITCH_MOVE_MODE_STATE_3D;
		processSetter(mmm, (s == CncSpeedWork));
		processSetter(PID_SPEED_X, getRpmSpeedX());
		processSetter(PID_SPEED_Y, getRpmSpeedY());
	}
}
///////////////////////////////////////////////////////////////////
void CncControl::changeCurrentRpmSpeedZ(CncSpeed s, unsigned int value) {
///////////////////////////////////////////////////////////////////
	if ( setActiveRpmSpeedZ(s, value) == true ) {
		
		int mmm = getSpeedControlMode() == DM_2D ? PID_SWITCH_MOVE_MODE_STATE_2D : PID_SWITCH_MOVE_MODE_STATE_3D;
		processSetter(mmm, (s == CncSpeedWork));
		processSetter(PID_SPEED_Z, getRpmSpeedZ());
	}
}
///////////////////////////////////////////////////////////////////
void CncControl::logProcessingStart() {
///////////////////////////////////////////////////////////////////
	// update command values
	typedef UpdateManagerThread::Event Event;
	static Event evt;
	
	if ( GET_GUI_CTL(mainFrame) )
		GET_GUI_CTL(mainFrame)->umPostEvent(evt.CommandEvent(0, 0));
	
	ftime(&startTime);
	commandCounter=0;
}
///////////////////////////////////////////////////////////////////
void CncControl::logProcessingCurrent() {
///////////////////////////////////////////////////////////////////
	typedef UpdateManagerThread::Event Event;
	static Event evt;
	
	ftime(&endTime);
	long t_diff = (long) (1000.0 * (endTime.time - startTime.time) + (endTime.millitm - startTime.millitm));  

	// update command values
	if ( GET_GUI_CTL(mainFrame) )
		GET_GUI_CTL(mainFrame)->umPostEvent(evt.CommandEvent(commandCounter, t_diff));
}
///////////////////////////////////////////////////////////////////
void CncControl::logProcessingEnd(bool valuesOnly) {
///////////////////////////////////////////////////////////////////
	// update position
	postAppPosition();
	
	if ( valuesOnly == false )
		logProcessingCurrent();
}
///////////////////////////////////////////////////////////////////
void CncControl::forceDisplayPositions() {
///////////////////////////////////////////////////////////////////
	logProcessingEnd(true);
}
///////////////////////////////////////////////////////////////////
bool CncControl::validatePostion(const CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	if ( positionCheck == false )
		return true;
		
	// will only be done for emulation ports. It didn't makes sense for a cnc run
	// see below
	if ( serialPort->getPortType() != CncPORT ) {
		// in the real cnc life this is not good enough. the check has to be done by
		// the end switches at last.
		bool error = false;
		CncLongPosition::Watermarks wm;
		pos.getWatermarks(wm);
		
		if ( (wm.xMax - wm.xMin)/cncConfig->getCalculationFactX() > cncConfig->getMaxDimensionX() ) error = true;
		if ( (wm.yMax - wm.yMin)/cncConfig->getCalculationFactY() > cncConfig->getMaxDimensionY() ) error = true;
		if ( (wm.zMax - wm.zMin)/cncConfig->getCalculationFactZ() > cncConfig->getMaxDimensionZ() ) error = true;
	
		if ( error == true ) {
			std::cerr << "Position out of range!" << std::endl;
			std::cerr << " Max valid X dimension: " << cncConfig->getMaxDimensionX() << std::endl;
			std::cerr << " Max valid Y dimension: " << cncConfig->getMaxDimensionY() << std::endl;
			std::cerr << " Max valid Z dimension: " << cncConfig->getMaxDimensionZ() << std::endl;
			std::cerr << " Pos: " << pos << std::endl;
			std::cerr << " Min Watermark: " << wm.xMin << "," << wm.yMin << "," << wm.zMin << "," << std::endl;
			std::cerr << " Max Watermark: " << wm.xMax << "," << wm.yMax << "," << wm.zMax << "," << std::endl;
			std::cerr << " Calculated spread X :" <<  (wm.xMax - wm.xMin)/cncConfig->getCalculationFactX() << std::endl;
			std::cerr << " Calculated spread Y :" <<  (wm.yMax - wm.yMin)/cncConfig->getCalculationFactY() << std::endl;
			std::cerr << " Calculated spread Z :" <<  (wm.zMax - wm.zMin)/cncConfig->getCalculationFactZ() << std::endl;
			
			return false;
		}
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
void CncControl::monitorPosition(const CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	// motion monitor
	static GLI::VerticeLongData vd;
	static CncLongPosition prevPos;

	if ( pos != prevPos ) {
		
		if ( IS_GUI_CTL_VALID(motionMonitor) ) {
			vd.setVertice(getClientId(), getCurrentSpeedType(), pos);
			GET_GUI_CTL(motionMonitor)->appendVertice(vd);
			
			updatePreview3D(false);
		}
		
		prevPos = pos;
		
		// position out of configured range?
		if ( validatePostion(pos) == false )
			interrupt();
		
	}
}
///////////////////////////////////////////////////////////////////
bool CncControl::SerialMessageCallback(const ControllerMsgInfo& cmi) {
///////////////////////////////////////////////////////////////////
	wxDateTime now = wxDateTime::UNow();
	wxString msg(cmi.message.str().c_str());
	char type = (char)msg[0];
	msg = msg.SubString(1, msg.length());
	
	int p1 = wxNOT_FOUND, p2 = wxNOT_FOUND;
	if ( (p1 = msg.Find("{[")) != wxNOT_FOUND ) {
		if ( (p2 = msg.Find("]}")) != wxNOT_FOUND && p2 >= p1 + 2) {
			wxString idStr = msg.SubString(p1+2, p2-1);
			long id;
			idStr.ToLong(&id);
			wxString errorCode = ArduinoErrorCodes::getECLabel(id);
			wxString replace("{[");
			replace << idStr;
			replace << "]}";
			
			msg.Replace(replace, errorCode);
		}
	}
	
	if ( GET_GUI_CTL(mainFrame) != NULL )
		GET_GUI_CTL(mainFrame)->displayNotification(type, "Controller Callback", msg, (type == 'E' ? 8 : 4));
	
	switch ( type ) {
		
		case 'W':	
					cnc::msg.logWarning(now.Format("Warning Message received: %H:%M:%S.%l\n"));
					cnc::msg.logWarning(msg);
					break;
					
		case 'E':	cnc::msg.logError(now.Format("Error Message received: %H:%M:%S.%l\n"));
					cnc::msg.logError(msg);
					break;
					
		default:	cnc::msg.logInfo(now.Format("Info Message received: %H:%M:%S.%l\n"));
					cnc::msg.logInfo(msg);
	}
	
	cnc::msg.setTextColour(wxColour(128, 128, 0));
	cnc::msg << "-------------------------------------------------------------------------------------------\n";
	cnc::msg.resetTextAttr();

	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::SerialControllerCallback(const ContollerInfo& ci) {
///////////////////////////////////////////////////////////////////
	// Event handling, enables the interrrpt functionality
	if ( cncConfig->isAllowEventHandling() )
		THE_APP->dispatchAll();
	
	switch ( ci.infoType ) {
		// --------------------------------------------------------
		case CITHeartbeat:
			if ( ci.command == 'T' ) {
				std::stringstream ss;
				ss << "Heartbeat received - Value: " << ci.heartbeatValue << std::endl;
				cnc::trc.logInfoMessage(ss);
			}
			break;
			
		// --------------------------------------------------------
		case CITPosition:
			// update controller position
			switch ( ci.posType ) {
				case PID_X_POS: 	curCtlPos.setX(ci.xCtrlPos); break;
				case PID_Y_POS: 	curCtlPos.setY(ci.yCtrlPos); break;
				case PID_Z_POS: 	curCtlPos.setZ(ci.zCtrlPos); break;
				
				case PID_XYZ_POS:
				default:			curCtlPos.setXYZ(ci.xCtrlPos, ci.yCtrlPos, ci.zCtrlPos);
			}
			
			// display controller coordinates
			postCtlPosition();
			
			// motion monitor
			monitorPosition(curCtlPos);
			
			break;
			
		// --------------------------------------------------------
		case CITSetterInfo:
			//if ( getSerial()->isSpyOutput() == true )
			//	cnc::spy << "Setter: " << ArduinoPIDs::getPIDLabel((int)ci.setterId) << ": " << ci.setterValue << std::endl;
			break;
		
		// --------------------------------------------------------
		case CITLimitInfo:
			//std::clog << "::L: " << ci.xLimit << ", " << ci.yLimit << ", " << ci.zLimit << std::endl;
			evaluateLimitState(ci.xLimit, ci.yLimit, ci.zLimit);
			break;
		
		// --------------------------------------------------------
		default:
			std::cerr << "CncControl::SerialControllerCallback:" << std::endl;
			std::cerr << " No handler defined for controller info type:" << ci.infoType << std::endl;
	}

	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::SerialCallback(int32_t cmdCount) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	wxASSERT(guiCtlSetup);

	if ( isInterrupted() ) {
		std::cerr << "SerialCallback: Interrupt detected"<< std::endl;
		return false;
	}

	// Evalutate the command counter
	commandCounter += cmdCount;
	
	// Event handling, enables the interrupt functionallity
	if ( cncConfig->isAllowEventHandling() )
		THE_APP->dispatchAll();
	
	// display application coordinates
	postAppPosition();
	
	// log processing
	logProcessingCurrent();
	
	if ( GetAsyncKeyState(VK_ESCAPE) != 0 ) {
		std::cerr << "SerialCallback: ESCAPE key detected" << std::endl;
		interrupt();
	}
	
	return !isInterrupted();
}
///////////////////////////////////////////////////////////////////
void CncControl::postAppPosition() {
///////////////////////////////////////////////////////////////////
	static CncLongPosition lastAppPos;
	
	if ( cncConfig->isOnlineUpdateCoordinates() ) {
		// application position
		typedef UpdateManagerThread::Event Event;
		static Event evt;
		
		#warning - impl speed value
		if ( lastAppPos != curAppPos ) {
			if ( GET_GUI_CTL(mainFrame) )
				GET_GUI_CTL(mainFrame)->umPostEvent(evt.AppPosEvent(getClientId(), getSpeedAsString(), 4242.7, curAppPos));
		}
	}
	
	lastAppPos.set(curAppPos);
}
///////////////////////////////////////////////////////////////////
void CncControl::postCtlPosition() {
///////////////////////////////////////////////////////////////////
	static CncLongPosition lastCtlPos;
	
	if ( cncConfig->isOnlineUpdateCoordinates() ) {
		// application position
		typedef UpdateManagerThread::Event Event;
		static Event evt;
		
		#warning - impl speed value
		if ( lastCtlPos != curCtlPos ) {
			if ( GET_GUI_CTL(mainFrame) )
				GET_GUI_CTL(mainFrame)->umPostEvent(evt.CtlPosEvent(getClientId(), getSpeedAsString(), 4242.7, curCtlPos));
		}
	}
	
	lastCtlPos.set(curCtlPos);
}
///////////////////////////////////////////////////////////////////
bool CncControl::simpleMoveXYToZeroPos(CncDimensions dim) {
///////////////////////////////////////////////////////////////////
	bool ret = false;
	if ( prepareSimpleMove() == true ) {
		ret = moveXYToZeroPos(dim);
	}
	reconfigureSimpleMove(ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::simpleMoveXYZToZeroPos(CncDimensions dim) {
///////////////////////////////////////////////////////////////////
	bool ret = false;
	if ( prepareSimpleMove() == true ) {
		ret = moveXYZToZeroPos(dim);
	}
	reconfigureSimpleMove(ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::simpleMoveZToZeroPos() {
///////////////////////////////////////////////////////////////////
	bool ret = false;
	if ( prepareSimpleMove() == true ) {
		ret = moveZToZeroPos();
	}
	reconfigureSimpleMove(ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveXYToZeroPos(CncDimensions dim) {
///////////////////////////////////////////////////////////////////
	bool ret = true;
	if ( curAppPos != zeroPos ) {
		int32_t moveX=0, moveY=0;
		moveX = zeroPos.getX() - curAppPos.getX(); 
		moveY = zeroPos.getY() - curAppPos.getY();
		
		if ( dim == CncDimension2D ) {
			ret = moveRelLinearStepsXY(moveX, moveY, false);
			
		} else {
			if ( moveRelLinearStepsXY(moveX, 0, false) == false ) 
				return false;
				
			if ( moveRelLinearStepsXY(0, moveY, false) == false )
				return false;
		}
	}
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveXYZToZeroPos(CncDimensions dim) {
///////////////////////////////////////////////////////////////////
	bool ret = true;
	if ( curAppPos != zeroPos ) {
		int32_t moveX=0, moveY=0, moveZ=0;
		moveX = zeroPos.getX() - curAppPos.getX(); 
		moveY = zeroPos.getY() - curAppPos.getY();
		moveZ = zeroPos.getZ() - curAppPos.getZ();
		
		if ( dim == CncDimension3D ) {
			ret = moveRelLinearStepsXYZ(moveX, moveY, moveZ, false);
			
		} else if ( dim == CncDimension2D ) {
			if ( moveRelLinearStepsXYZ(0, 0, moveZ, false) == false ) 
				return false;
				
			if ( moveRelLinearStepsXYZ(moveX, moveY, 0, false) == false ) 
				return false;
			
		} else {
			if ( moveRelLinearStepsXYZ(moveX, 0, 0, false) == false ) 
				return false;
				
			if ( moveRelLinearStepsXYZ(0, moveY, 0, false) == false ) 
				return false;
				
			if ( moveRelLinearStepsXYZ(0, 0, moveZ, false) == false ) 
				return false;
		}
	}
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveZToZeroPos() {
///////////////////////////////////////////////////////////////////
	bool ret = true;
	if ( curAppPos != zeroPos ) {
		int32_t moveZ=0;
		moveZ = zeroPos.getZ() - curAppPos.getZ();
		ret = moveRelLinearStepsXYZ(0, 0, moveZ, false);
	}
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveToStartPos() {
///////////////////////////////////////////////////////////////////
	bool ret = true;
	if ( curAppPos != startPos ) {
		int32_t moveX=0, moveY=0;
		moveX = startPos.getX() - curAppPos.getX(); 
		moveY = startPos.getY() - curAppPos.getY();
		moveRelLinearStepsXY(moveX, moveY, false);
	}
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveRelStepsZ(int32_t z) {
///////////////////////////////////////////////////////////////////
	if ( z == 0 )
		return true;
	// z moves are always linear, as a consequence alreadyRendered can be true
	return serialPort->processMoveZ(z, true, curAppPos);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveRelLinearStepsXY(int32_t x1, int32_t y1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	//avoid empty steps
	if ( x1 == 0 && y1 == 0 )
		return true;
	
	if ( renderMode == CncRenderAtController )
		return serialPort->processMoveXY(x1, y1, false, curAppPos);
		
	return false;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveRelLinearStepsXYZ(int32_t x1, int32_t y1, int32_t z1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	//avoid empty steps
	if ( x1 == 0 && y1 == 0 && z1 == 0 )
		return true;
	
	if ( renderMode == CncRenderAtController )
		return serialPort->processMoveXYZ(x1, y1, z1, alreadyRendered, curAppPos);

	return false;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveRelMetricZ(double z) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double sZ = z * cncConfig->getCalculationFactZ();
	
	return moveRelStepsZ((int32_t)round(sZ));
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveRelLinearMetricXY(double x1, double y1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double sX1 = x1 * cncConfig->getCalculationFactX();
	double sY1 = y1 * cncConfig->getCalculationFactY();
	
	return moveRelLinearStepsXY((int32_t)round(sX1), 
	                            (int32_t)round(sY1),
	                            alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveRelLinearMetricXYZ(double x1, double y1, double z1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double sX1 = x1 * cncConfig->getCalculationFactX();
	double sY1 = y1 * cncConfig->getCalculationFactY();
	double sZ1 = z1 * cncConfig->getCalculationFactZ();
	
	return moveRelLinearStepsXYZ((int32_t)round(sX1), 
	                             (int32_t)round(sY1),
	                             (int32_t)round(sZ1),
	                             alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveAbsMetricZ(double z) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double sZ = z * cncConfig->getCalculationFactZ();
	
	return moveRelStepsZ( (int32_t)round(sZ) - curAppPos.getZ() );
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveAbsLinearMetricXY(double x1, double y1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double sX1 = x1 * cncConfig->getCalculationFactX();
	double sY1 = y1 * cncConfig->getCalculationFactY();
	
	return moveRelLinearStepsXY((int32_t)round(sX1) - curAppPos.getX(), 
	                            (int32_t)round(sY1) - curAppPos.getY(),
	                            alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveAbsLinearMetricXYZ(double x1, double y1, double z1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double sX1 = x1 * cncConfig->getCalculationFactX();
	double sY1 = y1 * cncConfig->getCalculationFactY();
	double sZ1 = z1 * cncConfig->getCalculationFactZ();
	
	return moveRelLinearStepsXYZ((int32_t)round(sX1) - curAppPos.getX(),
	                             (int32_t)round(sY1) - curAppPos.getY(),
	                             (int32_t)round(sZ1) - curAppPos.getZ(),
	                             alreadyRendered);
}
///////////////////////////////////////////////////////////////////
void CncControl::setToolState(bool defaultStyle) {
///////////////////////////////////////////////////////////////////
	if ( defaultStyle == true ) {
		toolState.setState(CncToolStateControl::red);
	} else {
		if ( toolUpdateState == true ) {
			if ( powerOn == true ) {
				toolState.setState(CncToolStateControl::green);
			} else {
				toolState.setState(CncToolStateControl::red);
			}
		}
	}
}
///////////////////////////////////////////////////////////////////
void CncControl::switchToolOn() {
///////////////////////////////////////////////////////////////////
	if ( isInterrupted() )
		return;

	if ( powerOn == false ) { 
		if ( processSetter(PID_ROUTER_SWITCH, 1) ) {
			powerOn = true;
			setToolState();
		}
	}
}
///////////////////////////////////////////////////////////////////
void CncControl::switchToolOff(bool force) {
///////////////////////////////////////////////////////////////////
	if ( isInterrupted() )
		return;

	if ( powerOn == true || force == true ) {
		if ( processSetter(PID_ROUTER_SWITCH, 0) ) {
			powerOn = false;
			setToolState();
		}
	}
}
///////////////////////////////////////////////////////////////////
const int32_t CncControl::getControllerErrorCount() {
///////////////////////////////////////////////////////////////////
	std::vector<int32_t> list;

	if ( isInterrupted() == false )
		getSerial()->processGetter(PID_ERROR_COUNT, list);
		
	if ( list.size() != 1 ){
		if ( isInterrupted() == false ) {
			std::cerr << "CncControl::getControllerErrorCount: Unable to evaluate error count:" << std::endl;
			std::cerr << " Received value count: " << list.size() << std::endl;
		}
		
		return -1;
	}

	return list.at(0);
}
///////////////////////////////////////////////////////////////////
const CncLongPosition CncControl::getControllerPos() {
///////////////////////////////////////////////////////////////////
	std::vector<int32_t> list;

	if ( isConnected() == true && isInterrupted() == false )
		getSerial()->processGetter(PID_XYZ_POS, list);
		
	if ( list.size() != 3 ){
		controllerPos.setX(0);
		controllerPos.setY(0);
		controllerPos.setZ(0);
		
		if ( isConnected() == true && isInterrupted() == false ) {
			std::cerr << "CncControl::getControllerPos: Unable to evaluate controller position:" << std::endl;
			std::cerr << " Received value count: " << list.size() << std::endl;
		}
	} else {
		controllerPos.setX(list.at(0));
		controllerPos.setY(list.at(1));
		controllerPos.setZ(list.at(2));
	}
	
	return controllerPos;
}
///////////////////////////////////////////////////////////////////
const CncLongPosition CncControl::getControllerLimitState() {
///////////////////////////////////////////////////////////////////
	std::vector<int32_t> list;

	if ( isInterrupted() == false )
		getSerial()->processGetter(PID_LIMIT, list);
	
	if ( list.size() != 3 ){
		if ( isInterrupted() == false ) {
			std::cerr << "CncControl::getControllerLimitState: Unable to evaluate controller limit state:" << std::endl;
			std::cerr << " Received value count: " << list.size() << std::endl;
		}
	} else {
		return {list.at(0), list.at(1), list.at(2)};
	}
	
	return {0, 0, 0};
}
///////////////////////////////////////////////////////////////////
bool CncControl::validatePositions() {
///////////////////////////////////////////////////////////////////
	CncLongPosition ctlPos = getControllerPos();
	return ( curAppPos == ctlPos );
}
///////////////////////////////////////////////////////////////////
void CncControl::updateCncConfigTrace() {
///////////////////////////////////////////////////////////////////
	typedef UpdateManagerThread::Event Event;
	static Event evt;
	
	if ( GET_GUI_CTL(mainFrame) )
		GET_GUI_CTL(mainFrame)->umPostEvent(evt.ConfigUpdateEvent());
}
///////////////////////////////////////////////////////////////////
void CncControl::enableStepperMotors(bool s) {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	if ( isInterrupted() )
		return;
		
	if ( isConnected() == false )
		return;
	
	bool ret = false;
	if ( s == true ) 	ret = processCommand("E", std::cout);
	else				ret = processCommand("e", std::cout);
	
	if ( ret == false ) {
		std::cerr << "CncControl::enableStepperMotors" << std::endl;
		std::cerr << " Error while enabling stepper motors. State=" << s << std::endl;
		return;
	}
	
	if ( GET_GUI_CTL(motorState) )
		GET_GUI_CTL(motorState)->Check(s);
}
///////////////////////////////////////////////////////////////////
void CncControl::evaluateLimitState() {
///////////////////////////////////////////////////////////////////
	CncLongPosition ls = getControllerLimitState();
	evaluateLimitState(ls.getX(), ls.getY(), ls.getZ());
}
///////////////////////////////////////////////////////////////////
wxString& CncControl::getLimitInfoString(wxString& ret) {
///////////////////////////////////////////////////////////////////
	return limitStates.getLimitInfoString(ret);
}
///////////////////////////////////////////////////////////////////
void CncControl::evaluateLimitState(long x, long y, long z) {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	limitStates.setXLimit(x);
	limitStates.setYLimit(y);
	limitStates.setZLimit(z);
	
	displayLimitState(GET_GUI_CTL(xMinLimit), limitStates.getXMinLimit());
	displayLimitState(GET_GUI_CTL(xMaxLimit), limitStates.getXMaxLimit());
	displayLimitState(GET_GUI_CTL(yMinLimit), limitStates.getYMinLimit());
	displayLimitState(GET_GUI_CTL(yMaxLimit), limitStates.getYMaxLimit());
	displayLimitState(GET_GUI_CTL(zMinLimit), limitStates.getZMinLimit());
	displayLimitState(GET_GUI_CTL(zMaxLimit), limitStates.getZMaxLimit());
	
	limitStates.displayLimitState();
}
///////////////////////////////////////////////////////////////////
void CncControl::displayLimitState(wxStaticText* ctl, bool value) {
///////////////////////////////////////////////////////////////////
	if ( ctl != NULL ) {
		if ( value == true ) {
			ctl->SetLabel(wxString("1"));
			ctl->SetBackgroundColour(wxColour(255,128,128));
			ctl->SetForegroundColour(*wxWHITE);
			
		} else {
			ctl->SetLabel(wxString("0"));
			ctl->SetBackgroundColour(wxColour(181,230,29));
			ctl->SetForegroundColour(*wxBLACK);

		}
		ctl->Refresh();
		ctl->Update();
	}
}
///////////////////////////////////////////////////////////////////
bool CncControl::meassureDimension(const char axis, wxCheckBox* min, wxCheckBox* max, double& result) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	
	double maxSteps 	= (axis != 'Z' ? -450.0 : -100.0); // mm
	bool ret 			= false;
	long minPos 		= 0l;
	long maxPos 		= 0l;
	result 				= -DBL_MAX; // invalid Values
	
	if ( min ) min->SetValue(false);
	if ( max ) max->SetValue(false);
	
	if ( prepareSimpleMove() == true ) {
		
		// move to min position
		switch ( axis ) {
			case 'X': 	ret = moveRelLinearMetricXY(maxSteps, 0.0, true);
						if ( ret ) 
							minPos = curCtlPos.getX();
						break;
						
			case 'Y': 	ret = moveRelLinearMetricXY(0.0, maxSteps, true);
						if ( ret ) 
							minPos = curCtlPos.getY();
						break;
						
			case 'Z': 	ret = moveRelMetricZ(maxSteps);
						if ( ret ) 
							minPos = curCtlPos.getZ();
						break;
		}
		
		// move to max position
		if ( ret ) {
			if ( min ) min->SetValue(true);
			maxSteps *= -1;
			
			switch ( axis ) {
				case 'X': 	ret = moveRelLinearMetricXY(maxSteps, 0.0, true);
							if ( ret ) 
								maxPos = curCtlPos.getX();
							break;
							
				case 'Y': 	ret = moveRelLinearMetricXY(0.0, maxSteps, true);
							if ( ret ) 
								maxPos = curCtlPos.getY();
							break;
							
				case 'Z': 	ret = moveRelMetricZ(maxSteps);
							if ( ret ) 
								maxPos = curCtlPos.getZ();
							break;
			}
		}
		
		// free end position
		if ( ret ) {
			if ( max ) max->SetValue(true);
			
			switch ( axis ) {
				case 'X': 	ret = moveRelLinearMetricXY(-endSwitchStepBackMertic, 0.0, true);
							break;
							
				case 'Y': 	ret = moveRelLinearMetricXY(0.0, -endSwitchStepBackMertic, true);
							break;
							
				case 'Z': 	ret = moveRelMetricZ(-endSwitchStepBackMertic);
							break;
			}
		}
		
		// calculate result
		if ( ret ) {

			switch ( axis ) {
				case 'X': 	result = (maxPos - minPos) * cncConfig->getDisplayFactX();
							break;
							
				case 'Y': 	result = (maxPos - minPos) * cncConfig->getDisplayFactY();
							break;
							
				case 'Z': 	result = (maxPos - minPos) * cncConfig->getDisplayFactZ();
							break;
			}
		}
	}
	
	reconfigureSimpleMove(ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveXToMinLimit() {
// This function move the max distiance and will be latest stopped by the end switch
// However, the PC and controller postions are not equal at the end!
// the call of reconfigureSimpleMove(true) will correct that
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double maxSteps = cncConfig->getMaxDimensionX() * (-1);
	bool ret = false;
	if ( prepareSimpleMove() == true ) {
		ret = moveRelLinearMetricXY(maxSteps, 0.0, true);
		if (ret)
			ret = moveRelLinearMetricXY(+endSwitchStepBackMertic, 0.0, true);
	}
	reconfigureSimpleMove(ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveXToMaxLimit() {
// This function move the max distiance and will be latest stopped by the end switch
// However, the PC and controller postions are not equal at the end!
// the call of reconfigureSimpleMove(true) will correct that
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double maxSteps = cncConfig->getMaxDimensionX();
	bool ret = false;
	if ( prepareSimpleMove() == true ) {
		ret = moveRelLinearMetricXY(maxSteps, 0.0, true);
		if (ret)
			ret = moveRelLinearMetricXY(-endSwitchStepBackMertic, 0.0, true);
	}
	reconfigureSimpleMove(ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveYToMinLimit() {
// This function move the max distiance and will be latest stopped by the end switch
// However, the PC and controller postions are not equal at the end!
// the call of reconfigureSimpleMove(true) will correct that
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double maxSteps = cncConfig->getMaxDimensionY() * (-1);
	bool ret = false;
	if ( prepareSimpleMove() == true ) {
		ret = moveRelLinearMetricXY(0.0, maxSteps, true);
		if (ret)
			ret = moveRelLinearMetricXY(0.0, +endSwitchStepBackMertic, true);
	}
	reconfigureSimpleMove(ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveYToMaxLimit() {
// This function move the max distiance and will be latest stopped by the end switch
// However, the PC and controller postions are not equal at the end!
// the call of reconfigureSimpleMove(true) will correct that
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double maxSteps = cncConfig->getMaxDimensionY();
	bool ret = false;
	if ( prepareSimpleMove() == true ) {
		ret = moveRelLinearMetricXY(0.0, maxSteps, true);
		if (ret)
			ret = moveRelLinearMetricXY(0.0, -endSwitchStepBackMertic, true);
	}
	reconfigureSimpleMove(ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveZToMinLimit() {
// This function move the max distiance and will be latest stopped by the end switch
// However, the PC and controller postions are not equal at the end!
// the call of reconfigureSimpleMove(true) will correct that
///////////////////////////////////////////////////////////////////
	double maxSteps = cncConfig->getMaxDimensionZ() * (-1);
	bool ret = false;
	if ( prepareSimpleMove() == true ) {
		ret = moveRelMetricZ(maxSteps);
		if (ret)
			ret = moveRelMetricZ(+endSwitchStepBackMertic);
	}
	reconfigureSimpleMove(ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveZToMaxLimit() {
// This function move the max distiance and will be latest stopped by the end switch
// However, the PC and controller postions are not equal at the end!
// the call of reconfigureSimpleMove(true) will correct that
///////////////////////////////////////////////////////////////////
	double maxSteps = cncConfig->getMaxDimensionZ();
	bool ret = false;
	if ( prepareSimpleMove() == true ) {
		ret = moveRelMetricZ(maxSteps);
		if (ret)
			ret = moveRelMetricZ(-endSwitchStepBackMertic);
	}
	reconfigureSimpleMove(ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveXToMid() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double maxSteps = cncConfig->getMaxDimensionX();
	bool ret = false;

	if ( prepareSimpleMove() == true ) {
		ret = moveRelLinearMetricXY(maxSteps, 0.0, true);
		if ( ret )
			ret = moveRelLinearMetricXY(-maxSteps/2, 0.0, true);
	}
	reconfigureSimpleMove(ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveYToMid() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double maxSteps = cncConfig->getMaxDimensionY();

	bool ret = false;
	if ( prepareSimpleMove() == true ) {
		ret = moveRelLinearMetricXY(0.0, maxSteps, true);
		if ( ret )
			ret = moveRelLinearMetricXY(0.0, -maxSteps/2, true);
	}
	reconfigureSimpleMove(ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveZToMid() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double maxSteps = cncConfig->getMaxDimensionZ();

	bool ret = false;
	if ( prepareSimpleMove() == true ) {
		ret = moveRelMetricZ(maxSteps);
		if ( ret )
			ret = moveRelMetricZ(-maxSteps/2);
	}
	reconfigureSimpleMove(ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::manualSimpleMoveMetric(double x, double y, double z, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double sX = x * cncConfig->getCalculationFactX();
	double sY = y * cncConfig->getCalculationFactY();
	double sZ = z * cncConfig->getCalculationFactZ();
	
	return manualSimpleMoveSteps((int32_t)round(sX), (int32_t)round(sY), (int32_t)round(sZ), alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool CncControl::manualSimpleMoveMetric3D(double x, double y, double z, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double sX = x * cncConfig->getCalculationFactX();
	double sY = y * cncConfig->getCalculationFactY();
	double sZ = z * cncConfig->getCalculationFactZ();
	
	return manualSimpleMoveSteps3D((int32_t)round(sX), (int32_t)round(sY), (int32_t)round(sZ), alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool CncControl::manualSimpleMoveSteps(int32_t x, int32_t y, int32_t z, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	bool ret = true;

	if ( x != 0 || y != 0 ) {
		ret = false;
		if ( prepareSimpleMove(false) == true ) {
			ret = moveRelLinearStepsXY(x, y, alreadyRendered );
			if ( limitStates.hasLimit() ) {
				
				if ( x != 0 ) {
					if ( limitStates.isXLimitStateValid() && limitStates.getXMinLimit() )
						ret = moveRelLinearMetricXY(+endSwitchStepBackMertic, 0.0, false);
						
					if ( limitStates.isXLimitStateValid() && limitStates.getXMaxLimit() )
						ret = moveRelLinearMetricXY(-endSwitchStepBackMertic, 0.0, false);
				}
				
				if ( y != 0 ) {
					if ( limitStates.isYLimitStateValid() && limitStates.getYMinLimit() )
						ret = moveRelLinearMetricXY(0.0, +endSwitchStepBackMertic, false);
						
					if ( limitStates.isYLimitStateValid() && limitStates.getYMaxLimit() )
						ret = moveRelLinearMetricXY(0.0, -endSwitchStepBackMertic, false);
				}
			}
				
			reconfigureSimpleMove(ret);
		}
	}
	
	if ( z != 0 ) {
		ret = false;
		if ( prepareSimpleMove(false) == true ) {
			ret = moveRelStepsZ(z);
			if ( ret && limitStates.hasLimit() ) {
				
				if ( limitStates.isZLimitStateValid() && limitStates.getZMinLimit() )
					ret = moveRelMetricZ(+endSwitchStepBackMertic);
					
				if ( limitStates.isZLimitStateValid() && limitStates.getZMaxLimit() )
					ret = moveRelMetricZ(-endSwitchStepBackMertic);
			}
				
			reconfigureSimpleMove(ret);
		}
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::manualSimpleMoveSteps3D(int32_t x, int32_t y, int32_t z, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	bool ret = true;
	
	if ( x != 0 || y != 0 || z != 0 ) {
		ret = false;
		if ( prepareSimpleMove(false) == true ) {
			ret = moveRelLinearStepsXYZ(x, y, z, alreadyRendered );
			
			if ( limitStates.hasLimit() ) {
				
				if ( x != 0 ) {
					if ( limitStates.isXLimitStateValid() && limitStates.getXMinLimit() )
						ret = moveRelLinearMetricXYZ(+endSwitchStepBackMertic, 0.0, 0.0, false);
						
					if ( limitStates.isXLimitStateValid() && limitStates.getXMaxLimit() )
						ret = moveRelLinearMetricXYZ(-endSwitchStepBackMertic, 0.0, 0.0, false);
				}
				
				if ( y != 0 ) {
					if ( limitStates.isYLimitStateValid() && limitStates.getYMinLimit() )
						ret = moveRelLinearMetricXYZ(0.0, +endSwitchStepBackMertic, 0.0,false);
						
					if ( limitStates.isYLimitStateValid() && limitStates.getYMaxLimit() )
						ret = moveRelLinearMetricXYZ(0.0, -endSwitchStepBackMertic, 0.0, false);
				}
				
				if ( z != 0 ) {
					if ( limitStates.isZLimitStateValid() && limitStates.getZMinLimit() )
						ret = moveRelLinearMetricXYZ(0.0, 0.0, +endSwitchStepBackMertic, false);
						
					if ( limitStates.isZLimitStateValid() && limitStates.getZMaxLimit() )
						ret = moveRelLinearMetricXYZ(0.0, 0.0, -endSwitchStepBackMertic, false);
				}
			}
				
			reconfigureSimpleMove(ret);
		}
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::prepareSimpleMove(bool enaleEventHandling) {
///////////////////////////////////////////////////////////////////
	logProcessingStart();
	initNextDuration();
	cncConfig->setAllowEventHandling(enaleEventHandling);
	activatePositionCheck(false);
	enableStepperMotors(true);
	
	// currently no checks implemented, if checks necessary do it here
	return true;
}
///////////////////////////////////////////////////////////////////
void CncControl::reconfigureSimpleMove(bool correctPositions) {
///////////////////////////////////////////////////////////////////
	enableStepperMotors(false);
	activatePositionCheck(true);
	resetDurationCounter();
	
	if ( validatePositions() == false && correctPositions == true ) {
		curAppPos = getControllerPos();
	}
		
	logProcessingEnd();
}
///////////////////////////////////////////////////////////////////
bool CncControl::hasControllerConfigControl() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	return ( GET_GUI_CTL(controllerConfig) != NULL );
}
///////////////////////////////////////////////////////////////////
void CncControl::clearControllerConfigControl() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	if ( hasControllerConfigControl() ) 
		GET_GUI_CTL(controllerConfig)->DeleteAllItems();
}
///////////////////////////////////////////////////////////////////
void CncControl::appendPidKeyValueToControllerConfig(int pid, const char* key, const char* value) {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	if ( hasControllerConfigControl() ) {
		DcmItemList rows;

		DataControlModel::addNumKeyValueRow(rows, pid, key, value);
		GET_GUI_CTL(controllerConfig)->Freeze();
		for (DcmItemList::iterator it = rows.begin(); it != rows.end(); ++it) {
			GET_GUI_CTL(controllerConfig)->AppendItem(*it);
		}
		GET_GUI_CTL(controllerConfig)->Thaw();
	}
}
///////////////////////////////////////////////////////////////////
bool CncControl::hasControllerPinControl() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	return ( GET_GUI_CTL(controllerPinReport) != NULL );
}
///////////////////////////////////////////////////////////////////
bool CncControl::hasControllerErrorControl() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	return ( GET_GUI_CTL(controllerErrorInfo) != NULL );
}
///////////////////////////////////////////////////////////////////
void CncControl::clearControllerPinControl() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	if ( hasControllerErrorControl() )
		GET_GUI_CTL(controllerPinReport)->DeleteAllItems();
}
///////////////////////////////////////////////////////////////////
void CncControl::clearControllerErrorControl() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	if ( hasControllerErrorControl() ) 
		GET_GUI_CTL(controllerErrorInfo)->DeleteAllItems();
}
///////////////////////////////////////////////////////////////////
void CncControl::appendNumKeyValueToControllerErrorInfo(const char* desc, int pin, int type, int mode, int value) {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	if ( hasControllerErrorControl() ) {
		DcmItemList rows;

		DataControlModel::addPinReportRow(rows, desc, pin, type, mode, value);
		GET_GUI_CTL(controllerPinReport)->Freeze();
		for (DcmItemList::iterator it = rows.begin(); it != rows.end(); ++it) {
			GET_GUI_CTL(controllerPinReport)->AppendItem(*it);
		}
		GET_GUI_CTL(controllerPinReport)->Thaw();
	}
}
///////////////////////////////////////////////////////////////////
void CncControl::appendNumKeyValueToControllerErrorInfo(int num, int code, const char* key, const char* value) {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	if ( hasControllerErrorControl() ) {
		DcmItemList rows;

		DataControlModel::addNumKeyValueRow(rows, num, code, key, value);
		guiCtlSetup->controllerErrorInfo->Freeze();
		for (DcmItemList::iterator it = rows.begin(); it != rows.end(); ++it) {
			GET_GUI_CTL(controllerErrorInfo)->AppendItem(*it);
		}
		GET_GUI_CTL(controllerErrorInfo)->Thaw();
	}
}
///////////////////////////////////////////////////////////////////
void CncControl::updatePreview3D(bool force) {
///////////////////////////////////////////////////////////////////
	displayCounter++;
	
	if ( IS_GUI_CTL_NOT_VALID(motionMonitor) )
		return;
		
	if ( force == true ) {
		GET_GUI_CTL(motionMonitor)->display();
		displayCounter = 0;
		return;
	}
	
	// Online drawing coordinates
	if ( cncConfig->isOnlineUpdateDrawPane() ) {
		if ( displayCounter%cncConfig->getUpdateInterval() == 0 ) {
			GET_GUI_CTL(motionMonitor)->display();
			displayCounter = 0;
		}
	}
}
///////////////////////////////////////////////////////////////////
void CncControl::sendIdleMessage() {
///////////////////////////////////////////////////////////////////
	if ( getSerial() == NULL )
		return;
		
	if ( getSerial()->isCommandActive() == true ) {
		return;
	}
	
	//clog <<  wxDateTime::UNow().FormatTime() << " - idle,  delay:" << getStepDelay() << endl;
	getSerial()->processIdle();
}




///////////////////////////////////////////////////////////////////
bool CncControl::setActiveFeedSpeed(CncSpeed s, double value) {
// value unit: mm/minute
///////////////////////////////////////////////////////////////////
	double xsRpm = 0.0;
	double ysRpm = 0.0;
	double zsRpm = 0.0;

	if ( value > 0.0 ) {
		double vMMS = value / 60.0;
		
		xsRpm = vMMS / cncConfig->getPitchX() * 60.0;
		ysRpm = vMMS / cncConfig->getPitchY() * 60.0;
		zsRpm = vMMS / cncConfig->getPitchZ() * 60.0;
	
	} else {
		
		xsRpm = cncConfig->getWorkSpeedXY();
		ysRpm = cncConfig->getWorkSpeedXY();
		zsRpm = cncConfig->getWorkSpeedZ();
	}
	
	bool bx = setActiveRpmSpeedX(s, (unsigned int)round(xsRpm));
	bool by = setActiveRpmSpeedY(s, (unsigned int)round(ysRpm));
	bool bz = setActiveRpmSpeedZ(s, (unsigned int)round(zsRpm));
	
	return  ( bx || by || bz );
}


///////////////////////////////////////////////////////////////////
bool CncControl::setActiveRpmSpeedXYZ(CncSpeed s, unsigned int value) {
///////////////////////////////////////////////////////////////////
	bool b1 = setActiveRpmSpeedZ(s, value);
	bool b2 = setActiveRpmSpeedXY(s, value);
	
	return (b1 || b2);
}
///////////////////////////////////////////////////////////////////
bool CncControl::setActiveRpmSpeedXY(CncSpeed s, unsigned int value) {
///////////////////////////////////////////////////////////////////
	bool b1 = setActiveRpmSpeedX(s, value);
	bool b2 = setActiveRpmSpeedY(s, value);
	
	return (b1 || b2);
}
///////////////////////////////////////////////////////////////////
bool CncControl::setActiveRpmSpeedX(CncSpeed s, unsigned int value) {
///////////////////////////////////////////////////////////////////
	speedType = s;
	
	if ( value == getRpmSpeedX() )
		return false;
	
	if ( ( value > 0 ) == false ) {
		switch( s ) {
			
			case CncSpeedWork: 	value = cncConfig->getWorkSpeedXY();
								break;
								
			case CncSpeedRapid:	value = cncConfig->getRapidSpeedXY();
								break;
		}
	}
	
	rpmSpeedX = value;
	
	/*
	typedef UpdateManagerThread::Event Event;
	static Event evt;
	
	if ( GET_GUI_CTL(mainFrame) )
		GET_GUI_CTL(mainFrame)->umPostEvent(evt.SpeedEvent(getRpmSpeedX(), getRpmSpeedY(), getRpmSpeedZ()));
	*/
	
	return true;
}

///////////////////////////////////////////////////////////////////
bool CncControl::setActiveRpmSpeedY(CncSpeed s, unsigned int value) {
///////////////////////////////////////////////////////////////////
	speedType = s;
	
	if ( value == getRpmSpeedY() )
		return false;
	
	if ( ( value > 0 ) == false ) {
		switch( s ) {
			
			case CncSpeedWork: 	value = cncConfig->getWorkSpeedXY();
								break;
								
			case CncSpeedRapid:	value = cncConfig->getRapidSpeedXY();
								break;
		}
	}
	
	rpmSpeedY = value;
	
	/*
	typedef UpdateManagerThread::Event Event;
	static Event evt;
	
	
	if ( GET_GUI_CTL(mainFrame) )
		GET_GUI_CTL(mainFrame)->umPostEvent(evt.SpeedEvent(getRpmSpeedX(), getRpmSpeedY(), getRpmSpeedZ()));
	*/
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::setActiveRpmSpeedZ(CncSpeed s, unsigned int value) {
///////////////////////////////////////////////////////////////////
	speedType = s;
	
	if ( value == getRpmSpeedZ() )
		return false;
	
	if ( ( value > 0 ) == false ) {
		switch( s ) {
			
			case CncSpeedWork: 	value = cncConfig->getWorkSpeedZ();
								break;
								
			case CncSpeedRapid:	value = cncConfig->getRapidSpeedZ();
								break;
		}
	}
	
	rpmSpeedZ = value;
/*
	typedef UpdateManagerThread::Event Event;
	static Event evt;
	
	if ( GET_GUI_CTL(mainFrame) )
		GET_GUI_CTL(mainFrame)->umPostEvent(evt.SpeedEvent(getRpmSpeedX(), getRpmSpeedY(), getRpmSpeedZ()));
*/
	return true;
}





///////////////////////////////////////////////////////////////////
const wxString& CncControl::getSpeedAsString() {
///////////////////////////////////////////////////////////////////
	static wxString ret;

	switch( speedType ) {
		
		case CncSpeedWork: 	ret.assign("W");
							break;
							
		case CncSpeedRapid:	ret.assign("R");
							break;
	}
	
	return ret;
}
