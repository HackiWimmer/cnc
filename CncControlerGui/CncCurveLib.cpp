#include <algorithm>
#include <cmath>
#include "CncCommon.h"
#include "CncCurveLib.h"

//////////////////////////////////////////////////////////////////
const float CncCurveLib::PI = 3.14159265359f;

//////////////////////////////////////////////////////////////////
float CncCurveLib::clamp(float val, float minVal, float maxVal) {
//////////////////////////////////////////////////////////////////
	float tempMin = (std::min)(val, maxVal);
	return (std::max)(minVal, tempMin);
}
//////////////////////////////////////////////////////////////////
float CncCurveLib::toRadians(float angle) {
//////////////////////////////////////////////////////////////////
	return angle * (CncCurveLib::PI / 180);
}
//////////////////////////////////////////////////////////////////
float CncCurveLib::angleBetween(const CncCurveLib::Point& v0, const CncCurveLib::Point& v1) {
//////////////////////////////////////////////////////////////////

	float p     = v0.x * v1.x + v0.y * v1.y;
	float n     = sqrt((pow(v0.x, 2) + pow(v0.y, 2)) * (pow(v1.x, 2) + pow(v1.y, 2)));
	float sign  = v0.x * v1.y - v0.y * v1.x < 0 ? -1.0f : 1.0f;
	float angle = sign * acos(p / n);

	return angle;
}
//////////////////////////////////////////////////////////////////
float CncCurveLib::distance(const CncCurveLib::Point& p0, const CncCurveLib::Point& p1) {
//////////////////////////////////////////////////////////////////
	return sqrt(pow(p1.x - p0.x, 2) + pow(p1.y - p0.y, 2));
}
//////////////////////////////////////////////////////////////////
void CncCurveLib::init(const CncCurveLib::Setup& s) {
//////////////////////////////////////////////////////////////////
	setup = s;
}
//////////////////////////////////////////////////////////////////
const CncCurveLib::Point CncCurveLib::getPointOnLine(CncCurveLib::ParameterSet& ps, const float t) {
//////////////////////////////////////////////////////////////////
	auto calculateLinearLineParameter = [](float v0, float v1, float t) {
		const float result = v0 + (v1-v0)*t;
		return result;
	};

	return CncCurveLib::Point(	calculateLinearLineParameter(ps.p0.x, ps.p1.x, t),
								calculateLinearLineParameter(ps.p0.y, ps.p1.y, t)
	);
}
//////////////////////////////////////////////////////////////////
const CncCurveLib::Point CncCurveLib::getPointOnQuadraticBezierCurve (CncCurveLib::ParameterSet& ps, const float t) {
//////////////////////////////////////////////////////////////////
	auto calculateQuadraticBezierParameter = [](float v0, float v1, float v2, float t) {
		const float result = pow(1-t, 2)*v0 + 2*t*(1-t)*v1 + pow(t, 2)*v2;
		return result;
	};

	return CncCurveLib::Point(	calculateQuadraticBezierParameter(ps.p0.x, ps.p1.x, ps.p2.x, t),
								calculateQuadraticBezierParameter(ps.p0.y, ps.p1.y, ps.p2.y, t)
	);
}
//////////////////////////////////////////////////////////////////
const CncCurveLib::Point CncCurveLib::getPointOnCubicBezierCurve(CncCurveLib::ParameterSet& ps, const float t) {
//////////////////////////////////////////////////////////////////
	auto calculateCubicBezierParameter = [](float v0, float v1, float v2, float v3, float t) {
		const float result = pow(1-t, 3)*v0 + 3*t*pow(1-t, 2)*v1 + 3*(1-t)*pow(t, 2)*v2 + pow(t, 3)*v3;
		return result;
	};

	return CncCurveLib::Point( 	calculateCubicBezierParameter(ps.p0.x, ps.p1.x, ps.p2.x, ps.p3.x, t),
								calculateCubicBezierParameter(ps.p0.y, ps.p1.y, ps.p2.y, ps.p3.y, t)
	);
}
//////////////////////////////////////////////////////////////////
const CncCurveLib::Point CncCurveLib::getPointOnEllipticalArc(CncCurveLib::ParameterSet& ps, const float t) {
//////////////////////////////////////////////////////////////////
	// If the endpoints are identical, then this is equivalent to omitting the elliptical arc segment entirely.
	if( ps.p0 == ps.p1 ) return ps.p0;

	// If rx = 0 or ry = 0 then this arc is treated as a straight line segment joining the endpoints.
	if( ps.rx == 0 || ps.ry == 0) return getPointOnLine(ps, t);

	// From http://www.w3.org/TR/SVG/implnote.html#ArcParameterizationAlternatives
	float angle              = ps.EAPCI.startAngle + (ps.EAPCI.sweepAngle * t);
	double ellipseComponentX = ps.rx * cos(angle);
	double ellipseComponentY = ps.ry * sin(angle);

	// Attach some extra info to use
	ps.EARI.ellipticalArcCenter 	= CncCurveLib::Point(ps.EAPCI.center.x, ps.EAPCI.center.y);
	ps.EARI.ellipticalArcStartAngle = ps.EAPCI.startAngle;
	ps.EARI.ellipticalArcEndAngle 	= ps.EAPCI.startAngle + ps.EAPCI.sweepAngle;
	ps.EARI.ellipticalArcAngle 		= angle;
	ps.EARI.resultantRx 			= ps.rx;
	ps.EARI.resultantRy 			= ps.ry;

	auto point = CncCurveLib::Point(
		cos(ps.EAPCI.xAxisRotationRadians)*ellipseComponentX - sin(ps.EAPCI.xAxisRotationRadians)*ellipseComponentY + ps.EAPCI.center.x,
		sin(ps.EAPCI.xAxisRotationRadians)*ellipseComponentX + cos(ps.EAPCI.xAxisRotationRadians)*ellipseComponentY + ps.EAPCI.center.y
	);

	return point;
}
//////////////////////////////////////////////////////////////////
bool CncCurveLib::callback(const CncCurveLib::Point& p) {
//////////////////////////////////////////////////////////////////
	if ( caller == NULL )
		return false;

	return caller->callback(p);
}
//////////////////////////////////////////////////////////////////
bool CncCurveLib::render(CncCurveLib::ParameterSet& ps) {
//////////////////////////////////////////////////////////////////
	if ( ps.getType() != type ) {
		std::cerr << "Incompatible parameter set: ["
				  << type << " != " << ps.getType() << "]"
				  << std::endl;
		return false;
	}
	
	// ----------------------------------------------------------
	// step 1: prepare parameters
		ps.prepare();

	// ----------------------------------------------------------
	// step 2: approximate the curve length
		ps.RI.curveLength = 0.0;
		Point p0 = (this->*renderFunc)(ps, 0.0);
		Point p1;

		ps.RI.samples = setup.approximation.samples ? setup.approximation.samples : 50;

		const float factor = (1.0f / ps.RI.samples);
		for(unsigned int i = 0; i < ps.RI.samples; i++) {
			float t = clamp(i * factor, 0.0f, 1.0f);

			p1 = (this->*renderFunc)(ps, t);
			ps.RI.curveLength += distance(p0, p1);

			p0 = p1;
		}
		
		// stretch to end point
		p1 = (this->*renderFunc)(ps, 1.0);
		ps.RI.curveLength += distance(p0, p1);
		
	// ----------------------------------------------------------
	// step 3: determine the render increment
		ps.RI.resolution	= setup.resolution.size ? setup.resolution.size : 1;
		ps.RI.steps 		= setup.resolution.size ? ps.RI.curveLength / setup.resolution.size : 1 ;

		if ( ps.RI.steps > 1 )	ps.RI.increment = clamp(1.0 / ps.RI.steps, 0.0001, 0.99);
		else					ps.RI.increment = 1.0;

		if ( false ) {
			ps.trace(std::cout); 
			std::cout << std::endl;
			
			std::cout << CNC_LOG_FUNCT << " ps.RI.samples    : " << ps.RI.samples		<< std::endl;
			std::cout << CNC_LOG_FUNCT << " ps.RI.resolution : " << ps.RI.resolution	<< std::endl;
			std::cout << CNC_LOG_FUNCT << " ps.RI.curveLength: " << ps.RI.curveLength	<< std::endl;
			std::cout << CNC_LOG_FUNCT << " ps.RI.steps      : " << ps.RI.steps			<< std::endl;
			std::cout << CNC_LOG_FUNCT << " ps.RI.increment  : " << ps.RI.increment		<< std::endl;
		}
		
	// ----------------------------------------------------------
	// step 4: render the function
		Point p;

		for ( float t = 0.0f; t <1.0f; t += ps.RI.increment ) {
			p = (this->*renderFunc)(ps, t);
			callback(p);
		}

		// stretch to end point
		p = (this->*renderFunc)(ps, 1.0);
		callback(p);
		ps.RI.steps++;

	return true;
}
//////////////////////////////////////////////////////////////////
void CncCurveLib::ParameterElliptical::prepare() {
//////////////////////////////////////////////////////////////////
	EAPCI.preCalculated = true;
	rx					= fabs(rx);
	ry					= fabs(ry);
	xAxisRotation		= fmod(xAxisRotation, 360.0f);

	EAPCI.xAxisRotationRadians = CncCurveLib::toRadians(xAxisRotation);

	// Following "Conversion from endpoint to center parameterization"
	// http://www.w3.org/TR/SVG/implnote.html#ArcConversionEndpointToCenter

	// -----------------------------------------------------------
	// Step #1: Compute transformedPoint
		double dx = (p0.x - p1.x)/2;
		double dy = (p0.y - p1.y)/2;

		auto transformedPoint = CncCurveLib::Point(
			+cos(EAPCI.xAxisRotationRadians) * dx + sin(EAPCI.xAxisRotationRadians) * dy,
			-sin(EAPCI.xAxisRotationRadians) * dx + cos(EAPCI.xAxisRotationRadians) * dy
		);

		// Ensure radius are large enough
		double radiiCheck = pow(transformedPoint.x, 2)/pow(rx, 2) + pow(transformedPoint.y, 2)/pow(ry, 2);
		if( radiiCheck > 1 ) {
			rx = sqrt(radiiCheck) * rx;
			ry = sqrt(radiiCheck) * ry;
		}

	// -----------------------------------------------------------
	// Step #2: Compute transformedCenter
		double cSquareNumerator = pow(rx, 2)*pow(ry, 2) - pow(rx, 2)*pow(transformedPoint.y, 2) - pow(ry, 2)*pow(transformedPoint.x, 2);
		double cSquareRootDenom = pow(rx, 2)*pow(transformedPoint.y, 2) + pow(ry, 2)*pow(transformedPoint.x, 2);
		double cRadicand        = cSquareNumerator/cSquareRootDenom;

		// Make sure this never drops below zero because of precision
		cRadicand = cRadicand < 0 ? 0 : cRadicand;
		double cCoef = (largeArcFlag != sweepFlag ? 1 : -1) * sqrt(cRadicand);

		auto transformedCenter = CncCurveLib::Point(
			cCoef*(+(rx * transformedPoint.y) / ry),
			cCoef*(-(ry * transformedPoint.x) / rx)
		);

	// -----------------------------------------------------------
	// Step #3: Compute center
		EAPCI.center = CncCurveLib::Point(
			cos(EAPCI.xAxisRotationRadians) * transformedCenter.x - sin(EAPCI.xAxisRotationRadians) * transformedCenter.y + ((p0.x + p1.x)/2),
			sin(EAPCI.xAxisRotationRadians) * transformedCenter.x + cos(EAPCI.xAxisRotationRadians) * transformedCenter.y + ((p0.y + p1.y)/2)
		);

	// -----------------------------------------------------------
	// Step #4: Compute start/sweep angles
		// Start angle of the elliptical arc prior to the stretch and rotate operations.
		// Difference between the start and end angles
		auto startVector = CncCurveLib::Point(
			(transformedPoint.x-transformedCenter.x) / rx,
			(transformedPoint.y-transformedCenter.y) / ry
		);

		EAPCI.startAngle = CncCurveLib::angleBetween(CncCurveLib::Point(1.0f, 0.0f), startVector);

		auto endVector = CncCurveLib::Point(
			(-transformedPoint.x - transformedCenter.x) / rx,
			(-transformedPoint.y - transformedCenter.y) / ry
		);

		EAPCI.sweepAngle = CncCurveLib::angleBetween(startVector, endVector);

		if     ( !sweepFlag && EAPCI.sweepAngle > 0)	EAPCI.sweepAngle -= 2 * PI;
		else if(  sweepFlag && EAPCI.sweepAngle < 0)	EAPCI.sweepAngle += 2 * PI;

		// We use % instead of `mod(..)` because we want it to be -360deg to 360deg(but actually in radians)
		EAPCI.sweepAngle = fmod(EAPCI.sweepAngle, 2 * PI);
}

















/*
//////////////////////////////////////////////////////////////////
const CncCurveLib::Point CncCurveLib::getPointOnEllipticalArcOld(CncCurveLib::ParameterSet& ps, const float t) {
//////////////////////////////////////////////////////////////////
	ps.rx = fabs(ps.rx);
	ps.ry = fabs(ps.ry);

	ps.xAxisRotation = fmod(ps.xAxisRotation, 360.0f);

	float xAxisRotationRadians = CncCurveLib::toRadians(ps.xAxisRotation);

	// If the endpoints are identical, then this is equivalent to omitting the elliptical arc segment entirely.
	if( ps.p0 == ps.p1 ) return ps.p0;

	// If rx = 0 or ry = 0 then this arc is treated as a straight line segment joining the endpoints.
	if( ps.rx == 0 || ps.ry == 0) return getPointOnLine(ps, t);

	// Following "Conversion from endpoint to center parameterization"
	// http://www.w3.org/TR/SVG/implnote.html#ArcConversionEndpointToCenter

	// Step #1: Compute transformedPoint
	double dx = (ps.p0.x - ps.p1.x)/2;
	double dy = (ps.p0.y - ps.p1.y)/2;

	auto transformedPoint = CncCurveLib::Point(
		+cos(xAxisRotationRadians) * dx + sin(xAxisRotationRadians) * dy,
		-sin(xAxisRotationRadians) * dx + cos(xAxisRotationRadians) * dy
	);

	// Ensure radius are large enough
	double radiiCheck = pow(transformedPoint.x, 2)/pow(ps.rx, 2) + pow(transformedPoint.y, 2)/pow(ps.ry, 2);
	if( radiiCheck > 1 ) {
		ps.rx = sqrt(radiiCheck) * ps.rx;
		ps.ry = sqrt(radiiCheck) * ps.ry;
	}

	// Step #2: Compute transformedCenter
	double cSquareNumerator = pow(ps.rx, 2)*pow(ps.ry, 2) - pow(ps.rx, 2)*pow(transformedPoint.y, 2) - pow(ps.ry, 2)*pow(transformedPoint.x, 2);
	double cSquareRootDenom = pow(ps.rx, 2)*pow(transformedPoint.y, 2) + pow(ps.ry, 2)*pow(transformedPoint.x, 2);
	double cRadicand        = cSquareNumerator/cSquareRootDenom;

	// Make sure this never drops below zero because of precision
	cRadicand = cRadicand < 0 ? 0 : cRadicand;
	double cCoef = (ps.largeArcFlag != ps.sweepFlag ? 1 : -1) * sqrt(cRadicand);

	auto transformedCenter = CncCurveLib::Point(
		cCoef*(+(ps.rx * transformedPoint.y) / ps.ry),
		cCoef*(-(ps.ry * transformedPoint.x) / ps.rx)
	);

	// Step #3: Compute center
	auto center = CncCurveLib::Point(
		cos(xAxisRotationRadians) * transformedCenter.x - sin(xAxisRotationRadians) * transformedCenter.y + ((ps.p0.x + ps.p1.x)/2),
		sin(xAxisRotationRadians) * transformedCenter.x + cos(xAxisRotationRadians) * transformedCenter.y + ((ps.p0.y + ps.p1.y)/2)
	);

	// Step #4: Compute start/sweep angles
	// Start angle of the elliptical arc prior to the stretch and rotate operations.
	// Difference between the start and end angles
	auto startVector = CncCurveLib::Point(
		(transformedPoint.x-transformedCenter.x) / ps.rx,
		(transformedPoint.y-transformedCenter.y) / ps.ry
	);

	float startAngle = CncCurveLib::angleBetween(CncCurveLib::Point(1.0f, 0.0f), startVector);

	auto endVector = CncCurveLib::Point(
		(-transformedPoint.x - transformedCenter.x) / ps.rx,
		(-transformedPoint.y - transformedCenter.y) / ps.ry
	);

	float sweepAngle = CncCurveLib::angleBetween(startVector, endVector);

	if     ( !ps.sweepFlag && sweepAngle > 0)	sweepAngle -= 2 * PI;
	else if(  ps.sweepFlag && sweepAngle < 0)	sweepAngle += 2 * PI;

	// We use % instead of `mod(..)` because we want it to be -360deg to 360deg(but actually in radians)
	sweepAngle = fmod(sweepAngle, 2 * PI);

	// From http://www.w3.org/TR/SVG/implnote.html#ArcParameterizationAlternatives
	float angle              = startAngle + (sweepAngle * t);
	double ellipseComponentX = ps.rx * cos(angle);
	double ellipseComponentY = ps.ry * sin(angle);

	// Attach some extra info to use
	ps.EARI.ellipticalArcCenter 	= CncCurveLib::Point(center.x, center.y);
	ps.EARI.ellipticalArcStartAngle = startAngle;
	ps.EARI.ellipticalArcEndAngle 	= startAngle + sweepAngle;
	ps.EARI.ellipticalArcAngle 		= angle;
	ps.EARI.resultantRx 			= ps.rx;
	ps.EARI.resultantRy 			= ps.ry;

	auto point = CncCurveLib::Point(
		cos(xAxisRotationRadians)*ellipseComponentX - sin(xAxisRotationRadians)*ellipseComponentY + center.x,
		sin(xAxisRotationRadians)*ellipseComponentX + cos(xAxisRotationRadians)*ellipseComponentY + center.y
	);

	return point;
}
*/ 
