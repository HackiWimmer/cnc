#include "CncConfig.h"
#include "SVGPathHandlerBase.h"

//////////////////////////////////////////////////////////////////
void SVGPathHandlerBase::setSvgRootNode(const SVGRootNode& srn) {
//////////////////////////////////////////////////////////////////
	svgRootNode = srn;
}
//////////////////////////////////////////////////////////////////
bool SVGPathHandlerBase::processLinearMove(bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	LinearMoveParam param;
	param.absX 					= currentPos.getX();
	param.absY 					= currentPos.getY();
	param.absZ 					= currentPos.getZ();
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
	param.absZ 					= currentPos.getZ();
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
	if ( param.alreadyTransformed == false )
		transform(newPosAbsX, newPosAbsY);
	
	//  . . . then convert the input unit to mm . . 
	newPosAbsX = unitCalculator.convert(newPosAbsX);
	newPosAbsY = unitCalculator.convert(newPosAbsY);
	
	// shift the scene
	if ( CncConfig::getGlobalCncConfig()->getSvgReverseYAxisFlag() ) {
		newPosAbsY *= -1;
		newPosAbsY += 2* (svgRootNode.getHeight_MM());
	}
	
	// append
	const CncPathListEntry cpe =
			 pathListMgr.addEntryAbs(newPosAbsX, newPosAbsY, param.absZ, param.alreadyRendered);
								
	appendDebugValueDetail(cpe);
	return true;
}

