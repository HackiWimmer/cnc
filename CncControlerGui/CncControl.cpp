#include <iostream>
#include <sstream>
#include <cfloat>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/tglbtn.h>
#include <wx/slider.h>
#include <wx/dataview.h>
#include <wx/propgrid/manager.h>
#include <wx/evtloop.h>
#include <wx/richmsgdlg.h>
#include "OSD/CncAsyncKeyboardState.h"
#include "OSD/SerialOSD.h"
#include "GlobalFunctions.h"
#include "DataControlModel.h"
#include "SerialPort.h"
#include "SerialEmulatorNull.h"
#include "SerialEmulatorTextStreamer.h"
#include "SerialEmulatorSvgStreamer.h"
#include "SerialEmulatorGCodeStreamer.h"
#include "SerialEmulatorBinaryStreamer.h"
#include "SerialThreadStub.h"
#include "CncMotionMonitor.h"
#include "CncExceptions.h"
#include "CncCommon.h"
#include "CncContext.h"
#include "CncTemplateContext.h"
#include "CncBoundarySpace.h"
#include "CncAnchorInfo.h"
#include "CncGamePad.h"
#include "CncFileNameService.h"
#include "CncLoggerView.h"
#include "CncLoggerProxy.h"
#include "CncPathListManager.h"
#include "CncMillingSoundController.h"
#include "wxCrafterImages.h"
#include "MainFrame.h"
#include "CncControl.h"

#define CNC_PRINT_INTERACTIVE_FUNCT_A(msg) //CNC_PRINT_FUNCT_A(msg)

///////////////////////////////////////////////////////////////////
CncControl::CncControl(CncPortType pt) 
: currentClientId					(CLIENT_ID.INVALID)
, currentInteractiveMoveInfo		()
, setterMap							()
, runMode							(M_RealRun)
, serialPort						(NULL)
, realRunSerial						(NULL)
, dryRunSerial						(NULL)
, zeroAppPos						(0, 0, 0)
, startAppPos						(0, 0, 0)
, curAppPos							(0, 0, 0)
, curCtlPos							(0, 0, 0)
, realtimeFeedSpeed_MM_MIN			(0.0)
, defaultFeedSpeedRapid_MM_MIN		(THE_CONFIG->getDefaultRapidSpeed_MM_MIN())
, defaultFeedSpeedWork_MM_MIN		(THE_CONFIG->getDefaultRapidSpeed_MM_MIN())
, speedMemory_MM_MIN				()
, configuredSpeedMode				(CncSpeedRapid)
, configuredFeedSpeed_MM_MIN		(0.0)
, configuredSpeedModePreviewFlag	(false)
, configuredSpindleSpeed			(THE_CONFIG->getSpindleSpeedMax())
, durationCounter					(0)
, interruptState					(false)
, positionOutOfRangeFlag			(false)
, positionCheck						(true)
, ctrlPowerState					(CPS_NOT_INITIALIZED)
, spindlePowerState					(SPINDLE_STATE_OFF)
, stepDelay							(0)
, lastCncHeartbeatValue				(0)
{
//////////////////////////////////////////////////////////////////
	// Serial factory
	if      ( pt == CncPORT ) 				realRunSerial = new SerialOSD(this);
	else if ( pt == CncEMU_NULL )			realRunSerial = new SerialEmulatorNULL(this);
	else if ( pt == CncEMU_TXT )			realRunSerial = new SerialEmulatorTextStreamer(this);
	else if ( pt == CncEMU_SVG )			realRunSerial = new SerialEmulatorSvgStreamer(this);
	else if ( pt == CncEMU_GCODE )			realRunSerial = new SerialEmulatorGCodeStreamer(this);
	else if ( pt == CncEMU_BIN )			realRunSerial = new SerialEmulatorBinaryStreamer(this);
	else if ( pt == CncPORT_EMU_ARDUINO )	realRunSerial = new SerialThreadStub(this);
	else 									realRunSerial = new SerialEmulatorNULL(this);
	
	// join the current Serial
	serialPort = realRunSerial;
	
	if ( pt == CncPORT || pt == CncPORT_EMU_ARDUINO )
	{
		// only in this case create a try run serial
		// for all other serials this isn't necessary 
		dryRunSerial = new SerialEmulatorNULL(this);
	}
	
	serialPort->enableSpyOutput();
	serialPort->setContextInterface(THE_CONTEXT->templateContext);
}
///////////////////////////////////////////////////////////////////
CncControl::~CncControl() {
///////////////////////////////////////////////////////////////////
	switchRunMode(M_RealRun);
	
	setterMap.clear();
	switchSpindleOff();
	
	// safety
	disconnect();

	cncDELETE(realRunSerial);
	cncDELETE(dryRunSerial);
}
//////////////////////////////////////////////////////////////////
void CncControl::switchRunMode(RunMode m) {
//////////////////////////////////////////////////////////////////
	if ( m == M_DryRun && dryRunSerial != NULL )
	{
		serialPort = dryRunSerial;
		serialPort->setContextInterface(THE_CONTEXT->templateContext);
		
		connect("DryRun");
		setup(true);
	}
	else
	{
		const bool switchBack = ( serialPort == dryRunSerial );
		if ( switchBack == true )
			disconnect();
		
		serialPort = realRunSerial;
		serialPort->setContextInterface(THE_CONTEXT->templateContext);
		
		if ( switchBack == true )
		{
			const bool withSound = ( THE_CONFIG->getSimulateMillingWithSoundFlag() && serialPort->getPortType() == CncPORT_EMU_ARDUINO );
			CncSpindleSound::activate(withSound);
			
			// Regarding the serial switch: 
			// The locally stored controller position has to be aligned to the now present serial.
			// and furthermore the app position to the ctl pos
			curCtlPos = requestControllerPos();
			curAppPos = curCtlPos;
			
			// update position text controls
			postAppPosition(PID_XYZ_POS_MAJOR);
			postCtlPosition(PID_XYZ_POS_MAJOR);
			
			// update the motion monitor too
			monitorPosition(curCtlPos);
		}
	}
}
//////////////////////////////////////////////////////////////////
void CncControl::setClientId(long id) {
//////////////////////////////////////////////////////////////////
	currentClientId = id;
	
	ContextInterface* ci = getSerial()->getContextInterface();
	if ( ci != NULL )
		ci->notifyClientId(id);
}
//////////////////////////////////////////////////////////////////
const CncDoublePosition CncControl::getStartPosMetric() {
//////////////////////////////////////////////////////////////////
	const CncDoublePosition retValue(startAppPos.getX() * THE_CONFIG->getDisplayFactX(),
									 startAppPos.getY() * THE_CONFIG->getDisplayFactY(),
									 startAppPos.getZ() * THE_CONFIG->getDisplayFactZ()
	);
	return retValue;
}
//////////////////////////////////////////////////////////////////
const CncDoublePosition CncControl::getCurAppPosMetric() {
//////////////////////////////////////////////////////////////////
	const CncDoublePosition retValue(curAppPos.getX() * THE_CONFIG->getDisplayFactX(),
									 curAppPos.getY() * THE_CONFIG->getDisplayFactY(),
									 curAppPos.getZ() * THE_CONFIG->getDisplayFactZ()
	);
	return retValue;
}
//////////////////////////////////////////////////////////////////
const CncDoublePosition CncControl::getCurCtlPosMetric() {
//////////////////////////////////////////////////////////////////
	const CncDoublePosition retValue(curCtlPos.getX() * THE_CONFIG->getDisplayFactX(),
									 curCtlPos.getY() * THE_CONFIG->getDisplayFactY(),
									 curCtlPos.getZ() * THE_CONFIG->getDisplayFactZ()
	);
	return retValue;
}
//////////////////////////////////////////////////////////////////
const CncLongPosition CncControl::getMinPositions() {
//////////////////////////////////////////////////////////////////
	const CncLongPosition retValue(	curCtlPos.getXMin(), 
									curCtlPos.getYMin(), 
									curCtlPos.getZMin()
	);
	return retValue;
}//////////////////////////////////////////////////////////////////
const CncLongPosition CncControl::getMaxPositions() {
//////////////////////////////////////////////////////////////////
	const CncLongPosition retValue( curCtlPos.getXMax(), 
									curCtlPos.getYMax(),
									curCtlPos.getZMax()
	);
	return retValue;
}
//////////////////////////////////////////////////////////////////
const CncDoublePosition CncControl::getMinPositionsMetric() {
//////////////////////////////////////////////////////////////////
	const CncDoublePosition retValue(curCtlPos.getXMin() * THE_CONFIG->getDisplayFactX(),
									 curCtlPos.getYMin() * THE_CONFIG->getDisplayFactY(),
									 curCtlPos.getZMin() * THE_CONFIG->getDisplayFactZ()
	);
	return retValue;
}
//////////////////////////////////////////////////////////////////
const CncDoublePosition CncControl::getMaxPositionsMetric() {
//////////////////////////////////////////////////////////////////
	const CncDoublePosition retValue(curCtlPos.getXMax() * THE_CONFIG->getDisplayFactX(),
									 curCtlPos.getYMax() * THE_CONFIG->getDisplayFactY(),
									 curCtlPos.getZMax() * THE_CONFIG->getDisplayFactZ()
	);
	return retValue;
}
//////////////////////////////////////////////////////////////////
const CncLongPosition::Watermarks CncControl::getWaterMarks() {
//////////////////////////////////////////////////////////////////
	CncLongPosition::Watermarks retValue;
	return curAppPos.getWatermarks(retValue);
}
//////////////////////////////////////////////////////////////////
const CncDoublePosition::Watermarks CncControl::getWaterMarksMetric() {
//////////////////////////////////////////////////////////////////
	CncDoublePosition::Watermarks retValue;

	CncLongPosition::Watermarks xyz = CncControl::getWaterMarks();
	retValue.xMin = xyz.xMin * THE_CONFIG->getDisplayFactX();
	retValue.xMax = xyz.xMax * THE_CONFIG->getDisplayFactX();

	retValue.yMin = xyz.yMin * THE_CONFIG->getDisplayFactY();
	retValue.yMax = xyz.yMax * THE_CONFIG->getDisplayFactY();
	
	retValue.zMin = xyz.zMin * THE_CONFIG->getDisplayFactZ();
	retValue.zMax = xyz.zMax * THE_CONFIG->getDisplayFactZ();

	return retValue;
}
///////////////////////////////////////////////////////////////////
bool CncControl::execute(const unsigned char* buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	return getSerial()->execute(buffer, nbByte);
}
///////////////////////////////////////////////////////////////////
bool CncControl::processSetter(unsigned char pid, int32_t value) {
///////////////////////////////////////////////////////////////////
	cnc::SetterValueList values;
	values.push_back(value);
	
	return processSetter(pid, values);
}
///////////////////////////////////////////////////////////////////
bool CncControl::processSetter(unsigned char pid, const cnc::SetterValueList& values) {
///////////////////////////////////////////////////////////////////
	if ( isInterrupted() )
		return false;

	if ( isConnected() == false )
		return false;
	
	if ( pid != PID_SEPARATOR )
	{
		if ( THE_CONFIG->getAvoidDupSetterValuesFlag() )
		{
			auto it = setterMap.find((int)pid);
			if ( it != setterMap.end() )
			{
				// any value(s) are changed?
				cnc::SetterValueList smvl = it->second;
				if ( smvl.size() == values.size() )
				{
					bool somethingChanged = false;
					auto itvv=values.begin();
					for ( auto itmv=smvl.begin(); itmv != smvl.end(); itmv++, itvv++)
					{
						if ( *itmv != *itvv )
						{
							somethingChanged = true;
							break;
						}
					}
					if ( somethingChanged == false )
						return true;
				}
			}
		}
			
		if ( serialPort->processSetter(pid, values) == false )
		{
			std::cerr << std::endl << "CncControl::processSetter: Setter failed." << std::endl;
			std::cerr << " Id:    " << ArduinoPIDs::getPIDLabel((int)pid) << std::endl;
			std::cerr << " Value(s): ";
			cnc::traceSetterValueList(std::cerr, pid, values, pid < PID_FLOAT_RANG_START ? 1 : FLT_FACT);
			std::cerr << std::endl;
			return false;
		}
		
		// store
		appendToSetterMap(pid, values);
	}
	
	// publish setter event
	THE_APP->addSetter(pid, values);
	
	wxCommandEvent comevt(wxEVT_COMMAND_BUTTON_CLICKED);
	wxPostEvent(THE_APP->GetBtRefreshSetterList(), comevt);
	
	return true;
}
///////////////////////////////////////////////////////////////////
void CncControl::appendToSetterMap(unsigned char pid, const cnc::SetterValueList& values) {
///////////////////////////////////////////////////////////////////
	// add all pids which are should not stored 
	switch ( pid )
	{
		case PID_X_POS:
		case PID_Y_POS:
		case PID_Z_POS:	return;
	}
	
	setterMap[pid] = values;
}
///////////////////////////////////////////////////////////////////
bool CncControl::processSetterList(const Setters& setup) {
///////////////////////////////////////////////////////////////////
	for ( auto itl = setup.begin(); itl != setup.end(); ++itl)
	{
		cnc::SetterValueList svl = (*itl).values;
		unsigned char pid = (*itl).pid;
		
		if ( processSetter(pid, svl) == false )
			return false;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
void CncControl::resetSetterMap() {
///////////////////////////////////////////////////////////////////
	setterMap.clear();
}
///////////////////////////////////////////////////////////////////
bool CncControl::setup(bool doReset) {
///////////////////////////////////////////////////////////////////
	wxASSERT(serialPort);
	wxASSERT(THE_CONFIG);
	
	// always reset the map here to definitely reinitialize the controller
	resetSetterMap();
	
	if ( serialPort->isConnected() == false ) 
		return false;
		
	// init setup
	processSetter(PID_SEPARATOR, SEPARARTOR_SETUP);
	
	// reset controller on demand
	if ( doReset == true )
	{
		if ( reset() == false )
		{
			CNC_CERR_FUNCT_A("reset controller failed!\n");
			return false;
		}
		
		// Firmware check
		std::stringstream ss;
		processCommand(CMD_PRINT_VERSION, ss);
		
		std::cout << " Firmware check . . . [Available: " << ss.str() << "; Required: " << FIRMWARE_VERSION << "] . . .";
		REGISTER_LAST_FILLED_LOGGER_ROW
		
		if ( wxString(FIRMWARE_VERSION) == ss.str().c_str() )
		{
			std::cout << std::endl;
			SET_RESULT_FOR_REGISTERED_LOGGER_ROW_OK
		}
		else
		{ 
			cnc::cex1 << " Firmware is possibly not compatible!" << std::endl;
			SET_RESULT_FOR_REGISTERED_LOGGER_ROW_WARNING
		}
	}
	
	// always switch the tool off - safety - but may be already done by reset();
	switchSpindleOff(true);
	
	// evaluate limit states
	evaluateLimitState();
	
	std::cout << " Starting controller initialization . . .\n";
	REGISTER_LAST_FILLED_LOGGER_ROW
	
	// setup probe mode
	if ( enableProbeMode(THE_CONTEXT->isProbeMode()) == false )
	{
		SET_RESULT_FOR_REGISTERED_LOGGER_ROW_ERROR
		CNC_CERR_FUNCT_A(" Probe mode configuration failed!\n")
		return false;
	}
	
	if ( THE_CONFIG->getStepsX() <= 0 ) { std::cerr << CNC_LOG_FUNCT << ": Invalid Steps configuration, X axis! Steps: " << THE_CONFIG->getStepsX() << std::endl; return false; }
	if ( THE_CONFIG->getStepsY() <= 0 ) { std::cerr << CNC_LOG_FUNCT << ": Invalid Steps configuration, Y axis! Steps: " << THE_CONFIG->getStepsX() << std::endl; return false; }
	if ( THE_CONFIG->getStepsZ() <= 0 ) { std::cerr << CNC_LOG_FUNCT << ": Invalid Steps configuration, Z axis! Steps: " << THE_CONFIG->getStepsX() << std::endl; return false; }
	
	// process initial setters
	Setters setup;
	
	cnc::SetterValueList accelList;
	accelList.push_back(FEEDRATE_FACT	* (float)(THE_CONFIG->getPitchX() / THE_CONFIG->getStepsX()));
	accelList.push_back(FEEDRATE_FACT	* (float)(THE_CONFIG->getPitchY() / THE_CONFIG->getStepsY()));
	accelList.push_back(FEEDRATE_FACT	* (float)(THE_CONFIG->getPitchZ() / THE_CONFIG->getStepsZ()));
	accelList.push_back(FLT_FACT		* (float)(THE_CONFIG->getHighPulsWidthX()));
	accelList.push_back(FLT_FACT		* (float)(THE_CONFIG->getHighPulsWidthY()));
	accelList.push_back(FLT_FACT		* (float)(THE_CONFIG->getHighPulsWidthZ()));
	accelList.push_back(FLT_FACT		* (float)(THE_CONFIG->getAccelFunctParamA()));
	accelList.push_back(FLT_FACT		* (float)(THE_CONFIG->getAccelFunctParamB()));
	accelList.push_back(FLT_FACT		* (float)(THE_CONFIG->getAccelFunctParamC() /60.0));
	accelList.push_back(FLT_FACT		* (float)(THE_CONFIG->getDeaccelFunctParamA()));
	accelList.push_back(FLT_FACT		* (float)(THE_CONFIG->getDeaccelFunctParamB()));
	accelList.push_back(FLT_FACT		* (float)(THE_CONFIG->getDeaccelFunctParamC() / 60.0));
	setup.push_back(SetterTuple(PID_ACCEL_PROFILE, accelList));
	
	setup.push_back(SetterTuple(PID_POS_REPLY_THRESHOLD, THE_CONFIG->getReplyThresholdSteps()));
	
	const int32_t dirValueX = THE_CONFIG->getInverseCtrlDirectionXFlag() ? INVERSED_INCREMENT_DIRECTION_VALUE : NORMALIZED_INCREMENT_DIRECTION_VALUE;
	const int32_t dirValueY = THE_CONFIG->getInverseCtrlDirectionYFlag() ? INVERSED_INCREMENT_DIRECTION_VALUE : NORMALIZED_INCREMENT_DIRECTION_VALUE;
	const int32_t dirValueZ = THE_CONFIG->getInverseCtrlDirectionZFlag() ? INVERSED_INCREMENT_DIRECTION_VALUE : NORMALIZED_INCREMENT_DIRECTION_VALUE;
	const int32_t dirValueH = INVERSED_INCREMENT_DIRECTION_VALUE;

	setup.push_back(SetterTuple(PID_INC_DIRECTION_VALUE_X, dirValueX));
	setup.push_back(SetterTuple(PID_INC_DIRECTION_VALUE_Y, dirValueY));
	setup.push_back(SetterTuple(PID_INC_DIRECTION_VALUE_Z, dirValueZ));
	setup.push_back(SetterTuple(PID_INC_DIRECTION_VALUE_H, dirValueH));
	
	setup.push_back(SetterTuple(PID_FEEDRATE_H,			FEEDRATE_FACT * (float)(THE_CONFIG->getPitchH() / THE_CONFIG->getStepsH())));
	setup.push_back(SetterTuple(PID_PULSE_WIDTH_HIGH_H,	THE_CONFIG->getHighPulsWidthH()));
	
	if ( processSetterList(setup) == false)
	{
		SET_RESULT_FOR_REGISTERED_LOGGER_ROW_ERROR
		CNC_CERR_FUNCT_A(" Calling processSetterList() failed!\n");
		return false;
	}
	
	enableStepperMotors(ENABLE_STATE_ON);
	
	// speed setup
	changeSpeedToDefaultSpeed_MM_MIN(CncSpeedRapid);
	changeCurrentSpindleSpeed_U_MIN(THE_CONFIG->getSpindleSpeedDefault());
	
	SET_RESULT_FOR_REGISTERED_LOGGER_ROW_OK
	
	StreamBufferHighlighter sbh(std::clog);
	CNC_CLOG_A("Ready to go . . .")
	
	serialPort->notifySetupSuccesfullyFinsihed();
	return true;
}
///////////////////////////////////////////////////////////////////
long CncControl::convertDoubleToCtrlLong(unsigned char id, float f) { 
///////////////////////////////////////////////////////////////////
	if ( cnc::fltCmp::lt(f, MIN_LONG / FLT_FACT) )
	{
		CNC_CERR_FUNCT_A("Invalid double value: '%f' for PID: %s", f, ArduinoPIDs::getPIDLabel(id))
		return MIN_LONG; 
	}
		
	if ( cnc::fltCmp::gt(f, MAX_LONG / FLT_FACT ) )
	{
		CNC_CERR_FUNCT_A("Invalid double value: '%f' for PID: %s", f, ArduinoPIDs::getPIDLabel(id));
		return MAX_LONG; 
	}
	
	return ( f * FLT_FACT ); 
}
///////////////////////////////////////////////////////////////////
bool CncControl::disconnect() {
///////////////////////////////////////////////////////////////////
	if ( serialPort->isConnected() )
	{
		try
		{
			// this can fail if the corresponding connection is broken
			// e.g. the usb cable is switched off, etc.
			enableStepperMotors(ENABLE_STATE_OFF);
		}
		catch(...)
		{
			// the catch is placed here to enable a finally disconnect
			// in all circumstances to be able to connect to a different port
			// to get the application into a valid state again
			CNC_CERR_FUNCT_A(": The final try to disable the stepper motors failed");
		}
		
		std::cout << " Disconnecting serial port . . .\n";
		REGISTER_LAST_FILLED_LOGGER_ROW
		
		serialPort->disconnect();
		
		SET_RESULT_FOR_REGISTERED_LOGGER_ROW_OK
	}
	
	CncSpindleSound::deactivate();
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::connect(const char * portName) {
///////////////////////////////////////////////////////////////////
	wxASSERT(serialPort);
	disconnect();
	
	wxFileName fn(portName);
	std::clog << "Try to connect to: " << serialPort->getClassName() 
	                                   << "("
									   << (fn.DirExists() ? fn.GetFullName() : wxString(portName) )
									   << ")" 
									   << " - " 
									   << CncFileNameService::getSession()
									   << std::endl;
									  
	bool ret = serialPort->connect(portName);
	if ( ret == true )
	{
	
		std::clog << " . . . Connection established\n";
		SET_RESULT_FOR_LAST_FILLED_LOGGER_ROW_OK
		
		const bool withSound = ( THE_CONFIG->getSimulateMillingWithSoundFlag() && serialPort->getPortType() == CncPORT_EMU_ARDUINO );
		CncSpindleSound::activate(withSound);
	}
	else
	{
		std::cerr << " . . . Connection refused\n";
		SET_RESULT_FOR_LAST_FILLED_LOGGER_ROW_ERROR
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
void CncControl::onPeriodicallyAppEvent() {
///////////////////////////////////////////////////////////////////
	wxASSERT(serialPort);
	
	serialPort->onPeriodicallyAppEvent(isInterrupted());
}
///////////////////////////////////////////////////////////////////
bool CncControl::popSerial(bool dispatchUserEvents) {
///////////////////////////////////////////////////////////////////
	if ( isInterrupted() == true )
		return false;
		
	if ( isSpyOutputOn() )
		cnc::spy.addDebugEntry(CNC_LOG_FUNCT_A(": before popSerial()"));
			
	const bool prev = THE_CONTEXT->isAllowEventHandling();
	THE_CONTEXT->setAllowEventHandling(dispatchUserEvents);
	
		const bool ret = serialPort->popSerial();
	
	THE_CONTEXT->setAllowEventHandling(prev);
	
	// catch up held back events on demand and things like 
	// the motion monitor update
	dispatchEventQueue();
	
	if ( isSpyOutputOn() )
		cnc::spy.addDebugEntry(CNC_LOG_FUNCT_A(": after popSerial()"));

	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::pushCommand(const unsigned char cmd) {
///////////////////////////////////////////////////////////////////
	if ( isInterrupted() == true )
		return false;
	
	wxASSERT(serialPort);
	return serialPort->pushCommand(cmd);
}
///////////////////////////////////////////////////////////////////
bool CncControl::processCommand(const unsigned char cmd, std::ostream& txtCtl) {
///////////////////////////////////////////////////////////////////
	if ( isInterrupted() == true )
		return false;
	
	wxASSERT(serialPort);
	return serialPort->processCommand(cmd, txtCtl);
}
///////////////////////////////////////////////////////////////////
void CncControl::setZeroPosX(int32_t x, int32_t y, int32_t z) {
///////////////////////////////////////////////////////////////////
	curAppPos  .setXYZ(x, y, z);
	zeroAppPos .setXYZ(x, y, z);
	startAppPos.setXYZ(x, y, z);
	
	if ( isConnected() == true )
	{
		cnc::SetterValueList values;
		values.push_back(x);
		values.push_back(y);
		values.push_back(z);
		if ( processSetter(PID_XYZ_POS, values) == false )
		{
			CNC_CERR_FUNCT_A("Can't zero controllers XYZ position!");
			return;
		}
	}
	
	curCtlPos.setXYZ(x, y, z);
}
///////////////////////////////////////////////////////////////////
void CncControl::setZeroPosX(int32_t v) {
///////////////////////////////////////////////////////////////////
	curAppPos  .setX(v);
	zeroAppPos .setX(v);
	startAppPos.setX(v);
	
	if ( isConnected() == true )
	{
		if ( processSetter(PID_X_POS, (int32_t)(v)) == false )
		{
			CNC_CERR_FUNCT_A("Can't zero controllers X position!");
			return;
		}
	}
	
	curCtlPos.setX(v);
}
///////////////////////////////////////////////////////////////////
void CncControl::setZeroPosY(int32_t v) {
///////////////////////////////////////////////////////////////////
	curAppPos  .setY(v);
	zeroAppPos .setY(v);
	startAppPos.setY(v);
	
	if ( isConnected() == true )
	{
		if ( processSetter(PID_Y_POS, (int32_t)(v)) == false )
		{
			CNC_CERR_FUNCT_A("Can't zero controllers Y position!")
			return;
		}
	}
	
	curCtlPos.setY(v);
}
///////////////////////////////////////////////////////////////////
void CncControl::setZeroPosZ(int32_t v) {
///////////////////////////////////////////////////////////////////
	curAppPos  .setZ(v);
	zeroAppPos .setZ(v);
	startAppPos.setZ(v);
	
	if ( isConnected() == true )
	{
		if ( processSetter(PID_Z_POS, (int32_t)(v)) == false )
		{
			CNC_CERR_FUNCT_A("Can't zero controllers Z position!")
			return;
		}
	}
	
	curCtlPos.setZ(v);
}
///////////////////////////////////////////////////////////////////
void CncControl::setZeroPos(bool x, bool y, bool z) {
///////////////////////////////////////////////////////////////////
	int32_t zVal = 0L;
	if ( THE_BOUNDS->includesWpt() == true )
		zVal = (long)round(THE_BOUNDS->getWorkpieceThickness() * THE_CONFIG->getCalculationFactZ());
	
	if ( x ) setZeroPosX(0);
	if ( y ) setZeroPosY(0);
	if ( z ) setZeroPosZ(zVal);
	
	postAppPosition(PID_XYZ_POS_MAJOR);
	postCtlPosition(PID_XYZ_POS_MAJOR);
}
///////////////////////////////////////////////////////////////////
void CncControl::setZeroPos() {
///////////////////////////////////////////////////////////////////
	setZeroPos(true, true, true);
}
///////////////////////////////////////////////////////////////////
void CncControl::setStartPos() {
///////////////////////////////////////////////////////////////////
	startAppPos = curAppPos;
}
///////////////////////////////////////////////////////////////////
void CncControl::interrupt(const char* why) {
///////////////////////////////////////////////////////////////////
	CNC_CERR_FUNCT_A(wxString::Format("CncControl: Interrupted: %s", why ? why : ""));
	
	// try always to switch the spindle off
	const bool force = true;
	switchSpindleOff(force);
	
	// to do this only once
	if ( interruptState == false )
	{
		interruptState = true;
		
		CNC_CERR_FUNCT_A("CncControl: Throw a CncInterruption exception")
		throw CncInterruption(why);
	}
}
///////////////////////////////////////////////////////////////////
bool CncControl::isReadyToRun() {
///////////////////////////////////////////////////////////////////
	MainFrame::Notification notification;
	notification.location	= MainFrame::Notification::NL_MonitorView;
	notification.type		= 'E';
	notification.title		= CNC_LOG_FUNCT_B("");
	
	if ( isConnected() == false )
	{
		notification.message = "The controller isn't connected!";
		THE_APP->displayNotification(notification);
		return false;
	}
	
	if ( isInterrupted() == true )
	{
		notification.message = "The controller is interrupted. A reset is required!";
		THE_APP->displayNotification(notification);
		return false;
	}
	
	if ( isCommandActive() == true )
	{
		notification.message = "The controller is busy!";
		THE_APP->displayNotification(notification);
		return false;
	}
	
	// all relevant micro controller states have to be fresh 
	// queried by getter(PID_QUERY_READY_TO_RUN), otherwise
	// if local application variables or members are used here, 
	// there state isn't correct especially in the case of dry run!!!
	
	// query the serial port
	const CncSpindlePowerState evalSpindlePowerState = evaluateSpindlePowerState();
	if ( evalSpindlePowerState != spindlePowerState || evalSpindlePowerState == SPINDLE_STATE_ON )
	{
		const bool force = true;
		switchSpindleOff(force);
		
		if ( evalSpindlePowerState == SPINDLE_STATE_ON )
			cnc::cex1 << CNC_LOG_FUNCT_A(": Invalid Spindle state! Switch spindle off before start running\n");
		// else switch the state silent
	}
	
	GetterValues list;
	getSerial()->processGetter(PID_QUERY_READY_TO_RUN, list);
		
	if ( list.size() != 1 )
	{
		CNC_CERR_FUNCT_A(": Unable due to the corresponding state of the serial port:")
		return false;
	}
	
	return ( list.at(0) == 1L );
}
///////////////////////////////////////////////////////////////////
bool CncControl::processGetter(unsigned char pid, GetterValues& ret) { 
///////////////////////////////////////////////////////////////////
	return getSerial()->processGetter(pid, ret); 
}
///////////////////////////////////////////////////////////////////
bool CncControl::processMovePodium(int32_t steps, bool exact) {
///////////////////////////////////////////////////////////////////
	return getSerial()->processMovePodium(steps, exact); 
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
	zeroAppPos .resetWatermarks();
	startAppPos.resetWatermarks();
	curAppPos  .resetWatermarks();
	curCtlPos  .resetWatermarks();
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::reset() {
///////////////////////////////////////////////////////////////////
	getSerial()->purge();
	
	resetInterrupt();
	resetPositionOutOfRangeFlag();
	
	INC_LOGGER_INDENT
	
	CNC_COUT_A("Try to reset the controller . . .")
	bool ret = processCommand(CMD_RESET_CONTROLLER, std::cerr);
	if ( ret == true ) 
	{ 
		CNC_CLOG_A("Controller successfully reseted");
		SET_RESULT_FOR_LAST_FILLED_LOGGER_ROW_OK
	} 
	else
	{ 
		CNC_CERR_A("Controller reset failed")
		SET_RESULT_FOR_LAST_FILLED_LOGGER_ROW_ERROR
		return false; 
	}
	
	DEC_LOGGER_INDENT
	
	// do this after the controller is reseted, 
	// because setZeroPos will determine a new controller position on demand
	setZeroPos();
	
	curCtlPos = requestControllerPos();
	postCtlPosition(PID_XYZ_POS_MAJOR);
	
	evaluateLimitState();
	switchSpindleOff(true);
	
	return true;
}

///////////////////////////////////////////////////////////////////
unsigned int CncControl::getDurationCount() {
///////////////////////////////////////////////////////////////////
	return durationCounter ;
}
///////////////////////////////////////////////////////////////////
bool CncControl::hasNextDuration() {
///////////////////////////////////////////////////////////////////
	return ( durationCounter < getDurationCount() );
}
///////////////////////////////////////////////////////////////////
void CncControl::resetDurationCounter() {
///////////////////////////////////////////////////////////////////
	durationCounter = 0;
	THE_APP->GetPassingCount()->SetValue(wxString() << durationCounter);
}
///////////////////////////////////////////////////////////////////
void CncControl::initNextDuration() {
///////////////////////////////////////////////////////////////////
	durationCounter++;
	THE_APP->GetPassingCount()->SetValue(wxString() << durationCounter);
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
bool CncControl::changeCurrentSpindleSpeed_U_MIN(double value) {
///////////////////////////////////////////////////////////////////
	if ( THE_CONFIG->getSpindleSpeedSupportFlag() == false )
		return true;
	
	// values less then zero defines as default ans indicates 
	// not spindle speed change
	if ( cnc::dblCmp::lt(value, 0.0) )
		return true;
	
	const int16_t val = cnc::cvnSpindleSpeed_U_MIN_ToRaw(value);
	const int16_t rng = THE_CONFIG->getSpindleSpeedStepRange();
	
	const ArdoObj::SpindleTupleValue eVal = ArdoObj::SpindleTuple::encode(val, rng);
	if ( processSetter(PID_SPINDLE_SPEED, eVal) == false )
	{
		CNC_CERR_FUNCT_A("encode failed");
		return false;
	}
	
	ContextInterface* ci = getSerial()->getContextInterface();
	if ( ci != NULL )
		ci->notifySpindleSpeed(PID_SPINDLE_SPEED, eVal);
	
	CncSpindleSound::adjust(value);
	configuredSpindleSpeed = value;
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::changeCurrentFeedSpeedXYZ_MM_SEC(float value, CncSpeedMode s) {
///////////////////////////////////////////////////////////////////
	return changeCurrentFeedSpeedXYZ_MM_MIN(value * 60, s);
}
///////////////////////////////////////////////////////////////////
bool CncControl::changeCurrentFeedSpeedXYZ_MM_MIN(float value, CncSpeedMode s) {
///////////////////////////////////////////////////////////////////
	// always reset the real-time speed value, but don't
	// use MAX_FEED_SPEED_VALUE here because it is declared as MIN_LONG
	// realtimeFeedSpeed_MM_MIN = MAX_FEED_SPEED_VALUE;
	realtimeFeedSpeed_MM_MIN = 0.0;
	
	if ( false )
		std::clog << CNC_LOG_FUNCT_A(": %.1f\n", value);
	
	// safety checks 
	const double maxValue = THE_CONFIG->getMaxSpeedXYZ_MM_MIN();
	if ( value <= 0.0 )			value = 0.0;
	if ( value > maxValue )		value = maxValue;
	
	bool force = configuredSpeedModePreviewFlag;
	
	// always reset
	configuredSpeedModePreviewFlag = false;
	
	// avoid the code below if nothing will change
	if ( configuredSpeedMode != s || force == true )
	{
		configuredSpeedMode	= s;
		force				= true;
	}
	
	// avoid the setter below if nothing is/will change(d)
	if ( cnc::dblCompare(configuredFeedSpeed_MM_MIN, value) == false || force == true )
	{
		configuredFeedSpeed_MM_MIN	= value;
		
		const int32_t val = configuredFeedSpeed_MM_MIN * FLT_FACT / 60;
		const char mode   = cnc::getCncSpeedTypeAsCharacter(configuredSpeedMode);
		
		const ArdoObj::SpeedTupleValue eVal = ArdoObj::SpeedTuple::encode(mode, val);
		if ( processSetter(PID_SPEED_MM_SEC, eVal) == false )
		{
			CNC_CERR_FUNCT_A("processSetter(PID_SPEED_MM_SEC) failed");
			return false;
		}
		
		const Trigger::SpeedChange tr(configuredSpeedMode, configuredFeedSpeed_MM_MIN);
		processTrigger(tr);
		
		ContextInterface* ci = getSerial()->getContextInterface();
		if ( ci != NULL )
			ci->notifyStepperSpeed(PID_SPEED_MM_SEC, eVal);
			
		speedMemory_MM_MIN[configuredSpeedMode] = configuredFeedSpeed_MM_MIN;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::changeSpeedToDefaultSpeed_MM_MIN(CncSpeedMode s) {
///////////////////////////////////////////////////////////////////
	double value = 0.0;
	
	switch( s )
	{
		case CncSpeedWork: 			value = THE_CONFIG->getDefaultWorkSpeed_MM_MIN(); 	break;
		case CncSpeedRapid:			value = THE_CONFIG->getDefaultRapidSpeed_MM_MIN();	break;
		case CncSpeedMax:			value = THE_CONFIG->getMaxSpeedXYZ_MM_MIN();		break;
		case CncSpeedUserDefined:	return true;
	}
	
	return changeCurrentFeedSpeedXYZ_MM_MIN(value, s);
}
///////////////////////////////////////////////////////////////////
bool CncControl::changeSpeedToPrevStoredSpeed_MM_MIN(CncSpeedMode s) {
///////////////////////////////////////////////////////////////////
	return changeCurrentFeedSpeedXYZ_MM_MIN(speedMemory_MM_MIN[s], s);
}
///////////////////////////////////////////////////////////////////
void CncControl::setDefaultRapidSpeed_MM_MIN(float s) { 
///////////////////////////////////////////////////////////////////
	if ( s <= 0.0)
		return;
		
	if ( s > THE_CONFIG->getMaxSpeedXYZ_MM_MIN() )
		return;
	
	defaultFeedSpeedRapid_MM_MIN = s; 
}
///////////////////////////////////////////////////////////////////
void CncControl::setDefaultWorkSpeed_MM_MIN(float s)  { 
///////////////////////////////////////////////////////////////////
	if ( s <= 0.0)
		return;
		
	if ( s > THE_CONFIG->getMaxSpeedXYZ_MM_MIN() )
		return;

	defaultFeedSpeedWork_MM_MIN  = s; 
}
///////////////////////////////////////////////////////////////////
bool CncControl::isPositionOutOfRange(const CncLongPosition& pos, bool trace) {
///////////////////////////////////////////////////////////////////
	if ( positionCheck == false )
		return false;
		
	// will only be done for emulation ports. It didn't makes sense for a cnc run
	// see below
	if ( serialPort->getPortType() != CncPORT ) {
		// in the real cnc life this is not good enough. the check has to be done by
		// the end switches at last.
		bool error = false;
		CncLongPosition::Watermarks wm;
		pos.getWatermarks(wm);
		
		if ( (wm.xMax - wm.xMin)/THE_CONFIG->getCalculationFactX() > THE_BOUNDS->getMaxDimensionMetricX() ) error = true;
		if ( (wm.yMax - wm.yMin)/THE_CONFIG->getCalculationFactY() > THE_BOUNDS->getMaxDimensionMetricY() ) error = true;
		if ( (wm.zMax - wm.zMin)/THE_CONFIG->getCalculationFactZ() > THE_BOUNDS->getMaxDimensionMetricZ() ) error = true;
	
		if ( error == true && trace == true ) {
			std::cerr << "Position out of range!" << std::endl;
			std::cerr << " Max valid X dimension: " << THE_BOUNDS->getMaxDimensionMetricX() << std::endl;
			std::cerr << " Max valid Y dimension: " << THE_BOUNDS->getMaxDimensionMetricY() << std::endl;
			std::cerr << " Max valid Z dimension: " << THE_BOUNDS->getMaxDimensionMetricZ() << std::endl;
			std::cerr << " Pos: " << pos << std::endl;
			std::cerr << " Min Watermark: " << wm.xMin << "," << wm.yMin << "," << wm.zMin << "," << std::endl;
			std::cerr << " Max Watermark: " << wm.xMax << "," << wm.yMax << "," << wm.zMax << "," << std::endl;
			std::cerr << " Calculated spread X :" <<  (wm.xMax - wm.xMin)/THE_CONFIG->getCalculationFactX() << std::endl;
			std::cerr << " Calculated spread Y :" <<  (wm.yMax - wm.yMin)/THE_CONFIG->getCalculationFactY() << std::endl;
			std::cerr << " Calculated spread Z :" <<  (wm.zMax - wm.zMin)/THE_CONFIG->getCalculationFactZ() << std::endl;
			
			return true;
		}
	}
	
	return false;
}
///////////////////////////////////////////////////////////////////
void CncControl::monitorPosition(const CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	// motion monitor
	static CncLongPosition prevPos;
	
	if ( pos != prevPos )
	{
		if ( THE_APP->getMotionMonitor() )
			THE_APP->getMotionMonitor()->appendVertex(getClientId(), getConfiguredSpeedMode(), pos);
		
		prevPos.set(pos);
		
		if ( THE_CONFIG->getInterruptByPosOutOfRangeFlag() == true )
		{
			if ( isPositionOutOfRange(pos, true) == true )
			{
				interrupt("Position Out Of Range");
				updatePreview3D();
			}
		} 
		else 
		{
			if ( isPositionOutOfRange(pos, false) == true )
			{
				positionOutOfRangeFlag = true;
				updatePreview3D();
			}
		}
	}
}
///////////////////////////////////////////////////////////////////
void CncControl::waitActive(unsigned int millis) {
///////////////////////////////////////////////////////////////////
	THE_APP->waitActive(millis, true);
}
///////////////////////////////////////////////////////////////////
bool CncControl::dispatchEventQueue() {
///////////////////////////////////////////////////////////////////
	// Event handling, enables the interrupt functionallity
	static const CncMilliTimespan  thresholdEvent   = 100;
	static       CncMilliTimestamp tsLastDispatch   = 0;
	static       CncMilliTimestamp tsLastUpdate     = 0;
	
	if ( isInterrupted() )
	{
		std::cerr << CNC_LOG_FUNCT_A(": Interrupt detected\n");
		return false;
	}
	
	if ( currentInteractiveMoveInfo.driver == IMD_GAMEPAD )
	{
		// Check the gamepad status also here to stop immediately
		// Or in other words as fast as possible
		static CncGamepad gamepad;
		gamepad.refresh();
			
		// stop immediately . . .
		if ( gamepad.hasEmptyMovement() )
			if ( stopInteractiveMove() == false )
				CNC_CERR_FUNCT_A(" stopInteractiveMove() failed");
	}
	
	if ( THE_CONTEXT->isAllowEventHandling() )
	{
		const CncMilliTimespan timespanMonitor = CncTimeFunctions::getTimeSpan(CncTimeFunctions::getMilliTimestamp(), tsLastUpdate);
		const CncMilliTimespan timespanEvent   = CncTimeFunctions::getTimeSpan(CncTimeFunctions::getMilliTimestamp(), tsLastDispatch);
		
		if ( timespanMonitor >= THE_CONTEXT->getUpdateInterval() )
		{
			updatePreview3D();
			tsLastUpdate = CncTimeFunctions::getMilliTimestamp();
		}
		
		if ( timespanEvent >= thresholdEvent )
		{
			THE_APP->dispatchAll();
			tsLastDispatch = CncTimeFunctions::getMilliTimestamp();
		}
	}
	
	if ( CncAsyncKeyboardState::isEscapePressed() != 0 )
	{
		// assign ESC to an interrupt event only if the emergency button is active
		if ( THE_CONFIG->getTheApp()->GetBtnEmergenyStop()->IsEnabled() == true )
		{
			CNC_CERR_FUNCT_A(": ESCAPE key detected")
			interrupt("ESCAPE key detected");
		}
	}
	
	if ( isInterrupted() )
	{
		CNC_CERR_FUNCT_A("dispatchEventQueue: Interrupt detected")
	}
	
	return isInterrupted() == false;
}
///////////////////////////////////////////////////////////////////
bool CncControl::SerialCallback() {
///////////////////////////////////////////////////////////////////
	static const ContollerInfo ci;
	return SerialControllerCallback(ci);
}
///////////////////////////////////////////////////////////////////
bool CncControl::SerialControllerCallback(const ContollerInfo& ci) {
///////////////////////////////////////////////////////////////////
	// Event handling, enables the interrupt functionality
	if ( dispatchEventQueue() == false )
		return false;
	
	switch ( ci.infoType ) 
	{
		// --------------------------------------------------------
		case CITDefault:
		{
			break;
		}
		// --------------------------------------------------------
		case CITHandshake:
		{
			if ( false )
				std::cout << "handshake: " << ArduinoCMDs::getCMDLabel(ci.command) << " = " << ArduinoPIDs::getPIDLabel(ci.handshake) << std::endl;
				
			if ( ci.handshake == RET_QUIT ) 
			{
				// adjust the pc position
				curAppPos = curCtlPos;
				postCtlPosition(PID_XYZ_POS_MAJOR);
				postAppPosition(PID_XYZ_POS_MAJOR, true);
				
				//THE_APP->updateAppPositionControls();
				//THE_APP->updateCtlPositionControls();
			}
			
			break;
		}
		// --------------------------------------------------------
		case CITHeartbeat:
		{
			std::stringstream ss;
			ss << "Heartbeat received - Value: " << ci.heartbeatValue;
			
			const CtrlPowerState prevCtrlPowerState = ctrlPowerState;
			ctrlPowerState = CPS_UNKNOWN;
			
			if ( ci.healtyState == true ) {
				ctrlPowerState = ci.healtyStateValue > 0 ? CPS_ON : CPS_OFF;
				ss << " : " << getCtrlPowerStateAsStr(ctrlPowerState);
			}
			
			// Check against previous to get a single message per change 
			if ( ctrlPowerState != CPS_ON && ctrlPowerState != prevCtrlPowerState )
			{
				std::cerr	<< "Wrong Controller Power State!"
							<< std::endl
							<< " It changed from '"
							<< getCtrlPowerStateAsStr(prevCtrlPowerState)
							<< "' to '" 
							<< getCtrlPowerStateAsStr(ctrlPowerState)
							<< "'" 
							<< std::endl;
							
				SET_RESULT_FOR_LAST_FILLED_LOGGER_ROW_ERROR
			}
			
			THE_APP->notifyControllerHeartbeat();
			lastCncHeartbeatValue = ci.heartbeatValue;
			
			if ( ci.limitState == true ) 
			{
				CncInterface::ILS::States ls(ci.limitStateValue);
				displayLimitStates(ls);
				ss << " : " << ls.getValueAsString();
				ss << " (" << (int)ci.limitStateValue << ")";
			}
			
			if ( ci.supportState == true ) 
			{
				CncInterface::ISP::States sp(ci.supportStateValue);
				displaySupportStates(sp);
				ss << " : " << sp.getValueAsString();
				ss << " (" << (int)ci.supportStateValue << ")";
			}
			else
			{
				displayUnknownSupportStates();
			}
			
			displayHealtyStates();
			break;
		}
		
		// --------------------------------------------------------
		case CITLimit:
		{
			//CNC_CEX2_FUNCT_A("::L: %ld, %ld, %ld", ci.xLimit, ci.yLimit, ci.zLimit);
			displayLimitStates(ci.xLimit, ci.yLimit, ci.zLimit);
			break;
		}
		
		// --------------------------------------------------------
		case CITPosition:
		{
			bool syncAndPostAppPosToo = ci.synchronizeAppPos == true;
			
			// update controller position
			switch ( ci.posType ) 
			{
				case PID_H_POS: 		// nothing more to do here
										//CNC_CLOG_FUNCT_A("%ld", ci.hCtrlPos)
										break;
										
				case PID_X_POS: 		curCtlPos.setX(ci.xCtrlPos); 
										if ( syncAndPostAppPosToo ) 
											curAppPos.setX(ci.xCtrlPos);
										break;
										
				case PID_Y_POS: 		curCtlPos.setY(ci.yCtrlPos); 
										if ( syncAndPostAppPosToo ) 
											curAppPos.setY(ci.yCtrlPos);
										break;
				
				case PID_Z_POS: 		curCtlPos.setZ(ci.zCtrlPos); 
										if ( syncAndPostAppPosToo ) 
											curAppPos.setZ(ci.zCtrlPos);
										break;
				
				case PID_XYZ_POS:
				case PID_XYZ_POS_MAJOR:
				case PID_XYZ_POS_DETAIL:
				default:				curCtlPos.setXYZ(ci.xCtrlPos, ci.yCtrlPos, ci.zCtrlPos);
										if ( ci.hasSpeedInformation() )
											realtimeFeedSpeed_MM_MIN = ci.feedSpeed;
										
										if ( syncAndPostAppPosToo ) 
											curAppPos.setXYZ(ci.xCtrlPos, ci.yCtrlPos, ci.zCtrlPos);
										
										if ( false )
											std::cout << "CITPosition: speed: " << realtimeFeedSpeed_MM_MIN << std::endl;
			}
			
			// display controller coordinates
			postCtlPosition(ci.posType);
			
			if ( syncAndPostAppPosToo ) 
				postAppPosition(ci.posType);
				
			// motion monitor
			monitorPosition(curCtlPos);
			
			break;
		}
		
		// --------------------------------------------------------
		default:
			std::cerr << "CncControl::SerialControllerCallback:" << std::endl;
			std::cerr << " No handler defined for controller info type:" << ci.infoType << std::endl;
	}

	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::SerialMessageCallback(const ControllerMsgInfo& cmi) {
///////////////////////////////////////////////////////////////////
	// Event handling, enables the interrupt functionality
	if ( dispatchEventQueue() == false )
		return false;
	
	wxString msg(cmi.message.str().c_str());
	const wxDateTime now = wxDateTime::UNow();
	const char type = (char)msg[0];
	const char last = (char)msg[msg.length() - 1];
	
	msg.assign(msg.SubString(1, msg.length() - 1));
	if ( last != '\n')
		msg.append('\n');
	
	if ( type != 'D' ) {
		MainFrame::Notification notification;
		notification.location	= MainFrame::Notification::Location::NL_MonitorView;
		notification.type		= type;
		notification.title		= "Controller Callback";
		notification.message	= msg;
		THE_APP->displayNotification(notification);
	}
	
	// -----------------------------------------------------------
	auto format = [](wxString& s) 
	{
		if ( s.Last() == '\n' ) 
			s = s.BeforeLast('\n');
			
		s.Trim();
		return s;
	};
	
	switch ( type )
	{
		case 'W':	cnc::msg.logWarning(now.Format("Warning Message received: %H:%M:%S.%l\n"));
					cnc::msg.logWarning(msg);
					cnc::cex1 << "Received the following CNC Controller Warning: '" << format(msg) << "'\n";
					break;
					
		case 'E':	cnc::msg.logError(now.Format("Error Message received: %H:%M:%S.%l\n"));
					cnc::msg.logError(msg);
					std::cerr << "Received the following CNC Controller Error: '" << format(msg) << "'\n";
					break;
					
		case 'D':	cnc::msg.logDebug(now.Format("Debug Message received: %H:%M:%S.%l\n"));
					cnc::msg.logDebug(msg);
					cnc::cex1 << "Serial Remote Debug: " << msg;
					break;
					
		default:	cnc::msg.logInfo(now.Format("Info Message received: %H:%M:%S.%l\n"));
					cnc::msg.logInfo(msg);
					std::cout << "Received the following CNC Controller Information: '" << format(msg) << "'\n";
	}
	
	cnc::msg.setTextColour(wxColour(192, 192, 192));
	cnc::msg << "-------------------------------------------------------------------------------------------\n";
	cnc::msg.resetTextAttr();

	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::SerialExecuteControllerCallback(const ContollerExecuteInfo& cei) {
///////////////////////////////////////////////////////////////////
	auto checkSetterCount = [](unsigned char pid, size_t count, size_t ref)
	{
		bool ret = (count == ref);
		
		if ( ret == false )
		{
			std::cerr << "CncControl::SerialExecuteControllerCallback(): Invalid Setter(" << ArduinoPIDs::getPIDLabel(pid) << ") value count: " 
					  << "Given: " << count << ", Reference: " << ref
					  << std::endl;
		}
		
		return ret;
	};
	
	// Event handling, enables the interrupt functionality
	if ( dispatchEventQueue() == false )
		return false;
	
	switch ( cei.infoType )
	{
		// --------------------------------------------------------
		case CEITSetter:
		{
			size_t size = cei.setterValueList.size();
			
			switch ( cei.setterPid )
			{
				case PID_SPEED_MM_SEC:
				case PID_SPEED_MM_MIN:
				{
					if ( checkSetterCount(cei.setterPid, size, 1) == false )
						return false;
					
					const char mode       = ArdoObj::SpeedTuple::decodeMode(cei.setterValueList.front());
					const int32_t val     = ArdoObj::SpeedTuple::decodeValue_MMSec1000(cei.setterValueList.front());
					const float unitFact  = cei.setterPid == PID_SPEED_MM_SEC ? 60.0 : 1.0;
					
					configuredFeedSpeed_MM_MIN	= unitFact * val / FLT_FACT;
					configuredSpeedMode			= cnc::getCncSpeedType(mode);
					
					break;
				}
				case PID_SPINDLE_SWITCH:
				{
					if ( checkSetterCount(cei.setterPid, size, 1) == false )
						return false;
						
					// during an execute command - e. g. *.bct files -
					// this has to be done here
					spindlePowerState = cei.setterValueList.front() ? SPINDLE_STATE_ON : SPINDLE_STATE_OFF;
					THE_APP->decorateSpindleState(spindlePowerState);
					
					break;
				}
				case PID_SPINDLE_SPEED:
				{
					if ( checkSetterCount(cei.setterPid, size, 1) == false )
						return false;
						
					//const int16_t rng = ArdoObj::SpindleTuple::decodeRange(cei.setterValueList.front());
					const int16_t val = ArdoObj::SpindleTuple::decodeValue(cei.setterValueList.front());
					
					// during an execute command - e. g. *.bct files -
					// this has to be done here
					configuredSpindleSpeed = cnc::cnvSpidleSpeedRaw_To_U_MIN(val);
					
					// nothing more to do. the gui update is already handled by 
					// CncLCDPositionPanel::updateValues()
					break;
				}
				case PID_ENABLE_STEPPERS:
				{
					// nothing to do here
					break;
				}
				default:
				{
					CNC_CERR_FUNCT_A(	"Not registered Setter PID: (%d) %s [%s]",
										(int) cei.setterPid,
										ArduinoPIDs::getPIDLabel(cei.setterPid), 
										ArduinoPIDs::getPIDUnit(cei.setterPid)
					) 
				}
			}
			
			break;
		}
		
		// --------------------------------------------------------
		default:
			std::cerr << "CncControl::SerialExecuteControllerCallback:"  << std::endl;
			std::cerr << " No handler defined for controller info type:" << cei.infoType << std::endl;
	}
	return true;
}
///////////////////////////////////////////////////////////////////
void CncControl::postAppPosition(unsigned char pid, bool force) {
///////////////////////////////////////////////////////////////////
	static CncLongPosition lastAppPos;
	
	if ( THE_CONTEXT->isOnlineUpdateCoordinates() ) 
	{
		// app positions are always from type major
		// so || pid == PID_XYZ_POS_MAJOR isn't necessary
		// the comparison below is necessary, because this method is also called
		// from the serialCallback(...) which not only detects pos changes
		if ( lastAppPos != curAppPos || force == true)
		{
			PositionStorage::addPos(PositionStorage::TRIGGER_APP_POS, curAppPos);
			
			THE_APP->addAppPosition(	pid, 
										getClientId(), 
										configuredSpeedMode, 
										getConfiguredFeedSpeed_MM_MIN(), 
										getRealtimeFeedSpeed_MM_MIN(), 
										curAppPos
									);
		}
	}
	
	lastAppPos.set(curAppPos);
}
///////////////////////////////////////////////////////////////////
void CncControl::postCtlPosition(unsigned char pid) {
///////////////////////////////////////////////////////////////////
	if ( THE_CONTEXT->isOnlineUpdateCoordinates() )
	{
		PositionStorage::addPos(PositionStorage::TRIGGER_CTL_POS, curCtlPos);
		
		// a position comparison isn't necessary here because the serialControllerCallback(...)
		// call this method only on position changes
		THE_APP->addCtlPosition(	pid, 
									getClientId(), 
									cnc::getCncSpeedTypeAsCharacter(configuredSpeedMode), 
									getConfiguredFeedSpeed_MM_MIN(), 
									getRealtimeFeedSpeed_MM_MIN(), 
									curCtlPos
								);
								
	}
}
///////////////////////////////////////////////////////////////////
bool CncControl::processMoveImage(const CncMoveSequenceImage& moveImage) {
///////////////////////////////////////////////////////////////////
	if ( isInterrupted() == true )
		return false;

	wxASSERT(serialPort);
	return serialPort->processMoveImage(moveImage);
}
///////////////////////////////////////////////////////////////////
bool CncControl::processMoveSequence(CncMoveSequence& moveSequence) {
///////////////////////////////////////////////////////////////////
	if ( isInterrupted() == true )
		return false;

	wxASSERT(serialPort);
	return serialPort->processMoveSequence(moveSequence);
}
///////////////////////////////////////////////////////////////////
bool CncControl::simpleMoveXYToZeroPos(CncDimensions dim) {
///////////////////////////////////////////////////////////////////
	InitializeMove im(this);
	return moveXYToZeroPos(dim);
}
///////////////////////////////////////////////////////////////////
bool CncControl::simpleMoveXYZToZeroPos(CncDimensions dim) {
///////////////////////////////////////////////////////////////////
	InitializeMove im(this);
	return moveXYZToZeroPos(dim);
}
///////////////////////////////////////////////////////////////////
bool CncControl::simpleMoveZToZeroPos() {
///////////////////////////////////////////////////////////////////
	InitializeMove im(this);
	return moveZToZeroPos();
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveXYToZeroPos(CncDimensions dim) {
///////////////////////////////////////////////////////////////////
	if ( curAppPos != zeroAppPos )
	{
		const int32_t moveX = zeroAppPos.getX() - curAppPos.getX(); 
		const int32_t moveY = zeroAppPos.getY() - curAppPos.getY();
		
		if ( dim == CncDimension2D )
		{
			return moveRelLinearStepsXY(moveX, moveY, false);
		}
		else
		{
			if ( moveRelLinearStepsXY(moveX, 0, false) == false )
				return false;
				
			if ( moveRelLinearStepsXY(0, moveY, false) == false )
				return false;
		}
	}
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveXYZToZeroPos(CncDimensions dim) {
///////////////////////////////////////////////////////////////////
	if ( curAppPos != zeroAppPos )
	{
		const int32_t moveX = zeroAppPos.getX() - curAppPos.getX();
		const int32_t moveY = zeroAppPos.getY() - curAppPos.getY();
		const int32_t moveZ = zeroAppPos.getZ() - curAppPos.getZ();
		
		if ( dim == CncDimension3D )
		{
			return moveRelLinearStepsXYZ(moveX, moveY, moveZ, false);
			
		}
		else if ( dim == CncDimension2D )
		{
			if ( moveRelLinearStepsXYZ(0, 0, moveZ, false) == false )
				return false;
				
			if ( moveRelLinearStepsXYZ(moveX, moveY, 0, false) == false )
				return false;
			
		}
		else
		{
			if ( moveRelLinearStepsXYZ(moveX, 0, 0, false) == false )
				return false;
				
			if ( moveRelLinearStepsXYZ(0, moveY, 0, false) == false )
				return false;
				
			if ( moveRelLinearStepsXYZ(0, 0, moveZ, false) == false )
				return false;
		}
	}
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveZToZeroPos() {
///////////////////////////////////////////////////////////////////
	bool ret = true;
	if ( curAppPos != zeroAppPos ) 
	{
		const int32_t moveZ = zeroAppPos.getZ() - curAppPos.getZ();
		ret = moveRelLinearStepsXYZ(0, 0, moveZ, false);
	}
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveXYToStartPos() {
///////////////////////////////////////////////////////////////////
	const CncLongPosition pos(	startAppPos.getX(), 
								startAppPos.getY(),
								curAppPos.getZ()
	);
	return moveToPos(pos);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveToStartPos() {
///////////////////////////////////////////////////////////////////
	return moveToPos(startAppPos);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveToPos(const CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	bool ret = true;
	if ( curAppPos != pos )
	{
		const int32_t moveX = pos.getX() - curAppPos.getX(); 
		const int32_t moveY = pos.getY() - curAppPos.getY();
		const int32_t moveZ = pos.getZ() - curAppPos.getZ();
		ret = moveRelLinearStepsXYZ(moveX, moveY, moveZ, false);
	}
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveRelStepsX(int32_t distance) {
///////////////////////////////////////////////////////////////////
	if ( distance == 0 )
		return true;
		
	// one axis moves are always linear, 
	// as a consequence alreadyRendered can be true
	// but to see the detail positions use false
	const bool alreadyRendered = false;
	return serialPort->processMoveX(distance, alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveRelStepsY(int32_t distance) {
///////////////////////////////////////////////////////////////////
	if ( distance == 0 )
		return true;
		
	// one axis moves are always linear, 
	// as a consequence alreadyRendered can be true
	// but to see the detail positions use false
	const bool alreadyRendered = false;
	return serialPort->processMoveY(distance, alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveRelStepsZ(int32_t distance) {
///////////////////////////////////////////////////////////////////
	if ( distance == 0 )
		return true;
		
	// one axis moves are always linear, 
	// as a consequence alreadyRendered can be true
	// but to see the detail positions use false
	const bool alreadyRendered = false;
	return serialPort->processMoveZ(distance, alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveRelMetricZ(double z) {
///////////////////////////////////////////////////////////////////
	const double sZ = z * THE_CONFIG->getCalculationFactZ();
	
	return moveRelStepsZ((int32_t)wxRound(sZ));
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveRelLinearStepsXY(int32_t x1, int32_t y1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	//avoid empty steps
	if ( x1 == 0 && y1 == 0 )
		return true;
	
	return serialPort->processMoveXY(x1, y1, alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveRelLinearMetricXY(double x1, double y1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	const double sX1 = wxRound(x1 * THE_CONFIG->getCalculationFactX());
	const double sY1 = wxRound(y1 * THE_CONFIG->getCalculationFactY());
	
	return moveRelLinearStepsXY((int32_t)sX1,
	                            (int32_t)sY1,
	                            alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveRelLinearStepsXYZ(int32_t x1, int32_t y1, int32_t z1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	//avoid empty steps
	if ( x1 == 0 && y1 == 0 && z1 == 0 )
		return true;
	
	return serialPort->processMoveXYZ(x1, y1, z1, alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveRelLinearMetricXYZ(double x1, double y1, double z1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	const double sX1 = wxRound(x1 * THE_CONFIG->getCalculationFactX());
	const double sY1 = wxRound(y1 * THE_CONFIG->getCalculationFactY());
	const double sZ1 = wxRound(z1 * THE_CONFIG->getCalculationFactZ());
	
	return moveRelLinearStepsXYZ((int32_t)sX1,
	                             (int32_t)sY1,
	                             (int32_t)sZ1,
	                             alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveAbsLinearStepsXYZ(int32_t x1, int32_t y1, int32_t z1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	return moveRelLinearStepsXYZ(x1 - curAppPos.getX(),
	                             y1 - curAppPos.getY(),
	                             z1 - curAppPos.getZ(),
	                             alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveAbsMetricZ(double z) {
///////////////////////////////////////////////////////////////////
	const double sZ = wxRound(z * THE_CONFIG->getCalculationFactZ());
	
	return moveRelStepsZ((int32_t)sZ - curAppPos.getZ());
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveAbsLinearMetricXY(double x1, double y1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	const double sX1 = wxRound(x1 * THE_CONFIG->getCalculationFactX());
	const double sY1 = wxRound(y1 * THE_CONFIG->getCalculationFactY());
	
	return moveRelLinearStepsXY((int32_t)sX1 - curAppPos.getX(), 
	                            (int32_t)sY1 - curAppPos.getY(),
	                            alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveAbsLinearMetricXYZ(double x1, double y1, double z1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	const double sX1 = wxRound(x1 * THE_CONFIG->getCalculationFactX());
	const double sY1 = wxRound(y1 * THE_CONFIG->getCalculationFactY());
	const double sZ1 = wxRound(z1 * THE_CONFIG->getCalculationFactZ());
	
	return moveRelLinearStepsXYZ((int32_t)sX1 - curAppPos.getX(),
	                             (int32_t)sY1 - curAppPos.getY(),
	                             (int32_t)sZ1 - curAppPos.getZ(),
	                             alreadyRendered);
}
///////////////////////////////////////////////////////////////////
CncSpindlePowerState CncControl::evaluateSpindlePowerState() {
///////////////////////////////////////////////////////////////////
	GetterValues gv;
	
	if ( processGetter(PID_SPINDLE_SWITCH, gv) == false )
	{
		CNC_CERR_FUNCT_A(": Unable to evaluate spindle power state!")
		return false;
	}
	
	if ( gv.size() == 0 )
	{
		CNC_CERR_FUNCT_A(": Unable to read spindle power state!")
		return false;
	}
	
	return gv[0] != 0;
}
///////////////////////////////////////////////////////////////////
bool CncControl::switchSpindleState(CncSpindlePowerState state, bool force) {
///////////////////////////////////////////////////////////////////
	return state == SPINDLE_STATE_ON ? switchSpindleOn() : switchSpindleOff(force);
}
///////////////////////////////////////////////////////////////////
bool CncControl::switchSpindleOn() {
///////////////////////////////////////////////////////////////////
	if ( isInterrupted() )
		return false;
	
	//CNC_PRINT_FUNCT_A(": spindlePowerState == SPINDLE_STATE_OFF = %d", spindlePowerState == SPINDLE_STATE_OFF)
	if ( spindlePowerState == SPINDLE_STATE_OFF )
	{ 
		if ( processSetter(PID_SPINDLE_SWITCH, SPINDLE_STATE_ON) )
		{
			CncSpindleSound::play(getConfiguredSpindleSpeed());
			
			spindlePowerState = SPINDLE_STATE_ON;
			
			ContextInterface* ci = getSerial()->getContextInterface();
			if ( ci != NULL )
				ci->notifySpindleOn();
				
			THE_APP->decorateSpindleState(spindlePowerState);
			THE_APP->waitActive(500);
		}
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::switchSpindleOff(bool force) {
///////////////////////////////////////////////////////////////////
	if ( isInterrupted() )
		return false;
	
	if ( spindlePowerState == SPINDLE_STATE_ON || force == true )
	{
		if ( processSetter(PID_SPINDLE_SWITCH, SPINDLE_STATE_OFF) )
		{
			CncSpindleSound::stop();
			
			spindlePowerState = SPINDLE_STATE_OFF;
			THE_APP->decorateSpindleState(spindlePowerState);
			
			ContextInterface* ci = getSerial()->getContextInterface();
			if ( ci != NULL )
				ci->notifySpindleOff();
		}
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::displayGetterList(const PidList& pidList) {
///////////////////////////////////////////////////////////////////
	if ( pidList.size() == 0 )
		return false;
	
	GetterListValues map;
	if ( getSerial()->processGetterList(pidList, map) == false )
	{
		CNC_CERR_FUNCT_A("Error while processing getter list")
		return false;
	}
	
	// show content:
	std::cout << "Getter List Report:" << std::endl;
	for (GetterListValues::iterator itl=map.begin(); itl!=map.end(); ++itl)
	{
		std::cout << ' ' << ArduinoPIDs::getPIDLabel(itl->first) << " => [";
			
		GetterValues& list = itl->second;
		for (GetterValues::iterator itp = list.begin() ; itp != list.end(); ++itp)
		{
			if ( itp != list.begin() )
				std::cout << ',';
			
			std::cout << *itp;
		}
		
		std::cout << "]\n";
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
const CncLongPosition CncControl::requestControllerPos() {
///////////////////////////////////////////////////////////////////
	static CncLongPosition controllerPos;
	std::vector<int32_t> list;
	
	if ( isConnected() == true && isInterrupted() == false )
		getSerial()->processGetter(PID_XYZ_POS, list);
		
	if ( list.size() != 3 )
	{
		controllerPos.setXYZ(0, 0, 0);
		
		if ( isConnected() == true && isInterrupted() == false )
		{
			CNC_CERR_FUNCT_A(	"Unable to evaluate controllers position:\n" \
								" Received value count: %zu, expected: 3",  
								list.size()
			)
		}
	}
	else
	{
		controllerPos.setXYZ(list.at(0), list.at(1), list.at(2));
	}
	
	return controllerPos;
}
///////////////////////////////////////////////////////////////////
const CncLongPosition CncControl::requestControllerLimitState() {
///////////////////////////////////////////////////////////////////
	static CncLongPosition localStaticLimitStates;
	GetterValues list;
	
	if ( isInterrupted() == false )
		getSerial()->processGetter(PID_LIMIT, list);
	
	if ( list.size() != 3 )
	{
		if ( isInterrupted() == false )
		{
			CNC_CERR_FUNCT_A(	"Unable to evaluate controllers limit state:\n" \
								" Received value count: %zu, expected: 3", 
								list.size()
			)
		}
	}
	else
	{
		localStaticLimitStates.setXYZ(list.at(0), list.at(1), list.at(2));
		return localStaticLimitStates;
	}
	
	localStaticLimitStates.setXYZ(0, 0, 0);
	return localStaticLimitStates;
}
///////////////////////////////////////////////////////////////////
bool CncControl::validateAppAgainstCtlPosition() {
///////////////////////////////////////////////////////////////////
	CncLongPosition ctlPos = requestControllerPos();
	return ( curAppPos.isEqual(ctlPos, CncLongPosition::CMP_BASE) );
	
	#warning watermaks not in all cases identically
	//return ( curAppPos == ctlPos );
}
///////////////////////////////////////////////////////////////////
bool CncControl::enableStepperMotors(bool s) {
///////////////////////////////////////////////////////////////////
	if ( isInterrupted() )
		return false;
		
	if ( isConnected() == false )
		return false;
	
	bool ret = processSetter(PID_ENABLE_STEPPERS, (int32_t)s);
	if ( ret == false )
	{
		CNC_CERR_FUNCT_A("Error while enabling stepper motors. State= %d", s)
		THE_APP->GetMiMotorEnableState()->Check(false);
		return false;
	}
	
	std::vector<int32_t> list;
	getSerial()->processGetter(PID_ENABLE_STEPPERS, list);
		
	if ( list.size() != 1 )
	{
		CNC_CERR_FUNCT_A("Get Enable State: Unable to process getter!")
		THE_APP->GetMiMotorEnableState()->Check(false);
		return false;
	}
		
	THE_APP->GetMiMotorEnableState()->Check( list.at(0) == (int32_t)ENABLE_STATE_ON );
		
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::enableProbeMode(bool s) {
///////////////////////////////////////////////////////////////////
	if ( isInterrupted() )
		return false;
		
	if ( isConnected() == false )
		return false;
	
	bool ret = processSetter(PID_PROBE_MODE, (int32_t)s);
	if ( ret == false )
	{
		CNC_CERR_FUNCT_A("Error while enabling probe mode. State=%s", s)
		return false;
	}
	
	THE_APP->GetBtProbeMode()->SetValue(s);
	
	return true;
}
///////////////////////////////////////////////////////////////////
wxString& CncControl::getLimitInfoString(wxString& ret) {
///////////////////////////////////////////////////////////////////
	return limitStates.getLimitInfoString(ret);
}
///////////////////////////////////////////////////////////////////
void CncControl::evaluateLimitState() {
///////////////////////////////////////////////////////////////////
	CncLongPosition ls = requestControllerLimitState();
	CncInterface::ILS::States states(ls.getX(), ls.getY(), ls.getZ());
	
	displayLimitStates(states);
}
///////////////////////////////////////////////////////////////////
void CncControl::displayLimitStates(const int32_t x, const int32_t y, const int32_t z) {
///////////////////////////////////////////////////////////////////
	CncInterface::ILS::States ls(x, y, z);
	displayLimitStates(ls);
}
///////////////////////////////////////////////////////////////////
void CncControl::displayLimitStates(const CncInterface::ILS::States& ls) {
///////////////////////////////////////////////////////////////////
	if ( ls.hasLimits() )
	{
		ContextInterface* ci = getSerial()->getContextInterface();
		if ( ci != NULL )
			ci->notifyLimit(ls);
	}

	limitStates.setXMinLimit(ls.xMin());
	limitStates.setXMaxLimit(ls.xMax());
	limitStates.setYMinLimit(ls.yMin());
	limitStates.setYMaxLimit(ls.yMax());
	limitStates.setZMinLimit(ls.zMin());
	limitStates.setZMaxLimit(ls.zMax());
	
	displayLimitState(THE_APP->GetXMinLimit(), limitStates.getXMinLimit());
	displayLimitState(THE_APP->GetXMaxLimit(), limitStates.getXMaxLimit());
	displayLimitState(THE_APP->GetYMinLimit(), limitStates.getYMinLimit());
	displayLimitState(THE_APP->GetYMaxLimit(), limitStates.getYMaxLimit());
	displayLimitState(THE_APP->GetZMinLimit(), limitStates.getZMinLimit());
	displayLimitState(THE_APP->GetZMaxLimit(), limitStates.getZMaxLimit());
	
	// To avoid traces in heartbeat frequency, because in other power states
	// the limit information is always wrong and the power state i reported separately
	if ( ctrlPowerState == CPS_ON )
		limitStates.traceLimitInfo();
}
///////////////////////////////////////////////////////////////////
void CncControl::displayLimitState(wxWindow* ctl, bool value) {
///////////////////////////////////////////////////////////////////
	if ( ctl != NULL )
	{
		if ( value == true )
		{
			ctl->SetBackgroundColour(wxColour(255,128,128));
			ctl->SetForegroundColour(*wxWHITE);
		}
		else
		{
			ctl->SetBackgroundColour(wxColour(181,230,29));
			ctl->SetForegroundColour(*wxBLACK);
		}
		
		ctl->Refresh();
		ctl->Update();
	}
}
///////////////////////////////////////////////////////////////////
void CncControl::displaySupportStates(const CncInterface::ISP::States& sp) {
///////////////////////////////////////////////////////////////////
	if ( THE_APP != NULL ) 
	{
		THE_APP->GetSupportButton1State()->SetBackgroundColour(sp.isSupportButton1Pressed() ? wxColour(181,230,29) : wxColour(255,128,128));
		THE_APP->GetSupportButton2State()->SetBackgroundColour(sp.isSupportButton2Pressed() ? wxColour(181,230,29) : wxColour(255,128,128));
		THE_APP->GetSupportButton3State()->SetBackgroundColour(sp.isSupportButton3Pressed() ? wxColour(181,230,29) : wxColour(255,128,128));
		
		THE_APP->GetCableConnectedState()->SetBackgroundColour(sp.isCableConnected() ? wxColour(217,217,0) : wxColour(255,128,128));
		THE_APP->GetToolPowerObserverState()->SetBackgroundColour(sp.isToolPowered() ? wxColour(217,217,0) : wxColour(255,128,128));
	}
}
///////////////////////////////////////////////////////////////////
void CncControl::displayUnknownSupportStates() {
///////////////////////////////////////////////////////////////////
	if ( THE_APP != NULL )
	{
		THE_APP->GetSupportButton1State()->SetBackgroundColour(wxColour(128,128,128));
		THE_APP->GetSupportButton2State()->SetBackgroundColour(wxColour(128,128,128));
		THE_APP->GetSupportButton3State()->SetBackgroundColour(wxColour(128,128,128));
		
		THE_APP->GetCableConnectedState()->SetBackgroundColour(wxColour(128,128,128));
		THE_APP->GetToolPowerObserverState()->SetBackgroundColour(wxColour(128,128,128));
	}
}
///////////////////////////////////////////////////////////////////
void CncControl::displayHealtyStates() {
///////////////////////////////////////////////////////////////////
	if ( THE_APP != NULL )
		THE_APP->setControllerPowerStateBmp(ctrlPowerState == CPS_ON);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveXToMinLimit() {
///////////////////////////////////////////////////////////////////
	const bool b1	= THE_BOUNDS->isValid();
	const bool b2	= THE_BOUNDS->getHardwareOffset().isValid();
	double distance	= 0.0;
	bool ret		= false;
	
	if ( b1 && b2 )
	{
		// find min by using the physically boundary information
		const CncDoublePosition tPhy = { 0.0, 0.0, 0.0 };
		const CncDoublePosition tLog = THE_BOUNDS->getHardwareOffset().transPhy2Log(tPhy);
		distance = tLog.getX() - getCurCtlPosMetricX();
	}
	else
	{
		// try to find min by touching the end stops
		distance = -THE_BOUNDS->getMaxDimensionMetricX() - getCurCtlPosMetricX();
	}
	
	InitializeMove im(this);
	ret = moveRelLinearMetricXY(distance, 0.0, true);
	
	if ( ret == false && limitStates.hasLimit() )
		ret = resolveLimits(true, false, false);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveXToMaxLimit() {
///////////////////////////////////////////////////////////////////
	const bool b1	= THE_BOUNDS->isValid();
	const bool b2	= THE_BOUNDS->getHardwareOffset().isValid();
	double distance	= 0.0;
	bool ret		= false;
	
	if ( b1 && b2 )
	{
		// find max by using the physically boundary information
		const CncDoublePosition tPhy = { THE_BOUNDS->getMaxDimensionMetricX(), 0.0, 0.0 };
		const CncDoublePosition tLog = THE_BOUNDS->getHardwareOffset().transPhy2Log(tPhy);
		distance = tLog.getX() - getCurCtlPosMetricX();
	}
	else
	{
		// try to find max by touching the end stops
		distance = +THE_BOUNDS->getMaxDimensionMetricX() - getCurCtlPosMetricX();
	}
	
	InitializeMove im(this);
	ret = moveRelLinearMetricXY(distance, 0.0, true);
	if ( ret == false && limitStates.hasLimit() )
		ret = resolveLimits(true, false, false);

	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveYToMinLimit() {
///////////////////////////////////////////////////////////////////
	const bool b1	= THE_BOUNDS->isValid();
	const bool b2	= THE_BOUNDS->getHardwareOffset().isValid();
	double distance	= 0.0;
	bool ret		= false;
	
	if ( b1 && b2 )
	{
		// find min by using the physically boundary information
		const CncDoublePosition tPhy = { 0.0, 0.0, 0.0 };
		const CncDoublePosition tLog = THE_BOUNDS->getHardwareOffset().transPhy2Log(tPhy);
		distance = tLog.getY() - getCurCtlPosMetricY();
	}
	else
	{
		// try to find min by touching the end stops
		distance = -THE_BOUNDS->getMaxDimensionMetricY() - getCurCtlPosMetricY();
	}
	
	InitializeMove im(this);
	ret = moveRelLinearMetricXY(0.0, distance, true);
	if ( ret == false && limitStates.hasLimit() )
		ret = resolveLimits(false, true, false);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveYToMaxLimit() {
///////////////////////////////////////////////////////////////////
	const bool b1	= THE_BOUNDS->isValid();
	const bool b2	= THE_BOUNDS->getHardwareOffset().isValid();
	double distance	= 0.0;
	bool ret		= false;
	
	if ( b1 && b2 )
	{
		// find max by using the physically boundary information
		const CncDoublePosition tPhy = { 0.0, THE_BOUNDS->getMaxDimensionMetricY(), 0.0 };
		const CncDoublePosition tLog = THE_BOUNDS->getHardwareOffset().transPhy2Log(tPhy);
		distance = tLog.getY() - getCurCtlPosMetricY();
	}
	else
	{
		// try to find max by touching the end stops
		distance = +THE_BOUNDS->getMaxDimensionMetricY() - getCurCtlPosMetricY();
	}
	
	InitializeMove im(this);
	ret = moveRelLinearMetricXY(0.0, distance, true);
	if ( ret == false && limitStates.hasLimit() )
		ret = resolveLimits(false, true, false);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveZToMinLimit() {
///////////////////////////////////////////////////////////////////
	const bool b1	= THE_BOUNDS->isValid();
	const bool b2	= THE_BOUNDS->getHardwareOffset().isValid();
	double distance	= 0.0;
	bool ret		= false;
	
	if ( b1 && b2 )
	{
		// find min by using the physically boundary information
		const CncDoublePosition tPhy = { 0.0, 0.0, -THE_BOUNDS->getMaxDimensionMetricZ() };
		const CncDoublePosition tLog = THE_BOUNDS->getHardwareOffset().transPhy2Log(tPhy);
		distance = tLog.getZ() - getCurCtlPosMetricZ();
	}
	else
	{
		// try to find min by touching the end stops
		distance = -THE_BOUNDS->getMaxDimensionMetricZ() - getCurCtlPosMetricZ();
	}
	
	InitializeMove im(this);
	ret = moveRelMetricZ(distance);
	
	if ( ret == false && limitStates.hasLimit() )
		ret = resolveLimits(false, false, true);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveZToMaxLimit() {
///////////////////////////////////////////////////////////////////
	const bool b1	= THE_BOUNDS->isValid();
	const bool b2	= THE_BOUNDS->getHardwareOffset().isValid();
	double distance	= 0.0;
	bool ret		= false;
	
	if ( b1 && b2 )
	{
		// find max by using the physically boundary information
		const CncDoublePosition tPhy = { 0.0, 0.0, 0.0 };
		const CncDoublePosition tLog = THE_BOUNDS->getHardwareOffset().transPhy2Log(tPhy);
		distance = tLog.getZ() - getCurCtlPosMetricZ();
	}
	else
	{
		// try to find max by touching the end stops
		distance = +THE_BOUNDS->getMaxDimensionMetricZ() - getCurCtlPosMetricZ();
	}
	
	InitializeMove im(this);
	ret = moveRelMetricZ(distance);
	if ( ret == false && limitStates.hasLimit() )
		ret = resolveLimits(false, false, true);
		
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveXToMid() {
///////////////////////////////////////////////////////////////////
	const bool b1	= THE_BOUNDS->isValid();
	const bool b2	= THE_BOUNDS->getHardwareOffset().isValid();
	double distance	= 0.0;
	bool ret		= false;
	
	if ( b1 && b2 )
	{
		// find mid by using the physically boundary information
		const CncDoublePosition tPhy = { THE_BOUNDS->getMaxDimensionMetricX() / 2.0, 0.0, 0.0 };
		const CncDoublePosition tLog = THE_BOUNDS->getHardwareOffset().transPhy2Log(tPhy);
		distance = tLog.getX() - getCurCtlPosMetricX();
	}
	else
	{
		// try to find mid by touching the end stops
		distance = 5.0 + THE_BOUNDS->getMaxDimensionMetricX() - getCurCtlPosMetricX();
	}
	
	InitializeMove im(this);
	ret = moveRelLinearMetricXY(distance, 0.0, true);
	if ( ret == false && limitStates.hasLimit() )
		ret = moveRelLinearMetricXY(-THE_BOUNDS->getMaxDimensionMetricX() / 2, 0.0, true);
		
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveYToMid() {
///////////////////////////////////////////////////////////////////
	const bool b1	= THE_BOUNDS->isValid();
	const bool b2	= THE_BOUNDS->getHardwareOffset().isValid();
	double distance	= 0.0;
	bool ret		= false;
	
	if ( b1 && b2 )
	{
		// find mid by using the physically boundary information
		const CncDoublePosition tPhy = { 0.0, THE_BOUNDS->getMaxDimensionMetricY() / 2.0, 0.0 };
		const CncDoublePosition tLog = THE_BOUNDS->getHardwareOffset().transPhy2Log(tPhy);
		distance = tLog.getY() - getCurCtlPosMetricY();
	}
	else
	{
		// try to find mid by touching the end stops
		distance = 5.0 + THE_BOUNDS->getMaxDimensionMetricY() - getCurCtlPosMetricY();
	}
	
	InitializeMove im(this);
	ret = moveRelLinearMetricXY(0.0, distance, true);
	if ( ret == false && limitStates.hasLimit() )
		ret = moveRelLinearMetricXY(0.0, -THE_BOUNDS->getMaxDimensionMetricY() / 2, true);
		
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveZToMid() {
///////////////////////////////////////////////////////////////////
	const bool b1	= THE_BOUNDS->isValid();
	const bool b2	= THE_BOUNDS->getHardwareOffset().isValid();
	double distance	= 0.0;
	bool ret		= false;
	
	if ( b1 && b2 )
	{
		// find mid by using the physically boundary information
		const CncDoublePosition tPhy = { 0.0, 0.0, -THE_BOUNDS->getMaxDimensionMetricZ() / 2.0 };
		const CncDoublePosition tLog = THE_BOUNDS->getHardwareOffset().transPhy2Log(tPhy);
		distance = tLog.getZ() - getCurCtlPosMetricZ();
	}
	else
	{
		// try to find mid by touching the end stops
		distance = 5.0 + THE_BOUNDS->getMaxDimensionMetricZ() - getCurCtlPosMetricZ();
	}
	
	InitializeMove im(this);
	ret = moveRelMetricZ(distance);
	if ( ret == false && limitStates.hasLimit() )
		ret = moveRelMetricZ(-THE_BOUNDS->getMaxDimensionMetricZ() / 2);
		
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveZToTop() {
///////////////////////////////////////////////////////////////////
	const double topZPos = 0.0 + THE_CONFIG->getWorkpieceOffset();
	const double curZPos = getCurAppPosMetricZ();
	const double moveZ   = topZPos - curZPos;
	bool ret = false;
	
	InitializeMove im(this, false);
	if ( (ret = moveRelMetricZ(moveZ)) == false )
		CNC_CERR_FUNCT_A("CncControl: Move Z to top error")
		
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveToZeroZAxisAnchor() {
///////////////////////////////////////////////////////////////////
	if ( isInterrupted() )
		return false;
		
	const CncLongPosition	p1(getCurCtlPos());
	const float				s1   = getConfiguredFeedSpeed_MM_MIN();
	const CncSpeedMode		sm   = getConfiguredSpeedMode();
	const CncDoublePosition tPhy = THE_CONTEXT->anchorMap->getPhysicalAnchorZTouch();
	
	if ( moveToPhysicalPos(tPhy, Z) == false )
	{
		CNC_CERR_FUNCT_A(" moveToPhysicalPos(Z) failed")
		return false;
	}
	
	// switch spindle off
	switchSpindleOff(true);
	if ( evaluateSpindlePowerState() != SPINDLE_STATE_OFF )
	{
		CNC_CERR_FUNCT_A(" Can't switch of the spindle! Abort!")
		return false;
	}
	
	if ( moveToPhysicalPos(tPhy, XY) == false )
	{
		CNC_CERR_FUNCT_A(" moveToPhysicalPos(Z) failed")
		return false;
	}

	// interactive = true only for a real serial port 
	// with physically Z touch functionality
	const bool interactive = THE_CONTEXT->hasHardware() == true && serialPort->isEmulator() == false;
	const float st         = 500.0;
	
	// reduce feed speed for levelling the z axis
	if ( changeCurrentFeedSpeedXYZ_MM_MIN(st, CncSpeedRapid) == false )
	{
		CNC_CERR_FUNCT_A(" changeCurrentFeedSpeedXYZ_MM_MIN(%f) failed!", st)
		return false;
	}
	
	if ( interactive == true  )
	{
		// For all real physical serial ports
		
		
		// warning TODO: Move Z axis down until it touches the switch
		
	}
	else
	{
		// For all emulators - also the arduino emulator too
		// simulate z touch by moving again to the current Z zero position
		if ( moveAbsMetricZ(0.0) == false )
		{
			CNC_CERR_FUNCT_A("CncControl: Move Z to top error")
			return false;
		}
		
		// change speed back to the initial values
		if ( changeCurrentFeedSpeedXYZ_MM_MIN(s1, sm) == false )
		{
			CNC_CERR_FUNCT_A(" changeCurrentFeedSpeedXYZ_MM_MIN(%f) failed!", s1)
			return false;
		}
			
		// move Z axis back to logged start position
		if ( moveToLogicalPos(p1, Z) == false )
		{
			CNC_CERR_FUNCT_A(" finally moveToLogicalPos(Z) failed!", s1)
			return false;
		}
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveToToolChangeAnchor() {
///////////////////////////////////////////////////////////////////
	if ( isInterrupted() )
		return false;
		
	const CncDoublePosition tPhy = THE_CONTEXT->anchorMap->getPhysicalAnchorToolChange();
	return moveToPhysicalPos(tPhy, Z_XY);
}
///////////////////////////////////////////////////////////////////
bool CncControl::interactToolChange(int toolId) {
///////////////////////////////////////////////////////////////////
	CNC_CEX1_A("Processing manually tool change . . .")
	TmpLoggerIndent tli;
	
	// first log the current position, speed ans spindle state
	const CncLongPosition	p1(getCurCtlPos());
	const float				s1 = getConfiguredFeedSpeed_MM_MIN();
	const CncSpeedMode		sm = getConfiguredSpeedMode();
	CncSpindlePowerState	sp = getSpindlePowerState();
	
	// move Z axis up to work piece offset to be out of the work piece 
	// while asking for the tool change options below
	const CncDoublePosition pwo(0.0, 0.0, THE_CONFIG->getWorkpieceOffset() );
	if ( moveToLogicalPos(pwo, Z) == false )
	{
		CNC_CERR_FUNCT_A(" moveToLogicalPos(Z) to work piece offset failed!")
		return false;
	}
	
	const bool interactive = THE_CONTEXT->hasHardware() == true;
	if ( interactive == true )
	{
		const wxString msg(wxString::Format("Manually Tool change to %d\n%s\n\n"
											" Click Yes if the tool change should happen.\n" 
											" Click No if the current tool should continue exists\n"
											" Click Cancel if the curren t run should be aborted"
											, toolId, THE_CONFIG->getToolParamAsInfoStr(toolId)));
		
		wxRichMessageDialog dlg(THE_APP, msg, _T("Manually Tool Change"), 
							wxYES|wxNO|wxCANCEL|wxCENTRE|wxICON_INFORMATION);
		
		dlg.SetFooterText("Decide what will happen . . .");
		dlg.SetFooterIcon(wxICON_WARNING);
		
		const int ret = dlg.ShowModal();
		
		switch ( ret )
		{
			case wxID_YES:		CNC_CEX1_A("Manually tool change was finalized, template runs further with new tool setup.")
								break;
								
			case wxID_NO:		CNC_CEX1_A("Manually tool change was ignored, template runs further with previous tool setup.")
								return moveToLogicalPos(p1, Z);
								
			case wxID_CANCEL:	CNC_CEX1_A("Manually tool change was aborted, template run will be also aborted")
								return false;
								
			default:			CNC_CERR_FUNCT_A(" Unhandelt return value!")
								return false;
		}
	}
	
	const wxString msg(wxString::Format("Initialize tool change to tool id: %d\n %s", toolId, THE_CONFIG->getToolParamAsInfoStr(toolId)));
	CNC_CEX1_A(msg);
	
	// ------------------------------------------------------------------
	// tool change procedure starts from here
	
	// setup a fast movement speed to navigate to the tool change position 
	const float st = 5500.0;
	if ( changeCurrentFeedSpeedXYZ_MM_MIN(st, CncSpeedRapid) == false )
	{
		CNC_CERR_FUNCT_A(" changeCurrentFeedSpeedXYZ_MM_MIN(%f) failed!", st)
		return false;
	}
	
	// move to the predefined tool change anchor to have a good position 
	// to procedure  the manually tool change 
	if ( moveToToolChangeAnchor() == false )
	{
		CNC_CERR_FUNCT_A(" moveToToolChangeAnchor() failed!")
		return false;
	}
	
	// switch spindle off
	switchSpindleOff(true);
	if ( evaluateSpindlePowerState() != SPINDLE_STATE_OFF )
	{
		CNC_CERR_FUNCT_A(" Can't switch of the spindle! Abort!")
		return false;
	}
	
	// ask for finalisation
	if ( interactive == true )
	{
		const wxString msg(wxString::Format("Now manually replace the current tool to tool id: %d\n\n%s\n\nand click OK", 
											toolId, THE_CONFIG->getToolParamAsInfoStr(toolId)));
											
		wxRichMessageDialog dlg(THE_APP, msg, _T("Manually Tool Change"), 
							wxOK|wxCANCEL|wxCENTRE|wxICON_INFORMATION);
		
		dlg.SetFooterText("Waiting for completion . . .");
		dlg.SetFooterIcon(wxICON_WARNING);
		
		if ( dlg.ShowModal() == wxID_CANCEL )
		{
			CNC_COUT_A("Manually tool change was aborted, template run will be also aborted")
			return false;
		}
	}
	
	CNC_CEX1_A(wxString::Format("Manually tool change finalized to tool id: %d", toolId));
	
	// now level the z axis again
	if ( moveToZeroZAxisAnchor() == false )
	{
		CNC_CERR_FUNCT_A(" moveToZeroZAxisAnchor() failed!")
		return false;
	}
	
	// move back to logged start position
	if ( moveToLogicalPos(p1, XY) == false )
	{
		CNC_CERR_FUNCT_A(" moveToLogicalPos(XY) failed!")
		return false;
	}
	
	switchSpindleState(sp, true);

	// move back to logged start position
	if ( moveToLogicalPos(p1, Z) == false )
	{
		CNC_CERR_FUNCT_A(" moveToLogicalPos(Z) failed!")
		return false;
	}
	
	// and reconfigure the speed
	if ( changeCurrentFeedSpeedXYZ_MM_MIN(s1, sm) == false )
	{
		CNC_CERR_FUNCT_A(" changeCurrentFeedSpeedXYZ_MM_MIN(%f) failed!", s1)
		return false;
	}

	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveToPhysicalPos(const CncDoublePosition& pPhy, CncMoveOrder mo) {
///////////////////////////////////////////////////////////////////
	const bool b1 = THE_BOUNDS->isValid();
	const bool b2 = THE_BOUNDS->getHardwareOffset().isValid();
	
	if ( !b1 || !b2 )
	{
		CNC_CERR_FUNCT_A(" Boundaries are invalid!")
		return false;
	}
	
	CncDoublePosition pLog(THE_BOUNDS->getHardwareOffset().transPhy2Log(pPhy));
	return moveToLogicalPos(pLog, mo);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveToLogicalPos(const CncLongPosition& pLog, CncMoveOrder mo) {
///////////////////////////////////////////////////////////////////
	InitializeMove im(this);
	
	#define PCERR	CNC_CERR_FUNCT_A
	
	bool ret = false;
	const int32_t dx = pLog.getX() - getCurCtlPos().getX();
	const int32_t dy = pLog.getY() - getCurCtlPos().getY();
	const int32_t dz = pLog.getZ() - getCurCtlPos().getZ();
	
	switch ( mo )
	{
		case XYZ:
		{
			if ( ( ret = moveAbsLinearStepsXYZ(pLog.getX(), pLog.getY(), pLog.getZ(), true) ) == false )
				PCERR(" moveAbsLinearStepsXYZ() failed!"); 
			break;
		}
		case XY:
		{
			if ( ( ret = moveRelLinearStepsXY(dx, dy, true) ) == false )
				PCERR(" moveAbsLinearStepsXY() failed!"); 
			break;
		}
		case X:
		{
			if ( ( ret = moveRelStepsX(dx) ) == false ) 
				PCERR(" moveRelStepsX() failed!");
			break;
		}
		case Y:
		{
			if ( ( ret = moveRelStepsY(dy) ) == false ) 
				PCERR(" moveRelStepsY() failed!");
			break;
		}
		case Z:
		{
			if ( ( ret = moveRelStepsZ(dz) ) == false ) 
				PCERR(" moveRelStepsZ() failed!");
			break;
		}
		case X_Y_Z:
		{
			if ( ( ret = moveRelStepsX(dx) ) == false ) { PCERR(" moveRelStepsX() failed!"); break; }
			if ( ( ret = moveRelStepsY(dy) ) == false ) { PCERR(" moveRelStepsY() failed!"); break; }
			if ( ( ret = moveRelStepsZ(dz) ) == false ) { PCERR(" moveRelStepsZ() failed!"); break; }
			break;
		}
		case Z_X_Y:
		{
			if ( ( ret = moveRelStepsZ(dz) ) == false ) { PCERR(" moveRelStepsZ() failed!"); break; }
			if ( ( ret = moveRelStepsX(dx) ) == false ) { PCERR(" moveRelStepsX() failed!"); break; }
			if ( ( ret = moveRelStepsY(dy) ) == false ) { PCERR(" moveRelStepsY() failed!"); break; }
			break;
		}
		case Y_X_Z:
		{
			if ( ( ret = moveRelStepsY(dy) ) == false ) { PCERR(" moveRelStepsY() failed!"); break; }
			if ( ( ret = moveRelStepsX(dx) ) == false ) { PCERR(" moveRelStepsX() failed!"); break; }
			if ( ( ret = moveRelStepsZ(dz) ) == false ) { PCERR(" moveRelStepsZ() failed!"); break; }
			break;
		}
		case XY_Z:
		{
			if ( ( ret = moveRelLinearStepsXY(dx, dy, true) ) == false )
				{ PCERR(" moveAbsLinearStepsXYZ() failed!"); break; }
				
			if ( ( ret = moveRelStepsZ(dz) ) == false ) 
				{ PCERR(" moveRelStepsZ() failed!"); break; }
				
			break;
		}
		case Z_XY:
		{
			if ( ( ret = moveRelStepsZ(dz) ) == false ) 
				{ PCERR(" moveRelStepsZ() failed!"); break; }
				
			if ( ( ret = moveRelLinearStepsXY(dx, dy, true) ) == false )
				{ PCERR(" moveAbsLinearStepsXY() failed!"); break; }
				
			break;
		}
	}
	
	#undef PCERR
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveToLogicalPos(const CncDoublePosition& p, CncMoveOrder mo) {
///////////////////////////////////////////////////////////////////
	CncLongPosition pLong;
	return moveToLogicalPos(THE_CONFIG->convertMetricToSteps(pLong, p), mo);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveToPhysicalPos(const CncLongPosition& pPhy, CncMoveOrder mo) {
///////////////////////////////////////////////////////////////////
	const bool b1 = THE_BOUNDS->isValid();
	const bool b2 = THE_BOUNDS->getHardwareOffset().isValid();
	
	if ( !b1 || !b2 )
	{
		CNC_CERR_FUNCT_A(" Boundaries are invalid!")
		return false;
	}
	
	CncLongPosition pLog(THE_BOUNDS->getHardwareOffset().transPhy2Log(pPhy));
	return moveToLogicalPos(pLog, mo);
}
///////////////////////////////////////////////////////////////////
void CncControl::previewConfiguredFeedSpeed_MM_MIN(CncSpeedMode m, float v) {
///////////////////////////////////////////////////////////////////
	configuredFeedSpeed_MM_MIN		= v;
	configuredSpeedMode				= m;
	
	realtimeFeedSpeed_MM_MIN		= v;
	
	configuredSpeedModePreviewFlag	= true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::startInteractiveMove(CncStepSensitivity s, CncInteractiveMoveDriver imd ) {
///////////////////////////////////////////////////////////////////
	CNC_PRINT_INTERACTIVE_FUNCT_A("Begin\n");
	if ( isInteractiveMoveActive() )
		return true;
		
	const double newSpeed = cnc::getSpeedValue(s);
	changeCurrentFeedSpeedXYZ_MM_MIN(newSpeed, CncSpeedRapid);
	
	currentInteractiveMoveInfo.stepSensitivity = s;
	
	const bool b = serialPort->processStartInteractiveMove();
	currentInteractiveMoveInfo.driver = (b ? imd : IMD_NONE);
	
	CNC_PRINT_INTERACTIVE_FUNCT_A("End\n");
	return isInteractiveMoveActive();
}
///////////////////////////////////////////////////////////////////
bool CncControl::updateInteractiveMove(const CncLinearDirection x, const CncLinearDirection y, const CncLinearDirection z, int modifySpeed) {
///////////////////////////////////////////////////////////////////
	CNC_PRINT_INTERACTIVE_FUNCT_A("Begin\n");
	if ( isInteractiveMoveActive() == false )
		return false;
		
	const bool ret = serialPort->processUpdateInteractiveMove(x, y, z, modifySpeed);
	if ( ret == false )
	{
		CNC_CERR_FUNCT_A("processUpdateInteractiveMove failed");
		stopInteractiveMove();
	}
	
	CNC_PRINT_INTERACTIVE_FUNCT_A("End\n");
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::updateInteractiveMove() {
///////////////////////////////////////////////////////////////////
	CNC_PRINT_INTERACTIVE_FUNCT_A("Begin\n");
	if ( isInteractiveMoveActive() == false )
		return false;
		
	const bool ret = serialPort->processUpdateInteractiveMove();
	if ( ret == false )
	{
		CNC_CERR_FUNCT_A("processUpdateInteractiveMove failed");
		stopInteractiveMove();
	}
	
	CNC_PRINT_INTERACTIVE_FUNCT_A("End\n");
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::stopInteractiveMove() {
///////////////////////////////////////////////////////////////////
	CNC_PRINT_INTERACTIVE_FUNCT_A("Begin\n");
	
	if ( isInteractiveMoveActive() )
	{
		if ( getSerial()->sendQuitMove() == false )
		{
			CNC_CERR_FUNCT_A("sendQuitMove() failed");
			return false;
		}
		
		// do this immediately after sending the quit signal
		currentInteractiveMoveInfo.reset();
		
		// Don't do this here because it leads to a start asynchronous behaviour
		// of interactive commands 
		/*
			// read left information on demand
			const bool dispatchUserEvents = false;
			if ( getSerial()->isCommandActive() == false )
				popSerial(dispatchUserEvents);
		*/
	}
	
	CNC_PRINT_INTERACTIVE_FUNCT_A("End\n");
	
	// synchronize the app position
	curAppPos = curCtlPos;
	
	return ( isInteractiveMoveActive() == false );
}
///////////////////////////////////////////////////////////////////
bool CncControl::resolveLimits(bool x, bool y, bool z) {
///////////////////////////////////////////////////////////////////
	const unsigned int size = 3;
	static int32_t values[size];
	values[0] = (int32_t)x;
	values[1] = (int32_t)y;
	values[2] = (int32_t)z;
	
	return serialPort->resolveLimits(size, values);
}
///////////////////////////////////////////////////////////////////
bool CncControl::correctLimitPositions() {
///////////////////////////////////////////////////////////////////
	bool ret = true;
	
	if ( limitStates.hasLimit() )
	{
		const bool x = limitStates.getXMinLimit() || limitStates.getXMaxLimit();
		const bool y = limitStates.getYMinLimit() || limitStates.getYMaxLimit();
		const bool z = limitStates.getZMinLimit() || limitStates.getZMaxLimit();
		
		ret = resolveLimits(x, y, z);
		
		// adjust position on demand
		if ( validateAppAgainstCtlPosition() == false )
			curAppPos = requestControllerPos();
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::convertPositionToHardwareCoordinate(const CncLongPosition& pos, CncLongPosition& ret) {
///////////////////////////////////////////////////////////////////
	if ( THE_BOUNDS->getHardwareOffset().isValid() == false )
	{
		ret = pos;
		return false;
	}
	
	ret.setX(abs(THE_BOUNDS->getHardwareOffset().getAsStepsX()) + pos.getX());
	ret.setY(abs(THE_BOUNDS->getHardwareOffset().getAsStepsY()) + pos.getY());
	ret.setZ(abs(THE_BOUNDS->getHardwareOffset().getAsStepsZ()) + pos.getZ());
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::convertPositionToHardwareCoordinate(const CncDoublePosition& pos, CncDoublePosition& ret) {
///////////////////////////////////////////////////////////////////
	CncLongPosition sPos = THE_CONFIG->convertMetricToSteps(sPos, pos);
	CncLongPosition sRet;
	
	const bool b = convertPositionToHardwareCoordinate(sPos, sRet);
	
	ret = THE_CONFIG->convertStepsToMetric(ret, sRet);
	
	return b;
}
///////////////////////////////////////////////////////////////////
bool CncControl::evaluateHardwareReference() {
///////////////////////////////////////////////////////////////////
	const float				prevSpeed 	= getConfiguredFeedSpeed_MM_MIN();
	const CncLongPosition	prevCtlPos	= curCtlPos;
	
	// ------------------------------------------------------------
	auto returnFalse = [&](const wxString& msg)
	{
		std::cerr << CNC_LOG_FUNCT << ": " << msg << std::endl;
		return false;
	};
	
	// ------------------------------------------------------------
	bool ret = true;
	
	ret = changeCurrentFeedSpeedXYZ_MM_MIN(1500, CncSpeedUserDefined);
	if ( ret == false ) { return returnFalse("Error while changeCurrentFeedSpeedXYZ_MM_MIN()"); }

	// move to Zmax, Xmin and Ymin
	{
		ret = moveZToMaxLimit();
		if ( ret == false || limitStates.hasLimit() == true ) { return returnFalse("Error while moveZToMaxLimit()"); }
		
		ret = moveXToMinLimit();
		if ( ret == false || limitStates.hasLimit() == true ) { return returnFalse("Error while moveXToMinLimit()"); }
		
		ret = moveYToMinLimit();
		if ( ret == false || limitStates.hasLimit() == true ) { return returnFalse("Error while moveYToMinLimit()"); }
	}
	
	THE_BOUNDS->setHardwareOffset(curCtlPos);
	THE_BOUNDS->setHardwareOffsetValid(true); 
	
	ret = changeCurrentFeedSpeedXYZ_MM_MIN(4500, CncSpeedUserDefined);
	if ( ret == false ) { return returnFalse("Error while changeCurrentFeedSpeedXYZ_MM_MIN()"); }
	
	// move to previous positions
	moveRelLinearStepsXY(abs(prevCtlPos.getX() - curCtlPos.getX()), abs(prevCtlPos.getY() - curCtlPos.getY()), false);
	moveRelStepsZ(-abs(prevCtlPos.getZ() - curCtlPos.getZ()));

	changeCurrentFeedSpeedXYZ_MM_MIN(prevSpeed, CncSpeedUserDefined);
	if ( ret == false ) { return returnFalse("Error while resetting feed speed"); }
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::evaluateHardwareDimensionsZAxis(DimensionZAxis& result) {
///////////////////////////////////////////////////////////////////
	const float				prevSpeed 	= getConfiguredFeedSpeed_MM_MIN();
	const CncLongPosition	prevCtlPos	= curCtlPos;
	
	// ------------------------------------------------------------
	auto returnFalse = [&](const wxString& msg)
	{
		std::cerr << CNC_LOG_FUNCT << ": " << msg << std::endl;
		
		result.dimensionZ = 0.0;
		result.Details.pMin = prevCtlPos;
		result.Details.pMax = prevCtlPos;
		return false;
	};
	
	// ------------------------------------------------------------
	bool ret = true;
	result.Details.pMin = curCtlPos;
	result.Details.pMax = curCtlPos;
	
	ret = changeCurrentFeedSpeedXYZ_MM_MIN(5000, CncSpeedUserDefined);
	if ( ret == false ) { return returnFalse("Error while changeCurrentFeedSpeedXYZ_MM_MIN()"); }
	
	ret = correctLimitPositions();
	if ( ret == false ) { return returnFalse("Error while correctLimitPositions()"); }
	
	ret = moveZToMaxLimit(); 
	if ( ret == false ) { return returnFalse("Error while moveZToMaxLimit()"); }
	
	result.Details.pMax = curCtlPos;
	
	ret = moveZToMinLimit(); 
	if ( ret == false ) { return returnFalse("Error while moveZToMaxLimit()"); }
	
	result.Details.pMin = curCtlPos;
	
	result.dimensionZ = THE_CONFIG->convertStepsToMetricZ(result.Details.pMax.getZ() - result.Details.pMin.getZ());
	
	changeCurrentFeedSpeedXYZ_MM_MIN(prevSpeed, CncSpeedUserDefined);
	if ( ret == false ) { return returnFalse("Error while resetting feed speed"); }
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::evaluateHardwareDimensionsXYPlane(DimensionXYPlane& result) {
///////////////////////////////////////////////////////////////////
	const float 			prevSpeed 	= getConfiguredFeedSpeed_MM_MIN();
	const CncLongPosition	prevCtlPos	= curCtlPos;
	
	// ------------------------------------------------------------
	auto returnFalse = [&](const wxString& msg)
	{
		std::cerr << CNC_LOG_FUNCT << ": " << msg << std::endl;
		
		result.dimensionX = 0.0;
		result.dimensionY = 0.0;
		result.Details.p1 = prevCtlPos;
		result.Details.p2 = prevCtlPos;
		result.Details.p3 = prevCtlPos;
		result.Details.p4 = prevCtlPos;
		result.Details.p5 = prevCtlPos;
		return false;
	};
	
	// ------------------------------------------------------------
	bool ret = true;
	result.Details.p1 = curCtlPos;
	result.Details.p2 = curCtlPos;
	result.Details.p3 = curCtlPos;
	result.Details.p4 = curCtlPos;
	result.Details.p5 = curCtlPos;
	
	ret = changeCurrentFeedSpeedXYZ_MM_MIN(5000, CncSpeedUserDefined);
	if ( ret == false ) { return returnFalse("Error while changeCurrentFeedSpeedXYZ_MM_MIN()"); }
	
	ret = correctLimitPositions();
	if ( ret == false ) { return returnFalse("Error while correctLimitPositions()"); }
	
	ret = moveZToMaxLimit(); 
	if ( ret == false ) { return returnFalse("Error while moveZToMaxLimit()"); }
	
	ret = moveXToMinLimit();
	if ( ret == false ) { return returnFalse("Error while moveXToMinLimit()"); }
	
	ret = moveYToMinLimit();
	if ( ret == false ) { return returnFalse("Error while moveYToMinLimit()"); }
	
	result.Details.p1 = curCtlPos;
	
	ret = moveYToMaxLimit();
	if ( ret == false ) { return returnFalse("Error while moveYToMinLimit()"); }
	
	result.Details.p2 = curCtlPos;
	
	ret = moveXToMaxLimit();
	if ( ret == false ) { return returnFalse("Error while moveYToMinLimit()"); }
	
	result.Details.p3 = curCtlPos;
	
	ret = moveYToMinLimit();
	if ( ret == false ) { return returnFalse("Error while moveYToMinLimit()"); }
	
	result.Details.p4 = curCtlPos;
	
	ret = moveXToMinLimit();
	if ( ret == false ) { return returnFalse("Error while moveXToMinLimit()"); }
	
	result.Details.p5 = curCtlPos;
	
	int32_t y1 = result.Details.p2.getY() - result.Details.p1.getY();
	int32_t y2 = result.Details.p3.getY() - result.Details.p4.getY();
	
	int32_t x1 = result.Details.p3.getX() - result.Details.p2.getX();
	int32_t x2 = result.Details.p4.getX() - result.Details.p5.getX();
	
	result.dimensionX = THE_CONFIG->convertStepsToMetricX(std::min(x1, x2));
	result.dimensionY = THE_CONFIG->convertStepsToMetricY(std::min(y1, y2));
	
	changeCurrentFeedSpeedXYZ_MM_MIN(prevSpeed, CncSpeedUserDefined);
	if ( ret == false ) { return returnFalse("Error while resetting feed speed"); }
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::manualSimpleMoveMetric(double x, double y, double z, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	double sX = x * THE_CONFIG->getCalculationFactX();
	double sY = y * THE_CONFIG->getCalculationFactY();
	double sZ = z * THE_CONFIG->getCalculationFactZ();
	
	return manualSimpleMoveSteps((int32_t)round(sX), (int32_t)round(sY), (int32_t)round(sZ), alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool CncControl::manualSimpleMoveMetric3D(double x, double y, double z, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	double sX = x * THE_CONFIG->getCalculationFactX();
	double sY = y * THE_CONFIG->getCalculationFactY();
	double sZ = z * THE_CONFIG->getCalculationFactZ();
	
	return manualSimpleMoveSteps3D((int32_t)round(sX), (int32_t)round(sY), (int32_t)round(sZ), alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool CncControl::manualSimpleMoveSteps(int32_t x, int32_t y, int32_t z, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	bool ret = true;

	if ( x != 0 || y != 0 )
	{
		ret = false;
		if ( prepareSimpleMove(false) == true )
		{
			ret = moveRelLinearStepsXY(x, y, alreadyRendered );
			if ( limitStates.hasLimit() ) {
				
				if ( x != 0 )
				{
					if ( limitStates.isXLimitStateValid() && limitStates.getXMinLimit() )
						ret = resolveLimits(true, false, false);
						
					if ( limitStates.isXLimitStateValid() && limitStates.getXMaxLimit() )
						ret = resolveLimits(true, false, false);
				}
				
				if ( y != 0 )
				{
					if ( limitStates.isYLimitStateValid() && limitStates.getYMinLimit() )
						ret = resolveLimits(false, true, false);
						
					if ( limitStates.isYLimitStateValid() && limitStates.getYMaxLimit() )
						ret = resolveLimits(false, true, false);
				}
			}
				
			reconfigureSimpleMove(ret);
		}
	}
	
	if ( z != 0 ) {
		ret = false;
		if ( prepareSimpleMove(false) == true )
		{
			ret = moveRelStepsZ(z);
			if ( ret && limitStates.hasLimit() )
			{
				
				if ( limitStates.isZLimitStateValid() && limitStates.getZMinLimit() )
					ret = resolveLimits(false, false, true);
					
				if ( limitStates.isZLimitStateValid() && limitStates.getZMaxLimit() )
					ret = resolveLimits(false, false, true);
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
	
	if ( x != 0 || y != 0 || z != 0 )
	{
		ret = false;
		if ( prepareSimpleMove(false) == true )
		{
			ret = moveRelLinearStepsXYZ(x, y, z, alreadyRendered );
			
			if ( limitStates.hasLimit() ) {
				
				if ( x != 0 ) {
					if ( limitStates.isXLimitStateValid() && limitStates.getXMinLimit() )
						ret = resolveLimits(true, false, false);
						
					if ( limitStates.isXLimitStateValid() && limitStates.getXMaxLimit() )
						ret = resolveLimits(true, false, false);
				}
				
				if ( y != 0 ) {
					if ( limitStates.isYLimitStateValid() && limitStates.getYMinLimit() )
						ret = resolveLimits(false, true, false);
						
					if ( limitStates.isYLimitStateValid() && limitStates.getYMaxLimit() )
						ret = resolveLimits(false, true, false);
				}
				
				if ( z != 0 ) {
					if ( limitStates.isZLimitStateValid() && limitStates.getZMinLimit() )
						ret = resolveLimits(false, false, true);
						
					if ( limitStates.isZLimitStateValid() && limitStates.getZMaxLimit() )
						ret = resolveLimits(false, false, true);
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
	initNextDuration();
	THE_CONTEXT->setAllowEventHandling(enaleEventHandling);
	activatePositionCheck(false);
	
	// currently no checks implemented, if checks necessary do it here
	return true;
}
///////////////////////////////////////////////////////////////////
void CncControl::reconfigureSimpleMove(bool correctPositions) {
///////////////////////////////////////////////////////////////////
	activatePositionCheck(true);
	resetDurationCounter();
	evaluateLimitState();
	
	if ( validateAppAgainstCtlPosition() == false && correctPositions == true ) 
		curAppPos = requestControllerPos();
}
///////////////////////////////////////////////////////////////////
void CncControl::clearControllerConfigControl() {
///////////////////////////////////////////////////////////////////
	THE_APP->GetDvListCtrlControllerConfig()->DeleteAllItems();
}
///////////////////////////////////////////////////////////////////
void CncControl::appendPidKeyValueToControllerConfig(int pid, const char* key, const char* value, const char* unit) {
///////////////////////////////////////////////////////////////////
	DcmItemList rows;

	DataControlModel::addNumKeyValueUnitRow(rows, pid, key, value, unit);
	THE_APP->GetDvListCtrlControllerConfig()->Freeze();
	
	for (DcmItemList::iterator it = rows.begin(); it != rows.end(); ++it) 
		THE_APP->GetDvListCtrlControllerConfig()->AppendItem(*it);
	
	THE_APP->GetDvListCtrlControllerConfig()->Thaw();
}
///////////////////////////////////////////////////////////////////
void CncControl::clearControllerPinControl() {
///////////////////////////////////////////////////////////////////
	THE_APP->GetDvListCtrlControllerPins()->DeleteAllItems();
}
///////////////////////////////////////////////////////////////////
void CncControl::appendNumKeyValueToControllerPinInfo(const char* desc, int pin, int type, int mode, int value) {
///////////////////////////////////////////////////////////////////
	DcmItemList rows;

	DataControlModel::addPinReportRow(rows, desc, pin, type, mode, value);
	THE_APP->GetDvListCtrlControllerPins()->Freeze();
	
	for (DcmItemList::iterator it = rows.begin(); it != rows.end(); ++it)
		THE_APP->GetDvListCtrlControllerPins()->AppendItem(*it);
	
	THE_APP->GetDvListCtrlControllerPins()->Thaw();
}
///////////////////////////////////////////////////////////////////
void CncControl::updatePreview3D() {
///////////////////////////////////////////////////////////////////
	if ( THE_APP->getMotionMonitor() == NULL )
		return;
		
	if ( THE_CONTEXT->isOnlineUpdateDrawPane() ) 
		THE_APP->getMotionMonitor()->update(true);
}
///////////////////////////////////////////////////////////////////
bool CncControl::sendIdleMessage() {
///////////////////////////////////////////////////////////////////
	if ( getSerial() == NULL )
		return false;
		
	if ( getSerial()->isCommandActive() == true )
	{
		CNC_CEX1_A("A idle request is skipped because a other command is still active")
		return true;
	}
	
	return getSerial()->processIdle();
}
///////////////////////////////////////////////////////////////////
bool CncControl::addGuidePath(const CncPathListManager& plm) {
///////////////////////////////////////////////////////////////////
	// first release the trigger
	const Trigger::GuidePath tr(plm);
	processTrigger(tr);
	
	// do this last because appendGuidPath and follows use std::move(plm)
	if ( THE_APP->getMotionMonitor() )
		THE_APP->getMotionMonitor()->appendGuidPath(plm);
		
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::resetPodiumDistance() {
///////////////////////////////////////////////////////////////////
	if ( getSerial()->processSetter(PID_PODIUM_POS, 0) == false )
	{
		CNC_CERR_FUNCT_A("Can't reset podium position");
		return false;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
double CncControl::getPodiumDistanceMetric() {
///////////////////////////////////////////////////////////////////
	GetterValues list;
	
	if ( getSerial()->processGetter(PID_PODIUM_POS, list) == false )
	{
		CNC_CERR_FUNCT_A("Can't request podium position");
		return 0.0;
	}
	
	if ( list.size() != 1 ) 
	{
		CNC_CERR_FUNCT_A("Invalid response while request podium position, size = %ld", (long)list.size());
		return 0.0;
	}
	
	
	const int32_t dist = list.at(0);
	return THE_CONFIG->convertStepsToMetricH(dist);
}
