#include "SVGPathHandlerBase.h"

//////////////////////////////////////////////////////////////////
bool SVGPathHandlerBase::processLinearMove(bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	LinearMoveParam param;
	param.absX 					= currentPos.getX();
	param.absY 					= currentPos.getY();
	param.alreadyTransformed 	= false;
	param.alreadyRendered		= alreadyRendered;
	
	return processLinearMove(param);
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerBase::callback(const CncCurveLib::Point& p) {
//////////////////////////////////////////////////////////////////
	LinearMoveParam param;
	param.absX 					= p.x;
	param.absY 					= p.y;
	param.alreadyTransformed 	= true;
	param.alreadyRendered		= true;
	
	return processLinearMove(param);
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerBase::processLinearMove(const LinearMoveParam& param) {
//////////////////////////////////////////////////////////////////
	double newPosAbsX = param.absX;
	double newPosAbsY = param.absY;
	
	// first perform the transformations . . .
	if ( param.alreadyRendered == false )
		transform(newPosAbsX, newPosAbsY);
	
	//  . . . then convert the input unit to mm . . 
	newPosAbsX = unitCalculator.convert(newPosAbsX);
	newPosAbsY = unitCalculator.convert(newPosAbsY);
	
	// append
	const CncPathListEntry cpe = pathListMgr.calculateAndAddEntry(newPosAbsX, newPosAbsY, param.alreadyRendered , isZAxisDown());
	appendDebugValueDetail(cpe);
	
	return true;
}

