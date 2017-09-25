#ifndef OPENGL_CONTEXT_CNC_PATH_H
#define OPENGL_CONTEXT_CNC_PATH_H

#include "3D/GLContextPathBase.h"

/////////////////////////////////////////////////////////////////
class GLContextCncPath : public GLContextCncPathBase {
	
	public:
		GLContextCncPath(wxGLCanvas* canvas);
		virtual ~GLContextCncPath();
		
		virtual const char* getContextName() { return "OpenGLContextCncPath"; };
		
	protected:
		virtual void initContext();
		virtual void determineProjection(int w, int h);
		virtual void determineModel();
		virtual GLViewPort* createViewPort();
};

#endif