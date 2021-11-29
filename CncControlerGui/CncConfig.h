#ifndef CNC_CONFIG_H
#define CNC_CONFIG_H

#include <map>
#include <iostream>
#include <wx/config.h>
#include <wx/event.h>
#include <wx/textentry.h>
#include <wx/valnum.h>
#include <wx/combobox.h>
#include <wx/propgrid/propgrid.h>
#include "DataControlModel.h"
#include "CncPosition.h"
#include "CncUnitCalculator.h"
#include "CncArduino.h"
#include "CncOSDConfigList.h"
#include "CncCommon.h"

////////////////////////////////////////////////////////////////////////////// 
typedef void (*PropertyEventFunc)(wxPropertyGridEvent& event);
typedef void (*PropertyCommandFunc)(wxCommandEvent& event);
struct PGFuncPtrStore {
	wxString 				name;
	PropertyEventFunc 		propertyChanged			= NULL;
	PropertyEventFunc		propertyChanging		= NULL;
	PropertyEventFunc		propertySelected		= NULL;
	PropertyCommandFunc		propertyCommandButton	= NULL;
};

typedef std::map<wxPGProperty*, PGFuncPtrStore>		ConfigPGEventMap;
typedef std::map<const wxString, wxPGProperty*> 	ConfigCategoryMap;
typedef std::map<const wxString, wxPGProperty*> 	ConfigPropertyMap;

wxDECLARE_EVENT(wxEVT_CONFIG_UPDATE_NOTIFICATION, wxCommandEvent);

//////////////////////////////////////////////////////////////////////////////
class MainFrame;
class CncContext;

typedef std::map<wxWindow*, wxWindow*> RegisteredWindows;

static const int TOOL_MAGAZINE_MIN_ID = -1;
static const int TOOL_MAGAZINE_MAX_ID = 999;

class CncConfig {
	
	private:
		std::stringstream loadTrace;
		std::stringstream saveTrace;
		std::stringstream obsoleteTrace;
	
	public:
		typedef CncUnitCalculatorBase::Unit Unit;
		
		struct ToolMagazineEntry {
		
			double diameter 	= 0.0;
			double length		= 0.0;
			double offset		= 0.0;
			wxString type 		= "PEN";
			wxString comment 	= "";
			
			const wxString& serialize(wxString& ret );
			bool deserialize(const wxString& input);
		};
		
		struct ToolMagazineParameter {
			bool useDefaultTool			= true;
			wxString defaultMappedTo	= "-1";
		};
		
		typedef std::map<int, ToolMagazineEntry> ToolMagazine;
		
		struct ContainerMemoryAllocation {
			private:
				unsigned int portion 	= 1024;
				size_t capacity			=    1;
				
			public:
				explicit ContainerMemoryAllocation(unsigned int f = 4) 
				: capacity(portion * f)
				{}
				
				explicit ContainerMemoryAllocation(const ContainerMemoryAllocation& cma) 
				: capacity(cma.getCapacity())
				{}
				
				void setup(unsigned int f = 4) 		{ *this = ContainerMemoryAllocation(f); }
				const size_t getCapacity() const	{ return capacity; }
		};
		
	private:
		bool 						changed;
		bool 						notificationActivated;
		CncOSDConfigList 			osdConfigList;
		CncUnit 					currentUnit;
		MainFrame* 					theApp;
		CncContext*					context;
		ToolMagazine 				toolMagazine;
		ToolMagazineParameter 		toolMagazineParameter;
		RegisteredWindows 			registeredWindows;
		ContainerMemoryAllocation	contMemAllocation;
		
		double						dispFactX, dispFactY, dispFactZ, dispFactH;
		double						calcFactX, calcFactY, calcFactZ, calcFactH;
		float						dispFactX3D, dispFactY3D, dispFactZ3D;
		
		unsigned int replyThreshold;
		
		int currentToolId;
		
		static const unsigned int maxDurations = 32;
		unsigned int durationCount;
		double durationSteps[maxDurations];
		double workpieceOffset;
		double currentZDepth;
		double maxZDistance;
		
		float renderResolutionMM;
		
		const wxString defaultConfigValue = "";
		static unsigned int globalPropertyCounter;
		#define NEXT_PROP_ID wxString::Format("%d", globalPropertyCounter++)
		
		static void registerPageRoot(wxPGProperty* prop, PGFuncPtrStore& fps);
		static void registerCategory(const wxString& name, wxPGProperty* prop);
		static void registerProperty(const wxString& name, wxPGProperty* prop);
		static wxPGProperty* getProperty(const wxString& name, bool silent=false);
		static wxPGProperty* getPageRoot(wxPGProperty* prop);

		static void collapse(wxPGProperty* prop);
		static void expand(wxPGProperty* prop);
		
		static void setupWorkingCfgPage(wxConfigBase& config);
		static void setupGeneralCfgPage(wxConfigBase& config);
		static void setupApplicationCfgPage(wxConfigBase& config);
		static void setupEnvironmentCfgPage(wxConfigBase& config);
		static void setupSvgCfgPage(wxConfigBase& config);
		static void setupGCodeCfgPage(wxConfigBase& config);
		
		static void pgChangedWorkingCfgPage(wxPropertyGridEvent& event);
		static void pgChangingWorkingCfgPage(wxPropertyGridEvent& event);
		static void pgSelectedWorkingCfgPage(wxPropertyGridEvent& event);
		static void pgButtonWorkingCfgPage(wxCommandEvent& event);
		
		static void pgChangedGeneralCfgPage(wxPropertyGridEvent& event);
		static void pgChangedApplicationCfgPage(wxPropertyGridEvent& event);
		static void pgChangedSvgCfgPage(wxPropertyGridEvent& event);
		static void pgChangedGCodeCfgPage(wxPropertyGridEvent& event);
		
		void updateCalculatedFactors();
		
		void broadcastConfigUpdateNotification();
		
		bool loadNonGuiConfig(const wxString& groupName, const wxString& entryName, const wxString& value);
		void saveNonGuiConfig(wxConfigBase& config);
		
		void calculateFactors();
		void calculateSpeedValues();
		bool setPropertyValueFromConfig(const wxString& groupName, const wxString& entryName, const wxString& value);
		void releaseChangedCallback(wxPGProperty* prop);
		void initZAxisValues();
		
		unsigned int calculateThreshold(double pitch, unsigned int steps);
		void calculateThresholds();
		
		void sc();
		void rc();
		
	public:
		
		CncConfig(MainFrame* app) ;
		~CncConfig();
		
		void init();
		
		MainFrame* getTheApp() { wxASSERT(theApp); return theApp; }
		CncContext* getContext() { wxASSERT(context); return context; }
		
		// global config pointer - don't use this directly
		static CncConfig* globalCncConfig;
		
		void updateLoadTrace(wxTextCtrl* lTrace, wxTextCtrl* oTrace);
		void updateSaveTrace(wxTextCtrl* sTrace);

		ToolMagazine& getToolMagazine() { return toolMagazine; }
		ToolMagazineParameter& getToolMagazineParameter() { return toolMagazineParameter; }
		
		// global config interface
		static bool available()					{ return globalCncConfig != NULL; }
		static CncConfig* getGlobalCncConfig()	{ wxASSERT(globalCncConfig); return globalCncConfig; }
		static void setupGlobalConfigurationGrid(wxPropertyGridManager* sg, wxConfigBase& config);
		
		// global shortcuts
		#define CNC_READY   ( CncConfig::globalCncConfig ? true : false )
		#define THE_CONFIG  ( CncConfig::getGlobalCncConfig() )
		#define THE_CONTEXT ( THE_CONFIG ? THE_CONFIG->getContext() : NULL )
		#define THE_APP     ( THE_CONFIG ? THE_CONFIG->getTheApp()  : NULL )
		#define THE_BOUNDS  ( THE_CONTEXT->boundarySpace )
		#define THE_TPL_CTX ( THE_CONTEXT->templateContext)
		
		// notifications
		void registerWindowForConfigNotification(wxWindow* wnd);
		void deactivateConfigNotification();
		void activateConfigNotification(bool notify=true);
		
		class NotificationDeactivator {
			private:
				bool notifyOnEnd;
				
			public:
				NotificationDeactivator(bool noe = true) : notifyOnEnd(noe)  
					{ THE_CONFIG->deactivateConfigNotification(); }
					
				~NotificationDeactivator()
					{ THE_CONFIG->activateConfigNotification(notifyOnEnd); }
		};
		
		size_t getConstReserveCapacity() const { return contMemAllocation.getCapacity(); }
		
		// curve lib utils
		const float getRenderResolutionMM() const { return renderResolutionMM; }
		void setRenderResolution(double res);
		void setRenderResolution(const wxString& sel);
		
		// user events
		void loadConfiguration(wxConfigBase& config);
		void saveConfiguration(wxConfigBase& config);
		void setupGridChanged(wxPropertyGridEvent& event);
		void setupGridChanging(wxPropertyGridEvent& event);
		void setupGridCommandButton(wxCommandEvent& event);
		void setupGridSelected(wxPropertyGridEvent& event);
		
		int getCurrentToolId() { return currentToolId; }
		void setCurrentToolId(int id) { currentToolId = id; }
		const wxString getCurrentToolParamAsString();
		const wxString getToolParamAsString(int id);
		const wxString getDefaultToolParamAsString();
		
		// modification flag
		const bool isModified() 		{ return changed; }
		void discardModifications() 	{ changed = false; }
		
		double convertX(CncUnit oldUnit, CncUnit newUnit, double value);
		double convertY(CncUnit oldUnit, CncUnit newUnit, double value);
		double convertZ(CncUnit oldUnit, CncUnit newUnit, double value);
		
		const CncDoublePosition& convertStepsToMetric(CncDoublePosition& ret, const CncLongPosition& pos);
		const double			 convertStepsToMetricX(int32_t val);
		const double			 convertStepsToMetricY(int32_t val);
		const double			 convertStepsToMetricZ(int32_t val);
		const double			 convertStepsToMetricH(int32_t val);
		
		const CncLongPosition&   convertMetricToSteps(CncLongPosition& ret,   const CncDoublePosition& pos);
		const int32_t			 convertMetricToStepsX(double val);
		const int32_t			 convertMetricToStepsY(double val);
		const int32_t			 convertMetricToStepsZ(double val);
		const int32_t			 convertMetricToStepsH(double val);
		
		int32_t connvert_MM_SEC_TO_STP_SEC(double speed, unsigned int steps, double pitch);
		int32_t connvert_MM_MIN_TO_STP_SEC(double speed, unsigned int steps, double pitch);
		double connvert_STP_SEC_TO_MM_SEC(int32_t speed, unsigned int steps, double pitch);
		double connvert_STP_SEC_TO_MM_MIN(int32_t speed, unsigned int steps, double pitch);
		
		int32_t connvert_MM_SEC_TO_STP_SEC_X(double speed) 	{ return connvert_MM_SEC_TO_STP_SEC(speed, getStepsX(), getPitchX()); }
		int32_t connvert_MM_SEC_TO_STP_SEC_Y(double speed) 	{ return connvert_MM_SEC_TO_STP_SEC(speed, getStepsY(), getPitchY()); }
		int32_t connvert_MM_SEC_TO_STP_SEC_Z(double speed) 	{ return connvert_MM_SEC_TO_STP_SEC(speed, getStepsZ(), getPitchZ()); }
		
		int32_t connvert_MM_MIN_TO_STP_SEC_X(double speed)	{ return connvert_MM_MIN_TO_STP_SEC(speed, getStepsX(), getPitchX()); }
		int32_t connvert_MM_MIN_TO_STP_SEC_Y(double speed)	{ return connvert_MM_MIN_TO_STP_SEC(speed, getStepsY(), getPitchY()); }
		int32_t connvert_MM_MIN_TO_STP_SEC_Z(double speed)	{ return connvert_MM_MIN_TO_STP_SEC(speed, getStepsZ(), getPitchZ()); }
		
		double connvert_STP_SEC_TO_MM_SEC_X(int32_t speed)	{ return connvert_STP_SEC_TO_MM_SEC(speed, getStepsX(), getPitchX()); }
		double connvert_STP_SEC_TO_MM_SEC_Y(int32_t speed)	{ return connvert_STP_SEC_TO_MM_SEC(speed, getStepsY(), getPitchY()); }
		double connvert_STP_SEC_TO_MM_SEC_Z(int32_t speed)	{ return connvert_STP_SEC_TO_MM_SEC(speed, getStepsZ(), getPitchZ()); }
		
		double connvert_STP_SEC_TO_MM_MIN_X(int32_t speed)	{ return connvert_STP_SEC_TO_MM_MIN(speed, getStepsX(), getPitchX()); }
		double connvert_STP_SEC_TO_MM_MIN_Y(int32_t speed)	{ return connvert_STP_SEC_TO_MM_MIN(speed, getStepsY(), getPitchY()); }
		double connvert_STP_SEC_TO_MM_MIN_Z(int32_t speed)	{ return connvert_STP_SEC_TO_MM_MIN(speed, getStepsZ(), getPitchZ()); }
		
		bool checkToolExists(int toolId=-1);
		int translateToolId(int toolId=-1);
		const double getToolDiameter(int toolId=-1);
		const double getToolLength(int toolId=-1);
		const double getToolOffset(int toolId=-1);
		const wxString& getToolType(wxString& ret, int toolId=-1);
		
		const unsigned int getReplyThresholdSteps() { return replyThreshold; }
		
		// configuration getters
		const CncUnit getDisplayUnit(void);
		const CncUnit getDefaultDisplayUnit(void);
		const char* getDefaultDisplayUnitAsStr();
		
		const char getRunConfirmationModeAsChar();

		const bool getSplashScreenFlag();
		const bool getAutoConnectFlag();
		const bool getAutoOpenLastFlag();
		const bool getAutoProcessFlag();
		const bool getAutoOpenOglObserverFlag();
		const bool getUseMainPreviewFlag();
		const bool getUseMonitorPeviewFlag();
		const bool getShowTestMenuFlag();
		const bool getSvg3DViewFlag();
		const bool getSvgConvertToRightHandFlag();
		const bool getSvgConsiderViewboxFlag();
		const bool getSvgUseColourScheme();
		const bool getAvoidDupSetterValuesFlag();
		const bool getRequestIdleRequestFlag();
		const bool getInterruptByPosOutOfRangeFlag();
		const bool getInverseCtrlDirectionXFlag();
		const bool getInverseCtrlDirectionYFlag();
		const bool getInverseCtrlDirectionZFlag();
		const bool getPreProcessorAnalyseFlag();
		const bool getPreProcessoSkipEmptyFlag();
		const bool getPreProcessorCombineMovesFlag();
		const bool getPreProcessorUseOperatingTrace();
		const bool getPreProcessorCntPathListEntries();
		const bool getPreProcessorCntMoveSequneces();
		const bool getSimulateMillingWithSoundFlag();
		const bool getCameraSupportFlag();
		const bool getSpindleSpeedSupportFlag();
		
		const double getMaxSpeedXYZ_MM_MIN();
		
		const double getAccelFunctParamA();
		const double getAccelFunctParamB();
		const double getAccelFunctParamC();
		const double getDeaccelFunctParamA();
		const double getDeaccelFunctParamB();
		const double getDeaccelFunctParamC();
		
		const double getMaxXYPitchToKeep();
		const double getMaxZPitchToKeep();
		
		const double getTouchblockZThickness();
		const double getTouchblockXCheckThickness();
		const double getTouchblockYCheckThickness();
		
		const double getCameraOffsetX();
		const double getCameraOffsetY();
		const double getCameraOffsetZ();
		
		const double getSpindleSpeedMin();
		const double getSpindleSpeedMax();
		const double getSpindleSpeedDefault();
		
		const unsigned int getCameraDeviceId();
		const unsigned int getArtificallyStepDelay();
		const unsigned int getStepsX();
		const unsigned int getStepsY();
		const unsigned int getStepsZ();
		const unsigned int getStepsH();
		const unsigned int getStepsXYZ();
		const unsigned int getHighPulsWidthX();
		const unsigned int getHighPulsWidthY();
		const unsigned int getHighPulsWidthZ();
		const unsigned int getHighPulsWidthH();
		const unsigned int getSpindleSpeedStepRange();
		const unsigned int getMaxDurations() 					{ return maxDurations; }
		const unsigned int getDurationCount() 					{ return durationCount; }
		
		const double getMaxDimension();
		const double getMaxDimensionX();
		const double getMaxDimensionY();
		const double getMaxDimensionZ();
		const double getMaxDimensionH();
		
		const int32_t getMaxDimensionSteps()					{ return getMaxDimension()  * getCalculationFactX(); }
		const int32_t getMaxDimensionStepsX()					{ return getMaxDimensionX() * getCalculationFactX(); }
		const int32_t getMaxDimensionStepsY()					{ return getMaxDimensionY() * getCalculationFactY(); }
		const int32_t getMaxDimensionStepsZ()					{ return getMaxDimensionZ() * getCalculationFactZ(); }
		const int32_t getMaxDimensionStepsH()					{ return getMaxDimensionH() * getCalculationFactH(); }
	
		const double getPitchX();
		const double getPitchY();
		const double getPitchZ();
		const double getPitchH();
		const double getMaxDurationThickness();
		const double getSurefaceOffset();
		const double getReplyThresholdMetric();
		const double getDefaultRapidSpeed_MM_MIN();
		const double getDefaultWorkSpeed_MM_MIN();
		const double getDisplayFactX(CncUnit cu=CncMetric) 		{ return ( cu == CncMetric ? dispFactX : 1.0 ); }
		const double getDisplayFactY(CncUnit cu=CncMetric)		{ return ( cu == CncMetric ? dispFactY : 1.0 ); }
		const double getDisplayFactZ(CncUnit cu=CncMetric) 		{ return ( cu == CncMetric ? dispFactZ : 1.0 ); }
		const double getDisplayFactH(CncUnit cu=CncMetric) 		{ return ( cu == CncMetric ? dispFactH : 1.0 ); }
		const double getCalculationFactX(CncUnit cu=CncMetric) 	{ return ( cu == CncMetric ? calcFactX : 1.0 ); }
		const double getCalculationFactY(CncUnit cu=CncMetric) 	{ return ( cu == CncMetric ? calcFactY : 1.0 ); }
		const double getCalculationFactZ(CncUnit cu=CncMetric) 	{ return ( cu == CncMetric ? calcFactZ : 1.0 ); }
		const double getCalculationFactH(CncUnit cu=CncMetric) 	{ return ( cu == CncMetric ? calcFactH : 1.0 ); }
		const double getFeedrateX()							 	{ return getDisplayFactX(); }
		const double getFeedrateY()								{ return getDisplayFactY(); }
		const double getFeedrateZ()								{ return getDisplayFactZ(); }
		const double getFeedrateH()								{ return getDisplayFactH(); }
		
		const double getDispFactX3D() 							{ return dispFactX3D; }
		const double getDispFactY3D() 							{ return dispFactY3D; }
		const double getDispFactZ3D()							{ return dispFactZ3D; }
		
		const double getWorkpieceOffset()						{ return workpieceOffset; }

// to remove
const double getCurrentZDepth()							{ return currentZDepth; }
const double getMaxZDistance()							{ return maxZDistance; }
const double getCurZDistance() 							{ return 0.0 /*getWorkpieceThickness()*/ + workpieceOffset; }

		const wxString& getFileBrowser(wxString& ret);
		const wxString& getSVGFileViewer(wxString& ret);
		const wxString& getBINFileViewer(wxString& ret);
		const wxString& getGCodeFileViewer(wxString& ret);
		const wxString& getXMLFileViewer(wxString& ret);
		const wxString& getBrowser(wxString& ret);
		const wxString& getEditorTool(wxString& ret);
		const wxString& getHexEditorTool(wxString& ret);
		const wxString& getVeuszPlotterTool(wxString& ret);
		const wxString& getPyCamTool(wxString& ret);
		const wxString& getDefaultSpeedModeXYZ(wxString& ret);
		const wxString& getDefaultPort(wxString& ret);
		const wxString& getDefaultTestTplDir(wxString& ret);
		const wxString& getDefaultTplDir(wxString& ret);
		const wxString& getDefaultTplFile(wxString& ret);
		const wxString& getRunConfirmationMode(wxString& ret);
		
		// configuration setters
		CncConfig& setDisplayUnit(const CncUnit unit);
		CncConfig& setMaxDimensionX(const double val);
		CncConfig& setMaxDimensionY(const double val);
		CncConfig& setMaxDimensionZ(const double val);
};

#endif
