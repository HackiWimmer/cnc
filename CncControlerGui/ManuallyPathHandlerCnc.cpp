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
bool ManuallyPathHandlerCnc::processLinearMove(const CncMoveDefinition& md) {
//////////////////////////////////////////////////////////////////
	wxASSERT( cncControl != NULL );
	
	// tool handling
	switchSpindleState(md.toolState);
	
	// speed handling
	if ( md.speedMode != lastSpeedMode || md.speedValue != lastSpeedValue ) {
		if ( cncControl->changeCurrentFeedSpeedXYZ_MM_MIN(md.speedValue, md.speedMode) == false )
			return false;
	}
	
	// -------------------------------------------------------------
	auto move = [&]()
	{
		bool ret = processLinearMove(false);
		if ( ret == false && md.correctLimit == true ) {
			ret = cncControl->correctLimitPositions();
		}
		
		return ret;
	};
	
	// -------------------------------------------------------------
	auto prepareAxisX = [&](const CncMoveDefinition::Axis& a)
	{
		// define move
		if ( a.absolute )	currentPos.setX(a.value);
		else				currentPos.incX(a.value);
	};
	
	// -------------------------------------------------------------
	auto prepareAxisY = [&](const CncMoveDefinition::Axis& a)
	{
		// define move
		if ( a.absolute )	currentPos.setY(a.value);
		else				currentPos.incY(a.value);
	};
	
	// -------------------------------------------------------------
	auto prepareAxisZ = [&](const CncMoveDefinition::Axis& a)
	{
		// define move
		if ( a.absolute )	currentPos.setZ(a.value);
		else				currentPos.incZ(a.value);
	};
	
	// trace
	//std::clog << CNC_LOG_FUNCT << md << std::endl;
	
	bool ret = false;
	switch ( md.moveMode ) 
	{
		case CncMoveDefinition::MoveMode::MM_1D:
		{
			// define X move
			prepareAxisX(md.x);
			ret = move();
			
			if ( ret == true )
			{
				// define Y move
				prepareAxisY(md.y);
				ret = move();
				
				if ( ret == true )
				{
					// define Z move
					prepareAxisZ(md.z);
					ret = move();
				}
			}
			break;
		}
		case CncMoveDefinition::MoveMode::MM_2D:
		{
			// define XY move
			prepareAxisX(md.x);
			prepareAxisY(md.y);
			ret = move();
			
			if ( ret == true )
			{
				// define Z move
				prepareAxisZ(md.z);
				ret = move();
			}
			break;
		}
		case CncMoveDefinition::MoveMode::MM_3D:
		{
			// define move
			prepareAxisX(md.x);
			prepareAxisY(md.y);
			prepareAxisZ(md.z);
			
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