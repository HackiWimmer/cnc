#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <wx/string.h>
#include <wx/variant.h>
#include "DataControlModel.h"
#include "FileParserRunInfo.h"

class wxStaticText;
class wxBitmapButton;
class wxStyledTextCtrl;
class wxDataViewListCtrl;

class wxPropertyGridManager;
class wxPropertyGridPage;
class wxPropertyCategory;
class wxPGProperty;

typedef std::vector<int> ToolIds;

class FileParser {

	public:
		/////////////////////////////////////////////////////////////////////
		struct DebugCtrl {

			unsigned long 			propCount					= 0;
			wxPropertyGridPage*		currentPage					= NULL;
			wxPGProperty*			currentMainCategory			= NULL;
			wxPGProperty*			currentNode					= NULL;
			
			void currentNodeAdd(const wxString& key, const wxVariant& value);
			void currentNodeAdd(DcmRow& row);
			void currentNodeAdd(DcmItemList& rows);
			
			void currentNodeAddToCategory(const wxString& catLabel, const wxString& key, const wxVariant& value);
			void currentNodeAddToCategory(const wxString& catLabel, DcmRow& row);
			void currentNodeAddToCategory(const wxString& catLabel, DcmItemList& rows);
			
			struct Config {
				///////////////////////////////////////////////////////////////////////////
				wxPGProperty* getProperty(const wxString& name);
				
				bool autoBreakpoint();
				bool shouldDebugPreprocessing();
				bool shouldDebugSpooling();
				bool shouldStopAfterPreprocessing();
				bool shouldStopAfterSpooling();
				
			} config;
			
		};
		
		/////////////////////////////////////////////////////////////////////
		struct UserAgentOutputControls {
			wxDataViewListCtrl* inboundPathList 	= NULL;
			wxDataViewListCtrl* useDirectiveList	= NULL;
			wxDataViewListCtrl* detailInfo			= NULL;
		};
		
	public:
		FileParser(const wxString& fn);
		virtual ~FileParser();
		
		bool isPause() { return runInfo.getPauseFlag(); }
		bool isProcessing() { return runInfo.isProcessing(); }
		
		virtual bool processDebug();
		virtual bool processRelease();
		
		bool togglePause();
		
		void debugNextBreakPoint();
		void debugNextStep();
		void debugStop();
		void debugFinish();
		
		virtual void setUserAgentControls(UserAgentOutputControls& oc) {}
		virtual void displayUserAgentDetailInfo(unsigned int pos) {}
		virtual void clearControls();
		virtual int getCurrentLineNumber() { return currentLineNumber; };
		
		void setInboundSourceControl(wxStyledTextCtrl* stc) { inboundSourceControl = stc; }
		
		void appendDebugValueBase(const char* key, wxVariant value);
		void appendDebugValuePath(const char* key, wxVariant value);
		void appendDebugValueDetail(const char* key, wxVariant value);
		
		void appendDebugValueBase(DcmItemList& rows);
		void appendDebugValuePath(DcmItemList& rows);
		void appendDebugValueDetail(DcmItemList& rows);
		
		void appendDebugValueBase(DcmRow& row);
		void appendDebugValuePath(DcmRow& row);
		void appendDebugValueDetail(DcmRow& row);
		
		bool isWaitingForUserEvents() { return waitingForUserEvents; }
		
		// configuration page handling
		const int staticPageOffset = 1;
		static wxPropertyGridManager* debuggerConfigurationPropertyGrid;
		static void installDebugConfigPage(wxPropertyGridManager* pgm);
		
		virtual void selectSourceControl(unsigned long pos);

	protected:
		wxString fileName;
		FileParserRunInfo runInfo;
		bool waitingForUserEvents;

		DebugCtrl debugControls;
		wxStyledTextCtrl* inboundSourceControl;
		
		virtual bool process();
		virtual bool preprocess() = 0;
		virtual bool spool() = 0;
		virtual bool postprocess() = 0;
		virtual void initNextClientId(long id) = 0;
		virtual void logMeasurementStart() = 0;
		virtual void logMeasurementEnd() = 0;
		
		virtual void initNextRunPhase(FileParserRunInfo::RunPhase p);
		
		virtual bool isInterrupted() { return false; }
		virtual void broadcastDebugState(bool state) {}
		
		bool evaluateProcessingState();
		bool evaluateDebugState(bool force = false);
		
		void setCurrentLineNumber(long ln);
		void incCurrentLineNumber() { setCurrentLineNumber(++currentLineNumber); }
		bool hasLineABreakpoint(long ln);
		bool checkBreakpoint();
		
		void registerNextDebugNode(const wxString& nodeName);
		
		bool setNextToolID(unsigned int id);
		virtual bool shouldAToolChangeProcessed() { return false; }
		
		friend class SVGPathHandlerCnc;
		friend class GCodePathHandlerCnc;
		friend class GCodePathHandlerGL;
		
	private:
		
		long currentLineNumber;
		ToolIds	toolIds;
		
		#define SHOULD_DEBUG_HERE										\
			if ( runInfo.getCurrentDebugState() == false )				\
				return;
				
		void displayToolId(const wxString& id);
		void displayToolId(int id);
		
		bool isAToolAvailable();
		int getCurrentToolId();
};

#endif
