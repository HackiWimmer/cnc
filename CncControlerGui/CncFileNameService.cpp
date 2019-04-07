
#include "wx/stdpaths.h"
#include "wx/filename.h"
#include <wx/datetime.h>

#include "CncSha1Wrapper.h"
#include "CncFileNameService.h"

wxString CncFileNameService::_configFileName("CncController.ini");
wxString CncFileNameService::_lruFileName("CncControllerLruStore.ini");

wxString CncFileNameService::_preconfiguredSpeedConfigFileName(wxString("Database") + wxFileName::GetPathSeparator() + "PreconfiguredSpeedSetups.ini");
wxString CncFileNameService::_appTempDir("CncGuiController-TempFiles");

wxString CncFileNameService::_ret(wxT(""));
wxString CncFileNameService::_executablePath(wxT(""));
wxString CncFileNameService::_homeDirectory(wxT(""));
wxString CncFileNameService::_tempDirectory(wxT(""));
wxString CncFileNameService::_configDirectory(wxT(""));
wxString CncFileNameService::_baseDirectory(wxT(""));
wxString CncFileNameService::_databaseDirectory(wxT(""));
wxString CncFileNameService::_session(wxT(""));

wxString globalSessionString;

// house keeping
class CncWoodworkingSession {
	private:
	
		const char* sessionExt = ".Cnc.Woodworking.Session";
	
	public:
		CncWoodworkingSession() {
			
			// create session key
			wxDateTime unow(wxDateTime::UNow());
			wxString ts(wxString::Format("%s.%u", unow.FormatISOCombined(), unow.GetMillisecond()));
			wxString session;
			CncStringSha1::checksum(ts, session);
			session.append(sessionExt);
			
			globalSessionString.assign(session);
		}
		
		~CncWoodworkingSession() {
			std::cout << "Housekeeping:" << std::endl;
			
			// remove session directory
			wxFileName tmpDir(CncFileNameService::getTempDir());
			wxArrayString dirs 	= tmpDir.GetDirs();
			wxString last 		= dirs.Last();
			
			if ( last.length() == 40 + wxString(sessionExt).length() ) {
				
				#ifdef __WXMSW__
					std::cout << " Remove: " << tmpDir.GetFullPath() << std::endl;
					tmpDir.Rmdir(wxPATH_RMDIR_RECURSIVE);
				#else 
					std::cout << " Remove: " << tmpDir.GetFullPath() << std::endl;
					std::cout << "  <currently not implmeneted> " << std::endl;
				#endif
			}
		}
	
} CncWoodworkingSessionInitializer;

///////////////////////////////////////////////////////////////////
void CncFileNameService::init() {
///////////////////////////////////////////////////////////////////
	_session.assign(globalSessionString);
	
	if ( _session.IsEmpty() )
		_session.assign("Default");
		
	wxString sep(wxFileName::GetPathSeparator());
	
	wxString exe = wxStandardPaths::Get().GetExecutablePath();
	wxFileName f(exe);
	
	if ( f.HasVolume() )
		_executablePath = f.GetVolume() + wxFileName::GetVolumeSeparator();
 
	_executablePath += f.GetPath(wxPATH_GET_SEPARATOR);
	_homeDirectory 	 = wxFileName::GetHomeDir() + sep;
	_tempDirectory 	 = wxFileName::GetTempDir() + sep + _appTempDir + sep;
		
	if ( wxFileName::DirExists(_tempDirectory) == false )
		wxFileName::Mkdir(_tempDirectory); 
		
	_tempDirectory = wxFileName::GetTempDir() + sep + _appTempDir + sep + _session + sep;
	if ( wxFileName::DirExists(_tempDirectory) == false )
		wxFileName::Mkdir(_tempDirectory); 

	wxFileName cfg(_executablePath + _configFileName);
	if ( cfg.Exists( ) )	_configDirectory = _executablePath;
	else					_configDirectory = _executablePath + ".." + sep;
	
	_baseDirectory 		= _configDirectory;
	_databaseDirectory 	= _baseDirectory + "Database" + sep;
}
///////////////////////////////////////////////////////////////////
void CncFileNameService::trace(std::ostream& os) {
///////////////////////////////////////////////////////////////////
	os << "Executable Dir                    : " << CncFileNameService::getExecutableDir() 							<< std::endl;
	os << "Base Dir                          : " << CncFileNameService::getBaseDir()								<< std::endl;
	os << "Databse Dir                       : " << CncFileNameService::getDatabaseDir()							<< std::endl;
	os << "Config Dir                        : " << CncFileNameService::getConfigDir() 								<< std::endl;
	os << "Users Home Dir                    : " << CncFileNameService::getHomeDir() 								<< std::endl;
	os << "Useres Temp Dir                   : " << CncFileNameService::getTempDir() 								<< std::endl;
	os << "Outbound Binary Filename          : " << CncFileNameService::getCncOutboundBinFileName() 				<< std::endl;
	os << "Outbound TXT Filename             : " << CncFileNameService::getCncOutboundTxtFileName() 				<< std::endl;
	os << "Outbound SVG Filename             : " << CncFileNameService::getCncOutboundSvgFileName() 				<< std::endl;
	os << "Outbound GCODE Filename           : " << CncFileNameService::getCncOutboundGCodeFileName() 				<< std::endl;
	os << "Outbound Temp Filename            : " << CncFileNameService::getCncOutboundTempFileName() 				<< std::endl;
	os << "Template SVG Preview Filename     : " << CncFileNameService::getCncTemplatePreviewFileName(TplSvg) 		<< std::endl;
	os << "Template Default Preview Filename : " << CncFileNameService::getCncTemplatePreviewFileName(TplUnknown) 	<< std::endl;
	os << "Draw Pane Trace Filename          : " << CncFileNameService::getCncDrawPaneTraceFileName() 				<< std::endl;
	os << "LRU Filename                      : " << CncFileNameService::getLruFileName() 							<< std::endl;
	os << "Speed Config Filename             : " << CncFileNameService::getSpeedConfigFileName() 					<< std::endl;
}
///////////////////////////////////////////////////////////////////
void CncFileNameService::deleteFile(wxString fn) {
///////////////////////////////////////////////////////////////////
	wxFileName f(fn);
	if ( f.Exists() && f.IsFileWritable() )
		wxRemoveFile(fn);
}
///////////////////////////////////////////////////////////////////
const char* CncFileNameService::getTempFileName(TemplateFormat f) {
///////////////////////////////////////////////////////////////////
	
	switch ( f ) {
		case TplBinary:		_ret = wxFileName::CreateTempFileName(_tempDirectory + "BIN"); 
							break;
		case TplSvg:		_ret = wxFileName::CreateTempFileName(_tempDirectory + "SVG");
							break;
		case TplGcode:		_ret = wxFileName::CreateTempFileName(_tempDirectory + "GCODE");
							break;
							
		default:			_ret = wxFileName::CreateTempFileName(_tempDirectory + "CTF");
	}
	
	return _ret;
}
///////////////////////////////////////////////////////////////////
const char* CncFileNameService::getCncTemplatePreviewFileName(TemplateFormat f) { 
///////////////////////////////////////////////////////////////////
	return "";
}


