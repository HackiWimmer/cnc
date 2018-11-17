#ifndef CNC_CURVE_LIB_H
#define CNC_CURVE_LIB_H

#include <iostream>

/////////////////////////////////////////////////////////////////////////////
class CncCurveLib {

	public:

		const float PI = 3.14159265359f;
		enum Type {Line, Elliptical, QuadraticBezier, CubicBezier};

		struct Point{

			public:

				Point() 					: x(0),  y(9)  	{}
				Point(float x1, float y1)	: x(x1), y(y1) 	{}
				Point(double x1, double y1)	: x((float)x1), y((float)y1) 	{}
				Point(const Point& o)		: x(o.x), y(o.y){}

				float x;
				float y;

				double xDbl() { return (double)x; }
				double yDbl() { return (double)y; }

				bool operator == (const Point &v) { return ( x == v.x ) && ( y == v.y ); }
				bool operator != (const Point &v) { return !this->operator==(v); }
		};

		struct ParameterSet {

			protected:

				Type type = Type::Line;

			public:

				Point	p0;
				Point 	p1;
				Point 	p2;
				Point 	p3;

				double 	rx;
				double 	ry;
				double 	xAxisRotation;

				bool 	largeArcFlag;
				bool 	sweepFlag;

				const Type getType() const { return type; }

				struct EllipticalArcReturnInfo {

					CncCurveLib::Point ellipticalArcCenter;

					float ellipticalArcStartAngle 	=   0;
					float ellipticalArcEndAngle 	=   0;
					float ellipticalArcAngle 		=   0;
					float resultantRx 				=   0;
					float resultantRy 				=   0;

				} EARI;

				struct RenderInfo {
					float curveLength				=   0;
					unsigned int samples			=   0;
					unsigned int steps				=   0;
					float increment					= 0.0;
					float resolution				= 1.0;
				} RI;

		};

	public:

		struct ParameterLine 				: public ParameterSet { ParameterLine           () {type = Type::Line; }            };
		struct ParameterElliptical 			: public ParameterSet { ParameterElliptical     () {type = Type::Elliptical; }      };
		struct ParameterQuadraticBezier 	: public ParameterSet { ParameterQuadraticBezier() {type = Type::QuadraticBezier; } };
		struct ParameterCubicBezier 		: public ParameterSet { ParameterCubicBezier    () {type = Type::CubicBezier; }     };

		struct Setup {

			struct Approximation {
				unsigned int samples	= 50;
			} approximation;

			struct Resolution {
				float size				= 0.2;
			} resolution;

		} setup;


		class Caller {

			public:
				Caller() {}
				virtual ~Caller() {}

				friend CncCurveLib;

			protected:
				virtual bool callback(const CncCurveLib::Point& p) = 0;

		};

		// common interface
		void init(const CncCurveLib::Setup& s);
		const CncCurveLib::Setup& getSetup() { return setup; }

		bool render(CncCurveLib::ParameterSet& ps);


		const Point getPointOnLine                 (CncCurveLib::ParameterSet& ps, float t);
		const Point getPointOnEllipticalArc        (CncCurveLib::ParameterSet& ps, float t);
		const Point getPointOnQuadraticBezierCurve (CncCurveLib::ParameterSet& ps, float t);
		const Point getPointOnCubicBezierCurve     (CncCurveLib::ParameterSet& ps, float t);

	protected:

		CncCurveLib(Caller* c)
		: caller(c)
		, type(Type::Line)
		{
			renderFunc = &CncCurveLib::getPointOnLine;
		}

		virtual ~CncCurveLib() {}

		Caller* caller;
		Type type;

		const Point (CncCurveLib::*renderFunc) 	   (CncCurveLib::ParameterSet& ps, float t);

		bool callback(const CncCurveLib::Point& p);

		float clamp(float val, float minVal, float maxVal);
		float toRadians(float angle);
		float angleBetween(const CncCurveLib::Point& v0, const CncCurveLib::Point& v1);
		float distance(const CncCurveLib::Point& p0, const CncCurveLib::Point& p1);

};

/////////////////////////////////////////////////////////////////////////////
class CncLineCurve : public CncCurveLib {

	private:

		ParameterLine parameter;

	public:
		CncLineCurve(Caller* c)
		: CncCurveLib(c) {
			renderFunc = &CncCurveLib::getPointOnLine;
			type = Type::Line;
		}
		virtual ~CncLineCurve() {}

		ParameterLine& getParameterSet() { return parameter; }
};

/////////////////////////////////////////////////////////////////////////////
class CncEllipticalCurve : public CncCurveLib {

	private:

		ParameterElliptical parameter;

	public:
		CncEllipticalCurve(Caller* c)
		: CncCurveLib(c) {
			renderFunc = &CncCurveLib::getPointOnEllipticalArc;
			type = Type::Elliptical;
		}
		virtual ~CncEllipticalCurve() {}

		ParameterElliptical& getParameterSet() { return parameter; }
};

/////////////////////////////////////////////////////////////////////////////
class CncQuadraticBezierCurve : public CncCurveLib {

	private:

		ParameterQuadraticBezier parameter;

	public:
		CncQuadraticBezierCurve(Caller* c)
		: CncCurveLib(c) {
			renderFunc = &CncCurveLib::getPointOnQuadraticBezierCurve;
			type = Type::QuadraticBezier;
		}
		virtual ~CncQuadraticBezierCurve() {}

		ParameterQuadraticBezier& getParameterSet() { return parameter; }

};

/////////////////////////////////////////////////////////////////////////////
class CncCubicBezierCurve : public CncCurveLib {

	private:

		ParameterCubicBezier parameter;

	public:
		CncCubicBezierCurve(Caller* c)
		: CncCurveLib(c) {
			renderFunc = &CncCurveLib::getPointOnCubicBezierCurve;
			type = Type::CubicBezier;
		}
		virtual ~CncCubicBezierCurve() {}

		ParameterCubicBezier& getParameterSet() { return parameter; }

};

#endif