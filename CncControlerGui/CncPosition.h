#ifndef CNC_POSITION_H
#define CNC_POSITION_H

#include <cmath> 
#include <ctgmath>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////
template <class T> 
class CncXYDimension {
	
	protected:
		T width, height;
		
	public:
		CncXYDimension<T>() 
		: width(0)
		, height(0)
		{}
		CncXYDimension<T>(T w, T h) 
		: width(w)
		, height(h)
		{}
		explicit CncXYDimension<T>(const CncXYDimension<T>& cd) 
		: width(cd.getW())
		, height(cd.getH())
		{}		
		virtual ~CncXYDimension<T>() {}
		
		T getW() const { return width; }
		T getH() const { return height; }
		
		friend std::ostream &operator<< (std::ostream &ostr, const CncXYDimension<T> &a) {
			ostr << a.getW() << ',' << a.getH();
			return ostr;
		}
		
};

typedef CncXYDimension<int32_t>	CncXYLongDimension;
typedef CncXYDimension<double>	CncXYDoubleDimension;
typedef CncXYDimension<float>	CncXYFloatDimension;

////////////////////////////////////////////////////////////////////////////////////////////
template <typename T> 
class CncZDimension {
	
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
		
		friend std::ostream &operator<< (std::ostream &ostr, const CncZDimension<T> &a) {
			ostr << a.getZ();
			return ostr;
		}		
		
};

typedef CncZDimension<int32_t>	CncZLongDimension;
typedef CncZDimension<double>	CncZDoubleDimension;
typedef CncZDimension<float>	CncZFloatDimension;

////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class CncPosition {
	
	public:
	
		////////////////////////////////////////////////////////////////////////////////////////////
		struct Watermarks {
			public:
				T xMin = 0, xMax = 0;
				T yMin = 0, yMax = 0;
				T zMin = 0, zMax = 0;
				
				void reset() {
					xMin = xMax = 0;
					yMin = yMax = 0;
					zMin = zMax = 0;
				}
		};
		
	protected:
		
		T xMin, xMax;
		T yMin, yMax;
		T zMin, zMax;
		
		T xPos, yPos, zPos;
		
		double floatingPointEpsilon;
		
		////////////////////////////////////////////////////////////////
		inline void evaluateWatermarks() {
			if ( xPos < xMin ) xMin = xPos;
			if ( xPos > xMax ) xMax = xPos;
			
			if ( yPos < yMin ) yMin = yPos;
			if ( yPos > yMax ) yMax = yPos;
			
			if ( zPos < zMin ) zMin = zPos;
			if ( zPos > zMax ) zMax = zPos;
		}
		
		////////////////////////////////////////////////////////////////
		inline void setWatermarks(const Watermarks& wm) {
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
		: xMin(0), xMax(0)
		, yMin(0), yMax(0)
		, zMin(0), zMax(0)
		, xPos(0), yPos(0) , zPos(0)
		, floatingPointEpsilon(0.000001)
		{}
		
		////////////////////////////////////////////////////////////////
		CncPosition<T>(T x, T y, T z)
		: xMin(x), xMax(x)
		, yMin(y), yMax(y)
		, zMin(z), zMax(z)
		, xPos(x), yPos(y), zPos(z)
		, floatingPointEpsilon(0.000001)
		{}
		
		////////////////////////////////////////////////////////////////
		/*explicit*/ CncPosition<T>(const CncPosition<T>& cp)
		: xMin(cp.getXMin()),   xMax(cp.getXMax())
		, yMin(cp.getYMin()),   yMax(cp.getYMax())
		, zMin(cp.getZMin()),   zMax(cp.getZMax())
		, xPos(cp.getX()),      yPos(cp.getY()),     zPos(cp.getZ())
		, floatingPointEpsilon(0.000001)
		{}
		
		////////////////////////////////////////////////////////////////
		virtual ~CncPosition<T>() {}
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& operator= (const CncPosition<T>& cp) {
			xMin = cp.getXMin();
			xMax = cp.getXMax();
			yMin = cp.getYMin();
			yMax = cp.getYMax();
			zMin = cp.getZMin();
			zMax = cp.getZMax();
			xPos = cp.getX();
			yPos = cp.getY();
			zPos = cp.getZ();
			
			return *this;
		}
		
		////////////////////////////////////////////////////////////////
		double getFloatingPointPrecision() {
			return floatingPointEpsilon;
		}
		
		double setFloatingPointPrecision(double eps) {
			floatingPointEpsilon = std::fabs(eps);
			return floatingPointEpsilon;
		}
		
		////////////////////////////////////////////////////////////////
		const Watermarks& getWatermarks(Watermarks& wm) const {
			wm.xMin = xMin; wm.yMin = yMin;
			wm.zMin = zMin; wm.xMax = xMax;
			wm.yMax = yMax; wm.zMax = zMax;
			
			return wm;
		}
		////////////////////////////////////////////////////////////////
		void resetWatermarks() {
			xMin = xMax = 0;
			yMin = yMax = 0;
			zMin = zMax = 0;
		}
		
		////////////////////////////////////////////////////////////////
		void resetWatermarksToCurrentPos() {
			xMin = xMax = getX();
			yMin = yMax = getY();
			zMin = zMax = getZ();
		}
		
		////////////////////////////////////////////////////////////////
		T getXMin() const { return xMin; }
		T getYMin() const { return yMin; } 
		T getZMin() const { return zMin; }
		
		////////////////////////////////////////////////////////////////
		void getMinWatermarks(CncPosition& min) {
			min.setXYZ(xMin, yMin, zMin);
		}
		////////////////////////////////////////////////////////////////
		T getXMax() const { return xMax; }
		T getYMax() const { return yMax; } 
		T getZMax() const { return zMax; }
		
		////////////////////////////////////////////////////////////////
		void getMaxWatermarks(CncPosition& max) { max.setXYZ(xMax, yMax, zMax); }
		
		////////////////////////////////////////////////////////////////
		T getX() const { return xPos; }
		T getY() const { return yPos; } 
		T getZ() const { return zPos; }
		
		////////////////////////////////////////////////////////////////
		T setX(T x) { xPos = x; evaluateWatermarks(); return xPos; }
		T setY(T y) { yPos = y; evaluateWatermarks(); return yPos; }
		T setZ(T z) { zPos = z; evaluateWatermarks(); return zPos; }
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& setXYZ(T x, T y, T z) {
			xPos = x;
			yPos = y;
			zPos = z;
			evaluateWatermarks();
			return *this;
		} 
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& setXY(T x, T y) {
			xPos = x;
			yPos = y;
			evaluateWatermarks();
			return *this;
		} 
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& set(const CncPosition<T>& p) {
			xPos = p.getX();
			yPos = p.getY();
			zPos = p.getZ();
			evaluateWatermarks();
			return *this;
		} 
		
		////////////////////////////////////////////////////////////////
		T zeorX() { xPos = 0; evaluateWatermarks(); return xPos; }
		T zeroY() { yPos = 0; evaluateWatermarks(); return yPos; }
		T zeroZ() { zPos = 0; evaluateWatermarks(); return zPos; }
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& zeroXYZ() {
			xPos = 0;
			yPos = 0;
			zPos = 0;
			evaluateWatermarks();
			return *this;
		} 
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& incX(T x) { xPos = xPos + x; evaluateWatermarks(); return *this; }
		const CncPosition<T>& incY(T y) { yPos = yPos + y; evaluateWatermarks(); return *this; }
		const CncPosition<T>& incZ(T z) { zPos = zPos + z; evaluateWatermarks(); return *this; }
		
		const CncPosition<T>& incXY(T x, T y) {
			xPos = xPos + x;
			yPos = yPos + y;
			evaluateWatermarks(); 
			return *this; 
		}

		const CncPosition<T>& inc(T x, T y, T z) {
			xPos = xPos + x;
			yPos = yPos + y;
			zPos = zPos + z;
			evaluateWatermarks(); 
			return *this; 
		}
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& decX(T x) { xPos = xPos - x; evaluateWatermarks(); return *this; }
		const CncPosition<T>& decY(T y) { yPos = yPos - y; evaluateWatermarks(); return *this; }
		const CncPosition<T>& decZ(T z) { zPos = zPos - z; evaluateWatermarks(); return *this; }
		
		const CncPosition<T>& decXY(T x, T y) {
			xPos = xPos - x;
			yPos = yPos - y;
			evaluateWatermarks(); 
			return *this; 
		}

		const CncPosition<T>& dec(T x, T y, T z) {
			xPos = xPos - x;
			yPos = yPos - y;
			zPos = zPos - z;
			evaluateWatermarks(); 
			return *this; 
		}
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& invertX() { xPos *= -1; evaluateWatermarks(); return *this; }
		const CncPosition<T>& invertY() { yPos *= -1; evaluateWatermarks(); return *this; }
		const CncPosition<T>& invertZ() { zPos *= -1; evaluateWatermarks(); return *this; }
		const CncPosition<T>& invert()  { invertX(); invertY(); invertZ();  return *this; }
		
		////////////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const CncPosition<T> &a) {
			ostr << a.getX() << ", " << a.getY() << ", " << a.getZ();
			return ostr;
		}
		
		////////////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const Watermarks &a) {
			ostr << a.xMin << ", " << a.yMin << ", " << a.zMin << " | "<< a.xMax << ", " << a.yMax << ", " << a.zMax;
			return ostr;
		}
		
		////////////////////////////////////////////////////////////////
		bool isEqual(const CncPosition<T> &a) const {
			return (    (a.getX() == getX())
					 && (a.getY() == getY())
					 && (a.getZ() == getZ())
					);
		}
		
		////////////////////////////////////////////////////////////////
		bool isFloatingEqual(const CncPosition<double> &a, const double epsilon = -1.0) const {
			const bool xc = std::fabs(a.getX() - getX()) <= ( epsilon > 0.0 ? epsilon : floatingPointEpsilon );
			const bool yc = std::fabs(a.getY() - getY()) <= ( epsilon > 0.0 ? epsilon : floatingPointEpsilon );
			const bool zc = std::fabs(a.getZ() - getZ()) <= ( epsilon > 0.0 ? epsilon : floatingPointEpsilon );
			
			return ( xc && yc && zc ); 
		}
		
		////////////////////////////////////////////////////////////////
		bool isFloatingEqual(const CncPosition<float> &a, const float epsilon = -1.0) const {
			const bool xc = std::fabs(a.getX() - getX()) <= ( epsilon > 0.0 ? epsilon : floatingPointEpsilon );
			const bool yc = std::fabs(a.getY() - getY()) <= ( epsilon > 0.0 ? epsilon : floatingPointEpsilon );
			const bool zc = std::fabs(a.getZ() - getZ()) <= ( epsilon > 0.0 ? epsilon : floatingPointEpsilon );
			
			return ( xc && yc && zc ); 
		}

		////////////////////////////////////////////////////////////////
		bool operator== (const CncPosition<T> &a) const {
			return (    (a.getX() == getX())
					 && (a.getY() == getY())
					 && (a.getZ() == getZ())
					);
		}
		
		////////////////////////////////////////////////////////////////
		bool operator!= (const CncPosition<T> &a) const {
			return (!operator== (a));
		}
		
		////////////////////////////////////////////////////////////////
		CncPosition<T> operator+ (const CncPosition<T> &a) const {
			CncPosition<T> c(a);

			c.setX(c.getX() + getX());
			c.setY(c.getY() + getY());
			c.setZ(c.getZ() + getZ());
			
			return c;
		}
		
		////////////////////////////////////////////////////////////////
		CncPosition<T> operator- (const CncPosition<T> &a) const {
			CncPosition<T> c(a);

			c.setX(c.getX() - getX());
			c.setY(c.getY() - getY());
			c.setZ(c.getZ() - getZ());
			
			return c;
		}
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& operator+= (const CncPosition &a) {
			xPos = xPos + a.getX();
			yPos = yPos + a.getY();
			zPos = zPos + a.getZ();
			evaluateWatermarks(); 
			return *this;
		}
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& operator-= (const CncPosition &a) {
			xPos = xPos - a.getX();
			yPos = yPos - a.getY();
			zPos = zPos - a.getZ();
			evaluateWatermarks(); 
			return *this;
		}
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& operator++ () {
			xPos = xPos + 1;
			yPos = yPos + 1;
			zPos = zPos + 1;
			evaluateWatermarks(); 
			return *this;
		}
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& operator-- () {
			xPos = xPos - 1;
			yPos = yPos - 1;
			zPos = zPos - 1;
			evaluateWatermarks(); 
			return *this;
		}
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& operator++ (int size) {
			xPos = xPos + size;
			yPos = yPos + size;
			zPos = zPos + size;
			evaluateWatermarks(); 
			return *this;
		}
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& operator-- (int size) {
			xPos = xPos - size;
			yPos = yPos - size;
			zPos = zPos - size;
			evaluateWatermarks(); 
			return *this;
		}
};

// ----------------------------------------------------------------------
typedef CncPosition<int32_t> 	CncLongPosition;
typedef CncPosition<double> 	CncDoublePosition;
typedef CncPosition<float> 		CncFloatPosition;

#define CncDistance				CncPosition
typedef CncPosition<int32_t> 	CncLongDistance;
typedef CncPosition<double> 	CncDoubleDistance;
typedef CncPosition<float> 		CncFloatPosition;


#define CncOffset				CncPosition
typedef CncPosition<int32_t> 	CncLongOffset;
typedef CncPosition<double> 	CncDoubleOffset;
typedef CncPosition<float> 		CncFloatOffset;

// ----------------------------------------------------------------------
template <class T>
class CncBounderies : public CncPosition<T>::Watermarks {
	
	public: 
		CncBounderies<T>()
		: CncPosition<T>::Watermarks()
		{}
		
		explicit CncBounderies<T>(const CncBounderies<T>& b)
		: CncPosition<T>::Watermarks()
		{
			this->xMin = b.xMin;
			this->xMax = b.xMax;
			this->yMin = b.yMin;
			this->yMax = b.yMax;
			this->zMin = b.zMin;
			this->zMax = b.zMax;
		}
		
		explicit CncBounderies<T>(const typename CncPosition<T>::Watermarks& w)
		: CncPosition<T>::Watermarks()
		{
			this->xMin = w.xMin;
			this->xMax = w.xMax;
			this->yMin = w.yMin;
			this->yMax = w.yMax;
			this->zMin = w.zMin;
			this->zMax = w.zMax;
		}
		
		bool hasBounderies()												const;
		bool fits(const CncBounderies<T> b)									const;
		bool fitsInside(const CncBounderies<T> b)							const;
		bool fitsInside(const CncPosition<T> p)								const;
		bool fitsInside(const typename CncPosition<T>::Watermarks w)		const;
		
		CncDistance<T> getDistanceToMin(CncPosition<T> p)					const;
		CncDistance<T> getDistanceToMax(CncPosition<T> p)					const;
		
		CncPosition<T> getMinBound()										const;
		CncPosition<T> getMaxBound()										const;
		
		void setMinBound(CncPosition<T> p);
		void setMaxBound(CncPosition<T> p);
		void shift(CncPosition<T> p);
};

// ----------------------------------------------------------------------
typedef CncBounderies<int32_t> 	CncLongBounderies;
typedef CncBounderies<double> 	CncDoubleBounderies;
typedef CncBounderies<float> 	CncFloatBounderies;

#endif
