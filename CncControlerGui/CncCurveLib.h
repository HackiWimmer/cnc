#ifndef CNC_CURVE_LIB_H
#define CNC_CURVE_LIB_H

#include <iostream>

/////////////////////////////////////////////////////////////////////////////
class CncCurveLib {

	public:
		
		static const float PI;
		enum Type {Line, Elliptical, QuadraticBezier, CubicBezier};
		
		// -----------------------------------------------------------------
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
				
				friend std::ostream &operator<< (std::ostream &ostr, const Point &p) {
					ostr << "(" << p.x << ", " << p.y << ")";
					return ostr;
				}
		};
		
		// -----------------------------------------------------------------
		struct ParameterSet {
			protected:
				Type type = Type::Line;
				
			public:

				Point	p0;
				Point	p1;
				Point	p2;
				Point	p3;

				double	rx								= 0.0;
				double	ry								= 0.0;
				double	xAxisRotation					= 0.0;

				bool	largeArcFlag					= false;
				bool	sweepFlag						= false;
				
				virtual void prepare()					= 0;
				virtual void trace(std::ostream& ostr)	= 0;
				
				const Type getType() const { return type; }
				
				// ---------------------------------------------------------
				struct EllipticalArcPreCalcInfo {
					CncCurveLib::Point center;
					bool	preCalculated					= false;
					float	xAxisRotationRadians			= 0.0;
					float	startAngle						= 0.0;
					float	sweepAngle						= 0.0;
					
					void reset() {
						*this = EllipticalArcPreCalcInfo();
					}
				} EAPCI;
				
				// ---------------------------------------------------------
				struct EllipticalArcReturnInfo {
					CncCurveLib::Point ellipticalArcCenter;
					float ellipticalArcStartAngle			=   0;
					float ellipticalArcEndAngle				=   0;
					float ellipticalArcAngle				=   0;
					float resultantRx						=   0;
					float resultantRy						=   0;
				} EARI;
				
				// ---------------------------------------------------------
				struct RenderInfo {
					float curveLength						=   0;
					unsigned int samples					=   0;
					unsigned int steps						=   0;
					float increment							= 0.0;
					float resolution						= 1.0;
				} RI;
				
		};

		// -----------------------------------------------------------------
		struct ParameterLine : public ParameterSet { 
			ParameterLine() 
			{ type = Type::Line; }
			
			virtual void prepare() 					{}
			virtual void trace(std::ostream& ostr)	{ ostr << *this; }

			friend std::ostream &operator<< (std::ostream &ostr, const ParameterLine &para) {
				ostr	<< "CncCurveLib ParameterLine:"				<< std::endl
						<< " P0 : "	<< para.p0						<< std::endl
						<< " P1 : "	<< para.p1						<< std::endl
						;
				return ostr;
			}
		};
		
		// -----------------------------------------------------------------
		struct ParameterElliptical : public ParameterSet { 
			
			ParameterElliptical() 
			{ type = Type::Elliptical; }
			
			virtual void prepare();
			virtual void trace(std::ostream& ostr)	{ ostr << *this; }

			friend std::ostream &operator<< (std::ostream &ostr, const ParameterElliptical &para) {
				ostr	<< "CncCurveLib ParameterElliptical:"				<< std::endl
						<< " P0            : " << para.p0					<< std::endl
						<< " P1            : " << para.p1					<< std::endl
						<< " rx            : " << para.rx					<< std::endl
						<< " ry            : " << para.ry					<< std::endl
						<< " xAxisRotation : " << para.xAxisRotation		<< std::endl
						<< " largeArcFlag  : " << para.largeArcFlag			<< std::endl
						<< " sweepFlag     : " << para.sweepFlag			<< std::endl
						
						<< " Precalculation:"												<< std::endl
						<< "  preCalculated        : " << para.EAPCI.preCalculated			<< std::endl
						<< "  startAngle           : " << para.EAPCI.startAngle				<< std::endl
						<< "  sweepAngle           : " << para.EAPCI.sweepAngle				<< std::endl
						<< "  xAxisRotationRadians : " << para.EAPCI.xAxisRotationRadians	<< std::endl
						<< "  center               : " << para.EAPCI.center					<< std::endl
						;
				return ostr;
			}
		};
		
		// -----------------------------------------------------------------
		struct ParameterQuadraticBezier : public ParameterSet { 
			ParameterQuadraticBezier() 
			{ type = Type::QuadraticBezier; }
			
			virtual void prepare()					{}
			virtual void trace(std::ostream& ostr)	{ ostr << *this; }

			friend std::ostream &operator<< (std::ostream &ostr, const ParameterQuadraticBezier &para) {
				ostr	<< "CncCurveLib ParameterQuadraticBezier:"	<< std::endl
						<< " P0 : "	<< para.p0						<< std::endl
						<< " P1 : "	<< para.p1						<< std::endl
						<< " P2 : "	<< para.p2						<< std::endl
						;
				return ostr;
			}
		};
		
		// -----------------------------------------------------------------
		struct ParameterCubicBezier : public ParameterSet { 
			ParameterCubicBezier () 
			{ type = Type::CubicBezier; }
			
			virtual void prepare()					{}
			virtual void trace(std::ostream& ostr)	{ ostr << *this; }
			
			friend std::ostream &operator<< (std::ostream &ostr, const ParameterCubicBezier &para) {
				ostr	<< "CncCurveLib ParameterCubicBezier:"		<< std::endl
						<< " P0 : "	<< para.p0						<< std::endl
						<< " P1 : "	<< para.p1						<< std::endl
						<< " P2 : "	<< para.p2						<< std::endl
						<< " P3 : "	<< para.p3						<< std::endl
						;
				return ostr;
			}
		};
		
		// -----------------------------------------------------------------
		struct Setup {
			struct Approximation {
				unsigned int samples	= 50;
			} approximation;

			struct Resolution {
				float size				= 0.2;
			} resolution;

			friend std::ostream &operator<< (std::ostream &ostr, const Setup &s) {
				ostr	<< "CncCurveLib Setup:"										<< std::endl
						<< " Approximation samples : "	<< s.approximation.samples	<< std::endl
						<< " Resolution size       : "	<< s.resolution.size		<< std::endl
						;
						
				return ostr;
			}
			
		} setup;
		
		// -----------------------------------------------------------------
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
		
		explicit CncCurveLib(Caller* c)
		: caller	(c)
		, type		(Type::Line)
		{
			renderFunc = &CncCurveLib::getPointOnLine;
		}
		
		virtual ~CncCurveLib() 
		{}
		
		Caller* caller;
		Type type;
		
		// render function pointer
		const Point (CncCurveLib::*renderFunc)(CncCurveLib::ParameterSet& ps, float t);
		
		bool callback(const CncCurveLib::Point& p);

	public:
		
		static float clamp(float val, float minVal, float maxVal);
		static float toRadians(float angle);
		static float angleBetween(const CncCurveLib::Point& v0, const CncCurveLib::Point& v1);
		static float distance(const CncCurveLib::Point& p0, const CncCurveLib::Point& p1);

		class LastControlPoint {
			private:
				CncCurveLib::Point point;
				bool valid;

			public:
				LastControlPoint()
				: point(0.0, 0.0)
				, valid(false)
				{}

				~LastControlPoint() {}

				bool hasControlPoint() 	{ return valid; }

				void setControlPoint(const CncCurveLib::Point& currentPoint, const CncCurveLib::Point& p) {
					valid = true;

					//The first control point is assumed to be the reflection of the last control point
					//on the previous command relative to the current point.
					point = {(currentPoint.x + (currentPoint.x - p.x)),
						     (currentPoint.y + (currentPoint.y - p.y))};
				}

				void reset() {
					valid 	= false;
					point	= {0.0, 0.0};
				}

				const CncCurveLib::Point& getLastControlPoint(const CncCurveLib::Point& currentPoint) {
					if ( hasControlPoint() )
						return point;

					// If there is no previous command or if the previous command was not an beziert + curve,
					// assume the first control point is identically  with the current point
					return currentPoint;
				}
		};
};

/////////////////////////////////////////////////////////////////////////////
class CncLineCurve : public CncCurveLib {

	private:
		ParameterLine parameter;

	public:
		explicit CncLineCurve(Caller* c)
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
		explicit CncEllipticalCurve(Caller* c)
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
		explicit CncQuadraticBezierCurve(Caller* c)
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
		explicit CncCubicBezierCurve(Caller* c)
		: CncCurveLib(c) {
			renderFunc = &CncCurveLib::getPointOnCubicBezierCurve;
			type = Type::CubicBezier;
		}
		virtual ~CncCubicBezierCurve() {}

		ParameterCubicBezier& getParameterSet() { return parameter; }
};

#endif
