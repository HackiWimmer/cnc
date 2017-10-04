#ifndef CNC_POSITION_H
#define CNC_POSITION_H

#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////
template <typename T> 
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
		CncXYDimension<T>(const CncXYDimension<T>& cd) 
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
		CncZDimension<T>(T z)
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
template <typename T> 
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
		T xPos, yPos, zPos;
		T xMin, xMax;
		T yMin, yMax;
		T zMin, zMax;
		
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
		: xPos(0)
		, yPos(0)
		, zPos(0)
		, xMin(0)
		, xMax(0)
		, yMin(0)
		, yMax(0)
		, zMin(0)
		, zMax(0)
		{}
		////////////////////////////////////////////////////////////////
		CncPosition<T>(T x, T y, T z)
		: xPos(x)
		, yPos(y)
		, zPos(z)
		, xMin(x)
		, xMax(x)
		, yMin(y)
		, yMax(y)
		, zMin(z)
		, zMax(z)
		{}
		////////////////////////////////////////////////////////////////
		CncPosition<T>(const CncPosition& cp)
		: xPos(cp.getX())
		, yPos(cp.getY())
		, zPos(cp.getZ())
		, xMin(cp.getXMin())
		, xMax(cp.getXMax())
		, yMin(cp.getYMin())
		, yMax(cp.getYMax())
		, zMin(cp.getZMin())
		, zMax(cp.getZMax())
		{}
		////////////////////////////////////////////////////////////////
		virtual ~CncPosition<T>() {
		}
		////////////////////////////////////////////////////////////////
		void getWatermarks(Watermarks& wm) {
			wm.xMin = xMin;
			wm.yMin = yMin;
			wm.zMin = zMin;
			wm.xMax = xMax;
			wm.yMax = yMax;
			wm.zMax = zMax;
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
		void getMaxWatermarks(CncPosition& max) {
			max.setXYZ(xMax, yMax, zMax);
		}
		////////////////////////////////////////////////////////////////
		T getX() const { return xPos; }
		T getY() const { return yPos; } 
		T getZ() const { return zPos; }
		////////////////////////////////////////////////////////////////
		T setX(T x) { xPos = x; return xPos; }
		T setY(T y) { yPos = y; return yPos; }
		T setZ(T z) { zPos = z; return zPos; }
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
		T incX(T x) { xPos += x; evaluateWatermarks(); return xPos; }
		T incY(T y) { yPos += y; evaluateWatermarks(); return yPos; }
		T incZ(T z) { zPos += z; evaluateWatermarks(); return zPos; }
		////////////////////////////////////////////////////////////////
		T decX(T x) { xPos -= x; evaluateWatermarks(); return xPos; }
		T decY(T y) { yPos -= y; evaluateWatermarks(); return yPos; }
		T decZ(T z) { zPos -= z; evaluateWatermarks(); return zPos; }
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
		friend bool operator== (const CncPosition<T> &a, const CncPosition<T> &b) {
			return (    (a.getX() == b.getX())
					 && (a.getY() == b.getY())
					 && (a.getZ() == b.getZ())
					);
		}
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
			xPos += a.getX();
			yPos += a.getY();
			zPos += a.getZ();
			evaluateWatermarks(); 
			return *this;
		}
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& operator-= (const CncPosition &a) {
			xPos -= a.getX();
			yPos -= a.getY();
			zPos -= a.getZ();
			evaluateWatermarks(); 
			return *this;
		}
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& operator++ () {
			xPos++;
			yPos++;
			zPos++;
			evaluateWatermarks(); 
			return *this;
		}
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& operator-- () {
			xPos--;
			yPos--;
			zPos--;
			evaluateWatermarks(); 
			return *this;
		}
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& operator++ (int size) {
			xPos += size;
			yPos += size;
			zPos += size;
			evaluateWatermarks(); 
			return *this;
		}
		////////////////////////////////////////////////////////////////
		const CncPosition<T>& operator-- (int size) {
			xPos -= size;
			yPos -= size;
			zPos -= size;
			evaluateWatermarks(); 
			return *this;
		}
};

typedef CncPosition<int32_t> CncLongPosition;
typedef CncPosition<double> CncDoublePosition;

#endif