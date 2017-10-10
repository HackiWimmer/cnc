#ifndef OPENGL_MODEL_HELPER_H
#define OPENGL_MODEL_HELPER_H

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
			void resetScale() { _x = 1.0; _y = 1.0; _z = 1.0; }
			
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
			: _step(1.0f)
			, _x(IX3D),    _y(IY3D),    _z(IZ3D)
			, _dx2D(IX2D), _dy2D(IY2D), _dz2D(IZ2D)
			, _dx3D(IX3D), _dy3D(IY3D), _dz3D(IZ3D)
			{}
			
			////////////////////////////////////////////
			ModelRotate(float f)
			: _step(1.0f)
			, _x(f),       _y(f),       _z(f)
			, _dx2D(IX2D), _dy2D(IY2D), _dz2D(IZ2D)
			, _dx3D(IX3D), _dy3D(IY3D), _dz3D(IZ3D)
			{}
			
			////////////////////////////////////////////
			ModelRotate(float x, float y, float z)
			: _step(1.0f), _x(x), _y(y), _z(z)
			, _dx2D(IX2D), _dy2D(IY2D), _dz2D(IZ2D)
			, _dx3D(IX3D), _dy3D(IY3D), _dz3D(IZ3D)
			{}
			
			////////////////////////////////////////////
			ModelRotate(const ModelRotate& mr)
			: _step(mr.getStepWidth())
			, _x(mr.angleX()),        _y(mr.angleY()),        _z(mr.angleZ())
			, _dx2D(mr.defaultX2D()), _dy2D(mr.defaultY2D()), _dz2D(mr.defaultZ2D())
			, _dx3D(mr.defaultX3D()), _dy3D(mr.defaultY3D()), _dz3D(mr.defaultZ3D())
			{}
			
			////////////////////////////////////////////
			~ModelRotate() {
			}
			
			////////////////////////////////////////////
			const float defaultX2D() const { return _dx2D; }
			const float defaultY2D() const { return _dy2D; }
			const float defaultZ2D() const { return _dz2D; }
			
			const float defaultX3D() const { return _dx2D; }
			const float defaultY3D() const { return _dy3D; }
			const float defaultZ3D() const { return _dz3D; }
			
			////////////////////////////////////////////
			void setDefault2D(float f)  { _dx2D = f; _dy2D = f; _dz2D = f; }
			void setDefaultX2D(float f) { _dx2D = f; }
			void setDefaultY2D(float f) { _dy2D = f; }
			void setDefaultZ2D(float f) { _dz2D = f; }
			
			////////////////////////////////////////////
			void setDefault3D(float f)  { _dx3D = f; _dy3D = f; _dz3D = f; }
			void setDefaultX3D(float f) { _dx3D = f; }
			void setDefaultY3D(float f) { _dy3D = f; }
			void setDefaultZ3D(float f) { _dz3D = f; }
			
			////////////////////////////////////////////
			const float getStepWidth() const { return _step; }
			void setStepWidth(int f) {
				int q = f%360;
				_step = 1.0f * q;
			}
			
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
			
			void setAngleX(int a) { _x = 1.0f * a; }
			void setAngleY(int a) { _y = 1.0f * a; }
			void setAngleZ(int a) { _z = 1.0f * a; }
			
			////////////////////////////////////////////
			void reset2DDefaults()   { _x = _dx2D = IX2D; _y = _dy2D = IY2D; _z = _dz2D = IZ2D; }
			void reset3DDefaults()   { _x = _dx3D = IX3D; _y = _dy3D = IY3D; _z = _dz3D = IZ3D; }
			
			void restore2DDefaults() { _x = _dx2D; _y = _dy2D; _z = _dz2D; }
			void restore3DDefaults() { _x = _dx3D; _y = _dy3D; _z = _dz3D; }
			
		private:
		
			const float IX2D =  0.0f;
			const float IY2D =  0.0f;
			const float IZ2D =  0.0f;
			
			const float IX3D =  0.0f;
			const float IY3D =  0.0f;
			const float IZ3D = 90.0f;
			
			float _step;
			float _x, _y, _z;
			float _dx2D, _dy2D, _dz2D;
			float _dx3D, _dy3D, _dz3D;
	};

}; // namespace GLI

#endif