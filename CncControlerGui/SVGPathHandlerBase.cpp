#include <iostream>
#include <list>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include "SVGCurveLib.h"
#include "SVGPathHandlerBase.h"

//////////////////////////////////////////////////////////////////
SVGPathHandlerBase::SVGPathHandlerBase() 
: firstPath(true)
, newPath(false)
, startPos({0.0, 0.0, 0.0})
, currentPos({0.0, 0.0, 0.0})
, curveLibResolution(0.09)
, totalLength(0.0)
{
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
SVGPathHandlerBase::~SVGPathHandlerBase() {
//////////////////////////////////////////////////////////////////
	
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerBase::setPathList(const CncPathListManager& newPathList) {
	pathListMgr.reset();
	pathListMgr = newPathList;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerBase::isInitialized() {
//////////////////////////////////////////////////////////////////
	return true;
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerBase::appendDebugValueDetail(const char* key, wxVariant value) {
//////////////////////////////////////////////////////////////////
	// currently nothing to do
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerBase::appendDebugValueDetail(const CncPathListEntry& cpe) {
//////////////////////////////////////////////////////////////////
	// currently nothing to do
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerBase::debugCurrentPosition() {
//////////////////////////////////////////////////////////////////
	// currently nothing to do
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerBase::traceCurveLibPoint(const char* prefix, SVGCurveLib::PointGeneric<>& p) {
//////////////////////////////////////////////////////////////////
	if ( true ) {
		std::clog << prefix << ":" << p.x << "," << p.y << std::endl;
	}
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerBase::traceFunctionCall(const char* fn) {
//////////////////////////////////////////////////////////////////
	if ( false ) {
		std::cout << "SVGPathHandlerBase::" << fn << std::endl;
	}
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerBase::traceFirstMove(double moveX, double moveY) {
//////////////////////////////////////////////////////////////////
	if ( true )
		return;
	
	std::cout << "traceFirstMove(...)" << std::endl;
	std::cout << " Move Abs: " << currentPos.getX() + moveX << "," << currentPos.getY() + moveY << std::endl;
	std::cout << " Move Rel: " << moveX << "," << moveY << std::endl;
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerBase::tracePositions(const char* prefix) {
//////////////////////////////////////////////////////////////////
	if ( true )
		return;
	
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
	std::cout << " PL.First Move : " << pathListMgr.getFirstMove().x << "," << pathListMgr.getFirstMove().y << std::endl;

	std::cout << " StartPos      : " << startPos.getX()   << "," << startPos.getY()   << std::endl;
	std::cout << " CurrentPos    : " << currentPos.getX() << "," << currentPos.getY() << std::endl;
}
//////////////////////////////////////////////////////////////////
inline void SVGPathHandlerBase::traceCurrentPosition() {
//////////////////////////////////////////////////////////////////
	if ( true ) {
		std::clog << "CurrentPos: " << currentPos.getX() << "," << currentPos.getY() << std::endl;
	}
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerBase::processMove(char c, unsigned int count, double values[]) {
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
			pathListMgr.setFirstPathFlag();

			//the first move is always absolute!
			moveX = startPos.getX();
			moveY = startPos.getY();
			firstPath = false;
		} else {
			pathListMgr.setFirstPathFlag(false);

			//the first move is always absolute 
			//but the current position have to be considered!
			moveX = values[0] - currentPos.getX();
			moveY = values[1] - currentPos.getY();
		}
		pathListMgr.setStartPos({startPos.getX(), startPos.getY()});
		pathListMgr.setFirstMove({moveX, moveY});
		
		tracePositions("processMove");
		traceFirstMove(moveX, moveY);
		
		currentPos.setX(startPos.getX());
		currentPos.setY(startPos.getY());

		simulateZAxisUp();
		ret = processLinearMove(false);
		simulateZAxisDown();
		
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
			simulateZAxisUp();
			ret = processLinearMove(false);
			simulateZAxisDown();
		}
	}
	
	return ret;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerBase::processClose(char c, unsigned int count, double values[]) {
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
bool SVGPathHandlerBase::processLine(char c, unsigned int count, double values[]) {
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
bool SVGPathHandlerBase::processHLine(char c, unsigned int count, double values[]) {
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
bool SVGPathHandlerBase::processVLine(char c, unsigned int count, double values[]) {
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
inline bool SVGPathHandlerBase::processCurveLibPoint(SVGCurveLib::PointGeneric<> p) {
//////////////////////////////////////////////////////////////////
	currentPos.setX(p.x);
	currentPos.setY(p.y);
	
	if ( processLinearMove(true) == false )
		return false;
		
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerBase::processARC(char c, unsigned int count, double values[]) {
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
bool SVGPathHandlerBase::processQuadraticBezier(char c, unsigned int count, double values[]) {
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
bool SVGPathHandlerBase::processCubicBezier(char c, unsigned int count, double values[]) {
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
bool SVGPathHandlerBase::processQuadraticBezierSmooth(char c, unsigned int count, double values[]) {
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
bool SVGPathHandlerBase::processCubicBezierSmooth(char c, unsigned int count, double values[]) {
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
void SVGPathHandlerBase::debugProcess(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	std::cout << " pcmd: " << c << "(" << count << ") ";
	
	for (unsigned int i=0; i<count; i++) {
		std::cout << "[" << i << "]=" << values[i] << "\t";
	}	
	std::cout << "cp(" << currentPos.getX() << ", " << currentPos.getY() << ")" << std::endl;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerBase::process(char c, unsigned int count, double values[]) {
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
			std::cerr << "SVGPathHandlerBase: Unkown command: " << c << std::endl;
			ret = false;
	}
	
	return ret;
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerBase::prepareWork() {
//////////////////////////////////////////////////////////////////
	totalLength = 0.0;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerBase::initNextPath() {
//////////////////////////////////////////////////////////////////
	newPath = true;
	pathListMgr.reset();
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerBase::finishCurrentPath() {
//////////////////////////////////////////////////////////////////
	totalLength += getCurrentPathLength();
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerBase::runCurrentPath() {
//////////////////////////////////////////////////////////////////
	// currently nothing to do;
	return true;
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerBase::finishWork() {
//////////////////////////////////////////////////////////////////
	// currently nothing to do;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerBase::processLinearMove(bool alreadyRendered) {
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
double SVGPathHandlerBase::getCurrentPathLength() {
//////////////////////////////////////////////////////////////////
	return pathListMgr.getXYLength();
}
//////////////////////////////////////////////////////////////////
double SVGPathHandlerBase::getTotalLength() {
//////////////////////////////////////////////////////////////////
	return totalLength;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerBase::isPathClosed() {
//////////////////////////////////////////////////////////////////
	if ( pathListMgr.getPathListSize() > 0 ) {
		CncPathList::iterator itFirst = pathListMgr.begin(); 
		CncPathList::iterator itLast  = pathListMgr.end() - 1;
		
		//clog << (*itFirst).abs << endl;
		//clog << (*itLast).abs << endl;
	
		return ( cnc::dblCompare((*itFirst).abs.x, (*itLast).abs.x) && 
		         cnc::dblCompare((*itFirst).abs.y, (*itLast).abs.y)
		       ); 
	}
	
	return false;
}
//////////////////////////////////////////////////////////////////
SVGPathHandlerBase::WktTypeInfo SVGPathHandlerBase::getWktType() {
//////////////////////////////////////////////////////////////////
	switch ( pathListMgr.getPathListSize() ) {
		case 0:			return WKT_EMPTY;
		case 1:			return WKT_POINT;
		default:
						if ( isPathClosed() )	return WKT_POLYGON;
						else 					return WKT_LINESTRING;
	}
	
	// should not appear
	wxASSERT(NULL);
	return WKT_UNKNOWN;
}
//////////////////////////////////////////////////////////////////
const wxString& SVGPathHandlerBase::getWktTypeAsString() {
//////////////////////////////////////////////////////////////////
	static wxString s;
	switch ( getWktType() ) {
		case WKT_EMPTY:			s.assign("EMPTY"); 		break;
		case WKT_POINT:			s.assign("POINT"); 		break;
		case WKT_POLYGON:		s.assign("POLYGON"); 	break;
		case WKT_LINESTRING:	s.assign("LINESTRING"); break;
		default:				s.assign("UNKNOWN"); 	break;
	}
	
	return s;
}
//////////////////////////////////////////////////////////////////
const char* SVGPathHandlerBase::getAsSvgPathRepresentation(const wxString& style) {
//////////////////////////////////////////////////////////////////
	static wxString s;
	if ( pathListMgr.getPathListSize() == 0 ) {
		s.assign("<!-- no data available -->");
		return s.c_str();
	}
	
	s.assign("<path d=\"M");
	wxString x((pathListMgr.getPathListSize() > 1 ? " L" : ""));
		
	unsigned int cnt = 0;
	for (CncPathList::iterator it = pathListMgr.begin(); it != pathListMgr.end(); ++it) {
		if ( cnt == 0 ) s.append(wxString::Format("%.3lf,%.3lf%s", (*it).abs.x, (*it).abs.y, x));
		else			s.append(wxString::Format(" %.3lf,%.3lf",  (*it).abs.x, (*it).abs.y));
		cnt++;
	}
	
	s.append("\" ");
	
	if ( style.IsEmpty() == false ) 
		s.append(style);
	
	s.append("/>");
	return s.c_str();
}
//////////////////////////////////////////////////////////////////
const char* SVGPathHandlerBase::getAsWktRepresentation() {
//////////////////////////////////////////////////////////////////
	static wxString s;
	
	if ( pathListMgr.getPathListSize() == 0 ) {
		s.assign("");
		
	} else if ( pathListMgr.getPathListSize() == 1 ) {
		CncPathList::iterator itFirst = pathListMgr.begin();
		s.assign(wxString::Format("POINT(%.3lf %.3lf)", (*itFirst).abs.x, (*itFirst).abs.y));
		
	} else {
		if ( isPathClosed() )	s.assign("POLYGON((");
		else 					s.assign("LINESTRING(");
		
		unsigned int cnt = 0;
		for (CncPathList::iterator it = pathListMgr.begin(); it != pathListMgr.end(); ++it) {
			if ( cnt == 0 ) s.append(wxString::Format("%.3lf %.3lf", (*it).abs.x, (*it).abs.y));
			else			s.append(wxString::Format(",%.3lf %.3lf", (*it).abs.x, (*it).abs.y));
			cnt++;
		}
		
		if ( isPathClosed() )	s.append("))");
		else					s.append(")");
	}
	
	return s.c_str();
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerBase::getCentroid(wxRealPoint& centroid) {
//////////////////////////////////////////////////////////////////
	try {
		typedef boost::geometry::model::d2::point_xy<double> 	point_type;
		typedef boost::geometry::model::polygon<point_type> 	polygon_type;
		typedef boost::geometry::model::linestring<point_type> 	linestring_type;
		
		point_type p(0.0, 0.0);
		
		point_type		pointType;
		polygon_type 	polygonType;
		linestring_type	linestringType;
		
		switch ( getWktType() ) {
			case WKT_POINT:			boost::geometry::read_wkt(getAsWktRepresentation(), pointType);
									boost::geometry::centroid(pointType, p);
									break;
									
			case WKT_POLYGON:		boost::geometry::read_wkt(getAsWktRepresentation(), polygonType);
									boost::geometry::centroid(polygonType, p);
									break;
									
			case WKT_LINESTRING:	boost::geometry::read_wkt(getAsWktRepresentation(), linestringType);
									boost::geometry::centroid(linestringType, p);
									break;
									
			default:				std::cerr << "determineCentroid(): Unknown wkt type: " << getWktTypeAsString() << endl;
									return false;
			
		}
		
		centroid.x = (cnc::dblCompare(p.x(), 0.0, 0.001) == true ? 0.0 : p.x());
		centroid.y = (cnc::dblCompare(p.y(), 0.0, 0.001) == true ? 0.0 : p.y());
	}
	catch (boost::geometry::centroid_exception e) {
		std::cerr << "determineCentroid(): Error while determine centroid\n";
		std::cerr << e.what();
		std::cerr << std::endl;
		return false;
	}
	catch (...) {
		std::cerr << "determineCentroid(): Unknown Error while determine centroid\n";
		return false;
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerBase::reversePath() {
//////////////////////////////////////////////////////////////////
	if ( pathListMgr.getPathListSize() < 2 )
		return true;
	
	CncPathListEntry first = *(pathListMgr.begin());
	
	// reverse relativ move steps
	for (auto it = pathListMgr.begin() + 1; it != pathListMgr.end(); ++it) {
		(*it).move.x *= -1;
		(*it).move.y *= -1;
	}
	
	// append las entry, after reverse it will be first again
	pathListMgr.calculateAndAddEntry(pathListMgr.getPathList().back().abs, true, false);
	
	// reverse list
	std::reverse(pathListMgr.begin(), pathListMgr.end());
	
	// remove old first entry
	pathListMgr.eraseEntryAndRecalcuate(pathListMgr.end());
	
	return true;
	
/*
 * //todo
	try {
		typedef boost::geometry::model::d2::point_xy<double> 	point_type;
		typedef boost::geometry::model::polygon<point_type> 	polygon_type;
		typedef boost::geometry::model::linestring<point_type> 	linestring_type;
		
		polygon_type 	polygonType;
		linestring_type	linestringType;
		
		using boost::geometry::get;
		
		switch ( getWktType() ) {
			case WKT_EMPTY: 
			case WKT_POINT:			// nothing should happen
									return true;
									
			case WKT_POLYGON:		boost::geometry::read_wkt(getAsWktRepresentation(), polygonType);
									boost::geometry::reverse(polygonType);
									
									pathListMgr.reset();
									for(auto it = boost::begin(boost::geometry::exterior_ring(polygonType)); it != boost::end(boost::geometry::exterior_ring(polygonType)); ++it)
										pathListMgr.calculateAndAddEntry({get<0>(*it), get<1>(*it)});
										
									break;
									
			case WKT_LINESTRING:	boost::geometry::read_wkt(getAsWktRepresentation(), linestringType);
									boost::geometry::reverse(linestringType);
									
									pathListMgr.reset();
									for(auto it = boost::begin(linestringType); it != boost::end(linestringType); ++it)
										pathListMgr.calculateAndAddEntry({get<0>(*it), get<1>(*it)});
										
									break;
									
			default:				std::cerr << "reversePath(): Unknown wkt type: " << getWktTypeAsString() << endl;
									return false;
			
		}
		
		// correct the start posistion
		if ( pathListMgr.getFirstPathFlag() == true ) {
			CncPathList::iterator it = pathListMgr.begin();
			pathListMgr.setStartPos( {(*it).move.x, (*it).move.y});
		} else {
			CncPathList::iterator it = pathListMgr.begin();
			pathListMgr.incStartPos(pathListMgr.getFirstMove() - (*it).move);
		}
		
	}
	catch (boost::geometry::centroid_exception e) {
		std::cerr << "reversePath(): Error while reverse path\n";
		std::cerr << e.what();
		std::cerr << std::endl;
		return false;
	}
	catch (...) {
		std::cerr << "reversePath(): Unknown Error while reverse path\n";
		return false;
	}
	
	return true;
*/
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerBase::centerPath() {
//////////////////////////////////////////////////////////////////

//todo
	try {
		typedef boost::geometry::model::d2::point_xy<double> 	point_type;
		typedef boost::geometry::model::polygon<point_type> 	polygon_type;
		typedef boost::geometry::model::linestring<point_type> 	linestring_type;
		
		polygon_type 	polygonType;
		linestring_type	linestringType;
		
		using boost::geometry::get;
		
		wxRealPoint cp;
		if ( getCentroid(cp) == false ) {
			std::cerr << "centerPath(): Error while determining centriod: " << endl;
			return false;
		}
		
		switch ( getWktType() ) {
			case WKT_POINT:			// nothing should happen
									return true;
									
			case WKT_POLYGON:		boost::geometry::read_wkt(getAsWktRepresentation(), polygonType);
									
									pathListMgr.reset();
									for(auto it = boost::begin(boost::geometry::exterior_ring(polygonType)); it != boost::end(boost::geometry::exterior_ring(polygonType)); ++it)
										pathListMgr.calculateAndAddEntry(cp - wxRealPoint(get<0>(*it), get<1>(*it)));
										
									break;
									
			case WKT_LINESTRING:	boost::geometry::read_wkt(getAsWktRepresentation(), linestringType);
									
									pathListMgr.reset();
									for(auto it = boost::begin(linestringType); it != boost::end(linestringType); ++it)
										pathListMgr.calculateAndAddEntry(cp - wxRealPoint(get<0>(*it), get<1>(*it)));
										
									break;
									
			default:				std::cerr << "centerPath(): Unknown wkt type: " << getWktTypeAsString() << endl;
									return false;
			
		}
		
	}
	catch (boost::geometry::centroid_exception e) {
		std::cerr << "centerPath(): Error while center path\n";
		std::cerr << e.what();
		std::cerr << std::endl;
		return false;
	}
	catch (...) {
		std::cerr << "centerPath(): Unknown Error while center path\n";
		return false;
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerBase::tracePathList(std::ostream &ostr) {
//////////////////////////////////////////////////////////////////
	unsigned int cnt = 0;
	for (auto it = pathListMgr.begin(); it != pathListMgr.end(); ++it) {
		ostr << wxString::Format("%04d | ", cnt ) << it->getPointAsString() << std::endl;
		cnt++;
	}
}

