#include "GCodePathHandlerCnc.h"

//////////////////////////////////////////////////////////////////
GCodePathHandlerCnc::GCodePathHandlerCnc(CncControl* cnc) 
: PathHandlerBase()
, cncControl(cnc)
{
//////////////////////////////////////////////////////////////////
	wxASSERT(cncControl);
}
//////////////////////////////////////////////////////////////////
GCodePathHandlerCnc::~GCodePathHandlerCnc() {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
void GCodePathHandlerCnc::prepareWork() {
//////////////////////////////////////////////////////////////////
	PathHandlerBase::prepareWork();
}
//////////////////////////////////////////////////////////////////
void GCodePathHandlerCnc::finishWork() {
//////////////////////////////////////////////////////////////////
	PathHandlerBase::finishWork();
	wxASSERT(cncControl);
	
	currentPos.setXYZ(0.0, 0.0, 0.0);
	cncControl->moveXYZToZeroPos();
	
	cncControl->switchToolOff();
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerCnc::moveToOrigin(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	wxASSERT(cncControl);
	bool ret = cncControl->moveRelLinearMetricXYZ(0.0 - currentPos.getX(), 
	                                              0.0 - currentPos.getY(),
	                                              0.0 - currentPos.getZ(),
	                                              false);
	if ( ret == true )
		currentPos.setXYZ(0.0, 0.0, 0.0);
		
	return ret;
}
//////////////////////////////////////////////////////////////////
void GCodePathHandlerCnc::updateCurrentPosition(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	if ( gcb.hasX() ) currentPos.setX(gcb.getXMoveAbsolute(currentPos));
	if ( gcb.hasY() ) currentPos.setY(gcb.getYMoveAbsolute(currentPos));
	if ( gcb.hasZ() ) currentPos.setZ(gcb.getZMoveAbsolute(currentPos));
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerCnc::processLinearMove(bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	//std::clog << currentPos << std::endl;
	
	
	
	// todo - consider unit!
	return cncControl->moveAbsLinearMetricXYZ(currentPos.getX(), currentPos.getY(), currentPos.getZ(), alreadyRendered);
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerCnc::processRapidLinearMove(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	if ( gcb.hasOneOf_XYZ() == false )
		return true;
				
	updateCurrentPosition(gcb);
	if ( processLinearMove(false) ) 
		return true;
	
	return false;
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerCnc::processLinearMove(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	//todo - differ rapid vs. non rapid
	return processRapidLinearMove(gcb);
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerCnc::processArcMove(GCodeBlock& gcb, bool sweep) {
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
	
	// this renders the arc and releases the callback 
	// GCodePathHandlerCnc::processLinearMove(...)
	// for each curve lib point
	return processARC(cmd, 7, values);
}