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
#include "CncDrawPane.h"
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
, lastDrawPoint(0,0)
, lastDrawPoint3D(0,0,0)
, renderMode(CncRenderAtController)
, durationCounter(0)
, interruptState(false)
, powerOn(false)
, zAxisDown(false)
, toolUpdateState(true)
, showGrid(true)
, stepDelay(0)
, guiCtlSetup(NULL)
, commandCounter(0)
, positionCheck(true)
, drawControl(NULL)
, drawPaneMargin(30)
, motionMonitorMode(MMM_2D)
{
//////////////////////////////////////////////////////////////////
	if      ( pt == CncPORT ) 		serialPort = new Serial(this);
	else if ( pt == CncEMU_NULL )	serialPort = new SerialEmulatorNULL(this);
	else if ( pt == CncEMU_SVG )	serialPort = new SerialEmulatorSVG(this);
	else 							serialPort = new Serial(this);
	
	// create default config
	CncConfig cc;
	updateCncConfig(cc);
	
	// init coord systam
	drawPaneCoordSystem.setup(CST_NULL_Y_IS_TOP, drawPaneMargin, drawPaneMargin);
	
	// init pen handler
	penHandler.reset();
	
	//preallocate memory
	drawPoints.reserve(1000 * 1000);
	drawPoints3D.reserve(1000 * 1000);
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
	
	updateDrawPointFactors();
}
//////////////////////////////////////////////////////////////////
void CncControl::setDrawPaneZoomFactor(double f) {
///////////////////////////////////////////////////////////////////
	cncConfig->setDrawPaneZoomFactor(f);
	
	updateDrawPointFactors();
	initLastDrawPoint();
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

		if ( guiCtlSetup->setterValues != NULL ) {
			guiCtlSetup->setterValues->Freeze();
			guiCtlSetup->setterValues->DeleteAllItems();
			
			std::map<int, int32_t> smap = serialPort->getSetterMap();
			for (auto& x: smap) {
				DataControlModel::addKeyValueRow(rows, ArduinoPIDs::getPIDLabel(x.first), x.second);
			}
			for (wxVector<wxVector<wxVariant>>::iterator it = rows.begin(); it != rows.end(); ++it) {
				guiCtlSetup->setterValues->AppendItem(*it);
			}
			guiCtlSetup->setterValues->Thaw();
		}
		
		rows.clear();
		if ( guiCtlSetup->processedSetters != NULL ) {
			guiCtlSetup->processedSetters->Freeze();
			DataControlModel::addNumKeyValueRow(rows, guiCtlSetup->processedSetters->GetItemCount() + 1, ArduinoPIDs::getPIDLabel((int)id), value);
			
			for (wxVector<wxVector<wxVariant>>::iterator it = rows.begin(); it != rows.end(); ++it) {
				guiCtlSetup->processedSetters->AppendItem(*it);
			}
			
			int itemCount = guiCtlSetup->processedSetters->GetItemCount();
			guiCtlSetup->processedSetters->EnsureVisible(guiCtlSetup->processedSetters->RowToItem(itemCount - 1));
			guiCtlSetup->processedSetters->Thaw();
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

	setup.push_back(SetterTuple(PID_STEP_MULTIPLIER_X, cncConfig->getMultiplierX()));
	setup.push_back(SetterTuple(PID_STEP_MULTIPLIER_Y, cncConfig->getMultiplierY()));
	setup.push_back(SetterTuple(PID_STEP_MULTIPLIER_Z, cncConfig->getMultiplierZ()));
	
	setup.push_back(SetterTuple(PID_X_STEP_SIGN, cncConfig->getStepSignX()));
	setup.push_back(SetterTuple(PID_Y_STEP_SIGN, cncConfig->getStepSignY()));
	
	setup.push_back(SetterTuple(PID_POS_REPLY_THRESHOLD, cncConfig->getRelyThreshold()));
	
	if ( processSetterList(setup) ) {
		changeWorkSpeedXY(CncSpeedFly);
		
		// reset error info
		processCommand("r", std::cerr);
		
		std::cout << "Ready\n";
	}
	processCommand("V", std::cout);

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
	drawPoints.clear();
	penHandler.reset();
	updateDrawControl();
	
	drawPoints3D.clear();
	
	if ( guiCtlSetup != NULL && guiCtlSetup->drawPane3D != NULL )
		guiCtlSetup->drawPane3D->clear3D();
}
///////////////////////////////////////////////////////////////////
void CncControl::updateDrawControl() {
///////////////////////////////////////////////////////////////////
	if ( drawControl ) {
		drawControl->Refresh();
	}
	
	if ( guiCtlSetup != NULL && guiCtlSetup->drawPane3D != NULL )
		guiCtlSetup->drawPane3D->Refresh();
}
///////////////////////////////////////////////////////////////////
void CncControl::reconstructDrawControl(int oldCorrectionY, long oldNpX, long oldNpY) {
///////////////////////////////////////////////////////////////////
	for (DrawPoints::iterator it = drawPoints.begin(); it != drawPoints.end(); ++it) {
		(*it).lp.x -= oldNpX;
		(*it).cp.x -= oldNpX;
		(*it).lp.x += drawPaneCoordSystem.getNpX();
		(*it).cp.x += drawPaneCoordSystem.getNpX();
		
		(*it).lp.y -= oldNpY;
		(*it).lp.y *= oldCorrectionY;
		(*it).lp.y *= drawPaneCoordSystem.getCorrectionY();
		(*it).lp.y += drawPaneCoordSystem.getNpY();
		
		(*it).cp.y -= oldNpY;
		(*it).cp.y *= oldCorrectionY;
		(*it).cp.y *= drawPaneCoordSystem.getCorrectionY();
		(*it).cp.y += drawPaneCoordSystem.getNpY();
	}
	updateDrawControl();
}
///////////////////////////////////////////////////////////////////
void CncControl::reconstructDrawControl(double fact, double factOld) {
///////////////////////////////////////////////////////////////////
	for (DrawPoints::iterator it = drawPoints.begin(); it != drawPoints.end(); ++it) {
		(*it).lp.x -= drawPaneCoordSystem.getNpX();
		(*it).lp.x *= fact / factOld;
		(*it).lp.x += drawPaneCoordSystem.getNpX();
		
		(*it).lp.y -= drawPaneCoordSystem.getNpY();
		(*it).lp.y *= fact / factOld;
		(*it).lp.y += drawPaneCoordSystem.getNpY();
		
		(*it).cp.x -= drawPaneCoordSystem.getNpX();
		(*it).cp.x *= fact / factOld;
		(*it).cp.x += drawPaneCoordSystem.getNpX();
		
		(*it).cp.y -= drawPaneCoordSystem.getNpY();
		(*it).cp.y *= fact / factOld;
		(*it).cp.y += drawPaneCoordSystem.getNpY();
	}

	updateDrawControl();
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
	posMarker.setMarkerControls(guiCtlSetup->xAxisMarkerTop, guiCtlSetup->xAxisMarkerBottom, guiCtlSetup->yAxisMarker);
	toolState.setControl(guiCtlSetup->toolState);
	setToolState(true);
}
///////////////////////////////////////////////////////////////////
void CncControl::setZeroPosX() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	curPos.setX(0);
	zeroPos.setX(0);
	startPos.setX(0);
	setValue(guiCtlSetup->xAxis, curPos.getX() * getCncConfig()->getDisplayFactX());
}
///////////////////////////////////////////////////////////////////
void CncControl::setZeroPosY() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	curPos.setY(0);
	zeroPos.setY(0);
	startPos.setY(0);
	setValue(guiCtlSetup->yAxis, curPos.getY() * getCncConfig()->getDisplayFactY());
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

	setValue(guiCtlSetup->zAxis, curPos.getZ() * getCncConfig()->getDisplayFactZ());
}
///////////////////////////////////////////////////////////////////
void CncControl::setZeroPos() {
///////////////////////////////////////////////////////////////////
	setZeroPosX();
	setZeroPosY();
	setZeroPosZ();
	
	posMarker.setPosition({curPos.getX() + (int)drawPaneMargin, curPos.getY() + (int)drawPaneMargin - 5});
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
	switchToolOff();
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
	setValue(guiCtlSetup->xAxisCtl, cp.getX() * getCncConfig()->getDisplayFactX());
	setValue(guiCtlSetup->yAxisCtl, cp.getY() * getCncConfig()->getDisplayFactY());
	setValue(guiCtlSetup->zAxisCtl, cp.getZ() * getCncConfig()->getDisplayFactZ());
	
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
	
	if ( guiCtlSetup->passingTrace && toolUpdateState == true )
		guiCtlSetup->passingTrace->SetValue(wxString() << durationCounter);
}
///////////////////////////////////////////////////////////////////
void CncControl::initNextDuration() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	penHandler.initNextDuration();
	getSerial()->beginDuration(getDurationCounter());
	
	durationCounter++;
	
	if ( guiCtlSetup->passingTrace && toolUpdateState == true )
		guiCtlSetup->passingTrace->SetValue(wxString() << durationCounter);
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
		
		lastDrawPoint.x = convertToDrawPointX(curPos.getX());
		lastDrawPoint.y = convertToDrawPointY(curPos.getY());
		
		lastDrawPoint3D.setX(curPos.getX());
		lastDrawPoint3D.setY(curPos.getY());
		lastDrawPoint3D.setZ(curPos.getZ());
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void CncControl::changeWorkSpeedXY(CncSpeed s) {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	cncConfig->setActiveSpeedXY(s);
	processSetter(PID_SPEED_X, cncConfig->getSpeedX());
	processSetter(PID_SPEED_Y, cncConfig->getSpeedY());
	
	if (guiCtlSetup->speedX && toolUpdateState == true ) guiCtlSetup->speedX->SetValue(cncConfig->getSpeedX());
	if (guiCtlSetup->speedY && toolUpdateState == true ) guiCtlSetup->speedY->SetValue(cncConfig->getSpeedY());

	updateCncConfigTrace();
}
///////////////////////////////////////////////////////////////////
void CncControl::changeWorkSpeedZ(CncSpeed s) {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	cncConfig->setActiveSpeedZ(s);
	processSetter(PID_SPEED_Z, cncConfig->getSpeedZ());
	
	if (guiCtlSetup->speedZ && toolUpdateState == true ) guiCtlSetup->speedZ->SetValue(cncConfig->getSpeedZ());

	updateCncConfigTrace();
}
///////////////////////////////////////////////////////////////////
void CncControl::logProcessingStart() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	setValue(guiCtlSetup->cmdDuration, "0");
	setValue(guiCtlSetup->cmdCount, "0");
	
	ftime(&startTime);
	commandCounter=0;
}
///////////////////////////////////////////////////////////////////
void CncControl::logProcessingCurrent() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	ftime(&endTime);
	
	int t_diff = (int) (1000.0 * (endTime.time - startTime.time) + (endTime.millitm - startTime.millitm));  

	setValue(guiCtlSetup->cmdDuration, t_diff);
	setValue(guiCtlSetup->cmdCount, commandCounter);
}
///////////////////////////////////////////////////////////////////
void CncControl::logProcessingEnd(bool valuesOnly) {
	wxASSERT(guiCtlSetup);
	
	// final set to compensate commandCounter%100
	setValue(guiCtlSetup->xAxis, convertToDisplayUnit(curPos.getX(), cncConfig->getDisplayFactX()));
	setValue(guiCtlSetup->yAxis, convertToDisplayUnit(curPos.getY(), cncConfig->getDisplayFactY()));
	setValue(guiCtlSetup->zAxis, convertToDisplayUnit(curPos.getZ(), cncConfig->getDisplayFactZ()));
	
	setValue(guiCtlSetup->xAxisCtl, convertToDisplayUnit(curCtlPos.getX(), cncConfig->getDisplayFactX()));
	setValue(guiCtlSetup->yAxisCtl, convertToDisplayUnit(curCtlPos.getY(), cncConfig->getDisplayFactY()));
	setValue(guiCtlSetup->zAxisCtl, convertToDisplayUnit(curCtlPos.getZ(), cncConfig->getDisplayFactZ()));

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
void CncControl::waitActive(unsigned int milliseconds) {
///////////////////////////////////////////////////////////////////
	wxEventLoopBase* evtLoop = wxEventLoopBase::GetActive();
	if ( milliseconds == 0 ) {
		while (evtLoop->Pending()) {
			evtLoop->Dispatch();
		}
		
		return;
	}
	
	wxDateTime s = wxDateTime::UNow();
	wxDateTime e = wxDateTime::UNow();
	
	while ( wxTimeSpan(e - s).GetMilliseconds() < milliseconds ) {
		while (evtLoop->Pending()) {
			evtLoop->Dispatch();
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
			cout << idStr << endl;
			long id;
			idStr.ToLong(&id);
			wxString errorCode = ArduinoErrorCodes::getECLabel(id);
			wxString replace("{[");
			replace << idStr;
			replace << "]}";
			
			msg.Replace(replace, errorCode);
		}
	}
	
	
	if ( guiCtlSetup->mainWnd != NULL )
		guiCtlSetup->mainWnd->displayNotification(type, "Controller Callback", msg, (type == 'E' ? 8 : 4));
	
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
				if ( commandCounter%cncConfig->getUpdateInterval() == 0 ) {
					setValue(guiCtlSetup->xAxisCtl, convertToDisplayUnit(ci.controllerPos.getX(), cncConfig->getDisplayFactX()));
					setValue(guiCtlSetup->yAxisCtl, convertToDisplayUnit(ci.controllerPos.getY(), cncConfig->getDisplayFactY()));
					setValue(guiCtlSetup->zAxisCtl, convertToDisplayUnit(ci.controllerPos.getZ(), cncConfig->getDisplayFactZ()));
				}
			}
			// Online drawing coordinates
			if ( cncConfig->isOnlineUpdateDrawPane() ) {
				wxClientDC dc(drawControl);
				dc.SetPen(penHandler.getCurrentPen(false));

				static wxPoint p;
				p.x = convertToDrawPointX(ci.controllerPos.getX());
				p.y = convertToDrawPointY(ci.controllerPos.getY());
				posMarker.setPosition(p);
			}
			
			// pause hanling
			if ( ci.isPause == true ) {
				// to avoid positon failtures truning a pause interval 
				// synchronize the cur position with the controller position
				curPos = ci.controllerPos;
			}
			
			break;
			
		case CITSetterInfo:
			if ( getSerial()->getInfoOutput() == true )
				std::clog << "Setter: " << ArduinoPIDs::getPIDLabel((int)ci.setterId) << ": " << ci.setterValue << std::endl;
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
	
	// Event handling, enables the interrrpt functionality
	if ( cncConfig->isAllowEventHandling() ) {
		wxEventLoopBase* evtLoop = wxEventLoopBase::GetActive();
		while (evtLoop->Pending())
			evtLoop->Dispatch();
	}

	if ( validateCurrentPostion() == false )
		interrupt();

	static wxPoint p;
	p.x = convertToDrawPointX(curPos.getX());
	p.y = convertToDrawPointY(curPos.getY());
	
	// 2D points
	static PointPair pp;
	PointPair tmp = pp;
	pp.zAxisDown = zAxisDown;
	pp.lp = lastDrawPoint;
	pp.cp = p;
	lastDrawPoint = p;
	
	// 3D points
	static PositionInfo3D pi3d;
	pi3d.lp = lastDrawPoint3D;
	pi3d.cp = curPos;
	lastDrawPoint3D = curPos;
	pi3d.zAxisDown 	= zAxisDown;
	
	// Stores the 2D coordinates for later drawings
	if ( zAxisDown == true ) {
		if ( isLastDuration() ) {
			// avoid duplicate values
			if ( drawPoints.size() > 0 ) {
				PointPair lpp = drawPoints.back();
				if ( lpp != pp ) {
					drawPoints.push_back(pp);
				}
			} else {
				drawPoints.push_back(pp);
			}
		}
	}
	
	// Stores the 3D coordinates for later drawings
	// avoid duplicate values
	if ( tmp != pp ) {
		drawPoints3D.push_back(pi3d);
	}
	
	// Online drawing coordinates
	if ( cncConfig->isOnlineUpdateDrawPane() ) {
		wxClientDC dc(drawControl);
		dc.SetPen(penHandler.getCurrentPen(zAxisDown));
		
		// avoid duplicate values
		if ( tmp != pp ) {
			if ( motionMonitorMode == MMM_2D ) {
				dc.DrawLine(pp.lp, pp.cp);
			} else {
				set3DData(true);
			}
		}
	}
	
	// Display coordinates
	if ( cncConfig->isOnlineUpdateCoordinates() ) {
		if ( commandCounter%cncConfig->getUpdateInterval() == 0 ) {
			setValue(guiCtlSetup->xAxis, convertToDisplayUnit(curPos.getX(), cncConfig->getDisplayFactX()));
			setValue(guiCtlSetup->yAxis, convertToDisplayUnit(curPos.getY(), cncConfig->getDisplayFactY()));
			setValue(guiCtlSetup->zAxis, convertToDisplayUnit(curPos.getZ(), cncConfig->getDisplayFactZ()));
		
			logProcessingCurrent();
		}
	}
	
	updateZSlider();
	
	if ( GetAsyncKeyState(VK_ESCAPE) != 0 ) {
		std::cerr << "SerialCallback: ESCAPE key detected" << std::endl;
		interrupt();
	}
		
	return !isInterrupted();
}
///////////////////////////////////////////////////////////////////
wxPoint& CncControl::convertToCoordiateSystem(wxPoint& in) {
///////////////////////////////////////////////////////////////////
	in.x -= drawPaneCoordSystem.getNpX();
	in.y -= drawPaneCoordSystem.getNpY();
	
	if ( drawPaneCoordSystem.getType() == CST_NULL_Y_IS_BOTTOM )
		in.y *= -1;

	return in;
}
///////////////////////////////////////////////////////////////////
void CncControl::setCoordinateSystemType(CoordinateSytemType t) {
///////////////////////////////////////////////////////////////////
	// store old key values
	int oldCorrectionY 	= drawPaneCoordSystem.getCorrectionY();
	long oldNpX 		= drawPaneCoordSystem.getNpX();
	long oldNpY 		= drawPaneCoordSystem.getNpY();
	
	// init new coordinate system type
	unsigned int nullX = drawPaneMargin;
	unsigned int nullY = drawPaneMargin;
	
	posMarker.setXMarkerTyp(XMarkerTop);
	
	if ( drawControl != NULL && t == CST_NULL_Y_IS_BOTTOM) {
		nullY = drawControl->GetSize().GetHeight() - drawPaneMargin;
		posMarker.setXMarkerTyp(XMarkerBottom);
	}
	
	drawPaneCoordSystem.setup(t, nullX, nullY);
	updateDrawPointFactors();
	initLastDrawPoint();
		
	// reconstruct draw points for the new coordinate system
	reconstructDrawControl(oldCorrectionY, oldNpX, oldNpY);
}
///////////////////////////////////////////////////////////////////
void CncControl::updateDrawPointFactors() {
///////////////////////////////////////////////////////////////////
	drawPaneCoordSystem.setFactorX(cncConfig->getDrawPaneZoomFactor()/cncConfig->getCalculationFactX() * 1/SvgUnitCalculator::getFactorReferenceUnit2MM());
	drawPaneCoordSystem.setFactorY(cncConfig->getDrawPaneZoomFactor()/cncConfig->getCalculationFactY() * 1/SvgUnitCalculator::getFactorReferenceUnit2MM());
}
///////////////////////////////////////////////////////////////////
void CncControl::initLastDrawPoint() {
///////////////////////////////////////////////////////////////////
	lastDrawPoint.x = drawPaneCoordSystem.getNpX();
	lastDrawPoint.y = drawPaneCoordSystem.getNpY();
	
	lastDrawPoint3D.setXYZ(0, 0, 0);
}
///////////////////////////////////////////////////////////////////
long CncControl::convertToDrawPointX(long val) {
///////////////////////////////////////////////////////////////////
	return drawPaneCoordSystem.getNpX() + val * drawPaneCoordSystem.getFactorX();
}
///////////////////////////////////////////////////////////////////
long CncControl::convertToDrawPointY(long val) {
///////////////////////////////////////////////////////////////////
	return drawPaneCoordSystem.getNpY() + val * drawPaneCoordSystem.getCorrectionY() * drawPaneCoordSystem.getFactorY();
}
///////////////////////////////////////////////////////////////////
void CncControl::drawText(wxPaintDC& dc, wxString text, wxPoint pt) {
///////////////////////////////////////////////////////////////////
	wxFont font(7, wxFONTFAMILY_SWISS, wxNORMAL, wxBOLD);
	dc.SetFont(font);
	dc.SetBackgroundMode(wxTRANSPARENT);
	dc.SetTextForeground(*wxWHITE);
	dc.SetTextBackground(*wxBLACK);
	dc.DrawText(text, pt);
}
///////////////////////////////////////////////////////////////////
void CncControl::drawOrigin(wxPaintDC& dc) {
///////////////////////////////////////////////////////////////////
	if ( drawControl == NULL )
		return;
	
	const int length = 80;
	wxPoint n = {(int)drawPaneCoordSystem.getNpX(), 			(int)drawPaneCoordSystem.getNpY()};
	wxPoint x = {(int)drawPaneCoordSystem.getNpX() + length, 	(int)drawPaneCoordSystem.getNpY()};
	wxPoint y = {(int)drawPaneCoordSystem.getNpX(),				(int)drawPaneCoordSystem.getNpY() + length };
	
	if ( drawPaneCoordSystem.getType() == CST_NULL_Y_IS_BOTTOM )
		y.y = (int)drawPaneCoordSystem.getNpY() - length;
 
 
	dc.SetPen(penHandler.getGridPenDefault());
	
	// line + arrow X axis
	int al = -5;
	int ab = 2;
	dc.DrawLine(n, x);
	dc.DrawLine(wxPoint(x.x + al, x.y - ab), x);
	dc.DrawLine(wxPoint(x.x + al, x.y + ab), x);
	
	// line + arrow Y axis
	if ( drawPaneCoordSystem.getType() == CST_NULL_Y_IS_BOTTOM )
		al *= -1;
		
	dc.DrawLine(n, y);
	dc.DrawLine(y, wxPoint(y.x - ab, y.y + al));
	dc.DrawLine(y, wxPoint(y.x + ab, y.y + al));
	
	// draw lables
	int offset = 9;
	if ( drawPaneCoordSystem.getType() == CST_NULL_Y_IS_TOP ) {
		drawText(dc, wxString("0"), {n.x - offset, 	n.y - 8});
		drawText(dc, wxString("X"), {x.x - offset, 	x.y - 12});
		drawText(dc, wxString("Y"), {y.x - offset, 	y.y - 8});
	} else {
		drawText(dc, wxString("0"), {n.x - offset, 	n.y});
		drawText(dc, wxString("X"), {x.x - offset, 	x.y + 2});
		drawText(dc, wxString("Y"), {y.x - offset, 	y.y});
	}
}
///////////////////////////////////////////////////////////////////
void CncControl::drawGrid(wxPaintDC& dc, double fact) {
///////////////////////////////////////////////////////////////////
	if ( drawControl == NULL )
		return;
	
	if ( showGrid == false )
		return;

	unsigned int division = 10 * fact;
	unsigned int w = drawControl->GetSize().GetWidth()  - drawPaneMargin;
	unsigned int h = drawControl->GetSize().GetHeight() - drawPaneMargin;
	unsigned int cnt = 0;
	
	// vertical
	for (unsigned int i=drawPaneMargin; i<w; i+=division, cnt++) {
		if (      cnt%10 == 0 ) dc.SetPen(penHandler.getGridPen100());
		else if ( cnt%5  == 0 )	dc.SetPen(penHandler.getGridPen050());
		else 					dc.SetPen(penHandler.getGridPen010());
		dc.DrawLine(wxPoint(i, drawPaneMargin), wxPoint(i, h));
	}
	
	// horizontal
	cnt = 0;
	unsigned int start = (drawPaneCoordSystem.getType() == CST_NULL_Y_IS_TOP) ? drawPaneMargin : 0;
	unsigned int end   = (drawPaneCoordSystem.getType() == CST_NULL_Y_IS_TOP) ? h : h - drawPaneMargin;
	
	for (unsigned int i=start; i<end; i+=division, cnt++ ) {
		if (      cnt%10 == 0 ) dc.SetPen(penHandler.getGridPen100());
		else if ( cnt%5  == 0 )	dc.SetPen(penHandler.getGridPen050());
		else 					dc.SetPen(penHandler.getGridPen010());
		
		if ( drawPaneCoordSystem.getType() == CST_NULL_Y_IS_TOP )
			dc.DrawLine(wxPoint(drawPaneMargin, i), wxPoint(w, i));
		else
			dc.DrawLine(wxPoint(drawPaneMargin, drawPaneCoordSystem.getNpY() - i), wxPoint(w, drawPaneCoordSystem.getNpY() - i));
	}
}
///////////////////////////////////////////////////////////////////
void CncControl::redrawDrawPane(double fact) {
///////////////////////////////////////////////////////////////////
	if ( drawControl == NULL )
		return;

	wxPaintDC dc(drawControl);

	// grid
	drawGrid(dc, fact);

	// curve
	if ( /*getCncConfig()->isOnlineUpdateDrawPane() ==*/ true ) {
		dc.SetPen(penHandler.getCurrentPen(true));
		
		for (DrawPoints::iterator it = drawPoints.begin(); it != drawPoints.end(); ++it) {
			dc.DrawLine((*it).lp, (*it).cp);
		}
	}
	
	// coordinate system
	drawOrigin(dc);
}
///////////////////////////////////////////////////////////////////
void CncControl::traceDrawPaneIntern(wxTextCtrl* ctl, std::ostream& os) {
///////////////////////////////////////////////////////////////////
	// trace curve
	double fx = 1;//cncConfig->getCalculationFactX();
	double fy = 1;//cncConfig->getCalculationFactY();

	if ( ctl != NULL ) {
		ctl->Update();
		ctl->Freeze();
	}
	
	if ( isInterrupted() ) 
		resetInterrupt();
		
	unsigned int i=0;
	for (DrawPoints::iterator it = drawPoints.begin(); it != drawPoints.end(); ++it) {
		PointPair pp = *it;
		os << "  " << wxString::Format(wxT("%05d"),i) << "\t" << PointPair::processFactor(PointPair::processOffset(pp, drawPaneMargin, -drawPaneMargin), fx, fy);
		
		if ( ++i%50 == 0 ) {
			if ( ctl != NULL ) {
				ctl->Thaw();
				ctl->Update();
				ctl->ShowPosition(ctl->GetLastPosition()-1);
				waitActive(0);
				ctl->Freeze();
			}
		}
		
		if ( isInterrupted() ) {
			os << "The trace is not complete because it was interrupted!" << std::endl;
			break;
		}
	}
	
	if ( ctl != NULL ) {
		if ( ctl->IsFrozen() )
			ctl->Thaw();
			
		ctl->Update();
	} 
}
///////////////////////////////////////////////////////////////////
unsigned int CncControl::traceDrawPane(wxTextCtrl* ctl) {
///////////////////////////////////////////////////////////////////
	if ( ctl == NULL ) {
		std::fstream pfs;
		pfs.open(CncFileNameService::getCncDrawPaneTraceFileName(), std::ios::out | std::ios::trunc);
		
		if ( pfs.is_open() == false ) {
			std::cerr << "CncControl::traceDrawPane: Open file failed: " << CncFileNameService::getCncDrawPaneTraceFileName() << std::endl;
			return 0;
		}
		
		std::clog << "::Trace draw points: Vector size: " << drawPoints.size() << std::endl;

		if ( drawPoints.size() > 0 ) {
			pfs       << "::Trace draw points: Vector size: " << drawPoints.size() << std::endl;
			std::clog << " Full trace will be written to: " << CncFileNameService::getCncDrawPaneTraceFileName() << std::endl;
			
			traceDrawPaneIntern(ctl, pfs);
		}
		
		pfs.flush();
		pfs.close();
		
	} else {
		traceDrawPaneIntern(ctl, std::clog);
	}
	
	return drawPoints.size();
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

	if ( alreadyRendered == true )
		return serialPort->processMoveXY(x1, y1, alreadyRendered, curPos);

	// render here
	int32_t x0 = 0, y0 = 0;
	int32_t dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int32_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int32_t err = dx + dy, e2; /* error value e_xy */

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
			{ err += dy; x0 += sx; } /* e_xy+e_x > 0 */
		if (e2 < dx) 
			{ err += dx; y0 += sy; } /* e_xy+e_y < 0 */
	}

	return true;
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

	if ( alreadyRendered == true )
		return serialPort->processMoveXYZ(x1, y1, z1, false, curPos);
		
	// render here
	int i, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
	int pointA[3], pointB[3];

	pointA[0] = 0; //x1; 
	pointA[1] = 0; //y1;
	pointA[2] = 0; //z1;
	
	pointB[0] = 0;
	pointB[1] = 0;
	pointB[2] = 0;
	
	dx = x1; //x2 - x1;
	dy = y1; //y2 - y1;
	dz = z1; //z2 - z1;
	
	x_inc = (dx < 0) ? -1 : 1;
	l = abs(dx);
	
	y_inc = (dy < 0) ? -1 : 1;
	m = abs(dy);
	
	z_inc = (dz < 0) ? -1 : 1;
	n = abs(dz);
	
	dx2 = l << 1;
	dy2 = m << 1;
	dz2 = n << 1;

	if ((l >= m) && (l >= n)) {
		err_1 = dy2 - l;
		err_2 = dz2 - l;
		for (i = 0; i < l; i++) {
			
			//output
			if ( serialPort->processMoveXYZ(pointB[0] - pointA[0], pointB[1] - pointA[1], pointB[2] - pointA[2], true, curPos) == false )
				return false;
		
			for (int j=0; j<3; j++ )
				pointB[j] = pointA[j];
			
			if (err_1 > 0) {
				pointA[1] += y_inc;
				err_1 -= dx2;
			}
			if (err_2 > 0) {
				pointA[2] += z_inc;
				err_2 -= dx2;
			}
			err_1 += dy2;
			err_2 += dz2;
			pointA[0] += x_inc;
		}
	} else if ((m >= l) && (m >= n)) {
		err_1 = dx2 - m;
		err_2 = dz2 - m;
		for (i = 0; i < m; i++) {
			
			//output
			if ( serialPort->processMoveXYZ(pointB[0] - pointA[0], pointB[1] - pointA[1], pointB[2] - pointA[2], true, curPos) == false )
				return false;
		
			for (int j=0; j<3; j++ )
				pointB[j] = pointA[j];
				
			if (err_1 > 0) {
				pointA[0] += x_inc;
				err_1 -= dy2;
			}
			if (err_2 > 0) {
				pointA[2] += z_inc;
				err_2 -= dy2;
			}
			err_1 += dx2;
			err_2 += dz2;
			pointA[1] += y_inc;
		}
	} else {
		err_1 = dy2 - n;
		err_2 = dx2 - n;
		for (i = 0; i < n; i++) {
			
			//output
			if ( serialPort->processMoveXYZ(pointB[0] - pointA[0], pointB[1] - pointA[1], pointB[2] - pointA[2], true, curPos) == false )
				return false;
		
			for (int j=0; j<3; j++ )
				pointB[j] = pointA[j];
				
			if (err_1 > 0) {
				pointA[1] += y_inc;
				err_1 -= dz2;
			}
			if (err_2 > 0) {
				pointA[0] += x_inc;
				err_2 -= dz2;
			}
			err_1 += dy2;
			err_2 += dx2;
			pointA[2] += z_inc;
		}
	}
	
	//output
	if ( serialPort->processMoveXYZ(pointB[0] - pointA[0], pointB[1] - pointA[1], pointB[2] - pointA[2], true, curPos) == false )
		return false;
		
	return true;
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
		toolState.setState(CncToolStateControl::neutral);
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
		if ( serialPort->getInfoOutput() )
			std::cout << " Switch tool on" << std::endl;
		
		if ( processSetter(PID_ROUTER_SWITCH, 1) ) {
			powerOn = true;
			setToolState();
		}
	}
}
///////////////////////////////////////////////////////////////////
void CncControl::switchToolOff() {
///////////////////////////////////////////////////////////////////
	if ( isInterrupted() )
		return;

	if ( powerOn == true ) {
		if ( serialPort->getInfoOutput() )
			std::cout << " Switch tool off" << std::endl;
		
		if ( processSetter(PID_ROUTER_SWITCH, 0) ) {
			powerOn = false;
			setToolState();
		}
	}
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
	
	if ( cncConfig->isModified() ) {
		wxVector<wxVector<wxVariant>> rows;
		if ( guiCtlSetup->staticCncConfig ) {
			cncConfig->getStaticValues(rows);
			guiCtlSetup->staticCncConfig->Freeze();
			guiCtlSetup->staticCncConfig->DeleteAllItems();
			for (wxVector<wxVector<wxVariant>>::iterator it = rows.begin(); it != rows.end(); ++it) {
				wxVector<wxVariant> row = *it;
				guiCtlSetup->staticCncConfig->AppendItem(row);
			}
			guiCtlSetup->staticCncConfig->Thaw();
		}
		
		rows.clear();
		if ( guiCtlSetup->dynamicCncConfig ) {
			cncConfig->getDynamicValues(rows);
			guiCtlSetup->dynamicCncConfig->Freeze();
			guiCtlSetup->dynamicCncConfig->DeleteAllItems();
			for (wxVector<wxVector<wxVariant>>::iterator it = rows.begin(); it != rows.end(); ++it) {
				wxVector<wxVariant> row = *it;
				guiCtlSetup->dynamicCncConfig->AppendItem(row);
			}
			guiCtlSetup->dynamicCncConfig->Thaw();
		}
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
	
	if ( guiCtlSetup->motorState )
		guiCtlSetup->motorState->Check(s);
}
///////////////////////////////////////////////////////////////////
void CncControl::setZSliderRange(unsigned int min, unsigned int max) {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	if ( guiCtlSetup->zSlider == NULL )
		return;

	guiCtlSetup->zSlider->SetRange(min, max);
}
///////////////////////////////////////////////////////////////////
void CncControl::setZSliderMaxValue(unsigned int max) {
///////////////////////////////////////////////////////////////////
	setZSliderRange(0, max);
}
///////////////////////////////////////////////////////////////////
void CncControl::setZSliderValue(unsigned int val) {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	if ( guiCtlSetup->zSlider == NULL )
		return;
		
	guiCtlSetup->zSlider->SetValue(guiCtlSetup->zSlider->GetMax() - val);
	guiCtlSetup->zSlider->SetLabelText(wxString() << val);
}
///////////////////////////////////////////////////////////////////
void CncControl::restoreZSlider() {
///////////////////////////////////////////////////////////////////
	double wpt = getCncConfig()->getWorkpieceThickness();
	double oft = getCncConfig()->getWorkpieceOffset();
	
	unsigned int max = (wpt + oft ) * getCncConfig()->getCalculationFactZ();
	setZSliderMaxValue(max * 100);
	updateZSlider();
}
///////////////////////////////////////////////////////////////////
void CncControl::updateZSlider() {
///////////////////////////////////////////////////////////////////
	if ( cncConfig->isOnlineUpdateCoordinates() )
		setZSliderValue(curPos.getZ() * 100);
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
	
	displayLimitState(guiCtlSetup->xMinLimit, limitStates.getXMinLimit());
	displayLimitState(guiCtlSetup->xMaxLimit, limitStates.getXMaxLimit());
	displayLimitState(guiCtlSetup->yMinLimit, limitStates.getYMinLimit());
	displayLimitState(guiCtlSetup->yMaxLimit, limitStates.getYMaxLimit());
	displayLimitState(guiCtlSetup->zMinLimit, limitStates.getZMinLimit());
	displayLimitState(guiCtlSetup->zMaxLimit, limitStates.getZMaxLimit());
	
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
	
	double maxSteps 	= -450.0; // mm
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
	
	return ( guiCtlSetup->controllerConfig != NULL );
}
///////////////////////////////////////////////////////////////////
void CncControl::clearControllerConfigControl() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	if ( hasControllerConfigControl() ) 
		guiCtlSetup->controllerConfig->DeleteAllItems();
}
///////////////////////////////////////////////////////////////////
void CncControl::appendPidKeyValueToControllerConfig(int pid, const char* key, const char* value) {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	if ( hasControllerConfigControl() ) {
		DcmItemList rows;

		DataControlModel::addNumKeyValueRow(rows, pid, key, value);
		guiCtlSetup->controllerConfig->Freeze();
		for (DcmItemList::iterator it = rows.begin(); it != rows.end(); ++it) {
			guiCtlSetup->controllerConfig->AppendItem(*it);
		}
		guiCtlSetup->controllerConfig->Thaw();
	}
}
///////////////////////////////////////////////////////////////////
bool CncControl::hasControllerPinControl() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	return ( guiCtlSetup->controllerPinReport != NULL );
}
///////////////////////////////////////////////////////////////////
bool CncControl::hasControllerErrorControl() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	return ( guiCtlSetup->controllerErrorInfo != NULL );
}
///////////////////////////////////////////////////////////////////
void CncControl::clearControllerPinControl() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	if ( hasControllerErrorControl() )
		guiCtlSetup->controllerPinReport->DeleteAllItems();
}
///////////////////////////////////////////////////////////////////
void CncControl::clearControllerErrorControl() {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	if ( hasControllerErrorControl() ) 
		guiCtlSetup->controllerErrorInfo->DeleteAllItems();
}
///////////////////////////////////////////////////////////////////
void CncControl::appendNumKeyValueToControllerErrorInfo(const char* desc, int pin, int type, int mode, int value) {
///////////////////////////////////////////////////////////////////
	wxASSERT(guiCtlSetup);
	
	if ( hasControllerErrorControl() ) {
		DcmItemList rows;

		DataControlModel::addPinReportRow(rows, desc, pin, type, mode, value);
		guiCtlSetup->controllerPinReport->Freeze();
		for (DcmItemList::iterator it = rows.begin(); it != rows.end(); ++it) {
			guiCtlSetup->controllerPinReport->AppendItem(*it);
		}
		guiCtlSetup->controllerPinReport->Thaw();
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
			guiCtlSetup->controllerErrorInfo->AppendItem(*it);
		}
		guiCtlSetup->controllerErrorInfo->Thaw();
	}
}
///////////////////////////////////////////////////////////////////
void CncControl::set3DData(bool append) {
///////////////////////////////////////////////////////////////////
	if ( guiCtlSetup == NULL || guiCtlSetup->drawPane3D == NULL )
		return;
		
	double fx = cncConfig->getMaxDimensionX() * cncConfig->getCalculationFactX();
	double fy = cncConfig->getMaxDimensionY() * cncConfig->getCalculationFactY();
	double fz = cncConfig->getMaxDimensionZ() * cncConfig->getCalculationFactZ();
	
	unsigned int offset = 0;
	DrawPaneData& dpd = guiCtlSetup->drawPane3D->getDataVector();
	if ( append == false ) {
		// clear the 3D vector
		guiCtlSetup->drawPane3D->clearDataVector();
	} else {
		// determine offset for append operation
		if ( dpd.size() != 0 )
			offset = dpd.size();
	}
	
	DoublePointPair3D pp;
	for (DrawPoints3D::iterator it = drawPoints3D.begin() + offset; it != drawPoints3D.end(); ++it) {
		PositionInfo3D pi3d = *it;
		
		if ( pi3d.zAxisDown == true ) {
			pp.setDefaultToDrawColour();
			pp.setDefaultToLineStyle();
		} else {
			pp.setDrawColour(*wxYELLOW);
			pp.setLineStyle(wxDOT);
		}
		dpd.push_back(pp.set(pi3d.lp.getX() / fx, pi3d.lp.getY() / fy, pi3d.lp.getZ() / fz,
							 pi3d.cp.getX() / fx, pi3d.cp.getY() / fy, pi3d.cp.getZ() / fz)); 
	}
	
	if ( append == true ) {
		//cnc::trc.logInfo(wxString::Format("%d, %d", (int)dpd.size(), (int)drawPoints3D.size()));
		guiCtlSetup->drawPane3D->Refresh();
	}
}
///////////////////////////////////////////////////////////////////
void CncControl::setMotionMonitorMode(const MontionMoinorMode& mmm) {
///////////////////////////////////////////////////////////////////
	motionMonitorMode = mmm; 
	
	if ( guiCtlSetup == NULL || guiCtlSetup->drawPane3D == NULL )
		return;
		
	if ( motionMonitorMode == MMM_3D )
		guiCtlSetup->drawPane3D->viewTop();
}
///////////////////////////////////////////////////////////////////
void CncControl::sendIdleMessage() {
///////////////////////////////////////////////////////////////////
	if ( getSerial() == NULL )
		return;
		
	if ( getSerial()->isCommandActive() == true )
		return;
	
	getSerial()->processIdle();
}