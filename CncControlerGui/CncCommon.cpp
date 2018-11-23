#include <wx/filename.h>
#include "CncCommon.h"

//////////////////////////////////////////////////////////////
void cnc::traceSetterValueList(std::ostream& s, const cnc::SetterValueList& values, int32_t factor) {
//////////////////////////////////////////////////////////////
	unsigned int counter = 0;
	for ( auto it = values.begin(); it != values.end(); it++) {
		
		if ( factor == 1 ) 	s << wxString::Format("%ld",   *it);
		else				s << wxString::Format("%.2lf", (double)(*it)/factor);
		
		if ( ++counter != values.size() )
			s << ", ";
	}
}
//////////////////////////////////////////////////////////////
void cnc::traceLineNumberTranslater(std::ostream& s, const LineNumberTranslater& lnt) {
//////////////////////////////////////////////////////////////
	for ( auto it = lnt.begin(); it != lnt.end(); it++) {
		s << wxString::Format("%u -> %u\n",  it->first, it->second);
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
const char cnc::getCncSpeedTypeAsCharacter(CncSpeedMode s) {
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
const char* cnc::getTemplateFormatAsString(const TemplateFormat tf) {
//////////////////////////////////////////////////////////////
	switch ( tf ) {
		case TplUnknown:	return "UNKNOWN";
		case TplText:		return "TXT";
		case TplSvg:		return "SVG";
		case TplGcode:		return "GCODE";
		case TplBinary:		return "BIN"; 
		case TplManual:		return "MANUAL"; 
		case TplTest:		return "TEST"; 
	}
	
	return "????";
}
//////////////////////////////////////////////////////////////
const TemplateFormat cnc::getTemplateFormatFromExtention(const char* extention) {
//////////////////////////////////////////////////////////////
	if ( extention == NULL )
		return TplUnknown;
	
	wxString e(extention);
	e.MakeUpper();
	
	if      ( e == "SVG" )		return TplSvg;
	else if ( e == "GCODE") 	return TplGcode;
	else if ( e == "NGC") 		return TplGcode;
	else if ( e == "BCT") 		return TplBinary;
	else if ( e == "TXT") 		return TplText;
	
	return TplUnknown;
}
//////////////////////////////////////////////////////////////
const TemplateFormat cnc::getTemplateFormatFromFileName(const char* fileName) {
//////////////////////////////////////////////////////////////
	if ( fileName == NULL )
		return TplUnknown;
		
	wxFileName fn(fileName);
	wxString ext(fn.GetExt());
	
	return cnc::getTemplateFormatFromExtention(ext);
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

