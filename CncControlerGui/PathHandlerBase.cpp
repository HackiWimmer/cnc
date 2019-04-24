#include <iostream>
#include "CncUnitCalculator.h"
#include "CncConfig.h"
#include "FileParser.h"
#include "OSD/CncTimeFunctions.h"
#include "PathHandlerBase.h"

//////////////////////////////////////////////////////////////////
PathHandlerBase::PathHandlerBase() 
: CncCurveLib::Caller()
, firstPath(true)
, newPath(false)
, startPos({0.0, 0.0, 0.0})
, currentPos({0.0, 0.0, 0.0})
, totalLength(0.0)
, lastControlPoint()
, pathListMgr()
, unitCalculator(Unit::mm, Unit::mm)
, lineCurve(this)
, ellipticalCurve(this)
, quadraticBezierCurve(this)
, cubicBezierCurve(this)
{
//////////////////////////////////////////////////////////////////
	// init default setup
	CncCurveLib::Setup s;
	s.approximation.samples = 50;
	s.resolution.size 		= 0.02;
	
	lineCurve.init(s);
	ellipticalCurve.init(s);
	quadraticBezierCurve.init(s);
	cubicBezierCurve.init(s);
}
//////////////////////////////////////////////////////////////////
PathHandlerBase::~PathHandlerBase() {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::initCurrentPos(const CncDoublePosition& pos) {
//////////////////////////////////////////////////////////////////
	startPos 	= pos;
	currentPos	= pos;
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::setCurveLibResolution(float res) { 
//////////////////////////////////////////////////////////////////
	std::cerr << "PathHandlerBase::setCurveLibResolution is obsolete" << std::endl;
	#warning CncConfig::setCurveLibIncrement(res) is obsolete here 
	//CncConfig::setCurveLibIncrement(res); 
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
void PathHandlerBase::appendDebugValueDetail(const CncCurveLib::ParameterSet& ps) {
//////////////////////////////////////////////////////////////////
	// currently nothing to do
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::traceFunctionCall(const char* fn) {
//////////////////////////////////////////////////////////////////
	std::cout << "PathHandlerBase::" << fn << std::endl;
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::traceFirstMove(double moveX, double moveY) {
//////////////////////////////////////////////////////////////////
	std::cout << "traceFirstMove(...)" << std::endl;
	std::cout << " Move Abs: " << currentPos.getX() + moveX << "," << currentPos.getY() + moveY << std::endl;
	std::cout << " Move Rel: " << moveX << "," << moveY << std::endl;
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::tracePositions(const char* userPerspectivePrefix) {
//////////////////////////////////////////////////////////////////
	std::cout << "tracePositions(" << userPerspectivePrefix << ")" << std::endl;
	
	if ( pathListMgr.getPathListSize() > 0 ) {
		CncPathList::iterator it = pathListMgr.begin();
		std::cout << " PL.First.Pos  : " << (*it).entryDistance << std::endl;
		it = pathListMgr.end()-1;
		std::cout << " PL.Last.Pos   : " << (*it).entryDistance << std::endl;
	} else {
		std::cout << " PL.First.Pos  : empty" << std::endl;
		std::cout << " PL.Last.Pos   : empty" << std::endl;
	}
	std::cout << " PL.firstPath  : " << pathListMgr.getFirstPathFlag()   << std::endl;
	std::cout << " PL.Start Pos  : " << pathListMgr.getStartPos().getX() << std::endl;

	std::cout << " StartPos      : " << startPos   << std::endl;
	std::cout << " CurrentPos    : " << currentPos << std::endl;
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::traceCurrentPosition() {
//////////////////////////////////////////////////////////////////
	std::clog << "CurrentPos: " << currentPos << std::endl;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processMove(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 2 ) {
		std::cerr << "PathHandlerBase::processMove: Invalid parameter count: " << count << std::endl;
		return false;
	}
	
	appendDebugValueDetail("Move",c);
	
	bool ret = false;
	
	if ( newPath == true ) {
		
		startPos.setX(values[0]);
		startPos.setY(values[1]);
		
		// first of all give the path list manager a reference from where we are coming 
		pathListMgr.setReferencePos(currentPos);
		pathListMgr.setFirstPathFlag(firstPath);
		
		if ( firstPath == true ) {
			firstPath = false;
			
			if ( moveUpZ() == false )
				return false;
		}
		
		//the first move is always absolute!
		currentPos.setX(startPos.getX());
		currentPos.setY(startPos.getY());
		ret = processLinearMove(false);
		
		if ( moveDownZ() == false )
			return false;

		newPath = false;
		
	}
	else {
		double moveX = 0.0, moveY = 0.0;

		if ( c == 'M' ) {
			moveX = values[0] - currentPos.getX();
			moveY = values[1] - currentPos.getY();
		}
		else {
			moveX = values[0];
			moveY = values[1];
		}

		currentPos.incX(moveX);
		currentPos.incY(moveY);

		startPos.setX(currentPos.getX());
		startPos.setY(currentPos.getY());
		
		ret = true;
		if ( cnc::dblCompareNull(moveX) == false ||
			 cnc::dblCompareNull(moveY) == false ) {
			ret = processLinearMove(false);
		}
	}
	
	return ret;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processClose(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 0 ) {
		std::cerr << "PathHandlerBase::processClose: Invalid parameter count: " << count << std::endl;
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
	
	return processLinearMove(false);
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processLine(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 2 ) {
		std::cerr << "PathHandlerBase::processLine: Invalid parameter count: " << count << std::endl;
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
		std::cerr << "PathHandlerBase::processHLine: Invalid parameter count: " << count << std::endl;
		return false;
	}
	
	appendDebugValueDetail("HLine",c);
	
	switch ( c ) {
		case 'h':
			currentPos.incX(values[0]);
			break;
		case 'H':
			currentPos.setX(values[0]);
			break;
		default: ; // Do nothing, already checked before
	}
	
	return processLinearMove(false);
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processVLine(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 1 ) {
		std::cerr << "PathHandlerBase::processVLine: Invalid parameter count: " << count << std::endl;
		return false;
	}
	
	appendDebugValueDetail("VLine",c);
	
	switch ( c ) {
		case 'v':
			currentPos.incY(values[0]);
			break;
		case 'V':
			currentPos.setY(values[0]);
			break;
		default: ; // Do nothing, already checked before
	}
	
	return processLinearMove(false);
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::callback(const CncCurveLib::Point& p) {
//////////////////////////////////////////////////////////////////
	currentPos.setX(p.x);
	currentPos.setY(p.y);
	
	// already rendered == true not longer supported
	if ( processLinearMove(true) == false )
		return false;
		
	return true;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processARC(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 7 ) {
		std::cerr << "PathHandlerBase::processARC: Invalid parameter count: " << count << std::endl;
		return false;
	}
	
	appendDebugValueDetail("EllipticalARC", c);
	appendDebugValueDetail("Input Unit", CncUnitCalculatorBase::getUnitAsStr(unitCalculator.getInputUnit()));
	
	// define parameters
	//  - p0 (startPos) for curve lib is always absolute
	//  - p1 (endPos) for curve lib is always absolute
	CncCurveLib::ParameterElliptical& ps = ellipticalCurve.getParameterSet();
	ps.p0 = transformCurveLibPoint(currentPos.getX(), currentPos.getY());
	
	switch ( c ) {
		case 'a': 	ps.p1 = transformCurveLibPoint(values[5] + currentPos.getX(), values[6] + currentPos.getY()); 
					
					// set current pos and control point without transformation
					currentPos.setX(values[5] + currentPos.getX());
					currentPos.setY(values[6] + currentPos.getY());
					break;
					
		case 'A':	ps.p1 = transformCurveLibPoint(values[5], values[6]);
		
					// set current pos and control point without transformation
					currentPos.setX(values[5]);
					currentPos.setY(values[6]);
					break;
					
		default:	ps.p1 = ps.p0;
		
					// current pos leaves unchanged
	}
	
	ps.rx				= values[0];
	ps.ry				= values[1];
	ps.xAxisRotation	= values[2];
	ps.largeArcFlag		= (bool)values[3];
	ps.sweepFlag		= (bool)values[4];
	
	// render - releases PathHandlerBase::callback(const CncCurveLib::Point& p)
	bool ret = ellipticalCurve.render(ps);
	appendDebugValueDetail(ps);
	
	return ret;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processQuadraticBezier(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 4 ) {
		std::cerr << "PathHandlerBase::processQuadraticBezier: Invalid parameter count: " << count << std::endl;
		return false;
	}
	
	appendDebugValueDetail("QuadraticBezier",c);
	appendDebugValueDetail("Input Unit", CncUnitCalculatorBase::getUnitAsStr(unitCalculator.getInputUnit()));
	
	// define parameters
	//  - p0 (startPos) for curve lib is always absolute
	//  - p1 - p2 for curve lib is always absolute
	CncCurveLib::ParameterQuadraticBezier& ps = quadraticBezierCurve.getParameterSet();
	CncCurveLib::Point pCtl;
	ps.p0 = transformCurveLibPoint(currentPos.getX(), currentPos.getY()); 
	
	switch ( c ) {
		case 'q': 	ps.p1 = transformCurveLibPoint(values[0] + currentPos.getX(), values[1] + currentPos.getY()); 
					ps.p2 = transformCurveLibPoint(values[2] + currentPos.getX(), values[3] + currentPos.getY()); 
					
					// set current pos and control point without transformation
					currentPos.setX(values[2] + currentPos.getX());
					currentPos.setY(values[3] + currentPos.getY());
					
					pCtl = {values[0] + currentPos.getX(), values[1] + currentPos.getY()};
					break;
					
		case 'Q':	ps.p1 = transformCurveLibPoint(values[0], values[1]);
					ps.p2 = transformCurveLibPoint(values[2], values[3]);
					
					// set current pos and control point without transformation
					currentPos.setX(values[2]);
					currentPos.setY(values[3]);
					
					pCtl = {values[0], values[1]};
					break;
					
		default:	ps.p1 = ps.p0;
					ps.p2 = ps.p0;
					
					// current pos leaves unchanged
					pCtl = ps.p1;
	}
	
	// render - releases PathHandlerBase::callback(const CncCurveLib::Point& p)
	bool ret = quadraticBezierCurve.render(ps);
	appendDebugValueDetail(ps);
	
	// Store the last control point
	CncCurveLib::Point cp{currentPos.getX(), currentPos.getY()};
	lastControlPoint.setLastQuadraticBezierControlPoint(cp, pCtl);
	
	return ret;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processCubicBezier(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 6 ) {
		std::cerr << "PathHandlerBase::processCubicBezier: Invalid parameter count: " << count << std::endl;
		return false;
	}
	
	appendDebugValueDetail("CubicBezier",c);
	appendDebugValueDetail("Input Unit", CncUnitCalculatorBase::getUnitAsStr(unitCalculator.getInputUnit()));
	
	// define parameters
	//  - p0 (startPos) for curve lib is always absolute
	//  - p1 - p3 for curve lib is always absolute
	CncCurveLib::ParameterCubicBezier& ps = cubicBezierCurve.getParameterSet();
	CncCurveLib::Point pCtl;
	ps.p0 = transformCurveLibPoint(currentPos.getX(), currentPos.getY());
	
	switch ( c ) {
		case 'c': 	ps.p1 = transformCurveLibPoint(values[0] + currentPos.getX(), values[1] + currentPos.getY()); 
					ps.p2 = transformCurveLibPoint(values[2] + currentPos.getX(), values[3] + currentPos.getY()); 
					ps.p3 = transformCurveLibPoint(values[4] + currentPos.getX(), values[5] + currentPos.getY()); 
					
					// set current pos and control point without transformation
					currentPos.setX(values[4] + currentPos.getX());
					currentPos.setY(values[5] + currentPos.getY());
					
					pCtl = {values[2] + currentPos.getX(), values[3] + currentPos.getY()};
					break;
					
		case 'C':	ps.p1 = transformCurveLibPoint(values[0], values[1]);
					ps.p2 = transformCurveLibPoint(values[2], values[3]);
					ps.p3 = transformCurveLibPoint(values[4], values[5]);
					
					// set current pos and control point without transformation
					currentPos.setX(values[4]);
					currentPos.setY(values[5]);
					
					pCtl = {values[2], values[3]};
					
					break;
					
		default:	ps.p1 = ps.p0; 
					ps.p2 = ps.p0;
					ps.p3 = ps.p0;
					
					// current pos leaves unchanged
					pCtl = ps.p2;
	}
	
	// render - releases PathHandlerBase::callback(const CncCurveLib::Point& p)
	bool ret = cubicBezierCurve.render(ps);
	appendDebugValueDetail(ps);
	
	// Store the last control point
	CncCurveLib::Point cp{currentPos.getX(), currentPos.getY()};
	lastControlPoint.setLastCubicBezierControlPoint(cp, pCtl);
	
	return ret;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processQuadraticBezierSmooth(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( count != 2 ) {
		std::cerr << "PathHandlerBase::processQuadraticBezierSmooth: Invalid parameter count: " << count << std::endl;
		return false;
	}
	appendDebugValueDetail("QuadraticBezierSmooth",c);
	
	// p0 (startPos) for curve lib is always absolute
	CncCurveLib::Point p0 = {currentPos.getX(), currentPos.getY()};
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
		std::cerr << "PathHandlerBase::processCubicBezierSmooth: Invalid parameter count: " << count << std::endl;
		return false;
	}
	appendDebugValueDetail("CubicBezierSmooth",c);
	
	// p0 (startPos) for curve lib is always absolute
	CncCurveLib::Point p0 = {currentPos.getX(), currentPos.getY()};
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
		std::cerr << "PathHandlerBase not initialized "<< std::endl;
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
void PathHandlerBase::changeInputUnit(const Unit u, bool trace) {
//////////////////////////////////////////////////////////////////
	unitCalculator.changeInputUnit(u);
	
	CncUnitCalculator<float> uc(Unit::mm, u);
	
	CncCurveLib::Setup s;
	s.resolution.size = uc.convert(GBL_CONFIG->getRenderResolutionMM());
	
	lineCurve.init(s);
	ellipticalCurve.init(s);
	quadraticBezierCurve.init(s);
	cubicBezierCurve.init(s);
	
	if ( trace == true ) {
		std::stringstream ss;
		ss << " " << getName() << ": Setup " << unitCalculator;
		cnc::trc.logInfo(ss.str().c_str()); 
	}
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::tracePathList(std::ostream &ostr) {
//////////////////////////////////////////////////////////////////
	for (auto it = pathListMgr.begin(); it != pathListMgr.end(); ++it) {
		it->traceEntry(ostr);
	}
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::processWait(int64_t microseconds) {
//////////////////////////////////////////////////////////////////
	std::cout << " Processing Dwell: " << ((double)microseconds) / (1000 * 1000) << "[s] . . .";
	CncTimeFunctions::activeWaitMircoseconds(microseconds, true);
	std::clog << " Done\n";
}
