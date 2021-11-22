#include <wx/filename.h>
#include "MainFrame.h"
#include "CncCommon.h"
#include "CncTemplateContextSummaryPanel.h"
#include "CncBoundarySpace.h"
#include "CncTemplateContext.h"

//////////////////////////////////////////////////////////////
CncTemplateContext::CncTemplateContext(CncBoundarySpace* bs)
: ContextInterface		()
, name					("")
, path					("")
, toolTotList			("")
, toolSelList			("")
, runCount				(0)
, validRunCount			(0)
, bounderySpace			(bs)
, bounderies			()
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
	
	if ( hasErrors() == true )
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
void CncTemplateContext::registerBounderies(const CncDoubleBoundaries& b) {
//////////////////////////////////////////////////////////////
	bounderies = b;
	updateGui(false);
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::updateGui(bool force) const {
//////////////////////////////////////////////////////////////
	if ( CncConfig::available() )
	{
		if ( THE_APP->getTemplateContextSummary()->IsShownOnScreen() || force == true )
			THE_APP->getTemplateContextSummary()->update();
	}
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::traceTo(std::ostream& o, unsigned int indent) const {
//////////////////////////////////////////////////////////////
	const wxString prefix(' ', indent);
	
	auto traceBound = [&](const CncDoubleBoundaries& b) {
		return wxString::Format("(%.3lf, %.3lf)(%.3lf, %.3lf)(%.3lf, %.3lf)"
								, b.xMin, b.xMax
								, b.yMin, b.yMax
								, b.zMin, b.zMax
		);
	};
	
	wxFileName fn(getFileName());
	
	o	<< prefix << "Name                    : " << fn.GetFullName()				<< std::endl
		<< prefix << "Path                    : " << fn.GetPath()					<< std::endl
		<< prefix << "Valid                   : " << (isValid() ? "Yes" : "No" )	<< std::endl
		<< prefix << "Total run count         : " << runCount						<< std::endl
		<< prefix << "Valid run count         : " << validRunCount					<< std::endl
		<< prefix << "Errors                  : " << hasErrors()					<< std::endl
		<< prefix << "Tool Tot. List          : " << toolTotList					<< std::endl
		<< prefix << "Tool Sel. List          : " << toolSelList					<< std::endl
		<< prefix << "Tool Sel Count          : " << getToolSelCount()				<< std::endl
		<< prefix << "Bounderies         [mm] : " << traceBound(bounderies)			<< std::endl
	;
	
	if ( hasErrors() )
	{
		// additional error infos from ContextInterface
		o << " \n\nAdditional error info:\n";
		traceErrorInfoTo(o);
	}
	
	o << std::endl;
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::notifyBeginRun() {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	ContextInterface::notifyBeginRun();
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::notifyEndRun() {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	ContextInterface::notifyEndRun();
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::notifyClientId(long id) {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	ContextInterface::notifyClientId(id);
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::notifyLimit(const CncInterface::ILS::States& s) {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	ContextInterface::notifyLimit(s);
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::notifyMove(unsigned char cmd, int32_t dx, int32_t dy, int32_t dz) {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	ContextInterface::notifyMove(cmd, dx, dy, dz);
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::notifySpindleOn() {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	ContextInterface::notifySpindleOn();
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::notifySpindleOff() {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	ContextInterface::notifySpindleOff();
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::notifySpindleSpeed(unsigned char pid, ArdoObj::SpindleTupleValue s) {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	ContextInterface::notifySpindleSpeed(pid, s);
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::notifyStepperSpeed(unsigned char pid, ArdoObj::SpeedTupleValue s) {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	ContextInterface::notifyStepperSpeed(pid, s);
}

