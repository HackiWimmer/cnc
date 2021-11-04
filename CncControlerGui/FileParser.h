#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <wx/string.h>
#include <wx/variant.h>
#include "DataControlModel.h"
#include "CncProcessingInfo.h"
#include "CncContext.h"

class wxStaticText;
class wxBitmapButton;
class CncSourceEditor;
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
		
	public:
		FileParser(const wxString& fn);
		virtual ~FileParser();
		
		virtual void deligateTrigger(const Trigger::BeginRun& tr)				= 0;
		virtual void deligateTrigger(const Trigger::EndRun& tr)					= 0;
		virtual void changePathListRunnerInterface(const wxString& portName)	= 0;
		virtual void initCurrentPos(const CncDoublePosition& pos)				= 0;
		
		virtual bool processDebug();
		virtual bool processRelease();
		
		void debugNextBreakPoint();
		void debugNextStep();
		void debugStop();
		void debugFinish();
		
		virtual void enableUserAgentControls(bool state) {}
		virtual void displayUserAgentDetailInfo(unsigned int pos) {}
		
		virtual void clearControls();
		virtual int getCurrentLineNumber() { return currentLineNumber; };
		
		void setInboundSourceControl(CncSourceEditor* stc) { inboundSourceControl = stc; }
		
		void appendDebugValueBase(const char* key, wxVariant value);
		void appendDebugValuePath(const char* key, wxVariant value);
		void appendDebugValueDetail(const char* key, wxVariant value);
		
		void appendDebugValueBase(DcmItemList& rows);
		void appendDebugValuePath(DcmItemList& rows);
		void appendDebugValueDetail(DcmItemList& rows);
		
		void appendDebugValueBase(DcmRow& row);
		void appendDebugValuePath(DcmRow& row);
		void appendDebugValueDetail(DcmRow& row);
		
		// configuration page handling
		const int staticPageOffset = 1;
		static wxPropertyGridManager* debuggerConfigurationPropertyGrid;
		static void installDebugConfigPage(wxPropertyGridManager* pgm);
		
		virtual void selectSourceControl(unsigned long pos);

	protected:
		
		wxString			fileName;

		DebugCtrl			debugControls;
		CncSourceEditor*	inboundSourceControl;
		
		virtual bool process();
		virtual bool preprocess() = 0;
		virtual bool spool() = 0;
		virtual bool postprocess() = 0;
		virtual void initNextClientId(long id) = 0;
		virtual void logMeasurementStart() = 0;
		virtual void logMeasurementEnd() = 0;
		
		virtual void initNextRunPhase(CncProcessingInfo::RunPhase p);
		virtual void broadcastDebugState(bool state) {}
		
		bool evaluateProcessingState();
		bool evaluateDebugState(bool force = false);
		
		void setCurrentLineNumber(long ln);
		void incCurrentLineNumber() { setCurrentLineNumber(++currentLineNumber); }
		bool hasLineABreakpoint(long ln);
		
		void registerNextDebugNode(const wxString& nodeName);
		
		bool setNextToolID(unsigned int id);
		virtual bool shouldAToolChangeProcessed() { return false; }
		
		friend class CncPathListRunner;
		friend class PathHandlerBase;
		
	private:
		
		long currentLineNumber;
		ToolIds	toolIds;
		
		void displayToolId(const wxString& id);
		void displayToolId(int id);
		
		bool isAToolAvailable();
		int getCurrentToolId();
};

#endif
