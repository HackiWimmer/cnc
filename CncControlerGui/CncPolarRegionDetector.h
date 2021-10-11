#ifndef CNC_POLAR_REGION_DETECTOR
#define CNC_POLAR_REGION_DETECTOR

#include <cmath>
#include "CncCommon.h"

class CncPolarRegionDetector 
{
	public:
	
		typedef std::vector<double> Angles;
		static const Angles& getAngles() { return angles; }
		
		CncPolarRegionDetector()
		: xAbs		(0.0)
		, yAbs		(0.0)
		, xDir		(CncNoneDir)
		, yDir		(CncNoneDir)
		{}
		
		template <typename T>
		CncPolarRegionDetector(T x, T y)
		: xAbs		((double)x)
		, yAbs		((double)y)
		, xDir		(CncNoneDir)
		, yDir		(CncNoneDir)
		{
			calculate();
		}
		
		~CncPolarRegionDetector()
		{}
		
		template <typename T>
		void setCoordinatesAbs(T x, T y)
		{
			xAbs = (double)x;
			yAbs = (double)y;
			
			calculate();
		}
		
		void calculate()
		{
			if ( cnc::dblCmp::nu(xAbs) && cnc::dblCmp::nu(yAbs) )
			{
				xDir = CncNoneDir;
				yDir = CncNoneDir;
			}
			else
			{
				// ensure -180 ... 180 degree
				const double a = getAngle180();
				
				if ( cnc::dblCmp::ge(a, 0.0) )
				{
					if (      a >=    0.0 && a <  +67.5 )		xDir = CncPosDir;
					else if ( a >  +112.5 && a < +180.0 )		xDir = CncNegDir;
					else										xDir = CncNoneDir;
					
					if (      a >=  +22.5 && a < +157.5 )		yDir = CncPosDir;
					else										yDir = CncNoneDir;
				}
				else
				{
					if (      a <=    0.0 && a >  -67.5 )		xDir = CncPosDir;
					else if ( a <  -112.5 && a > -180.0 )		xDir = CncNegDir;
					else										xDir = CncNoneDir;

					if (      a <=  -22.5 && a > -157.5 )		yDir = CncNegDir;
					else										yDir = CncNoneDir;
				}
			}
			//std::cout << a << ": " << getDirectionX() << "," << getDirectionY()<< std::endl;
		}
		
		void				reset() 				{ *this = CncPolarRegionDetector(); }
		double				getAngle180()	const	{ return atan2(yAbs, xAbs) * 180 / PI; }
		double				getAngle360()	const	{ const double a = getAngle180(); return ( a < 0.0 ? 180.0 + (180.0 + a) : a ); }
		double				getLength()		const	{ return sqrt(xAbs * xAbs + yAbs * yAbs); }
		CncLinearDirection	getDirectionX() const	{ return xDir; }
		CncLinearDirection	getDirectionY() const	{ return yDir; }
		
	private:
		
		static Angles		angles;
		
		double				xAbs;
		double				yAbs;
		CncLinearDirection	xDir;
		CncLinearDirection	yDir;
};

#endif