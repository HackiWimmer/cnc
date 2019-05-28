#ifndef GL_LABEL_CLUSTER_H
#define GL_LABEL_CLUSTER_H

#include <vector>
#include "wx/string.h"

#include "3D/GLInclude.h"

namespace GLI {
	
	////////////////////////////////////////////////////////////////
	struct GLLabelInfo {
		float x		= 0.0;
		float y		= 0.0;
		float z		= 0.0;
		
		void* font 	= GLUT_BITMAP_8_BY_13;
		wxString label;
	};
	
	////////////////////////////////////////////////////////////////
	class GLLabelCluster : public std::vector<GLLabelInfo> {
	
		public:
			GLLabelCluster();
			virtual ~GLLabelCluster();
			
			void clear();
			void push_back(const GLLabelInfo& l);
	};

};

#endif
