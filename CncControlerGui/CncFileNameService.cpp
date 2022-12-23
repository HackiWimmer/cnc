
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/datetime.h>

#include "GlobalFunctions.h"
#include "CncSha1Wrapper.h"
#include "CncFileNameService.h"

wxPathList	CncFileNameService::_pathList;

wxString	CncFileNameService::_configFileName("CncController.ini");
wxString	CncFileNameService::_lruFileName("CncControllerLruStore.ini");
wxString	CncFileNameService::_appTempDir("CncGuiController-TempFiles");

wxString	CncFileNameService::_executablePath(wxT(""));
wxString	CncFileNameService::_homeDirectory(wxT(""));
wxString	CncFileNameService::_tempDirectoryOS(wxT(""));
wxString	CncFileNameService::_tempDirectoryApp(wxT(""));
wxString	CncFileNameService::_tempDirectorySession(wxT(""));
wxString	CncFileNameService::_configDirectory(wxT(""));
wxString	CncFileNameService::_baseDirectory(wxT(""));
wxString	CncFileNameService::_databaseDirectory(wxT(""));
wxString	CncFileNameService::_transferDirectory(wxT(""));
wxString	CncFileNameService::_session(wxT(""));

wxString 	globalSessionString;

// ----------------------------------------------------------
class CncWoodworkingSession {
		
	public:
	
		static wxString SessionToken;
		
		CncWoodworkingSession() {
			
			// create session key
			wxDateTime unow(wxDateTime::UNow());
			wxString ts(wxString::Format("%s.%u", unow.FormatISOCombined(), unow.GetMillisecond()));
			wxString session;
			CncStringSha1::checksum(ts, session);
			session.append(SessionToken);
			
			globalSessionString.assign(session);
		}
	
};
wxString CncWoodworkingSession::SessionToken = ".Cnc.Woodworking.Session";
CncWoodworkingSession CncWoodworkingSessionInitializer;

// ----------------------------------------------------------
class CncFileHousekeeping : public wxDirTraverser {
	
	public:
		// -------------------------------------------------------------------
		CncFileHousekeeping() 
		: wxDirTraverser()
		, cntTotalDirs(0)
		, cntTotalFiles(0)
		, cntDeleted(0)
		, cntWrongDate(0)
		, cntWrongState(0)
		, cntToYoung(0)
		, cntWrongDir(0)
		{}
		// -------------------------------------------------------------------
		virtual ~CncFileHousekeeping()
		{}
		
		// -------------------------------------------------------------------
		virtual wxDirTraverseResult OnFile(const wxString& filename) 
		{
			cntTotalFiles++;
			return wxDIR_CONTINUE;
		}
		
		// -------------------------------------------------------------------
		virtual wxDirTraverseResult OnDir(const wxString& dirname) 
		{
			cntTotalDirs++;
			
			// ensure name ends with the separator
			const wxString dirName(dirname.Last() == wxFileName::GetPathSeparator() ? dirname : dirname + wxFileName::GetPathSeparator());
			const wxString refName(CncFileNameService::getTempDirApp());
			const wxString endToken(CncWoodworkingSession::SessionToken.Last() == wxFileName::GetPathSeparator() ? CncWoodworkingSession::SessionToken : CncWoodworkingSession::SessionToken + wxFileName::GetPathSeparator());
			wxFileName fn(dirName);
			
			// ------------------------------------------------------------
			// check dir date
			wxDateTime dtAccess, dtMod, dtCreate;
			if ( fn.GetTimes(&dtAccess, &dtMod, &dtCreate) == false )
				return cntWrongDate++, wxDIR_CONTINUE;
				
			// < 1: This defines a moving spread of 1d or 24h
			if ( abs((dtAccess - wxDateTime::Now()).GetDays()) < 1 )
				return cntToYoung++, wxDIR_CONTINUE;
				
			// ------------------------------------------------------------
			// check dir state
			if ( fn.IsDir() == false || fn.IsDirWritable() == false ) 
				return cntWrongState++, wxDIR_CONTINUE;
				
			// ------------------------------------------------------------
			// check dircount
			const unsigned int refDirCount = wxFileName(refName).GetDirCount();
			const unsigned int dirDirCount = fn.GetDirCount();
			
			// dir = ref + session dir
			if ( refDirCount != dirDirCount - 1  )
				return cntWrongDir++, wxDIR_CONTINUE;
				
			// ------------------------------------------------------------
			// check names
			if ( dirName.length() 				 < 40    ||
				 dirName.StartsWith(refName) 	== false || 
				 dirName.EndsWith(endToken)		== false ) 
			{
				return cntWrongDir++, wxDIR_CONTINUE;
			}
			
			// ------------------------------------------------------------
			// delete
			//std::cout << "Dir  : " << dirname << std::endl;
			#ifdef __WXMSW__
				wxFileName::Rmdir(dirname, wxPATH_RMDIR_RECURSIVE); 
			#else 
				std::cout << "remdir for '" << dirname << " currently not inplemented!" << std::endl;
			#endif
			cntDeleted++;
			return wxDIR_CONTINUE;
		}
		
		unsigned int cntTotalDirs;
		unsigned int cntTotalFiles;
		unsigned int cntDeleted;
		unsigned int cntWrongDate;
		unsigned int cntWrongState;
		unsigned int cntToYoung;
		unsigned int cntWrongDir;
};

///////////////////////////////////////////////////////////////////
void CncFileNameService::sessionHousekeeping() {
///////////////////////////////////////////////////////////////////
	CncFileHousekeeping sessions;
	
	wxDir dir(_tempDirectoryApp);
	dir.Traverse(sessions);
	APPEND_LOCATION_TO_STACK_TRACE_FILE_A(wxString::Format("Total = %u, Deleted = %u, ToYoung = %u, WrongState = %u, WrongDir = %u, WrongDate = %u", 
	                                                        sessions.cntTotalDirs, 
															sessions.cntDeleted, 
															sessions.cntToYoung, 
															sessions.cntWrongState, 
															sessions.cntWrongDir, 
															sessions.cntWrongDate));
}
///////////////////////////////////////////////////////////////////
void CncFileNameService::init() {
///////////////////////////////////////////////////////////////////
	_session.assign(globalSessionString);
	
	if ( _session.IsEmpty() )
		_session.assign("Default");
		
	const wxString sep(wxFileName::GetPathSeparator());
	
	wxString exe = wxStandardPaths::Get().GetExecutablePath();
	wxFileName f(exe);
	
	if ( f.HasVolume() )
		_executablePath = f.GetVolume() + wxFileName::GetVolumeSeparator();
 
	_executablePath += f.GetPath(wxPATH_GET_SEPARATOR);
	_homeDirectory 	 	= wxFileName::GetHomeDir() + sep;
	_tempDirectoryOS	= wxFileName::GetTempDir() + sep;
	_tempDirectoryApp	= _tempDirectoryOS + _appTempDir + sep;
		
	if ( wxFileName::DirExists(_tempDirectoryOS) == false )
		wxFileName::Mkdir(_tempDirectoryOS); 
		
	if ( wxFileName::DirExists(_tempDirectoryApp) == false )
		wxFileName::Mkdir(_tempDirectoryApp); 
		
	_tempDirectorySession = wxFileName::GetTempDir() + sep + _appTempDir + sep + _session + sep;
	if ( wxFileName::DirExists(_tempDirectorySession) == false )
		wxFileName::Mkdir(_tempDirectorySession); 

	wxFileName cfg(_executablePath + _configFileName);
	if ( cfg.Exists( ) )	_configDirectory = _executablePath;
	else					_configDirectory = _executablePath + ".." + sep;
	
	_baseDirectory 		= _configDirectory;
	_databaseDirectory 	= _baseDirectory + "Database" + sep;
	
	_transferDirectory 	= _baseDirectory + "Transfer" + sep;
	
	// fill path store
	_pathList.AddEnvList("PATH");
	_pathList.Add(CncFileNameService::getExecutableDir());
	_pathList.Add(CncFileNameService::getBaseDir());
	_pathList.Add(CncFileNameService::getDatabaseDir());
	_pathList.Add(CncFileNameService::getTransferDir());
	_pathList.Add(CncFileNameService::getConfigDir());
	_pathList.Add(CncFileNameService::getHomeDir());
	_pathList.Add(CncFileNameService::getTempDirOS());
	_pathList.Add(CncFileNameService::getTempDirApp());
	_pathList.Add(CncFileNameService::getTempDirSession());
}
///////////////////////////////////////////////////////////////////
void CncFileNameService::trace(std::ostream& os) {
///////////////////////////////////////////////////////////////////
	os << "Executable Dir                    : " << CncFileNameService::getExecutableDir() 							<< std::endl;
	os << "Base Dir                          : " << CncFileNameService::getBaseDir()								<< std::endl;
	os << "Databse Dir                       : " << CncFileNameService::getDatabaseDir()							<< std::endl;
	os << "Transfer Dir                      : " << CncFileNameService::getTransferDir()							<< std::endl;
	os << "Config Dir                        : " << CncFileNameService::getConfigDir() 								<< std::endl;
	os << "Users Home Dir                    : " << CncFileNameService::getHomeDir() 								<< std::endl;
	os << "Users Temp Dir                    : " << CncFileNameService::getTempDirOS() 								<< std::endl;
	os << "App Temp Dir                      : " << CncFileNameService::getTempDirApp() 							<< std::endl;
	os << "Session Temp Dir                  : " << CncFileNameService::getTempDirSession() 						<< std::endl;
	os << "Outbound Binary Filename          : " << CncFileNameService::getCncOutboundBinFileName() 				<< std::endl;
	os << "Outbound TXT Filename             : " << CncFileNameService::getCncOutboundTxtFileName() 				<< std::endl;
	os << "Outbound SVG Filename             : " << CncFileNameService::getCncOutboundSvgFileName() 				<< std::endl;
	os << "Outbound GCODE Filename           : " << CncFileNameService::getCncOutboundGCodeFileName() 				<< std::endl;
	os << "Outbound Temp Filename            : " << CncFileNameService::getCncOutboundTempFileName() 				<< std::endl;
	os << "Template SVG Preview Filename     : " << CncFileNameService::getCncTemplatePreviewFileName(TplSvg) 		<< std::endl;
	os << "Template Default Preview Filename : " << CncFileNameService::getCncTemplatePreviewFileName(TplUnknown) 	<< std::endl;
	os << "Draw Pane Trace Filename          : " << CncFileNameService::getCncDrawPaneTraceFileName() 				<< std::endl;
	os << "LRU Filename                      : " << CncFileNameService::getLruFileName() 							<< std::endl;
	os << "Stack Trace Filename              : " << CncFileNameService::getStackTraceFileName() 					<< std::endl;
	
	os << "Search Path Entries               : " << ""											 					<< std::endl;
	for ( size_t i=0; i< _pathList.GetCount(); i++ )
	{
		os << wxString::Format(" - Path(%00ld): ", (long)i) << _pathList.Item(i)				 					<< std::endl;
	}
}
///////////////////////////////////////////////////////////////////
bool CncFileNameService::ensureEndWithPathSep(wxString& str) {
///////////////////////////////////////////////////////////////////
	str.Trim();
	
	if ( str.EndsWith(wxFileName::GetPathSeparator()) == false)
	{
		str.append(wxFileName::GetPathSeparator());
		return true;
	}
	
	return false;
}
///////////////////////////////////////////////////////////////////
void CncFileNameService::deleteFile(wxString fn) {
///////////////////////////////////////////////////////////////////
	wxFileName f(fn);
	if ( f.Exists() && f.IsFileWritable() )
		wxRemoveFile(fn);
}
///////////////////////////////////////////////////////////////////
wxString CncFileNameService::getTempFileName(const wxString& extention) {
///////////////////////////////////////////////////////////////////
	wxString ret = wxFileName::CreateTempFileName(_tempDirectorySession);
	ret.append(".");
	ret.append(extention);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
wxString CncFileNameService::getTempFileName(CncTemplateFormat f) {
///////////////////////////////////////////////////////////////////
	switch ( f )
	{
		case TplBinary:		return wxString(wxFileName::CreateTempFileName(_tempDirectorySession + "BIN")); 
		case TplSvg:		return wxString(wxFileName::CreateTempFileName(_tempDirectorySession + "SVG"));
		case TplGcode:		return wxString(wxFileName::CreateTempFileName(_tempDirectorySession + "GCODE"));
		default:			return wxString(wxFileName::CreateTempFileName(_tempDirectorySession + "CTF"));
	}
	
	return wxEmptyString;
}
///////////////////////////////////////////////////////////////////
wxString CncFileNameService::getCncTemplatePreviewFileName(CncTemplateFormat f) { 
///////////////////////////////////////////////////////////////////
	return wxEmptyString;
}
///////////////////////////////////////////////////////////////////
bool CncFileNameService::isAsNoSerialFile(const wxString& fileName) {
///////////////////////////////////////////////////////////////////
	wxFileName refFile(fileName);
	refFile.MakeAbsolute();
	
	wxFileName f1(getNoSerialPreviewAvailableFile());
	wxFileName f2(getNoSerialContentAvailableFile());
	wxFileName f3(getNoSerialPreviewRegisteredFile());
	f1.MakeAbsolute();
	f2.MakeAbsolute();
	f3.MakeAbsolute();
	
	if      ( refFile == f1 ) return true;
	else if ( refFile == f2 ) return true;
	else if ( refFile == f3 ) return true;
	
	return false;
}

