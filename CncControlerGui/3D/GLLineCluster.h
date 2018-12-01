#ifndef GL_LINE_CLUSTER_H
#define GL_LINE_CLUSTER_H

#include <vector>
#include "wx/string.h"
#include "3D/GLLabelCluster.h"

namespace GLI {

	////////////////////////////////////////////////////////////////
	class GLVectiesTuple {
		
		public:
			GLVectiesTuple();
			~GLVectiesTuple();
			
			void setData(float x1, float y1, float z1, float x2, float y2, float z2);
			
			float getX1() const { return _x1; }
			float getX2() const { return _x2; }
			float getY1() const { return _y1; }
			float getY2() const { return _y2; }
			float getZ1() const { return _z1; }
			float getZ2() const { return _z2; }
		
		private:
			float _x1;
			float _y1;
			float _z1;
			
			float _x2;
			float _y2;
			float _z2;
	};
	
	////////////////////////////////////////////////////////////////
	class GLLineCluster : public std::vector<GLVectiesTuple> {
	
		public:
			GLLineCluster();
			virtual ~GLLineCluster();
			
			void clear();
			void push_back(const GLVectiesTuple& t);
	};
	
	////////////////////////////////////////////////////////////////
	struct GLAxisRuler {
		GLLineCluster	axisLines;
		GLLabelCluster 	axisLables;
		
		void clear() {
			axisLines.clear();
			axisLables.clear();
		}
	};
	
	////////////////////////////////////////////////////////////////
	struct GLHelpLineXYPlane {
		GLLineCluster	helpLinesX;
		GLLineCluster	helpLinesY;
		
		void clear() {
			helpLinesX.clear();
			helpLinesY.clear();
		}
	};
	
	////////////////////////////////////////////////////////////////
	struct GLHelpLineXZPlane {
		GLLineCluster	helpLinesX;
		GLLineCluster	helpLinesZ;
		
		void clear() {
			helpLinesX.clear();
			helpLinesZ.clear();
		}
	};
	
	////////////////////////////////////////////////////////////////
	struct GLHelpLineYZPlane {
		GLLineCluster	helpLinesY;
		GLLineCluster	helpLinesZ;
		
		void clear() {
			helpLinesY.clear();
			helpLinesZ.clear();
		}
	};
	
	////////////////////////////////////////////////////////////////
	struct GLXYZRuler {
		GLAxisRuler 		xAxis;
		GLAxisRuler 		yAxis;
		GLAxisRuler 		zAxis;
		
		GLHelpLineXYPlane	helpLinesXY;
		GLHelpLineXZPlane	helpLinesXZ;
		GLHelpLineYZPlane	helpLinesYZ;
		
		void clear() {
			xAxis.clear();
			yAxis.clear();
			zAxis.clear();
			
			helpLinesXY.clear();
			helpLinesXZ.clear();
			helpLinesYZ.clear();
		}
	};
};

#endif
