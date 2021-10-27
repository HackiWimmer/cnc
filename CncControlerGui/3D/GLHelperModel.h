#ifndef OPENGL_MODEL_HELPER_H
#define OPENGL_MODEL_HELPER_H

#include "CncCommon.h"

namespace GLI {
	
	////////////////////////////////////////////////////////////////
	class ModelScale {
		
		public:
			////////////////////////////////////////////
			ModelScale()
			: _step(0.1f), _scale(1.0f)
			{}
			
			explicit ModelScale(float f)
			: _step(0.1f), _scale(f)
			{}
			
			explicit ModelScale(const ModelScale& ms)
			: _step(ms.getStepWidth()), _scale(ms.getScaleFactor())
			{}
			
			~ModelScale() 
			{}
			
			////////////////////////////////////////////
			const float getStepWidth()		const	{ return _step; }
			const float getScaleFactor()	const	{ return _scale; }
			const float getMinScaleFactor()	const 	{ return _minScaleFactor; }
			const float getMaxScaleFactor()	const 	{ return _maxScaleFactor; }
			void resetScale()						{ _scale = 1.0; }
			void setScale(float scale)				{ _scale = std::max(_minScaleFactor, std::min(scale,_maxScaleFactor)); }
			void incScale() 						{ _scale = std::min(_scale + _step, _maxScaleFactor); }
			void decScale()							{ _scale = std::max(_scale - _step, _step); }
			
			void setScaleByRatio(float ratio)
			{
				if      ( cnc::fltCmp::eq(ratio, 0.0) )		setScale( 1.0 );
				else if ( ratio < 0.0 )						setScale( (-1)/ratio * _minScaleFactor );
				else if ( ratio > 0.0 )						setScale( ratio      * _maxScaleFactor + 1.0 );
			}
			
			float getScaleRatio() const
			{
				const float posDist = +(_maxScaleFactor - 1.0);
				const float negDist = -(1.0 - _minScaleFactor);
				
				float ret = 1.0;
				if      ( cnc::fltCmp::eq(_scale, 1.0) )	ret = 0.0;
				else if ( _scale < 1.0 )					ret = _scale/negDist;
				else if ( _scale > 1.0)						ret = _scale/posDist;
				
				return ret;
			}
			
			void setStepWidth(float f) 				{ _step = ( f >= 0.01f && f <= 0.50f  ? f : _step); } 
			
		private:
			static constexpr float _minScaleFactor = 0.01f;
			static constexpr float _maxScaleFactor = 5.00f;
			
			float _step;
			float _scale;
	};
	
	////////////////////////////////////////////////////////////////
	class ModelRotate {
		
		public:

			ModelRotate()
			: _step		(1.0f)
			, _x		(0.0f)
			, _y		(0.0f)
			, _z		(0.0f)
			{}
			
			ModelRotate(float f)
			: _step		(1.0f)
			, _x		(f)
			, _y		(f)
			, _z		(f)
			{}
			
			ModelRotate(float x, float y, float z)
			: _step		(1.0f)
			, _x		(x)
			, _y		(y)
			, _z		(z)
			{}
			
			ModelRotate(const ModelRotate& mr)
			: _step		(mr.getStepWidth())
			, _x		(mr.angleX())
			, _y		(mr.angleY())
			, _z		(mr.angleZ())
			{}
			
			~ModelRotate() {
			}
			
			const float getStepWidth() const { return _step; }
			void setStepWidth(int f) {
				int q = f%360;
				_step = 1.0f * q;
			}
			
			const float angleX()	const	{ return _x; }
			const float angleY()	const	{ return _y; }
			const float angleZ()	const	{ return _z; }
			
			void incAngle()					{ _x += _step; _y += _step; _z += _step; }
			void decAngle()					{ _x -= _step; _y -= _step; _z -= _step; }
			
			void incAngleX()				{ _x += _step; }
			void incAngleY()				{ _y += _step; }
			void incAngleZ()				{ _z += _step; }
			
			void decAngleX()				{ _x -= _step; }
			void decAngleY()				{ _y -= _step; }
			void decAngleZ()				{ _z -= _step; }
			
			void setAngleX(int a)			{ _x = 1.0f * a; }
			void setAngleY(int a)			{ _y = 1.0f * a; }
			void setAngleZ(int a)			{ _z = 1.0f * a; }
			
			void reset()					{ _x = 0.0; _y = 0.0; _z = 0.0; }
			
		private:
			float _step;
			float _x, _y, _z;
	};

}; // namespace GLI

#endif