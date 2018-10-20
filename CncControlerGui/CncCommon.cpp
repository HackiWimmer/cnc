#include "CncCommon.h"

//////////////////////////////////////////////////////////////
void traceSetterValueList(std::ostream& s, const SetterValueList& values, int32_t factor) {
//////////////////////////////////////////////////////////////
	unsigned int counter = 0;
	for ( auto it=values.begin(); it != values.end(); it++) {
		if ( factor == 1 ) 	s << wxString::Format("%ld",   *it);
		else				s << wxString::Format("%.2lf", (double)(*it)/factor);
		
		if ( ++counter != values.size() )
			s << ", ";
	}
}
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
const wxString& cnc::dblFormat1(double d1) {
//////////////////////////////////////////////////////////////
	static wxString s;
	s.assign(wxString::Format("%10.3lf", d1));
	return s;
}
//////////////////////////////////////////////////////////////
const wxString& cnc::dblFormat2(double d1, double d2, const wxString& delimiter) {
//////////////////////////////////////////////////////////////
	static wxString s;
	s.assign(wxString::Format("%10.3lf%s%10.3lf", d1, delimiter, d2));
	return s;
}
//////////////////////////////////////////////////////////////
const wxString& cnc::dblFormat3(double d1, double d2, double d3, const wxString& delimiter) {
//////////////////////////////////////////////////////////////
	static wxString s;
	s.assign(wxString::Format("%10.3lf%s%10.3lf%s%10.3lf", d1, delimiter, d2, delimiter, d3));
	return s;
}
//////////////////////////////////////////////////////////////
const char cnc::getCncSpeedTypeAsCharacter(CncSpeed s) {
//////////////////////////////////////////////////////////////
	switch( s ) {
		case CncSpeedWork: 			return cnc::WORK_SPEED_CHAR; 			break;
		case CncSpeedRapid:			return cnc::RAPID_SPEED_CHAR;			break;
		case CncSpeedMax:			return cnc::MAX_SPEED_CHAR; 			break;
		case CncSpeedUserDefined:	return cnc::USER_DEFIND_SPEED_CHAR; 	break;
	}
	
	return cnc::USER_DEFIND_SPEED_CHAR;
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

