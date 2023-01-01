#include "CncCurveLib.h"
#include "GCodePathHandlerBase.h"

//////////////////////////////////////////////////////////////////
GCodePathHandlerBase::GCodePathHandlerBase() 
: PathHandlerBase()
, cutterCompMode		(CC_OFF)
, toolLengthOffsetId	(-1)
{
//////////////////////////////////////////////////////////////////
	changeInputUnit(Unit::mm, false);
}
//////////////////////////////////////////////////////////////////
GCodePathHandlerBase::~GCodePathHandlerBase() {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerBase::prepareWork() {
//////////////////////////////////////////////////////////////////
	if ( PathHandlerBase::prepareWork() == false)
		std::cerr << CNC_LOG_FUNCT_A(": PathHandlerBase::prepareWork() failed!\n");
	
	return prepareWorkImpl();
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerBase::finishWork() {
//////////////////////////////////////////////////////////////////
	if ( PathHandlerBase::finishWork() == false)
		std::cerr << CNC_LOG_FUNCT_A(": PathHandlerBase::finishWork() failed!\n");
		
	return finishWorkImpl();
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerBase::moveToOrigin(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	const CncDoublePosition prev(currentPos);
	currentPos.setXYZ(0.0, 0.0, 0.0);
	
	const bool ret = processLinearMove(false);
	if ( ret == false )
		currentPos.set(prev);
		
	return ret;
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerBase::updateCurrentPosition(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	const CncDoublePosition ref(currentPos);
	
	if ( gcb.hasX() ) currentPos.setX(gcb.getXMoveAbsolute(currentPos));
	if ( gcb.hasY() ) currentPos.setY(gcb.getYMoveAbsolute(currentPos));
	if ( gcb.hasZ() ) currentPos.setZ(gcb.getZMoveAbsolute(currentPos));
	
	return ref.isEqual(currentPos) == false;
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerBase::processParameterEFS(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	bool e = true;
	bool f = true;
	bool s = true;
	
	if ( gcb.hasE() )
		; // to be defined
	
	if ( gcb.hasF() )
		f = changeCurrentFeedSpeedXYZ(CncSpeedMode::CncSpeedWork, gcb.getCurrentFeedSpeed());
	
	if ( gcb.hasS() )
		s = changeCurrentSpindleSpeed(gcb.getCurrentSpindleSpeed());
		
	if ( e == false )	std::cerr << CNC_LOG_FUNCT_A(": E failed!\n");
	if ( f == false )	std::cerr << CNC_LOG_FUNCT_A(": F failed!\n");
	if ( s == false )	std::cerr << CNC_LOG_FUNCT_A(": S failed!\n");

	return e && f && s;
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerBase::processRapidLinearMove(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	if ( gcb.hasOneOf_XYZ() == false )
		return true;
	
	if ( gcb.hasF() )	changeCurrentFeedSpeedXYZ(CncSpeedMode::CncSpeedRapid, gcb.getCurrentFeedSpeed());
	else				changeCurrentFeedSpeedXYZ(CncSpeedMode::CncSpeedRapid);
	
	// in this case position ins't changed
	if ( updateCurrentPosition(gcb) == false)
		return true; 
		
	return processLinearMove(false);
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerBase::processLinearMove(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	if ( gcb.hasOneOf_XYZ() == false )
		return true;
	
	// in this case position ins't changed
	if ( updateCurrentPosition(gcb) == false)
		return true;
		
	return processLinearMove(false);
}
//////////////////////////////////////////////////////////////////
bool GCodePathHandlerBase::processArcMove(GCodeBlock& gcb, bool sweep) {
//////////////////////////////////////////////////////////////////
	double r  = sqrt(pow(gcb.i, 2) + pow(gcb.j, 2)); // todo relative vs. absolute
	double ex = gcb.x;
	double ey = gcb.y;
	char cmd  = 'A';
	
	if ( gcb.isAbsolutePositioningXYZ() == false )
	{
		cmd = 'a';
		ex = currentPos.getX() + gcb.x;
		ey = currentPos.getY() + gcb.y;
	}
	
	const unsigned int ValueCount = 7;
	double values[ValueCount];
	values[0] = r;
	values[1] = r;
	values[2] = 0.0;
	values[3] = 0.0;
	values[4] = ( sweep == true ? 1.0 : 0.0 );
	values[5] = ex;
	values[6] = ey;
	
	if ( gcb.hasF() )
		changeCurrentFeedSpeedXYZ(CncSpeedMode::CncSpeedWork, gcb.getCurrentFeedSpeed());
		
	// this renders the arc and releases the callback 
	// GCodePathHandlerBase::processLinearMove(...)
	// for each curve lib point
	return processCommand_2DXY(cmd, ValueCount, values);
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