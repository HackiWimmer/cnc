#ifndef FILENAMESERVICE_H
#define FILENAMESERVICE_H

#include <ostream>
#include <wx/string.h>
#include <wx/filefn.h>
#include "CncCommon.h"

class CncFileNameService {
	
	private:
		
		static wxPathList	_pathList;
		
		static wxString		_appTempDir;
		static wxString		_executablePath;
		static wxString		_homeDirectory;
		static wxString		_tempDirectoryOS;
		static wxString		_tempDirectoryApp;
		static wxString		_tempDirectorySession;
		static wxString		_configDirectory;
		static wxString		_baseDirectory;
		static wxString		_databaseDirectory;
		static wxString		_session;
		static wxString		_configFileName;
		static wxString		_lruFileName;
		
		static void deleteFile(wxString fn);
	
	public:
		
		static void init();
		static void sessionHousekeeping();
		
		static bool ensureEndWithPathSep(wxString& str);
		
		static void trace(std::ostream& os);
		static const wxString getConfigFileName()						{ return wxString(_configDirectory + _configFileName); }
		static const wxString getLruFileName()							{ return wxString(_configDirectory + _lruFileName); }
		
		static const wxString& getExecutableDir()						{ return _executablePath; }
		static const wxString& getHomeDir()								{ return _homeDirectory; }
		static const wxString& getBaseDir()								{ return _baseDirectory; }
		static const wxString& getTempDirOS()							{ return _tempDirectoryOS; }
		static const wxString& getTempDirApp()							{ return _tempDirectoryApp; }
		static const wxString& getTempDirSession()						{ return _tempDirectorySession; }
		static const wxString& getConfigDir()							{ return _configDirectory; }
		static const wxString& getDatabaseDir() 						{ return _databaseDirectory; }
		
		static const char* getSession()									{ return _session; }
		
		static const char* getTempFileName(CncTemplateFormat f);
		static const char* getTempFileName(const wxString& extention);
		static const char* getCncTemplatePreviewFileName(CncTemplateFormat f);
		
		static const char* getStackTraceFileName()						{ return wxString(_tempDirectorySession + "CncStackTrace.txt"); }
		
		static const char* getCncOutboundTxtFileName()					{ return wxString(_tempDirectorySession + "CncOutboundEmu.txt"); }
		static const char* getCncOutboundSvgFileName()					{ return wxString(_tempDirectorySession + "CncOutboundEmu.svg"); }
		static const char* getCncOutboundGCodeFileName()				{ return wxString(_tempDirectorySession + "CncOutboundEmu.gcode"); }
		static const char* getCncOutboundBinFileName()					{ return wxString(_tempDirectorySession + "CncOutboundEmu.bct"); }
		
		static const char* getCncOutboundTempFileName()					{ return wxString(_tempDirectorySession + "CncOutboundTempEmu.xml"); }
		static const char* getCncDrawPaneTraceFileName()				{ return wxString(_tempDirectorySession + "CncDrawPaneTrace.txt"); }
		
		static const char* getBlankHtmlPageFileName()					{ return wxString(_tempDirectorySession + "CncBlank.html"); }
		static const char* getErrorHtmlPageFileName()					{ return wxString(_tempDirectorySession + "CncError.html"); }
		
		static const char* getCncPathListFileStoreDefFileName()			{ return wxString(_tempDirectorySession + "CncPathListFileStore.csv"); }
		
		static void deleteCncOutboundSvgFileName()						{ deleteFile(wxString(getCncOutboundSvgFileName())); }
		static void deleteCncOutboundTempFileName()						{ deleteFile(wxString(getCncOutboundTempFileName())); }
		static void deleteCncDrawPaneTraceFileName()					{ deleteFile(wxString(getCncDrawPaneTraceFileName())); }
		
		static bool     ensureFileAccessible	(const wxString& file)	{ return _pathList.EnsureFileAccessible(file); }
		static wxString findAbsoluteValidPath	(const wxString& file)	{ return _pathList.FindAbsoluteValidPath(file); }
		static wxString findValidPath			(const wxString& file)	{ return _pathList.FindValidPath(file); }
};

#endif