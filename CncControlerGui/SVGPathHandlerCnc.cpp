#include <iostream>
#include <cstdlib>
#include <math.h>
#include <wx/string.h>
#include <wx/dataview.h>
#include "SerialPort.h"
#include "CncControl.h"
#include "FileParser.h"
#include "SvgUnitCalculator.h"
#include "CncToolCorrection.h"
#include "SVGPathHandlerCnc.h"

//////////////////////////////////////////////////////////////////
SVGPathHandlerCnc::SVGPathHandlerCnc(CncControl* cnc) 
: PathHandlerBase()
, unit(px)
, cncControl(cnc)
, processMode(SvgPhController)
, toolRadius(0.0)
, zAxisDown(false)
, initialized(false)
, debugState(false)
, width(0.0)
, height(0.0)
, viewBox("")
{
//////////////////////////////////////////////////////////////////
	wxASSERT(cncControl);
	
	toolRadius = CncConfig::getGlobalCncConfig()->getRouterBitDiameter();
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
void SVGPathHandlerCnc::debugCurrentPosition() {
//////////////////////////////////////////////////////////////////
	if ( debugState == false )
		return;
	
	wxString pos(wxString::Format("%ld, %ld, %ld [steps]", cncControl->getCurPos().getY(), cncControl->getCurPos().getX(), cncControl->getCurPos().getZ() ));
	appendDebugValueDetail("Current Pos(x,y,z)", pos);

	pos.assign(wxString::Format("%.3lf, %.3lf, %.3lf [mm]", cncControl->getCurPos().getX() * CncConfig::getGlobalCncConfig()->getDisplayFactX(), 
	                                                        cncControl->getCurPos().getY() * CncConfig::getGlobalCncConfig()->getDisplayFactY(),
	                                                        cncControl->getCurPos().getZ() * CncConfig::getGlobalCncConfig()->getDisplayFactZ()));
	appendDebugValueDetail("Current Pos(x,y,z)", pos);
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
CncWorkingParameters& SVGPathHandlerCnc::getCncWorkingParameters() {
//////////////////////////////////////////////////////////////////
	return currentCncParameters;
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::setCncWorkingParameters(CncWorkingParameters& cwp) {
//////////////////////////////////////////////////////////////////
	currentCncParameters = cwp;
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::setProcessMode(SvgPhProcessMode pm) {
//////////////////////////////////////////////////////////////////
	processMode = pm;
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::setMaxDimensions(SVGUnit u, double w, double h) {
//////////////////////////////////////////////////////////////////
	unit 		= u;
	initialized = SvgUnitCalculator::isUnitValid(u);

	width  = w;
	height = h;
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::setViewBox(const wxString& vb) {
//////////////////////////////////////////////////////////////////
	viewBox = vb;
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
	if ( spoolCurrentPathWrapper(cncControl->getDurationCounter() == 1) == false )
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
	
	cncControl->getSerial()->closePath();
	cncControl->getSerial()->closeDuration();
	
	// write the original path only one time
	if ( cncControl->getDurationCount() >= cncControl->getDurationCounter() )
		cncControl->getSerial()->writeOrigPath(origPathInfo); 
	
	return true;
}
//////////////////////////////////////////////////////////////////
inline bool SVGPathHandlerCnc::spoolCurrentPathWrapper(bool firstRun) {
//////////////////////////////////////////////////////////////////
	bool ret = spoolCurrentPath(firstRun);
	return ret;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::spoolCurrentPath(bool firstRun) {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL("spoolCurrentPath");
	
	unsigned int cnt = 0;
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

		if ( std::distance(pathListMgr.begin(), it) == 0 ) {
			TRACE_POSITIONS("spoolCurrentPath std::distance(pathList.begin(), it)");
			firstListEntry = true;

			if ( firstRun == true ) {
				// this time the cnc controller isn't moved before
				// so the local positions have to be alinged
				currentPos.setX(cncControl->getCurPosMetric().getX());
				currentPos.setY(cncControl->getCurPosMetric().getY());
				startPos.setX(cncControl->getStartPosMetric().getX());
				startPos.setY(cncControl->getStartPosMetric().getY());
			}

			moveX = pathListMgr.getStartPos().x - currentPos.getX();
			moveY = pathListMgr.getStartPos().y - currentPos.getY();

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
			if ( isZAxisUp() == true )
				cncControl->getSerial()->beginSubPath(moveX, moveY);
				
			if ( moveLinearXY(moveX, moveY, cpe.alreadyRendered) == false )
				return false;
		}
		
		// pure svg handling
		if ( std::distance(pathListMgr.begin(), it) == 0 ) {
			// this have to be defently done after the fist move above
			// otherwise this move will be also recorded by the svg out file and
			// serial->beginPath has a step to much
			double sx = pathListMgr.getStartPos().x;
			double sy = pathListMgr.getStartPos().y;
			cncControl->getSerial()->beginPath(sx, sy);
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
	
	//svg output handling
	cncControl->getSerial()->beginSVG(getSVGUnit(), getW(), getH(), getViewBox());
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
	cncControl->getSerial()->closeSVG(xyMax);
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
	double curZPos = cncControl->getCurPos().getZ() * CncConfig::getGlobalCncConfig()->getDisplayFactZ(); // we need it as mm
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
		cncControl->changeWorkSpeedXY(CncSpeedRapid);
	} else {
		std::cerr << "CncControl::moveUpZ() error: " << moveZ << ", " << curZPos << ", " << dist << std::endl;
	}

	return ret;
}
///////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::moveDownZ() {
///////////////////////////////////////////////////////////////////
	double curZPos = cncControl->getCurPos().getZ() * CncConfig::getGlobalCncConfig()->getDisplayFactZ(); // we need it as mm
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
	
	bool ret = cncControl->moveRelMetricZ(moveZ);
	if ( ret ) {
		zAxisDown = true;
		cncControl->changeWorkSpeedXY(CncSpeedWork);
	}
	
	return ret;
}