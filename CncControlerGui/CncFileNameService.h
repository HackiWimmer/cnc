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
		static wxString		_transferDirectory;
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
		static const wxString& getTransferDir()							{ return _transferDirectory; }
		
		static const char* getSession()									{ return _session; }
		
		static wxString getTempFileName(CncTemplateFormat f);
		static wxString getTempFileName(const wxString& extention);
		static wxString getCncTemplatePreviewFileName(CncTemplateFormat f);
		
		static wxString getStackTraceFileName()							{ return wxString(_tempDirectorySession + "CncStackTrace.txt"); }
		
		static wxString getCncOutboundTxtFileName()						{ return wxString(_tempDirectorySession + "CncOutboundEmu.txt"); }
		static wxString getCncOutboundSvgFileName()						{ return wxString(_tempDirectorySession + "CncOutboundEmu.svg"); }
		static wxString getCncOutboundGCodeFileName()					{ return wxString(_tempDirectorySession + "CncOutboundEmu.gcode"); }
		static wxString getCncOutboundBinFileName()						{ return wxString(_tempDirectorySession + "CncOutboundEmu.bct"); }
		
		static wxString getCncOutboundTempFileName()					{ return wxString(_tempDirectorySession + "CncOutboundTempEmu.xml"); }
		static wxString getCncDrawPaneTraceFileName()					{ return wxString(_tempDirectorySession + "CncDrawPaneTrace.txt"); }
		
		static wxString getBlankHtmlPageFileName()						{ return wxString(_tempDirectorySession + "CncBlank.html"); }
		static wxString getErrorHtmlPageFileName()						{ return wxString(_tempDirectorySession + "CncError.html"); }
		
		static wxString getCncPathListFileStoreDefFileName()			{ return wxString(_tempDirectorySession + "CncPathListFileStore.csv"); }
		
		static void deleteCncOutboundSvgFileName()						{ deleteFile(wxString(getCncOutboundSvgFileName())); }
		static void deleteCncOutboundTempFileName()						{ deleteFile(wxString(getCncOutboundTempFileName())); }
		static void deleteCncDrawPaneTraceFileName()					{ deleteFile(wxString(getCncDrawPaneTraceFileName())); }
		
		static bool     ensureFileAccessible	(const wxString& file)	{ return _pathList.EnsureFileAccessible(file); }
		static wxString findAbsoluteValidPath	(const wxString& file)	{ return _pathList.FindAbsoluteValidPath(file); }
		static wxString findValidPath			(const wxString& file)	{ return _pathList.FindValidPath(file); }
		
		static wxString getNoSerialPreviewAvailableFile()				{ return wxString::Format("%s%s", CncFileNameService::getDatabaseDir(), "NoSerialPreviewAvailable.svg"); }
		static wxString getNoSerialContentAvailableFile()				{ return wxString::Format("%s%s", CncFileNameService::getDatabaseDir(), "NoSerialContentAvailable.svg"); }
		static wxString getNoSerialPreviewRegisteredFile()				{ return wxString::Format("%s%s", CncFileNameService::getDatabaseDir(), "NoSerialPreviewRegistered.svg"); }
		static bool isAsNoSerialFile(const wxString& fileName);
};


#endif