#include "CncVector.h"

namespace TEST {
	
	struct xxx {
		
		xxx() {
			CncFloatVector ux(1.0, 0.0, 0.0);
			CncFloatVector uy(0.0, 1.0, 0.0);
			CncFloatVector uz(0.0, 0.0, 1.0);
			
			CncFloatVector v1(-1.0, -1.0, 0.0);
			
			std::cout << "CncVector Test" << std::endl;
			
			std::cout << CncFloatVector::radias2Degree(v1.getAlphaTo(ux)) << std::endl;
			std::cout << CncFloatVector::radias2Degree(v1.getAlphaTo(uy)) << std::endl;
			std::cout << CncFloatVector::radias2Degree(v1.getAlphaTo(uz)) << std::endl;
			
			
			CncFloatMatrix4x4 mat;
			//mat.set(CncFloatMatrix4x4::V11, 0.0);
			mat.set(CncFloatMatrix4x4::V1, v1);
			
			std::cout << mat << std::endl;
			
			
			//abort();
		}
		
	} ;//XXX;
};


///////////////////////////////////////////////////////////////////////
template <class T>
std::ostream& CncMatrix4x4<T>::traceRawMatrix(std::ostream &ostr, const T m[]) {
///////////////////////////////////////////////////////////////////////
	if ( m == NULL )
		return ostr;
		
	for (int r = 0; r < 4; r++ )
	{
		ostr << "| ";
		
		for (int c = 0; c < 4; c++ )
		{
			unsigned short mi = r + (c * 4);
			
			ostr << std::showpos << std::fixed << std::setw( 11 ) << std::setprecision( 6 )
				 << m[mi] << " " ;
		}
		
		ostr << " |"<< std::endl;
	}
	
	return ostr;
}
///////////////////////////////////////////////////////////////////////
template <class T>
void CncMatrix4x4<T>::transform(T& x, T& y, T& z) {
///////////////////////////////////////////////////////////////////////
	const T X = x;
	const T Y = y;
	const T Z = z;
	const T W = (T)1;
	
	x = mat[V11] * X + mat[V12] * Y + mat[V13] * Z + mat[V14] * W; 
	y = mat[V21] * X + mat[V22] * Y + mat[V23] * Z + mat[V24] * W;
	z = mat[V31] * X + mat[V32] * Y + mat[V33] * Z + mat[V34] * W;
	//T w = mat[V41] * X + mat[V42] * Y + mat[V43] * Z + mat[V44] * W;
}
///////////////////////////////////////////////////////////////////////
template <class T>
const CncMatrix4x4<T>& CncMatrix4x4<T>::multiply(const CncMatrix4x4<T>& m) {
///////////////////////////////////////////////////////////////////////
	mat[V11] = mat[V11] * m.get(V11) + mat[V12] * m.get(V21) + mat[V13] * m.get(V31) + mat[V14] * m.get(V41);
	mat[V21] = mat[V21] * m.get(V11) + mat[V22] * m.get(V21) + mat[V23] * m.get(V31) + mat[V24] * m.get(V41);
	mat[V31] = mat[V31] * m.get(V11) + mat[V32] * m.get(V21) + mat[V33] * m.get(V31) + mat[V34] * m.get(V41);
	mat[V41] = mat[V41] * m.get(V11) + mat[V42] * m.get(V21) + mat[V43] * m.get(V31) + mat[V44] * m.get(V41);
	
	mat[V12] = mat[V11] * m.get(V12) + mat[V12] * m.get(V22) + mat[V13] * m.get(V32) + mat[V14] * m.get(V42);
	mat[V22] = mat[V21] * m.get(V12) + mat[V22] * m.get(V22) + mat[V23] * m.get(V32) + mat[V24] * m.get(V42);
	mat[V32] = mat[V31] * m.get(V12) + mat[V32] * m.get(V22) + mat[V33] * m.get(V32) + mat[V34] * m.get(V42);
	mat[V42] = mat[V41] * m.get(V12) + mat[V42] * m.get(V22) + mat[V43] * m.get(V32) + mat[V44] * m.get(V42);
	
	mat[V13] = mat[V11] * m.get(V13) + mat[V12] * m.get(V23) + mat[V13] * m.get(V33) + mat[V14] * m.get(V43);
	mat[V23] = mat[V21] * m.get(V13) + mat[V22] * m.get(V23) + mat[V23] * m.get(V33) + mat[V24] * m.get(V43);
	mat[V33] = mat[V31] * m.get(V13) + mat[V32] * m.get(V23) + mat[V33] * m.get(V33) + mat[V34] * m.get(V43);
	mat[V43] = mat[V41] * m.get(V13) + mat[V42] * m.get(V23) + mat[V43] * m.get(V33) + mat[V44] * m.get(V43);
	
	mat[V14] = mat[V11] * m.get(V14) + mat[V12] * m.get(V24) + mat[V13] * m.get(V34) + mat[V14] * m.get(V44);
	mat[V24] = mat[V21] * m.get(V14) + mat[V22] * m.get(V24) + mat[V23] * m.get(V34) + mat[V24] * m.get(V44);
	mat[V34] = mat[V31] * m.get(V14) + mat[V32] * m.get(V24) + mat[V33] * m.get(V34) + mat[V34] * m.get(V44);
	mat[V44] = mat[V41] * m.get(V14) + mat[V42] * m.get(V24) + mat[V43] * m.get(V34) + mat[V44] * m.get(V44);
	
	return *this;
}
///////////////////////////////////////////////////////////////////////
template <class T>
const CncMatrix4x4<T>& CncMatrix4x4<T>::setTranslation(T x, T y, T z) {
///////////////////////////////////////////////////////////////////////
	CncMatrix4x4<T> m;
	
	m.set(V14, x);
	m.set(V24, y);
	m.set(V34, z);
	
	return this->multiply(m);
}


///////////////////////////////////////////////////////////////////////
template <class T>
const CncMatrix4x4<T>& CncMatrix4x4<T>::setRotationAxisX(double aDegree) {
///////////////////////////////////////////////////////////////////////
	CncMatrix4x4<T> m;
	
	const float angle = aDegree * CNC_VECTOR_PI / 180;
	const float COS   = cos(angle);
	const float SIN   = sin(angle);
	
	m.set(V22, +COS);
	m.set(V23, -SIN);
	m.set(V32, +SIN);
	m.set(V33, +COS);
	
	return this->multiply(m);
}


///////////////////////////////////////////////////////////////////////
template <class T>
const CncMatrix4x4<T>& CncMatrix4x4<T>::setRotationAxisY(double aDegree) {
///////////////////////////////////////////////////////////////////////
	CncMatrix4x4<T> m;
	
	const float angle = aDegree * CNC_VECTOR_PI / 180;
	const float COS   = cos(angle);
	const float SIN   = sin(angle);
	
	m.set(V11, +COS);
	m.set(V13, +SIN);
	m.set(V21, -SIN);
	m.set(V33, +COS);
	
	return this->multiply(m);
}
///////////////////////////////////////////////////////////////////////
template <class T>
const CncMatrix4x4<T>& CncMatrix4x4<T>::setRotationAxisZ(double aDegree) {
///////////////////////////////////////////////////////////////////////
	CncMatrix4x4<T> m;
	
	const float angle = aDegree * CNC_VECTOR_PI / 180;
	const float COS   = cos(angle);
	const float SIN   = sin(angle);
	
	m.set(V11, +COS);
	m.set(V12, -SIN);
	m.set(V21, +SIN);
	m.set(V22, +COS);
	
	return this->multiply(m);
}
///////////////////////////////////////////////////////////////////////
template <class T>
const CncMatrix4x4<T>& CncMatrix4x4<T>::setScaling(double factor) {
///////////////////////////////////////////////////////////////////////
	CncMatrix4x4<T> m;
	
	m.set(V11, factor);
	m.set(V22, factor);
	m.set(V33, factor);
	
	return this->multiply(m);
}



// Explicit template instantiation
template class CncMatrix4x4<int16_t>;
template class CncMatrix4x4<int32_t>;
template class CncMatrix4x4<double>;
template class CncMatrix4x4<float>;
