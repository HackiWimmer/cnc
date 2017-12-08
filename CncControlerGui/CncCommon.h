#ifndef CNC_COMMON_INCLUDES
#define CNC_COMMON_INCLUDES

#include <cmath>
#include <cfloat>
#include <limits>

#include "CncStreamBuffers.h"
#include "StdStreamRedirector.h"

// make essiential global variables available
namespace cnc {
	extern CncBasicLogStream cex1;
	extern CncTraceLogStream trc;
	extern CncMsgLogStream msg;
	extern CncSerialSpyStream spy;
	
	int gettimeofday(struct timeval *tv, struct timezone *tz);
	
	bool dblCompare(double a, double b, double eps = std::numeric_limits<double>::epsilon());
	bool dblCompareNull(double a, double eps = std::numeric_limits<double>::epsilon());
	const wxString& dblFormat1(double d);
	const wxString& dblFormat2(double d1, double d2, const wxString& delimiter = _T(","));
	const wxString& dblFormat3(double d1, double d2, double d3, const wxString& delimiter = _T(","));
}

// common ostream operators
std::ostream& operator<<(std::ostream& os, const wxPoint& p);
std::ostream& operator<<(std::ostream& os, const wxRealPoint& p);

// make std globally available
using namespace std;

// define globally const values
static const float PI 								= 3.14159265;
static const int MAX_PARAMETER_VALUES 				= 10;
static const int UNDEFINED_LINE_NUMBER 				= -1;

enum CncUnit 					{ CncSteps, CncMetric };
enum CncDirection 				{ CncUndefDir, CncClockwise, CncAnticlockwise };
enum CncSpeed 					{ CncSpeedWork, CncSpeedRapid };
enum CncPortType 				{ CncPORT, CncEMU_NULL, CncEMU_SVG };
enum CncToolCorretionType 		{ CncCT_None=0, CncCT_Inner=1, CncCT_Outer=2, CncCT_Center=3 };
enum CncClipperCornerType 		{ CncCCT_Round=0, CncCCT_Square=1, CncCCT_Miter=2 };
enum CncClipperEndType			{ CncCET_ClosedPolygon=0, CncCETClosedLine=1, CncCETOpenSquare=2, CncCETOpenRound=3, CncCETOpenButt=4};
enum TemplateFormat 			{ TplUnknown, TplSvg, TplGcode, TplManual, TplTest };
enum CncDimensions 				{ CncDimension1D = 1, CncDimension2D = 2, CncDimension3D = 3};

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
			CNC_MSG_HIST_PANEL			= 4,
			CNC_ERROR_PANEL				= 5
		};
};

#endif