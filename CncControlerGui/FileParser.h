#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <wx/string.h>
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
		FileParser(const wxString& fn);
		virtual ~FileParser();
		
		virtual bool processDebug();
		virtual bool processRelease();
		
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
		
		void selectSourceControl(wxStyledTextCtrl* ctl, unsigned long pos);
		void setDebuggerControls(DebugControls& dc);
		
	protected:
		wxString fileName;
		FileParserRunInfo runInfo;

		DebugControls debugControls;
		wxStyledTextCtrl* inboundSourceControl;
		
		virtual bool process() = 0;
		
		virtual void evaluateDebugState(bool force = false) {}
		virtual void broadcastDebugState(bool state) {}
		virtual void broadcastDebugControls(DebugControls& dc) {}
		
		void selectSourceControl(unsigned long pos);
};

#endif
