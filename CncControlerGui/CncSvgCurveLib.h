#ifndef CNC_SVG_CURVE_LIB_H
#define CNC_SVG_CURVE_LIB_H

#include "SvgUnitCalculator.h"
#include "SVGCurveLib.h"

/////////////////////////////////////////////////////////////////////////
struct ArcParameter {
	SVGCurveLib::PointGeneric<> p0;
	SVGCurveLib::PointGeneric<> p1;
	double 	rx; 
	double	ry;
	double 	xAxisRotation; 
	bool 	largeArcFlag; 
	bool 	sweepFlag;
 
	SVGCurveLib::PointGeneric<> (*pointOnCurveFunc)(float);
};

/////////////////////////////////////////////////////////////////////////
struct QuadraticBezierParameter {
	SVGCurveLib::PointGeneric<> p0;
	SVGCurveLib::PointGeneric<> p1;
	SVGCurveLib::PointGeneric<> p2;
	
	SVGCurveLib::PointGeneric<> (*pointOnCurveFunc)(float);
};

/////////////////////////////////////////////////////////////////////////
struct CubicBezierParameter {
	SVGCurveLib::PointGeneric<> p0;
	SVGCurveLib::PointGeneric<> p1;
	SVGCurveLib::PointGeneric<> p2;
	SVGCurveLib::PointGeneric<> p3;
	
	SVGCurveLib::PointGeneric<> (*pointOnCurveFunc)(float);
};

/////////////////////////////////////////////////////////////////////////
struct CurveInfo {
	public:
		float	increment;
		float	length;
		
		void init() {
			increment 	= -1.0f;
			length		= 0.0f;
			
		}
};

/////////////////////////////////////////////////////////////////////////
class CncSvgCurveLib : public SVGCurveLib {
	
	private:
		static float tIncrement;
		
		static CurveInfo _curveInfoResult;
		
	public:
		// default constant
		static const float autoIncrement;
		
		// global flag how to use the SvgCurveLib
		static bool useCncSvgCurveLib;
				
		// globale structures
		static ArcParameter AP;
		static QuadraticBezierParameter QP;
		static CubicBezierParameter CP;
		
		/////////////////////////////////////////////////////////////////////////
		static void setResolution(float t) {
			if ( t > 0 && t < 1 )	tIncrement = t;
			else					tIncrement = autoIncrement;
		}
		
		/////////////////////////////////////////////////////////////////////////
		class LinearCurve  {
			public:
				SVGCurveLib::ArcLengthInfo resultantInfo;
		};
			
		/////////////////////////////////////////////////////////////////////////
		class LinearArcCurve : public LinearCurve {
			public:

				//......................................................................
				LinearArcCurve(SVGCurveLib::PointGeneric<> p0, double rx, double ry, 
				               double xAxisRotation, bool largeArcFlag, bool sweepFlag, 
							   SVGCurveLib::PointGeneric<> p1) {
					
					AP.p0				= p0; 
					AP.rx 				= rx;
					AP.ry 				= ry;
					AP.xAxisRotation	= xAxisRotation; 
					AP.largeArcFlag		= largeArcFlag; 
					AP.sweepFlag		= sweepFlag; 
					AP.p1				= p1;
					
					AP.pointOnCurveFunc = LinearArcCurve::pointOnCurve;
				}
			
				//......................................................................
				static SVGCurveLib::PointGeneric<> pointOnCurve(float t) {
					return PointOnEllipticalArc(AP.p0, AP.rx, AP.ry, AP.xAxisRotation, AP.largeArcFlag, AP.sweepFlag, AP.p1, t);
				}
				
				//......................................................................
				static SVGCurveLib::PointGeneric<> getFp(float t) { 
					return AP.pointOnCurveFunc(t); 
				}

		};
		
		/////////////////////////////////////////////////////////////////////////
		class LinearQuadraticBezierCurve : public LinearCurve{
			public:
			
				//......................................................................
				LinearQuadraticBezierCurve(SVGCurveLib::PointGeneric<> p0,
				                           SVGCurveLib::PointGeneric<> p1,
										   SVGCurveLib::PointGeneric<> p2) {
					
					QP.p0 = p0;
					QP.p1 = p1;
					QP.p2 = p2;
					
					QP.pointOnCurveFunc = LinearQuadraticBezierCurve::pointOnCurve;
				}

				//......................................................................
				static SVGCurveLib::PointGeneric<> pointOnCurve(float t) {
					return PointOnQuadraticBezierCurve(QP.p0, QP.p1, QP.p2, t);
				}
				
				//......................................................................
				static SVGCurveLib::PointGeneric<> getFp(float t) { 
					return QP.pointOnCurveFunc(t); 
				}
		};
		
		/////////////////////////////////////////////////////////////////////////
		class LinearCubicBezierCurve : public LinearCurve {
			public:
			
				//......................................................................
				LinearCubicBezierCurve(SVGCurveLib::PointGeneric<> p0,
				                       SVGCurveLib::PointGeneric<> p1,
									   SVGCurveLib::PointGeneric<> p2,
									   SVGCurveLib::PointGeneric<> p3) {
					
					CP.p0 = p0;
					CP.p1 = p1;
					CP.p2 = p2;
					CP.p3 = p3;
					
					CP.pointOnCurveFunc = LinearCubicBezierCurve::pointOnCurve;
				}
				
				//......................................................................
				static SVGCurveLib::PointGeneric<> pointOnCurve(float t) {
					return PointOnCubicBezierCurve(CP.p0, CP.p1, CP.p2, CP.p3, t);
				}
				
				//......................................................................
				static SVGCurveLib::PointGeneric<> getFp(float t) { 
					return CP.pointOnCurveFunc(t); 
				}
		};

	/////////////////////////////////////////////////////////////////////////
	static CurveInfo& getCurveInfo(SVGCurveLib::LinearCurve lc);
	
	/////////////////////////////////////////////////////////////////////////
	static unsigned int getDefaultResolution() {
		return SvgUnitCalculator::getDPI();
	}

};

#endif