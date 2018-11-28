#ifndef OPENGL_CONTEXT_PATH_BASE_H
#define OPENGL_CONTEXT_PATH_BASE_H

#include <cfloat>
#include "3D/GLCncPathData.h"
#include "3D/GLLineCluster.h"
#include "3D/GLContextBase.h"

/////////////////////////////////////////////////////////////////
class GLContextCncPathBase : public GLContextBase {
	
	public:
	
		enum DrawType { DT_POINTS, DT_LINES, DT_LINE_STRIP };
		
		/////////////////////////////////////////////////////////
		GLContextCncPathBase(wxGLCanvas* canvas);
		virtual ~GLContextCncPathBase();
		
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
		GLContextCncPathBase::DrawType getDrawType() 		{ return drawType; }
		void setDrawType(GLContextCncPathBase::DrawType t) 	{ drawType = t; }
		
		// bound box
		/////////////////////////////////////////////////////////
		void enableBoundBox(bool enable=true) 		{ showBoundBox = enable; }
		bool isBoundBoxEnabled() 					{ return showBoundBox; }
		
		void setBoundBoxColour(const wxColour& c) 	{ boundBoxColour = c; }
		const wxColour& getBoundBoxColour() 		{ return boundBoxColour; }
		
		// ruler
		/////////////////////////////////////////////////////////
		void enableRuler(bool enable=true) 			{ showRuler = enable; }
		bool isRulerEnabled() 						{ return showRuler; }
		
		void enableHelpLines(bool enable=true) 		{ showHelpLines = enable; }
		bool isHelpLinesEnabled() 					{ return showHelpLines; }
		
		GLI::GLLabelCluster& getRulerOrigin() 		{ return ruler.origin; }
		GLI::GLAxisRuler& getRulerX() 				{ return ruler.xAxis; }
		GLI::GLAxisRuler& getRulerY() 				{ return ruler.yAxis; }
		GLI::GLAxisRuler& getRulerZ() 				{ return ruler.zAxis; }
		
		GLI::GLLineCluster& getXYPlane()			{ return ruler.helpLinesXY; }
		GLI::GLLineCluster& getXZPlane()			{ return ruler.helpLinesXZ; }
		GLI::GLLineCluster& getYZPlane()			{ return ruler.helpLinesYZ; }
		
		// origin
		/////////////////////////////////////////////////////////
		int getOriginX() { return ( viewPort != NULL ? viewPort->getX() : 0 ); }
		int getOriginY() { return ( viewPort != NULL ? viewPort->getY() : 0 ); }

	protected:
		GLI::GLCncPath		cncPath;
		GLI::GLXYZRuler		ruler;
		DrawType 			drawType;
		
		long 				currentClientId;
		
		bool 				showBoundBox;
		wxColour 			boundBoxColour;
		
		bool				showRuler;
		bool				showHelpLines;
		wxColour			rulerColourX;
		wxColour			rulerColourY;
		wxColour			rulerColourZ;
		
		virtual float getAutoScaleFactor() { return ( autoScale ? cncPath.getAutoScaleFact() : 1.0); }
		
		virtual void determineModel();
		virtual void markCurrentPosition();
		
		void drawBoundBox();
		void drawRuler();
		
	private:
		
		void drawPoints();
		void drawLines();
		void drawLineStrips();
};

#endif