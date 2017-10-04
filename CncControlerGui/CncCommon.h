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
static const float PI = 3.14159265;
static const int MAX_PARAMETER_VALUES = 10;

enum CncUnit 					{ CncSteps, CncMetric };
enum CncDirection 				{ CncUndefDir, CncClockwise, CncAnticlockwise };
enum CncSpeed 					{ CncSpeedWork, CncSpeedRapid };
enum CncPortType 				{ CncPORT, CncEMU_NULL, CncEMU_SVG };
enum CncRenderMode 				{ CncRenderAtPC, CncRenderAtController };
enum CncToolCorretionType 		{ CncCT_None=0, CncCT_Inner=1, CncCT_Outer=2, CncCT_Center=3 };
enum CncClipperCornerType 		{ CncCCT_Round=0, CncCCT_Square=1, CncCCT_Miter=2 };
enum CncClipperEndType			{ CncCET_ClosedPolygon=0, CncCETClosedLine=1, CncCETOpenSquare=2, CncCETOpenRound=3, CncCETOpenButt=4};
enum TemplateFormat 			{ TplUnknown, TplSvg, TplGcode, TplManual, TplTest };
enum CncDimensions 				{ CncDimension1D = 1, CncDimension2D = 2, CncDimension3D = 3};


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
static const unsigned int Outbound3DPage 			= 1;
static const unsigned int OutboundSvgPage 			= 2;
static const unsigned int OutboundSvgSource			= 3;

static const unsigned int ConfigSummaryPage 		= 0;
static const unsigned int CNCSetterPage 			= 1;
static const unsigned int CNCPCConfig	 			= 2;
static const unsigned int CNCControllerConfigPage 	= 3;
static const unsigned int CNCControllerPinsPage		= 4;
static const unsigned int CNCControllerMsgHistPage	= 5;
static const unsigned int CNCControllerErrorPage 	= 6;

#endif