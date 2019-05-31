#ifndef OPENGL_CONTEXT_CNC_PATH_H
#define OPENGL_CONTEXT_CNC_PATH_H

#include "3D/GLContextCncPathBase.h"

/////////////////////////////////////////////////////////////////
class GLContextCncPath : public GLContextCncPathBase {
	
	public:
		GLContextCncPath(wxGLCanvas* canvas);
		virtual ~GLContextCncPath();
		
	protected:
		virtual void initContext();
		virtual void determineProjection(int w, int h);
		virtual void determineModel();
		virtual GLViewPort* createViewPort();
};

#endif