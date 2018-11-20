#ifndef CNC_COMMON_INCLUDES
#define CNC_COMMON_INCLUDES

#include <cmath>
#include <cfloat>
#include <limits>
#include <map>

#include "CncStreamBuffers.h"
#include "StdStreamRedirector.h"


// common ostream operators
std::ostream& operator<<(std::ostream& os, const wxPoint& p);
std::ostream& operator<<(std::ostream& os, const wxRealPoint& p);

// global strings
#define _portSimulatorNULL	"<PortSimulator(dev/null)>"
#define _portEmulatorNULL 	"<PortEmulator(dev/null)>"
#define _portEmulatorSVG  	"<PortEmulator(SVGFile)>"
#define _portEmulatorBIN  	"<PortEmulator(Binary)>"
#define _maxSpeedLabel		"<MAX>"

// make std globally available
//using namespace std;

// define globally const values
static const float 	PI 									= 3.14159265;
static const int 	MAX_PARAMETER_VALUES 				= 10;
static const int 	UNDEFINED_LINE_NUMBER 				= -1;

enum CncUnit 					{ CncSteps, CncMetric };
enum CncDirection 				{ CncUndefDir, CncClockwise, CncAnticlockwise };
enum CncLinearDirection			{ CncNoneDir = 0, CncPosDir = 1, CncNegDir = -1};
enum CncSpeed 					{ CncSpeedWork, CncSpeedRapid, CncSpeedMax, CncSpeedUserDefined };
enum CncPortType 				{ CncPORT, CncPORT_SIMU, CncEMU_NULL, CncEMU_SVG, CncEMU_BIN };
enum CncToolCorretionType 		{ CncCT_None=0, CncCT_Inner=1, CncCT_Outer=2, CncCT_Center=3 };
enum CncClipperCornerType 		{ CncCCT_Round=0, CncCCT_Square=1, CncCCT_Miter=2 };
enum CncClipperEndType			{ CncCET_ClosedPolygon=0, CncCETClosedLine=1, CncCETOpenSquare=2, CncCETOpenRound=3, CncCETOpenButt=4 };
enum TemplateFormat 			{ TplUnknown, TplSvg, TplGcode, TplBinary, TplManual, TplTest };
enum CncDimensions 				{ CncDimension1D = 1, CncDimension2D = 2, CncDimension3D = 3 };
enum CncRefPositionMode 		{ CncRM_Unknown = 0, CncRM_Mode1 = 1, CncRM_Mode2 = 2, CncRM_Mode3 = 3, CncRM_Mode4 = 4, CncRM_Mode5 = 5, CncRM_Mode6 = 6 };

namespace cnc {
	
	typedef std::vector<int32_t> 						SetterValueList;
	typedef std::map<unsigned long, unsigned long> 		LineNumberTranslater;

	const char RAPID_SPEED_CHAR 		= 'R';
	const char WORK_SPEED_CHAR			= 'W';
	const char MAX_SPEED_CHAR 			= 'M';
	const char USER_DEFIND_SPEED_CHAR 	= 'U';
	const char getCncSpeedTypeAsCharacter(CncSpeed s);
	
	extern CncBasicLogStream  cex1;
	extern CncTraceLogStream  trc;
	extern CncMsgLogStream    msg;
	extern CncSerialSpyStream spy;

	bool dblCompare(double a, double b, double eps = std::numeric_limits<double>::epsilon());
	bool dblCompareNull(double a, double eps = std::numeric_limits<double>::epsilon());
	const wxString& dblFormat1(double d);
	const wxString& dblFormat2(double d1, double d2, const wxString& delimiter = _T(","));
	const wxString& dblFormat3(double d1, double d2, double d3, const wxString& delimiter = _T(","));
	
	const char* getTemplateFormatAsString(const TemplateFormat tf);
	
	void traceSetterValueList(std::ostream& s, const SetterValueList& values, int32_t factor = 1);
	void traceLineNumberTranslater(std::ostream& s, const LineNumberTranslater& lnt);

};

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

class MonitorBookSelection {
	public:
		enum VAL {
			CNC_PANEL					= 0,
			TEMPLATE_PANEL				= 1
		};
};

class TemplateBookSelection {
	public:
		enum VAL {
			SOURCE_PANEL				= 0,
			USER_AGENT_PANEL			= 1
		};
};

class OutboundSelection{
	public:
		enum VAL {
			SUMMARY_PANEL				= 0,
			MOTION_MONITOR_PANAL		= 1,
			SVG_OUTPUT_PANEL			= 2,
			SVG_SOURCE_PANEL			= 3
		};
};

class StatisticSelection{
	public:
		enum VAL {
			SUMMARY_PANEL				= 0,
			VECTIES_PANAL				= 1,
		};
};

class OutboundCfgSelection{
	public:
		enum VAL {
			SUMMARY_PANEL				= 0,
			CNC_SETTER_PANEL			= 1,
			CNC_CONFIG_PANEL			= 2,
			CNC_PIN_PANEL				= 3,
			CNC_MSG_HIST_PANEL			= 4
		};
};

class TestBookSelection {
	public:
		enum VAL {
			INTERVAL					= 0,
			DIMENSION					= 1,
			LIMIT						= 2,
			TOOL						= 3
		};
};

class LoggerSelection {
	public:
		enum VAL {
			CNC							= 0,
			GAMEPAD						= 1
		};
};

#endif