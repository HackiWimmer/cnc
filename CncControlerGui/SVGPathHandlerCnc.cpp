#include <iostream>
#include <cstdlib>
#include <math.h>
#include <wx/string.h>
#include <wx/dataview.h>
#include "SerialPort.h"
#include "CncControl.h"
#include "CncPathListRunner.h"
#include "FileParser.h"
#include "SVGPathHandlerCnc.h"

//////////////////////////////////////////////////////////////////
SVGPathHandlerCnc::SVGPathHandlerCnc(CncControl* cnc) 
: SVGPathHandlerBase	()
, CncPathListRunner		(cnc)
, cncControl			(cnc)
, initialized			(false)
, debugState			(false)
{
//////////////////////////////////////////////////////////////////
	wxASSERT(cncControl);
	
	// add the file parser to the runner setup
	// the rest is already set by CncPathListRunner(cnc)
	CncPathListRunner::Setup& setup = getSetup();
	setup.fileParser = fileParser;
}
//////////////////////////////////////////////////////////////////
SVGPathHandlerCnc::~SVGPathHandlerCnc() {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::logMeasurementStart() {
//////////////////////////////////////////////////////////////////
	CncPathListRunner::logMeasurementStart();
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::logMeasurementEnd() {
//////////////////////////////////////////////////////////////////
	CncPathListRunner::logMeasurementEnd();
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::initNextClientId(long id) {
//////////////////////////////////////////////////////////////////
	processClientId(id);
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::appendDebugValueDetail(const char* key, wxVariant value) {
//////////////////////////////////////////////////////////////////
	if ( debugState == false )
		return;

	if ( fileParser == NULL )
		return;
		
	DcmRow row;
	row.push_back(wxString(key));
	row.push_back(value.GetString());
	fileParser->appendDebugValueDetail(row);
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::setSvgRootNode(const SVGRootNode& srn) {
//////////////////////////////////////////////////////////////////
	SVGPathHandlerBase::setSvgRootNode(srn);
	changeInputUnit(srn.getOutputUnit());
	initialized = true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::moveLinearZ(double z) {
//////////////////////////////////////////////////////////////////
	return cncControl->moveRelMetricZ(z);
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::moveLinearXY(double x, double y, bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	return cncControl->moveRelLinearMetricXY(x, y, alreadyRendered);
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::moveLinearXYZ(double x, double y, double z, bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	return cncControl->moveRelLinearMetricXYZ(x, y, z, alreadyRendered);
}
//////////////////////////////////////////////////////////////////
inline void SVGPathHandlerCnc::appendDebugValueDetail(const CncPathListEntry& cpe) {
//////////////////////////////////////////////////////////////////
	if ( debugState == false )
		return;

	wxString value;
	appendDebugValueDetail((wxString("Point ") << pathListMgr.getPathListSize()), cpe.traceEntryToString(value));
}
//////////////////////////////////////////////////////////////////
inline void SVGPathHandlerCnc::appendDebugValueDetail(const CncCurveLib::ParameterSet& ps) {
//////////////////////////////////////////////////////////////////
	if ( debugState == false )
		return;
	
	appendDebugValueDetail("Curve length", 		wxString::Format("%.5lf", 	ps.RI.curveLength));
	appendDebugValueDetail("Render samples",	wxString::Format("%u", 		ps.RI.samples));
	appendDebugValueDetail("Render steps",		wxString::Format("%u", 		ps.RI.steps));
	appendDebugValueDetail("Render increment",	wxString::Format("%.5lf", 	ps.RI.increment));
	appendDebugValueDetail("Render resolution",	wxString::Format("%.5lf", 	ps.RI.resolution));
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::initNextPath() {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL("initNextPath");
	
	nextPath = true;
	if ( currentCncContext.isGuidePath() )	pathListMgr.initNextGuidePath(CncPathListManager::GuideType::HELP_PATH);
	else									pathListMgr.initNextCncPath();
	
	// Z depth management
	wxASSERT(cncControl);
	Serial::Trigger::NextPath tr;
	cncControl->processTrigger(tr);
	
	CncConfig* cc = CncConfig::getGlobalCncConfig();
	double zDepth = -currentCncContext.getCurrentZDepth();
	
	if ( currentCncContext.isCurrentZDepthAbs() == true ) {
		zDepth = cc->getWorkpieceThickness() - currentCncContext.getCurrentZDepth();
		
		if ( zDepth > cc->getWorkpieceThickness() )
			zDepth = cc->getWorkpieceThickness();
	} else {
		
		if ( zDepth < 0.0 ) {
			//std::cout << "xx: " << zDepth << std::endl;
			zDepth = 0.0;
		}
	}
	
	if ( cnc::dblCompare(cc->setCurrentZDepth(zDepth), zDepth) == false ) {
		if ( cc->getWorkpieceThickness() != 0 ) {
			std::cerr << "SVGPathHandlerCnc::initNextPath: error while setting Z depth: ";
			std::cerr << currentCncContext.getCurrentZDepthMode() << ( currentCncContext.isCurrentZDepthAbs() ? zDepth : -zDepth);
			std::cerr << ", Wpt: " << cc->getWorkpieceThickness() << std::endl;
		} else {
			std::clog << "Warning: ZDept operation [" << currentCncContext.getCurrentZDepthMode();
			std::clog << ( currentCncContext.isCurrentZDepthAbs() ? zDepth : -zDepth);
			std::clog << "] ignored because the current workpiece thickness is 0.0 mm" << std::endl;
		}
		//return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::finishCurrentPath() {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL("finsihCurrentPath");
	
	PathHandlerBase::finishCurrentPath();
	
	// reverse path
	if ( currentCncContext.getReverseFlag() == true ) {
		if ( pathListMgr.reversePath() == false )
			return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::runCurrentPath() {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL("runCurrentPath");

	bool ret = true;
	
	cncControl->switchToolOn();

	while ( cncControl->hasNextDuration() ) {
		
		ret = repeatCurrentPath();
		if ( ret == false )
			break;
	}
	
	//cncControl->switchToolOff();
	
	// path is finished now, reset the duration counter
	cncControl->resetDurationCounter();

	return ret;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::repeatCurrentPath() {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL("repeatCurrentPath");

	if ( beginCurrentPath() == false )
		return false;
	
	// spoolCurrentPath
	if ( onPhysicallyExecute(pathListMgr) == false )
		return false;
	
	if ( closeCurrentPath() == false )
		return false;

	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::beginCurrentPath() {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL("beginCurrentPath");
	cncControl->initNextDuration();

	if ( physicallyMoveZAxisDown() == false )
		return false;

	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::closeCurrentPath() {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL("closeCurrentPath");

	if ( cncControl->isLastDuration() ) {
		if ( physicallyMoveZAxisUp() == false )
			return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::prepareWork() {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL("prepareWork");
	PathHandlerBase::prepareWork();
	
	currentPos.resetWatermarks();
	startPos.resetWatermarks();

	if ( physicallyMoveZAxisUp() == false ) {
		std::cerr << "SVGPathHandlerCnc::prepareWork(): processZAxisUp() failed!" << std::endl;
	}
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::finishWork() {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL("finishWork");
	PathHandlerBase::finishWork();
	
	// secure Z axis is up
	physicallyMoveZAxisUp();
	
	// controller handling
	cncControl->switchToolOff();
	cncControl->changeCurrentFeedSpeedXYZ_MM_MIN(THE_CONFIG->getDefaultRapidSpeed_MM_MIN(), CncSpeedRapid);
	cncControl->moveXYToZeroPos();
	
	//svg output handling
	CncDoublePosition::Watermarks xyMax;

	//currentPos.getWatermarks(xyMax); // sometimes not in mm
	xyMax = cncControl->getWaterMarksMetric();

	if ( physicallyMoveZAxisUp() == false ) {
		std::cerr << "SVGPathHandlerCnc::prepareWork(): processZAxisUp() failed!" << std::endl;
	}
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::isZAxisUp() {
//////////////////////////////////////////////////////////////////
	const double curZDist = CncConfig::getGlobalCncConfig()->getCurZDistance();
	const double curZPos  = cncControl->getCurAppPosMetric().getZ(); 

	return cnc::dblCompare(curZPos, curZDist);
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::isZAxisDown() {
//////////////////////////////////////////////////////////////////
	return !isZAxisUp();
}
///////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::physicallyMoveZAxisUp() {
///////////////////////////////////////////////////////////////////
	const double curZDist = CncConfig::getGlobalCncConfig()->getCurZDistance();
	const double curZPos  = cncControl->getCurAppPosMetric().getZ();
	double moveZ = 0.0;
	
	if ( cnc::dblCompare(curZPos, curZDist) == false )
		moveZ = curZDist - curZPos;
	
	if ( (curZPos + moveZ) > CncConfig::getGlobalCncConfig()->getMaxZDistance() ) {
		std::cerr << "CncControl::moveUpZ error:" 	<< std::endl;
		std::cerr << "Z(abs): " << curZPos + moveZ 	<< std::endl;
		std::cerr << "Z(cur): " << curZPos 			<< std::endl;
		std::cerr << "Z(mv):  " << moveZ 			<< std::endl;
		std::cerr << "Z(max): " << CncConfig::getGlobalCncConfig()->getMaxZDistance() << std::endl;
		return false;
	}

	// Due to the fact that an SVG path could be repeated the Z axis activities
	// have to be organized outside the path list. This has to be done by
	// SVGPathHandlerCnc::begin/closeCurrentPath(). Only then will it be possible
	// to manage the z axis for svg files during each repetation.
	const double prevSpeed 		= cncControl->getConfiguredFeedSpeed_MM_MIN();
	const CncSpeedMode prevMode = cncControl->getConfiguredSpeedMode();

	long prevClientId = cncControl->getClientId();
	cncControl->setClientId(CLIENT_ID.SVG_Z_UP);

	if ( cncControl->changeCurrentFeedSpeedXYZ_MM_MIN(THE_CONFIG->getDefaultWorkSpeed_MM_MIN(), CncSpeedWork) == false )
		return false;

	if ( moveLinearZ(moveZ) == false )
		return false;

	cncControl->setClientId(prevClientId);
	return cncControl->changeCurrentFeedSpeedXYZ_MM_MIN(prevSpeed, prevMode);
}
///////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::physicallyMoveZAxisDown() {
///////////////////////////////////////////////////////////////////
	const double curZPos = cncControl->getCurAppPosMetric().getZ();
	const double newZPos = CncConfig::getGlobalCncConfig()->getDurationPositionAbs(cncControl->getDurationCounter());
	double moveZ         = (curZPos - newZPos) * (-1);

	if ( false ) {
		std::clog << "moveDownZ:  " << std::endl;
		std::clog << " zAxisDown  " << isZAxisDown() 					<< std::endl;
		std::clog << " curZPos:   " << curZPos 							<< std::endl;
		std::clog << " newZPos:   " << newZPos 							<< std::endl;
		std::clog << " moveZ:     "	<< moveZ 							<< std::endl;
		std::clog << " duration:  "	<< cncControl->getDurationCounter() << std::endl;
	}

	// Due to the fact that an SVG path could be repeated the Z axis activities
	// have to be organized outside the path list. This has to be done by
	// SVGPathHandlerCnc::begin/closeCurrentPath(). Only then will it be possible
	// to manage the z axis for svg files during each repetation.
	const double prevSpeed 		= cncControl->getConfiguredFeedSpeed_MM_MIN();
	const CncSpeedMode prevMode = cncControl->getConfiguredSpeedMode();

	long prevClientId = cncControl->getClientId();
	cncControl->setClientId(CLIENT_ID.SVG_Z_DOWN);

	if ( cncControl->changeCurrentFeedSpeedXYZ_MM_MIN(THE_CONFIG->getDefaultWorkSpeed_MM_MIN(), CncSpeedWork) == false )
		return false;

	if ( moveLinearZ(moveZ) == false )
		return false;
	
	bool ret = cncControl->changeCurrentFeedSpeedXYZ_MM_MIN(prevSpeed, prevMode);
	cncControl->setClientId(prevClientId);

	return ret;
}

