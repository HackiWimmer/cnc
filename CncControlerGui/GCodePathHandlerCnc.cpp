#include "CncControl.h"
#include "CncPosition.h"
#include "GCodePathHandlerCnc.h"

//////////////////////////////////////////////////////////////////
GCodePathHandlerCnc::GCodePathHandlerCnc(CncControl* cnc) 
: GCodePathHandlerBase	()
, CncPathListRunner		(cnc)
{
//////////////////////////////////////////////////////////////////
	changeInputUnit(Unit::mm);
}
//////////////////////////////////////////////////////////////////
GCodePathHandlerCnc::~GCodePathHandlerCnc() {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerCnc::prepareWorkImpl() {
//////////////////////////////////////////////////////////////////
	const bool b  = THE_CONFIG->getPreProcessorAnalyseFlag() 
				 && THE_CONFIG->getPreProcessorSkipGCodeFlag() == false
	;
	
	// the rest of the setup is already set by CncPathListRunner::autoSetup(...)
	CncPathListRunner::Setup& setup = getSetup();
	setup.optAnalyse				= b;
	setup.fileParser				= fileParser;

	return true;
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerCnc::finishWorkImpl() {
//////////////////////////////////////////////////////////////////
	// execute the the last movement
	return initNextPath();
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerCnc::initNextPath() {
//////////////////////////////////////////////////////////////////
	// execute already existing path list
	if ( processCncPath(pathListMgr) == false )
		return false;

	// default processing
	if ( PathHandlerBase::initNextPath() == false )
		return false;

	const Trigger::NextPath tr;
	processTrigger(tr);
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerCnc::processDwellIntern(int64_t microseconds) {
//////////////////////////////////////////////////////////////////
	processWait(microseconds);
	return true;
}
//////////////////////////////////////////////////////////////////
void GCodePathHandlerCnc::initNextClientId(long id) {
//////////////////////////////////////////////////////////////////
	processClientId(id);
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerCnc::changeCurrentFeedSpeedXYZ(CncSpeedMode s, double value) {
//////////////////////////////////////////////////////////////////
	processFeedSpeed(s, value);
	return true;
}
//////////////////////////////////////////////////////////////////
void GCodePathHandlerCnc::switchSpindleState(bool state) {
//////////////////////////////////////////////////////////////////
	processSpindleState(state);
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerCnc::changeCurrentSpindleSpeed(double value) {
//////////////////////////////////////////////////////////////////
	processSpindleSpeed(value);
	return true;
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerCnc::processLinearMove(bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	const CncPathListEntry& cpe = pathListMgr.addEntryAbs(currentPos.getX(), currentPos.getY(), currentPos.getZ(), alreadyRendered);
	logNextPathListEntry(cpe);
	
	return true;
}
