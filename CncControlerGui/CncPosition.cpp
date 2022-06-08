#include <iomanip>

#include "CncCommon.h"
#include "CncPosition.h"

///////////////////////////////////////////////////////////////////////
void test() { 
	CncIntPosition 		p1;
	CncLongPosition 	p2;
	CncDoublePosition 	p3;
	CncFloatPosition 	p4;
}

///////////////////////////////////////////////////////////////////////
template <class T>
bool CncBoundaries<T>::hasBoundaries() const {
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
bool CncBoundaries<T>::fits(const CncBoundaries<T>& b) const {
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
bool CncBoundaries<T>::fitsInside(const CncBoundaries<T>& b) const {
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
bool CncBoundaries<T>::fitsInside(const CncPosition<T>& p) const {
///////////////////////////////////////////////////////////////////////
	
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
bool CncBoundaries<T>::fitsInside(const typename CncPosition<T>::Watermarks& w) const {
///////////////////////////////////////////////////////////////////////
	
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
void CncBoundaries<T>::compare(std::ostream& o, const CncBoundaries<T>& b) const {
///////////////////////////////////////////////////////////////////////
	o	<< *this 
		<< " vs. " 
		<< std::endl
		<< b 
		<< "Fits inside: " 
		<< fitsInside(b)
		<< std::endl
	;
}
///////////////////////////////////////////////////////////////////////
template <class T>
void CncBoundaries<T>::compare(std::ostream& o, const CncPosition<T>& p) const {
///////////////////////////////////////////////////////////////////////
	o	<< *this 
		<< " vs. " 
		<< std::endl
		<< p 
		<< "Fits inside: " 
		<< fitsInside(p)
		<< std::endl
	;
}
///////////////////////////////////////////////////////////////////////
template <class T>
void CncBoundaries<T>::compare(std::ostream& o, const Watermarks& w) const {
///////////////////////////////////////////////////////////////////////
	o	<< *this 
		<< " vs. " 
		<< std::endl
		<< w 
		<< "Fits inside: " 
		<< fitsInside(w)
		<< std::endl
	;
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncDistance<T> CncBoundaries<T>::getDistanceToMin(const CncPosition<T>& p) const {
///////////////////////////////////////////////////////////////////////
	CncDistance<T> ret;
	
	ret.setX(p.getX() - this->xMin);
	ret.setY(p.getY() - this->yMin);
	ret.setZ(p.getZ() - this->zMin);
	
	return ret;
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncDistance<T> CncBoundaries<T>::getDistanceToMax(const CncPosition<T>& p) const {
///////////////////////////////////////////////////////////////////////
	CncDistance<T> ret;
	
	ret.setX(this->xMax - p.getX());
	ret.setY(this->yMax - p.getY());
	ret.setZ(this->zMax - p.getZ());
	
	return ret;
}
///////////////////////////////////////////////////////////////////////
template <class T>
void CncBoundaries<T>::setMinBound(const CncPosition<T>& p) {
///////////////////////////////////////////////////////////////////////
	this->xMin = p.getX();
	this->yMin = p.getY();
	this->zMin = p.getZ();
}
///////////////////////////////////////////////////////////////////////
template <class T>
void CncBoundaries<T>::setMaxBound(const CncPosition<T>& p) {
///////////////////////////////////////////////////////////////////////
	this->xMax = p.getX();
	this->yMax = p.getY();
	this->zMax = p.getZ();
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncPosition<T> CncBoundaries<T>::getMinBound() const {
///////////////////////////////////////////////////////////////////////
	CncPosition<T> ret;
	
	ret.setX(this->xMin);
	ret.setY(this->yMin);
	ret.setZ(this->zMin);
	
	return ret;
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncPosition<T> CncBoundaries<T>::getMaxBound() const {
///////////////////////////////////////////////////////////////////////
	CncPosition<T> ret;
	
	ret.setX(this->xMax);
	ret.setY(this->yMax);
	ret.setZ(this->zMax);

	return ret;
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncPosition<T> CncBoundaries<T>::getCentre() const {
///////////////////////////////////////////////////////////////////////
	CncPosition<T> ret
	(
		this->xMin + (this->xMax - this->xMin) / 2.0, 
		this->yMin + (this->yMax - this->yMin) / 2.0, 
		this->zMin + (this->zMax - this->zMin) / 2.0
	);

	return ret;
}
///////////////////////////////////////////////////////////////////////
template <class T>
void CncBoundaries<T>::shift(const CncPosition<T>& p) {
///////////////////////////////////////////////////////////////////////
	this->xMin += p.getX();
	this->xMax += p.getX();
	this->yMin += p.getY();
	this->yMax += p.getY();
	this->zMin += p.getZ();
	this->zMax += p.getZ();
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncPosition<T> CncBoundaries<T>::getPbA() const {
///////////////////////////////////////////////////////////////////////
	CncPosition<T> ret
	(
		getMinBound().getX(), 
		getMinBound().getY(), 
		getMinBound().getZ()
	);
	
	return ret;
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncPosition<T> CncBoundaries<T>::getPbB() const {
///////////////////////////////////////////////////////////////////////
	CncPosition<T> ret
	(
		getMaxBound().getX(), 
		getMinBound().getY(), 
		getMinBound().getZ()
	);
	
	return ret;
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncPosition<T> CncBoundaries<T>::getPbC() const {
///////////////////////////////////////////////////////////////////////
	CncPosition<T> ret
	(
		getMaxBound().getX(), 
		getMaxBound().getY(), 
		getMinBound().getZ()
	);
	
	return ret;
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncPosition<T> CncBoundaries<T>::getPbD() const {
///////////////////////////////////////////////////////////////////////
	CncPosition<T> ret
	(
		getMinBound().getX(), 
		getMaxBound().getY(), 
		getMinBound().getZ()
	);
	
	return ret;
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncPosition<T> CncBoundaries<T>::getPtA() const {
///////////////////////////////////////////////////////////////////////
	CncPosition<T> ret
	(
		getMinBound().getX(), 
		getMinBound().getY(), 
		getMaxBound().getZ()
	);
	
	return ret;
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncPosition<T> CncBoundaries<T>::getPtB() const {
///////////////////////////////////////////////////////////////////////
	CncPosition<T> ret
	(
		getMaxBound().getX(), 
		getMinBound().getY(), 
		getMaxBound().getZ()
	);
	
	return ret;
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncPosition<T> CncBoundaries<T>::getPtC() const {
///////////////////////////////////////////////////////////////////////
	CncPosition<T> ret
	(
		getMaxBound().getX(), 
		getMaxBound().getY(), 
		getMaxBound().getZ()
	);
	
	return ret;
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncPosition<T> CncBoundaries<T>::getPtD() const {
///////////////////////////////////////////////////////////////////////
	CncPosition<T> ret
	(
		getMinBound().getX(), 
		getMaxBound().getY(), 
		getMaxBound().getZ()
	);
	
	return ret;
}
///////////////////////////////////////////////////////////////////////
template <class T>
bool CncBoundaries<T>::isCornerIndexValid(CornerArea ca, unsigned int idx) const {
///////////////////////////////////////////////////////////////////////
	if ( idx >= CI_PbA && idx <= CI_PtD )
	{
		switch ( ca )
		{
			case CA_ALL:	return ( idx >= CI_PbA     && idx <= CI_PtD     );
			case CA_BOTTOM:	return ( idx >= CI_PbA     && idx <= CI_PbD     );
			case CA_TOP:	return ( idx >= CI_PtA - 4 && idx <= CI_PtD - 4 );
		}
	}
	
	return false;
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncPosition<T> CncBoundaries<T>::getCorner(CornerID id) const {
///////////////////////////////////////////////////////////////////////
	switch ( id )
	{
		case CI_PbA:		return getPbA();
		case CI_PbB:		return getPbB();
		case CI_PbC:		return getPbC();
		case CI_PbD:		return getPbD();
		case CI_PtA:		return getPtA();
		case CI_PtB:		return getPtB();
		case CI_PtC:		return getPtC();
		case CI_PtD:		return getPtD();
		case CI_UNKNOWN:	;
	}
	
	// any default
	static CncPosition<T> p;
	return p;
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncPosition<T> CncBoundaries<T>::getCorner(CornerArea ca, unsigned int idx) const {
///////////////////////////////////////////////////////////////////////
	if ( idx >= CI_PbA && idx <= CI_PtD )
	{
		switch ( ca )
		{
			case CA_ALL:
			case CA_BOTTOM:	return getCorner((CornerID)(idx));
			case CA_TOP:	return getCorner((CornerID)(idx + CI_PtA));
		}
	}
	
	// any default
	return CncPosition<T>();
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncBoundaries<T>::CornerID CncBoundaries<T>::getCornerID(CornerArea ca, unsigned int idx) const {
///////////////////////////////////////////////////////////////////////
	if ( idx >= CI_PbA && idx <= CI_PtD )
	{
		switch ( ca )
		{
			case CA_ALL:
			case CA_BOTTOM:	return (CornerID)(idx);
			case CA_TOP:	return (CornerID)(idx + CI_PtA);
		}
	}
	
	return CI_UNKNOWN;
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncBoundaries<T>::Corners& CncBoundaries<T>::getAllCorners(CncBoundaries<T>::Corners& ret, CornerArea ca) const {
///////////////////////////////////////////////////////////////////////
	switch ( ca )
	{
		case CA_ALL:
			ret.push_back(getPbA());
			ret.push_back(getPbB());
			ret.push_back(getPbC());
			ret.push_back(getPbD());
			ret.push_back(getPtA());
			ret.push_back(getPtB());
			ret.push_back(getPtC());
			ret.push_back(getPtD());
			break;
			
		case CA_BOTTOM:
			ret.push_back(getPbA());
			ret.push_back(getPbB());
			ret.push_back(getPbC());
			ret.push_back(getPbD());
			break;
			
		case CA_TOP:
			ret.push_back(getPtA());
			ret.push_back(getPtB());
			ret.push_back(getPtC());
			ret.push_back(getPtD());
			break;
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////////
template <class T>
const char* CncBoundaries<T>::getCornerName(CornerArea ca, unsigned int idx) const {
///////////////////////////////////////////////////////////////////////
	if ( idx >= CI_PbA && idx <= CI_PtD )
	{
		switch ( ca )
		{
			case CA_ALL:
			case CA_BOTTOM:	return getCornerName((CornerID)(idx));
			case CA_TOP:	return getCornerName((CornerID)(idx + CI_PtA));
		}
	}
	
	// any default
	return "Unknown";
}
///////////////////////////////////////////////////////////////////////
template <class T>
const char* CncBoundaries<T>::getCornerName(CornerID id) const {
///////////////////////////////////////////////////////////////////////
	switch ( id )
	{
		case CI_PbA:		return "PbA";
		case CI_PbB:		return "PbB";
		case CI_PbC:		return "PbC";
		case CI_PbD:		return "PbD";
		case CI_PtA:		return "PtA";
		case CI_PtB:		return "PtB";
		case CI_PtC:		return "PtC";
		case CI_PtD:		return "PtD";
		case CI_UNKNOWN:	;
	}
	
	// any default
	return "Unknown";
}
///////////////////////////////////////////////////////////////////////
template <class T>
unsigned int CncBoundaries<T>::getCornerIndex(CornerArea ca, CornerID id) const {
///////////////////////////////////////////////////////////////////////
	if ( ca == CA_TOP )
	{
		switch ( id )
		{
			case CI_PtA:	return 0;
			case CI_PtB:	return 1;
			case CI_PtC:	return 2;
			case CI_PtD:	return 3;
			default:		;
		}
	}
	else
	{
		switch ( id )
		{
			case CI_PbA:		return 0;
			case CI_PbB:		return 1;
			case CI_PbC:		return 2;
			case CI_PbD:		return 3;
			case CI_PtA:		return 4;
			case CI_PtB:		return 5;
			case CI_PtC:		return 6;
			case CI_PtD:		return 7;
			case CI_UNKNOWN:	;
		}
	}
	
	// any default
	return -1;
}
///////////////////////////////////////////////////////////////////////
template <class T>
void CncBoundaries<T>::multiply(T factor) {
///////////////////////////////////////////////////////////////////////
	this->xMin *= factor;
	this->xMax *= factor;
	this->yMin *= factor;
	this->yMax *= factor;
	this->zMin *= factor;
	this->zMax *= factor;
}
///////////////////////////////////////////////////////////////////////
template <class T>
void CncBoundaries<T>::multiply(T fX, T fY, T fZ) {
///////////////////////////////////////////////////////////////////////
	this->xMin *= fX;
	this->xMax *= fX;
	this->yMin *= fY;
	this->yMax *= fY;
	this->zMin *= fZ;
	this->zMax *= fZ;
}
///////////////////////////////////////////////////////////////////////
template <class T>
void CncBoundaries<T>::divide(T divisor) {
///////////////////////////////////////////////////////////////////////
	if ( divisor == T(0) )
		return;
		
	this->xMin /= divisor;
	this->xMax /= divisor;
	this->yMin /= divisor;
	this->yMax /= divisor;
	this->zMin /= divisor;
	this->zMax /= divisor;
}
///////////////////////////////////////////////////////////////////////
template <class T>
void CncBoundaries<T>::divide(T dX, T dY, T dZ) {
///////////////////////////////////////////////////////////////////////
	if ( dX == T(0) || dY == T(0) || dZ == T(0) )
		return;
		
	this->xMin /= dX;
	this->xMax /= dX;
	this->yMin /= dY;
	this->yMax /= dY;
	this->zMin /= dZ;
	this->zMax /= dZ;
}
///////////////////////////////////////////////////////////////////////
template <class T>
T CncBoundaries<T>::getMaxDistanceX() const {
///////////////////////////////////////////////////////////////////////
	return this->xMax - this->xMin;
}
///////////////////////////////////////////////////////////////////////
template <class T>
T CncBoundaries<T>::getMaxDistanceY() const {
///////////////////////////////////////////////////////////////////////
	return this->yMax - this->yMin;
}
///////////////////////////////////////////////////////////////////////
template <class T>
T CncBoundaries<T>::getMaxDistanceZ() const {
///////////////////////////////////////////////////////////////////////
	return this->zMax - this->zMin;
}
///////////////////////////////////////////////////////////////////////
template <class T>
float CncBoundaries<T>::getOriginRatioX() const {
///////////////////////////////////////////////////////////////////////
	const T axMin = abs(this->xMin);
	const T axMax = abs(this->xMax);
	const T total = axMin + axMax;
	
	if ( total == T(0) )
		return 0.0;
	
	return float(double(axMin) / double(total));
}
///////////////////////////////////////////////////////////////////////
template <class T>
float CncBoundaries<T>::getOriginRatioY() const {
///////////////////////////////////////////////////////////////////////
	const T ayMin = abs(this->yMin);
	const T ayMax = abs(this->yMax);
	const T total = ayMin + ayMax;
	
	if ( total == T(0) )
		return 0.0;
		
	return float(double(ayMin) / double(total));
}
///////////////////////////////////////////////////////////////////////
template <class T>
float CncBoundaries<T>::getOriginRatioZ() const {
///////////////////////////////////////////////////////////////////////
	const T azMin = abs(this->zMin);
	const T azMax = abs(this->zMax);
	const T total = azMin + azMax;
	
	if ( total == T(0) )
		return 0.0;
		
	return float(double(azMin) / double(total));
}
///////////////////////////////////////////////////////////////////////
template <class T>
void CncBoundaries<T>::trace(std::ostream& o) const {
///////////////////////////////////////////////////////////////////////
	CncBoundaries<T>::Corners corners;
	getAllCorners(corners, CncBoundaries<T>::CornerArea::CA_ALL);
	
	for ( auto it = corners.begin(); it != corners.end(); ++it )
	{
		const int distance = std::distance(corners.begin(), it);
		const CncPosition<T>& p = (*it);
		
		o	<< getCornerName(CA_ALL, distance) << ": "
			<< std::fixed << std::setw(10) << std::setprecision(6) << std::setfill(' ')
			<< p
			<< std::endl
		;
	}
	
	o << wxString::Format("Bound (X)(Y)(Z): (%.3lf, %.3lf)(%.3lf, %.3lf)(%.3lf, %.3lf)\n",
							this->xMin, this->xMax,
							this->yMin, this->yMax,
							this->zMin, this->zMax
						)
	;
}
///////////////////////////////////////////////////////////////////////
template <class T>
CncBoundaries<T> CncBoundaries<T>::normalize() const {
///////////////////////////////////////////////////////////////////////
	CncBoundaries<T> ret(*this);
	
	const CncPosition<T> p(
			fabs(this->xMin),
			fabs(this->yMin),
			fabs(this->zMin)
	);
	
	ret.shift(p);
	return ret;
}

// Explicit template instantiation
template class CncBoundaries<int16_t>;
template class CncBoundaries<int32_t>;
template class CncBoundaries<double>;
template class CncBoundaries<float>;
