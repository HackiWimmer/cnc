#include "CncCommon.h"

//////////////////////////////////////////////////////////////
bool cnc::dblCompare( double a, double b, double eps) {
//////////////////////////////////////////////////////////////
	return std::abs(a - b) <= eps; 
}
//////////////////////////////////////////////////////////////
bool cnc::dblCompareNull( double a, double eps) {
//////////////////////////////////////////////////////////////
	return cnc::dblCompare(a, 0.0, eps);
	
}
//////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const wxPoint& p) {
//////////////////////////////////////////////////////////////
	os << wxString::Format("%10ld,%10ld ", p.x, p.y);
	return os;
}
//////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const wxRealPoint& p) {
//////////////////////////////////////////////////////////////
	os << wxString::Format("%10.3lf,%10.3lf ", p.x, p.y);
	return os;
}