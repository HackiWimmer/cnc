#ifndef OPENGL_CAMERA_HELPER_H
#define OPENGL_CAMERA_HELPER_H

namespace GLI {
	
	////////////////////////////////////////////////////////////////
	class CameraPosition {
		
		public:
			
			enum UpType { CUT_XTOP, CUT_YTOP, CUT_ZTOP };
			
			////////////////////////////////////////////
			CameraPosition() 
			: _eyeX(0.0f), _eyeY(0.0f), _eyeZ(0.0f)
			, _cntX(0.0f), _cntY(0.0f), _cntZ(0.0f)
			, _upX(0.0f),  _upY(0.0f),  _upZ(0.0f)
			{}
			
			////////////////////////////////////////////
			virtual ~CameraPosition() {
			}
			
			////////////////////////////////////////////
			void setEyePos(float x, float y, float z) {
				_eyeX = x; _eyeY = y; _eyeZ = z;
			}
			
			////////////////////////////////////////////
			void setCenterPos(float x, float y, float z) {
				_cntX = x; _cntY = y; _cntZ = z;
			}

			////////////////////////////////////////////
			void setUpPos(float x, float y, float z) {
				_upX = x; _upY = y; _upZ = z;
			}
			
			////////////////////////////////////////////
			void setUpPos(CameraPosition::UpType t) {
				switch ( t ) {
					case CUT_XTOP: 	setUpPos(1.0f, 0.0f, 0.0f); break;
					case CUT_YTOP: 	setUpPos(0.0f, 1.0f, 0.0f); break;
					case CUT_ZTOP: 	setUpPos(0.0f, 0.0f, 1.0f); break;
					
					default: 		setUpPos(0.0f, 0.0f, 1.0f);
				}
			}
			
			////////////////////////////////////////////
			const float getEyeX() const { return _eyeX; }
			const float getEyeY() const { return _eyeY; }
			const float getEyeZ() const { return _eyeZ; }
			
			const float getCenterX() const { return _cntX; }
			const float getCenterY() const { return _cntY; }
			const float getCenterZ() const { return _cntZ; }
			
			const float getUpX() const { return _upX; }
			const float getUpY() const { return _upY; }
			const float getUpZ() const { return _upZ; }
			
			const int getCurXYPlaneEyeAngle() 		{ return round(std::atan2(_eyeY, _eyeX) * 180 / PI); }
			const float getCurXYPlaneEyeRadius() 	{ return sqrt(_eyeX * _eyeX + _eyeY * _eyeY); }
			
			////////////////////////////////////////////
			void rotateXYPlanTopZ_3D(int angle, float radius, float z) {
				setUpPos(0.0, 0.0, 1.0);
				setCenterPos(0.0, 0.0, 0.0);
				
				_eyeX = std::cos(angle*PI/180) * radius;
				_eyeY = std::sin(angle*PI/180) * radius;
				_eyeZ = z;
			}
			
			
	protected:
			
			const float PI = 3.14159265;
			
			float _eyeX, _eyeY, _eyeZ;
			float _cntX, _cntY, _cntZ;
			float _upX,  _upY,  _upZ;
	};
	
	////////////////////////////////////////////////////////////////
	class CameraPosition2D : public CameraPosition {
		
		public:
			////////////////////////////////////////////
			CameraPosition2D() 
			: CameraPosition()
			{}
			
			////////////////////////////////////////////
			virtual ~CameraPosition2D() {
			}
	};
	
	////////////////////////////////////////////////////////////////
	class CameraPosition3D : public CameraPosition {
		
		public:
			////////////////////////////////////////////
			CameraPosition3D() 
			: CameraPosition()
			{}
			
			////////////////////////////////////////////
			virtual ~CameraPosition3D() {
			}
	};
	
};

#endif