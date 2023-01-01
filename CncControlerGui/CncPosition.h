#ifndef CNC_POSITION_H
#define CNC_POSITION_H

#include <cmath> 
#include <ctgmath>
#include <iomanip>
#include <iostream>

#include <list>
#include <stack>

#include <wx/string.h>
#include "CncVector.h"

////////////////////////////////////////////////////////////////////////////////////////////
template <class T> 
class CncXYDimension 
{
	protected:
		T width, height;
		
	public:
		CncXYDimension<T>() 
		: width (0)
		, height(0)
		{}
		
		CncXYDimension<T>(T w, T h) 
		: width (w)
		, height(h)
		{}
		
		explicit CncXYDimension<T>(const CncXYDimension<T>& cd) 
		: width(cd.getW())
		, height(cd.getH())
		{}
		
		virtual ~CncXYDimension<T>() {}
		
		T getW() const { return width;  }
		T getH() const { return height; }
		
		void setW(T w) { width  = w; }
		void setH(T h) { height = h; }
		
		void reset()
		{
			*this = CncXYDimension<T>();
		}
		
		friend std::ostream &operator<< (std::ostream &ostr, const CncXYDimension<T> &a) 
		{
			const unsigned int	p = ostr.precision();
			const unsigned int	w = ostr.width();
			const char			f = ostr.fill();
			
			ostr	<< std::fixed << std::setw(w) << std::setprecision(p) << std::setfill(f)
					<< a.getW() << ',' 
					<< std::fixed << std::setw(w) << std::setprecision(p) << std::setfill(f)
					<< a.getH()
			;
			return ostr;
		}
		
};

typedef CncXYDimension<int16_t>	CncXYIntDimension;
typedef CncXYDimension<int32_t>	CncXYLongDimension;
typedef CncXYDimension<double>	CncXYDoubleDimension;
typedef CncXYDimension<float>	CncXYFloatDimension;

////////////////////////////////////////////////////////////////////////////////////////////
template <typename T> 
class CncZDimension 
{
	protected:
		T zDim;
		
	public:
		CncZDimension<T>()
		: zDim(0)
		{}
		
		explicit CncZDimension<T>(T z)
		: zDim(z)
		{}
		
		virtual ~CncZDimension<T>() {}
		
		T getZ() const { return zDim; }
		
		friend std::ostream &operator<< (std::ostream &ostr, const CncZDimension<T> &a) 
		{
			const unsigned int	p = ostr.precision();
			const unsigned int	w = ostr.width();
			const char			f = ostr.fill();
			
			ostr	<< std::fixed << std::setw(w) << std::setprecision(p) << std::setfill(f)
					<< a.getZ()
			;
			
			return ostr;
		}
};

typedef CncZDimension<int16_t>	CncZIntDimension;
typedef CncZDimension<int32_t>	CncZLongDimension;
typedef CncZDimension<double>	CncZDoubleDimension;
typedef CncZDimension<float>	CncZFloatDimension;

////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class CncPosition 
{
	public:
		
		static constexpr double DefaultfloatingPointEpsilon = 0.000001;
		
		enum CompareFlags
		{
			CMP_POS		=  1,
			CMP_ORG		=  2,
			CMP_WMS		=  4,
			CMP_FPE		=  8,
			CMP_MTX		= 16,
			CMP_BASE	= CMP_POS | CMP_ORG,
			CMP_ALL		= CMP_POS | CMP_ORG | CMP_WMS | CMP_FPE | CMP_MTX
		};
		
		////////////////////////////////////////////////////////////////////////////////////////////
		struct Watermarks 
		{
			public:
				T xMin = 0, xMax = 0;
				T yMin = 0, yMax = 0;
				T zMin = 0, zMax = 0;
				
				void reset()
				{
					xMin = xMax = 0;
					yMin = yMax = 0;
					zMin = zMax = 0;
				}
				
				friend std::ostream& operator<< (std::ostream &ostr, const Watermarks& o) 
				{ 
					const unsigned int	p = ostr.precision();
					const unsigned int	w = ostr.width();
					const char			f = ostr.fill();
					
					ostr	<< std::fixed << std::setw(w) << std::setprecision(p) << std::setfill(f)
							<< o.xMin << ", " 
							<< std::fixed << std::setw(w) << std::setprecision(p) << std::setfill(f)
							<< o.yMin << ", " 
							<< std::fixed << std::setw(w) << std::setprecision(p) << std::setfill(f)
							<< o.zMin << " | "
							<< std::fixed << std::setw(w) << std::setprecision(p) << std::setfill(f)
							<< o.xMax << ", " 
							<< std::fixed << std::setw(w) << std::setprecision(p) << std::setfill(f)
							<< o.yMax << ", " 
							<< std::fixed << std::setw(w) << std::setprecision(p) << std::setfill(f)
							<< o.zMax
					;
					
					return ostr;
				}
		};
		
	protected:
		
		T xOrg, yOrg, zOrg;
		T xPos, yPos, zPos;
		
		T xMin, xMax;
		T yMin, yMax;
		T zMin, zMax;

		double			floatingPointEpsilon;
		CncMatrix4x4<T>	transformationMatrix;
		
		bool compare(T v1, T v2, double fpe = DefaultfloatingPointEpsilon) const;
		
		////////////////////////////////////////////////////////////////
		inline void evaluateWatermarks()
		{
			if ( xPos < xMin ) xMin = xPos;
			if ( xPos > xMax ) xMax = xPos;
			
			if ( yPos < yMin ) yMin = yPos;
			if ( yPos > yMax ) yMax = yPos;
			
			if ( zPos < zMin ) zMin = zPos;
			if ( zPos > zMax ) zMax = zPos;
		}
		
		////////////////////////////////////////////////////////////////
		inline void setWatermarks(const Watermarks& wm) 
		{
			xMin = wm.xMin;
			xMax = wm.xMax;
			
			yMin = wm.yMin;
			yMax = wm.yMax;

			zMin = wm.zMin;
			zMax = wm.zMax;
		}
		
	public:
		
		////////////////////////////////////////////////////////////////
		CncPosition<T>()
		: xOrg(0)
		, yOrg(0)
		, zOrg(0)
		, xPos(xOrg)
		, yPos(yOrg)
		, zPos(zOrg)
		, xMin(xOrg), xMax(xOrg)
		, yMin(yOrg), yMax(yOrg)
		, zMin(zOrg), zMax(zOrg)
		, floatingPointEpsilon	(DefaultfloatingPointEpsilon)
		, transformationMatrix	()
		{}
		
		////////////////////////////////////////////////////////////////
		CncPosition<T>(T x, T y, T z)
		: xOrg(x)
		, yOrg(y)
		, zOrg(z)
		, xPos(x)
		, yPos(y)
		, zPos(z)
		, xMin(x), xMax(x)
		, yMin(y), yMax(y)
		, zMin(z), zMax(z)
		, floatingPointEpsilon	(DefaultfloatingPointEpsilon)
		, transformationMatrix	()
		{}
		
		////////////////////////////////////////////////////////////////
		/*explicit*/ CncPosition<T>(const CncPosition<T>& cp)
		: xOrg(cp.xOrg) 
		, yOrg(cp.yOrg) 
		, zOrg(cp.zOrg) 
		, xPos(cp.xPos)
		, yPos(cp.yPos)
		, zPos(cp.zPos)
		, xMin(cp.xMin), xMax(cp.xMax)
		, yMin(cp.yMin), yMax(cp.yMax)
		, zMin(cp.zMin), zMax(cp.zMax)
		, floatingPointEpsilon	(cp.floatingPointEpsilon)
		, transformationMatrix	(cp.transformationMatrix)
		{}
		
		virtual ~CncPosition<T>() {}
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& setTranslation(T x, T y, T z)			{ transformationMatrix.setTranslation(x, y, z );	return *this; }
		const CncPosition<T>& setRotationAxisX(double aDegree)		{ transformationMatrix.setRotationAxisX(aDegree);	return *this; }
		const CncPosition<T>& setRotationAxisY(double aDegree)		{ transformationMatrix.setRotationAxisY(aDegree);	return *this; }
		const CncPosition<T>& setRotationAxisZ(double aDegree)		{ transformationMatrix.setRotationAxisZ(aDegree);	return *this; }
		const CncPosition<T>& setScaling(double factor)				{ transformationMatrix.setScaling(factor);			return *this; }
		const CncPosition<T>& resetTransformation();
		
		const CncMatrix4x4<T>& getTransformationMatrix() const		{ return transformationMatrix; }
		bool hasTransformation()                         const		{ return transformationMatrix.hasTransformation(); }
		
		////////////////////////////////////////////////////////////////
		double getFloatingPointPrecision()               const		{ return floatingPointEpsilon; }
		const CncPosition<T>& setFloatingPointPrecision(double eps)	{ floatingPointEpsilon = std::fabs(eps); return *this; }
		
		////////////////////////////////////////////////////////////////
		const Watermarks& getWatermarks(Watermarks& wm) const 
		{
			wm.xMin = xMin; wm.yMin = yMin;
			wm.zMin = zMin; wm.xMax = xMax;
			wm.yMax = yMax; wm.zMax = zMax;
			
			return wm;
		}
		////////////////////////////////////////////////////////////////
		void resetWatermarks() 
		{
			xMin = xMax = 0;
			yMin = yMax = 0;
			zMin = zMax = 0;
		}
		
		////////////////////////////////////////////////////////////////
		void resetWatermarksToCurrentPos() 
		{
			xMin = xMax = getX();
			yMin = yMax = getY();
			zMin = zMax = getZ();
		}
		
		////////////////////////////////////////////////////////////////
		T getXMin() const { return xMin; }
		T getYMin() const { return yMin; } 
		T getZMin() const { return zMin; }
		void getMinWatermarks(CncPosition& min) { min.setXYZ(xMin, yMin, zMin); }
		
		////////////////////////////////////////////////////////////////
		T getXMax() const { return xMax; }
		T getYMax() const { return yMax; } 
		T getZMax() const { return zMax; }
		void getMaxWatermarks(CncPosition& max) { max.setXYZ(xMax, yMax, zMax); }
		
		////////////////////////////////////////////////////////////////
		T getX() const { return xPos; }
		T getY() const { return yPos; } 
		T getZ() const { return zPos; }
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& setX(T x)						{ return setXYZ(       x,     yOrg,     zOrg); }
		const CncPosition<T>& setY(T y)						{ return setXYZ(    xOrg,        y,     zOrg); }
		const CncPosition<T>& setZ(T z)						{ return setXYZ(    xOrg,     yOrg,        z); }
		const CncPosition<T>& setXY(T x, T y)				{ return setXYZ(       x,        y,     zOrg); } 
		const CncPosition<T>& set(const CncPosition<T>& p);
		const CncPosition<T>& setXYZ(T x, T y, T z); 
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& zeorX()						{ return setXYZ(   0, yOrg, zOrg); }
		const CncPosition<T>& zeorY()						{ return setXYZ(xOrg,    0, zOrg); }
		const CncPosition<T>& zeorZ()						{ return setXYZ(xOrg, yOrg,    0); }
		const CncPosition<T>& zeroXYZ()						{ return setXYZ(   0,    0,    0); }
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& incX(T x)						{ return inc(x, 0, 0); }
		const CncPosition<T>& incY(T y)						{ return inc(0, y, 0); }
		const CncPosition<T>& incZ(T z)						{ return inc(0, 0, z); }
		const CncPosition<T>& incXY(T x, T y)				{ return inc(x, y, 0); }
		const CncPosition<T>& shift(T x, T y, T z)			{ return inc(x, y, z); }
		const CncPosition<T>& inc(T x, T y, T z);
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& decX(T x)						{ return dec(x, 0, 0); }
		const CncPosition<T>& decY(T y)						{ return dec(0, y, 0); }
		const CncPosition<T>& decZ(T z) 					{ return dec(0, 0, z); }
		const CncPosition<T>& decXY(T x, T y)				{ return dec(x, y, 0); }
		const CncPosition<T>& dec(T x, T y, T z);
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& invertX()						{ xPos *= -1; evaluateWatermarks(); return *this; }
		const CncPosition<T>& invertY()						{ yPos *= -1; evaluateWatermarks(); return *this; }
		const CncPosition<T>& invertZ()						{ zPos *= -1; evaluateWatermarks(); return *this; }
		const CncPosition<T>& invert()						{ invertX(); invertY(); invertZ();  return *this; }
		
		////////////////////////////////////////////////////////////////
		bool isEqual(const CncPosition<T> &a, CompareFlags = CMP_BASE, double fpe = DefaultfloatingPointEpsilon) const;
		
		////////////////////////////////////////////////////////////////
		bool operator== (const CncPosition<T> &a) const;
		bool operator!= (const CncPosition<T> &a) const;
		const CncPosition<T>& operator=  (const CncPosition<T>& cp);
		const CncPosition<T>& operator+= (const CncPosition &a);
		const CncPosition<T>& operator-= (const CncPosition &a);
		const CncPosition<T>& operator++ ();
		const CncPosition<T>& operator-- ();
		CncPosition<T> operator++ (int size);
		CncPosition<T> operator-- (int size);
		CncPosition<T> operator+ (const CncPosition<T> &a) const;
		CncPosition<T> operator- (const CncPosition<T> &a) const;
		
		////////////////////////////////////////////////////////////////
		wxString asStr() const;
		wxString asFullStr() const;
		std::ostream& trace(std::ostream &ostr) const;
		std::ostream& traceAll(std::ostream &ostr, bool matrix=false) const;
		std::ostream& traceMatrix(std::ostream &ostr) const;
		std::ostream& traceWatermarks(std::ostream &ostr) const;
		friend std::ostream& operator<< (std::ostream &ostr, const CncPosition<T> &a) { return a.trace(ostr); }
};

// ----------------------------------------------------------------------
typedef CncPosition<int16_t>			CncIntPosition;
typedef CncPosition<int32_t>			CncLongPosition;
typedef CncPosition<double>				CncDoublePosition;
typedef CncPosition<float>				CncFloatPosition;

typedef std::vector<CncIntPosition>		CncIntPositionVector;
typedef std::vector<CncLongPosition>	CncLongPositionVector;
typedef std::vector<CncDoublePosition>	CncDoublePositionVector;
typedef std::vector<CncFloatPosition>	CncFloatPositionVector;

typedef std::list<CncIntPosition>		CncIntPositionList;
typedef std::list<CncLongPosition>		CncLongPositionList;
typedef std::list<CncDoublePosition>	CncDoublePositionList;
typedef std::list<CncFloatPosition>		CncFloatPositionList;

typedef std::stack<CncIntPosition>		CncIntPositionStack;
typedef std::stack<CncLongPosition>		CncLongPositionStack;
typedef std::stack<CncDoublePosition>	CncDoublePositionStack;
typedef std::stack<CncFloatPosition>	CncFloatPositionStack;

#define CncDistance						CncPosition
typedef CncPosition<int16_t>			CncIntDistance;
typedef CncPosition<int32_t>			CncLongDistance;
typedef CncPosition<double>				CncDoubleDistance;
typedef CncPosition<float>				CncFloatPosition;

typedef std::vector<CncIntDistance>		CncIntDistanceVector;
typedef std::vector<CncLongDistance>	CncLongDistanceVector;
typedef std::vector<CncDoubleDistance>	CncDoubleDistanceVector;
typedef std::vector<CncFloatPosition>	CncFloatDistanceVector;

typedef std::list<CncIntDistance>		CncIntDistanceList;
typedef std::list<CncLongDistance>		CncLongDistanceList;
typedef std::list<CncDoubleDistance>	CncDoubleDistanceList;
typedef std::list<CncFloatPosition>		CncFloatDistanceList;

typedef std::stack<CncIntDistance>		CncIntDistanceStack;
typedef std::stack<CncLongDistance>		CncLongDistanceStack;
typedef std::stack<CncDoubleDistance>	CncDoubleDistanceStack;
typedef std::stack<CncFloatPosition>	CncFloatDistanceStack;

#define CncOffset						CncPosition
typedef CncPosition<int16_t>			CncIntOffset;
typedef CncPosition<int32_t>			CncLongOffset;
typedef CncPosition<double>				CncDoubleOffset;
typedef CncPosition<float>				CncFloatOffset;

typedef std::vector<CncIntOffset>		CncIntOffsetVector;
typedef std::vector<CncLongOffset>		CncLongOffsetVector;
typedef std::vector<CncDoubleOffset>	CncDoubleOffsetVector;
typedef std::vector<CncFloatOffset>		CncFloatOffsetVector;

typedef std::list<CncIntOffset>			CncIntDistanceList;
typedef std::list<CncLongOffset>		CncLongDistanceList;
typedef std::list<CncDoubleOffset>		CncDoubleDistancList;
typedef std::list<CncFloatOffset>		CncFloatDistanceList;

typedef std::stack<CncIntOffset>		CncIntDistanceStack;
typedef std::stack<CncLongOffset>		CncLongDistanceStack;
typedef std::stack<CncDoubleOffset>		CncDoubleDistancStack;
typedef std::stack<CncFloatOffset>		CncFloatDistanceStack;

// ----------------------------------------------------------------------
//
template <class T>
class CncBoundaries : public CncPosition<T>::Watermarks 
{
	public:
		
		typedef std::vector<CncPosition<T>> Corners;
		
		CncBoundaries<T>()
		: CncPosition<T>::Watermarks()
		{}
		
		CncBoundaries<T>(const CncPosition<T>& minBound, const CncPosition<T>& maxBound)
		: CncPosition<T>::Watermarks()
		{
			this->xMin = minBound.getX();
			this->yMin = minBound.getY();
			this->zMin = minBound.getZ();
			this->xMax = maxBound.getX();
			this->yMax = maxBound.getY();
			this->zMax = maxBound.getZ();
		}
		
		CncBoundaries<T>(const CncBoundaries<T>& b)
		: CncPosition<T>::Watermarks()
		{
			this->xMin = b.xMin;
			this->xMax = b.xMax;
			this->yMin = b.yMin;
			this->yMax = b.yMax;
			this->zMin = b.zMin;
			this->zMax = b.zMax;
		}
		
		CncBoundaries<T>(const typename CncPosition<T>::Watermarks& w)
		: CncPosition<T>::Watermarks()
		{
			this->xMin = w.xMin;
			this->xMax = w.xMax;
			this->yMin = w.yMin;
			this->yMax = w.yMax;
			this->zMin = w.zMin;
			this->zMax = w.zMax;
		}
		
		const CncBoundaries<T>& operator= (const CncBoundaries<T>& b) 
		{
			this->xMin = b.xMin;
			this->xMax = b.xMax;
			this->yMin = b.yMin;
			this->yMax = b.yMax;
			this->zMin = b.zMin;
			this->zMax = b.zMax;
			
			return *this;
		}
		
		typedef typename CncPosition<T>::Watermarks Watermarks;
		
		CncBoundaries<T> normalize()										const;
		
		bool hasBoundaries()												const;
		bool fits(const CncBoundaries<T>& b)								const;
		bool fitsInside(const CncBoundaries<T>& b)							const;
		bool fitsInside(const CncPosition<T>& p)							const;
		bool fitsInside(const typename CncPosition<T>::Watermarks& w)		const;
		
		void compare(std::ostream& o, const CncBoundaries<T>& b)			const;
		void compare(std::ostream& o, const CncPosition<T>& p)				const;
		void compare(std::ostream& o, const Watermarks& w)					const;
		
		CncDistance<T> getDistanceToMin(const CncPosition<T>& p)			const;
		CncDistance<T> getDistanceToMax(const CncPosition<T>& p)			const;
		
		CncPosition<T> getMinBound()										const;
		CncPosition<T> getMaxBound()										const;
		CncPosition<T> getCentre()											const;
		
		T getMaxDistanceX()													const;
		T getMaxDistanceY()													const;
		T getMaxDistanceZ()													const;
		
		//       pTD-------------pTC
		//       /|               |
		//      / |              /|
		//     / pBD------------/pBC
		//    /  /             / /
		//  pTA-/-----------pTB /
		//   | /             | /
		//   |/              |/
		//  pBA-------------pBB
		
		enum CornerArea	{ CA_ALL, CA_BOTTOM, CA_TOP };
		enum CornerID	
		{ 
			CI_PbA = 0, CI_PbB = 1, CI_PbC = 2, CI_PbD = 3, 
			CI_PtA = 4, CI_PtB = 5, CI_PtC = 6, CI_PtD = 7,
			CI_UNKNOWN = 100
		 };
		
		CncPosition<T> getPbA() const;
		CncPosition<T> getPbB() const;
		CncPosition<T> getPbC() const;
		CncPosition<T> getPbD() const;
		
		CncPosition<T> getPtA() const;
		CncPosition<T> getPtB() const;
		CncPosition<T> getPtC() const;
		CncPosition<T> getPtD() const;
		
		CncPosition<T> getCorner(CornerID id) const;
		CncPosition<T> getCorner(CornerArea ca, unsigned int idx) const;
		
		CncBoundaries<T>::Corners& getAllCorners(CncBoundaries<T>::Corners& ret, CornerArea ca = CA_ALL) const;
		
		bool			isCornerIndexValid(CornerArea ca, unsigned int idx) const;
		unsigned int	getCornerIndex(CornerArea ca, CornerID id) const ;
		CornerID		getCornerID(CornerArea ca, unsigned int idx) const;
		const char*		getCornerName(CornerArea ca, unsigned int idx) const;
		const char*		getCornerName(CornerID id) const;
		
		float getOriginRatioX() const;
		float getOriginRatioY() const;
		float getOriginRatioZ() const;
		
		void setMinBound(const CncPosition<T>& p);
		void setMaxBound(const CncPosition<T>& p);
		void shift(const CncPosition<T>& p);
		
		void multiply(T factor);
		void multiply(T fX, T fY, T fZ);
		
		void divide(T divisor);
		void divide(T dX, T dY, T dZ);
		
		void trace(std::ostream& o) const;
		
		friend std::ostream &operator<< (std::ostream &o, const CncBoundaries<T> &b) 
		{
			b.trace(o);
			return o;
		}
};

// ----------------------------------------------------------------------
typedef CncBoundaries<int16_t>				CncIntBoundaries;
typedef CncBoundaries<int32_t>				CncLongBoundaries;
typedef CncBoundaries<double>				CncDoubleBoundaries;
typedef CncBoundaries<float>				CncFloatBoundaries;

typedef std::vector<CncIntBoundaries>		CncIntBoundariesVector;
typedef std::vector<CncLongBoundaries>		CncLongBoundariesVector;
typedef std::vector<CncDoubleBoundaries>	CncDoubleBoundariesVector;
typedef std::vector<CncFloatBoundaries>		CncFloatBoundariesVector;

typedef std::list<CncIntBoundaries>			CncIntBoundariesList;
typedef std::list<CncLongBoundaries>		CncLongBoundariesList;
typedef std::list<CncDoubleBoundaries>		CncDoubleBoundariesList;
typedef std::list<CncFloatBoundaries>		CncFloatBoundariesList;

typedef std::stack<CncIntBoundaries>		CncIntBoundariesStack;
typedef std::stack<CncLongBoundaries>		CncLongBoundariesStack;
typedef std::stack<CncDoubleBoundaries>		CncDoubleBoundariesStack;
typedef std::stack<CncFloatBoundaries>		CncFloatBoundariesStack;

// ----------------------------------------------------------------------
//
template <class T>
class CncRectangle
{
	private:
		T X;
		T Y;
		T W;
		T H;
		
	public:
		
		CncRectangle<T>()
		: X	(T(0))
		, Y	(T(0))
		, W	(T(0))
		, H	(T(0))
		{}
		
		CncRectangle<T>(T x, T y, T w, T h)
		: X	(T(x))
		, Y	(T(y))
		, W	(T(w))
		, H	(T(h))
		{}
		
		T getX() const { return X; }
		T getY() const { return Y; }
		T getW() const { return W; }
		T getH() const { return H; }
		
		const CncRectangle& reset() { *this = CncRectangle(); return *this;}
		
		const CncRectangle& setX(T x) { X = x; return *this;}
		const CncRectangle& setY(T y) { Y = y; return *this;}
		const CncRectangle& setW(T w) { W = w; return *this;}
		const CncRectangle& setH(T h) { H = h; return *this;}
};

typedef CncRectangle<int16_t>				CncIntRectangle;
typedef CncRectangle<int32_t>				CncLongRectangle;
typedef CncRectangle<double>				CncDoubleRectangle;
typedef CncRectangle<float>					CncFloatRectangle;

typedef std::vector<CncIntRectangle>		CncIntRectanglesVector;
typedef std::vector<CncLongRectangle>		CncLongRectangleVector;
typedef std::vector<CncDoubleRectangle>		CncDoubleRectangleVector;
typedef std::vector<CncFloatRectangle>		CncFloatRectangleVector;

typedef std::list<CncIntRectangle>			CncIntRectangleList;
typedef std::list<CncLongRectangle>			CncLongRectangleList;
typedef std::list<CncDoubleRectangle>		CncDoubleRectangleList;
typedef std::list<CncFloatRectangle>		CncFloatRectangleList;

typedef std::stack<CncIntRectangle>			CncIntRectangleStack;
typedef std::stack<CncLongRectangle>		CncLongRectangleStack;
typedef std::stack<CncDoubleRectangle>		CncDoubleRectangleStack;
typedef std::stack<CncFloatRectangle>		CncFloatRectangleStack;

#endif

