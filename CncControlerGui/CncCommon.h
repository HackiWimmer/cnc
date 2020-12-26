#ifndef CNC_COMMON_INCLUDES
#define CNC_COMMON_INCLUDES

#include <cmath>
#include <cfloat>
#include <limits>
#include <map>
#include <wx/filename.h>
#include <wx/msgdlg.h>

#include "CncPosition.h"
#include "CncStreamBuffers.h"
#include "StdStreamRedirector.h"

// global strings
#define _maxSpeedLabel			"<MAX>"

#define _portEmulatorNULL 		"<PortEmulator(dev/null)>"
#define _portEmulatorTEXT  		"<PortEmulator(File::Text)>"
#define _portEmulatorSVG  		"<PortEmulator(File::SVG)>"
#define _portEmulatorGCODE		"<PortEmulator(File::GCode)>"
#define _portEmulatorBIN  		"<PortEmulator(File::Binary)>"
#define _portEmulatorArduino 	"<PortEmulator(Arduino)>"

// common ostream operators
std::ostream& operator<<(std::ostream& os, const wxPoint& p);
std::ostream& operator<<(std::ostream& os, const wxRealPoint& p);
std::ostream& operator<<(std::ostream& os, const wxColour& c);

// make std globally available
//using namespace std;

// define globally const values
static const float 	PI 									=  3.14159265;
static const int 	MAX_PARAMETER_VALUES 				=  10;
static const int 	UNDEFINED_LINE_NUMBER 				= -1;

namespace cnc {
	
	typedef std::vector<long>	LongValues;
	typedef std::vector<float>	FloatValues;
	typedef std::vector<double>	DoubleValues;
	
};

static struct ClientIds {
	const long	TPL_FACTOR				=    10L;
	const long	INVALID					=    -1L;
	const long	SVG_Z_UP				= -1000L;
	const long	SVG_Z_DOWN				= -2000L;
} CLIENT_ID;

enum CncUnit 					{ CncSteps, CncMetric };
enum CncInteractiveMoveDriver	{ IMD_NONE, IMD_GAMEPAD, IMD_NAVIGATOR };
enum CncDirection 				{ CncUndefDir, CncClockwise, CncCounterClockwise };
enum CncLinearDirection			{ CncNoneDir = 0, CncPosDir = 1, CncNegDir = -1};
enum CncSpeedMode				{ CncSpeedWork = 0, CncSpeedRapid = 1, CncSpeedMax = 2, CncSpeedUserDefined = 3 }; // dont change the values
enum CncPortType 				{ CncPORT, CncPORT_EMU_ARDUINO, CncEMU_NULL, CncEMU_TXT, CncEMU_SVG, CncEMU_GCODE, CncEMU_BIN };
enum CncPathModificationType 	{ CncPM_None=0, CncPM_Inner, CncPM_Outer, CncPM_Center, CncPM_Pocket };
enum CncPathRuleType 			{ CncPR_None=0, CncPR_EnsureClockwise, CncPR_EnsureCounterClockwise, CncPR_Reverse };
enum CncClipperCornerType 		{ CncCCT_Round=0, CncCCT_Square=1, CncCCT_Miter=2 };
enum CncStepSensitivity 		{ FINEST = 1, FINE = 20 , MEDIUM = 50, ROUGH = 80, ROUGHEST = 100 };
enum CncStepMode		 		{ SM_INTERACTIVE = 0, SM_STEPWISE = 1 };
enum CncClipperEndType			{ CncCET_ClosedPolygon=0, CncCETClosedLine=1, CncCETOpenSquare=2, CncCETOpenRound=3, CncCETOpenButt=4 };
enum CncTemplateFormat 			{ TplUnknown, TplText, TplSvg, TplGcode, TplBinary, TplManual, TplTest };
enum CncDimensions 				{ CncDimension1D = 1, CncDimension2D = 2, CncDimension3D = 3 };
enum CncRefPositionMode 		{ CncRM_Unknown = 0, CncRM_Mode1 = 1, CncRM_Mode2 = 2, CncRM_Mode3 = 3, CncRM_Mode4 = 4, CncRM_Mode5 = 5, CncRM_Mode6 = 6 };

namespace cnc {
	
	const wxString& lformat(wxString& str,   unsigned int len, char c = ' ');
	const wxString& rformat(wxString& str,   unsigned int len, char c = ' ');
	const wxString& lformat(const char* str, unsigned int len, char c = ' ');
	const wxString& rformat(const char* str, unsigned int len, char c = ' ');
	
};

#define CNC_SRC_PATH_FILE			__FILE__
#define CNC_SRC_FILE				wxFileName(__FILE__).GetFullName()

#define CNC_LOG_LOCATION			wxString::Format("%s: %s: Line %d", 	CNC_SRC_FILE, __PRETTY_FUNCTION__, __LINE__)
#define CNC_LOG_LOCATION_A(msg)		wxString::Format("%s: %s: Line %d: %s", CNC_SRC_FILE, __PRETTY_FUNCTION__, __LINE__, msg)
#define CNC_LOG_FUNCT				wxString::Format( "%s", 				__PRETTY_FUNCTION__ )
#define CNC_LOG_FUNCT_A(msg)		wxString::Format( "%s: %s",				__PRETTY_FUNCTION__, msg)

#define CNC_PRINT_LOCATION			std::cout << CNC_LOG_LOCATION 			<< std::endl;
#define CNC_PRINT_LOCATION_A(msg)	std::cout << CNC_LOG_LOCATION_A(msg) 	<< std::endl;
#define CNC_PRINT_FUNCT 			std::cout << CNC_LOG_FUNCT 				<< std::endl;
#define CNC_PRINT_FUNCT_A(msg)		std::cout << CNC_LOG_FUNCT_A(msg) 		<< std::endl;


namespace cnc {
	
	typedef std::vector<int32_t> 						SetterValueList;
	typedef std::map<unsigned long, unsigned long> 		LineNumberTranslater;

	const char RAPID_SPEED_CHAR 		= 'R';
	const char WORK_SPEED_CHAR			= 'W';
	const char MAX_SPEED_CHAR 			= 'M';
	const char USER_DEFIND_SPEED_CHAR 	= 'U';
	
	const char 		getCncSpeedTypeAsCharacter(CncSpeedMode s);
	const char*		getCncSpeedTypeAsString(CncSpeedMode s);
	CncSpeedMode	getCncSpeedType(char m);
	bool			isCncSpeedType(char m);
	
	extern CncBasicLogStream  cex1;
	extern CncTraceLogStream  trc;
	extern CncMsgLogStream    msg;
	extern CncSerialSpyStream spy;
	
	template <class T, class Compare>
	bool between(T val, T lo, T hi, Compare comp) 	{ return comp(lo, val) && comp(val, hi); }

	template<class T>
	bool between(T val, T lo, T hi) 				{ return between( val, lo, hi, std::less<T>() ); }
	
	bool dblCompare(const double a, const double b, const double eps = 0.000001 /*std::numeric_limits<double>::epsilon()*/);
	bool dblCompareNull(const double a, const double eps = std::numeric_limits<double>::epsilon());
	
	namespace dblCmp {

		const double epsilon = 0.000001;

		inline bool eq(const double &a, const double &b)	{ return std::fabs(a - b) <= epsilon; }
		inline bool nu(const double &a)						{ return eq(a, 0.0); }
		
		inline bool lt(const double &a, const double &b)	{ return eq(a, b) ? false : a < b; }
		inline bool gt(const double &a, const double &b)	{ return eq(a, b) ? false : a > b; }
		inline bool le(const double &a, const double &b)	{ return eq(a, b) ? true  : a < b; } 
		inline bool ge(const double &a, const double &b)	{ return eq(a, b) ? true  : a > b; }
	};
	
	const wxString& longFormat(const CncLongPosition& p);
	const wxString& dblFormat(const CncDoublePosition& p);
	
	const wxString& dblFormat1(const double d);
	const wxString& dblFormat2(const double d1, const double d2, const wxString& delimiter = _T(","));
	const wxString& dblFormat3(const double d1, const double d2, const double d3, const wxString& delimiter = _T(","));
	
	const CncTemplateFormat getTemplateFormatFromFileName(const char* fileName);
	const CncTemplateFormat getTemplateFormatFromExtention(const char* extention);
	const char* getTemplateFormatAsString(const CncTemplateFormat tf);
	const char* getExtention(const CncTemplateFormat tf);
	
	void traceSetterValueList(std::ostream& s, unsigned char pid, const SetterValueList& values, int32_t factor = 1);
	void traceLineNumberTranslater(std::ostream& s, const LineNumberTranslater& lnt);
	
	CncStepSensitivity getStepSensitivityOfIndex(unsigned int index);
	float getSpeedValue(CncStepSensitivity s);
};

namespace ClientIdSelSource {

	enum ID {TSS_POS_SPY			=  0,
			TSS_REPLAY				=  1,
			TSS_PATH_LIST			=  2,
			TSS_EDITOR				=  3,
			TSS_MONITOR				=  4,
			TSS_GCODE_SEQ			=  5,
			TSS_MOVE_SEQ_OVW		=  6,
			TSS_MOVE_SEQ			=  7,
			TSS_VERTEX_DATA_TRACE	=  8,
			TSS_VERTEX_INDEX_TRACE	=  9
	};

	const char* getTemplateSelSourceAsString(const ID ts);
	const char* getTemplateSelSourceAsLongString(const ID ts);
};

//-----------------------------------------------------------------
class MainBookSelection {
	public:
		enum VAL {
			SOURCE_PANEL				= 0,
			SETUP_PANEL					= 1,
			REFERENCE_PANEL				= 2,
			MANUEL_PANEL				= 3,
			TEST_PANEL					= 4,
			PREVIEW_PANEL				= 5
		};
};

//-----------------------------------------------------------------
class MonitorBookSelection {
	public:
		enum VAL {
			CNC_PANEL					= 0,
			TEMPLATE_PANEL				= 1
		};
};

//-----------------------------------------------------------------
class TemplateBookSelection {
	public:
		enum VAL {
			SOURCE_PANEL				= 0,
			EXT_INFO_PANEL				= 1
		};
};

//-----------------------------------------------------------------
class SourceBookSelection {
	public:
		enum VAL {
			OBSERVER					= 0,
			EDITOR						= 1,
			CONTEXT						= 2
		};
};

//-----------------------------------------------------------------
class SourceExtBookSelection {
	public:
		enum VAL {
			NULL_PANEL					= 0,
			USER_AGENT_PANEL			= 1,
			GCODE_SEQUENCE_PANEL		= 2,
			NESTED_INFO_PANEL			= 3
		};
};

//-----------------------------------------------------------------
class OutboundSelection{
	public:
		enum VAL {
			SUMMARY_PANEL				= 0,
			PREPOCESSOR_PANAL			= 1,
			MOTION_MONITOR_PANAL		= 2,
			POSTPOCESSOR_PANAL			= 3
		};
};

//-----------------------------------------------------------------
class OutboundMonitorSelection{
	public:
		enum VAL {
			MOTION_MONITOR_PANAL		= 0,
			CNC_SETTER_PANEL			= 1,
			CNC_MSG_HIST_PANEL			= 2,
			MOTION_VERTEX_TRACE			= 3
		};
};

//-----------------------------------------------------------------
class PreProcessorSelection{
	public:
		enum VAL {
			PARSING_SYNOPSIS			= 0,
			OPERATIG_TRACE				= 1,
			PATH_LIST_ENTRIES			= 2,
			MOVE_SEQ_ENTRIES			= 3
		};
};

//-----------------------------------------------------------------
class PostProcessorSelection{
	public:
		enum VAL {
			OUTBOUND_SOURCE				= 0,
			OUTBOUND_PREVIEW			= 1
		};
};

//-----------------------------------------------------------------
class StatisticSelection{
	public:
		enum VAL {
			SUMMARY_PANEL				= 0
		};
};

//-----------------------------------------------------------------
class OutboundCfgSelection{
	public:
		enum VAL {
			SUMMARY_PANEL				= 0,
			CNC_CONFIG_PANEL			= 1,
			CNC_PIN_PANEL				= 2
		};
};

//-----------------------------------------------------------------
class TestBookSelection {
	public:
		enum VAL {
			INTERVAL					= 0,
			TOOL						= 1
		};
};

//-----------------------------------------------------------------
class LoggerSelection {
	public:
		enum VAL {
			STARTUP						= 0,
			CNC							= 1
		};
};

//-----------------------------------------------------------------
class PosSpySelection {
	public:
		enum VAL {
			SPEED_SPY					= 0,
			POSITION_SPY				= 1,
		};
};

//-----------------------------------------------------------------
class MontiorBottomContextSelection {
	public:
		enum VAL {
			STATISTIC_PANEL				= 0,
			REPLAY_PANEL				= 1
		};
};

#endif
