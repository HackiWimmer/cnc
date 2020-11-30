#include "CncCommon.h"
#include "CncClipperLib.h"

//////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const ClipperLib::IntPoint& p) {
//////////////////////////////////////////////////////////////
	os << wxString::Format("%10ld,%10ld,%10ld", p.X, p.Y, p.Z);
	return os;
}
//////////////////////////////////////////////////////////////////
double ClipperLib::convert(double v) {
//////////////////////////////////////////////////////////////////
	return v * ClipperLib::floatFactor;
}
//////////////////////////////////////////////////////////////////
double ClipperLib::transform(const cInt& v) {
//////////////////////////////////////////////////////////////////
	return ((double)v) /  ClipperLib::floatFactor;
}
//////////////////////////////////////////////////////////////////
ClipperLib::cInt ClipperLib::transform(double v) {
//////////////////////////////////////////////////////////////////
	return v * ClipperLib::floatFactor;
}
//////////////////////////////////////////////////////////////////
ClipperLib::IntPoint ClipperLib::transform(CncLongPosition p) { 
//////////////////////////////////////////////////////////////////
	return ClipperLib::IntPoint(p.getX(), 
								p.getY(), 
								p.getZ()
							   ); 
}
//////////////////////////////////////////////////////////////////
ClipperLib::IntPoint ClipperLib::transform(double pX, double pY, double pZ) { 
//////////////////////////////////////////////////////////////////
	return ClipperLib::IntPoint(pX * ClipperLib::floatFactor, 
								pY * ClipperLib::floatFactor, 
								pZ * ClipperLib::floatFactor
							   ); 
}
//////////////////////////////////////////////////////////////////
ClipperLib::IntPoint ClipperLib::transform(CncDoublePosition p) { 
//////////////////////////////////////////////////////////////////
	return ClipperLib::IntPoint(p.getX() * ClipperLib::floatFactor, 
								p.getY() * ClipperLib::floatFactor, 
								p.getZ() * ClipperLib::floatFactor
							   ); 
}
//////////////////////////////////////////////////////////////////
CncLongPosition ClipperLib::asCncLongPosition(IntPoint p) {
//////////////////////////////////////////////////////////////////
	return CncLongPosition(	p.X, 
							p.Y, 
							p.Z
						  );
}
//////////////////////////////////////////////////////////////////
CncDoublePosition ClipperLib::asCncDoublePosition(IntPoint p) {
//////////////////////////////////////////////////////////////////
	return CncDoublePosition(	((double)p.X) /  ClipperLib::floatFactor, 
								((double)p.Y) /  ClipperLib::floatFactor, 
								((double)p.Z) /  ClipperLib::floatFactor
							);
}
//////////////////////////////////////////////////////////////////
bool ClipperLib::isPathClosed(const ClipperLib::Path& p) {
//////////////////////////////////////////////////////////////////
	if ( p.size() == 0 )
		return true;
		
	return ( *p.begin() == *(p.end() - 1) );
}
//////////////////////////////////////////////////////////////////
bool ClipperLib::closePath(ClipperLib::Path& p) {
//////////////////////////////////////////////////////////////////
	if ( ClipperLib::isPathClosed(p) == true )
		return true;
	
	if ( p.size() > 1 )
		p.push_back(*p.begin());
	
	return p.size() > 2;
}


//////////////////////////////////////////////////////////////////
bool ClipperLib::hasZAxisMovement(const Path& path) {
//////////////////////////////////////////////////////////////////
	for ( auto it=path.cbegin(); it !=path.cend(); ++it) {
		const IntPoint& p = *it;
		if ( p.Z != 0 )
			return true;
	}
	
	return false;
}
//////////////////////////////////////////////////////////////////
bool ClipperLib::isHelix(const Path& path, double diameter) {
//////////////////////////////////////////////////////////////////
	#warning helix - may be
	std::clog << CNC_LOG_FUNCT_A("may be an helix\n");

	
	if ( cnc::dblCmp::le(diameter, 0.0) )
		return false;
	
	if ( path.size() <= 10 )
		return false;
	
	const double refScope = PI * diameter * 1.005;
	double length = 0.0;
	
	#warning helix - better checking
	
	for ( auto it=path.cbegin() + 1; it !=path.cend(); ++it) {
		const IntPoint& p1 = *(it - 1);
		const IntPoint& p2 = *(it);
		
		// stop if path has a z component
		if ( p1.Z != 0 )
			return false;
		
		// concatinate the current path length
		length += sqrt( pow(p2.X - p1.X, 2) + pow(p2.Y - p1.Y, 2) );
		
		std::cout << "L:  "<< length / ClipperLib::floatFactor << " > " << refScope << std::endl;
		
		// stop if path legth > the scope of the given diameter
		// convert length to compare metric with metric
		if ( length / ClipperLib::floatFactor > refScope )
			return false;
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
ClipperLib::cInt ClipperLib::getPathLength(const Path& path) {
//////////////////////////////////////////////////////////////////
	if ( path.size() <= 1 )
		return 0;
		
	double length = 0.0;
	
	for ( auto it=path.cbegin() + 1; it !=path.cend(); ++it) {
		const IntPoint& p1 = *(it - 1);
		const IntPoint& p2 = *(it);
		
		length += sqrt(pow(p2.X - p1.X, 2) + pow(p2.Y - p1.Y, 2) + pow(p2.Z - p1.Z, 2));
	}
	
	return round(length);
}

