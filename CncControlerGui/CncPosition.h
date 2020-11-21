#ifndef CNC_POSITION_H
#define CNC_POSITION_H

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

typedef CncXYDimension<int32_t> CncXYLongDimension;
typedef CncXYDimension<double> CncXYDoubleDimension;

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

typedef CncZDimension<int32_t> CncZLongDimension;
typedef CncZDimension<double> CncZDoubleDimension;


////////////////////////////////////////////////////////////////////////////////////////////
// Forward declare the class
template <typename T> class CncPosition;

// Forward declare the template operator
template <typename T>
bool operator== (const CncPosition<T> &a, const CncPosition<T> &b);

// Forward declare the function
bool operator== (const CncPosition<double> &a, const CncPosition<double> &b);

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
		{}
		
		////////////////////////////////////////////////////////////////
		CncPosition<T>(T x, T y, T z)
		: xMin(x), xMax(x)
		, yMin(y), yMax(y)
		, zMin(z), zMax(z)
		, xPos(x), yPos(y), zPos(z)
		{}
		
		////////////////////////////////////////////////////////////////
		/*explicit*/ CncPosition<T>(const CncPosition<T>& cp)
		: xMin(cp.getXMin()),   xMax(cp.getXMax())
		, yMin(cp.getYMin()),   yMax(cp.getYMax())
		, zMin(cp.getZMin()),   zMax(cp.getZMax())
		, xPos(cp.getX()),      yPos(cp.getY()),     zPos(cp.getZ())
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
		
		const CncPosition<T>& dec(T x, T y, T z) {
			xPos = xPos - x;
			yPos = yPos - y;
			zPos = zPos - z;
			evaluateWatermarks(); 
			return *this; 
		}
		
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
		
		
		friend bool operator== (const CncPosition<double> &a, const CncPosition<double> &b);
		friend bool operator== <>(const CncPosition<T> &a, const CncPosition<T> &b);
		
		/*
		////////////////////////////////////////////////////////////////
		friend bool operator== (const CncPosition<T> &a, const CncPosition<T> &b) {
			return (    (a.getX() == b.getX())
					 && (a.getY() == b.getY())
					 && (a.getZ() == b.getZ())
					);
		}
		*/
		
		////////////////////////////////////////////////////////////////
		friend bool operator!= (const CncPosition<T> &a, const CncPosition<T> &b) {
			return (!operator== (a, b));
		}
		
		////////////////////////////////////////////////////////////////
		friend CncPosition<T> operator+ (const CncPosition<T> &a, const CncPosition<T> &b) {
			CncPosition<T> c(a);

			c.setX(c.getX() + b.getX());
			c.setY(c.getY() + b.getY());
			c.setZ(c.getZ() + b.getZ());
			
			return c;
		}
		
		////////////////////////////////////////////////////////////////
		friend CncPosition<T> operator- (const CncPosition<T> &a, const CncPosition<T> &b) {
			CncPosition<T> c(a);

			c.setX(c.getX() - b.getX());
			c.setY(c.getY() - b.getY());
			c.setZ(c.getZ() - b.getZ());
			
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
template <class T>
bool operator== (const CncPosition<T> &a, const CncPosition<T> &b) {
	return (    (a.getX() == b.getX())
			 && (a.getY() == b.getY())
			 && (a.getZ() == b.getZ())
			);
}

// ----------------------------------------------------------------------
typedef CncPosition<int32_t> 	CncLongPosition;
typedef CncPosition<double> 	CncDoublePosition;

typedef CncPosition<int32_t> 	CncLongDistance;
typedef CncPosition<double> 	CncDoubleDistance;

#endif
