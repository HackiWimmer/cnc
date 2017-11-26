#include <iostream>
#include "SVGCurveLib.h"
#include "CncConfig.h"
#include "FileParser.h"
#include "PathHandlerBase.h"

//////////////////////////////////////////////////////////////////
PathHandlerBase::PathHandlerBase() 
: firstPath(true)
, newPath(false)
, startPos({0.0, 0.0, 0.0})
, currentPos({0.0, 0.0, 0.0})
, totalLength(0.0)
{
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
PathHandlerBase::~PathHandlerBase() {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::setCurveLibResolution(float res) { 
//////////////////////////////////////////////////////////////////
	CncConfig::setCurveLibIncrement(res); 
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::setPathList(const CncPathListManager& newPathList) {
	pathListMgr.reset();
	pathListMgr = newPathList;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::isInitialized() {
//////////////////////////////////////////////////////////////////
	return true;
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::appendDebugValueDetail(const char* key, wxVariant value) {
//////////////////////////////////////////////////////////////////
	// currently nothing to do
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::appendDebugValueDetail(const CncPathListEntry& cpe) {
//////////////////////////////////////////////////////////////////
	// currently nothing to do
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::traceCurveLibPoint(const char* prefix, SVGCurveLib::PointGeneric<>& p) {
//////////////////////////////////////////////////////////////////
	if ( true ) {
		std::clog << prefix << ":" << p.x << "," << p.y << std::endl;
	}
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::traceFunctionCall(const char* fn) {
//////////////////////////////////////////////////////////////////
	std::cout << "SVGPathHandlerBase::" << fn << std::endl;
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::traceFirstMove(double moveX, double moveY) {
//////////////////////////////////////////////////////////////////
	std::cout << "traceFirstMove(...)" << std::endl;
	std::cout << " Move Abs: " << currentPos.getX() + moveX << "," << currentPos.getY() + moveY << std::endl;
	std::cout << " Move Rel: " << moveX << "," << moveY << std::endl;
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::tracePositions(const char* prefix) {
//////////////////////////////////////////////////////////////////
	std::cout << "tracePositions(" << prefix << ")" << std::endl;
	
	if ( pathListMgr.getPathListSize() > 0 ) {
		CncPathList::iterator it = pathListMgr.begin();
		std::cout << " PL.First.Pos  : " << (*it).move.x << "," << (*it).move.y << std::endl;
		it = pathListMgr.end()-1;
		std::cout << " PL.Last.Pos   : " << (*it).move.x << "," << (*it).move.y << std::endl;
	} else {
		std::cout << " PL.First.Pos  : empty" << std::endl;
		std::cout << " PL.Last.Pos   : empty" << std::endl;
	}
	std::cout << " PL.firstPath  : " << pathListMgr.getFirstPathFlag() << std::endl;
	std::cout << " PL.Start Pos  : " << pathListMgr.getStartPos().x << "," << pathListMgr.getStartPos().y << std::endl;

	std::cout << " StartPos      : " << startPos.getX()   << "," << startPos.getY()   << std::endl;
	std::cout << " CurrentPos    : " << currentPos.getX() << "," << currentPos.getY() << std::endl;
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::traceCurrentPosition() {
//////////////////////////////////////////////////////////////////
	std::clog << "CurrentPos: " << currentPos.getX() << "," << currentPos.getY() << std::endl;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processMove(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 2 ) {
		std::cerr << "SVGPathHandlerCnc::processMove: Invalid command count: " << count << std::endl;
		return false;
	}
	appendDebugValueDetail("Move",c);
	
	bool ret = false;
	
	if ( newPath == true ) {
		
		startPos.setX(values[0]);
		startPos.setY(values[1]);
		
		// first of all give the path list manager a reference from where we are coming 
		pathListMgr.setReferencePos({currentPos.getX(), currentPos.getY()});
				
		if ( firstPath == true ) {
			pathListMgr.setFirstPathFlag(true);
			firstPath = false;
		} else {
			pathListMgr.setFirstPathFlag(false);
		}

		//the first move is always absolute!
		currentPos.setX(startPos.getX());
		currentPos.setY(startPos.getY());

		simulateZAxisUp();
		ret = processLinearMove(false);
		simulateZAxisDown();
		
		newPath = false;
		
	} else {
		double moveX = 0, moveY = 0;
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
		if ( cnc::dblCompareNull(moveX) == false || cnc::dblCompareNull(moveY) == false ) {
			simulateZAxisUp();
			ret = processLinearMove(false);
			simulateZAxisDown();
		}
	}
	
	return ret;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processClose(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 0 ) {
		std::cerr << "SVGPathHandlerBase::processClose: Invalid command count: " << count << std::endl;
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
bool PathHandlerBase::processLine(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 2 ) {
		std::cerr << "SVGPathHandlerBase::processLine: Invalid command count: " << count << std::endl;
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
bool PathHandlerBase::processHLine(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 1 ) {
		std::cerr << "SVGPathHandlerBase::processHLine: Invalid command count: " << count << std::endl;
		return false;
	}
	// Redirect to lineto command
	// L - H is the same as l -h
	char cmd = c + 'L' - 'H';
	values[1] = 0.0;
	
	return processLine(cmd, count + 1, values);
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processVLine(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 1 ) {
		std::cerr << "SVGPathHandlerBase::processVLine: Invalid command count: " << count << std::endl;
		return false;
	}
	// Redirect to lineto command
	// V - L is the same as v - l
	char cmd = c + 'V' - 'L';
	values[1] = 0.0;
	
	return processLine(cmd, count + 1, values);
}
//////////////////////////////////////////////////////////////////
inline bool PathHandlerBase::processCurveLibPoint(SVGCurveLib::PointGeneric<> p) {
//////////////////////////////////////////////////////////////////
	currentPos.setX(p.x);
	currentPos.setY(p.y);
	
	if ( processLinearMove(true) == false )
		return false;
		
	return true;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processARC(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 7 ) {
		std::cerr << "SVGPathHandlerBase::processARC: Invalid command count: " << count << std::endl;
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
		appendDebugValueDetail("CurveLibRes", CncConfig::getCurveLibIncrement());
		for (float t=0; t<1; t+=CncConfig::getCurveLibIncrement() ) {
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
bool PathHandlerBase::processQuadraticBezier(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 4 ) {
		std::cerr << "SVGPathHandlerBase::processQuadraticBezier: Invalid command count: " << count << std::endl;
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
		appendDebugValueDetail("CurveLibRes", CncConfig::getCurveLibIncrement());
		for (float t=0; t<1; t+=CncConfig::getCurveLibIncrement() ) {
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
bool PathHandlerBase::processCubicBezier(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 6 ) {
		std::cerr << "SVGPathHandlerBase::processCubicBezier: Invalid command count: " << count << std::endl;
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
		appendDebugValueDetail("CurveLibRes", CncConfig::getCurveLibIncrement());
		for (float t=0; t<1; t+=CncConfig::getCurveLibIncrement() ) {
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
bool PathHandlerBase::processQuadraticBezierSmooth(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 2 ) {
		std::cerr << "SVGPathHandlerBase::processQuadraticBezierSmooth: Invalid command count: " << count << std::endl;
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
bool PathHandlerBase::processCubicBezierSmooth(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 4 ) {
		std::cerr << "SVGPathHandlerBase::processCubicBezierSmooth: Invalid command count: " << count << std::endl;
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
//////////////////////////////////////////////////////////////////
void PathHandlerBase::debugProcess(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	std::cout << " pcmd: " << c << "(" << count << ") ";
	
	for (unsigned int i=0; i<count; i++) {
		std::cout << "[" << i << "]=" << values[i] << "\t";
	}	
	std::cout << "cp(" << currentPos.getX() << ", " << currentPos.getY() << ")" << std::endl;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::process(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( isInitialized() == false ) {
		std::cerr << "SVGPathHandlerBase not initialized "<< std::endl;
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
			std::cerr << "PathHandlerBase: Unkown command: " << c << std::endl;
			ret = false;
	}
	
	return ret;
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::prepareWork() {
//////////////////////////////////////////////////////////////////
	totalLength = 0.0;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::initNextPath() {
//////////////////////////////////////////////////////////////////
	newPath = true;
	pathListMgr.reset();
	return true;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::finishCurrentPath() {
//////////////////////////////////////////////////////////////////
	totalLength += getCurrentPathLength();
	return true;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::runCurrentPath() {
//////////////////////////////////////////////////////////////////
	// currently nothing to do;
	return true;
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::finishWork() {
//////////////////////////////////////////////////////////////////
	// currently nothing to do;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processLinearMove(bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	double newPosAbsX = currentPos.getX();
	double newPosAbsY = currentPos.getY();
	
	// first perform the transformations . . .
	currentSvgTransformMatrix.transform(newPosAbsX, newPosAbsY);
	
	//  . . . then convert the input unit to mm . . .newPosAbsX
	if ( shouldConvertRefToMM() == true ) {
		newPosAbsX = SvgUnitCalculator::convertReferenceUnit2MM(newPosAbsX);
		newPosAbsY = SvgUnitCalculator::convertReferenceUnit2MM(newPosAbsY);
	}
	
	// append
	const CncPathListEntry cpe = pathListMgr.calculateAndAddEntry(newPosAbsX, newPosAbsY, alreadyRendered, isZAxisDown());
	appendDebugValueDetail(cpe);
	
	return true;
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::tracePathList(std::ostream &ostr) {
//////////////////////////////////////////////////////////////////
	unsigned int cnt = 0;
	for (auto it = pathListMgr.begin(); it != pathListMgr.end(); ++it) {
		ostr << wxString::Format("%04d | ", cnt ) << it->getPointAsString() << std::endl;
		cnt++;
	}
}

