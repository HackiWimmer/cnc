
#include "wx/stdpaths.h"
#include "wx/filename.h"

#include "CncFileNameService.h"

wxString CncFileNameService::_ret(wxT(""));
wxString CncFileNameService::_executablePath(wxT(""));
wxString CncFileNameService::_homeDirectory(wxT(""));
wxString CncFileNameService::_tempDirectory(wxT(""));

///////////////////////////////////////////////////////////////////
void CncFileNameService::init() {
///////////////////////////////////////////////////////////////////
	wxString exe = wxStandardPaths::Get().GetExecutablePath();
	wxFileName f(exe);
	
	if ( f.HasVolume() )
		_executablePath = f.GetVolume() + wxFileName::GetVolumeSeparator();
 
	_executablePath += f.GetPath(wxPATH_GET_SEPARATOR);
	
	_homeDirectory = wxFileName::GetHomeDir() + wxFileName::GetPathSeparator();
	_tempDirectory = wxFileName::GetTempDir() + wxFileName::GetPathSeparator();
}
///////////////////////////////////////////////////////////////////
void CncFileNameService::trace(std::ostream& os) {
///////////////////////////////////////////////////////////////////
	os << "CncFileNameService:" << std::endl;
	os << " Executable Dir             : " << CncFileNameService::getExecutableDir() << std::endl;
	os << " Home Dir                   : " << CncFileNameService::getHomeDir() << std::endl;
	os << " Temp Dir                   : " << CncFileNameService::getTempDir() << std::endl;
	os << " Outbound Trace Filename    : " << CncFileNameService::getCncOutboundTraceFileName() << std::endl;
	os << " Outbound SVG Filename      : " << CncFileNameService::getCncOutboundSvgFileName() << std::endl;
	os << " Outbound Temp Filename     : " << CncFileNameService::getCncOutboundTempFileName() << std::endl;
	os << " Template Preview Filename  : " << CncFileNameService::getCncTemplatePreviewFileName(TplSvg) << std::endl;
	os << " Template Preview Filename  : " << CncFileNameService::getCncTemplatePreviewFileName(TplUnknown) << std::endl;
	os << " Draw Pane Trace Filename   : " << CncFileNameService::getCncDrawPaneTraceFileName() << std::endl;
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
		case TplSvg:	_ret = wxFileName::CreateTempFileName("CTF") + ".svg";
						return _ret;
						
		default:  		_ret = wxFileName::CreateTempFileName("CTF") + ".txt";
						return _ret;
		
		//{TplUnknown, TplSvg, TplText, TplGcode, TplManual};
	}
	
	return "";
}
///////////////////////////////////////////////////////////////////
const char* CncFileNameService::getCncTemplatePreviewFileName(TemplateFormat f) { 
///////////////////////////////////////////////////////////////////
	switch ( f ) {
		case TplSvg:	_ret = _tempDirectory + "CncTemplatePreview.svg";
						return _ret;
						
		default:  		_ret = _tempDirectory + "CncTemplatePreview.txt";
						return _ret;
		
		//{TplUnknown, TplSvg, TplText, TplGcode, TplManual};
	}
	
	return "";
}
