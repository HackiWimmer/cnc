#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <wx/string.h>
#include "CncControl.h"
#include "FileParserRunInfo.h"

class wxStyledTextCtrl;
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
			wxMenuItem*				debugUserAgent			= NULL;
			wxMenuItem*				debugSpooling			= NULL;
		};
		
		/////////////////////////////////////////////////////////////////////
		struct UserAgentOutputControls {
			wxDataViewListCtrl* inboundPathList 	= NULL;
			wxDataViewListCtrl* useDirectiveList	= NULL;
			wxDataViewListCtrl* detailInfo			= NULL;
		};
		
	public:
		FileParser(const wxString& fn, CncControl* cnc);
		virtual ~FileParser();
		
		virtual bool processDebug();
		virtual bool processRelease();
		
		virtual void displayCollectedTrace(bool blank=false) = 0;

		virtual void setUserAgentControls(UserAgentOutputControls& oc) {}
		virtual void displayUserAgentDetailInfo(unsigned int pos) {}
		virtual void clearControls() {}
		
		bool pause();
		bool isPause() { return runInfo.getPauseFlag(); }
		bool isProcessing() { return runInfo.isProcessing(); }
		
		void debugNextPath();
		void debugNextStep();
		void debugStop();
		void debugFinish();
		
		void setInboundSourceControl(wxStyledTextCtrl* stc) { inboundSourceControl = stc; }
		void setInboundTraceControl(wxWebView* wv)          { inboundTraceControl = wv; }
		
		void selectSourceControl(wxStyledTextCtrl* ctl, unsigned long pos);
		void setDebuggerControls(DebugControls& dc);
		
	protected:
		CncControl* cncControl;
		wxString fileName;
		FileParserRunInfo runInfo;

		DebugControls debugControls;
		wxWebView* inboundTraceControl;
		wxStyledTextCtrl* inboundSourceControl;
		
		virtual bool process() = 0;
		
		virtual void evaluateDebugState(bool force = false) = 0;
		virtual void broadcastDebugState(bool state) = 0;
		virtual void broadcastDebugControls(DebugControls& dc) = 0;
		
		void selectSourceControl(unsigned long pos);
};

#endif
