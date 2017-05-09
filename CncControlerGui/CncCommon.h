#ifndef CNC_COMMON_INCLUDES
#define CNC_COMMON_INCLUDES

#include <cmath>
#include <cfloat>
#include <limits>

#include "CncStreamBuffers.h"

// make essiential global variables available
namespace cnc {
	extern CncBasicLogStream cex1;
	extern CncTraceLogStream trc;
	extern CncMsgLogStream msg;
	extern CncSerialSpyStream spy;
	
	namespace pg {
		extern CncPGTLogStream trc;
	}
	
	
	bool dblCompare( double a, double b, double eps = std::numeric_limits<double>::epsilon());
	bool dblCompareNull( double a, double eps = std::numeric_limits<double>::epsilon());
}

// make std globally available
using namespace std;

// define globally const values
static const float PI = 3.14159265;
static const int MAX_PARAMETER_VALUES = 10;

enum CncUnit {CncSteps, CncMetric};
enum CncDirection {CncUndefDir, CncClockwise, CncAnticlockwise};
enum CncSpeed {CncSpeedWork, CncSpeedFly};
enum CncPortType {CncPORT, CncEMU_NULL, CncEMU_SVG};
enum CncRenderMode {CncRenderAtPC, CncRenderAtController };
enum CncToolCorretionType {CncCT_None, CncCT_Center, CncCT_Inner, CncCT_Outer};
enum TemplateFormat {TplUnknown, TplSvg, TplText, TplGcode, TplManual, TplTest};
enum CncDimensions {CncDimension1D = 1, CncDimension2D = 2, CncDimension3D = 3};

static const int UNDEFINED_LINE_NUMBER 				= -1;

static const unsigned int MainReferencePage 		= 0;
static const unsigned int MainSetupPage 			= 1;
static const unsigned int MainTestPage 				= 2; 
static const unsigned int MainManuallyPage 			= 3;
static const unsigned int MainTemplatePage 			= 4;

static const unsigned int TemplateContentPage 		= 0; 
static const unsigned int TemplateUserAgentPage 	= 1;
static const unsigned int TemplateDebuggerPage 		= 2;
static const unsigned int TemplatePreviewPage 		= 3;

static const unsigned int OutboundCNCValuesPage 	= 0;
static const unsigned int OutboundMotionMonitorPage = 1;
static const unsigned int Outbound3DPage 			= 2;
static const unsigned int OutboundSvgPage 			= 3;
static const unsigned int OutboundSvgSource			= 4;
static const unsigned int OutboundSvgTrace			= 5;

static const unsigned int ConfigSummaryPage 		= 0;
static const unsigned int CNCSetterPage 			= 1;
static const unsigned int CNCPCConfig	 			= 2;
static const unsigned int CNCControllerConfigPage 	= 3;
static const unsigned int CNCControllerPinsPage		= 4;
static const unsigned int CNCControllerMsgHistPage	= 5;
static const unsigned int CNCControllerErrorPage 	= 6;

#endif