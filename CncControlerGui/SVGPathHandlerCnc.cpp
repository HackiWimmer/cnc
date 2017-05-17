#include <iostream>
#include <cstdlib>
#include <math.h>
#include <wx/string.h>
#include <wx/dataview.h>
#include "SerialPort.h"
#include "CncControl.h"
#include "SvgUnitCalculator.h"
#include "CncToolCorrection.h"
#include "CncPathReverser.h"
#include "SVGPathHandlerCnc.h"

//////////////////////////////////////////////////////////////////
SVGPathHandlerCnc::SVGPathHandlerCnc(CncControl* cnc) 
: SVGPathHandlerBase()
, unit(px)
, cncControl(cnc)
, processMode(SvgPhController)
, toolRadius(0.0)
, initialized(false)
, debugState(false)
, width(0.0)
, height(0.0)
, viewBox("")
, debuggerControlDetail(NULL)
{
//////////////////////////////////////////////////////////////////
	wxASSERT( cncControl && cncControl->getCncConfig() );
	
	toolRadius = cncControl->getCncConfig()->getRouterBitDiameter();
	toolRadius /= 2.0; 
	
	if ( toolRadius < 0 )
		toolRadius *= (-1.0);
		
	if ( toolRadius > 0 )
		cnc::trc.logInfoMessage(wxString::Format("Tool path correction will be activated: Radius: %.3lf", toolRadius));
	
	curveLibResolution = (float)(cncControl->getCncConfig()->getCurveLibResolution());
}
//////////////////////////////////////////////////////////////////
SVGPathHandlerCnc::~SVGPathHandlerCnc() {
//////////////////////////////////////////////////////////////////
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
	
	wxString pos("");
	pos << cncControl->getCurPos().getX();
	pos << ", ";
	pos << cncControl->getCurPos().getY();
	pos << ", ";
	pos << cncControl->getCurPos().getZ();
	pos << " [steps]";
	appendDebugValueDetail("Current Pos(x,y,z)", pos);

	pos.clear();
	pos << cncControl->getCurPos().getX() * cncControl->getCncConfig()->getDisplayFactX();
	pos << ", ";
	pos << cncControl->getCurPos().getY() * cncControl->getCncConfig()->getDisplayFactY();
	pos << ", ";
	pos << cncControl->getCurPos().getZ() * cncControl->getCncConfig()->getDisplayFactZ();
	pos << " [mm]";
	appendDebugValueDetail("Current Pos(x,y,z)", pos);
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::appendDebugValueDetail(const char* key, wxVariant value) {
//////////////////////////////////////////////////////////////////
	if ( debugState == false )
		return;
		
	if ( debuggerControlDetail == NULL )
		return;
		
	DcmRow row;
	row.push_back(wxString(key));
	row.push_back(value.GetString());
	debuggerControlDetail->AppendItem(row);
	
	int itemCount = debuggerControlDetail->GetItemCount();
	debuggerControlDetail->EnsureVisible(debuggerControlDetail->RowToItem(itemCount - 1));
	debuggerControlDetail->EnsureVisible(debuggerControlDetail->RowToItem(0));
	debuggerControlDetail->Update();
	debuggerControlDetail->Refresh();
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
	return cncControl->moveLinearMetricXY(x, y, alreadyRendered);
}
//////////////////////////////////////////////////////////////////
inline void SVGPathHandlerCnc::appendDebugValueDetail(CncPathListEntry& cpe) {
//////////////////////////////////////////////////////////////////
	if ( debugState == false )
		return;

	appendDebugValueDetail((wxString("Point ") << pathList.list.size()), cpe.getPointAsString());
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
	traceFunctionCall("initNextPath");
	newPath 			= true;
	origPathInfo	 	= sopi;
	
	pathList.reset();
	
	// Z depth management
	wxASSERT( cncControl && cncControl->getCncConfig() );
	CncConfig* cc = cncControl->getCncConfig();
	double zDepth = -currentCncParameters.getCurrentZDepth();
	
	if ( currentCncParameters.isCurrentZDepthAbs() == true ) {
		zDepth = cc->getWorkpieceThickness() - currentCncParameters.getCurrentZDepth();
		
		if ( zDepth > cc->getWorkpieceThickness() )
			zDepth = cc->getWorkpieceThickness();
	} else {
		
		if ( zDepth < 0.0 ) {
			cout << "xx: " <<zDepth<<endl;
			zDepth = 0.0;
			
		}
	}

	if ( cnc::dblCompare(cc->setCurrentZDepth(zDepth), zDepth) == false ) {
		if ( cc->getWorkpieceThickness() != 0 ) {
			std::cerr << "SVGPathHandlerCnc::initNextPath: error while setting Z depth: ";
			std::cerr << currentCncParameters.getCurrentZDepthMode() << ( currentCncParameters.isCurrentZDepthAbs() ? zDepth : -zDepth);
			std::cerr << ", Wpt: " << cc->getWorkpieceThickness() << std::endl;
		}
		//return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::finishCurrentPath() {
//////////////////////////////////////////////////////////////////
	traceFunctionCall("finsihCurrentPath");
	
	// if the tool radius > 0 the path have to be tool corrected
	if ( toolRadius > 0 ) {
		CncToolCorrection tc(toolRadius, currentCncParameters.getCorrectionType());
		if ( tc.process(pathList.list) == false ) 
			return false;
			
		pathList.isCorrected = (tc.getType() != CncCT_Center);
		tracePositions("Corrected before");
		
		// correct the start posistion
		if ( pathList.firstPath == 	true ) {
			CncPathList::iterator it = pathList.list.begin();
			pathList.startPos = {(*it).move.x, (*it).move.y};
		} else {
			CncPathList::iterator it = pathList.list.begin();
			pathList.startPos -= pathList.firstMove - (*it).move;
		}
		
		tracePositions("Corrected after");
	}
	
	// Reverse Path
	if ( currentCncParameters.getReverseFlag() == true ) {
		CncPathReverser pr;
		pathList.list = pr.reversePath(pathList.list);
		
		// correct the start posistion
		if ( pathList.firstPath == 	true ) {
			CncPathList::iterator it = pathList.list.begin();
			pathList.startPos = {(*it).move.x, (*it).move.y};
		} else {
			CncPathList::iterator it = pathList.list.begin();
			pathList.startPos -= pathList.firstMove - (*it).move;
		}	
	}

	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::runCurrentPath() {
//////////////////////////////////////////////////////////////////
	traceFunctionCall("runCurrentPath");

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
	traceFunctionCall("repeatCurrentPath");

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
	traceFunctionCall("beginCurrentPath");
	cncControl->initNextDuration();
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::closeCurrentPath() {
//////////////////////////////////////////////////////////////////
	traceFunctionCall("closeCurrentPath");
	
	if ( cncControl->isLastDuration() ) {
		if ( cncControl->isZAxisDown() == true ) {
			if ( cncControl->moveUpZ() == false )
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
	traceFunctionCall("spoolCurrentPath");
	
	unsigned int cnt = 0;
	for (CncPathList::iterator it = pathList.list.begin(); it != pathList.list.end(); ++it) {
		
		CncPathListEntry cpe = *it;
		cnt++;
		
		if ( cpe.zAxisDown == false && cncControl->isZAxisDown() == true ) {
			if ( cncControl->moveUpZ() == false )
					return false;
					/* todo
			if ( cncControl->getDurationCounter() == 1 ) {
				// If this will be done for further durations the z axis moves to top before moving to the depth of the next duration
				if ( cncControl->moveUpZ() == false )
					return false;
			} else {
				cncControl->simulateZAxisUp();
			}*/

		} else if ( cpe.zAxisDown == true && cncControl->isZAxisUp() == true ) {
			if ( cncControl->moveDownZ() == false )
				return false;
		}
		
		double moveX = cpe.move.x;
		double moveY = cpe.move.y;
		bool firstListEntry = false;

		if ( std::distance(pathList.list.begin(), it) == 0 ) {
			tracePositions("spoolCurrentPath std::distance(pathList.list.begin(), it)");
			firstListEntry = true;

			if ( firstRun == true ) {
				// this time the cnc controller isn't moved before
				// so the local positions have to be alinged
				currentPos.setX(cncControl->getCurPosMetric().getX());
				currentPos.setY(cncControl->getCurPosMetric().getY());
				startPos.setX(cncControl->getStartPosMetric().getX());
				startPos.setY(cncControl->getStartPosMetric().getY());
			}

			moveX = pathList.startPos.x - currentPos.getX();
			moveY = pathList.startPos.y - currentPos.getY();

			traceFirstMove(moveX, moveY);
		}
		
		currentPos.incX(moveX);
		currentPos.incY(moveY);
		
		if ( firstListEntry == true ) {
			if ( moveLinearXY(moveX, 0, cpe.alreadyRendered) == false )
				return false;

			if ( moveLinearXY(0, moveY, cpe.alreadyRendered) == false )
				return false;
		} else {
			if ( cncControl->isZAxisUp() == true )
				cncControl->getSerial()->beginSubPath(moveX, moveY);
				
			if ( moveLinearXY(moveX, moveY, cpe.alreadyRendered) == false )
				return false;
		}
		
		// pure svg handling
		if ( std::distance(pathList.list.begin(), it) == 0 ) {
			// this have to be defently done after the fist move above
			// otherwise this move will be also recorded by the svg out file and
			// serial->beginPath has a step to much
			double sx = pathList.startPos.x;
			double sy = pathList.startPos.y;
			cncControl->getSerial()->beginPath(sx, sy);
		}
		//todo evaluateDebugState
		//MessageBoxA(0,"","",0);
	}

	return true;
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::prepareWork() {
//////////////////////////////////////////////////////////////////
	traceFunctionCall("prepareWork");
	SVGPathHandlerBase::prepareWork();
	
	currentPos.resetWatermarks();
	startPos.resetWatermarks();
	
	//svg output handling
	cncControl->getSerial()->beginSVG(getSVGUnit(), getW(), getH(), getViewBox());
	// controller handling
	if ( cncControl->isZAxisDown() == true )
		cncControl->moveUpZ();
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::finishWork() {
//////////////////////////////////////////////////////////////////
	traceFunctionCall("finishWork");
	SVGPathHandlerBase::finishWork();
	
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
	cncControl->simulateZAxisUp();
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::simulateZAxisDown() {
//////////////////////////////////////////////////////////////////
	cncControl->simulateZAxisDown();
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::isZAxisUp() {
//////////////////////////////////////////////////////////////////
	return cncControl->isZAxisUp();
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::isZAxisDown() {
//////////////////////////////////////////////////////////////////
	return cncControl->isZAxisDown();
}