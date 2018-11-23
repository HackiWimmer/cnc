#ifndef FILENAMESERVICE_H
#define FILENAMESERVICE_H

#include <ostream>
#include "CncCommon.h"
#include "wx/string.h"

class CncFileNameService {
	
	private:
	
		static wxString _appTempDir;
		static wxString _ret;
		static wxString _executablePath;
		static wxString _homeDirectory;
		static wxString _tempDirectory;
		static wxString _configDir;
		static wxString _session;
		static wxString _configFileName;
		static wxString _lruFileName;
		static wxString _preconfiguredSpeedConfigFileName;
		
		static void deleteFile(wxString fn);
	
	public:
		
		static void init();
		static void trace(std::ostream& os);
		static const wxString& getConfigFileName()				{ _ret = _configDir + _configFileName; return _ret; }
		static const wxString& getLruFileName()					{ _ret = _configDir + _lruFileName; return _ret; }
		static const wxString& getSpeedConfigFileName()			{ _ret = _configDir + _preconfiguredSpeedConfigFileName; return _ret; }
		
		static const wxString& getExecutableDir() 				{ return _executablePath; }
		static const wxString& getHomeDir() 					{ return _homeDirectory; }
		static const wxString& getTempDir() 					{ return _tempDirectory; }
		static const wxString& getConfigDir()					{ return _configDir; }
		
		static const char* getSession()							{ return _session; }
		
		static const char* getTempFileName(TemplateFormat f);
		static const char* getCncTemplatePreviewFileName(TemplateFormat f);
	
		static const char* getCncOutboundTxtFileName() 			{ _ret = _tempDirectory + "CncOutboundEmu.txt"; 	return _ret; }
		static const char* getCncOutboundSvgFileName() 			{ _ret = _tempDirectory + "CncOutboundEmu.svg"; 	return _ret; }
		static const char* getCncOutboundGCodeFileName() 		{ _ret = _tempDirectory + "CncOutboundEmu.gcode"; 	return _ret; }
		static const char* getCncOutboundBinFileName() 			{ _ret = _tempDirectory + "CncOutboundEmu.bct"; 	return _ret; }
		
		static const char* getCncOutboundTempFileName()			{ _ret = _tempDirectory + "CncOutboundTempEmu.xml"; return _ret; }
		static const char* getCncDrawPaneTraceFileName() 		{ _ret = _tempDirectory + "CncDrawPaneTrace.txt"; 	return _ret; }
		
		static const char* getBlankHtmlPageFileName()			{ _ret = _tempDirectory + "CncBlank.html"; 			return _ret; }
		static const char* getErrorHtmlPageFileName()			{ _ret = _tempDirectory + "CncError.html"; 			return _ret; }
		
		static void deleteCncOutboundSvgFileName() 				{ deleteFile(wxString(getCncOutboundSvgFileName())); }
		static void deleteCncOutboundTempFileName() 			{ deleteFile(wxString(getCncOutboundTempFileName())); }
		static void deleteCncDrawPaneTraceFileName() 			{ deleteFile(wxString(getCncDrawPaneTraceFileName())); }
		
};

#endif