#include <iostream>
#include <cstdlib>
#include <math.h>
#include <wx/string.h>
#include <wx/dataview.h>
#include "SerialPort.h"
#include "CncControl.h"
#include "FileParser.h"
#include "CncToolCorrection.h"
#include "SVGPathHandlerCnc.h"

//////////////////////////////////////////////////////////////////
SVGPathHandlerCnc::SVGPathHandlerCnc(CncControl* cnc) 
: SVGPathHandlerBase()
, cncControl(cnc)
, toolRadius(0.0)
, zAxisDown(false)
, initialized(false)
, debugState(false)
, currentCncParameters()
, svgRootNode()
{
//////////////////////////////////////////////////////////////////
	wxASSERT(cncControl);
	
	toolRadius = CncConfig::getGlobalCncConfig()->getToolDiameter();
	toolRadius /= 2.0; 
	
	if ( toolRadius < 0 )
		toolRadius *= (-1.0);
		
	if ( toolRadius > 0 )
		cnc::trc.logInfoMessage(wxString::Format("Tool path correction will be activated: Radius: %.3lf", toolRadius));
}
//////////////////////////////////////////////////////////////////
SVGPathHandlerCnc::~SVGPathHandlerCnc() {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::logMeasurementStart() {
//////////////////////////////////////////////////////////////////
	wxASSERT(cncControl);
	cncControl->startSerialMeasurement();
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::logMeasurementEnd() {
//////////////////////////////////////////////////////////////////
	wxASSERT(cncControl);
	cncControl->stopSerialMeasurement();
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::initNextClientId(long id) {
//////////////////////////////////////////////////////////////////
	wxASSERT(cncControl);
	cncControl->setClientId(id);
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::isInitialized() {
//////////////////////////////////////////////////////////////////
	return initialized;
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
SvgCncParameters& SVGPathHandlerCnc::getSvgCncParameters() {
//////////////////////////////////////////////////////////////////
	return currentCncParameters;
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::setCncWorkingParameters(SvgCncParameters& cwp) {
//////////////////////////////////////////////////////////////////
	currentCncParameters = cwp;
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::setSvgRootNode(const SVGRootNode& srn) {
//////////////////////////////////////////////////////////////////
	svgRootNode = srn;
	changeInputUnit(srn.getInputUnit());

	initialized = true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::moveLinearXY(double x, double y, bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	// unit is always converted to mm before
	return cncControl->moveRelLinearMetricXY(x, y, alreadyRendered);
}
//////////////////////////////////////////////////////////////////
inline void SVGPathHandlerCnc::appendDebugValueDetail(const CncPathListEntry& cpe) {
//////////////////////////////////////////////////////////////////
	if ( debugState == false )
		return;

	appendDebugValueDetail((wxString("Point ") << pathListMgr.getPathListSize()), cpe.getPointAsString());
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
	
	/*
	#warning
	std::cout << std::endl;
	std::cout << "Curve length       : "<< 	wxString::Format("%.5lf", 	ps.RI.curveLength) 	<< std::endl;
	std::cout << "Render samples     : " <<	wxString::Format("%u", 		ps.RI.samples) 		<< std::endl;
	std::cout << "Render steps       : " <<	wxString::Format("%u", 		ps.RI.steps) 		<< std::endl;
	std::cout << "Render increment   : " <<	wxString::Format("%.5lf", 	ps.RI.increment)	<< std::endl;
	std::cout << "Render resolution  : " <<	wxString::Format("%.5lf", 	ps.RI.resolution)	<< std::endl;
	*/
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::initNextPath() {
//////////////////////////////////////////////////////////////////
	SvgOriginalPathInfo sopi;
	return initNextPath(sopi);
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::initNextPath(const SvgOriginalPathInfo& sopi) {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL("initNextPath");
	newPath 			= true;
	origPathInfo	 	= sopi;
	
	PathHandlerBase::initNextPath();
	
	// Z depth management
	wxASSERT(cncControl);
	CncConfig* cc = CncConfig::getGlobalCncConfig();
	double zDepth = -currentCncParameters.getCurrentZDepth();
	
	if ( currentCncParameters.isCurrentZDepthAbs() == true ) {
		zDepth = cc->getWorkpieceThickness() - currentCncParameters.getCurrentZDepth();
		
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
			std::cerr << currentCncParameters.getCurrentZDepthMode() << ( currentCncParameters.isCurrentZDepthAbs() ? zDepth : -zDepth);
			std::cerr << ", Wpt: " << cc->getWorkpieceThickness() << std::endl;
		} else {
			std::clog << "Warning: ZDept operation [" << currentCncParameters.getCurrentZDepthMode();
			std::clog << ( currentCncParameters.isCurrentZDepthAbs() ? zDepth : -zDepth);
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
	
	// if the tool radius > 0 the path have to be tool corrected
	if ( toolRadius > 0 ) {
		CncToolCorrection tc(toolRadius, currentCncParameters.getCorrectionType());
		if ( tc.process(pathListMgr.getPathListtoModify()) == false ) 
			return false;
			
		pathListMgr.setCorretedFlag(tc.getType() != CncCT_Center);
		TRACE_POSITIONS("Corrected before");
		
		// correct the start posistion
		/*
		 * todo
		if ( pathListMgr.getFirstPathFlag() == true ) {
			CncPathList::iterator it = pathListMgr.begin();
			pathListMgr.setStartPos({(*it).move.x, (*it).move.y});
		} else {
			CncPathList::iterator it = pathListMgr.begin();
			pathListMgr.incStartPos(pathListMgr.getFirstMove() - (*it).move);
		}*/
		
		TRACE_POSITIONS("Corrected after");
	}
	
	// reverse path
	if ( currentCncParameters.getReverseFlag() == true ) {
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
	
	// spoolCurrentPath(false) --> means this isn't the first move
	if ( spoolCurrentPath(cncControl->getDurationCounter() == 1) == false )
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
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::closeCurrentPath() {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL("closeCurrentPath");
	
	if ( cncControl->isLastDuration() ) {
		if ( isZAxisDown() == true ) {
			if ( moveUpZ() == false )
				return false;
		}
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::spoolCurrentPath(bool firstRun) {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL("spoolCurrentPath");
	// firstRun = (cncControl->getDurationCounter() == 1)
	
	bool reverseYAxis = CncConfig::getGlobalCncConfig()->getSvgReverseYAxisFlag();
	
	unsigned int cnt = 0;
	// over one stored svg path <path M...../>
	for (CncPathList::iterator it = pathListMgr.begin(); it != pathListMgr.end(); ++it) {

		if ( fileParser != NULL )
			fileParser->evaluateDebugState();
		
		CncPathListEntry cpe = *it;
		cnt++;
		
		if ( cpe.zAxisDown == false && isZAxisDown() == true ) {
			if ( moveUpZ() == false )
					return false;
			/* todo
			if ( cncControl->getDurationCounter() == 1 ) {
				// If this will be done for further durations the z axis moves to top before moving to the depth of the next duration
				if ( cncControl->moveUpZ() == false )
					return false;
			} else {
				cncControl->simulateZAxisUp();
			}*/

		} else if ( cpe.zAxisDown == true && isZAxisUp() == true ) {
			if ( moveDownZ() == false )
				return false;
		}
		
		double moveX = cpe.move.x;
		double moveY = cpe.move.y;
		bool firstListEntry = false;
		
		if ( reverseYAxis == true )
			moveY *= -1;

		// first path entry, always absolute
		if ( std::distance(pathListMgr.begin(), it) == 0 ) {
			TRACE_POSITIONS("spoolCurrentPath std::distance(pathList.begin(), it)");
			firstListEntry = true;

			if ( firstRun == true ) {
				// this time the cnc controller isn't moved before
				// so the local positions have to be alinged
				currentPos.setX(cncControl->getCurAppPosMetric().getX());
				currentPos.setY(cncControl->getCurAppPosMetric().getY());
				startPos.setX(cncControl->getStartPosMetric().getX());
				startPos.setY(cncControl->getStartPosMetric().getY());
			}
			// reconstuct the first move, this overrides moveX and moveY
			// pathListMgr.getStartPos() is always absolute as well as currentPos
			moveX = pathListMgr.getStartPos().x - currentPos.getX();
			
			if ( reverseYAxis == false )	moveY = +pathListMgr.getStartPos().y - currentPos.getY();
			else 							moveY = -pathListMgr.getStartPos().y - currentPos.getY();

			TRACE_FIRST_MOVE(moveX, moveY);
		}
		
		currentPos.incX(moveX);
		currentPos.incY(moveY);
		
		if ( firstListEntry == true ) {
			if ( moveLinearXY(moveX, 0, cpe.alreadyRendered) == false )
				return false;

			if ( moveLinearXY(0, moveY, cpe.alreadyRendered) == false )
				return false;
		} else {
			if ( moveLinearXY(moveX, moveY, cpe.alreadyRendered) == false )
				return false;
		}
		
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
	
	// controller handling
	if ( isZAxisDown() == true )
		moveUpZ();
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::finishWork() {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL("finishWork");
	PathHandlerBase::finishWork();
	
	cncControl->switchToolOff();
	
	// controller handling
	cncControl->moveXYToZeroPos();
	
	//svg output handling
	CncDoublePosition::Watermarks xyMax;
	//currentPos.getWatermarks(xyMax); // sometimes not in mm
	xyMax = cncControl->getWaterMarksMetric();
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::simulateZAxisUp() {
//////////////////////////////////////////////////////////////////
	zAxisDown = false;
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::simulateZAxisDown() {
//////////////////////////////////////////////////////////////////
	zAxisDown = true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::isZAxisUp() {
//////////////////////////////////////////////////////////////////
	return !zAxisDown;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::isZAxisDown() {
//////////////////////////////////////////////////////////////////
	return zAxisDown;
}
///////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::moveUpZ() {
///////////////////////////////////////////////////////////////////
	double dist = CncConfig::getGlobalCncConfig()->getCurZDistance();
	double curZPos = cncControl->getCurAppPos().getZ() * CncConfig::getGlobalCncConfig()->getDisplayFactZ(); // we need it as mm
	double moveZ = 0.0;
	
	if ( curZPos != dist ) {
		moveZ = dist - curZPos;
		// correct round deviations
		if ( moveZ < 0.00001 )
			moveZ = 0.0;
	}
	
	if ( (curZPos + moveZ) > CncConfig::getGlobalCncConfig()->getMaxZDistance() ) {
		std::cerr << "CncControl::moveUpZ error:" << std::endl;
		std::cerr << "Z(abs): " << curZPos + moveZ << std::endl;
		std::cerr << "Z(cur): " << curZPos << std::endl;
		std::cerr << "Z(mv):  " << moveZ << std::endl;
		std::cerr << "Z(max): " << CncConfig::getGlobalCncConfig()->getMaxZDistance() << std::endl;
		return false;
	}
	
	bool ret = cncControl->moveRelMetricZ(moveZ);
	if ( ret ) {
		zAxisDown = false;
		cncControl->changeSpeedToDefaultSpeed_MM_MIN(CncSpeedRapid);
	} else {
		std::cerr << "CncControl::moveUpZ() error: " << moveZ << ", " << curZPos << ", " << dist << std::endl;
	}

	return ret;
}
///////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::moveDownZ() {
///////////////////////////////////////////////////////////////////
	double curZPos = cncControl->getCurAppPos().getZ() * CncConfig::getGlobalCncConfig()->getDisplayFactZ(); // we need it as mm
	double newZPos = CncConfig::getGlobalCncConfig()->getDurationPositionAbs(cncControl->getDurationCounter());
	double moveZ   = (curZPos - newZPos) * (-1);

	if ( false ) {
		std::clog << "moveDownZ:  " << std::endl;
		std::clog << " zAxisDown  " << zAxisDown << std::endl;
		std::clog << " curZPos:   " << curZPos << std::endl;
		std::clog << " newZPos:   " << newZPos << std::endl;
		std::clog << " moveZ:     "	<< moveZ << std::endl;
		std::clog << " duration:  "	<< cncControl->getDurationCounter() << std::endl;
	}
	
	cncControl->changeSpeedToDefaultSpeed_MM_MIN(CncSpeedWork);
	
	bool ret = cncControl->moveRelMetricZ(moveZ);
	if ( ret ) {
		zAxisDown = true;
	}
	
	return ret;
}

