#include "CncConfig.h"
#include "CncFlowPositionConverter.h"

///////////////////////////////////////////////////////////////////
CncFlowPositionConverter::CncFlowPositionConverter()
: pos()
, ret()
///////////////////////////////////////////////////////////////////
{}
///////////////////////////////////////////////////////////////////
CncFlowPositionConverter::~CncFlowPositionConverter(){
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void CncFlowPositionConverter::reset() { 
///////////////////////////////////////////////////////////////////
	pos.zeroXYZ();
	ret.zeroXYZ();
}
///////////////////////////////////////////////////////////////////
void CncFlowPositionConverter::set(double dx, double dy, double dz) {
///////////////////////////////////////////////////////////////////
	pos.inc(dx * THE_CONFIG->getCalculationFactX(), 
			dy * THE_CONFIG->getCalculationFactY(), 
			dz * THE_CONFIG->getCalculationFactZ()
		    );
}
///////////////////////////////////////////////////////////////////
const CncLongPosition& CncFlowPositionConverter::get() { 
///////////////////////////////////////////////////////////////////
	// -----------------------------------------------------------
	if ( abs(pos.getX()) < 1.0 ) {
		ret.setX(0);
	}
	else {
		const int32_t X = (int32_t)round(pos.getX());
		ret.setX(X);
		pos.decX((double)X);
	}
	
	// -----------------------------------------------------------
	if ( abs(pos.getY()) < 1.0 ) {
		ret.setY(0);
	}
	else {
		const int32_t Y = (int32_t)round(pos.getY());
		ret.setY(Y);
		pos.decY((double)Y);
	}
	
	// -----------------------------------------------------------
	if ( abs(pos.getZ()) < 1.0 ) {
		ret.setZ(0);
	}
	else {
		const int32_t Z = (int32_t)round(pos.getZ());
		ret.setZ(Z);
		pos.decZ((double)Z);
	}

	return ret;
}
