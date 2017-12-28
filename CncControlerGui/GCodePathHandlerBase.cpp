#include "GCodePathHandlerBase.h"

//////////////////////////////////////////////////////////////////
GCodePathHandlerBase::GCodePathHandlerBase() 
: PathHandlerBase()
, cutterCompMode(CC_OFF)
, toolLengthOffsetId(-1)
{
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
GCodePathHandlerBase::~GCodePathHandlerBase() {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
void GCodePathHandlerBase::prepareWork() {
//////////////////////////////////////////////////////////////////
	PathHandlerBase::prepareWork();
	prepareWorkImpl();
}
//////////////////////////////////////////////////////////////////
void GCodePathHandlerBase::finishWork() {
//////////////////////////////////////////////////////////////////
	PathHandlerBase::finishWork();
	finishWorkImpl();
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerBase::moveToOrigin(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	currentPos.setXYZ(0.0, 0.0, 0.0);
	return processLinearMove(false);
}
//////////////////////////////////////////////////////////////////
void GCodePathHandlerBase::updateCurrentPosition(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	if ( gcb.hasX() ) currentPos.setX(gcb.getXMoveAbsolute(currentPos));
	if ( gcb.hasY() ) currentPos.setY(gcb.getYMoveAbsolute(currentPos));
	if ( gcb.hasZ() ) currentPos.setZ(gcb.getZMoveAbsolute(currentPos));
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerBase::processRapidLinearMove(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	if ( gcb.hasOneOf_XYZ() == false )
		return true;
	
	// change to the maximum configured speed
	changeCurrentFeedSpeedXYZ(CncSpeed::CncSpeedRapid, GBL_CONFIG->getMaxSpeedXYZ_MM_MIN());
	
	updateCurrentPosition(gcb);
	return processLinearMove(false);
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerBase::processLinearMove(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	if ( gcb.hasOneOf_XYZ() == false )
		return true;
	
	changeCurrentFeedSpeedXYZ(CncSpeed::CncSpeedWork, gcb.getCurrentFeedSpeed());
	
	updateCurrentPosition(gcb);
	return processLinearMove(false);
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerBase::processArcMove(GCodeBlock& gcb, bool sweep) {
//////////////////////////////////////////////////////////////////
	double r  = sqrt(pow(gcb.i, 2) + pow(gcb.j, 2)); // todo relative vs. absolute
	double ex = gcb.x;
	double ey = gcb.y;
	char cmd  = 'A';
	
	if ( gcb.isAbsolutePositioningXYZ() == false ) {
		cmd = 'a';
		ex = currentPos.getX() + gcb.x;
		ey = currentPos.getY() + gcb.y;
	}
	
	double values[7];
	values[0] = r;
	values[1] = r;
	values[2] = 0.0;
	values[3] = 0.0;
	values[4] = ( sweep == true ? 1.0 : 0.0 );
	values[5] = ex;
	values[6] = ey;
	
	changeCurrentFeedSpeedXYZ(CncSpeed::CncSpeedWork, gcb.getCurrentFeedSpeed());
	// this renders the arc and releases the callback 
	// GCodePathHandlerBase::processLinearMove(...)
	// for each curve lib point
	return process(cmd, 7, values);
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerBase::processDwell(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	int64_t waitTimeout = 0LL;
	
	if ( gcb.hasP() && gcb.p > 0.0 ) {
		// p contains seconds
		waitTimeout = (unsigned int)(gcb.p * 1000 * 1000);
	} 
	
	if ( waitTimeout != 0LL ) {
		return processDwellIntern(waitTimeout);
	}
		
	std::cerr << "GCodePathHandlerBase::processDwell: Invalid parameter value P: " << waitTimeout << " [us]" << std::endl;
	return false;
}