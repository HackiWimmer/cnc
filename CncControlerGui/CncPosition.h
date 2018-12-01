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
template <typename T, const int32_t T_MIN, const int32_t T_MAX> 
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
		
		////////////////////////////////////////////////////////////////////////////////////////////
		struct BoundBox {
			
			//static const int MAX_VALUE = (-(INT16_MAX - 10));
			//static const int MIN_VALUE = (+(INT16_MAX - 10));
			
			public:
				T xMin = T_MIN, xMax = T_MAX;
				T yMin = T_MIN, yMax = T_MAX;
				T zMin = T_MIN, zMax = T_MAX;
				
				void reset() {
					xMin = T_MIN, xMax = T_MAX;
					yMin = T_MIN, yMax = T_MAX;
					zMin = T_MIN, zMax = T_MAX;
				}
		};
		
	protected:
		
		T xBMin, xBMax;
		T yBMin, yBMax;
		T zBMin, zBMax;
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
		
		////////////////////////////////////////////////////////////////
		inline T setPosValue(T val, T minBound, T maxBound) { 
			if 		( val < minBound )	return minBound;
			else if	( val > maxBound )	return maxBound;
			
			return val;
		}
		
		inline T setPosValX(T val) { return setPosValue(val, xBMin, xBMax); }
		inline T setPosValY(T val) { return setPosValue(val, yBMin, yBMax); }
		inline T setPosValZ(T val) { return setPosValue(val, zBMin, zBMax); }
		
	public:
		
		////////////////////////////////////////////////////////////////
		CncPosition<T, T_MIN, T_MAX>()
		: xBMin(T_MIN) , xBMax(T_MAX)
		, yBMin(T_MIN) , yBMax(T_MAX)
		, zBMin(T_MIN) , zBMax(T_MAX)
		, xMin(0), xMax(0)
		, yMin(0), yMax(0)
		, zMin(0), zMax(0)
		, xPos(0), yPos(0) , zPos(0)
		{}
		
		////////////////////////////////////////////////////////////////
		CncPosition<T, T_MIN, T_MAX>(T x, T y, T z)
		: xBMin(T_MIN) , xBMax(T_MAX)
		, yBMin(T_MIN) , yBMax(T_MAX)
		, zBMin(T_MIN) , zBMax(T_MAX)
		, xMin(setPosValX(x)), xMax(setPosValX(x))
		, yMin(setPosValY(y)), yMax(setPosValY(y))
		, zMin(setPosValZ(z)), zMax(setPosValZ(z))
		, xPos(setPosValX(x)), yPos(setPosValY(y)), zPos(setPosValZ(z)) 
		{}
		
		////////////////////////////////////////////////////////////////
		CncPosition<T, T_MIN, T_MAX>(const CncPosition& cp)
		: xBMin(cp.getBXMin()), xBMax(cp.getBXMax())
		, yBMin(cp.getBYMin()), yBMax(cp.getBYMax())
		, zBMin(cp.getBZMin()), zBMax(cp.getBZMax())
		, xMin(cp.getXMin()),   xMax(cp.getXMax())
		, yMin(cp.getYMin()),   yMax(cp.getYMax())
		, zMin(cp.getZMin()),   zMax(cp.getZMax())
		, xPos(cp.getX()),      yPos(cp.getY()),     zPos(cp.getZ())
		{}
		
		////////////////////////////////////////////////////////////////
		virtual ~CncPosition<T, T_MIN, T_MAX>() {}
		
		////////////////////////////////////////////////////////////////
		void setBoundings(const BoundBox& bb) {
			xBMin = bb.xMin, xBMax = bb.xMax;
			yBMin = bb.yMin, yBMax = bb.yMax;
			zBMin = bb.zMin, zBMax = bb.zMax;
		}
		
		////////////////////////////////////////////////////////////////
		void setXBoundings(T min, T max) { xBMin = min, xBMax = max; }
		void setYBoundings(T min, T max) { yBMin = min, yBMax = max; }
		void setZBoundings(T min, T max) { zBMin = min, zBMax = max; }
		
		////////////////////////////////////////////////////////////////
		const BoundBox& getBoundings(BoundBox& bb) {
			bb.xMin = xBMin, bb.xMax = xBMax;
			bb.yMin = yBMin, bb.yMax = yBMax;
			bb.zMin = zBMin, bb.zMax = zBMax;
			return bb;
		}
		
		T getBXMin() const { return xBMin; }
		T getBYMin() const { return yBMin; } 
		T getBZMin() const { return zBMin; }
		T getBXMax() const { return xBMax; }
		T getBYMax() const { return yBMax; } 
		T getBZMax() const { return zBMax; }
		
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
		T setX(T x) { xPos = setPosValX(x); evaluateWatermarks(); return xPos; }
		T setY(T y) { yPos = setPosValY(y); evaluateWatermarks(); return yPos; }
		T setZ(T z) { zPos = setPosValZ(z); evaluateWatermarks(); return zPos; }
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T, T_MIN, T_MAX>& setXYZ(T x, T y, T z) {
			xPos = setPosValX(x);
			yPos = setPosValY(y);
			zPos = setPosValZ(z);
			evaluateWatermarks();
			return *this;
		} 
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T, T_MIN, T_MAX>& set(const CncPosition<T, T_MIN, T_MAX>& p) {
			xPos = setPosValX(p.getX());
			yPos = setPosValY(p.getY());
			zPos = setPosValZ(p.getZ());
			evaluateWatermarks();
			return *this;
		} 
		
		////////////////////////////////////////////////////////////////
		T zeorX() { xPos = 0; evaluateWatermarks(); return xPos; }
		T zeroY() { yPos = 0; evaluateWatermarks(); return yPos; }
		T zeroZ() { zPos = 0; evaluateWatermarks(); return zPos; }
		////////////////////////////////////////////////////////////////
		const CncPosition<T, T_MIN, T_MAX>& zeroXYZ() {
			xPos = 0;
			yPos = 0;
			zPos = 0;
			evaluateWatermarks();
			return *this;
		} 
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T, T_MIN, T_MAX>& incX(T x) { xPos = setPosValX(xPos + x); evaluateWatermarks(); return *this; }
		const CncPosition<T, T_MIN, T_MAX>& incY(T y) { yPos = setPosValY(yPos + y); evaluateWatermarks(); return *this; }
		const CncPosition<T, T_MIN, T_MAX>& incZ(T z) { zPos = setPosValZ(zPos + z); evaluateWatermarks(); return *this; }
		
		const CncPosition<T, T_MIN, T_MAX>& inc(T x, T y, T z) { 
			xPos = setPosValX(xPos + x); 
			yPos = setPosValY(yPos + y); 
			zPos = setPosValZ(zPos + z); 
			evaluateWatermarks(); 
			return *this; 
		}
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T, T_MIN, T_MAX>& decX(T x) { xPos = setPosValX(xPos - x); evaluateWatermarks(); return *this; }
		const CncPosition<T, T_MIN, T_MAX>& decY(T y) { yPos = setPosValY(yPos - y); evaluateWatermarks(); return *this; }
		const CncPosition<T, T_MIN, T_MAX>& decZ(T z) { zPos = setPosValZ(zPos - z); evaluateWatermarks(); return *this; }
		
		const CncPosition<T, T_MIN, T_MAX>& dec(T x, T y, T z) { 
			xPos = setPosValX(xPos - x); 
			yPos = setPosValY(yPos - y);
			zPos = setPosValZ(zPos - z);
			evaluateWatermarks(); 
			return *this; 
		}
		
		////////////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const CncPosition<T, T_MIN, T_MAX> &a) {
			ostr << a.getX() << ", " << a.getY() << ", " << a.getZ();
			return ostr;
		}
		
		////////////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const Watermarks &a) {
			ostr << a.xMin << ", " << a.yMin << ", " << a.zMin << " | "<< a.xMax << ", " << a.yMax << ", " << a.zMax;
			return ostr;
		}
		
		////////////////////////////////////////////////////////////////
		friend bool operator== (const CncPosition<T, T_MIN, T_MAX> &a, const CncPosition<T, T_MIN, T_MAX> &b) {
			return (    (a.getX() == b.getX())
					 && (a.getY() == b.getY())
					 && (a.getZ() == b.getZ())
					);
		}
		
		////////////////////////////////////////////////////////////////
		friend bool operator!= (const CncPosition<T, T_MIN, T_MAX> &a, const CncPosition<T, T_MIN, T_MAX> &b) {
			return (!operator== (a, b));
		}
		
		////////////////////////////////////////////////////////////////
		friend CncPosition<T, T_MIN, T_MAX> operator+ (const CncPosition<T, T_MIN, T_MAX> &a, const CncPosition<T, T_MIN, T_MAX> &b) {
			CncPosition<T, T_MIN, T_MAX> c(a);

			c.setX(c.getX() + b.getX());
			c.setY(c.getY() + b.getY());
			c.setZ(c.getZ() + b.getZ());
			
			return c;
		}
		
		////////////////////////////////////////////////////////////////
		friend CncPosition<T, T_MIN, T_MAX> operator- (const CncPosition<T, T_MIN, T_MAX> &a, const CncPosition<T, T_MIN, T_MAX> &b) {
			CncPosition<T, T_MIN, T_MAX> c(a);

			c.setX(c.getX() - b.getX());
			c.setY(c.getY() - b.getY());
			c.setZ(c.getZ() - b.getZ());
			
			return c;
		}
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T, T_MIN, T_MAX>& operator+= (const CncPosition &a) {
			xPos = setPosValX(xPos + a.getX());
			yPos = setPosValY(yPos + a.getY());
			zPos = setPosValZ(zPos + a.getZ());
			evaluateWatermarks(); 
			return *this;
		}
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T, T_MIN, T_MAX>& operator-= (const CncPosition &a) {
			xPos = setPosValX(xPos - a.getX());
			yPos = setPosValY(yPos - a.getY());
			zPos = setPosValZ(zPos - a.getZ());
			evaluateWatermarks(); 
			return *this;
		}
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T, T_MIN, T_MAX>& operator++ () {
			xPos = setPosValX(xPos + 1);
			yPos = setPosValY(yPos + 1);
			zPos = setPosValZ(zPos + 1);
			evaluateWatermarks(); 
			return *this;
		}
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T, T_MIN, T_MAX>& operator-- () {
			xPos = setPosValX(xPos - 1);
			yPos = setPosValY(yPos - 1);
			zPos = setPosValZ(zPos - 1);
			evaluateWatermarks(); 
			return *this;
		}
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T, T_MIN, T_MAX>& operator++ (int size) {
			xPos = setPosValX(xPos + size);
			yPos = setPosValY(yPos + size);
			zPos = setPosValZ(zPos + size);
			evaluateWatermarks(); 
			return *this;
		}
		
		////////////////////////////////////////////////////////////////
		const CncPosition<T, T_MIN, T_MAX>& operator-- (int size) {
			xPos = setPosValX(xPos - size);
			yPos = setPosValY(yPos - size);
			zPos = setPosValZ(zPos - size);
			evaluateWatermarks(); 
			return *this;
		}
};

typedef CncPosition<int32_t, INT32_MIN, INT32_MAX> CncLongPosition;
typedef CncPosition<double,  INT32_MIN, INT32_MAX> CncDoublePosition;

#endif