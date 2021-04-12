#include <iostream>
#include <cstdlib>
#include <math.h>
#include <wx/string.h>
#include <wx/dataview.h>
#include "SerialPort.h"
#include "CncControl.h"
#include "CncContext.h"
#include "CncBoundarySpace.h"
#include "CncPathListRunner.h"
#include "MainFrameProxy.h"
#include "FileParser.h"
#include "SVGPathHandlerCnc.h"

#define CTX_ADD_SEP(msg)	APP_PROXY::parsingSynopsisTraceAddSeparator(wxString::Format("[LN: %8ld]: %s", getSvgCncContext().getCurrentLineNumber(), msg));
#define CTX_LOG_INF(msg)	APP_PROXY::parsingSynopsisTraceAddInfo     (wxString::Format("[LN: %8ld]: %s", getSvgCncContext().getCurrentLineNumber(), msg));
#define CTX_LOG_WAR(msg)	APP_PROXY::parsingSynopsisTraceAddWarning  (wxString::Format("[LN: %8ld]: %s", getSvgCncContext().getCurrentLineNumber(), msg));
#define CTX_LOG_ERR(msg)	APP_PROXY::parsingSynopsisTraceAddError    (wxString::Format("[LN: %8ld]: %s", getSvgCncContext().getCurrentLineNumber(), msg));

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
	
	// add the file parser to the runner setup
	// the rest is already set by CncPathListRunner(cnc)
	CncPathListRunner::Setup& setup = getSetup();
	setup.fileParser = fileParser;
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
bool SVGPathHandlerCnc::invertPathArgSweepFlag() const {
//////////////////////////////////////////////////////////////////
	return currentCncContext.invertPathArgSweeFlag();
}
//////////////////////////////////////////////////////////////////
void SVGPathHandlerCnc::processFeedSpeed(CncSpeedMode mode) {
//////////////////////////////////////////////////////////////////
	switch ( mode ) {
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
	
	CTX_ADD_SEP(wxString::Format("Run next Path:"));
	
	nextPath = true;
	if ( currentCncContext.isGuidePath() )	pathListMgr.initNextGuidePath(CncPathListManager::GuideType::HELP_PATH);
	else									pathListMgr.initNextCncPath();
	
	pathListMgr.normalizeLinkedEntry(clientID, CncSpeedWork, currentCncContext.getCurrentWorkSpeed_MM_MIN());
	
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
	
	bool ret			= true;
	const bool hasMod	= currentCncContext.hasPathModifications();
	CTX_LOG_INF(wxString::Format("Has Modifications    : %s",	hasMod ? "Yes" : "No"));
	
	if ( hasMod ) {
		if ( performModifications() == false )
			return false;
	}
	
	CTX_LOG_INF(wxString::Format("Initial Run Position : %s",	cnc::dblFormat(curRunPosition)));

if ( false ) {
	std::cout << std::endl;
	std::cout << "curRunPosition  : " << cnc::dblFormat(curRunPosition) << std::endl;
	std::cout << "ctl Position    : " << cnc::dblFormat(cncControl->getCurCtlPosMetric()) << std::endl;
	std::cout << "currentPos      : " << cnc::dblFormat(currentPos) << std::endl;
	std::cout << "startPos        : " << cnc::dblFormat(startPos) << std::endl;
}

	// determine current Z target position value
	const double zSureface	= THE_BOUNDS->getSurfaceOffset();
	double zTarget			= curRunPosition.getZ();
	
	switch ( currentCncContext.getCurrentZDepthMode() ) {
		case 'z':	zTarget = curRunPosition.getZ() + currentCncContext.getCurrentZDepth();
					break;
					
		case 'Z':	zTarget = zSureface + currentCncContext.getCurrentZDepth();
					break;
					
		default:	std::cerr << CNC_LOG_FUNCT_A(" Invalid ZDepth mode\n");
	}
	
	// display guide pathes on demand
	if ( guidePath ) {
		publishGuidePath(*guidePath, zSureface);
		
		if ( guidePath->getGuideType() == CncPathListManager::GuideType::ORIG_PATH )
			publishGuidePath(*guidePath, zTarget);
			
		if ( guidePath->getGuideType() != CncPathListManager::GuideType::ORIG_PATH )
			return true;
	}
	
	// perform some checks
	if ( currentCncContext.isCurrentZFeedStepToStrong() == true )
		return false;
	
	#warning zDepth - performing more checks here?
	//if ( zTarget > ....)
	
	// first, move to surface and reset the watermaks to reinitalize the Z depth
	moveZAxisToLogicalTop();
	curRunPosition.resetWatermarksToCurrentPos();
	
	// second, process path
	if ( pathListMgr.getExecRecommendation() == CncPathListManager::ExeRecomm::ER_HELIX ) {
		
		cncControl->initNextDuration();

		if ( processHelix(zTarget) == false ) {
			std::cerr << CNC_LOG_FUNCT_A(wxString::Format(": processHelix() failed! \n"));
			return false;
		}
	}
	else {
		
		unsigned int cntDurations	= 0;
		unsigned int maxDurations	= abs((zTarget - zSureface ) / currentCncContext.getCurrentZMaxFeedStep() ) + 2;
		
		do {
			cncControl->initNextDuration();
			
			ret = repeatCurrentPath(zTarget);
			if ( ret == false )
				break;
				
			// avoid endless loops
			if ( cntDurations++ > maxDurations ) {
				std::cerr << CNC_LOG_FUNCT_A(wxString::Format(": The max. amound of crossings a reached. The path will be stopped here! \n"));
				
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
	if ( onPhysicallyExecute(pathListMgr) == false )
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
	
	// spool current path
	if ( onPhysicallyExecute(pathListMgr) == false )
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
	MoveParameter mp;
	mp.idOffset = CO::START_POS;
	mp.mode		= m;
	mp.zToTop	= true;
	mp.pos		= &pathListMgr.getStartPos();
	
	const bool ret = moveXYToPos(mp);
	pathListMgr.normalizeStartPosDistance();
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::moveXYToPos(const MoveParameter& mp) {
///////////////////////////////////////////////////////////////////
	if ( mp.pos == NULL ) {
		std::cerr << CNC_LOG_FUNCT_A(": Invalid position!\n");
		return false;
	}
	
	const CncDoublePosition& pos = *mp.pos;
	
	// nothing to do in this case
	if ( isXYEqual(curRunPosition, pos) )
		return true;
	
	// first move to top if not alreay done - on demand
	if ( mp.zToTop == true ) {
		if ( moveZAxisToLogicalTop() == false )
			return false;
	}
	
	const double dx		= pos.getX() - curRunPosition.getX();
	const double dy 	= pos.getY() - curRunPosition.getY();
	const long clientID	= currentCncContext.getCurrentClientID(mp.idOffset);
	
	CncPathListEntry initialEntry;
	initialEntry.content			= CncPathListEntry::ContentCFS;
	initialEntry.pathListReference	= CncTimeFunctions::getNanoTimestamp();
	initialEntry.entryDistance		= CncPathListEntry::NoDistance;
	initialEntry.entryTarget		= curRunPosition;
	initialEntry.clientId			= clientID;
	initialEntry.feedSpeedMode		= mp.mode;
	initialEntry.feedSpeed_MM_MIN	= currentCncContext.getCurrentSpeed_MM_MIN(mp.mode);
	initialEntry.spindleState		= currentCncContext.getCurrentSpindleState();
	initialEntry.spindleSpeed_U_MIN	= currentCncContext.getCurrentSpindleSpeed_U_MIN();
	
	CncPathListManager plm(initialEntry);
	plm.addEntryRel(dx, dy, 0.0);
	
	curRunPosition += {dx, dy, 0.0};
	return onPhysicallyExecute(plm);
}
///////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::moveZAxisToLogicalTop() {
///////////////////////////////////////////////////////////////////
	const double zTopRefValue = THE_BOUNDS->getSurfaceOffset() + THE_CONFIG->getSurefaceOffset();
	
	if ( cnc::dblCompare(curRunPosition.getZ(), zTopRefValue) == false ) {
		const double zDist	= zTopRefValue - curRunPosition.getZ();
		const long clientID	= currentCncContext.getCurrentClientID(CO::Z_TO_LOGICAL_TOP);
		
		CncPathListEntry initialEntry;
		initialEntry.content			= CncPathListEntry::ContentCFS;
		initialEntry.pathListReference	= CncTimeFunctions::getNanoTimestamp();
		initialEntry.entryDistance		= CncPathListEntry::NoDistance;
		initialEntry.entryTarget		= curRunPosition;
		initialEntry.clientId			= clientID;
		initialEntry.feedSpeedMode		= CncSpeedWork;
		initialEntry.feedSpeed_MM_MIN	= currentCncContext.getCurrentWorkSpeed_MM_MIN();
		initialEntry.spindleState		= currentCncContext.getCurrentSpindleState();
		initialEntry.spindleSpeed_U_MIN	= currentCncContext.getCurrentSpindleSpeed_U_MIN();
		
		CncPathListManager plm(initialEntry);
		plm.addEntryRel(0.0, 0.0, zDist);
		
		curRunPosition += {0.0, 0.0, zDist};
		return onPhysicallyExecute(plm);
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::moveZAxisToSurface() {
///////////////////////////////////////////////////////////////////
	const double zSureface = THE_BOUNDS->getSurfaceOffset();
	
	// perform the move - on demand
	if ( cnc::dblCompare(curRunPosition.getZ(), zSureface) == false ) {
		const double zDist	= (-1) * fabs(curRunPosition.getZ() - zSureface);
		const long clientID	= currentCncContext.getCurrentClientID(CO::Z_TO_SUREFACE);
		
		CncPathListEntry initialEntry;
		initialEntry.content			= CncPathListEntry::ContentCFS;
		initialEntry.pathListReference	= CncTimeFunctions::getNanoTimestamp();
		initialEntry.entryDistance		= CncPathListEntry::NoDistance;
		initialEntry.entryTarget		= curRunPosition;
		initialEntry.clientId			= clientID;
		initialEntry.feedSpeedMode		= CncSpeedWork;
		initialEntry.feedSpeed_MM_MIN	= currentCncContext.getCurrentWorkSpeed_MM_MIN();
		initialEntry.spindleState		= currentCncContext.getCurrentSpindleState();
		initialEntry.spindleSpeed_U_MIN	= currentCncContext.getCurrentSpindleSpeed_U_MIN();
		
		CncPathListManager plm(initialEntry);
		plm.addEntryRel(0.0, 0.0, zDist);
		
		curRunPosition += {0.0, 0.0, zDist};
		return onPhysicallyExecute(plm);
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
	if ( zMin > zSureface ) {
		if ( moveZAxisToSurface() == false )
			return false;
			
		zMin = curRunPosition.getZMin();
	}
	
	// target already reached
	if ( zMin <= zTarget )
		return true;
	
	// first, initialize.  
	double curZTarget = curRunPosition.getZ();
	if ( zTarget <= 0.0 ) {
		
		// std::max because the values are negative
		if ( zMin > 0.0 )	curZTarget = std::max(0.0	- zDistRefValue, zTarget);
		else				curZTarget = std::max(zMin	- zDistRefValue, zTarget);
		
	}
	else {
		
		curZTarget = std::max(zMin - zDistRefValue, zTarget);
	}
	
	// second, perform the move
	if ( cnc::dblCompare(curRunPosition.getZ(), curZTarget) == false ) {
		const double zDist	= (-1) * fabs(curRunPosition.getZ() - curZTarget);
		const long clientID	= currentCncContext.getCurrentClientID(CO::Z_NEXT_STEP_DOWN);
		
		CncPathListEntry initialEntry;
		initialEntry.content			= CncPathListEntry::ContentCFS;
		initialEntry.pathListReference	= CncTimeFunctions::getNanoTimestamp();
		initialEntry.entryDistance		= CncPathListEntry::NoDistance;
		initialEntry.entryTarget		= curRunPosition;
		initialEntry.clientId			= clientID;
		initialEntry.feedSpeedMode		= CncSpeedWork;
		initialEntry.feedSpeed_MM_MIN	= currentCncContext.getCurrentWorkSpeed_MM_MIN();
		initialEntry.spindleState		= currentCncContext.getCurrentSpindleState();
		initialEntry.spindleSpeed_U_MIN	= currentCncContext.getCurrentSpindleSpeed_U_MIN();
		
		CncPathListManager plm(initialEntry);
		plm.addEntryRel(0.0, 0.0, zDist);
		
		curRunPosition += {0.0, 0.0, zDist};
		return onPhysicallyExecute(plm);
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::prepareWork() {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL(CNC_LOG_FUNCT);
	
	if ( PathHandlerBase::prepareWork() == false ) {
		std::cerr << CNC_LOG_FUNCT_A(": PathHandlerBase::prepareWork() failed!\n");
		return false;
	}
	
	currentPos.resetWatermarks();
	startPos.resetWatermarks();
	
	// align again
	curRunPosition.set(cncControl->getCurCtlPosMetric());
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerCnc::finishWork() {
//////////////////////////////////////////////////////////////////
	TRACE_FUNCTION_CALL(CNC_LOG_FUNCT);
	
	if ( PathHandlerBase::finishWork() == false )
		std::cerr << CNC_LOG_FUNCT_A(": PathHandlerBase::finishWork() failed!\n");
	
	CncDoublePosition p(0.0, 0.0, 0.0);
	MoveParameter mp;
	mp.idOffset = CO::START_POS;
	mp.mode		= CncSpeedRapid;
	mp.zToTop	= true;
	mp.pos		= &p;
	
	const bool ret = moveXYToPos(mp);
	
	//svg output handling only
	if ( ret == true ) {
		CncDoublePosition::Watermarks xyMax;
		xyMax = cncControl->getWaterMarksMetric();
	}
	
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
		case CncPM_Center:
		case CncPM_None:
		{
			// nothing to do here
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

