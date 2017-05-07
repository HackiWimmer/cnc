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
#include "SVGPathHandler.h"

//////////////////////////////////////////////////////////////////
SVGPathHandlerCnc::SVGPathHandlerCnc(CncControl* cnc) 
: unit(px)
, cncControl(cnc)
, processMode(SvgPhController)
, toolRadius(0.0)
, curveLibResolution(0.09)
, firstPath(true)
, newPath(false)
, initialized(false)
, debugState(false)
, width(0.0)
, height(0.0)
, viewBox("")
, startPos({0.0, 0.0, 0.0})
, currentPos({0.0, 0.0, 0.0})
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
	
	//preallocate memory
	pathList.list.reserve(1000 * 1000);
}
//////////////////////////////////////////////////////////////////
SVGPathHandlerCnc::~SVGPathHandlerCnc() {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::traceFirstMove(double moveX, double moveY) {
//////////////////////////////////////////////////////////////////
	if ( true )
		return;
	
	std::cout << "traceFirstMove(...)" << std::endl;
	std::cout << " Move Abs: " << currentPos.getX() + moveX << "," << currentPos.getY() + moveY << std::endl;
	std::cout << " Move Rel: " << moveX << "," << moveY << std::endl;
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::tracePositions(const char* prefix) {
//////////////////////////////////////////////////////////////////
	if ( true )
		return;
	
	std::cout << "tracePositions(" << prefix << ")" << std::endl;
	
	if ( pathList.list.size() > 0 ) {
		CncPathList::iterator it = pathList.list.begin();
		std::cout << " PL.First.Pos  : " << (*it).move.x << "," << (*it).move.y << std::endl;
		it = pathList.list.end()-1;
		std::cout << " PL.Last.Pos   : " << (*it).move.x << "," << (*it).move.y << std::endl;
	} else {
		std::cout << " PL.First.Pos  : empty" << std::endl;
		std::cout << " PL.Last.Pos   : empty" << std::endl;
	}
	std::cout << " PL.firstPath  : " << pathList.firstPath << std::endl;
	std::cout << " PL.Start Pos  : " << pathList.startPos.x << "," << pathList.startPos.y << std::endl;
	std::cout << " PL.First Move : " << pathList.firstMove.x << "," << pathList.firstMove.y << std::endl;
	std::cout << " CNC Pos       : " << cncControl->getCurPosMetric().getX() << "," << cncControl->getCurPosMetric().getY() << std::endl;

	std::cout << " StartPos      : " << startPos.getX()   << "," << startPos.getY()   << std::endl;
	std::cout << " CurrentPos    : " << currentPos.getX() << "," << currentPos.getY() << std::endl;
}
//////////////////////////////////////////////////////////////////
inline void SVGPathHandlerCnc::traceCurrentPosition() {
//////////////////////////////////////////////////////////////////
	if ( true ) {
		std::clog << "CurrentPos: " << currentPos.getX() << "," << currentPos.getY() << std::endl;
	}
}
//////////////////////////////////////////////////////////////////
inline void SVGPathHandlerCnc::traceCurveLibPoint(const char* prefix, SVGCurveLib::PointGeneric<>& p) {
//////////////////////////////////////////////////////////////////
	if ( true ) {
		std::clog << prefix << ":" << p.x << "," << p.y << std::endl;
	}
}
//////////////////////////////////////////////////////////////////
inline void SVGPathHandlerCnc::traceFunctionCall(const char* fn) {
//////////////////////////////////////////////////////////////////
	if ( false ) {
		std::cout << "SVGPathHandlerCnc::" << fn << std::endl;
	}
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::debugProcess(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	std::cout << " pcmd: " << c << "(" << count << ") ";
	
	for (unsigned int i=0; i<count; i++) {
		std::cout << "[" << i << "]=" << values[i] << "\t";
	}	
	std::cout << "cp(" << currentPos.getX() << ", " << currentPos.getY() << ")" << std::endl;
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
bool SVGPathHandlerCnc::processLinearMove(bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	double newPosAbsX = currentPos.getX();
	double newPosAbsY = currentPos.getY();
	
	// first perform the transformations . . .
	currentSvgTransformMatrix.transform(newPosAbsX, newPosAbsY);

	//  . . . then convert the input unit to mm . . .newPosAbsX = 
	newPosAbsX = SvgUnitCalculator::convertReferenceUnit2MM(newPosAbsX);
	newPosAbsY = SvgUnitCalculator::convertReferenceUnit2MM(newPosAbsY);

	// . . . furthermore determine the relative move parameters . . .
	double moveX = newPosAbsX - pathList.prevPosAbs.x;
	double moveY = newPosAbsY - pathList.prevPosAbs.y;
	
	// . . . and last but not least store the move command.
	CncPathListEntry cpe;
	cpe.zAxisDown 			= cncControl->isZAxisDown();
	cpe.move.x				= moveX;
	cpe.move.y				= moveY;
	cpe.alreadyRendered 	= alreadyRendered;
	
	// correct the start position
	if ( pathList.list.size() == 0 ) {
		pathList.startPos.x  = newPosAbsX;
		pathList.startPos.y  = newPosAbsY;
		pathList.firstMove.x = moveX;
		pathList.firstMove.y = moveY;
	}
	
	// store position
	pathList.prevPosAbs.x = newPosAbsX;
	pathList.prevPosAbs.y = newPosAbsY;
	pathList.list.push_back(cpe);
	appendDebugValueDetail(cpe);
	
	return true;
}
//////////////////////////////////////////////////////////////////
inline void SVGPathHandlerCnc::appendDebugValueDetail(CncPathListEntry& cpe) {
//////////////////////////////////////////////////////////////////
	if ( debugState == false )
		return;

	appendDebugValueDetail((wxString("Point ") << pathList.list.size()), cpe.getPointAsString());
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::initNextPath(const SvgOriginalPathInfo& sopi) {
//////////////////////////////////////////////////////////////////
	traceFunctionCall("initNextPath");
	newPath 			= true;
	origPathInfo	 	= sopi;
	
	pathList.isCorrected = false;
	pathList.list.clear();
	
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
bool SVGPathHandlerCnc::process(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( initialized == false ) {
		std::cerr << "SVGPathHandlerCnc not initialized "<< std::endl;
		return false;
	}
	
	// only to debug
	//debugProcess(c, count, values);

	bool ret = false;
	switch ( c ) {
		case 'm':
		case 'M': 	lastControlPoint.reset(); 
					ret = processMove(c, count, values); 
					break;
		case 'l':
		case 'L': 	lastControlPoint.reset(); 
					ret = processLine(c, count, values); 
					break;
		case 'h':
		case 'H': 	lastControlPoint.reset(); 
					ret = processHLine(c, count, values); 
					break;
		case 'v':
		case 'V': 	lastControlPoint.reset(); 
					ret = processVLine(c, count, values); 
					break;
		case 'z':
		case 'Z': 	lastControlPoint.reset(); 
					ret = processClose(c, count, values); 
					break;
		case 'a':
		case 'A': 	lastControlPoint.reset(); 
					ret = processARC(c, count, values); 
					break;
		case 'c':
		case 'C': 	ret = processCubicBezier(c, count, values); 
					break;
		case 's':
		case 'S': 	ret = processCubicBezierSmooth(c, count, values); 
					break;
		case 'q':
		case 'Q': 	ret = processQuadraticBezier(c, count, values); 
					break;
		case 't':
		case 'T': 	ret = processQuadraticBezierSmooth(c, count, values); 
					break;
		default: 
			std::cerr << "SVGPathHandlerCnc: Unkown command: " << c << std::endl;
			ret = false;
	}
	
	return ret;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::processMove(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 2 ) {
		std::cerr << "SVGPathHandlerCnc::processMove: Invalid command count: " << count << std::endl;
		return false;
	}
	appendDebugValueDetail("Move",c);
	
	bool ret = false;
	
	double moveX = 0, moveY = 0;
	if ( newPath == true ) {
		
		startPos.setX(values[0]);
		startPos.setY(values[1]);
				
		if ( firstPath == true ) {
			pathList.firstPath = true;

			//the first move is always absolute!
			moveX = startPos.getX();
			moveY = startPos.getY();
			firstPath = false;
		} else {
			pathList.firstPath = false;

			//the first move is always absolute 
			//but the current position have to be considered!
			moveX = values[0] - currentPos.getX();
			moveY = values[1] - currentPos.getY();
		}
		pathList.startPos  = {startPos.getX(), startPos.getY()};
		pathList.firstMove = {moveX, moveY};
		
		tracePositions("processMove");
		traceFirstMove(moveX, moveY);
		
		currentPos.setX(startPos.getX());
		currentPos.setY(startPos.getY());

		cncControl->simulateZAxisUp();
		ret = processLinearMove(false);
		cncControl->simulateZAxisDown();
		
		newPath = false;
		
	} else {
		if ( c == 'M' ) {
			moveX = values[0] - currentPos.getX();
			moveY = values[1] - currentPos.getY();
		} else {
			moveX = values[0];
			moveY = values[1];
		}

		currentPos.incX(moveX);
		currentPos.incY(moveY);
		startPos.setX(currentPos.getX());
		startPos.setY(currentPos.getY());
		
		ret = true;
		if ( moveX != 0.0 || moveY != 0.0 ) {
			cncControl->simulateZAxisUp();
			ret = processLinearMove(false);
			cncControl->simulateZAxisDown();
		}
	}
	
	return ret;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::processClose(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 0 ) {
		std::cerr << "SVGPathHandlerCnc::processClose: Invalid command count: " << count << std::endl;
		return false;
	}
	appendDebugValueDetail("Close",c);
	
	switch ( c ) {
		case 'z':
		case 'Z':
			currentPos.setX(startPos.getX());
			currentPos.setY(startPos.getY());
			break;
		default: ; // Do nothing, already checked before
	}
	bool ret = processLinearMove(false);

	return ret;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::processLine(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 2 ) {
		std::cerr << "SVGPathHandlerCnc::processLine: Invalid command count: " << count << std::endl;
		return false;
	}
	appendDebugValueDetail("Line",c);

	switch ( c ) {
		case 'l':
			currentPos.incX(values[0]);
			currentPos.incY(values[1]);
			break;
		case 'L':
			currentPos.setX(values[0]);
			currentPos.setY(values[1]);
			break;
		default: ; // Do nothing, already checked before
	}
	
	return processLinearMove(false);
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::processHLine(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 1 ) {
		std::cerr << "SVGPathHandlerCnc::processHLine: Invalid command count: " << count << std::endl;
		return false;
	}
	// Redirect to lineto command
	// L - H is the same as l -h
	char cmd = c + 'L' - 'H';
	values[1] = 0.0;
	
	return processLine(cmd, count + 1, values);
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::processVLine(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 1 ) {
		std::cerr << "SVGPathHandlerCnc::processVLine: Invalid command count: " << count << std::endl;
		return false;
	}
	// Redirect to lineto command
	// V - L is the same as v - l
	char cmd = c + 'V' - 'L';
	values[1] = 0.0;
	
	return processLine(cmd, count + 1, values);
}
//////////////////////////////////////////////////////////////////
inline bool SVGPathHandlerCnc::processCurveLibPoint(SVGCurveLib::PointGeneric<> p) {
//////////////////////////////////////////////////////////////////
	currentPos.setX(p.x);
	currentPos.setY(p.y);
	
	if ( processLinearMove(true) == false )
		return false;
		
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::processARC(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 7 ) {
		std::cerr << "SVGPathHandlerCnc::processARC: Invalid command count: " << count << std::endl;
		return false;
	}
	appendDebugValueDetail("EllipticalARC",c);
	
	// p0 (startPos) for curve lib is always absolute
	SVGCurveLib::PointGeneric<> p0 = {(float)(currentPos.getX()), (float)(currentPos.getY())}; 
	
	// p1 (endPos) for curve lib is always absolute
	SVGCurveLib::PointGeneric<> p1 = p0;
	switch ( c ) {
		case 'a': 	p1 = {(float)(values[5] + p0.x), (float)(values[6] + p0.y)}; 
					break;
					
		case 'A':	p1 = {(float)values[5], (float)values[6]};
					break;
	}
	
	if ( CncSvgCurveLib::useCncSvgCurveLib == true ) {
		
		CncSvgCurveLib::LinearArcCurve lac(p0, values[0], values[1], values[2], (bool)values[3], (bool)values[4], p1);
		SVGCurveLib::LinearCurve lc = SVGCurveLib::LinearCurve(CncSvgCurveLib::getDefaultResolution(), CncSvgCurveLib::AP.pointOnCurveFunc);
		CurveInfo ci = CncSvgCurveLib::getCurveInfo(lc);
		appendDebugValueDetail("CurveLibResAuto", 	ci.increment);
		appendDebugValueDetail("CurveLength", 		ci.length);
		
		// First process the curve
		for( float t=0; t<1; t+=ci.increment ) {
			if ( processCurveLibPoint(lc.PointOnLinearCurve(t)) == false )
				return false;
		}
		// Last stretch to the endpoint
		if ( processCurveLibPoint(lc.PointOnLinearCurve(1.0f)) == false )
			return false;

	} else {
		// First process the curve
		appendDebugValueDetail("CurceLibRes", curveLibResolution);
		for (float t=0; t<1; t+=curveLibResolution ) {
			if ( processCurveLibPoint(SVGCurveLib::PointOnEllipticalArc(p0, values[0], values[1], values[2], (bool)values[3], (bool)values[4], p1, t).point) == false )
				return false;
		}
		// Last stretch to the endpoint
		if ( processCurveLibPoint(SVGCurveLib::PointOnEllipticalArc(p0, values[0], values[1], values[2], (bool)values[3], (bool)values[4], p1, 1.0f).point) == false )
			return false;
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::processQuadraticBezier(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 4 ) {
		std::cerr << "SVGPathHandlerCnc::processQuadraticBezier: Invalid command count: " << count << std::endl;
		return false;
	}
	appendDebugValueDetail("QuadraticBezier",c);
	
	// p0 (startPos) for curve lib is always absolute
	SVGCurveLib::PointGeneric<> p0 = {(float)(currentPos.getX()), (float)(currentPos.getY())}; 
	
	// p1 - p2 for curve lib is always absolute
	SVGCurveLib::PointGeneric<> p1 = p0; 
	SVGCurveLib::PointGeneric<> p2 = p0;

	switch ( c ) {
		case 'q': 	p1 = {(float)(values[0] + p0.x), (float)(values[1] + p0.y)}; 
					p2 = {(float)(values[2] + p0.x), (float)(values[3] + p0.y)}; 
					break;
		case 'Q':	p1 = {(float)values[0], (float)values[1]};
					p2 = {(float)values[2], (float)values[3]};
					break;
	}
	
	if ( CncSvgCurveLib::useCncSvgCurveLib == true ) {
		
		CncSvgCurveLib::LinearQuadraticBezierCurve lqbc(p0, p1, p2);
		SVGCurveLib::LinearCurve lc = SVGCurveLib::LinearCurve(CncSvgCurveLib::getDefaultResolution(), CncSvgCurveLib::QP.pointOnCurveFunc);
		CurveInfo ci = CncSvgCurveLib::getCurveInfo(lc);
		appendDebugValueDetail("CurveLibResAuto", 	ci.increment);
		appendDebugValueDetail("CurveLength", 		ci.length);
		
		// First process the curve
		for( float t=0; t<1; t+=ci.increment ) {
			if ( processCurveLibPoint(lc.PointOnLinearCurve(t)) == false )
				return false;
		}
		// Last stretch to the endpoint
		if ( processCurveLibPoint(lc.PointOnLinearCurve(1.0f)) == false )
			return false;
			
	} else {
		// First process the curve
		appendDebugValueDetail("CurveLibRes", curveLibResolution);
		for (float t=0; t<1; t+=curveLibResolution ) {
			if ( processCurveLibPoint(SVGCurveLib::PointOnQuadraticBezierCurve(p0, p1, p2, t)) == false )
				return false;
		}
		// Last stretch to the endpoint
		if ( processCurveLibPoint(SVGCurveLib::PointOnQuadraticBezierCurve(p0, p1, p2, 1.0f)) == false )
			return false;
	}
	
	// Store the last control point
	SVGCurveLib::PointGeneric<> cp{(float)currentPos.getX(), (float)currentPos.getY()};
	lastControlPoint.setLastQuadraticBezierControlPoint(cp, p1);
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::processCubicBezier(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 6 ) {
		std::cerr << "SVGPathHandlerCnc::processCubicBezier: Invalid command count: " << count << std::endl;
		return false;
	}
	appendDebugValueDetail("CubicBezier",c);
	
	// p0 (startPos) for curve lib is always absolute
	SVGCurveLib::PointGeneric<> p0 = {(float)(currentPos.getX()), (float)(currentPos.getY())}; 
	
	// p1 - p3 for curve lib is always absolute
	SVGCurveLib::PointGeneric<> p1 = p0; 
	SVGCurveLib::PointGeneric<> p2 = p0;
	SVGCurveLib::PointGeneric<> p3 = p0;
	
	switch ( c ) {
		case 'c': 	p1 = {(float)(values[0] + p0.x), (float)(values[1] + p0.y)}; 
					p2 = {(float)(values[2] + p0.x), (float)(values[3] + p0.y)}; 
					p3 = {(float)(values[4] + p0.x), (float)(values[5] + p0.y)}; 
					break;
		case 'C':	p1 = {(float)values[0], (float)values[1]};
					p2 = {(float)values[2], (float)values[3]};
					p3 = {(float)values[4], (float)values[5]};
					break;
	}
	
	if ( CncSvgCurveLib::useCncSvgCurveLib == true ) {
		
		CncSvgCurveLib::LinearCubicBezierCurve lcbc(p0, p1, p2, p3);
		SVGCurveLib::LinearCurve lc = SVGCurveLib::LinearCurve(CncSvgCurveLib::getDefaultResolution(), CncSvgCurveLib::CP.pointOnCurveFunc);
		CurveInfo ci = CncSvgCurveLib::getCurveInfo(lc);
		appendDebugValueDetail("CurveLibResAuto", 	ci.increment);
		appendDebugValueDetail("CurveLength", 		ci.length);
		
		// First process the curve
		for( float t=0; t<1; t+=ci.increment ) {
			if ( processCurveLibPoint(lc.PointOnLinearCurve(t)) == false )
				return false;
		}
		// Last stretch to the endpoint
		if ( processCurveLibPoint(lc.PointOnLinearCurve(1.0f)) == false )
			return false;
			
	} else {
		// First process the curve
		appendDebugValueDetail("CurveLibRes", curveLibResolution);
		for (float t=0; t<1; t+=curveLibResolution ) {
			if ( processCurveLibPoint(SVGCurveLib::PointOnCubicBezierCurve(p0, p1, p2, p3, t)) == false )
				return false;
		}
		// Last stretch to the endpoint
		if ( processCurveLibPoint(SVGCurveLib::PointOnCubicBezierCurve(p0, p1, p2, p3, 1.0f)) == false )
			return false;
	}
	
	// Store the last control point
	SVGCurveLib::PointGeneric<> cp{(float)currentPos.getX(), (float)currentPos.getY()};
	lastControlPoint.setLastCubicBezierControlPoint(cp, p2);
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::processQuadraticBezierSmooth(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 2 ) {
		std::cerr << "SVGPathHandlerCnc::processQuadraticBezierSmooth: Invalid command count: " << count << std::endl;
		return false;
	}
	appendDebugValueDetail("QuadraticBezierSmooth",c);
	
	// p0 (startPos) for curve lib is always absolute
	SVGCurveLib::PointGeneric<> p0 = {(float)(currentPos.getX()), (float)(currentPos.getY())};
	values[3] = values[1];
	values[2] = values[0];
	values[1] = lastControlPoint.getLastQuadraticBezierControlPoint(p0).y; // todo abs or rel???
	values[0] = lastControlPoint.getLastQuadraticBezierControlPoint(p0).x; // todo abs or rel???
	
	switch ( c ) {
		case 't': c = 'q'; break;
		case 'T': c = 'Q'; break;
	}
	
	return process(c, 4, values);
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::processCubicBezierSmooth(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 4 ) {
		std::cerr << "SVGPathHandlerCnc::processCubicBezierSmooth: Invalid command count: " << count << std::endl;
		return false;
	}
	appendDebugValueDetail("CubicBezierSmooth",c);
	
	// p0 (startPos) for curve lib is always absolute
	SVGCurveLib::PointGeneric<> p0 = {(float)(currentPos.getX()), (float)(currentPos.getY())};
	values[5] = values[3];
	values[4] = values[2];
	values[3] = values[1];
	values[2] = values[0];
	values[1] = lastControlPoint.getLastCubicBezierControlPoint(p0).y; // todo abs or rel ???
	values[0] = lastControlPoint.getLastCubicBezierControlPoint(p0).x; // todo abs or rel ???
	
	switch ( c ) {
		case 's': c = 'C'; break;
		case 'S': c = 'C'; break;
	}
	
	return process(c, 6, values);
}
