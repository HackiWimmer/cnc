#ifndef OPENGL_MODEL_HELPER_H
#define OPENGL_MODEL_HELPER_H

#include "CncCommon.h"

namespace GLI {
	
	////////////////////////////////////////////////////////////////
	class ModelScale {
		
		public:
			////////////////////////////////////////////
			ModelScale();
			explicit ModelScale(float f);
			explicit ModelScale(const ModelScale& ms);
			
			~ModelScale();
			
			const float getStepWidth()		const	{ return _step; }
			const float getScaleFactor()	const	{ return _scale; }
			
			bool canScale()					const;
			bool canIncScale()				const;
			bool canDecScale()				const;

			void resetScale();
			void setScale(float scale);
			void incScale(float s=0.0);
			void decScale(float s=0.0);
			
			void setScaleByRatio(float ratio);
			float getScaleRatio() const;
			
			void setStepWidth(float f);
			
			static float getMinScaleFactor()	{ return _minScaleFactor; }
			static float getMaxScaleFactor()	{ return _maxScaleFactor; }
			
		private:
			static const float _minScaleFactor;
			static const float _maxScaleFactor;
			
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