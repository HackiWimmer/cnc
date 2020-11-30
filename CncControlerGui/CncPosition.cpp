#include "CncCommon.h"
#include "CncPosition.h"

///////////////////////////////////////////////////////////////////////
void test() { 
	CncLongPosition 	p1;
	CncDoublePosition 	p2;
}

///////////////////////////////////////////////////////////////////////
template <class T>
bool CncBounderies<T>::hasBounderies() const {
///////////////////////////////////////////////////////////////////////
	
	if ( cnc::dblCmp::nu(this->xMin) == false ) return true;
	if ( cnc::dblCmp::nu(this->xMax) == false ) return true;
	if ( cnc::dblCmp::nu(this->yMin) == false ) return true;
	if ( cnc::dblCmp::nu(this->yMax) == false ) return true;
	if ( cnc::dblCmp::nu(this->zMin) == false ) return true;
	if ( cnc::dblCmp::nu(this->zMax) == false ) return true;
	
	return false;
}
///////////////////////////////////////////////////////////////////////
template <class T>
bool CncBounderies<T>::fits(const CncBounderies<T> b) const {
///////////////////////////////////////////////////////////////////////
	
	if ( cnc::dblCmp::eq(this->xMin, b.xMin) == false ) return false;
	if ( cnc::dblCmp::eq(this->xMax, b.xMax) == false ) return false;
	if ( cnc::dblCmp::eq(this->yMin, b.yMin) == false ) return false;
	if ( cnc::dblCmp::eq(this->yMax, b.yMax) == false ) return false;
	if ( cnc::dblCmp::eq(this->zMin, b.zMin) == false ) return false;
	if ( cnc::dblCmp::eq(this->zMax, b.zMax) == false ) return false;

	return true;
}
///////////////////////////////////////////////////////////////////////
template <class T>
bool CncBounderies<T>::fitsInside(const CncBounderies<T> b) const {
///////////////////////////////////////////////////////////////////////
	
	if ( cnc::dblCmp::ge(this->xMin, b.xMin) == false ) return false;
	if ( cnc::dblCmp::le(this->xMax, b.xMax) == false ) return false;
	if ( cnc::dblCmp::ge(this->yMin, b.yMin) == false ) return false;
	if ( cnc::dblCmp::le(this->yMax, b.yMax) == false ) return false;
	if ( cnc::dblCmp::ge(this->zMin, b.zMin) == false ) return false;
	if ( cnc::dblCmp::le(this->zMax, b.zMax) == false ) return false;

	return true;
}
///////////////////////////////////////////////////////////////////////
template <class T>
bool CncBounderies<T>::fitsInside(const CncPosition<T> p) const {
	
	if ( cnc::dblCmp::ge(this->xMin, p.getX()) == false ) return false;
	if ( cnc::dblCmp::le(this->xMax, p.getX()) == false ) return false;
	if ( cnc::dblCmp::ge(this->yMin, p.getY()) == false ) return false;
	if ( cnc::dblCmp::le(this->yMax, p.getY()) == false ) return false;
	if ( cnc::dblCmp::ge(this->zMin, p.getZ()) == false ) return false;
	if ( cnc::dblCmp::le(this->zMax, p.getZ()) == false ) return false;

	return true;
}
///////////////////////////////////////////////////////////////////////
template <class T>
bool CncBounderies<T>::fitsInside(const typename CncPosition<T>::Watermarks w) const {
	
	if ( cnc::dblCmp::ge(this->xMin, w.xMin) == false ) return false;
	if ( cnc::dblCmp::le(this->xMax, w.xMax) == false ) return false;
	if ( cnc::dblCmp::ge(this->yMin, w.yMin) == false ) return false;
	if ( cnc::dblCmp::le(this->yMax, w.yMax) == false ) return false;
	if ( cnc::dblCmp::ge(this->zMin, w.zMin) == false ) return false;
	if ( cnc::dblCmp::le(this->zMax, w.zMax) == false ) return false;

	return true;
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncDistance<T> CncBounderies<T>::getDistanceToMin(CncPosition<T> p) const {
///////////////////////////////////////////////////////////////////////
	CncDistance<T> ret;
	
	ret.setX(p.getX() - this->xMin);
	ret.setY(p.getY() - this->yMin);
	ret.setZ(p.getZ() - this->zMin);
	
	return ret;
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncDistance<T> CncBounderies<T>::getDistanceToMax(CncPosition<T> p) const {
///////////////////////////////////////////////////////////////////////
	CncDistance<T> ret;
	
	ret.setX(this->xMax - p.getX());
	ret.setY(this->yMax - p.getY());
	ret.setZ(this->zMax - p.getZ());
	
	return ret;
}
///////////////////////////////////////////////////////////////////////
template <class T>
void CncBounderies<T>::setMinBound(CncPosition<T> p) {
///////////////////////////////////////////////////////////////////////
	this->xMin = p.getX();
	this->yMin = p.getY();
	this->zMin = p.getZ();
}
///////////////////////////////////////////////////////////////////////
template <class T>
void CncBounderies<T>::setMaxBound(CncPosition<T> p) {
///////////////////////////////////////////////////////////////////////
	this->xMax = p.getX();
	this->yMax = p.getY();
	this->zMax = p.getZ();
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncPosition<T> CncBounderies<T>::getMinBound() const {
///////////////////////////////////////////////////////////////////////
	CncPosition<T> ret;
	
	ret.setX(this->xMin);
	ret.setY(this->yMin);
	ret.setZ(this->zMin);
	
	return ret;
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncPosition<T> CncBounderies<T>::getMaxBound() const {
///////////////////////////////////////////////////////////////////////
	CncPosition<T> ret;
	
	ret.setX(this->xMax);
	ret.setY(this->yMax);
	ret.setZ(this->zMax);

	return ret;
}
///////////////////////////////////////////////////////////////////////
template <class T>
void CncBounderies<T>::shift(CncPosition<T> p) {
///////////////////////////////////////////////////////////////////////
	this->xMin += p.getX();
	this->xMax += p.getX();
	this->yMin += p.getY();
	this->yMax += p.getY();
	this->zMin += p.getZ();
	this->zMax += p.getZ();
}


// Explicit template instantiation
template class CncBounderies<double>;
template class CncBounderies<int32_t>;
