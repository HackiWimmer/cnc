#ifndef OPENGL_MODEL_HELPER_H
#define OPENGL_MODEL_HELPER_H

#include <vector>

namespace GLI {
	
	////////////////////////////////////////////////////////////////
	class ModelScale {
		
		public:
			////////////////////////////////////////////
			ModelScale()
			: _step(0.1f), _x(1.0f), _y(1.0f), _z(1.0f)
			{}
			
			////////////////////////////////////////////
			ModelScale(float f)
			: _step(0.1f), _x(f), _y(f), _z(f)
			{}
			
			////////////////////////////////////////////
			ModelScale(float x, float y, float z)
			: _step(0.1f), _x(x), _y(y), _z(z)
			{}
			
			////////////////////////////////////////////
			ModelScale(const ModelScale& ms)
			: _step(ms.getStepWidth()), _x(ms.factX()), _y(ms.factY()), _z(ms.factZ())
			{}
			
			////////////////////////////////////////////
			~ModelScale() {
			}
			
			////////////////////////////////////////////
			const float factX() const { return _x; }
			const float factY() const { return _y; }
			const float factZ() const { return _z; }
			
			////////////////////////////////////////////
			void incScale() { _x += _step; _y += _step; _z += _step; }
			void decScale() { _x -= _step; _y -= _step; _z -= _step; }
			
			void incScaleX() { _x += _step; }
			void incScaleY() { _y += _step; }
			void incScaleZ() { _z += _step; }
			
			void decScaleX() { _x -= _step; }
			void decScaleY() { _y -= _step; }
			void decScaleZ() { _z -= _step; }
			
			////////////////////////////////////////////
			const float getStepWidth() const { return _step; }
			void setStepWidth(float f) {
				if ( f >= 0.01f && f <= 10.0f )
					_step = f;
			}
			
		private:
			float _step;
			float _x, _y, _z;
	};
	
	////////////////////////////////////////////////////////////////
	class ModelRotate {
		
		public:
			////////////////////////////////////////////
			ModelRotate()
			: _step(1.0f), _x(0.0f), _y(0.0f), _z(0.0f)
			, _dx(0.0f), _dy(0.0f), _dz(0.0f)
			{}
			
			////////////////////////////////////////////
			ModelRotate(float f)
			: _step(1.0f), _x(f), _y(f), _z(f)
			, _dx(0.0f), _dy(0.0f), _dz(0.0f)
			{}
			
			////////////////////////////////////////////
			ModelRotate(float x, float y, float z)
			: _step(1.0f), _x(x), _y(y), _z(z)
			, _dx(0.0f), _dy(0.0f), _dz(0.0f)
			{}
			
			////////////////////////////////////////////
			ModelRotate(const ModelRotate& mr)
			: _step(mr.getStepWidth()), _x(mr.angleX()), _y(mr.angleY()), _z(mr.angleZ())
			, _dx(mr.defaultX()), _dy(mr.defaultY()), _dz(mr.defaultZ())
			{}
			
			////////////////////////////////////////////
			~ModelRotate() {
			}
			
			////////////////////////////////////////////
			const float defaultX() const { return _dx; }
			const float defaultY() const { return _dy; }
			const float defaultZ() const { return _dz; }
			
			////////////////////////////////////////////
			void setDefault(float f)  { _dx = f; _dy = f; _dz = f; }
			void setDefaultX(float f) { _dx = f; }
			void setDefaultY(float f) { _dy = f; }
			void setDefaultZ(float f) { _dz = f; }
			
			////////////////////////////////////////////
			const float angleX() const { return _x; }
			const float angleY() const { return _y; }
			const float angleZ() const { return _z; }
			
			////////////////////////////////////////////
			void incAngle() { _x += _step; _y += _step; _z += _step; }
			void decAngle() { _x -= _step; _y -= _step; _z -= _step; }
			
			void incAngleX() { _x += _step; }
			void incAngleY() { _y += _step; }
			void incAngleZ() { _z += _step; }
			
			void decAngleX() { _x -= _step; }
			void decAngleY() { _y -= _step; }
			void decAngleZ() { _z -= _step; }
			
			////////////////////////////////////////////
			void reset() { _x = 0.0f; _y = 0.0f; _z = 0.0f; }
			void restoreDefaults() { _x = _dx; _y = _dy; _z = _dz; }
			
			////////////////////////////////////////////
			const float getStepWidth() const { return _step; }
			void setStepWidth(int f) {
				int q = f%360;
				_step = 1.0f * q;
			}
			
		private:
			float _step;
			float _x, _y, _z;
			float _dx, _dy, _dz;
	};

}; // namespace GLI

#endif