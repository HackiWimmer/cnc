#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <wx/string.h>
#include "DataControlModel.h"
#include "FileParserRunInfo.h"

class wxStyledTextCtrl;
class wxStaticText;
class wxDataViewListCtrl;
class wxWebView;

class FileParser {

	public:
		/////////////////////////////////////////////////////////////////////
		struct DebugControls {
			wxStaticText*			debugPhase 				= NULL;
			wxDataViewListCtrl* 	debuggerControlBase 	= NULL;
			wxDataViewListCtrl* 	debuggerControlPath 	= NULL;
			wxDataViewListCtrl* 	debuggerControlDetail 	= NULL;
			wxMenuItem*				debugPreprocessing 		= NULL;
			wxMenuItem*				debugSpooling			= NULL;
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
		virtual void clearControls() { clearDebugControlBase(); }
		
		int getCurrentLineNumber() { return currentLineNumber; };
		void setInboundSourceControl(wxStyledTextCtrl* stc) { inboundSourceControl = stc; }
		
		void setDebuggerControls(DebugControls& dc);
		
	protected:
		wxString fileName;
		FileParserRunInfo runInfo;

		DebugControls debugControls;
		wxStyledTextCtrl* inboundSourceControl;
		
		virtual bool process();
		virtual bool preprocess() = 0;
		virtual bool spool() = 0;
		virtual void initNextRunPhase(FileParserRunInfo::RunPhase p);
		
		virtual bool isInterrupted() { return false; }
		virtual void broadcastDebugState(bool state) {}
		virtual void broadcastDebugControls(DebugControls& dc) {}
		
		virtual void selectSourceControl(unsigned long pos);
		
		bool evaluateProcessingState();
		bool evaluateDebugState(bool force = false);
		
		void setCurrentLineNumber(long ln);
		void incCurrentLineNumber() { setCurrentLineNumber(++currentLineNumber); }
		bool hasLineABreakpoint(long ln);
		bool checkBreakpoint();
		
		void clearDebugControlBase();
		void clearDebugControlPath();
		void clearDebugControlDetail();
		
		void appendDebugValue(wxDataViewListCtrl* ctl, const char* key, wxVariant value);
		void appendDebugValue(wxDataViewListCtrl* ctl, DcmItemList& rows);
		void appendDebugValueDetail(const char* key, wxVariant value);
		
		void appendDebugValuePath(const char* key, wxVariant value);
		void appendDebugValueBase(const char* key, wxVariant value);
		
		void appendDebugValueBase(DcmItemList& rows);
		void appendDebugValuePath(DcmItemList& rows);
		void appendDebugValueDetail(DcmItemList& rows);
		
		friend class SVGPathHandlerCnc;
		friend class GCodePathHandlerCnc;
		friend class GCodePathHandlerGL;
		
	private:
		long currentLineNumber;
		bool checkIfCurrentRunPhaseShouldBeDebugged();
};

#endif
