#include <wx/filename.h>
#include "MainFrame.h"
#include "CncCommon.h"
#include "CncTemplateContextSummaryPanel.h"
#include "CncPathListInterfaceCnc.h"
#include "CncBoundarySpace.h"
#include "CncTemplateContext.h"

//////////////////////////////////////////////////////////////
CncTemplateContext::CncTemplateContext(CncBoundarySpace* bs)
: ContextInterface		()
, modifyFlag			(false)
, name					("")
, path					("")
, toolTotList			("")
, toolSelList			("")
, runCount				(0)
, validRunCount			(0)
, boundarySpace			(bs)
, templateBounds		()
, measuredBounds		()
, cncInterface			(NULL)
//////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::reset() {
//////////////////////////////////////////////////////////////
	*this = CncTemplateContext(boundarySpace);
	updateGui(true);
}
//////////////////////////////////////////////////////////////
bool CncTemplateContext::isValid() const {
//////////////////////////////////////////////////////////////
	if ( name.IsEmpty() )
		return false;
		
	if ( wxFileName::Exists(getFileName()) == false )
		return false;
		
	if ( modifyFlag == true )
		return false;
	
	if ( cncInterface == NULL )
		return false;
	
	if ( hasErrors() == true )
		return false;
		
	// ....
	
	return true;
}
//////////////////////////////////////////////////////////////
bool CncTemplateContext::fitsIntoCurrentHardwareBoundaries(std::ostream& o) {
//////////////////////////////////////////////////////////////
	if ( boundarySpace == NULL )
	{
		o << "Error:\n Invalid pointer!\n";
		return false;
	}
	
	if ( boundarySpace->isValid() == false )
	{
		o << "Error:\n No hardware information available!\n";
		return false;
	}
	
	if ( getBoundaries().hasBoundaries() == false ) 
	{
		o << "Error:\n No template boundary information available!\n";
		return false;
	}
	
	const CncDoublePosition tplMin = getBoundaries().getMinBound();
	const CncDoublePosition tplMax = getBoundaries().getMaxBound();
	const CncDoublePosition hwdMin = boundarySpace->getPhysicallyBoundaries().getMinBound();
	const CncDoublePosition hwdMax = boundarySpace->getPhysicallyBoundaries().getMaxBound();
	
	const CncDoubleBoundaries nrmTplBounds = getBoundaries().normalize();
	const CncDoubleBoundaries nrmHwdBounds = boundarySpace->getLogicallyBoundaries().normalize();
	
	if ( nrmTplBounds.fitsInside(nrmHwdBounds) == false )
	{
		o	<< "Error:\n"
			<< " The current template don't fits into the available hardware space!\n"
			<< " Issue List [Template vs. Hardware]:\n"
		;
		
		if ( nrmTplBounds.getMaxDistanceX() > nrmHwdBounds.getMaxDistanceX() )
			o << "  Distance X [mm]: " << nrmTplBounds.getMaxDistanceX() << " > " << nrmHwdBounds.getMaxDistanceX() << std::endl;
			
		if ( nrmTplBounds.getMaxDistanceY() > nrmHwdBounds.getMaxDistanceY() )
			o << "  Distance Y [mm]: " << nrmTplBounds.getMaxDistanceY() << " > " << nrmHwdBounds.getMaxDistanceY() << std::endl;
			
		if ( nrmTplBounds.getMaxDistanceZ() > nrmHwdBounds.getMaxDistanceZ() )
			o << "  Distance Z [mm]: " << nrmTplBounds.getMaxDistanceZ() << " > " << nrmHwdBounds.getMaxDistanceZ() << std::endl;
		
		updateGui(true);
		return false;
	}

	if ( getBoundaries().fitsInside(boundarySpace->getPhysicallyBoundaries()) == false )
	{
		o	<< "Error:\n"
			<< " The current template don't fits regarding the current origin position!\n"
			<< " Issue List [Template vs. Hardware]:\n"
		;
		
		if ( tplMin.getX() < hwdMin.getX() )	o << "  Xmin [mm]: " << tplMin.getX() << " < " << hwdMin.getX() << std::endl;
		if ( tplMin.getY() < hwdMin.getY() )	o << "  Ymin [mm]: " << tplMin.getY() << " < " << hwdMin.getY() << std::endl;
		if ( tplMin.getZ() < hwdMin.getZ() )	o << "  Zmin [mm]: " << tplMin.getZ() << " < " << hwdMin.getZ() << std::endl;
		if ( tplMax.getX() > hwdMax.getX() )	o << "  Xmax [mm]: " << tplMax.getX() << " > " << hwdMax.getX() << std::endl;
		if ( tplMax.getY() > hwdMax.getY() )	o << "  Ymax [mm]: " << tplMax.getY() << " > " << hwdMax.getY() << std::endl;
		if ( tplMax.getZ() > hwdMax.getZ() )	o << "  Zmax [mm]: " << tplMax.getZ() << " > " << hwdMax.getZ() << std::endl;
		
		updateGui(true);
		return false;
	}
	
	o << "Info:\n The current template fits . . .\n";
	updateGui(true);
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
	updateGui(true);
	
	return wxFileName::Exists(getFileName());
}
//////////////////////////////////////////////////////////////
bool CncTemplateContext::init(const wxString& path, const wxString& name) {
//////////////////////////////////////////////////////////////
	reset();
	
	this->name.assign(name);
	this->path.assign(path);
	updateGui(true);
	
	return wxFileName::Exists(getFileName());
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::registerBoundaries(const CncDoubleBoundaries& b, BoundType bt) {
//////////////////////////////////////////////////////////////
	if ( bt == BT_TEMPLATE )	templateBounds	= b;
	else						measuredBounds	= b;
		
	updateGui(true);
}
//////////////////////////////////////////////////////////////
const CncDoubleBoundaries& CncTemplateContext::getBoundaries(BoundType bt) const {
//////////////////////////////////////////////////////////////
	if ( bt == BT_TEMPLATE )
		return templateBounds;
		
	return measuredBounds; 
}
//////////////////////////////////////////////////////////////
bool CncTemplateContext::hasBoundaries(BoundType bt) const { 
//////////////////////////////////////////////////////////////
	if ( bt == BT_TEMPLATE )
		return templateBounds.hasBoundaries(); 
		
	return measuredBounds.hasBoundaries(); 
}
//////////////////////////////////////////////////////////////
const CncDoubleBoundaries& CncTemplateContext::getBoundaries() const {
//////////////////////////////////////////////////////////////
	// measured beats template
	if ( measuredBounds.hasBoundaries() )
		return measuredBounds; 
		
	return templateBounds;
}
//////////////////////////////////////////////////////////////
bool CncTemplateContext::hasBoundaries() const { 
//////////////////////////////////////////////////////////////
	// measured beats template
	if ( measuredBounds.hasBoundaries() )
		return true;
		
	return templateBounds.hasBoundaries(); 
}
//////////////////////////////////////////////////////////////
CncTemplateContext::BoundType CncTemplateContext::getBoundLevel() const {
//////////////////////////////////////////////////////////////
	// measured beats template
	if ( measuredBounds.hasBoundaries() )
		return BT_MEASURED;
		
	return BT_TEMPLATE; 
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::updateGui(bool force) const {
//////////////////////////////////////////////////////////////
	if ( CncConfig::available() )
	{
		if ( THE_APP->getTemplateContextSummary()->IsShownOnScreen() || force == true )
		{
			// do not propagate the force flag here because the receiving view 
			// decides by itself id a force is necessary ore not
			// only call the update function
			THE_APP->getTemplateContextSummary()->update(false);
		}
	}
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::traceTo(std::ostream& o, unsigned int indent) const {
//////////////////////////////////////////////////////////////
	const wxString prefix(' ', indent);
	
	auto traceBound = [](const CncDoubleBoundaries& b) {
		return wxString::Format("(%8.3lf, %8.3lf)(%8.3lf, %8.3lf)(%8.3lf, %8.3lf)"
								, b.xMin, b.xMax
								, b.yMin, b.yMax
								, b.zMin, b.zMax
		);
	};
	
	auto traceBoundLvl = [](BoundType bt) {
		return ( bt == BT_TEMPLATE ? "Template" : "Measured");
	};
	
	wxFileName fn(getFileName());
	
	o	<< prefix << "Name                           : " << fn.GetFullName()				<< std::endl
		<< prefix << "Path                           : " << fn.GetPath()					<< std::endl
		<< prefix << "Valid                          : " << (isValid()    ? "Yes" : "No" )	<< std::endl
		<< prefix << "Modified                       : " << (modifyFlag   ? "Yes" : "No" )	<< std::endl
		<< prefix << "CNC Interface                  : " << (cncInterface ? "Yes" : "No" )	<< std::endl
		<< prefix << "Total run count                : " << runCount						<< std::endl
		<< prefix << "Valid run count                : " << validRunCount					<< std::endl
		<< prefix << "Errors                         : " << hasErrors()						<< std::endl
		<< prefix << "Tool Tot. List                 : " << toolTotList						<< std::endl
		<< prefix << "Tool Sel. List                 : " << toolSelList						<< std::endl
		<< prefix << "Tool Sel Count                 : " << getToolSelCount()				<< std::endl
		<< prefix << "Bound Level                    : " << traceBoundLvl(getBoundLevel())	<< std::endl
		<< prefix << "Template Bounds (X)(Y)(Z) [mm] : " << traceBound(templateBounds)		<< std::endl
		<< prefix << "Measured Bounds (X)(Y)(Z) [mm] : " << traceBound(measuredBounds)		<< std::endl
	;
	
	if ( isValid() == false )
	{
		// additional error infos from ContextInterface
		o << " \n\nAdditional error info:\n";
		
		const bool fileExists  = wxFileName::Exists(getFileName());
		const bool interfaceOk = hasValidRuns() ? cncInterface != NULL : true;
		
		
		if ( modifyFlag   == true  )	o << " Template is modified\n";
		if ( interfaceOk  == false )	o << " Invalid CNC interface\n";
		if ( fileExists   == false )	o << " Invalid file name\n";
		
		if ( hasErrors() )
			traceErrorInfoTo(o);
	}
	
	o << std::endl;
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::notifyBeginRun() {
//////////////////////////////////////////////////////////////
	CNC_CEX2_FUNCT_A(": %d", hasValidRuns())
	
	//this notification is only meaningful as long as 
	//the template was not valid processed.
	if ( hasValidRuns() == false )
		ContextInterface::notifyBeginRun();
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::notifyEndRun() {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	
	//this notification is only meaningful as long as 
	//the template was not valid processed.
	if ( hasValidRuns() == false )
		ContextInterface::notifyEndRun();
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::notifyClientId(long id) {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	
	//this notification is only meaningful as long as 
	//the template was not valid processed.
	if ( hasValidRuns() == false )
		ContextInterface::notifyClientId(id);
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::notifyLimit(const CncInterface::ILS::States& s) {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	
	//this notification is only meaningful as long as 
	//the template was not valid processed.
	if ( hasValidRuns() == false )
		ContextInterface::notifyLimit(s);
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::notifyMove(unsigned char cmd, int32_t dx, int32_t dy, int32_t dz) {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	
	//this notification is only meaningful as long as 
	//the template was not valid processed.
	if ( hasValidRuns() == false )
		ContextInterface::notifyMove(cmd, dx, dy, dz);
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::notifySpindleOn() {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	
	//this notification is only meaningful as long as 
	//the template was not valid processed.
	if ( hasValidRuns() == false )
		ContextInterface::notifySpindleOn();
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::notifySpindleOff() {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	
	//this notification is only meaningful as long as 
	//the template was not valid processed.
	if ( hasValidRuns() == false )
		ContextInterface::notifySpindleOff();
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::notifySpindleSpeed(unsigned char pid, ArdoObj::SpindleTupleValue s) {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	
	//this notification is only meaningful as long as 
	//the template was not valid processed.
	if ( hasValidRuns() == false )
		ContextInterface::notifySpindleSpeed(pid, s);
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::notifyStepperSpeed(unsigned char pid, ArdoObj::SpeedTupleValue s) {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	
	//this notification is only meaningful as long as 
	//the template was not valid processed.
	if ( hasValidRuns() == false )
		ContextInterface::notifyStepperSpeed(pid, s);
}
//////////////////////////////////////////////////////////////
void CncTemplateContext::unregisterCncInterface() {
//////////////////////////////////////////////////////////////
	cncInterface = NULL;
	updateGui(true);
}
//////////////////////////////////////////////////////////////
bool CncTemplateContext::registerCncInterface(CncPathListInterfaceCnc* ci) {
//////////////////////////////////////////////////////////////
	if ( ci == NULL )
		return false;
		
	cncInterface = ci;
	updateGui(true);
	
	return true;
}
//////////////////////////////////////////////////////////////
bool CncTemplateContext::executeCncInterface() {
//////////////////////////////////////////////////////////////
	if ( cncInterface == NULL )
		return false;
	
	return cncInterface->spoolInstructions();
}