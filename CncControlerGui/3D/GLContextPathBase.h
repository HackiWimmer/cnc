#ifndef OPENGL_CONTEXT_PATH_BASE_H
#define OPENGL_CONTEXT_PATH_BASE_H

#include <cfloat>
#include "3D/GLCncPathData.h"
#include "3D/GLContextBase.h"

/////////////////////////////////////////////////////////////////
class GLContextCncPathBase : public GLContextBase {
	
	public:
	
		enum DrawType { DT_POINTS, DT_LINES, DT_LINE_STRIP };
		
		/////////////////////////////////////////////////////////
		GLContextCncPathBase(wxGLCanvas* canvas)
		: GLContextBase(canvas)
		, cncPath()
		, drawType(DT_LINE_STRIP)
		, boundBox(true)
		, boundBoxColour(185, 127, 87)
		{
			cncPath.reserve(1024 * 1024);
		}
		
		/////////////////////////////////////////////////////////
		virtual ~GLContextCncPathBase() {
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
			for( GLI::GLCncPath::iterator it = cncPath.begin(); it < cncPath.end(); ++it ) {
				out << ' ' << it->getX() << ',' << it->getY() << ',' << it->getZ() << std::endl;
			}
		}
		
		/////////////////////////////////////////////////////////
		void appendPathData(const GLI::GLCncPathVertices& cpv) {
			cncPath.push_back(cpv);
		}
		
		/////////////////////////////////////////////////////////
		void appendPathData(const GLI::GLCncPath& cp) {
			if ( cp.size() > 0 ) {
				for ( GLI::GLCncPath::const_iterator it = cp.begin(); it != cp.end() - 1; ++it )
					cncPath.push_back(*it);
			} else {
				clearPathData();
			}
		}
		
		/////////////////////////////////////////////////////////
		GLContextCncPathBase::DrawType getDrawType() { return drawType; }
		void setDrawType(GLContextCncPathBase::DrawType t) { drawType = t; }
		
		// bound box
		/////////////////////////////////////////////////////////
		void enableBoundBox(bool enable=true) { boundBox = enable; }
		bool isBoundBoxEnabled() { return boundBox; }
		
		void setBoundBoxColour(const wxColour& c) {  boundBoxColour = c; }
		const wxColour& getBoundBoxColour() { return boundBoxColour; }

	protected:
		GLI::GLCncPath	cncPath;
		DrawType drawType;
		
		bool boundBox;
		wxColour boundBoxColour;
		
		virtual float getAutoScaleFactor() { return ( autoScale ? cncPath.getAutoScaleFact() : 1.0); }
		
		virtual void determineModel();
		virtual void markCurrentPosition();
		
		void drawBoundBox();
		
	private:
		void drawPoints();
		void drawLines();
		void drawLineStrips();
};

#endif