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
		, currentClientId(-1L)
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
			unsigned long cnt = 0;
			for( GLI::GLCncPath::iterator it = cncPath.begin(); it < cncPath.end(); ++it ) {
				out << ' ';
				out << wxString::Format("%6lu", cnt++) << ": ";
				out << wxString::Format("%+.10lf", it->getX()) << ", ";
				out << wxString::Format("%+.10lf", it->getY()) << ", ";
				out << wxString::Format("%+.10lf", it->getZ());
				out << std::endl;
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
		void setCurrentClientId(long id) { currentClientId = id; }
		
		/////////////////////////////////////////////////////////
		GLContextCncPathBase::DrawType getDrawType() { return drawType; }
		void setDrawType(GLContextCncPathBase::DrawType t) { drawType = t; }
		
		// bound box
		/////////////////////////////////////////////////////////
		void enableBoundBox(bool enable=true) { boundBox = enable; }
		bool isBoundBoxEnabled() { return boundBox; }
		
		void setBoundBoxColour(const wxColour& c) {  boundBoxColour = c; }
		const wxColour& getBoundBoxColour() { return boundBoxColour; }
		
		// origin
		int getOriginX() { return ( viewPort != NULL ? viewPort->getX() : 0 ); }
		int getOriginY() { return ( viewPort != NULL ? viewPort->getY() : 0 ); }

	protected:
		GLI::GLCncPath	cncPath;
		DrawType drawType;
		
		long currentClientId;
		
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