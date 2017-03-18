#include "CncSvgCurveLib.h"


/////////////////////////////////////////////////////////////////////////
bool 						CncSvgCurveLib::useCncSvgCurveLib 	= true;
const float 				CncSvgCurveLib::autoIncrement 		= -1.0f;
float 						CncSvgCurveLib::tIncrement			= CncSvgCurveLib::autoIncrement;
ArcParameter 				CncSvgCurveLib::AP;
QuadraticBezierParameter 	CncSvgCurveLib::QP;
CubicBezierParameter 		CncSvgCurveLib::CP;
CurveInfo					CncSvgCurveLib::_curveInfoResult;

/////////////////////////////////////////////////////////////////////////
CurveInfo& CncSvgCurveLib::getCurveInfo(SVGCurveLib::LinearCurve lc) {
	lc.PointOnLinearCurve(0.0f);
	_curveInfoResult.increment 	= tIncrement;
	_curveInfoResult.length 	= lc.resultantInfo.arcLength;
	
	if ( tIncrement == CncSvgCurveLib::autoIncrement ) {
		/*
		//todo calculate CurveLib resolution
		_curveInfoResult.increment = SvgUnitCalculator::getFactorReferenceUnit2MM()/lc.resultantInfo.arcLength;
		_curveInfoResult.increment = 1.0f/lc.resultantInfo.arcLength;
		//_curveInfoResult.increment = SvgUnitCalculator::getDPI()/lc.resultantInfo.arcLength;
		
		if ( _curveInfoResult.increment > 1.0f )
			_curveInfoResult.increment = lc.resultantInfo.arcLength;
		*/
		_curveInfoResult.increment = 0.09f;
	} 
	
	//std::clog << _curveInfoResult.increment << "     " << _curveInfoResult.length << std::endl;
	
	return _curveInfoResult;
}