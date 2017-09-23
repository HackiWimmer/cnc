#ifndef OPENGL_CONTEXT_CNC_PATH_H
#define OPENGL_CONTEXT_CNC_PATH_H

#include "3D/GLContextPathBase.h"

/////////////////////////////////////////////////////////////////
class OpenGLContextCncPath : public OpenGLContextCncPathBase {
	
	public:
		OpenGLContextCncPath(wxGLCanvas* canvas);
		virtual ~OpenGLContextCncPath();
		
		virtual const char* getContextName() { return "OpenGLDrawPaneContextCncPath"; };
		
	protected:
		virtual void initContext();
		virtual void determineProjection(int w, int h);
		virtual void determineModel();
		virtual GLViewPort* createViewPort();
};

#endif