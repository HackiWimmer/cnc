#ifndef OPENGL_CONTEXT_PATH_BASE_H
#define OPENGL_CONTEXT_PATH_BASE_H

#include "3D/GLContextBase.h"

/////////////////////////////////////////////////////////////////
class OpenGLContextCncPathBase : public OpenGLContextBase {
	
	public:
	
		enum DrawType {DT_POINTS, DT_LINE_STRIP };
		
		/////////////////////////////////////////////////////////
		OpenGLContextCncPathBase(wxGLCanvas* canvas)
		: OpenGLContextBase(canvas)
		, cncPath()
		, drawType(DT_LINE_STRIP)
		{
			cncPath.reserve(1024 * 1024);
		}
		
		/////////////////////////////////////////////////////////
		virtual ~OpenGLContextCncPathBase() {
			clearPathData();
		}
		
		/////////////////////////////////////////////////////////
		void clearPathData() {
			cncPath.clear();
		}
		
		/////////////////////////////////////////////////////////
		const GLI::GLCncPath& getPathData() {
			return cncPath;
		}
		
		/////////////////////////////////////////////////////////
		void tracePathData(std::ostream& out) {
			out << "Size: " << cncPath.size() << std::endl;
			for( GLI::GLCncPath::iterator it = cncPath.begin(); it < cncPath.end(); it++ ) {
				out << ' ' << it->getX() << ',' << it->getY() << ',' << it->getZ() << std::endl;
			}
		}
		
		/////////////////////////////////////////////////////////
		void appendPathData(const GLI::GLCncPathVertices& cpv) {
			cncPath.push_back(cpv);
		}
		
		/////////////////////////////////////////////////////////
		void appendPathData(const GLI::GLCncPath& cp) {
			if ( cp.size() > 0 )	cncPath.assign(cp.begin(), cp.end() - 1);
			else 					clearPathData();
		}
		
		/////////////////////////////////////////////////////////
		OpenGLContextCncPathBase::DrawType getDrawType() { return drawType; }
		void setDrawType(OpenGLContextCncPathBase::DrawType t) { drawType = t; }
		
	protected:
		GLI::GLCncPath	cncPath;
		DrawType drawType;
		
		virtual void determineModel();
};

#endif