#ifndef CNC_VECTOR_H
#define CNC_VECTOR_H

#include <cmath> 
#include <cstring>
#include <ctgmath>
#include <iomanip>
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
		
		CncVector<T>(const CncVector<T>& v)
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
		const T getW() const { return (T)0; }
		
		//--------------------------------------------------------
		CncVector<T> &  add(const CncVector<T>& v)	{ x += v.x; y += v.y; z += v.z; return *this; }
		CncVector<T> &  sub(const CncVector<T>& v)	{ x -= v.x; y -= v.y; z -= v.z; return *this;  }
		CncVector<T> &  mul(T r) 					{ x *= r; y *= r; z *= r; return *this;  }
		
		CncVector<T> & normalize() {
			const T l = getLength();
			
			x = ( l != (T)0 ? x / l : (T)0 );
			y = ( l != (T)0 ? y / l : (T)0 );
			z = ( l != (T)0 ? z / l : (T)0 );
			return *this;
		}
		
		bool isNullVector() const {
			return x == (T)0 && y == (T)0 && z == (T)0;
		}
		
		bool isParallelTo(const CncVector<T>& v) const {
			return getVectorProduct(v).isNullVector();
		}
		
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
		
		CncVector<T> getNormale() const { 
			static const CncVector<T> ux((T)1, (T)0, (T)0);
			static const CncVector<T> uy((T)0, (T)1, (T)0);
			static const CncVector<T> uz((T)0, (T)0, (T)1);
			static const CncVector<T> un((T)0, (T)0, (T)0);
			
			CncVector<T> q1 = getVectorProduct(ux);
			if ( q1.isNullVector() == false )
				return q1;
				
			CncVector<T> q2(getVectorProduct(uy));
			if ( q2.isNullVector() == false )
				return q2;

			CncVector<T> q3(getVectorProduct(uz));
			if ( q3.isNullVector() == false )
				return q3;
			
			return un;
		}
		
		////////////////////////////////////////////////////////////////
		const CncVector<T>& operator= (const CncVector<T>& v) {
			x = v.x;
			y = v.y;
			z = v.z;
			
			return *this;
		}
		
		////////////////////////////////////////////////////////////////
		bool operator== (const CncVector<T> &v) const {
			return (    (v.x == x)
					 && (v.y == y)
					 && (v.z == z)
					);
		}
		
		////////////////////////////////////////////////////////////////
		bool operator!= (const CncVector<T> &v) const {
			return (!operator== (v));
		}
		
		friend std::ostream &operator<< (std::ostream &ostr, const CncVector<T> &v) {
			ostr << v.x << ", " << v.y << ", " << v.z;
			return ostr;
		}
};

typedef CncVector<int32_t>	CncLongVector;
typedef CncVector<double>	CncDoubleVector;
typedef CncVector<float>	CncFloatVector;


template <class T>
class CncMatrix4x4 {
	
	private:
		T mat[16];
	
	void resetMat() {
		memset(mat, 0, 16 * sizeof(T));
		
		mat[V11] = (T)1;
		mat[V22] = (T)1;
		mat[V33] = (T)1;
		mat[V44] = (T)1;
	}
	
	public:
		enum MatVec {	V1 = 0, V2 = 1, V3 = 2, V4 = 3 };
		
		enum MatIdx {	V11 =  0, V12 =  1, V13 =  2, V14 =  3, 
						V21 =  4, V22 =  5, V23 =  6, V24 =  7,
						V31 =  8, V32 =  9, V33 = 10, V34 = 11,
						V41 = 12, V42 = 13, V43 = 14, V44 = 15
		};
		
		CncMatrix4x4<T>() 
		{
			resetMat();
		}
		
		void set(const MatIdx mi, T val) {
			mat[mi] = val;
		}
		
		void set(const MatVec mv, const CncVector<T>& v) {
			unsigned short mi = 0 + mv * 4;
			
			mat[mi + 0] = v.getX();
			mat[mi + 1] = v.getY();
			mat[mi + 2] = v.getZ();
			mat[mi + 3] = v.getW();
		}

		T get(const MatIdx mi) const { 
			return mat[mi]; 
		}
		
		const T* get() const { 
			return mat; 
		}
		
		friend std::ostream &operator<< (std::ostream &ostr, const CncMatrix4x4<T> &m) {
			
			for (int r = 0; r < 4; r++ ) {
				ostr << "| ";
				
				for (int c = 0; c < 4; c++ ) {
					unsigned short mi = r + (c * 4);
					
					ostr << std::showpos << std::fixed << std::setw( 11 ) << std::setprecision( 6 )
						 << m.mat[mi] << " " ;
				}
				
				ostr << " |"<< std::endl;
			}
			
			return ostr;
		}
};

typedef CncMatrix4x4<int32_t>	CncLongMatrix4x4;
typedef CncMatrix4x4<double>	CncDoubleMatrix4x4;
typedef CncMatrix4x4<float>		CncFloatMatrix4x4;

#endif