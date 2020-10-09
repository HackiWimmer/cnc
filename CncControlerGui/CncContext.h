#ifndef CNC_CONTEXT_H
#define CNC_CONTEXT_H

#include <map>
#include <vector>
#include <iostream>
#include <wx/string.h>
#include <wx/font.h>
#include "OSD/CncTimeFunctions.h"

typedef std::map<wxString, wxString> VersionInfoMap;
typedef std::vector<wxString> CommandLineParameterMap;

//#ifdef __WXMSW__
//#ifdef __WXGTK__

struct CncContext {
	
	public:
		enum OSType {UNDEF, WXMSW, WXGTK};

	private:
		
		OSType os						= OSType::UNDEF;
		bool probeMode					= true;
		bool interactiveMoveMode				= false;
		bool speedMonitor				= false;
		bool onlineUpdateCoordinates	= true;
		bool onlineUpdateDrawPane		= true;
		bool allowEventHandling			= true;
		bool hardwareFlag				= false;
		
		int updateInterval				= 100;

	public:
		
		wxFont outboundListBookFont		= wxFont(7, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
		
		struct HardwareOriginOffset {
			bool valid = false;
			
			// stores the signed ofset origin to Xmin, Ymin and Zmax
			int32_t dx = 0;
			int32_t dy = 0;
			int32_t dz = 0;
			
			HardwareOriginOffset() 
			: dx(0), dy(0), dz(0)
			{}
			
			void reset() {
				*this = HardwareOriginOffset();
			}
		};
		
		struct SecureModeInfo {
			bool useIt						= false;
			bool isActive					= false;
			bool isActivatedByStartup		= false;
			bool isDeactivatedByUser		= false;
		};
		
		struct TsTplProcessing {

			private:
				CncNanoTimestamp tsTotalStart	= CncTimeFunctions::getNanoTimestamp();
				CncNanoTimestamp tsTotalEnd	 	= CncTimeFunctions::getNanoTimestamp();

				CncNanoTimestamp tsPreStart	 	= CncTimeFunctions::getNanoTimestamp();
				CncNanoTimestamp tsPreEnd		= CncTimeFunctions::getNanoTimestamp();

				CncNanoTimestamp tsPostStart	= CncTimeFunctions::getNanoTimestamp();
				CncNanoTimestamp tsPostEnd	 	= CncTimeFunctions::getNanoTimestamp();

				CncNanoTimestamp tsSerialStart 	= CncTimeFunctions::getNanoTimestamp();
				CncNanoTimestamp tsSerialEnd	= CncTimeFunctions::getNanoTimestamp();

			public:
				// -------------------------------------------------------
				void logTotalTimeStart() 	{ tsTotalStart	= CncTimeFunctions::getNanoTimestamp(); }
				void logTotalTimeEnd() 		{ tsTotalEnd	= CncTimeFunctions::getNanoTimestamp(); }

				void logPreTimeStart() 		{ tsPreStart	= CncTimeFunctions::getNanoTimestamp(); }
				void logPreTimeEnd() 		{ tsPreEnd		= CncTimeFunctions::getNanoTimestamp(); }

				void logPostTimeStart()		{ tsPostStart	= CncTimeFunctions::getNanoTimestamp(); }
				void logPostTimeEnd() 		{ tsPostEnd		= CncTimeFunctions::getNanoTimestamp(); }

				void logSerialTimeStart() 	{ tsSerialStart	= CncTimeFunctions::getNanoTimestamp(); }
				void logSerialTimeEnd() 	{ tsSerialEnd	= CncTimeFunctions::getNanoTimestamp(); }

				// -------------------------------------------------------
				const wxString& getTimeConsumedFormated(wxString& ret, const CncNanoTimespan duartionNanos) const {
					int n = (int) ( duartionNanos % std::nano::den );
					int s = (int) ( duartionNanos / std::nano::den) % 60 ;
					int m = (int) ((duartionNanos / (std::nano::den * 60)) % 60);
					int h = (int) ((duartionNanos / (std::nano::den * 60 * 60)) % 24);

					ret.assign(wxString::Format("%02d:%02d:%02d.%06d", h, m, s, n / 1000));
					return ret;
				}

				// -------------------------------------------------------
				const wxString& getTimeConsumedFormated(wxString& ret, CncNanoTimestamp tsStart, CncNanoTimestamp tsEnd) const {
					const CncNanoTimespan duartionNanos = CncTimeFunctions::getTimeSpan(tsEnd, tsStart);
					return getTimeConsumedFormated(ret, duartionNanos);
				}

				long getTotalDurationMillis() 	const { return getTotalDurationMicros()  / 1000; }
				long getPreDurationMillis()   	const { return getPreDurationMicros()    / 1000; }
				long getSerialDurationMillis()	const { return getSerialDurationMicros() / 1000; }
				long getPostDurationMillis()  	const { return getPostDurationMicros()   / 1000; }

				long getTotalDurationMicros() 	const { return getTotalDurationNanos()   / 1000; }
				long getPreDurationMicros()   	const { return getPreDurationNanos()     / 1000; }
				long getSerialDurationMicros()	const { return getSerialDurationNanos()  / 1000; }
				long getPostDurationMicros()  	const { return getPostDurationNanos()    / 1000; }

				CncNanoTimestamp getTotalDurationNanos() 	const { return CncTimeFunctions::getTimeSpan(tsTotalEnd,  tsTotalStart);  }
				CncNanoTimestamp getPreDurationNanos()   	const { return CncTimeFunctions::getTimeSpan(tsPreEnd,    tsPreStart);    }
				CncNanoTimestamp getSerialDurationNanos()	const { return CncTimeFunctions::getTimeSpan(tsSerialEnd, tsSerialStart); }
				CncNanoTimestamp getPostDurationNanos()  	const { return CncTimeFunctions::getTimeSpan(tsPostEnd,   tsPostStart);   }

				const wxString& getTotalTimeConsumedFormated(wxString& ret)  const { return getTimeConsumedFormated(ret, tsTotalStart,  tsTotalEnd);  }
				const wxString& getPreTimeConsumedFormated(wxString& ret)    const { return getTimeConsumedFormated(ret, tsPreStart,    tsPreEnd);    }
				const wxString& getSerialTimeConsumedFormated(wxString& ret) const { return getTimeConsumedFormated(ret, tsSerialStart, tsSerialEnd); }
				const wxString& getPostTimeConsumedFormated(wxString& ret)   const { return getTimeConsumedFormated(ret, tsPostStart,   tsPostEnd);   }

				friend std::ostream &operator<< (std::ostream& ostr, const TsTplProcessing& ts) {
					wxString value;

					const CncNanoTimestamp rest = ts.getTotalDurationNanos()
												- ts.getPreDurationNanos()
												- ts.getSerialDurationNanos()
												- ts.getPostDurationNanos();

					ostr 	<< "Time consumed:"												<< std::endl
							<< " * Total  : " << ts.getTotalTimeConsumedFormated(value)		<< std::endl
							<< " * Pre    : " << ts.getPreTimeConsumedFormated(value) 		<< std::endl
							<< " * Serial : " << ts.getSerialTimeConsumedFormated(value)	<< std::endl
							<< " * Post   : " << ts.getPostTimeConsumedFormated(value) 		<< std::endl
							<< " * Rest   : " << ts.getTimeConsumedFormated(value, rest)	<< std::endl;

					return ostr;
				}

		};


		CncContext();
		~CncContext();
		
		TsTplProcessing 			timestamps;
		SecureModeInfo				secureModeInfo;
		VersionInfoMap				versionInfoMap;
		HardwareOriginOffset		hardwareOriginOffset;

		
		bool isWinOS() { return os == WXMSW; }
		bool isGtkOS() { return os == WXGTK; }
		OSType getOSType() { return os; }
		const char* getOSTypeAsString();

		const std::ostream& traceVersionInfo(std::ostream& os) const;
		const std::ostream& traceCommandLineParameter(std::ostream& os) const;
		const wxString& traceCommandLineParameter(wxString& s) const;

		void setProbeMode(bool state); 
		bool isProbeMode() 							{ return probeMode; }
		
		void setInteractiveMoveingMode(bool state) 	{ interactiveMoveMode = state; }
		bool canInteractiveMoveing()				{ return interactiveMoveMode;  }
		
		void setSpeedMonitoring(bool state)			{ speedMonitor = state; } 
		bool canSpeedMonitoring() 					{ return speedMonitor; }
		
		void setHardwareFlag(bool state)			{ hardwareFlag = state; } 
		bool hasHardware()		 					{ return hardwareFlag; }
		
		bool isOnlineUpdateCoordinates() 						{ return onlineUpdateCoordinates; }
		bool isOnlineUpdateDrawPane() 							{ return onlineUpdateDrawPane; }
		bool isAllowEventHandling() 							{ return allowEventHandling; }
		int getUpdateInterval() 								{ return updateInterval; }
		
		CncContext& setOnlineUpdateCoordinates(bool b) 			{ onlineUpdateCoordinates=b; return *this; }
		CncContext& setOnlineUpdateDrawPane(bool b) 			{ onlineUpdateDrawPane=b; return *this; }
		CncContext& setAllowEventHandling(bool b) 				{ allowEventHandling=b; return *this; }
		CncContext& setUpdateInterval(int i) 					{ updateInterval=i; return *this; }

};

#endif
