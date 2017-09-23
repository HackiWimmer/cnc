#ifndef OPENGL_CONTEXT_TEST_CUBE_H
#define OPENGL_CONTEXT_TEST_CUBE_H

#include "3D/GLContextPathBase.h"

/////////////////////////////////////////////////////////////////
class OpenGLContextTestCube : public OpenGLContextCncPathBase {
	
	public:
		OpenGLContextTestCube(wxGLCanvas* canvas);
		virtual ~OpenGLContextTestCube();
		
		virtual const char* getContextName() { return "OpenGLDrawPaneContextTestCube"; };
		
	protected:
		virtual void initContext();
		virtual void determineProjection(int w, int h);
		virtual void determineModel();
		virtual GLViewPort* createViewPort();
};

#endif

