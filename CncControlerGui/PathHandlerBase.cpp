#include <iostream>
#include "CncUnitCalculator.h"
#include "CncContext.h"
#include "CncConfig.h"
#include "CncCommon.h"
#include "CncParsingSynopsisTrace.h"
#include "CncPreprocessor.h"
#include "MainFrame.h"
#include "FileParser.h"
#include "OSD/CncTimeFunctions.h"
#include "PathHandlerBase.h"

//////////////////////////////////////////////////////////////////
#define ASSERT_PH_PARA_COUNT(c) \
	if ( count != c ) { \
		std::cerr	<< CNC_LOG_FUNCT \
					<< ": Invalid parameter count: " << count \
					<< std::endl; \
		return false; \
	}

//////////////////////////////////////////////////////////////////
PathHandlerBase::PathHandlerBase() 
: CncCurveLib::Caller()
, pathListMgr				()
, fileParser				(NULL)
, nextPath					(false)
, startPos					({0.0, 0.0, 0.0})
, currentPos				({0.0, 0.0, 0.0})
, totalLength				(0.0)
, unitCalculator			(Unit::mm, Unit::mm)
, lineCurve					(this)
, ellipticalCurve			(this)
, quadraticBezierCurve		(this)
, cubicBezierCurve			(this)
, lastQuadraticControlPoint	()
, lastCubicControlPoint		()
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
void PathHandlerBase::logNextPathListEntry(const CncPathListEntry& cpe) {
//////////////////////////////////////////////////////////////////
	appendDebugValueDetail(cpe);
	
	const CncDoublePosition& p =  cpe.entryTarget;
	PositionStorage::addPos(PositionStorage::TRIGGER_PH_CB_POS, p);
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::processClientId(long id) {
//////////////////////////////////////////////////////////////////
	pathListMgr.addEntryAdm(id);
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::processFeedSpeed(CncSpeedMode mode) {
//////////////////////////////////////////////////////////////////
	// do nothing, have to be overridden by classes which
	// knowing current feed speed values for the given mode
	std::cerr << CNC_LOG_FUNCT_A(": Invalid call. This method have to be overridden to use\n");
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::processFeedSpeed(CncSpeedMode mode, double feedSpeed_MM_MIN) {
//////////////////////////////////////////////////////////////////
	pathListMgr.addEntryAdm(mode, feedSpeed_MM_MIN);
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::processSpindleState(bool state) {
//////////////////////////////////////////////////////////////////
	pathListMgr.addEntrySpl(state);
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::processSpindleSpeed(double feedSpeed_U_MIN) {
//////////////////////////////////////////////////////////////////
	pathListMgr.addEntrySpl(feedSpeed_U_MIN);
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processMove_2DXY(char c, unsigned int count, const double values[]) {
//////////////////////////////////////////////////////////////////
	ASSERT_PH_PARA_COUNT(2) // will return on failure
	
	appendDebugValueDetail("Move", c);
	
	// Start a new sub-path at the given (x,y) coordinate. M (uppercase) indicates that absolute coordinates will follow; 
	// m (lowercase) indicates that relative coordinates will follow. If a moveto is followed by multiple pairs of coordinates, 
	// the subsequent pairs are treated as implicit lineto commands. Hence, implicit lineto commands will be relative 
	// if the moveto is relative, and absolute if the moveto is absolute. 
	// If a relative moveto (m) appears as the first element of the path, then it is treated as a pair of absolute coordinates. 
	// In this case, subsequent pairs of coordinates are treated as relative even though the initial moveto is interpreted as an 
	// absolute moveto.
	
	bool ret = false;
	
	if ( nextPath == true ) {
		// Move to the path start
		// first register start pos
		// the first move is always absolute - see comment above!
		startPos.setXY(values[0], values[1]); 
		
		// than give the path list manager the reference from where we are coming 
		pathListMgr.setReferencePos(currentPos);
		
		// the first move is always absolute - see comment above!
		currentPos.setXY(startPos.getX(), startPos.getY());
		
		// drive to first path
		ret = processLinearMove(false);
		nextPath = false;
	}
	else {
		// Moving the path forward
		const double moveX = ( c == 'M' ? values[0] - currentPos.getX() : values[0] );
		const double moveY = ( c == 'M' ? values[1] - currentPos.getY() : values[1] );
		
		currentPos.incXY(moveX, moveY);
		startPos.setXY(currentPos.getX(), currentPos.getY());
		
		// to decouple the sub path the following speed mode switch is important . . . 
		processFeedSpeed(CncSpeedRapid);
		{
			// . . . now this is a flying (rapid) move 
			ret = processLinearMove(false);
		}
		processFeedSpeed(CncSpeedWork);
	}
	
	return ret;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processClose_2DXY(char c, unsigned int count, const double values[]) {
//////////////////////////////////////////////////////////////////
	ASSERT_PH_PARA_COUNT(0) // will return on failure
	
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
bool PathHandlerBase::processLine_2DXY(char c, unsigned int count, const double values[]) {
//////////////////////////////////////////////////////////////////
	ASSERT_PH_PARA_COUNT(2) // will return on failure
	
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
bool PathHandlerBase::processHLine_2DXY(char c, unsigned int count, const double values[]) {
//////////////////////////////////////////////////////////////////
	ASSERT_PH_PARA_COUNT(1) // will return on failure
	
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
bool PathHandlerBase::processVLine_2DXY(char c, unsigned int count, const double values[]) {
//////////////////////////////////////////////////////////////////
	ASSERT_PH_PARA_COUNT(1) // will return on failure
	
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
bool PathHandlerBase::processARC_2DXY(CncCurveLib::ParameterElliptical& ps) {
//////////////////////////////////////////////////////////////////
	// render - releases PathHandlerBase::callback(const CncCurveLib::Point& p)
	bool ret = ellipticalCurve.render(ps);
	appendDebugValueDetail(ps);

	return ret;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processARC_2DXY(char c, unsigned int count, const double values[]) {
//////////////////////////////////////////////////////////////////
	ASSERT_PH_PARA_COUNT(7) // will return on failure
	
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
	
	// Note: 
	// With respect to the svg description below the sweep flag has to be 1
	// but this results in a wrong display regarding the svg is converted to a 
	// right hand coordinate system.
	// conversion: translate="transform(0,hight) scale(1,-1) - where scaleY = -1
	// changes the path direction . . . 
	// Therefore, this has to be considered. 
	//const int sweepFlag = THE_CONFIG->getSvgConvertToRightHandFlag() ? 0 : 1;
	ps.sweepFlag		= THE_CONFIG->getSvgConvertToRightHandFlag() ? !ps.sweepFlag : ps.sweepFlag;
	
	// additionally the following context flag is also considered here 
	// - my be debug only
	ps.sweepFlag		= invertPathArgSweepFlag()					 ? !ps.sweepFlag : ps.sweepFlag;
	
	return processARC_2DXY(ps);
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processQuadraticBezier_2DXY(CncCurveLib::ParameterQuadraticBezier& ps) {
//////////////////////////////////////////////////////////////////
	// render - releases PathHandlerBase::callback(const CncCurveLib::Point& p)
	bool ret = quadraticBezierCurve.render(ps);
	appendDebugValueDetail(ps);

	return ret;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processQuadraticBezier_2DXY(char c, unsigned int count, const double values[]) {
//////////////////////////////////////////////////////////////////
	ASSERT_PH_PARA_COUNT(4) // will return on failure
	
	appendDebugValueDetail("QuadraticBezier",c);
	appendDebugValueDetail("Input Unit", CncUnitCalculatorBase::getUnitAsStr(unitCalculator.getInputUnit()));
	
	// define parameters
	//  - p0 (startPos):	for curve lib is always absolute
	//  - p1 - p2:			for curve lib are always absolute
	//  - pCtl:				is always absolute
	CncCurveLib::ParameterQuadraticBezier& ps = quadraticBezierCurve.getParameterSet();
	CncCurveLib::Point pCtl;
	ps.p0 = transformCurveLibPoint(currentPos.getX(), currentPos.getY()); 
	
	switch ( c ) {
		case 'q': 	ps.p1 = transformCurveLibPoint(values[0] + currentPos.getX(), values[1] + currentPos.getY()); 
					ps.p2 = transformCurveLibPoint(values[2] + currentPos.getX(), values[3] + currentPos.getY()); 
					
					// set control point and current pos without transformation
					pCtl = {values[0] + currentPos.getX(), values[1] + currentPos.getY()};
					
					currentPos.setX(values[2] + currentPos.getX());
					currentPos.setY(values[3] + currentPos.getY());
					
					break;
					
		case 'Q':	ps.p1 = transformCurveLibPoint(values[0], values[1]);
					ps.p2 = transformCurveLibPoint(values[2], values[3]);
					
					// set control point and current pos without transformation
					pCtl = {values[0], values[1]};
					
					currentPos.setX(values[2]);
					currentPos.setY(values[3]);
					
					break;
					
		default:	wxASSERT_MSG(NULL, wxString::Format("QuadraticBezier: Invalid command: %c", c));
	}
	
	const bool ret = processQuadraticBezier_2DXY(ps);
	if ( ret == true ) {
		// Store the last control point
		lastQuadraticControlPoint.setCtrlPointAbs(pCtl);
	}

	return ret;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processCubicBezier_2DXY(CncCurveLib::ParameterCubicBezier& ps) {
//////////////////////////////////////////////////////////////////
	// render - releases PathHandlerBase::callback(const CncCurveLib::Point& p)
	bool ret = cubicBezierCurve.render(ps);
	appendDebugValueDetail(ps);

	return ret;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processCubicBezier_2DXY(char c, unsigned int count, const double values[]) {
//////////////////////////////////////////////////////////////////
	ASSERT_PH_PARA_COUNT(6) // will return on failure
	
	appendDebugValueDetail("CubicBezier",c);
	appendDebugValueDetail("Input Unit", CncUnitCalculatorBase::getUnitAsStr(unitCalculator.getInputUnit()));
	
	// define parameters
	//  - p0 (startPos) for curve lib is always absolute
	//  - p1 - p3 for curve lib are always absolute
	CncCurveLib::ParameterCubicBezier& ps = cubicBezierCurve.getParameterSet();
	CncCurveLib::Point pCtl;
	ps.p0 = transformCurveLibPoint(currentPos.getX(), currentPos.getY());
	
	switch ( c ) {
		case 'c': 	ps.p1 = transformCurveLibPoint(values[0] + currentPos.getX(), values[1] + currentPos.getY()); 
					ps.p2 = transformCurveLibPoint(values[2] + currentPos.getX(), values[3] + currentPos.getY()); 
					ps.p3 = transformCurveLibPoint(values[4] + currentPos.getX(), values[5] + currentPos.getY()); 
					
					// set control point and current pos without transformation
					pCtl = {values[2] + currentPos.getX(), values[3] + currentPos.getY()};

					currentPos.setX(values[4] + currentPos.getX());
					currentPos.setY(values[5] + currentPos.getY());
					
					break;
					
		case 'C':	ps.p1 = transformCurveLibPoint(values[0], values[1]);
					ps.p2 = transformCurveLibPoint(values[2], values[3]);
					ps.p3 = transformCurveLibPoint(values[4], values[5]);
					
					// set control point and current pos without transformation
					pCtl = {values[2], values[3]};
					
					currentPos.setX(values[4]);
					currentPos.setY(values[5]);
					
					break;
					
		default:	wxASSERT_MSG(NULL, wxString::Format("CubicBezier: Invalid command: %c", c));
	}
	
	const bool ret = processCubicBezier_2DXY(ps);
	if ( ret == true ) {
		// Store the last control point
		lastCubicControlPoint.setCtrlPointAbs(pCtl);
	}

	return ret;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processQuadraticBezierSmooth_2DXY(char c, unsigned int count, const double values[]) {
//////////////////////////////////////////////////////////////////
	ASSERT_PH_PARA_COUNT(2) // will return on failure
	
	appendDebugValueDetail("QuadraticBezierSmooth",c);
	
	// p0 (startPos) for curve lib is always absolute
	CncCurveLib::Point p0 = {currentPos.getX(), currentPos.getY()};
	
	double newValues[MAX_PARAMETER_VALUES];
	newValues[3] = values[1];
	newValues[2] = values[0];
	
	switch ( c ) {
		case 't':	c = 'q'; 
					newValues[1] = lastQuadraticControlPoint.getLastCtrlPointReflectedAbs(p0).y - currentPos.getY();
					newValues[0] = lastQuadraticControlPoint.getLastCtrlPointReflectedAbs(p0).x - currentPos.getX();
					break;
					
		case 'T':	c = 'Q'; 
					newValues[1] = lastQuadraticControlPoint.getLastCtrlPointReflectedAbs(p0).y;
					newValues[0] = lastQuadraticControlPoint.getLastCtrlPointReflectedAbs(p0).x;
					break;
					
		default:	wxASSERT_MSG(NULL, wxString::Format("QuadraticBezierSmooth: Invalid command: %c", c));
	}
	
	// debug only
	if ( false ) {
		const float cpX  = lastQuadraticControlPoint.getLastCtrlPointAbs(p0).x;
		const float cpY  = lastQuadraticControlPoint.getLastCtrlPointAbs(p0).y;
		const float cprX = lastQuadraticControlPoint.getLastCtrlPointReflectedAbs(p0).x;
		const float cprY = lastQuadraticControlPoint.getLastCtrlPointReflectedAbs(p0).y;
		
		std::cout << wxString::Format("cur(%lf,%lf)\n", 					currentPos.getX(), currentPos.getY());
		std::cout << wxString::Format("ctl(%lf,%lf) reflected(%lf,%lf)\n",	cpX, cpY, cprX, cprY);
		std::cout << wxString::Format("%c %lf,%lf %lf,%lf\n",				c, newValues[0], newValues[1], newValues[2], newValues[3]);
	}
	
	return processCommand_2DXY(c, 4, newValues);
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processCubicBezierSmooth_2DXY(char c, unsigned int count, const double values[]) {
//////////////////////////////////////////////////////////////////
	ASSERT_PH_PARA_COUNT(4) // will return on failure
	
	appendDebugValueDetail("CubicBezierSmooth",c);
	
	// p0 (startPos) for curve lib is always absolute
	CncCurveLib::Point p0 = {currentPos.getX(), currentPos.getY()};
	
	double newValues[MAX_PARAMETER_VALUES];
	newValues[5] = values[3];
	newValues[4] = values[2];
	newValues[3] = values[1];
	newValues[2] = values[0];
	
	switch ( c ) {
		case 's':	c = 'c'; 
					newValues[1] = lastCubicControlPoint.getLastCtrlPointReflectedAbs(p0).y - currentPos.getY();
					newValues[0] = lastCubicControlPoint.getLastCtrlPointReflectedAbs(p0).x - currentPos.getX();
					break;
			
		case 'S':	c = 'C'; 
					newValues[1] = lastCubicControlPoint.getLastCtrlPointReflectedAbs(p0).y;
					newValues[0] = lastCubicControlPoint.getLastCtrlPointReflectedAbs(p0).x;
					break;
					
		default:	wxASSERT_MSG(NULL, wxString::Format("CubicBezierSmooth: Invalid command: %c", c));
	}
	
	// debug only
	if ( false ) {
		const float cpX  = lastQuadraticControlPoint.getLastCtrlPointAbs(p0).x;
		const float cpY  = lastQuadraticControlPoint.getLastCtrlPointAbs(p0).y;
		const float cprX = lastQuadraticControlPoint.getLastCtrlPointReflectedAbs(p0).x;
		const float cprY = lastQuadraticControlPoint.getLastCtrlPointReflectedAbs(p0).y;
		
		std::cout << wxString::Format("cur(%lf,%lf)\n", 					currentPos.getX(), currentPos.getY());
		std::cout << wxString::Format("ctl(%lf,%lf) reflected(%lf,%lf)\n",	cpX, cpY, cprX, cprY);
		std::cout << wxString::Format("%c %lf,%lf %lf,%lf %lf,%lf\n",		c, newValues[0], newValues[1], newValues[2], newValues[3], newValues[4], newValues[5]);
	}
	
	return processCommand_2DXY(c, 6, newValues);
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::processCommand_2DXY(char c, unsigned int count, const double values[]) {
//////////////////////////////////////////////////////////////////
	if ( isInitialized() == false ) {
		std::cerr	<< CNC_LOG_FUNCT 
					<< ": PathHandlerBase not initialized "
					<< std::endl;
		return false;
	}
	
	//------------------------------------------------------------
	/*
	auto debug = [&](char c, unsigned int count, double values[]) {
		std::cout << " pcmd: " << c << "(" << count << ") ";
		
		for (unsigned int i=0; i<count; i++)
			std::cout << "[" << i << "]=" << values[i] << "\t";
			
		std::cout << "cp(" << currentPos.getX() << ", " << currentPos.getY() << ")" << std::endl;
	};

	// only to debug
	debug(c, count, values);
	*/
	
	bool ret = false;
	switch ( c ) {
		case 'm':
		case 'M': 	lastQuadraticControlPoint.reset();
					lastCubicControlPoint.reset();
					ret = processMove_2DXY(c, count, values);
					break;
		case 'l':
		case 'L': 	lastQuadraticControlPoint.reset();
					lastCubicControlPoint.reset();
					ret = processLine_2DXY(c, count, values);
					break;
		case 'h':
		case 'H': 	lastQuadraticControlPoint.reset();
					lastCubicControlPoint.reset();
					ret = processHLine_2DXY(c, count, values);
					break;
		case 'v':
		case 'V': 	lastQuadraticControlPoint.reset();
					lastCubicControlPoint.reset();
					ret = processVLine_2DXY(c, count, values);
					break;
		case 'z':
		case 'Z': 	lastQuadraticControlPoint.reset();
					lastCubicControlPoint.reset();
					ret = processClose_2DXY(c, count, values);
					break;
		case 'a':
		case 'A': 	lastQuadraticControlPoint.reset();
					lastCubicControlPoint.reset();
					ret = processARC_2DXY(c, count, values);
					break;
		case 'c':
		case 'C': 	ret = processCubicBezier_2DXY(c, count, values);
					break;
		case 's':
		case 'S': 	ret = processCubicBezierSmooth_2DXY(c, count, values);
					break;
		case 'q':
		case 'Q': 	ret = processQuadraticBezier_2DXY(c, count, values);
					break;
		case 't':
		case 'T': 	ret = processQuadraticBezierSmooth_2DXY(c, count, values);
					break;
		default: 
					std::cerr << CNC_LOG_FUNCT << ": Unkown command: " << c << std::endl;
					ret = false;
	}
	
	return ret;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::prepareWork() {
//////////////////////////////////////////////////////////////////
	pathListMgr.clear();
	totalLength = 0.0;
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::initNextPath() {
//////////////////////////////////////////////////////////////////
	nextPath = true;
	pathListMgr.initNextCncPath();
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
	return true;
}
//////////////////////////////////////////////////////////////////
bool PathHandlerBase::finishWork() {
//////////////////////////////////////////////////////////////////
	return true;
}
//////////////////////////////////////////////////////////////////
void PathHandlerBase::changeInputUnit(const Unit u, bool trace) {
//////////////////////////////////////////////////////////////////
	unitCalculator.changeInputUnit(u);
	
	CncUnitCalculator<float> uc(Unit::mm, u);
	CncCurveLib::Setup s;
	s.resolution.size = uc.convert(THE_CONFIG->getRenderResolutionMM());
	
	lineCurve.init(s);
	ellipticalCurve.init(s);
	quadraticBezierCurve.init(s);
	cubicBezierCurve.init(s);
	
	{
		std::stringstream ss;
		ss << " " << getName() << ": Setup " << unitCalculator;
		THE_APP->getParsingSynopsisTrace()->addSeparator("Setup");
		THE_APP->getParsingSynopsisTrace()->addInfo(wxString::Format("%s\n", ss.str()));
	}
	{
		std::stringstream ss;
		ss << " " << s;
		THE_APP->getParsingSynopsisTrace()->addInfo(wxString::Format("%s\n", ss.str()));
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
