
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
: portType(pt)
, serialPort(NULL)
, cncConfig(NULL)
, zeroPos(0,0,0)
, startPos(0,0,0)
, curPos(0,0,0)
, controllerPos(0,0,0)
, renderMode(CncRenderAtController)
, durationCounter(0)
, interruptState(false)
, powerOn(false)
, zAxisDown(false)
, toolUpdateState(true)
, stepDelay(0)
, guiCtlSetup(NULL)
, commandCounter(0)
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
	CncConfig cc;
	updateCncConfig(cc);
	
	// init pen handler
	penHandler.reset();
}
///////////////////////////////////////////////////////////////////
CncControl::~CncControl() {
///////////////////////////////////////////////////////////////////
	assert(serialPort);
	assert(cncConfig);
	
	if ( getToolState() == true )
		switchToolOff();
	
	// safty
	if ( serialPort->isConnected() )
		serialPort->disconnect();

	delete serialPort;
	delete cncConfig;
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
	retValue.setXYZ(curPos.getX() * cncConfig->getDisplayFactX(),
				    curPos.getY() * cncConfig->getDisplayFactY(),
	                curPos.getZ() * cncConfig->getDisplayFactZ());
	return retValue;
}
//////////////////////////////////////////////////////////////////
const CncLongPosition CncControl::getMinPositions() {
//////////////////////////////////////////////////////////////////
	CncLongPosition retValue;
	retValue.setXYZ(curPos.getXMin(), curPos.getYMin(), curPos.getZMin());
	return retValue;
}//////////////////////////////////////////////////////////////////
const CncLongPosition CncControl::getMaxPositions() {
//////////////////////////////////////////////////////////////////
	CncLongPosition retValue;
	retValue.setXYZ(curPos.getXMax(), curPos.getYMax(), curPos.getZMax());
	return retValue;
}
//////////////////////////////////////////////////////////////////
const CncDoublePosition CncControl::getMinPositionsMetric() {
//////////////////////////////////////////////////////////////////
	CncDoublePosition retValue;
	retValue.setXYZ(curPos.getXMin() * cncConfig->getDisplayFactX(),
					curPos.getYMin() * cncConfig->getDisplayFactY(),
					curPos.getZMin() * cncConfig->getDisplayFactZ());
	return retValue;
}
//////////////////////////////////////////////////////////////////
const CncDoublePosition CncControl::getMaxPositionsMetric() {
//////////////////////////////////////////////////////////////////
	CncDoublePosition retValue;
	retValue.setXYZ(curPos.getXMax() * cncConfig->getDisplayFactX(),
					curPos.getYMax() * cncConfig->getDisplayFactY(),
					curPos.getZMax() * cncConfig->getDisplayFactZ());
	return retValue;
}
//////////////////////////////////////////////////////////////////
const CncLongPosition::Watermarks CncControl::getWaterMarks() {
//////////////////////////////////////////////////////////////////
	CncLongPosition::Watermarks retValue;
	curPos.getWatermarks(retValue);
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
//////////////////////////////////////////////////////////////////
void CncControl::updateCncConfig(CncConfig& cc) {
///////////////////////////////////////////////////////////////////
	if ( cncConfig != NULL )
		delete cncConfig;
	
	cncConfig = new CncConfig(cc); 
}
///////////////////////////////////////////////////////////////////
bool CncControl::processSetter(unsigned char id, int32_t value) {
///////////////////////////////////////////////////////////////////
	if ( isInterrupted() )
		return false;

	if ( isConnected() == false )
		return false;
		
	if ( serialPort->processSetter(id, value) == false ) {
		std::cerr << std::endl << "CncControl::processSetterList: Setter failed." << std::endl;
		std::cerr << " Id:    " << ArduinoPIDs::getPIDLabel((int)id) << std::endl;
		std::cerr << " Value: " << value << std::endl;

		return false;
		
	} else {
		DcmItemList rows;

		if ( IS_GUI_CTL_VALID(setterValues) ) {
			GET_GUI_CTL(setterValues)->Freeze();
			GET_GUI_CTL(setterValues)->DeleteAllItems();
			
			std::map<int, int32_t> smap = serialPort->getSetterMap();
			for (auto& x: smap) {
				DataControlModel::addKeyValueRow(rows, ArduinoPIDs::getPIDLabel(x.first), x.second);
			}
			for (wxVector<wxVector<wxVariant>>::iterator it = rows.begin(); it != rows.end(); ++it) {
				GET_GUI_CTL(setterValues)->AppendItem(*it);
			}
			GET_GUI_CTL(setterValues)->Thaw();
		}
		
		rows.clear();
		if ( IS_GUI_CTL_VALID(processedSetters) ) {
			GET_GUI_CTL(processedSetters)->Freeze();
			DataControlModel::addNumKeyValueRow(rows, GET_GUI_CTL(processedSetters)->GetItemCount() + 1, ArduinoPIDs::getPIDLabel((int)id), value);
			
			for (wxVector<wxVector<wxVariant>>::iterator it = rows.begin(); it != rows.end(); ++it) {
				GET_GUI_CTL(processedSetters)->AppendItem(*it);
			}
			
			int itemCount = guiCtlSetup->processedSetters->GetItemCount();
			GET_GUI_CTL(processedSetters)->EnsureVisible(guiCtlSetup->processedSetters->RowToItem(itemCount - 1));
			GET_GUI_CTL(processedSetters)->Thaw();
 		}
	}

	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::processSetterList(std::vector<SetterTuple>& setup) {
///////////////////////////////////////////////////////////////////
	for (std::vector<SetterTuple>::iterator it = setup.begin(); it != setup.end(); ++it) {
		if ( processSetter((*it).id, (*it).value) == false ) {
			return false;
		}
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
void CncControl::setup(bool doReset) {
///////////////////////////////////////////////////////////////////
	wxASSERT(serialPort);
	wxASSERT(cncConfig);
	
	if ( serialPort->isConnected() == false) 
		return;

	logProcessingStart();

	if ( doReset == true )
		reset();
		
	evaluateLimitState();

	std::cout << " Starting Controller initialization . . . ";

	std::vector<SetterTuple> setup;
	setup.push_back(SetterTuple(PID_STEPS_X, cncConfig->getStepsX()));
	setup.push_back(SetterTuple(PID_STEPS_Y, cncConfig->getStepsY()));
	setup.push_back(SetterTuple(PID_STEPS_Z, cncConfig->getStepsZ()));
	
	setup.push_back(SetterTuple(PID_PITCH_X, convertDoubleToCtrlLong(PID_PITCH_X, cncConfig->getPitchX())));
	setup.push_back(SetterTuple(PID_PITCH_Y, convertDoubleToCtrlLong(PID_PITCH_Y, cncConfig->getPitchY())));
	setup.push_back(SetterTuple(PID_PITCH_Z, convertDoubleToCtrlLong(PID_PITCH_Z, cncConfig->getPitchZ())));
	
	setup.push_back(SetterTuple(PID_SPEED_X, cncConfig->getSpeedX()));
	setup.push_back(SetterTuple(PID_SPEED_Y, cncConfig->getSpeedY()));
	setup.push_back(SetterTuple(PID_SPEED_Z, cncConfig->getSpeedZ()));
	
	setup.push_back(SetterTuple(PID_SDRV_PULS_WITDH_OFFSET_X, cncConfig->getPulsWidthOffsetX()));
	setup.push_back(SetterTuple(PID_SDRV_PULS_WITDH_OFFSET_Y, cncConfig->getPulsWidthOffsetY()));
	setup.push_back(SetterTuple(PID_SDRV_PULS_WITDH_OFFSET_Z, cncConfig->getPulsWidthOffsetZ()));

	setup.push_back(SetterTuple(PID_STEP_MULTIPLIER_X, cncConfig->getMultiplierX()));
	setup.push_back(SetterTuple(PID_STEP_MULTIPLIER_Y, cncConfig->getMultiplierY()));
	setup.push_back(SetterTuple(PID_STEP_MULTIPLIER_Z, cncConfig->getMultiplierZ()));
	
	setup.push_back(SetterTuple(PID_X_STEP_SIGN, cncConfig->getStepSignX()));
	setup.push_back(SetterTuple(PID_Y_STEP_SIGN, cncConfig->getStepSignY()));
	
	setup.push_back(SetterTuple(PID_POS_REPLY_THRESHOLD, cncConfig->getRelyThreshold()));
	
	if ( processSetterList(setup) ) {
		changeWorkSpeedXY(CncSpeedFly, true);
		
		// reset error info
		processCommand("r", std::cerr);
		
		std::cout << "Ready\n";
	}
	
	// Firmware check
	std::cout << "Firmware:" << std::endl;
	std::cout << " Available:\t";
	std::stringstream ss;
	processCommand("V", ss);
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
double CncControl::convertToDisplayUnit(int32_t val, double fact) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	
	if ( cncConfig->getUnit() == CncMetric ) {
		return (double)(val * fact);
	} 
	return val;
}
///////////////////////////////////////////////////////////////////
bool CncControl::processCommand(const unsigned char c, std::ostream& txtCtl) {
///////////////////////////////////////////////////////////////////
	wxASSERT(serialPort);
	return serialPort->processCommand(c, txtCtl, curPos);
}
///////////////////////////////////////////////////////////////////
bool CncControl::processCommand(const char* cmd, std::ostream& txtCtl) {
///////////////////////////////////////////////////////////////////
	wxASSERT(serialPort);
	return serialPort->processCommand(cmd, txtCtl, curPos);
}
///////////////////////////////////////////////////////////////////
bool CncControl::processMoveXYZ(int32_t x1, int32_t y1, int32_t z1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	wxASSERT(serialPort);
	return serialPort->processMoveXYZ(x1, y1, z1, alreadyRendered, curPos);
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
	wxASSERT(guiCtlSetup);
	
	curPos.setX(0);
	zeroPos.setX(0);
	startPos.setX(0);
	setValue(GET_GUI_CTL(xAxis), curPos.getX() * getCncConfig()->getDisplayFactX());
}
///////////////////////////////////////////////////////////////////
void CncControl::setZeroPosY() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	curPos.setY(0);
	zeroPos.setY(0);
	startPos.setY(0);
	setValue(GET_GUI_CTL(yAxis), curPos.getY() * getCncConfig()->getDisplayFactY());
}
///////////////////////////////////////////////////////////////////
void CncControl::setZeroPosZ() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	int32_t val = 0L;
	
	if ( getCncConfig()->getReferenceIncludesWpt() == true )
		val = (long)round(getCncConfig()->getWorkpieceThickness() * getCncConfig()->getCalculationFactZ());
	
	curPos.setZ(val);
	zeroPos.setZ(val);
	startPos.setZ(val);
	
	if ( isConnected() == true ) {
		if ( processSetter(PID_Z_POS, val)  == false ) {
			std::cerr << "CncControl::setZeroPosZ: processSetter failed!"<< std::endl;
		}
	}

	setValue(GET_GUI_CTL(zAxis), curPos.getZ() * getCncConfig()->getDisplayFactZ());
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
	startPos = curPos;
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
	curPos.resetWatermarks();
	curCtlPos.resetWatermarks();
	controllerPos.resetWatermarks();
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::reset() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	getSerial()->purge();
	resetInterrupt();
	
	std::cout << "Try to reset the controller\n";
	if ( processCommand("R", std::cerr) ) {
		std::cout << " Controller reseted\n";
	} else {
		std::cerr << " Controller reset failed\n";
		return false;
	}
	
	// do this after the controller reset, because setZeroPos will determine a new controller position on demand
	setZeroPos();
	
	CncLongPosition cp = getControllerPos();
	setValue(GET_GUI_CTL(xAxisCtl), cp.getX() * getCncConfig()->getDisplayFactX());
	setValue(GET_GUI_CTL(yAxisCtl), cp.getY() * getCncConfig()->getDisplayFactY());
	setValue(GET_GUI_CTL(zAxisCtl), cp.getZ() * getCncConfig()->getDisplayFactZ());
	
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
	
	double curZPos = curPos.getZ() * cncConfig->getDisplayFactZ(); // we need it as mm
	double moveZ   = curZPos * (-1);
	
	bool ret = true;
	if ( prepareSimpleMove() == true ) {
		if ( moveMetricZ(moveZ) == false ) {
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
	double curZPos = curPos.getZ() * cncConfig->getDisplayFactZ(); // we need it as mm
	double moveZ   = topZPos - curZPos;
	
	bool ret = true;
	if ( prepareSimpleMove() == true ) {
		if ( moveMetricZ(moveZ) == false ) {
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
bool CncControl::moveUpZ() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	//std::cout << "CncControl::moveUpZ()" << std::endl;
	
	double dist = cncConfig->getCurZDistance();
	double curZPos = curPos.getZ() * cncConfig->getDisplayFactZ(); // we need it as mm
	double moveZ = 0.0;
	
	if ( curZPos != dist ) {
		moveZ = dist - curZPos;
		// correct round deviations
		if ( moveZ < 0.00001 )
			moveZ = 0.0;
	}
	
	if ( (curZPos + moveZ) > cncConfig->getMaxZDistance() ) {
		std::cerr << "CncControl::moveUpZ error:" << std::endl;
		std::cerr << "Z(abs): " << curZPos + moveZ << std::endl;
		std::cerr << "Z(cur): " << curZPos << std::endl;
		std::cerr << "Z(mv):  " << moveZ << std::endl;
		std::cerr << "Z(max): " << cncConfig->getMaxZDistance() << std::endl;
		return false;
	}
	
	bool ret = moveMetricZ(moveZ);
	if ( ret ) {
		zAxisDown = false;
		updateZSlider();
		changeWorkSpeedXY(CncSpeedFly);
	} else {
		std::cerr << "CncControl::moveUpZ() error: " << moveZ << ", " << curZPos << ", " << dist << std::endl;
	}

	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveDownZ() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	//std::cout << "CncControl::moveDownZ()" << std::endl;

	double curZPos = curPos.getZ() * cncConfig->getDisplayFactZ(); // we need it as mm
	double newZPos = cncConfig->getDurationPositionAbs(getDurationCounter());
	double moveZ   = (curZPos - newZPos) * (-1);

	if ( false ) {
		std::clog << "moveDownZ:  " << std::endl;
		std::clog << " zAxisDown  " << zAxisDown << std::endl;
		std::clog << " curZPos:   " << curZPos << std::endl;
		std::clog << " newZPos:   " << newZPos << std::endl;
		std::clog << " moveZ:     "	<< moveZ << std::endl;
		std::clog << " duration:  "	<< getDurationCounter() << std::endl;
	}
	
	bool ret = moveMetricZ(moveZ);
	if ( ret ) {
		zAxisDown = true;
		updateZSlider();
		changeWorkSpeedXY(CncSpeedWork);
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void CncControl::changeWorkSpeedXY(CncSpeed s, bool force) {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	cncConfig->setActiveSpeedXY(s);
	int mmm = getSpeedControlMode() == DM_2D ? PID_SWITCH_MOVE_MODE_STATE_2D : PID_SWITCH_MOVE_MODE_STATE_3D;
	processSetter(mmm, (s == CncSpeedWork));
	processSetter(PID_SPEED_X, cncConfig->getSpeedX());
	processSetter(PID_SPEED_Y, cncConfig->getSpeedY());
	
	if (GET_GUI_CTL(speedView) && toolUpdateState == true ) GET_GUI_CTL(speedView)->setCurrentSpeedX(cncConfig->getSpeedX());
	if (GET_GUI_CTL(speedView) && toolUpdateState == true ) GET_GUI_CTL(speedView)->setCurrentSpeedY(cncConfig->getSpeedY());

	if ( force == true )
		updateCncConfigTrace();
}
///////////////////////////////////////////////////////////////////
void CncControl::changeWorkSpeedZ(CncSpeed s, bool force) {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	cncConfig->setActiveSpeedZ(s);
	int mmm = getSpeedControlMode() == DM_2D ? PID_SWITCH_MOVE_MODE_STATE_2D : PID_SWITCH_MOVE_MODE_STATE_3D;
	processSetter(mmm, (s == CncSpeedWork));
	processSetter(PID_SPEED_Z, cncConfig->getSpeedZ());
	
	if ( GET_GUI_CTL(speedView) && toolUpdateState == true ) GET_GUI_CTL(speedView)->setCurrentSpeedZ(cncConfig->getSpeedZ());

	if ( force == true )
		updateCncConfigTrace();
}
///////////////////////////////////////////////////////////////////
void CncControl::logProcessingStart() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	setValue(GET_GUI_CTL(cmdDuration), "0");
	setValue(GET_GUI_CTL(cmdCount), "0");
	
	ftime(&startTime);
	commandCounter=0;
}
///////////////////////////////////////////////////////////////////
void CncControl::logProcessingCurrent() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	ftime(&endTime);
	
	int t_diff = (int) (1000.0 * (endTime.time - startTime.time) + (endTime.millitm - startTime.millitm));  

	setValue(GET_GUI_CTL(cmdDuration), t_diff);
	setValue(GET_GUI_CTL(cmdCount), commandCounter);
}
///////////////////////////////////////////////////////////////////
void CncControl::logProcessingEnd(bool valuesOnly) {
	wxASSERT(guiCtlSetup);
	
	// final set to compensate commandCounter%100
	setValue(GET_GUI_CTL(xAxis), convertToDisplayUnit(curPos.getX(), cncConfig->getDisplayFactX()));
	setValue(GET_GUI_CTL(yAxis), convertToDisplayUnit(curPos.getY(), cncConfig->getDisplayFactY()));
	setValue(GET_GUI_CTL(zAxis), convertToDisplayUnit(curPos.getZ(), cncConfig->getDisplayFactZ()));
	
	setValue(GET_GUI_CTL(xAxisCtl), convertToDisplayUnit(curCtlPos.getX(), cncConfig->getDisplayFactX()));
	setValue(GET_GUI_CTL(yAxisCtl), convertToDisplayUnit(curCtlPos.getY(), cncConfig->getDisplayFactY()));
	setValue(GET_GUI_CTL(zAxisCtl), convertToDisplayUnit(curCtlPos.getZ(), cncConfig->getDisplayFactZ()));

	updateZSlider();
	
	if ( valuesOnly == false )
		logProcessingCurrent();
}
///////////////////////////////////////////////////////////////////
void CncControl::forceDisplayPositions() {
///////////////////////////////////////////////////////////////////
	logProcessingEnd(true);
}
///////////////////////////////////////////////////////////////////
void CncControl::waitActive(unsigned int milliseconds, bool once) {
///////////////////////////////////////////////////////////////////
	if ( milliseconds == 0 ) {
		if ( once == true ) {
			while (wxEventLoopBase::GetActive()->Pending()) {
				wxEventLoopBase::GetActive()->Dispatch();
			}
		}
		return;
	}
	
	wxDateTime s = wxDateTime::UNow();
	wxDateTime e = wxDateTime::UNow();
	
	while ( wxTimeSpan(e - s).GetMilliseconds() < milliseconds ) {
		while (wxEventLoopBase::GetActive()->Pending()) {
			wxEventLoopBase::GetActive()->Dispatch();
			
			e = wxDateTime::UNow();
			if ( wxTimeSpan(e - s).GetMilliseconds() < milliseconds )
				break;
		}
		
		e = wxDateTime::UNow();
	}
}
///////////////////////////////////////////////////////////////////
bool CncControl::validateCurrentPostion() {
///////////////////////////////////////////////////////////////////
	if ( positionCheck == false )
		return true;
		
	// todo - performance - wxRealPoint, switch to predef values, etc
	wxPoint p(curPos.getX()/cncConfig->getCalculationFactX(), curPos.getY()/cncConfig->getCalculationFactY());

	if ( p.x >= cncConfig->getMaxDimensionX() || p.y >= cncConfig->getMaxDimensionY() ) {
		
		std::clog << "Position out of range!" << std::endl;
		std::clog << " Max valid X dimension: " << cncConfig->getMaxDimensionX() << std::endl;
		std::clog << " Max valid Y dimension: " << cncConfig->getMaxDimensionY() << std::endl;
		std::clog << " Current X Pos: " << p.x << std::endl;
		std::clog << " Current Y Pos: " << p.y << std::endl;
		return false;
	}
	
	if ( p.x < -cncConfig->getMaxDimensionX() || p.y < -cncConfig->getMaxDimensionY() ) {
		
		std::clog << "Position out of range!" << std::endl;
		std::clog << " Min valid X dimension: " << -cncConfig->getMaxDimensionX() << std::endl;
		std::clog << " Min valid Y dimension: " << -cncConfig->getMaxDimensionY() << std::endl;
		std::clog << " Current X Pos: " << p.x << std::endl;
		std::clog << " Current Y Pos: " << p.y << std::endl;
		return false;
	}
	
	return true;
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
	
	if ( GET_GUI_CTL(mainWnd) != NULL )
		GET_GUI_CTL(mainWnd)->displayNotification(type, "Controller Callback", msg, (type == 'E' ? 8 : 4));
	
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
	wxASSERT(guiCtlSetup);
	
	// Event handling, enables the interrrpt functionality
	if ( cncConfig->isAllowEventHandling() ) {
		wxEventLoopBase* evtLoop = wxEventLoopBase::GetActive();
		while (evtLoop->Pending())
			evtLoop->Dispatch();
	}
	
	switch ( ci.infoType ) {
		case CITHeartbeat:
			if ( ci.command == 'T' ) {
				std::stringstream ss;
				ss << "Heartbeat received - Value: " << ci.heartbeatValue << std::endl;
				cnc::trc.logInfoMessage(ss);
			}
			break;
		
		case CITPosition:
			// Display coordinates
			if ( cncConfig->isOnlineUpdateCoordinates() ) {
				curCtlPos = ci.controllerPos;
				//todo update interval
				//if ( commandCounter%cncConfig->getUpdateInterval() == 0 ) {
					setValue(GET_GUI_CTL(xAxisCtl), convertToDisplayUnit(ci.controllerPos.getX(), cncConfig->getDisplayFactX()));
					setValue(GET_GUI_CTL(yAxisCtl), convertToDisplayUnit(ci.controllerPos.getY(), cncConfig->getDisplayFactY()));
					setValue(GET_GUI_CTL(zAxisCtl), convertToDisplayUnit(ci.controllerPos.getZ(), cncConfig->getDisplayFactZ()));
				//}
			}
			
			// pause hanling
			if ( ci.isPause == true ) {
				// to avoid positon failtures truning a pause interval 
				// synchronize the cur position with the controller position
				curPos = ci.controllerPos;
			}
			
			break;
			
		case CITSetterInfo:
			//if ( getSerial()->isSpyOutput() == true )
			//	cnc::spy << "Setter: " << ArduinoPIDs::getPIDLabel((int)ci.setterId) << ": " << ci.setterValue << std::endl;
			break;
		
		case CITLimitInfo:
			//std::clog << "::L: " << ci.xLimit << ", " << ci.yLimit << ", " << ci.zLimit << std::endl;
			evaluateLimitState(ci.xLimit, ci.yLimit, ci.zLimit);
			break;
		
		default:
			std::cerr << "CncControl::SerialControllerCallback:" << std::endl;
			std::cerr << " No handler defined for controller info type:" << ci.infoType << std::endl;
	}

	return true;
}
///////////////////////////////////////////////////////////////////
bool CncControl::SerialCallback(int32_t cmcCount) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	wxASSERT(guiCtlSetup);

	if ( isInterrupted() ) {
		std::cerr << "SerialCallback: Interrupt detected"<< std::endl;
		return false;
	}

	// Evalutate the command counter
	commandCounter += cmcCount;
	
	// Event handling, enables the interrupt functionallity
	if ( cncConfig->isAllowEventHandling() ) {
		wxEventLoopBase* evtLoop = wxEventLoopBase::GetActive();
		while (evtLoop->Pending())
			evtLoop->Dispatch();
	}

	// position out of configured range?
	if ( validateCurrentPostion() == false )
		interrupt();

	// motion monitor
	static CncMotionMonitor::VerticeData vd;
	// todo . . . remove zAxisDown and penHandler
	if ( zAxisDown == true )	vd.setWorkVertice(curPos);
	else						vd.setFlyVertice(curPos);
	
	if ( IS_GUI_CTL_VALID(motionMonitor) ) {
		GET_GUI_CTL(motionMonitor)->appendVertice(vd);
		updatePreview3D(false);
	}

	// Display coordinates
	if ( cncConfig->isOnlineUpdateCoordinates() ) {
		if ( commandCounter%cncConfig->getUpdateInterval() == 0 ) {
			setValue(GET_GUI_CTL(xAxis), convertToDisplayUnit(curPos.getX(), cncConfig->getDisplayFactX()));
			setValue(GET_GUI_CTL(yAxis), convertToDisplayUnit(curPos.getY(), cncConfig->getDisplayFactY()));
			setValue(GET_GUI_CTL(zAxis), convertToDisplayUnit(curPos.getZ(), cncConfig->getDisplayFactZ()));
			
			logProcessingCurrent();
		}
	
		updateZSlider();
	}
	
	if ( GetAsyncKeyState(VK_ESCAPE) != 0 ) {
		std::cerr << "SerialCallback: ESCAPE key detected" << std::endl;
		interrupt();
	}
	
	return !isInterrupted();
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
	if ( curPos != zeroPos ) {
		int32_t moveX=0, moveY=0;
		moveX = zeroPos.getX() - curPos.getX(); 
		moveY = zeroPos.getY() - curPos.getY();
		
		if ( dim == CncDimension2D ) {
			ret = moveLinearStepsXY(moveX, moveY, false);
			
		} else {
			if ( moveLinearStepsXY(moveX, 0, false) == false ) 
				return false;
				
			if ( moveLinearStepsXY(0, moveY, false) == false )
				return false;
		}
	}
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveXYZToZeroPos(CncDimensions dim) {
///////////////////////////////////////////////////////////////////
	bool ret = true;
	if ( curPos != zeroPos ) {
		int32_t moveX=0, moveY=0, moveZ=0;
		moveX = zeroPos.getX() - curPos.getX(); 
		moveY = zeroPos.getY() - curPos.getY();
		moveZ = zeroPos.getZ() - curPos.getZ();
		
		if ( dim == CncDimension3D ) {
			ret = moveLinearStepsXYZ(moveX, moveY, moveZ, false);
			
		} else if ( dim == CncDimension2D ) {
			if ( moveLinearStepsXYZ(0, 0, moveZ, false) == false ) 
				return false;
				
			if ( moveLinearStepsXYZ(moveX, moveY, 0, false) == false ) 
				return false;
			
		} else {
			if ( moveLinearStepsXYZ(moveX, 0, 0, false) == false ) 
				return false;
				
			if ( moveLinearStepsXYZ(0, moveY, 0, false) == false ) 
				return false;
				
			if ( moveLinearStepsXYZ(0, 0, moveZ, false) == false ) 
				return false;
		}
	}
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveZToZeroPos() {
///////////////////////////////////////////////////////////////////
	bool ret = true;
	if ( curPos != zeroPos ) {
		int32_t moveZ=0;
		moveZ = zeroPos.getZ() - curPos.getZ();
		ret = moveLinearStepsXYZ(0, 0, moveZ, false);
	}
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveToStartPos() {
///////////////////////////////////////////////////////////////////
	bool ret = true;
	if ( curPos != startPos ) {
		int32_t moveX=0, moveY=0;
		moveX = startPos.getX() - curPos.getX(); 
		moveY = startPos.getY() - curPos.getY();
		moveLinearStepsXY(moveX, moveY, false);
	}
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveLinearStepsXY(int32_t x1, int32_t y1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	//avoid empty steps
	if ( x1 == 0 && y1 == 0 )
		return true;
	
	// optimize pure vertical moves, nothing to render
	if ( x1 == 0 )
		return serialPort->processMoveXY(x1, y1, true, curPos);
		
	// optimize pure horizontal moves, nothing to render
	if ( y1 == 0 )
		return serialPort->processMoveXY(x1, y1, true, curPos);

	if ( renderMode == CncRenderAtController )
		return serialPort->processMoveXY(x1, y1, false, curPos);
		
/*
	if ( alreadyRendered == true )
		return serialPort->processMoveXY(x1, y1, alreadyRendered, curPos);

	// render here
	int32_t x0 = 0, y0 = 0;
	int32_t dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int32_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int32_t err = dx + dy, e2; // error value e_xy 

	int32_t xOld = x0, yOld = y0;
	while( true ) {
		//std::cout << xOld - x0 << ", " << yOld - y0 << std::endl;
		if ( serialPort->processMoveXY(xOld - x0, yOld - y0, true, curPos) == false )
			return false;
			
		xOld = x0; yOld = y0;
		
		if ( x0 == x1 && y0 == y1 ) 
			break;
			
		e2 = 2*err;
		if (e2 > dy) 
			{ err += dy; x0 += sx; } // e_xy+e_x > 0 
		if (e2 < dx) 
			{ err += dx; y0 += sy; } // e_xy+e_y < 0 
	}

	return true;
	*/
	
	return false;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveLinearStepsXYZ(int32_t x1, int32_t y1, int32_t z1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	long sum = x1 + y1 + z1;
	
	//avoid empty steps
	if ( sum == 0 )
		return true;
		
	// optimize pure one xisis moves, nothing to render
	if ( sum == x1 || sum == y1 || sum == z1 )
		return serialPort->processMoveXYZ(x1, y1, z1, true, curPos);
	
	if ( renderMode == CncRenderAtController ) {
		return serialPort->processMoveXYZ(x1, y1, z1, alreadyRendered, curPos);
	}

	return false;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveLinearMetricXY(double x1, double y1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double sX1 = x1 * cncConfig->getCalculationFactX();
	double sY1 = y1 * cncConfig->getCalculationFactY();
	
	return moveLinearStepsXY((int32_t)round(sX1), 
	                         (int32_t)round(sY1),
							 alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveLinearMetricXYZ(double x1, double y1, double z1, bool alreadyRendered) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double sX1 = x1 * cncConfig->getCalculationFactX();
	double sY1 = y1 * cncConfig->getCalculationFactY();
	double sZ1 = z1 * cncConfig->getCalculationFactZ();
	
	return moveLinearStepsXYZ((int32_t)round(sX1), 
	                          (int32_t)round(sY1),
							  (int32_t)round(sZ1),
							  alreadyRendered);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveStepsZ(int32_t z) {
///////////////////////////////////////////////////////////////////
	if ( z == 0 )
		return true;
	// z moves are always linear, as a consequence alreadyRendered can be true
	return serialPort->processMoveZ(z, true, curPos);
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveMetricZ(double z) {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double sZ = z * cncConfig->getCalculationFactZ();
	
	return moveStepsZ((int32_t)round(sZ));
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

	if ( isInterrupted() == false )
		getSerial()->processGetter(PID_XYZ_POS, list);
	
	if ( list.size() != 3 ){
		controllerPos.setX(0);
		controllerPos.setY(0);
		controllerPos.setZ(0);
		
		if ( isInterrupted() == false ) {
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
	return ( curPos == ctlPos );
}
///////////////////////////////////////////////////////////////////
void CncControl::updateCncConfigTrace() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	if ( cncConfig->isModified() == false )
		return;
	
	wxVector<wxVector<wxVariant>> rows;
	if ( guiCtlSetup->staticCncConfig ) {
		cncConfig->getStaticValues(rows);
		GET_GUI_CTL(staticCncConfig)->Freeze();
		GET_GUI_CTL(staticCncConfig)->DeleteAllItems();
		for (wxVector<wxVector<wxVariant>>::iterator it = rows.begin(); it != rows.end(); ++it) {
			wxVector<wxVariant> row = *it;
			GET_GUI_CTL(staticCncConfig)->AppendItem(row);
		}
		GET_GUI_CTL(staticCncConfig)->Thaw();
	}
	
	rows.clear();
	if ( GET_GUI_CTL(dynamicCncConfig) ) {
		cncConfig->getDynamicValues(rows);
		GET_GUI_CTL(dynamicCncConfig)->Freeze();
		GET_GUI_CTL(dynamicCncConfig)->DeleteAllItems();
		for (wxVector<wxVector<wxVariant>>::iterator it = rows.begin(); it != rows.end(); ++it) {
			wxVector<wxVariant> row = *it;
			GET_GUI_CTL(dynamicCncConfig)->AppendItem(row);
		}
		GET_GUI_CTL(dynamicCncConfig)->Thaw();
	}
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
void CncControl::resetZSlider() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	if ( guiCtlSetup->zView == NULL ) 
		return;
		
	GET_GUI_CTL(zView)->resetAll();
}
///////////////////////////////////////////////////////////////////
void CncControl::updateZSlider() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	if ( guiCtlSetup->zView == NULL ) 
		return;
		
	wxASSERT(cncConfig);
	GET_GUI_CTL(zView)->updateView(curPos.getZ() * cncConfig->getDisplayFactZ(), *cncConfig);
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
			ctl->SetBackgroundColour(*wxRED);
			ctl->SetForegroundColour(*wxWHITE);
			
		} else {
			ctl->SetLabel(wxString("0"));
			ctl->SetBackgroundColour(*wxGREEN);
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
			case 'X': 	ret = moveLinearMetricXY(maxSteps, 0.0, true);
						if ( ret ) 
							minPos = curCtlPos.getX();
						break;
						
			case 'Y': 	ret = moveLinearMetricXY(0.0, maxSteps, true);
						if ( ret ) 
							minPos = curCtlPos.getY();
						break;
						
			case 'Z': 	ret = moveMetricZ(maxSteps);
						if ( ret ) 
							minPos = curCtlPos.getZ();
						break;
		}
		
		// move to max position
		if ( ret ) {
			if ( min ) min->SetValue(true);
			maxSteps *= -1;
			
			switch ( axis ) {
				case 'X': 	ret = moveLinearMetricXY(maxSteps, 0.0, true);
							if ( ret ) 
								maxPos = curCtlPos.getX();
							break;
							
				case 'Y': 	ret = moveLinearMetricXY(0.0, maxSteps, true);
							if ( ret ) 
								maxPos = curCtlPos.getY();
							break;
							
				case 'Z': 	ret = moveMetricZ(maxSteps);
							if ( ret ) 
								maxPos = curCtlPos.getZ();
							break;
			}
		}
		
		// free end position
		if ( ret ) {
			if ( max ) max->SetValue(true);
			
			switch ( axis ) {
				case 'X': 	ret = moveLinearMetricXY(-endSwitchStepBackMertic, 0.0, true);
							break;
							
				case 'Y': 	ret = moveLinearMetricXY(0.0, -endSwitchStepBackMertic, true);
							break;
							
				case 'Z': 	ret = moveMetricZ(-endSwitchStepBackMertic);
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
	double maxSteps = getCncConfig()->getMaxDimensionX() * (-1);
	bool ret = false;
	if ( prepareSimpleMove() == true ) {
		ret = moveLinearMetricXY(maxSteps, 0.0, true);
		if (ret)
			ret = moveLinearMetricXY(+endSwitchStepBackMertic, 0.0, true);
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
	double maxSteps = getCncConfig()->getMaxDimensionX();
	bool ret = false;
	if ( prepareSimpleMove() == true ) {
		ret = moveLinearMetricXY(maxSteps, 0.0, true);
		if (ret)
			ret = moveLinearMetricXY(-endSwitchStepBackMertic, 0.0, true);
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
	double maxSteps = getCncConfig()->getMaxDimensionY() * (-1);
	bool ret = false;
	if ( prepareSimpleMove() == true ) {
		ret = moveLinearMetricXY(0.0, maxSteps, true);
		if (ret)
			ret = moveLinearMetricXY(0.0, +endSwitchStepBackMertic, true);
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
	double maxSteps = getCncConfig()->getMaxDimensionY();
	bool ret = false;
	if ( prepareSimpleMove() == true ) {
		ret = moveLinearMetricXY(0.0, maxSteps, true);
		if (ret)
			ret = moveLinearMetricXY(0.0, -endSwitchStepBackMertic, true);
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
	double maxSteps = getCncConfig()->getMaxDimensionZ() * (-1);
	bool ret = false;
	if ( prepareSimpleMove() == true ) {
		ret = moveMetricZ(maxSteps);
		if (ret)
			ret = moveMetricZ(+endSwitchStepBackMertic);
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
	double maxSteps = getCncConfig()->getMaxDimensionZ();
	bool ret = false;
	if ( prepareSimpleMove() == true ) {
		ret = moveMetricZ(maxSteps);
		if (ret)
			ret = moveMetricZ(-endSwitchStepBackMertic);
	}
	reconfigureSimpleMove(ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveXToMid() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double maxSteps = getCncConfig()->getMaxDimensionX();
	bool ret = false;

	if ( prepareSimpleMove() == true ) {
		ret = moveLinearMetricXY(maxSteps, 0.0, true);
		if ( ret )
			ret = moveLinearMetricXY(-maxSteps/2, 0.0, true);
	}
	reconfigureSimpleMove(ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveYToMid() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double maxSteps = getCncConfig()->getMaxDimensionY();

	bool ret = false;
	if ( prepareSimpleMove() == true ) {
		ret = moveLinearMetricXY(0.0, maxSteps, true);
		if ( ret )
			ret = moveLinearMetricXY(0.0, -maxSteps/2, true);
	}
	reconfigureSimpleMove(ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncControl::moveZToMid() {
///////////////////////////////////////////////////////////////////
	wxASSERT(cncConfig);
	double maxSteps = getCncConfig()->getMaxDimensionZ();

	bool ret = false;
	if ( prepareSimpleMove() == true ) {
		ret = moveMetricZ(maxSteps);
		if ( ret )
			ret = moveMetricZ(-maxSteps/2);
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
			ret = moveLinearStepsXY(x, y, alreadyRendered );
			if ( limitStates.hasLimit() ) {
				
				if ( x != 0 ) {
					if ( limitStates.isXLimitStateValid() && limitStates.getXMinLimit() )
						ret = moveLinearMetricXY(+endSwitchStepBackMertic, 0.0, false);
						
					if ( limitStates.isXLimitStateValid() && limitStates.getXMaxLimit() )
						ret = moveLinearMetricXY(-endSwitchStepBackMertic, 0.0, false);
				}
				
				if ( y != 0 ) {
					if ( limitStates.isYLimitStateValid() && limitStates.getYMinLimit() )
						ret = moveLinearMetricXY(0.0, +endSwitchStepBackMertic, false);
						
					if ( limitStates.isYLimitStateValid() && limitStates.getYMaxLimit() )
						ret = moveLinearMetricXY(0.0, -endSwitchStepBackMertic, false);
				}
			}
				
			reconfigureSimpleMove(ret);
		}
	}
	
	if ( z != 0 ) {
		ret = false;
		if ( prepareSimpleMove(false) == true ) {
			ret = moveStepsZ(z);
			if ( ret && limitStates.hasLimit() ) {
				
				if ( limitStates.isZLimitStateValid() && limitStates.getZMinLimit() )
					ret = moveMetricZ(+endSwitchStepBackMertic);
					
				if ( limitStates.isZLimitStateValid() && limitStates.getZMaxLimit() )
					ret = moveMetricZ(-endSwitchStepBackMertic);
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
			ret = moveLinearStepsXYZ(x, y, z, alreadyRendered );
			
			if ( limitStates.hasLimit() ) {
				
				if ( x != 0 ) {
					if ( limitStates.isXLimitStateValid() && limitStates.getXMinLimit() )
						ret = moveLinearMetricXYZ(+endSwitchStepBackMertic, 0.0, 0.0, false);
						
					if ( limitStates.isXLimitStateValid() && limitStates.getXMaxLimit() )
						ret = moveLinearMetricXYZ(-endSwitchStepBackMertic, 0.0, 0.0, false);
				}
				
				if ( y != 0 ) {
					if ( limitStates.isYLimitStateValid() && limitStates.getYMinLimit() )
						ret = moveLinearMetricXYZ(0.0, +endSwitchStepBackMertic, 0.0,false);
						
					if ( limitStates.isYLimitStateValid() && limitStates.getYMaxLimit() )
						ret = moveLinearMetricXYZ(0.0, -endSwitchStepBackMertic, 0.0, false);
				}
				
				if ( z != 0 ) {
					if ( limitStates.isZLimitStateValid() && limitStates.getZMinLimit() )
						ret = moveLinearMetricXYZ(0.0, 0.0, +endSwitchStepBackMertic, false);
						
					if ( limitStates.isZLimitStateValid() && limitStates.getZMaxLimit() )
						ret = moveLinearMetricXYZ(0.0, 0.0, -endSwitchStepBackMertic, false);
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
	getCncConfig()->setAllowEventHandling(enaleEventHandling);
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
		curPos = getControllerPos();
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
	if ( IS_GUI_CTL_NOT_VALID(motionMonitor) )
		return;
		
	if ( force == true ) {
		GET_GUI_CTL(motionMonitor)->display();
		return;
	}
	
	// Online drawing coordinates
	if ( cncConfig->isOnlineUpdateDrawPane() ) {
		if ( commandCounter%cncConfig->getUpdateInterval() == 0 ) {
			GET_GUI_CTL(motionMonitor)->display();
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
