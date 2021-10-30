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

// -------------------------------------------------------------------
// global strings
	#define _maxSpeedLabel				"<MAX>"

	#define _portSeparator				"-------------------------------"
	#define _portPreProcMonitor			"<Preprocessor(Monitor)>"
	#define _portPreProcFile			"<Preprocessor(File)>"
	#define _portEmulatorNULL			"<PortEmulator(dev/null)>"
	#define _portEmulatorTEXT			"<PortEmulator(File::Text)>"
	#define _portEmulatorSVG			"<PortEmulator(File::SVG)>"
	#define _portEmulatorGCODE			"<PortEmulator(File::GCode)>"
	#define _portEmulatorBIN			"<PortEmulator(File::Binary)>"
	#define _portEmulatorArduino		"<PortEmulator(Arduino)>"

// -------------------------------------------------------------------
// global macros
	#define CNC_FORMAT(...)				wxString::Format(__VA_ARGS__)
	#define CNC_SRC_PATH_FILE			__FILE__
	#define CNC_SRC_FILE				wxFileName(__FILE__).GetFullName()

	#define CNC_LOG_LOCATION			wxString::Format("%s: %s: Line %d", 	CNC_SRC_FILE, __PRETTY_FUNCTION__, __LINE__)
	#define CNC_LOG_LOCATION_A(...)		wxString::Format("%s: %s: Line %d: %s", CNC_SRC_FILE, __PRETTY_FUNCTION__, __LINE__, wxString::Format(__VA_ARGS__))
	#define CNC_LOG_FUNCT				wxString::Format( "%s", 				__PRETTY_FUNCTION__ )
	#define CNC_LOG_FUNCT_A(...)		wxString::Format( "%s: %s",				__PRETTY_FUNCTION__, wxString::Format(__VA_ARGS__))
	#define CNC_LOG_FUNCT_B(...)		wxString::Format( "%s: %s",				__FUNCTION__, wxString::Format(__VA_ARGS__))
	
	#define CNC_COUT_LOCATION			std::cout << CNC_LOG_LOCATION									<< std::endl;
	#define CNC_COUT_LOCATION_A(...)	std::cout << CNC_LOG_LOCATION_A(wxString::Format(__VA_ARGS__))	<< std::endl;
	#define CNC_COUT_FUNCT 				std::cout << CNC_LOG_FUNCT										<< std::endl;
	#define CNC_COUT_FUNCT_A(...)		std::cout << CNC_LOG_FUNCT_A(wxString::Format(__VA_ARGS__))		<< std::endl;
	#define CNC_COUT_FUNCT_B(...)		std::cout << CNC_LOG_FUNCT_B(wxString::Format(__VA_ARGS__))		<< std::endl;
	
	#define CNC_CLOG_LOCATION			std::clog << CNC_LOG_LOCATION									<< std::endl;
	#define CNC_CLOG_LOCATION_A(...)	std::clog << CNC_LOG_LOCATION_A(wxString::Format(__VA_ARGS__))	<< std::endl;
	#define CNC_CLOG_FUNCT 				std::clog << CNC_LOG_FUNCT										<< std::endl;
	#define CNC_CLOG_FUNCT_A(...)		std::clog << CNC_LOG_FUNCT_A(wxString::Format(__VA_ARGS__))		<< std::endl;
	#define CNC_CLOG_FUNCT_B(...)		std::clog << CNC_LOG_FUNCT_B(wxString::Format(__VA_ARGS__))		<< std::endl;
	
	#define CNC_CERR_LOCATION			std::cerr << CNC_LOG_LOCATION									<< std::endl;
	#define CNC_CERR_LOCATION_A(...)	std::cerr << CNC_LOG_LOCATION_A(wxString::Format(__VA_ARGS__))	<< std::endl;
	#define CNC_CERR_FUNCT 				std::cerr << CNC_LOG_FUNCT										<< std::endl;
	#define CNC_CERR_FUNCT_A(...)		std::cerr << CNC_LOG_FUNCT_A(wxString::Format(__VA_ARGS__))		<< std::endl;
	#define CNC_CERR_FUNCT_B(...)		std::cerr << CNC_LOG_FUNCT_B(wxString::Format(__VA_ARGS__))		<< std::endl;
	
	#define CNC_CEX1_LOCATION			cnc::cex1 << CNC_LOG_LOCATION									<< std::endl;
	#define CNC_CEX1_LOCATION_A(...)	cnc::cex1 << CNC_LOG_LOCATION_A(wxString::Format(__VA_ARGS__))	<< std::endl;
	#define CNC_CEX1_FUNCT 				cnc::cex1 << CNC_LOG_FUNCT										<< std::endl;
	#define CNC_CEX1_FUNCT_A(...)		cnc::cex1 << CNC_LOG_FUNCT_A(wxString::Format(__VA_ARGS__))		<< std::endl;
	#define CNC_CEX1_FUNCT_B(...)		cnc::cex1 << CNC_LOG_FUNCT_B(wxString::Format(__VA_ARGS__))		<< std::endl;
	
	#define CNC_PRINT_LOCATION			CNC_COUT_LOCATION
	#define CNC_PRINT_LOCATION_A(...)	CNC_COUT_LOCATION_A(__VA_ARGS__)
	#define CNC_PRINT_FUNCT 			CNC_COUT_FUNCT
	#define CNC_PRINT_FUNCT_A(...)		CNC_COUT_FUNCT_A(__VA_ARGS__)
	#define CNC_PRINT_FUNCT_B(...)		CNC_COUT_FUNCT_B(__VA_ARGS__)
	
	#define CNC_DISPLAY_LOCATION		wxMessageBox(CNC_LOG_LOCATION,									"CNC_DISPLAY_LOCATION");
	#define CNC_DISPLAY_LOCATION_A(...)	wxMessageBox(CNC_LOG_LOCATION_A(wxString::Format(__VA_ARGS__)),	"CNC_DISPLAY_LOCATION_A");
	#define CNC_DISPLAY_FUNCT			wxMessageBox(CNC_LOG_FUNCT,										"CNC_DISPLAY_FUNCT");
	#define CNC_DISPLAY_FUNCT_A(...)	wxMessageBox(CNC_LOG_FUNCT_A(wxString::Format(__VA_ARGS__)),	"CNC_DISPLAY_FUNCT_A");
	#define CNC_DISPLAY_FUNCT_B(...)	wxMessageBox(CNC_LOG_FUNCT_B(wxString::Format(__VA_ARGS__)),	"CNC_DISPLAY_FUNCT_B");

// -------------------------------------------------------------------
// common ostream operators
	std::ostream& operator<<(std::ostream& os, const wxPoint& p);
	std::ostream& operator<<(std::ostream& os, const wxRealPoint& p);
	std::ostream& operator<<(std::ostream& os, const wxColour& c);

// -------------------------------------------------------------------
// define globally const values
	static const float 	PI 									=  3.14159265;
	static const int 	MAX_PARAMETER_VALUES 				=  10;
	static const int 	UNDEFINED_LINE_NUMBER 				= -1;

// -------------------------------------------------------------------
static struct ClientIds {
	
	#define ClientIds_FACTOR 10L
	
	const long	TPL_FACTOR				= ClientIds_FACTOR;
	const long	INVALID					=    -1L;
	const long	SVG_Z_UP				= -1000L;
	const long	SVG_Z_DOWN				= -2000L;
	
	static const long normalize(long id)	{ return ( ( id / ClientIds_FACTOR ) * ClientIds_FACTOR ); }
	static const long lineNumber(long id)	{ return ( id / ClientIds_FACTOR ); }
	
	#undef FACTOR
	
} CLIENT_ID;

// -------------------------------------------------------------------
// global enumartions
	enum CncUnit 					{ CncSteps, CncMetric };
	enum CncAxis					{ Axis_X, Axis_Y, Axis_Z, Axis_H };
	enum CncEdge					{ cncLeft, cncTop, cncRight, cncBottom, cncCenter };
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
	enum CncRefPositionMode 		{ CncRM_Unknown = 0, CncRM_Mode1 = 1, CncRM_Mode2 = 2, CncRM_Mode3 = 3, CncRM_Mode4 = 4, CncRM_Mode5 = 5, CncRM_Mode6 = 6, CncRM_Touchblock = 7, CncRM_Camera = 8 };

// -------------------------------------------------------------------
namespace cnc {
	
	typedef std::vector<long>							LongValues;
	typedef std::vector<float>							FloatValues;
	typedef std::vector<double>							DoubleValues;
	typedef std::vector<int32_t> 						SetterValueList;
	typedef std::map<unsigned long, unsigned long> 		LineNumberTranslater;
	
	extern CncBasicLogStream							cex1;
	extern CncTraceLogStream							trc;
	extern CncMsgLogStream								msg;
	extern CncSerialSpyStream							spy;
	
	const char RAPID_SPEED_CHAR 						= 'R';
	const char WORK_SPEED_CHAR							= 'W';
	const char MAX_SPEED_CHAR							= 'M';
	const char USER_DEFIND_SPEED_CHAR					= 'U';
	
	const wxString&				lformat(wxString& str,   unsigned int len, char c = ' ');
	const wxString&				rformat(wxString& str,   unsigned int len, char c = ' ');
	const wxString&				lformat(const char* str, unsigned int len, char c = ' ');
	const wxString&				rformat(const char* str, unsigned int len, char c = ' ');
	
	const wxString&				longFormat(const CncLongPosition& p);
	const wxString&				dblFormat(const CncDoublePosition& p);
	
	const wxString&				dblFormat1(const double d);
	const wxString&				dblFormat2(const double d1, const double d2, const wxString& delimiter = _T(","));
	const wxString&				dblFormat3(const double d1, const double d2, const double d3, const wxString& delimiter = _T(","));
	
	const char 					getCncSpeedTypeAsCharacter(CncSpeedMode s);
	const char*					getCncSpeedTypeAsString(CncSpeedMode s);
	CncSpeedMode				getCncSpeedType(char m);
	float						getSpeedValue(CncStepSensitivity s);
	float						getSpeedValue(float amplitude);
	int							getSpeedStepSensitivityIndex(CncStepSensitivity s);
	int							getSpeedStepSensitivityIndex(float amplitude);
	const std::vector<float>&	getSpeedStepSensitivityFactors(std::vector<float>& ret);
	bool						isCncSpeedType(char m);
	
	const CncTemplateFormat		getTemplateFormatFromFileName(const char* fileName);
	const CncTemplateFormat		getTemplateFormatFromExtention(const char* extention);
	const char*					getTemplateFormatAsString(const CncTemplateFormat tf);
	const char*					getExtention(const CncTemplateFormat tf);
	
	const char*					getReferenceModeAsString(CncRefPositionMode m);
	
	void						traceSetterValueList(std::ostream& s, unsigned char pid, const SetterValueList& values, int32_t factor = 1);
	void						traceLineNumberTranslater(std::ostream& s, const LineNumberTranslater& lnt);
	
	CncStepSensitivity			getStepSensitivityOfIndex(unsigned int index);
	
	bool						dblCompare(const double a, const double b, const double eps = 0.000001 /*std::numeric_limits<double>::epsilon()*/);
	bool						dblCompareNull(const double a, const double eps = std::numeric_limits<double>::epsilon());
	
	namespace dbl {
								const double MIN = -DBL_MAX;
								const double MAX = +DBL_MAX;
	};
	
	struct FloatingComparePara {
		static constexpr float epsilon = 0.000001;
	};
	
	template <typename T, typename P>
	struct FloatingCompare {
								static constexpr T epsilon = P::epsilon;
								
								static bool eq(const T &a, const T &b)	{ return std::fabs(a - b) <= epsilon; }
								static bool nu(const T &a)				{ return eq(a, 0.0); }
								
								static bool lt(const T &a, const T &b)	{ return eq(a, b) ? false : a < b; }
								static bool gt(const T &a, const T &b)	{ return eq(a, b) ? false : a > b; }
								static bool le(const T &a, const T &b)	{ return eq(a, b) ? true  : a < b; } 
								static bool ge(const T &a, const T &b)	{ return eq(a, b) ? true  : a > b; }
	};
	
	typedef FloatingCompare<float,  FloatingComparePara>	fltCmp;
	typedef FloatingCompare<double, FloatingComparePara>	dblCmp;
	
	template <class T, class Compare>
	bool between(T val, T lo, T hi, Compare comp) 	{ return comp(lo, val) && comp(val, hi); }
	
	template<class T>
	bool between(T val, T lo, T hi) 				{ return between( val, lo, hi, std::less<T>() ); }
};

//-----------------------------------------------------------------
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

//-----------------------------------------------------------------
class SecureLeftBookSelection {
	public:
		enum VAL {
			FILE_PANEL				= 0,
			PROCESS_PANEL			= 1
		};
};

//-----------------------------------------------------------------
class SecurePreviewBookSelection {
	public:
		enum VAL {
			LEFT_PREVIEW			= 0,
			RIGHT_PREVIEW			= 1,
			CAMERA_PREVIEW			= 2
		};
};

//-----------------------------------------------------------------
class CncPathListManager;
namespace Trigger {
	
	
	struct ParameterSet {
		
		std::string fullFileName;
		
		struct Source {
			wxString fileName;
			wxString fileType;
		} SRC;

		struct Setup {
			float hardwareResX	= 0.0;
			float hardwareResY	= 0.0;
			float hardwareResZ	= 0.0;
			 
		} SET;
		
		struct Process {
			std::string user;
			 
		} PRC;
	};
	
	struct BeginRun {
		const Trigger::ParameterSet parameter;
		
		BeginRun(const Trigger::ParameterSet& ps)
		: parameter(ps)
		{}
	};
	
	struct EndRun {
		const bool succcess = false;
		
		explicit EndRun(bool s)
		: succcess(s)
		{}
	};
	
	struct NextPath {
		NextPath()
		{}
	};
	
	struct SpeedChange {
		const CncSpeedMode	currentSpeedMode;
		const double		currentSpeedValue;
		
		SpeedChange(CncSpeedMode sm = CncSpeedUserDefined, double sv = 0.0)
		: currentSpeedMode (sm)
		, currentSpeedValue(sv)
		{}
		
	};
	
	struct GuidePath {
		const CncPathListManager* plm;
		
		GuidePath(const CncPathListManager* m)
		: plm(m)
		{}
	};
};

#endif
