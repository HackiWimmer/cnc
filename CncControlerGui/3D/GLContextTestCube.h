#ifndef OPENGL_CONTEXT_TEST_CUBE_H
#define OPENGL_CONTEXT_TEST_CUBE_H

#include "3D/GLContextPathBase.h"

/////////////////////////////////////////////////////////////////
class GLContextTestCube : public GLContextCncPathBase {
	
	public:
		GLContextTestCube(wxGLCanvas* canvas);
		virtual ~GLContextTestCube();
		
		virtual const char* getContextName() { return "GLContextTestCube"; };
		
	protected:
		virtual void initContext();
		virtual void determineProjection(int w, int h);
		virtual void determineModel();
		virtual void markCurrentPosition() {}
		virtual GLViewPort* createViewPort();
};

#endif

