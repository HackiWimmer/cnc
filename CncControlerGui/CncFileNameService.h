#ifndef FILENAMESERVICE_H
#define FILENAMESERVICE_H

#include <ostream>
#include "CncCommon.h"
#include "wx/string.h"

class CncFileNameService {
	
	private:
		static wxString _ret;
		static wxString _executablePath;
		static wxString _homeDirectory;
		static wxString _tempDirectory;
		
		static void deleteFile(wxString fn);
	
	public:
		static void init();
		static void trace(std::ostream& os);
		static wxString& getExecutableDir() 					{ return _executablePath; }
		static wxString& getHomeDir() 							{ return _homeDirectory; }
		static wxString& getTempDir() 							{ return _tempDirectory; }
		
		static const char* getTempFileName(TemplateFormat f);
		static const char* getCncTemplatePreviewFileName(TemplateFormat f);
	
		static const char* getCncOutboundTraceFileName() 		{ _ret = _tempDirectory + "CncOutboundTrace.xml"; 	return _ret; }
		static const char* getCncOutboundSvgFileName() 			{ _ret = _tempDirectory + "CncOutboundEmu.svg"; 	return _ret; }
		static const char* getCncOutboundTempFileName()			{ _ret = _tempDirectory + "CncOutboundTempEmu.xml"; return _ret; }
		static const char* getCncDrawPaneTraceFileName() 		{ _ret = _tempDirectory + "CncDrawPaneTrace.txt"; 	return _ret; }
		
		static const char* getBlankHtmlPageFileName()			{ _ret = _tempDirectory + "CncBlank.html"; 			return _ret; }
		static const char* getErrorHtmlPageFileName()			{ _ret = _tempDirectory + "CncError.html"; 			return _ret; }
		
		static void deleteCncOutboundTraceFileName() 			{ deleteFile(wxString(getCncOutboundTraceFileName())); }
		static void deleteCncOutboundSvgFileName() 				{ deleteFile(wxString(getCncOutboundSvgFileName())); }
		static void deleteCncOutboundTempFileName() 			{ deleteFile(wxString(getCncOutboundTempFileName())); }
		static void deleteCncDrawPaneTraceFileName() 			{ deleteFile(wxString(getCncDrawPaneTraceFileName())); }
		
};

#endif