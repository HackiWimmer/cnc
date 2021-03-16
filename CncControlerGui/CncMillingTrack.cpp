#include "CncCommon.h"
#include "CncMillingTrack.h"

/////////////////////////////////////////////////////////////////
// Note: the given tool diameter have to be already normalized to the 
//       underlying coordinate system of the further given points
/////////////////////////////////////////////////////////////////
CncMillingTrackCreator::CncMillingTrackCreator(float toolDiameter)
: state			(MTCS_UNKNOWN) 
, track			()
, posCount		(0)
, first			(0.0f, 0.0f, 0.0f)
, dp0			()
, dp1			()
, dp2			()
, toolRadius	(toolDiameter / 2.0f)
, topDir		(0.0f, 0.0f, 1.0f)
/////////////////////////////////////////////////////////////////
{
}
/////////////////////////////////////////////////////////////////
CncMillingTrackCreator::~CncMillingTrackCreator() {
/////////////////////////////////////////////////////////////////
	track.clear();
}
/////////////////////////////////////////////////////////////////
void CncMillingTrackCreator::reset() {
/////////////////////////////////////////////////////////////////
	state		= MTCS_UNKNOWN;
	posCount	= 0;
	
	first	.set(0.0f, 0.0f, 0.0f);
	dp0		.reset();
	dp1		.reset();
	dp2		.reset();
	track	.clear();
}
/////////////////////////////////////////////////////////////////
const char* CncMillingTrackCreator::getStateAsStr() const {
/////////////////////////////////////////////////////////////////
	switch ( state ) {
		case MTCS_UNKNOWN:		return "Unknown";
		case MTCS_STARTED:		return "Started";
		case MTCS_PROCESSING:	return "Processing";
		case MTCS_FINISHED:		return "Finished";
		case MTCS_ERROR:		return "Error";
	}
	
	return "???";
}
/////////////////////////////////////////////////////////////////
void CncMillingTrackCreator::skip(float px, float py, float pz) {
/////////////////////////////////////////////////////////////////
	dp0 = dp1;
	dp1 = dp2;
	
	dp2.pnt.set(px, py, pz);
}
/////////////////////////////////////////////////////////////////
bool CncMillingTrackCreator::calculate(float px, float py, float pz) {
/////////////////////////////////////////////////////////////////
	posCount++;
	
	if ( state == MTCS_PROCESSING ) {
		
		skip();
		
	}
	else if ( state == MTCS_STARTED ) {
		
		first  .set(px, py, pz);
		skip(px, py, pz);
		
		return true;
	}
	else {
		
		std::cerr << CNC_LOG_FUNCT_A("Invalid state (%s) to calculate!\n", getStateAsStr());
		return false;
	}
	
	/*
	// determine direction
	curr.dir.set(curr.pnt.getX() - prev.pnt.getX(),
				 curr.pnt.getY() - prev.pnt.getY(),
				 curr.pnt.getZ() - prev.pnt.getZ()
				);
				
	if ( curr.dir.isParallelTo(topDir) ) {
		
		// ......
		return true;
	}
	
	// determine current radius vectors
	curr.rd1.set(curr.dir.getVectorProduct(topDir));
	curr.rd1.normalize();
	curr.rd1.mul(toolRadius);
	curr.rd2.set(curr.rd1);
	curr.rd2.mul(-1);
	
	// expand track
	track.push_back (curr.rd2);
	track.push_front(curr.rd1);
	*/
	
	return true;
}
/////////////////////////////////////////////////////////////////
bool CncMillingTrackCreator::start(float px, float py, float pz) {
/////////////////////////////////////////////////////////////////
	if ( state != MTCS_UNKNOWN ) {
		std::cerr << CNC_LOG_FUNCT_A("Invalid state (%s) to start!\n", getStateAsStr());
		return false;
	}
	
	state = MTCS_STARTED;
	
	if ( calculate(px, py, pz) == false ) {
		state = MTCS_ERROR;
		return false;
	}
	
	return ( state == MTCS_STARTED );
}
/////////////////////////////////////////////////////////////////
bool CncMillingTrackCreator::next(float px, float py, float pz) {
/////////////////////////////////////////////////////////////////
	if ( state != MTCS_PROCESSING ) {
		if ( state != MTCS_STARTED ) {
			std::cerr << CNC_LOG_FUNCT_A("Invalid state (%s) to go forward!\n", getStateAsStr());
			return false;
		}
	}
	
	state = MTCS_PROCESSING;
	
	if ( calculate(px, py, pz) == false ) {
		state = MTCS_ERROR;
		return false;
	}
	
	return ( state == MTCS_PROCESSING );
}
/////////////////////////////////////////////////////////////////
bool CncMillingTrackCreator::finish() {
/////////////////////////////////////////////////////////////////
	if ( state != MTCS_PROCESSING ) {
		if ( state != MTCS_STARTED ) {
			std::cerr << CNC_LOG_FUNCT_A("Invalid state (%s) to go forward!\n", getStateAsStr());
			return false;
		}
	}
	
	if ( posCount == 1 ) {
		// only one point
	}
	
	if ( posCount == 2 ) {
		// only a single line
	}

	if ( first == dp2.pnt ) {
		// closed path
	}
	
	
	
	
	state = MTCS_FINISHED;
	return ( state == MTCS_FINISHED );
}
