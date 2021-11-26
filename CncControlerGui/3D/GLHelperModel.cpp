#include "CncCommon.h"
#include "GLHelperModel.h"

///////////////////////////////////////////////////
GLI::ModelScale::ModelScale()
: _step		(0.1f)
, _scale	(1.0f)
///////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////
GLI::ModelScale::ModelScale(float f)
: _step		(0.1f)
, _scale	(f)
///////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////
GLI::ModelScale::ModelScale(const ModelScale& ms)
: _step		(ms.getStepWidth())
, _scale	(ms.getScaleFactor())
///////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////
GLI::ModelScale::~ModelScale() {
///////////////////////////////////////////////////
}
///////////////////////////////////////////////////
void GLI::ModelScale::resetScale() {
///////////////////////////////////////////////////
	_scale = 1.0; 
}
///////////////////////////////////////////////////
void GLI::ModelScale::setScale(float scale) { 
///////////////////////////////////////////////////
	_scale = std::max(_minScaleFactor, std::min(scale, _maxScaleFactor)); 
}
///////////////////////////////////////////////////
void GLI::ModelScale::incScale(float s) {
///////////////////////////////////////////////////
	if ( cnc::fltCmp::nu(s) )
	{
		if ( cnc::fltCmp::lt(_scale, 1.0) )	s = _step / 5;
		else								s = _step;
	}

	_scale = std::min(_scale + fabs(s), _maxScaleFactor); 
}
///////////////////////////////////////////////////
void GLI::ModelScale::decScale(float s) {
///////////////////////////////////////////////////
	if ( cnc::fltCmp::nu(s) )
	{
		if ( cnc::fltCmp::lt(_scale, 1.0) )	s = _step / 5;
		else								s = _step;
	}

	_scale = std::max(_scale - fabs(s), _minScaleFactor); 
}
///////////////////////////////////////////////////
void GLI::ModelScale::setScaleByRatio(float ratio) {
///////////////////////////////////////////////////
	if      ( cnc::fltCmp::eq(ratio, 0.0) )		setScale( 1.0 );
	else if ( ratio < 0.0 )						setScale( (-1)/ratio * _minScaleFactor );
	else if ( ratio > 0.0 )						setScale( ratio      * _maxScaleFactor + 1.0 );
}
///////////////////////////////////////////////////
float GLI::ModelScale::getScaleRatio() const {
///////////////////////////////////////////////////
	const float posDist = +(_maxScaleFactor - 1.0);
	const float negDist = -(1.0 - _minScaleFactor);
	
	float ret = 1.0;
	if      ( cnc::fltCmp::eq(_scale, 1.0) )	ret = 0.0;
	else if ( _scale < 1.0 )					ret = _scale/negDist;
	else if ( _scale > 1.0)						ret = _scale/posDist;
	
	return ret;
}
///////////////////////////////////////////////////
void GLI::ModelScale::setStepWidth(float f) {
///////////////////////////////////////////////////
	_step = ( f >= _minScaleFactor && f <= 0.50f  ? f : _step);
} 
