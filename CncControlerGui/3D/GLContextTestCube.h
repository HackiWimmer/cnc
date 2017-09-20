#ifndef OPENGL_CONTEXT_TEST_CUBE_H
#define OPENGL_CONTEXT_TEST_CUBE_H

#include "3D/GLContextBase.h"

/////////////////////////////////////////////////////////////////
class OpenGLContextTestCube : public OpenGLContextCncPathBase {
	
	public:
		OpenGLContextTestCube(wxGLCanvas* canvas);
		virtual ~OpenGLContextTestCube();
		
		virtual const char* getContextName() { return "OpenGLDrawPaneContextTestCube"; };
		
	protected:
		virtual void initContext();
		virtual void determineProjection(int w, int h);
		virtual void displayContext();
		virtual GLViewPort* createViewPort();
};

#endif

