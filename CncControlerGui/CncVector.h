#ifndef CNC_VECTOR_H
#define CNC_VECTOR_H

#include <cmath> 
#include <ctgmath>
#include <iostream>

#define CNC_VECTOR_PI 3.14159265

template <class T>
class CncVector {
	
	protected:
		T x, y, z;
		
	public:
		
		static float radias2Degree(float r) { static float PI = CNC_VECTOR_PI; return ( r * 180 / PI ); }
		static float degree2Radias(float d) { static float PI = CNC_VECTOR_PI; return ( d * PI / 180 ); }
		
		CncVector<T>()
		: x			(0) 
		, y			(0)
		, z			(0)
		{}
		
		CncVector<T>(T xx, T yy, T zz)
		: x			(xx)
		, y			(yy)
		, z			(zz)
		{}
		
		explicit CncVector<T>(const CncVector<T>& v)
		: x			(v.x)
		, y			(v.y)
		, z			(v.z)
		{}
		
		virtual ~CncVector<T>() 
		{}
		
		//--------------------------------------------------------
		const T getX() const { return x; }
		const T getY() const { return y; }
		const T getZ() const { return z; }
		
		//--------------------------------------------------------
		void add(const CncVector<T>& v)	{ x += v.x; y += v.y; z += v.z; }
		void sub(const CncVector<T>& v)	{ x -= v.x; y -= v.y; z -= v.z; }
		void mul(T r) 					{ x *= r; y *= r; z *= r; }
		
		T getLength() const { 
			return sqrt((x*x) + (y*y) + (z*z)); 
		}
		
		float getAlphaTo(const CncVector<T>& v) const { 
			return acos( getScalarProduct(v) / (getLength() * v.getLength()) ); 
		}
		
		T getScalarProduct(const CncVector<T>& v) const { 
			return (x * v.x + y * v.y + z * v.z); 
		}
			
		CncVector<T> getVectorProduct(const CncVector<T>& v) const { 
			return CncVector<T> ( (y * v.z - z * v.y), (z * v.x - x * v.z), (x * v.y - y * v.x) ); 
		}
		
		friend std::ostream &operator<< (std::ostream &ostr, const CncVector<T> &v) {
			ostr << v.x << ", " << v.y << ", " << v.z;
			return ostr;
		}
};

typedef CncVector<int32_t>	CncLongVector;
typedef CncVector<double>	CncDoubleVector;
typedef CncVector<float>	CncFloatVector;

#endif