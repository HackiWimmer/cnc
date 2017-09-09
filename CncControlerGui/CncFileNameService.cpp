
#include "wx/stdpaths.h"
#include "wx/filename.h"

#include "CncFileNameService.h"

wxString CncFileNameService::_configFileName("CncController.ini");
wxString CncFileNameService::_lruFileName("CncControllerLruStore.ini");
wxString CncFileNameService::_preconfiguredSpeedConfigFileName(wxString("Database") + wxFileName::GetPathSeparator() + "PreconfiguredSpeedSetups.ini");

wxString CncFileNameService::_appTempDir("CncGuiController-TempFiles");

wxString CncFileNameService::_ret(wxT(""));
wxString CncFileNameService::_executablePath(wxT(""));
wxString CncFileNameService::_homeDirectory(wxT(""));
wxString CncFileNameService::_tempDirectory(wxT(""));
wxString CncFileNameService::_configDir(wxT(""));

///////////////////////////////////////////////////////////////////
void CncFileNameService::init() {
///////////////////////////////////////////////////////////////////
	wxString exe = wxStandardPaths::Get().GetExecutablePath();
	wxFileName f(exe);
	
	if ( f.HasVolume() )
		_executablePath = f.GetVolume() + wxFileName::GetVolumeSeparator();
 
	_executablePath += f.GetPath(wxPATH_GET_SEPARATOR);
	
	_homeDirectory = wxFileName::GetHomeDir() + wxFileName::GetPathSeparator();
	_tempDirectory = wxFileName::GetTempDir() + wxFileName::GetPathSeparator() + _appTempDir + wxFileName::GetPathSeparator();
	
	if ( wxFileName::DirExists(_tempDirectory) == false )
		wxFileName::Mkdir(_tempDirectory); 
	
	wxFileName cfg(_executablePath + _configFileName);
	if ( cfg.Exists( ) )	_configDir = _executablePath;
	else					_configDir = _executablePath + ".." + wxFileName::GetPathSeparator();
}
///////////////////////////////////////////////////////////////////
void CncFileNameService::trace(std::ostream& os) {
///////////////////////////////////////////////////////////////////
	os << "CncFileNameService:" << std::endl;
	os << " Executable Dir             : " << CncFileNameService::getExecutableDir() << std::endl;
	os << " Config Dir                 : " << CncFileNameService::getConfigDir() << std::endl;
	os << " Home Dir                   : " << CncFileNameService::getHomeDir() << std::endl;
	os << " Temp Dir                   : " << CncFileNameService::getTempDir() << std::endl;
	os << " Outbound Trace Filename    : " << CncFileNameService::getCncOutboundTraceFileName() << std::endl;
	os << " Outbound SVG Filename      : " << CncFileNameService::getCncOutboundSvgFileName() << std::endl;
	os << " Outbound Temp Filename     : " << CncFileNameService::getCncOutboundTempFileName() << std::endl;
	os << " Template Preview Filename  : " << CncFileNameService::getCncTemplatePreviewFileName(TplSvg) << std::endl;
	os << " Template Preview Filename  : " << CncFileNameService::getCncTemplatePreviewFileName(TplUnknown) << std::endl;
	os << " Draw Pane Trace Filename   : " << CncFileNameService::getCncDrawPaneTraceFileName() << std::endl;
	os << " LRU Filename               : " << CncFileNameService::getLruFileName() << std::endl;
	os << " Speed Connfig Filename     : " << CncFileNameService::getSpeedConfigFileName() << std::endl;
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
	_ret = wxFileName::CreateTempFileName(_tempDirectory + "CTF");
	return _ret;
}
///////////////////////////////////////////////////////////////////
const char* CncFileNameService::getCncTemplatePreviewFileName(TemplateFormat f) { 
///////////////////////////////////////////////////////////////////
	switch ( f ) {
		case TplSvg:
		case TplGcode:
						_ret = _tempDirectory + "CncTemplatePreview.svg";
						return _ret;
						
		default:  		_ret = _tempDirectory + "CncTemplatePreview.txt";
						return _ret;
		
		//{TplUnknown, TplSvg, TplText, TplGcode, TplManual};
	}
	
	return "";
}


