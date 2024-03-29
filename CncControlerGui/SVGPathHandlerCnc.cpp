#include <iostream>
#include <cstdlib>
#include <math.h>
#include <wx/string.h>
#include <wx/dataview.h>
#include "SerialPort.h"
#include "CncControl.h"
#include "CncContext.h"
#include "CncBoundarySpace.h"
#include "CncAnchorInfo.h"
#include "CncPathListRunner.h"
#include "GlobalFunctions.h"
#include "MainFrame.h"
#include "FileParser.h"
#include "SVGPathHandlerCnc.h"

//////////////////////////////////////////////////////////////////

#define CTX_ADD_SEP(msg)	THE_CONTEXT->addParsingSynopisSeparator(wxString::Format("[LINE: %8ld]: %s\n", getSvgCncContext().getCurrentLineNumber(), msg));
#define CTX_LOG_INF(msg)	THE_CONTEXT->addParsingSynopisInfo     (wxString::Format("[LINE: %8ld]: %s\n", getSvgCncContext().getCurrentLineNumber(), msg));
#define CTX_LOG_WAR(msg)	THE_CONTEXT->addParsingSynopisWarning  (wxString::Format("[LINE: %8ld]: %s\n", getSvgCncContext().getCurrentLineNumber(), msg));
#define CTX_LOG_ERR(msg)	THE_CONTEXT->addParsingSynopisError    (wxString::Format("[LINE: %8ld]: %s\n", getSvgCncContext().getCurrentLineNumber(), msg));

//////////////////////////////////////////////////////////////////
class CncPathListEntry;

SVGPathHandlerCnc::SVGPathHandlerCnc(CncControl* cnc) 
: SVGPathHandlerBase	()
, CncPathListRunner		(cnc)
, cncControl			(cnc)
, guidePath				(NULL)
, curRunPosition		({0.0, 0.0, 0.0})
, initialized			(false)
, debugState			(false)
{
//////////////////////////////////////////////////////////////////
	wxASSERT(cncControl);
}
//////////////////////////////////////////////////////////////////
SVGPathHandlerCnc::~SVGPathHandlerCnc() {
//////////////////////////////////////////////////////////////////
	resetGuidePath();
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::resetGuidePath() {
//////////////////////////////////////////////////////////////////
	wxDELETE( guidePath );
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::registerGuidePath(CncPathListManager* gp) {
//////////////////////////////////////////////////////////////////
	resetGuidePath();
	guidePath = gp;
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::logMeasurementStart() {
//////////////////////////////////////////////////////////////////
	CncPathListRunner::logMeasurementStart();
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::logMeasurementEnd() {
//////////////////////////////////////////////////////////////////
	CncPathListRunner::logMeasurementEnd();
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::initNextClientId(long id) {
//////////////////////////////////////////////////////////////////
	processClientId(id);
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::isXYEqual(const CncDoublePosition& p1, const CncDoublePosition& p2) const {
//////////////////////////////////////////////////////////////////
	const bool xc = std::fabs(p1.getX() - p2.getX()) <= 0.0001;
	const bool yc = std::fabs(p1.getY() - p2.getY()) <= 0.0001;
	
	return xc && yc;
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::updateXY(const CncDoublePosition& p1, CncDoublePosition& p2) const {
//////////////////////////////////////////////////////////////////
	p2.setX(p1.getX());
	p2.setY(p1.getY());
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::updateZ(const CncDoublePosition& p1, CncDoublePosition& p2) const {
//////////////////////////////////////////////////////////////////
	p2.setZ(p1.getZ());
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::appendDebugValueDetail(const char* key, wxVariant value) {
//////////////////////////////////////////////////////////////////
	if ( debugState == false )
		return;

	if ( fileParser == NULL )
		return;
		
	DcmRow row;
	row.push_back(wxString(key));
	row.push_back(value.GetString());
	fileParser->appendDebugValueDetail(row);
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::setSvgRootNode(const SVGRootNode& srn) {
//////////////////////////////////////////////////////////////////
	SVGPathHandlerBase::setSvgRootNode(srn);
	changeInputUnit(srn.getInputUnit());
	initialized = true;
}
//////////////////////////////////////////////////////////////////
inline void SVGPathHandlerCnc::appendDebugValueDetail(const CncPathListEntry& cpe) {
//////////////////////////////////////////////////////////////////
	if ( debugState == false )
		return;

	wxString value;
	appendDebugValueDetail((wxString("Point ") << pathListMgr.getPathListSize()), cpe.traceEntryToString(value));
}
//////////////////////////////////////////////////////////////////
inline void SVGPathHandlerCnc::appendDebugValueDetail(const CncCurveLib::ParameterSet& ps) {
//////////////////////////////////////////////////////////////////
	if ( debugState == false )
		return;
	
	appendDebugValueDetail("Curve length", 		wxString::Format("%.5lf", 	ps.RI.curveLength));
	appendDebugValueDetail("Render samples",	wxString::Format("%u", 		ps.RI.samples));
//	appendDebugValueDetail("Render steps",		wxString::Format("%u", 		ps.RI.steps));
	appendDebugValueDetail("Render increment",	wxString::Format("%.5lf", 	ps.RI.increment));
	appendDebugValueDetail("Render resolution",	wxString::Format("%.5lf", 	ps.RI.resolution));
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::finishCurrentPath() {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL(CNC_LOG_FUNCT);
	return PathHandlerBase::finishCurrentPath();
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::convertToRightHandCoordSystem() const {
//////////////////////////////////////////////////////////////////
	return THE_CONFIG->getSvgConvertToRightHandFlag();
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::invertPathArgSweepFlag() const {
//////////////////////////////////////////////////////////////////
	return currentCncContext.invertPathArgSweeFlag();
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::processFeedSpeed(CncSpeedMode mode) {
//////////////////////////////////////////////////////////////////
	switch ( mode ) 
	{
		case CncSpeedRapid:	pathListMgr.addEntryAdm(mode, currentCncContext.getCurrentRapidSpeed_MM_MIN());
							break;
							
		case CncSpeedWork:	pathListMgr.addEntryAdm(mode, currentCncContext.getCurrentWorkSpeed_MM_MIN());
							break;
							
		default:			std::cerr << CNC_LOG_FUNCT_A(": Invalid speed mode\n");
	}
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::activateNextPath(long clientID) {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL(CNC_LOG_FUNCT);
	
	nextPath = true;
	pathCounter++;
	
	CTX_ADD_SEP(wxString::Format("Run next Path(%u): ", pathCounter));
	
	if ( currentCncContext.isGuidePath() )
	{
		if ( currentCncContext.isZeroPosPath() )	pathListMgr.initNextGuidePath(CncPathListManager::GuideType::REFPOS_PATH);
		else										pathListMgr.initNextGuidePath(CncPathListManager::GuideType::HELP_PATH);
	}
	else
	{
		pathListMgr.initNextCncPath();
	}
	
	CncPathListEntry reference;
	reference.clientId				= clientID;
	reference.feedSpeedMode			= CncSpeedWork;
	reference.feedSpeed_MM_MIN		= currentCncContext.getCurrentWorkSpeed_MM_MIN();
	reference.spindleState			= currentCncContext.getCurrentSpindleState();
	reference.spindleSpeed_U_MIN	= currentCncContext.getCurrentSpindleSpeed_U_MIN();
	
	pathListMgr.normalizeLinkedEntry(reference);
	
	const Trigger::NextPath tr;
	processTrigger(tr);
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::runCurrentPath() {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL(CNC_LOG_FUNCT);
	
	// the guide path has always to be reset here, otherwise a deprecated one will be used below.
	// performModifications() will generate a new one on demand.
	resetGuidePath();
	
	const bool hasMod = currentCncContext.hasPathModifications();
	
	CTX_LOG_INF(wxString::Format("Has Modifications    : %s",	hasMod ? "Yes" : "No"));
	
	if ( hasMod )
	{
		if ( performModifications() == false )
			return false;
	}
	
	CTX_LOG_INF(wxString::Format("Initial Run Position : %s",	cnc::dblFormat(curRunPosition)));

	if ( false )
	{
		std::cout << std::endl;
		std::cout << "curRunPosition  : " << cnc::dblFormat(curRunPosition) << std::endl;
		std::cout << "ctl Position    : " << cnc::dblFormat(cncControl->getCurCtlPosMetric()) << std::endl;
		std::cout << "currentPos      : " << cnc::dblFormat(currentPos) << std::endl;
		std::cout << "startPos        : " << cnc::dblFormat(startPos) << std::endl;
	}
	
	// -------------------------------------------------------------
	// determine current Z target position value
	const double zSurface	= THE_BOUNDS->getSurfaceOffset();
	double zTarget			= curRunPosition.getZ();
	
	switch ( currentCncContext.getCurrentZDepthMode() )
	{
		case 'z':	zTarget = curRunPosition.getZ() + currentCncContext.getCurrentZDepth();
					break;
					
		case 'Z':	zTarget = zSurface + currentCncContext.getCurrentZDepth();
					break;
					
		default:	std::cerr << CNC_LOG_FUNCT_A(" Invalid ZDepth mode\n");
	}
	
	// -------------------------------------------------------------
	// display extra guide paths (e. g. for inner outer path modification)
	// on demand
	if ( guidePath )
	{
		// publish the zero ref triangle on demand ...
		if ( currentCncContext.isZeroPosPath() )
		{
			processGuidePath(*guidePath, zSurface);
			//... and skip the "original one"
			return true;
		}
		
		// this will publish the guide path in general on the upper surface
		processGuidePath(*guidePath, zSurface);
		
		// for the original ones do it ones more on the lower surface
		// otherwise return here which will only display the guide path
		// and skip the "original one"
		const bool b = guidePath->getGuideType() == CncPathListManager::GuideType::ORIG_PATH;
		if ( b )	processGuidePath(*guidePath, zTarget);
		else		return true;
	}
	
	// perform some checks
	if ( currentCncContext.isCurrentZFeedStepToStrong() == true )
		return false;
	
	#warning zDepth - performing more checks here?
	//if ( zTarget > ....)
	
	// first, move to surface and reset the watermarks to re-initialize the Z depth
	moveZAxisToLogicalTop();
	curRunPosition.resetWatermarksToCurrentPos();
	
	// second, process path
	bool ret = true;
	if ( pathListMgr.getExecRecommendation() == CncPathListManager::ExeRecomm::ER_HELIX )
	{
		cncControl->initNextDuration();

		if ( processHelix(zTarget) == false )
		{
			std::cerr << CNC_LOG_FUNCT_A(wxString::Format(": processHelix() failed! \n"));
			return false;
		}
	}
	else 
	{
		unsigned int cntDurations	= 0;
		unsigned int maxDurations	= abs((zTarget - zSurface ) / currentCncContext.getCurrentZMaxFeedStep() ) + 2;
		
		do
		{
			cncControl->initNextDuration();
			
			ret = repeatCurrentPath(zTarget);
			if ( ret == false )
				break;
				
			// avoid endless loops
			if ( cntDurations++ > maxDurations ) {
				std::cerr << CNC_LOG_FUNCT_A(wxString::Format(": The max. amount of crossings a reached. The path will be stopped here! \n"));
				
				ret = false;
				break;
			}
			
		} while ( hasMoreDurations(zTarget) );
		
	}
	
	// path is finished now, reset the duration counter
	cncControl->resetDurationCounter();
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::hasMoreDurations(double zTarget) const {
///////////////////////////////////////////////////////////////////
	const double zMin	= curRunPosition.getZMin();
	const double zTrg	= zTarget;
	
	//std::cout << CNC_LOG_FUNCT << ": " << zMin << ", " << zTrg << std::endl;
	return cnc::dblCmp::gt(zMin, zTrg);
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::processHelix(double zTarget) {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL(CNC_LOG_FUNCT);
	
	// first move to start position if necessary
	if ( moveXYToStartPos(CncSpeedRapid) == false )
		return false;
		
	// move down to surface
	if ( moveZAxisToSurface() == false )
		return false;
	
	// modify path to a helix
	if ( pathListMgr.processXYHelix(curRunPosition.getZ(), zTarget, currentCncContext.getCurrentZMaxFeedStep()) == false )
		return false;
		
	// spool current path
	if ( processCncPath(pathListMgr) == false )
		return false;
		
	// update position information
	CncDoublePosition p;
	if ( pathListMgr.getTargetPos(p) )
		updateXY(p, curRunPosition);
		
	// here also update the z axis
	curRunPosition.setZ(zTarget);
	
	if ( moveZAxisToLogicalTop() == false )
		return false;
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::repeatCurrentPath(double zTarget) {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL(CNC_LOG_FUNCT);
	
	// first move to start position if necessary
	if ( moveXYToStartPos(CncSpeedRapid) == false )
		return false;
	
	// second if the start pos is reached move the next Z step down
	if ( moveZAxisNextStepDown(zTarget) == false )
		return false;
	
	// update the current depth
	if ( pathListMgr.adjustZOffsetAbs(curRunPosition.getZ()) == false )
		return false;
	
	//  current path
	if ( processCncPath(pathListMgr) == false )
		return false;
		
	// update position information
	CncDoublePosition p;
	if ( pathListMgr.getTargetPos(p) )
		updateXY(p, curRunPosition);
	
	// if the path isn't closed ...
	if ( isXYEqual(curRunPosition, pathListMgr.getStartPos()) == false ) {
		
		// move to top only if the path isn't closed
		if ( moveZAxisToLogicalTop() == false )
			return false;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::moveXYToStartPos(CncSpeedMode m) {
///////////////////////////////////////////////////////////////////
	// first move to top if not already done - on demand
	if ( moveZAxisToLogicalTop(m) == false )
		return false;

	MoveParameter mp;
	mp.idOffset = CO::START_POS;
	mp.mode		= m;
	mp.pos		= pathListMgr.getStartPos();
	
	const bool ret = moveXYToPos(mp);
	pathListMgr.normalizeStartPosDistance();
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::moveXYToPos(const MoveParameter& mp) {
///////////////////////////////////////////////////////////////////
	const CncDoublePosition& pos = mp.pos;
	
	// nothing to do in this case
	if ( isXYEqual(curRunPosition, pos) )
		return true;
	
	const long clientId	= currentCncContext.getCurrentClientID(mp.idOffset);
	const bool tc       = initToolChange(currentCncContext.getCurrentToolId());
	const int content   = tc ? CncPathListEntry::ContentCFST : CncPathListEntry::ContentCFS;
	
	const double dx		= pos.getX() - curRunPosition.getX();
	const double dy 	= pos.getY() - curRunPosition.getY();
	
	CncPathListEntry initialEntry;
	initialEntry.content			= content;
	initialEntry.pathListReference	= CncTimeFunctions::getNanoTimestamp();
	initialEntry.entryDistance		= CncPathListEntry::NoDistance;
	initialEntry.entryTarget		= curRunPosition;
	initialEntry.clientId			= clientId;
	initialEntry.toolId				= currentCncContext.getCurrentToolId();
	initialEntry.feedSpeedMode		= mp.mode;
	initialEntry.feedSpeed_MM_MIN	= currentCncContext.getCurrentSpeed_MM_MIN(mp.mode);
	initialEntry.spindleState		= currentCncContext.getCurrentSpindleState();
	initialEntry.spindleSpeed_U_MIN	= currentCncContext.getCurrentSpindleSpeed_U_MIN();
	
	CncPathListManager plm(initialEntry);
	plm.addEntryRel(dx, dy, 0.0);
	
	curRunPosition += {dx, dy, 0.0};
	return processCncPath(plm);
}
///////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::moveXYZToPos(const MoveParameter& mp) {
///////////////////////////////////////////////////////////////////
	const CncDoublePosition& pos = mp.pos;

	if ( moveZAxisToLogicalPos(mp) == false ) 
	{
		CNC_CERR_FUNCT_A(": moveZAxisToLogicalPos() failed!\n")
		return false;
	}
	
	// nothing to do in this case
	if ( isXYEqual(curRunPosition, pos) )
		return true;
	
	const long clientId	= currentCncContext.getCurrentClientID(mp.idOffset);
	const bool tc       = initToolChange(currentCncContext.getCurrentToolId());
	const int content   = tc ? CncPathListEntry::ContentCFST : CncPathListEntry::ContentCFS;
	
	const double dx		= pos.getX() - curRunPosition.getX();
	const double dy 	= pos.getY() - curRunPosition.getY();
	
	CncPathListEntry initialEntry;
	initialEntry.content			= content;
	initialEntry.pathListReference	= CncTimeFunctions::getNanoTimestamp();
	initialEntry.entryDistance		= CncPathListEntry::NoDistance;
	initialEntry.entryTarget		= curRunPosition;
	initialEntry.clientId			= clientId;
	initialEntry.toolId				= currentCncContext.getCurrentToolId();
	initialEntry.feedSpeedMode		= mp.mode;
	initialEntry.feedSpeed_MM_MIN	= currentCncContext.getCurrentSpeed_MM_MIN(mp.mode);
	initialEntry.spindleState		= currentCncContext.getCurrentSpindleState();
	initialEntry.spindleSpeed_U_MIN	= currentCncContext.getCurrentSpindleSpeed_U_MIN();
	
	CncPathListManager plm(initialEntry);
	plm.addEntryRel(dx, dy, 0.0);
	
	curRunPosition += {dx, dy, 0.0};
	return processCncPath(plm);
}
///////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::moveZAxisToLogicalPos(const MoveParameter& mp) {
///////////////////////////////////////////////////////////////////
	const double zPos = mp.pos.getZ();
	
	if ( cnc::dblCompare(curRunPosition.getZ(), zPos) == false )
	{
		const long clientId	= currentCncContext.getCurrentClientID(mp.idOffset);
		const bool tc       = initToolChange(currentCncContext.getCurrentToolId());
		const int content   = tc ? CncPathListEntry::ContentCFST : CncPathListEntry::ContentCFS;
		
		const double zDist	= zPos - curRunPosition.getZ();
		
		CncPathListEntry initialEntry;
		initialEntry.content			= content;
		initialEntry.pathListReference	= CncTimeFunctions::getNanoTimestamp();
		initialEntry.entryDistance		= CncPathListEntry::NoDistance;
		initialEntry.entryTarget		= curRunPosition;
		initialEntry.clientId			= clientId;
		initialEntry.toolId				= currentCncContext.getCurrentToolId();
		initialEntry.feedSpeedMode		= mp.mode;
		initialEntry.feedSpeed_MM_MIN	= currentCncContext.getCurrentSpeed_MM_MIN(mp.mode);
		initialEntry.spindleState		= currentCncContext.getCurrentSpindleState();
		initialEntry.spindleSpeed_U_MIN	= currentCncContext.getCurrentSpindleSpeed_U_MIN();
		
		CncPathListManager plm(initialEntry);
		plm.addEntryRel(0.0, 0.0, zDist);
		
		curRunPosition += {0.0, 0.0, zDist};
		return processCncPath(plm);
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::moveZAxisToLogicalTop(CncSpeedMode m) {
///////////////////////////////////////////////////////////////////
	const double zTopRefValue = THE_BOUNDS->getSurfaceOffset() + THE_CONFIG->getSurefaceOffset();
	
	MoveParameter mp;
	mp.idOffset = CO::Z_TO_LOGICAL_TOP;
	mp.mode		= m;
	mp.pos		= { curRunPosition.getX(), curRunPosition.getY(), zTopRefValue};
	
	return moveZAxisToLogicalPos(mp);
}
///////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::moveZAxisToSurface() {
///////////////////////////////////////////////////////////////////
	const double zSureface = THE_BOUNDS->getSurfaceOffset();
	
	// perform the move - on demand
	if ( cnc::dblCompare(curRunPosition.getZ(), zSureface) == false )
	{
		const long clientId	= currentCncContext.getCurrentClientID(CO::Z_TO_SUREFACE);
		const bool tc       = initToolChange(currentCncContext.getCurrentToolId());
		const int content   = tc ? CncPathListEntry::ContentCFST : CncPathListEntry::ContentCFS;

		const double zDist	= (-1) * fabs(curRunPosition.getZ() - zSureface);
		
		CncPathListEntry initialEntry;
		initialEntry.content			= content;
		initialEntry.pathListReference	= CncTimeFunctions::getNanoTimestamp();
		initialEntry.entryDistance		= CncPathListEntry::NoDistance;
		initialEntry.entryTarget		= curRunPosition;
		initialEntry.clientId			= clientId;
		initialEntry.toolId				= currentCncContext.getCurrentToolId();;
		initialEntry.feedSpeedMode		= CncSpeedWork;
		initialEntry.feedSpeed_MM_MIN	= currentCncContext.getCurrentWorkSpeed_MM_MIN();
		initialEntry.spindleState		= currentCncContext.getCurrentSpindleState();
		initialEntry.spindleSpeed_U_MIN	= currentCncContext.getCurrentSpindleSpeed_U_MIN();
		
		CncPathListManager plm(initialEntry);
		plm.addEntryRel(0.0, 0.0, zDist);
		
		curRunPosition += {0.0, 0.0, zDist};
		return processCncPath(plm);
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::moveZAxisNextStepDown(double zTarget) {
///////////////////////////////////////////////////////////////////
	const double zSureface		= THE_BOUNDS->getSurfaceOffset();
	const double zDistRefValue	= currentCncContext.getCurrentZMaxFeedStep();
	double zMin					= curRunPosition.getZMin();
	
	// to avoid cycles as "air-numbers" above the surface
	if ( zMin > zSureface ) 
	{
		if ( moveZAxisToSurface() == false )
			return false;
			
		zMin = curRunPosition.getZMin();
	}
	
	// target already reached
	if ( zMin <= zTarget )
		return true;
	
	// first, initialize.  
	double curZTarget = curRunPosition.getZ();
	if ( zTarget <= 0.0 ) 
	{
		// std::max because the values are negative
		if ( zMin > 0.0 )	curZTarget = std::max(0.0	- zDistRefValue, zTarget);
		else				curZTarget = std::max(zMin	- zDistRefValue, zTarget);
	}
	else
	{
		curZTarget = std::max(zMin - zDistRefValue, zTarget);
	}
	
	// second, perform the move
	if ( cnc::dblCompare(curRunPosition.getZ(), curZTarget) == false )
	{
		const long clientId	= currentCncContext.getCurrentClientID(CO::Z_NEXT_STEP_DOWN);
		const bool tc       = initToolChange(currentCncContext.getCurrentToolId());
		const int content   = tc ? CncPathListEntry::ContentCFST : CncPathListEntry::ContentCFS;

		const double zDist	= (-1) * fabs(curRunPosition.getZ() - curZTarget);
		
		CncPathListEntry initialEntry;
		initialEntry.content			= content;
		initialEntry.pathListReference	= CncTimeFunctions::getNanoTimestamp();
		initialEntry.entryDistance		= CncPathListEntry::NoDistance;
		initialEntry.entryTarget		= curRunPosition;
		initialEntry.clientId			= clientId;
		initialEntry.toolId				= currentCncContext.getCurrentToolId();;
		initialEntry.feedSpeedMode		= CncSpeedWork;
		initialEntry.feedSpeed_MM_MIN	= currentCncContext.getCurrentWorkSpeed_MM_MIN();
		initialEntry.spindleState		= currentCncContext.getCurrentSpindleState();
		initialEntry.spindleSpeed_U_MIN	= currentCncContext.getCurrentSpindleSpeed_U_MIN();
		
		CncPathListManager plm(initialEntry);
		plm.addEntryRel(0.0, 0.0, zDist);
		
		curRunPosition += {0.0, 0.0, zDist};
		return processCncPath(plm);
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::prepareWork() {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL(CNC_LOG_FUNCT);
	
	if ( PathHandlerBase::prepareWork() == false ) 
	{
		std::cerr << CNC_LOG_FUNCT_A(": PathHandlerBase::prepareWork() failed!\n");
		return false;
	}
	
	// the rest of the setup is already set by CncPathListRunner::autoSetup(...)
	CncPathListRunner::Setup& setup	= getSetup();
	setup.optAnalyse				= THE_CONFIG->getPreProcessorAnalyseFlag();
	setup.fileParser				= fileParser;
	
	currentPos.resetWatermarks();
	startPos.resetWatermarks();
	
	// align again
	#warning align yes or no
	//curRunPosition.set(cncControl->getCurCtlPosMetric());
	curRunPosition.zeroXYZ();
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::initToolChange(int id) {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL(CNC_LOG_FUNCT);

	if ( PathHandlerBase::initToolChange(id) == false )
	{
		// id didn't changed
		return false;
	}
	
	// not necessary to do this because the tool change was already included
	// be an other pathListMgr entry
	//pathListMgr.addEntryToC(id);
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::finishWork() {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL(CNC_LOG_FUNCT);
	
	if ( PathHandlerBase::finishWork() == false )
		std::cerr << CNC_LOG_FUNCT_A(": PathHandlerBase::finishWork() failed!\n");
	
	// Move to end position:
	MoveParameter mp;
	mp.idOffset = CO::FINALIZE_TEMPLATE;
	mp.mode		= CncSpeedRapid;
	mp.pos		= THE_CONTEXT->anchorMap->getLogicalAnchorParking();
	
	// move to parking position
	const bool ret = moveXYZToPos(mp);
	return ret;
}
///////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::performModifications() {
///////////////////////////////////////////////////////////////////
	// tool modification
	const double toolDiameter			= currentCncContext.getCurrentToolDiameter();
	const CncPathModificationType pmt	= currentCncContext.getPathModificationType();
	
	CTX_LOG_INF(wxString::Format("Tool Diameter        : %.3lf",	toolDiameter));
	CTX_LOG_INF(wxString::Format("PathModification     : %s",		currentCncContext.getPathModificationTypeAsStr()));
	CTX_LOG_INF(wxString::Format("PathRule             : %s",		currentCncContext.getPathRuleTypeAsStr()));
	
	switch ( pmt ) {
		// --------------------------------------------------------
		case CncPM_Pocket:
		{
			// first create a guide path based on the current path
			registerGuidePath(new CncPathListManager(pathListMgr));
			guidePath->changeToGuideType();
			
			// if this did not work the further code also fails. 
			// To avoid double error messages stop already here
			if ( guidePath->roundXYCorners(toolDiameter)  == false )
				return false;
				
			// then create the pocket
			if ( pathListMgr.processXYPocket(toolDiameter) == false )
				return false;
			
			break;
		}
		// --------------------------------------------------------
		case CncPM_Inner:
		{
			// first create a guide path based on the current path
			registerGuidePath(new CncPathListManager(pathListMgr));
			guidePath->changeToGuideType();
			
			// if this did not work the further code also fails. 
			// To avoid double error messages stop already here
			if ( guidePath->roundXYCorners(toolDiameter)  == false )
				return false;
			
			// then correct the current path
			const double offset = toolDiameter / 2.0;
			if ( pathListMgr.processXYInnerOffset(offset) == false )
				return false;
			
			break;
		}
		// --------------------------------------------------------
		case CncPM_Outer:
		{
			// first create a guide path based on the current path
			registerGuidePath(new CncPathListManager(pathListMgr));
			guidePath->changeToGuideType();
			
			// then correct the current path
			const double offset = toolDiameter / 2.0;
			if ( pathListMgr.processXYOuterOffset(offset) == false )
				return false;
			
			break;
		}
		// --------------------------------------------------------
		case CncPM_Guide:
		{
			// create a guide path based on the current path
			registerGuidePath(new CncPathListManager(pathListMgr));
			guidePath->changeToGuideType(CncPathListManager::HELP_PATH);
			
			break;
		}
		// --------------------------------------------------------
		case CncPM_ZeroRef:
		{
			// check the zero reference pos conditions
			CncDoublePosition zeroRefPos;
			if ( pathListMgr.isRightTriangle(zeroRefPos) )
			{
				// create a guide path based on the current path
				registerGuidePath(new CncPathListManager(pathListMgr));
				guidePath->changeToGuideType(CncPathListManager::GuideType::REFPOS_PATH);
				
				CncDoubleDistance offset(zeroRefPos);
				offset.invertX();
				offset.invertY();
				
				// modify path list runner transformation matrix
				setTranslation(offset);
			}
			else
			{
				// otherwise reset the path modification type 
				// CncPM_Center see this path as normal cnc path
				currentCncContext.setPathModification(CncPM_None);
			}
			
			break;
		}
		// --------------------------------------------------------
		case CncPM_Center:
		case CncPM_None:
		{
			// nothing to do here. Furthermore, normally for these cases 
			// the function isn't called  . . . 
			break;
		}
	}
	
	const CncPathRuleType prt	= currentCncContext.getPathRuleType();
	switch ( prt ) {
		// --------------------------------------------------------
		case CncPR_EnsureClockwise:
		{
			if ( pathListMgr.ensureDirection(CncClockwise) == false )
				return false;
				
			break;
		}
		// --------------------------------------------------------
		case CncPR_EnsureCounterClockwise:
		{
			if ( pathListMgr.ensureDirection(CncCounterClockwise) == false )
				return false;
				
			break;
		}
		// --------------------------------------------------------
		case CncPR_Reverse:
		{
			if ( pathListMgr.reversePath() == false )
				return false;
				
			break;
		}
		// --------------------------------------------------------
		case CncPR_None:
		{
			// nothing to do here
		}
		
	}
	
	return true;
}

