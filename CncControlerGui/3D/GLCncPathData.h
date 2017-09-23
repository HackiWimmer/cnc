#ifndef OPENGL_CNC_PATH_DATA_H
#define OPENGL_CNC_PATH_DATA_H

#include <vector>

namespace GLI {
	
	////////////////////////////////////////////////////////////////
	class GLCncPathVertices {
		
		public:
			////////////////////////////////////////////
			GLCncPathVertices() 
			: _x(0)
			, _y(0)
			, _z(0)
			{}

			////////////////////////////////////////////
			GLCncPathVertices(long x, long y, long z) 
			: _x(x)
			, _y(y)
			, _z(z)
			{}
			
			////////////////////////////////////////////
			GLCncPathVertices(const GLCncPathVertices& cpv) 
			: _x(cpv.getX())
			, _y(cpv.getY())
			, _z(cpv.getZ())
			{}
			
			////////////////////////////////////////////
			~GLCncPathVertices() {
			}
			
			////////////////////////////////////////////
			const long getX() const { return _x; }
			const long getY() const { return _y; }
			const long getZ() const { return _z; }
			
			////////////////////////////////////////////
			const GLCncPathVertices& set(long x, long y, long z) {
				_x = x;
				_y = y;
				_z = z;
				
				return *this;
			}
			
		private:
			long _x;
			long _y;
			long _z;
	};
	
	typedef std::vector<GLCncPathVertices> GLCncPath;
	
}; // namespace GLI

#endif