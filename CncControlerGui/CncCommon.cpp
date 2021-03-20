#include <functional>
#include <wx/filename.h>
#include "ArduinoEnvWrapper.h"
#include "CncCommon.h"

namespace cnc {
	const char* RAPID_SPEED_CHAR_DESC			= "Rappid speed";
	const char* WORK_SPEED_CHAR_DESC			= "Work speed";
	const char* MAX_SPEED_CHAR_DESC				= "Max speed";
	const char* USER_DEFIND_SPEED_CHAR_DESC		= "Userdefined speed";
};


//////////////////////////////////////////////////////////////
CncStepSensitivity cnc::getStepSensitivityOfIndex(unsigned int index) {
//////////////////////////////////////////////////////////////
	CncStepSensitivity stepSensitivity = CncStepSensitivity::FINEST;
	
	switch ( index ) {
		case 0: 	stepSensitivity = CncStepSensitivity::FINEST;		break;
		case 1: 	stepSensitivity = CncStepSensitivity::FINE; 		break;
		case 2: 	stepSensitivity = CncStepSensitivity::MEDIUM; 		break;
		case 3: 	stepSensitivity = CncStepSensitivity::ROUGH; 		break;
		case 4: 	stepSensitivity = CncStepSensitivity::ROUGHEST; 	break;
		
		default:	stepSensitivity = CncStepSensitivity::FINEST;
	}
	
	return stepSensitivity;
}
//////////////////////////////////////////////////////////////
float cnc::getSpeedValue(CncStepSensitivity s) {
//////////////////////////////////////////////////////////////
	double newSpeed = 1.0;
	
	switch ( s ) {
		case FINEST:	newSpeed = cnc::getSpeedValue(0.1);	break;
		case FINE:		newSpeed = cnc::getSpeedValue(0.2);	break;
		case MEDIUM:	newSpeed = cnc::getSpeedValue(0.5);	break;
		case ROUGH:		newSpeed = cnc::getSpeedValue(0.8);	break;
		case ROUGHEST:	newSpeed = cnc::getSpeedValue(1.0);	break;
	}
	
	return newSpeed;
}
//////////////////////////////////////////////////////////////
float cnc::getSpeedValue(float amplitude) {
//////////////////////////////////////////////////////////////
	amplitude = std::max(0.0f, amplitude);
	amplitude = std::min(1.0f, amplitude);
	
	// Any default value . . .
	// Don't increase the max speed to much high, because there's 
	// no de-acceleration phase at the end of a move and higher speeds
	//  hare not much hardware friendly
	double			newSpeed =    1.0;
	const double	maxSpeed = 4500.0;
	
	std::vector<float> factors;
	cnc::getSpeedStepSensitivityFactors(factors);
	
	if		( amplitude <= factors.at(0) )	newSpeed = 60.0;
	else if	( amplitude <= factors.at(1) )	newSpeed = maxSpeed * 0.2;
	else if ( amplitude <= factors.at(2) )	newSpeed = maxSpeed * 0.5;
	else if ( amplitude <= factors.at(3) )	newSpeed = maxSpeed * 0.8;
	else									newSpeed = maxSpeed * 1.0;
	
	return newSpeed;
}
//////////////////////////////////////////////////////////////
int cnc::getSpeedStepSensitivityIndex(float amplitude) {
//////////////////////////////////////////////////////////////
	amplitude = std::max(0.0f, amplitude);
	amplitude = std::min(1.0f, amplitude);
	
	std::vector<float> factors;
	cnc::getSpeedStepSensitivityFactors(factors);
	
	if		( amplitude <= factors.at(0) )	return 0;
	else if	( amplitude <= factors.at(1) )	return 1;
	else if ( amplitude <= factors.at(2) )	return 2;
	else if ( amplitude <= factors.at(3) )	return 3;
	else									return 4;
	
	return -1;
}
//////////////////////////////////////////////////////////////
const std::vector<float>& cnc::getSpeedStepSensitivityFactors(std::vector<float>& ret) {
//////////////////////////////////////////////////////////////
	ret.clear();
	ret.push_back(0.10);
	ret.push_back(0.25);
	ret.push_back(0.50);
	ret.push_back(0.90);
	ret.push_back(1.00);
	
	wxASSERT( ret.size() == 5 );
	
	return ret;
}
//////////////////////////////////////////////////////////////
const wxString& cnc::lformat(wxString& str, unsigned int len, char c) {
//////////////////////////////////////////////////////////////
	unsigned int l = 0;
	if ( ( l = str.length() ) < len )
		str.assign(wxString::Format("%s%s", str, wxString(c, len - l) ));
	
	return str;
}
//////////////////////////////////////////////////////////////
const wxString& cnc::lformat(const char* str, unsigned int len, char c) {
//////////////////////////////////////////////////////////////
	if ( str != NULL ) {
		wxString s(str);
		return lformat(s, len, c);
	}
	
	return _("");
}
//////////////////////////////////////////////////////////////
const wxString& cnc::rformat(wxString& str, unsigned int len, char c) {
//////////////////////////////////////////////////////////////
	unsigned int l = 0;
	if ( ( l = str.length() ) < len )
		str.assign(wxString::Format("%s%s", wxString(c, len - l), str ));
	
	return str;
}
//////////////////////////////////////////////////////////////
const wxString& cnc::rformat(const char* str, unsigned int len, char c) {
//////////////////////////////////////////////////////////////
	if ( str != NULL ) {
		wxString s(str);
		return rformat(s, len, c);
	}
	
	return _("");
}
//////////////////////////////////////////////////////////////
void cnc::traceSetterValueList(std::ostream& s, unsigned char pid, const cnc::SetterValueList& values, int32_t factor) {
//////////////////////////////////////////////////////////////
	
	//-------------------------------------------------------
	auto trace = [&](int32_t val, const wxString& fltForm = "%.2lf") {
		if ( factor == 1 ) 	s << wxString::Format("%ld",   (long)val);
		else				s << wxString::Format(fltForm, (double)(val)/factor);
	};
	
	unsigned int counter = 0;
	for ( auto it = values.begin(); it != values.end(); it++) {
		
		const int32_t val = (*it);
		switch ( pid ) {
			case PID_SPEED_MM_SEC:
			{
				trace(ArdoObj::SpeedTuple::decodeValue_MMSec1000(val));
				const char m = ArdoObj::SpeedTuple::decodeMode(val);
				s << " - " << ( m ? ArdoObj::SpeedTuple::decodeMode(val) : '?');
				break;
			}
			case PID_SPEED_MM_MIN:
			{
				trace(ArdoObj::SpeedTuple::decodeValue_MMSec1000(val) * 60);
				const char m = ArdoObj::SpeedTuple::decodeMode(val);
				s << " - " << ( m ? ArdoObj::SpeedTuple::decodeMode(val) : '?');
				break;
			}
			case PID_ACCEL_PROFILE:
			{
				trace(val, "%.3lf");
				break;
			}
			case PID_FEEDRATE_H:
			{
				trace(val, "%.4lf");
				break;
			}
			default:
			{
				trace(val);
			}
		}
		
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
bool cnc::dblCompare(const double a, const double b, const double eps) {
//////////////////////////////////////////////////////////////
	return std::fabs(a - b) <= eps; 
}
//////////////////////////////////////////////////////////////
bool cnc::dblCompareNull(const double a, const double eps) {
//////////////////////////////////////////////////////////////
	return cnc::dblCompare(a, 0.0, eps);
}
//////////////////////////////////////////////////////////////
const wxString& cnc::longFormat(const CncLongPosition& p) {
//////////////////////////////////////////////////////////////
	static wxString s;
	s.assign(wxString::Format("%10ld,%10ld,%10ld", p.getX(), p.getY(), p.getZ()));
	return s;
}
//////////////////////////////////////////////////////////////
const wxString& cnc::dblFormat(const CncDoublePosition& p) {
//////////////////////////////////////////////////////////////
	return cnc::dblFormat3(p.getX(), p.getY(), p.getZ());
}
//////////////////////////////////////////////////////////////
const wxString& cnc::dblFormat1(const double d1) {
//////////////////////////////////////////////////////////////
	static wxString s;
	s.assign(wxString::Format("%10.3lf", d1));
	return s;
}
//////////////////////////////////////////////////////////////
const wxString& cnc::dblFormat2(const double d1, const double d2, const wxString& delimiter) {
//////////////////////////////////////////////////////////////
	static wxString s;
	s.assign(wxString::Format("%10.3lf%s%10.3lf", d1, delimiter, d2));
	return s;
}
//////////////////////////////////////////////////////////////
const wxString& cnc::dblFormat3(const double d1, const double d2, const double d3, const wxString& delimiter) {
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
const char* cnc::getCncSpeedTypeAsString(CncSpeedMode s) {
//////////////////////////////////////////////////////////////
	switch( s ) {
		case CncSpeedWork: 			return cnc::WORK_SPEED_CHAR_DESC; 			break;
		case CncSpeedRapid:			return cnc::RAPID_SPEED_CHAR_DESC;			break;
		case CncSpeedMax:			return cnc::MAX_SPEED_CHAR_DESC; 			break;
		case CncSpeedUserDefined:	return cnc::USER_DEFIND_SPEED_CHAR_DESC; 	break;
	}
	
	return "Unknown Speed mode";

}
//////////////////////////////////////////////////////////////
CncSpeedMode cnc::getCncSpeedType(char m) {
//////////////////////////////////////////////////////////////
	switch( m ) {
		case cnc::WORK_SPEED_CHAR:	return CncSpeedWork;
		case cnc::RAPID_SPEED_CHAR:	return CncSpeedRapid;
		case cnc::MAX_SPEED_CHAR:	return CncSpeedMax;
	}
	
	return CncSpeedUserDefined;
}
//////////////////////////////////////////////////////////////
bool cnc::isCncSpeedType(char m) {
//////////////////////////////////////////////////////////////
	switch( m ) {
		case cnc::WORK_SPEED_CHAR:
		case cnc::RAPID_SPEED_CHAR:
		case cnc::MAX_SPEED_CHAR:
		case cnc::USER_DEFIND_SPEED_CHAR: return true;
	}
	
	return false;
}
//////////////////////////////////////////////////////////////
const char* cnc::getTemplateFormatAsString(const CncTemplateFormat tf) {
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
const char* cnc::getExtention(const CncTemplateFormat tf) {
//////////////////////////////////////////////////////////////
	switch ( tf ) {
		case TplUnknown:	return "unknown";
		case TplText:		return "txt";
		case TplSvg:		return "svg";
		case TplGcode:		return "gcode";
		case TplBinary:		return "bct"; 
		case TplManual:		return "unknown"; 
		case TplTest:		return "unknown"; 
	}
	
	return "unknown";
}
//////////////////////////////////////////////////////////////
const CncTemplateFormat cnc::getTemplateFormatFromExtention(const char* extention) {
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
const CncTemplateFormat cnc::getTemplateFormatFromFileName(const char* fileName) {
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
//////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const wxColour& c) {
//////////////////////////////////////////////////////////////
	os << wxString::Format("%3d, %3d, %3d, % 3d", (int)c.Red(), (int)c.Green(), (int)c.Blue(), (int)c.Alpha());
	return os;
}
//////////////////////////////////////////////////////////////
const char* ClientIdSelSource::getTemplateSelSourceAsString(const ID ts) {
//////////////////////////////////////////////////////////////
	switch ( ts ) {
		case TSS_POS_SPY:				return "TSS_POS_SPY";
		case TSS_REPLAY:				return "TSS_REPLAY";
		case TSS_PATH_LIST:				return "TSS_PATH_LIST";
		case TSS_EDITOR: 				return "TSS_EDITOR";
		case TSS_MONITOR:				return "TSS_MONITOR";
		case TSS_GCODE_SEQ:				return "TSS_GCODE_SEQ";
		case TSS_MOVE_SEQ_OVW:			return "TSS_MOVE_SEQ_OVW";
		case TSS_MOVE_SEQ:				return "TSS_MOVE_SEQ";
		case TSS_VERTEX_DATA_TRACE:		return "TSS_VERTEX_DATA_TRACE";
		case TSS_VERTEX_INDEX_TRACE:	return "TSS_VERTEX_INDEX_TRACE";
	}

	return "Unknown TemplateSelSource";
}
//////////////////////////////////////////////////////////////
const char* ClientIdSelSource::getTemplateSelSourceAsLongString(const ID ts) {
//////////////////////////////////////////////////////////////
	switch ( ts ) {
		case TSS_POS_SPY:				return "Position Spy";
		case TSS_REPLAY:				return "Replay Control";
		case TSS_PATH_LIST:				return "PathList ";
		case TSS_EDITOR: 				return "Template Editor";
		case TSS_MONITOR:				return "Motion Monitor";
		case TSS_GCODE_SEQ:				return "Parsed GCodes";
		case TSS_MOVE_SEQ_OVW:			return "Move Sequence Overview";
		case TSS_MOVE_SEQ:				return "Move Sequence";
		case TSS_VERTEX_DATA_TRACE:		return "Vertex trace";
		case TSS_VERTEX_INDEX_TRACE:	return "Vertex Index trace";
	}

	return "Unknown TemplateSelSource";
}

