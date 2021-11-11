#include <wx/filename.h>
#include "MainFrame.h"
#include "CncCommon.h"
#include "CncBoundarySpace.h"
#include "CncTemplateContext.h"

//////////////////////////////////////////////////////////////
CncTemplateContext::CncTemplateContext(CncBoundarySpace* bs)
: name					("")
, path					("")
, toolTotList			("")
, toolSelList			("")
, validRunCount			(0)
, bounderySpace			(bs)
, bounderies			()
, logInformation		()
//////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::reset() {
//////////////////////////////////////////////////////////////
	*this = CncTemplateContext(bounderySpace);
	updateGui(false);
}
//////////////////////////////////////////////////////////////
bool CncTemplateContext::isValid() const {
//////////////////////////////////////////////////////////////
	if ( name.IsEmpty() )
		return false;
		
	if ( wxFileName::Exists(getFileName()) == false )
		return false;
	
	// ....
	
	return true;
}
//////////////////////////////////////////////////////////////
const wxString CncTemplateContext::getFileName() const {
//////////////////////////////////////////////////////////////
	wxString ret(wxFileName(path, name).GetFullPath());
	return ret;
}
//////////////////////////////////////////////////////////////
bool CncTemplateContext::init(const wxString& pathFileName) {
//////////////////////////////////////////////////////////////
	reset();
	
	wxFileName fn(pathFileName);
	this->name.assign(fn.GetFullName());
	this->path.assign(fn.GetPath());
	
	return wxFileName::Exists(getFileName());
}
//////////////////////////////////////////////////////////////
bool CncTemplateContext::init(const wxString& path, const wxString& name) {
//////////////////////////////////////////////////////////////
	reset();
	
	this->name.assign(name);
	this->path.assign(path);
	
	return wxFileName::Exists(getFileName());
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::registerBounderies(const CncDoubleBounderies& b) {
//////////////////////////////////////////////////////////////
	bounderies = b;
	updateGui(false);
}




//////////////////////////////////////////////////////////////
void CncTemplateContext::updateGui(bool force) const {
//////////////////////////////////////////////////////////////
	if ( CncConfig::available() ) {
		wxTextCtrl* ctrl = THE_APP->GetTemplateContext();
		if ( ctrl->IsShownOnScreen() || force == true ) {
			std::stringstream ss;
			traceTo(ss, 2);
			ctrl->ChangeValue(ss.str());
		}
	}
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::traceTo(std::ostream& o, unsigned int indent) const {
//////////////////////////////////////////////////////////////
	const wxString prefix(' ', indent);
	
	auto traceBound = [&](const CncDoubleBounderies& b) {
		return wxString::Format("(%.3lf, %.3lf)(%.3lf, %.3lf)(%.3lf, %.3lf)"
								, b.xMin, b.xMax
								, b.yMin, b.yMax
								, b.zMin, b.zMax
		);
	};
	
	o	<< prefix << "Name                    : " << getFileName()					<< std::endl
		<< prefix << "Valid                   : " << (isValid() ? "Yes" : "No" )	<< std::endl
		<< prefix << "Valid run count         : " << validRunCount					<< std::endl
		<< prefix << "Tool Tot. List          : " << toolTotList					<< std::endl
		<< prefix << "Tool Sel. List          : " << toolSelList					<< std::endl
		<< prefix << "Tool Sel Count          : " << getToolSelCount()				<< std::endl
		<< prefix << "Bounderies         [mm] : " << traceBound(bounderies)			<< std::endl
	;
	
	o << std::endl;
	streamLogInfo(o, indent);
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::streamLogInfo(std::ostream& o, unsigned int indent) const {
//////////////////////////////////////////////////////////////
	const wxString prefix(' ', indent);
	
	o << "Log Information:" << std::endl;
	for ( auto it = logInformation.begin(); it != logInformation.end(); ++it )
	{
		o << "******************************************************\n";
		
		wxStringTokenizer lines((*it), "\n");
		while ( lines.HasMoreTokens() )
		{
			const wxString& token = lines.GetNextToken();
			o << prefix << token << std::endl;
		}
	}
}
//////////////////////////////////////////////////////////////
const wxString CncTemplateContext::getLastLogInfo(unsigned int indent) const {
//////////////////////////////////////////////////////////////
	const wxString prefix(' ', indent);
	if ( logInformation.size() > 0 )
	{
		if ( logInformation.back().Contains("\n") )
		{
			wxStringTokenizer lines(logInformation.back(), "\n");
			wxString ret;
			while ( lines.HasMoreTokens() )
				ret.append(wxString::Format("%s%s\n", prefix,  lines.GetNextToken()));
			
			return ret;
		}
		else
		{
			return logInformation.back();
		}
	}
	
	return wxEmptyString;
}