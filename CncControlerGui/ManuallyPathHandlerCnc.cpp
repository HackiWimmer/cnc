#include "ManuallyPathHandlerCnc.h"

/////////////////////////////////////////////////////////////
ManuallyPathHandlerCnc::ManuallyPathHandlerCnc(CncControl* cnc)
: PathHandlerBase()
, cncControl(cnc)
, lastSpeedMode(CncSpeedRapid)
, lastSpeedValue(0.0)
/////////////////////////////////////////////////////////////
{
	changeInputUnit(Unit::mm);
}
/////////////////////////////////////////////////////////////
ManuallyPathHandlerCnc::~ManuallyPathHandlerCnc() {
/////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
void ManuallyPathHandlerCnc::logMeasurementStart() {
//////////////////////////////////////////////////////////////////
	wxASSERT(cncControl);
	cncControl->startSerialMeasurement();
}
//////////////////////////////////////////////////////////////////
void ManuallyPathHandlerCnc::logMeasurementEnd() {
//////////////////////////////////////////////////////////////////
	wxASSERT(cncControl);
	cncControl->stopSerialMeasurement();
}
//////////////////////////////////////////////////////////////////
bool ManuallyPathHandlerCnc::prepareWork() {
//////////////////////////////////////////////////////////////////
	wxASSERT( cncControl != NULL );
	cncControl->resetDurationCounter();
	return true;
}
//////////////////////////////////////////////////////////////////
bool ManuallyPathHandlerCnc::finishWork() {
//////////////////////////////////////////////////////////////////
	return true;
}
//////////////////////////////////////////////////////////////////
void ManuallyPathHandlerCnc::switchSpindleState(bool state) {
//////////////////////////////////////////////////////////////////
	state == true ? cncControl->switchSpindleOn() : cncControl->switchSpindleOff();
}
//////////////////////////////////////////////////////////////////
bool ManuallyPathHandlerCnc::processLinearMove(const MoveDefinition& md) {
//////////////////////////////////////////////////////////////////
	wxASSERT( cncControl != NULL );
	
	// tool handling
	switchSpindleState(md.toolState);
	
	// speed handling
	if ( md.speedMode != lastSpeedMode || md.f != lastSpeedValue ) {
		if ( cncControl->changeCurrentFeedSpeedXYZ_MM_MIN(md.f, md.speedMode) == false )
			return false;
	}
	
	// -------------------------------------------------------------
	auto move = [&]() {
		
		bool ret = processLinearMove(false);
		if ( ret == false && md.correctLimit == true ) {
			ret = cncControl->correctLimitPositions();
		}
		
		return ret;
	};
	
	bool ret = false;
	
	switch ( md.moveMode ) {
		case MoveDefinition::MM_1D:
		{
			// define X move
			if ( md.absoluteMove == true ) {
				currentPos.setX(md.x);
				currentPos.setY(currentPos.getY());
				currentPos.setZ(currentPos.getZ());
			} else {
				currentPos.incX(md.x);
				currentPos.incY(0.0);
				currentPos.incZ(0.0);
			}
			
			ret = move();
			if ( ret == false)
				break;
			
			// define Y move
			if ( md.absoluteMove == true ) {
				currentPos.setX(currentPos.getX());
				currentPos.setY(md.y);
				currentPos.setZ(currentPos.getZ());
			} else {
				currentPos.incX(0.0);
				currentPos.incY(md.y);
				currentPos.incZ(0.0);
			}
			
			ret = move();
			if ( ret == false)
				break;

			// define Z move
			if ( md.absoluteMove == true ) {
				currentPos.setX(currentPos.getX());
				currentPos.setY(currentPos.getY());
				currentPos.setZ(md.z);
			} else {
				currentPos.incX(0.0);
				currentPos.incY(0.0);
				currentPos.incZ(md.z);
			}
			
			ret = move();
			break;
		}
		case MoveDefinition::MM_2D:
		{
			// define XY move
			if ( md.absoluteMove == true ) {
				currentPos.setX(md.x);
				currentPos.setY(md.y);
				currentPos.setZ(currentPos.getZ());
			} else {
				currentPos.incX(md.x);
				currentPos.incY(md.y);
				currentPos.incZ(0.0);
			}
			
			ret = move();
			if ( ret == false)
				break;
			
			// define Z move
			if ( md.absoluteMove == true ) {
				currentPos.setX(currentPos.getX());
				currentPos.setY(currentPos.getY());
				currentPos.setZ(md.z);
			} else {
				currentPos.incX(0.0);
				currentPos.incY(0.0);
				currentPos.incZ(md.z);
			}
			
			ret = move();
			break;
		}
		case MoveDefinition::MM_3D:
		{
			// define move
			if ( md.absoluteMove == true ) {
				currentPos.setX(md.x);
				currentPos.setY(md.y);
				currentPos.setZ(md.z);
			} else {
				currentPos.incX(md.x);
				currentPos.incY(md.y);
				currentPos.incZ(md.z);
			}
			
			ret = move();
			break;
		}
	}
	
	switchSpindleState(false);
	return ret;
}
//////////////////////////////////////////////////////////////////
bool ManuallyPathHandlerCnc::processLinearMove(bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	return cncControl->moveAbsLinearMetricXYZ(currentPos.getX(), currentPos.getY(), currentPos.getZ(), alreadyRendered);
}