#ifndef CNC_CONTEXT_H
#define CNC_CONTEXT_H

#include <map>
#include <vector>
#include <iostream>
#include <wx/string.h>
#include <wx/font.h>
#include "ArduinoEnvWrapper.h"
#include "OSD/CncTimeFunctions.h"
#include "CncTsTplProcessing.h"
#include "CncPositionStorage.h"
#include "CncPosition.h"
#include "CncAverage.h"

// -----------------------------------------------------------
typedef std::map<wxString, wxString>	VersionInfoMap;
typedef std::vector<wxString>			CommandLineParameterMap;

// -----------------------------------------------------------
class CncProcessingInfo;
class CncBoundarySpace;
class CncAnchorMap;
class CncTemplateContext;
class CncGamepadFilterInstance;

// -----------------------------------------------------------
struct CncContext {
	
	public:
		enum OSType {UNDEF, WXMSW, WXGTK};

	private:
		
		OSType	os							= OSType::UNDEF;
		bool	probeMode					= true;
		bool	interactiveMoveMode			= false;
		bool	speedMonitor				= false;
		bool	onlineUpdateCoordinates		= true;
		bool	onlineUpdateDrawPane		= true;
		bool	allowEventHandling			= true;
		bool	hardwareFlag				= false;
		bool	requestIdleState			= false;
		bool	cncTransactionState			= false;
		
		int		updateInterval				= 100;
		
		double	currentToolDiameter			= 0.0;
		
	public:
	
		wxFont outboundListBookFont		= wxFont(7, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
		
		// ----------------------------------------------------------------
		struct SecureModeInfo 
		{
			bool useIt						= false;
			bool isActive					= false;
			bool isActivatedByStartup		= false;
			bool isDeactivatedByUser		= false;
		};
		
		CncContext();
		~CncContext();
		
		TsTplProcessing 			timestamps;
		SecureModeInfo				secureModeInfo;
		VersionInfoMap				versionInfoMap;
		CncProcessingInfo*			processingInfo;
		CncBoundarySpace*			boundarySpace;
		CncAnchorMap*				anchorMap;
		CncTemplateContext*			templateContext;
		CncGamepadFilterInstance*	gamepadFilterInstance;
		
		const std::ostream& traceVersionInfo(std::ostream& os) 			const;
		const std::ostream& traceCommandLineParameter(std::ostream& os)	const;
		const wxString& traceCommandLineParameter(wxString& s)			const;

		bool isWinOS()											const	{ return os == WXMSW; }
		bool isGtkOS()											const	{ return os == WXGTK; }
		OSType getOSType()										const	{ return os; }
		const char* getOSTypeAsString();
		
		void registerCncTransaction()									{ cncTransactionState = true; }
		void unregisterCncTransaction()									{ cncTransactionState = false; }
		bool isCncTransactionActive()							const	{ return cncTransactionState; }
		
		void setCurrentToolDiameter(double d )							{ currentToolDiameter = fabs(d); }
		double getCurrentToolDiameter()							const	{ return currentToolDiameter; }
		
		void setProbeMode(bool state); 
		bool isProbeMode() 										const	{ return probeMode; }
		
		bool isPause()											const;
		bool isProcessing()										const;
		bool togglePause();
		void initPreparationPhase();
		void resetProcessing();
		
		void setInteractiveMoveingMode(bool state)						{ interactiveMoveMode = state; }
		bool canInteractiveMoving()								const	{ return interactiveMoveMode;  }
		
		void setSpeedMonitoring(bool state)								{ speedMonitor = state; } 
		bool canSpeedMonitoring() 								const	{ return speedMonitor; }
		
		void setHardwareFlag(bool state)								{ hardwareFlag = state; } 
		bool hasHardware()		 								const	{ return hardwareFlag; }
		
		bool isOnlineUpdateCoordinates() 						const	{ return onlineUpdateCoordinates; }
		bool isOnlineUpdateDrawPane() 							const	{ return onlineUpdateDrawPane; }
		bool isAllowEventHandling() 							const	{ return allowEventHandling; }
		int getUpdateInterval() 								const	{ return updateInterval; }
		
		CncContext& setOnlineUpdateCoordinates(bool b) 					{ onlineUpdateCoordinates=b; return *this; }
		CncContext& setOnlineUpdateDrawPane(bool b) 					{ onlineUpdateDrawPane=b; return *this; }
		CncContext& setAllowEventHandling(bool b) 						{ allowEventHandling=b; return *this; }
		CncContext& setUpdateInterval(int i) 							{ updateInterval=i; return *this; }
		
		void addParsingSynopisSeparator	(const wxString& m) const;
		void addParsingSynopisInfo		(const wxString& m) const;
		void addParsingSynopisWarning	(const wxString& m) const;
		void addParsingSynopisDebug		(const wxString& m) const;
		void addParsingSynopisError		(const wxString& m) const;
		
		bool parsingSynopsisHasInfoEntries()    const;
		bool parsingSynopsisHasNonInfoEntries() const;
		bool parsingSynopsisHasDebugEntries()   const;
		bool parsingSynopsisHasWarningEntries() const;
		bool parsingSynopsisHasErrorEntries()   const;
};

#endif
